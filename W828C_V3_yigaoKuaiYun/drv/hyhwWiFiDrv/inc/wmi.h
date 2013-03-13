
#ifndef _HTC_H_
#define _HTC_H_

#include "hyTypes.h"
#include "llist.h"
#include "sigs.h"
#include "mlme.h"

#define HostCmd_BSS_TYPE_BSS                        0x0001
#define HostCmd_BSS_TYPE_IBSS                       0x0002
#define HostCmd_BSS_TYPE_ANY                        0x0003
#define	WLAN_SUPPORTED_RATES		14
#define MAX_AC_QUEUES 4
/** Host Command ID */
#define HostCmd_CMD_NONE                      0x0000
#define HostCmd_CMD_CODE_DNLD                 0x0002
#define HostCmd_CMD_GET_HW_SPEC               0x0003
#define HostCmd_CMD_EEPROM_UPDATE             0x0004
#define HostCmd_CMD_802_11_RESET              0x0005
#define HostCmd_CMD_802_11_SCAN               0x0006
#define HostCmd_CMD_802_11_GET_LOG            0x000b
#define HostCmd_CMD_MAC_MULTICAST_ADR         0x0010
#define HostCmd_CMD_802_11_AUTHENTICATE       0x0011
#define HostCmd_CMD_802_11_EEPROM_ACCESS      0x0059
#define HostCmd_CMD_802_11_ASSOCIATE          0x0050
#define HostCmd_CMD_802_11_SET_WEP            0x0013
#define HostCmd_CMD_802_11_GET_STAT           0x0014
#define HostCmd_CMD_802_3_GET_STAT            0x0015
#define HostCmd_CMD_802_11_SNMP_MIB           0x0016
#define HostCmd_CMD_MAC_REG_MAP               0x0017
#define HostCmd_CMD_BBP_REG_MAP               0x0018
#define HostCmd_CMD_MAC_REG_ACCESS            0x0019
#define HostCmd_CMD_BBP_REG_ACCESS            0x001a
#define HostCmd_CMD_RF_REG_ACCESS             0x001b
#define HostCmd_CMD_802_11_RADIO_CONTROL      0x001c
#define HostCmd_CMD_802_11_RF_CHANNEL         0x001d
#define HostCmd_CMD_802_11_RF_TX_POWER        0x001e
#define HostCmd_CMD_802_11_RSSI               0x001f
#define HostCmd_CMD_802_11_RF_ANTENNA         0x0020

#define HostCmd_CMD_802_11_PS_MODE	      0x0021

#define HostCmd_CMD_802_11_DATA_RATE          0x0022
#define HostCmd_CMD_RF_REG_MAP                0x0023
#define HostCmd_CMD_802_11_DEAUTHENTICATE     0x0024
#define HostCmd_CMD_802_11_REASSOCIATE        0x0025
#define HostCmd_CMD_802_11_DISASSOCIATE       0x0026
#define HostCmd_CMD_MAC_CONTROL               0x0028
#define HostCmd_CMD_802_11_AD_HOC_START       0x002b
#define HostCmd_CMD_802_11_AD_HOC_JOIN        0x002c

#define HostCmd_CMD_802_11_QUERY_TKIP_REPLY_CNTRS  0x002e
#define HostCmd_CMD_802_11_ENABLE_RSN              0x002f
#define HostCmd_CMD_802_11_PAIRWISE_TSC       0x0036
#define HostCmd_CMD_802_11_GROUP_TSC          0x0037
#define HostCmd_CMD_802_11_KEY_MATERIAL       0x005e

#define HostCmd_CMD_802_11_DEEP_SLEEP         0x003e

#define HostCmd_CMD_802_11_SET_AFC            0x003c
#define HostCmd_CMD_802_11_GET_AFC            0x003d

#define HostCmd_CMD_802_11_AD_HOC_STOP        0x0040

#define HostCmd_CMD_802_11_HOST_SLEEP_CFG   0x0043
#define HostCmd_CMD_802_11_HOST_SLEEP_AWAKE_CONFIRM 0x0044

#define HostCmd_CMD_802_11_BEACON_STOP        0x0049
#define HostCmd_CMD_802_11_SET_RGN_CODE        0x004C

#define HostCmd_CMD_802_11_MAC_ADDRESS        0x004D
#define HostCmd_CMD_802_11_EEPROM_ACCESS      0x0059




#define HostCmd_CMD_802_11_BAND_CONFIG        0x0058

#define HostCmd_CMD_802_11D_DOMAIN_INFO       0x005b

#define HostCmd_CMD_WMM_ACK_POLICY            0x005C
#define HostCmd_CMD_WMM_PRIO_PKT_AVAIL        0x005D



#define HostCmd_CMD_802_11_SLEEP_PARAMS          0x0066

#define HostCmd_CMD_802_11_INACTIVITY_TIMEOUT    0x0067

#define HostCmd_CMD_802_11_SLEEP_PERIOD          0x0068
#define HostCmd_CMD_802_11_BCA_CONFIG_TIMESHARE  0x0069

#define HostCmd_CMD_802_11_BG_SCAN_CONFIG        0x006b
#define HostCmd_CMD_802_11_BG_SCAN_QUERY         0x006c

#define HostCmd_CMD_802_11_CAL_DATA_EXT          0x006d


#define HostCmd_CMD_WMM_GET_STATUS               0x0071

#define HostCmd_CMD_802_11_TPC_CFG               0x0072
#define HostCmd_CMD_802_11_PWR_CFG               0x0073

#define HostCmd_CMD_802_11_FW_WAKEUP_METHOD      0x0074

#define HostCmd_CMD_802_11_LED_GPIO_CTRL         0x004e

#define HostCmd_CMD_802_11_SUBSCRIBE_EVENT       0x0075

#define HostCmd_CMD_802_11_RATE_ADAPT_RATESET    0x0076


#define HostCmd_CMD_802_11_TX_RATE_QUERY	0x007f

#define HostCmd_CMD_GET_TSF                      0x0080


#define HostCmd_CMD_802_11_POWER_ADAPT_CFG_EXT	0x007e



/* For the IEEE Power Save */
#define HostCmd_SubCmd_Enter_PS               0x0030
#define HostCmd_SubCmd_Exit_PS                0x0031
#define HostCmd_SubCmd_Sleep_Confirmed        0x0034
#define HostCmd_SubCmd_Full_PowerDown         0x0035
#define HostCmd_SubCmd_Full_PowerUp           0x0036

/* Command RET code, MSB is set to 1 */
#define HostCmd_RET_NONE                      0x8000
#define HostCmd_RET_HW_SPEC_INFO              0x8003
#define HostCmd_RET_EEPROM_UPDATE             0x8004
#define HostCmd_RET_802_11_RESET              0x8005
#define HostCmd_RET_802_11_SCAN               0x8006
#define HostCmd_RET_802_11_GET_LOG            0x800b
#define HostCmd_RET_MAC_CONTROL               0x8028
#define HostCmd_RET_MAC_MULTICAST_ADR         0x8010
#define HostCmd_RET_802_11_AUTHENTICATE       0x8011
#define HostCmd_RET_802_11_DEAUTHENTICATE     0x8024
#define HostCmd_RET_802_11_SET_RGN_CODE		  0x804c
#define HostCmd_RET_802_11_ASSOCIATE          0x8012
#define HostCmd_RET_802_11_REASSOCIATE        0x8025
#define HostCmd_RET_802_11_DISASSOCIATE       0x8026
#define HostCmd_RET_802_11_SET_WEP            0x8013
#define HostCmd_RET_802_11_STAT               0x8014
#define HostCmd_RET_802_3_STAT                0x8015
#define HostCmd_RET_802_11_SNMP_MIB           0x8016
#define HostCmd_RET_MAC_REG_MAP               0x8017
#define HostCmd_RET_BBP_REG_MAP               0x8018
#define HostCmd_RET_RF_REG_MAP                0x8023
#define HostCmd_RET_MAC_REG_ACCESS            0x8019
#define HostCmd_RET_BBP_REG_ACCESS            0x801a
#define HostCmd_RET_RF_REG_ACCESS             0x801b
#define HostCmd_RET_802_11_RADIO_CONTROL      0x801c
#define HostCmd_RET_802_11_RF_CHANNEL         0x801d
#define HostCmd_RET_802_11_RSSI               0x801f
#define HostCmd_RET_802_11_RF_TX_POWER        0x801e
#define HostCmd_RET_802_11_RF_ANTENNA         0x8020
#define HostCmd_RET_802_11_PS_MODE            0x8021
#define HostCmd_RET_802_11_DATA_RATE          0x8022

#define HostCmd_RET_802_11_AD_HOC_START       0x802B
#define HostCmd_RET_802_11_AD_HOC_JOIN        0x802C

#define HostCmd_RET_802_11_QUERY_TKIP_REPLY_CNTRS  0x802e
#define HostCmd_RET_802_11_ENABLE_RSN              0x802f
#define HostCmd_RET_802_11_PAIRWISE_TSC       0x8036
#define HostCmd_RET_802_11_GROUP_TSC          0x8037
#define HostCmd_RET_802_11_KEY_MATERIAL       0x805e

#define HostCmd_ENABLE_RSN                    0x0001
#define HostCmd_DISABLE_RSN                   0x0000
#define TYPE_ANTENNA_DIVERSITY                0xffff

#define HostCmd_ACT_SET                       0x0001
#define HostCmd_ACT_GET                       0x0000

#define HostCmd_ACT_GET_AES                   (HostCmd_ACT_GET + 2)
#define HostCmd_ACT_SET_AES                   (HostCmd_ACT_SET + 2)
#define HostCmd_ACT_REMOVE_AES                (HostCmd_ACT_SET + 3)

#define HostCmd_RET_802_11_DEEP_SLEEP         0x803e

#define HostCmd_RET_802_11_SET_AFC            0x803c
#define HostCmd_RET_802_11_GET_AFC            0x803d

#define HostCmd_RET_802_11_AD_HOC_STOP        0x8040
#define HostCmd_RET_802_11_CAL_DATA_EXT      0x806d
#define HostCmd_RET_802_11_HOST_SLEEP_CFG   0x8043
#define HostCmd_RET_802_11_HOST_SLEEP_AWAKE_CONFIRM 0x8044
#define HostCmd_RET_802_11_WAKEUP_CONFIRM     0x8044
#define HostCmd_RET_802_11_HOST_SLEEP_ACTIVATE	0x8045
#define HostCmd_RET_802_11_BEACON_STOP        0x8049

#define HostCmd_RET_802_11_MAC_ADDRESS        0x804D
#define HostCmd_RET_802_11_EEPROM_ACCESS      0x8059



#define HostCmd_RET_802_11_BAND_CONFIG        0x8058

#define HostCmd_RET_802_11_SLEEP_PARAMS          0x8066

#define HostCmd_RET_802_11_INACTIVITY_TIMEOUT    0x8067

#define HostCmd_RET_802_11_SLEEP_PERIOD          0x8068
#define HostCmd_RET_802_11_BCA_CONFIG_TIMESHARE  0x8069

#define HostCmd_RET_802_11D_DOMAIN_INFO      (0x8000 |                  \
                                              HostCmd_CMD_802_11D_DOMAIN_INFO)

#define HostCmd_RET_WMM_ACK_POLICY           0x805C
#define HostCmd_RET_WMM_PRIO_PKT_AVAIL       0x805D


#define HostCmd_RET_802_11_BG_SCAN_CONFIG    0x806b
#define HostCmd_RET_802_11_BG_SCAN_QUERY     0x806c
#define HostCmd_RET_802_11_FW_WAKE_METHOD    0x8074
#define HostCmd_RET_WMM_GET_STATUS           0x8071
#define HostCmd_RET_WMM_ADDTS_REQ            0x806E
#define HostCmd_RET_WMM_DELTS_REQ            0x806F
#define HostCmd_RET_WMM_QUEUE_CONFIG         0x8070
#define HostCmd_RET_WMM_GET_STATUS           0x8071
#define HostCmd_RET_TX_PKT_STATS              0x808d
#define HostCmd_RET_802_11_TPC_CFG        (HostCmd_CMD_802_11_TPC_CFG | 0x8000)
#define HostCmd_RET_802_11_LED_CONTROL       0x804e
#define HostCmd_RET_802_11_PWR_CFG        (HostCmd_CMD_802_11_PWR_CFG | 0x8000)

#define HostCmd_RET_802_11_LED_GPIO_CTRL     0x804e

#define HostCmd_RET_802_11_FW_WAKEUP_METHOD	(HostCmd_CMD_802_11_FW_WAKEUP_METHOD | 0x8000)

#define HostCmd_RET_802_11_SUBSCRIBE_EVENT	(HostCmd_CMD_802_11_SUBSCRIBE_EVENT | 0x8000)

#define HostCmd_RET_802_11_RATE_ADAPT_RATESET	(HostCmd_CMD_802_11_RATE_ADAPT_RATESET | 0x8000)


#define HostCmd_RTE_802_11_TX_RATE_QUERY 	(HostCmd_CMD_802_11_TX_RATE_QUERY | 0x8000)
#define HostCmd_RET_802_11_IBSS_COALESCING_STATUS	0x8083
#define HostCmd_RET_GET_TSF             0x8080
#define HostCmd_RET_SDIO_PULL_CTRL		0x8093
#define HostCmd_RET_802_11_LDO_CONFIG         0x8096
#define HostCmd_RET_802_11_POWER_ADAPT_CFG_EXT	(HostCmd_CMD_802_11_POWER_ADAPT_CFG_EXT	| 0x8000)
#define HostCmd_RET_VERSION_EXT               0x8097
#define HostCmd_RET_WMM_QUEUE_STATS     	0x8081
typedef enum if_pkt_type {
   IF_DATA_PKT = 0,
   IF_CMD_PKT,
   IF_TX_DONE_EVENT,
   IF_MAC_EVENT,
   IF_INVALID_PKT_TYPE   //dralee, last entry of sdio_pkt_type
} IF_PKT_TYPE;




/** EventSubscribe*/
typedef struct _EventSubscribe {
	U16	Action;
	U16	Events;
	U8	RSSIValue;
	U8	RSSIFreq;
	U8	SNRValue;
	U8	SNRFreq;
	U8	FailValue;
	U8	FailFreq;
	U8	BcnMissed;
} EventSubscribe;

/*  802.11-related definitions */
typedef enum _mv_ms_type{
	MVMS_DAT = 0,
	MVMS_CMD = 1,
	MVMS_TXDONE = 2,
	MVMS_EVENT
} mv_ms_type;
/** TxPD descriptor */
typedef __packed struct _TxPD
{
	/** Current Tx packet status */
	U32	TxStatus;
	/** Tx Control */
	U32	TxControl;
	U32	TxPacketLocation;
	/** Tx packet length */
	U16	TxPacketLength;
	/** First 2 byte of destination MAC address */
	U8	TxDestAddrHigh[2];
	/** Last 4 byte of destination MAC address */
	U8	TxDestAddrLow[4];
	/** Pkt Priority */
	U8	Priority;
	/** Pkt Trasnit Power control*/
	U8	PowerMgmt;
	/** Amount of time the packet has been queued in the driver (units = 2ms)*/
	U8 PktDelay_2ms;
    /** Reserved */
    U8 Reserved1;
					
} TxPD, *PTxPD;


/** 2008.11.13 Zeet added */
typedef enum _BandSelection {
	S802_11b_2_4GHz = 0x0,
	S802_11g_2_4GHz = 0x1,
	S802_11a_5_GHz,
	S802_11j_4_GHz
}	BandSelection_e;

typedef enum _ChannelWidth {
	CHAN_WIDTH_20MHZ = 0,
	CHAN_WIDTH_10MHZ
}	ChannelWidth_e;

typedef enum _ChannelNumber {
	Channel_1 = 2412,
    Channel_2 = 2417,
    Channel_3 = 2422,
    Channel_4 = 2427,
    Channel_5 = 2432,
    Channel_6 = 2437,
    Channel_7 = 2442,
    Channel_8 = 2447,
    Channel_9 = 2452,
    Channel_10 = 2457,
    Channel_11 = 2462,
    Channel_12 = 2467,
    Channel_13 = 2472 
}	ChannelNum_e;
/* 2008.11.13 Zeet added fot band configuration */
typedef __packed struct _HostCmd_DS_802_11_BAND_CONFIG {
	
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	/** Action */
	U16	Action;	/*  0 - ACT_GET  1 - ACT_SET */
	
	/** Band Selection */
	BandSelection_e BandSelection;
	
	/** Channel Number */
	U16 ChannelNum;
	
	/** Channel Width */
	ChannelWidth_e ChannelWidth;
	
}	HostCmd_DS_802_11_BAND_CONFIG,
	*pHostCmd_DS_802_11_BAND_CONFIG;

/** RxPD Descriptor */
typedef __packed struct _RxPD {
	/** Current Rx packet status */
	U16	Status;
	
	/** SNR */
	U8	SNR;
	
	/** Tx Control */
	U8	RxControl;
	
	/** Pkt Length */
	U16	PktLen;
	
	/** Noise Floor */	
	U8	NF;
	
	/** Rx Packet Rate */
	U8	RxRate;
	
	/** Pkt addr*/
	U32	PktPtr;
	
	/** Next Rx RxPD addr */
	U32	NextRxPDPtr;
	
	/** Pkt Priority */
	U8	Priority;
	U8	Reserved[3];
				
} RxPD, *PRxPD;



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

#define WMI_WPA2_PSK_TKIP_PAIRWISE_OUI	 	0x02AC0F00
#define WMI_WPA2_PSK_AES_PAIRWISE_OUI		0x04AC0F00
#define WMI_WPA_PSK_AES_OUI					0x04f25000
#define	WMI_WPA_PSK_TKIP_OUI				0x02f25000

typedef __packed struct tBss_H
{
    U8     channel;
    U8     frameType;          /* see WMI_BI_FTYPE */
    U8	   snr;//	ver1.3
    S8     apRssi;//	ver1.3
    U8     bssid[6];  
    
}tWmiBssInfoHead_st;
#define IEEE_MAX_IE_SIZE  256
typedef __packed struct _tWmiBssInfoApName_st
{
    U8  length;
	char apName[32]; 
    
}tWmiBssInfoApName_st;

typedef __packed struct _tWmiBssInfoApSupportRate_st
{
    U8  length;
	U8 	dataRates[WLAN_SUPPORTED_RATES]; 
}tWmiBssInfoApSupportRate_st;

typedef __packed struct
{
    U8 ElementId;
    U8 Len;
    U8 Oui[3];
    U8 OuiType;
    U8 OuiSubtype;
    U8 Version;
} IEEEtypes_VendorHeader_t;

typedef __packed struct
{
    IEEEtypes_VendorHeader_t VendHdr;

    /* IE Max - size of previous fields */
    U8 Data[IEEE_MAX_IE_SIZE - sizeof(IEEEtypes_VendorHeader_t)];

}IEEEtypes_VendorSpecific_t;

typedef __packed struct
{
    U8 ElementId;
    U8 Len;
} IEEEtypes_Header_t;

typedef __packed struct
{
    U8 ElementId;
    U8 Len;

    /* IE Max - size of previous fields */
    U8 Data[IEEE_MAX_IE_SIZE - sizeof(IEEEtypes_Header_t)];

} EEEtypes_Generic_t;   



typedef __packed struct
{
    U8 Aifsn:4;
    U8 Acm:1;
    U8 Aci:2;
    U8 Reserved:1;
} IEEEtypes_WmmAciAifsn_t;

/**  data structure of WMM ECW */
typedef __packed struct
{
    U8 EcwMin:4;
    U8 EcwMax:4;
} IEEEtypes_WmmEcw_t;

/** data structure of WMM AC parameters  */
typedef __packed struct
{
    IEEEtypes_WmmAciAifsn_t AciAifsn;
    IEEEtypes_WmmEcw_t Ecw;
    U16 TxopLimit;
} IEEEtypes_WmmAcParameters_t;
/** data structure of WMM QoS information */
typedef __packed struct
{
    U8 ParaSetCount:4;
    U8 Reserved:3;
    U8 QosUAPSD:1;
} IEEEtypes_WmmQosInfo_t;
/** data structure of WMM parameter IE  */
typedef __packed struct
{
    /**
     * WMM Parameter IE - Vendor Specific Header:
     *   ElementId   [221/0xdd]
     *   Len         [24] 
     *   Oui         [00:50:f2]
     *   OuiType     [2]
     *   OuiSubType  [1]
     *   Version     [1]
     */
    IEEEtypes_VendorHeader_t VendHdr;

    IEEEtypes_WmmQosInfo_t QoSInfo;
    U8 Reserved;

    /** AC Parameters Record WMM_AC_BE, WMM_AC_BK, WMM_AC_VI, WMM_AC_VO */
    IEEEtypes_WmmAcParameters_t AcParams[MAX_AC_QUEUES];

} IEEEtypes_WmmParameter_t;



typedef __packed struct
{
    IEEEtypes_Header_t IeeeHdr;

    /* IE Max - size of previous fields */
    U8 Data[IEEE_MAX_IE_SIZE - sizeof(IEEEtypes_Header_t)];

}IEEEtypes_Generic_t;
/** IEEEtypes_SsParamSet_t */
typedef __packed struct 
{
	U8	ElementId;
	U8	Len;
	U8	CfpCnt;
	U8	CfpPeriod;
	U16	CfpMaxDuration;
	U16	CfpDurationRemaining;
} IEEEtypes_CfParamSet_t;

typedef __packed struct{
	U8	ElementId;
	U8	Len;
	U16	AtimWindow;
}  IEEEtypes_IbssParamSet_t;

typedef __packed struct _IEEEtypes_CapInfo_t {
	U8	Ess:1;
	U8	Ibss:1;
	U8	CfPollable:1;
	U8	CfPollRqst:1;
	U8	Privacy:1;
	U8	ShortPreamble:1;
	U8	Pbcc:1;
	U8	ChanAgility:1;
	U8	SpectrumMgmt:1;
	U8	Rsrvd3:1;
	U8	ShortSlotTime:1;
	U8	Apsd:1;
	U8	Rsvrd2:1;
	U8	DSSSOFDM:1;
	U8	Rsrvd1:2;
}  IEEEtypes_CapInfo_t;

typedef struct WMI_BECON_BUFF
{
	tWmiBssInfoApName_st ap;
	tWmiBssInfoHead_st	apBssInfo;
	eWMI_ENCRYPTION_TYPE 	wmiEncryptionType;//º”√‹∑Ω Ω0625
	U8 	apStatus;	//BIT0 :LINK STATUS;	0:UNLINKED; 1:LINKED
					//BIT1 :AP MODE			0:INFRA_NETWORK 1:ADHOC_NETWORK
	tWmiBssInfoApSupportRate_st supportRate;
	U8 extra_ie;
	IEEEtypes_CfParamSet_t cfParam;
	IEEEtypes_IbssParamSet_t ibssParam;
	U8 ERPFlags;
	IEEEtypes_CapInfo_t capInfo;
	
	IEEEtypes_VendorSpecific_t wpaIE;
    IEEEtypes_WmmParameter_t wmmIE;
    IEEEtypes_Generic_t rsnIE;

    IEEEtypes_VendorSpecific_t wpsIE;
    
    CSR_MLME_SCAN_INDICATION msi;
	ies_t   ies;
    
}tWMI_BECON_BUFF;

#define WIFI_MAX_BECON_BUF_NUM  (20)

typedef  struct WMI_PARSE_BECON
{
	struct llist_head list;
	tWMI_BECON_BUFF beconBuff;
	
}tWMI_PARSE_BECON;


#define MRVL_KEY_BUFFER_SIZE_IN_BYTE  16
/** MRVL_WEP_KEY */
typedef __packed struct _MRVL_WEP_KEY
{	
	U32 Length;
	U32 KeyIndex;
	U32 KeyLength;
	U8 KeyMaterial[MRVL_KEY_BUFFER_SIZE_IN_BYTE];
} MRVL_WEP_KEY, *PMRVL_WEP_KEY;

typedef struct _MRVL_WEP_KEY_SET
{	
	MRVL_WEP_KEY WepKey[4];
} MRVL_WEP_KEY_SET, *PMRVL_WEP_KEY_SET;


typedef long       WLAN_802_11_KEY_RSC;

#define WLAN_MAX_SSID_LENGTH		32
#define ETH_ALEN			6
typedef U8 WLAN_802_11_RATES[WLAN_SUPPORTED_RATES]; 
typedef U8 WLAN_802_11_MAC_ADDRESS[ETH_ALEN];
/** WPA Key LENGTH*/						    
/* Support 4 keys per key set */
#define MRVL_NUM_WPA_KEY_PER_SET        4
#define MRVL_MAX_WPA_KEY_LENGTH 	32
#define MRVL_MAX_KEY_WPA_KEY_LENGTH     32

#define WPA_AES_KEY_LEN 		16
#define WPA_TKIP_KEY_LEN 		32

#define MRVDRV_SIZE_OF_PPA		0x00000008
#define MRVDRV_SIZE_OF_DPA		0x00000008
#define MRVDRV_NUM_OF_TxPD		0x00000020
#define MRVDRV_MAX_MULTICAST_LIST_SIZE	32
#define MRVDRV_NUM_OF_CMD_BUFFER        10
#define MRVDRV_SIZE_OF_CMD_BUFFER       (2 * 1024) 
#define MRVDRV_MAX_CHANNEL_SIZE		14
#define MRVDRV_MAX_BSSID_LIST		64
#define MRVDRV_TIMER_10S		10000
#define MRVDRV_TIMER_5S			5000
#define MRVDRV_TIMER_1S			1000
#define MRVDRV_ASSOCIATION_TIME_OUT	255
#define MRVDRV_SNAP_HEADER_LEN          8
#define MRVDRV_ETH_ADDR_LEN             6
#define MRVDRV_ETH_HEADER_SIZE          14

#define	WLAN_UPLD_SIZE			2312



/** TLV  type ID definition */
#define PROPRIETARY_TLV_BASE_ID		0x0100

/* Terminating TLV Type */
#define MRVL_TERMINATE_TLV_ID		0xffff

#define TLV_TYPE_SSID				0x0000
#define TLV_TYPE_RATES				0x0001
#define TLV_TYPE_PHY_FH				0x0002
#define TLV_TYPE_PHY_DS				0x0003
#define TLV_TYPE_CF				    0x0004
#define TLV_TYPE_IBSS				0x0006

#define TLV_TYPE_DOMAIN				0x0007

#define TLV_TYPE_POWER_CAPABILITY	0x0021

#define TLV_TYPE_KEY_MATERIAL       (PROPRIETARY_TLV_BASE_ID + 0)
#define TLV_TYPE_CHANLIST           (PROPRIETARY_TLV_BASE_ID + 1)
#define TLV_TYPE_NUMPROBES          (PROPRIETARY_TLV_BASE_ID + 2)
#define TLV_TYPE_RSSI               (PROPRIETARY_TLV_BASE_ID + 4)
#define TLV_TYPE_SNR                (PROPRIETARY_TLV_BASE_ID + 5)
#define TLV_TYPE_FAILCOUNT          (PROPRIETARY_TLV_BASE_ID + 6)
#define TLV_TYPE_BCNMISS            (PROPRIETARY_TLV_BASE_ID + 7)
#define TLV_TYPE_LED_GPIO           (PROPRIETARY_TLV_BASE_ID + 8)
#define TLV_TYPE_LEDBEHAVIOR        (PROPRIETARY_TLV_BASE_ID + 9)
#define TLV_TYPE_PASSTHROUGH        (PROPRIETARY_TLV_BASE_ID + 10)
#define TLV_TYPE_REASSOCAP          (PROPRIETARY_TLV_BASE_ID + 11)
#define TLV_TYPE_POWER_TBL_2_4GHZ   (PROPRIETARY_TLV_BASE_ID + 12)
#define TLV_TYPE_POWER_TBL_5GHZ     (PROPRIETARY_TLV_BASE_ID + 13)
#define TLV_TYPE_BCASTPROBE	    (PROPRIETARY_TLV_BASE_ID + 14)
#define TLV_TYPE_NUMSSID_PROBE	    (PROPRIETARY_TLV_BASE_ID + 15)
#define TLV_TYPE_WMMQSTATUS   	    (PROPRIETARY_TLV_BASE_ID + 16)
#define TLV_TYPE_CRYPTO_DATA	    (PROPRIETARY_TLV_BASE_ID + 17)
#define TLV_TYPE_WILDCARDSSID	    (PROPRIETARY_TLV_BASE_ID + 18)
#define TLV_TYPE_TSFTIMESTAMP	    (PROPRIETARY_TLV_BASE_ID + 19)
#define TLV_TYPE_POWERADAPTCFGEXT   (PROPRIETARY_TLV_BASE_ID + 20)


#define TLV_TYPE_RSSI_HIGH          (PROPRIETARY_TLV_BASE_ID + 22) 
#define TLV_TYPE_SNR_HIGH           (PROPRIETARY_TLV_BASE_ID + 23) 
#define TLV_TYPE_AUTH_TYPE          (PROPRIETARY_TLV_BASE_ID + 31)



#define  RSN_IE_AES         1
#define  RSN_IE_TKIP        2


/* Define general purpose action */
#define HostCmd_ACT_GEN_READ                    0x0000
#define HostCmd_ACT_GEN_WRITE                   0x0001
#define HostCmd_ACT_GEN_GET                     0x0000
#define HostCmd_ACT_GEN_SET                     0x0001
#define HostCmd_ACT_GEN_REMOVE			0x0002
#define HostCmd_ACT_GEN_OFF                     0x0000
#define HostCmd_ACT_GEN_ON                      0x0001


/** IEEE 802.11 OIDs */
#define OID_802_11_BSSID                      0x00008001
#define OID_802_11_SSID                       0x00008002
#define OID_802_11_NETWORK_TYPES_SUPPORTED    0x00008003
#define OID_802_11_NETWORK_TYPE_IN_USE        0x00008004
#define OID_802_11_RSSI                       0x00008006
#define OID_802_11_RSSI_TRIGGER               0x00008007
#define OID_802_11_INFRASTRUCTURE_MODE        0x00008008
#define OID_802_11_FRAGMENTATION_THRESHOLD    0x00008009
#define OID_802_11_RTS_THRESHOLD              0x0000800A
#define OID_802_11_NUMBER_OF_ANTENNAS         0x0000800B
#define OID_802_11_RX_ANTENNA_SELECTED        0x0000800C
#define OID_802_11_TX_ANTENNA_SELECTED        0x0000800D
#define OID_802_11_SUPPORTED_RATES            0x0000800E
#define OID_802_11_DESIRED_RATES              0x00008010
#define OID_802_11_CONFIGURATION              0x00008011
#define OID_802_11_STATISTICS                 0x00008012
#define OID_802_11_ADD_WEP                    0x00008013
#define OID_802_11_REMOVE_WEP                 0x00008014
#define OID_802_11_DISASSOCIATE               0x00008015
#define OID_802_11_POWER_MODE                 0x00008016
#define OID_802_11_BSSID_LIST                 0x00008017
#define OID_802_11_AUTHENTICATION_MODE        0x00008018
#define OID_802_11_PRIVACY_FILTER             0x00008019
#define OID_802_11_BSSID_LIST_SCAN            0x0000801A
#define OID_802_11_WEP_STATUS                 0x0000801B
#define OID_802_11_RELOAD_DEFAULTS            0x0000801C
#define OID_802_11_TX_RETRYCOUNT              0x0000801D

#define OID_802_11D_ENABLE                    0x00008020


#define HostCmd_CMD_MFG_COMMAND               0x0080
#define HostCmd_RET_MFG_COMMAND               0x8080

/** Marvel specific OIDs */
#define OID_MRVL_OEM_GET_ULONG                0xff010201
#define OID_MRVL_OEM_SET_ULONG                0xff010202
#define OID_MRVL_OEM_GET_STRING               0xff010203
#define OID_MRVL_OEM_COMMAND                  0xff010204


/* Define action or option for HostCmd_CMD_802_11_SET_WEP */
#define HostCmd_ACT_ADD                         0x0002
#define HostCmd_ACT_REMOVE                      0x0004
#define HostCmd_ACT_USE_DEFAULT                 0x0008

#define HostCmd_TYPE_WEP_40_BIT                 0x0001
#define HostCmd_TYPE_WEP_104_BIT                0x0002

#define HostCmd_NUM_OF_WEP_KEYS                 4

#define HostCmd_WEP_KEY_INDEX_MASK              0x3fff

/** Card Event definition */
#define MACREG_INT_CODE_DUMMY_HOST_WAKEUP_SIGNAL       0x00000001
#define MACREG_INT_CODE_LINK_LOST_WITH_SCAN     0x00000002
#define MACREG_INT_CODE_LINK_LOST       		0x00000003
#define MACREG_INT_CODE_LINK_SENSED             0x00000004
#define MACREG_INT_CODE_MIB_CHANGED             0x00000006
#define MACREG_INT_CODE_INIT_DONE               0x00000007
#define MACREG_INT_CODE_DEAUTHENTICATED         0x00000008
#define MACREG_INT_CODE_DISASSOCIATED           0x00000009
#define MACREG_INT_CODE_PS_AWAKE                0x0000000a
#define MACREG_INT_CODE_PS_SLEEP                0x0000000b
#define MACREG_INT_CODE_MIC_ERR_MULTICAST       0x0000000d
#define MACREG_INT_CODE_MIC_ERR_UNICAST         0x0000000e
#define MACREG_INT_CODE_WM_AWAKE                0x0000000f
#define MACREG_INT_CODE_DEEP_SLEEP_AWAKE        0x00000010
#define MACREG_INT_CODE_ADHOC_BCN_LOST          0x00000011
#define MACREG_INT_CODE_HOST_SLEEP_AWAKE        0x00000012
#define MACREG_INT_CODE_WMM_STATUS_CHANGE		0x00000017
#define MACREG_INT_CODE_BG_SCAN_REPORT			0x00000018
#define MACREG_INT_CODE_RSSI_LOW				0x00000019
#define MACREG_INT_CODE_SNR_LOW					0x0000001a
#define MACREG_INT_CODE_MAX_FAIL				0x0000001b
#define MACREG_INT_CODE_RSSI_HIGH				0x0000001c
#define MACREG_INT_CODE_SNR_HIGH				0x0000001d
#define MACREG_INT_CODE_IBSS_COALESCED			0x0000001e

#define WEP_40_BIT_LEN	5
#define WEP_104_BIT_LEN	13

typedef __packed struct _RX_DATA_STRUCT
{
	U16 rxStatus;
	U8 	snr;
	U8	rxControl;
	U16 rxPacketLength;
	U8 	nf;
	U8 	rxRate;
	U32	rxPacketLocation;
	U32 reserved_1;
	U32 priority;
	U8 	payLoad[1];
}RX_DATA_STRUCT;

typedef __packed struct _TX_DATA_STRUCT
{
	U16 totalLength;
	U16 dataType;
	
	U32 txStatus;
	U32	txControl;
	U32	rxPacketLocation;
	U16 rxPacketLength;
	U16 TxDestAddrHigh;
	U32	TxDestAddrLow;
	U8	priority;
	U8  flags;
	U8  pktDelay_2ms;
	U8  reserved;
}TX_DATA_STRUCT;

typedef __packed struct ATH_MAC_HDR
{
    U8     dstMac[6];
    U8     srcMac[6];
    U16    typeOrLen;
} tATH_MAC_HDR; 

typedef __packed struct ATH_LLC_SNAP_HDR
{
    U8     dsap;
    U8     ssap;
    U8     cntl;
    U8     orgCode[3];
    U16    etherType;
}tATH_LLC_SNAP_HDR;


typedef enum _MV_PS_STATUS
{
	MV_PS_WAKEUP,
	MV_PS_SLEEP,
	MV_PS_DEEP_SLEEP
}MV_PS_STATUS;

typedef enum _MV_READY_STATUS
{
	MV_READY_UN_READY,
	MV_READY_READY,
	MV_READY_ERROR
}MV_READY_STATUS;
typedef enum _WLAN_STATUS
{
	WLAN_STATUS_NOT_CONNECT,
	WLAN_STATUS_CONNECT
}MV_WLAN_STATUS;

typedef enum _MV_NEXT_DO
{
	MV_NEXT_DO_IDDLE,
	MV_NEXT_DO_SEND_SCAN_QUERY_CMD,
	MV_NEXT_DO_WAKEUP_PROCESS_INT
}MV_NEXT_DO;

typedef enum _MV_INT_STATUS
{
	MV_INT_STATUS_NONE,
	MV_INT_STATUS_INT_REQ
}MV_INT_STATUS;

typedef enum 
{
	workType_normal,
	workType_netWork,
	workType_voip,
	workType_adhoc
} eWifiWorkType;

typedef struct _ADAPTER
{
	MV_PS_STATUS 	mvPsStatus;
	MV_READY_STATUS mvReadyStatus;
	MV_WLAN_STATUS 	mvWlanStatus;
	MV_NEXT_DO		nextDo;	
	
	MV_INT_STATUS	intReq;
	U8 txDone;
	U8 macAddr[6];
	
	U32 txCtrl;
	
	U16	seqNum;
	U16	oldSeqNum;
	
	S32 rssi;
	U8 	isCmdProcess;
	eWifiWorkType workType;
	eWifiWorkType lastWorkType;
	U8 tstFlag;
	U8 wifiPowerNum;
	U8 wifiScanFlag;
	U8 wifiEncrapt;
}ADAPTER,*PADAPTER;

typedef __packed struct _WLAN_802_11_KEY
{
	U32   Length;
	U32   KeyIndex;
	U32   KeyLength;
	WLAN_802_11_MAC_ADDRESS BSSID;
	WLAN_802_11_KEY_RSC KeyRSC;
	U8   KeyMaterial[MRVL_MAX_KEY_WPA_KEY_LENGTH];
}  WLAN_802_11_KEY, *PWLAN_802_11_KEY;


/** KEY_TYPE_ID */
typedef enum _KEY_TYPE_ID{
	KEY_TYPE_ID_WEP = 0,
	KEY_TYPE_ID_TKIP,
	KEY_TYPE_ID_AES
} KEY_TYPE_ID;
/** MRVL_WPA_KEY */
typedef __packed struct _MRVL_WPA_KEY {
	U32   KeyIndex;
	U32   KeyLength;
	U32	KeyRSC;
	U8   KeyMaterial[MRVL_MAX_KEY_WPA_KEY_LENGTH];
} MRVL_WPA_KEY, *PMRVL_WPA_KEY;

/** MRVL_WLAN_WPA_KEY */
typedef __packed struct _MRVL_WLAN_WPA_KEY {
	U8   EncryptionKey[16];
	U8   MICKey1[8];
	U8   MICKey2[8];
} MRVL_WLAN_WPA_KEY, *PMRVL_WLAN_WPA_KEY;

/** IE_WPA */
typedef __packed struct _IE_WPA {
	U8	Elementid;
	U8	Len;
	U8	oui[4];
	U16	version;
} IE_WPA,*PIE_WPA;

/** KEY_INFO_AES*/
typedef enum _KEY_INFO_AES
{
	KEY_INFO_AES_MCAST = 0x01,
	KEY_INFO_AES_UNICAST = 0x02,
	KEY_INFO_AES_ENABLED = 0x04
} KEY_INFO_AES;

/* Received Signal Strength Indication  in dBm*/


#define KEY_INFO_ENABLED	0x01

/** KEY_INFO_TKIP */
typedef enum _KEY_INFO_TKIP{
	KEY_INFO_TKIP_MCAST = 0x01,
	KEY_INFO_TKIP_UNICAST = 0x02,
	KEY_INFO_TKIP_ENABLED = 0x04
} KEY_INFO_TKIP;
/** WLAN_802_11_WEP */
typedef struct _WLAN_802_11_WEP
{
	/* Length of this structure */
	U32 Length;	
	
	/* 0 is the per-client key, 1-N are the global keys */
	U32 KeyIndex;		
	
	/* length of key in bytes */
	U32 KeyLength;		
	
	/* variable length depending on above field */
	U8 KeyMaterial[1];	
} WLAN_802_11_WEP, *PWLAN_802_11_WEP;

/** WLAN_802_11_SSID */
typedef __packed struct _WLAN_802_11_SSID {
	/* SSID Length*/
	U32 SsidLength;		
	
	/* SSID information field */
	U8 Ssid[WLAN_MAX_SSID_LENGTH];	
} WLAN_802_11_SSID, *PWLAN_802_11_SSID;

typedef __packed struct _WPA_SUPPLICANT {
	U8 	Wpa_ie[256];
	U8	Wpa_ie_len;
} WPA_SUPPLICANT, *PWPA_SUPPLICANT;


typedef U32 WLAN_802_11_FRAGMENTATION_THRESHOLD;
typedef U32 WLAN_802_11_RTS_THRESHOLD;
typedef U32 WLAN_802_11_ANTENNA;

/** wlan_offset_value */
typedef __packed struct _wlan_offset_value {
	U32	offset;
	U32	value;
} wlan_offset_value;

/** WLAN_802_11_FIXED_IEs */
typedef __packed struct _WLAN_802_11_FIXED_IEs {
	U8	Timestamp[8];
	U16	BeaconInterval;
	U16	Capabilities;
} WLAN_802_11_FIXED_IEs, *PWLAN_802_11_FIXED_IEs;

/** WLAN_802_11_VARIABLE_IEs */
typedef __packed struct _WLAN_802_11_VARIABLE_IEs {
	U8	ElementID;
	U8	Length;
	U8	data[1];
} WLAN_802_11_VARIABLE_IEs, *PWLAN_802_11_VARIABLE_IEs;

/** WLAN_802_11_AI_RESFI */
typedef __packed struct _WLAN_802_11_AI_RESFI {
	U16	Capabilities;
	U16	StatusCode;
	U16	AssociationId;
} WLAN_802_11_AI_RESFI, *PWLAN_802_11_AI_RESFI;

/** WLAN_802_11_AI_REQFI */
typedef __packed struct _WLAN_802_11_AI_REQFI {
	U16	Capabilities;
	U16	ListenInterval;
	WLAN_802_11_MAC_ADDRESS	CurrentAPAddress;
} WLAN_802_11_AI_REQFI, *PWLAN_802_11_AI_REQFI;


/* Define general data structure */
/** HostCmd_DS_GEN */
typedef __packed struct _HostCmd_DS_GEN {
    U16 Command;
    U16 Size;
    U16 SeqNum;
    U16 Result;
}  HostCmd_DS_GEN, *PHostCmd_DS_GEN
    ,HostCmd_DS_802_11_DEEP_SLEEP, *PHostCmd_DS_802_11_DEEP_SLEEP
    ;

#define S_DS_GEN    sizeof(HostCmd_DS_GEN)
/*
 * Define data structure for HostCmd_CMD_GET_HW_SPEC
 * This structure defines the response for the GET_HW_SPEC command
 */
/** HostCmd_DS_GET_HW_SPEC */
typedef __packed struct _HostCmd_DS_GET_HW_SPEC
{
	/** Command Header */
    U16 Command;
    U16 Size;
    U16 SeqNum;
    U16 Result;
	
	/* HW Interface version number */
	U16	HWIfVersion;
	
	/* HW version number */
	U16	Version;
	
	/* Max number of TxPD FW can handle*/
	U16	NumOfTxPD;
	
	/* Max no of Multicast address  */	
	U16	NumOfMCastAdr;
	
	/* MAC address */	
	U8	PermanentAddr[6];
	
	/* Region Code */
	U16	RegionCode;
	
	/* Number of antenna used */
	U16	NumberOfAntenna;
	
	/* FW release number, example 0x1234=1.2.3.4 */
	U32	FWReleaseNumber;
					
	/* Base Address of TxPD queue */
	U32	WcbBase; 	
	/* Read Pointer of RxPd queue */
	U32	RxPdRdPtr;
	
	/* Write Pointer of RxPd queue */	
	U32	RxPdWrPtr;
	
	/*FW/HW Capability*/
    U32 fwCapInfo;
} HostCmd_DS_GET_HW_SPEC, *PHostCmd_DS_GET_HW_SPEC;

/**  HostCmd_CMD_EEPROM_UPDATE */
typedef __packed struct _HostCmd_DS_EEPROM_UPDATE {
	U16	Action;
	U32	Value;
} HostCmd_DS_EEPROM_UPDATE, *PHostCmd_DS_EEPROM_UPDATE;

/**  HostCmd_CMD_802_11_RESET */
typedef __packed struct _HostCmd_DS_802_11_RESET
{
    U16 Action; 
} HostCmd_DS_802_11_RESET, *PHostCmd_DS_802_11_RESET;

/** HostCmd_DS_802_11_POWER_ADAPT_CFG_EXT */

typedef __packed struct _MrvlIEtypesHeader {
	U16	Type;
	U16	Len;
}  MrvlIEtypesHeader_t;
/** MrvlIEtypes_RssiParamSet_t */
typedef __packed struct _MrvlIEtypes_RssiThreshold_t 
{
	MrvlIEtypesHeader_t	Header;
	U8			RSSIValue;
	U8			RSSIFreq;
} MrvlIEtypes_RssiParamSet_t;

/** MrvlIEtypes_SnrThreshold_t */
typedef __packed struct _MrvlIEtypes_SnrThreshold_t {
	MrvlIEtypesHeader_t	Header;
	U8			SNRValue;
	U8			SNRFreq;
} MrvlIEtypes_SnrThreshold_t;

/** MrvlIEtypes_FailureCount_t */
typedef __packed struct _MrvlIEtypes_FailureCount_t {
	MrvlIEtypesHeader_t	Header;
	U8			FailValue;
	U8			FailFreq;
} MrvlIEtypes_FailureCount_t;

/** MrvlIEtypes_BeaconsMissed_t */
typedef __packed struct _MrvlIEtypes_BeaconsMissed_t {
	MrvlIEtypesHeader_t	Header;
	U8			BeaconMissed;
	U8			Reserved;
} MrvlIEtypes_BeaconsMissed_t;

typedef __packed struct _MrvlIEtypes_NumSSIDProbe_t 
{
    MrvlIEtypesHeader_t Header;
    U16          NumSSIDProbe;
} MrvlIEtypes_NumSSIDProbe_t;
//
// additional TLV data structures for background scan
//
typedef __packed struct _MrvlIEtypes_BcastProbe_t 
{
    MrvlIEtypesHeader_t Header;
    U16              BcastProbe;
} MrvlIEtypes_BcastProbe_t;

#define TLV_PAYLOAD_SIZE	2	
/* 
 * This scan handle Country Information IE(802.11d compliant) 
 * Define data structure for HostCmd_CMD_802_11_SCAN 
 */
typedef __packed  struct  _MrvlIEtypes_SsIdParamSet_t {
    MrvlIEtypesHeader_t Header;
 //   U8   SsId[1];
} MrvlIEtypes_SsIdParamSet_t;

/** CfParamSet_t */
typedef __packed struct _CfParamSet_t
{
    U8 CfpCnt;
    U8 CfpPeriod;
    U16 CfpMaxDuration;
    U16 CfpDurationRemaining;
} CfParamSet_t;



/** IEEE Type definitions  */
typedef enum _IEEEtypes_ElementId_e
{
    SSID = 0,
    SUPPORTED_RATES = 1,
    FH_PARAM_SET = 2,
    DS_PARAM_SET = 3,
    CF_PARAM_SET = 4,

    IBSS_PARAM_SET = 6,

    COUNTRY_INFO = 7,

    ERP_INFO = 42,
    EXTENDED_SUPPORTED_RATES = 50,

    VENDOR_SPECIFIC_221 = 221,
    WMM_IE = VENDOR_SPECIFIC_221,

    WPS_IE = VENDOR_SPECIFIC_221,

    WPA_IE = VENDOR_SPECIFIC_221,
    RSN_IE = 48,

    EXTRA_IE = 133
}  IEEEtypes_ElementId_e;

/** IbssParamSet_t */
typedef __packed struct _IbssParamSet_t
{
    U16 AtimWindow;
} IbssParamSet_t;

/** MrvlIEtypes_SsParamSet_t */
typedef __packed struct _MrvlIEtypes_SsParamSet_t
{
    MrvlIEtypesHeader_t Header;
    __packed union 
    {
        CfParamSet_t CfParamSet[1];
        IbssParamSet_t IbssParamSet[1];
    } cf_ibss;
} MrvlIEtypes_SsParamSet_t;

/** FhParamSet_t */
typedef __packed struct _FhParamSet_t
{
    U16 DwellTime;
    U8 HopSet;
    U8 HopPattern;
    U8 HopIndex;
} FhParamSet_t;
/** DsParamSet_t */
typedef __packed struct _DsParamSet_t
{
    U8 CurrentChan;
} DsParamSet_t;

/** MrvlIEtypes_PhyParamSet_t */
typedef __packed struct _MrvlIEtypes_PhyParamSet_t
{
    MrvlIEtypesHeader_t Header;
    __packed union
    {
        FhParamSet_t FhParamSet[1];
        DsParamSet_t DsParamSet[1];
    } fh_ds;
} MrvlIEtypes_PhyParamSet_t;


typedef __packed struct _ChanScanParamSet_t {
    U8   RadioType;
    U8   ChanNumber;
    U8   ScanType;
    U16  MinScanTime;
    U16  ScanTime;
} ChanScanParamSet_t;
typedef __packed struct  _MrvlIEtypes_ChanListParamSet_t {
    MrvlIEtypesHeader_t Header;
    ChanScanParamSet_t  ChanScanParam;
} MrvlIEtypes_ChanListParamSet_t; 


typedef __packed struct  _MrvlIEtypes_NumProbes_t {
    MrvlIEtypesHeader_t Header;
    U16              NumProbes;
} MrvlIEtypes_NumProbes_t;
//ahan [2005-12-09]

#define G_SUPPORTED_RATES		14
typedef __packed struct  _MrvlIEtypes_RatesParamSet_t {
    MrvlIEtypesHeader_t Header;
    U8   Rates[G_SUPPORTED_RATES];
} MrvlIEtypes_RatesParamSet_t;
/** Auth type to be used in the Authentication portion of an Assoc seq */
typedef __packed struct
{
    MrvlIEtypesHeader_t Header;
    U16 AuthType;
} MrvlIEtypes_AuthType_t;
/** MrvlIEtypes_RsnParamSet_t */
typedef __packed struct _MrvlIEtypes_RsnParamSet_t
{
    MrvlIEtypesHeader_t Header;
    U8 RsnIE[1];
} MrvlIEtypes_RsnParamSet_t;

typedef __packed struct  _MrvlIEtypes_WmmParamSet_t {
    MrvlIEtypesHeader_t Header;
    U8   WmmIE[1];
}  MrvlIEtypes_WmmParamSet_t;
typedef __packed struct  _MrvlIEtypes_WmmUapsd_t {
    MrvlIEtypesHeader_t Header;
    U8   WmmIE[1];
}  MrvlIEtypes_WmmUapsd_t; 

typedef __packed struct _HostCmd_DS_802_11_SCAN {
    U16  Command;
    U16  Size;
    U16  SeqNum;
    U16  Result;
    U8   BSSType;
    U8   BSSID[MRVDRV_ETH_ADDR_LEN];
    MrvlIEtypes_SsIdParamSet_t      SsIdParamSet;
    MrvlIEtypes_ChanListParamSet_t  ChanListParamSet;
    MrvlIEtypes_RatesParamSet_t     OpRateSet;

/*	
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U8	BSSType;
	U8	BSSID[ETH_ALEN];
	U8  TlvBuffer[1];
*/
	/* MrvlIEtypes_SsIdParamSet_t 	SsIdParamSet; 
	 * MrvlIEtypes_ChanListParamSet_t	ChanListParamSet;
	 * MrvlIEtypes_RatesParamSet_t 	OpRateSet; 
	 * */
}  HostCmd_DS_802_11_SCAN, *PHostCmd_DS_802_11_SCAN;

/**
 * @brief Structure used internally in the wlan driver to configure a scan.
 *
 * Sent to the command processing module to configure the firmware
 *   scan command prepared by wlan_cmd_802_11_scan.
 *
 * @sa wlan_scan_networks
 *
 */
typedef struct
{
    /**
     *  @brief BSS Type to be sent in the firmware command
     *
     *  Field can be used to restrict the types of networks returned in the
     *    scan.  Valid settings are:
     *
     *   - WLAN_SCAN_BSS_TYPE_BSS  (infrastructure)
     *   - WLAN_SCAN_BSS_TYPE_IBSS (adhoc)
     *   - WLAN_SCAN_BSS_TYPE_ANY  (unrestricted, adhoc and infrastructure)
     */
    U8 bssType;

    /**
     *  @brief Specific BSSID used to filter scan results in the firmware
     */
    U8 specificBSSID[MRVDRV_ETH_ADDR_LEN];

    /**
     *  @brief Length of TLVs sent in command starting at tlvBuffer
     */
    int tlvBufferLen;

    /**
     *  @brief SSID TLV(s) and ChanList TLVs to be sent in the firmware command
     *
     *  @sa TLV_TYPE_CHANLIST, MrvlIEtypes_ChanListParamSet_t
     *  @sa TLV_TYPE_SSID, MrvlIEtypes_SsIdParamSet_t
     */
    U8 tlvBuffer[1];            //!< SSID TLV(s) and ChanList TLVs are stored here
} wlan_scan_cmd_config;

typedef __packed struct _HostCmd_DS_802_11_SCAN_RSP 
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16	BSSDescriptSize;
	U8	NumberOfSets;
	U8  BssDescAndTlvBuffer[1];

} HostCmd_DS_802_11_SCAN_RSP,
    *PHostCmd_DS_802_11_SCAN_RSP;

/** HostCmd_CMD_802_11_GET_LOG */
typedef __packed struct _HostCmd_DS_802_11_GET_LOG {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U32   mcasttxframe;
	U32   failed;
	U32   retry;
	U32   multiretry;
	U32   framedup;
	U32   rtssuccess;
	U32   rtsfailure;
	U32   ackfailure;
	U32   rxfrag;
	U32   mcastrxframe;
	U32   fcserror;
	U32   txframe;
	U32   wepundecryptable;
}  HostCmd_DS_802_11_GET_LOG,
    *PHostCmd_DS_802_11_GET_LOG;

/**  HostCmd_CMD_MAC_CONTROL */
typedef __packed struct _HostCmd_DS_MAC_CONTROL
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16 Action;
	U16 Reserved;
}  HostCmd_DS_MAC_CONTROL, *PHostCmd_DS_MAC_CONTROL;

/**  HostCmd_CMD_MAC_MULTICAST_ADR */
typedef __packed struct _HostCmd_DS_MAC_MULTICAST_ADR {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;

    U16 Action;
    U16 NumOfAdrs;
    U8  MACList[MRVDRV_ETH_ADDR_LEN * MRVDRV_MAX_MULTICAST_LIST_SIZE];
} HostCmd_DS_MAC_MULTICAST_ADR,
    *PHostCmd_DS_MAC_MULTICAST_ADR;

/** HostCmd_CMD_802_11_AUTHENTICATE */
typedef __packed struct _HostCmd_DS_802_11_AUTHENTICATE {
	U8	MacAddr[ETH_ALEN];
	U8	AuthType;
	U8	Reserved[10];
} HostCmd_DS_802_11_AUTHENTICATE,
    *PHostCmd_DS_802_11_AUTHENTICATE;

/** HostCmd_RET_802_11_AUTHENTICATE */
typedef __packed struct _HostCmd_DS_802_11_AUTHENTICATE_RSP
{
    U8  MacAddr[6];
    U8  AuthType;
    U8  AuthStatus;
} HostCmd_DS_802_11_AUTHENTICATE_RSP,
    *PHostCmd_DS_802_11_AUTHENTICATE_RSP;

/**  HostCmd_CMD_802_11_DEAUTHENTICATE */
typedef __packed struct _HostCmd_DS_802_11_DEAUTHENTICATE
{
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
    U8  MacAddr[6];
    U16 ReasonCode;
} HostCmd_DS_802_11_DEAUTHENTICATE,
    *PHostCmd_DS_802_11_DEAUTHENTICATE;



/** HostCmd_DS_802_11_ASSOCIATE */
typedef __packed struct _HostCmd_DS_802_11_ASSOCIATE 
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U8				PeerStaAddr[6];
	IEEEtypes_CapInfo_t		CapInfo;
	U16			 	ListenInterval;
	U16				BcnPeriod;
	U8				DtimPeriod;
    


	/*
	 * 	MrvlIEtypes_SsIdParamSet_t 	SsIdParamSet;
	 * 	MrvlIEtypes_PhyParamSet_t	PhyParamSet;
	 *	MrvlIEtypes_SsParamSet_t	SsParamSet;
	 *	MrvlIEtypes_RatesParamSet_t	RatesParamSet;
	 */
} HostCmd_DS_802_11_ASSOCIATE,
    *PHostCmd_DS_802_11_ASSOCIATE;

/**  HostCmd_CMD_802_11_DISASSOCIATE */
typedef __packed struct _HostCmd_DS_802_11_DISASSOCIATE
{
	U8 DestMacAddr[6];
	U16 ReasonCode; 
} HostCmd_DS_802_11_DISASSOCIATE,
    *PHostCmd_DS_802_11_DISASSOCIATE;

/** HostCmd_RET_802_11_ASSOCIATE */


typedef U16 IEEEtypes_AId_t;
typedef U16 IEEEtypes_StatusCode_t;
typedef __packed struct 
{
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
    IEEEtypes_CapInfo_t Capability;
    IEEEtypes_StatusCode_t StatusCode;
    IEEEtypes_AId_t AId;
    U8 IEBuffer[1];
}  IEEEtypes_AssocRsp_t;

typedef __packed struct
{
    IEEEtypes_AssocRsp_t assocRsp;
} HostCmd_DS_802_11_ASSOCIATE_RSP;

/**  HostCmd_RET_802_11_AD_HOC_JOIN */
typedef __packed struct _HostCmd_DS_802_11_AD_HOC_RESULT
{
  	U8 PAD[3];
	U8 BSSID[MRVDRV_ETH_ADDR_LEN];
} HostCmd_DS_802_11_AD_HOC_RESULT,
    *PHostCmd_DS_802_11_AD_HOC_RESULT;

/**  HostCmd_CMD_802_11_SET_WEP */
typedef __packed struct _HostCmd_DS_802_11_SET_WEP
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	/* ACT_ADD, ACT_REMOVE or ACT_ENABLE  */
	U16 Action;  
	
	/* Key Index selected for Tx */
	U16 KeyIndex; 
	
	/* 40, 128bit or TXWEP */
	U8 WEPTypeForKey1;
	U8 WEPTypeForKey2;
	U8 WEPTypeForKey3;
	U8 WEPTypeForKey4;
	U8 WEP1[16];
	U8 WEP2[16];
	U8 WEP3[16];
	U8 WEP4[16];
} HostCmd_DS_802_11_SET_WEP,
    *PHostCmd_DS_802_11_SET_WEP;
/** MRVL_WEP_KEY 
typedef __packed struct _MRVL_WEP_KEY
{	
	U32 Length;
	U32 KeyIndex;
	U32 KeyLength;
	U8 KeyMaterial[MRVL_KEY_BUFFER_SIZE_IN_BYTE];
} MRVL_WEP_KEY, *PMRVL_WEP_KEY;*/
/**  HostCmd_CMD_802_3_GET_STAT */
typedef __packed struct _HostCmd_DS_802_3_GET_STAT
{
	U32 XmitOK;
	U32 RcvOK;
	U32 XmitError;
	U32 RcvError;
	U32 RcvNoBuffer;
	U32 RcvCRCError;
} HostCmd_DS_802_3_GET_STAT,
    *PHostCmd_DS_802_3_GET_STAT;

/** HostCmd_CMD_802_11_GET_STAT */
typedef __packed struct _HostCmd_DS_802_11_GET_STAT
{
	U32 TXFragmentCnt;
	U32 MCastTXFrameCnt;
	U32 FailedCnt;
	U32 RetryCnt;
	U32 MultipleRetryCnt;
	U32 RTSSuccessCnt;
	U32 RTSFailureCnt;
	U32 ACKFailureCnt;
	U32 FrameDuplicateCnt;
	U32 RXFragmentCnt;
	U32 MCastRXFrameCnt;
	U32 FCSErrorCnt;
	U32 BCastTXFrameCnt;
	U32 BCastRXFrameCnt;
	U32 TXBeacon;
	U32 RXBeacon;
	U32 WEPUndecryptable;
} HostCmd_DS_802_11_GET_STAT,
    *PHostCmd_DS_802_11_GET_STAT;


/** HostCmd_DS_802_11_BEACON_STOP 
typedef __packed struct _HostCmd_DS_802_11_BEACON_STOP {

} HostCmd_DS_802_11_BEACON_STOP, 
    *PHostCmd_DS_802_11_BEACON_STOP;*/

/**  HostCmd_CMD_802_11_SNMP_MIB */
typedef __packed struct _HostCmd_DS_802_11_SNMP_MIB
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;

	U16 QueryType;	
	U16 OID;	
	U16 BufSize;	
	U8 Value[128];
} HostCmd_DS_802_11_SNMP_MIB,
    *PHostCmd_DS_802_11_SNMP_MIB;

/**  HostCmd_CMD_MAC_REG_MAP */
typedef __packed struct _HostCmd_DS_MAC_REG_MAP
{
	U16 BufferSize;
	U8 RegMap[128];
	U16 Reserved;
} HostCmd_DS_MAC_REG_MAP, *PHostCmd_DS_MAC_REG_MAP;

/*  HostCmd_CMD_BBP_REG_MAP */
typedef __packed struct _HostCmd_DS_BBP_REG_MAP
{
	U16 BufferSize;
	U8 RegMap[128];
	U16 Reserved;
} HostCmd_DS_BBP_REG_MAP, *PHostCmd_DS_BBP_REG_MAP;

/** HostCmd_CMD_RF_REG_MAP */
typedef __packed struct _HostCmd_DS_RF_REG_MAP
{
	U16 BufferSize;
	U8 RegMap[64];
	U16 Reserved;
} HostCmd_DS_RF_REG_MAP, *PHostCmd_DS_RF_REG_MAP;

/** HostCmd_CMD_MAC_REG_ACCESS */
typedef __packed struct _HostCmd_DS_MAC_REG_ACCESS
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;	
	
	U16 Action;
	U16 Offset;
	U32 Value;
} HostCmd_DS_MAC_REG_ACCESS,
    *PHostCmd_DS_MAC_REG_ACCESS;

/** HostCmd_CMD_BBP_REG_ACCESS */
typedef __packed struct _HostCmd_DS_BBP_REG_ACCESS
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16 Action;
	U16 Offset;
	U8 Value;
	U8 Reserved[3];
} HostCmd_DS_BBP_REG_ACCESS,
    *PHostCmd_DS_BBP_REG_ACCESS;

/**  HostCmd_CMD_RF_REG_ACCESS */
typedef __packed struct _HostCmd_DS_RF_REG_ACCESS
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;	
	
	U16 Action;
	U16 Offset;
	U8 Value;
	U8 Reserved[3];
} HostCmd_DS_RF_REG_ACCESS,
    *PHostCmd_DS_RF_REG_ACCESS;

/** HostCmd_CMD_802_11_RADIO_CONTROL */
typedef __packed struct _HostCmd_DS_802_11_RADIO_CONTROL
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16 Action;
	U16 Control;
}  HostCmd_DS_802_11_RADIO_CONTROL,
    *PHostCmd_DS_802_11_RADIO_CONTROL;

/* HostCmd_DS_802_11_SLEEP_PARAMS */
typedef __packed struct _HostCmd_DS_802_11_SLEEP_PARAMS
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	/* ACT_GET/ACT_SET */
	U16 	Action;		
	
	/* Sleep clock error in ppm */
	U16 	Error;		
	
	/* Wakeup offset in usec */
	U16 	Offset; 	
	
	/* Clock stabilization time in usec */
	U16 	StableTime; 	
	
	/* Control periodic calibration */
	U8 	CalControl; 	
	
	/* Control the use of external sleep clock */
	U8 	ExternalSleepClk;
	
	/* Reserved field, should be set to zero */
	U16 	Reserved; 
}  HostCmd_DS_802_11_SLEEP_PARAMS,
    *PHostCmd_DS_802_11_SLEEP_PARAMS;

/* HostCmd_DS_802_11_SLEEP_PERIOD */
typedef __packed struct _HostCmd_DS_802_11_SLEEP_PERIOD
{
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
		
	/* ACT_GET/ACT_SET */
	U16 	Action;	
	
	/* Sleep Period in msec */
	U16 	Period; 
}  HostCmd_DS_802_11_SLEEP_PERIOD,
    *PHostCmd_DS_802_11_SLEEP_PERIOD;

/* HostCmd_DS_802_11_BCA_TIMESHARE */
typedef __packed struct _HostCmd_DS_802_11_BCA_TIMESHARE
{
    /* ACT_GET/ACT_SET */ 
    U16     Action;     
    
    /* Type: WLAN, BT */
    U16     TrafficType;   
    
    /* 20msec - 60000msec */
    U32     TimeShareInterval; 
    
    /* PTA arbiter time in msec */  
    U32     BTTime;    
} HostCmd_DS_802_11_BCA_TIMESHARE,
    *PHostCmd_DS_802_11_BCA_TIMESHARE;

/* HostCmd_DS_802_11_INACTIVITY_TIMEOUT */
typedef __packed struct _HostCmd_DS_802_11_INACTIVITY_TIMEOUT
{
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
    
    /* ACT_GET/ACT_SET */   
    U16     Action;     
    
    /* Inactivity timeout in msec */
    U16     Timeout;    
} HostCmd_DS_802_11_INACTIVITY_TIMEOUT,
    *PHostCmd_DS_802_11_INACTIVITY_TIMEOUT;

/** HostCmd_CMD_802_11_RF_CHANNEL */
typedef __packed struct _HostCmd_DS_802_11_RF_CHANNEL
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16	Action;
	U16	CurrentChannel;
//	U16	RFType;
//	U16	Reserved;
//	U8	ChannelList[32];
}  HostCmd_DS_802_11_RF_CHANNEL,
    *PHostCmd_DS_802_11_RF_CHANNEL;

/**  HostCmd_CMD_802_11_RSSI */
typedef __packed struct _HostCmd_DS_802_11_RSSI {
	    
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	/* weighting factor */
	U16	N;  
	
	U16	Reserved_0;
	U16	Reserved_1;
	U16	Reserved_2;
}  HostCmd_DS_802_11_RSSI, *PHostCmd_DS_802_11_RSSI;


typedef __packed struct _HostCmd_DS_802_11_AUTHEN {
	    
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	/* weighting factor */
	U8  mac[6];  
	
	U8	authType;
}  HostCmd_DS_802_11_AUTHEN, *PHostCmd_DS_802_11_AUTHEN;

typedef __packed struct _HostCmd_DS_802_11_SUBSCRIBE_EVENT 
{
    U16      Command;
    U16      Size;
    U16      SeqNum;
    U16      Result;
    U16      Action;
    U16      Events;
    MrvlIEtypes_RssiParamSet_t      RssiLow;
    MrvlIEtypes_SnrThreshold_t      SnrLow;
    MrvlIEtypes_FailureCount_t      FailCnt;
    MrvlIEtypes_BeaconsMissed_t     BcnMiss;
    MrvlIEtypes_RssiParamSet_t      RssiHigh;
    MrvlIEtypes_SnrThreshold_t      SnrHigh;    
} HostCmd_DS_802_11_SUBSCRIBE_EVENT, *PHostCmd_DS_802_11_SUBSCRIBE_EVENT;


#define DEFAULT_BCN_AVG_FACTOR		8
#define DEFAULT_DATA_AVG_FACTOR		8
#define MIN_BCN_AVG_FACTOR		1
#define MAX_BCN_AVG_FACTOR		8
#define MIN_DATA_AVG_FACTOR		1
#define MAX_DATA_AVG_FACTOR		8
#define AVG_SCALE			100

typedef enum _SNRNF_TYPE
{
    TYPE_BEACON = 0,
    TYPE_RXPD,
    MAX_TYPE_B
} SNRNF_TYPE;

/** SNRNF_DATA*/
typedef enum _SNRNF_DATA
{
    TYPE_NOAVG = 0,
    TYPE_AVG,
    MAX_TYPE_AVG
} SNRNF_DATA;

/** HostCmd_DS_802_11_RSSI_RSP */ 
typedef __packed struct _HostCmd_DS_802_11_RSSI_RSP {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16	SNR;
	U16	NoiseFloor;
	U16	AvgSNR;
	U16	AvgNoiseFloor;
} HostCmd_DS_802_11_RSSI_RSP, *PHostCmd_DS_802_11_RSSI_RSP;

/** HostCmd_DS_802_11_MAC_ADDRESS */
typedef __packed struct _HostCmd_DS_802_11_MAC_ADDRESS {
        /** Command Header */
    U16 Command;
    U16 Size;
    U16 SeqNum;
    U16 Result;
    
    U16 Action;
    U8  MacAdd[ETH_ALEN];
}  HostCmd_DS_802_11_MAC_ADDRESS,
    *PHostCmd_DS_802_11_MAC_ADDRESS;

/** HostCmd_CMD_802_11_RF_TX_POWER */
typedef __packed struct _HostCmd_DS_802_11_RF_TX_POWER {
    U16 Action;
    U16 CurrentLevel;
}  HostCmd_DS_802_11_RF_TX_POWER,
    *PHostCmd_DS_802_11_RF_TX_POWER;

/** HostCmd_CMD_802_11_RF_ANTENNA */
typedef __packed struct _HostCmd_DS_802_11_RF_ANTENNA {
    U16 Action;

    /*  Number of antennas or 0xffff(diversity) */
    U16 AntennaMode;           
    
}  HostCmd_DS_802_11_RF_ANTENNA,
    *PHostCmd_DS_802_11_RF_ANTENNA;

/** HostCmd_CMD_802_11_PS_MODE */
typedef __packed struct _HostCmd_DS_802_11_PS_MODE
{
		/** Command Header */
    U16 Command;
    U16 Size;
    U16 SeqNum;
    U16 Result;
    
	U16 Action;
	U16 NullPktInterval;
	U16 MultipleDtim;
	U16 Reserved;
	U16 LocalListenInterval;
}  HostCmd_DS_802_11_PS_MODE,
    *PHostCmd_DS_802_11_PS_MODE;

/** PS_CMD_ConfirmSleep */
typedef __packed struct _PS_CMD_ConfirmSleep {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;

	U16	Action;
	U16	Reserved1;
	U16	MultipleDtim;
	U16	Reserved;
	U16 LocalListenInterval;
}  PS_CMD_ConfirmSleep, *PPS_CMD_ConfirmSleep;

/** HostCmd_CMD_802_11_FW_WAKEUP_METHOD */
typedef __packed struct _HostCmd_DS_802_11_FW_WAKEUP_METHOD
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16 Action;
	U16 Method;
}  HostCmd_DS_802_11_FW_WAKEUP_METHOD,
  *PHostCmd_DS_802_11_FW_WAKEUP_METHOD;
typedef __packed struct _HostCmd_DS_802_11_SET_REGION_CODE
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16 Action;
	U16 RegionCode;
}  HostCmd_DS_802_11_SET_REGION_CODE,
  *PHostCmd_DS_802_11_SET_REGION_CODE;
#define HOSTCMD_SUPPORTED_RATES 14
/** HostCmd_CMD_802_11_DATA_RATE */
typedef __packed struct _HostCmd_DS_802_11_DATA_RATE {
	U16	Action;
	U16	Reserverd;
	U8	DataRate[HOSTCMD_SUPPORTED_RATES];
}  HostCmd_DS_802_11_DATA_RATE,
    *PHostCmd_DS_802_11_DATA_RATE;

/** HostCmd_DS_802_11_RATE_ADAPT_RATESET */
typedef __packed struct _HostCmd_DS_802_11_RATE_ADAPT_RATESET {
	U16	Action;
	U16	EnableHwAuto;
	U16	Bitmap;
}  HostCmd_DS_802_11_RATE_ADAPT_RATESET,
	*PHostCmd_DS_802_11_RATE_ADAPT_RATESET;

#define MRVDRV_MAX_SSID_LENGTH			32
#define MRVDRV_MAX_BSS_DESCRIPTS		16
#define MRVDRV_MAX_REGION_CODE			6

#define MRVDRV_IGNORE_MULTIPLE_DTIM		0xfffe
#define MRVDRV_MIN_MULTIPLE_DTIM		1
#define MRVDRV_MAX_MULTIPLE_DTIM		5
#define MRVDRV_DEFAULT_MULTIPLE_DTIM		1

#define MRVDRV_DEFAULT_LISTEN_INTERVAL		10
#define MRVDRV_DEFAULT_LOCAL_LISTEN_INTERVAL		0
#define MRVDRV_MAX_TX_POWER		      	15

#define	MRVDRV_CHANNELS_PER_SCAN		4
#define	MRVDRV_MAX_CHANNELS_PER_SCAN		14

#define	MRVDRV_CHANNELS_PER_ACTIVE_SCAN		14

/** HostCmd_DS_802_11_AD_HOC_START*/



typedef __packed union _IEEEtypes_SsParamSet_t {
	IEEEtypes_CfParamSet_t		CfParamSet;
	IEEEtypes_IbssParamSet_t	IbssParamSet;
} IEEEtypes_SsParamSet_t;


typedef __packed struct _IEEEtypes_DsParamSet_t {
	U8	ElementId;
	U8	Len;
	U8	CurrentChan;
}  IEEEtypes_DsParamSet_t;

typedef __packed struct _IEEEtypes_FhParamSet_t {
	U8	ElementId;
	U8	Len;
	U16	DwellTime;
	U8	HopSet;
	U8	HopPattern;
	U8	HopIndex;
}  IEEEtypes_FhParamSet_t;

typedef __packed union   
{
	IEEEtypes_FhParamSet_t	FhParamSet;
	IEEEtypes_DsParamSet_t	DsParamSet;
}  IEEEtypes_PhyParamSet_t;

typedef __packed struct _HostCmd_DS_802_11_AD_HOC_START
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U8 SSID[MRVDRV_MAX_SSID_LENGTH];
	U8 BSSType;
	U16 BeaconPeriod;
	U8 DTIMPeriod;
	IEEEtypes_SsParamSet_t SsParamSet;
	IEEEtypes_PhyParamSet_t PhyParamSet;
	U16 ProbeDelay;
	IEEEtypes_CapInfo_t Cap;
	U8 DataRate[HOSTCMD_SUPPORTED_RATES];
    U8 tlv_memory_size_pad[100];
}  HostCmd_DS_802_11_AD_HOC_START,
    *PHostCmd_DS_802_11_AD_HOC_START;

typedef __packed struct _HostCmd_DS_802_11_AD_HOC_STOP
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
}  HostCmd_DS_802_11_AD_HOC_STOP,
    *PHostCmd_DS_802_11_AD_HOC_STOP;    

/** AdHoc_BssDesc_t */
typedef __packed struct  _AdHoc_BssDesc_t {
	U8	BSSID[6];
	U8	SSID[32];
	U8	BSSType;
	U16	BeaconPeriod;
	U8	DTIMPeriod;
	U8	TimeStamp[8];
	U8	LocalTime[8];
	IEEEtypes_PhyParamSet_t	PhyParamSet;
	IEEEtypes_SsParamSet_t	SsParamSet;
	IEEEtypes_CapInfo_t	Cap;
	U8  DataRates[HOSTCMD_SUPPORTED_RATES];
	
	/* DO NOT ADD ANY FIELDS TO THIS STRUCTURE.	 It is used below in the
	 *	Adhoc join command and will cause a binary layout mismatch with 
	 *	the firmware 
	 */
}  AdHoc_BssDesc_t;


/** HostCmd_DS_802_11_AD_HOC_JOIN */
typedef __packed struct _HostCmd_DS_802_11_AD_HOC_JOIN
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	AdHoc_BssDesc_t BssDescriptor;
	U16 FailTimeOut;
	U16 ProbeDelay;
       
}  HostCmd_DS_802_11_AD_HOC_JOIN,
  *PHostCmd_DS_802_11_AD_HOC_JOIN;

/** HostCmd_DS_802_11_ENABLE_RSN */
typedef __packed struct _HostCmd_DS_802_11_ENABLE_RSN 
{
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U16                  Action;	
	U16                  Enable;
} HostCmd_DS_802_11_ENABLE_RSN,
    *PHostCmd_DS_802_11_ENABLE_RSN;

/** HostCmd_DS_802_11_QUERY_TKIP_REPLY_CNTRS */
typedef __packed struct _HostCmd_DS_802_11_QUERY_TKIP_REPLY_CNTRS {
	U16                  CmdCode;
	U16                  Size;
	U16                  SeqNum;
	U16                  Result;
	U32                  NumTkipCntrs;
} HostCmd_DS_802_11_QUERY_TKIP_REPLY_CNTRS,
    *PHostCmd_DS_802_11_QUERY_TKIP_REPLY_CNTRS;

/** HostCmd_DS_802_11_PAIRWISE_TSC */
typedef __packed struct _HostCmd_DS_802_11_PAIRWISE_TSC {
	U16                  CmdCode;
	U16                  Size;
	U16                  SeqNum;
	U16                  Result;
    U16                  Action;
    U32                  Txlv32;
    U16                  Txlv16;
} HostCmd_DS_802_11_PAIRWISE_TSC, 
    *PHostCmd_DS_802_11_PAIRWISE_TSC;

/** HostCmd_DS_802_11_GROUP_TSC */
typedef __packed struct _HostCmd_DS_802_11_GROUP_TSC {
	U16                  CmdCode;
	U16                  Size;
	U16                  SeqNum;
	U16                  Result;
	U16                  Action;
	U32                  Txlv32;
	U16                  Txlv16;
} HostCmd_DS_802_11_GROUP_TSC, 
    *PHostCmd_DS_802_11_GROUP_TSC;

typedef __packed union _KeyInfo_WEP_t {
    U8  Reserved;           
    
    /* bits 1-4: Specifies the index of key */
    U8  WepKeyIndex;        
    
    /* bit 0: Specifies that this key is 
     * to be used as the default for TX data packets 
     * */
    U8  isWepDefaultKey;   
}  KeyInfo_WEP_t;

typedef __packed union _KeyInfo_TKIP_t {
    U8  Reserved;           
    
    /* bit 2: Specifies that this key is 
     * enabled and valid to use */
    U8  isKeyEnabled;       
    
    /* bit 1: Specifies that this key is
     * to be used as the unicast key */
    U8  isUnicastKey;       
    
    /* bit 0: Specifies that this key is 
     * to be used as the multicast key */
    U8  isMulticastKey;     
}  KeyInfo_TKIP_t;

typedef __packed union _KeyInfo_AES_t {
    U8  Reserved;           

    /* bit 2: Specifies that this key is
     * enabled and valid to use */
    U8  isKeyEnabled;       

    /* bit 1: Specifies that this key is
     * to be used as the unicast key */
    U8  isUnicastKey;       
    
    /* bit 0: Specifies that this key is 
     * to be used as the multicast key */
    U8  isMulticastKey;     
}  KeyInfo_AES_t;

/** KeyMaterial_TKIP_t */
typedef __packed struct _KeyMaterial_TKIP_t {
    /* TKIP encryption/decryption key */
    U8  TkipKey[16];            
    
    /* TKIP TX MIC Key */
    U8  TkipTxMicKey[16];      
    
    /* TKIP RX MIC Key */
    U8  TkipRxMicKey[16];       
}  KeyMaterial_TKIP_t,
    *PKeyMaterial_TKIP_t;

/** KeyMaterial_AES_t */
typedef __packed struct _KeyMaterial_AES_t {
    /* AES encryption/decryption key */
    U8  AesKey[16];         
}  KeyMaterial_AES_t,
    *PKeyMaterial_AES_t;

/** MrvlIEtype_KeyParamSet_t */
typedef __packed struct _MrvlIEtype_KeyParamSet_t {
    /* Type ID */
    U16 Type;       
    
    /* Length of Payload */
    U16 Length;     
    
    /* Type of Key: WEP=0, TKIP=1, AES=2 */
    U16 KeyTypeId;  
    
    /* Key Control Info specific to a KeyTypeId */
    U16 KeyInfo;    
    
    /* Length of key */
    U16 KeyLen;     
    
    /* Key material of size KeyLen */
    U8  Key[32];    
}  MrvlIEtype_KeyParamSet_t,
    *PMrvlIEtype_KeyParamSet_t;

/** HostCmd_DS_802_11_KEY_MATERIAL */
typedef __packed struct _HostCmd_DS_802_11_KEY_MATERIAL 
{
    
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
    
    U16             Action;     
    
    MrvlIEtype_KeyParamSet_t	KeyParamSet;	
}  HostCmd_DS_802_11_KEY_MATERIAL,
    *PHostCmd_DS_802_11_KEY_MATERIAL;

/** HostCmd_DS_802_11_HOST_SLEEP_CFG */
typedef __packed struct _HostCmd_DS_HOST_802_11_HOST_SLEEP_CFG 
{
    U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
       /* bit0=1: non-unicast data
        * bit1=1: unicast data
        * bit2=1: mac events
     	* bit3=1: magic packet 
	*/    
	U32 conditions;  
    U8  gpio;
	/* in milliseconds */
    U8  gap;
}  HostCmd_DS_802_11_HOST_SLEEP_CFG;

/** HostCmd_DS_802_11_CAL_DATA_EXT */
typedef __packed struct _HostCmd_DS_802_11_CAL_DATA_EXT {
	U16	Action;
	U16	Revision;
	U16	CalDataLen;
	U8	CalData[1024]; 
}  HostCmd_DS_802_11_CAL_DATA_EXT,
	*pHostCmd_DS_802_11_CAL_DATA_EXT;


/** HostCmd_DS_802_11_EEPROM_ACCESS */
typedef __packed struct _HostCmd_DS_802_11_EEPROM_ACCESS {
    U16 Action;
    
   /* multiple 4 */ 
    U16 Offset;
    U16 ByteCount;
    U8  Value; 
}  HostCmd_DS_802_11_EEPROM_ACCESS,
    *pHostCmd_DS_802_11_EEPROM_ACCESS;



/** HostCmd_DS_802_11_BG_SCAN_CONFIG */
typedef __packed struct _HostCmd_DS_802_11_BG_SCAN_CONFIG  {
	
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	/** Action */
	U16	Action;
	
	/** Enable */
	/*  0 - Disable 1 - Enable */
	U8	Enable;	
	
	/** bssType */
	/*  1 - Infrastructure
	 *  2 - IBSS
	 *  3 - any 
	*/
	U8	BssType;
	
	/** ChannelsPerScan */
	/* No of channels to scan at one scan */
	U8	ChannelsPerScan;
	
	/* 0 - Discard old scan results
	 * 1 - Discard new scan results 
	 */
	U8	DiscardWhenFull;
	
	U16 	Reserved;
	
	/** ScanInterval */
	U32	ScanInterval;
	
	/** StoreCondition */
	/* - SSID Match
	 * - Exceed RSSI threshold
	 * - SSID Match & Exceed RSSI Threshold 
	 * - Always 
	*/	
	U32	StoreCondition;	
	
	/** ReportConditions */
	/* - SSID Match
	 * - Exceed RSSI threshold
	 * - SSID Match & Exceed RSSIThreshold
	 * - Exceed MaxScanResults
	 * - Entire channel list scanned once 
	 * - Domain Mismatch in country IE 
	 */
	U32	ReportConditions;	

	/** MaxScanResults */
	/* Max scan results that will trigger 
	 * a scn completion event */
	U16	MaxScanResults;
	
	/*	attach TLV based parameters as needed, e.g.
	*	MrvlIEtypes_SsIdParamSet_t 	SsIdParamSet;
	*	MrvlIEtypes_ChanListParamSet_t	ChanListParamSet;
	*	MrvlIEtypes_NumProbes_t		NumProbes;
	*/
//	U8 TlvData[1];
}  HostCmd_DS_802_11_BG_SCAN_CONFIG,
	*pHostCmd_DS_802_11_BG_SCAN_CONFIG;

/** HostCmd_DS_802_11_BG_SCAN_QUERY */
typedef __packed struct _HostCmd_DS_802_11_BG_SCAN_QUERY {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	U8	Flush;
}  HostCmd_DS_802_11_BG_SCAN_QUERY,
	*pHostCmd_DS_802_11_BG_SCAN_QUERY;

/** HostCmd_DS_802_11_BG_SCAN_QUERY_RSP */
typedef __packed struct _HostCmd_DS_802_11_BG_SCAN_QUERY_RSP {
	U16	Command;
	U16	Size;
	U16	SeqNum;
	U16	Result;
	
	U32	ReportCondition;
	U16	BSSDescriptSize;
	U8	NumberOfSets;
	U8  BssDescAndTlvBuffer[1];
//	HostCmd_DS_802_11_SCAN_RSP	scanresp;
}  HostCmd_DS_802_11_BG_SCAN_QUERY_RSP,
				*PHostCmd_DS_802_11_BG_SCAN_QUERY_RSP;


/** HostCmd_DS_802_11_TPC_CFG */
typedef __packed struct _HostCmd_DS_802_11_TPC_CFG {
	U16   Action;
	U8    Enable;
	S8    P0;
	S8    P1;
	S8    P2;
	U8    UseSNR;
}  HostCmd_DS_802_11_TPC_CFG;

/** HostCmd_DS_802_11_LED_CTRL */
typedef __packed struct _HostCmd_DS_802_11_LED_CTRL {
  U16 Action;
  U16 NumLed;
  U8 data[256];
}  HostCmd_DS_802_11_LED_CTRL;

/*** HostCmd_DS_802_11_PWR_CFG */
typedef __packed struct _HostCmd_DS_802_11_PWR_CFG {
	U16   Action;
	U8    Enable;
	S8    PA_P0;
	S8    PA_P1;
	S8    PA_P2;
}  HostCmd_DS_802_11_PWR_CFG;


typedef __packed struct 	_PA_Group_t {
	U16			PowerAdaptLevel;
	U16			RateBitmap;
	U32			Reserved;
}  PA_Group_t;
 
#define	MAX_POWER_ADAPT_GROUP		5

typedef __packed struct 	_MrvlIEtypes_PowerAdapt_Group_t {
	MrvlIEtypesHeader_t	Header;
	PA_Group_t		PA_Group[MAX_POWER_ADAPT_GROUP];
} MrvlIEtypes_PowerAdapt_Group_t;

typedef __packed struct _HostCmd_DS_802_11_POWER_ADAPT_CFG_EXT {
	/** Action */
	U16	Action;		/* 0 = ACT_GET; 1 = ACT_SET; */
	U16	EnablePA;	/* 0 = disable; 1 = enable; */
	MrvlIEtypes_PowerAdapt_Group_t 	PowerAdaptGroup;
}  HostCmd_DS_802_11_POWER_ADAPT_CFG_EXT,
	*PHostCmd_DS_802_11_POWER_ADAPT_CFG_EXT;

/** HostCmd_DS_802_11_AFC */


typedef  struct _HostCmd_DS_802_11_AFC 
{
    U16 afc_auto;
    union 
    {
        struct {
            U16 threshold;
            U16 period;
        } auto_mode;

        struct 
        {
            S16 timing_offset;
            S16 carrier_offset;
        } manual_mode;
    }b;
}  HostCmd_DS_802_11_AFC;

typedef __packed struct _HostCmd_TX_RATE_QUERY
{
    U16 TxRate; 
}  HostCmd_TX_RATE_QUERY, *PHostCmd_TX_RATE_QUERY;

typedef __packed struct {
	U64 TsfValue;
}  HostCmd_DS_GET_TSF;

/** _HostCmd_DS_COMMAND*/
struct  _HostCmd_DS_COMMAND 
{

    /** Command Header */
    U16 Command;
    U16 Size;
    U16 SeqNum;
    U16 Result;

    /** Command Body */
    union {
        HostCmd_DS_GET_HW_SPEC               hwspec;
        HostCmd_DS_802_11_PS_MODE            psmode;
        HostCmd_DS_802_11_SCAN               scan;
        HostCmd_DS_802_11_SCAN_RSP           scanresp;
        HostCmd_DS_MAC_CONTROL               macctrl;
        HostCmd_DS_802_11_ASSOCIATE          associate;
        HostCmd_DS_802_11_ASSOCIATE_RSP      associatersp;
        HostCmd_DS_802_11_DEAUTHENTICATE     deauth; 
        HostCmd_DS_802_11_SET_WEP            wep;
        HostCmd_DS_802_11_AD_HOC_START       ads;
        HostCmd_DS_802_11_RESET              reset;
        HostCmd_DS_802_11_AD_HOC_RESULT      result;
        HostCmd_DS_802_11_GET_LOG            glog;
        HostCmd_DS_802_11_AUTHENTICATE       auth;
        HostCmd_DS_802_11_AUTHENTICATE_RSP   rauth;
        HostCmd_DS_802_11_GET_STAT           gstat;
        HostCmd_DS_802_3_GET_STAT            gstat_8023;
        HostCmd_DS_802_11_SNMP_MIB           smib;
        HostCmd_DS_802_11_RF_TX_POWER        txp;
        HostCmd_DS_802_11_RF_ANTENNA         rant;
        HostCmd_DS_802_11_DATA_RATE          drate;
        HostCmd_DS_802_11_RATE_ADAPT_RATESET rateset;
        HostCmd_DS_MAC_MULTICAST_ADR         madr;
        HostCmd_DS_802_11_AD_HOC_JOIN        adj;
        HostCmd_DS_802_11_RADIO_CONTROL      radio;
        HostCmd_DS_802_11_RF_CHANNEL         rfchannel;
        HostCmd_DS_802_11_RSSI               rssi;
        HostCmd_DS_802_11_RSSI_RSP           rssirsp;
        HostCmd_DS_802_11_DISASSOCIATE       dassociate;
  //      HostCmd_DS_802_11_AD_HOC_STOP        adhoc_stop;
        HostCmd_DS_802_11_MAC_ADDRESS        macadd;
        HostCmd_DS_802_11_ENABLE_RSN         enbrsn;
        HostCmd_DS_802_11_KEY_MATERIAL       keymaterial;
        HostCmd_DS_MAC_REG_ACCESS            macreg;
        HostCmd_DS_BBP_REG_ACCESS            bbpreg;
        HostCmd_DS_RF_REG_ACCESS             rfreg;
 //       HostCmd_DS_802_11_BEACON_STOP        beacon_stop;
        HostCmd_DS_802_11_CAL_DATA_EXT       caldataext;
        HostCmd_DS_802_11_HOST_SLEEP_CFG     hostsleepcfg;
        HostCmd_DS_802_11_EEPROM_ACCESS      rdeeprom;

    
 //       HostCmd_DS_802_11D_DOMAIN_INFO       domaininfo;
//        HostCmd_DS_802_11D_DOMAIN_INFO_RSP   domaininforesp;
        HostCmd_DS_802_11_BG_SCAN_CONFIG     bgscancfg;
        HostCmd_DS_802_11_BG_SCAN_QUERY      bgscanquery;
        HostCmd_DS_802_11_BG_SCAN_QUERY_RSP  bgscanqueryresp;
 //       HostCmd_DS_WMM_GET_STATUS            getWmmStatus;
 //       HostCmd_DS_WMM_ACK_POLICY            ackpolicy;

        HostCmd_DS_802_11_SLEEP_PARAMS       sleep_params;
        HostCmd_DS_802_11_BCA_TIMESHARE      bca_timeshare;
        HostCmd_DS_802_11_INACTIVITY_TIMEOUT inactivity_timeout;
        HostCmd_DS_802_11_SLEEP_PERIOD       ps_sleeppd;
        HostCmd_DS_802_11_TPC_CFG            tpccfg;
        HostCmd_DS_802_11_PWR_CFG            pwrcfg;
        HostCmd_DS_802_11_AFC                afc;
        HostCmd_DS_802_11_LED_CTRL           ledgpio;
        HostCmd_DS_802_11_FW_WAKEUP_METHOD   fwwakeupmethod;
        HostCmd_DS_802_11_SUBSCRIBE_EVENT    events;

        HostCmd_TX_RATE_QUERY                txrate;	
        HostCmd_DS_GET_TSF                   gettsf;
    } params;
} ;


//extern struct llist_head tApList;

#endif