#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwChip.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwUart.h"
#include "hyhwIntCtrl.h"
#include "hardWare_Resource.h"

#include "ringbuf.h"


#define BT_UART_RX_BUF_SIZE (2 * 1024)
static __align(4) U8 btRxBuffer[BT_UART_RX_BUF_SIZE];
static tRING_BUF tBtRxRingBuf;

static void csrUartRx_ISR(void)
{
	int i;
	char tempData[16];

	for (i=0; i<16; i++)
	{
		if (HY_OK == hyhwUart_CheckRecDataReady(BT_UART))
		{
			hyhwUart_ReadFifo((U8 *)&tempData[i], BT_UART);	//接收数据，可以优化为直接读寄存器
		}
		else
		{
			break;
		}
	}
	if (i > 0)
	{
		//有数据
		RingBuf_Write(&tBtRxRingBuf, (U8 *)&tempData[0], i);
		cond_bt_wakeup();
	}
}



#define UART_BAUDRATE_NUM		(11)
static const U32 uartArrayBaudRate[UART_BAUDRATE_NUM]=
{
	3000000,//	Uart_Baudrate_3000000,
	1000000,//	Uart_Baudrate_1000000,
	500000,//		Uart_Baudrate_500000,
	230400,//		Uart_Baudrate_230400,
	115200,//		Uart_Baudrate_115200,
	57600,//		Uart_Baudrate_57600,
	38400,//		Uart_Baudrate_38400,
	19200,//		Uart_Baudrate_19200,
	9600,//		Uart_Baudrate_9600,
	4800,//		Uart_Baudrate_4800,
	2400,//		Uart_Baudrate_2400
};

int uart_BaudRateConvert(U32 baud)
{
	int i;
	for (i=0; i<UART_BAUDRATE_NUM; i++)
	{
		if (baud == uartArrayBaudRate[i])
		{
			break;
		}
	}
	if (i >= UART_BAUDRATE_NUM)
	{
		return -1;
	}
	return i;
}
void UartDrv_Configure(int baud, char *device)
{
	int uartBaudRate;
	
	uartBaudRate = uart_BaudRateConvert(baud);
	if (uartBaudRate == -1) return;
	
	hyhwUart_Init((eUART_BAUDRATE)uartBaudRate, Uart_DataBits_8,
					Uart_StopBits_1, Uart_Parity_Even,//Uart_Parity_Disable,
					Uart_Fifo_Mode, BT_UART,
					BT_UART_PORT, FLOW_UNSUPPORT,
					DMA_UNSUPPORT);

	RingBuf_init(&tBtRxRingBuf, btRxBuffer, BT_UART_RX_BUF_SIZE);
	
	hyhwUart_rxIntDisable(BT_UART);
	hyhwInt_ConfigUart1_ISR(csrUartRx_ISR);
    hyhwInt_setIRQ(INT_BIT_UART);
    hyhwInt_enable(INT_BIT_UART);
	hyhwUart_rxIntEnable(BT_UART);
}

int hyco_uart_read(char *buf, int len)
{
#ifndef BT_UART_DEBUG
	if (len <= 0) return 0;
	//return RingBuf_Read(&tBtRxRingBuf, buf, len);
	return RingBuf_Read(&tBtRxRingBuf, buf, 128);
#else
	U32 readLen;
	if (len <= 0) return 0;
	//return RingBuf_Read(&tBtRxRingBuf, buf, 128);
	readLen = RingBuf_Read(&tBtRxRingBuf, buf, 128);
	if (readLen > 0)
	{
		hyUsbPrintf("---bt recv %d--- T=%d\r\n", readLen, rawtime(NULL));
		hyUsbData(buf, readLen);
		hyUsbMessage("\r\n---bt recv end---\r\n");
	}
	return readLen;
#endif
}
int hyco_uart_write(char *buf, unsigned int num_to_send)
{
	int ret;
#ifdef BT_UART_DEBUG
	//testPrint(buf, num_to_send);
	hyUsbPrintf("---bt send %d--- T=%d\r\n", num_to_send, rawtime(NULL));
	hyUsbData(buf, num_to_send);
	hyUsbMessage("\r\n---bt send end---\r\n");
#endif
	ret = hyhwUart_sendData(BT_UART, buf, num_to_send);
	if (ret == HY_OK)
	{
		return num_to_send;
	}
	hyUsbMessage("send_err!!\r\n");
	return HY_ERROR;
}
