#ifndef _WIFI_API_H_
#define _WIFI_API_H_

#include "hyTypes.h"

#include "hy_unifi.h"


#define MAX_AP_COUNT	20

typedef enum _hy_ap_cmd
{
	HY_APCMD_GETMAC,
	HY_APCMD_MACCTR,
	HY_APCMD_SCAN,
	HY_APCMD_BGSCAN,
	HY_APCMD_STOPSCAN,
	HY_APCMD_CONNECT,
	HY_APCMD_DISCONNECT,
	HY_APCMD_RECONNECT,
	HY_APCMD_RSSI,
	HY_APCMD_ENTPS,
	HY_APCMD_EXITPS,
	HY_APCMD_CREAT_ADHOC,
	HY_APCMD_JOIN_ADHOC,
	HY_APCMD_STOP_ADHOC,
	HY_APCMD_MULTICAST_ADDR,
	HY_APCMD_SNMP,
	
	HY_APCMD_SIP_ENTPS,
	HY_APCMD_SIP_EXITPS,
	HY_APCMD_END
	
}eHY_APCMD;


typedef struct _tHY_CRT_ADHOC
{
    U8 ssid[33];//adhoc名称
    U8 ssidLen;//名称长度
    U8 channel;//通道号
}tHY_CRT_ADHOC;

typedef struct _tHY_JOIN_ADHOC
{
    U8 bssid[6]; //连接的mac
    U8 ssid[33];//连接的名字
    U8 ssidLen;//名字的长度
    U8 channel;//通道号  
}tHY_JOIN_ADHOC;

extern U8 gu8Wifi_ConnectState;//wifi连接状态 0:未连接  1:已连接
//extern U8 gApSortedInfo[];

//wifi网络类型 不包括GSM
#define AP_NET_TYPE_UNKNOW  0
#define AP_NET_TYPE_WIFI    1
#define AP_NET_TYPE_ADHOC   2

#define AP_LIST_REFRESH     1
#define AP_LIST_PART_DEL    2


extern struct llist_head tApList;


#endif
