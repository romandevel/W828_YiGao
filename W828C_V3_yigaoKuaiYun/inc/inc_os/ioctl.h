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
* 文件:	ioctl.h 
* 作者:	ZhouJie 
* 创建:	2005 12 05 15:52
* 说明:	os支持的io类型定义
* 相关：无
* 平台:	ARM 32位，小端格式
*****************************************************************************/

#ifndef _IOCTL
#define _IOCTL


#define BLKSIZ		1	/* get disk block size as function return value */
#define TABLE		2	/* get tty or queue table address (CE-VIEW only) */
						/* buf paramter points to where pointer is returned */
#define DEVTYP		3	/* obtain device type as function return value */
#define FORMAT		4	/* format disk media */
#define TIME_OUT	5	/* set TTY or queue read timeout */
#define CAPACITY	6	/* get disk or contiguous file capacity in blocks. */
						/* buf points to a long. */
#define FILE_SIZE	7	/* get current size of file (eof value) */
						/* buf points to a long. */
#define FLUSH		8	/* flush a queue or a terminal input buffer */
#define MIN_READ	9	/* set minimim number of TTY messages to read */
#define MSGDEV		10	/* establish this TTY as the system message device */
#define NERRS		11	/* get device error count as function value */
#define L_ORDER		12	/* convert a long between file subsystem order */
						/* and host order.  buf is (long *) */
#define DK_PARAMS	13	/* get disk parameters */
#define CRT_CLR		14	/* clear crt screen */
#define CRT_CUR		15	/* position crt cursor */
#define S_ORDER		16	/* convert a short between file subsystem order */
						/* and host order.  buf is (long *) */
						
#define	SET_FILE_ATTRIBUTE	17	
#define UPDATE_FAT2			18							

#define DEVPOLL		20	/* poll device */
#define EXCEPTION	21	/* exception indicator for select */
#define ASSIGN_SOCK	22	/* assign a socket */	
#define FREE_SOCK	23	/* free a socket */
#define CARD_DETECT 29  /* detect if physical memory card is present */
#define READ_ID     30  /* read unique memory card ID */
#define CARD_INIT   31  /* initialize card driver parameters */
#define CARD_SIZE   32  /* gives card size */


/* NOTE: ioctl function values of 29-49 are reserved for future use */


/*	Definition of values returned by the DEVTYP ioctl call. */
#define IS_TTY		1
#define IS_QUE		2
#define IS_DISK		3
#define IS_FILE		4
#define IS_NULL		5
#define IS_SOCKET	6
#define IS_FIFO		7

/* function prototypes*/
int ioctl(int fd, int func, ...);

#endif	/*_IOCTL */
