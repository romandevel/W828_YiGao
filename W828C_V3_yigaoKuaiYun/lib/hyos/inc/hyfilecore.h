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
* 文件:	dosfile.h 
* 作者:	ZhouJie 
* 创建:	2005 12 05 15:52
* 说明:	有关文件系统的一些结构的定义，包括
*		1。文件系统结构定义
*		2。目录操作项的结构定义
*		3。当前工作目录操作项的结构定义
*		4。打开的文件结构定义
* 相关：1。driver.h	  文件描述表FDTAB的结构定义
* 平台:	ARM 32位，小端格式
*****************************************************************************/

#ifndef _DOSFILE_H_
#define _DOSFILE_H_


/* Windows 98／95允许用户使用长文件名，包括路径在内的文件名长度最大可达255个ASCII字符。
	这是windows 白皮书规定的*/
#define	MAX_PATH_LENGTH				255		
typedef char PATH[MAX_PATH_LENGTH + 1];

/*	unicode的字符数，最坏情况为ascii转来的，就乘以2*/
#define MAX_LONGNAME_UNICODE_LENGTH	((MAX_PATH_LENGTH<<1)+4) 

/*	文件系统的结构，对于每个使用的文件系统，对应一个这样的结构变量
	当然，往往我们只有一个文件系统 */
#define		FAT_POOL_SIZE	0x400		/* fat Pool 的字节数 */
typedef struct
{
	FDTAB			fsfdtab;			/* info for file sub-system device  */
										/*	主要表示文件系统中，使用什么打开文件 */
										/*	例如使用什么的磁盘（设备）驱动 */
	short volatile	fsopens;			/* number of concurrent opens */
	short volatile	fsflags;			/* 仅借用offlag中的	F_BUSY标记，表示文件系统忙 */
	
	char			*fswait;			/* process waiting for FILE_SYS */
	
	/*	bpb 的部分信息 ，由于BPB信息中有些是我们用不到的，所以就不需要存下来，以便节省内存 	*/
	unsigned short	bpb_bytsPerSec;		/* bytes per sector */
	unsigned short	bpb_rsvdSecCnt;		/* reserved sectors, starting at 0 */
	unsigned int	bpb_FATSz;			/* 由FATSz16或FATSz32 而来，number of sectors per FAT */
	char			bpb_secPerClus;		/* sectors per allocation unit */
	char			bpb_numFATs;		/* number of FATs */
	unsigned short	bpb_rootEntCnt;		/* 仅适用于FAT16/12，number of root-directory entries */
	
	/* 存储介质的某些关键信息，从bpb信息中导出的 */
	unsigned int	fsfat_start;		/* 第一个FAT表的起始地址，以字节为单位 */
	unsigned int	fsrootDir;			/* 对于FAT16/12，为根目录区的起始地址，以字节为单位 */
										/* 对于FAT32，为根目录起始的簇号	*/
	unsigned int	fsfile_start;		/* 文件（数据）区的起始地址，以字节为单位的地址 */
	unsigned int	fscluster_size;		/* 一个簇的字节数 */
	unsigned int	number_of_clusters;	/* 数据区的cluster数目，即可用来存储数据的簇数 */
	unsigned int volatile free_cluster;	/* lst. free cluster */
	
	int				fatType;			/* FAT type： 0-FAT32，1-FAT16，2-FAT12 */
	


	/* 以下为FAT表的相关的缓存*/									
	char    	fatPool[FAT_POOL_SIZE];	/*	当进行过写操作时，pfatcache_wr 指向fatPool，
											这时，fatPool的数据存储规则是pfatcache_wr规定的 
											当没有进行过写操作时，fatPool存的是磁盘上的fat区
											的局部的数据 */
	
	unsigned int	*pfatcache_wr;		/*	注意FAT表的cache中存的是一个文件的FAT表的一对一对的数据，
											例如某个文件占用簇3，5，6
											则在cache中存的就是(3，5)，(5，6)，(6，0x0fffffff)  */
											
	unsigned		fatsnapstart;		/*	当没有进行过写操作，在fat缓存区放的是fat数据的局部，
											这个全局变量表示fat区局部的开始 */
 
	unsigned		fat_change_start;
	unsigned		fat_change_end;		/* 	当文件系统有两个fat表时，在关闭文件系统时，需将第一个fat读出，存到第二个fat中
											这两个全局的变量表示需要存的局部fat的开始和结束*/ 
	
	/* 20061016 如果要支持双FAT表的操作，则需要定义以下的域，但是现在还没有支持 */
	//int  fat_2_is_present;			/*第二个FAT表的标志*/
 	//unsigned long  fsfat2_start;		/* start of second FAT adrress*/
} FILE_SYS;

/*	FILE_SYS结构中fatType的取值 */
#define	FATTYPE_FAT32		0	
#define	FATTYPE_FAT16		1	
#define	FATTYPE_FAT12		2	
#define	FATTYPE_NUM			3	


/*	目录操作项， */
typedef struct
{
	unsigned long long disk_addr;		/* 目录项的地址，是从绝对0地址开始计算的地址,如果为0，表示根目录 */
											
	FILE_SYS		*pfs;				/* 所使用的文件系统 */
	DIRECTORY		dir;				/* 目录项的拷贝，即32个字节的copy */
	unsigned 		eof_cluster;		/* end of file cluster */
} FSDIR;

/*	当前工作目录操作项， Current Working Directory Structure，
	对于每个进程有一个这个结构变量 
	注意这个结构的前3个变量和FSDIR结构是一样的*/
typedef struct
{
	unsigned		disk_addr;			/* 目录项的地址，是从绝对0地址开始计算的地址 */
	FILE_SYS		*pfs;				/* 所使用的文件系统 */
	DIRECTORY		dir;				/* 目录项的拷贝，即32个字节的copy */
	PATH			path;				/* pathname of directory */
	void			(*pexit)();			/* onexit linkage */
} CWD;

/*  打开的文件结构体，Open File Structure, 
	对于每个打开的文件,有一个这个变量 */
typedef struct
{
	FSDIR			ofsdir;				/* 文件的目录项信息 */
	PATH			ofpath;				/* complete pathname */
	short			ofopens;			/* 打开了几次 */
	short			offlags;			/* 标记，具体含义见下面的定义，主要标记目录项内容或文件内容是否
										   被改了 */
	unsigned 		ofcloffset;			/* 当前正在读写的 cluster offset，从该文件的第一个cluster算起 
											表示用到多少cluster，表示了在文件中的相对读写位置*/
	unsigned 		ofclustno;			/* 当前正在读写的绝对 cluster no. 表示当前的在磁盘上的读写位置 */
} OFILE;


/*  OFILE 结构体中 offlags bits 的定义 */
#define F_MOD		1				/* directory was modified */
#define F_BUSY		2				/* some process has exclusive use */
#define F_ERROR		4				/* I/O error occurred */
#define F_FMOD		8				/* file was modified */


#if 0  // 以下为田提出的参考 20061016
//在定义文件的时候,定义FAT32/16/12; ????
//PC的操作文件的定义
#ifndef _FILE_DEFINED
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;
#endif
#endif

/* windows 白皮书规定的特殊的 Cluster 值*/
#define EOF_CLUSTER		0x0FFFFFFF
#define FREE_CLUSTER	0x00000000
#define BAD_CLUSTER		0x0FFFFFF7
#define IS_EOF(c)		((unsigned)(c) >= 0x0FFFFFF8)
#define IS_FREE(c)		((unsigned)(c) == FREE_CLUSTER)
#define IS_BAD(c)		((unsigned)(c) >= 0xFFF0 && !IS_EOF(c))


/*	function prototypes */
int		_chkfs(char **ppath);
//int		 _closfile(FILE_SYS *pfs);
short	_dosdate(void);
short	_dostime(void);
OFILE	*hyfile_findOpenfileHandler(FILE_SYS *pfs);
//void	_lockfs(FILE_SYS *pfs);
//void	_ulockfs(FILE_SYS *pfs);
//void	_unopen(FILE_SYS *pfs);


/* 外部变量  */
extern CWD *_cwd;		/* process current working directory */



#endif /*_DOSFILH */
