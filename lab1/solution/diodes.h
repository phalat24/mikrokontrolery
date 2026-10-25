#ifndef DIODES_H
#define DIODES_H

#include <stm32.h>

#define DIODE_G2_PIN 5U
#define DIODE_R_PIN 6U
#define DIODE_G_PIN 7U
#define DIODE_B_PIN 0U

#define DIODE_G2_PORT GPIOA
#define DIODE_R_PORT GPIOA
#define DIODE_G_PORT GPIOA
#define DIODE_B_PORT GPIOB

typedef enum
{
    ON,
    OFF,
    TOGGLE
} diode_operation;

typedef enum
{
    DIODE_G2,
    DIODE_R,
    DIODE_G,
    DIODE_B
} diode_t;

#endif