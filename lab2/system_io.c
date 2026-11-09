#include <stm32.h>

#include "system_io.h"

uint8_t is_dma_busy(void)
{
    return (DMA1_Stream6->CR & DMA_SxCR_EN) != 0 ||
           (DMA1->HISR & DMA_HISR_TCIF6) != 0;
}

void start_dma_transfer(const char *text, uint16_t length)
{
    DMA1_Stream6->M0AR = (uint32_t)text;
    DMA1_Stream6->NDTR = length;
    DMA1_Stream6->CR |= DMA_SxCR_EN;
}
