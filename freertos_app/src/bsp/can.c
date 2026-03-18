#include "./bsp/can.h"
#include "Cpu.h"

char IRQ_CAN0_RX;
can_message_t recvMsg_CAN0;


void CAN0_Callback_Func (uint32_t instance,can_event_t event,uint32_t buffIdx,void *flexcanState)
{
	(void)flexcanState;//此处防止警报
	(void)instance;
	(void)buffIdx;
	CAN_Receive(&can_pal0_instance, RX_MAILBOX_CAN0, &recvMsg_CAN0); //接收报文并重新注册回调函数
	switch(event)//回调事件
		{
			case CAN_EVENT_RX_COMPLETE: //接收完成 事件
				IRQ_CAN0_RX =1;
				break;
			case CAN_EVENT_TX_COMPLETE: //发送完成事件
				break;
			default:
				break;
		}
	}

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
	  CAN_ConfigRxBuff(&can_pal0_instance, RX_MAILBOX_CAN0, &Rx_buffCfg, Rx_Filter); //注册接收配置和MSGID过滤器(如过滤器配置为0x1，则只接受msgid 0x1发来的报文)
	  CAN_ConfigTxBuff(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_buffCfg); //配置发送
	  /*设置MSGID的掩码，掩码粗略可以理解为对11bit MSGID地址的过滤
	  如果某bit位需要过滤设置为1,不过滤设置为0,例如掩码设置为0x7ff则过滤全部标准id,如果设置为0x7fe,这只能接受0x01的报文(不存在0x0的地址)*/
	  CAN_SetRxFilter(&can_pal0_instance,CAN_MSG_ID_STD,RX_MAILBOX_CAN0,0); //设置MSGID掩码，
	  CAN_InstallEventCallback(&can_pal0_instance,&CAN0_Callback_Func,(void*)0); //注册回调函数
	  CAN_Receive(&can_pal0_instance, RX_MAILBOX_CAN0, &recvMsg_CAN0); //*****重点****此函数不只有接收作用 还有续订回调函数的作用.
}
void can_init(void)
{
	CAN0_Init();
}
