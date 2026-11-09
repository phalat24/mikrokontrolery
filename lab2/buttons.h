#ifndef BUTTONS_H
#define BUTTONS_H

#include <stm32.h>
#include <gpio.h>

#include "pins.h"

#define BUTTON_USER_PORT GPIOC
#define BUTTON_USER_PIN PIN_13

#define BUTTON_LEFT_PORT GPIOB
#define BUTTON_LEFT_PIN PIN_3

#define BUTTON_RIGHT_PORT GPIOB
#define BUTTON_RIGHT_PIN PIN_4

#define BUTTON_UP_PORT GPIOB
#define BUTTON_UP_PIN PIN_5

#define BUTTON_DOWN_PORT GPIOB
#define BUTTON_DOWN_PIN PIN_6

#define BUTTON_FIRE_PORT GPIOB
#define BUTTON_FIRE_PIN PIN_10

#define BUTTON_MODE_PORT GPIOA
#define BUTTON_MODE_PIN PIN_0

typedef enum
{
    PRESSED,
    RELEASED
} button_state;

typedef enum
{
    BUTTON_USER,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FIRE,
    BUTTON_MODE,
    BUTTON_COUNT
} button;

typedef struct
{
    const char *name;
    GPIO_TypeDef *button_port;
    GPIOPuPd_TypeDef pull;
    gpio_pin button_pin;
    uint8_t is_low_pressed;
} button_properties_t;

button_state get_button_state(button btn);
const char *get_button_state_name(button_state btn_state);
const char *get_button_name(button btn);
GPIO_TypeDef *get_button_port(button btn);
gpio_pin get_button_pin(button btn);
GPIOPuPd_TypeDef get_button_pull(button btn);
IRQn_Type get_button_irqn(button btn);
button get_pin_button(gpio_pin pin);

#endif