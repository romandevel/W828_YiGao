/*
 * C EXECUTIVE Demonstration Program
 * Copyright (C) 1981-1999 by JMI Software Systems, Inc.
 * All rights reserved.
 *
 * All data in this module is read only.
 */

/*------------------------------------------------------------------------------
   Standard include files:
   ---------------------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*----------------------------------------------------------------------------*/

#include "std.h"
#include "uproc.h"

#include "ospriority.h"
/*----------------------------------------------------------------------------*/

//#define ISRAM_HEAP Image$$ISRAM_ZW_ZI$$ZI$$Limit
//#define ISRAM_HEAP (char*)0x20180000
#define ISRAM_HEAP (char*)0x25C00000

//#define SYS_STACK_LEN	128//系统堆栈大小，以32bit计
#define SYS_STACK_LEN	1024//系统堆栈大小，以32bit计



/*******************************************************************************
 * USER DEFINED SYSTEM PARAMETERS
 ******************************************************************************/

//#define _HEAP   &ISRAM_HEAP /* start of system heap */
#define _HEAP   ISRAM_HEAP /* start of system heap */
#define _TICKS  1           /* number of clock interrupts per system time unit*/
//#define _MAXFD  3           /* number open files allowed per process */
#define _MAXFD  5           /* number open files allowed per process */
#define _NRES   20          /* number resources available */
#define _PREMPT YES         /* process pre-emption allowed ? */
#define _KEEPH  YES         /* keep allocated memory on process exit ? */
#define _SVFLT  NO          /* save floating point registers ? */
#define _KQUEHI 7           /* number of entries in hi-level kernel que */
#define _KQUELO 7           /* number of entries in lo-level kernel que */
#define _ZOMBIE NO          /* processes become zombies on exit */
#define _NOCTTY NO          /* clear controlling terminal on last exit */
#define _PROFILE 0          /* profile stats: 0 none, 1 clear on exit, */
                            /*                2 cumulative per system */
#define _TSLICE  1          /* timeslice quantum */
#define _MAXPIDS 30//10//         /* maximum no. of process identifiers */
#define _MAXPRI  PRIORITY_HIGHEST_OSPP//15//         /* maximum priority value */

#define MAIN_PROCESS_PRIORITY           PRIORITY_HIGHEST_OSPP//15//
#define MAIN_PROCESS_STACK_SIZE			0xA0

/*******************************************************************************
 * DEVICE INITIALIZATION TABLES
 ******************************************************************************/

/* DEVICE NAMES */
static CONST char dvnul[]     = "dvnul";

#ifdef TMFL_TRACE /* TTY UART LOGGING */
    static CONST char tty0[]  = "tty0";
# endif

/*----------------------------------------------------------------------------*/

#define DVNUL     (char *) dvnul

#ifdef TMFL_TRACE /* TTY UART LOGGING */
    #define TTY0  (char *) tty0
#endif

#define NDOSFILE  "dosfile"

/*----------------------------------------------------------------------------*/

/* DEVICE DRIVER FUNCTION ENTRY POINTS */
extern int nuldrv(void);
extern int dosfile(void);
extern int hyosVolumeDrv_Entry(void);

#ifdef TMFL_TRACE /* TTY UART LOGGING */
    extern int ttydrv(void);
#endif

/*----------------------------------------------------------------------------*/

/* TTY */
#define IBUF        80      /* tty input buffer size */
#define OBUF        80      /* tty output buffer size */
#define RUBOUT      '\b'
#define LINE_ERASE  '@'
#define E_TXT       '\0'
#define KEY_INTR    '\3'    /* ^C */
#define CE_VIEW     '\1'    /* ^A */

#ifdef TMFL_TRACE /* TTY UART LOGGING */
static CONST TTYI tty = {IBUF,OBUF,
    {_NTABS|_CRLF|_KILL|_ERASE|_ECHO,RUBOUT,LINE_ERASE,E_TXT,KEY_INTR,CE_VIEW}};
#endif

/*----------------------------------------------------------------------------*/

/* DOSFILE */
//#define FILESI_MAX_OPEN_FILES           (5)      /* maxfiles open at one time */
#define FILESI_MAX_OPEN_FILES           (10)      /* maxfiles open at one time */
#define FILESI_MAX_SYSTEMS              (1)      /* max file subsystems       */
static CONST FILESI files = { FILESI_MAX_OPEN_FILES, FILESI_MAX_SYSTEMS };

/*----------------------------------------------------------------------------*/

/* DEVICE CONFIGURATION TABLE */
static CONST DVCFG _dvcfg[] = {
    {DVNUL,         nuldrv,     0,  NULL},
    {NDOSFILE,      dosfile,    0,  (FILESI *) &files},
    {"C",           hyosVolumeDrv_Entry,     0,  0},
    {"D",           hyosVolumeDrv_Entry,     1,  0},
    {"E",           hyosVolumeDrv_Entry,     2,  0},
    {"F",           hyosVolumeDrv_Entry,     3,  0},
#ifdef TMFL_TRACE /* TTY UART LOGGING */
    {TTY0,          ttydrv,     0,  (TTYI *) &tty},
#endif
    {NULL},
};   

/*******************************************************************************
 * USER PROCESS INITIALIZATION TABLE
 ******************************************************************************/

extern int CreateProcessor(void);

static CONST UPROC _uproc[] = 
{
    {CreateProcessor, "entry", MAIN_PROCESS_STACK_SIZE, NULL, NULL, NULL, T_ON,T_C, 0, 0, 0, MAIN_PROCESS_PRIORITY},
    {NULL},
};

/*******************************************************************************
 * USER CONFIGURATION TABLE
 ******************************************************************************/

CONST USRCFG _usrcfg =
     { _version, (UPROC *)_uproc, (DVCFG *)_dvcfg, _HEAP, _TICKS,
       _MAXFD,   _NRES, _PREMPT, _KEEPH, _SVFLT, _KQUEHI, _KQUELO,
       _ZOMBIE,  _NOCTTY, _PROFILE, _TSLICE, _MAXPIDS, _MAXPRI };

/*******************************************************************************
 * ARM ADS Usage Specifics
 ******************************************************************************/

#ifndef TMFL_TRACE /* TTY UART LOGGING */

    /* Generate Link  Error when functionality that requires semi hosting is
    ** included in the link step
    */
    #pragma import(__use_no_semihosting_swi)

    /* Avoid resolution of the __stderr symbol to the ARM stdio.o library
    ** member
    */
    FILE __stderr = STDERR;

#endif /* TTY UART LOGGING */

extern const U32 topOfStacks;// = TOP_OF_STACKS;

/*----------------------------------
获得系统堆栈底
----------------------------------*/
int getBottomOfSysStacks(void)
{
	return (int)(topOfStacks - (SYS_STACK_LEN<<2));
}

/*----------------------------------------------------------------------------*/

/*
** Avoid resolution of the __raise symbol to the ARM __raise.o library member
*/
int __raise(int major, int minor)
{
    while(1);
}
/*----------------------------------------------------------------------------*/

/* Avoid resolution of the _sys_exit symbol to the ARM _sys_exit.o library 
** member
*/
void _sys_exit(int return_code)
{
    while(1);
}
/*----------------------------------------------------------------------------*/

