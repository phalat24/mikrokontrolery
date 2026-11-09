#include <stm32.h>

#include "configure.h"

int main(void)
{
    enable_peripheral_clocks();
    configure_usart2_tx_with_dma_requests();
    configure_dma_for_usart2_tx();
    configure_buttons_as_interrupt_sources();

    clear_pending_button_interrupts();
    clear_dma_usart2_tx_flags();

    configure_interrupt_priorities();
    enable_interrupts();

    for (;;)
    {
        __WFI();
    }
}
