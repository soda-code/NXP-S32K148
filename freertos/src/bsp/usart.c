#include "./bsp/usart.h"
#include "Cpu.h"


#define BUFFER_SIZE 256U
uint8_t buffer[200];
uint8_t rxdata[200];
uint8_t bufferIdx;
uint32_t bytesRemaining;
status_t status;
int triger;

void LPUART1_RX_ISR (void *driverState, uart_event_t event, void *userData)
 {
	(void) driverState;
	(void) userData;

	if(LPUART_DRV_GetReceiveStatus(INST_LPUART1, &bytesRemaining) != STATUS_BUSY)
	{
		status = LPUART_DRV_GetReceiveStatus(INST_LPUART1, &bytesRemaining); //串口数据接收成功
		if(status == STATUS_SUCCESS)
		{
			/* 为接收到的字符串添加终止符 */
			bufferIdx++;
			triger =1;
			buffer[bufferIdx] = 0U;
			/* 将接收缓存文件复制到rxdata数组 */
			memcpy(rxdata,buffer,bufferIdx);
			memset(buffer,0,sizeof(buffer));
			//LPUART_DRV_SendDataBlocking(INST_LPUART1, buffer, bufferIdx, 100UL);
			/* Reset the buffer index to start a new reception */
			bufferIdx = 0U;
			/*完成操作后清除中断标志位并重新开始接收串口数据*/
			LPUART_DRV_ReceiveData(INST_LPUART1, buffer, 1U);
		}
		else
		{
			//error handle
			LPUART_DRV_ReceiveData(INST_LPUART1, buffer, 1U);
		}
	   }

	/*串口接收缓存满事件*/
	if(event==UART_EVENT_RX_FULL)
		{
		/*判断是否收到换行符*/
		if ((buffer[bufferIdx] != '\n') && (bufferIdx != (BUFFER_SIZE - 2U)))
			{
			/*每接收一字节数据,就将储存位置指向数组的下一位,直到接收到换行符*/
				bufferIdx++;
				LPUART_DRV_SetRxBuffer(INST_LPUART1, &buffer[bufferIdx], 1U);
			}
		}
}

char USART1_TX_BUF[200];

void u1_printf(char* fmt, ...)
{
    uint32_t bytesRemaining;
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf((char*)USART1_TX_BUF,  sizeof(USART1_TX_BUF),  fmt, ap);
    va_end(ap);

    if (len > 0 && len < sizeof(USART1_TX_BUF)) 
	{
        LPUART_DRV_SendData(INST_LPUART1, (uint8_t*)USART1_TX_BUF,len);
        while (LPUART_DRV_GetTransmitStatus(INST_LPUART1, &bytesRemaining) 
           != STATUS_SUCCESS) {}
    }
}

void usart_init(void)
{

	LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0); //初始化串口
	LPUART_DRV_InstallRxCallback(INST_LPUART1,LPUART1_RX_ISR, NULL);//安装接收中断回调函数
	LPUART_DRV_ReceiveData(INST_LPUART1, buffer, 1); //开启串口接收,每次接收一个字节,并安装中断
}
