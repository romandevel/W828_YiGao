#ifndef __GRAP_EDIT2_H__
#define __GRAP_EDIT2_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

//箭头颜色   0x3186
#define ARROWCOLOR			(0x000)//0x3186 0x320
#define SCROLLBARHEIGHT		10//箭头区域高
#define SCROLLBARWIDTH		16//箭头区域宽

#define EDIT2_DATA_SIZE    12288//2048


typedef S32 (*getDefaultStrInfo)(void *pView);
typedef S32 (*Edit2GetFocus)    (void *pView); 
typedef U32 (*Edit2KeyInput)(void *pView, U32 *keycode);
typedef S32 (*Edit2Enter)(void *pView);
typedef S32 (*Edit2DrawOneLine)(void *pView, U32 line_idx, U16 x_start, U16 y_start);


typedef struct tEdit2Scroll
{
	U16 barScrollLen ;//滑块滑动的区域的长度
	U16 bar_start_y;//y坐标起始结束的坐标
	U16 bar_end_y;
	U16 bar_start_x;//x坐标起始结束的坐标
	U16 bar_end_x;
	U16 start_y_pos;//上一次滚动条画到的位置(用于垂直的)
	U16 end_y_pos;
	
	U16 arrow1_areaX1;//滚动条上方箭头区域
	U16 arrow1_areaY1;
	U16 arrow1_areaX2;
	U16 arrow1_areaY2;
	U16 arrow2_areaX1;//滚动条下方箭头区域
	U16 arrow2_areaY1;
	U16 arrow2_areaX2;
	U16 arrow2_areaY2;
	U16 need;
	U16 barBGColor ;//背景颜色

}tEdit2Scroll;

typedef struct tGrapEdit2Priv
{
	U8 	*digt;     		// size - 2k
	U16 color;			//字体颜色 
	U8  font_size;		//现在只有16×16，24×24，32×32三中字体
	U8  font_type;
	U8  position;       //字符串开始显示的位置
	U16 coverPicId;		//定义需要刷新的背景图片ID,如果需要单色填写则直接写颜色

	U8	cursor_en;		//光标是否可用
	U16 TimerTot;		//光标闪烁一次总间隔时间
	U32 containLen;		//可以输入的长度(总长度)

	U8  xoffset;
	U8  yoffset;		//相对本行左上角偏移
	U8  mallocFlag;     //自己开辟空间还是使用指定的空间 0:自己开辟 1:使用自定的
						/* 注意 : 使用指定的空间 要比可输入的长度大2个字节！！！！ */
	U8  input;    /* 输入法类型 0:关闭  1:123  2:abc  3:ABC  4:手写  5:拼音*/
	
}tGrapEdit2Priv;

typedef struct tCharPos
{
	U32 firstCharPos;//该行首字符的位置
	U16 LineNo;//该行是第几行 从0计
	struct tCharPos *pNext;
	
}tCHARPOS;

//该控件内部使用的控制信息,不需要外部传入
typedef struct tGrapEdit2Ctrl
{
	//line info
	U32 topLine_idx;   //当前屏的第一行在总行数中的位置
	U32 totLine;       //总行数
	//U32 *line_pos;     //存储每行首字符在buff中的位置的数组，大小为LINE_MAX*4
	U16 line_height;   //每行的行高
	U16 line_width;    //每行的行宽， 要小于viewWidth
	U16 line_screen;   //当前一屏能容纳的行数
	U16 char_line;     //一行能容纳的字符数

	//cursor info
	U32	cursor_pos;		//cursor所对应的在字符串中的位置
	U32 cursor_line;    //光标所在的行
	U8	showState;		//光标当前的闪烁状态。0: not show out; 1: show out
	U16 TimerLft;		//光标闪烁时还差多少时间
	
	tCHARPOS  *pLineInfoH;//头指针
	tCHARPOS  *pLineInfoT;//尾指针
	
}tGRAPEDIT2CTRL;


typedef struct tGrapEdit2
{
	// view start 
	tGrapViewAttr view_attr;   //显示区域应该根据行高(20)的倍数来定， 宽为16的倍数 take care

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end

	tGrapEdit2Priv edit_privattr;
	tGRAPEDIT2CTRL edit_ctrl;
	getDefaultStrInfo get_default_str;
	Edit2GetFocus     get_focus; //当焦点发生改变的时候，需要做输入法等显示操作的时候可以重载该函数
	RecvSysMsg recvSysMsg;
	
	RecvPullMsg recvPullMsg;//处理拖拽消息
	Edit2KeyInput	edit2KeyInput;
	tEdit2Scroll  edit2Scr;
	
	Edit2Enter		edit2Enter;
	Edit2DrawOneLine edit2DrawOneLine;
	
			
}tGRAPEDIT2;


tGRAPEDIT2 *Grap_InsertEdit2(void *pView, tGrapViewAttr *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor);
tGRAPEDIT2 *Grap_CreateEdit2(tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor);
S32 Grap_Edit2GetDefaultStr(void *pView);

extern const U16 Edit2ScrollBarColor[16];

extern const U8 UpArrow[16];//w=16 h=8
extern const U8 DownArrow[16];//w=16 h=8


#ifdef __cplusplus
}
#endif

#endif