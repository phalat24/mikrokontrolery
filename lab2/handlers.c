#include <stm32.h>

#include "buttons.h"
#include "messages.h"
#include "sending.h"

#define EXTI_LINES_0 EXTI_PR_PR0
#define EXTI_LINES_3 EXTI_PR_PR3
#define EXTI_LINES_4 EXTI_PR_PR4
#define EXTI_LINES_9_5 (0x1FU << 5)
#define EXTI_LINES_15_10 (0x3FU << 10)

static void handle_button(button btn)
{
    send_or_enqueue_message(get_button_message_id(btn, get_button_state(btn)));
}

static gpio_pin lowest_set_bit(uint32_t bits)
{
    return (gpio_pin)__CLZ(__RBIT(bits));
}

static void handle_button_interrupts(uint32_t lines)
{
    uint32_t pending = EXTI->PR & lines;
    EXTI->PR = pending;

    while (pending != 0)
    {
        gpio_pin pin = lowest_set_bit(pending);
        pending &= pending - 1;

        button btn = get_pin_button(pin);
        if (btn != BUTTON_COUNT)
        {
            handle_button(btn);
        }
    }
}

void EXTI0_IRQHandler(void) { handle_button_interrupts(EXTI_LINES_0); }
void EXTI3_IRQHandler(void) { handle_button_interrupts(EXTI_LINES_3); }
void EXTI4_IRQHandler(void) { handle_button_interrupts(EXTI_LINES_4); }
void EXTI9_5_IRQHandler(void) { handle_button_interrupts(EXTI_LINES_9_5); }
void EXTI15_10_IRQHandler(void) { handle_button_interrupts(EXTI_LINES_15_10); }

void DMA1_Stream6_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF6)
    {
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;
        send_queued_message();
    }
}
