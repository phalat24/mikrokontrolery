#include "diodes.h"

typedef struct
{
    GPIO_TypeDef *diode_port;
    volatile uint32_t *ccr;
    uint32_t ccer_enable;
    uint32_t ccer_polarity;
    uint8_t diode_pin;
    uint8_t is_low_on;
} diode_properties_t;

static const diode_properties_t diode_properties[DIODE_COUNT] = {
    [DIODE_R] = {.diode_pin = DIODE_R_PIN, .diode_port = DIODE_R_PORT, .ccr = &TIM3->CCR1, .ccer_enable = TIM_CCER_CC1E, .ccer_polarity = TIM_CCER_CC1P, .is_low_on = 1},
    [DIODE_G] = {.diode_pin = DIODE_G_PIN, .diode_port = DIODE_G_PORT, .ccr = &TIM3->CCR2, .ccer_enable = TIM_CCER_CC2E, .ccer_polarity = TIM_CCER_CC2P, .is_low_on = 1},
    [DIODE_B] = {.diode_pin = DIODE_B_PIN, .diode_port = DIODE_B_PORT, .ccr = &TIM3->CCR3, .ccer_enable = TIM_CCER_CC3E, .ccer_polarity = TIM_CCER_CC3P, .is_low_on = 1},
};

GPIO_TypeDef *get_diode_port(diode_t diode)
{
    return diode_properties[diode].diode_port;
}

uint8_t get_diode_pin(diode_t diode)
{
    return diode_properties[diode].diode_pin;
}

uint32_t get_diode_ccer_bits(diode_t diode)
{
    const diode_properties_t *p = &diode_properties[diode];
    return p->ccer_enable | (p->is_low_on ? p->ccer_polarity : 0U);
}

void set_diode_level(diode_t diode, diode_level level)
{
    *diode_properties[diode].ccr = level;
}
