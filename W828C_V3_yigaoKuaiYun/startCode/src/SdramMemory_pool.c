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

#include "memory_pool.h"
#include "SdramMemory_pool.h"
/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/

/*	Shark Sdram Memory 分配

shark 外部的 2M SDRAM 的分配使用表， 根据硬件不同，可能有
=================================================================================	
	只有2M SDRAM 暂时只适用于220*176的显示大小
	0x20000000的地址是cachable，bufferable的
	0x28000000的地址是noncachable，nonbufferable的
	+----------------------+------+----------------------+
	|0x20000000~0x200FFFFF |  1M  | 保留前1M给程序       |  **
	+----------------------------------------------------+
	|0x20100000~0x201007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20100800~0x20101FFF |  6K  | 需要保存的数据buffer |  **
	+----------------------------------------------------+
	|0x20102000~0x20114BFF |  76K | 显存                 |  **
	+----------------------------------------------------+
	|0x20114C00~0x201277FF |  76K | 背景图               |  **
	+----------------------------------------------------+
	|0x20127800~0x2013A3FF |  76K | 显示缓冲区           |  **
	+----------------------------------------------------+
	|0x2013A400~0x20185BFF | 302K | 音频播放buffer       |  **
	+----------------------------------------------------+
	
	注:
		1.0x20185C00之后可以在音乐播放时用作其他(如:电子书,图片等);
		2.视频(MP4/AVI/SWF)播放时,可从0x20114C00之后开始使用.

shark 外部的 8M SDRAM 的分配使用表， 根据硬件不同，可能有
=================================================================================	
	0x20000000的地址是cachable，bufferable的
	0x28000000的地址是noncachable，nonbufferable的
	+----------------------+------+----------------------+
	|0x20000000~0x200FFFFF |  1M  | 保留前1M给程序       |  **
	+----------------------------------------------------+
	|0x20100000~0x201007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20100800~0x20101FFF |  6K  | 需要保存的数据buffer |  **
	+----------------------------------------------------+
	|0x20100800~0x2010BFFF | 40K  | 应用层管理文件buffer |  **
	+----------------------------------------------------+
	|0x2010C000~0x2010C3FF |  1K  | SD卡初始化buffer     |  **
	+----------------------------------------------------+
	|0x2010C400~0x2017CBFF |150K*3| 显存+背景图+显示缓冲区,由应用层划分  |  **
	+----------------------------------------------------+
	|0x2017CC00~0x201C83FF | 302K | 10K+40K+252K=302K    |  **
	+----------------------------------------------------+
	|除code本身，需要占用2+6+40+1+150*3+302k=801k        |
	+----------------------------------------------------+
	2007-11-1 05:42下午 高，修正说明

shark 外部的 16M SDRAM 的分配使用表， 根据硬件不同，可能有
=================================================================================	
	0x20000000的地址是cachable，bufferable的
	0x28000000的地址是noncachable，nonbufferable的
	+----------------------+------+----------------------+
	|0x20000000~0x201FFFFF |  1M  | 保留前1M给程序       |  **
	+----------------------------------------------------+
	|0x20200000~0x202007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20200800~0x20201FFF |  6K  | 需要保存的数据buffer |  **
	+----------------------------------------------------+
	|0x20200800~0x2020BFFF | 40K  | 应用层管理文件buffer |  **
	+----------------------------------------------------+
	|0x2020C000~0x2020C3FF |  1K  | SD卡初始化buffer     |  **
	+----------------------------------------------------+
	|0x2020C400~0x2027CBFF |150K*3| 显存+背景图+显示缓冲区,由应用层划分  |  **
	+----------------------------------------------------+
	|0x2027CC00~0x202C83FF | 302K | 10K+40K+252K=302K    |  **
	+----------------------------------------------------+
	|除code本身，需要占用2+6+40+1+150*3+302k=801k        |
	+----------------------------------------------------+
	2007-11-1 05:42下午 高，修正说明
*/


#define NON_CACHEABLE_MASK		(~0x04000000)
/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/

#pragma arm section zidata = "sdram_memory_pool"
	char gSdramSysMemoryPool[SDRAM_SYS_MEMORY_POOL_SIZE];
#pragma arm section

#pragma arm section zidata = "sdram_video_sfc_memory_pool"
	char gSdramVideoSfcMemoryPool[SDRAM_VIDEO_SFC_MEMORY_POOL_SIZE];
#pragma arm section

//#pragma arm section zidata = "file_manager_memory_pool"
//	char gSdramFileMangerMemoryPool[SDRAM_FILE_MANAGER_MEM_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "sdram_recinfo_memory_pool"
//	char gSdramRecInfoMemoryPool[SDRAM_REC_INFO_POOL_SIZE];
//#pragma arm section

#pragma arm section zidata = "sdram_pic_decode_memory_pool"
	char gSdramPicDecodeMemoryPool[SDRAM_PIC_DECODE_MEMORY_POOL_SIZE];
#pragma arm section

//#pragma arm section zidata = "sdram_res_head_memory_pool"
//	char gSdramResHeadMemoryPool[SDRAM_RES_HEAD_MEMORY_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "config_file_memory_pool"
//	char gSdramCnfgFileMemoryPool[SDRAM_CONFIG_FILE_MEMORY_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "ie_memory_pool"
//	char gSdramIeMemoryPool[SDRAM_IE_MEMORY_POOL_SIZE];
//#pragma arm section

/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/
//小记录存储使用
#define SMALL_REC_BUF_OFFSET	0
#define SMALL_REC_BUF_SIZE		(140*1024)//预留128k用来做坏块数据搬移用

//大记录存储使用
#define LARGE_REC_BUF_OFFSET	(SMALL_REC_BUF_OFFSET+SMALL_REC_BUF_SIZE)
#define LARGE_REC_BUF_SIZE		(64*1024)

//一票多件使用
#define MULTI_CODE_BUF_OFFSET	(LARGE_REC_BUF_OFFSET+LARGE_REC_BUF_SIZE)
#define MULTI_CODE_BUF_SIZE		(256*1024)

//小记录存放的队列
#define REC_QUEUE_BUF_OFFSET	(MULTI_CODE_BUF_OFFSET+MULTI_CODE_BUF_SIZE)
#define REC_QUEUE_BUF_SIZE		(512*1024)

//发送记录时使用的部分
#define REC_SEND_BUF_OFFSET		(REC_QUEUE_BUF_OFFSET+REC_QUEUE_BUF_SIZE)
#define REC_SEND_BUF_SIZE		(100*1024)

//存储打包记录   (该大小决定了可以打包的最大个数  目前是最大50个)
#define REC_GROUP_BUF_OFFSET	(REC_SEND_BUF_OFFSET+REC_SEND_BUF_SIZE)
#define REC_GROUP_BUF_SIZE		(26*1024)

//条码对比buf
#define BARCOMPARE_BUF_OFFSET	(REC_GROUP_BUF_OFFSET+REC_GROUP_BUF_SIZE)
#define BARCOMPARE_SIZE			(64*1024)

//数据编解码
#define DECODE_BUF_OFFSET		(BARCOMPARE_BUF_OFFSET+BARCOMPARE_SIZE)
#define DECODE_BUF_SIZE			(200*1024)

//用户分组(登录时返回登录人员分组信息)
#define USERGROUP_BUF_OFFSET	(DECODE_BUF_OFFSET+DECODE_BUF_SIZE)
#define USERGROUP_BUF_SIZE		(2048)

/*--------以上共占用 1197056 bytes   1.1416015625 M------------*/
//GPS使用
#define GPS_BUF_OFFSET			(USERGROUP_BUF_OFFSET+USERGROUP_BUF_SIZE)
#define GPS_BUF_SIZE			(32*1024)

//手写识别
#define HAND_WRITE_BUF_OFFSET	(GPS_BUF_OFFSET+GPS_BUF_SIZE)
#define HAND_WRITE_BUF_SIZE		(0x210000)

//拼音输入法
#define ALPHA_INPUT_OFFSET      (HAND_WRITE_BUF_OFFSET+HAND_WRITE_BUF_SIZE)
#define ALPHA_INPUT_SIZE        (0x70800)

//拼音输入法显示
#define ALPHA_DISPLAY_OFFSET	(ALPHA_INPUT_OFFSET+ALPHA_INPUT_SIZE)
#define ALPHA_DISPLAY_SIZE		(0x4B000)

//联想输入法  0x4aa89e
#define INPUT_BUF_OFFSET		(ALPHA_DISPLAY_OFFSET+ALPHA_DISPLAY_SIZE)
#define INPUT_BUF_SIZE			(0x4aa900)

//供前台使用的临时buf 1
#define FRONT_BUF1_OFFSET		(INPUT_BUF_OFFSET+INPUT_BUF_SIZE)
#define FRONT_BUF1_SIZE			(4096)

//供前台使用的临时buf 2
#define FRONT_BUF2_OFFSET		(FRONT_BUF1_OFFSET+FRONT_BUF1_SIZE)
#define FRONT_BUF2_SIZE			(10240)

//供前台使用的临时buf 3
#define FRONT_BUF3_OFFSET		(FRONT_BUF2_OFFSET+FRONT_BUF2_SIZE)
#define FRONT_BUF3_SIZE			(20480)

//公司列表
#define COMPANY_BUF_OFFSET		(FRONT_BUF3_OFFSET+FRONT_BUF3_SIZE)
#define COMPANY_BUF_SIZE		(0x200000)

//员工列表
#define EMPLOYE_BUF_OFFSET		(COMPANY_BUF_OFFSET+COMPANY_BUF_SIZE)
#define EMPLOYE_BUF_SIZE		(200*1024)

//(收派区域)目的地列表
#define REGION_BUF_OFFSET		(EMPLOYE_BUF_OFFSET+EMPLOYE_BUF_SIZE)
#define REGION_BUF_SIZE			(0x100000)

//异常原因表
#define ABNORMAL_BUF_OFFSET		(REGION_BUF_OFFSET+REGION_BUF_SIZE)
#define ABNORMAL_BUF_SIZE		(10*1024)

//车辆信息表
#define CAR_BUF_OFFSET			(ABNORMAL_BUF_OFFSET+ABNORMAL_BUF_SIZE)
#define CAR_BUF_SIZE			(100*1024)

//航空信息表
#define FLIGHT_BUF_OFFSET		(CAR_BUF_OFFSET+CAR_BUF_SIZE)
#define FLIGHT_BUF_SIZE			(100*1024)

//问题件列表
#define PROBLEM_BUF_OFFSET		(FLIGHT_BUF_OFFSET+FLIGHT_BUF_SIZE)
#define PROBLEM_BUF_SIZE		(10*1024)

//留仓件列表
#define LEAVE_BUF_OFFSET		(PROBLEM_BUF_OFFSET+PROBLEM_BUF_SIZE)
#define LEAVE_BUF_SIZE			(10*1024)

#define CHAXUN_BUFF_OFFSET     (LEAVE_BUF_OFFSET+LEAVE_BUF_SIZE)
#define CHAXUN_BUFF_SIZE        (40960)//40K

//重量体积编码
#define VOLWEI_BUF_OFFSET		(CHAXUN_BUFF_OFFSET+CHAXUN_BUFF_SIZE)
#define VOLWEI_BUF_SIZE			(2*1024)

//运输方式(班次)
#define TRANSPORT_BUF_OFFSET	(VOLWEI_BUF_OFFSET+VOLWEI_BUF_SIZE)
#define TRANSPORT_BUF_SIZE		(10*1024)


//软件升级使用    (由于升级必须前台执行,所以该buf可以用来作为前台的临时buf使用)
#define UPGRADE_BUF_OFFSET		(TRANSPORT_BUF_OFFSET+TRANSPORT_BUF_SIZE)
#define UPGRADE_BUF_SIZE		(0x400000)


//后台使用的临时buf
/*------从到该处共占用  bytes   M   */


//以上共占用  +   =  M

//注意::::该处剩余如果小于3M,需要特别注意调用该函数的地方,是否有超出的可能,切记切记!!
#define REUSABLE_BUF_OFFSET		(UPGRADE_BUF_OFFSET+UPGRADE_BUF_SIZE)
#define REUSABLE_BUF_SIZE		(SDRAM_VIDEO_SFC_MEMORY_POOL_SIZE-REUSABLE_BUF_OFFSET)


/* 注意二维识别的buff空间是从SFC中分配的
   注意他的起始不要和重要的buff重用 大小不要超过了SFC的范围 20110504 */
//#define QR_DECODE_BUF_OFFSET  (0x25400000)
//#define QR_DECODE_BUF_SIZE    (0x700000)
/*----------------------------------------------------------------------------*/
static char *hsaSdramMemory()
{
	return ((char *) ((int)&gSdramVideoSfcMemoryPool[0] ) );
}

/*----------------------------------------------------------------------------*/
//小记录存储使用
void *hsaSdram_SmallRecBuf()
{
	return hsaSdramMemory()+SMALL_REC_BUF_OFFSET;
}

U32 hsaSdram_SmallRecSize()
{
	return SMALL_REC_BUF_SIZE;
}

//大记录存储使用
void *hsaSdram_LargeRecBuf()
{
	return hsaSdramMemory()+LARGE_REC_BUF_OFFSET;
}

U32 hsaSdram_LargeRecSize()
{
	return LARGE_REC_BUF_SIZE;
}

//一票多件使用
void *hsaSdram_MultiRecBuf()
{
	return hsaSdramMemory()+MULTI_CODE_BUF_OFFSET;
}

U32 hsaSdram_MultiRecSize()
{
	return MULTI_CODE_BUF_SIZE;
}

//小记录存放的队列
void *hsaSdram_RecQueBuf()
{
	return hsaSdramMemory()+REC_QUEUE_BUF_OFFSET;
}

U32 hsaSdram_RecQueSize()
{
	return REC_QUEUE_BUF_SIZE;
}

//发送记录时使用的部分
void *hsaSdram_RecSendBuf()
{
	return hsaSdramMemory()+REC_SEND_BUF_OFFSET;
}

U32 hsaSdram_RecSendSize()
{
	return REC_SEND_BUF_SIZE;
}

//存储打包记录   (该大小决定了可以打包的最大个数  目前是最大50个)
void *hsaSdram_RecGroupBuf()
{
	return hsaSdramMemory()+REC_GROUP_BUF_OFFSET;
}

U32 hsaSdram_RecGroupSize()
{
	return REC_GROUP_BUF_SIZE;
}

//条码对比
void *hsaSdram_BarCompareBuf()
{
	return hsaSdramMemory()+BARCOMPARE_BUF_OFFSET;
}

U32 hsaSdram_BarCompareSize()
{
	return BARCOMPARE_SIZE;
}

//数据编解码
void *hsaSdram_DecodeBuf()
{
	return hsaSdramMemory()+DECODE_BUF_OFFSET;
}

U32 hsaSdram_DecodeSize()
{
	return DECODE_BUF_SIZE;
}

//用户分组(登录时返回登录人员分组信息)
void *hsaSdram_UserGroupBuf()
{
	return hsaSdramMemory()+USERGROUP_BUF_OFFSET;
}

U32 hsaSdram_UserGroupSize()
{
	return USERGROUP_BUF_SIZE;
}


//GPS使用
void *hsaSdram_GpsBuf()
{
	return hsaSdramMemory()+GPS_BUF_OFFSET;
}

U32 hsaSdram_GpsSize()
{
	return GPS_BUF_SIZE;
}

//手写识别
void *hsaSdram_HandWriteBuf()
{
	return hsaSdramMemory()+HAND_WRITE_BUF_OFFSET;
}

U32 hsaSdram_HandWriteSize()
{
	return HAND_WRITE_BUF_SIZE;
}

//拼音输入法
void *hsaSdram_AlphaInputBuf()
{
	return hsaSdramMemory()+ALPHA_INPUT_OFFSET;
}

U32 hsaSdram_AlphaInputSize()
{
	return ALPHA_INPUT_SIZE;
}

//拼音输入法显示
void *hsaSdram_AlphaDisplayBuf()
{
	return hsaSdramMemory()+ALPHA_DISPLAY_OFFSET;
}

U32 hsaSdram_AlphaDisplaySize()
{
	return ALPHA_DISPLAY_SIZE;
}

//联想输入法
void *hsaSdram_InputBuf()
{
	return hsaSdramMemory()+INPUT_BUF_OFFSET;
}

U32 hsaSdram_InputSize()
{
	return INPUT_BUF_SIZE;
}

//供前台使用的临时buf 1
void *hsaSdram_FrontBuf1()
{
	return hsaSdramMemory()+FRONT_BUF1_OFFSET;
}

U32 hsaSdram_Front1Size()
{
	return FRONT_BUF1_SIZE;
}

//供前台使用的临时buf 2
void *hsaSdram_FrontBuf2()
{
	return hsaSdramMemory()+FRONT_BUF2_OFFSET;
}

U32 hsaSdram_Front2Size()
{
	return FRONT_BUF2_SIZE;
}

//供前台使用的临时buf 3
void *hsaSdram_FrontBuf3()
{
	return hsaSdramMemory()+FRONT_BUF3_OFFSET;
}

U32 hsaSdram_Front3Size()
{
	return FRONT_BUF3_SIZE;
}

//公司列表
void *hsaSdram_CompanyBuf()
{
	return hsaSdramMemory()+COMPANY_BUF_OFFSET;
}

U32 hsaSdram_CompanySize()
{
	return COMPANY_BUF_SIZE;
}

//员工列表
void *hsaSdram_EmployeBuf()
{
	return hsaSdramMemory()+EMPLOYE_BUF_OFFSET;
}

U32 hsaSdram_EmployeSize()
{
	return EMPLOYE_BUF_SIZE;
}

//(收派区域)目的地列表
void *hsaSdram_RegionBuf()
{
	return hsaSdramMemory()+REGION_BUF_OFFSET;
}

U32 hsaSdram_RegionSize()
{
	return REGION_BUF_SIZE;
}
//异常原因列表
void *hsaSdram_AbnormalBuf()
{
	return hsaSdramMemory()+ABNORMAL_BUF_OFFSET;
}

U32 hsaSdram_AbnormalSize()
{
	return ABNORMAL_BUF_SIZE;
}
//车辆信息列表
void *hsaSdram_CarBuf()
{
	return hsaSdramMemory()+CAR_BUF_OFFSET;
}

U32 hsaSdram_CarSize()
{
	return CAR_BUF_SIZE;
}
//航空信息列表
void *hsaSdram_FlightBuf()
{
	return hsaSdramMemory()+FLIGHT_BUF_OFFSET;
}
//资料列表模糊查询使用buf
void *hsaSdram_GetChaXunBuf(void)
{
	return hsaSdramMemory()+CHAXUN_BUFF_OFFSET;
}
int hsaSdram_GetChaXunBufSize()
{
	return CHAXUN_BUFF_SIZE;
}
U32 hsaSdram_FlightSize()
{
	return FLIGHT_BUF_SIZE;
}
//问题件列表
void *hsaSdram_ProblemBuf()
{
	return hsaSdramMemory()+PROBLEM_BUF_OFFSET;
}

U32 hsaSdram_ProblemSize()
{
	return PROBLEM_BUF_SIZE;
}

//留仓件列表
void *hsaSdram_LeaveBuf()
{
	return hsaSdramMemory()+LEAVE_BUF_OFFSET;
}

U32 hsaSdram_LeaveSize()
{
	return LEAVE_BUF_SIZE;
}

//重量体积编码
void *hsaSdram_WeiVolBuf()
{
	return hsaSdramMemory()+VOLWEI_BUF_OFFSET;
}

U32 hsaSdram_WeiVolSize()
{
	return VOLWEI_BUF_SIZE;
}


//运输方式(班次)
void *hsaSdram_TransportBuf()
{
	return hsaSdramMemory()+TRANSPORT_BUF_OFFSET;
}

U32 hsaSdram_TransportSize()
{
	return TRANSPORT_BUF_SIZE;
}

//软件升级使用    (由于升级必须前台执行,所以该buf可以用来作为前台的临时buf使用)
void *hsaSdram_UpgradeBuf()
{
	return hsaSdramMemory()+UPGRADE_BUF_OFFSET;
}

U32 hsaSdram_UpgradeSize()
{
	return UPGRADE_BUF_SIZE;
}


//
void * hsaSdramReusableMemoryPool( void )
{
	return hsaSdramMemory()+REUSABLE_BUF_OFFSET;
}

unsigned int hsaSdramReusableMemoryPool_Size( void )
{
	return (REUSABLE_BUF_SIZE);
}

/*void *hsaSdram_QRDecodeBuf()
{
	return (U8 *)QR_DECODE_BUF_OFFSET;//hsaSdramReusableMemoryPool()+REUSABLE_BUF_SIZE-QR_DECODE_BUF_SIZE;
}

int hsaSdram_QRDecodeBufSize()
{
	return QR_DECODE_BUF_SIZE ;
}*/

void * hsaSdramUnCacheReusableMemoryPool( void )
{
	return (((U32)hsaSdramReusableMemoryPool())&NON_CACHEABLE_MASK);
}

unsigned int hsaSdramUnCacheReusableMemoryPool_Size( void )
{
	return (REUSABLE_BUF_SIZE);
}


//用于图片解码用
void *hsaSdram_GetPicDecodeBuf(void)
{
     return ((void *) &(gSdramPicDecodeMemoryPool[0]) ); 
}

int hsaSdram_GePicDecodeBufLen(void)
{
    return  SDRAM_PIC_DECODE_MEMORY_POOL_SIZE;
}


//获得nand reserve block map(2k)在Sdram上的位置
void * hsaNandReserv_GetBlockMapBuffer(void ) 
{
	return ((void *) &(gSdramSysMemoryPool[SDRAM_BLOCK_MAP_OFFSET]) );
}
//获得nand reserve block map的大小(2k)
unsigned int  hsaNandReserv_GetBlockMapBufferSize(void )
{
	return ((unsigned int) SDRAM_BLOCK_MAP_SIZE );
}

//获得需要保存的参数(6k)在Sdram上的位置
void * hsaPersistentParameters_GetBuffer(void )
{
	return ((void *) &(gSdramSysMemoryPool[SDRAM_PERSISTENT_PARAMETER_OFFSET]) );
}
//获得需要保存的参数总大小(6k)
unsigned int  hsaPersistentParameters_GetBufferSize(void )
{
	return ((unsigned int) SDRAM_PERSISTENT_PARAMETER_SIZE );
}

//获得显示所需的内存
//显示屏QVGA大小320*240-----(((320*240)*2)*3)=150k*3
//显示屏QCIF大小220*176-----(((220*176)*2)*3)=77440*3
void * hsaDisplayMemoryPool_GetBuffer( void )
{
	return ((void *) (&gSdramSysMemoryPool[SDRAM_DISP_MEMORY_POOL_OFFSET]) );
}
//获得显示所需的内存大小
unsigned int  hsaDisplayMemoryPool_GetBufferSize( void )
{
	return ((unsigned int)SDRAM_DISP_MEMORY_POOL_SIZE);
}

/*----------------------------------------------------------------------------*/
#if 1
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

void * hyswNandFlash_GetReadPool(void)
{
	//在Sdram上预留4k = 1个Nand物理pagesize
	(void)hyswNandFlash_GetReadPool_NoUse();
    return ((void * ) (&gSdramSysMemoryPool[SW_NAND_READ_POOL_OFFSET]));
}
void * hyswNandFlash_GetWritePool(void)
{
	//72k = 4+4+64k(4k nand写缓存，4k MLC nand写缓存，64k mapping table)
    return ((void * ) (&gSdramSysMemoryPool[SW_NAND_WRITE_POOL_OFFSET]));
}
/*----------------------------------------------------------------------------*/

//USB时，BLOCK映射使用的buffer
void * hsaUsbTransport_GetBuffer( void )
{
	return ((void *) (&gSdramSysMemoryPool[USB_TRANSPORT_OFFSET] ) );
}

unsigned int hsaUsbTransport_GetBufferSize( void )
{
    return (USB_TRANSPORT_SIZE);
}

/*-----------------------------------------------------------
 * 函数:hsaConvertUnCacheMemory
 * 功能:把cacheble的地址转换成uncheble地址
 * 参数:需要转化的指针
 * 返回:得到uncheble的地址
 *----------------------------------------------------------*/
void *hsaConvertUnCacheMemory(void *pMem)
{
    return ((void *)((U32)(((U32)pMem) & NON_CACHEABLE_MASK)));    
}

/*----------------------------------------------------------------------------*/
