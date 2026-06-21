#ifndef BSP_API_H_
#define BSP_API_H_

#include "Cpu.h"

#include "./bsp/can.h"
#include "./bsp/adc.h"
#include "./bsp/led.h"
#include "./bsp/usart.h"


#define KEY1 ((PINS_DRV_ReadPins(PTC)>>12)&0x01)
#define KEY2 ((PINS_DRV_ReadPins(PTC)>>13)&0x01)
#define KEY3 ((PINS_DRV_ReadPins(PTC)>>14)&0x01)


#endif /* RTOS_H_ */
