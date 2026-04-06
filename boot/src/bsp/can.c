#include "./bsp/can.h"
#include "Cpu.h"

char IRQ_CAN0_RX;
can_message_t recvMsg_CAN0;

//********************************************************************************************
//@fun     : CAN0_Callback_Func
//@brief   : CAN0 回调函数
//@param   : instance 		- CAN实例
//            event 		- 事件类型
//            buffIdx 		- 缓冲区索引
//            flexcanState 	- CAN状态
//@return  : 无
//********************************************************************************************

void CAN0_Callback_Func (uint32_t instance,can_event_t event,uint32_t buffIdx,void *flexcanState)
{
	(void)flexcanState;//
	(void)instance;
	(void)buffIdx;
	CAN_Receive(&can_pal0_instance, RX_MAILBOX_CAN0, &recvMsg_CAN0);
	switch(event)
	{
		case CAN_EVENT_RX_COMPLETE:
			IRQ_CAN0_RX =1;
			break;
		case CAN_EVENT_TX_COMPLETE:
			break;
		default:
			break;
	}
}

//********************************************************************************************
// @fun     : CAN0_Init
// @brief   : CAN0 初始化
// @param   : 无
// @return  : 无
//********************************************************************************************

static void CAN0_Init(void)
{
	  CAN_Init(&can_pal0_instance, &can_pal0_Config0);
	  can_buff_config_t Rx_buffCfg =  {
	      .enableFD = false,
	      .enableBRS = false,
	      .fdPadding = 0U,
	      .idType = CAN_MSG_ID_STD,
	      .isRemote = false
	  };

	  can_buff_config_t Tx_buffCfg =  {
	      .enableFD = false,
	      .enableBRS = false,
	      .fdPadding = 0U,
	      .idType = CAN_MSG_ID_STD,
	      .isRemote = false
	  };
	  CAN_ConfigRxBuff(&can_pal0_instance, RX_MAILBOX_CAN0, &Rx_buffCfg, Rx_Filter); 
	  CAN_ConfigTxBuff(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_buffCfg); 

	  CAN_SetRxFilter(&can_pal0_instance,CAN_MSG_ID_STD,RX_MAILBOX_CAN0,0); 
	  CAN_InstallEventCallback(&can_pal0_instance,&CAN0_Callback_Func,(void*)0); 
	  CAN_Receive(&can_pal0_instance, RX_MAILBOX_CAN0, &recvMsg_CAN0); 
}

//********************************************************************************************
// @fun     : can_init
// @brief   : CAN 初始化
// @param   : 无
// @return  : 无
//********************************************************************************************

void can_init(void)
{
	CAN0_Init();
}

//********************************************************************************************
// @fun     : can_send
// @brief   : CAN 发送
// @param   : id 	- 消息ID
//            data 	- 消息数据
//            len 	- 消息长度
// @return  : 无
//********************************************************************************************

void can_send(uint32_t id,uint8_t *data,uint8_t len)
{
	uint16_t num=0;
	can_message_t TxMsg;
	TxMsg.cs = 0U; 				
	TxMsg.id = id;				
	TxMsg.length = len; 		
	for(uint8_t i=0;i<len;i++)	
	{
		TxMsg.data[i] = data[i];
	}
	if(STATUS_SUCCESS == CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &TxMsg))
	{
		num=10;
	}
	
}

//********************************************************************************************
// @fun     : can_receive
// @brief   : CAN 接收
// @param   : id 	- 消息ID
//            data 	- 消息数据
//            len 	- 消息长度
// @return  : 无
//********************************************************************************************

void can_receive(uint32_t *id,uint8_t *data,uint8_t *len)
{
	*id = recvMsg_CAN0.id; 
	*len = recvMsg_CAN0.length; 
	for(uint8_t i=0;i<*len;i++)
	{
		data[i] = recvMsg_CAN0.data[i]; 
	}
	IRQ_CAN0_RX = 0; 
}


