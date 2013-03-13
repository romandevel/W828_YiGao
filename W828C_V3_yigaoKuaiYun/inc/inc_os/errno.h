/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*******************************************************************************/

/******************************************************************************
* 文件:	errno.h 
* 作者:	ZhouJie 
* 创建:	2005 12 05 15:52
* 说明:	os定义的错误号
* 相关：sysconf.h, 错误定义
*		hyos_gdata.c	全局变量errno
* 平台:	ARM 32位，小端格式
*****************************************************************************/

#ifndef _ERRNO_H_
#define _ERRNO_H_


#ifndef _SYSCONF
#include <sysconf.h>
#endif

/* HYOS error codes */

#define E_LCKED _E_LCKED	/* resource locked */
#define E_NLCKED _E_NLCKED	/* resource not locked */
#define E_BADFD _E_BADFD	/* bad file ID */
#define E_NOCWD _E_NOCWD	/* no current working directory */
#define E_RDWR _E_RDWR		/* write to read only or read from write only */
#define E_NOFD _E_NOFD		/* no avail fds, too many open files */
#define E_NODEV _E_NODEV	/* unknown device name */
#define E_INVMD _E_INVMD	/* invalid mode */
#define E_INVPR _E_INVPR	/* invalid process id or name */
#define E_ACTIV _E_ACTIV	/* process already active */
#define E_CONNECT _E_CONNECT/* serial line connection failure */
#define E_PARITY _E_PARITY	/* serial line parity error */
#define E_BADIO _E_BADIO	/* I/O operaton is illegal for device */
#define E_IOERR _E_IOERR	/* I/O error */
#define E_SEEK _E_SEEK		/* invalid seek pointer value */
#define E_SYNTAX _E_SYNTAX	/* bad file name syntax */
#define E_NOFS _E_NOFS		/* too many open file systems */
#define E_NOFILE _E_NOFILE	/* too many open files for file system */
#define E_DIRFUL _E_DIRFUL	/* file system directory full */
#define E_NOROOM _E_NOROOM	/* file system full (no data space) */
#define E_EXIST _E_EXIST	/* file does not exist */
#define E_LENGTH _E_LENGTH	/* file name too long */
#define E_BUSY _E_BUSY		/* device or file system is busy */
#define E_ISOPEN _E_ISOPEN	/* file to be created, removed or renamed is open */
#define E_BADCODE _E_BADCODE/* invalid file system superblock code */
#define E_INVDIR _E_INVDIR	/* invalid directory */
#define E_TMOUT _E_TMOUT	/* read timeout */
#define E_DIR _E_DIR		/* attempt to rm/rn/create file system directory */
#define E_ACCESS _E_ACCESS	/* non-cached disk access not on block boundary */
 							/* or not integral number of blocks */
#define E_OPTION _E_OPTION	/* option not present */
#define E_BADVAL _E_BADVAL	/* illegal parameter value */
#define E_OPENDV _E_OPENDV	/* illegal use of opendv (file system file) */
#define E_NOWRT _E_NOWRT	/* file can not be opened for write */
#define E_NOMEM _E_NOMEM	/* can not allocate memory */
#define E_AGAIN	_E_AGAIN	/* resource temporarily unavailable */
#define E_INTR _E_INTR		/* operation interrupted by a signal */
#define E_INVCONF _E_INVCONF	/* invalid configuration for dynamic system */
#define E_THREAD _E_THREAD	/* operation can not be done on or by thread */

/* 全局变量，在hyos_gdata.c中定义 */
extern int errno;


#endif	/* _ERRNO_H_ */
