#include "hyTypes.h"
#include "glbVariable.h"
#include "TcpIp_p.h"
#include "http.h"
#include "ppp_gprs.h"
#include "socket_api.h"
#include "time.h"
#include "semaphore.h"
#include "hyhwRtc.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "Op_Config.h"


#ifdef VER_CONFIG_HYCO


#define   NTP_PORT            123             //NTP 协议端口号
#define   NTP_PACKET_LEN      48              //NTP 请求数据包为48字节
#define   NTP_SERVER_IP1      "210.72.145.44" // NTP 服务器网络地址 （国家时间授权中心）
#define   NTP_SERVER_IP2      "202.120.2.101" // NTP 服务器网络地址(上海交通大学网络中心NTP服务器地址)  


//是否已经同步时间 0x00:未同步    0x01:正在同步    0x02:成功      0x82:成功且已提示    0x03:失败     0x83:失败且已提示
U8 gu8UpDateTime=0;
char gas8FailReason[50]={0};	//同步时间失败原因

static U8 gServSelcFlag;//服务器地址选择标志 

#define UPDATETIME_P_NAME		"updatetime_p"

SEMAPHORE *UpdateTime_p_Exit_sem = NULL;

typedef struct ntp { 
    char flag;                        
    char stratum; 
    char poll; 
    char precision; 
    char rootDelay[4]; 
    char rootDispersion[4]; 
    char refId[4]; 
    char refTime[8]; 
    char originateTime[8]; 
    char recvTime[8]; 
    char transmitTime[8]; 
    
}tNTP;


void SetUpDateState(U8 type)
{
	gu8UpDateTime = type;
}

U8 GetUpDateState()
{
	return gu8UpDateTime;
}

/*---------------------------------------------
* 函数:UpDateTime_SetFailReason
* 功能:设置同步时间失败原因失败的原因
* 参数:pReason:原因
* 返回:none
----------------------------------------------*/
void UpDateTime_SetFailReason(U8 *pReason)
{
	int len;
	
	memset(gas8FailReason, 0, sizeof(gas8FailReason));
	
	len = strlen(pReason);
	if(len >= 50)
	{
		len = 49;
	}
	memcpy(gas8FailReason, pReason, len);
}

/*---------------------------------------------
* 函数:UpDateTime_GetFailReason
* 功能:获取同步时间失败的原因
* 参数:nome
* 返回:原因
----------------------------------------------*/
U8	*UpDateTime_GetFailReason()
{
	return gas8FailReason;
}

/*-----------------------------------------
*函数: Comm_TimeStr2Rtc
*功能: 把字符串形式时间转换成结构体形式 2010-01-11 15:22:12
*参数: 
*返回: 
*------------------------------------------*/
void Comm_TimeStr2Rtc(U8 *pTime, RtcTime_st *pRtc)
{
	U32 i, res, len, datalen;
	U8  *pStr;

	memset(pRtc,0,sizeof(RtcTime_st));
	
	datalen = strlen(pTime);

	//年
	pStr = strstr(pTime,"-");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->year = (U16)res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//数据完
	{
		return ;
	}
	
	//月
	pStr = strstr(pTime,"-");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->month = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//数据完
	{
		return ;
	}
	
	//日
	pStr = strstr(pTime," ");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->day = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//数据完
	{
		return ;
	}
	
	//时
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->hour = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//数据完
	{
		return ;
	}
	
	//分
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->minute = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//数据完
	{
		return ;
	}
	
	//秒
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->second = res;

	return ;
}

/*-----------------------------------------------------------------------
* 函数: SetSyn_GetNetworkTime
* 利用NTP协议同步网络时间
* @param newTime OUT 网络获取的最新时间/
* @param otimeSec IN 超时时间,在otimeSec秒内,服务器无回应
* @param ser_selFlag 服务器选择标志 0：NTP_SERVER_IP1  1：NTP_SERVER_IP2
* @ret SUCCEED 1 FAILED -1
 -----------------------------------------------------------------------*/
int SetSyn_GetNetworkTime(time_t *newTime, int otimeSec,U8 ser_selFlag)
{
    struct sockaddr_in recvAddr; 
    struct sockaddr_in sendAddr; 
    int sendAddrSize, sock;
    time_t recvTime; 
    struct timeval to;   
    fd_set fs;   
	tNTP pdu; 
	
	sendAddrSize = sizeof(sendAddr);
        
    memset(&pdu, '\0', sizeof(tNTP));
	pdu.flag = 0x1b;
	pdu.transmitTime[0] = 0xc7;
	pdu.transmitTime[1] = 0x50;
	pdu.transmitTime[2] = 0xe7;
	pdu.transmitTime[3] = 0xa0;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (sock < 0)
    { 
    	return -1;
    }

    recvAddr.sin_family = AF_INET; 
    recvAddr.sin_port = htons(NTP_PORT); 
    if(0 == ser_selFlag)
    {
    	 recvAddr.sin_addr.s_addr = inet_addr(NTP_SERVER_IP1); 
    }
    else if(1 == ser_selFlag)
    {
    	 recvAddr.sin_addr.s_addr = inet_addr(NTP_SERVER_IP2); 
    }
   

    sendto(sock, (char *)&pdu, NTP_PACKET_LEN, 0, (struct sockaddr *) &recvAddr, sizeof(recvAddr)); 

	to.tv_sec = otimeSec;   
    to.tv_usec = 0;   
        
    FD_ZERO(&fs);   
    FD_SET(sock, &fs);   

	if (select(sock, &fs, 0, 0, &to) > 0)
    {
    	recvfrom(sock, (char *)&pdu, NTP_PACKET_LEN, 0, (struct sockaddr *)&sendAddr, &sendAddrSize); 
   		closesocket(sock);
    }
    else 
    {
	    closesocket(sock);
		return -1;
    }
    
    memcpy(&recvTime, pdu.recvTime, 4); 
    recvTime = ntohl(recvTime) - 2208988800;   //起始于1900年 = 2208988800
    *newTime = recvTime;  
    
    return 1;
}

//2:成功   3:失败   0:网络原因失败
int SetSyn_NetTime(void)
{
	time_t		netTime;
	struct tm	*tt;
	RtcTime_st	SNetTime;
	int			ret=0;
//U32 tick = rawtime(NULL);

	gServSelcFlag = 0;
	ret = SetSyn_GetNetworkTime(&netTime, 3,gServSelcFlag);
	if(ret == -1)//更新出错:ret=-1   更新成功:ret=1
	{
		gServSelcFlag = 1;
		ret = SetSyn_GetNetworkTime(&netTime, 3,gServSelcFlag);
	}
//hyUsbPrintf("updatetime ret = %d   = %d \r\n",ret, ABS(rawtime(NULL)-tick));	
	if(ret == 1)
	{
		tt = localtime(&netTime);

		SNetTime.second	= tt->tm_sec;
		SNetTime.minute	= tt->tm_min;
		SNetTime.hour	= tt->tm_hour;
		SNetTime.day    = tt->tm_mday;
		SNetTime.month	= tt->tm_mon+1;
		SNetTime.year	= tt->tm_year+1900;
		
        if( SNetTime.year<1950 || SNetTime.year>2085 || SNetTime.month<1 || SNetTime.month>12 ||
            SNetTime.day<1     || SNetTime.day>31    || SNetTime.hour<0  || SNetTime.hour>24  || 
            SNetTime.minute<0  || SNetTime.minute>59 || SNetTime.second<0|| SNetTime.second>59 ) 
        {//更新的时间超出日历所支持的时间范围
			ret = -1; 
        }
        else 
        {
			hyhwRtc_SetTime(&SNetTime);
			ret = 2;
		}
	}
	else
	{
		ret = 0;
	}
	
	return ret;
}


#endif //#ifdef VER_CONFIG_HYCO