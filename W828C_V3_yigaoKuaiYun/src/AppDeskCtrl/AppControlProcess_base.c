/*----------------------文件描述--------------------------------
* 创建日期: 2008/04/12 
* 描述    : 系统消息定义，包括键盘、鼠标、外部中断
*			应用程序启动，退出，挂起，恢复等
*           1、确切点说，当前的focus应用进程只有一个，也就是说不可能
*              同时存在两个应用进程都具有focus,
*           2、对于时间要求较高的应用，不能使用该架构方式来操作，例如:camera
*           3、在应用进程中，控件获取消息的时候是使用get方式来获取消息[camera不太适合这种方式]   
*--------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "appControlProcess.h"

/*
* extern function or variable
*/


/* 全局变量，系统的事件 */
tKERNEL_EVENT gtkernelEvent;

/* 全局变量，有了系统事件后后，需要唤醒的进程列表，现在就一个控制台进程 */
void *pvEventWait	= NULL;
tAPPLICATION	gtActiveApp[TOTAL_APP_AN];


/*---------------------------------------------------------------------------
 函数：AppCtrl_DeletePro
 功能：杀死进程和消息队列并填充信息表
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
void AppCtrl_DeletePro(U16 id)
{ 
	U16 pid;
	
	if (id >= TOTAL_APP_AN)
	{
	    return;
	}
	
	if (APPCTRL_INVALID_PID == gtActiveApp[id].pid) return;	
	
	pid = gtActiveApp[id].pid;
	gtActiveApp[id].pid = APPCTRL_INVALID_PID;
	gtActiveApp[id].isFocus	= 0;	

	//只处理管理信息，真正的进程销毁不在这里做---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		return;
	}

	if (NULL != gtActiveApp[id].ptmsg)
	{
		msg_queue_destroy(gtActiveApp[id].ptmsg);
		gtActiveApp[id].ptmsg = NULL;						
	}
	 
	//由控制台进程去杀死其他应用进程
	kill(pid,1);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
	
}
/*---------------------------------------------------------------------------
 函数：AppCtrl_IsValidPro
 功能：进程是否创建
 参数：无
 返回：1---已创建
 	   0---没有创建
* -----------------------------------------------------------------------------*/
int AppCtrl_IsValidPro(U16 id)
{
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid)
	{
		return 1;
	}
	return 0;
}