#ifndef KEY_APP_H_
#define KEY_APP_H_

#include <stdio.h>
#include <stdint.h>




#define KEY1 ((PINS_DRV_ReadPins(PTC)>>12)&0x01)
#define KEY2 ((PINS_DRV_ReadPins(PTC)>>13)&0x01)
#define KEY3 ((PINS_DRV_ReadPins(PTC)>>14)&0x01)

typedef enum KEY_NUM_
{
	KEY_PRESS_NULL 	= 0,
	KEY_PRESS_1 = 1,
	KEY_PRESS_2 ,
	KEY_PRESS_3 ,
	KEY_PRESS_LOCK = 255,
}key_scan_num;


void key_task_creat(void);

#endif /* RTOS_H_ */
