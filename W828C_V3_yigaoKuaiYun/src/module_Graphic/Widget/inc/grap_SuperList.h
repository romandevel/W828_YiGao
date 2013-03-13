/**************************************************************************
 1:与之前的list控件有所不同,该控件自带滚动条,滚动块的位置是根据当前
  屏中第一项的位置调整的(PC上的滚动条基本上都是如此),之前的滚动条是
  根据焦点项的位置调整的.
 2:该控件允许焦点项不在当前屏内.
 3:滑动块最小为10个象素高
 
 4:'+'代表复选框  复选框区域内不能拖拽
                ----------------------
               | + 全部      0011/0022|  <-----------标题行
               |----------------------|
               | +  显示字符       |  |
               |-------------------|滚|
               | +  显示字符       |  |
 list条目区--> |-------------------|动|  <-----------滚动条
               | +  显示字符       |  |
               |-------------------|条|
               | +  显示字符       |  |
               |-------------------|区|
               | +  显示字符       |  |
               |-------------------|--|


***************************************************************************/

#ifndef __GRAP_SUPPERLIST_H__
#define __GRAP_SUPPERLIST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"


#define 	NEED_TITLE			0x01	//需要标题行
#define		CHECK_BOX			0x02	//复选框
#define		NEED_TIMER			0x04	//字符过长时需要timer移动
#define		SELECT_ALL			0x08	//选择了全部
#define		NEED_SCRBAR			0x10	//要画滚动条
#define		CONFIRMANDENTER		0x20	//第一次点击就进入


#define		FIRSTPOINT_INTITLEICON		1//第一次点击点在标题行的小图标上
#define		FIRSTPOINT_INSCRBAR			2//第一次点击点在滚动条
#define		FIRSTPOINT_INLIST			4//第一次点击点在list区
#define		FIRSTPOINT_INLISTICON		8//第一次点击点在list区的小图标上
#define		HAS_PULLED					128//产生拖拽


typedef S32 (*SupList_int)(void *pView);
typedef S32 (*SupList_update)(void *pView);
typedef S32 (*SupList_timer)(void *pView, U16 itemNo);
typedef S32 (*SupList_lineup)(void *pView,U32 flag);
typedef S32 (*SupList_linedown)(void *pView,U32 flag);
typedef S32 (*SupList_pageup)(void *pView);
typedef S32 (*SupList_pagedown)(void *pView);

typedef S32 (*SupList_getstr)(void *pView, U16 itemNo, U8 *str, U16 maxLen);
typedef S32 (*SupList_drawIcon)(void *pView, U16 itemNo);
typedef S32 (*SupList_drawone)(void *pView, U16 itemNo);
typedef S32 (*SupList_drawsrc)(void *pView);
typedef S32 (*SupList_drawtitle)(void *pView);
typedef S32 (*SupList_enter)(void *pView, U16 itemNo);
typedef S32 (*SupList_delete)(void *pView);
typedef S32 (*SupList_delopen)(void *pView, U16 itemNo);
typedef S32 (*SupList_updatebytotal)(void *pView);
typedef S32 (*SupListKeyInput)(void *pSupLister, U16 keycode);//按键输入,主要用作快捷键进入(1,2,3,4.....)

typedef struct tItemInfo
{
	U16 isSelect;  //该项是否被选中
	U16 curItemNo; //当前项

	struct tItemInfo  *pNext;
	
}tItemInfo;

typedef struct tSupperListPriv
{
	U8  ListInfo;		//bit0:是否需要标题行; 1:需要 bit1:是复选框还是单选;1:复选 
						//bit2:是否需要timer;1:要  
						//bit3:是否选择了全部;1:是 (初值一般为0) bit4:此时是否需要显示右侧滚动条 1:需要 (初始化时程序会计算)
						//bit5:是否第一次点击就进入 1:是
	//需要标题行时才有用
	U8  titleH;			//标题行的高--如果不需要标题行,参数可以填0
	U16 titleW;			//宽---和整个控件的宽一样			

	//
	U16 itemH;			//每一项的高
	U16 itemScreenNum;	//一屏中能显示的项的个数 (不包括标题行)
	U16 fontNoFocusColor;//非焦点时字符颜色
	U16 fontFocusColor;	//焦点时字符颜色
	U16 topItemNo;		//当前一屏第一项的编号
	U16 focusItemNo;	//焦点项编号
	U16 totalItem;		//总项数 (不包括标题行)
	U16 longCoverPicId;	//长反显条的ID,--不需要显示出右侧的滚动条时用
	U16 shortCoverPicId;//短反显条的ID,--需要显示出右侧的滚动条时用
	U16 backPicId;		//该控件没有指定图片时需要用背景刷新,背景图片的ID
	
	U16 iconX;			//每项小图标的显示位置---相对于该项viewX1的偏移
	U16 iconY;			//每项小图标的显示位置---相对于该项viewY1的偏移
	U16 iconW;
	U16 iconH;
	U16 icon_NoSelectId;  //未选中时的图标
	U16 icon_SelectId;    //选中时的图标
	
	U16 strX;
	U16 strY;			//字符开始显示的位置 和小图标一样,是相对于该项起始点的位置
	U16 screenStrLen;	//一屏中能显示的字节数(要显示的字符超过该数时滚动显示)
	U16 firstCharPos;	//显示在LIST开头的字符在整个字符串中的位置,初值为0
	
	U16 TimerLft;		
	U16 TimerTot;		//需要timer时才有效
	U8  font_size;      //字体的大小
	U8  font_type;      //字体类型 unicode，GB等
	U8  position;      //显示位置  居中靠左靠右三种
	
	//下面3个参数要正确填写,即使不需要滚动条也要填写 
	U16 scrBarHeadId;	//滚动条上方图片 含上箭头的图片
	U16 scrBarTailId;	//下方			 含下箭头的图片
	U16 scrBarId;		//中间部分
	
}tSupListPriv;


typedef struct tListScrBar
{
	U16 viewX1;
	U16 viewY1;
	U16 viewX2;
	U16 viewY2;			//整个滚动条区域
	
	U16 scrLen;			//滚动区域长度
	U16 scrStartY;		//滚动区起点
	U16 scrEndY;		//滚动区终点
	
	U16 barStart;		//滑块上次起点
	U16 barEnd;			//滑块上次终点
	
	U16 arrow1_Y1;		
	U16 arrow1_Y2;
	U16 arrow2_Y1;
	U16 arrow2_Y2;		//箭头信息
	
	U16 barH;//滑动块的高
	float	ratio;//比值
		
}tListScr;

typedef struct tSupperList
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
	
	tSupListPriv	SupList_priv;
	tListScr		ListScr;
	tItemInfo		*pItemInfoH;//head
	tItemInfo		*pItemInfoT;//tail
	
	RecvSysMsg		recvSysMsg;
	RecvPullMsg		recvPullMsg;
	
	SupList_int			SupListInt;
	SupList_update		SupListUpDate;
	SupList_timer		SupListTimer;
	SupList_lineup		SupListLineUp;
	SupList_linedown	SupListLineDown;
	SupList_pageup		SupListPageUp;
	SupList_pagedown	SupListPageDown;
	SupList_getstr		SupListGetStr;
	SupList_drawIcon	SupListDrawIcon;
	SupList_drawone		SupListDrawOne; 
	SupList_drawsrc		SupListDrawSrc;
	SupList_drawtitle	SupListDrawTitle;
	SupList_enter		SupListEnter;
	SupList_delete		SupListDelete;
	SupList_delopen		SupListDelOpen;
	SupList_updatebytotal	SupListUpDateByTotal;
	SupListKeyInput		SupkeyInput;
	
	U8 timerEn;
	U8 lastKey;	//上次按键值  (bit7记录之前的timer是否enable)
	U8 pullFlag;//bit7为1表示拖动过
	
}tSUPPERLIST;

tSUPPERLIST *Grap_InsertSupList(void *pView, tGrapViewAttr *pview_attr, tSupListPriv *plst_privattr);


extern U16 gFrontColor;
extern const U8 Suspension_points[];
extern int Grap_GetDispStr(Character_Type_e kStr_Type,U16 factwid,U8 *tmpStr);
extern const U8 *const titalStr[];

#ifdef __cplusplus
}
#endif

#endif


