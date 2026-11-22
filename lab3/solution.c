#include <stm32.h>

#include "accelerometer.h"
#include "configure.h"
#include "diodes.h"
#include "i2c.h"

static const acceleration_coordinate diode_coordinate[DIODE_COUNT] = {
    [DIODE_R] = ACCEL_X,
    [DIODE_G] = ACCEL_Y,
    [DIODE_B] = ACCEL_Z,
};

#define ACCEL_TO_LEVEL_GAIN 4U

static diode_level accel_to_level(int8_t a)
{
    uint32_t level = (uint32_t)(a < 0 ? -a : a) * ACCEL_TO_LEVEL_GAIN;
    return level > DIODE_LEVEL_MAX ? DIODE_LEVEL_MAX : (diode_level)level;
}

static void on_accel_record(const accelerometer_record *record)
{
    for (diode_t d = 0; d < DIODE_COUNT; d++)
    {
        set_diode_level(d, accel_to_level(record->axis[diode_coordinate[d]]));
    }
}

int main(void)
{
    enable_peripheral_clocks();
    rgb_init();
    i2c_init();

    if (!accel_init(on_accel_record))
    {
        set_diode_level(DIODE_R, DIODE_LEVEL_MAX);
    }

    for (;;)
    {
        __WFI();
    }
}
