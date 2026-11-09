#ifndef SYSTEM_IO_H
#define SYSTEM_IO_H

#include <stm32.h>

uint8_t is_dma_busy(void);

void start_dma_transfer(const char *text, uint16_t length);

#endif
