#include <gpio.h>

#include "configure.h"
#include "diodes.h"

#define DIODE_PWM_PSC 15U
#define DIODE_PWM_ARR 254U

void enable_peripheral_clocks(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    __NOP();
}

void rgb_init(void)
{
    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_ARPE;
    TIM3->PSC = DIODE_PWM_PSC;
    TIM3->ARR = DIODE_PWM_ARR;

    TIM3->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
                  TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
    TIM3->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;

    uint32_t ccer = 0;
    for (diode_t d = 0; d < DIODE_COUNT; d++)
    {
        set_diode_level(d, 0);
        ccer |= get_diode_ccer_bits(d);
    }
    TIM3->CCER = ccer;

    TIM3->EGR = TIM_EGR_UG;

    for (diode_t d = 0; d < DIODE_COUNT; d++)
    {
        GPIOafConfigure(get_diode_port(d), get_diode_pin(d), GPIO_OType_PP,
                        GPIO_Low_Speed, GPIO_PuPd_NOPULL, GPIO_AF_TIM3);
    }

    TIM3->CR1 |= TIM_CR1_CEN;
}
