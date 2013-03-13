/*******************************************************
* ---------孔2009-12-22
*******************************************************/
#ifndef __GRAP_COMBOBOX_H__
#define __GRAP_COMBOBOX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"


typedef S32 (*Combox_int)(void *pView);
typedef S32 (*Combox_drawone)(void *pView, U16 i);
typedef S32 (*Combox_enter)(void *pView, U16 i);
typedef S32 (*Combox_timer)(void *pView, U16 i);
typedef S32 (*Combox_drawscr)(void *pView);
typedef S32 (*Combox_getstr )(void *pView,U16 i, U8 *pStr);
typedef S32 (*ComboxGetFocus)(void *pView);

typedef struct _tComboBoxScr
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

}tComboboxScroll;


typedef struct tComboboxPriv
{
	U16 total_num;	//总条目数
	U16 dis_num;	//一次最多可以显示出的条目数(该数*控件高 <= 屏高/2)
	U8  cur_index;	//当前焦点项索引
	U8  opencur_index;	//展开后焦点项
	U8  top_index;	//当前所显示出的条目中第一个条目在总条目中的索引
	U8  direction;	//展开时的方向(下 0,  上 1)
	
	U8	timerenable;//字符过多时是否滚动
	U8  closewidth;	//闭合时可以显示字符区域的宽度
	U8  closecharlen;//闭合时一行最多显示的字符数,超过时滚动显示
	U8  openwidth;	//展开时可以显示字符区的宽度
	U8  opencharlen;//展开时一行最多可以显示的字符数,超过该值滚动显示(注意右侧是否有滚动条)
	U16 firstcharpos;//该行首字符位置,初值为0
	U16 timerlft;		
	U16 timertot;
	
	U16 font_size;  //字体的大小
	U16 font_type;  //字体类型 unicode，GB等
	
	U16 shortbar;	//焦点项反显条,首项
	U16 longbar;	//焦点项反显条,展开项
	U16 bar_color;	//焦点项底色(无反显条图片时使用)
	U16 clrnormal;	//非焦点时颜色   字符颜色
	U16 clrfocus;	//焦点时颜色     字符颜色
	U16 normalbgcolor;	//非焦点是底图颜色
	U16 openpicid;	//展开后的图片   无图片时用normalbgcolor填充
	
	U16 start_x;	
	U16 start_y;	//相对该行左上角位置
	U16 charhigh;	//字符高
	
	U16 backpicid;	//定义需要刷新的背景图片ID
	
	U16 needscr;	//该控件可以事先知道是否需要右侧滚动条,请正确填写
	//下面3个参数要正确填写,needscr为0时可以不必填写 
	U16 scrBarHeadId;	//滚动条上方图片 含上箭头的图片
	U16 scrBarTailId;	//下方			 含下箭头的图片
	U16 scrBarId;		//中间部分
	
}tComboboxPriv;

typedef struct tGrapCombobox
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

	//combo box  private
	tComboboxPriv  comboPriv;
	tComboboxScroll comboScr;
	
	Combox_int		comboxinit;
	Combox_drawone	comboxdrawone;
	Combox_enter	comboxenter;
	Combox_timer	comboxtimer;
	Combox_drawscr	combocdrawscr;
	Combox_getstr   comboxgetstr;
	ComboxGetFocus  combox_getFocus;
	
	RecvSysMsg		recvSysMsg;
	RecvPullMsg		recvPullMsg;
	
	U8 list_opened; // 1:下拉列表已打开
	
	U8  pullflag; //记录按下点区域
	
	U16 openX1;
	U16 openX2;
	U16 openY1;
	U16 openY2;//展开后的触摸区域
	
	U16 tempwidth;//记录展开后的宽度,防止有滚动条时改变私有属性中的值

} tGRAPCOMBOBOX;

extern const U16 ComboboxScrollBarColor[24];

tGRAPCOMBOBOX *Grap_InsertCombobox(void *pView, tGrapViewAttr  *pview_attr, tComboboxPriv  *pcomboPriv);


#ifdef __cplusplus
}
#endif

#endif //__GRAP_COMBOBOX_H__ 
