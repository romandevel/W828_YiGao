/*                 base64.h           */
#ifndef _MBASE64_H_
#define _MBASE64_H_

#include "hyTypes.h"

/*
	data:待编码的字串
	data_len:字串长度
	encode:编码后的字串
	encodelen:in:encode buf的长度  out:实际编码后的长度   失败时返回编码所需长度
	
	返回:0:成功  -1:失败
*/
int mbase64_encode(const char *data, int data_len, char *encode, int *encodelen);

/*
	data:待解码的字串
	data_len:字串长度
	decode:解码后的字串
	decodelen:in:decode buf的长度  out:实际解码后的长度   失败时返回解码所需长度
	
	返回:0:成功  -1:失败
*/
int mbase64_decode(const char *data, int data_len, char *decode, int *decodelen);


#endif /* _MBASE64_H_ */