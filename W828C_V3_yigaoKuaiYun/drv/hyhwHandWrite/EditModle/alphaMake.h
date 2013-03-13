#ifndef __ALPHA_MAKE_H__
#define __ALPHA_MAKE_H__

#include "DisplayMemory.h"

/*-----------------------------------------------------------------------------
* 函数:	edit_make_alpha_value
* 功能:	设置alpha值
* 参数:	type 类型
* 返回:	base offset
*----------------------------------------------------------------------------*/
void edit_make_alpha_value(void);

/*-----------------------------------------------------------------------------
* 函数:	edit_bmp_mix_buf
* 功能:	混合两种buf输出
* 参数:	算出当前行的alpha，并混合buf输出
* 返回:	none
*----------------------------------------------------------------------------*/
void edit_bmp_mix_buf(AREA_t *pArea,char *upTempMem, char *dispBuf,tCTR_PANEL *pCtrPan);

#endif