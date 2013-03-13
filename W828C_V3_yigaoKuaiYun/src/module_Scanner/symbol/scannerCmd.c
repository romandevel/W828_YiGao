/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "scanner.h"

#define SCANNER_CMD_TIMEOUT		(50)//500ms

#if 1
//扫描输出加后缀0x0D
const U8 suffixCmd[][9]=
{
	0x07, 0xC6, 0x04, 0x08, 0xFF, 0xEB, 0x02, 0xFD, 0x3B,
	0x07, 0xC6, 0x04, 0x09, 0xFF, 0xEB, 0x02, 0xFD, 0x3A
};
#else
const U8 suffixCmd[][9]=
{
	0x07, 0xC6, 0x04, 0x00, 0xFF, 0xEB, 0x00, 0xFD, 0x45,
	0x07, 0xC6, 0x04, 0x01, 0xFF, 0xEB, 0x00, 0xFD, 0x44
};
#endif

const U8 Enable_ack[][9]=
{
	0x07, 0xC6, 0x04, 0x00, 0xFF, 0x9f, 0x01, 0xFD, 0x90,
	0x07, 0xC6, 0x04, 0x01, 0xFF, 0x9f, 0x01, 0xFD, 0x8f
};

const U8 Disable_ack[][9]=
{
	0x07, 0xC6, 0x04, 0x00, 0xFF, 0x9f, 0x00, 0xFD, 0x91,
	0x07, 0xC6, 0x04, 0x01, 0xFF, 0x9f, 0x00, 0xFD, 0x90
};


/*-----------------------------------------------------------------------------
* 函数:	scannerUart_sendCmd
* 功能:	发送命令
* 参数:	cmd_val---要发送的命令数据
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int scannerUart_sendCmd(const U8 *cmd_val)
{
	U16 i, len, timeOut, cnt;
	U8 tempBuf[50], point;
	S8 flag = -1;

	len = *cmd_val+2;
	//PhoneTrace(1, "send cmd");
	for(i=0; i<len; i++)
	{
		hyhwUart_WriteFifo(cmd_val[i], SCANNER_UART);
	}
	
	memset(tempBuf, 0x00, 50);
	/* 等待命令响应 */
	timeOut = SCANNER_CMD_TIMEOUT;
	point = 0;
	while(timeOut--)
	{
		syssleep(1);
		len = barBuf_readData(&tempBuf[point], 50);
		if (len > 0)
		{
			//PhoneTrace(1, "cnt=%d", cnt);
			point = len;
			//PhoneTrace(1, "recv=%02X,%02X", tempBuf[0], tempBuf[1]);
			/* 判断响应值 */
			if ((tempBuf[0]==0x04) && (tempBuf[1]==0xd0))
			{
				flag = 1;
			}
			break;
		}
	}
	//PhoneTrace(1, "flag=%d", flag);
	if (flag == -1)
	{
		/* 没有得到回应 超时 */
		return HY_ERROR;
	}
	return HY_OK;
}

void scannerUart_sendwkup(void)
{
	U32 state = HY_ERROR;
	
	while(state != HY_OK)
	{
		state = hyhwUart_WriteFifo(0x00, SCANNER_UART);
	}
}


