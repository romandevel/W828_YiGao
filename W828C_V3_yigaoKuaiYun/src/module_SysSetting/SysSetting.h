#ifndef _SYS_SETTING_H
#define _SYS_SETTING_H

#include "hyTypes.h"
#include "base.h"
#include "grap_api.h"
#include "grap_view.h"
#include "glbVariable.h"
#include "glbVariable_base.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Net_Evt.h"
#include "bmpComData.h"
#include "AbstractFileSystem.h"
#include "drvKeyArray.h"

typedef int (*SettingFun)(void *pView);

typedef struct _tSetting_info
{
	U8	*pName;		//显示名称
	SettingFun	setfun;		//执行函数
	
}tSETTING_INFO;


extern U32	gu32SettingTemp;
extern S32 ComWidget_LabTimerISR(void *pView);

/*-------------- fun ---------------*/
/*---------------------------------------------
*函数:Help_GetInfo
*功能:帮助
*参数:pStr:存放帮助信息  
      limitSize:pStr的大小
      pHelpName:帮助文件名
*返回:
*-----------------------------------------------*/
void Help_GetInfo(U8 *pStr, int limitSize, U8 *pHelpName);

/*---------------------------------------------
*函数:Set_SysSettingDesk
*功能:设置主界面
*参数:
*返回:
*-----------------------------------------------*/
int Set_SysSettingDesk(void *pDeskFather);

/*---------------------------------------------
*函数:Set_OperationsSelect
*功能:业务选择界面
*参数:
*返回:
*-----------------------------------------------*/
void Set_OperationsSelect(void *pDesk);

/*---------------------------------------------
*函数:Set_FactorySetting
*功能:恢复出厂值
*参数:
*返回:
*-----------------------------------------------*/
void Set_FactorySetting(void *pDesk);

/*---------------------------------------------
*函数:Set_Format
*功能:格式化
*参数:
*返回:
*-----------------------------------------------*/
void Set_Format(void *pDesk);

/*---------------------------------------------
*函数:Set_RecClear
*功能:记录清空
*参数:
*返回:
*-----------------------------------------------*/
void Set_RecClear(void *pDesk);

/*---------------------------------------------
*函数:Set_UpdateSoft
*功能:软件升级
*参数:
*返回:
*-----------------------------------------------*/
void Set_UpdateSoft(void *pDesk);

/*---------------------------------------------
*函数:Set_UpdateTime
*功能:时间同步
*参数:
*返回:
*-----------------------------------------------*/
void Set_UpdateTime(void *pDesk);

/*---------------------------------------------
*函数:Set_AutoSend
*功能:自动发送
*参数:
*返回:
*-----------------------------------------------*/
void Set_AutoSend(void *pDesk);

/*---------------------------------------------
*函数:Set_BackLight
*功能:背光设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_BackLight(void *pDesk);

/*---------------------------------------------
*函数:Set_BT
*功能:蓝牙设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_BT(void *pDesk);

/*---------------------------------------------
*函数:Set_CompanyID
*功能:网点编码
*参数:
*返回:
*-----------------------------------------------*/
void Set_CompanyID(void *pDesk);

/*---------------------------------------------
*函数:Set_DeviceID
*功能:巴枪编号
*参数:
*返回:
*-----------------------------------------------*/
void Set_DeviceID(void *pDesk);

/*---------------------------------------------
*函数:Set_Gps
*功能:GPS设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_Gps(void *pDesk);

/*---------------------------------------------
*函数:Set_Instructions
*功能:使用说明
*参数:
*返回:
*-----------------------------------------------*/
void Set_Instructions(void *pDesk);

/*---------------------------------------------
*函数:Set_NetSelect
*功能:网络选择
*参数:
*返回:
*-----------------------------------------------*/
void Set_NetSelect(void *pDesk);

/*---------------------------------------------
*函数:Set_PrintTime
*功能:打印次数
*参数:
*返回:
*-----------------------------------------------*/
void Set_PrintTime(void *pDesk);

/*---------------------------------------------
*函数:Set_ScreenProtect
*功能:屏保设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_ScreenProtect(void *pDesk);

/*---------------------------------------------
*函数:Set_ScreenVerify
*功能:屏幕校正
*参数:
*返回:
*-----------------------------------------------*/
void Set_ScreenVerify(void *pDesk);

/*---------------------------------------------
*函数:Set_SIM
*功能:SIM卡号设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_SIM(void *pDesk);

/*---------------------------------------------
*函数:Set_Sound
*功能:声音设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_Sound(void *pDesk);

/*---------------------------------------------
*函数:Set_Url
*功能:服务器设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_Url(void *pDesk);

/*---------------------------------------------
*函数:Set_VersionInfo
*功能:版本信息
*参数:
*返回:
*-----------------------------------------------*/
void Set_VersionInfo(void *pDesk);

/*---------------------------------------------
*函数:Set_WIFI
*功能:WIFI设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_WIFI(void *pDesk);

/*---------------------------------------------
*函数:Set_ParamConfig
*功能:系统参数设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_ParamConfig(void *pDeskFather);

/*---------------------------------------------
*函数:Set_WorkDay
*功能:工作日设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_WorkDay(void *pDeskFather);

void Set_GprsAccess(void *pDeskFather);

S32 Set_EditDrawInputCnt(tGRAPEDIT *pEdit);

void Set_ScannerDesk(void *pDeskFather);

#endif //_SYS_SETTING_H