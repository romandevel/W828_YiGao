#include "SysSetting.h"


extern int Comm_UpgradeSoft(void *p);
extern int Comm_GetSoftInfo();


/*---------------------------------------------
*函数:Set_UpdateSoft
*功能:软件升级
*参数:
*返回:
*-----------------------------------------------*/
void Set_UpdateSoft(void *pDesk)
{
	U64 	totalspace,freespace = 0;
	int		ret,upgrade,lastValue;
	eDLG_STAT		kDlgState;
	
	AbstractFileSystem_VolumeSpace( "C", &totalspace, &freespace);
	freespace >>= 20;
	if(freespace < 2)//M
	{
		Com_SpcDlgDeskbox("磁盘空间不足,需2M.",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		return;
	}
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否确认升级?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
	{
		if(Net_ChangeEvt(pDesk,NET_UPDATE_SOFT_EVT) < 0) return ;
		
		lastValue = BackLight_CloseScreenProtect();//关闭屏保
		
		ConSume_UpdataSpeedDirect(SHELL_AN, 192);
		
		cartoon_app_entry(Comm_GetSoftInfo,NULL);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在获取信息,请稍候!",NULL,NULL,ComWidget_LabTimerISR,50,12000);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		if(DLG_CONFIRM_EN == kDlgState)
		{
			ret = kernal_app_get_stat();
			if(6 == ret)//网络连接失败
			{
				Com_SpcDlgDeskbox("网络故障,请稍等1分钟再试!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			else if(7 == ret || 8 == ret)//下载失败
			{
				Com_SpcDlgDeskbox("网络故障,请稍等1分钟再试!",6, pDesk, 2,NULL, NULL,  0x8FFFFFFF);
			}
			else if(9 == ret)//已是最新版本
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("已是最新版本,是否强制升级?",0, pDesk, 3, NULL, NULL, 0x8FFFFFFF))
				{
					ret = 5;
				}
			}
			else if(10 == ret)
			{
				Com_SpcDlgDeskbox("没有程序文件!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			
			//
			if(5 == ret)
			{
				cartoon_app_entry(Comm_UpgradeSoft,NULL);
				AppCtrl_CreatPro(KERNAL_APP_AN, 0);
				kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在下载,请稍等!",NULL,NULL,Com_PercentLabTimerISR,4, 0x8FFFFFFF);
				AppCtrl_DeletePro(KERNAL_APP_AN);
				
				Comm_SetUpgradeState(0);
				
				if (kDlgState == DLG_CONFIRM_EN)
				{
					ret = kernal_app_get_stat();
				
					if(5 == ret)
					{
						if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("下载完成,请关机后再次开机升级.",0, pDesk, 2, NULL, NULL, 0x8FFFFFFF))				
						{
							AppCtrl_PowerOffProc();
						}
					}
					else if(7 == ret)
					{
						Com_SpcDlgDeskbox("文件错误,请重试!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
					else if(6 == ret)
					{
						Com_SpcDlgDeskbox("网络故障,请稍等1分钟再试!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
					else
					{
						Com_SpcDlgDeskbox("网络故障,请稍等1分钟再试!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
				}
				else
				{
					Com_SpcDlgDeskbox("网络故障,请稍等1分钟再试!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
				}
			}
		}
		else
		{
			Com_SpcDlgDeskbox("网络连接失败!",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		}
		
		Net_ClearEvt(NET_FOREGROUND_EVT);
		BackLight_SetScreenProtect(lastValue);//打开屏保
		
		ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	}
}
