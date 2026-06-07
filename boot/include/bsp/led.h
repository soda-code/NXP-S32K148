#ifndef LED_H_
#define LED_H_

#include "app.h"

typedef enum 
{
	LED_NUM_1 = 0,
	LED_NUM_2,
	LED_NUM_3,
	LED_NUM_4
} led_t;

void led_toggle(led_t led);

#endif /* RTOS_H_ */
