
#ifndef __HY_MEM_MANAGE_API_H__
#define __HY_MEM_MANAGE_API_H__


////////////////////////////	INCLUDE		//////////////////////////////

//#include "../basic_type.h"
#include "hyTypes.h"


#ifdef __cplusplus
extern "C"{
#endif


////////////////////////////	MACRO	//////////////////////////////////

#define GETBUFFER		hy_malloc
#define REGETBUFFER		hy_realloc

#define FREEBUFFER( Ptr ,idx)	\
{								\
	if	( (Ptr) != NULL )		\
	{							\
		hy_free( (Ptr) ,idx);	\
		(Ptr) = NULL;			\
	}							\
}


////////////////////////////	PROTOTYPE	//////////////////////////////

//------------------------------------------------------------------------
//	下列函数可以在hy_mem_create, hy_mem_load_blocks, hy_mem_add_block
//	之后, hy_mem_release 之前进行任意的调用.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// name:	hy_malloc - public	
// desc:	内存分配函数 同 pc 上的 malloc
// params:	
// return:	HBOOL, TRUE to indicate success
//------------------------------------------------------------------------
void* hy_malloc( U32 size, U8 idx);


//------------------------------------------------------------------------
//	prototype:	hy_realloc( void *memblock, U32 size_new )
//	function:	same as realloc on PC
//				开辟一段新的内存区，把老内存区的东西拷贝进去。
//	parameter:	void *memblock	老内存区的地址
//				U32 size_new	新内存区的尺寸

//	return:		如果开辟成功拷贝成功，返回新开辟内存段地址
//				如果传入内存段首地址为NULL，则为其开辟一块新的内存
//				如果新开辟内存失败，则返回NULL
//------------------------------------------------------------------------
void* hy_realloc( void *p_memblock, U32 size_new, U8 idx);


//------------------------------------------------------------------------
//	prototype:	void hy_free( void* memblock )
//	function:	same as free() on PC
//	parameter:	void *p_memblock:	address of the memblock you wanna release
//	copyright:	Hyctron
//------------------------------------------------------------------------
void hy_free( void *p_memblock, U8 idx);


//------------------------------------------------------------------------
//	debug func
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// name:	hy_mem_inspect - public
// desc:	错误监测, 夹逼用, 可以在任意时刻调用
//			调用后, 可知道内存表是否被破坏. 
// params:	NULL
// return:	返回 TURE 则表示仍然完好! 如果被破坏则返回False, 或者干脆死在里面.
//------------------------------------------------------------------------
BOOL hy_mem_inspect(U8 idx);


//------------------------------------------------------------------------
// name:	buddy_rest - public
// desc:	return the quantity ( size in bite ) of the usable memory
//			剩余内存查看. 可以在任意时刻调用
// params:	NULL
// return:	size of the remain memory
//------------------------------------------------------------------------
S32 hy_mem_rest(U8 idx);




#ifdef __cplusplus
}
#endif


#endif	//	__HY_MEM_MANAGE_API_H__