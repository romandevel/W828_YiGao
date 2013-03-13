#ifndef __GRAP_CARTOON_H__
#define __GRAP_CARTOON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

typedef S32 (*GetCartoonString)(void *pView, U32 id, U8 *str);

typedef struct tGrapCartoonPriv
{
	U32 flag;			// cartoon type
	U16 focus;
	U16 timeIntv;		//时间间隔，多长时间播放一桢
	U16 lftTimeIntv;	//还差多长时间来播放下一桢
	U16 totFrame;		//动画的总桢数
	U16 curFrame;		//当前播放到哪桢
	U32 *picArray;		//动画中每桢的图片id保存序列
	U16 backgndPicId;	//需要以背景刷新的图片
}tGrapCartoonPriv;

typedef struct tGrapCartoon
{
	// view start 
	tGrapViewAttr view_attr; 

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end

	tGrapCartoonPriv cartoon_privattr;
	
	GetCartoonString getCartString;
	PressEnter pressEnter;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;

}tGRAPCARTOON;

tGRAPCARTOON *Grap_InsertCartoon(void *pView, tGrapViewAttr *pview_attr, tGrapCartoonPriv *pcartoon_privattr);
//可重载函数
S32 Grap_GetCartoonString(void *pView, U32 id, U8 *str);


#define MAX_CARTOON_STR_LEN 80

#define TIMER_MOD 1	//在设置控件级的timer时，最小的Timer触发时间单位（毫秒）


#ifdef __cplusplus
}
#endif

#endif


