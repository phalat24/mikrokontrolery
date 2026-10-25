#include "buttons.h"

static const button_properties_t button_properties[] = {
    [BUTTON_USER] = {.name = "USER", .button_port = BUTTON_USER_PORT, .button_pin = BUTTON_USER_PIN, .is_low_pressed = 1},
    [BUTTON_LEFT] = {.name = "LEFT", .button_port = BUTTON_LEFT_PORT, .button_pin = BUTTON_LEFT_PIN, .is_low_pressed = 1},
    [BUTTON_RIGHT] = {.name = "RIGHT", .button_port = BUTTON_RIGHT_PORT, .button_pin = BUTTON_RIGHT_PIN, .is_low_pressed = 1},
    [BUTTON_UP] = {.name = "UP", .button_port = BUTTON_UP_PORT, .button_pin = BUTTON_UP_PIN, .is_low_pressed = 1},
    [BUTTON_DOWN] = {.name = "DOWN", .button_port = BUTTON_DOWN_PORT, .button_pin = BUTTON_DOWN_PIN, .is_low_pressed = 1},
    [BUTTON_FIRE] = {.name = "FIRE", .button_port = BUTTON_FIRE_PORT, .button_pin = BUTTON_FIRE_PIN, .is_low_pressed = 1},
    [BUTTON_MODE] = {.name = "MODE", .button_port = BUTTON_MODE_PORT, .button_pin = BUTTON_MODE_PIN, .is_low_pressed = 0}};

static const char *const button_state_names[] = {
    [PRESSED] = "PRESSED",
    [RELEASED] = "RELEASED",
};

button_state get_button_state(button btn)
{
    uint8_t pin_is_high = (button_properties[btn].button_port->IDR & (1U << button_properties[btn].button_pin)) != 0;

    uint8_t is_pressed = button_properties[btn].is_low_pressed != pin_is_high;

    return is_pressed ? PRESSED : RELEASED;
}

const char *get_button_state_name(button_state btn_state)
{
    return button_state_names[btn_state];
}

const char *get_button_name(button btn)
{
    return button_properties[btn].name;
}