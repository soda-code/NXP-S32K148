#include "./app/key_app.h"
#include "./app/freertos_app.h"
#include "./bsp/bsp_api.h"

 QueueHandle_t LedControlQueue; //创建队列

//*****************************************************************
// @fun     : KEY_Proc
// @brief   : 按键处理函数
// @param   : mode - 按键模式
// @return  : 按键扫描结果
//*****************************************************************
key_scan_num  KEY_Proc (void)
{

    //static uint8_t key_tick_lock = 0;      //按键松开标志
    //static uint16_t key_tick_long = 0;      //按键长按计数
    static uint8_t  key_tick = 0;           //按键消抖计数
    key_scan_num return_key = KEY_PRESS_NULL;

    if((KEY1==1||KEY2==1||KEY3==1)&&(return_key == KEY_PRESS_NULL))
    {
 		key_tick++;
        if(key_tick == 10)
        {
            key_tick = 0;
            if(KEY1 ==1)
            {
                return_key = KEY_PRESS_1;
            }
            else if(KEY2==1)
            {
                return_key = KEY_PRESS_2;
            }
            else if(KEY3==1)
            {
                return_key = KEY_PRESS_3;
            }
            else
            {
                return_key = KEY_PRESS_NULL;
            }
        }
    }
    else
    {

        key_tick=0;
        return_key = KEY_PRESS_NULL;
    }

    return return_key;   //无按键按下
}

//*****************************************************************
// @fun     : key_Task
// @brief   : 按键任务函数
// @param   : pvParameters - 任务参数
// @return  : 无
//*****************************************************************

volatile key_scan_num key_test = KEY_PRESS_NULL;
uint16_t temp = 0;
void key_Task( void *pvParameters )
 {

 	(void) pvParameters;
 	while(1)
 	{
 		key_test=KEY_Proc();
        switch (key_test)
        {
            case KEY_PRESS_1:
                if(pdPASS != xQueueSend(LedControlQueue, &(uint16_t){1}, 0)) // 将控制命令 1 发送到队列
                {
                	temp=1;
                }
            	else
				{
					temp=0;
				}
                break;
            case KEY_PRESS_2:
                if(pdPASS != xQueueSend(LedControlQueue, &(uint16_t){2}, 0)) // 将控制命令 2 发送到队列
                {
                	temp=1;
                }
            	else
				{
					temp=0;
				}
                break;
            case KEY_PRESS_3:
            	if(pdPASS != xQueueSend(LedControlQueue, &(uint16_t){3}, 0)) // 将控制命令 3 发送到队列
                {
            		temp=1;
                }
            	else
            	{
            		temp=0;
            	}
                break;
            default:
                break;
        }
 		vTaskDelay(10);
 	}
 }

 //*****************************************************************
// @fun     : Init_key_Queues
// @brief   : 初始化按键相关的队列
// @param   : 无
// @return  : 无
//*****************************************************************
 void Init_key_Queues(void) 
 {
    // 创建一个可以容纳 10 个 uint16_t 数据的队列
    LedControlQueue = xQueueCreate(1, sizeof(uint16_t));
    if (LedControlQueue == NULL) 
    {
        // 队列创建失败处理（通常是内存不足）
    }
}


void key_task_creat(void)
{
    BaseType_t res = 0x00;
    Init_key_Queues(); // 初始化按键相关的队列
	res = xTaskCreate((TaskFunction_t	)key_Task,(char * )"key_Task",(const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE ,(void *	) NULL,
					(UBaseType_t) 12,
					(TaskHandle_t *)NULL ) ;

}
