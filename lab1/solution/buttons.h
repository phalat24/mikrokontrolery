#ifndef BUTTONS_H
#define BUTTONS_H

#include <stm32.h>
#include "utils.h"

#define BUTTON_USER_PORT GPIOC
#define BUTTON_USER_PIN 13U

#define BUTTON_LEFT_PORT GPIOB
#define BUTTON_LEFT_PIN 3U

#define BUTTON_RIGHT_PORT GPIOB
#define BUTTON_RIGHT_PIN 4U

#define BUTTON_UP_PORT GPIOB
#define BUTTON_UP_PIN 5U

#define BUTTON_DOWN_PORT GPIOB
#define BUTTON_DOWN_PIN 6U

#define BUTTON_FIRE_PORT GPIOB
#define BUTTON_FIRE_PIN 10U

#define BUTTON_MODE_PORT GPIOA
#define BUTTON_MODE_PIN 0U

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
    uint8_t button_pin;
    uint8_t is_low_pressed;
} button_properties_t;

button_state get_button_state(button btn);
const char *get_button_state_name(button_state btn_state);
const char *get_button_name(button btn);

#endif