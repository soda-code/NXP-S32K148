#include "./bsp/led.h"
#include "Cpu.h"

//*****************************************************************
// @fun     : led_off
// @brief   : 鐔勭伃LED
// @param   : led - LED缂栧彿
// @return  : 鏃�
//*****************************************************************


void led_toggle(led_t led)
{
    switch(led)
    {
        case LED_NUM_1:
            PINS_DRV_TogglePins(PTE, 1U << 20);
            break;
        case LED_NUM_2:
            PINS_DRV_TogglePins(PTE, 1U << 21);
            break;
        case LED_NUM_3:
            PINS_DRV_TogglePins(PTE, 1U << 22);
            break;
        case LED_NUM_4:
            PINS_DRV_TogglePins(PTE, 1U << 23);
            break;
    }
}

