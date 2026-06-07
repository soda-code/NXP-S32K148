#include "./bsp/can.h"
#include "./app/can_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"

extern uint16_t  IRQ_CAN0_RX ;
//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

void CanTX_Task( void *pvParameters )
 {
	uint8_t led_command[8] = {0}; //定义一个8字节的数组，表示要发送的LED命令
 	(void) pvParameters;
 	while(1)
 	{
	
		can_send(0x01, led_command, 8); //发送CAN消息，ID为0x01，数据为led_command数组，长度为8字节
 		vTaskDelay(100);
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
	xTaskCreate((TaskFunction_t	)CanTX_Task,(char * )"CanTX_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
					(UBaseType_t) 13, //设置任务优先级为13，确保CAN任务的优先级高于LED任务和ADC任务
					(TaskHandle_t *)NULL ) ;
  
  xTaskCreate((TaskFunction_t	)CanRX_Task,(char * )"CanRX_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
          (UBaseType_t) 14, //设置任务优先级为14，确保CAN任务的优先级高于LED任务和ADC任务
          (TaskHandle_t *)NULL ) ;
}
