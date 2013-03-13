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
#ifndef _SYSCONF
#define _SYSCONF


/*	HYOS版本 */
#define _SYSNAME	"HYOS1.0"

/*	HYOS 错误码 */
#define _ERR_NONE				(0x0)	/* success */

#define _ERR_RES_LOCKED			(-0x1)	/* resource locked */
#define _ERR_RES_NLOCKED		(-0x2)	/* resource not locked */
#define _ERR_BAD_FD				(-0x3)	/* bad file ID */
#define _ERR_NOCWD				(-0x4)	/* no current working directory */
#define _ERR_RDWR				(-0x5)	/* write to read only or read from write only file */
#define _ERR_NOFD				(-0x6)	/* no avail fds, too many open files */
#define _ERR_NODEV				(-0x7)	/* unknown device name */
#define _ERR_INV_MODE			(-0x8)	/* invalid mode */
#define _ERR_INV_PROC			(-0x9)	/* invalid process id or name */
#define _ERR_ACTIV				(-0xA)	/* process already active */
#define _ERR_CONNECT			(-0xB)	/* serial line connection failure */
#define _ERR_PARITY				(-0xC)	/* serial line parity error */
#define _ERR_BADIO				(-0xD)	/* I/O operaton is illegal for device */
#define _ERR_IOERR				(-0xE)	/* I/O error */
#define _ERR_INV_SEEK			(-0xF)	/* invalid seek pointer value */
#define _ERR_SYNTAX				(-0x10)	/* bad file name syntax */
#define _ERR_NOFS				(-0x11)	/* too many open file systems */
#define _ERR_NOFILE				(-0x12)	/* too many open files for file system */
#define _ERR_DIRFUL				(-0x13)	/* file system directory full */
#define _ERR_NOROOM				(-0x14)	/* file system full (no data space) */
#define _ERR_NOTEXIST			(-0x15)	/* file does not exist */
#define _ERR_LENGTH				(-0x16)	/* file name too long */
#define _ERR_BUSY				(-0x17)	/* device or file system is busy */
#define _ERR_ISOPEN				(-0x18)	/* file to be created, removed or renamed is open */
#define _ERR_BADCODE			(-0x19)	/* invalid file system superblock code */
#define _ERR_INVDIR				(-0x1A)	/* invalid directory */
#define _ERR_TMOUT				(-0x1B)	/* read timeout */
#define _ERR_DIR				(-0x1C)	/* attempt to rm/rn/create file system directory */
#define _ERR_ACCESS				(-0x1D)	/* non-cached disk access not on block boundary */
										/* or not integral number of blocks */
#define _ERR_OPTION				(-0x1E)	/* option not present */
#define _ERR_BAD_PARAM			(-0x1F)	/* illegal parameter value */
#define _ERR_OPENDV				(-0x20)	/* illegal use of opendv (file system file) */
#define _ERR_NOWRT				(-0x21)	/* file can not be opened for write */
#define _ERR_NO_MEMORY			(-0x22)	/* can not allocate memory */
#define _ERR_AGAIN				(-0x23)	/* resource temporarily unavailable */
#define _ERR_INTR_BY_SIG		(-0x24)	/* function was interrupted by signal */
#define _ERR_INVCONF			(-0x25)	/* invalid configuration for system call */
#define _ERR_TOOBIG				(-0x26)	/* executable too big */
#define _ERR_THREAD				(-0x27)	/* operation can not be done on or by thread */
#define _ERR_NOCHILD			(-0x28)	/* no child process (NOSYS) */
#define _ERR_DEADLOCK			(-0x29)	/* fcntl SETLKW would cause a deadlock */
#define _ERR_EXIST				(-0x2A)	/* file already exists */
#define _ERR_FAULT				(-0x2B)	/* invalid address detected (NOSYS) */
#define _ERR_FILEBIG			(-0x2C)	/* attempt to write to file too big */

#define _ERR_MLINK				(-0x2E)	/* links would exceed LINK_MAX (NOSYS) */
										/* 空 */
#define _ERR_NOEXEC				(-0x30)	/* execute file in bad format (NOSYS) */
#define _ERR_NOLOCK				(-0x31)	/* no file locks available from fcntl */
#define _ERR_NOMEM				(-0x32)	/* no memory for exec (NOSYS) */
#define _ERR_NOSYS				(-0x33)	/* not implemented */
										/* 空 */
#define _ERR_NOTEMPTY			(-0x35)	/* rename or remove non-empty director */
#define _ERR_NOTTY				(-0x36)	/* terminal control function invalid on device */
#define _ERR_PERM				(-0x37)	/* process has insufficient privilege (NOSYS) */
#define _ERR_PIPE				(-0x38)	/* attempt to write pipe/fifo with no reader */
#define _ERR_ROFS				(-0x39)	/* read only filesystem (NOSYS) */
#define _ERR_SPIPE				(-0x3A)	/* lseek invalid on pipe/fifo */
#define _ERR_XDEV				(-0x3B)	/* cant link across filesystems (NOSYS) */
#define _ERR_DOM				(-0x3C)	/* domain error */
#define _ERR_RANGE				(-0x3D)	/* range error */
#define _ERR_FPOS				(-0x3E)	/* file position error */
#define	_ERR_TIMEDOUT			(-0x3F)	/* time out */


/*	内存分配错误的一组错误码 */
#define	_ERR_MEM_NOT_WITHIN_HEAP	(-0x40)
#define	_ERR_MEM_OVERLAP			(-0x41)
#define	_ERR_MEM_BAD_FREE_CALL		(-0x42)
#define	_ERR_MEM_BLOCK_NOT_OWNED	(-0x43)


/* POSIX 错误码 */
#define _E2BIG		_ERR_TOOBIG		/* executable too big */
#define _EACCES		_ERR_DIR		/* no permission on directory */
#define _EAGAIN		_ERR_AGAIN		/* either O_NONBLOCK is set for I/O or system */
									/* temporarily did not have resources */
#define _EBADF		_ERR_BADFD
#define _EBUSY		_ERR_ISOPEN
#define _ECHILD		_ERR_NOCHILD	/* no child process (NOSYS) */
#define _EDEADLK	_ERR_DEADLOCK	/* fcntl SETLKW would cause a deadlock */
#define _EEXIST		_ERR_EXIST		/* file already exists */
#define _EFAULT		_ERR_FAULT		/* invalid address detected (NOSYS) */
#define _EFBIG		_ERR_FILEBIG	/* attempt to write to file too big */
#define _EINTR		_ERR_INTR		/* function was interrupted by signal */
#define _EINVAL		_ERR_BADVAL 	/* invalid argument */
#define _EIO		_ERR_IOERR 		/* I/O error */
#define _EISDIR		_ERR_NOWRT 		/* can't write or rename directory */
#define _EMFILE		_ERR_NOFD		/* too many FDs for process */
#define _EMLINK		_ERR_MLINK		/* links would exceed LINK_MAX (NOSYS) */
#define _ENAMETOOLONG	_ERR_LENGTH	/* filename exceeds PATH_MAX and */
									/* _POSIX_NO_TRUNC is in effect */
#define _ENFILE		_ERR_NOFILE		/* too many files open in system (NOSYS) */
#define _ENODEV		_ERR_BADIO 		/* I/O operation is inappropriate on device */
#define _ENOENT		_ERR_EXIST 		/* file does not exist */
#define _ENOEXEC	_ERR_NOEXEC		/* execute file in bad format (NOSYS) */
#define _ENOLCK		_ERR_NOLOCK		/* no file locks available from fcntl */
#define _ENOMEM		_ERR_NOMEM		/* no memory for exec (NOSYS) */
#define _ENOSPC		_ERR_NOROOM		/* no space on disk */
#define _ENOSYS		_ERR_NOSYS		/* not implemented */
#define _ENOTDIR	_ERR_INVDIR		/* component of name is not a directory */
#define _ENOTEMPTY	_ERR_NOTEMPTY	/* rename or remove non-empty director */
#define _ENOTTY		_ERR_NOTTY		/* terminal control function invalid on device */
#define _ENXIO		_ERR_NODEV		/* no such device, or off(-)line */
#define _EPERM		_ERR_PERM		/* process has insufficient privilege (NOSYS) */
#define _EPIPE		_ERR_PIPE		/* attempt to write pipe/fifo with no reader */
#define _EROFS		_ERR_ROFS		/* read only filesystem (NOSYS) */
#define _ESPIPE		_ERR_SPIPE		/* lseek invalid on pipe/fifo */
#define _ESRCH		_ERR_INVPR		/* no such process, kill() and setpgid() */
#define _EXDEV		_ERR_XDEV		/* cant link across filesystems (NOSYS) */
#define _EDOM		_ERR_DOM		/* domain error */
#define _ERANGE		_ERR_RANGE		/* range error */
#define _EFPOS		_ERR_FPOS		/* file position error */
#define	_ETIMEDOUT	_ERR_TIMEDOUT	/* time out */

/* _ERRMAX	== 64  */



/* 系统启动过程中的异常码 */
#define AB_KNOMEM	0x1		/* kernel memory allocation error */
#define AB_BADDEV	0x2		/* stdin/stdout/stderr incorrect */
#define AB_MAXPID	0x3		/* max pids insufficient */
#define AB_MAXPRI	0x4		/* max priority insufficient */
#define AB_NOTIM	0x5		/* = priority processes, timeslicing not configured */
#define AB_TNOMEM	0x6		/* tty driver memory allocation error */
#define AB_QNOMEM	0x7		/* queue driver memory allocation error */
#define AB_MNOMEM	0x8		/* memory driver memory allocation error */
#define AB_FLNOMEM	0x9		/* floppy disk driver memory allocation error */
#define AB_FXNOMEM	0xA		/* fixed disk driver memory allocation error */
#define AB_TTYDEV	0xB		/* tty minor device number out of range */
#define AB_MEMDEV	0xC		/* memory unit number out of range */
#define AB_FXDEV	0xD		/* fixed disk controller number out of range */
#define AB_FLDEV	0xE		/* floppy disk drive number out of range */
#define AB_ETHDEV	0xF		/* Ethernet unit number out of range */
#define AB_DOSNOMEM	0x10	/* CE-DOSFILE driver memory allocation error */
#define AB_MEMREG	0x11	/* allocated memory region exceeds 64K  (8086/80186) */
#define AB_IRQVAL	0x12	/* IRQ exceeds configured IRQ's (80386, PowerPC) */
#define AB_INVCONF	0x13	/* invalid configuration, (no processes, or timeslicing value but timeslicing stub is linked) */
						 	/* timeslicing value but timeslicing stub is linked */
#define AB_INVINT	0x14	/* invalid interrupt */



/* 全局变量在hyos_gdata.c中定义 */
extern char	_abort_code;		/* system start-up abort code */


/*  信号定义，现在仅仅支持1个信号，用来杀死一个进程 */
#define _SIGKILL	1
#define _SIGMAX		2		/* one more than maximum signal number */

/*  process return values */
#define _EXSUCCESS	0
#define _EXFAIL		1


#endif