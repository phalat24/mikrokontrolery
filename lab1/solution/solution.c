#include <stm32.h>
#include <gpio.h>
#include <delay.h>

#include <stm32.h>
#include <stdbool.h>

#include "system_io.h"
#include "buffers.h"
#include "diodes.h"
#include "commands.h"
#include "utils.h"
#include "buttons.h"
#include "sending.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    __NOP();

    DIODE_G2_PORT->BSRR = (1U << (DIODE_G2_PIN + SHIFT_HIGH));
    DIODE_B_PORT->BSRR = (1U << (DIODE_B_PIN + SHIFT_LOW));
    DIODE_G_PORT->BSRR = (1U << (DIODE_G_PIN + SHIFT_LOW));
    DIODE_R_PORT->BSRR = (1U << (DIODE_R_PIN + SHIFT_LOW));

    GPIOoutConfigure(GPIOA, DIODE_G2_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(GPIOA, DIODE_R_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(GPIOA, DIODE_G_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(GPIOB, DIODE_B_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);

    GPIOafConfigure(GPIOA, 2U, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_UP, 7U);
    GPIOafConfigure(GPIOA, 3U, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_UP, 7U);
    USART2->BRR = 0x0683U;
    USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    GPIOinConfigure(GPIOC, BUTTON_USER_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOB, BUTTON_LEFT_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOB, BUTTON_RIGHT_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOB, BUTTON_UP_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOB, BUTTON_DOWN_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOB, BUTTON_FIRE_PIN, GPIO_PuPd_UP, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);
    GPIOinConfigure(GPIOA, BUTTON_MODE_PIN, GPIO_PuPd_DOWN, EXTI_Mode_Disable, EXTI_Trigger_Irrelevant);

    command_parser parser = new_command_parser();

    char internal_sender_buffer[SENDER_BUFFER_SIZE];
    circular_buffer sender_buffer = new_circular_buffer(internal_sender_buffer, SENDER_BUFFER_SIZE);

    button_state button_states[BUTTON_COUNT];
    for (button b = 0; b < BUTTON_COUNT; b++)
    {
        button_states[b] = get_button_state(b);
    }

    while (1)
    {
        handle_diode_command(command_parser_feed(&parser, MAX_INPUT_BATCH));

        for (button b = 0; b < BUTTON_COUNT; b++)
        {
            button_state current_state = get_button_state(b);
            if (current_state != button_states[b])
            {
                send_button_state_message(&sender_buffer, b, current_state);
                button_states[b] = current_state;
            }
        }

        flush_sender_buffer(&sender_buffer);
    }
}