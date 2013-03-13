#include "grap_api.h"
#include "glbVariable.h"
#include "Net_Evt.h"
#include "Rec_Info.h"


//#define RETURN_TO_SWITCH_TICKS		18000	//3分钟


//static U32 gsu32LastKeyorPull=0;

/*
 * const data
*/
const tGrapViewAttr gtComLabTime[]=
{
  //特殊的ID号，避免重复
  {0xFFFD, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
};

const tGrapLabelPriv gtComLabTimePri[]=
{
	{NULL,0,0,0,0,0,0,0},
};

//最后一次按键或点击事件
/*void common_updatelast_keyorpull()
{
	gsu32LastKeyorPull = rawtime(NULL);
}

void common_check_lastevent(tGRAPDESKBOX *pDesk)
{
	hyUsbPrintf("rawtime = %d    last  = %d    %d    %d \r\n",
			rawtime(NULL),gsu32LastKeyorPull, ABS(rawtime(NULL)-gsu32LastKeyorPull), RETURN_TO_SWITCH_TICKS);
	if(ABS(rawtime(NULL)-gsu32LastKeyorPull) > RETURN_TO_SWITCH_TICKS)
	{
		if(pDesk->view_attr.id != MAIN_DESKBOX_ID)	
		{
			Grap_Return2Switch(pDesk, RETURN_TOSWITCHMODE);
		}
		else//清空主界面输入的账号信息
		{
			Gqy_ClearInputData();
			ReDraw(pDesk, 0, 0);
		}
	}
}*/

/*---------------------------------------------------------
*函数: common_glob_lab_draw
*功能: 空函数不进行刷新   
*参数: none
*返回: none
*---------------------------------------------------------*/
static S32 common_glob_lab_draw(void *pView, U32 xy, U32 wh)
{
	 return SUCCESS;
}


/*---------------------------------------------------------
*函数: common_glob_lab_timer_isr
*功能: 定时器函数，各个应用可以放入改函数下面   
*参数: none
*返回: none
*---------------------------------------------------------*/
static void common_glob_lab_timer_isr(void *pView)
{
    tGRAPDESKBOX	*pDesk;
    static	U32		lastTime=0;
	U32				now;
	int				ret;
    
    pDesk = ((tGRAPLABEL*)pView)->pParent;
    
	bat_show_low_battery_deskbox(pDesk);
	
//	common_check_lastevent(pDesk);
	
	//事件提醒
	Evt_Event_Remind(pDesk);
	
	//不是真正断开,只是判断时间
	Evt_CloseSocket_Check();
	
	//检测wifi是否因为信号或其他原因 非人为断开
	if(gtHyc.NetSelected == 0)
	{
		WifiConnect_CheckState();
	}
	
	now = rawtime(NULL);
	if(ABS(now - lastTime) > 500)
	{
		//检查程序是否需要升级
		if(Login_GetLoginFlag() == 1)
		{
			//程序是否需要升级
			ret = Comm_GetUpFlag();
			if(ret == 1)//有程序需要升级,但不强制
			{
				Comm_SetUpFlag(0x81);
				Com_SpcDlgDeskbox("程序有新版本,请到[系统设置]进行升级!",6,pDesk,2,NULL,NULL,500);
				ReDraw(pDesk,0,0);
				Comm_SetUpFlag(0);
			}
			else if(ret == 2)//强制程序升级
			{
				Comm_SetUpFlag(0x82);
				Com_SpcDlgDeskbox("程序有重要更新,请立即点击[确认]升级.",0, pDesk, 2, NULL, NULL, 0xFFFFFFFF);
				Common_NetUpgrade(3, pDesk);
				Comm_SetUpFlag(0);
			}
		}
		
		//检查是否有记录需要发送或加载   时间是否同步
		Evt_RecLoadSend_Check();
		
		//检查公司信息是否需要更新
		Evt_UpdateCompany_Check();
		
		//检查gsm模块是否需要重启
		Evt_GsmReset_Check();
		
		lastTime = now;
		
	}	
}

/*---------------------------------------------------------
*函数: common_insert_glob_lab
*功能: 在窗体中插入该控件   
*参数: none
*返回: none
*---------------------------------------------------------*/
static S32 common_recv_sys_msg(void *pView)
{
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    
    //检测USB
    ComWidget_RevUSBMsg(pView);
    
    return SUCCESS;
} 


/*---------------------------------------------------------
*函数: common_insert_glob_lab
*功能: 在窗体中插入该控件   
*参数: none
*返回: none
*---------------------------------------------------------*/
void common_insert_glob_lab(tGRAPDESKBOX *pDesk) 	
{	
	tGRAPLABEL *pLable;
	int powerOn,powerDown;
	
	powerOn = AppCtrl_IsValidPro(POWER_ON_AN);
	powerDown = AppCtrl_IsValidPro(POWER_DOWN_AN);
	
	if (1 == powerOn || 1 == powerDown)
	{
	    return;
	}	
	
	pLable = Grap_InsertLabel(pDesk, &gtComLabTime[0], &gtComLabTimePri[0]);
	if(NULL == pLable) return;
	pLable->draw = common_glob_lab_draw;
	pLable->label_timer.enable = 1;
	pLable->label_timer.TimerLft = 10;
	pLable->label_timer.TimerTot = 10;
	pLable->labelTimerISR =  common_glob_lab_timer_isr;
	pLable->recvSysMsg = common_recv_sys_msg;
}	