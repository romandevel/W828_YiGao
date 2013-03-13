#ifndef __GRAP_EDIT_H__
#define __GRAP_EDIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

#define MAX_DIGITEDIT_LEN 	300

#define CONTAIN_ANY		0
#define	CONTAIN_NUM		1
#define	CONTAIN_CHAR	2
#define CONTAIN_NUM1    4

typedef struct tGrapEdit tGRAPEDIT;
typedef S32 (*GetFocus)(tGRAPEDIT *pView); 
typedef S32 (*EditEnter)(void *pView);
typedef U32 (*EditKeyInput)(void *pView, U32 *keycode);
typedef U32 (*EditKeyEnd)(void *pView, U8 type);//输入或删除结束  type:0 输入   1:删除
typedef S32 (*EditTimer)(void *pView);
typedef S32 (*EditDropListGetStr)(tGRAPEDIT *pView, U16 i, U8 *str);
typedef S32 (*EditDropListDrawScroll)(tGRAPEDIT *pView);
typedef S32 (*EditDropListDrawOne)(tGRAPEDIT *pView, U16 i);
typedef S32 (*EditDrawInputCnt)(tGRAPEDIT *pView); 
typedef S32 (*EditDropListEnter)(tGRAPEDIT *pView, U16 i); 

typedef struct tGrapEditPriv
{
	U8 	*digt;
	U16 color;			//字体颜色 
	U8  font_size;
	U8  font_type;		//gbk,unicode,utf-8
	U8  xoffset;
	U8  yoffset;		//相对控件左上角偏移
	U8  len;			//当前编辑框能够显示的字符个数(因为是简单的编辑框,只有单一宽度字符)
	U8	position;		//对其方式
	U16 containLen;		//可以输入的长度(总长度)
	U16	Pos;			//所对应的在字符串中的位置
	U16 firstCharPos;	//该窗口第一个显示字符在字符串的位置
	U8  containFlag;	//输入限制(0:无限制,可以接收任何字符;1:只能输入数字;2:可以输入汉字以外的字符,4:数字和.)

	U16 coverPicId;			//定义需要刷新的背景图片ID
	
	U8	showState;
	U8	enable;			//光标是否可用
	U16 TimerLft;		//光标闪烁时还差多少时间
	U16 TimerTot;		//光标闪烁一次总间隔时间
	
	U8  view_pwd;       //显示形式 0:字符显示 1:密码显示方式  **************

	U8  mallocFlag;     //控件自动开辟空间还是使用指定的空间 0:控件自动开辟 1:使用自己制定的(和digt配合使用)
						/* 注意 : 使用指定的空间 要比可输入的长度大2个字节！！！！ */	

	U8  scanEnable;     // 是否接受扫描数据

	U8  input ;    /* 输入法类型 0:关闭  1:123  2:abc  3:ABC  4:手写  5:拼音*/

	U8	sendOK;		//扫描成功后是否自动发送一个OK键信息  0:不发送   1:发送

}tGrapEditPriv;


//edit附属下拉框信息
typedef struct _drop_list_t
{
	U8	state;	//0:未展开   1:展开
	U8	laststate;	//上次状态
	U8	direction;	//0:向下  1:向上
	U8	high;	//展开后的高度
	U8	listnum;	//展开后一屏可以显示的条数
	U8	total;	//总条数
	U8	focus;	//当前焦点项 从0开始计数  0xff表示没有焦点项
	U8	top;
	U16	backColor;	//展开时底图颜色
	U16 focusColor;	//焦点项颜色
	
}tDROPLIST;

typedef struct tGrapEdit//增加模糊查找功能.下方显示出来查询的字串 (还未实现)
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

	tGrapEditPriv edit_privattr;

	RecvSysMsg recvSysMsg;
	GetFocus   get_focus; //当焦点发生改变的时候，需要做输入法等显示操作的时候可以重载该函数  		
	RecvPullMsg recvPullMsg;

	EditEnter	editenter;
	EditKeyInput	editKeyInput;
	EditKeyEnd		editKeyEnd;
	EditTimer		editTimer;
	EditDropListGetStr dropListGetStr;
	EditDropListDrawScroll dropListDrawScroll;
	EditDropListDrawOne    dropListDrawOne;
	EditDrawInputCnt	   DrawInputCnt;
	EditDropListEnter	   dropListEnter;
	
	tDROPLIST	dropList;
	
	U8  pullflag;		//拖拽标志 bit0:0:未产生过拖拽 1:产生了拖拽 bit1:0:向前拖拽 1:向后拖拽
	U8  charh;
	U8  charw;
	U16 disLenBack;//有些输入法展开后需要调整edit位置,可能导致私有属性中的len有变动,这里填写变动后的值
	
}tGRAPEDIT;

tGRAPEDIT *Grap_InsertEdit(void *pView, tGrapViewAttr *pview_attr, tGrapEditPriv *pedit_privattr);

#ifdef __cplusplus
}
#endif

#endif


/*
2012-09-12 孔祥文  增加下拉功能,根据输入框中输入的内容,决定下拉框中应该显示的内容;
下拉框默认向下,除非下方不够展开,默认下拉框展开后最多显示4条信息,超过4条需要滚动;
下拉框展开后可通过上下按键选择;
*/