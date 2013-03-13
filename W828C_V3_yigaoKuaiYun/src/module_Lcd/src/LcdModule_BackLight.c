/*----------------------文件描述--------------------------------
* 创建日期: 2008/05/28 
* 描述    : 该文件为关于屏保的一些操作
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyTypes.h"
#include "LcdModule_BackLight.h"
#include "glbVariable_base.h"
#include "appControlProcess.h"
#include "grap_api.h"
#include "edit.h"




/*
*define variable
*/
//屏保计时
S32  gu32ScreenPro = -1;

//lcd PWM背光控制级数
const U8 gu8Lcd_BackLight[5]=
{
    LCD_PWM_GRADE_1,
    LCD_PWM_GRADE_2,
    LCD_PWM_GRADE_3,
    LCD_PWM_GRADE_4,
    LCD_PWM_GRADE_5
};

/*---------------------------------------------------------------------------
 函数：BackLight_Get_LevValue
 功能：
 参数：无
 返回：
* -----------------------------------------------------------------------------*/
int BackLight_Get_LevValue(U32 lev)
{
    if (lev > 4)
    {
        lev = 3;
    }
    
    return  gu8Lcd_BackLight[lev];
}

/*---------------------------------------------------------------------------
 函数：BackLight_UpdataLightTime
 功能：更新背光时间
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void BackLight_UpdataLightTime(U32 t)
{
     gu32ScreenPro = t;
}

/*---------------------------------------------------------------------------
 函数：BackLight_CalculateTime
 功能：对背光计时,计时为0时则关闭背光
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void  BackLight_CalculateTime(void)
{
    if (gu32ScreenPro != 0)
    {
        if (--gu32ScreenPro == 0)
		{
			LcdModulePowerOnOff(0);
			hyhwLcd_resetOn();
			gu8EnableRefreshSrc = 0; //不允许刷屏			
		}    
    }
}

/*---------------------------------------------------------------------------
 函数：BackLight_Light
 功能：是否处于屏保状态
 参数：无
 返回：1:有背光 0:无背光
* -----------------------------------------------------------------------------*/ 
U32 BackLight_HaveLight(void)
{
    return gu8EnableRefreshSrc;        
}

/*---------------------------------------------------------------------------
 函数：BackLight_ShutLight
 功能：关闭背光，置掉某些变量
 参数：无
 返回：无
* -----------------------------------------------------------------------------*/ 
S32 BackLight_ShutLight(void)
{
	int rv = HY_OK;

	//已经关闭则无效，继续处于关闭状态
	if (0 == gu8EnableRefreshSrc) return rv;

	gu32ScreenPro = 0;

	LcdModulePowerOnOff(0);
	hyhwLcd_resetOn();
	gu8EnableRefreshSrc = 0; //不允许刷屏

	return rv; 
}


/*---------------------------------------------------------------------------
 函数：BackLight_ResumeChg_BKValue
 功能：恢复背光值
 参数：无
 返回：
* -----------------------------------------------------------------------------*/
void BackLight_ResumeChg_BKValue(U8 value)
{
	gtHyc.BackLight = value;
}


/*---------------------------------------------------------------------------
 函数：BackLight_EventTurnOnLcd
 功能：由于处于屏保状态下，来了事件需要点亮背光 如:来电话了，按键按下等
 参数：无
 返回：1：点亮 0：维持
* -----------------------------------------------------------------------------*/ 
U32  BackLight_EventTurnOnLcd(void *pView)
{
	U32 rc = 0;
	tGRAPDESKBOX *pDesk; 
	
	if (0 == gu8EnableRefreshSrc)//080922
	{
		hyhwLcd_resetOff();
		drvLcd_InitLcdModuleInstructs();

		pDesk = ((tGRAPVIEW *)pView)->pParent;

		ConSume_ChangeCoefTemp(360,1);

		if (1 == edit_get_input_status(pDesk))
		{
			drvLcd_IsUseQueue(0);
			alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata); 
		}
		else
		{
			if(gScreen_Display_Mode == 1)
			{//如果屏保前是横屏,这里再转一次
				gScreen_Display_Mode = 0;
				drvLcd_220176DisplayMode();
			}

			//090331
			ReDraw(pDesk,0,0);
			drvLcd_IsUseQueue(1); 
			drvLcd_ClearQueue();
			drvLcd_BMPFromDisplayMemory2Screen();
		}

		syssleep(5);
		LcdModulePowerOnOff(1);

		gu8EnableRefreshSrc = 1;
		rc = 1;
		SysSleep_SetWakeMode(0);
	}

	gu32ScreenPro = gtHyc.ScreenProtect;

	return rc;
}


/*---------------------------------------------------------------------------
 函数：BackLight_KeyLight
 功能：有按键时调整屏保时间
 参数：无
 返回：无
* -----------------------------------------------------------------------------*/ 
void BackLight_KeyLight(void)
{
    if (1 == gu8EnableRefreshSrc)    
    {  
		if(gtHyc.ScreenProtect != 0)
		gu32ScreenPro = gtHyc.ScreenProtect;		
    }
}

/***************************************
* 函数: BackLight_CloseScreenProtect
* 功能: 关闭屏保并返回之前屏保值
* 参数: none
* 返回: 之前屏保值
****************************************/
U16 BackLight_CloseScreenProtect()
{
	U16 temp;
	
	temp = gtHyc.ScreenProtect;
	
	gtHyc.ScreenProtect = cSCREENPROTECT_CLOSE;
	
	return temp;
}

/***************************************
* 函数: BackLight_CloseScreenProtect
* 功能: 关闭屏保并返回之前屏保值
* 参数: 屏保值
* 返回: none
****************************************/
void BackLight_SetScreenProtect(U16 value)
{
	gtHyc.ScreenProtect = value;
	gu32ScreenPro = value;	
	return ;
}
/*---------------------------------------------------------------------------
 函数：BackLight_AvoidThis
 功能：在当前模式下屏蔽此项功能
 参数：无
 返回：1:屏蔽 0:不屏蔽
* -----------------------------------------------------------------------------*/ 
U32 BackLight_AvoidThis(void)
{
    U32 rc = 0;
    U32 len,i;
    tAPPLICATION *pApp;	 //eAPPLICATION_NO
    U32 app_pid[]={/*USB_HANDLER_AN,*/ POWER_ON_AN, POWER_DOWN_AN};
    
    len = sizeof(app_pid)/4;
        
    for (i=0; i<len; i++)
    {
        pApp = AppCtrl_GetAppInfor(app_pid[i]);
        
        if (0 != pApp->isFocus)
        {
            rc = 1;
            break;
        }    
    }
    
    return rc;
}
