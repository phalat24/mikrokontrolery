#include "commands.h"
#include "utils.h"

void set_diode_state(uint8_t diode_pin, GPIO_TypeDef *diode_port, diode_operation operation, uint8_t is_low_on)
{
    if (operation == TOGGLE)
    {
        diode_port->ODR ^= (1U << diode_pin);
    }
    else
    {
        uint8_t shift = ((is_low_on != 0) != (operation == OFF)) ? SHIFT_HIGH : SHIFT_LOW;
        diode_port->BSRR = 1U << (diode_pin + shift);
    }
}

void handle_diode_command(input_command command)
{
    if (!command.is_correct)
    {
        return;
    }

    uint8_t diode_pin;
    GPIO_TypeDef *diode_port;
    uint8_t is_low_on;

    switch (command.diode)
    {
    case DIODE_G2:
        diode_pin = DIODE_G2_PIN;
        diode_port = DIODE_G2_PORT;
        is_low_on = 0;
        break;
    case DIODE_G:
        diode_pin = DIODE_G_PIN;
        diode_port = DIODE_G_PORT;
        is_low_on = 1;
        break;
    case DIODE_B:
        diode_pin = DIODE_B_PIN;
        diode_port = DIODE_B_PORT;
        is_low_on = 1;
        break;
    default:
        diode_pin = DIODE_R_PIN;
        diode_port = DIODE_R_PORT;
        is_low_on = 1;
        break;
    }

    set_diode_state(diode_pin, diode_port, command.operation, is_low_on);
}