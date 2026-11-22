#ifndef DIODES_H
#define DIODES_H

#include <stm32.h>
#include <stdint.h>

#define DIODE_R_PIN 6U
#define DIODE_G_PIN 7U
#define DIODE_B_PIN 0U

#define DIODE_R_PORT GPIOA
#define DIODE_G_PORT GPIOA
#define DIODE_B_PORT GPIOB

typedef uint8_t diode_level;
#define DIODE_LEVEL_MAX ((diode_level)255U)

typedef enum
{
    DIODE_R,
    DIODE_G,
    DIODE_B,
    DIODE_COUNT
} diode_t;

GPIO_TypeDef *get_diode_port(diode_t diode);
uint8_t get_diode_pin(diode_t diode);
uint32_t get_diode_ccer_bits(diode_t diode);

void set_diode_level(diode_t diode, diode_level level);

#endif
