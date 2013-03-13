/*******************************************************************************
*  (C) Copyright 2003 Philips Semiconductors, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole
*  property of Philips Corporation and is provided pursuant to a Software
*  License Agreement.  This code is the proprietary information of Philips
*  Corporation and is confidential in nature.  Its use and dissemination by
*  any party other than Philips Corporation is strictly limited by the
*  confidential information provisions of the Agreement referenced above.
*
*******************************************************************************/

/**
 * \defgroup tmAppDemoApplication tmAppDemoApplication
 * @brief Header file for the `reusable memory pool`
 *
 * @{
 */

/** 
 * @file : SdramMemory_pool.h
 * @brief Header file for the `reusable memory pool`
 *
 * Header file for the `reusable memory pool`
 * 
 */

#ifndef TM_APP_DEMO_REUSABLE_SDRAM_MEMORY_H_
#define TM_APP_DEMO_REUSABLE_SDRAM_MEMORY_H_


/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/
	//8M/16M SDRAM
	#define SDRAM_BLOCK_MAP_OFFSET				(0)
	#define SDRAM_BLOCK_MAP_SIZE				(2048)

	#define SDRAM_DISP_MEMORY_POOL_OFFSET		(2048)//(43*1024)
	#define SDRAM_DISP_MEMORY_POOL_SIZE			(921600)//(320*240*2*6)  //kong rgb

	#define SDRAM_PERSISTENT_PARAMETER_OFFSET	(SDRAM_DISP_MEMORY_POOL_OFFSET+SDRAM_DISP_MEMORY_POOL_SIZE)//(795*1024)
	#define SDRAM_PERSISTENT_PARAMETER_SIZE		(40960)//(40*1024)

	#define SW_NAND_READ_POOL_OFFSET			(SDRAM_PERSISTENT_PARAMETER_OFFSET + SDRAM_PERSISTENT_PARAMETER_SIZE)
	#define SW_NAND_READ_POOL_SIZE				(4*1024)//4k = 1个Nand物理pagesize
	#define SW_NAND_WRITE_POOL_OFFSET			(SW_NAND_READ_POOL_OFFSET + SW_NAND_READ_POOL_SIZE)
	//#define SW_NAND_WRITE_POOL_SIZE				(72*1024)//72k = 4+4+64k(4k nand写缓存，4k MLC nand写缓存，64k mapping table)
	//72k = 4+4+128k(4k nand写缓存，4k MLC nand写缓存，128k mapping table[可记录64k个block]，按512k/block计，支持32G)
	#define SW_NAND_WRITE_POOL_SIZE				(136*1024)
	
	#define USB_TRANSPORT_OFFSET				(SW_NAND_WRITE_POOL_OFFSET + SW_NAND_WRITE_POOL_SIZE)
	#define USB_TRANSPORT_SIZE					(0x0100000)		//1M=2*512k(2个block大小，block最大512k)
	
	#define SDRAM_SYS_MEMORY_POOL_SIZE			(USB_TRANSPORT_OFFSET+USB_TRANSPORT_SIZE)
    
	#define SDRAM_VIDEO_SFC_MEMORY_POOL_SIZE	(0x157D000) 	

	#define SDRAM_PIC_DECODE_MEMORY_POOL_SIZE	(10240)//10K的buf	
	//资源文件头占用的大小
	//#define SDRAM_RES_HEAD_MEMORY_POOL_SIZE	    (0x32000)	//20M
	
	//#define SDRAM_IE_MEMORY_POOL_SIZE           (0x300000)  //分给IE3M


/*
 *extern function
*/

void * hsaSdramReusableMemoryPool( void );

#endif /* TM_APP_DEMO_REUSABLE_SDRAM_MEMORY_H_ */