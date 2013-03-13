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

#ifndef _SYSH
#define _SYSH

#include "driver.h"
#include "uproc.h"
#include "signal.h"
#include "libs.h"


/* SIGNAL HANDLER STRUCTURE 	信号句柄结构定义 */
typedef struct sig_hand
{
	void		**volatile	which_wlist;	/* pointer to ewait list. */
											/* Signal handler sets which_wlist to 1 for E_INTR */
	sigset_t	volatile	pending;		/* pending signals */
	sigset_t	volatile	mask;			/* masked signals */
} SIG_HAND;


/*	SYSTEM PROCESS TABLE	系统进程的结构
	注意，asmarm.s 中，默认的sproc结构的顺序为这个结构的顺序，如果这个结构变化了，
	则在汇编中的代码也需要变化  sizeof(SPROC) == 0xAC */
typedef struct sproc
{
	unsigned volatile	state;			/* 0x0	进程状态，见下面的位定义 */
	pid_t				pid;			/* 0x4	进程id */
	char				ptype;			/* 0x6	进程还是线程 */
	char	volatile	selecting;		/* 0x7	process is currently doing select */
	int					(*pentry)();	/* 0x8	进程入口函数的地址 */
	LNKLST				*plnklst;		/* 0xC	ptr to save area for ulib */
	short				epri;			/* 0x10	effective 进程优先级 */
	short				ipri;			/* 0x12	进程初始优先级 */
	unsigned			*stack;			/* 0x14	进程栈的顶，进程建立后，这个值不再改变 */
	unsigned *volatile	ustack;			/* 0x18	进程栈指针，随着进程的运行而动态变化 */
	unsigned			*slimit;		/* 0x1C	进程栈的底，进程建立后，这个值不再改变 */
	void	(*volatile	exitf)();		/* 0x20	指向进程退出时需要调用的第一个函数，包括一些现场的清理 
												参加exit函数中，在进程退出时，退出函数是递归调用
												当有几个函数需要调用时，指向第一个被调用的函数，而这个函
												数的返回值是函数指针，指向第二个要被调用的函数，余类推*/
	FDTAB				*pfd;			/* 0x24	process fd table entry */
	short	volatile	per;			/* 0x28 如果进程是周期地被调用的，这个参数表示时间间隔 */
	short	volatile	due;			/* 0x2A	进程延时调用时间 periodic schedule due time counter */
	short				sleep;			/* 0x2C	进程进入睡眠状态时的睡眠时间计数器 */
	unsigned short		nlocks;			/* 0x2E	no. of locked resources */
	void	*volatile	param;			/* 0x30	parameter to pass on periodic schedule */
	
	struct sproc *volatile wlist;		/* 0x34	等待进程链表，最后一个进程的wlist为NULL。和wlist_tail配合使用 */
	struct sproc *volatile clklst;		/* 0x38	时间控制进程链表 */
	
	short				cpri;			/* 0x3C	current priority saved for heir */
	short				cslice;			/* 0x3E	current timeslice value saved for heir */
	DVCFG				*pstdin;		/* 0x40	device to use for standard input */
	DVCFG				*pstdout;		/* 0x44	device to use for standard output */
	DVCFG				*pstderr;		/* 0x48	device to use for standard error */
	struct sproc *volatile heir;		/* 0x4C	process to whom priority is given, heir 继承人 
												提升了和当前进程一样优先级的进程，往往是因为这个进程占用
												了资源，同时高优先级的进程申请这个资源，所以临时提升它的优先级 */
	struct sproc *volatile benefact;	/* 0x50	heir's benefactor 赠送者 
												如果这个进程被临时提升了优先级，这个优先级的来历，往往是
												由于当前较低优先级的进程占用了benefact的资源
												当前进程就是heir进程，它原先的优先级在成员cpri保存，原先的
												时间片在cslice保存*/
	TMOUT				tmout;			/* 0x54	+ 0x1C process tmout structure */
	SIG_HAND			*psighand;		/* 0x70	pointer to signal handler struct */
	KERNLCK				*kernlock;		/* 0x74	pointer to kernel lock */
	unsigned short volatile time_left;	/* 0x78	remaining time for sleep/wait */
	unsigned short volatile slice_left;	/* 0x7A	remaining time for peer process */
	struct sproc		*peer_next;		/* 0x7C process of peer group */
	union
	{
		int volatile	proc;			/* 0x80 process return value */
		void volatile	*thread;		/* 		thread return value */
	} exit_status;
	void	*volatile	wchan;			/* 0x84 event waiting for in timed sleep */
	struct sproc *volatile pnext;		/* 0x88 pointer to next SPROC */
	char				*pname;			/* 0x8C 进程名字 */
	clock_t 			tms_ptime;		/* 0x90 pointer to process times */
	struct sproc **volatile wlist_tail;	/* 0x94 记录等待进程链表的尾的指针，和wlist配合使用 */
	void	(*volatile	exlock)();		/* 0x98 saves onexit return in lock */
	void	(*volatile	extmout)();		/* 0x9C saves onexit return in tmout 
												从tmout返回时需要调用的函数，由于要对tmout链表进行处理，这个必然有 */
	void				*ce_dos;		/* 0xA0 CE-DOS specific data */
	void				*ce_tcp;		/* 0xA4 CE-TCP specific data*/
										/* this is required by finit, which is not compiled for PSX */
	struct sproc		*main_thread;	/* 0xA8 main thread process */
} SPROC;

/*	上面的结构SPROC中state成员的定义*/
#define	STATE_MASK	0x3ff
#define P_ACTIV		0x001
#define	P_INACT		0x002
#define P_SLEEP		0x004
#define P_WAIT		0x008
#define P_ZOMBI		0x010
#define P_SIGWAIT	0x040
#define P_SUSPEND	0x080
#define P_CONDWAIT	0x100
#define P_TRANS		0x200
#define P_SIG		BIT31		/*process flags: top bit is signal pending */

/* process types */
#define PT_PROCESS	0x00		/* standard process */
#define PT_THREAD	0x01		/* thread */
#define PT_ITHREAD	0x02		/* initial thread of multi-threaded process */

/* thread type macros
 */
#define IS_THREAD(a)		((a)->ptype == PT_THREAD)
#define IS_MAIN_THREAD(a)	((a)->ptype == PT_ITHREAD)
#define IN_THREAD_PROC(a)	((a)->ptype != PT_PROCESS)

/* priority compares*/
#define HI_PRI(a,b)	((a)->epri > (b)->epri)
#define EQ_PRI(a,b)	((a)->epri == (b)->epri)
#define LO_PRI(a,b)	((a)->epri < (b)->epri)

/*	process ID table entry 
	进程ID表，*/
union pidtab_ent
{
	SPROC *volatile psproc;
	union pidtab_ent *volatile pfree;
};

/* get pointer to SPROC for a specific pid */
#define PSPROC(a) (_pidtab[(a)].psproc)

/* validity check of pid */
#define VAL_PID(a)	(_pidtab[(a)].pfree > _pidtab_end)
#define INV_PID(a)	(_pidtab[(a)].pfree <= _pidtab_end)

/*  优先级表是一个双向链表，按优先级从高到低连接起来 
	priority table entry */
typedef struct pritab_ent
{
	struct sproc *volatile prsproc;		/* 链表的内容1: 当前优先级的进程*/
	struct sproc *volatile prtail;		/* 链表的内容2: 具有相同优先级的尾进程，是为了加快速度。
										   而链表是通过sproc结构中的成员peer_next连起来的
										   在prtail的peer_next中，指向空，表示结束 */
	struct pritab_ent *pr_next;			/* 后一个链表项，比当前的优先级低，闭环, 队列中最低优先级的next比当前高*/
	struct pritab_ent *pr_back;			/* 前一个链表项，比当前的优先级高，闭环，所以最高优先级的pr_back 比第一个最高优先级的低   */   
										/* 例如 优先级列表为 11 - 8 - 5 -1  
											11.next = 8;   8.next= 5;  5.next = 1; 1.next = 11;
											1.back  = 5;   5.back = 8; 8.back = 11; 11.back = 1;*/
										
} PRITAB;

/* priority tests */
#define IS_PEER(a)   (_pritab[(a)->epri].prtail)
#define PRI_USED(a)  ((a)->prsproc)

/* empty priority table entry */
#define PRI_EMPTY (NULL)

/* scheduler_event states  调度器事件*/
#define SCH_NOEVENT		0		/* no event, normal advance to next process */
#define SCH_ISEVENT		1		/* event, go directly to highest active */
#define SCH_NOADVANCE	2		/* no event, do not preempt to next process */

/* benefactor/heir region states */
#define RUN_HEIR		1		/* heir process to be run with no preemption */
#define RUN_BENEFACTOR	2		/* return to benefactor with no preemption */

/* 内核延缓函数的参数，KERNEL QUEUES */
union kqitem
{
	void *p;
	unsigned long l;
};


/*	记录需要执行的中断延缓函数及其参数的结构，例如，ewakeup函数可在中断中调用，但是实际上
	在中断中并不真正执行，而是把它放入到系统的中断延缓函数队列中，退出中断后再执行
	这个函数，中断延缓函数队列存放3个值，要执行的函数的地址，该函数的参数，现在支持
	最多两个参数。*/
typedef struct kqentry
{
	void	(*func)();
	union kqitem *p1;
	union kqitem *p2;
}KQUE;

/* kernel process options
 * NOTE: These bits values are encoded in the kqueXXX.asm modules! */
#define K_HI		(1U << (BITS_INT - 1))
#define K_CLKLST	0x0001
#define K_KQUEHI	0x0002
#define K_KQUELO	0x0004
#define K_TMOUT		0x0008
#define K_TIMSLIC	0x0010		/* asynchronous, peer timeslice expired */


/*	RESOURCE COORDINATOR TABLE */
typedef struct rstab
{
	unsigned	volatile	rsource;	/* 锁定的资源的值 */
	pid_t		volatile	rspid;		/* 如果大于等于0，表示锁定资源的进程id号，如果小于0，表示资源状态，如下面预订义 */
	void		*volatile	rswait;		/* 指向等待资源的进程链表的首进程，链表是通过进程结构串起来 */
} RSTAB;

/*	resource states ，用rspid纪录*/
#define R_EMPTY	(-1)	/* 表示资源的结构表该位置还没有使用过*/
#define R_NBUSY	(-2)	/* 表示资源的结构表该位置使用过，但现在空闲 */


/*	fast form of entreg, entsig, _entsys
 */
#define _entreg() ++_noswit
#define _entsig() ++_nosig
#define _entsys() {++_nosig;++_noswit;}

/* fast form of lvreg
 */
#define _lvreg() \
	{ \
	if (!--_noswit && (_klev_flags || _event)) \
		__lvreg(); \
	}

/* fast form of lvsig
 */
#define _lvsig() \
	{ \
	lvsig(); \
	}

/* fast form of _lvsys
 */
#define _lvsys() \
	{ \
	_lvreg(); lvsig(); \
	}

#define tsleep(x)	_tsleep(x)

/* function prototypes
 */
short _chgepri(short pri);
short _chgpri(pid_t pid, short priority);
int _close(register FDTAB *pfd);
int _cmpbuf(char *s1, char *s2, int n);
int _cmpstr(char *s1, char *s2);
void _creatclk(SPROC *p);
void _deletep(SPROC *pu);
void _doclk(unsigned nunits);
void _entp(void *param);
int _ewait(void **list, int sig_ign);
void _ewait_tmo(TMOUT *tm);
int _ewaittm(register void **list, unsigned tout, int sig_ign);
void _extract_proc(SPROC *p);
unsigned _fill(char *buf, unsigned n, char c);
void _ientp(void *param);
void _initproc(char *mem, char *name, unsigned ssize, unsigned thread,
	char *sin, char *sout, char *serr);
void _insert_proc(SPROC *pu);
void _isignal(pid_t);
int _is_peer(void);
void _istart(SPROC *, void *);
char *_kalloc(register unsigned need);
int _kewait(void **list, int tmoflag);
int _kfree(char *p);
void _klev_hi(void);
int _kqgethi(void (**func)(), union kqitem **item1, union kqitem **item2);
int _kqgetlo(void (**func)(), union kqitem **item1, union kqitem **item2);
void _kqputhi(void (*func)(), ...);
void _kqputlo(void (*func)(), ...);
unsigned _lenstr(char *);
int *_limit(void);
extern SPROC *_lo_proc;	
void __lvreg(void);
int _open(DVCFG *pdev, int mode, FDTAB *pfd, long size);
void _peer_add_head(PRITAB *pt, SPROC *p);
void _peer_add_tail(PRITAB *pt, SPROC *p);
void _peer_delete(PRITAB *pt, SPROC *p);
void _peer_delete_head(PRITAB *pt, SPROC *p);
void _peer_done(void);
void _peer_raise(void);
void _putclk(SPROC *p);
void _rellock(KERNLCK *plck);
int _send_sig(SPROC *p, int signo);
int _setlock(KERNLCK *plck, int tmoflag);
void _signal(int sig, short p);
char *_strcpy(register char *s1, register char *s2);
void _unsched_peer(void);
void _utos(void);
void _wakeup(void ** list); /* 20071122*/

FDTAB *_chkfd(int fd);

#if 0
void _abort_proc(SPROC *p);
SPROC *_add_peer(SPROC *pu);
void _init_thread(SPROC *p);
unsigned _ltob(char *s, long n, short base);
void _mrelease(void);
void _private_destroy(void);
void _putstr(int file, ...);
int _sig_thread(SPROC *p, int signo);
void selwakeup(SPROC *p, int collision);
void _trelease(void);
void _thread_cancel_test(void);
void _thread_exit(void);
int _tsleep(char *pchan, int pri, char msg[], int tval);
void _twakeup(char *pchan);
void _wakepid(int status);
void _del_peer(SPROC *pu);
#endif

/* data declarations*/

extern ALLDAT alldat;
extern char	volatile _benereg;	
extern clock_t _clk_tck;
extern SPROC *volatile _clklst;
extern SPROC *_cproc;
extern char *_def_fs;
extern char	volatile _event;	
extern int	 _freex;	
extern KERNLCK _heaplck;
extern unsigned volatile _hi_ticks;
extern unsigned volatile _hi_timun;
extern char volatile _input_wake;
extern char	*_hptop;
extern char	_in_klev;
extern char	_in_idle;
extern char	_in_nest;
extern char	_in_sched;
extern char	_in_state;
extern char	volatile _intsig;
extern char	_is_init;
extern char	_keep_main;
extern unsigned volatile _klev_clk;
extern unsigned volatile _klev_flags;
extern char	_klevhipri;
extern unsigned volatile _klev_ticks;
extern unsigned volatile _klev_timun;
extern unsigned volatile _kqcnthi;
extern unsigned volatile _kqcntlo;
extern int volatile _kqovf_hi;
extern int volatile _kqovf_lo;
extern KQUE *_kque_hi;
extern KQUE *_kque_lo;
extern RSTAB *volatile _lprstab;	
extern short _maxdev;
extern short _maxfd;	
extern unsigned _maxkqhi;
extern unsigned _maxkqlo;
extern pid_t _maxpids;
extern short _maxpri;
extern int _nkqhi;
extern int _nkqlo;
extern char	volatile _nosig;	
extern char	volatile _noswit;
extern int volatile _nproc;	
extern short _nres;		
extern KQUE *_pgethi;
extern KQUE *_pgetlo;
extern char	*volatile _pheap;	
extern union pidtab_ent *_pidfree_hd;
extern union pidtab_ent *_pidfree_tail;
extern union pidtab_ent *_pidtab;
extern union pidtab_ent *_pidtab_end;
extern KQUE *_pmaxhi;
extern KQUE *_pmaxlo;
extern int _prempt;	
extern PRITAB *volatile _pri_cur;
extern PRITAB *volatile _pri_head;
extern PRITAB *_pritab;
extern PRITAB *_pritab_end;
extern RSTAB *_prstab;	
extern void (*_pdrv_dos)();
extern void (*_pdrv_tcp)();
extern KQUE *_pputhi; 
extern KQUE *_pputlo; 
extern short _psiz_dos;
extern short _psiz_tcp;
extern char _psx;
extern USRCFG *_pusrcfg;	
extern UPROC *_puproc;	
extern KERNLCK _reslck;	
extern clock_t volatile _seconds;	/* seconds for date & time */
extern clock_t volatile _sec_cnt;	/* partial second measured in clock ticks */
extern SPROC *volatile selwait;	
extern unsigned short _slice_val;
extern char	*_sstack;	
extern char	_stack_clr;
extern char	volatile _state;		
extern int *sused;	
extern char	_svflt;		
extern unsigned volatile _tck_cnt;		
extern unsigned _tcks_unit;		
extern clock_t _tick;		
extern clock_t	volatile _timer;		
extern unsigned short volatile _timeslice;
extern clock_t	*_tms_utime;
extern void	*volatile _tsleep_hd;
extern TMOUT *volatile _tlist;	
extern char	*tzname[2];

extern CONST char *_err_tab[];
extern CONST int _max_errs;
extern unsigned _ndoub;
extern unsigned _nreg;
extern USRCFG * (*pulink)();

extern char	_environ[];			/* environment string */

extern char	**environ;			/* environment list */
extern SPROC *volatile _twaitlst;	
extern SPROC *volatile _zwaitlst;	

#endif
