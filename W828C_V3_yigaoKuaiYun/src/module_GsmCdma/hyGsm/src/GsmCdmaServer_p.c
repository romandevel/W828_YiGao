
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "semaphore.h"

#include "appControlProcess.h"



int gSIM_pin_ready = 0;
int gsmcdma_client_exit = 0;
int gsmcdma_server_exit = 0;
SEMAPHORE *gsmcdma_client_exit_sem = NULL;
SEMAPHORE *gsmcdma_server_exit_sem = NULL;


/*---------------------------------------------------------
*函数: SIM_set_pin_stat
*功能: 设置SIM卡验证PIN、PUK码的状态
*参数: stat	---	0:没有通过验证	1:验证OK
*返回: none
*---------------------------------------------------------*/
void SIM_set_pin_stat(int stat)
{
	gSIM_pin_ready = stat;
}


/*---------------------------------------------------------
*函数: SIM_get_pin_stat
*功能: 查询SIM卡验证PIN、PUK码是否通过
*参数: 
*返回: 0:没有通过验证		1:验证OK
*---------------------------------------------------------*/
int SIM_get_pin_stat(void)
{
	return gSIM_pin_ready;
}

void gsmcdma_powerOffthread(void)
{
	gsmcdma_adapter_poweronoff(1);
	syssleep(50);
	gsmcdma_adapter_poweronoff(0);
}

void gsmcdma_powerOff_start(void)
{
	int pid, startValue;
	
	pid = createp((int (*)())gsmcdma_powerOffthread,
							"gsmPowerOff_p",
							128,
							PRIORITY_4_OSPP,
							NULL,NULL,NULL);
	startValue = pstart( pid, (void *)gsmcdma_powerOffthread );
}

void gsmcdma_powerOff_stop(void)
{
	int pid;
	
	pid = ntopid("gsmPowerOff_p");
	if (pid<0 || pid>30) return;
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}
/*---------------------------------------------------------
*函数: gsmcdma_stop_service
*功能: 关闭gsm/cdma
*参数: 
*返回: none
*---------------------------------------------------------*/
void gsmcdma_stop_service(void)
{
#if 0
	gsmcdma_adapter_poweronoff(0);
#else
// 
	if (0 == AppCtrl_IsValidPro(GSMCDMA_SERVER_AN))	 return;
	
	//stop gsmcdma client thread
	gsmcdma_client_exit_sem = semaphore_init(0);
	gsmcdma_client_exit = 1;
	//semaphore_wait(gsmcdma_client_exit_sem);
	semaphore_timed_wait(gsmcdma_client_exit_sem,500);
	semaphore_destroy(gsmcdma_client_exit_sem,OS_DEL_NO_PEND);
	gsmcdma_client_exit_sem = NULL;
	gsmcdma_client_exit = 0;
	
	AppCtrl_DeletePro(GSMCDMA_CLIENT_AN);

	//stop gsmcdma server thread
	gsmcdma_server_exit_sem = semaphore_init(0);
	gsmcdma_server_exit = 1;
	//semaphore_wait(gsmcdma_server_exit_sem);
	semaphore_timed_wait(gsmcdma_server_exit_sem,500);
	semaphore_destroy(gsmcdma_server_exit_sem,OS_DEL_NO_PEND);
	gsmcdma_server_exit_sem = NULL;
	gsmcdma_server_exit = 0;
	
	AppCtrl_DeletePro(GSMCDMA_SERVER_AN);
#endif
}



extern tMSG_QUEUE *guser_reqmsg_que;
/*---------------------------------------------------------
*函数: GsmCdmaServer_ThreadMethodInit
*功能: 初始化一些变量或overlay变量等
*参数: 
*返回: none
*---------------------------------------------------------*/
static void GsmCdmaServer_ThreadMethodInit(void)
{
	gsmcdma_server_exit = 0;
	guser_reqmsg_que = NULL;
	SIM_set_pin_stat(0);
	init_variable();

}



/*---------------------------------------------------------
*函数: GsmCdmaServer_ThreadMethod
*功能: GsmCdmaServer进程
*参数: 
*返回: none
*---------------------------------------------------------*/
int GsmCdmaServer_ThreadMethod(void)
{
	GsmCdmaServer_ThreadMethodInit();
    //SIM_set_pin_stat(1);

	gsmcdma_server();

	return 1;
}


