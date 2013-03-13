/*----------------------文件描述--------------------------------
*创建日期: 08/03/14
*描述    : 电池电量
*--------------------------------------------------------------*/
#include "hyTypes.h"
#include "Battery.h"

//Battery Detect Timer
#define BATTERY_DETECT_TIMER_INTR  10//200 //5s 调用一次
#define BATTERY_LEV_DIFFERENCE 	   8

/*
*DEFINE variable
*/
U32 gu32Battery_DetectCnt = 0;
volatile U32 gu32BatteryLev = 0xFFFFFFFF;//电量满

static U8 gu8ReadAdcCnt = 0; //adc 读取的次数
//static U16 gu16ReadAdc[3] = {POWER_LEVER_3V3};
//static 
volatile U16 gu16ReadAdc[BATTERY_DETECT_CNT] = {POWER_LEVER_3V3};


extern UInt8 gScsiApplnScsiMaxLunFlag;


//#define BATTERY_WIRTE_LOG

#define BATTERY_FILE_NAME  "C:/battery.txt"
#define BATTERY_POOL_OFFSET  0x25600000
#define BATTERY_POOL_SIZE  0x500000
#define BATTERY_LOG_FLAG  "\n ----end-------\n"

#ifdef BATTERY_WIRTE_LOG


char *debug_batter_pool  = (char *)BATTERY_POOL_OFFSET;
int debug_batter_size = 0;
void Battery_CreatLog(void)
{
     debug_batter_pool = (char *)BATTERY_POOL_OFFSET;
     debug_batter_size = 0;
}

void Battery_WriteLog(void)
{
    int i;
    char tempBuf[200];
    int len;
    
    sprintf(tempBuf,"%x,%x,%x,%x,%x\n",gu16ReadAdc[0],gu16ReadAdc[1],gu16ReadAdc[2],gu16ReadAdc[3],gu16ReadAdc[4]);
    len = strlen(tempBuf); 
    sprintf(tempBuf+len,"%x,%x,%x,%x,%x\n",gu16ReadAdc[5],gu16ReadAdc[6],gu16ReadAdc[7],gu16ReadAdc[8],gu16ReadAdc[9]);
    len = strlen(tempBuf);     
    
    if (debug_batter_size+len > BATTERY_POOL_SIZE)
    {
         debug_batter_size = 0;
    }
    
    memcpy(debug_batter_pool+debug_batter_size,tempBuf,len);
    debug_batter_size += len; 
}

#endif

void Battery_CloseLog(void)
{
#ifdef BATTERY_WIRTE_LOG    
    
    int len;
    int fp;
    
    fp = AbstractFileSystem_Open(BATTERY_FILE_NAME,2);
    if (fp >=0) 
    {
        int size;
        size = AbstractFileSystem_FileSize(fp);
        AbstractFileSystem_Seek(fp,size,0);
    }
    else
    {
        fp = AbstractFileSystem_Create(BATTERY_FILE_NAME); 
    }    
    
    if (debug_batter_size+100 > BATTERY_POOL_SIZE)
    {
         debug_batter_size = 0;
    }
    
    len = strlen(BATTERY_LOG_FLAG); 
    memcpy(debug_batter_pool+debug_batter_size,BATTERY_LOG_FLAG,len);
    debug_batter_size += len;
    
    AbstractFileSystem_Write(fp,debug_batter_pool,debug_batter_size);    
    AbstractFileSystem_Close(fp);
    
#endif    
}

void Battery_WriteInfo()
{}

int hy_adc_read(int chl)
{
    int adc;
    
    adc = hyhwAdc_Read(chl);
    
    if (0x3ff == adc)
    {
        adc = -1;
    }
    
    return adc;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_IntialVariable()
* 功能:	开机的时候初始化变量
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_IntialVariable(void)
{
    int i;
#ifdef BATTERY_WIRTE_LOG     
    Battery_CreatLog();
#endif    
    
    //for (i=0; i<10; i++)
    {
    	hy_adc_read(ADCCHANNEL_BATTERY);
    }
    
    //每次都进行电压采集
    gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
    gu8ReadAdcCnt = 0;
    
    memset(gu16ReadAdc,0,sizeof(gu16ReadAdc)); 
    
}

void Battery_NormalDetect(void)
{
	//BATTERY_DETECT_TIMER_INTR次进行一次电压采集
	gu32Battery_DetectCnt = 0;
}


/*-----------------------------------------------------------------------------
* 函数:	Battery_DetectIntial()
* 功能:	初始化检测电压，开机的时候强制检测3次保证能够把电量检测出来
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_DetectIntial(void)
{
	int tempADCLev=0;
#if 0
    
    hyhwAdc_start();
    tempADCLev = hy_adc_read(ADCCHANNEL_BATTERY);
    
    if (tempADCLev == -1) return ;
    
    gu16ReadAdc[gu8ReadAdcCnt++] = tempADCLev; 
    
//	hyhwAdc_Init();
//	hyhwAdc_start(ADCCHANNEL_BATTERY);
//	gu16ReadAdc[gu8ReadAdcCnt++] = hyhwAdc_Read(ADCCHANNEL_BATTERY);

	
	if (gu8ReadAdcCnt >= BATTERY_DETECT_CNT /*|| 0xFFFFFFFF != gu32BatteryLev*/)
	{
		int i,j;
		U32 ktempAdcValue;U16 tempAdc;
#ifdef BATTERY_WIRTE_LOG		
		Battery_WriteLog();
#endif		
		
		//排序
		for (i=0; i<(BATTERY_DETECT_CNT-1); i++)
		{
			for (j=i+1; j<BATTERY_DETECT_CNT; j++)
			{
				if (gu16ReadAdc[i] > gu16ReadAdc[j])
				{
					ktempAdcValue  = gu16ReadAdc[i];
					gu16ReadAdc[i] = gu16ReadAdc[j];
					gu16ReadAdc[j] = ktempAdcValue;
				}
			}
		}
		//去掉最大最小，取平均
		ktempAdcValue = 0;
		for (i=1; i<(BATTERY_DETECT_CNT-1); i++)
		{
			ktempAdcValue += gu16ReadAdc[i];
		}
		ktempAdcValue = ktempAdcValue/(BATTERY_DETECT_CNT-2);
		
		if (ABS(gu32BatteryLev - ktempAdcValue) > BATTERY_LEV_DIFFERENCE)
		{//更新ADC值
			gu32BatteryLev = ktempAdcValue;
		}
		gu8ReadAdcCnt = 0;
       	    
	}
#else
    gu32BatteryLev = POWER_LEVER_3V3; 
#endif	
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_Detect()
* 功能:	检测电池电压
* 参数:	none
* 返回:	读到的实际电压值
*----------------------------------------------------------------------------*/
U32  Battery_Detect(void)
{
    if (gu32Battery_DetectCnt == 0xFFFFFFFF)
    {
        Battery_DetectIntial();
        return gu32BatteryLev;
	}
    
    if (gu32Battery_DetectCnt == 0)
    {
        Battery_DetectIntial();
	}
	
	gu32Battery_DetectCnt++;
	if (gu32Battery_DetectCnt >= BATTERY_DETECT_TIMER_INTR)  gu32Battery_DetectCnt = 0;
		
	return gu32BatteryLev;
}


/*-----------------------------------------------------------------------------
* 函数:	Battery_Level()
* 功能:	检测电池等级
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 Battery_Level(void)
{
	U16 battery;
	U32 lev = 4;
	
	battery = gu32BatteryLev;
	
	if (battery >= POWER_LEVER_FULL) lev = 5;
	else if (battery >= POWER_LEVER_3V3) lev = 4;
	else if (battery >= POWER_LEVER_2V3) lev = 3;
	else if (battery >= POWER_LEVER_1V3) lev = 2;
	else if (battery >= POWER_LEVEL_0V3) lev = 1; 
	else  lev = 0; 	
	
	return lev;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_If_StartPowerOff()
* 功能:	开机的时候检测是否小于3.1V如果小于该值，则直接关电池
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_If_StartPowerOff(void)
{
}

U32 Battery_CurrentLevel(void)
{
    return 5;
}

U32 Battery_UpdateLevel(void)
{

}

/*-----------------------------------------------------------------------------
* 函数:	Battery_PermitRun()
* 功能:	用于判断当前电池是否较低不允许大功耗设备运行
* 参数:	none
* 返回:	0：不允许  1：允许
*----------------------------------------------------------------------------*/
U32 Battery_PermitRun(void)
{
   	 
   	 return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_EnforcePowerDown()
* 功能:	只有在低电量的时候才允许使用该函数进行强制关机
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_EnforcePowerDown(void)
{

}

/*-----------------------------------------------------------------------------
* 函数:	Battery_DetectEnforcePowerDown()
* 功能:	检测是需要强制关机
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
int Battery_DetectEnforcePowerDown(void)
{
    int rv = 0;
    
   
    return rv;
}
void bat_show_initial(void)
{
}
void  bat_show_low_battery_deskbox(void	*pFatherDesk)
{
}
/*-------------------------修改记录--------------------------------
*
*-----------------------------------------------------------------*/