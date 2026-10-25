#ifndef COMMANDS_H
#define COMMANDS_H

#include <stm32.h>

#include "diodes.h"

typedef struct
{
    uint8_t is_correct;
    diode_t diode;
    diode_operation operation;
} input_command;

typedef enum
{
    PARSE_EXPECT_PREFIX,
    PARSE_EXPECT_DIODE,
    PARSE_EXPECT_OPERATION,
    PARSE_EXPECT_END,
    PARSE_INVALID
} parse_phase;

typedef struct
{
    parse_phase phase;
    diode_t diode;
    diode_operation operation;
} command_parser;

void handle_diode_command(input_command command);
command_parser new_command_parser(void);
input_command command_parser_feed(command_parser *parser, uint8_t steps);

#endif
