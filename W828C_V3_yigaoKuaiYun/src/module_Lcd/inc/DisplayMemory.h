/*************************************************************
%
% Filename     : DisplayMemory.h
% Project name : by use of displaymemory, part of the LCD driver library of Hyctron
%
% Copyright 2003-2004 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Author        Begin        Complete      Comments
%                    (yymmdd)     (yymmdd)
% -------------------------------------------------------------
% 001   HYCJB   	 060421                    Primary version
% -------------------------------------------------------------
%
%  此源文件包括了320*240的显示缓存程序
****************************************************************/

#ifndef _DISPLAYMEMORY_H_ 
#define _DISPLAYMEMORY_H_

#include "HSA_API.h"
#include "LcdAPIFunc.h"
#include "AlphaDisplay.h"


#ifdef __cplusplus
extern "C"
{
#endif

//NandFlash资源图片:
//每个资源图片的Offset、High、Width

#define QUESIZE				15	//队列最大长度

typedef struct area_t
{
	U16 kRow; 		//屏幕显示行
	U16 kColumn;	//屏幕显示列
	U16 kHigh;		//屏幕高度
	U16 kWidth;		//屏幕宽度
}AREA_t;

typedef struct tqueue
{
	int length;				//队列长度
	AREA_t data[QUESIZE]; 	//队列数据
}QUEUE_t;


typedef struct _RECT
{
	U16 y;      //行   定义矩形区域
	U16 x;      //列
	U16 h;      //高
	U16 w;      //宽
}RECT;

typedef struct _POS
{
	U16 x;      //列   定义坐标位置
	U16 y;      //行
}POS;


typedef struct _TEXTOUT
{
	U16 y;//行
	U16 x;//列
	U16 kDisplayFlag;//字体类型 | 大小，是否为文件名等
	U16 FrontColor;  //字体的前景色
	U16 BackColor;   //字体的背景色
	U16 keyStrColor;	//关键字符颜色
	U8  *pKeyStr;		//关键字符串
	U8  *pkString;    //显示字符指针
	U8  EraseBack;    //擦除背景色模式 0：不擦除 1：只擦使用透明色 2：使用BackColor颜色擦除
	                 //注意:擦除的区域大小为prect所指的区域
	RECT *prect; //如果EraseBack！=1则需要刷新rect区域的底色	
	
}TEXTOUT;

typedef enum
{
	MEMORYBACKGROUND=0,
	MEMORYDISPLAY,
	MEMORYBUFFER,
	ALPHABACKGROUND,
	ALPHADISPLAY,
	ALPHABUFFER
}MemoryType_e;	  


//////////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------
* 功能:	得到文件管理的区域,以及大小
*----------------------------------------------------------------------------*/
U32 Get_Display_DisplayMemory_Offset(void);

U32 Get_Display_Buffer_Size(void);

U32 Get_Display_DisplayBuffer_Offset(void);

U32 Get_Display_BackGround_Offset(void);

/*-----------------------------------------------------------------------------
* 函数:	GetSDRAMOffset
* 功能:	获得行和列在SDRAM中的地址
* 参数:	gMemory-----背景图存储区或显存
*		kRow--------屏幕显示行
*		kColumn-----屏幕显示列
* 返回:	offset------SDRAM地址
*----------------------------------------------------------------------------*/
U32 GetSDRAMOffset(U16 kRow, U16 kColumn);

void Memory_SetRowColumn(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_IsUseQueue
* 功能:	是否使用队列
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_IsUseQueue(int type);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearQueue
* 功能:	清除显示队列
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearQueue(void);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetMemoryOffset
* 功能:	得到memory的offset
* 参数: x,y memory的列行
* 返回:	none
*----------------------------------------------------------------------------*/
char *drvLcd_GetMemoryOffset(U16 x, U16 y, U32 type);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Background2DisplayMemory
* 功能:	从SDRAM背景图缓存中读取需要刷新位置的数据到SDRAM显存中对应位置
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Background2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearDisplayMemory
* 功能:	清楚显示缓冲区域
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearDisplayMemory(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2Memory
* 功能:	从NandFlash中读取界面上的图片写入指定的SDRAM显存
* 参数:	BMPOffsetInNand---界面图片在NandFlash资源图片中的偏移地址
*		kBMPHigh----------图片高度
*       kBMPWidth---------图片宽度
*       kRow--------------屏幕显示行
*		kColumn-----------屏幕显示列
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2Memory(char *pSrc, char *pDest,U16 kBMPHigh, U16 kBMPWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2DisplayMemory
* 功能:	从SDRAM其他缓存中读取数据到SDRAM显存中指定位置
* 参数:	pBuf------数据源地址
*		kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemory(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2DisplayMemoryClarity
* 功能:	从SDRAM其他缓存中读取数据到SDRAM显存中指定位置
* 参数:	pBuf------数据源地址
*		kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemoryClarity(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn, U16 startH, U16 startW, U16 filterColor);

/*-----------------------------------------------------------------------------
* 函数:	Set_DispMemory2Queue
* 功能:	把指定的显存插入队列
* 参数:	 h w y x :高\宽\行\列
* 返回:	none
*----------------------------------------------------------------------------*/
void Set_DispMemory2Queue(U16 h, U16 w, U16 y, U16 x);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DispString
* 功能:	显示字符串
* 参数:	pos----显示位置
*		kDisplayFlag--显示方式
*		*pkString-----待显示的字符串
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DispString(TEXTOUT *text);

/*-----------------------------------------------------------------------------
* 函数:	DisplayData2Screen
* 功能:	将显存数据送显示屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void DisplayData2Screen(void);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetDisplayMemoryArea
* 功能:	设置SDRAM显存中对应位置的信息
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDisplayMemoryArea(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetDisplayArea
* 功能:	设置显示地址
* 参数: DisplayOffset------显示
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetDisplayAddress(U32 DisplayOffset);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Rectangle
* 功能:	画矩形
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Rectangle(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth, U16 FrameWidth);

/*-----------------------------------------------------------------------------
* 函数:	BMPFromSDRAMDisplayMemory2Screen
* 功能:	SDRAM显存中所有数据（整屏）直接送入显示屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayMemory2Screen(void);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_BMPFromDisplayBuffer2Screen
* 功能:	SDRAM缓存中所有数据（整屏）直接送入显示屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayBuffer2Screen(void);

/*-----------------------------------------------------------------------------
* 函数:	WritePoint2SDRAM
* 功能:	写一个彩色的点到SDRAM（背景缓存、显存、BUffer都有可能）
* 参数:	u16Data-----16位彩色数据
* 返回:	none
*----------------------------------------------------------------------------*/
void WritePoint2SDRAM(U16 u16Data);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_AddDispRect
* 功能:	把显示区域加入队列，以便能够进行送屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void  drvLcd_AddDispRect(U16 y, U16 x, U16 h, U16 w);
#ifdef __cplusplus
}
#endif

#endif 	//_DISPLAYMEMORY_H_

