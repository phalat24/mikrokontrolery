#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "buttons.h"

void configure_gpio_input_for_button(button btn);
void clear_dma_usart2_tx_flags(void);
void clear_pending_button_interrupts(void);
void configure_buttons_as_interrupt_sources(void);
void configure_dma_for_usart2_tx(void);
void configure_usart2_tx_with_dma_requests(void);
void enable_peripheral_clocks(void);
void configure_interrupt_priorities(void);
void enable_interrupts(void);

#endif
