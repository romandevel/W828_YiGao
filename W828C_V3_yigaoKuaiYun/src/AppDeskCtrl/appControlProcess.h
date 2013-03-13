#ifndef _APPCONTROL_PROCESS_H_
#define _APPCONTROL_PROCESS_H_

/*----------------------文件描述--------------------------------
* 创建日期: 2008/04/12 
* 描述    : 系统消息定义，包括键盘、鼠标、外部中断
*			应用程序启动，退出，挂起，恢复等
*--------------------------------------------------------------*/

/*include standard files*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "keyCodeEvent.h"
#include "Msgqueue.h"
#include "touchScreenMain.h"

#define SYS_DATA_SHOWLEN           (20)

//高四位为事件类型	
#define	KERNEL_EVENT_TYPE_MASK		0xF0000000		

/* 第一级分类 */
#define	KERNEL_EVENT_KEY_INPUT		0x10000000
#define	KERNEL_EVENT_TIMER			0x20000000
#define	KERNEL_EVENT_PULL			0x40000000
#define	KERNEL_EVENT_APP			0x80000000


/* 第二级分类之外部中断 */
#define	KERNEL_EVENT_EXTINT_MASK	0x0000000F		
#define	KERNEL_EVENT_EXTINT_USB		0x00000000		

/* 应用程序事件
 BIT31   |29                                           0
	+----+-------------------------------------------------------------+
	|type| 在启动时，是应用程序的序列号，其它情况是应用程序pid[15:0]， |         		
	+----+-------------------------------------------------------------+*/
#define KERNEL_EVENT_APP_MASK		0xC0000000
#define KERNEL_EVENT_APP_PID_MASK	0x0000FFFF

#define KERNEL_EVENT_APP_START		0x00000000
#define KERNEL_EVENT_APP_QUIT		0x40000000
#define KERNEL_EVENT_APP_SUSPEND	0x80000000
#define KERNEL_EVENT_APP_RESUME		0xC0000000



//使用到的消息队列，每个进程都有一个独立的消息队列
//各个进程的消息队列参数
#define MSG_PUT_HEAD_SIZE               (8)

#define POWER_ON_MSG_QUE_DEEP           (4)
#define POWER_ON_MSG_MAX_SIZE           (40)

#define POWER_DOWN_MSG_QUE_DEEP         (4)
#define POWER_DOWN_MSG_MAX_SIZE         (40)

#define USB_MSG_QUE_DEEP                (10)
#define USB_MSG_MAX_SIZE                (40)

#define SHELL_MSG_QUE_DEEP              (10)
#define SHELL_MSG_MAX_SIZE              (40)

#define KERNAL_APP_MSG_QUE_DEEP         (0)
#define KERNAL_APP_MSG_MAX_SIZE         (0)

#define SMS_APP_MSG_QUE_DEEP	        (10)
#define SMS_APP_MSG_MAX_SIZE            (50)

#define GSMCDMA_SERVER_MSG_QUE_DEEP	    (0)
#define GSMCDMA_SERVER_MSG_MAX_SIZE     (50)

#define GSMCDMA_CLIENT_MSG_QUE_DEEP	    (0)
#define GSMCDMA_CLIENT_MSG_MAX_SIZE     (50)


#define PPP_MSG_QUE_DEEP	            (0)
#define PPP_MSG_MAX_SIZE                (0)

#define BT_MSG_QUE_DEEP                (0)
#define BT_MSG_MAX_SIZE                (0)

//各个进程的名称
#define USB_HANDLER_NAME		 "USBHandler"
#define POWER_ON_NAME			 "PowerOn"
#define POWER_DOWN_NAME			 "PowerDown"
#define SHELL_NAME				 "Shell"
#define TCPIP_NAME				 "TcpIp"
#define CTRL_APP_NAME		     "Ctrl"
#define KERNAL_APP_NAME      	 "KernalApp"
#define SMS_APP_NAME             "SMSApp"
#define GSMCDMA_SERVER_NAME      "GsmCdma"
#define GSMCDMA_CLIENT_NAME      "GsmCdmaUser"
#define PPP_NAME                 "PPP"
#define UPLOAD_NAME				 "UpLoad"
#define BEEPER_PRO_NAME          "beeper"
#define TOUCH_NAME               "touch"
#define HANDWRITING_NAME         "ISHW_RCOG"
#define VIBRATOR_CTRL_NAME		 "vibrator"
#define GPS_P_NAME		         "Gps_p"
#define GPS_MANAGE_NAME		     "Gps Manage"
#define BT_NAME                  "bt service"
#define WIFI_DRIVER_NAME		 "WiFiDriver"

//进程栈的大小
#define USB_HANDLER_STACK_SIZE		(2048)
#define POWER_ON_STACK_SIZE			(1024)
#define POWER_DOWN_STACK_SIZE		(1024)
#define SHELL_STACK_SIZE			(6144)
#define TCPIP_SIZE					(2048)
#define CTRL_APP_SIZE			    (2048)
#define KERNAL_APP_SIZE      		(6144)
#define SMS_APP_SIZE      		    (2048)
#define GSMCDMA_SERVER_SIZE         (5120)
#define GSMCDMA_CLIENT_SIZE         (4096)
#define PPP_SIZE                    (4096)
#define UPLOAD_SIZE					(4096)
#define BEEPER_SIZE                 (256)
#define TOUCH_SIZE					(512)
#define HANDWRITING_SIZE            (512)
#define VIBRATOR_SIZE               (128)
#define GPS_P_SIZE                  (4096)
#define GPS_MANAGE_SIZE             (1024)
#define BT_SIZE                     (1024)
#define WIFI_DRIVER_SIZE			(2048)

//初步定义进程优先级 应用 7 网络 8-9 USB 7 AppCtrl 15
//进程的优先级

#define USB_HANDLER_PRIOPITY		PRIORITY_5_OSPP
#define POWER_ON_PRIOPITY			PRIORITY_6_OSPP
#define POWER_DOWN_PRIOPITY			PRIORITY_6_OSPP

#define CTRL_APP_PRIOPITY		    PRIORITY_10_OSPP /* 5-->10 */
#define KERNAL_APP_PRIOPITY	        PRIORITY_6_OSPP
#define SHELL_PRIOPITY				PRIORITY_10_OSPP /* 7-->10 */
#define TCPIP_PRIOPITY				PRIORITY_9_OSPP
#define GSMCDMA_SERVER_PRIOPITY	    PRIORITY_10_OSPP
#define GSMCDMA_CLIENT_PRIOPITY	    PRIORITY_8_OSPP
#define PPP_PRIOPITY	            PRIORITY_11_OSPP
#define UPLOAD_PRIOPITY				PRIORITY_7_OSPP  /* 6-->7 */
#define GSMRESET_PRIOPITY			PRIORITY_6_OSPP
#define WIFICONNECT_PRIOPITY		PRIORITY_6_OSPP
#define BEEPER_PRIOPITY				PRIORITY_12_OSPP /* 8-->12*/
#define TOUCH_PRIOPITY              PRIORITY_10_OSPP /* 6-->10*/
#define HANDWRITING_PRIOPITY        PRIORITY_4_OSPP
#define VIBRATOR_PRIOPITY           PRIORITY_11_OSPP
#define GPS_P_PRIOPITY              PRIORITY_4_OSPP
#define GPS_MANAGE_PRIOPITY         PRIORITY_5_OSPP
#define BT_PRIOPITY                 PRIORITY_11_OSPP
#define WIFI_DRIVER_PRIOPITY		PRIORITY_11_OSPP

#define BARCODE_PRIOPITY			PRIORITY_12_OSPP

/*无效的进程pid号*/
#define APPCTRL_INVALID_PID    (0xFFFF)

/* 系统允许的最多同时活动的应用程序数 */
#define	MAX_ACTIVE_APP				TOTAL_APP_AN

typedef	enum 
{
	USB_HANDLER_AN	= 0,		
	POWER_ON_AN,		
	POWER_DOWN_AN,		
	SHELL_AN,		
	TCPIP_AN,	
	KERNAL_APP_AN,   //内核程序进程，可以使用函数指针进行入口函数赋值，一般用于多任务进程显示中	      
    GSMCDMA_SERVER_AN,
    GSMCDMA_CLIENT_AN,
    PPP_AN,     
    WIFI_DRIVER_AN,
    BT_DRIVER_AN,
    UPLOAD_APP_AN,//后台上传
    GPS_DRIVER_AN,
    
	TOTAL_APP_AN      /* 最后一个 */	   
}eAPPLICATION_NO;


//注意: 在发送消息时，应该严格按照要求发送消息，不然系统可能会不响应消息
typedef enum {
	SYS_FOCUS_EVENT = 1,      //进程得到焦点消息，进程开始运行  控制台->应用
	SYS_REQ_QUIT_EVENT,		  //发送进程退出，这个命令由控制台来发送给运行的进程，让运行的进程强行退出后，由控制台收回资源	
    USE_ACK_QUIT_EVENT,       //进程确认退出命令 应用->控制台，一般情况下当应用进程收到 SYS_QUIT_EVENT 命令后，应用进程关闭相应
                              //资源后，由控制台回收资源  
	USE_REQ_START_TASK_EVENT, //启动进程， 应用进程->控制台进程，由控制台进程来启动另外应用进程，自己进程资源让控制台回收
	USE_LOST_FOCUS_EVENT,     //丢失自己的焦点,应用进程->控制台，自己阻塞，控制台不收回资源，必定会启动另外的应用进程
	SYS_DEPRIVE_FOCUS_EVENT,  //剥夺焦点  控制台->应用，应用收到命令立即阻塞自己 [例:电话来了，要求焦点暂停，电话结束后再恢复焦点]
	USE_DEPRIVED_FOCUS_EVENT, //焦点已经被剥夺 应用->控制台，应用阻塞[好让控制台作安排]
	USE_QUIT_EVENT,           //进程自杀  应用->控制台，控制台收到命令后回收资源
	
	SYS_PHONE_EVENT,          //控制台-〉应用[电话进程]，其他进程请勿使用该消息

    USE_KILL_OTHER_EVENT,     //应用->控制，应用层要求杀死其他应用进程
    
	USE_BATTERY_LOW_EVENT,    //应用->控制台，当控制台收到该消息后，控制台杀死所有进程，并关机
	
	USE_PHONE_NUMB_EVENT,     //电话号码消息
	
	USE_USB_IS_ACT,           //控制台->应用 是否进入USB状态
	USE_USB_ENTER_ACT,        //应用->控制台 用户确认进入USB状态
	USE_USB_IGNORE,           //应用->控制台 忽略USB不进入USB状态
	
	USE_REQ_POWERDOWN,        //用户进程要求强制关机，控制进程则进行强制关机

	SYS_REQ_USE_KILL_SELF,    //控制台要求进程自己主动退出
	USE_ACK_KILL_SELF,        //应用进程自行退出

	SYS_REQ_TIPSHOW ,         //显示系统的提示信息
	
	KEY_INPUT_EVENT,  //机械按键消息，这个消息只有foucus进程能够接受
	TOUCH_KEY_EVENT,  //触摸按键消息，这个消息只有foucus进程能够接受 	
	SYS_TIMER_EVENT,  //系统timer消息，这个消息只有foucus进程能够接受，暂时不考虑其它服务进程
	KEY_PULL_EVENT,   //触摸拖拽消息
	TOUCH_KEY_PULL_EVENT,   //触摸拖拽或点触消息(此时触摸屏全屏为绘图模式)
	
	KEY_LOCK_SCR,    //机械按键锁屏消息
	KEY_UNLOCK_SCR,  //机械按键解锁消息 20100427
	
	KEY_WAKEUP_SYS,	 //通过按键唤醒系统
	
	POP_UNLOCKDIALOG_EVENT,	//锁屏状态弹出提示解锁对话框 20100427
	
	MSG_MAX_EVENT = 0x7FFFFFFF,
	
	INVALID_EVENT = -1
}eMSG_ID;


#define KERNEL_EVENT_NUM 20
typedef	struct kernelEvent_st
{
	U32 event[KERNEL_EVENT_NUM];
	U16 rd;
	U16 wr;
}tKERNEL_EVENT;


/* 一个应用进程的结构，进程 和接收消息的队列*/
typedef struct application_st
{
	//eAPPLICATION_NO	app_no;
	U16				pid;
	U16				isFocus;
	tMSG_QUEUE		*ptmsg;
}tAPPLICATION;


typedef struct _tPIDINFOR
{
	U16 MyPid;         /*发送这个消息时，自己的pid号*/
	U16 StartPid;	   /*需要启动的PID号*/
	U32 Flag;          /*发送进程号的时候有时需要附加消息*/
}tPIDINFOR;

typedef struct _tKEY_EVENT
{
	U32 keycode;
	U8 keyevent;
}tKEY_EVENT;

typedef struct _tTOUCH_EVENT
{
	U16 row;
	U16 col;
}tTOUCH_EVENT;


typedef struct _tPHONE_EVENT
{
	U8 PhoneEvent;
	U8 Type;      /*0:GSM  1 : wifi */
	U8 Buf[80];  /*消息字符串*/
}tPHONE_EVENT;

//拖拽点
typedef struct _tPOINT_PULL
{
    U16 x;
    U16 y;
}tPOINT_PULL;

typedef struct _tYD_NET
{
    U16 lpa;
    U16 wpa;
}tYDNETmsg;


typedef union _UMSG
{
	tPOINT_KEY_PULL   PointKeyPull;    /*拖拽或点触点, 目前用于网页浏览中20080811*/
	tPOINT_PULL   Point;    /*拖拽点*/
	tKEY_EVENT    Key;	    /*机械按键消息*/
	tTOUCH_EVENT  Touchkey; /*触摸屏的坐标*/
	U32 Timer;              /*系统tick,即一般为timer消息*/
	U32 Flag;	            /*某些Event消息可能需要这个标志来识别消息*/
	tPIDINFOR Pid;          /*需要启动的pid号*/
	U32 MsgQueAdd;          /*消息队列的指针位置*/
	
	tPHONE_EVENT PhoneMsg;  /*电话专用消息结构，请勿随便使用*/
	tYDNETmsg    net;        /* 网络通讯信息 */
	
	char         buf[200];
}uMSG;

/*
*一般情况下使用这个默认的结构体，足以发送和接受消息了,当
*需要其它消息内容的时候可以增加成员msg变量，建议不要加太大
*的union 成员
*/
typedef struct _tMSG_BODY
{
	eMSG_ID	id;       /*系统事件ID，使用时请参见 msg_Msgqueue.h文件*/
	uMSG	msg;      /*消息体内容*/
}tMSG_BODY;

typedef struct _tMSG_PUT_PARAM
{
	U32			length;
	U32			priority;
	tMSG_BODY	body;
}tMSG_PUT_PARAM;	
 
 
/* */
extern tKERNEL_EVENT gtkernelEvent;
extern tMSG_QUEUE *ptMsg_controlDesk;
extern void *pvEventWait;

/*---------------------------------------------------------------------------
 函数：AppCtrl_CreatPro
 功能：创建进程和消息队列并填充信息表
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro(U16 id,void *pText);

/*---------------------------------------------------------------------------
 函数：AppCtrl_DeletePro
 功能：杀死进程和消息队列并填充信息表
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/
void AppCtrl_DeletePro(U16 id);

/*---------------------------------------------------------------------------
 函数：AppCtrl_GetAppInfor
 功能：返回进程信息，见结构 tAPPLICATION
 参数：appindex ; 应用进程的id号
 返回：进程信息指针
* -----------------------------------------------------------------------------*/
tAPPLICATION *AppCtrl_GetAppInfor(eAPPLICATION_NO appindex);

/*---------------------------------------------------------------------------
 函数：AppCtrl_GetCurAppPid
 功能：得到当前运行进程的虚pid号
 参数：none
 返回：进程虚pid号
* -----------------------------------------------------------------------------*/
U32 AppCtrl_GetCurAppPid(void);





#endif