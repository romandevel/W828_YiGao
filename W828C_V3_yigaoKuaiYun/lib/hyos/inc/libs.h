/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

#ifndef _LIBSH
#define _LIBSH

/*	HEAP MANAGEMENT STRUCTURE
	每个分配的内存堆的头就是这个结构，头8个字节就是HEAP结构表明这块内存的特性
	后面的内存大小就是可以给用户使用的内存 
	
	对于回收过的内存，由kalloc 和kfree维护回收的内存链表 第一个释放过的内存块
	的地址保存在alldat (见kalloc.c）中的成员avail中
	
	对于分配的内存，如果系统要求进程退出时全部释放内存，则用来维护某个进程的
	分配的内存堆，由nalloc 和_free 维护	*/
typedef struct heap
{
	struct heap *volatile next;
	union
	{
		unsigned size;		/* number of bytes following the header */
		void *p;			/* assure HEAP is rounded to size of pointer */
	} u;
} HEAP;


/*	计算内存堆的顶的地址，用于相邻内存堆的地址的检查 */
#define	topofheap(pheap)	((unsigned)(pheap) + sizeof(HEAP) + pheap->u.size)


/*	DATA STRUCTURE FOR alloc AND free
	This structure exists only once and holds static data for alloc and free */
typedef struct
{
	HEAP avail;				/* dummy pointing to first free chunk of heap，指向被释放的内存块 */
	HEAP *volatile bottom;	/* lowest valid heap address for free */
	HEAP *volatile top;		/* current top of heap */
	void *volatile plock;	/* wait list for heap management */
	volatile int inuse;		/* heap management in use , heap 是否正在进行分配或释放*/
} ALLDAT;


/*	PROCESS LINK LIST
 *	This structure saves onexit returns and linked list pointers.
 *	If anything is added to this structure which is not a pointer,
 *	function istart will have to be changed.
 */
typedef struct lnklst
{
	void /*FIO*/*volatile pfio;				/* 0x0 buffer-flush link */
	HEAP	*volatile alloc;			/* 0x4 ptr to alloced heap chunks */
	void	(*volatile exalloc)();		/* 0x8 place to save onexit return in alloc */
	void	(*volatile exfinit)();		/* 0xc place to save onexit return in finit */
	void	(*volatile extsleep)();		/* 0x10 place to save onexit return in tsleep */
	void /*FIO*/		*volatile stdin;			/* 0x14 address of input control buffer */
	void /*FIO*/		*volatile stdout;			/* 0x18 address of output control buffer */
	void /*FIO*/		*volatile stderr;			/* 0x1c address of std. error control buffer */
} LNKLST;

#endif
