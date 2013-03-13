#ifndef _MDES_H
#define _MDES_H

#include "hyTypes.h"

/*
3des加解密
该接口最后填充数据方式为:
k = 8-(srclen%8);
memset(&src[srclen],k,k);

src:源数据
srclen:源数据长度
dst:接收编解码后的数据buf
key:密钥
type:类型  0:加密   1:解密
*/
int Des3(char *src, int srclen, char *dst, char *key, int type);

#endif