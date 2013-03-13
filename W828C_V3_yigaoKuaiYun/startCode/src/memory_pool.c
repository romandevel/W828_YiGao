/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/**
 * @file memory_pool.c
 * @brief Reusable memory pool.
 *
 * 
 * Assumptions:
 * - None
 *
 * Functional limitations:                     
 * - None
 */
/**
 * \addtogroup tmAppDemolication
 */
/*@{*/

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"


/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/

/*	Shark Memory 分配
第一部分	shark 内部的128k SRAM 的分配使用表
=================================================================================	
	
	1。ISRAM (0x20000 = 128K) 的使用 
  0x0	 0x2C   0x42C     0xC2C      0x2000           0x18800        0x1D000                      0x20000 
 	+------+------+---------+----------+------------------+---------------+----------+--------------+
	|0x2C  | 1024 |2048     |<=0x13D4  | 0x18000 = 96K    |<0x3000字节    |      < 0x3000字节       |
	|中断  |读文件|nandflash|常驻内存  | overlay momory   |               | heap ->         <-stack |
	|向量  |buffer|读cache  |所有函数  | 用法见下面的表   |全局变量ZW，ZI | 向上增长 	  	向下增长|						                                    		
	+------+------+---------+----------+----------------------------------+----------+--------------+
	注：1. 在nand读函数中，实际上也有调用在写函数区的函数，但是由于实际上并不进入，所以，write函数那时
		没有初始化到运行区也没有问题。
		2. 常驻内存函数中，nandflash logic read 的函数可以放在sdram中，这是由于usb状态下实际上
		是调用了另一组 nandflash 的logic read 的函数 
==================================================================================	
	
	2。 overlay memory 的使用 ，memory 从内存某个offset 开始，overlay 主要是一些可以overlay
		的函数和buffer的overlay
		由于系统的运行情况，主要overlay如下图表示，
	
	|<-1. playback audio 或 playback video->|																													|
 0x2000	     0x4000                                        0x1A000	
	+----------+------------------------------------------------+
	|0x2000    |90* 1024 足够了，所以不需要使用overlay函数的区域|
	|可overlay |playback audio 需要的各种解码器的               |
	|函数      |instance，input output                          |	 								                                    		
	+----------+------------------------------------------------+
	+-----------------------------------------------------------+
	| 由于play video时需要内存多，所以需要使用overlay函数的区域 | 
	| playback video  需要的各种解码器的 instance，input output |
	| 	                    	                                |					                                   		
	+-----------------------------------------------------------+
		1.1 为了加快速度，将mp3解码库放在RAM中运行,mp3需要的instance和输入输出buffer为0x9d80
			所以，放代码的空间为 (0x4000 + 0x9D80 = 0xDD80), 设置scatter 数据
			mp3 运行空间（code + RO data ) 需要最多0x8320的RAM 即33568 Byte

	|<-2. record 或  usb disk 或 format 或 nand init ->|																																					|
 0x2000	 	0x4000    0x4800    0x8800    
	+----------+---------+---------+----------------------------------------------------+
	|0x2000    |2048     |  16k    |1. record 需要的各种编码器的instance，input output  |
	|可overlay |nandflash|nandflash|2. usb 的功能函数和buffer及usb的所有zw 和zi 的变量  |
	|函数      |写cache  |映射表   |3. format   										|	 								                                    		
	|		   |         |         |4. nand 的初始化函数overlay	仅包括hardware层即可	|
	|		   |         |         |   仅在系统启动时被调用一次	                        |
	+----------+-------------------+----------------------------------------------------+

	注：1。 nandflash 写cache和mapping table 要求连在一起
			nandflash的映射表，在仅仅是对nandflash进行读操作的时候，可以不需要
			写操作时必须，如果是支持1G的flash，则需要16k，2G 需要32k 其余类推
		2.	在写操作时，从memory开始，要预留21k + 16k = 0x9400  内存用途如上所示
			其中16k的空间为写操作需要的函数，如写，擦除等
		3.	各种编/解码器需要的instance 为
				WMA_FULL或WMA_EVALUATION 	 	(0x9d80)
				WMA_PDDRM						(0x9db4)
				MP3ENCODING(PHILIPS LIB)		(0xA070)
			现在可提供的远大于这些要求的数据
		4.
================================================================================


第二部分	shark 外部的 8M SDRAM 的分配使用表， 根据硬件不同，可能有
=================================================================================	
	0x20000000的地址是cachable，bufferable的
	0x28000000的地址是noncachable，nonbufferable的
	+----------------------+------+------------------+
	|0x20000000~0x200FFFFF | 1M   | 保留前1M给程序   |  **
	+------------------------------------------------+
	|0x28100000~0x2813FFFF | 150K | 背景图 	         |  **
	+------------------------------------------------+
	|0x28140000~0x2817FFFF | 150K | 显存 	         |
	+------------------------------------------------+
	|0x28180000~0x28189FFF | 40K  | SDRAM数据缓存    |
	+------------------------------------------------+
	|0x2818A000~0x2818A000 | 3K   | 数据缓存         |
	+------------------------------------------------+
	|0x20200000~0x20200000 |      |                  |
	+------------------------------------------------+
		
*/



/*	以下是几个关键的memory 的size ，注意一定要和scatter文件中的一致
	OMEM	表示 overlay memory */
#define OMEM_SIZE            		(10*1024)//(20*1024)//(92*1024)//   /*0x16800 = 90k 0x18000= 96k */


/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/

/*	0x2C ~ 0xC2B , 共3K， 非overlay区域*/
#pragma arm section zidata = "read_pool"
    __align(4) char gkReadPool[ 0x800 ];//0x800
#pragma arm section

/*	0xC2C ~ 0x1FFF ， 常驻内存的函数， 非overlay区域，由scatter定义 */

/*	0x2000 ~ ...   overlay区域 */
#pragma arm section zidata = "reusable_memory"
    __align(4) char gtmMemoryPool[ OMEM_SIZE ];
#pragma arm section


/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/

#if 0
/*----------------------------------------------------------------------------*/
void * hyswNandFlash_GetReadPool_NoUse(void)
{
    //return ((void * ) ( &gkReadPool[0x400]));
    return ((void * ) ( &gkReadPool[0]));
}
/*----------------------------------------------------------------------------*/
void * hyswNandFlash_GetWritePool_NoUse(void)
{
    return ((void * ) (&gtmMemoryPool[0]));
}
/*----------------------------------------------------------------------------*/
#endif
