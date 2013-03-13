/*----------------------文件描述--------------------------------
* 创建日期: 2008/04/29 
* 描述    : 定义进程属性
*--------------------------------------------------------------*/


#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "std.h"

#include "msgQueue.h"
#include "appControlProcess.h"
#include "keyCodeEvent.h"
#include "glbStatusDef.h"
#include "ospriority.h"
#include "AppCtrlConstData.h"



const tPROCESS_CONFIG gtproc_Config[TOTAL_APP_AN] =
{
	/* 进程入口地址、名字、栈长度、优先级、属性、消息队列深度，最大允许的消息长度 */
	//USB
	{
		(int (*)())USBHandler_ThreadMethod,	USB_HANDLER_NAME, 	USB_HANDLER_STACK_SIZE,	USB_HANDLER_PRIOPITY,	
		(APP_ATTR_SYSTEM_AA), USB_MSG_QUE_DEEP, USB_MSG_MAX_SIZE
	},
	
	//PowserOn
	{
		(int (*)())PowerOn_ThreadMethod,	POWER_ON_NAME,		POWER_ON_STACK_SIZE,  POWER_ON_PRIOPITY,	
		(APP_ATTR_EXCLUSIVE_AA+APP_ATTR_ONLY_FOREGROUND_AA), POWER_ON_MSG_QUE_DEEP, POWER_ON_MSG_MAX_SIZE
	},
	
	//PowserDown	
	{
		(int (*)())PowerDown_ThreadMethod,	POWER_DOWN_NAME,    SHELL_STACK_SIZE,  SHELL_PRIOPITY,	
		(APP_ATTR_EXCLUSIVE_AA+APP_ATTR_ONLY_FOREGROUND_AA), POWER_DOWN_MSG_QUE_DEEP, POWER_DOWN_MSG_MAX_SIZE
	},
	//Shell	
	{
		(int (*)())Shell_ThreadMethod,		SHELL_NAME,		SHELL_STACK_SIZE, SHELL_PRIOPITY,	
		(APP_ATTR_NON_EXCLUSIVE_AA +APP_ATTR_ALLOW_BACKGROUND_AA),SHELL_MSG_QUE_DEEP, SHELL_MSG_MAX_SIZE
	},	
	//Tcpip
	{
		(int (*)())TcpIp_ThreadMethod,		TCPIP_NAME,		TCPIP_SIZE,  TCPIP_PRIOPITY,
		APP_ATTR_SYSTEM_AA, 0, 0
	},
	
	//kernal app
	{
		(int (*)())Kernal_App_ThreadMethod,	KERNAL_APP_NAME,	KERNAL_APP_SIZE, KERNAL_APP_PRIOPITY,
		(APP_ATTR_EXCLUSIVE_AA+APP_ATTR_ONLY_FOREGROUND_AA), KERNAL_APP_MSG_QUE_DEEP, KERNAL_APP_MSG_MAX_SIZE
	},
	
	//GsmCdmaServer
	{
		(int (*)())GsmCdmaServer_ThreadMethod,		GSMCDMA_SERVER_NAME, GSMCDMA_SERVER_SIZE, GSMCDMA_SERVER_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, GSMCDMA_SERVER_MSG_QUE_DEEP, GSMCDMA_SERVER_MSG_MAX_SIZE
	},
	
    //GsmCdmaClient
	{
		(int (*)())GsmCdmaClient_ThreadMethod,		GSMCDMA_CLIENT_NAME, GSMCDMA_CLIENT_SIZE, GSMCDMA_CLIENT_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, GSMCDMA_CLIENT_MSG_QUE_DEEP, GSMCDMA_CLIENT_MSG_MAX_SIZE
	},

/////以下进程运行没有真正被控制台管理，这里只是填入信息，供频率调整用  gao //////
	//ppp
	{
		(int (*)())ppp_ThreadMethod,		PPP_NAME, PPP_SIZE, PPP_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, PPP_MSG_QUE_DEEP, PPP_MSG_MAX_SIZE
	},	

	//wifi drv
	{
		(int (*)())WiFiDriver_ThreadMethod,		WIFI_DRIVER_NAME, WIFI_DRIVER_SIZE, WIFI_DRIVER_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, 0, 0
	},
	
	//BT drv
	{
		(int (*)())hycoBt_main,		BT_NAME, BT_SIZE, BT_PRIOPITY, 
		APP_ATTR_SYSTEM_AA, 0, 0
	},
	
	{//上传
		(int (*)())Net_ThreadMain,	UPLOAD_NAME,UPLOAD_SIZE, UPLOAD_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, 0, 0
	},

	{//GPS
		(int (*)())Gps_Process,	GPS_P_NAME,GPS_P_SIZE, GPS_P_PRIOPITY,	
		APP_ATTR_SYSTEM_AA, 0, 0
	},
};