/**************************************************
*目前程序中GSM模块经常出现问题,GPRS通道AT不响应/GSM信号消失/所有AT都不
*响应等,该进程主要工作是在GSM出现问题是重启GSM模块
***************************************************/
#include "hyTypes.h"
#include "glbVariable.h"
#include "ppp_gprs.h"
#include "time.h"
#include "semaphore.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "appControlProcess.h"

/////////////////////////////////////////////////////////////////////////////////
#define GSMRESET_P_NAME		"GsmReset_p"

SEMAPHORE	*GsmReset_p_Exit_sem = NULL;
int			*GsmResetWait = NULL;

U8 gu8GsmIsReset;//GSM是否正在复位   1：是   0：否
U8 gsmReset_enable = 1;

//////////////////////////////////////////////////////////////////////////
/*-------------------------------
* 函数:GsmReset_IfReset
* 功能:获取当前GSM模块状态(是否在重启)
* 参数:none
* 返回:0：不在重启   1：重启中
*-------------------------------*/
U8 GsmReset_IfReset()
{
	return gu8GsmIsReset;
}

void GsmReset_SetState(U8 state)
{
	gu8GsmIsReset = state;
}
/*-------------------------------
* 函数:GsmReset_WakeUp_Ctrl
* 功能:允许或禁止唤醒GSM RESET进程
* 参数:none
* 返回:none
*-------------------------------*/
void GsmReset_WakeUp_Ctrl(U8 enable)
{
	gsmReset_enable = enable;
}

/*-------------------------------
* 函数:GsmReset_WakeUp
* 功能:唤醒进程
* 参数:none
* 返回:none
*-------------------------------*/
void GsmReset_WakeUp()
{
	if(GsmResetWait != NULL)
	{
		ewakeup(&GsmResetWait);
	}
}

/*-------------------------------
* 函数:GsmReset_Process
* 功能:GSM模块出现问题时重启模块
* 参数:none
* 返回:1
*-------------------------------*/
int GsmReset_Process(void)
{
	unsigned ilev;
	int ret;
	
	GsmReset_p_Exit_sem = semaphore_init(0);
	
	while(1)
	{
		GsmReset_SetState(0);
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		ilev = setil(_ioff);
		ewait(&GsmResetWait);
		(void) setil(ilev);
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		if (gsmReset_enable == 0)
		{
			//GSM RESET已经被禁止，空转
			continue;
		}
		
		//置GSM模块状态为重启
		GsmReset_SetState(1);
		
		Ie_clear_connectfailcnt();
		//关闭gprs
		ppp_close(); 
		ppp_clear_brg_flag();
		//等待gprs关闭
		while((int *)ppp_get_eventppp() == NULL)
		//while(gprs_get_status() == 0)
		{
		//hyUsbMessage("close ppp\r\n");
			syssleep(5);
		}
		//hyUsbMessage("ppp exit\r\n");
		//ppp_p_Exit();
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		if(gtHyc.NetSelected == 1)
		{
			Ie_clear_connectfailcnt();
			netdev_close_tcpip();
		}
		
		gsmcdma_stop_service();
		
		if(gtHyc.NetSelected == 1)
		{
			syssleep(100);
			hyIE_resetSocket();
		}
		
		//开始重启模块
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		//hyUsbMessage("restart gsm\r\n");
		Gsm_InitToSdram_buf();
		gsm_SetPinCode(getPINCode());
		//hyUsbMessage("BBreset gsm\r\n");
		hyhwGsm_BBreset();
		syssleep(100);
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		AppCtrl_CreatPro(GSMCDMA_CLIENT_AN,0);
    	AppCtrl_CreatPro(GSMCDMA_SERVER_AN,0);
    	
    	AppCtrl_CreatPro(TCPIP_AN,0);
    	//hyUsbMessage("sleep gsm\r\n");
    	syssleep(100);
		SIM_set_pin_stat(1);
		//hyUsbMessage("set_pin\r\n");
		
		syssleep(600);
		
		Rec_SetScanTime(rawtime(NULL));
	}
	
	semaphore_destroy(GsmReset_p_Exit_sem, OS_DEL_NO_PEND);
	GsmReset_p_Exit_sem = NULL;
	GsmResetWait = NULL;
	
	return 1;
}

/*-------------------------------
* 函数:GsmReset_Start
* 功能:启动GSM监测进程
* 参数:none
* 返回:
*-------------------------------*/
int GsmReset_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(GSMRESET_P_NAME);
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
		//进程不存在，需要创建
		pid = createp((int (*)())GsmReset_Process,
								GSMRESET_P_NAME,
								1024,
								GSMRESET_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)GsmReset_Process );
	
	return HY_OK;
}

/*-------------------------------
* 函数:GsmReset_p_Exit
* 功能:退出GSM监测进程
* 参数:none
* 返回:
*-------------------------------*/
void GsmReset_p_Exit(void)
{
	int pid;
	
	pid = ntopid(GSMRESET_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (GsmReset_p_Exit_sem != NULL)
		semaphore_post(GsmReset_p_Exit_sem);
	
	ewakeup(&GsmResetWait);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}