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
* 文件:	device_cfg.h 
* 作者:	ZhouJie 
* 创建:	2005 12 05 15:52
* 说明:	os支持的设备的定义，包括
*		1。某些设备的初始化数据的结构的定义
*		2。设备配置表的定义
* 相关：dstat.h, 设备状态
* 平台:	ARM 32位，小端格式
*****************************************************************************/

#ifndef _DEVICE_CFG_H_
#define _DEVICE_CFG_H_


/*	FILE SYSTEM DEVICE INITIALIZATION STRUCTURE 仅在DVCFG的初始化中用到 */
typedef struct filesi
{
	short	max_open;		/* max number of files open at once */
	short	max_fs;			/* max number of file systems accessed at once */
} FILESI;


/*	DEVICE CONFIGURATION TABLE 设备配置表
	对于操作系统来说，设备是多种的
	例如有文件系统，fifo，memory，存储介质等等*/
typedef struct dvcfg
{
	char	*_name;				/* name，设备的名字 */
	int		(*_driver)();		/* driver entry point，设备的driver 
								   例如
								   设备的名字是"dvnul", 驱动函数为 nuldrv()
								   设备的名字是"dosfile", 驱动函数为 dosfile()
								   设备的名字是"C", 驱动函数为 hyosVolumeDrv_Entry*/
	short	_dminor;			/* minor no. 
								   如果不同设备的驱动函数一样，表明是第几个这样的设备
								   例如，"C"，"D"，"E"，"F" 设备的驱动函数都是hyosVolumeDrv_Entry
								   则它们的_dminor 依次为0，1，2，3 */
	union
	{
		void *pnuli;
		
		FILESI *pfilesi;
		
	} _init;					/* pointer to initialization structure 
								   有些设备初始化时需要一些初始化的数据
								   例如文件系统，需要最多打开的文件数和最多支持的文件系统数*/
	
} DVCFG;

/* function prototypes	*/
DVCFG *_n2cfg(char *);


/* 外部全局变量，在hyos_gdata.c中定义 */
extern DVCFG *_pdvtab;	


#endif	/* _DEVICE_CFG_H_ */
