#include "SysSetting.h"

int Set_RecClearing(void *p)
{
	while(Net_GetEvt()&NET_CLEARREC_EVT)
	{//等待后台清空记录
	
		Net_WakeUp();
		syssleep(50);
	}
	
	return 1;
}


/*---------------------------------------------
*函数:Set_RecClear
*功能:记录清空
*参数:
*返回:
*-----------------------------------------------*/
void Set_RecClear(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否清空所有记录?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		Net_AddEvt(NET_CLEARREC_EVT);
		cartoon_app_entry(Set_RecClearing,NULL);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		Com_CtnDlgDeskbox(pDesk,2,"   请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		Com_SpcDlgDeskbox("记录已清空!",0,pDesk,1,NULL,NULL,100);
	}
}
