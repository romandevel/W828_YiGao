#ifndef _HY_UNIFI_H_
#define _HY_UNIFI_H_

#include "hyTypes.h"

/*  */
typedef enum 
{
	CMD_STA_IDLE=0,			//状态：无命令需要处理
	CMD_STA_INITED,			//状态：初始化完成
	
	CMD_SCAN=2,				//命令：SCAN
	CMD_STA_SCANNING,		//状态：正在处理SCAN命令
	CMD_STA_SCANNED,		//状态：SCAN命令处理完成
	CMD_STA_SCAN_ERR,		//状态：SCAN命令处理错误
	
	CMD_SET_WEP_KEY=6,		//命令：SET WEP KEY
	CMD_STA_SET_WEP_KEY_OK,	//状态：SET WEP KEY
	CMD_STA_SET_WEP_KEY_ERR,	//状态：SET WEP KEY
	
	CMD_DEL_WEP_KEY=9,		//命令：SET WEP KEY
	CMD_STA_DEL_WEP_KEY_OK,	//状态：SET WEP KEY
	CMD_STA_DEL_WEP_KEY_ERR,	//状态：SET WEP KEY
	
	CMD_SET_WPA_KEY=12,		//命令：SET WPA KEY
	CMD_STA_SET_WPA_KEY_OK,	//状态：SET WPA KEY
	CMD_STA_SET_WPA_KEY_ERR,	//状态：SET WPA KEY
	
	CMD_DEL_WPA_KEY=15,		//命令：SET WPA KEY
	CMD_STA_DEL_WPA_KEY_OK,	//状态：SET WPA KEY
	CMD_STA_DEL_WPA_KEY_ERR,	//状态：SET WPA KEY
	
	CMD_CONNECT=18,			//命令：CONNECT
//	CMD_STA_JOINING,		//状态：正在处理JOIN命令
	CMD_STA_JOINED,			//状态：JOIN命令处理成功
	CMD_AUTHENTICATE,		//AUTHENTICATE命令
//	CMD_STA_AUTHENTICATING,	//状态：正在处理AUTHENTICATE命令
	CMD_STA_AUTHENTICATED,	//状态：AUTHENTICATE命令处理成功
	CMD_ASSOCIATE,			//ASSOCIATE命令
//	CMD_STA_ASSOCIATING,	//状态：正在处理ASSOCIATE命令
	CMD_STA_CONNECTED,		//状态：ASSOCIATE命令处理成功，已连接
	CMD_STA_CONNECT_ERR,	//状态：连接命令错误
	
	CMD_DISCONNECT=25,			//命令：DISCONNECT
//	CMD_STA_DISCONNECTING,	//状态：断开连接执行中
	CMD_STA_DISCONNECTED,	//状态：已断开连接
	CMD_STA_DISCONNECT_ERR,	//状态：断开命令错误
	
	CMD_SET_POWER_MODE=28,		//命令：SET POWER MODE
	CMD_STA_SET_POWER_MODE_OK,	//状态：SET POWER MODE OK
	CMD_STA_SET_POWER_MODE_ERR,	//状态：SET POWER MODE ERR
}eWifi_cmd_state;

typedef enum 
{
	STATE_DISCONNECTED=0,		//断开状态
	STATE_CONNECTED,			//连接状态但未关联
	STATE_ASSOCIATED			//关联状态，此状态下才能使用网络(肯定处于连接状态)
} eWifi_connectState;


extern eWifi_cmd_state hy_wifiCmdAndState;
extern eWifi_connectState hy_wifiConnectState;

#endif //#ifndef _HY_UNIFI_H_
