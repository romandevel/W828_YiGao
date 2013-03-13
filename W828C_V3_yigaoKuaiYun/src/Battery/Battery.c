/*----------------------文件描述--------------------------------
*创建日期: 08/03/14
*描述    : 电池电量
*--------------------------------------------------------------*/
#include "hyTypes.h"
#include "Battery.h"
#include "hyhwGpio.h"
#include "hardWare_Resource.h"

/* 充电检测端口 有4种状态:  充电使能,无电池       该口为低
							充电不使能,           该口为低
							充电使能,有电池且充满 该口为低
							充电使能,有电池没充满 该口为高
*/


#define BATTERY_DETECT_TIMER_INTR   50//10//200 //5s 调用一次
#define BATTERY_LEV_DIFFERENCE 	   8

/*
*DEFINE variable
*/
#define BATTERY_ENFORCE_VALUE   (0x12)
static U8 gu8EnforcePDn = 0;
U32 gu32Battery_DetectCnt = 0;
volatile U32 gu32BatteryLev = 0xFFFFFFFF;//电量(用于显示)
volatile U32 gu32BatteryCurLev = 0xFFFFFFFF;//当前电量

static U8 gu8ReadAdcCnt = 0; //adc 读取的次数
//static U16 gu16ReadAdc[3] = {POWER_LEVER_3V3};
//static 
volatile U16 gu16ReadAdc[BATTERY_DETECT_CNT] = {POWER_LEVER_3V3};



//测试打开此二宏
//#define BATTERY_WIRTE_LOG
//#define BATTERY_WIRTE_LOG_FILE

//#define BATTERY_WIRTE_LOG_UART

#ifdef BATTERY_WIRTE_LOG
#include "appControlProcess.h"
#include "AppCtrlConstData.h"

#ifdef BATTERY_WIRTE_LOG_UART
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwUart.h"
#endif

#define BATTERY_FILE_NAME		"C:/battery.txt"
#define BATTERY_LOG_FLAG		"\r\n ----end------- \r\n"

int fd_battery = -1;
char *pBatterybuf = NULL;
U16 gu16ReadAdcTest0[BATTERY_DETECT_CNT];
U16 gu16ReadAdcTest[BATTERY_DETECT_CNT];
volatile U8  gu16WriteFull = 0;
#define TEMP_BUF_SIZE (300)	 //512

void Battery_UartSend(char *data, int len)
{
#ifdef BATTERY_WIRTE_LOG_UART
	int i;
	
	for (i=0; i<len; i++)
	{
		hyhwUart_WriteFifo(*data++, UART3);
	}
#endif
}

void Battery_CreatLog(void)
{
#ifdef BATTERY_WIRTE_LOG
#ifdef BATTERY_WIRTE_LOG_FILE
	int fd;
	fd = AbstractFileSystem_Open(BATTERY_FILE_NAME, 2);//AFS_RDWR
	if (fd >= 0)
	{
		AbstractFileSystem_Seek(fd, 0, 2);
	}
	else
	{
		fd = AbstractFileSystem_Create(BATTERY_FILE_NAME);
	}
	if (fd >= 0)
	{
		AbstractFileSystem_Write(fd, "\r\n---bat test---\r\n", strlen("\r\n---bat test---\r\n"));
		fd_battery = fd;
	}
#endif
#ifdef BATTERY_WIRTE_LOG_UART
	Battery_UartSend(infoStr, strlen(infoStr));
#endif
#endif
}

void Battery_WriteBuf(U16 adcValue)
{}

void Battery_WriteLog(U16 adcValue)
{
#ifdef BATTERY_WIRTE_LOG
	float v;char tempStr[50];
	int i, len;
	
	v = (float)(((float)adcValue*3*2)/1024);
	sprintf(tempStr, "ADC=%04X, V=%1.4f, T=%08X\r\n", adcValue, v, rawtime(NULL));
	len = strlen(tempStr);
	for (i=12; i<18; i++)
	{
		if (tempStr[i]<'0' || tempStr[i]>'9')
		{
			tempStr[i] = '.';
			break;
		}
	}
	#ifdef BATTERY_WIRTE_LOG_FILE
	if (fd_battery >= 0)
	{
		AbstractFileSystem_Write(fd_battery, tempStr, len);
	}
	#endif
	#ifdef BATTERY_WIRTE_LOG_UART
	#endif
#endif
}

#endif

void Battery_WriteInfo(char *infoStr)
{
#ifdef BATTERY_WIRTE_LOG
	if (fd_battery >= 0)
	{
	#ifdef BATTERY_WIRTE_LOG_FILE
		AbstractFileSystem_Write(fd_battery, infoStr, strlen(infoStr));
	#endif
	#ifdef BATTERY_WIRTE_LOG_UART
		Battery_UartSend(infoStr, strlen(infoStr));
	#endif
	}
#endif
}

void Battery_CloseLog(void)
{
#ifdef BATTERY_WIRTE_LOG
	char *tempBuf = (char *)BATTERY_LOG_FLAG;
	int len = strlen(BATTERY_LOG_FLAG);
	if (fd_battery >= 0)
	{
#ifdef BATTERY_WIRTE_LOG_FILE
		AbstractFileSystem_Write(fd_battery, tempBuf, len);
		AbstractFileSystem_Close(fd_battery);
#endif
#ifdef BATTERY_WIRTE_LOG_UART
		Battery_UartSend(tempBuf, len);
#endif
		if (pBatterybuf != NULL)
		{
			free(pBatterybuf);
		}
		fd_battery = -1;
		pBatterybuf = NULL;
	}
#endif
}

/*------------------------------------------------------------
* 函数:	Battery_Init
* 功能:	电源切换部分的初始化 以下gpio口不可被复用
* 参数:	无
* 返回:	无
--------------------------------------------------------------*/
void Battery_Init(void)
{
	hyhwGpio_setAsGpio(DCIN_DET_PORT, DCIN_DET_GPIO);		//DC插入检测
	hyhwGpio_setIn(DCIN_DET_PORT, DCIN_DET_GPIO);
	
	//hyhwGpio_setAsGpio(CHARGE_STATE, CHARGE_STATE_BIT);		//充电检测
	//hyhwGpio_setIn(CHARGE_STATE, CHARGE_STATE_BIT);
	
	//hyhwGpio_setAsGpio(ENCHG_BAT, ENCHG_BAT_BIT);		//使能充电
	//hyhwGpio_setOut(ENCHG_BAT, ENCHG_BAT_BIT);
	//hyhwGpio_setLow(ENCHG_BAT, ENCHG_BAT_BIT);
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_DcInCheck
* 功能:	是否有充电器插入
* 参数:	none
* 返回:	0 :no 1：yes
*----------------------------------------------------------------------------*/
U32 Battery_DcInCheck(void)
{
	return (hyhwGpio_Read(DCIN_DET_PORT, DCIN_DET_GPIO));
}

int hy_adc_read(int chl)
{
    int adc;
    
    adc = hyhwAdc_Read(chl);
    
    //PhoneTrace4(0,"%x",adc);
    
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
    
    hyhwGpio_setAdcFunction();
    //for (i=0; i<10; i++)
    {
    	hy_adc_read(ADCCHANNEL_BATTERY);
    }
    
    //每次都进行电压采集
    gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
    gu8ReadAdcCnt = 0;
    
    memset(gu16ReadAdc,0,sizeof(gu16ReadAdc));
    
    hyhwInt_ConfigADC(hyhwAdc_start); 
    
    gu8EnforcePDn = 0;
    
}

void Battery_NormalDetect(void)
{
	//BATTERY_DETECT_TIMER_INTR次进行一次电压采集
	gu32Battery_DetectCnt = 0;
	//gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_AmendValue
* 功能:	根据系统工作状态和外设使用情况，修正电池ADC
* 参数:	adcLev----实际采集到的ADC
* 返回:	修正后的ADC
*----------------------------------------------------------------------------*/
int Battery_AmendValue(int adcLev)
{
#if 1
	int status;
	int coeff, offsetAdcLev;
	
	offsetAdcLev = 0;
	if (Battery_DcInCheck() == 1)//原来是下面的USB检测
	{
		offsetAdcLev -= 16;
	}
	/*if(YD_getPrintEnable() == 1)
	{
		offsetAdcLev += 45;
	}*/
	/*
	if (GetUSBStatus() == 1)
	{//USB host
		offsetAdcLev += 20;
	}*/  	
	adcLev += offsetAdcLev;
#endif
	return adcLev;
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
#if 1

    tempADCLev = hy_adc_read(ADCCHANNEL_BATTERY);
    
	#ifdef BATTERY_WIRTE_LOG
    	gu16ReadAdcTest0[gu8ReadAdcCnt] = tempADCLev;
	#endif
    
    tempADCLev = Battery_AmendValue(tempADCLev);
    
    if (tempADCLev == -1) return ;
    
    gu16ReadAdc[gu8ReadAdcCnt++] = tempADCLev; 
    
	if (gu8ReadAdcCnt >= BATTERY_DETECT_CNT /*|| 0xFFFFFFFF != gu32BatteryLev*/)
	{
		int i,j;
		U32 ktempAdcValue;U16 tempAdc;
		
	#ifdef BATTERY_WIRTE_LOG
		memcpy(gu16ReadAdcTest, gu16ReadAdc, BATTERY_DETECT_CNT*2);
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
		
		if (ABS((int)gu32BatteryLev - (int)ktempAdcValue) > BATTERY_LEV_DIFFERENCE)
		{//更新ADC值
			gu32BatteryLev = ktempAdcValue;
		}
		gu32BatteryCurLev = ktempAdcValue;
		#if 0
		{
			float v;char tempStr[20];
			v = (float)(((float)ktempAdcValue*3.3*2)/1024);
			sprintf(tempStr, "%1.4f", v);
			for (i=0; i<strlen(tempStr); i++)
			{
				if (tempStr[i]<'0' || tempStr[i]>'9')
				{
					tempStr[i] = '.';
					break;
				}
			}
			//PhoneTrace(1,"bat ADC = %04X, V=%s", gu32BatteryCurLev, tempStr);
			hyUsbPrintf("bat ADC = %04X, V=%s\r\n", gu32BatteryCurLev, tempStr);
		}
		#endif
		gu8ReadAdcCnt = 0;
       	
		#ifdef BATTERY_WIRTE_LOG
			Battery_WriteLog(ktempAdcValue);
			if (gu32BatteryCurLev >= POWER_LEVER_FULL)
			{
				Battery_WriteInfo("***********bat full!!!**************\r\n");
			}
		#endif		
	}
#else
    gu32BatteryLev = POWER_LEVER_3V3; 
	//if(PhoneTrace_base)
	//	PhoneTrace1(1,"SdramSize= %d M",hyhwSdramGetCapability());
#endif

	//PhoneTrace1(1,"bat LevADC = %04X",gu32BatteryLev);
    //gu32BatteryLev = POWER_LEVEL_OFFV3+2;
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
    
    //if (ABS(rawtime(NULL) - gu32Battery_DetectCnt) >= 100)
    if (ABS(rawtime(NULL) - gu32Battery_DetectCnt) >= 20)
    {
		gu32Battery_DetectCnt = rawtime(NULL);
        Battery_DetectIntial();
	}
	
	
#ifdef BATTERY_WIRTE_LOG    
    //Battery_WriteLog(hyhwAdc_Read(ADC_CH2));
#endif
	
	//gu32Battery_DetectCnt++;
	//if (gu32Battery_DetectCnt >= BATTERY_DETECT_TIMER_INTR)  gu32Battery_DetectCnt = 0;
	
	/*if(ABS(rawtime(NULL)-tt) > 3000)//30秒打印一次打压值
	{
		bat = gu32BatteryLev*66;
		bat = bat/10240;
		sprintf(buf,"bat=0x%x__%f",gu32BatteryLev,bat);
		if(PhoneTrace_base)
			PhoneTrace_base(0,"%s",buf);
		tt = rawtime(NULL);
	}*/

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
	else if (battery >= POWER_LEVER_LOW) lev = 1;  //POWER_LEVEL_0V3
	else  lev = 0; 	
	
	return lev;
}
/*-----------------------------------------------------------------------------
* 函数:	Battery_CurrentLevel()
* 功能:	检测电池当前等级
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 Battery_CurrentLevel(void)
{
	U16 battery;
	U32 lev;
	
	battery = (U16)gu32BatteryCurLev;
	
	if (battery >= POWER_LEVER_FULL) lev = 5;
	else if (battery >= POWER_LEVER_3V3) lev = 4;
	else if (battery >= POWER_LEVER_2V3) lev = 3;
	else if (battery >= POWER_LEVER_1V3) lev = 2;
	else if (battery >= POWER_LEVER_LOW) lev = 1;  //POWER_LEVEL_0V3
	else  lev = 0; 	
	
	return lev;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_UpdateLevel()
* 功能:	更新显示电量为当前电量
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 Battery_UpdateLevel(void)
{
	gu32BatteryLev = gu32BatteryCurLev;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_PermitRun()
* 功能:	用于判断当前电池是否较低不允许大功耗设备运行
* 参数:	none
* 返回:	0：不允许  1：允许
*----------------------------------------------------------------------------*/
U32 Battery_PermitRun(void)
{
   	 if (0 == Battery_DcInCheck() && gu32BatteryLev < POWER_PERMIT_RUN_LEV)
   	 {
   	     return 0;
   	 }
   	 
   	 return 1;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_EnforcePowerDown()
* 功能:	只有在低电量的时候才允许使用该函数进行强制关机
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_EnforcePowerDown(void)
{
     if (0 != Battery_PermitRun()) 
     {
         return;
     }
     
     gu8EnforcePDn = BATTERY_ENFORCE_VALUE;
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
    
    if (0 == Battery_PermitRun() && gu8EnforcePDn == BATTERY_ENFORCE_VALUE)
    {
        gu8EnforcePDn = 0;
        rv = 1;
    }
    
    return rv;
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_If_StartPowerOff()
* 功能:	开机的时候检测是否小于3.55V如果小于该值，则直接关电池
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_If_StartPowerOff(void)
{
    int adc;
    int count;
    
    //syssleep(15); /* 开机的时候 ADC一启动 起初几个值是不正确的,这里延迟让中断去读掉 */
    for(count = 0; count <= BATTERY_DETECT_CNT; count++)
    {
    	syssleep(2);
    	Battery_DetectIntial();
    }
    
    adc = Battery_Detect();
    
    if (0 != Battery_DcInCheck()) return;
    
    if (adc < POWER_LEVEL_FORBID)
    {                
        for(count = 0; count <= BATTERY_DETECT_CNT; count++) //做一次确认
	    {
	    	syssleep(2);
	    	Battery_DetectIntial();
	    }
	    adc = Battery_Detect();
	    if (0 != Battery_DcInCheck()) return;
	    if (adc > POWER_LEVEL_FORBID) return;
	    
        InterruptDisable();  
					
		while(1)
		{
		    hyhwBatteryPowerOnOff(0);
		    for (count=0; count < 5000; count++);
		}
    }
}

/*-----------------------------------------------------------------------------
* 函数:	Battery_print
* 功能:	切换到打印状态，或退出打印状态
* 参数:	printFlag
*			0-----进入打印状态
*			1-----退出打印状态
* 返回:	0----状态切换成功
		-1---状态切换失败(不允许进入打印状态)
*----------------------------------------------------------------------------*/
int Battery_print(int printFlag)
{
	int state;
		
	//在充电
	//if (1 == Battery_DcInCheck()) return 0;
	
	//电量低时，不允许打印
	if(gu32BatteryLev < POWER_PERMIT_RUN_LEV)
	{
		return -1;
	}
	
	return 0;
}

U32 Battery_getAdc(void)
{
	return gu32BatteryLev ;
}

/*-------------------------修改记录--------------------------------
*
*-----------------------------------------------------------------*/