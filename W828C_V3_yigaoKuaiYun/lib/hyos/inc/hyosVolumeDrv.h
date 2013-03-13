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
 *  file		   : hyosVolumeDriver.c (Hyctron Nandflash Abstract Layer)
 *  Created_by     : ZhouJie 
 *  Creation date  : 2005 12 05 15:52
 *  Comment        : the OS common-entry wrapper for abstract nandflash driver
 *****************************************************************************/


#ifndef _HYOS_VOLUMEDRV_H_
#define _HYOS_VOLUMEDRV_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/

#include "std.h"
#include "driver.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

/* IOCTL calls */
#define VD_IOCTL_PARAMETERS     (0x80)
#define VD_IOCTL_ERASE          (0x81)
#define VD_IOCTL_FORMAT         (0x82)
#define VD_IOCTL_MAINTENANCE    (0x83)

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
 
typedef int (*pdrvFunc)(int func, FDTAB *pfd, char *buf, int n);

typedef struct
{
    unsigned volumeSize;
    unsigned erasableBlockSize;

} VolumeParameters_st; 

typedef struct
{
    unsigned position;
    unsigned size;

} EraseParameters_st; 


/*************************************************************************
 * IMPORTED
 *************************************************************************/

extern int NandFlashDrv(int func, FDTAB *pfd, char *buf, int n);
extern int NandFlashDrv2(int func, FDTAB *pfd, char *buf, int n);

#if defined(HYHW_MEMORY_SD_MMC)
extern int SDMMCCardDrv(int func, FDTAB *pfd, char *buf, int n);
#endif

#if defined(FEATURE_SELECTOR_USBHOST)
extern int UsbHostDrv(int func, FDTAB * pfd, char * buf, int n);
#endif

extern int nopresent(int func, FDTAB *pfd, char *buf, int n);


/*************************************************************************
 * FUNCTION DECLARATIONS
 *************************************************************************/

/*************************************************************************
 * Function            : hyosVolumeDrv_Entry
 * Description         : This is the entry point for all storage volume
 *                       drivers. The driver forwards the call to the correct
 *                       subsystem driver if required.
 * input parameters    : func, Driver operation requested
 *                       pfd, Pointer to File Descriptor Table entry,
 *                       buf, Pointer to user data buffer
 *                       n, Operation specific parameter
 * return parameter    : error code.
 *
 *************************************************************************/

int hyosVolumeDrv_Entry( int func, FDTAB *pfd, char *buf, int n );

#endif  //_HYOS_VOLUMEDRV_H_