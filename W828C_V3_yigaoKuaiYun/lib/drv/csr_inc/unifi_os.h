/*
 * ---------------------------------------------------------------------------
 *
 * FILE: unifi_os.h
 * 
 * PURPOSE:
 *      This header file provides the OS-dependent facilities for a freeRTOS
 *      environment.
 *
 * Copyright (C) 2010 by Cambridge Silicon Radio Ltd.
 * ---------------------------------------------------------------------------
 */

#ifndef __UNIFI_OS_H__
#define __UNIFI_OS_H__

#include "unifi.h"

#define ETH_ALEN		6		/* octets in one ethernet addr */
#define ETH_HLEN        14		/* total octets ethernet in header. */

/*
 * malloc and free abstraction.
 */
void *unifi_malloc(void *ospriv, size_t size);
void unifi_free(void *ospriv, void *ptr);

CsrInt32 unifi_net_data_malloc(void *ospriv, bulk_data_desc_t *bulk_data_slot, size_t size);
void unifi_net_data_free(void *ospriv, bulk_data_desc_t *bulk_data_slot);

/*
 * time delay abstraction.
 */
void unifi_sleep_ms(void *ospriv, CsrUint32 t);

// int unifi_wait_event_timeout(int condition, int timeout);

typedef CsrInt32 unifi_timer_t;		/* needs to be something. */

// different levels of diagnostic detail.
#define UDBG1       1
#define UDBG2       2
#define UDBG3       3
#define UDBG4       4
#define UDBG5       5
#define UDBG6       6
#define UDBG7       7

#if 0
	void unifi_trace(void *ospriv, CsrInt16 level, const CsrUint8 *fmt, ...);
	void unifi_info(void *ospriv, const CsrUint8 *fmt, ...);
	void unifi_notice(void *ospriv, const CsrUint8 *fmt, ...);
	void unifi_warning(void *ospriv, const CsrUint8 *fmt, ...);
	void unifi_error(void *ospriv, const CsrUint8 *fmt, ...);
	#define unifi_print(...)
#else
	#define unifi_trace(...)
	//#define unifi_trace		hyUsbPrintf_unifiTrac
	#define unifi_info(...)
	#define unifi_notice(...)
	#define unifi_warning(...)
	#define unifi_error(...)
	//void unifi_print(void *ospriv, const CsrUint8 *fmt, ...);
	#define unifi_print		hyUsbPrintf
#endif

#endif /* __UNIFI_OS_H__ */
