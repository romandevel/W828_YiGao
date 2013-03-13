/*************************************************************
%
% Filename     : LcdDotArray.c
% Project name : part of the LCD driver library of Hyctron
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040106  HYCZJ		Primary version
% -------------------------------------------------------------
%
%  This source file contains the character dot array operation for LCD display
%
****************************************************************/
#include "HSA_API.h"

#include "glbVariable_base.h"

#include "LcdAPIFunc.h"
#include "LcdDotArray.h"
#include "AbstractFileSystem.h"


#define c1WORD_BYTES		32//一个全角字符字模在字库中所占用的字节数
#define c1WORD_OCCUPY_BYTES	32//一个全角字符字模的字节数

#define FONTTEMPLATE_FILENAME	"C:/TestFont/gbk_23.bin"

typedef enum _FontTemplate_en
{
	Template_In_Reserve = 0,	//字模在reserve区内
	Template_In_File			//字模在指定的文件内

} FontTemplate_en;

//extern volatile __align(128) unsigned char horAscii8x16[256][16];

/***********************************************************************************
 * Function: void hyc_UTF8toGB(char *string, int size)
 * Description: convert the 2-byte unicode string to 2-byte GB string
 *             注意，unicode 为2个字节???， gb2312为2个字节 ??? to confirm?
 * Parameter: *string --  string need to be convert, 注意一定要有结束符'\0'
 * return: none
 ************************************************************************************/
void hyc_UTF8toGB(char *string)
{
	return;
}

/*-----------------------------------------------------------------------------
* 函数:	Get_GBKCurLocalCharBMP_Offset
* 功能:	得到本地编码中文(简繁)字模在字模库中的offset
* 参数:	kQu---区码
*		kWei--位码
* 返回:	字模在字模库中的offset
*----------------------------------------------------------------------------*/
static U32 Get_GBKCurLocalCharBMP_Offset(U8 kQu,U8 kWei,FontTemplate_en * pGBK_FontTemplate)
{
	int offset;
/*
	// 使用GBK分類顺序排列的字库，
	if(kQu>=0xA1 && kQu<=0xF7 && kWei>=0xA1)
	{
		// GBK/1: GB2312非汉字符号 GBK/2: GB2312 汉字
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if(kQu >=0x81 && kQu<= 0xA0)
	{
		// GBK/3: 扩充汉字
		offset=((kQu-0x81)*191 +(kWei-0x40)+17954)*c1WORD_BYTES;
	}
	else if(kQu >= 0xAA && kQu <=0xFE && kWei<0xA1)
	{
		// GBK/4: 扩充汉字
		offset=((kQu-0xAA)*97 +(kWei-0x40)+9709)*c1WORD_BYTES;
	}
	else
	{
		//kQu = 0xA3;
		//kWei = 0xBF;
		//不可识别字符则显示"？"（全角）
		offset=0x1B40;
	}
	return offset;
*/
	*pGBK_FontTemplate = Template_In_Reserve;
	// 使用GBK分類顺序排列的字库，
	if(kQu>=0xA1 && kQu<=0xF7 && kWei>=0xA1)
	{
		// GBK/1: GB2312非汉字符号 GBK/2: GB2312 汉字
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if((kQu >=0x81 && kQu<= 0xA0) && (kWei>=0x40))
	{
		// GBK/3: 扩充汉字
		offset=((kQu-0x81)*191 +(kWei-0x40)+17954)*c1WORD_BYTES;
	}
	else if(kQu >= 0xAA && kQu <=0xFE && kWei<0xA1)
	{
		// GBK/4: 扩充汉字
		offset=((kQu-0xAA)*97 +(kWei-0x40)+9709)*c1WORD_BYTES;
	}
	else if(kQu >= 0xA8 && kQu <=0xA9 && kWei<0xA1)
	{	// GBK/5: 扩充非汉字
		offset=0x1B40;//不可识别字符则显示"？"（全角）
	}
	else if((kQu>=0xAA && kQu<=0xAF) && ( kWei>=0xA1))
	{	// 用户自定义区 (1)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xAA)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if((kQu>=0xF8 && kQu<=0xFE) && ( kWei>=0xA1))
	{	// 用户自定义区 (2)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xF8)*94 +(kWei-0xA1)+564)*c1WORD_BYTES;
	}
	else if((kQu>=0xA1 && kQu<=0xA7) && (kWei>=0x40 && kWei<0xA1))
	{	// GBK 用户自定义区 (3)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xA1)*97 + (kWei-0x40)+564+658) * c1WORD_BYTES;
	}
	else if((kQu >=0x81) &&(kWei>=0x20) && (kWei<0x40))
	{	//双字节单宽度用户5区 Code in 新英汉世纪版
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0x81)*32 + (kWei-0x20)+564+658+679) * c1WORD_BYTES;
	}
	else
	{
		//kQu = 0xA3;
		//kWei = 0xBF;
		//不可识别字符则显示"？"（全角）
		offset = 0x1B40;
	}
	return offset;
}

/*-----------------------------------------------------------------------------
* 函数:	Get_KOREANCurLocalCharBMP_Offset
* 功能:	得到本地编码韩文字模在字模库中的offset
* 参数:	kQu---区码
*		kWei--位码
* 返回:	字模在字模库中的offset
*----------------------------------------------------------------------------*/
static U32 Get_KOREANCurLocalCharBMP_Offset(U8 kQu,U8 kWei)
{
//	韩文：
//	从0X8141开始:
//	1）。韩文字部分、字符和数字：
//	从第0X81区开始到0xC8区：
//	每个区从0XXX41--0XXXFE
//	2）。汉字部分：
//	从第0XCA区到0XFD区，
//	每个区从0XXXA0--0XXXFE
	int offset;

	if(kQu>=0xA1 && kQu<=0xFD&& kQu!=0xC9 && kWei>=0xA1)
	{
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
//	else if(kQu >= 0xA1 && kQu <=0xC8 && kWei<0xA1)
//	{
//		offset=((kQu-0xA1)*97 +(kWei-0x40)+8742)*c1WORD_BYTES;
//	}
//	else if(kQu >=0x81 && kQu<= 0xA0)
//	{
//		offset=((kQu-0x81)*191 +(kWei-0x40)+11961)*c1WORD_BYTES;
//	}
	else
	{
		//kQu = 0xA3;
		//kUnicode_Low = 0xBF;
		//不可识别字符则显示"？"（全角）
		offset=0x1B40;
	}

	return offset;
}

/*-----------------------------------------------------------------------------
* 函数:	Get_JAPANCurLocalCharBMP_Offset
* 功能:	得到本地编码日文字模在字模库中的offset
* 参数:	kQu---区码
*		kWei--位码
* 返回:	字模在字模库中的offset
*----------------------------------------------------------------------------*/
static U32 Get_JAPANCurLocalCharBMP_Offset(U8 kQu,U8 kWei)
{
//	1）。每个区188个。
//	2）。每个区中间的0x007F没有用，注意是没有用，而不是为空，所以：
//		每个区在0X40-0X7e位的区中偏移量就是（位-0X40），而在0x80-0xfc位
//		的区中偏移量就是（位-0X40-1）。
//	3).0XA0区----0XBF区：没有用
//
//偏移量计算：
//	。0x8140-0x9FFC
//	1.如果，位码小于0X7F，则：
//		((区-0x81)*188+(位-0x40))
//	2.如果，位码大于0X7F，则：
//		((区-0x81)*188+(位-0x40-1))
//	。0xE040-0xEAFC
//	3.如果，位码小于0X7F,则：
//		(((区-0XE0)+(0XA0-0x81))*188+(位-0x40))
//	4.如果，位码大于0X7F，则：
//		(((区-0XE0)+(0XA0-0x81))*188+(位-0x40-1))
//	。后面的编码区在本字模库中没有字模。

//	从0X8140开始:
//	每个区从0X40---0XFC共188个,每个区的其中0xXX7F没有用，所以每个区才188个
//	1）。符号部分：
//		一个区0X8140----0X81FC
//		各种符号，如：算术符号、标点符号等等。
//	2).数字字母部分：
//		六个区0X8240----0X82FC、0X8340----0X83FC、0X8440----0X84FC、0X8540----0X85FC、0X8640----0X86FC、0X8740----0X87FC
//		数字、英文字母、日文字母等
//		其中：
//		0X85和0X86两个区为空（全为空格）。
//	3).汉字区：
//		从0X8840----0X82FC、0X8940----0X89FC、.....0XFB40----0XFBFC、0XFC40----0XFCFC、
//		其中：
//		。0X8840----0X88FC区：实际从0X889F开始；所以前面有95（0X9F-0X40）个空格
//		。0X9840----0X98FC区：从0X9873----0X989E为空；
//		。0XA0区----0XBF区：没有用，注意：是没有使用，而不是为空格。
//		所以，0X9F紧接着为0XE0区。
//		。0XEB区----0XEC区、0xEF区：没有用
//		。0XFC区：从0XFC4C----0XFCFC为空。

	int offset;

	//1
	if(kQu>=0x81 && kQu<=0x9F && kWei>=0x40 && kWei<=0x7F)//现在把7F加上,jemy,07-06-04
	{
		offset=((kQu-0x81)*188 +(kWei-0x40))*c1WORD_BYTES;
	}
	//2
	else if(kQu>=0x81 && kQu<=0x9F &&  kWei>0x7F)
	{
		offset=((kQu-0x81)*188 +(kWei-0x41))*c1WORD_BYTES;
	}
	//3
	else if(kQu >= 0xE0 && kQu <=0xEA && kWei>=0x40 && kWei<0x7F)
	{
		offset=(((kQu-0xe0)+(0xA0-0x80))*188 +(kWei-0x40))*c1WORD_BYTES;
	}
	//4
	else if(kQu >= 0xE0 && kQu <=0xEA && kWei>0x7F)
	{
		offset=(((kQu-0xe0)+(0xA0-0x80))*188 +(kWei-0x41))*c1WORD_BYTES;
	}
	//不可识别
	else
	{
		//kQu = 0x81;
		//kWei = 0x48;
		//不可识别字符则显示"？"（全角）
		offset=0x0100;
	}

	return offset;
}

/*-----------------------------------------------------------------------------
* 函数:	GetFontTemplateFromeFile
* 功能:	从指定的文件中获得特殊的字模
* 参数:	data----存放字模的buffer
*		offset--字模在文件中的偏移
*		size----字模数据的大小
* 返回:	0-------成功
*		非0-------失败
*----------------------------------------------------------------------------*/
static int GetFontTemplateFromeFile( char * data, int offset, int size )
{
	int fp,rc=-1;

	fp = AbstractFileSystem_Open((char*)FONTTEMPLATE_FILENAME,HSA_READ_FOM);
	if (fp>=0)
	{
		AbstractFileSystem_Seek( fp, offset, HSA_OFFSET_FROM_START_FSS );
		if (size == AbstractFileSystem_Read( fp, (char*)data, size ))
		{
			rc = 0;
		}
	}
	AbstractFileSystem_Close( fp);

	return rc;
}

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
void drvLcd_GetDBCCharBMP(StrAttribute *AttriStr)
{//全角字符

	U32 offset;
	U8 HighByte;
	U8 LowByte;
	FontTemplate_en GBK_FontTemplate = Template_In_Reserve;	
	
	HighByte = *AttriStr->pStr;
	LowByte  = *(AttriStr->pStr+1);

	if (AttriStr->Type==CHARACTER_UNICODE)
	{//unicode	
		
		offset = ((HighByte<<8) +  LowByte) * (AttriStr->BytesOccupy);			
	}
	else
	{//localcode本地码
		
		if((HighByte>=0xAA && HighByte<=0xAF) && ( LowByte>=0xA1))
		{	// 用户自定义区 (1)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xAA)*94 +(LowByte-0xA1))*AttriStr->BytesOccupy;
		}
		else if((HighByte>=0xF8 && HighByte<=0xFE) && ( LowByte>=0xA1))
		{	// 用户自定义区 (2)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xF8)*94 +(LowByte-0xA1)+564)*AttriStr->BytesOccupy;
		}
		else if((HighByte>=0xA1 && HighByte<=0xA7) && (LowByte>=0x40 && LowByte<0xA1))
		{	// GBK 用户自定义区 (3)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xA1)*97 + (LowByte-0x40)+564+658) * AttriStr->BytesOccupy;
		}
		else if((HighByte >=0x81) &&(LowByte>=0x20) && (LowByte<0x40))
		{	//双字节单宽度用户5区 Code in 新英汉世纪版
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0x81)*32 + (LowByte-0x20)+564+658+679) * AttriStr->BytesOccupy;
		}
		else if(HighByte >= 0x81 && HighByte <= 0xFE && LowByte>=0x40)
		{//在字库中查找字模
			offset = ((HighByte - 0x81)*0xC0 + LowByte - 0x40)*AttriStr->BytesOccupy;
		}
		else
		{//不在编码表中则显示？来代替字符显示
			offset = ((0xA3 - 0x81)*0xC0 + 0xBF - 0x40)*AttriStr->BytesOccupy;
		}		
	}	
	
	offset += AttriStr->LanguOffset;

	if (GBK_FontTemplate == Template_In_Reserve)
	{		
		HSA_AccessReservedArea((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy);
	}
	else
	{//从指定文件中获得字模
		if (GetFontTemplateFromeFile((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy) != 0)
		{//文件打开或读取失败，则读取全角问号"？"
			
			offset = ((0xA3 - 0x81)*0xC0 + 0xBF - 0x40)*AttriStr->BytesOccupy + AttriStr->LanguOffset;
			
			HSA_AccessReservedArea((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy);
		}
	}
}

