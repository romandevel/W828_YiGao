/******************************************************************************
 * FILE: mlme.h
 *
 * PURPOSE:
 *      Routines to send MLME requests to the UniFi.
 *
 * Copyright (C) 2010 by Cambridge Silicon Radio Ltd.
 *
 * HISTORY:
 ******************************************************************************/

#ifndef _MLME_H
#define _MLME_H

#include "csr_types.h"
#include "unifi_priv.h"

#define SNAP_HEADER			6
#define SCAN_STARTUP_TIME	300	/* the additional time taken by the UniFi to do a scan per channel in millisecs. */
#define SCAN_INFO_ITEMS		1

typedef	struct ies_tag 
{
	CsrUint8*	buf;
	CsrInt32	len;
} ies_t;

typedef struct mlme_scan_info_tag
{
	CsrInt32					count, index;
	ies_t						ies[SCAN_INFO_ITEMS];
	CSR_MLME_SCAN_INDICATION	msi[SCAN_INFO_ITEMS];
	/* 当前AP的加密类型 wmi.h */
	/*
	typedef enum WMI_ENCRYPTION_TYPE{
	    WMI_CRYPTO_OPEN,
	    WMI_CRYPTO_WEP_40,
	    WMI_CRYPTO_WEP_104,
	    WMI_CRYPTO_WEP_128,
	    WMI_WEP_RECONNECT,
	    WMI_WPA_PSK_TKIP,
	    WMI_WPA_PSK_AES,
	    WMI_WPA_PSK_TKIP_AES,
	    WMI_WPA2_PSK_TKIP,
	    WMI_WPA2_PSK_AES,
	    WMI_WPA2_PSK_TKIP_AES
	} eWMI_ENCRYPTION_TYPE;
	*/
	CsrUint8					encryptionType;
} mlme_scan_info_t;

void mlme_init_scan_info(void);
void mlme_deinit_scan_info(void);
//void mlme_update_scan_info(ies_t *ies, CSR_MLME_SCAN_INDICATION *msi);
void mlme_update_scan_info(ies_t *ies, CSR_MLME_SCAN_INDICATION *msi, CsrUint8 encryptionType);
CsrInt32 mlme_request(unifi_priv_t *priv, unifi_client_t *pcli,
							CSR_SIGNAL *sig, bulk_data_param_t *data_ptrs);
CsrInt32 mlme_do_reset(unifi_priv_t *priv, unifi_client_t *pcli, CSR_MACADDRESS *macaddr, CsrUint8 set_default_mib);
CsrInt32 mlme_do_scan(unifi_priv_t *priv, unifi_client_t *pcli, CsrInt32 scantype, 
							CSR_BSS_TYPE bsstype, CsrUint8 *ssid, CsrInt32 ssid_len);
CsrInt32 mlme_do_join(unifi_priv_t *priv, unifi_client_t *pcli);
CsrInt32 mlme_do_authenticate(unifi_priv_t *priv, unifi_client_t *pcli);
//CsrInt32 mlme_do_associate(unifi_priv_t *priv, unifi_client_t *pcli);
CsrInt32 mlme_do_associate(unifi_priv_t *priv, unifi_client_t *pcli, CsrUint8 *ies_buf, int ies_len);
CsrInt32 mlme_do_leave(unifi_priv_t *priv, unifi_client_t *pcli, CsrUint8 *ies_buf, int ies_len);

#endif	/* _MLME_H */
