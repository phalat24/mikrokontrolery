#ifndef BUFFERS_H
#define BUFFERS_H

#include <stm32.h>

#define SENDER_BUFFER_SIZE 128

typedef struct
{
    char *buffer;
    uint8_t size;
    uint8_t used_size;
    uint8_t is_empty;
    uint8_t next_read;
    uint8_t next_write;
} circular_buffer;

circular_buffer new_circular_buffer(char *buffer, uint8_t size);
int8_t circular_buffer_write_char(circular_buffer *cb, char data);
int8_t circular_buffer_write_text(circular_buffer *cb, const char *text);
int8_t circular_buffer_read(circular_buffer *cb, char *data);

#endif
