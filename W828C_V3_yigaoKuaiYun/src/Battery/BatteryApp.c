/*
 * 电池应用文件
*/
#include "grap_api.h"
#include "BatteryApp.h"
#include "Battery.h"
#include "Common_Dlg.h"



static const U16 gu16BatterLowMaxLev[]=
{
     POWER_LEVEL_0V3, 1000,
     0x255, 1000,
     0x253, 800,
     POWER_LEVEL_OFFV3, 500,
     0xffff, 500,     
};

/*---------------------------------------------------------------------------
 函数：bat_allow_show_dlg
 功能：是否允许显示对话框，如不能则采用其他手段
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
static int  bat_allow_show_dlg(void *pFatherDesk)
{
     int rv = -1;
     int pid;
     
     pid = AppCtrl_GetCurAppPid();
     
     if (pid == POWER_ON_AN || pid == POWER_DOWN_AN || pid == USB_HANDLER_AN)
     {
         return rv;
     }
     
     return 0;
}

/*---------------------------------------------------------------------------
 函数：bat_show_low_tip
 功能：电量低做显示或提示
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
static void bat_show_low_tip(void *pFatherDesk)
{
	int showFlag;
	int timeOut;

	showFlag = bat_allow_show_dlg(pFatherDesk);

	if (showFlag == -1)
	{           
		return;
	}

	//timeOut = wake_set_timeout(6000);

	//没有背光先点亮背光再显示对话框
	if (0 == BackLight_HaveLight())
	{
		BackLight_EventTurnOnLcd(pFatherDesk);           
	}

	//提示电量低
	Com_SpcDlgDeskbox("电池电量低!",0, pFatherDesk, 2,NULL,NULL,DLG_DEFAULT_TIME);
	//Com_DlgDeskbox(pFatherDesk,2,cDIALOG_BATTERY_LOW,NULL,NULL,DLG_DEFAULT_TIME);

	//图片模块未处理
	ReDraw(pFatherDesk,0,0);
}

static volatile U8  batLowFlag = 0;
 
/*---------------------------------------------------------------------------
 函数：bat_show_initial
 功能：初始化电量显示
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void bat_show_initial(void)
{
    batLowFlag = 0;
}

/*---------------------------------------------------------------------------
 函数：bat_show_low_battery_deskbox
 功能：显示低电量对话框
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void  bat_show_low_battery_deskbox(void	*pFatherDesk)
{
	int adcValue;

	if (BIT4 == (batLowFlag&BIT4)) return;
	
	batLowFlag |= BIT4;
	
	//hyUsbPrintf("batLowFlag = %d \r\n",batLowFlag);
	
	if (0 == Battery_DcInCheck()/* && 0 == AppCtrl_IsValidPro(USB_HANDLER_AN)*/)
	{
		adcValue =  Battery_Detect();
		
		//hyUsbPrintf("vat = %d \r\n",adcValue);
		if (adcValue < POWER_TIP1_LEV && (batLowFlag&BIT1) != BIT1)
		{
			//第一次提示电量低
			batLowFlag |= BIT1;

			bat_show_low_tip(pFatherDesk);
		}
	}
	else
	{
		batLowFlag = 0;
	}

	batLowFlag &= (~BIT4);
}
 