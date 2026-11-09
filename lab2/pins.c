#include "pins.h"

static const pin_properties_t pin_properties[PIN_COUNT] = {
    [PIN_0] = {.exti_irqn = EXTI0_IRQn},
    [PIN_1] = {.exti_irqn = EXTI1_IRQn},
    [PIN_2] = {.exti_irqn = EXTI2_IRQn},
    [PIN_3] = {.exti_irqn = EXTI3_IRQn},
    [PIN_4] = {.exti_irqn = EXTI4_IRQn},
    [PIN_5] = {.exti_irqn = EXTI9_5_IRQn},
    [PIN_6] = {.exti_irqn = EXTI9_5_IRQn},
    [PIN_7] = {.exti_irqn = EXTI9_5_IRQn},
    [PIN_8] = {.exti_irqn = EXTI9_5_IRQn},
    [PIN_9] = {.exti_irqn = EXTI9_5_IRQn},
    [PIN_10] = {.exti_irqn = EXTI15_10_IRQn},
    [PIN_11] = {.exti_irqn = EXTI15_10_IRQn},
    [PIN_12] = {.exti_irqn = EXTI15_10_IRQn},
    [PIN_13] = {.exti_irqn = EXTI15_10_IRQn},
    [PIN_14] = {.exti_irqn = EXTI15_10_IRQn},
    [PIN_15] = {.exti_irqn = EXTI15_10_IRQn},
};

IRQn_Type get_pin_exti_irqn(gpio_pin pin)
{
    return pin_properties[pin].exti_irqn;
}
