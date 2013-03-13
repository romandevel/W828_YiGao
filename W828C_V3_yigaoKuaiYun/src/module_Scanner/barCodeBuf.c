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


/*	解码程序在timer中断中调用，该timer中断的优先级最低
	解码后的数据放在一个ring buf中，在主程序中扫描该buf，
	获取条码字符数据。
*/
#define BARCODE_BUF_SIZE		(128)//(256)//(1024)//
#define BARCODE_REMAIN_SIZE		(32)
typedef struct barcode_buf_st
{
	U8	buf[BARCODE_BUF_SIZE];
	U16	readPos;
	U16	writePos;
	U16	valideNum;
}BARCODE_BUF_st;

BARCODE_BUF_st gtbarCodeBuf;


/*-----------------------------------------------------------------
* 函数: barBuf_writeBarCode
* 功能: 向barBuf中写入数据
* 参数: pData----数据buf
*		len-----需要写入数据的长度
* 返回: 实际写入的len
*-----------------------------------------------------------------*/
int barBuf_writeBarCode(U8 *pData, int len)
{
	U16 readPos, writePos, valideNum;
	U8 *pbuf;
	
	if (len <= 0) return 0;
	
	//readPos		= gtbarCodeBuf.readPos;
	writePos	= gtbarCodeBuf.writePos;
	valideNum	= gtbarCodeBuf.valideNum;
	pbuf		= &gtbarCodeBuf.buf[0];
	
	if ((BARCODE_BUF_SIZE-valideNum) < BARCODE_REMAIN_SIZE)
	{
		/* 空间不足，返回0长度 */
		return 0;
	}
	
	/* 空间足够，copy数据 */
	{
		int tempLen;
		tempLen = BARCODE_BUF_SIZE - writePos;
		/* 剩余空间不是连续的 */
		if (tempLen >= len)
		{
			/* buf writePos后端的空间足够 */
			memcpy(&pbuf[writePos], pData, len);
		}
		else
		{
			/* buf writePos后端的空间不足，需要copy两次 */
			memcpy(&pbuf[writePos], pData, tempLen);
			memcpy(&pbuf[0], &pData[tempLen], len - tempLen);
		}
	}
	writePos += len;
	if (writePos >= BARCODE_BUF_SIZE)
	{
		writePos -= BARCODE_BUF_SIZE;
	}
	gtbarCodeBuf.writePos	= writePos;
	//gtbarCodeBuf.valideNum	= valideNum + len;
	gtbarCodeBuf.valideNum	+= len;
	return len;
}

/*-----------------------------------------------------------------
* 函数: barBuf_readBarCode
* 功能: 从barBuf中读取条码数据，以BARCODE_END_CHAR为一组数据的判别条件
* 参数: pData----数据buf
*		len-----需要写入数据的长度
* 返回: 0----读取成功
		-1---读取失败
*-----------------------------------------------------------------*/
int barBuf_readBarCode(U8 *pData, int *pLen)
{
	U16 readPos, tempReadPos, writePos, valideNum, len;
	U8	*pbuf, *pbufLimit;
	U8	flag;
	int ret = -1;
	
	if (pData==NULL || pLen==NULL)
	{
		return -1;
	}
	
	valideNum	= gtbarCodeBuf.valideNum;
	if (valideNum == 0)
	{
		*pLen = 0;
		return 0;
	}
	
	readPos		= gtbarCodeBuf.readPos;
	tempReadPos	= readPos;
	//writePos	= gtbarCodeBuf.writePos;
	pbuf		= &gtbarCodeBuf.buf[readPos];
	pbufLimit	= &gtbarCodeBuf.buf[BARCODE_BUF_SIZE];
	
	/* 搜索一组完整条码 */
	len = 0;
	flag = 0;
	while(valideNum--)
	{
		if (*pbuf == BARCODE_END_CHAR)
		{
		#if 0
			if (len == 0)
			{
				/* buf第一个字符就是条码结束符，则跳过 */
				tempReadPos++;
				pbuf++;
				continue;
			}
			else
		#endif
			{
				flag = 1;
				break;
			}
		}
		pbuf++;
		if (pbuf >= pbufLimit)
		{
			/* buf回头查找 */
			pbuf = &gtbarCodeBuf.buf[0];
		}
		
		len++;
	}
	if (flag == 1)
	{
		/* 找到一组完整条码 */
		int tempLen;
		
		tempLen = BARCODE_BUF_SIZE - tempReadPos;
		if (tempLen >= len)
		{
			/* 条码数据在一个整段上 */
			memcpy(&pData[0], &gtbarCodeBuf.buf[tempReadPos], len);
		}
		else
		{
			/* 条码数据在两个段上 */
			memcpy(&pData[0], &gtbarCodeBuf.buf[tempReadPos], tempLen);
			memcpy(&pData[tempLen], &gtbarCodeBuf.buf[0], len-tempLen);
		}
		tempReadPos += (len + 1);	/* 加上条码结束符 */
		if (tempReadPos >= BARCODE_BUF_SIZE)
		{
			tempReadPos -= BARCODE_BUF_SIZE;
		}
		
		gtbarCodeBuf.readPos	= tempReadPos;
		//gtbarCodeBuf.valideNum	= valideNum;
		gtbarCodeBuf.valideNum	-= (len + 1);
		*pLen = len;
		return 0;
	}
	else
	{
		/* 没有找到一组完整条码 */
		*pLen = 0;
		return -1;
	}
	return ret;
}

/*-----------------------------------------------------------------
* 函数: barBuf_readData
* 功能: 从barBuf中读取数据
* 参数: pData----数据buf
*		readLen--需要读取的数据长度
* 返回: 实际读取的len
*-----------------------------------------------------------------*/
int barBuf_readData(U8 *pData, U16 readLen)
{
	U16 readPos, writePos, valideNum, left, actRd;
	U8 *pBuf;
	
	if (pData==NULL || readLen==0)
	{
		return -1;
	}
	
	valideNum	= gtbarCodeBuf.valideNum;
	if (valideNum == 0)
	{
		return 0;
	}
	
	readPos		= gtbarCodeBuf.readPos;
	writePos	= gtbarCodeBuf.writePos;
	pBuf		= &gtbarCodeBuf.buf[0];
	actRd		= 0;
	
	if(readPos > writePos)
	{
		left = BARCODE_BUF_SIZE - readPos;
		if(left >= readLen)
		{
			actRd = readLen;
			memcpy(&pData[0], &pBuf[readPos], actRd);
		}
		else
		{
			if((left+writePos) >= readLen)
			{
				actRd = readLen;
			}
			else
			{
				actRd = left + writePos;
			}
			memcpy(&pData[0], &pBuf[readPos], left);
			memcpy(&pData[left], &pBuf[0], actRd-left);
		}
		readPos += actRd;
		if(readPos >= BARCODE_BUF_SIZE) readPos -= BARCODE_BUF_SIZE;
		gtbarCodeBuf.readPos = readPos;
	}
	else if(readPos < writePos)
	{
		left = writePos - readPos;
		actRd = (left >= readLen)?readLen:left;
		memcpy(&pData[0], &pBuf[readPos], actRd);
		readPos += actRd;
		if(readPos >= BARCODE_BUF_SIZE) readPos -= BARCODE_BUF_SIZE;
		gtbarCodeBuf.readPos = readPos;
	}
	
	return actRd;
}


/*-----------------------------------------------------------------
* 函数: barBuf_init
* 功能: 初始化barBuf
* 参数: none
* 返回: none
*-----------------------------------------------------------------*/
void barBuf_init(void)
{
	memset(&gtbarCodeBuf, 0x00, sizeof(BARCODE_BUF_st));
}