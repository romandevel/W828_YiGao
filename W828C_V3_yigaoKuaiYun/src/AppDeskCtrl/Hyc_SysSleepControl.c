 /*----------------------文件描述--------------------------------
* 创建日期: 2008/05/29 
* 描述    : 整个系统的功耗控制策略
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyTypes.h"

#include "hyhwGpio.h"
#include "hyhwIntCtrl.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "AppControlProcess.h"
#include "AppControlConsume.h"
#include "hardWare_Resource.h"

#include "hyhwTcc7901.h"
#include "Hyc_SysSleepMode.h"
#include "hyhwCkc.h"
#include "HycDeviceConfig.h"
#include "glbStatusDef.h"
#include "glbVariable.h"

volatile U32 gu32WakeMode = 0;  //唤醒模式
volatile U32 gu32PwnDCnt = 0;


U8  gu8GprsMode;//gprs工作模式  0：空闲  1：上传  2：下载

U8	su8GpsWorkMode = 0;

volatile U32 gu32LastTicks = 0;	


U8 GpsIsRun()
{
	return su8GpsWorkMode;
}

void Gprs_setMode(U8 mode)
{
	if(0x0100 == mode)
	{
		gu8GprsMode = 0 ;
	}
	else if(mode > 3) /* 因为把上传记录和上传位置分别设置为0x01和0x02 其他都为下载 */
	{
		gu8GprsMode = 2;
	}
	else
	{
		gu8GprsMode = mode;
	}
}

U8 Gprs_getMode(void)
{
	return gu8GprsMode;
}


//更新系统上次的时间
void Grap_UpdataLastTick(void)
{
    gu32LastTicks = rawtime(NULL);    
}

U32 Grap_GetLastTick()
{
	return gu32LastTicks;
}

void Grap_SetLastTick(U32 tick)
{
	gu32LastTicks = tick;
}
static void PowerOnPeripherals(void)
{
	//hyhwAk4183_SleepAndIntOn();
	
	//恢复串口功能
	hyhwUart_InitPort();
	//Test_LedLight();
	scanner_reinit();
	//Test_LcdLight();
	hyhwAdc_HwInit();
	
	hyhwTimer_countEnable(4);
}

static void PowerOffPeripherals(void)
{
	hyhwTimer_countDisable(4);
	hyhwAdc_PowerIdle();	
	//hyhwAk4183_PenDetectHi_z();
	/* 打开RTC总线 */
	hyhwCpm_busClkEnable(CKC_RTC);
	
	/* 关闭扫描头 */
	scanner_deinit();
	
	hyhwCmosSensor_powerCtrl(EnableOff);
}

/*---------------------------------------------------------------------------
* 函数: ConSume_SetWakeMode
* 功能: 设置系统唤醒模式
* 参数: none
* 返回: 1: 唤醒系统后，背光不开，还会自动进入待机；该唤醒方式，用户不知道
*		2: 唤醒系统后，开背光，用户可以进行操作
*		0：不是睡眠
*	
* -----------------------------------------------------------------------------*/
U32 SysSleep_SetWakeMode(U32 mode)
{
    gu32WakeMode = mode;
    
    return mode;
}
/*---------------------------------------------------------------------------
 函数：ConSume_CheckSdioWake
 功能：判断是否是sdio唤醒系统，如果是则恢复系统
 参数：none
 返回：1:sdio 2；key 0：不是睡眠
* -----------------------------------------------------------------------------*/
U32 SysSleep_CheckWakeMode(void)
{
    return gu32WakeMode;
}

U32 USB_isRunning(void)
{
	int pid;
	
	pid = ntopid(USB_HANDLER_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
		return 0;
}

U32 SysSleep_isValid(void)
{
	U32 result = 1;
	U32 wakeUpMode, sleepFlag, now;
	
	
	if (BackLight_HaveLight() == 1)
	{
		/* 未关闭背光，不允许进入待机模式 */
		return (0);//不允许
	}
	if(1 == Battery_DcInCheck()) //正在充电不允许
	{
		return (0);
	}

	if (1 != drvKeyb_PermitPowerSave())
	{   /*是否允许按键进入*/
	    return (0);
	}	
	
	if (USBHandler_GetScsiFlag() != 0 ||
		USB_isRunning() != 0)
	{
		/* 正在充电或进入USB，不允许进入待机模式 */
		return (0);//不允许
	}	
	
    if (0 == Battery_Level())
    {
        /*090416低电量的时候不能让系统进入低功耗状态*/
        return (0);
    }    
	
	if (hyco_getConnectState() == TRUE)
	{
		//bt处于连接状态，不进入待机
		return 0;
	}
	
	if (gtHyc.NetSelected == 1)//选择的gsm网络
    {
		if(modem_get_sleep_enable() == 0)
		{
			/*	禁止睡眠，不允许进入低功耗模式 */
			return (0);//不允许
		}
		
	    if((net_onlyRead_regStatus() >= 0 && gprs_get_status() >= 0 ) || GsmReset_IfReset() == 1)
	    {
	    	 /* gprs正在传输，不允许进入待机 */
	    	return (0);
	    }
    }
    else//选择的wifi网络
    {
		if ((hy_get_ap_link_status() == 1) &&				//wifi已连接
			(Gprs_getMode()!=0 || Net_GetEvt()!=0))	//有网络发送事件或准备发送
		{
			return 0;
		}
    }
    
    if(0 != GpsIsRun())
    {
    	return (0);
    }
	
	/* 判断是否允许立即进入 */
	wakeUpMode = SysSleep_CheckWakeMode();
	if (wakeUpMode == 1)
	{
		now = rawtime(NULL);
		if (ABS(now - gu32PwnDCnt) < 5)
		{
			/* 背景唤醒后，运行时间未到，不许进入待机 */
			return (0);//不允许
		}
	}
	else if (wakeUpMode == 2)
	{//已经退出来了，但是其他进程还没有机会运行
		now = rawtime(NULL);
		if (ABS(now - gu32PwnDCnt) < 200)
		{
			/* 背景唤醒后，运行时间未到，不许进入待机 */
			return (0);//不允许
		}
	}
	
	return (1);//允许进入待机模式
}


extern U32 gTime;
void SysSleepHandler(void)
{
	U32 wakeupFlag;
	U8  intLevel;
	U32 inPutPort, inPutGpio;
	U32 enterTime, leaveTime, i;
	
	if (SysSleep_isValid() == 0)
	{
		return;
	}
	
	intLevel = InterruptDisable();
	
	PowerOffPeripherals();	//关闭外设
	//Battery_WriteInfo("sys sleep in!\r\n");
	
	#if 1
	if (gtHyc.NetSelected == 1)//选择的gsm网络
	{
		/*	串口CTS GSM模块流控制输入
			CTS保持高电平，通知GSM模块系统不接收数据。
			CTS保持低电平，通知GSM模块系统可以接收数据。*/
		hyhwGpio_setHigh(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);

	}
	#endif

	enterTime = gTime;
	Hyc_SysSleepOverlay();
	Hyc_Set_netFlag(gtHyc.NetSelected);
	//先降频到12MHz，为调整core电压到较低
	hyhwSysClk_Set(ICLK12M_AHB6M, EXTAL_CLK);

#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_disable();
#endif	
	
	//LedRed_statusCtrl(1);
	//wakeupFlag = Hyc_SysSleepMain();
	hyhwArm926e_writeBackDcache();
	hyhwArm926ejs_drainWriteBuffer();
	hyhwArm926ejs_invalidIcacheDcache();
	
	hyhwArm926ejs_disableIcache();
	hyhwArm926ejs_disableDcache();
	wakeupFlag = Hyc_SysSleepMain();
	
	//hyhwSysClk_Set(ICLK192M_AHB96M, EXTAL_CLK);
	hyhwSysClk_Set(ICLK96M_AHB48M, EXTAL_CLK);
	/* 系统有n个周期没有运行，在这里补上(更新系统时间) */
	leaveTime = gTime;
	_clock_s(ABS(leaveTime-enterTime));
	
	ConSume_ChangeMode();
	if ((wakeupFlag & (UART1_WAKEUP|KEY_WAKEUP|LOW_POWER_WAKEUP|USB_VBUS_WAKEUP|POWER_OFF_WAKEUP|G_KEY_WAKEUP|W_KEY_WAKEUP)) != 0)
	{	
		//hyhwApi_stm102Wake();
		PowerOnPeripherals();						//打开外设		
		//InterruptRestore(intLevel);
		enable_IF();
	#ifdef HYC_DEV_USE_WATCHDOG	
		hyhwWatchdog_feed();
		hyhwWatchdog_enable();
	#endif	
		
		#if 1
		if (gtHyc.NetSelected == 1)//选择的gsm网络
    	{
			/* 串口CTS 允许GSM模块向CPU发送数据 */
			hyhwGpio_setLow(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
		}
		#endif
			
		syssleep(10);
		drvKeyb_ClearKey();
		
		//ConSume_ChangeCoefTemp(120,0);
		USB_InitMemory_buf();
		
		SysSleep_SetWakeMode(2);
	    gu32PwnDCnt = rawtime(NULL);//ticks
	    
		if ((wakeupFlag&KEY_WAKEUP))
		{
			//PhoneTrace(0,"power key");
			
			//AppCtrl_WakeupPutGWKey(2);
			drvKeyb_SetEvent(Keyb_Code_Power, cKEYB_EVENT_RELEASE);
		}
		else
		{
			//AppCtrl_WakeupPutGWKey(2);
		}
		
		
		//更新timer时间
		Grap_UpdataLastTick();
		
		if ((wakeupFlag & POWER_OFF_WAKEUP) != 0)
		{
			/*	在待机模式下，长按power键，关机 */
			
		}
	}
	else
	{
		/*	SDIO_WAKEUP  UART1_WAKEUP  TIME_30S_WAKEUP 唤醒
			以上方式唤醒时，屏保不打开，只是在后台做任务，用户不知道
		*/
		PowerOnPeripherals();
		//InterruptRestore(intLevel);
		enable_IF();
	#ifdef HYC_DEV_USE_WATCHDOG	
		hyhwWatchdog_feed();
		hyhwWatchdog_enable();
	#endif	
		
		syssleep(1);
		drvKeyb_ClearKey();
		USB_InitMemory_buf();
		
		SysSleep_SetWakeMode(1);
		gu32PwnDCnt = rawtime(NULL);//ticks
		//add
		//ConSume_ChangeCoefTemp(360);
		//更新timer时间，不能加会影响alarm时间
		Grap_UpdataLastTick();
				
		#if 1
		if (gtHyc.NetSelected == 1)//选择的gsm网络
    	{
			/* 串口CTS 允许GSM模块向CPU发送数据 */
			hyhwGpio_setLow(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
		}
		#endif
		
	}
	
	barBuf_init();
	//Battery_WriteInfo("sys sleep out!\r\n");
}

/* end of file */
/*---------------------------------------------------------------------
090821 : jiezhang  增加低功耗退出USB overlay USB_InitMemory(); 

----------------------------------------------------------------------*/