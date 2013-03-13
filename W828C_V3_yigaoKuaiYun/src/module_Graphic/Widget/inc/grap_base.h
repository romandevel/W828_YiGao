#ifndef __GRAP_BASE_H__
#define __GRAP_BASE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"



/*=========================================================================*/
/*----------------------------------------------
*函数: Grap_GetViewByID
*功能: 获取当前窗体指定ID的控件
*参数: view 控件指针
*返回: id: 要找的控件id
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByID(void *view, U16 id);

/*----------------------------------------------
*函数: Grap_GetCurFocus
*功能: 获取当前窗体焦点控件,如果有panel则继续查找panel中的控件
*参数: pView 控件指针 pPanelid:如果焦点控件在panel上,则返回panel的id   不需要该参数时传NULL
*返回: pFocus: 焦点项控件指针,如果为找到焦点项则返回NULL
*----------------------------------------------*/
tGRAPVIEW *Grap_GetCurFocus(void *pView, U16 *pPanelid);

/*----------------------------------------------
*函数: Grap_WriteBMPColor
*功能: 用指定的颜色画取模数据
*参数: x1:列
       y1:行
       width:取模数据宽
       height:取模数据高
       BmpColor:取模数据
       colorP:颜色
*返回: none
*----------------------------------------------*/
void Grap_WriteBMPColor(U16 x1, U16 y1, U16 width, U16 height, U8 *BmpColor, U16 colorP);
/*----------------------------------------------
*函数: Grap_BrushScreen
*功能: 用指定的颜色填充指定区域
*参数: x1:起始列
       y1:起始行
       x2:结束列
       y2:结束行
       colorP:颜色
*返回: none
*----------------------------------------------*/
void Grap_BrushScreen(U16 x1, U16 y1, U16 x2, U16 y2, U16 colorP);
/*----------------------------------------------
*函数: Grap_ClearScreen
*功能: 用当前背景图刷新指定区域
*参数: x1:起始列
       y1:起始行
       x2:结束列
       y2:结束行
*返回: none
*----------------------------------------------*/
void Grap_ClearScreen(U16 x1, U16 y1, U16 x2, U16 y2);
/*----------------------------------------------
*函数: Grap_ShowPicture
*功能: 在指定位置显示指定id号的图片
*参数: picId:图片枚举,要确保存在
	   posX:起始列
       posY:起始行
*返回: none
*----------------------------------------------*/
void Grap_ShowPicture(U16 picId, U16 posX, U16 posY);
/*----------------------------------------------
*函数: Grap_Show_Part_BackPicture
*功能: 显示指定图片的一部分
*参数: pView1:底图所在的控件(不一定是大的背景图)
	   pView2:当前要刷新的控件,区域要在pView1中
*返回: none
*----------------------------------------------*/
void Grap_Show_Part_BackPicture(void *pView1,void *pView2);
/*----------------------------------------------
*函数: Grap_WriteString
*功能: 在指定位置显示字符串
*参数: 
	   
*返回: none
*----------------------------------------------*/
void Grap_WriteString(U16 x1, U16 y1, U16 x2, U16 y2, tStrDispInfo *disp_info);
/*----------------------------------------------
*函数: Grap_WriteStringMulti
*功能: 在指定区域内显示多行字符,可以自动换行;
*参数: 
	   
*返回: none
*----------------------------------------------*/
void Grap_WriteStringMulti(U16 x1, U16 y1, U16 x2, U16 y2, tStrDispInfo *disp_info);
/*----------------------------------------------
*函数: Grap_ChangeFocusEnable
*功能: 切换焦点项
*参数: pView:控件指针
*返回: none
*----------------------------------------------*/
void Grap_ChangeFocusEnable(tGRAPVIEW *pView);
/*----------------------------------------------
*函数: Grap_SendMessage
*功能: 给指定控件发送消息(阻塞方式)   id为0表示给窗体发送
*参数: pView:控件指针
       pEvent:消息
*返回: none
*----------------------------------------------*/
int Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent);

#ifdef __cplusplus
}
#endif

#endif

