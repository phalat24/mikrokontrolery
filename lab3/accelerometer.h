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

typedef struct
{
    int8_t axis[ACCEL_COORDINATE_COUNT];
} accelerometer_record;

typedef void (*accel_record_callback)(const accelerometer_record *record);

bool accel_init(accel_record_callback on_record);

#endif
