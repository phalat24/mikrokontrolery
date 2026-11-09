#ifndef SENDING_H
#define SENDING_H

#include "buffers.h"

void send_queued_message(void);
void send_or_enqueue_message(message_id id);

#endif