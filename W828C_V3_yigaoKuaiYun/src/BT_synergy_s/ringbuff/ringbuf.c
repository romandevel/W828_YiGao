#include "ringbuf.h"


/*--------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------
* 函数:	RingBuf_init
* 功能:	初始化ring buffer的结构，同时指定buffer
* 参数:	pRingBuf-----管理结构
*		pBuf---------buffer
*		buflen-------buffer长度
* 返回:	none
*-----------------------------------------------------------------*/
void RingBuf_init(tRING_BUF *pRingBuf, U8 *pBuf, U16 buflen)
{
	memset((char *)pRingBuf, 0, sizeof(tRING_BUF));
	
	pRingBuf->size = buflen;
	pRingBuf->pBuf = pBuf;
	
	return ;
}

/*-----------------------------------------------------------------
* 函数:RingBuf_Write
* 功能:向指定ringbuf写入指定长度的数据
* 参数:
* 返回:实际写入的长度
*-----------------------------------------------------------------*/
U16 RingBuf_Write(tRING_BUF *pRingBuf, U8 *pWrite, U16 writeLen)
{
	U16		left,actWr=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	
	if(len+writeLen >= size)//不能写入全部时就一点也不写
	{
		return 0;
	}
	
	left = size-len-1;
	actWr = (left >= writeLen)? writeLen:left;
	if(wr >= rd)
	{
		left = size - wr;
		if(left >= actWr)
		{
			memcpy(&pBuf[wr], pWrite, actWr);
		}
		else
		{
			memcpy(&pBuf[wr], pWrite, left);
			memcpy(&pBuf[0], &pWrite[left], actWr-left);
		}
	}
	else// if(wr < rd)
	{
		memcpy(&pBuf[wr], pWrite, actWr);
	}
	
	wr += actWr;
	if(wr >= size) wr -= size;
	pRingBuf->wr = wr;
	pRingBuf->havelen += actWr;
	
	return actWr;
}

/*-----------------------------------------------------------------
* 函数:RingBuf_Read
* 功能:从指定ringbuf读取指定长度的数据
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_Read(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen)
{
	U16		left,actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
	if(len == 0)
	{
		return 0;
	}
	actRd = (len >= readLen)?readLen:len;
	if(rd > wr)
	{
	  	left = size - rd;
		if(left >= actRd)
		{
			memcpy(&pRead[0], &pBuf[rd], actRd);
		}
		else
		{
			memcpy(&pRead[0], &pBuf[rd], left);
			memcpy(&pRead[left], &pBuf[0], actRd-left);
		}
		rd += actRd;
		if(rd >= size) rd -= size;
		pRingBuf->rd = rd;
		pRingBuf->havelen -= actRd;
	}
	else if(rd < wr)
	{
		memcpy(&pRead[0], &pBuf[rd], actRd);
		
		rd += actRd;
		if(rd >= size) rd -= size;
		pRingBuf->rd = rd;
		pRingBuf->havelen -= actRd;
	}
	
	return actRd;
}

/*-----------------------------------------------------------------
* 函数:RingBuf_PreRead
* 功能:从指定ringbuf读取指定长度的数据，只读取，不改变ring的指针
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_PreRead(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen)
{
	U16		left,actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
	if(len == 0)
	{
		return 0;
	}
	actRd = (len >= readLen)?readLen:len;
	if(rd > wr)
	{
	  	left = size - rd;
		if(left >= actRd)
		{
			memcpy(&pRead[0], &pBuf[rd], actRd);
		}
		else
		{
			memcpy(&pRead[0], &pBuf[rd], left);
			memcpy(&pRead[left], &pBuf[0], actRd-left);
		}
		rd += actRd;
		if(rd >= size) rd -= size;
		//pRingBuf->rd = rd;
		//pRingBuf->havelen -= actRd;
	}
	else if(rd < wr)
	{
		memcpy(&pRead[0], &pBuf[rd], actRd);
		
		rd += actRd;
		if(rd >= size) rd -= size;
		//pRingBuf->rd = rd;
		//pRingBuf->havelen -= actRd;
	}
	
	return actRd;
}

/*-----------------------------------------------------------------
* 函数:RingBuf_ConfirmRead
* 功能:从指定ringbuf读取指定长度的数据，只改变指针，不读取内容，
*		要与RingBuf_PreRead配对连续使用，否则会导致读取数据内容错误
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_ConfirmRead(tRING_BUF *pRingBuf, U16 readLen)
{
	U16		actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
#if 1
	if(readLen > len)
	{
		pRingBuf->havelen = 0;
		pRingBuf->rd = pRingBuf->wr;
	}
	else
	{
		len -= readLen;
		rd += readLen;
		if(rd >= size) rd -= size;
		pRingBuf->havelen = len;
		pRingBuf->rd = rd;
	}
#else
	//只调整rd值
	rd += readLen;
	if(rd >= size) rd -= size;
	//intLev = InterruptDisable();
	pRingBuf->rd = rd;
	//InterruptRestore(intLev);
#endif	
	return actRd;	
}

/*-----------------------------------------------------------------
* 函数:RingBuf_getValidNum
* 功能:从指定ringbuf获得有效数据
* 参数:
* 返回:有效数据个数
*-----------------------------------------------------------------*/
U16 RingBuf_getValidNum(tRING_BUF *pRingBuf)
{
/*	U16	left = 0;
	U16	rd,wr,size;
	U8 intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	//InterruptRestore(intLev);
	
	if(rd > wr)
	{
		left = size - rd;
	}
	else if(rd < wr)
	{
		left = wr - rd;
	}*/
	
	return pRingBuf->havelen;
}
