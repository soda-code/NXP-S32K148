#include "./bsp/adc.h"
#include "./app/adc_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"

 extern QueueHandle_t LedControlQueue; //创建队列

//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

uint16_t adc_data = 0;
void adc_Task( void *pvParameters )
 {
 	(void) pvParameters;
 	while(1)
 	{
 		adc_data = adc_read();
 		vTaskDelay(100);
 	}
 }

 //*****************************************************************
// @fun     : adc_task_creat
// @brief   : 创建ADC任务
// @param   : 无
// @return  : 无
//*****************************************************************
void adc_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)adc_Task,
				(char * )"adc_Task",
				(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,
				(void *	) NULL,
				(UBaseType_t) 6,
				(TaskHandle_t *)NULL ) ;

}
