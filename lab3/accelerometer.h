#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    ACCEL_X,
    ACCEL_Y,
    ACCEL_Z,
    ACCEL_COORDINATE_COUNT
} acceleration_coordinate;

/* Jeden odczyt czujnika: przyspieszenie na każdej osi, indeksowane
   acceleration_coordinate. Jednostka ≈ 18 mg, zakres [-128, 127]. */
typedef struct
{
    int8_t axis[ACCEL_COORDINATE_COUNT];
} accelerometer_record;

/* Wywoływane z przerwania po każdym nowym pomiarze. */
typedef void (*accel_record_callback)(const accelerometer_record *record);

/* Wymaga wcześniejszego i2c_init(). Po sukcesie pomiary przychodzą same:
   Data Ready (INT1 → PA1) → odczyt I2C na przerwaniach → on_record. */
bool accel_init(accel_record_callback on_record);

#endif
