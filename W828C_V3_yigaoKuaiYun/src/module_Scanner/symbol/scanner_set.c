/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwIntCtrl.h"
#include "scanner.h"

extern const U8 suffixCmd[][9];
extern const U8 Enable_ack[][9];
extern const U8 Disable_ack[][9];

void scannerUart_intIsr(void);
void scannerUart_Setting(void);
void scanner_checkType(void);

void scanner_symbol_set(void)
{
	int ret, i;
	
	for (i=0; i<3; i++)
	{
		syssleep(5);
		ret = scannerUart_sendCmd(suffixCmd[0]);
		//PhoneTrace(0, "suffixCmd0=%d", ret);
		if (ret == HY_ERROR)
		{
			//syssleep(5);
			ret = scannerUart_sendCmd(suffixCmd[1]);
			//PhoneTrace(0, "suffixCmd1=%d", ret);
			if (ret == HY_OK) break;
		}
		else
		{
			break;
		}
	}
	if (i == 3)
	{
		PhoneTrace(0, "suffixCmd err!!");
		//hyUsbMessage("suffixCmd err!!\r\n");
	}
	else
	{
		PhoneTrace(1, "suffixCmd ok!!");
	}
	for (i=0; i<3; i++)
	{
		syssleep(5);
		ret = scannerUart_sendCmd(Disable_ack[0]);
		//PhoneTrace(0, "Disable_ack0=%d", ret);
		if (ret == HY_ERROR)
		{
			//syssleep(5);
			ret = scannerUart_sendCmd(Disable_ack[1]);
			//PhoneTrace(0, "Disable_ack1=%d", ret);
			if (ret == HY_OK) break;
		}
		else
		{
			break;
		}
	}
	if (i == 3)
	{
		PhoneTrace(0, "symbol set err!!");
		//hyUsbMessage("symbol set err!!\r\n");
	}
	else
	{
		PhoneTrace(1, "symbol set ok!!");
	}
	
	barBuf_init();
}

/*-----------------------------------------------------------------------------
* 函数:	scannerUart_intIsr
* 功能:	中断服务程序
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void scannerUart_intIsr(void)
{
	U8		i;
	U8		data[20];
	U8		recvLen;
	
	//接收数据
	memset(data,0,20);
	recvLen = 0;
	for (i=0; i<16; i++)
	{
		if (hyhwUart_CheckRecDataReady(SCANNER_UART)==HY_OK)
		{
			hyhwUart_ReadFifo((U8 *)&data[recvLen++], SCANNER_UART);
		}
    	else
    	{
    		break;
    	}
	}
	if (recvLen > 0)
	{
		//写入的数据不一定是一个完整的条码数据
		barBuf_writeBarCode(&data[0], recvLen);
		//PhoneTrace(0, "wBarCode=%d", recvLen);
	}
}
