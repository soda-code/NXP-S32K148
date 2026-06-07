#include "./bsp/led.h"
#include "./app/led_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"


//*****************************************************************
// @fun     : led_Task
// @brief   : LED 任务运行函数
// @param   : pvParameters - 参数
// @return  : 无
//*****************************************************************

void led_Task( void *pvParameters )
 {
 	(void) pvParameters;
 	while(1)
 	{
 		led_toggle(LED_NUM_1); // Ϩ��LED1
 		vTaskDelay(1000);
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
	xTaskCreate((TaskFunction_t	)led_Task,(char * )"led_Task",256 ,(void *	) NULL,
					(UBaseType_t) 5,
					(TaskHandle_t *)NULL ) ;

}
