#include "./app/ota_app.h"
#include "./app/freertos_app.h"
#include "Cpu.h"

/* * APP 向量表起始地址：
 * 按照 Bootloader 占用 128 KB（0x20000 字节）规划，
 * APP A 区的起始地址必须挪到 0x00020000
 */
#define APP_START_ADDRESS    0x00020000


/* 定义 NXP 官方规范的跳转函数指针类型 */
typedef void (*app_entry_t)(void);

/**
 * @brief  官方规范的 APP 跳转函数
 * @param  userSP: APP 的栈顶指针（由 r0 传入）
 * @param  userStartup: APP 的复位向量地址（由 r1 传入）
 */
void JumpToUserApplication(unsigned int userSP, unsigned int userStartup)
{
    app_entry_t JumpToApplication;

    /* 1. 安全校验：检查 APP 的栈顶指针是否在 S32K 的物理 SRAM 范围内 */
    if ((userSP < 0x1FFF8000U) || (userSP > 0x20030000U))
    {
        return; /* 栈指针非法，说明 APP 区域没有有效固件，拒绝跳转 */
    }

    /* 2. 【核心安全步】接管 CPU 前，必须全面关闭全局中断，防止干扰 APP 启动 */
    INT_SYS_DisableIRQGlobal();

    /* 3. 【核心安全步】关闭并全清滴答定时器（SysTick），使用 S32K 专属寄存器名 */
    S32_SysTick->CSR = 0x00000000U;
    S32_SysTick->RVR = 0x00000000U;
    S32_SysTick->CVR = 0x00000000U;

    /* 4. 清除当前 Bootloader 留下的所有 NVIC 中断挂起状态，避免带入 APP */
    for (uint8_t i = 0U; i < (((uint32_t)NUMBER_OF_INT_VECTORS + 31U) / 32U); i++)
    {
        S32_NVIC->ICPR[i] = 0xFFFFFFFFU;
    }

    /* 5. 重新定向内核中断向量表（VTOR）到 128K 边界处的 APP 首地址 */
    S32_SCB->VTOR = (uint32_t)APP_START_ADDRESS;

    /* 6. 利用 GCC 兼容扩展汇编语法设置堆栈指针（MSP 和 PSP）
     * 解决严格 C 标准下单独写 "asm" 报错的问题，且动态绑定寄存器，比硬编码 r0 更安全
     */
    __asm__ __volatile__("msr msp, %0" : : "r" (userSP) : "sp");
    __asm__ __volatile__("msr psp, %0" : : "r" (userSP) : "sp");

    /* 7. 【彻底拒绝 mov pc, r1】
     * 将复位地址强制转换为 NXP 官方函数指针类型，让编译器自动生成带 T 位切换的 bx 跳转指令
     */
    JumpToApplication = (app_entry_t)userStartup;

    /* 8. 正式切入 APP 程序，交出 CPU 控制权 */
    JumpToApplication();

    /* 如果跳转成功，程序永远死在下面，绝对不会返回 */
    while(1)
    {
        __asm("NOP");
    }

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
    	JumpToUserApplication(*((unsigned int *)APP_START_ADDRESS), *((unsigned int *)(APP_START_ADDRESS + 4U)));
    	vTaskDelay(1);
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
