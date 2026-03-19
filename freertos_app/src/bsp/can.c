#include "./bsp/can.h"
#include "Cpu.h"

char IRQ_CAN0_RX;
can_message_t recvMsg_CAN0;

//********************************************************************************************
//函数名称：CAN0_Callback_Func
//函数功能：CAN0回调函数
//输入参数：instance:实例索引；event:事件类型；buffIdx:缓冲区索引；flexcanState:驱动状态
//输出参数：无
//返回值：无
//********************************************************************************************

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

//********************************************************************************************
//函数名称：CAN0_Init
//函数功能：CAN0初始化函数
//输入参数：无
//输出参数：无
//返回值：无
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
	  CAN_ConfigRxBuff(&can_pal0_instance, RX_MAILBOX_CAN0, &Rx_buffCfg, Rx_Filter); //注册接收配置和MSGID过滤器(如过滤器配置为0x1，则只接受msgid 0x1发来的报文)
	  CAN_ConfigTxBuff(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_buffCfg); //配置发送
	  /*设置MSGID的掩码，掩码粗略可以理解为对11bit MSGID地址的过滤
	  如果某bit位需要过滤设置为1,不过滤设置为0,例如掩码设置为0x7ff则过滤全部标准id,如果设置为0x7fe,这只能接受0x01的报文(不存在0x0的地址)*/
	  CAN_SetRxFilter(&can_pal0_instance,CAN_MSG_ID_STD,RX_MAILBOX_CAN0,0); //设置MSGID掩码，
	  CAN_InstallEventCallback(&can_pal0_instance,&CAN0_Callback_Func,(void*)0); //注册回调函数
	  CAN_Receive(&can_pal0_instance, RX_MAILBOX_CAN0, &recvMsg_CAN0); //*****重点****此函数不只有接收作用 还有续订回调函数的作用.
}

//********************************************************************************************
//函数名称：can_init
//函数功能：CAN初始化函数
//输入参数：无
//输出参数：无
//返回值：无
//********************************************************************************************
void can_init(void)
{
	CAN0_Init();
}

//********************************************************************************************
//函数名称：can_send
//函数功能：CAN发送函数
//输入参数：id:消息ID；data:数据指针；len:数据长度
//输出参数：无
//返回值：无
//********************************************************************************************
void can_send(uint32_t id,uint8_t *data,uint8_t len)
{
	#if 0
	uint16_t num=0;
	can_message_t TxMsg;
	TxMsg.cs = 0U; 				//此处可以设置一些发送选项,如是否使用CAN FD,是否使用BRS等,具体可以参考can_buff_config_t结构体中的成员
	TxMsg.id = id;				//设置消息ID
	TxMsg.length = len; 		//设置数据长度
	for(uint8_t i=0;i<len;i++)	
	{
		TxMsg.data[i] = data[i];
	}
	if(STATUS_SUCCESS == CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &TxMsg))
	{
		num=10;
	}
		#endif
			can_message_t Tx_msg = {
							 .cs = 0U,
							 .id = 0x01,
							 .data[0] = 0x0,
							 .data[1] = 0x1,
							 .data[2] = 0x2,
							 .data[3] = 0x3,
							 .data[4] = 0x4,
							 .data[5] = 0x5,
							 .data[6] = 0x6,
							 .data[7] = 0x7,
							 .length = 8
						 };
				CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_msg);
}

//********************************************************************************************
//函数名称：can_receive
//函数功能：CAN接收函数
//输入参数：id:消息ID指针；data:数据指针；len:数据长度指针
//输出参数：id:消息ID；data:数据；len:数据长度
//返回值：无
//********************************************************************************************
void can_receive(uint32_t *id,uint8_t *data,uint8_t *len)
{
	*id = recvMsg_CAN0.id; //获取消息ID
	*len = recvMsg_CAN0.length; //获取数据长度
	for(uint8_t i=0;i<*len;i++)
	{
		data[i] = recvMsg_CAN0.data[i]; //获取数据
	}
	IRQ_CAN0_RX = 0; //清除接收标志位
}

void  can_send_test(void)
{
	#if 0
	uint16_t num=0;
	can_message_t TxMsg;
	TxMsg.cs = 0U; 				//此处可以设置一些发送选项,如是否使用CAN FD,是否使用BRS等,具体可以参考can_buff_config_t结构体中的成员
	TxMsg.id = id;				//设置消息ID
	TxMsg.length = len; 		//设置数据长度
	for(uint8_t i=0;i<len;i++)	
	{
		TxMsg.data[i] = data[i];
	}
	if(STATUS_SUCCESS == CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &TxMsg))
	{
		num=10;
	}
		#endif
			can_message_t Tx_msg = {
							 .cs = 0U,
							 .id = 0x01,
							 .data[0] = 0x0,
							 .data[1] = 0x1,
							 .data[2] = 0x2,
							 .data[3] = 0x3,
							 .data[4] = 0x4,
							 .data[5] = 0x5,
							 .data[6] = 0x6,
							 .data[7] = 0x7,
							 .length = 8
						 };
				CAN_Send(&can_pal0_instance, TX_MAILBOX_CAN0, &Tx_msg);
}