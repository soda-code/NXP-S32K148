#include "./bsp/led.h"
#include "Cpu.h"

//*****************************************************************
// @fun     : led_off
// @brief   : 熄灭LED
// @param   : led - LED编号
// @return  : 无
//*****************************************************************

void led_off(led_t led)
{
    switch(led)
    {
        case LED_NUM_1:
            PINS_DRV_SetPins(PTE, 1U << 20);
            break;
        case LED_NUM_2:
            PINS_DRV_SetPins(PTE, 1U << 21);
            break;
        case LED_NUM_3:
            PINS_DRV_SetPins(PTE, 1U << 22);
            break;
        case LED_NUM_4:
            PINS_DRV_SetPins(PTE, 1U << 23);
            break;
    }
}

//*****************************************************************
// @fun     : led_on
// @brief   : 点亮LED
// @param   : led - LED编号
// @return  : 无
//*****************************************************************
void led_on(led_t led)
{
    switch(led)
    {
        case LED_NUM_1:
            PINS_DRV_ClearPins(PTE, 1U << 20);
            break;
        case LED_NUM_2:
            PINS_DRV_ClearPins(PTE, 1U << 21);
            break;
        case LED_NUM_3:
            PINS_DRV_ClearPins(PTE, 1U << 22);
            break;
        case LED_NUM_4:
            PINS_DRV_ClearPins(PTE, 1U << 23);
            break;
    }
}

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

