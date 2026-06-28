/**********************************************************************************************************************
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  os_task_can.c
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
#include "os_task_can.h"
#include "bsp.h"
#include "MultiCAN_lld_cfg.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  @brief  鏍规嵁璺敱閰嶇疆琛ㄥ鐞嗘帴鏀跺埌鐨凜AN鎶ユ枃锛氬瓨鍌ㄦ暟鎹� + 澶氶�氶亾璺敱杞彂
 *  @param  rx_table:  鎸囧悜璇AN閫氶亾鐨凴X鎶ユ枃璺敱琛�
 *  @param  table_size: 璺敱琛ㄦ潯鐩暟閲�
 *  @param  msg:       鎺ユ敹鍒扮殑CAN鎶ユ枃
 *********************************************************************************************************************/
static void can_rx_router_process(const CAN_CUS_RXMSG_TYPE *rx_table, uint16 table_size, const CAN_DATATYPE *msg)
{
    if((NULL == rx_table) || (NULL == msg) || (0U == table_size)) return;

    for(uint16 i = 0U; i < table_size; i++)
    {
        if(rx_table[i].id == msg->ID)
        {
            /* 灏嗘帴鏀跺埌鐨勫師濮嬫姤鏂囨暟鎹瓨鍌ㄥ埌瀵瑰簲鐨勬暟鎹寚閽� */
            if(NULL != rx_table[i].pmg)
            {
                (void)memcpy(rx_table[i].pmg, msg, sizeof(CAN_DATATYPE));
            }

            /* 鏍规嵁璺敱閰嶇疆杩涜澶氶�氶亾杞彂 */
            if(rx_table[i].router_cfg.chn_num > 0U)
            {
                CAN_DATATYPE route_msg;
                (void)memcpy(&route_msg, msg, sizeof(CAN_DATATYPE));

                for(uint8 j = 0U; j < rx_table[i].router_cfg.chn_num; j++)
                {
                    const uint8 target_chn = rx_table[i].router_cfg.chn[j];

                    if((0xFFU == target_chn) || (INVALID_CAN_ID == rx_table[i].router_cfg.ID[j]))
                    {
                        continue;   /* 鏃犳晥閫氶亾鎴栨棤鏁圛D锛岃烦杩� */
                    }

                    /* 鏋勯�犺浆鍙戞姤鏂囷細鏇挎崲涓虹洰鏍嘔D鍜屽抚绫诲瀷 */
                    route_msg.ID       = rx_table[i].router_cfg.ID[j];
                    route_msg.Extended = rx_table[i].router_cfg.Extended[j];
                    route_msg.fd_msg   = rx_table[i].router_cfg.fd_msg[j];

                    switch(target_chn)
                    {
                        case 1U: (void)xQueueSend(g_can1_tx_queue, &route_msg, 0); break;
                        case 2U: (void)xQueueSend(g_can2_tx_queue, &route_msg, 0); break;
                        case 3U: (void)xQueueSend(g_can3_tx_queue, &route_msg, 0); break;
                        default: break;
                    }
                }
            }
            break;   /* ID宸插尮閰嶏紝閫�鍑哄惊鐜� */
        }
    }
}

static void can1_rx(const CAN_DATATYPE *msg)
{
    can_rx_router_process(can_lld_can1_rx_msg_table, can_lld_can1_rx_msg_table_szie, msg);
}

static void can2_rx(const CAN_DATATYPE *msg)
{
    can_rx_router_process(can_lld_can2_rx_msg_table, can_lld_can2_rx_msg_table_szie, msg);
}

static void can3_rx(const CAN_DATATYPE *msg)
{
    can_rx_router_process(can_lld_can3_rx_msg_table, can_lld_can3_rx_msg_table_szie, msg);
}

/**********************************************************************************************************************
 *  @brief  鍗曢�氶亾鎶ユ枃鍙戦�侊紙鏍规嵁甯х被鍨嬮�夋嫨鏍囧噯甯�/鎵╁睍甯э級
 *********************************************************************************************************************/
static void can_tx_send(uint8 channel, const CAN_DATATYPE *msg)
{
    if(NULL == msg) return;

    switch(channel)
    {
        case 1U:
            if(1U == msg->Extended) { BSP_CAN1_Send_EXT(msg->ID, msg->Length, msg->Data); }
            else                    { BSP_CAN1_Send_STD(msg->ID, msg->Length, msg->Data); }
            break;
        case 2U:
            if(1U == msg->Extended) { BSP_CAN2_Send_EXT(msg->ID, msg->Length, msg->Data); }
            else                    { BSP_CAN2_Send_STD(msg->ID, msg->Length, msg->Data); }
            break;
        case 3U:
            if(1U == msg->Extended) { BSP_CAN3_Send_EXT(msg->ID, msg->Length, msg->Data); }
            else                    { BSP_CAN3_Send_STD(msg->ID, msg->Length, msg->Data); }
            break;
        default: break;
    }
}

static void can_tx_send_test(const CAN_DATATYPE *msg)
{

    if(1U == msg->Extended) 
    { 
        BSP_CAN1_Send_EXT(msg->ID, msg->Length, msg->Data); 
    }
    else                   
    { 
        BSP_CAN1_Send_STD(msg->ID, msg->Length, msg->Data); 
    }
}

CAN_DATATYPE test_msg;
void some_function(void)
{

    
    // 初始化CAN消息
    test_msg.ID  = 0x123;        // 消息ID
    test_msg.Length = 8;            // 数据长度
    test_msg.Data[0] = 0x01;     // 数据字节
    test_msg.Data[1] = 0x02;
    test_msg.Data[2] +=1;
    
    can_tx_send_test(&test_msg);  // 调用发送测试函数
}

/**********************************************************************************************************************
 *  @brief  浠庢寚瀹歍X闃熷垪涓彇鍑哄苟鍙戦�佹墍鏈夊緟鍙戞姤鏂�
 *********************************************************************************************************************/
static void can_tx_drain_queue(uint8 channel, QueueHandle_t queue)
{
    CAN_DATATYPE send_msg;

    while(pdTRUE == xQueueReceive(queue, &send_msg, 0))
    {
        can_tx_send(channel, &send_msg);
    }
}

void OS_Task_CanRx_10ms(void *pvPara)
{
    (void)pvPara;
    TickType_t LastTick = xTaskGetTickCount();
    CAN_DATATYPE recv_msg;

    while(1)
    {
        if(pdTRUE == xQueueReceive(g_can1_rx_queue, &recv_msg, 0))
        {
            can1_rx(&recv_msg);
        }
        if(pdTRUE == xQueueReceive(g_can2_rx_queue, &recv_msg, 0))
        {
            can2_rx(&recv_msg);
        }
        if(pdTRUE == xQueueReceive(g_can3_rx_queue, &recv_msg, 0))
        {
            can3_rx(&recv_msg);
        }
        some_function();
        vTaskDelayUntil(&LastTick, TASK_PERIOD_10MS);
    }
}

void OS_Task_CanTx_10ms(void *pvPara)
{
    (void)pvPara;
    TickType_t LastTick = xTaskGetTickCount();

    while(1)
    {

        vTaskDelayUntil(&LastTick, TASK_PERIOD_10MS);
    }
}
/**********************************************************************************************************************
 *  END OF FILE: os_task_can.c
 *********************************************************************************************************************/
