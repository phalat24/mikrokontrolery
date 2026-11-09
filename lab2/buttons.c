#include "buttons.h"

static const button_properties_t button_properties[] = {
    [BUTTON_USER] = {.name = "USER", .button_port = BUTTON_USER_PORT, .button_pin = BUTTON_USER_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_LEFT] = {.name = "LEFT", .button_port = BUTTON_LEFT_PORT, .button_pin = BUTTON_LEFT_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_RIGHT] = {.name = "RIGHT", .button_port = BUTTON_RIGHT_PORT, .button_pin = BUTTON_RIGHT_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_UP] = {.name = "UP", .button_port = BUTTON_UP_PORT, .button_pin = BUTTON_UP_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_DOWN] = {.name = "DOWN", .button_port = BUTTON_DOWN_PORT, .button_pin = BUTTON_DOWN_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_FIRE] = {.name = "FIRE", .button_port = BUTTON_FIRE_PORT, .button_pin = BUTTON_FIRE_PIN, .pull = GPIO_PuPd_UP, .is_low_pressed = 1},
    [BUTTON_MODE] = {.name = "MODE", .button_port = BUTTON_MODE_PORT, .button_pin = BUTTON_MODE_PIN, .pull = GPIO_PuPd_DOWN, .is_low_pressed = 0}};

#define PIN_TO_BUTTON_ENTRY(name) [BUTTON_##name##_PIN] = BUTTON_##name + 1

static const uint8_t pin_to_button_plus_one[PIN_COUNT] = {
    PIN_TO_BUTTON_ENTRY(USER),
    PIN_TO_BUTTON_ENTRY(LEFT),
    PIN_TO_BUTTON_ENTRY(RIGHT),
    PIN_TO_BUTTON_ENTRY(UP),
    PIN_TO_BUTTON_ENTRY(DOWN),
    PIN_TO_BUTTON_ENTRY(FIRE),
    PIN_TO_BUTTON_ENTRY(MODE),
};

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

GPIO_TypeDef *get_button_port(button btn)
{
    return button_properties[btn].button_port;
}

gpio_pin get_button_pin(button btn)
{
    return button_properties[btn].button_pin;
}

GPIOPuPd_TypeDef get_button_pull(button btn)
{
    return button_properties[btn].pull;
}

IRQn_Type get_button_irqn(button btn)
{
    return get_pin_exti_irqn(button_properties[btn].button_pin);
}

button get_pin_button(gpio_pin pin)
{
    uint8_t entry = pin_to_button_plus_one[pin];
    return entry == 0 ? BUTTON_COUNT : (button)(entry - 1);
}
