#ifndef I2C_H
#define I2C_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*i2c_done_callback)(bool ok);

void i2c_init(void);

bool i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *result);
bool i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t value);

bool i2c_read_regs_async(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len,
                         i2c_done_callback done);

#endif
