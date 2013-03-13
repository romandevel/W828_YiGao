#ifndef NET_API_H
#define NET_API_H


//device id
#define NET_IF_WIFI 0
#define NET_IF_ADHOC 1
#define NET_IF_GSM 2

//error code
#define NET_SUCCESS 0
#define NET_ERROR -1
#define NET_ERR_INVALID -2
#define NET_ERR_MEMORY -3
#define NET_ERR_PARAM -4
#define NET_ERR_HANDLE -5
#define NET_ERR_INSUFF_RESOURCE -6
#define NET_ERR_UNSUPPORT -7


//net state
#define NET_STATE_CLOSE 0
#define NET_STATE_ACTIVE 1
#define NET_STATE_DEACTIVE 2
#define NET_STATE_CLOSING 3
#define NET_STATE_ACTIVING 4
#define NET_STATE_DEACTIVING 5

#define NET_STATE_EMPTY 10
#define NET_STATE_FINISH 11
#define NET_STATE_ERROR 12


//device state
#define IF_DEV_STATE_CLOSE 0
#define IF_DEV_STATE_ACTIVE 1
#define IF_DEV_STATE_DEACTIVE 2
#define IF_DEV_STATE_CLOSING 3
#define IF_DEV_STATE_ACTIVING 4
#define IF_DEV_STATE_DEACTIVING 5

#define IF_DEV_STATE_EMPTY 10
#define IF_DEV_STATE_FINISH 11
#define IF_DEV_STATE_ERROR 12

//dhcp state
#define IF_ADDR_STATE_STOP 0
#define IF_ADDR_STATE_OK 1
#define IF_ADDR_STATE_CONFIG 2
#define IF_ADDR_STATE_ERROR 3


typedef struct net_dev
{
	U8		DHCP;
	
	U8		EtAddr[6];

	U8		*pHostName;
	
	U32		IpAddr;
	U32		Mask;
	U32		Gateway;
	U32		DNS0;
	U32		DNS1;
}tNET_DEV;

typedef struct net_state
{
	U16		DevState;
	U16		AddrState;
}tNET_STATE;


int Net_Init(U8 *pBuf,U32 BufLen);
void Net_Close();
int Net_Active();
int Net_DeActive();
U16 Net_GetState();

int Net_AddDev(U16 Id,tNET_DEV *pDev);
void Net_RemoveDev(U16 Id);
int Net_ActiveDev(U16 Id);
int Net_DeActiveDev(U16 Id);
int Net_GetDev(U16 Id,tNET_DEV *pDev);
int Net_GetDevState(U16 Id,tNET_STATE *pState);
int Net_ModifyDevMac(U16 Id,U8 *pEtAddr);

#endif