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
* 文件:	hyfilebios.h 
* 作者:	ZhouJie 
* 创建:	2005 12 05 15:52
* 说明:	有关磁盘的底层的一些结构的定义，参考白皮书，包括
*		1。Bios Parameter Block的信息的结构
*		2。目录项信息的结构及结构中的成员的位定义
* 相关：无相关文件
* 平台:	ARM 32位，小端格式
*****************************************************************************/

#ifndef _HYFILE_BIOS_H_
#define _HYFILE_BIOS_H_


/*	16 字节的dos分区表信息的结构, 是MBR结构的一部分数据的结构，MBR结构见下 
	http://en.wikipedia.org/wiki/Partition_(computing)*/
typedef struct _PARTITION 
{
	unsigned char		active;			/* offset 0		分区状态活动分区0x80，非活动分区0x0  */
	unsigned char		start_head;		/* offset 1		该分区的起始磁头  */
	unsigned char		start_cs_0;		/* offset 2		该分区的起始柱面和扇区  */
	unsigned char		start_cs_1;		
	unsigned char		type;			/* offset 4		该分区类型。
														0x4	-- DOS FAT16<32M
														0x5	-- EXTEND
														0x6	-- DOS FAT16>32M
														0x7	-- NTFS(OS/2)
														0xB	-- FAT32
														83---LINUX>64M  */
	unsigned char		end_head;		/* offset 5		该分区的终止磁头	*/
	unsigned char		end_cs_0;		/* offset 6		该分区的终止柱面和扇区  */
	unsigned char		end_cs_1;		
	unsigned char		start_0;		/* offset 8		该分区起始的扇区号  */
	unsigned char		start_1;		
	unsigned char		start_2;		
	unsigned char		start_3;		
	unsigned char		size_0;			/* offset 12	分区的大小，以扇区数计算 */
	unsigned char		size_1;			
	unsigned char		size_2;			
	unsigned char		size_3;			
} PARTITION, *pPARTITION;

/*	分区结构的预定义 */
#define	PT_ACTIVE			0x80
#define	PT_NOACTIVE			0x00

#define PT_TYPE_FAT16S		0x04		/* DOS FAT16<32M */
#define PT_TYPE_EXTEND		0x05		
#define PT_TYPE_FAT16L		0x06		/* DOS FAT16>32M */
#define PT_TYPE_NTFS		0x07		
#define PT_TYPE_FAT32		0x0B
		



/*	Master Boot Record structure 
	对于可移动的存储介质，可以有，也可以没有.如果有，在逻辑0扇区上*/
#define	MBR_BOOTCODE_SIZE		0x1BE
#define	MAX_DOS_PARTITION		4
typedef struct _MBR 
{
	unsigned char	bootcode[MBR_BOOTCODE_SIZE];	/* boot sector 446 字节 */
	PARTITION		partition[MAX_DOS_PARTITION];	/* 4个分区表信息，每个16字节 */
	unsigned char	sig_55;							/* 0xAA55的签名 */
	unsigned char	sig_aa;					
} MBR, *pMBR;



/*	BPB- BIOS Parameter Block 的结构，参见白皮书  */
#define BPB16_SIZE		62				/* number of bytes in a BPB for FAT16/12 */
#define BPB32_SIZE		90				/* number of bytes in a BPB for FAT32*/

/*	注意这个结构由于变量对齐的问题， 有些使用了几个byte表示一个int或short数据
	为了借用buffer和从介质中读取方便，包括从 offset从0开始的信息
	这样,就和BPB的二进制信息对应了，也和buffer，和结构容易对应 */
typedef struct	_BPB16
{
	/*	FAT12/16 和FAT32 在offset 0x0 ~ offset 0x36 的BPB的定义是一样的
		BPB信息不包含3个字节的跳转表和８个字节的 OEM 信息 */
	char			jmpBoot[3];			/* offset 0	 跳转表*/
	char			OEMName[8];			/* OEM name & version */
	
	/* 以下为白皮书中的ｂｐｂ信息的开始　*/
	unsigned char	bytsPerSec_0;		/* offset 11	每扇区字节数。取值只能是下列值: 512,1024,2048,4096 */
	unsigned char	bytsPerSec_1;			
	char			secPerClus;			/* 每簇的扇区数, 合法值为1, 2, 4, 8, 16, 32, 64,128. */
										/* 和bytsPerSec配合，每簇字节数不应超过32k	*/
	unsigned char	rsvdSecCnt_0;		/* 从第一个扇区开始的保留区的扇区数。必须非零。*/
	unsigned char	rsvdSecCnt_1;		/* 对于FAT16/12卷, 必须只能为1，对于FAT32卷, 典型值为32。*/
										
	unsigned char	numFATs;			/* offset 16	number of FATs */
	unsigned char	rootEntCnt_0;		/* 对于FAT16/12, 表示在根目录区的32字节的目录项的个数 */
	unsigned char	rootEntCnt_1;		/* 典型值为0x0200 ，对于FAT32卷, 必须设为0*/
	
	unsigned char	totSec16_0;			/* 逻辑盘的总扇区数，存在totSec16 或totSec32中 */
	unsigned char	totSec16_1;			/* 如果总扇区数小于0x10000, 那16位数能存下,就存在totSec16中 
										   同时totSec32为0*/
	unsigned char	media;				/* offset 21	media descriptor byte */
	
	unsigned char	FATSz16_0;			/* FAT12/FAT16 的一个FAT 占用的扇区数	FAT32 此值必须为0 */
	unsigned char	FATSz16_1;			
	unsigned char	secPerTrk_0;		/* 为中断0x13使用的每个磁道的扇区数 */
	unsigned char	secPerTrk_1;		
	
	unsigned char	numHeads_0;			/* 为中断0x13使用的磁头数 */
	unsigned char	numHeads_1;		
		
	unsigned char	hiddSec_0;			/* 在包含此FAT卷的分区之前的隐藏扇区的数目。 */  
	unsigned char	hiddSec_1;			/* 此域仅和中断0x13可见的介质有关。 */
	unsigned char	hiddSec_2;			/* 对于没有分区的介质，这个域应总是设为0。 */
	unsigned char	hiddSec_3;			/* 如果有分区的介质，表示BPB扇区之前的扇区数 */

	unsigned char	totSec32_0;			/* offset 32	number of huge sectors (>32MGB) */
	unsigned char	totSec32_1;			/* 4 bytes */
	unsigned char	totSec32_2;			
	unsigned char	totSec32_3;		
	
	/*	以下 offset 0x36开始，为扩展BPB信息, 为FAT12/16 特有的，和FAT32不同 */	
	char			drvNum;				/* drive number */
	char			reserved1;			/* reserved，FAT12/16 应总设为0 */
	char			bootSig;			/* 扩展启动签名(0x29). 这是个签名字节指明了在启动扇区中存在下面的三个域。*/
	unsigned char	volID_0;			/* volume ID number  卷序列号*/
	unsigned char	volID_1;			
	unsigned char	volID_2;			
	unsigned char	volID_3;			
	char			volLab[11];			/* volume label */
	char			filSysType[8];		/* FAT16/12，可设为字符串 "FAT12   ", "FAT16   ", 或 "FAT     "之一。 */
										/* FAT32，仅设为字符串 "FAT32   " */
} BPB16, *pBPB16;



typedef struct	_BPB32
{
	char			jmpBoot[3];
	char			OEMName[8];		
	
	unsigned char	bytsPerSec_0;		
	unsigned char	bytsPerSec_1;			
	char			secPerClus;			
										
	unsigned char	rsvdSecCnt_0;		
	unsigned char	rsvdSecCnt_1;		
										
	unsigned char	numFATs;			
	unsigned char	rootEntCnt_0;		
	unsigned char	rootEntCnt_1;		
	
	unsigned char	totSec16_0;			
	unsigned char	totSec16_1;			 
										  
	unsigned char	media;				
	
	unsigned char	FATSz16_0;			
	unsigned char	FATSz16_1;			
	unsigned char	secPerTrk_0;		
	unsigned char	secPerTrk_1;		
	
	unsigned char	numHeads_0;			
	unsigned char	numHeads_1;			
	unsigned char	hiddSec_0;			
	unsigned char	hiddSec_1;			
	unsigned char	hiddSec_2;			
	unsigned char	hiddSec_3;			

	unsigned char	totSec32_0;			
	unsigned char	totSec32_1;			
	unsigned char	totSec32_2;			
	unsigned char	totSec32_3;		
	
	/*	以下 offset 0x36开始，为扩展BPB信息
		为FAT32 特有的，和FAT12/16不同 */	
	unsigned char	FATSz32_0;			/* big FAT size in sectors  */
	unsigned char	FATSz32_1;			
	unsigned char	FATSz32_2;			
	unsigned char	FATSz32_3;			
	unsigned char	extFlags_0;			/* extended flags */
	unsigned char	extflags_1;			
	unsigned char	FSVer_0;			/* filesystem version (0x00) low byte	*/
	unsigned char	FSVer_1;			/* filesystem version (0x00) high byte	*/
	unsigned char	rootClus_0;			/*	根目录的第一个簇的簇号，通常为2但不要求一定是2 */
	unsigned char	rootClus_1;				
	unsigned char	rootClus_2;				
	unsigned char	rootClus_3;				
	unsigned char	FSInfo_0;			/* 在FAT32 卷的保留区的 FSINFO 结构的扇区号，通常为 1。*/
	unsigned char	FSInfo_1;			
	unsigned char	bkboot_0;			/* 如果非零，表明boot record的一个拷贝在卷保留区的扇区号。通常为6。*/
	unsigned char	bkboot_1;			
	unsigned char	reserved[12];		/* reserved, should be 0 */

	/* 以下此域定义和对FAT12 和 FAT16介质的定义相同。
		唯一不同是对于 FAT32介质，在启动扇区的偏移量不同。	
		偏移量为0x64 */	
	char			drvNum;				
	char			reserved1;			
	char			bootSig;			
	unsigned char	volID_0;			
	unsigned char	volID_1;			
	unsigned char	volID_2;			
	unsigned char	volID_3;			
	char			volLab[11];			
	char			filSysType[8];		
} BPB32, *pBPB32;




/* Disk Directory， 目录项，32个字节 */
#define	NAME_SIZE			8			/*	8.3的短文件名 */
#define	EXT_SIZE			3
#define	SHORT_NAME_SIZE		NAME_SIZE	/*	8.3的短文件名 */
#define	SHORT_EXT_SIZE		EXT_SIZE

#define NAME_LOWERCASE		0x08		/*	8.3的短文件名全部小写的情况*/
#define EXT_LOWERCASE		0x10		/*	8.3的后缀名全部小写的情况*/


typedef struct
{
	char			name[NAME_SIZE];	/* filename */
	char 			ext[EXT_SIZE];		/* extension */
	unsigned char 	attribute;			/* offset 11 -- file attribute */
	
	unsigned char	reserved;			/* offset 12  
										   当名字符合8.3长度，且文件名和或扩展名全部小写时，
										   不创建长文件名，而是在磁盘上只有大写的短文件名和/或目录名，
										   0x08表示文件名全部小写，0x10表示扩展名全部小写,
										   如果其他情况，则按照长文件名的方式创建 */
	unsigned char	creattime10th;		/* create time, 10ths of a second (0-199 are valid) */
	unsigned short	creattime;			/* offset 14 -- creation time  */
	unsigned short	creatdate;			/* offset 16 -- creation date	*/
	unsigned short	lastaccessdate;		/* offset 18 -- last access date  */
	unsigned short	startcluster_h;		/* high word of first cluster,  (FAT32) */
	unsigned short	time;				/* time created or last updated */
	unsigned short	date;				/* date created or last updated */
	unsigned short	startcluster;		/* starting cluster */
	unsigned int 	file_size;			/* file size */
} DIRECTORY;


/*	目录项的名字域 name 的第一个字节的特殊意义的预定义 */
#define FN_NEVER_USED   0x00	/*	此目录项从没有使用过，标记了目录的结束 */
#define FN_E5			0x05	/*	名字的第一个字符是0xE5，由于0xE5用来标记删除的文件 */
#define FN_DIRECTORY	0x2E	/*	当前或父的子目录的别名。即字符".",如果下一个字节也是0x2E, 
									即字符串"..", 则在目录项的簇的域中是父目录的簇号（如果簇号
									为0，则父目录为根目录） */
#define FN_ERASED		0xE5	/*	已删除的文件 */

/* Attributes */
#define AT_RDONLY		0x01	/* file is read-only */
#define AT_HIDDEN		0x02	/* file is hidden */
#define AT_SYSTEM		0x04	/* system file */
#define AT_VOLNAME		0x08	/* volume name */
#define AT_DIRECTORY	0x10	/* subdirectory */
#define AT_ARCHIVE		0x20	/* file has been written to and closed */
#define AT_VFATENTRY    0x0F    /* VFAT long filename entry */


/* Time field encoding bit masks */
#define TM_SECS			0x001F	/*	binary number of 2-second increments
									0-29, corresponding to 0-58 seconds */
#define TM_MINUTES  	0x07E0	/* binary number of minutes */
#define TM_HOURS		0xF800	/* binary number of hours */

/* Date field encoding bit masks */
#define DT_DAY			0x001F	/* day of month */
#define DT_MONTH		0x01E0	/* month */
#define DT_YEAR			0xFE00	/* year (relative to 1980) */


#endif   /* _HYFILE_BIOS_H_ */
