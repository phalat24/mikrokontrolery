#include <stm32.h>

#include "buffers.h"
#include "system_io.h"

uint8_t read_input_byte(char *byte)
{
    if (!(USART2->SR & USART_SR_RXNE))
    {
        return 0;
    }
    *byte = (char)(USART2->DR & 0xFFU);
    return 1;
}

static inline void write_usart2(char byte)
{
    USART2->DR = (uint8_t)byte;
}

void flush_sender_buffer(circular_buffer *sender_buffer)
{
    while ((USART2->SR & USART_SR_TXE) && (sender_buffer->used_size > 0))
    {
        char byte;
        if (circular_buffer_read(sender_buffer, &byte) == 0)
        {
            write_usart2(byte);
        }
    }
}
