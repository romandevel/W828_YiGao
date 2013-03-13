#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "EventEncumberInit.h"
#include "TcpIp_p.h"
#include "net_api.h"
#include "std.h"

int hyFdSet(int n, int p)
{}

int hyFdClr(int n, int p)
{}

int hyFdIsSet(int n, int p)
{}

void inet_ntoa()
{}

int Net_Init(U8 *pBuf,U32 BufLen)
{}

void Net_Close()
{}

int Net_Active()
{}

int Net_DeActive()
{}

U16 Net_GetState()
{}

int Net_AddDev(U16 Id,tNET_DEV *pDev)
{}

void Net_RemoveDev(U16 Id)
{}

int Net_ActiveDev(U16 Id)
{}

int Net_DeActiveDev(U16 Id)
{}

int Net_GetDev(U16 Id,tNET_DEV *pDev)
{}

int Net_GetDevState(U16 Id,tNET_STATE *pState)
{}

int Net_ModifyDevMac(U16 Id,U8 *pEtAddr)
{}

void inet_addr()
{}

void Net_SetDefaultRoute()
{}

void hySelect()
{}

void hyRecvfrom()
{}

void hySendto()
{}






void hySend()
{}

void hyRecv()
{}

int hySocket()
{}

void hySetsockopt()
{}

void hyGetHostByName()
{}

void hyClose()
{}

void hyConnect()
{}

void inet_aton()
{}

void *EMACGetBuf(U16 len)
{return NULL;}

void EMACNotifyReceiveEnd(S8 flag,S16 len)
{}

int TcpIp_ThreadMethod(void)
{exit(1);}