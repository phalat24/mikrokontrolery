#include <stm32.h>

#include "buffers.h"

int8_t circular_buffer_write(circular_buffer *cb, message_id data)
{
    if (cb->used_size == cb->size)
    {
        return -1;
    }
    cb->buffer[cb->next_write] = data;
    cb->next_write = (cb->next_write + 1) % cb->size;
    cb->used_size++;
    return 0;
}

int8_t circular_buffer_read(circular_buffer *cb, message_id *data)
{
    if (cb->used_size == 0)
    {
        return -1;
    }
    *data = cb->buffer[cb->next_read];
    cb->next_read = (cb->next_read + 1) % cb->size;
    cb->used_size--;
    return 0;
}