#ifndef RESERVE_MANAGER_H
#define RESERVE_MANAGER_H

/*************************************************************************
 * INCLUDES
 *************************************************************************/

 /*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "vhChip.h"
#include "hyhwNandFlash.h"

#include "string.h"
#include "AbstractFileSystem.h"
#include "BoardSupportPackage.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

/********************************************************************************
第一片物理nand 的分配图 
    +----------------------------------------------------------+
    | block0 | block1--     |block10开始 | 。。。 |最后8M       |
    |        | block9       |            |       |             |
    +----------------------------------------------------------+
    |telechip| nand的U盘区  | reserve 区 | U盘区域| 启动程序    |
    | 占用   | mapping table|            |       |底层驱动保护  |
    +----------------------------------------------------------+
*********************************************************************************/

#define NAND_SIZE_UNIT						(1024*1024)	//NAND SIZE单位为 M

#define RESERVED_AREA_USED					(0xF0F0F0F0)

#define RESEVE_START_BLOCK_R_ONLY			(1)		//reserve区起始block号(只有reserve区)
#define RESEVE_START_BLOCK_R_FILE			(10)	//reserve区起始block号(有reserve区，也有文件系统区)
#define RESEVE_EARE_START_BLOCK				HSA_GetReserveEare_StartBlockNum()

//#define IMAGE_AREA_SIZE						(20)	//以M为单位
//#define PROGRAM_AREA_SIZE					(IMAGE_AREA_SIZE*2)	//以M为单位 用于存放系统整个程序，分为2个区，分别管理
#define IMAGE_AREA_SIZE						(10)	//以M为单位
#define PROGRAM_AREA_SIZE					(IMAGE_AREA_SIZE*4)	//以M为单位 用于存放系统整个程序，分为2个区，分别管理

#define IDENTIFIER_STRING_MAX_LEN			(64)	//标识符的最大长度

#define RESERVED_AREA_BLK_MAP_SIZE  		(8*1024)	//SDRAM_BLOCK_MAP_SIZE


/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
typedef enum
{
	reserveOnly = 0,		/*	nand只有reserve区，而没有文件系统，
								没有nand sw层
							*/
	reserveAndFileSys		/*	nand有reserve区，也有文件系统，
								且reserve区的管理在nand sw层之下
							*/
	
} eNAND_USE;

typedef struct image_data_info_st
{
	int  imageLen;
	int  imageCrc;
} tIMAGE_DATA_INFO;

typedef struct app_image_data_info_st
{
	char versionStr[16];
	U32  *pRunAddr;		//appImage 运行地址
	int  imageMaxLen;
	int  imageLen;
	int  imageCrc;
} tAPP_IMAGE_DATA_INFO;

/*************************************************************************
 * GLOBALS
 *************************************************************************/



/*************************************************************************
* 函数: HSA_SetPersistentParameter
* 说明: 保存一些需要关机时保存的参数，这个函数必须一次给所有的数据
* 参数: destination	--	指向调用者给出的缓存，函数调用完后，获得的信息在此缓存中
*		offset		--	相对persistent area的偏移量，以字节计，总是为0
*		len			--	长度，以字节计，最大的长度
* 返回: 错误码
 *************************************************************************/
int HSA_SetPersistentParameter(int type, char* source, int offset, int len);

/*************************************************************************
* 函数: HSA_GetPersistentParameter
* 说明: 获取一些需要关机时保存的参数 
* 参数: destination	--	指向调用者给出的缓存，函数调用完后，获得的信息在此缓存中
*		offset		--	相对persistent area的偏移量，以字节计
*		len			--	长度，以字节计
* 返回: 错误码
 *************************************************************************/
int HSA_GetPersistentParameter(int type, char *destination, int offset, int len);


int HSA_UpdateReservedArea( char* reservedAreaFile );
int HSA_UpdateReservedAreaFile( char* reservedAreaFile, unsigned offset);


/*************************************************************************
* 函数: HSA_GetReserveAreaBlockMap
* 说明: 获得存储资源文件的reserve区的blockMap Table
* 参数: memory	--	调用者管理的内存，至少2K，用作blockMap的临时buffer
* 返回: 第1个资源文件的size，（取block大小的size）,
*		如果没有 blockMap，将返回0	
*		并且，全局变量gkBlockMap指向的内存会写入blockmap 表
 *************************************************************************/
int HSA_GetReserveAreaBlockMap( char * memory );

/*************************************************************************
* 函数: HSA_AccessReservedArea
* 说明: 获得reserve区中的信息
* 参数: data	--	调用者管理的内存，用于返回获得的数据
*		offset	--	reserve信息的offset
*		size	--	要获得的数据的字节数
* 返回: 错误码
 *************************************************************************/
int HSA_AccessReservedArea( char * data, int offset, int size );



#endif //#ifndef RESERVE_MANAGER_H