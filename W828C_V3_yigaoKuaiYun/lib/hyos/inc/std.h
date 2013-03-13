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

#ifndef _STDH
#define _STDH

#include "hyTypes.h"
#include "hyosdef.h"
#include "string.h"



/* the pseudo types */

#ifndef _CLOCKT
#define _CLOCKT
typedef unsigned long	clock_t;
typedef unsigned int	time_t;
#endif

#ifndef _PIDT
#define _PIDT
typedef short pid_t;
#endif

#ifndef _MODET
#define _MODET
typedef unsigned long mode_t;	/* used for file mode attributes */
#endif


/*	其他的一些定义 */
//#define NUL		'\0'
//#define FOREVER	for (;;)
//#define BUFSIZE	512
//#define BUFSIZ	512


#define CLOCKS_PER_SEC	_clk_tck
#define TMUNIT_TO_SEC(n) ((n * _tcks_unit) / CLOCKS_PER_SEC)
#define SEC_TO_TMUNIT(n) ((n * CLOCKS_PER_SEC) / _tcks_unit)

/*	I/O parameter definitions */
#define STDIN		0
#define STDOUT		1
#define STDERR		2
#define READ		0
#define WRITE		1
#define UPDATE		2
#define CONTIG		4
#define CREATE		8
#define BWRITE		(-1)
#define EOF			(-1)
#define O_RDONLY 	READ
#define O_WRONLY 	WRITE
#define O_RDWR		UPDATE
#define O_CONTIG	CONTIG		/* 创建的文件必须是连续的簇 */
#define O_CREAT		CREATE
#define O_REMOVE	0x200

/*	macro definitions */
#define exit(x)		_exit(x)
#define sleep(x)	syssleep((unsigned)x)
#define time(x)		rawtime(x)
#define wait(x)		ewait(x)
#define wakeup(x)	ewakeup(x)
#define gtc(p) ((p)->_nleft > 0 ? ((p)->_nleft--, *(p)->_pnext++ & 0377) : getc(p))
#define ptc(p,c) (((p)->_nleft < (BUFSIZE-1)) ? (p)->_buf[(p)->_nleft++] = (c) : putc(p,c))

/* error codes */

#define E_LCKED		_ERR_RES_LOCKED		/* -0x1		resource locked */
#define E_NLCKED	_ERR_RES_NLOCKED	/* -0x2		resource not locked */
#define E_BADFD		_ERR_BAD_FD			/* -0x3		bad file ID */
#define E_NOCWD		_ERR_NOCWD			/* -0x4		no current working directory */
#define E_RDWR		_ERR_RDWR			/* -0x5		write to read only or read from write only */
#define E_NOFD		_ERR_NOFD			/* -0x6		no avail fds, too many open files */
#define E_NODEV		_ERR_NODEV			/* -0x7		unknown device name */
#define E_INVMD		_ERR_INV_MODE		/* -0x8		invalid mode */
#define E_INVPR		_ERR_INV_PROC		/* -0x9		invalid process id or name */
#define E_ACTIV		_ERR_ACTIV			/* -0xA		process already active */
#define E_CONNECT	_ERR_CONNECT		/* -0xB		serial line connection failure */
#define E_PARITY	_ERR_PARITY			/* -0xC		serial line parity error */
#define E_BADIO		_ERR_BADIO			/* -0xD		I/O operaton is illegal for device */
#define E_IOERR		_ERR_IOERR			/* -0xE		I/O error */
#define E_SEEK		_ERR_INV_SEEK		/* -0xF		invalid seek pointer value */
#define E_SYNTAX	_ERR_SYNTAX			/* -0x10	bad file name syntax */
#define E_NOFS		_ERR_NOFS			/* -0x11	too many open file systems */
#define E_NOFILE	_ERR_NOFILE			/* -0x12	too many open files for file system */
#define E_DIRFUL	_ERR_DIRFUL			/* -0x13	file system directory full */
#define E_NOROOM	_ERR_NOROOM			/* -0x14	file system full (no data space) */
#define E_EXIST		_ERR_EXIST			/* -0x15	file does not exist */
#define E_LENGTH	_ERR_LENGTH			/* -0x16	file name too long */
#define E_BUSY		_ERR_BUSY			/* -0x17	device or file system is busy */
#define E_ISOPEN	_ERR_ISOPEN			/* -0x18	file to be created or removed already open */
#define E_BADCODE	_ERR_BADCODE		/* -0x19	invalid file system superblock code */
#define E_INVDIR	_ERR_INVDIR			/* -0x1A	invalid directory */
#define E_TMOUT		_ERR_TMOUT			/* -0x1B	read timeout */
#define E_DIR		_ERR_DIR			/* -0x1C	attempt remove/creat file system directory */
#define E_ACCESS	_ERR_ACCESS			/* -0x1D	non-cached disk access not on block boundary
										 * or not integral number of blocks */
#define E_OPTION	_ERR_OPTION			/* -0x1E	option not present */
#define E_BADVAL	_ERR_BAD_PARAM		/* -0x1F	illegal parameter value */
#define E_OPENDV	_ERR_OPENDV			/* -0x20	illegal use of opendv (file system file) */
#define E_NOWRT		_ERR_NOWRT			/* -0x21	file can not be opened for write */
#define E_NOMEM		_ERR_NOMEM			/* -0x22	can not allocate memory */
#define E_AGAIN		_ERR_AGAIN			/* -0x23	resource temporarily unavailable */
#define E_INTR		_ERR_INTR_BY_SIG	/* -0x24	interrupted by a signal */
#define E_INVCONF	_ERR_INVCONF		/* -0x25	invalid configuration for dynamic system */
#define E_THREAD	_ERR_THREAD			/* -0x26	operation can not be done on or by thread */

#define _ERRNO(e)	((e) < 0 ? (errno = -(e),(e)) : (e))	/* error return */
#define _PSX_ERRNO(e)	((e) < 0 ? (errno = -(e),(e)) : (e)) /* error return */

/* if more errors are added, update errtxt.c also */

/* 全局变量，在hyos_gdata.c 中定义*/
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
short		getpid(void);
int			gevent(void);

short		lock(unsigned res);
void		lvreg(void);
void		lvsig(void);

pid_t		ntopid(char *us);
void		(*onexit(void (*(*func)())()))();

char		*pidton(pid_t pid);
clock_t		profile(clock_t *buf);

int			pstart(pid_t pid, void *param);
char		*sbreak(unsigned n);

void		sevent(void);
short		unlock(unsigned res);
short		wlock(unsigned res);
short		wlocktm(unsigned res, unsigned tout);

int			creat(char *name, mode_t mode);
int			create(char *name, int oflags, ...);
long		lseek(int fd, long offset, int mode);
int			open(char *name, int mode, ...);
clock_t		rawtime(clock_t *t);
int			read(int fd, char *buf, int size);

int			remove(char *name);
unsigned	syssleep(unsigned n);
int			write(int fd, char *buf, int size);

int			ioctl(int fd, int func, ...);


int			gdevno(int fd);
int			devmsg(int fd);
unsigned	intoff(void);
void		intrst(unsigned psw);
int			mkdir(char *dname);
void		msgfmt(char *fmt, ...);
pid_t		ncycle(char *s, short period, short due, void *param);
pid_t		nstart(char *s, void *p);
int			opendv(int devno, int mode);
void		prterr(int n);
unsigned	setil(unsigned psw);
void		sync(void);

int			chdir(char *dname);
unsigned	cpybuf(char *t, char *s, unsigned n);
char		*getcwd(char *buf, int n);
int			readtm(int fd, char *buf, int size, unsigned tout);
int			rename(char *s, CONST char *t);
int			rmdir(char *dname);
int			writeqhd(int fd, char *buf, int size);
int			writetm(int fd, char *buf, int size, unsigned tout);

#if 0
#ifndef _ANSI_LIBRARY
/* Portable library 
	 the file IO structure
	_pnext points to next char to read in _buf if _fmode is READ.
	otherwise, _pnext is not used. */
typedef struct fio
{
	struct fio	*_nxfio;			/*0x0*/
	short		_fd;				/*0x4*/
	short		_fmode;				/*0x6*/
	short		_flags;				/*0x8*/
	short		volatile _nleft;	/*0xA*/
	char		*volatile _pnext;	/*0xC*/
	char		_buf[BUFSIZE];		/*0x10*/
} FIO;

extern FIO stdin, stdout;
#endif  /* #ifndef _ANSI_LIBRARY */
#endif

int			atoi(const char *s);
long		atol(const char *s);
void		error(char *s1, char *s2);
//FIO			*fopen(FIO *pfio, char *name, short mode);
char		*free(char *p);
//int			getc(FIO *xfio);
short		getch(void);

void		itoa(int n, char *s);
int			lenstr(char *s);
char		*malloc(unsigned size);
void		printf(char *fmt, ...);
//short		putc(FIO *xfio, short c);
void		putstr(int file, ...);
void		remark(char *s1, char *s2);

#if 0
short		_norm(char *s, double d, unsigned prec);
short		_round(char *s, unsigned n, unsigned prec);
FIO			*_stderr(void);
FIO			*_stdin(void);
FIO			*_stdout(void);
char		*alloc(unsigned need, char *link);
double		atof(char *s);
unsigned	btod(char *buf, unsigned len, double *v);
unsigned	btoi(char *s, unsigned n, int *pinum, short base);
unsigned	btol(char *s, unsigned n, long *pinum, short base);
unsigned	btos(char *s, unsigned n, short *pinum, short base);
char		*calloc(unsigned n, unsigned size);
int			cmpbuf(const char *s1, const char *s2, unsigned n);
int			cmpstr(const char *s1, const char *s2);
char		*cpystr(char *to, ...);
unsigned	decode(char *sbuf, unsigned nlen, char *fmt, ...);
unsigned	dtoe(char *buf, double v, unsigned p, unsigned q);
unsigned	dtof(char *buf, double v, unsigned p, unsigned q);
short		encode(char *sbuf, unsigned nlen, char *pfmt, ...);
void		errfmt(char *fmt, ...);
FIO			*fclose(FIO *pf);
FIO			*fcreate(FIO *pfio, char *name, short mode);
int			fflush(FIO *f);
unsigned	fill(char *buf, unsigned n, char c);
FIO			*finit(FIO *pf, int fd, short mode);
void		fprintf(FIO *apf, char *fmt, ...);
int			fread(int fd, char *buf, unsigned size);


char		*frelst(char **list, char **pstop);
int			fscanf(FIO *apf, char *fmt, ...);
short		getf(FIO *pfio, char *pfmt, ...);
short		getfmt(char *pfmt, ...);
unsigned	getl(FIO *xfio, char *buf, unsigned len);
unsigned	getlin(char *buf, unsigned len);
unsigned	inbuf(char *buf, unsigned n, char *set);
unsigned	instr(char *str, char *set);
unsigned	itob(char *s, int n, short base);
unsigned	lower(char *s, unsigned n);
unsigned	ltob(char *s, long n, short base);
char		*nalloc(unsigned need, char *link);
unsigned	notbuf(char *buf, unsigned n, char *set);
unsigned	notstr(char *str, char *set);
short		ordbuf(char *s1, char *s2, unsigned n);
int			prefix(char *s1, char *s2);
short		putch(short c);
void		putf(FIO *pfio, char *fmt, ...);
void		putfmt(char *fmt, ...);
unsigned	putl(FIO *xfio, char *buf, unsigned len);
unsigned	putlin(char *buf, unsigned len);
int			scanf(char *fmt, ...);
unsigned	scnbuf(char *buf, unsigned n, char c);
unsigned	scnstr(char *str, char c);
char		*sprintf(char *sbuf, char *fmt, ...);
unsigned	squeeze(char *buf, unsigned n, char c);
int			sscanf(char *sbuf, char *fmt, ...);
unsigned	stob(char *s, short n, short base);
unsigned	subbuf(char *s, unsigned ns, char *p, unsigned np);
unsigned	substr(char *s, char *p);
unsigned	upper(char *s, unsigned n);
#endif

/* minimum value for an object of type long int */
#ifndef LONG_MAX
#define LONG_MAX  0x7fffffffL
#endif

/* maximum value for an object of type long int */
#ifndef ULONG_MAX
#define ULONG_MAX 0xffffffffUL
#endif

// copy from 'stddef.h'
//typedef unsigned int size_t;

#ifndef __CLIBNS
#define __CLIBNS
#endif

#ifndef offsetof
#define offsetof(type, member) \
    ((__CLIBNS size_t)((char *)&(((type *)0)->member) - (char *)0))
#endif

#endif
