/*-----------------------------------------------------------
*
*gps管理主要是管理三个情况
*1.根据设置自动定位,关系到低功耗
*2.手动定位,考虑到和1不冲突
*3.收到去硬件测试可以归并为2统一为请求坐标信息
*
*------------------------------------------------------------*/


#include "glbVariable.h"
#include "ospriority.h"
#include "sys.h"
#include "Msgqueue.h"
#include "GPS_Parse.h"

#define GPS_VALID_MAXNUM		60
#define GPS_RUN_TIMES           4  /* gps关闭的调整时间数 主要是为了低功耗而设计 */
#define GPS_CLOSE_TIMES         4  /* gps关闭的调整时间数 主要是为了低功耗而设计 */
#define GPS_LAT_CMP            (3) /* 纬度比较的位数 */
#define GPS_LONG_CMP           (4) /* 经度比较的位数 */
static tMSG_QUEUE *ptGpsMsg ;/* 消息队列 */
static uDATA *ptGpsmSend ;  /* 记录获取的gps信息     */
static U8  su8GpsmGetState; /* gps是否定位成功的标志 1 成功 0 未定位到 */
extern U8  su8GpsWorkMode;  /* gps工作模式: bit[0]=1 自动定位上传 bit[1]=1 手动定位  */
static int sGpsRunTick   ;  /* 记录gps工作模式1运行的Tick数 */
static U32 su8GpsRunTimes[GPS_RUN_TIMES] = 
			{30000,18000,6000,3500};/* 5min,3min,1min,20s,10s */
static U32 su8GpsCloseTimes[GPS_CLOSE_TIMES] = 
			{30000,60000,90000,120000};/* 5min,10min,15min,20min */

static U8  su8RunIndex,su8CloseIndex,getGpsCnt;
static int saveGet,saveSend,gDelayTick;
static tGPGGA stGpgga ;
static U8 gu8Lat[8],gu8Long[8];

static U32 gu32GetGpsInfo = 0 ;

void gpsm_sendMsg(U8 onoff);
/*-------------------------------
* 函数:gpsm_int()
* 功能:
* 参数:none
* 返回:
*-------------------------------*/
void gpsm_int(void)
{
	memset(&stGpgga,0,sizeof(tGPGGA));
	ptGpsmSend = (uDATA *)hsaSdram_GetGpsBuf();
	memset((U8 *)ptGpsmSend,0,hsaSdram_GetGpsBufSize());
	saveGet  = rawtime(NULL);
	saveSend = rawtime(NULL);
	su8RunIndex = 0;
	su8CloseIndex = 0xFF ;
	getGpsCnt    = 0;
	su8GpsWorkMode=0;
	su8GpsmGetState=0;
	gDelayTick = 0 ;
	
	memset(gu8Lat ,0,8);
	memset(gu8Long,0,8);
}
/*-------------------------------
* 函数:gpsm_setState()
* 功能:
* 参数:none
* 返回:
*-------------------------------*/
void gpsm_setState(tGPGGA *pt)
{
	wlock((unsigned)&gu32GetGpsInfo);
	memcpy(&stGpgga,pt,sizeof(tGPGGA));
	YDhtest_gps(pt);
	su8GpsmGetState = 1 ;
	unlock((unsigned)&gu32GetGpsInfo);
}

/*-------------------------------
* 函数:gpsm_CommEvent
* 功能:
* 参数:none
* 返回:
*-------------------------------*/
void gpsm_CommEvent(void)
{
	int getTime,sendTime ;
	int curTime,i;
	
	if(gDelayTick > 0 && su8CloseIndex < GPS_CLOSE_TIMES)
	{	
		/* 延时定位的时间以免信号不好一直定位不到而无法进入低功耗 */
		if(ABS(rawtime(NULL)-gDelayTick) >= su8GpsCloseTimes[su8CloseIndex])
		{
			gDelayTick = 0 ;
		}
		
		return ;
	}
	
	
	if(gtHyc.GPSSwitch != 0) /* 允许使用gps 并统一做gps的开启操作 */
	{
		if(1 == net_onlyRead_regStatus()) /* sim卡可用才开启自动定位上传 */
		{
			getTime  = atoi(gtHyc.GPSGetTime );
			sendTime = atoi(gtHyc.GPSSendTime);
			getTime = (getTime >=1 && getTime <= 60) ? getTime : 3 ; /* 判断获取时间的正确性 */
			sendTime= (sendTime>=3 && sendTime<= 60) ? sendTime: 3 ; /* 判断发送时间的正确性 */	
			
			if(ABS(rawtime(NULL) - saveGet) >= getTime*6000)	/* 获取gps信息 */
			{
				saveGet = rawtime(NULL) ; /* 计时更新 */
				su8GpsWorkMode |= 0x01 ;
				gpsm_sendMsg(1);          /* 启动gps */
			}
			
			if(ABS(rawtime(NULL) - saveSend) >= sendTime*6000) /* 将gps信息写入发送队列 */
			{
				saveSend = rawtime(NULL) ; /* 计时更新 */
				if(getGpsCnt > 0)
				{
					for(i = 0 ; i < getGpsCnt ; i++)
					{
						YDnet_add2Que(0,0,POSTGPS,&ptGpsmSend[i]);
					}
					memset((U8 *)ptGpsmSend,0,hsaSdram_GetGpsBufSize());
					getGpsCnt = 0 ; /* 清空 */
				}
			}
		}
	}
	
	/* 自动上传gps模式的关闭操作 */
	if(su8GpsWorkMode & 0x01)
	{		
		curTime = ABS(rawtime(NULL)-sGpsRunTick) ;
		if(curTime < su8GpsRunTimes[su8RunIndex]) /* 在指定的时间内定位到 */
		{
			if(1 == su8GpsmGetState)
			{
				if((su8GpsWorkMode & 0x02) == 0) /* 无工作模式2就关闭 */
				{
					gpsm_sendMsg(0);
				}
				if(getGpsCnt < (hsaSdram_GetGpsBufSize()/sizeof(uDATA))) /* gps定位的个数做了限制,以防溢出 */
				{
					wlock((unsigned)&gu32GetGpsInfo);
					
					if(0 != memcmp(gu8Lat,stGpgga.lat,GPS_LAT_CMP) || 0 != memcmp(gu8Long,stGpgga.Long,GPS_LONG_CMP)) /* 100m内不记录坐标 */
					{
						memcpy(gu8Lat,stGpgga.lat,GPS_LAT_CMP);
						memcpy(gu8Long,stGpgga.Long,GPS_LONG_CMP);
						gu8Lat[GPS_LAT_CMP]   = 0 ;
						gu8Long[GPS_LONG_CMP] = 0 ;
						
						strcpy(ptGpsmSend[getGpsCnt].gps.Long,stGpgga.Long);
						strcpy(ptGpsmSend[getGpsCnt].gps.Lat ,stGpgga.lat);
						strcpy(ptGpsmSend[getGpsCnt].gps.high,stGpgga.high);
						strcpy(ptGpsmSend[getGpsCnt].gps.groupId,gtLogoInfo.pGroup[gtLogoInfo.groupSelect].groupId);
						strcpy(ptGpsmSend[getGpsCnt].gps.date,YD_GetTimeStr());
						getGpsCnt++;
					}
					
					unlock((unsigned)&gu32GetGpsInfo);
				}
				su8GpsmGetState = 0 ;
				su8GpsWorkMode &= 0xFE ;
				
				for(i = GPS_RUN_TIMES ; i > 0 ; i--)
				{
					if(curTime < su8GpsRunTimes[i])
					{
						su8RunIndex = i ;
						break;
					}
				}
				
				su8CloseIndex = 0xFF ;
			}
		}
		else /* 超出指定的时间就关闭 */
		{
			if((su8GpsWorkMode & 0x02) == 0) /* 无工作模式2就关闭 */
			{
				gpsm_sendMsg(0);				
				su8GpsWorkMode &= 0xFE ;
				gDelayTick = rawtime(NULL) ;/* 延时的起始tick */				
				su8RunIndex = (0 >= su8RunIndex) ? 0 : (--su8RunIndex) ; /* 调整定位时间 */
				if(su8CloseIndex >= GPS_CLOSE_TIMES)su8CloseIndex = 0 ;
				else
				{
					su8CloseIndex ++ ;
				}
			}
			
			sGpsRunTick = rawtime(NULL);   /* 更新起始tick */
		}
	}	
}
/*-------------------------------
* 函数:gpsm_process
* 功能:
* 参数:none
* 返回:
*-------------------------------*/
void gpsm_process(void)
{
	int		 ret ;
	U8       msgbuf[40] ;
	
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgbuf;
	
	while(1)
	{
		ret = msg_queue_get(ptGpsMsg,(char*)(&ptPutMsg->body),ptPutMsg->length);
		if (ret < MSG_QUEUE_OK)continue;
		
		//PhoneTrace2(0,"get gps msg");
		
		if(GPS_OPEN == ptPutMsg->body.id)/* 启动gps */
		{
			if(0 != Gps_isRunning())
			{
				sGpsRunTick = rawtime(NULL);   /* 启动的时候记录起始tick */
				//PhoneTrace2(0,"get open gps msg");
				Gps_Init();
				//PhoneTrace2(0,"get open gps OK");
			}
		}
		else if(GPS_CLOSE == ptPutMsg->body.id)/* 关闭gps */
		{
			if(0 == Gps_isRunning())  
			{
				//PhoneTrace2(0,"get close gps msg");
				Gps_DeInit();
				//PhoneTrace2(0,"get close gps OK");
			}
		}
	}
}
/*-------------------------------------------------------------------------
* 函数: gpsm_sendMsg
* 说明: 发送消息gps 开关进程
* 参数: 开关标识 0 :close 1 :open
* 返回: 无
* -------------------------------------------------------------------------*/
void gpsm_sendMsg(U8 onoff)
{
	U8       msgbuf[40] ;
	U32      msgId      ;
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgbuf;
	
	msgId = (0 == onoff) ? GPS_CLOSE : GPS_OPEN;
	
	ptPutMsg->length   = sizeof(eMSG_ID);
	ptPutMsg->body.id  = msgId;
	ptPutMsg->priority = 10;
	//PhoneTrace2(0,"send %d gps",onoff);
	msg_queue_tryput(ptGpsMsg,(char*)(&ptPutMsg->body),ptPutMsg->length,ptPutMsg->priority);
}


/*-------------------------------
* 函数:gpsm_thread
* 功能:只管理gps的开和关
* 参数:none
* 返回:
*-------------------------------*/
int gpsm_thread(void)
{
	int pid, startValue;
	
	gpsm_int();
	Gps_DeInit();
	pid = createp((int (*)())gpsm_process,
							GPS_MANAGE_NAME,
							GPS_MANAGE_SIZE,
							GPS_MANAGE_PRIOPITY,
							NULL,NULL,NULL);
							
	ptGpsMsg = msg_queue_init(10,40);
	
	startValue = pstart( pid, (void *)gpsm_process);
	
	return HY_OK;
}


/*-------------------------------------------------------------------------
* 函数: gpsm_mode2
* 说明: 手动定位的开关
* 参数: 开关标识 0 :close 1 :open
* 返回: -1 :gps 为开启
* -------------------------------------------------------------------------*/
int gpsm_mode2(U8 onoff)
{	
	if(gtHyc.GPSSwitch == 0)  return -1;
	if(0 == onoff)su8GpsWorkMode &= 0xFD;
	else  su8GpsWorkMode |= 0x02 ;
	
	//PhoneTrace2(0,"open gps");
	if(su8GpsWorkMode & 0x01) return 0 ; /* 工作在模式1 即gps 开了 */
	
	gpsm_sendMsg(onoff);
}

/*-------------------------------------------------------------------------
* 函数: gpsm_workState
* 说明: 返回gps工作情况
* 参数: none
* 返回: 0 :gps 为关闭 
* -------------------------------------------------------------------------*/
U8 gpsm_workState(void)
{
	return su8GpsWorkMode ;
}
