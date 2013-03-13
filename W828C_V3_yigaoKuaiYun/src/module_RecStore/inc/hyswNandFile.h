/******************************************************************************
*  文件:		nandSeqStore.c (Hyctron Nandflash Abstract Layer 's fbd driver)
*  创建：		ZhouJie 
*  创建日期 :	2010 12 17 15:52
*  说明     :	针对物流应用，直接使用nand进行存储的方案

*   需求
*    1：获取磁盘上所有未上传数据。
*    2：生成记录。
*    3：上传成功后对该记录做标记，标识该记录已上传。
*    4：删除记录。(已上传和正在上传的记录不能删除)，删除后标记该记录；
*    5：业绩统计: 根据日期、扫描类型 获取指定日期的所有该扫描类型 的总数 和 已上传数  
*       (注：日期可以为指定天,指定月； 同一个文件包括多种扫描类型)
*    6：查看记录: 查看某一种扫描类型 在指定某一天的所有扫描的状态 (注: 状态 为 已上传、未上传) ，
*		还支持更改状态，即删除未上传的状态记录 ,以便后台不予上传
*    7：删除指定扫描类型的当天记录 及 删除 除最近三天的历史记录

*	设计

*	1。 bitmap 占一个page 循环写入，最后的字节标示有效与否与序号
*   2。 file entry 占一个page 循环写入, 最后的字节标示有效与否与序号
*   3。 每个file 的第一个block 记录文件中的记录的状态等信息，page循环写入
*        一个文件24 block  一个block 128 page，则记录最多 24*128 = 3076， 一个记录占2bit
*		如果这些记录都放在一个page中，则每个记录可以分10个字节

*	限制
*	所有设计，为了简单起见，nand 的page 设为4096 字节的大小

*  变量名字约定:	page 指 物理的page，现在有两种，512 或2048
*					sector， 指 logic sector，仅有512的size
*  预定义说明：	NAND_SUPPORT_HW_MLC			是否支持MLC的flash，如果支持，则目前仅支持1G的flash

*  修改(孔祥文 2011-06-15)
*  支持slc (512M   page size 2048  一个block 64个page)
*
*  修改(孔祥文 2012-07-21)
   mlc的支持去掉,没有同步更新
   最多支持30个文件,每个文件最多存储4608条记录,每条记录最大512字节,每天可以占用多个文件存储,但每个文件不能同时存储多天的记录,
   如果某个文件的记录全部删除,则该文件也被释放,可再次使用. 
   
   如果某个block的记录全部被删除,则该block可以释放,并重新计算该文件的记录总数; ????????????目前还没实现,调整这个时需要动到存放摘要的block
   
   增加记录状态(3bit表示 bit2,bit1,bit0,最多表示7种状态),
   0:未传(这种状态的记录可随时加载到上传队列发送)
   1:已传(该状态记录表示已经成功上传,还占用存储空间,业务统计时可以统计出来)
   2:已删(该状态的记录表示已删除但还占用着存储空间,文件中的存储总数也包括它,业务统计时不统计它)
   3:待传(这种状态表示该记录不要添加到发送队列中,需要等待信号才加载)
   4:可传(该状态是"待传"状态等到发送信号后改变成的,可以加载到上传队列发送;与"未传"状态的区别在于该状态加载时需要找到所有关联的记录一起发送)
   
 *****************************************************************************/


#ifndef HY_HW_NANDFILE_H_
#define HY_HW_NANDFILE_H_

/*---------------------------------------------------------------------------
   Project include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"


//#define NANDFLASH_DEBUG


/*********************************************
* slc 512M 一个block 64个page  一个page 2k
  分小记录存储区
  大记录存储区
  
  
  小记录存储区 30个文件     每个文件最多 4608条  每个文件要占用23block(摘要占用5个  记录占用18个)    共要占用23*30=690 blocks
  
*********************************************/
#define PAGE_SIZE_2K			//nand page的大小

#define NAND_FLASH_SIZE			512		//M  nand总大小
#define NAND_ENABLE_USESIZE		320		//M  可以提供存储记录使用的nand大小
#define NAND_PAGE_SIZE			2048	//nand page的大小

/*==============================================小记录===========================================================*/

#define MAX_NAND_FILE			30		//最大支持的文件数

#define REC_USE_MAXSIZE			100 	//M  可供此处使用的nand大小
#define BLOCK_MAX_NUM			(REC_USE_MAXSIZE<<3) /*可以使用的总共block数 (REC_USE_MAXSIZE*1024)/128 */

#define SAVE_REC_MAX_BLOCKS		18		/*用来存放记录的block   共可存放4608条记录  需要5个block存放摘要
								 		 即每个文件最大占用23个block    预存最多30个文件   共需 690block
								 		 bitmap  14个block  fileentry 26个block   共730个block  需91.25M磁盘空间*/
								 
#define RECORD_PER_SIZE			512		/*每个记录占用字节数*/
#define RECORDS_PER_PAGE		4		/*每个page存放记录的条数*/
#define PAGE_PER_BLOCK			64		/*每个block中page个数*/
#define RECORDS_PER_BLOCK		(RECORDS_PER_PAGE*PAGE_PER_BLOCK)		/* 一个block中存放记录的条数,就是page 数*4 256 */

#define PAGE_PER_BLK_BIT		6		/*  1<<6  = 64 */
#define BLK_SIZE_BIT			17		/*  1<<17 = 128k*/

#define NAND_PAGE_DATASIZE		(NAND_PAGE_SIZE - 8) /*由于在一个block 中最后8个字节用来存储seq crc 0xaa55 0xaa55，
                                                      *所以实际数据容量需要减掉*/

#define	NAND_BITMAP_WORDS		128	/*可管理block数= NAND_BITMAP_WORDS*32*/

#define NAND_BAD_BLOCKS			NAND_BITMAP_WORDS	//记录坏块

#define	BLOCK_NUM_BITMAP		14	/* 预留给nand bitmap的一组物理block的数目*/
#define	BLOCK_START_BITMAP		nandFile_startBlock()	/* 预留给nand bitmap的一组物理block的起始block index*/
#define	BLOCK_END_BITMAP		(BLOCK_START_BITMAP+BLOCK_NUM_BITMAP-1)	/* 预留给nand bitmap的一组物理block的最后block index*/

#define	BLOCK_NUM_FILEENTRY		26	/* 预留给file entry的一组物理block的数目*/
#define BLOCK_START_FILE		(BLOCK_END_BITMAP+1)//file的block开始
#define BLOCK_END_FILE			(BLOCK_START_FILE+BLOCK_NUM_FILEENTRY-1)//file的block结束


#define BLOCK_DATA_NUM			(BLOCK_MAX_NUM-BLOCK_NUM_BITMAP-BLOCK_NUM_FILEENTRY)	/*数据可用block总数*/
#define BLOCK_DATA_START		(BLOCK_END_FILE+1)										/*数据开始的block*/
#define BLOCK_DATA_END			(BLOCK_DATA_START+BLOCK_DATA_NUM-1)						/*数据结束的block*/


/*	一个文件占用的最大的block 数
	所有需要管理的文件信息存储在一个page中,一个page可用大小为2048-8 = 2040;
	2040/MAX_NAND_FILE = 每个文件可以使用的大小 filesize
	每个文件前面12字节固定,则后面存储pageOffset[]及blk[]的空间为  (filesize-12)/2  由于是U16的,所以要 /2*/
#define	MAX_NUMS_PER_FILE		((NAND_PAGE_DATASIZE/MAX_NAND_FILE - 12) / 2)	//pageOffset[]及blk[] 可以占用的空间   存30个文件,该值为28

/*根据MAX_NUMS_PER_FILE来计算需要存放摘要的block个数,假设需要摘要block数为x  (摘要中不再存crc等信息)
  根据一元方程式
  (MAX_NUMS_PER_FILE-x-x)*64*4 = 2048/2*x ; 
即 MAX_NUMS_PER_FILE = (1024/256 + 2)*x ;
   x = MAX_NUMS_PER_FILE / (1024/256 + 2)
 
 所以 x = MAX_NUMS_PER_FILE/6 (向上取整) */
#define ABS_BLOCKS_PER_FILE		((MAX_NUMS_PER_FILE+5)/6)//摘要占用block    存30个文件,该值为5

#define MAX_BLOCKS_PER_FILE		(MAX_NUMS_PER_FILE-ABS_BLOCKS_PER_FILE)//每个文件可管理block数 包括摘要和记录  存30个文件,该值为23
#define DATA_BLOCKS_PER_FILE	(MAX_BLOCKS_PER_FILE-ABS_BLOCKS_PER_FILE)//每个文件存放记录的block数  存30个文件,该值为18

/*前面已经定义每个文件中用来存储记录的最大block数   如果此处计算出来的值比之前定义大,则必须使用之前定义的*/
#if (DATA_BLOCKS_PER_FILE > SAVE_REC_MAX_BLOCKS)
	#define MAX_RECORDS_PER_FILE	(SAVE_REC_MAX_BLOCKS * RECORDS_PER_BLOCK)	//每个文件可以存放数据的最大数目
#else
	#define MAX_RECORDS_PER_FILE	(DATA_BLOCKS_PER_FILE * RECORDS_PER_BLOCK)	//每个文件可以存放数据的最大数目
#endif


/* 摘要中记录的信息为*/
typedef struct RECORD_STATUS
{
	unsigned short	type:8;        	//扫描类型
	unsigned short	userType:5; 	//员工类型(权限)
	unsigned short  recState:3;		/*记录状态
									  0:未传(这种状态的记录可随时加载到上传队列发送)
			   						  1:已传(该状态记录表示已经成功上传,还占用存储空间,业务统计时可以统计出来)
			   						  2:已删(该状态的记录表示已删除但还占用着存储空间,文件中的存储总数也包括它,业务统计时不统计它)
			   						  3:待传(这种状态表示该记录不要添加到发送队列中,需要等待信号才加载)
			   						  4:可传(该状态是"待传"状态等到发送信号后改变成的,可以加载到上传队列发送;与"未传"状态的区别在于该状态加载时需要找到所有关联的记录一起发送)
			   						*/
	
}tRECORD_STATUS;

#define ABSTRACT_SIZE			sizeof(tRECORD_STATUS)	/* 2 byte. 每个记录的摘要所占的字节数, 里面包括8位业务类型   5位员工类型   3位记录状态*/
#define ABSTRACT_NUM_PER_PAGE	(NAND_PAGE_SIZE/ABSTRACT_SIZE)	/*一个page存储摘要的个数*/

#define	NAND_SEQUECE_PAGE_TAG	0xAA55	/*每个block最后8个字节为存放sequence， CRC16 ,0xAA55, 0xAA55*/


#define	FILE_NAME_SIZE			6		/* 文件名， 6个字节*/

typedef struct
{
	char data[NAND_PAGE_DATASIZE];	/* 数据  (2048 - 8)字节，一个page 2k， 后面有8个字节是标示数据*/
	
	unsigned short	sequece;	/* 因为循环写入，关机后，不知道写入的最新的page，使用sequece 标示一轮
	                               例如，第一轮都是1，第2轮开始都是2，如果读到一个page 序号是2，而下一个
	                               page 序号是1，则表示page 为2的page 为最新，跨block 时有用*/
	
	unsigned short	crc16;		/* 每个block最后6个字节为存放 CRC ,0xAA55, 0xAA55*/
	
	unsigned short	tag1;
	unsigned short	tag2;
	
}tSEQUECE_PAGE;	



/* 文件信息项，和管理的block 数目有关 */
typedef struct
{
	//使用int   剩余2字节做文件索引
	int				name;
	unsigned short	dayIdx;					/*该文件是当天第几个文件*/
	/*char			name[FILE_NAME_SIZE];*/	/* 正常文件名共6字节：年2字节，月2字节，日2字节 
										   		长文件支持使用多个目录项，后续的目录项定义为
										   		name[0] == 0xF，后面为第几个附加目录项，下面的数据都是block index*/
	unsigned short	totalRecords;			/* 文件内容记录的总记录数  */
	unsigned short	reportedRecords;		/* 已向后台报告的记录数 */
	unsigned short	deletedRecords;			/* 已删除的记录数 */
	
	unsigned short  pageOffset[ABS_BLOCKS_PER_FILE];/* 每个摘要block中最新信息的位置*/
	unsigned short	blk[MAX_BLOCKS_PER_FILE];		/* 存放摘要和数据*/
	
}tFILE_ENTRY;



/* 通过nand 的block的bitmap，来获知是否该block可用 , 对应该bit 为1表示没有占用*/
#define  IS_BLOCK_FREE(blk)		(gnandBitmap[blk>>5] & (1<<(blk &0x1F)))
#define	 SET_BLOCK_USED(blk)	(gnandBitmap[blk>>5] &= ~(1<<(blk &0x1F)))
#define	 SET_BLOCK_FREE(blk)	(gnandBitmap[blk>>5] |= (1<<(blk &0x1F)))

//返回修改后的状态
typedef int (* MODIFY_STATUE_FUNC)(tRECORD_STATUS *pStatus);


extern	tFILE_ENTRY	gtFileEntry[MAX_NAND_FILE];

extern U8	*gpu8RecBuf;


/*==============================================大记录===========================================================*/
/*
共分2个文件  
每个文件最多存储1550条  
2个文件共存储 1550*2 = 3100
共占用 1600+50 = 1650
*/
#define MAX_PIC_FILE				2		//最大支持的文件数

#define	PIC_USE_MAXSIZE				(NAND_ENABLE_USESIZE-REC_USE_MAXSIZE)	//220
#define PIC_BLOCK_NUM				(PIC_USE_MAXSIZE<<3)	//1760 blocks

#define PIC_PER_SIZE				(1024*64)	//每个图片的大小
#define PIC_PER_BLOCK				2			//每个block中存放图片的个数

#define	PIC_BITMAP_WORDS			128	/*可管理block数= PIC_BITMAP_WORDS*32*/

/*bitmap  和 fileentry 共使用50 blocks*/
#define	PIC_BLOCK_NUM_BITMAP		14	/* 预留给nand bitmap的一组物理block的数目*/
#define	PIC_BLOCK_START_BITMAP		(BLOCK_START_BITMAP+BLOCK_MAX_NUM)	/* 预留给nand bitmap的一组物理block的起始block index*/
#define	PIC_BLOCK_END_BITMAP		(PIC_BLOCK_START_BITMAP+PIC_BLOCK_NUM_BITMAP-1)	/* 预留给nand bitmap的一组物理block的最后block index*/

//由于page限制,分2个文件存储   36 
#define PIC_BLOCK_FILEINFO			18	//存储文件信息的block

#define PIC_FILE1_START				(PIC_BLOCK_END_BITMAP+1)
#define PIC_FILE1_END				(PIC_FILE1_START+PIC_BLOCK_FILEINFO-1)

#define PIC_FILE2_START				(PIC_FILE1_END+1)
#define PIC_FILE2_END				(PIC_FILE2_START+PIC_BLOCK_FILEINFO-1)


//1760-50 = 1710 
#define BLOCK_PIC_NUM				(PIC_BLOCK_NUM-PIC_BLOCK_NUM_BITMAP-PIC_BLOCK_FILEINFO*2)	/*数据可用block总数*/
#define BLOCK_PIC_START				(PIC_FILE2_END+1)											/*数据开始的block*/
#define BLOCK_PIC_END				(BLOCK_PIC_START+BLOCK_PIC_NUM-1)							/*数据结束的block*/


#if 0//按找下面方法计算后,实际需要的block为1916, 但可以使用的block只有1710,  不够用
	
	//2个文件信息分开存放  所以每个page中只存放了一个fileentry
	#define	MAX_NUMS_PIC_FILE			((NAND_PAGE_DATASIZE - 4) / 2)	//1018

	/*
	(MAX_NUMS_PIC_FILE-x-x)*PIC_PER_BLOCK = PIC_ABS_NUM_PER_BLOCK*x
	(1018-x-x)*2 = 64*x
	x = 29.9
	*/
	#define PIC_ABS_BLOCKS_PER_FILE		30		//每个文件中存放摘要block的个数
	#define PIC_MAX_BLOCKS_PER_FILE		(MAX_NUMS_PIC_FILE-PIC_ABS_BLOCKS_PER_FILE)	//存储摘要和数据的总共block数  1018-30 = 988

	#define PIC_DATA_BLOCKS_PER_FILE	(PIC_MAX_BLOCKS_PER_FILE-PIC_ABS_BLOCKS_PER_FILE)//存储图片数据的block数  988-30 = 958
	#define MAX_PICS_PER_FILE			(PIC_DATA_BLOCKS_PER_FILE*PIC_PER_BLOCK)	//每个文件总共可以存放图片数 958*2 = 1916
	
#else
	
	#define PIC_ABS_BLOCKS_PER_FILE		25
	#define PIC_MAX_BLOCKS_PER_FILE		800
	
	#define PIC_DATA_BLOCKS_PER_FILE	(PIC_MAX_BLOCKS_PER_FILE-PIC_ABS_BLOCKS_PER_FILE)	//775
	#define MAX_PICS_PER_FILE			(PIC_DATA_BLOCKS_PER_FILE*PIC_PER_BLOCK)	//775*2 = 1550
	
#endif

#define PIC_SAVECODE_LEN		29

//32 byte
typedef struct _picAbs_t
{
	char		barcode[PIC_SAVECODE_LEN];	//对应包裹条码
	U8			status;			//数据状态 1:有效  0:无效
	U16			size;			//数据大小
	
}tPICABS;

#define PIC_ABSTRACT_SIZE			sizeof(tPICABS)			//每个摘要占用字节数
#define PIC_ABS_NUM_PER_BLOCK		(NAND_PAGE_SIZE/PIC_ABSTRACT_SIZE)	//一个block中可以保存的摘要数  2048/32 = 64



typedef struct picfile_t
{
	unsigned short	totalPic;			/* 文件内容记录的总记录数  */
	unsigned short	validPic;			/* 有效的记录数 */
	
	unsigned short  pageOffset[PIC_ABS_BLOCKS_PER_FILE];/* 每个摘要block中最新信息的位置*/
	unsigned short	blk[PIC_MAX_BLOCKS_PER_FILE];		/* 存放摘要和数据*/
	
}tPICFILE;


/* 通过nand 的block的bitmap，来获知是否该block可用 , 对应该bit 为1表示没有占用*/
#define  PIC_IS_BLOCK_FREE(blk)		(gu32PicBitmap[blk>>5] & (1<<(blk &0x1F)))
#define	 PIC_SET_BLOCK_USED(blk)	(gu32PicBitmap[blk>>5] &= ~(1<<(blk &0x1F)))
#define	 PIC_SET_BLOCK_FREE(blk)	(gu32PicBitmap[blk>>5] |= (1<<(blk &0x1F)))


extern U32	appLockNandCtr;

#endif	//HY_HW_NANDFILE_H_
