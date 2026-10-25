#include "diodes.h"
#include "commands.h"
#include "system_io.h"

static input_command input_command_unknown(void)
{
    return (input_command){0, DIODE_B, TOGGLE};
}

static uint8_t is_end_of_command(char byte)
{
    return (byte == '\n' || byte == '\r');
}

command_parser new_command_parser(void)
{
    return (command_parser){PARSE_EXPECT_PREFIX, DIODE_G2, TOGGLE};
}

static input_command command_parser_step(command_parser *parser, char byte)
{
    if (is_end_of_command(byte))
    {
        uint8_t is_complete = parser->phase == PARSE_EXPECT_END;
        parser->phase = PARSE_EXPECT_PREFIX;
        if (is_complete)
        {
            return (input_command){1, parser->diode, parser->operation};
        }
        return input_command_unknown();
    }

    switch (parser->phase)
    {
    case PARSE_EXPECT_PREFIX:
        parser->phase = byte == 'L' ? PARSE_EXPECT_DIODE : PARSE_INVALID;
        break;
    case PARSE_EXPECT_DIODE:
        parser->phase = PARSE_EXPECT_OPERATION;
        switch (byte)
        {
        case 'R':
            parser->diode = DIODE_R;
            break;
        case 'G':
            parser->diode = DIODE_G;
            break;
        case 'B':
            parser->diode = DIODE_B;
            break;
        case 'g':
            parser->diode = DIODE_G2;
            break;
        default:
            parser->phase = PARSE_INVALID;
            break;
        }
        break;
    case PARSE_EXPECT_OPERATION:
        parser->phase = PARSE_EXPECT_END;
        switch (byte)
        {
        case '0':
            parser->operation = OFF;
            break;
        case '1':
            parser->operation = ON;
            break;
        case 'T':
            parser->operation = TOGGLE;
            break;
        default:
            parser->phase = PARSE_INVALID;
            break;
        }
        break;
    default:
        parser->phase = PARSE_INVALID;
        break;
    }

    return input_command_unknown();
}

input_command command_parser_feed(command_parser *parser, uint8_t steps)
{
    char byte;
    while (steps > 0 && read_input_byte(&byte))
    {
        steps--;
        input_command command = command_parser_step(parser, byte);
        if (command.is_correct)
        {
            return command;
        }
    }
    return input_command_unknown();
}
