#ifndef MEMO_BUDDY_H
#define MEMO_BUDDY_H

#include "hyTypes.h"

//////////	CONSTANT  //////////////////////////////////////
#define EMPTY
#define power_max		27
#define sz_min_page		0x10	//	size of page
#define sz_page_head	0x10	//	size of page head


/*========	buddy memory node  ========*/
typedef struct bm_node_tag *bm_node_ptr;

typedef struct bm_node_tag	
{
	S32			kval;		/*	2^kval */
	bm_node_ptr	mstart;		/*	starting address */
	bm_node_ptr	l_link;		/*	prior */
	bm_node_ptr	r_link;		/*	next */
} bm_node, *bm_node_ptr;

typedef struct bm_head_node_tag	
{
	U32			size;		/*	该链表空闲块大小 */
	bm_node_ptr	first;
} bm_head_node, *bm_head_node_ptr;

typedef struct buddy_memo_t 
{
	U8			inuse;		/* 标记该段内存是否正在被某个进程使用 */
	void		*plock;		/* 如果该段进程正在被某个进程使用，其他进来的进程放在这个队列 */
	bm_head_node nodehead[32];	/* buddy用到的内存管理的头结点数组 */
} buddy_memo;

/*----------------------------------------------------------------------
* 函数: buddy_init
* 说明: 初始化内存管理head
* 参数: pbuddymemo:	内存管理结构体的指针
*		pmem:	该段内存的起始地址
*		len:	该段内存的长度
* 返回: none
* 注意：在对内存分块时由从大到小分和从小到大分的区别，现在使用的是第二种
*-----------------------------------------------------------------------*/
void buddy_init( buddy_memo *pbuddymemo, void *pmem, U32 len );


/*----------------------------------------------------------------------
* 函数: buddy_alloc
* 说明: 从phead的内存块中分配出来一个大小为size的内存
* 参数: pbuddymemo:	内存管理结构体的指针
*		size:		申请的内存块的大小
* 返回: NULL or pointer
*-----------------------------------------------------------------------*/
void* buddy_alloc( buddy_memo *pbuddymemo, U32 size );


/*----------------------------------------------------------------------
* 函数: buddy_free
* 说明: 从phead的内存块中释放p_memblock指向的内存块
* 参数: pbuddymemo:	内存管理结构体的指针
*		p_memblock:	要释放的内存块
* 返回: none
*-----------------------------------------------------------------------*/
void buddy_free( buddy_memo *pbuddymemo, void* p_memblock );


/*----------------------------------------------------------------------
* 函数: buddy_realloc
* 说明: 得到phead的内存块中所剩余的总空间大小
* 参数: pbuddymemo:	内存管理结构体的指针
*		memblock:	老内存的地址
*		size_new:	新内存的大小
* 返回: 剩余总空间的大小
* 功能: 开辟一段新的内存区，把老内存区的东西拷贝进去
*-----------------------------------------------------------------------*/
void* buddy_realloc( buddy_memo *pbuddymemo, void *memblock, U32 size_new );


/*----------------------------------------------------------------------
* 函数: buddy_rest
* 说明: 得到phead的内存块中所剩余的总空间大小
* 参数: pbuddymemo:		内存管理结构体的指针
* 返回: 剩余总空间的大小
*-----------------------------------------------------------------------*/
U32	buddy_rest( buddy_memo *pbuddymemo );



#endif //MEMO_BUDDY_H


