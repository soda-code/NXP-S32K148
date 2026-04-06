#include "./bsp/led.h"
#include "./app/led_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"

 extern QueueHandle_t LedControlQueue; //创建队列

//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

void led_Task( void *pvParameters )
 {
	uint16_t led_command = 0;
	uint8_t loop_tick=0;
 	(void) pvParameters;
 	while(1)
 	{
		if(xQueueReceive(LedControlQueue, &led_command, pdMS_TO_TICKS(1)) == pdPASS)
		{
			loop_tick = 10;
		}
		if(loop_tick >0)
		{
			loop_tick--;
			switch (led_command)
			{
				case 1:
					led_off(LED_NUM_2); // 熄灭LED2
					led_toggle(LED_NUM_1); // 点亮LED1
					vTaskDelay(500); // 延时500ms
					break;
				case 2:
					led_off(LED_NUM_1); // 熄灭LED2
					led_toggle(LED_NUM_2); // 点亮LED2
					vTaskDelay(500); // 延时500ms
					break;
				case 3:
					led_toggle(LED_NUM_3); // 点亮LED3
					led_toggle(LED_NUM_4); // 点亮LED4
					vTaskDelay(500); // 延时500ms
					break;
				default:
					break;
			}
		}
		else
		{
			led_off(LED_NUM_1); // 熄灭LED1
			led_off(LED_NUM_2); // 熄灭LED2
			led_off(LED_NUM_3); // 熄灭LED3
			led_off(LED_NUM_4); // 熄灭LED4
		}
 		vTaskDelay(1);
 	}
 }

//*****************************************************************
// @fun     : led_task_creat
// @brief   : 创建LED任务
// @param   : 无
// @return  : 无
//*****************************************************************
void led_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)led_Task,(char * )"led_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
					(UBaseType_t) 5,
					(TaskHandle_t *)NULL ) ;

}
