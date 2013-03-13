#ifndef __GRAP_BUTTON_H__
#define __GRAP_BUTTON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

#define MAX_BUTTON_NAME_LEN	16	//button的名字字串长度。

#define LEFT_POSITION      1
#define CENTER_POSITION    2
#define RIGHT_POSITION     4

//这个结构体是支持button上显示buff图片的形式的
// button上边现在可以显示3种东西：pic(ID形式)，pic(buff形式)，文字
typedef struct tGrapButtonBuffPicInfo_st
{
	U16 off_x1;		//图片要显示的地方相对于button左上角的偏移
	U16 off_y1;
	U16 width;		//图片的宽和高
	U16 height;
	U8  *rgb;		//图片buffer

}tGrapButtonBuffPic;


typedef struct tGrapButtonPriv
{
	U8  *pName;
	U16 cl_normal;     //非按下状态颜色
	U16 cl_focus;	   //按下状态颜色
	U8  font_size;     //字体的大小
	U8  font_type;     //字体类型 unicode，GB等
	U8  position;      //显示位置  居中靠左靠右三种
	U8 	keycode;	//该button绑定的按键键值。Button只响应这个按键。
	U16 backgndPicId;	//需要以背景刷新的图片
	tGrapButtonBuffPic *pBufPic;	//以buff形式传图片时用的buff
	
}tGrapButtonPriv;

typedef struct tGrapButton
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
	
	tGrapButtonPriv btn_privattr;  //name 字符串及显示属性
	PressEnter pressEnter;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;

	U8	state;	// now has only two state: normal and press. later maybe add focus. 0: normal, 1: press.

}tGRAPBUTTON;

tGRAPBUTTON *Grap_InsertButton(void *pView, tGrapViewAttr  *pview_attr, tGrapButtonPriv *pbtn_privattr);

#ifdef __cplusplus
}
#endif

#endif


