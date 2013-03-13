#include "SysSetting.h"
#include "wmi.h"
#include "hy_unifi.h"

/*---------------------------------------------
*函数:Set_FactorySetting
*功能:恢复出厂值
*参数:
*返回:
*-----------------------------------------------*/
void Set_FactorySetting(void *pDesk)
{
	U8 netSelected;
	tWMI_BECON_BUFF *pap;
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("确认恢复出厂设置?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		if(gtHyc.NetSelected != 1)//之前选的wifi
		{
			/*netSelected = gtHyc.NetSelected;
			gtHyc.NetSelected = 0xff;//值为网络切换中
			if(0 == ComWidget_ChangeNetLogo(pDesk, netSelected, 1))
			{
				gtHyc.NetSelected = 1;
			}
			else
			{
				gtHyc.NetSelected = netSelected;
				
				Com_SpcDlgDeskbox("恢复出厂值失败,请重试!",0,pDesk,2,NULL,NULL,100);
				return ;
			}*/
			
			pap = hy_ap_get_connect_infor();
			if(hy_get_ap_link_status() == 1 && (pap->apStatus&0x01))
			{
				ComWidget_ApEvent(pDesk, "正在断开网络,请稍候...", CMD_DISCONNECT, (void *)pap);
			}
		}
		
		netSelected = gtHyc.NetSelected;
		glbVariable_LoadDefaultValue();
		gtHyc.NetSelected = netSelected;
		hyhwLcd_lightSet(gtHyc.BackLight);		
		hyIE_clearDns();
		Net_AddEvt(NET_CLOSEPPP_EVT);
		glbVariable_SaveParameter();
		Com_SpcDlgDeskbox("设置成功!",0,pDesk,2,NULL,NULL,100);
	}
}
