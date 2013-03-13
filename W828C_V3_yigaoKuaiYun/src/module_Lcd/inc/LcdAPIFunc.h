
#ifndef _LCDAPIFUNC_H_
#define _LCDAPIFUNC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "glbVariable_base.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* external Variable ,constant or function
--------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* LcdAPIFunc.c
*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_WriteChar
* 功能:	显示一个半角字符
* 参数:	kPage----显示位置的页序号（彩屏为行序号）
*		kPage----显示位置的列序号
*		kDisplayFlag--显示方式
*		kChar----半角字符
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_WriteChar(U16 kPage,U16 kColumn,U8 kDisplayFlag,U8 kChar);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_WritePoint
* 功能:	写一个彩色的点
*		仅用于彩色屏
* 参数:	u16Data(u32Data)-----16(32)位彩色数据
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_WritePoint(U16 u16Data);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearLcdRAM
* 功能:	在打开LCD模块电源之前将LCD RAM清除(置为0xFFFF)
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearLcdRAM(void);

/*-----------------------------------------------------------------------------
* 函数:	DecodeUTF8String
* 功能:	将UTF8编码的pSourceStr字符串还原
* 参数:	kSrcStr_Type----输入字符类型
					CHARACTER_LOCALCODE=0,
					CHARACTER_UNICODE,
					CHARACTER_UTF8
*		*pSourceStr-----待转换的字符
*		*pUniLocalStr---解码后的字符（本地码或UNICODE）
* 返回:	字符串长度
*----------------------------------------------------------------------------*/
U32 DecodeUTF8String(Character_Type_e kSrcStr_Type,U8 *pSourceStr,U8 *pUniLocalStr, U32 maxlen);

/*-----------------------------------------------------------------------------
* 函数:	Get_StringLength
* 功能:	获得字符串的长度
* 参数:	kStr_Type----输入字符类型
*			CHARACTER_UNICODE---unicode字符串
*			CHARACTER_LOCALCODE-本地码字符串
*		*pString-----字符串
* 返回:	字符串长度
*----------------------------------------------------------------------------*/
int Get_StringLength(Character_Type_e kStr_Type,U8* pString);

/*-----------------------------------------------------------------------------
* 函数:	Get_StringLength
* 功能:	获得字符串的长度
* 参数:	kStringType_Src----字符串类型
*			CHARACTER_SRC_PROGRAM-----程序或同步歌词文件中字符
*			CHARACTER_SRC_FILENAME----从HSA提取的文件名
*			CHARACTER_SRC_MP3ID3------从HSA提取的MP3ID3信息
*			CHARACTER_SRC_WMAID3------从HSA提取的WMAID3信息
*			CHARACTER_SRC_LRCLOCAL----从LRC文件中得到的本地码字符
*			CHARACTER_SRC_LRCUNICODE--从LRC文件中得到的UNICODE字符
* 返回:
*		CHARACTER_UNICODE---unicode字符串
*		CHARACTER_LOCALCODE-本地码字符串
*----------------------------------------------------------------------------*/
Character_Type_e Get_CurStringType(U16 kStringType_Src);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayString
* 功能:	显示字符串
* 参数:	kPage---------显示位置的页序号（彩屏为行序号）
*		kPage---------显示位置的列序号
*		kDisplayFlag--显示方式
*		*pkString-----待显示的字符串
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayString(U16 tempRow, U16 tempColumn, U16 kFontType, const U8 *pkString, const U8 *pKeyStr, U16 keyColor);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetColor
* 功能:	设置彩屏的前景色和背景色，并保存为全局变量，
* 参数:	frontColor----前景色
*		BackColor-----背景色
* 返回:	none
*----------------------------------------------------------------------------*/
#ifdef cLCD_16BIT_COLOR
	void drvLcd_SetColor(U16 FrontColor,U16 BackColor);
#endif
#ifdef cLCD_18BIT_COLOR
	void drvLcd_SetColor(U32 FrontColor,U32 BackColor);
#endif



/*-----------------------------------------------------------------------------
* LcdLowFunc.c
*----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetRowColumn
* 功能:	设置彩色显示屏的显示起始地址，
*		地址为行序号和列序号row和Column,以及高度和宽度
* 参数:	kRow - 行序号	row
*		kColumn - 列序号  Column
*		U8 kHeight - 高度
*		U8 kWidth - 宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth);

/*-----------------------------------------------------------------------------
* 函数:	LcdShowCharBMP
* 功能:	在指定位置显示一个字符
* 参数:	kRow---------行序号	row
*		kColumn------列序号  Column
		kDisplayFlag
*		*pkbuf-------被显示字符的字模数据
* 返回:	none
*----------------------------------------------------------------------------*/
void LcdShowCharBMP(U16 kRow, U16 kColumn, U8 kDisplayFlag, U8 *pkbuf);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_WriteBMPColor
* 功能:	只针对彩色屏，画单色图片，用前景色和背景色扩充
*		（如果是单色屏，就用drvLcd_WriteBMP()）
* 参数:	*pkBMPPointer--单色图片数据
*		kBMPHigh-------图片高度
*		kBMPWidth------图片宽度
*		kRow-----------行序号
*		kColumn--------列序号
* 返回:	none
*----------------------------------------------------------------------------*/
//彩屏 使用横排的彩色bmp图，bit7...bit0, bit7在左， 扩展到彩图
void drvLcd_WriteBMPColor(const U8 *pkBMPPointer,U16 kBMPHigh,U16 kBMPWidth,U16 kRow,U16 kColumn);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearBox
* 功能:	清除指定区域，彩屏时，以背景清除
* 参数:	kPage----页序号（彩屏时为行序号）
*		kColumn--列序号
*		kHeight--高度（单色屏时为页高度，彩屏时为行高度）
*		kWidth---列宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearBox(U16 kPage, U16 kColumn, U16 kHeight,U16 kWidth);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_WritePoint
* 功能:	写一个彩色的点
*		仅用于彩色屏
* 参数:	u8Data1-----16位彩色数据的高8位
*		u8Data2-----16位彩色数据的低8位
* 返回:	none
*----------------------------------------------------------------------------*/
#ifdef cLCD_16BIT_COLOR
	void drvLcd_WritePoint(U16 u16Data);
#elif defined cLCD_18BIT_COLOR
	void drvLcd_WritePoint(U32 u32Data);
#endif


#ifdef __cplusplus
}
#endif

#endif // _LCDAPIFUNC_H_
/*----------------------------------------------------------
Version 0.1  2003-11-24		HYCZJ
Version 0.2  2004-01-06		HYCZJ
-----------------------------------------------------------*/

