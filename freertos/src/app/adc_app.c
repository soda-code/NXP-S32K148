#include "./app/adc_app.h"
#include "./app/freertos_app.h"
#include "./bsp/bsp_api.h"

/* 定义队列句柄 */
QueueHandle_t xAdcQueue = NULL;

//*****************************************************************
// @fun     : vAdcQueueInit
// @brief   : 初始化ADC数据队列
// @param   : 无
// @return  : 无
//*****************************************************************
static void vAdcQueueInit(void)
{
    xAdcQueue = xQueueCreate(ADC_QUEUE_LENGTH, ADC_QUEUE_ITEM_SIZE);

    if (xAdcQueue == NULL)
    {
        /* 队列创建失败，处理错误 */
        /* 可增加断言或错误日志 */
    }
}


//*****************************************************************
// @fun     : vAdcSendToQueue
// @brief   : 发送ADC值到队列（从中断或任务调用）
// @param   : adc_value - ADC原始值
// @return  : pdTRUE: 成功, pdFALSE: 队列满
//*****************************************************************
BaseType_t vAdcSendToQueue(uint16_t adc_value)
{
    BaseType_t xResult;

    /* 从任务调用 */
    xResult = xQueueSend(xAdcQueue, &adc_value, 0);

    return xResult;
}

//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************


static uint32_t tick = 0;
void adc_Task( void *pvParameters )
 {
	uint16_t adc_data = 0;
 	(void) pvParameters;
	vAdcQueueInit();

 	while(1)
 	{
		tick++;
 		adc_get_value(ADC_CH_POT, &adc_data );
		vAdcSendToQueue(adc_data);
		//u1_printf("%d,%3.2f\r\n",tick, ((float)adc_data)*3.3/4096);
 		vTaskDelay(10);
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
				(UBaseType_t) 15,
				(TaskHandle_t *)NULL ) ;

}
