/*                 md5.h           */
#ifndef _MMD5_H_
#define _MMD5_H_

#include "hyTypes.h"

/*
pData:待加密数据
Len:待加密数据长度
pCode:加密后的数据
*/
void mMD5_Encode(U8 *pData,U32 Len,U32 *pCode);

#endif /* _MMD5_H_ */