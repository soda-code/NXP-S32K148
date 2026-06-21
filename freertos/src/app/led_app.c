#include "./app/led_app.h"
#include "./app/freertos_app.h"
#include "./bsp/bsp_api.h"

 extern QueueHandle_t LedControlQueue; //鍒涘缓闃熷垪

//*****************************************************************
// @fun     : led_Task
// @brief   : LED浠诲姟鍑芥暟
// @param   : pvParameters - 浠诲姟鍙傛暟
// @return  : 鏃�
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
					led_off(LED_NUM_2); // 鐔勭伃LED2
					led_toggle(LED_NUM_1); // 鐐逛寒LED1
					vTaskDelay(500); // 寤舵椂500ms
					break;
				case 2:
					led_off(LED_NUM_1); // 鐔勭伃LED2
					led_toggle(LED_NUM_2); // 鐐逛寒LED2
					vTaskDelay(500); // 寤舵椂500ms
					break;
				case 3:
					led_toggle(LED_NUM_3); // 鐐逛寒LED3
					led_toggle(LED_NUM_4); // 鐐逛寒LED4
					vTaskDelay(500); // 寤舵椂500ms
					break;
				default:
					break;
			}
		}
		else
		{
			led_off(LED_NUM_1); // 鐔勭伃LED1
			led_off(LED_NUM_2); // 鐔勭伃LED2
			led_off(LED_NUM_3); // 鐔勭伃LED3
			led_off(LED_NUM_4); // 鐔勭伃LED4
		}
 		vTaskDelay(1);
 	}
 }

//*****************************************************************
// @fun     : led_task_creat
// @brief   : 鍒涘缓LED浠诲姟
// @param   : 鏃�
// @return  : 鏃�
//*****************************************************************
void led_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)led_Task,(char * )"led_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
					(UBaseType_t) 5,
					(TaskHandle_t *)NULL ) ;

}
