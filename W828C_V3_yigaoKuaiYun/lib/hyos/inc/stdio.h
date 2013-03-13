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

#ifndef _STDIO
#define _STDIO


#include "hyTypes.h"
#include "hyosdef.h"
#include "string.h"


#ifndef _ANSI_LIBRARY
/* the pseudo types*/

#ifndef _CLOCKT
#define _CLOCKT
typedef unsigned long clock_t;
#endif
#ifndef _PIDT
#define _PIDT
typedef short pid_t;
#endif
#ifndef _MODET
#define _MODET
typedef unsigned long mode_t;	/* used for file mode attributes */
#endif



/*	misc definitions
 */

/* the file IO structure
	_pnext points to next char to read in _buf if _fmode is READ.
	otherwise, _pnext is not used.
 */
 #if 0
typedef struct fio
	{
	struct fio	*volatile _nxfio;
	short		volatile _fd;
	short		volatile _fmode;
	short		volatile _flags;
	short		volatile _nleft;
	char		*volatile _pnext;
	char		volatile _buf[BUFSIZ];
	} FIO;

/*	I/O parameter definitions
 */
#define STDIN	0
#define STDOUT	1
#define STDERR	2
#define READ	0
#define WRITE	1
#define UPDATE	2
#define CONTIG	4
#define CREATE	8
#define BWRITE	(-1)
#define EOF		(-1)
#define O_RDONLY READ
#define O_WRONLY WRITE
#define O_RDWR UPDATE
#define O_CREAT CREATE
#define O_CONTIG CONTIG
#define O_REMOVE 0x200

#define FILE struct fio
#define stdin _stdin()
#define stdout _stdout()
#define stderr _stderr()
#define fopen _ufopen
#define getc(p) ((p)->_nleft > 0 ? ((p)->_nleft--, *(p)->_pnext++ & 0377) : fgetc(p))
#define putc(c,p) (((p)->_nleft < (BUFSIZE-1)) ? (p)->_buf[(p)->_nleft++] = (c) : fputc(c,p))
#define getchar() fgetc(_stdin())
#define putchar(c) fputc(c, _stdout())
#endif

/*	macro definitions */

#define exit(x)		_exit(x)
#define sleep(x)	syssleep((unsigned)x)
#define time(x)		rawtime(x)
#define wait(x)		ewait(x)
#define wakeup(x)	ewakeup(x)
#else
/* definitions for ANSI library here
 */
#endif

/* os error codes */
#define E_LCKED		_E_LCKED	/* resource locked */
#define E_NLCKED	_E_NLCKED	/* resource not locked */
#define E_BADFD		_E_BADFD	/* bad file ID */
#define E_NOCWD		_E_NOCWD	/* no current working directory */
#define E_RDWR		_E_RDWR		/* write to read only or read from write only */
#define E_NOFD		_E_NOFD		/* no avail fds, too many open files */
#define E_NODEV		_E_NODEV	/* unknown device name */
#define E_INVMD		_E_INVMD	/* invalid mode */
#define E_INVPR		_E_INVPR	/* invalid process id or name */
#define E_ACTIV		_E_ACTIV	/* process already active */
#define E_CONNECT	_E_CONNECT	/* serial line connection failure */
#define E_PARITY	_E_PARITY	/* serial line parity error */
#define E_BADIO		_E_BADIO	/* I/O operaton is illegal for device */
#define E_IOERR		_E_IOERR	/* I/O error */
#define E_SEEK		_E_SEEK		/* invalid seek pointer value */
#define E_SYNTAX	_E_SYNTAX	/* bad file name syntax */
#define E_NOFS		_E_NOFS		/* too many open file systems */
#define E_NOFILE	_E_NOFILE	/* too many open files for file system */
#define E_DIRFUL	_E_DIRFUL	/* file system directory full */
#define E_NOROOM	_E_NOROOM	/* file system full (no data space) */
#define E_EXIST		_E_EXIST	/* file does not exist */
#define E_LENGTH	_E_LENGTH	/* file name too long */
#define E_BUSY		_E_BUSY		/* device or file system is busy */
#define E_ISOPEN	_E_ISOPEN	/* file to be created or removed already open */
#define E_BADCODE	_E_BADCODE	/* invalid file system superblock code */
#define E_INVDIR	_E_INVDIR	/* invalid directory */
#define E_TMOUT		_E_TMOUT	/* read timeout */
#define E_DIR		_E_DIR		/* attempt remove/creat file system directory */
#define E_ACCESS	_E_ACCESS	/* non-cached disk access not on block boundary
								 * or not integral number of blocks */
#define E_OPTION	_E_OPTION	/* option not present */
#define E_BADVAL	_E_BADVAL	/* illegal parameter value */
#define E_OPENDV	_E_OPENDV	/* illegal use of opendv (file system file) */
#define E_NOWRT		_E_NOWRT	/* file can not be opened for write */
#define E_NOMEM		_E_NOMEM	/* can not allocate memory */
#define E_AGAIN		_E_AGAIN	/* resource temporarily unavailable */
#define E_INTR		_E_INTR		/* interrupted by a signal */
#define E_INVCONF	_E_INVCONF	/* invalid configuration for dynamic system */
#define E_THREAD	_E_THREAD	/* operation can not be done on or by thread */

#define _ERRNO(e)	((e) < 0 ? (errno = -(e),(e)) : (e))	/* error return */
#define _PSX_ERRNO(e)	((e) < 0 ? (errno = -(e),(e)) : (e)) /* error return */

/* if more errors are added, update errtxt.c also */

/* data declarations */
extern int		errno;
extern unsigned _ioff;
extern unsigned _clkpri;
extern clock_t	_clk_tck;
extern pid_t	_maxpids;


/* function prototype definitions */
void		_exit(int status);
short		chgepri(short priority);
short		chgpri(pid_t pid, short priority);
int			chkstk(void);
int			close(int fd);
pid_t		createp(int (*entry)(), char *name, unsigned scnt, short priority,
					char *sin, char *sout, char *serr);
pid_t		cycle(pid_t pid, short period, short due, void *param);
void		delay(void);
int			deletep(pid_t pid);
int			devcnt(int fd);

void		entreg(void);
void		entsig(void);
int			ewait(void **list);
int			ewaittm(register void **list, unsigned tout);
void		ewakeup(void **list);
short		getepri(void);
short 		getpid(void);
int 		gevent(void);

short		lock(unsigned res);
void 		lvreg(void);
void 		lvsig(void);
int mkdir(char *dname);

pid_t ntopid(char *us);
void (*onexit(void (*(*func)())()))();

char *pidton(pid_t pid);
clock_t profile(clock_t *buf);
int pstart(pid_t pid, void *param);
char *sbreak(unsigned n);
void sevent(void);
void sync(void);
short unlock(unsigned res);
short wlock(unsigned res);
short wlocktm(unsigned res, unsigned tout);
unsigned setil(unsigned psw);

int 		chdir(char *dname);
unsigned 	cpybuf(char *t, char *s, unsigned n);
int 		creat(char *name, mode_t mode);
int 		create(char *name, int mode, ...);
char 		*getcwd(char *buf, int n);
long 		lseek(int fd, long offset, int mode);
int 		open(char *name, int mode, ...);
clock_t 	rawtime(clock_t *t);
int 		read(int fd, char *buf, int size);

int 		remove(char *name);
int 		rename(char *s, CONST char *t);
int 		rmdir(char *dname);
unsigned 	syssleep(unsigned n);
int 		write(int fd, char *buf, int size);


/* Portable library */
int atoi(const char *s);
long atol(const char *s);
void error(char *s1, char *s2);

char		*free(char *p);

void itoa(int n, char *s);
int lenstr(char *s);
char *malloc(unsigned size);
void printf(char *fmt, ...);
void putstr(int file, ...);
void remark(char *s1, char *s2);

#endif
