#include <stm32.h>

#include "buffers.h"

circular_buffer new_circular_buffer(char *buffer, uint8_t size)
{
    circular_buffer cb;
    cb.buffer = buffer;
    cb.size = size;
    cb.used_size = 0;
    cb.is_empty = 1;
    cb.next_read = 0;
    cb.next_write = 0;
    return cb;
}

int8_t circular_buffer_write_char(circular_buffer *cb, char data)
{
    if (cb->used_size == cb->size)
    {
        return -1;
    }
    cb->buffer[cb->next_write] = data;
    cb->next_write = (cb->next_write + 1) % cb->size;
    cb->used_size++;
    cb->is_empty = 0;
    return 0;
}

int8_t circular_buffer_write_text(circular_buffer *cb, const char *text)
{
    const char *ptr = text;
    while (*ptr != '\0')
    {
        if (circular_buffer_write_char(cb, *ptr) == -1)
        {
            return -1;
        }
        ptr++;
    }
    return 0;
}

int8_t circular_buffer_read(circular_buffer *cb, char *data)
{
    if (cb->is_empty)
    {
        return -1;
    }
    *data = cb->buffer[cb->next_read];
    cb->next_read = (cb->next_read + 1) % cb->size;
    cb->used_size--;
    if (cb->used_size == 0)
    {
        cb->is_empty = 1;
    }
    return 0;
}