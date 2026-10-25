#include "sending.h"

static uint8_t text_length(const char *text)
{
    uint8_t length = 0;
    while (text[length] != '\0')
    {
        length++;
    }
    return length;
}

int8_t send_button_state_message(circular_buffer *cb, button btn, button_state btn_state)
{
    const char *name = get_button_name(btn);
    const char *state = get_button_state_name(btn_state);
    uint8_t message_length = text_length(name) + 1 + text_length(state) + 2;

    if (cb->size - cb->used_size < message_length)
    {
        return -1;
    }

    circular_buffer_write_text(cb, name);
    circular_buffer_write_char(cb, ' ');
    circular_buffer_write_text(cb, state);
    circular_buffer_write_text(cb, "\r\n");
    return 0;
}
