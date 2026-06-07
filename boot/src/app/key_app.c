#include "./app/key_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"

//*****************************************************************
// @fun     : KEY_Proc
// @brief   : 按键扫描函数
// @param   : mode - 按键模式
// @return  : 按键状态
//*****************************************************************
key_scan_num  KEY_Proc (void)
{

    static uint8_t  key_tick = 0;           //按键扫描计数
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

    return return_key;   //鏃犳寜閿寜涓�
}

//*****************************************************************
// @fun     : key_Task
// @brief   : 鎸夐敭浠诲姟鍑芥暟
// @param   : pvParameters - 浠诲姟鍙傛暟
// @return  : 鏃�
//*****************************************************************
extern TaskHandle_t OtaTaskHandle ; /* OTA 任务句柄 */
volatile key_scan_num key_test = KEY_PRESS_NULL;
void key_Task( void *pvParameters )
 {

 	(void) pvParameters;
 	while(1)
 	{
      key_test=KEY_Proc();
      switch (key_test)
      {
          case KEY_PRESS_1:
            break;
          case KEY_PRESS_2:
            break;
          case KEY_PRESS_3:
              vTaskResume(OtaTaskHandle);
              break;
          default:
              break;
      }
 		vTaskDelay(10);
 	}
 }



void key_task_creat(void)
{
	xTaskCreate((TaskFunction_t	)key_Task,(char * )"key_Task",512 ,(void *	) NULL,
					(UBaseType_t) 6,
					(TaskHandle_t *)NULL ) ;

}
