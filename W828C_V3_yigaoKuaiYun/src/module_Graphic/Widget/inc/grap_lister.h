#ifndef __GRAP_LISTER_H__
#define __GRAP_LISTER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"
	
typedef struct tGrapLister tGRAPLIST;


typedef S32 (*UpdateList)(tGRAPLIST *pLister, U16 i);
typedef S32 (*GetListStr)(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen);
typedef S32 (*EnterLister)(tGRAPLIST *pLister, U16 i);
typedef S32 (*ListerTimer)(tGRAPLIST *pLister, U16 i);
typedef S32 (*ListDrawIcon)(tGRAPLIST *pLister,U16 i);//画小图标
typedef S32 (*ChangeListItem)(tGRAPLIST *pLister,U16 i);//当改变focus item时重载函数
typedef S32 (*ListKeyInput)(tGRAPLIST *pLister, U16 keycode);//按键输入,主要用作快捷键进入(1,2,3,4.....)

typedef struct tGrapListerPriv
{
	U8  lstHeight;		//list每行的行高
	U8  lstNum;			//一屏中list的个数
	U16	lstColor;		//list正常背景色
	U16	lstFocusColor;	//list反白背景色
	U16 focusItem;		//当前反白条的idx
	U16	totalItem;		//总加进来的条数
	U16	topItem;		//当前屏最上边一条的idx。(支持list的自动翻屏,第一次进来时初始默认为0)
	U16 backgndPicId;	//需要部分刷新的背景图ID
	U16 scrbarPicId;	//反显图片ID(焦点项反显条)
	
	U8  x_start;		
	U8  y_start;		//开始显示字符的位置, 相对于本行左上角
	U8  curLen;			//该行一屏中能显示的字符数(要显示的字符超过该数时滚动显示)		
	U16 firstCharPos;	//显示在LIST开头的字符在整个字符串中的位置,初值为0
	U16 TimerLft;		
	U16 TimerTot;
	U8	enable;			//字符过多时是否滚动
	U8  font_size;     //字体的大小
	U8  font_type;     //字体类型 unicode，GB等
	U8  position;      //显示位置  居中靠左靠右三种
	U8  isShowCount;   //是否显示 "当前数/总计数"  若需要显示，则控件高度上最后要预留20像素高
	U8	numkeyenable;	//是否使用数字快捷键  1:使用 0:不使用   使用数字快捷解,按数字键可以进入对应序号的项

}tGrapListerPriv;

typedef struct tGrapLister
{
	// view start.
	tGrapViewAttr view_attr;

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end.

	// bellow is Lister property
	tGrapListerPriv lst_privattr;

	UpdateList updateList;
	GetListStr getListString;	// 这个函数必须重载，因为默认函数会为空直接返回。（即画不出字串）
	EnterLister enter;			// 在list画面按回车键时应到达哪个界面。这个函数也必须重载，不然无法进入下层函数。
	ListerTimer	listtimer;
	ListDrawIcon	drawIcon;
	ChangeListItem   changeItem;
	ListKeyInput	keyInput;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;
 	
 	U8	ConfirmAndEnter;		//1:点击后直接进入.0:第一次选中,第二次进入
	U8	lastKey;	//上次按键值  (bit7记录之前的timer是否enable)
	U32 startPos; //拖拽时第一点的位置
	
}tGRAPLIST;

// 开放出来的接口。
tGRAPLIST *Grap_InsertLister(void *pView, tGrapViewAttr *pview_attr, tGrapListerPriv *plst_privattr);

// 以下函数是为了上层便于重载开放的
S32 Grap_ListerUpdateItem(tGRAPLIST *pLister, U16 i);
S32 Grap_ListerEnterItem(tGRAPLIST *pLister, U16 i);
S32 Grap_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen);
S32 Grap_ListerDraw(void *pView, U32 xy, U32 wh);
S32 Grap_ListerHandle(void *pView, GRAP_EVENT *pEvent);
S32 Grap_ListerTimer(tGRAPLIST *pLister, U16 i);
S32 Grap_ListDrawIcon(tGRAPLIST *pLister, U16 i);

#define LISTSTR_MAXLEN 	100


#ifdef __cplusplus
}
#endif

#endif


