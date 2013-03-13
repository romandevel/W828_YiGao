#ifndef _WIFI_USER_H_
#define _WIFI_USER_H_

#include "hyTypes.h"
#include "wmi.h"

#define DATA_BUFF_NUM 			16
#define WIFIDRIVER_SENDBUFF_LENGTH 		1536		//too large
#define WIFIDRIVER_RECVBUFF_LENGTH 		1536		//too large
#define WIFI_PS
//#define BLOCK256

typedef __packed struct _WMI_DATA_BUFF
{
	U32 *pDataSend;
	U32 dataSendLength; 
}tWMI_DATA_BUFF;

typedef __packed struct _WMI_DATA_SEND
{
	U8 dataBuffSend;
	U8 dataBuffRecv; 
	U8 dataBuffNum;
	U8 reserved;
	tWMI_DATA_BUFF dataBuff[DATA_BUFF_NUM];  //THE SEND BUFF
}tWMI_DATA_SEND;

extern U32 gSendDataBuff[ WIFIDRIVER_SENDBUFF_LENGTH ];
extern tWMI_DATA_SEND gDataBuffSend;
extern U32 sdioDataTemp[512];
extern const unsigned char fmimage[];
extern const unsigned char helperimage[];
extern ADAPTER adapter;
#endif
