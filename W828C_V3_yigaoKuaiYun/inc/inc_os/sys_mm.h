#ifndef SYS_MM_H
#define SYS_MM_H

#define MEM_TYPE_SRAM 0
#define MEM_TYPE_SDRAM 1

#define EMPTY
#define power_max	27
#define word_size	0x10

typedef struct bm_node_tag *bm_node_ptr;

typedef struct bm_node_tag	{
	S32			kval;		//	2^kval
	bm_node_ptr	mstart;		//	starting address
	bm_node_ptr	l_link;		//	prior
	bm_node_ptr	r_link;		//	next
}	bm_node, *bm_node_ptr;

typedef struct bm_head_node_tag	{
	U32			size;		//	该链表空闲块大小
	bm_node_ptr	first;
}	bm_head_node, *bm_head_node_ptr;

void *Sys_MemAlloc(U32 Type,U32 Len);

#endif