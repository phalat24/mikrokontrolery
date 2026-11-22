#include <i2c_configure.h>
#include <stdbool.h>
#include <stm32.h>

#include "i2c.h"

#define PCLK1_HZ 16000000U
#define I2C_TIMEOUT 100000U

#define I2C_SR1_ERRORS (I2C_SR1_AF | I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_OVR)

#define WAIT_OR_FAIL(flag)   \
    do                       \
    {                        \
        if (!wait_sr1(flag)) \
            return false;    \
    } while (0)

#define PUT_CR1(flag)        \
    do                       \
    {                        \
        I2C1->CR1 |= (flag); \
    } while (0)

#define PUT_DR(data)       \
    do                     \
    {                      \
        I2C1->DR = (data); \
    } while (0)

#define CLEAR_ADDR       \
    do                   \
    {                    \
        (void)I2C1->SR2; \
    } while (0)

void i2c_init(void)
{
    i2c_configure(PCLK1_HZ);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
}

static bool wait_sr1(uint16_t flag)
{
    for (uint32_t i = 0; i < I2C_TIMEOUT; ++i)
    {
        uint16_t sr1 = I2C1->SR1;
        if (sr1 & flag)
            return true;
        if (sr1 & I2C_SR1_AF)
        {
            I2C1->SR1 = ~I2C_SR1_AF;
            break;
        }
    }
    PUT_CR1(I2C_CR1_STOP);
    return false;
}

static bool wait_stop_done(void)
{
    for (uint32_t i = 0; i < I2C_TIMEOUT; ++i)
    {
        if (!(I2C1->CR1 & I2C_CR1_STOP))
            return true;
    }
    return false;
}

bool i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *value)
{
    if (!wait_stop_done())
        return false;

    PUT_CR1(I2C_CR1_START);
    WAIT_OR_FAIL(I2C_SR1_SB);

    PUT_DR(addr << 1);
    WAIT_OR_FAIL(I2C_SR1_ADDR);
    CLEAR_ADDR;

    PUT_DR(reg);
    WAIT_OR_FAIL(I2C_SR1_BTF);

    PUT_CR1(I2C_CR1_START);
    WAIT_OR_FAIL(I2C_SR1_SB);

    PUT_DR(addr << 1 | 1);
    I2C1->CR1 &= ~I2C_CR1_ACK;
    WAIT_OR_FAIL(I2C_SR1_ADDR);
    CLEAR_ADDR;
    PUT_CR1(I2C_CR1_STOP);

    WAIT_OR_FAIL(I2C_SR1_RXNE);
    *value = I2C1->DR;

    return true;
}

bool i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t value)
{
    if (!wait_stop_done())
        return false;

    PUT_CR1(I2C_CR1_START);
    WAIT_OR_FAIL(I2C_SR1_SB);

    PUT_DR(addr << 1);
    WAIT_OR_FAIL(I2C_SR1_ADDR);
    CLEAR_ADDR;

    PUT_DR(reg);
    WAIT_OR_FAIL(I2C_SR1_TXE);

    PUT_DR(value);
    WAIT_OR_FAIL(I2C_SR1_BTF);

    PUT_CR1(I2C_CR1_STOP);
    return true;
}

typedef enum
{
    XFER_IDLE,
    XFER_START_W,
    XFER_ADDR_W,
    XFER_REG,
    XFER_START_R,
    XFER_ADDR_R,
    XFER_DATA
} xfer_state;

static volatile xfer_state state = XFER_IDLE;
static uint8_t xfer_addr;
static uint8_t xfer_reg;
static uint8_t *xfer_buf;
static uint8_t xfer_len;
static uint8_t xfer_idx;
static i2c_done_callback xfer_done;

static void xfer_finish(bool ok)
{
    I2C1->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
    state = XFER_IDLE;
    xfer_done(ok);
}

bool i2c_read_regs_async(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len,
                         i2c_done_callback done)
{
    if (state != XFER_IDLE || len == 0)
        return false;

    if (!wait_stop_done())
        return false;

    xfer_addr = addr;
    xfer_reg = reg;
    xfer_buf = buf;
    xfer_len = len;
    xfer_idx = 0;
    xfer_done = done;
    state = XFER_START_W;

    I2C1->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
    PUT_CR1(I2C_CR1_START);
    return true;
}

void I2C1_EV_IRQHandler(void)
{
    uint16_t sr1 = I2C1->SR1;

    if (sr1 & I2C_SR1_SB)
    {
        if (state == XFER_START_W)
        {
            PUT_DR(xfer_addr << 1);
            state = XFER_ADDR_W;
        }
        else if (state == XFER_START_R)
        {
            PUT_DR(xfer_addr << 1 | 1);
            if (xfer_len == 1)
                I2C1->CR1 &= ~I2C_CR1_ACK;
            else
                PUT_CR1(I2C_CR1_ACK);
            state = XFER_ADDR_R;
        }
    }
    else if (sr1 & I2C_SR1_ADDR)
    {
        CLEAR_ADDR;
        if (state == XFER_ADDR_W)
        {
            PUT_DR(xfer_reg);
            state = XFER_REG;
        }
        else if (state == XFER_ADDR_R)
        {
            if (xfer_len == 1)
                PUT_CR1(I2C_CR1_STOP);
            I2C1->CR2 |= I2C_CR2_ITBUFEN;
            state = XFER_DATA;
        }
    }
    else if (sr1 & I2C_SR1_RXNE)
    {
        xfer_buf[xfer_idx++] = I2C1->DR;
        uint8_t remaining = xfer_len - xfer_idx;
        if (remaining == 1)
        {
            I2C1->CR1 &= ~I2C_CR1_ACK;
            PUT_CR1(I2C_CR1_STOP);
        }
        else if (remaining == 0)
        {
            xfer_finish(true);
        }
    }
    else if (sr1 & I2C_SR1_BTF)
    {
        if (state == XFER_REG)
        {
            PUT_CR1(I2C_CR1_START);
            state = XFER_START_R;
        }
    }
}

void I2C1_ER_IRQHandler(void)
{
    I2C1->SR1 = ~I2C_SR1_ERRORS;
    PUT_CR1(I2C_CR1_STOP);
    if (state != XFER_IDLE)
        xfer_finish(false);
}
