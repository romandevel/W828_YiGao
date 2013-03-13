#ifndef __GRAP_DESKBOX_H__
#define __GRAP_DESKBOX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"



//行列高宽
typedef struct _tGRAP_RECT
{
    U16 row; 
    U16 col;
    U16 hig;
    U16 wid;
}tGRAP_RECT;



#define RETURN_TOSWITCHMODE		200

/* 窗体结构*/
typedef struct tGrapDeskBox
{
	// view start 
	tGrapViewAttr	view_attr; 	/* 窗体的公共属性 */

	HandleEvent		handle;
	Draw			draw;
	Destroy			destroy;
	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end
	
	struct tGrapView *pTail;//本窗体的最后一个控件
	
	tGRAPZORDER		*pZorderList;	/* 给控件排序*/


	tMSG_QUEUE		*pGetMsg;
	tMSG_QUEUE		**pPutMsg;
	
	tMSG_BODY		*ptGetMsgBody;
	tMSG_PUT_PARAM	*ptPutMsgParam;
	
	U32             *pPullChild;
	U32             lastPoint;
    U32             pullTimeOut;
    
	U8				*pKeyCodeArr;//指向按键数组
	U8				*pUIFocus;	/* */
	U8				*pQuitCnt;	//窗体退出层数，（即该值为几就退出几层），为0xff时表示全部退出
	
    U8				kDlgState;//记录对话框点击状态
    U8				inputTypes;//当前窗体输入法个数; bit0(123)   bit1(abc)  bit2(ABC)  bit3(手写)  bit4(拼音)  对应bit为1时则表示该窗体可以使用对应的输入法,否则没有; 如果该值为0,则不含输入法
    U8				inputMode;//当前窗体输入法  0: 关闭  1:123   2:abc   3:ABC  4:手写  5:拼音
    U8				scanEnable;//当前窗体是否可以扫描  0:否   1:是
    U16				editMinId;	//当前窗体上希望切换焦点的最小edit控件id
    U16				editMaxId;	//
    
    void            *pdata;
 	void			*pT9;
    void            (*threadCallback)(void *pDesk);//进程回调函数指针，在进程初始化时赋值，可继承
   	char 			*pDisName;//显示窗体上的名字
   	
    struct tGrapDeskBox *pFather; //父窗体的指针
    
}tGRAPDESKBOX;

//在desk阻塞后再次运行的时候需要重载的函数指针, pView为当前desk指针
typedef S32 (*DeskSpecialAction)(tGRAPDESKBOX *pDesk,GRAP_EVENT *pEvent);
//直接返回主界面
typedef S32 (*ReturnToSwitchMode)(tGRAPDESKBOX *pDesk,U8 Pid_Flag);//kong
extern ReturnToSwitchMode Grap_ReturnToSwitchMode;//kong

S32 Grap_DeskboxHandle(void *pView, GRAP_EVENT *pEvent);
tGRAPDESKBOX *Grap_CreateDeskbox(tGrapViewAttr *pview_attr, U8 *pDisplayName);
tGRAPDESKBOX *Grap_CreateDeskboxNoCom(tGrapViewAttr *pview_attr, U8 *pDisplayName);
S32 Grap_DeskboxRun(tGRAPDESKBOX *pDesk);
S32 Grap_DeskboxDestroy(void *pView);
S32 Grap_DeskboxDraw(void *pView, U32 xy, U32 wh);


S32 Grap_UpdateZorder(void *pView, tGRAPZORDER *pZorder) ;
S32 Grap_ZorderListDestroy(void *pView);

extern DeskSpecialAction Grap_DeskSpecialAction;

//直接返回到主界面
S32 Grap_Return2Switch(tGRAPDESKBOX *pDesk,U8 Pid_Flag);


#define GRAP_INWIDGET_RANGE(x,y,pView) (x >= pView->view_attr.touchX1 && x < pView->view_attr.touchX2 && \
                		            y >= pView->view_attr.touchY1 && y < pView->view_attr.touchY2)
 

#ifdef __cplusplus
}
#endif

#endif


