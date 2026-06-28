/**********************************************************************************************************************
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  bsp_can3.c
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
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "os_task.h"
#include "bsp_can3.h"

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
static can_buff_config_t Rx_buffCfg_STD =  {
	.enableFD = false,
	.enableBRS = false,
	.fdPadding = 8U,    //data length code (DLC)
	.idType = CAN_MSG_ID_STD, //standard or extended CAN_MSG_ID_STD
	.isRemote = false   //standard or remote
};

static can_buff_config_t Rx_buffCfg_EXT =  {
	.enableFD = false,
	.enableBRS = false,
	.fdPadding = 8U,    //data length code (DLC)
	.idType = CAN_MSG_ID_EXT, //standard or extended CAN_MSG_ID_EXT
	.isRemote = false   //standard or remote
};

static can_buff_config_t Tx_buffCfg_STD =  {
	.enableFD = false,
	.enableBRS = false,
	.fdPadding = 8U,    //data length code (DLC)
	.idType = CAN_MSG_ID_STD, //standard or extended
	.isRemote = false   //standard or remote
};

static can_buff_config_t Tx_buffCfg_EXT =  {
	.enableFD = false,
	.enableBRS = false,
	.fdPadding = 8U,    //data length code (DLC)
	.idType = CAN_MSG_ID_EXT, //standard or extended
	.isRemote = false   //standard or remote
};
/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/
static can_message_t 	recvMsg_CAN_STD;
static can_message_t 	recvMsg_CAN_EXT;

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/
void BSP_CAN3_Send_STD(uint32_t id, uint8_t length, const uint8_t data[64])
{
    uint16_t i;
    can_message_t Tx_msg;

    Tx_msg.id = id;
	Tx_msg.length = length;
	Tx_msg.cs = 0;

	for(i=0; i<length; i++)
	{
		Tx_msg.data[i] = data[i];
	}

	_Bool  EX_Flag = false;
    while(CAN_GetTransferStatus(&can3_instance, TX_MAILBOX_CAN3_STD) != STATUS_SUCCESS){
		if((++i) > 500)
		{
			EX_Flag = true;
			CAN_AbortTransfer(&can3_instance,TX_MAILBOX_CAN3_STD);
			CAN_Send(&can3_instance, TX_MAILBOX_CAN3_STD, &Tx_msg);
			break;
		}
	}
	if (!EX_Flag) CAN_Send(&can3_instance, TX_MAILBOX_CAN3_STD, &Tx_msg);
}

void BSP_CAN3_Send_EXT(uint32_t id, uint8_t length, const uint8_t data[64])
{
    can_message_t Tx_msg;
    uint16_t timeout = 0;
    status_t status;

    Tx_msg.id = id;
    Tx_msg.length = length;
    Tx_msg.cs = 0U;
    memcpy(Tx_msg.data, data, length);

    do {
        status = CAN_GetTransferStatus(&can3_instance, TX_MAILBOX_CAN3_EXT);
        if (status == STATUS_SUCCESS) {
            if (CAN_Send(&can3_instance, TX_MAILBOX_CAN3_EXT, &Tx_msg) == STATUS_SUCCESS) {
                return ;
            }
        }
    } while (++timeout < 500);

    if (CAN_AbortTransfer(&can3_instance, TX_MAILBOX_CAN3_EXT) != STATUS_SUCCESS) {
        return;
    }

    if (CAN_Send(&can3_instance, TX_MAILBOX_CAN3_EXT, &Tx_msg) == STATUS_SUCCESS) {
        return ;
    } else {
        return ;
    }
}
/**
 * @brief CAN3中断回调函数，处理接收和发送完成事件
 *
 * @param instance CAN实例号（未使用）
 * @param event 触发的事件类型（CAN_EVENT_RX_COMPLETE或CAN_EVENT_TX_COMPLETE）
 * @param buffIdx 缓冲区索引（未使用）
 * @param flexcanState FlexCAN状态指针（未使用）
 *
 * @note 此函数在中断上下文中执行，必须保持简短高效
 * @note 接收到的标准帧和扩展帧消息会被分别处理
 * @note 扩展帧消息会被转换为统一格式并通过消息队列发送给上层任务
 *
 * @details 主要功能：
 * - 处理CAN接收完成事件
 * - 读取标准帧和扩展帧消息
 * - 转换扩展帧消息格式
 * - 通过FreeRTOS队列发送消息指针
 * - 清空接收缓冲区标志
 */
void CAN3_Callback(uint32_t instance, can_event_t event, uint32_t buffIdx, void *flexcanState)
{
    (void)flexcanState;
    (void)instance;
	(void)buffIdx;

	CAN_Receive(&can3_instance, RX_MAILBOX_CAN3_STD, &recvMsg_CAN_STD);
	CAN_Receive(&can3_instance, RX_MAILBOX_CAN3_EXT, &recvMsg_CAN_EXT);

	switch(event)
	{
		case CAN_EVENT_RX_COMPLETE:

			if(recvMsg_CAN_STD.id != 0)
			{
			    CAN_DATATYPE temp_msg;
			    temp_msg.ID = recvMsg_CAN_STD.id;
			    temp_msg.Length = recvMsg_CAN_STD.length;
			    temp_msg.Extended = CAN_MSG_ID_STD;
			    temp_msg.fd_msg = 0;
			    temp_msg.available_flg = 1;
			    memcpy(temp_msg.Data, recvMsg_CAN_STD.data, recvMsg_CAN_STD.length);
			    xQueueSendFromISR(g_can3_rx_queue, &temp_msg, 0);

			}

			if(recvMsg_CAN_EXT.id != 0)
			{
			    CAN_DATATYPE temp_msg;
			    temp_msg.ID = recvMsg_CAN_EXT.id;
			    temp_msg.Length = recvMsg_CAN_EXT.length;
			    temp_msg.Extended = CAN_MSG_ID_EXT;
			    temp_msg.fd_msg = 0;
			    temp_msg.available_flg = 1;
			    memcpy(temp_msg.Data, recvMsg_CAN_EXT.data, recvMsg_CAN_EXT.length);
			    xQueueSendFromISR(g_can3_rx_queue, &temp_msg, 0);

            }

			recvMsg_CAN_STD.id = 0;
			recvMsg_CAN_EXT.id = 0;
			break;

		case CAN_EVENT_TX_COMPLETE:
			break;
		default:
			break;
	}
}

/**
 * @brief 初始化CAN控制器
 *
 * 完成CAN控制器的完整硬件配置，包括：
 * - 控制器初始化
 * - 标准帧和扩展帧的收发邮箱配置
 * - 接收过滤器设置（全通模式）
 * - 中断回调函数注册
 * - 中断优先级设置
 * - 启动接收操作
 *
 * @note 该函数应在系统初始化阶段调用，且只调用一次
 * @note 使用前需确保CAN3时钟已使能
 * @note 中断优先级设置为0x02（较高优先级）
 */
void BSP_CAN3_Init(void)
{
    uint32_t Rx_Filter = 0x00;

    CAN_Init(&can3_instance, &can2_Config0);

	CAN_ConfigRxBuff(&can3_instance, RX_MAILBOX_CAN3_STD, &Rx_buffCfg_STD, Rx_Filter);
	CAN_ConfigTxBuff(&can3_instance, TX_MAILBOX_CAN3_STD, &Tx_buffCfg_STD);

	CAN_ConfigRxBuff(&can3_instance, RX_MAILBOX_CAN3_EXT, &Rx_buffCfg_EXT, Rx_Filter);
	CAN_ConfigTxBuff(&can3_instance, TX_MAILBOX_CAN3_EXT, &Tx_buffCfg_EXT);

	CAN_SetRxFilter(&can3_instance, CAN_MSG_ID_STD, RX_MAILBOX_CAN3_STD, 0x0000);
	CAN_SetRxFilter(&can3_instance, CAN_MSG_ID_EXT, RX_MAILBOX_CAN3_EXT, 0x00000000);

	CAN_InstallEventCallback(&can3_instance, &CAN3_Callback, (void*)0);

	INT_SYS_SetPriority(CAN2_ORed_0_15_MB_IRQn, 0x02);

	CAN_Receive(&can3_instance, RX_MAILBOX_CAN3_STD, &recvMsg_CAN_STD);
	CAN_Receive(&can3_instance, RX_MAILBOX_CAN3_EXT, &recvMsg_CAN_EXT);
}
/**********************************************************************************************************************
 *  END OF FILE: bsp_can3.c
 *********************************************************************************************************************/
