#include "SysSetting.h"


/*---------------------------------------------
*函数:Set_UpdateTime
*功能:时间同步
*参数:
*返回:
*-----------------------------------------------*/
void Set_UpdateTime(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("确定同步时间?",0, pDesk, 3, NULL, NULL, 3000))
	{
		Net_AddEvt(NET_UPDATE_TIME_EVT);//设置为需要同步时间
	}
}
