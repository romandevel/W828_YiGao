#ifndef _SEL_WORD_H_
#define _SEL_WORD_H_


//面板的开始行列
#define  SEL_PANEL_WR_ROW  14
#define  SEL_PANEL_WR_COL  7
#define  SEL_PANEL_IM_ROW  24
#define  SEL_PANEL_IM_COL  7

#define  SEL_PANEL_HIG  176
#define  SEL_PANEL_WID  228


/*---------------------------------------------------------
*函数: sel_if_enable_selword
*功能: 关闭或打开备选字
*参数: none
*返回: none
*--------------------------------------------------------*/
int sel_if_enable_selword(tGRAPDESKBOX *pDesk);

/*---------------------------------------------------------
*函数: sel_have_selword
*功能: 是否具有备选字
*参数: none
*返回: -1
*--------------------------------------------------------*/
int sel_have_selword(tGRAPDESKBOX *pDesk);

#endif