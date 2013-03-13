#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwGpio.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "semaphore.h"
#include "vibratorCtrl_p.h"
#include "hardWare_Resource.h"
#include "appControlProcess.h"


#define VIBRATOR_CTRL_RUN_TIME		(100)//1秒，震动时间
#define VIBRATOR_CTRL_STOP_TIME		(100)//1秒，停止时间



/*
                            RUN_TIME                   RUN_TIME
run ----------             ------------              ------------
stop          |___________|            |____________|            |_______
               STOP_TIME                STOP_TIME                 STOP_TIME
*/

SEMAPHORE *vibratorCtrlExit_sem = NULL;
U32 vibratorCtrlRunningTicks;

int VibratorCtrl_Process(void);


void VibratorCtrl_Init(void)
{
	hyhwGpio_setAsGpio(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
	hyhwGpio_setOut(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
	hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
}

void VibratorCtrl_RunTime(int millisecond)
{
	VibratorCtrl_Start(millisecond);
	sleep(millisecond/10);
	VibratorCtrl_Stop();
}


/*---------------------------------------------------------------------------
 函数: VibratorCtrl_Start
 功能: 启动振动器
 参数: millisecond----要求振动总时间，毫秒为单位，时间到时，主动退出该进程
 					 当该值为0时，不主动退出，直到调用VibratorCtrl_Stop()
 返回: HY_ERROR---该进程正在运行，当前设置无效
 	   HY_OK------启动成功
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Start(U32 millisecond)
{
	int pid, startValue, pidStatus = HY_ERROR;
	int ret;
	
	VibratorCtrl_Stop();
	pid = ntopid(VIBRATOR_CTRL_NAME);
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
	
	if (pidStatus != HY_OK)
	{
		vibratorCtrlRunningTicks = millisecond/10;//从ms转换为系统ticks
		vibratorCtrlExit_sem = semaphore_init(0);
	
		//进程不存在，需要创建
		pid = createp((int (*)())VibratorCtrl_Process,
								VIBRATOR_CTRL_NAME,
								VIBRATOR_SIZE,
								VIBRATOR_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)VibratorCtrl_Process );
	return HY_OK;
}

/*---------------------------------------------------------------------------
 函数: VibratorCtrl_Stop
 功能: 停止振动器
 参数: 无
 返回: 无
* -----------------------------------------------------------------------------*/
void VibratorCtrl_Stop(void)
{
	int pid;
	
	pid = ntopid(VIBRATOR_CTRL_NAME);
	if (pid<0 || pid>30) return;
	
	if (vibratorCtrlExit_sem != NULL)
		semaphore_post(vibratorCtrlExit_sem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*---------------------------------------------------------
* 函数: VibratorCtrl_IsRunnning
* 功能: 振动器控制进程是否在运行
* 参数: none
* 返回: =1----正在运行
*		=0-----没有运行
*---------------------------------------------------------*/
int VibratorCtrl_IsRunnning(void)
{
	int pid;
	
	pid = ntopid(VIBRATOR_CTRL_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}


/*---------------------------------------------------------------------------
 函数: VibratorCtrl_Process
 功能: 振动器控制进程
 参数: 无
 返回: 总是返回1，操作系统要求
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Process(void)
{
	int ret, vibratorAct;
	U32 runningTicks, ticks_cnt;
	
	VibratorCtrl_Init();
	
	runningTicks = 0;
	ticks_cnt = 0;
	vibratorAct = 1;//一开始就启动振动器
	while(1)
	{
		ret = semaphore_trywait(vibratorCtrlExit_sem);
		if (ret == 0)
		{
			//响应退出要求，停止振动器，退出进程
			hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			break;
		}
		if (vibratorCtrlRunningTicks!=0 &&
			runningTicks>=vibratorCtrlRunningTicks)
		{
			//运行时间到，停止振动器，退出进程
			hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			break;
		}
		
		if (vibratorAct == 1)
		{
			if (ticks_cnt == 0)
			{
				//启动振动器
				hyhwGpio_setHigh(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			}
			else if (ticks_cnt >= VIBRATOR_CTRL_RUN_TIME)
			{
				//振动时间到，该停止振动了
				vibratorAct = 0;
				ticks_cnt = 0;
				continue;
			}
		}
		else
		{
			if (ticks_cnt == 0)
			{
				//停止振动
				hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			}
			else if (ticks_cnt >= VIBRATOR_CTRL_STOP_TIME)
			{
				//停止振动时间到，该振动了
				vibratorAct = 1;
				ticks_cnt = 0;
				continue;
			}
		}
		ticks_cnt++;
		runningTicks++;
		syssleep(1);
		
	} /* while(1) */

	semaphore_destroy(vibratorCtrlExit_sem, OS_DEL_NO_PEND);
	vibratorCtrlExit_sem = NULL;
	
	return 1;
}


