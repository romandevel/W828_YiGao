#include "GPS_Parse.h"
#include "string.h"
#include "hyhwRtc.h"

// 接口使用说明: 首先调用Gps_Int初始化，然后接收的数据 
// 使用Gps_GpggaData 函数进行解析处理，调用Gps_GetInfo 来获取所需信息

//#define USE_GPS


//#define USE_GPGGA	//解析GPGGA段


#define GPS_VALID_MAXNUM		60
extern RtcTime_st gtRtc_solar;

static U8  sU8DataOffset;	//记录GPS数据buf的偏移
static U8  sU8SectionCnt;	//字段偏移
static U8  sU8Cnt;			//针对不同的字段的接受数据数

//#ifdef USE_GPGGA
tGPGGA		gtGPGGA;
//#else
tGPRMC		gtGPRMC;
//#endif

U8			gu8GpsCount;//有效定位到的次数
U8			gu8GpsRead,gu8GpsWrite;

eGPSSTATE	eGpsState;

U8		gu8ParseFlag;
U8		gu8Clr=0;
U32		gu32GpsGet,gu32GpsSend;
U32		gu32GpsRunTime;
U8      gu8ColdStart[10];
/////////////////////////////////////////////////////////////
char buf_cn[4];

void Gps_SetTime(tGPGGA *pInfo, tGPRMC *pRmc)
{
	U8 buf[3];
	RtcTime_st	Rtc_solar;
	
	//hyhwRtc_GetTime(&Rtc_solar);
	
	memset(buf,0,3);
	//日
	buf[0] = pRmc->date[0];
	buf[1] = pRmc->date[1];
	Rtc_solar.day = (U8)atoi(buf);
	//月
	buf[0] = pRmc->date[2];
	buf[1] = pRmc->date[3];
	Rtc_solar.month = (U8)atoi(buf);
	//年
	buf[0] = pRmc->date[4];
	buf[1] = pRmc->date[5];
	Rtc_solar.year = 2000+(U8)atoi(buf);
	
	//时
	buf[0] = pInfo->t[0];
	buf[1] = pInfo->t[1];
	Rtc_solar.hour = (U8)atoi(buf);
	Rtc_solar.hour += 8;
	if(Rtc_solar.hour >= 24)
	{
		Rtc_solar.hour -= 24;
	}
	//分
	buf[0] = pInfo->t[2];
	buf[1] = pInfo->t[3];
	Rtc_solar.minute = (U8)atoi(buf);
	//秒
	buf[0] = pInfo->t[4];
	buf[1] = pInfo->t[5];
	Rtc_solar.second = (U8)atoi(buf);
	
	hyhwRtc_SetTime(&Rtc_solar);
		
	return ;
}

/* 目前使用的GPS定位精度比较高,小数点后又6位，不同的精度要做不同的修改
   将GPS的坐标信息由ddmm.mmmm 及 dddmm.mmmm 格式转换成ddd.dddd格式     */
void Gps_SetLatLong(tGPGGA *pInfo)
{
	U32 dertLat , dertLong ;
	U8  dlat[5] ,dlong[5]  ;
	U8  mlat[10],mlong[10] ;
	
	memset(dlat ,0,5) ;
	memset(dlong,0,5) ;
	memset(mlat ,0,10);
	memset(mlong,0,10);
	
	memcpy(dlat ,pInfo->lat ,2) ;
	memcpy(dlong,pInfo->Long,3) ;
	
	memcpy(mlat ,&pInfo->lat[2],2) ;
	memcpy(mlong,&pInfo->Long[3],2) ;
	
	memcpy(&mlat[2] ,&pInfo->lat[5],6) ;
	memcpy(&mlong[2],&pInfo->Long[6],6) ;

	dertLat = atoi(mlat) ;
	dertLong= atoi(mlong);
	
	dertLat  /= 6 ;
	dertLong /= 6 ;

	sprintf(pInfo->lat ,"%s.%07d",dlat ,dertLat) ;
	sprintf(pInfo->Long,"%s.%07d",dlong,dertLong) ;

}

/*
GPGSV 解析
*/
int Gps_GetGPGSVInfo(U8 data)
{
	
	
	if (GPS_SYMBOL == data)
	{
		sU8Cnt = 0;
		sU8SectionCnt++;
		if(gu8ParseFlag == 2)
		{
			gu8ParseFlag = 1;
			sU8SectionCnt = 0;
		//	sprintf(YDtest_gpsGetCN(),"%s",buf_cn);
			//PhoneTrace2(gu8Clr,"C/N = %s",buf_cn);
		}
	}
	
	if(gu8ParseFlag == 2)
	{
		buf_cn[sU8Cnt++] = data;
	}
	else if(gu8ParseFlag == 1)
	{
		if(sU8SectionCnt == 3)
		{
			gu8ParseFlag = 2;
			memset(buf_cn,0,4);
		}
	}
	else if(sU8SectionCnt == 4)
	{
		gu8ParseFlag = 1;
		sU8SectionCnt = 0;
	}
	
	
	return 0;
}

void Gps_Parse_GPGSV(U8 *pNMEA)
{
	int ret,len;
	U8  i;
	U8  *data,*pend;
	U8	*pGpgsv = GPS_GPGSV;
	
	data = strstr(pNMEA,pGpgsv);
	
	if(data == NULL)
		return ;
	data += strlen(pGpgsv);
	pend = strstr(data,"*");
	if(pend)
		len = (int)(pend-data);
	else
		len = strlen(data);
	
	gu8ParseFlag  = 0;
	sU8SectionCnt = 0;
	
	for(i = 0; i < len; i++)
	{
		Gps_GetGPGSVInfo(data[i]);
	}
}

/*
 * 解析 GPRMC数据
 */
int Gps_GetGPRMCInfo(U8 data)
{
	int ret = -1;
	
	if(GPS_SYMBOL == data)
	{
		sU8SectionCnt++;
		sU8Cnt = 0; //每一个字段 已接收数 清零
		if (sU8SectionCnt > GPRMC_DATE)
		{
			sU8SectionCnt = 0;
			sU8DataOffset = 0;
			ret = 0;
		}
		return ret;
	}
	switch(sU8SectionCnt)
	{
	case GPGGA_UTC_TIME:
		gtGPRMC.t[sU8Cnt++] = data;
		break;
	case GPRMC_ENBLE:
		gtGPRMC.enable = data;
		if (GPRMC_NOTOK == data)
		{
			sU8DataOffset = 0;
			sU8SectionCnt = 0;
		}
		break;
	case GPRMC_LAT:
		gtGPRMC.lat[sU8Cnt++] = data;
		break;
	case GPRMC_N_S:
		gtGPRMC.ns = data;
		break;
	case GPRMC_LONG:
		gtGPRMC.Long[sU8Cnt++] = data;
		break;
	case GPRMC_E_W:
		gtGPRMC.ew = data;		
		break;
	case GPRMC_SPEED:
		gtGPRMC.speed[sU8Cnt++] = data;
		break;
	case GPRMC_DIR:
		gtGPRMC.moveDir[sU8Cnt++] = data;
		break;
	case GPRMC_DATE:
		gtGPRMC.date[sU8Cnt++] = data;		
		break;
	default:
		break;
	}

	return ret;
}


/*
解析GPRMC的数据
*/
void Gps_Parse_GPRMC(U8 *pNMEA)
{
	int ret,len;
	U8  i;
	U8  *data,*pend;
	U8	*pGprmc = GPS_GPRMC;
	
	data = strstr(pNMEA,pGprmc);
	
	if(data == NULL)
		return ;
	data += strlen(pGprmc);
	pend = strstr(data,"$GPVTG");
	if(pend)
		len = (int)(pend-data);
	else
		len = strlen(data);
	
	gu8ParseFlag  = 0;
	sU8SectionCnt = 0;
	
	for(i = 0; i < len; i++)
	{
		if(0 == Gps_GetGPRMCInfo(data[i]))
		{
			//if(gu32GpsRunTime > 30)
				//Gps_SetTime(&gtGPGGA,&gtGPRMC);
		}
	}

	return ;
}

/*
 * 解析 GPGGA 数据
 */
int Gps_GetGPGGAInfo(U8 data)
{
	int ret = -1;
	
	if (GPS_SYMBOL == data)
	{
		sU8SectionCnt++;
		sU8Cnt = 0; //每一个字段 已接收数 清零
		if (sU8SectionCnt > GPGGA_HIGHT_SEA)
		{
			sU8SectionCnt = 0;
			sU8DataOffset = 0;
			if(gtGPGGA.enable != '0')
				ret = 0;
		}
		return ret;
	}
	switch(sU8SectionCnt)
	{
	case GPGGA_UTC_TIME:
		gtGPGGA.t[sU8Cnt++] = data;
		break;
	case GPGGA_LAT:
		gtGPGGA.lat[sU8Cnt++] = data;
		break;
	case GPGGA_N_S:
		gtGPGGA.ns = data;
		break;
	case GPGGA_LONG:
		gtGPGGA.Long[sU8Cnt++] = data;
		break;
	case GPGGA_E_W:
		gtGPGGA.ew = data;
		break;
	case GPGGA_ENBLE:
	//PhoneTrace(0,"enable__%c",data);
		gtGPGGA.enable = data;
		break;
	case GPGGA_STAR:
		gtGPGGA.star[sU8Cnt++] = data;
		break;
	case GPGGA_PRICISION:
		gtGPGGA.hdop[sU8Cnt++] = data;
		break;
	case GPGGA_HIGHT_SEA:
		gtGPGGA.high[sU8Cnt++] = data;
		break;
	/*case GPGGA_HIGHT_EAR:
		break;
	case GPGGA_DIFFER:
		break;
	case GPGGA_DIFFER_ID:
		break;*/
	default:
		break;
	}
	
	return ret;
}


/*
解析GPGGA的数据
*/
void Gps_Parse_GPGGA(U8 *pNMEA)
{
	int ret,len;
	U8  i;
	U8  *data,*pend;
	U8	*pGpgga = GPS_GPGGA;
	
	data = strstr(pNMEA,pGpgga);
	
	if(data == NULL)
		return ;
	data += strlen(pGpgga);
	pend = strstr(data,"$GPGLL");
	if(pend)
		len = (int)(pend-data);
	else
		len = strlen(data);
	if(len < 40)
	{
		//PhoneTrace2(gu8Clr,"GPS_GPGGA --");
		return ;
	}	
	
	sU8DataOffset = 0;
	sU8SectionCnt = 0;
	memset((char *)&gtGPGGA,0,sizeof(tGPGGA));

	for(i = 0; i < len; i++)
	{
		ret = Gps_GetGPGGAInfo(data[i]);
		//PhoneTrace2(gu8Clr,"GPS_GPGGA ret = %d",ret);
		if(ret == 0)
		{
			if(gu32GpsRunTime != 0xffffffff)
			{
				memset(gu8ColdStart,0,10);
				gu32GpsRunTime = (ABS(rawtime(NULL) - gu32GpsRunTime))/100;
				sprintf(gu8ColdStart,"%d",gu32GpsRunTime);
				
				Gps_Parse_GPRMC(pend);
				gu32GpsRunTime = 0xffffffff;
			}
			
			//PhoneTrace2(gu8Clr,"------------------");
			//PhoneTrace2(gu8Clr,"%04d-%02d-%02d %02d:%02d:%02d",
			//				gtRtc_solar.year,gtRtc_solar.month,gtRtc_solar.day,
			//				gtRtc_solar.hour,gtRtc_solar.minute,gtRtc_solar.second);
			//PhoneTrace2(gu8Clr,"cold start = %s(S)",gu8ColdStart);
			//PhoneTrace2(gu8Clr,"Long = %s",gtGPGGA.Long);
			//PhoneTrace2(gu8Clr,"lat = %s",gtGPGGA.lat);
			//PhoneTrace2(gu8Clr,"star = %s",gtGPGGA.star);
			//Gps_Parse_GPGSV(pend);
			
			//PhoneTrace2(gu8Clr,"------------------");
			
			gu8Clr = !gu8Clr;
			
			Gps_SetLatLong(&gtGPGGA);
		
			//test
			TestGps_SetLong(gtGPGGA.Long);
			TestGps_SetLat(gtGPGGA.lat);
			TestGps_SetStar(gtGPGGA.star);
			
			/*if(1 == YDhtest_Getgps())
			{
				Gps_Parse_GPGSV(pend);
				gpsm_setState(&gtGPGGA);
			}*/
			
			break;
		}
	}

	return ;
}



