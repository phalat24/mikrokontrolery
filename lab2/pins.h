#ifndef PINS_H
#define PINS_H

#include <stm32.h>

typedef enum
{
    PIN_0,
    PIN_1,
    PIN_2,
    PIN_3,
    PIN_4,
    PIN_5,
    PIN_6,
    PIN_7,
    PIN_8,
    PIN_9,
    PIN_10,
    PIN_11,
    PIN_12,
    PIN_13,
    PIN_14,
    PIN_15,
    PIN_COUNT
} gpio_pin;

typedef struct
{
    IRQn_Type exti_irqn;
} pin_properties_t;

IRQn_Type get_pin_exti_irqn(gpio_pin pin);

#endif
