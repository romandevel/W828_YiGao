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

#ifndef _UPROCH
#define _UPROCH

#include "dvcfg.h"

/*	USER PROCESS TABLE */
typedef struct uproc
{
	int  (*_entry)();			/* process entry address */
	char  *_tname;				/* process name */
	unsigned _scnt;				/* no. word entries for stack */
	char *_stdin;				/* standard input device name */
	char *_stdout;				/* standard output device name */
	char *_stderr;				/* standard error output device name */
	char  _tstart;				/* start-up process flag */
	char  _ptype;				/* program type */
	short _period;				/* reschedule interval */
	short _due;					/* initial due delay */
	void  *_param;				/* parameter to pass on periodic sched */
	short _priority;			/* process priority */
} UPROC;

/* start up flag */
#define T_OFF	0				/* do not start up initially */
#define T_ON	1				/* start up initially */

/* program types */
#define T_C		0				/* C task */

/*	USER CONFIGURATION TABLE */
typedef struct usrcfg
{
	char  *version;			/* Version string */
	UPROC *puptab;			/* user process table address */
	DVCFG *pdv;				/* device table address */
	char  *pheap;			/* heap address */
	short ticks;			/* no. of clock interrupts per time period */
	short maxfd;			/* no. of files allowed per process */
	short nres;				/* number of resources available */
	int  prempt;			/* interrupt preemption enable */
	int  keeph;				/* keep allocated heap on process exit */
	int  svflt;				/* save floating point registers */
	int  kquehi;			/* no. of entries in hi-level kernel queue */
	int  kquelo;			/* no. of entries in lo-level kernel queue */
	char zombie;			/* exiting processes must be waited for? */
	char noctty;			/* last close of tty clears controlling tty? */
	char profile;			/* profiling is turned on? */
	short timeslice;		/* timeslice period for peer processes */
	pid_t maxpids;			/* maximum process ID */
	short maxpri;			/* maximum priority */
	char stack_clr;			/* dynamic clearing of stack on creating process */
	char keep_main;			/* main proc stays alive when threads terminate */
} USRCFG;


/* 全局变量  在ikernal.c 中定义 */
extern char _version[];			/* system release info string */

/*	全局变量  在hyos_config_.c 中定义 */
extern CONST USRCFG _usrcfg;


#endif
