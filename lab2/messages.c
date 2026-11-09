#include "messages.h"
#include "buffers.h"

_Static_assert(PRESSED == 0 && RELEASED == 1, "button_state has to be exactly PRESSED = 0 and RELEASED = 1");
_Static_assert(MESSAGE_COUNT <= 256, "message_id (uint8_t) won't store all messages");

#define MESSAGE_ID(btn, state) (BUTTON_STATE_COUNT * (btn) + (state))

#define BUTTON_NAMES(X, F)      \
    X(BUTTON_USER, "USER", F)   \
    X(BUTTON_LEFT, "LEFT", F)   \
    X(BUTTON_RIGHT, "RIGHT", F) \
    X(BUTTON_UP, "UP", F)       \
    X(BUTTON_DOWN, "DOWN", F)   \
    X(BUTTON_FIRE, "FIRE", F)   \
    X(BUTTON_MODE, "MODE", F)

#define BUTTON_MESSAGE_ENTRIES(btn, name, F)             \
    [MESSAGE_ID(btn, PRESSED)] = F(name " PRESSED\r\n"), \
                     [MESSAGE_ID(btn, RELEASED)] = F(name " RELEASED\r\n"),

#define AS_TEXT(s) (s)
#define AS_LENGTH(s) (sizeof(s) - 1)

static const char *const message_texts[MESSAGE_COUNT] = {
    BUTTON_NAMES(BUTTON_MESSAGE_ENTRIES, AS_TEXT)};

static const uint16_t message_lengths[MESSAGE_COUNT] = {
    BUTTON_NAMES(BUTTON_MESSAGE_ENTRIES, AS_LENGTH)};

message_id get_button_message_id(button btn, button_state state)
{
    return MESSAGE_ID(btn, state);
}

const char *get_message_text(message_id id)
{
    return message_texts[id];
}

uint16_t get_message_length(message_id id)
{
    return message_lengths[id];
}
