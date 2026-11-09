#ifndef BUFFERS_H
#define BUFFERS_H

#include <stm32.h>
#include "messages.h"

#define SENDER_BUFFER_SIZE 128
#define CIRCULAR_BUFFER_INIT(storage)                   \
    {                                                   \
        .buffer = (storage),                            \
        .size = sizeof(storage) / sizeof((storage)[0]), \
        .used_size = 0,                                 \
        .next_read = 0,                                 \
        .next_write = 0,                                \
    }

typedef struct
{
    message_id *buffer;
    uint8_t size;
    uint8_t used_size;
    uint8_t next_read;
    uint8_t next_write;
} circular_buffer;

int8_t circular_buffer_write(circular_buffer *cb, message_id data);
int8_t circular_buffer_read(circular_buffer *cb, message_id *data);

#endif
