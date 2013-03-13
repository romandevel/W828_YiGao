 /*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "TcpIp_p.h"
#include "net_api.h"

#pragma arm section zidata = "tcpip_memory"
//char gu8tcpip_memory[(1024*512)];
char gu8tcpip_memory[(1024*1024)];
#pragma arm section

int TcpIp_ThreadMethod(void)
{
    int res = -1;
    
    TcpIp_InitMemory_buf();
    
    while(res != NET_SUCCESS)
    {
    	//res = Net_Init(gu8tcpip_memory,sizeof(gu8tcpip_memory));//提供协议栈运行的数据空间
    	res = Net_Init2(gu8tcpip_memory,sizeof(gu8tcpip_memory));//提供协议栈运行的数据空间
    }
    /*if(res != NET_SUCCESS)
    {//初始化内存出错
        while(1);
    }*/
    
    Net_Main(); 

    return 1;
        
}

void *EMACGetBuf(U16 len)
{
	return (void *)Dev_WifiAllocPack(len);
}

void EMACNotifyReceiveEnd(S8 flag,S16 len)
{
	Dev_WifiRecvPack(0,0);
}