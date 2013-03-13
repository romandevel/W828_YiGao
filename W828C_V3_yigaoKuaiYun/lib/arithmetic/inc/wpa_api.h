#ifndef __WPA_API_H__
#define __WPA_API_H__

#include "wmi.h"


typedef struct wpa_param_st
{
	unsigned char *ApMac;	//the AP's mac address
	unsigned char *ApSsid;	//the AP's ssid, that is, AP name.
	unsigned char *ApPwd;	//AP's password
	short ApChannel;		//AP transfer channel
	eWMI_ENCRYPTION_TYPE cpt_type;	//support: WMI_WPA_PSK_TKIP / WMI_WPA_PSK_AES / WMI_WPA_PSK_TKIP_AES /
	                                //         WMI_WPA2_PSK_TKIP / WMI_WPA2_PSK_AES / WMI_WPA2_PSK_TKIP_AES.
}tWPA_PARAM;


typedef enum
{
	WPA_SUCCESS = 0,
	WPA_INIT_FAIL,
	WPA_CONNECT_FAIL,
	WPA_WAIT_TIMEOUT,
	WPA_MEMORY_OUT,
	WPA_PROCESS_ERR,
	WPA_UNKNOWN_ERR,
}eWpaRet;


/*----------------------------------------------------------------------
* 函数: Wpa_IsRunning
* 功能: 判断Wpa/wpa2是不是正在运行状态
* 参数: none
* 返回: 1: running; 0: no run
* 说明：
*-----------------------------------------------------------------------*/
int Wpa_IsRunning();


/*----------------------------------------------------------------------
* 函数: Wpa_Setup
* 功能: 启动Wpa/Wpa2认证程序，切换到WPA/WPA2加密模式。
* 参数: tWPA_PARAM *pParam:
*			ApMac：要连接的AP的Mac地址
*			ApSsid：要连接的AP的ssid值
*			ApPwd：要连接的AP的password
*			ApChannel：要连接的AP允许连接的通道号
*			cpt_type：哪种加密类型，目前支持6种类型。已是WPA-PSK/WPA2-PSK
*					  的所有类型
* 返回: 
*		WPA_SUCCESS: success; other: fail
* 说明：该函数正确返回后，以后所有再收发的数据就是TKIP(AES,+AES)加密的数据。
*		但这些数据是网卡端直接加密/解密的，应用层无须关心。
*		只要知道该函数正确返回后，即启动了WPA/WPA2加密即可。
*-----------------------------------------------------------------------*/
eWpaRet Wpa_Setup(tWPA_PARAM *pParam);


/*----------------------------------------------------------------------
* 函数: Wpa_end
* 功能: 结束Wpa/wpa2加密/解密，断开连接。
* 参数: none
* 返回: 0：成功；other: 失败
* 说明：该函数返回后，将断开连接。
*-----------------------------------------------------------------------*/
int Wpa_end();



//#define NO_MEMORY_CONTROL	//MACRO: 打开时将启用WPA工程自带的malloc函数。


#endif	//__WPA_API_H__



