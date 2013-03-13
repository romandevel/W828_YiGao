/*
 *****************************************************************************
 *
 * FILE : unifi_priv.h
 * 
 * PURPOSE : Private header file for unifi driver.
 *
 *           UDI = UniFi Debug Interface
 *
 * Copyright (C) 2008 by Cambridge Silicon Radio Ltd.
 ***************************************************************************** 
 */

#ifndef __UNIFI_PRIV_H__
#define __UNIFI_PRIV_H__

#include "unifi.h"
#include "unifi_client.h"

// Configuration flags.
#define CLI_LOGGING_ENABLED     0x0001
#define CLI_USING_WIRE_FORMAT   0x0002
#define CLI_CONFIG_IND_ENABLED  0x0004
#define CLI_MODIFIES_DATA       0x0008
#define CLI_SME_EMB             0x0010

/* struct iw_encode_ext ->alg */
#define IW_ENCODE_ALG_NONE	0
#define IW_ENCODE_ALG_WEP	1
#define IW_ENCODE_ALG_TKIP	2
#define IW_ENCODE_ALG_CCMP	3


typedef enum { UNIFI_WAITING,
	   UNIFI_INIT_CHIP,
	   UNIFI_DOWNLOAD_MIB_DATA,
	   UNIFI_RESET_CHIP,
	   UNIFI_SCAN,
	   UNIFI_JOIN,
	   UNIFI_AUTHENTICATE,
	   UNIFI_ASSOCIATE,
	   UNIFI_ASSOCIATED,
	   UNIFI_PROCESS_UIP
} unifi_state_t;

typedef struct unifi_priv 
{
    card_t*			card;
    void*			sdio;

    // file handle for the firmware patch and MIB files.
	CsrUint8*		file_handle;

    // flag to say that an operation was aborted.
    CsrInt32		io_aborted;

    CsrInt32		bh_error_detected;
    
//    SysSema_t*			sema_mlme_blocking_request;
	CsrInt32		sender_id, seq_no;

	card_info_t		card_info;

    // which wireless interface to use (1 - 2.4GHz, 2 - 5GHz).
    CSR_IFINTERFACE	if_index;
    
    unifi_client_t*	pclient;
} unifi_priv_t;

// firmware download related functions.
CsrInt32 uf_request_firmware(unifi_priv_t *priv);
CsrInt32 uf_read_firmware(unifi_priv_t *priv, unsigned long arg);
CsrInt32 do_firmware_download(unifi_priv_t *priv);

#endif	// __UNIFI_PRIV_H__
