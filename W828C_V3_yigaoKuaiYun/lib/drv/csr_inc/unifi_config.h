/*
 * ---------------------------------------------------------------------------
 *
 * FILE: unifi_config.h
 * 
 * PURPOSE:
 *      This header file provides parameters that configure the operation
 *      of the driver.
 *
 * Copyright (C) 2006-2008 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 * ---------------------------------------------------------------------------
 */
 
#ifndef __UNIFI_CONFIG_H__
#define __UNIFI_CONFIG_H__

#include "hyTypes.h"

#define unifi_sprintf CsrSprintf	/* needs to be in os.c */
#define dump(...)

#define func_enter()
#define func_exit()
#define func_exit_r(...)

//#define ASSERT(...)

#define INLINE __inline 

/*
 * Override for the SDIO function block size on this host. When byte mode CMD53s
 * are not used/supported by the SD host controller, transfers are padded up to
 * the next block boundary. The 512-byte default on UF6xxx wastes too much space
 * on the chip, so the block size is reduced to support this configuration.
 */
#define CSR_WIFI_HIP_SDIO_BLOCK_SIZE  64

#endif /* __UNIFI_CONFIG_H__ */
