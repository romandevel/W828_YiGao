#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "hyTypes.h"

typedef struct _ringbuf_t
{
	U16	rd;
	U16	wr;
	U16	size;
	U16	havelen;
	U8	*pBuf;
	
}tRING_BUF;

/*-----------------------------------------------------------------
* 函数:	RingBuf_init
* 功能:	初始化ring buffer的结构，同时指定buffer
* 参数:	pRingBuf-----管理结构
*		pBuf---------buffer
*		buflen-------buffer长度
* 返回:	none
*-----------------------------------------------------------------*/
void RingBuf_init(tRING_BUF *pRingBuf, U8 *pBuf, U16 buflen);
/*-----------------------------------------------------------------
* 函数:RingBuf_Write
* 功能:向指定ringbuf写入指定长度的数据
* 参数:
* 返回:实际写入的长度
*-----------------------------------------------------------------*/
U16 RingBuf_Write(tRING_BUF *pRingBuf, U8 *pWrite, U16 writeLen);
/*-----------------------------------------------------------------
* 函数:RingBuf_Read
* 功能:从指定ringbuf读取指定长度的数据
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_Read(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen);
/*-----------------------------------------------------------------
* 函数:RingBuf_PreRead
* 功能:从指定ringbuf读取指定长度的数据，只读取，不改变ring的指针
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_PreRead(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen);
/*-----------------------------------------------------------------
* 函数:RingBuf_ConfirmRead
* 功能:从指定ringbuf读取指定长度的数据，只改变指针，不读取内容，
*		要与RingBuf_PreRead配对连续使用，否则会导致读取数据内容错误
* 参数:
* 返回:实际读取的长度
*-----------------------------------------------------------------*/
U16 RingBuf_ConfirmRead(tRING_BUF *pRingBuf, U16 readLen);
/*-----------------------------------------------------------------
* 函数:RingBuf_getValidNum
* 功能:从指定ringbuf获得有效数据
* 参数:
* 返回:有效数据个数
*-----------------------------------------------------------------*/
U16 RingBuf_getValidNum(tRING_BUF *pRingBuf);



#ifdef __cplusplus
}
#endif

#endif //_RING_BUF_H_