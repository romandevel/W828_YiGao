/*
 *****************************************************************************
 *
 * FILE : unifi_types.h
 * 
 * PURPOSE: Type declarations for UniFi.
 *          This is particularly structures that must be the same on host
 *          (i.e. driver) and chip.
 * 
 * Copyright (C) 2005-2008 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 ***************************************************************************** 
 */
#ifndef __UNIFI_TYPES_H__
#define __UNIFI_TYPES_H__


#ifndef NULL
#define NULL (0L)
#endif

/** int8 definition */
typedef signed char			int8;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;
typedef unsigned long long	uint64;
typedef short				int16;
typedef long				int32;


/* Structure of an entry in the Symbol Look Up Table (SLUT). */
typedef struct _symbol {
    uint16 id;
    uint32 obj;
} symbol_t;

/** Boolean definition */
typedef int16   Boolean;
#define HAVE_BOOLEAN_TYPE   1


#endif /* __UNIFI_TYPES_H__ */
