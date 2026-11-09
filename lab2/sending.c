#include "sending.h"
#include "system_io.h"

static message_id sender_buffer_storage[SENDER_BUFFER_SIZE];
static circular_buffer sender_buffer = CIRCULAR_BUFFER_INIT(sender_buffer_storage);

static void send_message(message_id id)
{
    start_dma_transfer(get_message_text(id), get_message_length(id));
}

void send_or_enqueue_message(message_id id)
{
    if (is_dma_busy())
    {
        circular_buffer_write(&sender_buffer, id);
        return;
    }

    send_message(id);
}

void send_queued_message(void)
{
    message_id id;
    if (circular_buffer_read(&sender_buffer, &id) != 0)
    {
        return;
    }

    send_message(id);
}