#include <irq.h>

#include "configure.h"

#define MESSAGE_IRQ_PRIO MIDDLE_IRQ_PRIO
#define MESSAGE_IRQ_SUBPRIO MIDDLE_IRQ_SUBPRIO

#define USART2_TX_PORT GPIOA
#define USART2_TX_PIN 2U

#define USART_BRR_9600_BAUD_AT_16MHZ 0x0683U

#define DMA_CHANNEL_USART2_TX 4U
#define DMA_STREAM6_ALL_FLAGS                                 \
    (DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | \
     DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6)

void configure_gpio_input_for_button(button btn)
{
    GPIOinConfigure(get_button_port(btn), get_button_pin(btn), get_button_pull(btn), EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
}

void enable_peripheral_clocks(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA1EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    __NOP();
}

void configure_usart2_tx_with_dma_requests(void)
{
    GPIOafConfigure(USART2_TX_PORT, USART2_TX_PIN, GPIO_OType_PP,
                    GPIO_High_Speed, GPIO_PuPd_UP, GPIO_AF_USART2);

    USART2->BRR = USART_BRR_9600_BAUD_AT_16MHZ;
    USART2->CR3 = USART_CR3_DMAT;
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

void configure_dma_for_usart2_tx(void)
{
    DMA1_Stream6->CR = DMA_CHANNEL_USART2_TX << DMA_SxCR_CHSEL_Pos | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_TCIE;
    DMA1_Stream6->PAR = (uint32_t)&USART2->DR;
}

void configure_buttons_as_interrupt_sources(void)
{
    for (button b = 0; b < BUTTON_COUNT; b++)
    {
        configure_gpio_input_for_button(b);
    }
}

void clear_pending_button_interrupts(void)
{
    uint32_t mask = 0;
    for (button b = 0; b < BUTTON_COUNT; b++)
    {
        mask |= 1U << get_button_pin(b);
    }
    EXTI->PR = mask;
}

void clear_dma_usart2_tx_flags(void)
{
    DMA1->HIFCR = DMA_STREAM6_ALL_FLAGS;
}

static void set_message_irq_priority(IRQn_Type irqn)
{
    IRQsetPriority(irqn, MESSAGE_IRQ_PRIO, MESSAGE_IRQ_SUBPRIO);
}

static void clear_and_enable_irq(IRQn_Type irqn)
{
    NVIC_ClearPendingIRQ(irqn);
    NVIC_EnableIRQ(irqn);
}

void configure_interrupt_priorities(void)
{
    IRQprotectionConfigure();

    for (button b = 0; b < BUTTON_COUNT; b++)
    {
        set_message_irq_priority(get_button_irqn(b));
    }
    set_message_irq_priority(DMA1_Stream6_IRQn);
}

void enable_interrupts(void)
{
    for (button b = 0; b < BUTTON_COUNT; b++)
    {
        clear_and_enable_irq(get_button_irqn(b));
    }
    clear_and_enable_irq(DMA1_Stream6_IRQn);
}
