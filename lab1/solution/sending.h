#ifndef SENDING_H
#define SENDING_H

#include "buttons.h"
#include "buffers.h"

int8_t send_button_state_message(circular_buffer *cb, button btn, button_state btn_state);

#endif