
#ifndef _LCDDOTARRAY_H_ 
#define _LCDDOTARRAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "LcdAPIFunc.h"
 

typedef struct _StrAttribute
{
	U32 LanguOffset; //各种大小字符在字库中的offset
	U16 FontSize;        //字体属性（大小 8*16 ，24*24 32*32）
	U16 BytesOccupy;    //字体在字库中占据的大小
	U8  *pStr;          //字符编码指针
	U8  *pBuf;          //字模指针
	Character_Type_e  	Type; //当前字符在字库中的类型(Unicode ,Local GBK , 韩语 ，日语)
	
}StrAttribute,*PStrAttribute;

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetSBCCaseCharBMP
* 功能:	得到输入编码的文字字模
* 参数:	kStr_Type------字符类型
*			CHARACTER_UNICODE---unicode字符串
*			CHARACTER_LOCALCODE-本地码字符串
*		kUnicode_High--unicode高8位（本地编码时为区码）
*		kUnicode_Low---unicode低8位（本地编码时为位码）
*		*buff----------字模缓存
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetSBCCaseCharBMP(Character_Type_e kStr_Type,U8 kUnicode_High,U8 kUnicode_Low,U8 *buff,U16 bytes);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetSBCCaseCharBMP
* 功能:	得到输入编码的文字字模
* 参数:	kStr_Type------字符类型
*			CHARACTER_UNICODE---unicode字符串
*			CHARACTER_LOCALCODE-本地码字符串
*		kUnicode_High--unicode高8位（本地编码时为区码）
*		kUnicode_Low---unicode低8位（本地编码时为位码）
*		*buff----------字模缓存
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDBCCharBMP(StrAttribute *AttriStr);

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetDBCCaseCharBMP_Special
* 功能:	获取特殊字符的字模（>0x80）
* 参数:	kChar----------字符（>0x80）
*		kLanguageType--语言种类
*		*buff----------字模缓存
* 返回:	none
*----------------------------------------------------------------------------*/
//void drvLcd_GetDBCCaseCharBMP_Special(U8 kChar, U16 kLanguageType,U8 *buff);


#ifdef __cplusplus
}
#endif

#endif // _LCDDOTARRAY_H_

/*============================ History List ================================
 1. Rev 0.1					 20040107             HYCZJ
========================================================================*/
