#include <gpio.h>
#include <stm32.h>

#include "accelerometer.h"
#include "i2c.h"

/* Adres na magistrali: 0x1C albo 0x1D, zależnie od zworki SAD. */
#define ACCELEROMETER_ADDR_0 0x1CU
#define ACCELEROMETER_ADDR_1 0x1DU

/* Rejestry całego czujnika. */
#define ACCELEROMETER_CTRL_REG1 0x20U
#define ACCELEROMETER_CTRL_REG3 0x22U

/* CTRL_REG1: DR=0 (100 Hz), PD=1 (włączony), FS=0 (±2,3 g); bity osi z tablicy. */
#define ACCELEROMETER_CTRL_REG1_PD 0x40U
#define ACCELEROMETER_CTRL_REG1_OFF 0x00U

/* CTRL_REG3: IHL=0 (aktywny wysoki), PP_OD=0 (push-pull), INT1 = Data Ready (100). */
#define ACCELEROMETER_CTRL_REG3_OFF 0x00U
#define ACCELEROMETER_CTRL_REG3_INT1_DRDY 0x04U

/* Najstarszy bit numeru rejestru: autoinkrementacja przy odczycie wielu bajtów. */
#define ACCELEROMETER_AUTO_INCREMENT 0x80U

/* Odczytujemy jednym transferem rejestry OUT_X..OUT_Z (0x29..0x2D, co drugi pusty). */
#define ACCELEROMETER_OUT_FIRST 0x29U
#define ACCELEROMETER_OUT_LAST 0x2DU
#define ACCELEROMETER_OUT_LEN (ACCELEROMETER_OUT_LAST - ACCELEROMETER_OUT_FIRST + 1U)

/* Linia INT1 czujnika: PA1, przerwanie EXTI1. */
#define ACCELEROMETER_INT1_PORT GPIOA
#define ACCELEROMETER_INT1_PIN 1U

typedef struct
{
    uint8_t out_reg;          /* rejestr z wynikiem pomiaru tej osi */
    uint8_t ctrl_reg1_enable; /* bit Xen/Yen/Zen w CTRL_REG1 */
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

/* CTRL_REG1: czujnik włączony + bity włączające wszystkie osie z tablicy. */
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
    /* false = transfer już trwa; nic nie ginie, bo po jego końcu
       on_read_done sprawdzi linię INT1. */
    (void)i2c_read_regs_async(accelerometer_addr,
                              ACCELEROMETER_OUT_FIRST | ACCELEROMETER_AUTO_INCREMENT,
                              raw, ACCELEROMETER_OUT_LEN, on_read_done);
}

/* Wołane z I2C1_EV_IRQHandler / I2C1_ER_IRQHandler po zakończeniu odczytu. */
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

    /* Zgubione zbocze: jeśli linia wciąż wysoka, czujnik ma już kolejny pomiar,
       a EXTI nie zobaczy nowego zbocza. Czytamy od razu. */
    if (is_data_ready_line_high())
        start_read();
}

/* Zbocze narastające na INT1: czujnik ma nowy pomiar. */
void EXTI1_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR1;
    start_read();
}

/* Znajdź czujnik pod jednym z dwóch adresów i go wyłącz. Linia INT1 = GND. */
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

    /* 1. Czujnik wyłączony, przerwania czujnika wyłączone (reset STM32 go nie resetuje). */
    if (!find_and_power_down())
        return false;

    /* 2. INT1 na PA1 jako przerwanie od zbocza narastającego; linia jest teraz na pewno niska. */
    GPIOinConfigure(ACCELEROMETER_INT1_PORT, ACCELEROMETER_INT1_PIN, GPIO_PuPd_NOPULL,
                    EXTI_Mode_Interrupt, EXTI_Trigger_Rising);
    EXTI->PR = EXTI_PR_PR1;
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    /* 3. Data Ready na INT1 i start pomiarów: pierwszy pomiar da zbocze. */
    return i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG3, ACCELEROMETER_CTRL_REG3_INT1_DRDY) &&
           i2c_write_reg(accelerometer_addr, ACCELEROMETER_CTRL_REG1, build_ctrl_reg1());
}
