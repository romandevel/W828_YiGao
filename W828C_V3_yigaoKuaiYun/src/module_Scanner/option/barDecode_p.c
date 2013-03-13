#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwChip.h"
#include "hyhwIntCtrl.h"
#include "hyhwGpio.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "semaphore.h"
#include "barDecode_p.h"
#include "glbVariable.h"
#include "appControlProcess.h"

#define BARCODE_P_NAME		"bar_decode_p"


SEMAPHORE *barDecode_p_Exit_sem = NULL;
int *EventBarDecode = NULL;

U32 barDecode_runningTicks = 0;
U8	barDecodeEnable = 0;

U32 gu32barLastScanTime;


////////////////////////////////////////////////////////////////////
int barDecode_Process(void);

/*---------------------------------------------------------------------------
 函数: barDecode_p_Start
 功能: 启动扫描进程，但不启动激光
 参数: 
 返回: HY_ERROR---该进程正在运行，当前设置无效
 	   HY_OK------启动成功
* -----------------------------------------------------------------------------*/
int barDecode_p_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(BARCODE_P_NAME);
	if (pid>0 && pid<30)
	{
		//进程存在，判断状态
		if (PSPROC(pid)->state == P_INACT)
		{
			//deletep(pid);//删除后再创建
			pidStatus = HY_OK;//进程存在且处于exit状态，则直接启动
		}
		else
		{
			return HY_ERROR;//进程还在运行，直接退出，当前设置无效
		}
	}
	barDecode_p_Exit_sem = semaphore_init(0);
	barDecode_runningTicks = rawtime(NULL);

	if (pidStatus != HY_OK)
	{
		//进程不存在，需要创建
		pid = createp((int (*)())barDecode_Process,
								BARCODE_P_NAME,
								1024,
								BARCODE_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)barDecode_Process );
	return HY_OK;
}

/*---------------------------------------------------------------------------
* 函数:	barDecode_Enable
* 功能:	使能扫描功能，如果扫描成功，立即熄灭激光；否则，超时熄灭激光
* 参数:	enableSecond----扫描激光点亮时间(秒)
* 返回:	none
* -----------------------------------------------------------------------------*/
void barDecode_Enable(U32 enableSecond)
{
#if 0
	// 如果进程没有运行，则启动
	//barDecode_p_Start();
	barApp_powerOnOff(1);
	barDecode_runningTicks = enableSecond*100+rawtime(NULL);
//	ewakeup(&EventBarDecode);
	barApp_laserCtrl(1);
#endif
}

/*---------------------------------------------------------------------------
* 函数:	barDecode_Enable
* 功能:	禁止扫描功能，但不销毁进程
* 参数:	none
* 返回:	none
* -----------------------------------------------------------------------------*/
void barDecode_Disable(void)
{
	//barDecode_runningTicks = rawtime(NULL);
	barApp_laserCtrl(0);
}

/*---------------------------------------------------------------------------
 函数: barDecode_p_Exit
 功能: 停止扫描功能
 参数: 无
 返回: 无
* -----------------------------------------------------------------------------*/
void barDecode_p_Exit(void)
{
	int pid;
	
	pid = ntopid(BARCODE_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (barDecode_p_Exit_sem != NULL)
		semaphore_post(barDecode_p_Exit_sem);
	
	ewakeup(&EventBarDecode);
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*---------------------------------------------------------
* 函数: barDecode_p_IsRunnning
* 功能: 扫描进程是否在运行
* 参数: none
* 返回: =1----正在运行
*		=0-----没有运行
*---------------------------------------------------------*/
int barDecode_p_IsRunnning(void)
{
	int pid;
	
	pid = ntopid(BARCODE_P_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}

void barDecode_Wakeup(void)
{
	ewakeup(&EventBarDecode);
}
/*---------------------------------------------------------------------------
 函数: barDecode_Process
 功能: 扫描进程
 参数: 无
 返回: 总是返回1，操作系统要求
* -----------------------------------------------------------------------------*/
int barDecode_Process(void)
{
	int ret;
	unsigned ilev;

	barScanner_init();
	barDecode_Wakeup_Config(barDecode_Wakeup);
	barApp_powerOnOff(0);
	while(1)
	{
		ilev = setil(_ioff);
		ewait(&EventBarDecode);
		//ewaittm(&EventBarDecode, 2);
		(void) setil(ilev);
	//	syssleep(1);
		barApp_powerCtrl();
		
		ret = semaphore_trywait(barDecode_p_Exit_sem);
		if (ret == 0)
		{
			//响应退出要求，退出进程
			barApp_laserCtrl(0);
			break;
		}
		#if 0
		if (barDecode_runningTicks!=0 &&
			rawtime(NULL)>=barDecode_runningTicks)
		{
			gu32barLastScanTime = rawtime(NULL);
			//运行时间到，停止激光
			barApp_laserCtrl(0);
			barApp_powerOnOff(0);
			continue;
		}
		#endif
		
		if(barApp_decodeBarcode() > 0)
		{
			//barApp_powerOnOff(0);
		}
		barApp_powerCtrl();
		
		#if 0
		{
			static U32 lastTick=0;
			U32 now = rawtime(NULL);
			
			if ((now-lastTick) >= 50)
			{
				PhoneTrace(0, "barApp_RUN...");
				lastTick = now;
			}
		}
		#endif
	} /* while(1) */

	barScanner_close();
	
	semaphore_destroy(barDecode_p_Exit_sem, OS_DEL_NO_PEND);
	barDecode_p_Exit_sem = NULL;
	EventBarDecode = NULL;
	
	return 1;
}

void bar_updatelasttime(U32 last)
{
	gu32barLastScanTime = last;
}

void bar_ifstartscan()
{
	U32 delay;

#if 0//暂时取消自动触发功能
	if(barGetScanType() == 1 && gtHyc.ScanTrigger == 1 && barDecode_p_IsRunnning() == 1)
	{
		delay = ABS(rawtime(NULL)-gu32barLastScanTime);
		
		if(delay >= 100 && barApp_getLaserState() == 0)
		{
			//更新自动锁屏时间
	    	cls_updata_lock_time(rawtime(NULL));

			barDecode_Enable(gtHyc.ScanLightTime);
		}
	}
#endif
	return ;
}

#if 0
int total = 0;
int okCnt = 0;
int errCnt = 0;
void testScan(void)
{

	barDecode_Enable(5);
	while(1)
	{	//退出條件：扫描成功，超时
		int len;U8 recvBuf[50];
		barBuf_readBarCode(&recvBuf[0], &len);
		if (len > 0)
		{
			//break;
			syssleep(10);
			barDecode_Enable(5);
		}
		else if (barApp_getLaserState() == 0)
		{
			//break;
			syssleep(50);
			barDecode_Enable(5);
		}
		syssleep(1);
	}

}
#endif
