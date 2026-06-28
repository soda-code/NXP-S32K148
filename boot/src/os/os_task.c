/**********************************************************************************************************************
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  os_task.c
 *    Component:  -
 *       Module:  -
 *    Generator:  -
 *
 *  Description:  
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  REVISION HISTORY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Version   Date        Author  Change Id     Description
 *  -------------------------------------------------------------------------------------------------------------------

*********************************************************************************************************************/

/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/

#include "os_task.h"
#include "bsp.h"
#include "os_task_can.h"
#include "os_task_lld.h"

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/
#define HSRUN (0u) /* High speed run      */
#define RUN   (1u) /* Run                 */
#define VLPR  (2u) /* Very low power run  */
#define STOP1 (3u) /* Stop option 1       */
#define STOP2 (4u) /* Stop option 2       */
#define VLPS  (5u) /* Very low power stop */

/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/
TaskHandle_t    	Task_Init_Handler;
TaskHandle_t    	Task_CanTx_Handler;
TaskHandle_t    	Task_CanRx_Handler;
TaskHandle_t		Task_asw_10ms_Handler;
TaskHandle_t		Task_asw_100ms_Handler;

/* CAN message queue */
QueueHandle_t 		g_can1_rx_queue;      // CAN1接收队列
QueueHandle_t 		g_can2_rx_queue;      // CAN2接收队列
QueueHandle_t 		g_can3_rx_queue;      // CAN3接收队列
QueueHandle_t 		g_can1_tx_queue;
QueueHandle_t 		g_can2_tx_queue;
QueueHandle_t 		g_can3_tx_queue;

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

void vApplicationIdleHook(void){
	WDG_Refresh(&wdg_pal1_Instance);
}

/**
 * @brief 系统初始化函数，负责硬件和RTOS基础环境初始化
 *
 * 该函数执行以下关键初始化操作：
 * 1. 系统时钟初始化（48MHz配置）
 * 2. MCU电源模式设置（HSRUN高速运行模式）
 * 3. 板级引脚初始化
 * 4. 创建系统初始化任务（T_Init）
 *
 * @note 此函数应在main()函数中首先调用，且只调用一次，通过PEX_RTOS_INIT宏定义控制是否调用
 * @warning 调用前需确保已正确配置g_clockManConfigsArr等全局配置数组
 *
 * @param[in] void 无输入参数
 * @return void 无返回值
 *
 * @see CLOCK_SYS_Init()
 * @see POWER_SYS_Init()
 * @see xTaskCreate()
 */
void OS_Init(void)
{
    /** @brief 系统时钟初始化 */
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	/** @brief 时钟更新配置,默认48MHZ, HSRUN下112MHZ */
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	/** @brief 电源模式初始化与设置 */
	POWER_SYS_Init(&powerConfigsArr, POWER_MANAGER_CONFIG_CNT, &powerStaticCallbacksConfigsArr, POWER_MANAGER_CALLBACK_CNT);
	POWER_SYS_SetMode(HSRUN, POWER_MANAGER_POLICY_AGREEMENT);

    /** @brief 板级引脚初始化 */
	BSP_Pins_Init();
	/** @brief 看门狗初始化 */
	//WDG_Init(&wdg_pal1_Instance, &wdg_pal1_Config0);

    /** @brief 创建系统初始化任务 */
	xTaskCreate((TaskFunction_t	) 					OS_Task_Init,
	 			(        char * ) 					"T_Init",
	 			(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE,
	 			(void *	        ) 			    NULL,
	 			(UBaseType_t	) 				    4,
	 			(TaskHandle_t *) 					&Task_Init_Handler);
}

/**
 * @brief 系统任务启动函数，启动任务调度器
 */
void OS_Task_Start(void)
{
	vTaskStartScheduler();
}

/**
 * @brief 外设初始化函数，负责外设初始化
 *
 */
void BSP_Init(void)
{
	BSP_CAN1_Init();
	BSP_CAN2_Init();
	BSP_CAN3_Init();
}

/**
 * @brief 任务创建函数，负责创建系统任务
 */
void Task_Create(void)
{
	xTaskCreate((TaskFunction_t	) 					OS_Task_CanRx_10ms,
	 			(        char * ) 					"OS_Task_CanRx_10ms",
	 			(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE * 3,
	 			(void *	        ) 			    	NULL,
	 			(UBaseType_t	) 				    5,
	 			(TaskHandle_t *) 					&Task_CanRx_Handler );

	xTaskCreate((TaskFunction_t	) 					OS_Task_CanTx_10ms,
	 			(        char * ) 					"OS_Task_Cantx_10ms",
	 			(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE * 3,
	 			(void *	        ) 			    	NULL,
	 			(UBaseType_t	) 				    4,
	 			(TaskHandle_t *) 					&Task_CanTx_Handler );


	xTaskCreate((TaskFunction_t	) 					task_lld_asw_10ms,
	 			(        char * ) 					"Task_asw_10ms",
	 			(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE * 2,
	 			(void *	        ) 			    	NULL,
	 			(UBaseType_t	) 				    6,
	 			(TaskHandle_t *) 					&Task_asw_10ms_Handler );


	xTaskCreate((TaskFunction_t	) 					task_lld_asw_100ms,
	 			(        char * ) 					"Task_asw_100ms",
	 			(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE * 2,
	 			(void *	        ) 			    	NULL,
	 			(UBaseType_t	) 				    6,
	 			(TaskHandle_t *) 					&Task_asw_100ms_Handler );
}

/**
 * @brief 队列创建函数，负责创建系统队列
 */
void Queue_Create(void)
{
	g_can1_rx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));
	g_can2_rx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));
	g_can3_rx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));

	g_can1_tx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));
	g_can2_tx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));
	g_can3_tx_queue = xQueueCreate(10, sizeof(CAN_DATATYPE));

}

/**
 * @brief 系统初始化任务函数，负责系统初始化
 *
 * @param[in] pvPara 无输入参数
 * @return void 无返回值
 */
void OS_Task_Init(void *pvPara)
{
	(void)pvPara;
    /** 进入临界区 */
	taskENTER_CRITICAL();

	// 外设初始化
	BSP_Init();

	// 创建队列
	Queue_Create();

	// 应用层初始化
	task_lld_asw_init();

	// 创建多任务
	Task_Create();

    vTaskDelete(Task_Init_Handler);

    /** 退出临界区*/
    taskEXIT_CRITICAL();
}
/**********************************************************************************************************************
 *  END OF FILE: os_task.c
 *********************************************************************************************************************/
