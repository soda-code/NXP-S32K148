#include "./bsp/can.h"
#include "./app/can_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"


//*****************************************************************
// @fun     : led_Task
// @brief   : LED任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

void can_Task( void *pvParameters )
 {
	uint8_t led_command[8] = {0}; //定义一个8字节的数组，表示要发送的LED命令
 	(void) pvParameters;
 	while(1)
 	{
	
		can_message_t Tx_msg = {
							 .cs = 0U,
							 .id = 0x01,
							 .data[0] = 0x0,
							 .data[1] = 0x1,
							 .data[2] = 0x2,
							 .data[3] = 0x3,
							 .data[4] = 0x4,
							 .data[5] = 0x5,
							 .data[6] = 0x6,
							 .data[7] = 0x7,
							 .length = 8
						 };
				CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_msg);
 		vTaskDelay(100);
 	}
 }

void can_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)can_Task,(char * )"can_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
					(UBaseType_t) 13,
					(TaskHandle_t *)NULL ) ;

}
