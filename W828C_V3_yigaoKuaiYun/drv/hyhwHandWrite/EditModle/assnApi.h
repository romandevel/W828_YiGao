#ifndef __ASSN_API_H__
#define __ASSN_API_H__

#include "edit.h"

/*---------------------------------------------------------------------------
* 函数: EI_getAssociateWord
* 功能: 读取联想字库
* 参数: pAssn-------读取次数，从1开始
* 返回：正常 HY_OK  其他 HY_ERROR
* -----------------------------------------------------------------------------*/
int EI_getAssociateWord(tASSN_CTR *pAssn);

#endif