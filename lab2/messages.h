#ifndef MESSAGES_H
#define MESSAGES_H

#include "buttons.h"

#define BUTTON_STATE_COUNT 2
#define MESSAGE_COUNT (BUTTON_STATE_COUNT * BUTTON_COUNT)

typedef uint8_t message_id;

message_id get_button_message_id(button btn, button_state state);

const char *get_message_text(message_id id);

uint16_t get_message_length(message_id id);

#endif
