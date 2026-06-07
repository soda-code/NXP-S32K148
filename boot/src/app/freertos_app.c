#include "./app/freertos_app.h"
#include "./app/led_app.h"
#include "./app/key_app.h"
#include "./app/can_app.h"
#include "./app/ota_app.h"

void FreeRtos_Init(void)
{
	taskENTER_CRITICAL();
	led_task_creat();
	can_task_creat();
	key_task_creat();
	ota_task_creat();
	vTaskStartScheduler();
	taskEXIT_CRITICAL();
}
