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
	
		can_send(0x01, led_command, 8); //发送CAN消息，ID为0x01，数据为led_command数组，长度为8字节
 		vTaskDelay(100);
 	}
 }

void can_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)can_Task,(char * )"can_Task",1024,(void *	) NULL,
					(UBaseType_t) 5,
					(TaskHandle_t *)NULL ) ;

}
