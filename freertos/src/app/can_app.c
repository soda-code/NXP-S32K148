#include "./bsp/bsp_api.h"
#include "./app/freertos_app.h"


/* 队列句柄（全局声明，在.c中定义） */
extern QueueHandle_t xAdcQueue;

extern uint16_t  IRQ_CAN0_RX ;

//*****************************************************************
// @fun     : vAdcReceiveFromQueue
// @brief   : 从队列接收ADC值（阻塞等待）
// @param   : pAdcValue  - 输出参数，存放接收到的ADC值
// @param   : xTicksToWait - 等待时间（portMAX_DELAY = 一直等）
// @return  : pdTRUE: 成功, pdFALSE: 超时
//*****************************************************************
static BaseType_t vAdcReceiveFromQueue(uint16_t *pAdcValue, TickType_t xTicksToWait)
{
    BaseType_t xResult;
		xResult = xQueueReceive(xAdcQueue, pAdcValue, xTicksToWait);
    return xResult;
}

//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

void CanTX_Task( void *pvParameters )
 {
	uint8_t led_command[8] = {0}; //定义一个8字节的数组，表示要发送的LED命令
	uint16_t sample;
 	(void) pvParameters;
 	while(1)
 	{
		if (vAdcReceiveFromQueue(&sample, pdMS_TO_TICKS(100)) == pdTRUE)
		{
			led_command[0] = sample/256;
			led_command[1] = sample%256;
			can_send(0x01, led_command, 8); //发送CAN消息，ID为0x01，数据为led_command数组，长度为8字节
		}
 	}

//*****************************************************************
// @fun     : canrx_Task
// @brief   : CAN接收任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

 }void CanRX_Task( void *pvParameters )
 {
 	(void) pvParameters;
 	while(1)
 	{
    if(IRQ_CAN0_RX) //检查接收标志位，如果为1表示有新的CAN消息接收到
    {
      uint32_t id;
      uint8_t data[8];
      uint8_t len;
      can_receive(&id, data, &len); //调用can_receive函数获取接收到的消息ID、数据和数据长度
      //在这里可以根据接收到的消息ID和数据进行相应的处理，例如控制LED的状态等
    } 

 		vTaskDelay(100);
 	}
 }

/**
 * @brief 创建CAN发送任务
 * 该函数用于创建一个CAN发送任务，设置任务的优先级和堆栈大小等参数
 */
void can_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)CanTX_Task,(char * )"CanTX_Task",(const configSTACK_DEPTH_TYPE)1024 ,(void *	) NULL,
					(UBaseType_t) 13, //设置任务优先级为13，确保CAN任务的优先级高于LED任务和ADC任务
					(TaskHandle_t *)NULL ) ;
  
  xTaskCreate((TaskFunction_t	)CanRX_Task,(char * )"CanRX_Task",(const configSTACK_DEPTH_TYPE)1024 ,(void *	) NULL,
          (UBaseType_t) 14, //设置任务优先级为14，确保CAN任务的优先级高于LED任务和ADC任务
          (TaskHandle_t *)NULL ) ;
}
