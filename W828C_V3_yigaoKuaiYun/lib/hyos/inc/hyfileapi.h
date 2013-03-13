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
/******************************************************************************
*  文件:	hyfileapi.h
*  作者:	ZhouJie 
*  创建:	2007 3 30 15:23
*  说明:	文件系统代码，和操作系统无关的部分
*****************************************************************************/

#ifndef _HY_FILE_API_H_
#define _HY_FILE_API_H_


/* 应用层需要知道的磁盘分区的信息*/
typedef struct _DISK_PARTITION_INFO 
{
	unsigned int		start_sector;	/* offset 0		该分区起始的扇区号  */
	unsigned int		size;			/* offset 4		分区的大小，以扇区数计算 */
} DISK_PARTITION_INFO, *pDISK_PARTITION_INFO;


/*-------------------------------------------------------------------------------------------
* 函数:	hyfile_format					
* 功能:	建立文件系统，即format存储介质, pfd仅是用来传递必要的参数，并不是对应某个文件
*		该函数仅在格式化磁盘时被dosfile调用
*		临时借用buffer为fatPool, fatPool现在还没有用，所以可以在这个函数中使用
* 参数:	volmue		--	 为"C","D","E","F", 直接操作介质
*		secPerClus	--	 用来控制cluster的大小，sector总是为512字节
*		psectorBuf	--	 用来作为sector的buffer的一块内存，由调用者给出，4字节对齐	 		
* 返回:	Error code
*--------------------------------------------------------------------------------------------*/
int  hyfile_format( char * volume	, int secPerClus, char *psectorBuf);

    


#endif /* ABSTRACT_FILE_SYSTEM_H */
 
