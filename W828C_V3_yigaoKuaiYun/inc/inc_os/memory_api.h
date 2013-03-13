#ifndef MEMORY_API_H
#define MEMORY_API_H

#include "hyTypes.h"
#include "memory_buddy.h"


/* 内存操作的方式, 目前仅支持buddy方式 */
#define MEMO_METHOD_BUDDY	1
#define MEMO_METHOD_HEAP	2

/* 封装用的结构体 */
typedef struct sys_memo_st
{
	U16	type;					// 内存操作使用的是哪套函数。目前只加了buddy的方式。
	buddy_memo buddymemo;		// buddy方式操作时需要的结构体
	
	//other memory operate struct
	
}	sys_memo_st, *sys_memo_st_ptr;



/*----------------------------------------------------------------------
* 功能：内存初始化函数（sram和sdram中内存的初始化）		
* 参数：type ---   内存操作的类型（目前只有buddy方式一种）
*		pram ---   内存块的起始地址
*		ramlen --- 创建该块的内存长度
* 返回：
*		NULL --- 初始化失败	
*		结构体指针  --- 成功
*-----------------------------------------------------------------------*/
void *memo_init( int type, U8 *pram, U32 ramlen );

/* 对应的free函数。第一个参数为memo_init时的返回值 */
void memo_free( void *pmemo, void *pbuf );

/* 对应到的malloc函数。第一个参数为memo_init时的返回值 */
void *memo_alloc( void *pmemo, U32 size );

/* 对应到的realloc函数。第一个参数为memo_init时的返回值 */
void *memo_realloc( void *pmemo, void *memblock, U32 size_new );

/* 得到剩余内存大小值，参数为memo_init时的返回值 */
U32 memo_rest( void *pmemo );

/* 销毁整段内存 */
void memo_destroy( U8 *pram );



#endif	//MEMORY_API_H

