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
#include "hyTypes.h"
#include "std.h"
#include "msgQueue.h"
#include "ospriority.h"
#include "hyhwGpio.h"
#include "sys.h"
#include "hyhwRtc.h"
#include "hyhwUart.h"
#include "HycDeviceConfig.h"
#include "hyUsbDebug.h"
#include "PhoneDebug.h"
#include "beeperCtrl.h"
#include "AbstractFileSystem.h"
#include "appControlProcess.h"
#include "glbStatusDef.h"
#include "AppCtrlConstData.h"
#include "Battery.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "grap_api.h"
#include "grap_view.h"
#include "Common_Dlg.h"
#include "AppControlConsume.h"

#include "DisplayMemory.h"
#include "Common_Widget.h"
#include "LcdModule_BackLight.h"

/*
* extern function or variable
*/
extern void SoftWare_Init(void);

U8  gu8USBtest;//0:未开启debug功能   1:已经开启,但还未连接    2:已经成功连接debug
static U32 gu32UsbDebugTime = 0;//kong 2012-10-22


static U32 VbsInvalidTime;
//当控制台要求系统退出时记录的信息表
static tSYS_REQ_QUIT_INF sgtSysReqInf;

/* 全局变量，控制台进程的接收信息的消息队列 */
tMSG_QUEUE *ptMsg_controlDesk;

//static tAPPLICATION	gtActiveApp[TOTAL_APP_AN];
extern tAPPLICATION	gtActiveApp[TOTAL_APP_AN];

void AppCtrl_CreatPro(U16 id , void *pText);


U8 App_GetUsbDebugStat()
{
	return gu8USBtest;
}


void App_SetVbsInvalidTime(void)
{
	VbsInvalidTime = rawtime(NULL);
}
/*---------------------------------------------------------------------------
 函数：AppCtrl_IntialVariable
 功能：初始化控制结构体
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_IntialVariable(void)
{
    int i;
    
    for (i=0; i<TOTAL_APP_AN; i++)
	{
		gtActiveApp[i].pid = APPCTRL_INVALID_PID;
		gtActiveApp[i].isFocus = 0;	
		gtActiveApp[i].ptmsg = NULL;	
	}  
	
	pvEventWait	= NULL;
	memset(&gtkernelEvent,0,sizeof(tKERNEL_EVENT));
	
	memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));	 
	
	ptMsg_controlDesk =  msg_queue_init(10, 40);  
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_SysReqQuitAction
 功能：创建进程和消息队列并发送相应的focus消息
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_SysReqQuitAction(void)
{ 	
	U16 startpid;
	U16 mypid;
	U8 flag;
	U8  msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;

	startpid = sgtSysReqInf.StartPid;
	mypid = sgtSysReqInf.MyPid;
	flag =  sgtSysReqInf.Flag;

	if (APPCTRL_INVALID_PID == gtActiveApp[startpid].pid)
	{
		AppCtrl_CreatPro(startpid,0);
	}

	ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
	ptPutMsg->body.msg.Pid.MyPid = mypid; //是那个进程激活了这个应用进程，告诉对方
	ptPutMsg->body.msg.Pid.StartPid = startpid;//将要激活的进程
	ptPutMsg->body.msg.Pid.Flag = flag; //进程启动时的附加信息
	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
	ptPutMsg->priority = 10;
	msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);

}

/*---------------------------------------------------------------------------
 函数：AppCtrl_SysAckStopMusic
 功能：控制台按照应用的要求已经停止了后台音乐播放，并进行消息回发确认
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_SysAckEnterUSB(void)
{
	U32 i;
	U8  msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
    
    USBHandler_SetScsiFlag(0);
    
    memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
    
    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
	ptPutMsg->length  = sizeof(eMSG_ID);
	ptPutMsg->priority = 10;
	for (i=0; i<TOTAL_APP_AN; i++)
	{ 	
		tAPPLICATION *pAppActive;
		int pid;
		
		pAppActive = &gtActiveApp[i];
		pid = pAppActive->pid;
		if(APPCTRL_INVALID_PID != pid  && NULL != pAppActive->ptmsg && PSPROC(pid)->state != P_INACT)
		{
			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
			sgtSysReqInf.ReqPidNum++;
		}
	}
		
    if(sgtSysReqInf.ReqPidNum == 0)
    {
    	//当前没有正在运行的引用进程，没有发送任何消息
    	memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
    	return;
    }
    
    //创建USB进程
    sgtSysReqInf.StartPid = USB_HANDLER_AN;
    sgtSysReqInf.MyPid = TOTAL_APP_AN;
    sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
    
    return;
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_CreatPro
 功能：创建进程和消息队列并填充信息表
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro(U16 id , void *pText)
{
	U32	startValue;
	tPROCESS_CONFIG  *ptProcConfig;
	ptProcConfig	= &(gtproc_Config[id]);
	
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid) return;
	
	ConSume_UpdataAppCoef(id, 0);
	
	//只处理管理信息，真正的进程创建不在这里做---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		return;
	}
	
	gtActiveApp[id].pid = createp(  ptProcConfig->entry,
					                ptProcConfig->pname,
					                ptProcConfig->stackSize,
					                ptProcConfig->priority,
					                NULL,NULL,NULL);

	if (ptProcConfig->msgQueDeep != 0)
	{//有消息队列才创建消息队列
		gtActiveApp[id].ptmsg	= msg_queue_init(ptProcConfig->msgQueDeep, ptProcConfig->msg_maxSize);
	}

	startValue = pstart( gtActiveApp[id].pid, (void *)ptProcConfig->entry);
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_CreatPro_Virtual
 功能：虚拟创建进程并填充信息表
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro_Virtual(U16 id , int pid)
{
	U32	startValue;
	tPROCESS_CONFIG  *ptProcConfig;
	ptProcConfig	= &(gtproc_Config[id]);
	
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid) return;
	
	//只处理管理信息，真正的进程创建不在这里做---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		gtActiveApp[id].pid = pid;
		ConSume_UpdataAppCoef(id, 0);
	}
}

/*---------------------------------------------------------------------------
 函数：USB_VbusDetect
 功能：USB Vbus的检测		 
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
static int USB_Need_USB_Status(void)
{
    int rc = 2;
    tAPPLICATION *pAppPowerOn,*pAppPowerDwon;
    
    //开机时直接进入USB
    pAppPowerOn = AppCtrl_GetAppInfor(POWER_ON_AN);
    
    pAppPowerDwon = AppCtrl_GetAppInfor(POWER_DOWN_AN);
    
    if (1 == pAppPowerOn->isFocus)
    {
    	int pid;
    	pid = gtActiveApp[POWER_ON_AN].pid;
    	if (PSPROC(pid)->state == P_INACT)
    	{
    		//开机进程已经exit，但CtrlDesk还未处理完其退出信息
    		//所以不能向其发送USB消息
    		rc = 3;
    	}
    	else
    	{
    		rc = 1;
    		AppCtrl_SysAckEnterUSB();
    	}
        return rc;
    }
    else if (1 == pAppPowerDwon->isFocus)
    {
    	int pid;
    	pid = gtActiveApp[POWER_DOWN_AN].pid;
    	if (PSPROC(pid)->state == P_INACT)
    	{
    		rc = 3;
    	}
    	else
    	{
    		rc = 1;
    		AppCtrl_SysAckEnterUSB();
    	}
        return rc;
    }    
    
    if (NULL != sgtSysReqInf.AppCtrl_Action)
    {
        rc = 0;        
    }
    
    return rc;        
}

/*---------------------------------------------------------------------------
 函数：USB_VbusDetect
 功能：USB Vbus的检测
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
static void USB_VbusDetect(void)
{
	//#define ADAPTER_DETECT_COUNT	(100)//100*10ms = 1s，以系统tick计
	#define ADAPTER_DETECT_COUNT	(400)//400*10ms = 4s，以系统tick计
	/*
	USB检测逻辑：
		1.检测Vbus是否跳高
		2.如果在3次(总时间根据当前进程多长时间运行一次而变化，现在是2ticks运行一次)
		3.确认Vbus跳高后，进行USB的硬件初始化
		4.在1秒内检测是否为USB线插入，如果是，则启动并进入USB进程；如果不是，则为适配器插入
	*/
	static U8 vbus_scan_cnt = 0;
	static U8 usbstate = 0;
	static int usb_sysTicks = 0;   	
	//static U32 VbsInvalidTime;
											 
	U32 i;
	U8  msgputbuf[10]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	int vbus_status;
	
#ifdef USB_DEBUG
	if (hyUsbTrans_isRunning() == HY_OK && USBHandler_USBConnect() == 0)
	{
	     //重新进行初始化
	     hyUsbDebugDeviceInit();
	}
	else if (hyUsbTrans_isRunning() == HY_OK)
	{
		hyUsbTrans_DetectStart();
		return;
	}
	else if(gu8USBtest == 2 && hyUsbTrans_isRunning() == HY_ERROR)//kong 2012-10-22
	{
		gu8USBtest = 0;
	}
#endif
	
    #if 1
	if (AppCtrl_IsValidPro(USB_HANDLER_AN) == 1)
	{
		if(0 == usb_inflag_get() && 1 == USBHandler_USBConnect())
		{
			usb_inflag_set(1);
		}
		
		return;
	}
	#endif
    if (sgtSysReqInf.AppCtrl_Action == AppCtrl_SysAckEnterUSB)
    {//USB回应是否超时
        if (ABS(rawtime(NULL)-VbsInvalidTime) > 550)
        {
            memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF)); 
        }
   	}
	vbus_status = USBHandler_USBConnect();
	if (USBHandler_GetScsiFlag() == 0)
    {
        if(vbus_status == 1)
        {           
            if (0 == usbstate)
            {
	            vbus_scan_cnt++;
	            if (vbus_scan_cnt >= 1)
	            {//Vbus滤波，确认
	            	usbstate = 1;
	            	vbus_scan_cnt = 0;
	            	
	     	        hyudsDcd_HwInit();
	     	        usb_sysTicks = rawtime(NULL);
	            }
            }
            
            if (1 == usbstate)
            {
            	int sysTicks;int rc;
            	
            	sysTicks = rawtime(NULL);            	
            	if (TRUE == hyudsDcd_GetSpeedDetectFlag())
            	{//确认USB线插入，启动并进入USB进程
					
            		usbstate = 2;
            		
        	    	USBHandler_SetScsiFlag(2);
            		rc = USB_Need_USB_Status();
	        	    if (3 == rc)
	        	    {
	        	    	//rc = 3还不能发送消息启动USB进程
	        	    	//恢复状态，下一个循环再检测
	        	    	USBHandler_SetScsiFlag(0);
	        	    	usbstate = 1;
	        	        return;
	        	    }
	        	    else if (2 != rc)
	        	    {
	        	    	//rc = 1开关机过程中，直接进入USB状态
	        	    	//rc = 0不能进入USB的情况(在电话界面上，无公共控件界面上等)，直接进入充电状态
						hyudsDcd_StopSpeedDetect();
	        	        return;
	        	    }
					hyudsDcd_StopSpeedDetect();
	        	    
#ifdef USB_DEBUG
	if(gu8USBtest == 1)
	{
		if (hyUsbTrans_softInit() == HY_OK)
		{
			//hyUsbMessage("hyUsbTrans_softInit OK!!\r\n");
			
			syssleep(100);
			hyUsbTrans_Init();
			
			usbstate = 2;
			gu8USBtest = 2;////kong 2012-10-22 成功连接USB debug
			return ;
		}
	}
#endif
					
	        	    if(NULL != sgtSysReqInf.AppCtrl_Action) return ;
	        	    //弹出对话框，用户可选择是否进入U盘状态
	            	ptPutMsg->body.id = USE_USB_IS_ACT;
	            	ptPutMsg->length  = sizeof(eMSG_ID);
	            	ptPutMsg->priority = 10;
	            	
	            	memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
	            	sgtSysReqInf.ReqPidNum = 0;
	            	for (i=0; i<TOTAL_APP_AN; i++)
	            	{
	            		if (1 == gtActiveApp[i].isFocus && NULL != gtActiveApp[i].ptmsg)
	            		{
	            		    msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
	            		    sgtSysReqInf.ReqPidNum++;
	            		}
	            	}
	            	
			        //创建USB进程
			        sgtSysReqInf.StartPid = USB_HANDLER_AN;
			        sgtSysReqInf.MyPid = TOTAL_APP_AN;
					sgtSysReqInf.AppCtrl_Action = AppCtrl_SysAckEnterUSB;
					
					VbsInvalidTime = sysTicks;
            		return;
            	}
            	else if (ABS(sysTicks - usb_sysTicks) >= ADAPTER_DETECT_COUNT)
            	{//确认适配器线插入，进入充电状态
					
					hyudsDcd_HwDeinit();
					hyudsDcd_StopSpeedDetect();
            		USBHandler_SetScsiFlag(2);
            		usbstate = 2;
            		
            		return;
            	}
            }
        }
        else
        {
            vbus_scan_cnt = 0;//清除计数
            usbstate = 0;
        }
    }
    
    if (vbus_status == 0)
    {
		if (AppCtrl_IsValidPro(USB_HANDLER_AN) == 0)
		{
    		if (2 == usbstate)
    		{
        		hyudsDcd_HwDeinit();
        		hyudsDcd_StopSpeedDetect();
    		}

	    	if (USBHandler_GetScsiFlag() == 2)
	    	    USBHandler_SetScsiFlag(0); 
    	}
    	
    	vbus_scan_cnt = 0;//清除计数
    	usbstate = 0;
    }
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_GetAppInfor
 功能：返回进程信息，见结构 tAPPLICATION
 参数：appindex ; 应用进程的id号
 返回：进程信息指针
* -----------------------------------------------------------------------------*/
tAPPLICATION *AppCtrl_GetAppInfor(eAPPLICATION_NO appindex)
{
	 tAPPLICATION *pApp = NULL;
	 
	 if (TOTAL_APP_AN > appindex)
	 {
	 	pApp =  &gtActiveApp[appindex];
	 }
	 
	 return pApp;
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_GetCurAppPid
 功能：得到当前运行进程的虚pid号
 参数：none
 返回：进程虚pid号
* -----------------------------------------------------------------------------*/
U32 AppCtrl_GetCurAppPid(void)
{
	 tAPPLICATION *pApp = NULL;
	 U32 i;
	 
	 for (i=0; i<TOTAL_APP_AN; i++)
	 {
	    pApp =  &gtActiveApp[i];
	    if (1 == pApp->isFocus) break;  
	 } 
	 						
	 return i;
} 

/*---------------------------------------------------------------------------
 函数：AppCtrl_GetPidFromSystem
 功能：得到当前应用进程的虚pid号
 参数：none
 返回：进程虚pid号
* -----------------------------------------------------------------------------*/
S32 AppCtrl_GetPidFromSystem(void)
{
	S32 pid,i;
    S32 curPid = -1;
    
    pid = getpid();
	 
	 for (i=0; i<TOTAL_APP_AN; i++)
	 {
	    if (pid == gtActiveApp[i].pid)
	    {
	        curPid = i;
	        break;
	    }
	 } 
	 
	 return curPid;
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_PowerOffProc
 功能：机械按键关机处理  是关机键，先让应用进程退出，再启动关机进程，USB状态下关机还得另外再考虑 如果是开机键，再分情况讨论
 参数：无
 返回：1：成功 0：失败此时是USB状态应该进行开机动作
* -----------------------------------------------------------------------------*/ 
U32 AppCtrl_PowerOffProc(void)
{
	U32 i;
	U32 sendMsgStatus;
	U8  flag = 0;//是开机还是关机状态 1：开机 0 ：关机
	U8  msgputbuf[20]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	
	if (gtActiveApp[POWER_DOWN_AN].isFocus == 1) return flag;
	
	if (gtActiveApp[USB_HANDLER_AN].isFocus != 1 && gtActiveApp[POWER_ON_AN].isFocus != 1)
	{//不是USB
		flag = 1;
	}
	
	if (NULL != sgtSysReqInf.AppCtrl_Action) return flag;//已经有按键关机
	
	if (1 == flag)
	{//开机状态下则进行关机操作
	
		ptPutMsg->body.id =  SYS_REQ_QUIT_EVENT;
		ptPutMsg->priority = 10;
		ptPutMsg->length = sizeof(eMSG_ID);

		for(i = 0; i<TOTAL_APP_AN; i++)
		{
			if(APPCTRL_INVALID_PID != gtActiveApp[i].pid && NULL != gtActiveApp[i].ptmsg)
			{
				/*这个消息一定得发出去，不然关机有BUG*/
				sendMsgStatus	= msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
				//gtActiveApp[i].isFocus = 0;
				sgtSysReqInf.ReqPidNum++;
			}
		}
		
		//创建关机进程关机
		sgtSysReqInf.StartPid = POWER_DOWN_AN;
		sgtSysReqInf.MyPid = TOTAL_APP_AN;
		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		
	}
	
	return flag;
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_PowerOffShow
 功能：
 参数：无
 返回：1：成功 0：失败
* -----------------------------------------------------------------------------*/ 
U8 AppCtrl_PowerOffShow(void)
{
	U32 i;
	U32 sendMsgStatus;
	U8  msgputbuf[20]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	
	
	if (1 == gtActiveApp[USB_HANDLER_AN].isFocus
	  || 1 == gtActiveApp[POWER_ON_AN].isFocus
	  || 1 == gtActiveApp[POWER_DOWN_AN].isFocus)
	{//不是USB
		return 0;
	}
	
	if (NULL != sgtSysReqInf.AppCtrl_Action) return 0;//已经有按键关机或者USB有插入动作了
	
	
	ptPutMsg->body.id = USE_REQ_POWERDOWN;
	ptPutMsg->length  = sizeof(eMSG_ID);
	
	ptPutMsg->priority = 10;
	
	for (i=0; i<TOTAL_APP_AN; i++)
	{
		if (gtActiveApp[i].isFocus)
		{
			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
			break ;
		}
	}
	
	sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction ;
	
	return 1 ;
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_WakeupPutGWKey
 功能：从低功耗唤醒时，GW键唤醒
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void AppCtrl_WakeupPutGWKey(int type)
{
	drvKeyb_SetEvent(Keyb_Code_Power, cKEYB_EVENT_RELEASE);
}

/*---------------------------------------------------
 * 函数 : AppCtrl_Record_Trace
 * 功能 : 记录坐标点
 * 参数 : pbuf:传入的buf
 * 返回 : 得到的字符数 
---------------------------------------------------*/
static void AppCtrl_Record_Trace(int type , int x, int y)
{
    if (type == VIEW_PULL_END)
    {
        hyHWRecog_RecogTrace(-1,-1);
    }
    else
    {
        //hzrecog_rec_trace(x,y); 
        hyHWRecog_RecogTrace(x,y); 
    }
}

/*------------------------------------------
* 函数:AppCtrl_UsbDebugTimer
* 功能:usbdebug超时关闭
* 参数:
* 返回:
*------------------------------------------*/
void AppCtrl_UsbDebugTimer()//kong 2012-10-22
{
	//是否超时未连接debug
	if(gu8USBtest == 1 && ABS(rawtime(NULL) - gu32UsbDebugTime) >= 1000)
	{
		gu8USBtest = 0;
		hyUsbTrans_DeInit();
	}
}

//*135791  开启debug   *135790  关闭   超时未连接关闭
/*---------------------------------------------------------
* 函数:AppCtrl_UsbDebug
* 功能:打开或关闭usbdebug功能;  连续输入 *135791 打开usb debug     连续输入*135790 关闭usb debug功能   (按键之间不能超过2S)
* 参数:keycode:当前按键值
* 返回:
*--------------------------------------------------------*/
void AppCtrl_UsbDebug(Keyb_Code_e keycode)//kong 2012-10-22
{
	static Keyb_Code_e	lastKey = 0;
	
	if(lastKey != 0 && Keyb_Code_Xing != keycode && ABS(rawtime(NULL) - gu32UsbDebugTime) > 200)
	{
		lastKey = 0;
		return ;
	}
	
	switch(lastKey)
	{
	case 0:
		if(keycode == Keyb_Code_Xing)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		break;
	case Keyb_Code_Xing:
		if(keycode == Keyb_Code_1)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_1:
		if(keycode == Keyb_Code_3)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_3:
		if(keycode == Keyb_Code_5)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_5:
		if(keycode == Keyb_Code_7)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_7:
		if(keycode == Keyb_Code_9)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_9:
		if(keycode == Keyb_Code_0)//关闭
		{
			if(gu8USBtest != 0)
			{
				gu8USBtest = 0;
				hyUsbTrans_DeInit();
			}
		}
		else if(keycode == Keyb_Code_1)//开启
		{
			if(gu8USBtest == 0)
			{
				gu8USBtest = 1;
				USBHandler_SetScsiFlag(0);
				hyudsDcd_connectInit();
				
				gu32UsbDebugTime = rawtime(NULL);
			}
		}
		lastKey = 0;
		break;
	default:
		lastKey = 0;
		break;
	}
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_ProcIRQEvent
 功能：控制台对IRQ消息分析并处理
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
__inline void AppCtrl_ProcIRQEvent(U32 event)
{
	U32		i,keyEvent;
	S32		sendMsgStatus;
	U8		flag = 1;//1：消息正常 0：没有消息 2:特殊消息
	U8		msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	Keyb_Code_e eKeyCode;
	tPOINT_KEY_PULL	pointKeyPull;
	static U32 lasttick = 0;
	static U8  powerncnt = 0;
	int haveLight;
	
	 /*消息发送是有等级的，我们认为用户操作的优先级要高于一般的timer消息，故在发送消息的时候
	   按键的优先级为6，而timer消息的优先级为5
	*/
	if(KERNEL_EVENT_KEY_INPUT == (event&KERNEL_EVENT_KEY_INPUT))
	{
		/* 如果是按键消息，发送按键消息给焦点应用进程的接收消息队列 */				
		
		ConSume_ChangeCoefTemp(192,0);
		haveLight = BackLight_HaveLight();
		
	    if (drvKeyb_GetEvent(&eKeyCode,&keyEvent) == cSSA_OK)
	    {
		#ifdef USB_DEBUG
			
			AppCtrl_UsbDebug(eKeyCode);//kong 2012-10-22

		#endif
			
			if(eKeyCode == Keyb_Code_RightLongPressed)
			{
				Net_AddSendEvt();//设置上传标记
			}
			
	    	//PhoneTrace1(0,"key = %x", eKeyCode);
	    	if(Keyb_Code_PowerLongPressed == eKeyCode && cKEYB_EVENT_NORMAL == keyEvent)
	    	{
	    		powerncnt++;    		
	    	}
	    	else
    		{
    			powerncnt = 0 ;
    		}
	    	
	    	ptPutMsg->body.id =  KEY_INPUT_EVENT;
	    	ptPutMsg->body.msg.Key.keycode = eKeyCode;
	    	ptPutMsg->body.msg.Key.keyevent = keyEvent;
	    	ptPutMsg->priority = 6;
	    	ptPutMsg->length = sizeof(tKEY_EVENT)+sizeof(eMSG_ID);
	    	
	    	
	    	//屏状态时
	    	if(0 == haveLight)
	    	{
	    		flag = 0;
	    		ptPutMsg->body.id = KEY_WAKEUP_SYS;
	    		
	    		//通过发消息唤醒点亮显示屏
	    		if((Keyb_Code_OK == eKeyCode || Keyb_Code_F1 == eKeyCode || Keyb_Code_Power == eKeyCode || Keyb_Code_ScanL == eKeyCode || Keyb_Code_ScanR == eKeyCode) &&
	    			cKEYB_EVENT_RELEASE == keyEvent)//只有power键可以唤醒
				{
	    	    	flag = 2;
	    	    }
	    	}
	    	else
	    	{
	    	 //按键音  只有短按响
	    		if(gtHyc.KeySound && cKEYB_EVENT_RELEASE == keyEvent)
	    		{
	    			if(eKeyCode != Keyb_Code_Power 
	    			&& eKeyCode != Keyb_Code_ScanL 
	    			&& eKeyCode != Keyb_Code_ScanR
	    			&& eKeyCode != Keyb_Code_ScanM
	    			&& eKeyCode != Keyb_Code_PowerLongPressed 
	    			&& eKeyCode != Keyb_Code_ScanLLongPressed
	    			&& eKeyCode != Keyb_Code_ScanRLongPressed
	    			&& eKeyCode != Keyb_Code_ScanMLongPressed )
	    			{
	    				//beeper_Start(KEY_SOUND,1,0);
	    				Remind_Fun(KEY_SOUND, 1);
	    			}
	    		}
	    	}
	    	
	    	BackLight_KeyLight();
	    	
	    	if (flag == 1)
	    	{
	    		if(Keyb_Code_PowerLongPressed == eKeyCode && 4 <= powerncnt)
	    		{
	    			/*if(Net_GetEvt() != 0) // 有后台传输
	    			{
	    				AppCtrl_PowerOffShow();
	    				flag = 0;
	    			}
	    			else */if (1 == AppCtrl_PowerOffProc())	    			
			 		{
			 			flag = 0;
			 		}
	    		}
	    	}
	    	else if (2 == flag)
	    	{
	    	    flag = 1;
	    	} 
	    	
	    	//更新背光时间
	    	BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	    }
	    else if (ts_get_touch_pull_position(&pointKeyPull) == HY_OK)
	    {     
	        int key;
	        U8 pullflag = 1;
	    	
		    if(pullflag == 0)
	        {
	        	ptPutMsg->body.id = POP_UNLOCKDIALOG_EVENT;
	        }
	        else
	        {
	        	ptPutMsg->body.id = TOUCH_KEY_PULL_EVENT;
	    	}
	    		
	    	ptPutMsg->body.msg.PointKeyPull.x = pointKeyPull.x;
	    	ptPutMsg->body.msg.PointKeyPull.y = pointKeyPull.y;
	    	ptPutMsg->body.msg.PointKeyPull.endFlag = pointKeyPull.endFlag;
	    	ptPutMsg->priority = 6;
	    	ptPutMsg->length = sizeof(tPOINT_KEY_PULL)+sizeof(eMSG_ID);
	    	
	    	AppCtrl_Record_Trace(pointKeyPull.endFlag,pointKeyPull.x,pointKeyPull.y);
	    	
	    	if (0 == haveLight)
	    	{
	    	    flag = 0;
	    	}
	    	else
	    	{
				BackLight_KeyLight();
	    	}
	    	
	    	//更新背光时间
	    	BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	    }
	    else
	    {//防止取不到数值而发空消息
	    	flag = 0;
	    }
	     
		/* 给具有焦点的应用进程发信息*/
		if (1 == flag)
		{
			for(i = 0; i<TOTAL_APP_AN; i++)
			{
				if(gtActiveApp[i].isFocus)
				{
					/*暂时没有考虑消息发送不出的情况*/
					sendMsgStatus	= msg_queue_tryput(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
					break;
				}
			}
		}
	}			
	
	ConSume_ChangeCoefTempElapseTime();
	
	if(KERNEL_EVENT_TIMER == (event&KERNEL_EVENT_TIMER))
	{ 								
		extern U32	gTime;
		
		/*ie隔1s发一次timer事件*/
		static U32  ieLastTick = 0;	
		int ieTickFlg = 0;	
		
	#ifdef USB_DEBUG
		AppCtrl_UsbDebugTimer();//kong 2012-10-22
	#endif
		
		ptPutMsg->body.id =  SYS_TIMER_EVENT;
    	ptPutMsg->body.msg.Timer = gTime;    	
    	ptPutMsg->priority = 5;
    	ptPutMsg->length = sizeof(U32)+sizeof(eMSG_ID);
    	
    	if (ABS(gTime-ieLastTick) > 50)
    	{
    	    ieLastTick = gTime;
    	    ieTickFlg = 1;
    	}
    	
		/*只有focus进程才能够接受timer消息*/
		for(i = 0; i<TOTAL_APP_AN; i++)
		{
			if(gtActiveApp[i].isFocus)
			{
				sendMsgStatus = msg_queue_tryput(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
				break;
			}
		}
		
		AppCtrl_Record_Trace(VIEW_PULL_END,-1,-1);
	#if 1	
		//屏保判断
		if (cSCREENPROTECT_CLOSE != gtHyc.ScreenProtect)
		{
			if (1 != BackLight_AvoidThis())//判断是否需要屏蔽背光
			{
    			if(ABS(gTime-lasttick)>= 100)
    			{
    				lasttick = gTime;
    				BackLight_CalculateTime();
    			}
			}
		}
	#endif
	}
}


/*---------------------------------------------------------------------------
 函数：AppCtrl_RespondMsg
 功能：控制台进程收到消息后做响应的动作
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_RespondMsg(tMSG_BODY *pMsg)
{
	U32 i;
	U16 mypid;
	U16 startpid;
	eMSG_ID event;
	U8  msgputbuf[60]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	tPROCESS_CONFIG  *ptProcConfig;	 

    //进程切换时改变频率，加快切换速度
    ConSume_ChangeCoefTemp(192,1);
    
	event =  pMsg->id;
	
	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;

	switch(event)
	{
		case USE_LOST_FOCUS_EVENT://应用进程主动丢失焦点，应用进程处于背景状态，启动相关的UI进程 	
			startpid = pMsg->msg.Pid.StartPid;
			mypid = pMsg->msg.Pid.MyPid;
			
			//检测当前要求启动的进程是否为独占的UI进程，如果是则需停止后台进程运行让其退出 			   
	        ptProcConfig =  &gtproc_Config[startpid];			
			{ 			
			
				if (APPCTRL_INVALID_PID == gtActiveApp[startpid].pid)
				{
					AppCtrl_CreatPro(startpid,0);				
				}			
				
				//发送focus消息,激活阻塞的进程
				ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
		    	ptPutMsg->body.msg.Pid.MyPid = mypid; //是那个进程激活了这个应用进程，告诉对方
		    	ptPutMsg->body.msg.Pid.StartPid = startpid;//将要激活的进程
		    	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;
		    	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
		    	ptPutMsg->priority = 10;
				msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
							
				gtActiveApp[startpid].isFocus	= 1;
				//把发给自己的进程focus置为0 
			}
			
			gtActiveApp[mypid].isFocus = 0;
			break;
		 				
		case USE_REQ_START_TASK_EVENT://应用进程主动要求启动相关进程，而自己则自杀	
			mypid = pMsg->msg.Pid.MyPid;
			startpid = pMsg->msg.Pid.StartPid;			
			
			//清楚信息，防止插入USB后不响应消息
			if (mypid == POWER_ON_AN && startpid == SHELL_AN)
			{
			    memset(&sgtSysReqInf,0,sizeof(sgtSysReqInf));
			}
			
			AppCtrl_DeletePro(mypid);	
			
			//检测当前要求启动的进程是否为独占的UI进程，如果是则需停止后台进程运行让其退出 			   
	        ptProcConfig =  &gtproc_Config[startpid];
			
			{
			
    			if (gtActiveApp[startpid].pid == APPCTRL_INVALID_PID)
    			{					
    				AppCtrl_CreatPro(startpid,0);				
    			}			
    			
    			if (NULL != gtActiveApp[startpid].ptmsg)
    			{
    				S32 test;
    				ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
    		    	ptPutMsg->body.msg.Pid.MyPid = mypid;      //是那个进程激活了这个应用进程，告诉对方
    		    	ptPutMsg->body.msg.Pid.StartPid = startpid;//将要激活的进程
    		    	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;
    		    	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
    		    	ptPutMsg->priority = 10;
    				test = msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
    			
    			    if (test >= 0) test++;
    			}			
    			gtActiveApp[startpid].isFocus	= 1;
			}
			break;
						
		case USE_ACK_KILL_SELF:				
		case USE_QUIT_EVENT://系统进程收到该消息后系统强行杀掉进程，回收资源					
			AppCtrl_DeletePro(pMsg->msg.Pid.MyPid);			
			break;
			
		case USE_DEPRIVED_FOCUS_EVENT://应用焦点已经被剥夺了，现在控制台可以做相关事宜		
			for (i=0; i<TOTAL_APP_AN; i++)
			{
			    if (APPCTRL_INVALID_PID != gtActiveApp[i].pid && pMsg->msg.MsgQueAdd == (U32)gtActiveApp[i].ptmsg)
			    {
			        gtActiveApp[i].isFocus	= 0;
			        break; 
			    }
			    
			}
			
			if (0 != sgtSysReqInf.ReqPidNum) sgtSysReqInf.RecPidNum++;
			
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();
		   			gtActiveApp[sgtSysReqInf.StartPid].isFocus	= 1;	
				}
				memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
			}			
			break;
		case USE_ACK_QUIT_EVENT://应用进程确认退出控制台回收资源
			for (i=0; i<TOTAL_APP_AN; i++)
			{				
				if (pMsg->msg.MsgQueAdd == (U32)gtActiveApp[i].ptmsg)
				{//消息确认退出的时候发送自己消息队列指针
				    AppCtrl_DeletePro(i);
				    
				    if (0 != sgtSysReqInf.ReqPidNum) sgtSysReqInf.RecPidNum++;
				    
					break;					
				}
			}	   
		
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();
		   			gtActiveApp[sgtSysReqInf.StartPid].isFocus	= 1;  		   			
				}
				memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
			}
					
			break;
						
		case USE_KILL_OTHER_EVENT://去让其他进程退出应用
		    startpid = pMsg->msg.Pid.StartPid;
			mypid = pMsg->msg.Pid.MyPid;
			
			ptPutMsg->body.id =  SYS_REQ_QUIT_EVENT;			    	
	    	ptPutMsg->length = sizeof(eMSG_ID);
	    	ptPutMsg->priority = 10;
	    	
	    	if (NULL != gtActiveApp[startpid].ptmsg)
			msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
		    break;	
		case USE_BATTERY_LOW_EVENT:
		    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //创建关机进程
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 1;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		    break;             		
		 case USE_USB_ENTER_ACT:		 
		    sgtSysReqInf.RecPidNum++;  			
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();		   			
				}
				else
				{
				    memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
				}    
			}					  
		    break;
		 case USE_USB_IGNORE:
		    memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
		    break;                   		
		
		//进行强制关机程序    
		 case USE_REQ_POWERDOWN:
		    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //创建关机进程
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 2;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		    
		    break;
		case SYS_REQ_TIPSHOW :
			ptPutMsg->body.id = SYS_REQ_TIPSHOW;
        	ptPutMsg->length  = sizeof(eMSG_ID) + SYS_DATA_SHOWLEN;
        	
        	memcpy(ptPutMsg->body.msg.buf,pMsg->msg.buf,ptPutMsg->length);
        	
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (gtActiveApp[i].isFocus)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			break ;
        		}
        	}
			break;
	default:	
		break;
	}
	
	return;
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_BatteryLevDetect
 功能：检测电量低的时候，关机操作
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_BatteryLevDetect(void)
{
    static U16 batterLowCnt = 0;
    static U32  deadUiTime = 0; /* UI 死机计时,超过一定时间认为UI进程死掉了 */
     
    #define BATTERY_POWER_DWON_CNT  200 //10s内连续监测到低于3.5V则关机处理
    int enPowerDn;
    U32 batlev;
  	
    batlev = Battery_Detect();
    
    if(1 == Battery_DcInCheck())
    {
    	deadUiTime = 0;
    	return ; /* Dc in 不检测关机 */
    }
    
    enPowerDn = Battery_DetectEnforcePowerDown();
    
    if (NULL == sgtSysReqInf.AppCtrl_Action 
       && 0 == AppCtrl_IsValidPro(POWER_DOWN_AN)
       && (batlev <= POWER_DOWN_LEV || 1 == enPowerDn))
    {   
        if (batterLowCnt > BATTERY_POWER_DWON_CNT || enPowerDn == 1)   
        {//发出关机命令:电量低且不是关机进程的时候
     
            U32 i;
            U8  msgputbuf[40]={0};
        	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
        	
        	if (NULL != sgtSysReqInf.AppCtrl_Action) return;
        	
        	//PhoneTrace(0,"send msg to dc");
        	
        	ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			//gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //创建关机进程
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 1;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
        	
        	deadUiTime = rawtime(NULL); /* 记录发送强制关机的tick */
        	
        }
        else
        {
             batterLowCnt++;
        }
    }
    else
    {
         batterLowCnt = 0;
    } 
    
    
    
    if(1 == AppCtrl_IsValidPro(POWER_DOWN_AN))
    {
    	if(0 == deadUiTime)
    	{
    		deadUiTime = rawtime(NULL); /* 记录发送强制关机的tick */
    	}
    }
    
    if(deadUiTime != 0)
    {
    	if(ABS(rawtime(NULL) - deadUiTime) >= 6000) /* 1分钟强制关机 */
    	{
    		int count ;
    		
			InterruptDisable();  
				
			while(1)
			{
			    hyhwBatteryPowerOnOff(0);
			    for (count=0; count < 5000; count++);
			}
    	}
    }
}

/*---------------------------------------------------------------------------
 函数：AppCtrl_UpdataFileSysTime
 功能：更新系统文件的时间
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
static void AppCtrl_UpdataFileSysTime(void)
{
    U8 TempData[10]={0};
    U8 TempTime[10]={0};
    U32 cur_time;
    static U32 last_time=0;
    
    cur_time = rawtime(NULL);   
    
    if (ABS(cur_time-last_time) > 200)
    {
        hyhwRtc_GetTime(&gtRtc_solar);
        
        sprintf(TempData,"%4d/%02d/%02d",gtRtc_solar.year,gtRtc_solar.month,gtRtc_solar.day);
        sprintf(TempTime,"%02d/%02d/%02d",gtRtc_solar.hour,gtRtc_solar.minute,gtRtc_solar.second);

        Hyc_Set_SysTime(TempData,TempTime);
                
        last_time = cur_time;
    } 
}



//加密验证退出
static void hyAuthen_errorHandle(void)
{
    exit(1);
}

static void hyAuthen_check(void)
{
    char *pRandomBuf;
	pRandomBuf = malloc(128);
	hyAuthen_reg(NULL, hyAuthen_errorHandle);
	hyAuthen_verify(1, pRandomBuf, NULL);
	free(pRandomBuf);
}

#pragma arm section code="appInterface", rodata="appInterface"
/*---------------------------------------------------------------------------
 函数：appControlDesk_process
 功能：处理系统消息、管理应用功能的进程
 参数：无
 返回：总是返回1，操作系统要求
* -----------------------------------------------------------------------------*/
int appControlDesk_process(void)
{
	U32				intLevel;
	tKERNEL_EVENT	tKernelEvent;		/* 外部event，如timer、按键、usb*/
	tPROCESS_CONFIG *ptProcConfig;
	U32				startValue;	
	U32				msgLength;
	U32				sendMsgStatus;	
	U8              Msg[40];
	U32             event;

	
	beeper_PwmDeinit();	//蜂鸣器
		
	gu8USBtest = 0;
#ifdef USB_DEBUG
	//USBHandler_SetScsiFlag(0);//kong 2012-10-22
	//hyudsDcd_connectInit();
#endif
 
    memset( (char *)&tKernelEvent, 0, sizeof(tKERNEL_EVENT));
	
	
	hyhwAdc_start();

	//加密芯片验证
	hyAuthen_check();

	SoftWare_Init();

 	Battery_IntialVariable();
 	
	//注册按键
	drvKeyb_Initialize();
	
	USBHandler_SetScsiFlag(0);
	
	hyudsDcd_connectInit();    
    
    AppCtrl_IntialVariable();
	
	hyhwRtc_GetTime(&gtRtc_solar);	
	
	usb_charge_intial();
	
	bat_show_initial();
    
    ConSume_CoefInitial();
    
	enable_IF();
	
	touchscreen_create_thread();
	hyHWRecog_CreatThread();

//	gpsm_thread(); /* gps开关进程 */
	
	{	
		int re1,re2;
		
		syssleep(50);
		re1 = USBHandler_USBConnect();
		re2 = GetResouceBmpOffset();
		
		/* 不是USB和充电状态才启动开机进程 */
	    if (re1 == 0 && 0 == Battery_DcInCheck() && re2 != -1)
	    {
	    	drvBadScanKey_check(1); // 开始坏扫描键的检测
	    	AppCtrl_CreatPro(POWER_ON_AN,0);
	    	gtActiveApp[POWER_ON_AN].isFocus = 1;
	    }
	    else
	    {
	    	if(1 == re1)usb_inflag_set(1); /* 置USB插入标志 */

	    	AppCtrl_CreatPro(USB_HANDLER_AN, 0); /* USB UI 和 充电UI 公用的 */
	    	gtActiveApp[USB_HANDLER_AN].isFocus = 1;
	    }
	}	
	
#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_init(5);
#endif

	while(1)
	{ 				    
		
		/* 禁止中断，在ewait 之后，才能使能中断，从而允许ewakeup执行 */
		intLevel = setil(_ioff); /* do not want interrupt */

		/* 等待系统消息，中断函数会唤醒此进程 */
		(void) ewait(&pvEventWait); 

		/* 取到系统的事件 */
		memcpy( (char *)&tKernelEvent, (char *)&gtkernelEvent, sizeof(tKERNEL_EVENT));
		memset((char *)&gtkernelEvent,0, sizeof(tKERNEL_EVENT));
		
		/* 恢复中断 */
		(void) setil(intLevel);
		
		
#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_feed();
#endif

		while(tKernelEvent.rd < tKernelEvent.wr)
		{			
			event = tKernelEvent.event[tKernelEvent.rd];
			tKernelEvent.rd ++;
			//对IRQ中的事件进行处理
		    AppCtrl_ProcIRQEvent(event);
		}
				
		//USB 检测并作响应的处理
		USB_VbusDetect();
		
		//更新系统文件时间
		AppCtrl_UpdataFileSysTime();
shell_log(NULL);		
		//电池电量检测，主要是电量低的时候进行关机操作
		AppCtrl_BatteryLevDetect();
		
		usb_updata_charge_fsm();//充电状态机更新
		
		//功耗控制策略
		ConSume_DetectCurCoef();
		
		/* 处理完系统事件，处理应用程序发来的msg, 主要是各种应用的启动、退出、挂起、恢复*/
		while(1)
		{			 			
			sendMsgStatus	= msg_queue_tryget(ptMsg_controlDesk, (char *)Msg, &msgLength);
			if(sendMsgStatus != HY_OK)
			{
				break;  /* 拿不到消息了，退出 */
			}		
			
			//控制台对收到的消息进行响应的处理
			AppCtrl_RespondMsg((tMSG_BODY *)Msg);			
		} 	
		
		Led_keyPad_timeCtrl();
		
//#ifndef USB_DEBUG	
#ifdef HYC_DEV_USE_LOW_PS
		/* 判断是否可以进入最低功耗待机状态 */
		if(gu8USBtest == 0)//kong 2012-10-22
		{
			SysSleepHandler();
		}
#endif
//#endif

		syssleep(1); /* 让出tick给GSM 通讯使用 */
	
	} /* while(1) */

	return 1;
}
#pragma arm section	//code="appInterface", rodata="appInterface"
