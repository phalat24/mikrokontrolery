#include <gpio.h>
#include <stm32.h>

#include "accelerometer.h"
#include "i2c.h"

#define ACCELEROMETER_ADDR_0 0x1CU
#define ACCELEROMETER_ADDR_1 0x1DU

#define ACCELEROMETER_CTRL_REG1 0x20U
#define ACCELEROMETER_CTRL_REG3 0x22U

#define ACCELEROMETER_CTRL_REG1_PD 0x40U
#define ACCELEROMETER_CTRL_REG1_OFF 0x00U

#define ACCELEROMETER_CTRL_REG3_OFF 0x00U
#define ACCELEROMETER_CTRL_REG3_INT1_DRDY 0x04U

#define ACCELEROMETER_AUTO_INCREMENT 0x80U

#define ACCELEROMETER_OUT_FIRST 0x29U
#define ACCELEROMETER_OUT_LAST 0x2DU
#define ACCELEROMETER_OUT_LEN (ACCELEROMETER_OUT_LAST - ACCELEROMETER_OUT_FIRST + 1U)

#define ACCELEROMETER_INT1_PORT GPIOA
#define ACCELEROMETER_INT1_PIN 1U

typedef struct
{
    uint8_t out_reg;
    uint8_t ctrl_reg1_enable;
} acceleration_coordinate_properties_t;

static const acceleration_coordinate_properties_t acceleration_coordinate_properties[ACCEL_COORDINATE_COUNT] = {
    [ACCEL_X] = {.out_reg = 0x29U, .ctrl_reg1_enable = 0x01U},
    [ACCEL_Y] = {.out_reg = 0x2BU, .ctrl_reg1_enable = 0x02U},
    [ACCEL_Z] = {.out_reg = 0x2DU, .ctrl_reg1_enable = 0x04U},
};

static uint8_t accelerometer_addr;
static accel_record_callback record_callback;
static uint8_t raw[ACCELEROMETER_OUT_LEN];

static uint8_t get_coordinate_out_reg(acceleration_coordinate c)
{
    return acceleration_coordinate_properties[c].out_reg;
}

static uint8_t get_coordinate_ctrl_reg1_enable(acceleration_coordinate c)
{
    return acceleration_coordinate_properties[c].ctrl_reg1_enable;
}

static uint8_t build_ctrl_reg1(void)
{
    uint8_t ctrl_reg1 = ACCELEROMETER_CTRL_REG1_PD;
    for (acceleration_coordinate c = 0; c < ACCEL_COORDINATE_COUNT; c++)
    {
        ctrl_reg1 |= get_coordinate_ctrl_reg1_enable(c);
    }
    return ctrl_reg1;
}

static bool is_data_ready_line_high(void)
{
    return (ACCELEROMETER_INT1_PORT->IDR & (1U << ACCELEROMETER_INT1_PIN)) != 0;
}

static void on_read_done(bool ok);

static void start_read(void)
{
    (void)i2c_read_regs_async(accelerometer_addr,
                              ACCELEROMETER_OUT_FIRST | ACCELEROMETER_AUTO_INCREMENT,
                              raw, ACCELEROMETER_OUT_LEN, on_read_done);
}

static void on_read_done(bool ok)
{
    if (ok)
    {
        accelerometer_record record;
        for (acceleration_coordinate c = 0; c < ACCEL_COORDINATE_COUNT; c++)
        {
            /* bajt w kodzie U2 → liczba ze znakiem */
            record.axis[c] = (int8_t)raw[get_coordinate_out_reg(c) - ACCELEROMETER_OUT_FIRST];
        }
        record_callback(&record);
    }

    if (is_data_ready_line_high())
        start_read();
}

void EXTI1_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR1;
    start_read();
}

static bool find_and_power_down(void)
{
    const uint8_t candidates[] = {ACCELEROMETER_ADDR_0, ACCELEROMETER_ADDR_1};

    for (unsigned i = 0; i < sizeof(candidates); ++i)
    {
        accelerometer_addr = candidates[i];
        if (i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG1, ACCELEROMETER_CTRL_REG1_OFF) &&
            i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG3, ACCELEROMETER_CTRL_REG3_OFF))
        {
            return true;
        }
    }
    return false;
}

bool accel_init(accel_record_callback on_record)
{
    record_callback = on_record;

    if (!find_and_power_down())
        return false;

    GPIOinConfigure(ACCELEROMETER_INT1_PORT, ACCELEROMETER_INT1_PIN, GPIO_PuPd_NOPULL,
                    EXTI_Mode_Interrupt, EXTI_Trigger_Rising);
    EXTI->PR = EXTI_PR_PR1;
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    return i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG3, ACCELEROMETER_CTRL_REG3_INT1_DRDY) &&
           i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG1, build_ctrl_reg1());
}
