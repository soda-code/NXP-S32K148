#include "./app/ota_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"


uint16_t g_jump_to_app_flag =0;

/* 定义 NXP 官方规范的跳转函数指针类型 */
typedef void (*app_entry_t)(void);

void JumpToUserApplication( unsigned int userSP,  unsigned int userStartup)
{
	(void)userSP; //使参数不报警告信息
	(void)userStartup;
	/* 设置栈指针  */
	__asm("msr msp, r0");
	__asm("msr psp, r0");

	/* 重新定向中断向量表 */
	S32_SCB->VTOR = (uint32_t)APP_START_ADDRESS;

	/* 跳转至APP */
	__asm("mov pc, r1");
}

/**
 * @brief  FreeRTOS OTA 轮询任务
 */
void Ota_Task(void *pvParameters)
{
    /* 显式转换，消除无用参数警告 */
    (void)pvParameters;
    
    while(1)
    {
    	g_jump_to_app_flag = 1;
    	taskENTER_CRITICAL();
		JumpToUserApplication(*((uint32_t*)APP_START_ADDRESS), *((uint32_t*)(APP_START_ADDRESS + 4)));
    }
}

TaskHandle_t OtaTaskHandle = NULL; /* OTA 任务句柄 */
/**
 * @brief  创建 OTA 任务
 */
void ota_task_creat(void)
{
    xTaskCreate(
        (TaskFunction_t)Ota_Task,
        (const char * )"Ota_Task",
        (const uint16_t)512,
        (void * )NULL,
        (UBaseType_t   )6, /* 任务优先级 */
        (TaskHandle_t *)&OtaTaskHandle
    );
    vTaskSuspend(OtaTaskHandle); /* 创建后立即挂起，等待外部事件触发再恢复执行 */
}
