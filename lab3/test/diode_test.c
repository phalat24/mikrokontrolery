#include <stm32.h>

#include "configure.h"
#include "diodes.h"

int main(void)
{
    enable_peripheral_clocks();
    configure_interrupt_priorities();
    rgb_init();

    /* Test PWM: fioletowy (czerwony 100% + niebieski ~25%). */
    set_diode_level(DIODE_R, DIODE_LEVEL_MAX);
    set_diode_level(DIODE_G, 0);
    set_diode_level(DIODE_B, DIODE_LEVEL_MAX / 4);

    for (;;)
    {
        __WFI();
    }
}
