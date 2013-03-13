#ifndef __GRAP_EVENT_H__
#define __GRAP_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

// 消息结构体
typedef struct tGrapEvent 
{
	void *pView;
	U32  id;
	U32	 message;
	U32	 wParam;
	U32  lParam;
	U32	 reserve;
	U32  extParam; //消息扩张
}GRAP_EVENT;

//消息池是一个消息数组（容量为EVENTNUMINPOOL），EventHead中将记录:
//	struct tEvent *pEventTop: 消息池的开始消息地址(实际物理地址)
//	struct tEvent *pEventBtm: 消息池的最后消息地址的下一个(实际物理地址)
//	struct tEvent *pEventCur: 消息池中当前消息的第一个消息
//	struct tEvent *pEventEnd: 消息池中当前消息的最后一个消息
//pEventTop和pEventBtm是为了形成循环数组而设的；pEventCur和pEventEnd是真正用到的消息位置。
//不过以上应该都是被封装起来的，使用层是不用考虑的，e

S32 Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent);

#ifdef __cplusplus
}
#endif

#endif

