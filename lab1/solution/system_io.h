#ifndef SYSTEM_IO_H
#define SYSTEM_IO_H

#include <stm32.h>
#include "buffers.h"

#define MAX_INPUT_BATCH 16

uint8_t read_input_byte(char *byte);
void flush_sender_buffer(circular_buffer *sender_buffer);

#endif
