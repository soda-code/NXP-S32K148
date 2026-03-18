#include "./app/freertos_app.h"
#include "./app/led_app.h"
#include "./app/key_app.h"
#include "./app/adc_app.h"
void FreeRtos_Init(void)
{
	led_task_creat();
	adc_task_creat();
	key_task_creat();
	vTaskStartScheduler();
}
