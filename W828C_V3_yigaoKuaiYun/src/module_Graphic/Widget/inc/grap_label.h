#ifndef __GRAP_LABEL_H__
#define __GRAP_LABEL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

typedef S32 (*UpdateString)(void *pView, U8 *str);
typedef void (*Label_TimerISR)(void *pView);
typedef S32 (*LabelEnter)(void *pView, U16 viewX, U16 viewY);
typedef S32 (*GetNewString)(void *pView);
typedef struct tGrapLabelPriv
{
	U8 	*pStr;
	U16	cl_normal;
	U16 cl_focus;
	U8  font_size;
	U8  font_type;
	U8  position;
	U8  focus;
	U16 backgndPicId;	//需要以背景刷新的图片

}tGrapLabelPriv;

typedef struct tLabelTimer
{
	U8	enable;			
	U16 TimerLft;		
	U16 TimerTot;		
}tLabelTimer;

typedef struct tGrapLabel
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

	tGrapLabelPriv label_privattr;
	//UpdateString updateString;
	
	tLabelTimer label_timer;
	Label_TimerISR labelTimerISR;
	LabelEnter labelEnter;
	GetNewString getnewstring;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;
		
}tGRAPLABEL;

tGRAPLABEL *Grap_InsertLabel(void *pView, tGrapViewAttr  *pview_attr, tGrapLabelPriv *plabel_privattr);
tGRAPLABEL *Grap_CreateLabel(tGrapViewAttr *pview_attr, tGrapLabelPriv *plabel_privattr);
S32 Grap_GetNewString(void *pView);

#ifdef __cplusplus
}
#endif

#endif


