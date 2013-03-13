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

#ifndef _SIGNAL
#define _SIGNAL

#include "std.h"
#include "hyosdef.h"

/*  macros for testing for deliverable or masked signal */
#define SIG_PENDING(p) ((p)->psighand->pending)
#define INMASK(psig, sig) (0)

/*  signal values */
#define SIGKILL	_SIGKILL
#ifndef SIGABRT			   // amo
#define SIGABRT	_SIGKILL
#endif
#define SIGALRM	_SIGKILL
#ifndef SIGFPE			  // amo
#define SIGFPE	_SIGKILL
#endif
#define SIGHUP	_SIGKILL
#ifndef SIGILL            // amo
#define SIGILL	_SIGKILL
#endif
#ifndef SIGINT
#define SIGINT	_SIGKILL
#endif
#define SIGPIPE	_SIGKILL
#define SIGQUIT	_SIGKILL
#ifndef SIGSEGV
#define SIGSEGV	_SIGKILL
#endif
#ifndef SIGTERM
#define SIGTERM	_SIGKILL
#endif
#ifndef SIGUSR1
#define SIGUSR1	_SIGKILL
#endif
#ifndef SIGUSR2
#define SIGUSR2	_SIGKILL
#endif
#define SIGCHLD	_SIGKILL
#define SIGCONT	_SIGKILL
#define SIGSTOP	_SIGKILL
#define SIGTSTP	_SIGKILL
#define SIGTTIN	_SIGKILL
#define SIGTTOU	_SIGKILL
#define SIGIO	_SIGKILL


#define _NSIG	_SIGMAX	/* one more than last code */

/* definitions for POSIX signal functions
 * signal set type */
#ifndef _SIGSET_T
#define _SIGSET_T
typedef unsigned long sigset_t;
#endif

/*  function prototypes */
void	_sig_handler(void);
int		kill(pid_t pid, int sig);

#endif
