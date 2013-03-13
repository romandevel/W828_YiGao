#ifndef _GBK2UNICODE_H_
#define _GBK2UNICODE_H_

#include "hyTypes.h"


#define   LITTLE_ENDIAN 0 //小端编码方式
#define   BIG_ENDIAN    1 //大端编码方式

/*-----------------------------------------------------------
* 函数:Hyc_Gbk2Unicode 
* 功能:把gbk字符串转换成utf16字符串 结尾带 两个字节的NULL
* 参数: pGbk gbk字符串 pUTF16：接受的unicode地址空间 type:0:小端 1：大端 （输出uicode方式,gbk一定是小端方式）
* 返回: unicode 字符串所占的长度（bytes）不算结尾符
*-----------------------------------------------------------*/
U32 Hyc_Gbk2Unicode(char *pGbk, char *pUTF16, U32 type);

/*-----------------------------------------------------------
* 函数:Hyc_Unicode2Gbk 
* 功能:把unicode字符串转换成gbk字符串 结尾带 1个字节的NULL
* 参数: pUTF16：unicode字符串 pGbk: 接受的gbk地址空间  type:0:小端 1：大端 （unicode输入方式，gbk输出为小端方式）
* 返回: gbk 字符串所占的长度（bytes）不算结尾符
*-----------------------------------------------------------*/
U32 Hyc_Unicode2Gbk( char *pUTF16, char *pGbk, U32 type);

#endif