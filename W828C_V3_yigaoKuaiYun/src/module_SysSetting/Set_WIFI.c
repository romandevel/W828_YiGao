#include "SysSetting.h"
#include "net_api.h"
#include "socket_api.h"
#include "wmi.h"
#include "hy_unifi.h"


#define PP(x) (x<<24)|((x<<8)&0xff0000)|((x>>8)&0xff00)|(x>>24)
#define GETHEXDIGIT(x) (((x)>='0' && (x)<='9') ? (x)-'0' : ((x)>='a' ? (x)-'a'+10 : (x)-'A'+10))

U8  gu8ApTotal;
static eWMI_ENCRYPTION_TYPE gecpy_type; //ap加密方式 
U8	gu8WifiSavePwIdx, gu8WifiSaveFlag;
tWMI_BECON_BUFF *gpWifiInfo;
U8  gau8ConnMac[8];
U8	gau8ConnPw[50];

/*===============================WIFI IP填写界面===================================*/
//控件
const tGrapViewAttr InputIPViewAttr[]=
{
	{COMM_DESKBOX_ID, 	0,0,240,320,		0,0,0,0,	  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_3,	5,60,80,20, 		0,0,0,0,   	0,0,0,0,1,0,0,0},//IP地址
	{BUTTON_ID_4,	5,105,80,20, 		0,0,0,0,   	0,0,0,0,1,0,0,0},//子网掩码
	{BUTTON_ID_5,	5,150,80,20, 		0,0,0,0,   	0,0,0,0,1,0,0,0},//默认网关
	{BUTTON_ID_6,	5,200,80,20, 		0,0,0,0,   	0,0,0,0,1,0,0,0},//首选DNS服务器
	//{BUTTON_ID_7,	5,230,90,20, 0,0,0,0,   0,  0,0,0,0,1,0,0,0},//备用DNS服务器
	
	{EDIT_ID_1,	  90,60,130,24,  	90, 60,220, 84,   	0,0,COLOR_STR_UNSELECT,0,1,1,1,0},//IP地址1
	
	{EDIT_ID_2,  90,105,130,24,  	90, 105,220, 129,   0,0,COLOR_STR_UNSELECT,0,1,0,1,0},//子网掩码

	{EDIT_ID_3,  90,150,130,24,  	90, 150,220, 174,  	0,0,COLOR_STR_UNSELECT,0,1,0,1,0},//默认网关

	{EDIT_ID_4,  90,200,130,24,  	90, 200,220, 224,  	0,0,COLOR_STR_UNSELECT,0,1,0,1,0},//首选DNS服务器1

/*
	{EDIT_ID_41,  90,230,33,24,  90, 230,139, 254,   COLOR_STR_UNSELECT,  0,0,&gu8LangIndex,1,0,1,0},//备用DNS服务器1

*/
};

const tGrapButtonPriv  InputIPBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN,  NULL},//
	{(U8 *)"返回", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN,  NULL},//
#else
	{(U8 *)"返回", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN,  NULL},//
	{(U8 *)"确定", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN,  NULL},//
#endif	
	{(U8 *)"IP地址:", 	0xffff,0xffff,FONTSIZE8X16,0,2,0,TY_UNITY_BG1_EN,  NULL},//
	{(U8 *)"子网掩码:", 0xffff,0xffff,FONTSIZE8X16,0,2,0,TY_UNITY_BG1_EN,  NULL},//
	{(U8 *)"默认网关:", 0xffff,0xffff,FONTSIZE8X16,0,2,0,TY_UNITY_BG1_EN,  NULL},//
	{(U8 *)"DNS服务器", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN,  NULL},//
	//{NULL,(U8 *)"备用DNS服务器", 0xffff,0x0,FONTSIZE8X16,0,2,KEY105_OK,0,  0,  NULL,NULL},//
};


const tGrapEditPriv  InputIPEditPri[] = 
{
	{NULL,0x0,0,0,2,5,15,  1,15,0,0,2,0,0,1,  50,50,0,  0,0,1,0},
};

S32 InputIP_BtnPress(void *pView, U16 state)
{
	tGRAPBUTTON     *pButton;
	tGRAPDESKBOX    *pDesk;
	tGRAPEDIT 		*pEdit;
	U8				*pStr[]={gtHyc.ipAddr, gtHyc.mask, gtHyc.gateWay, gtHyc.dns1};
	U8 i=0;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pButton = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pButton->pParent;
	switch(pButton->view_attr.id)
	{
	case BUTTON_ID_1:
		for(i = 0; i < 4; i++)
		{
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1+i);
			if(i < 3 && strlen(pEdit->edit_privattr.digt) == 0)
			{
				Com_SpcDlgDeskbox("信息不完整!",6,pDesk,1,NULL,NULL,100);
				return RETURN_REDRAW;
			}
			
			if(glbVariable_CheckIp(pEdit->edit_privattr.digt) == -1)
			{
				Com_SpcDlgDeskbox("格式错误!",6,pDesk,1,NULL,NULL,100);
				return RETURN_REDRAW;
			}
			strcpy(pStr[i], pEdit->edit_privattr.digt);
		}
		
		glbVariable_SaveParameter();
		Com_SpcDlgDeskbox("保存成功!",0,pDesk,1,NULL,NULL,100);
		
		//如果wifi已经连接,  需要断开重新连接
		ComWidget_DisConnectAp(hy_ap_get_connect_infor());
		
		pDesk->view_attr.reserve = 1;
		return RETURN_QUIT;
		break;	
	case BUTTON_ID_2:
		pDesk->view_attr.reserve = 0;
		return RETURN_QUIT;
		break;	
	default:
		break;
	}

	return RETURN_REDRAW;
}



int Mask_EditEnter(void *pView)
{
	tGRAPEDIT 	 	*pEdit;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(strlen(pEdit->edit_privattr.digt) == 0)
	{
		strcpy(pEdit->edit_privattr.digt, "255.255.255.0");
		pEdit->draw(pEdit, 0, 0);
	}
	
	return SUCCESS;
}


int GateWaye_EditEnter(void *pView)
{
	tGRAPEDIT 	 	*pEdit,*pEditIp;
	tGRAPDESKBOX	*pDesk;
	int len;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk = (tGRAPDESKBOX *)pEdit->pParent;
	
	pEditIp = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
	len = strlen(pEditIp->edit_privattr.digt);
	if(strlen(pEdit->edit_privattr.digt) == 0 && len > 0)
	{
		strcpy(pEdit->edit_privattr.digt, pEditIp->edit_privattr.digt);
		pEdit->edit_privattr.Pos = len;
		if(len > pEdit->edit_privattr.len)
		{
			pEdit->edit_privattr.firstCharPos = len-pEdit->edit_privattr.len;
		}
		pEdit->draw(pEdit, 0, 0);
	}
	
	return SUCCESS;
}


int InputIP_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT 		*pEdit;
	tNET_DEV    	tNet_Dev;
	struct in_addr 	addr;
	U8				*pTmp, *pStr[]={gtHyc.ipAddr, gtHyc.mask, gtHyc.gateWay, gtHyc.dns1};
	int i, idx = 0,ret;
	
	pDesk = Grap_CreateDeskbox(&InputIPViewAttr[idx++], 0);
	if(!pDesk)	return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_5;
	
	for (i=0;i<6; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &InputIPViewAttr[idx++], &InputIPBtnAttr[i]);
		pBtn->pressEnter = InputIP_BtnPress;
	}		
	
	for(i=0;i<4;i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &InputIPViewAttr[idx++], &InputIPEditPri[0]);
		pEdit->DrawInputCnt = Set_EditDrawInputCnt;
		if (pEdit == NULL) return;
		strcpy(pEdit->edit_privattr.digt, pStr[i]);
		
		if(i == 1)//子网掩码
		{
			pEdit->editenter = Mask_EditEnter;
		}
		else if(i == 2)//网关
		{
			pEdit->editenter = GateWaye_EditEnter;
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ret = pDesk->view_attr.reserve;
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ret;
}


/*===============================WIFI IP信息界面===================================*/
const tGrapViewAttr  WifiIpViewAttr[]= 
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0x12345678},//背景图片1 
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{BUTTON_ID_2,  10, 40, 80, 16,     0, 0,0,0,       0,0,0,	0,1,0,0,0},//MAC地址
	{BUTTON_ID_3,  10, 70, 80, 16,		0, 0,0,0,		0,0,0,	0,1,0,0,0},//IP地址
	{BUTTON_ID_4,  10, 100,80, 16,		0, 0,0,0,		0,0,0,	0,1,0,0,0},//子网掩码
	{BUTTON_ID_5,  10, 130,80, 16,		0, 0,0,0,		0,0,0,	0,1,0,0,0},//默认网关
	{BUTTON_ID_6,  10, 160,80, 16,		0, 0,0,0,		0,0,0,	0,1,0,0,0},//DNS服务器
	{BUTTON_ID_7,  10, 190,80, 16,		0, 0,0,0,		0,0,0,	0,1,0,0,0},//连接AP的MAC
	
	{EDIT_ID_1,   	100,  40, 130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//MAC地址
	{EDIT_ID_2,   	100,  70, 130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//IP地址
	{EDIT_ID_3,   	100,  100,130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//子网掩码
	{EDIT_ID_4,   	100,  130,130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//默认网关
	{EDIT_ID_5,   	100,  160,130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//DNS服务器
	{EDIT_ID_6,   	100,  190,130, 16,      0,   0,  0,   0,   0,	0,	0,	0,1,0,1,0},//连接AP的MAC
};

const tGrapButtonPriv  WifiIpBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
	
	{(U8 *)"MAC地址",	0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//MAC地址
	{(U8 *)"IP地址",	0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//IP地址
	{(U8 *)"子网掩码",	0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//子网掩码
	{(U8 *)"默认网关",	0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//默认网关
	{(U8 *)"DNS服务器", 0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//DNS服务器
	{(U8 *)"AP MAC", 	0xffff,0,0,0,0,0,TY_UNITY_BG1_EN, NULL},//
};

const tGrapEditPriv  WifiIpEditAttr[]=
{
 	{NULL,	0xffff,0,0,2,2,20,0,20,0,0,2,TY_UNITY_BG1_EN,0,1,50,50,0,0,0,1,0},
};

int WifiIp_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://返回
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------
*函数:WifiIp_Desk
*功能:IP详细信息
*参数:
*返回:
*-----------------------------------------------*/
void WifiIp_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit[6];
	tNET_DEV    	tNet_Dev;
	tWMI_BECON_BUFF	*pAp;
	struct in_addr 	addr;
	U8				*pTmp;
	int				i,idx=0;
	U8				*pStr[]={gtHyc.ipAddr, gtHyc.mask, gtHyc.gateWay, gtHyc.dns1};
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&WifiIpViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	//按钮
	for(i = 0; i < 7; i++)
	{
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr*)&WifiIpViewAttr[idx++], (tGrapButtonPriv*)&WifiIpBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    if(i == 0)
	    {
	    	pBtn->pressEnter = WifiIp_BtnEnter;
	    }
    }
	
	for(i = 0; i < 6; i++)
	{
		pEdit[i] = Grap_InsertEdit(pDesk, &WifiIpViewAttr[idx++], &WifiIpEditAttr[0]);
		pEdit[i]->DrawInputCnt = Set_EditDrawInputCnt;
		if(pEdit[i] == NULL) return ;
		if(i == 5)
		{
			pAp = hy_ap_get_connect_infor();
			if(hy_get_ap_link_status() == 1 && pAp != NULL)
			{
				sprintf(pEdit[i]->edit_privattr.digt, "%02x-%02x-%02x-%02x-%02x-%02x",
					pAp->apBssInfo.bssid[0],pAp->apBssInfo.bssid[1],pAp->apBssInfo.bssid[2],
					pAp->apBssInfo.bssid[3],pAp->apBssInfo.bssid[4],pAp->apBssInfo.bssid[5]);
			}
			else
			{
				strcpy(pEdit[i]->edit_privattr.digt, "0.0.0.0");
			}
		}
		else if(i > 0)
		{
			strcpy(pEdit[i]->edit_privattr.digt, "0.0.0.0");
		}
	}	
	
	pTmp = csr_getmac();
	sprintf(pEdit[0]->edit_privattr.digt, "%02x-%02x-%02x-%02x-%02x-%02x",pTmp[0],pTmp[1],pTmp[2],pTmp[3],pTmp[4],pTmp[5]);
	
	if(gtHyc.ipMode == 1)//手动填写
	{
		//ip
		if(strlen(gtHyc.ipAddr) > 0)
		{
			strcpy(pEdit[1]->edit_privattr.digt,gtHyc.ipAddr);
		}
		
		//子网掩码
		if(strlen(gtHyc.mask) > 0)
		{
			strcpy(pEdit[2]->edit_privattr.digt,gtHyc.mask);
		}
		
		//默认网关
		if(strlen(gtHyc.gateWay) > 0)
		{
			strcpy(pEdit[3]->edit_privattr.digt,gtHyc.gateWay);
		}
		
		//dns
		if(strlen(gtHyc.dns1) > 0)
		{
			strcpy(pEdit[4]->edit_privattr.digt,gtHyc.dns1);
		}
		else if(strlen(gtHyc.dns2) > 0)
		{
			strcpy(pEdit[4]->edit_privattr.digt,gtHyc.dns2);
		}
	}
	else//自动获取IP
	{
		memset(&tNet_Dev ,0,sizeof(tNET_DEV));
		Net_GetDev(NET_IF_WIFI,&tNet_Dev);
		
		addr.s_addr = PP(tNet_Dev.IpAddr);
		pTmp = inet_ntoa(addr);
		strcpy(pEdit[1]->edit_privattr.digt,pTmp);

		addr.s_addr = PP(tNet_Dev.Mask);
		pTmp = inet_ntoa(addr);
		strcpy(pEdit[2]->edit_privattr.digt,pTmp);

		addr.s_addr = PP(tNet_Dev.Gateway);
		pTmp = inet_ntoa(addr);
		strcpy(pEdit[3]->edit_privattr.digt,pTmp);
		
		if(gtHyc.dnsMode == 1)
		{
			if(strlen(gtHyc.dns1) > 0)
			{
				strcpy(pEdit[4]->edit_privattr.digt,gtHyc.dns1);
			}
			else if(strlen(gtHyc.dns2) > 0)
			{
				strcpy(pEdit[4]->edit_privattr.digt,gtHyc.dns2);
			}
		}
		else
		{
			if(tNet_Dev.DNS0 == 0)
			{
				addr.s_addr = PP(tNet_Dev.DNS1);
				pTmp = inet_ntoa(addr);
				strcpy(pEdit[4]->edit_privattr.digt,pTmp);  
			}
			else
			{
				addr.s_addr = PP(tNet_Dev.DNS0);
				pTmp = inet_ntoa(addr);
				strcpy(pEdit[4]->edit_privattr.digt,pTmp);
			}
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}


/*===============================WIFI 设置界面===================================*/
const tGrapViewAttr  Set_WIFIViewAttr[]= 
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0x12345678},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_3,	   50,150,100,16,  50,140,150,180,   0,0,0,0,1,0,0,0},//IP详细信息	
	
	{LISTER_ID_1,  	   20,50,210,40,   20,50,230,90,   	 0,0,0,0,1,1,1,0},/*list   */
};

const tGrapButtonPriv  Set_WIFIBtnAttr[]=
{
#ifdef W818C
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,  TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,  TY_UNITY_BG1_EN,NULL},//返回
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
#endif
	{"IP详细信息",	0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//IP详细信息
};

const tGrapListerPriv	Set_WIFIListAttr[]=
{
	{24, 2, 0xffff, COLOR_YELLOW,	0, 2, 0, TY_UNITY_BG1_EN, 0, 50, 4, 10, 0, 50, 50, 1, 0, 0, 1, 0,0},
};

/*
return 1:success   2:timeout
*/
int Set_WIFI_IPType(void)
{
	U32 timeout;
	int ret=1;
	U8 SelectTemp;
	
	timeout = rawtime(NULL);
	Net_AddEvt(NET_STOP);//停止后台进程
	while(Net_GetEvt() != 0)
	{
		syssleep(5);
		if(ABS(rawtime(NULL)-timeout) >= 1000*60)
		{
			ret = 2;
			break;
		}
	}
	
	if (ret == 1)
	{
		SelectTemp = gtHyc.NetSelected;
		gtHyc.NetSelected = 0xff;
		ComWidget_DisConnectAp(hy_ap_get_connect_infor());
		WifiConnect_SetDelay(0);
		gtHyc.NetSelected = SelectTemp;
	}

	return ret;
}

int Set_WIFI_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPLIST       *pList;
	int checkResult;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			if(gtHyc.ipMode != pList->lst_privattr.focusItem)
			{
				if(gtHyc.NetSelected == 0)//选择的是wifi
				{
					cartoon_app_entry(Set_WIFI_IPType,NULL);
			        AppCtrl_CreatPro(KERNAL_APP_AN, 0);
					Com_CtnDlgDeskbox(pDesk,2,"正在断开当前网络",NULL,NULL,ComWidget_LabTimerISR,50,0xFFFFFFFF);
					AppCtrl_DeletePro(KERNAL_APP_AN);
					
					checkResult = kernal_app_get_stat();
					if (checkResult == 1)
					{
						gtHyc.ipMode = pList->lst_privattr.focusItem;
						
						glbVariable_SaveParameter();
						Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,200);
					}
					else
					{
						Com_SpcDlgDeskbox("设置失败!",0,pDesk,1,NULL,NULL,200);
					}
				}
				else
				{
					gtHyc.ipMode = pList->lst_privattr.focusItem;
					glbVariable_SaveParameter();
					Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,200);
				}
			}
			else
			{
				glbVariable_SaveParameter();
				Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,200);
			}
			return RETURN_QUIT;
			break;
		case BUTTON_ID_2://返回
			return RETURN_QUIT;
			break;
		case BUTTON_ID_3://IP详细信息
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			WifiIp_Desk(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}


/**
 * lister 获得列表字符
 */
static S32 Set_WIFI_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	switch(i)
	{
	case 0:
		strcpy(str, "自动获取IP");
		break;
	case 1:
		strcpy(str, "手动设置IP");
		break;
	default:
		break;
	}
	
	return SUCCESS;
}
														
/*														
 * 点击lister各项										
 */
static S32 Set_WIFI_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	int ret;
	
	pDesk = (tGRAPDESKBOX *)(pLister->pParent);
	if(i == 1)//手动填写IP
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = InputIP_Desk(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		if(ret == 0)
		{
			ReDraw(pDesk, 0, 0); 
			return PARSED;
		}
	}
	else//自动获取
	{
		pLister->draw(pLister, 0, 0);
	}
	
	ReDraw(pDesk, 0, 0); 
	
	return SUCCESS;
}


/****************************************************************
*函数:Set_WIFI_ListDrawIcon
*功能:list 画前面的点选图
*参数:
*返回:
****************************************************************/
static S32 Set_WIFI_ListDrawIcon(tGRAPLIST *pLister, U16 i)
{
	tGrapViewAttr   *pview_attr    = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);
	U16             x1,x2,y1,y2;
		
	if ( i >= plst_privattr->totalItem)
	{
		return SUCCESS;
	}
	
	x1 = pview_attr->viewX1;
	x2 = pview_attr->viewX1 + pview_attr->viewWidth;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + plst_privattr->lstHeight;
	
	if (i == plst_privattr->focusItem)
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
	drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+4,x1+20);
	
	return SUCCESS;
}


/*---------------------------------------------
*函数:Set_WIFI
*功能:WIFI设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_WIFI(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST       *pList;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox(&Set_WIFIViewAttr[idx++], "WIFI设置");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	//按钮
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Set_WIFIViewAttr[idx++], &Set_WIFIBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    pBtn->pressEnter = Set_WIFI_BtnEnter;
    }
	
	pList =   Grap_InsertLister(pDesk, &Set_WIFIViewAttr[idx++], &Set_WIFIListAttr[0]);
	if (pList == NULL) return;
	pList->lst_privattr.focusItem = gtHyc.ipMode;
	pList->getListString          = Set_WIFI_ListerGetString;
	pList->drawIcon               = Set_WIFI_ListDrawIcon;
	pList->enter                  = Set_WIFI_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}


/*===============================WIFI 密码输入界面===================================*/


void WifiPw_SetPwType(eWMI_ENCRYPTION_TYPE type)
{
	gecpy_type = type;
}


void WifiPw_SetPw(U8 *pw)
{
	strcpy(gau8ConnPw, pw);
}

U8 *WifiPw_GetPw()
{
	return gau8ConnPw;
}


void WifiPw_SetMac(U8 *mac)
{
	memcpy(gau8ConnMac, mac, 6);
}

U8 *WifiPw_GetMac()
{
	return gau8ConnMac;
}

//获取最近连接ap的信息
tAPINFO *Wifi_GetLastAp(U8 idx)
{
	tAPINFO *pLastAp = NULL;
	int		i,j;
	U32		temp,time[WIFI_SAVEPW_MAX][2];
	
	for(i = 0; i < WIFI_SAVEPW_MAX; i++)
	{
		time[i][0] = i;
		time[i][1] = gtHyc.apInfo[i].lastTime;
		
		//hyUsbPrintf("cur ap idx = %d   time = %d \r\n", i, time[i][1]);
		//hyUsbPrintf("get ap mac = %02x-%02x-%02x-%02x-%02x-%02x \r\n",
		//			    	gtHyc.apInfo[i].mac[0],gtHyc.apInfo[i].mac[1],gtHyc.apInfo[i].mac[2],
		//			    	gtHyc.apInfo[i].mac[3],gtHyc.apInfo[i].mac[4],gtHyc.apInfo[i].mac[5]);
	}
	
	//排序
	for(i = 0; i < WIFI_SAVEPW_MAX-1; i++)
	{
		for(j = i+1; j < WIFI_SAVEPW_MAX; j++)
		{
			if(time[i][1] < time[j][1])
			{
				temp = time[i][0];
				time[i][0] = time[j][0];
				time[j][0] = temp;
				
				temp = time[i][1];
				time[i][1] = time[j][1];
				time[j][1] = temp;
			}
		}
	}
	
	//hyUsbPrintf("i = %d  idx = %d  time = %d \r\n", i, time[i][0], time[i][1]);
	if(time[idx][1] > 0)
	{
		pLastAp = &gtHyc.apInfo[time[idx][0]];
		
		hyUsbPrintf("get ap mac = %02x-%02x-%02x-%02x-%02x-%02x \r\n",
					    	pLastAp->mac[0],pLastAp->mac[1],pLastAp->mac[2],
					    	pLastAp->mac[3],pLastAp->mac[4],pLastAp->mac[5]);
	}
	
	return pLastAp;
}


/*----------------------------------------------
*函数: WifiPw_check_wep_type
*功能: 重新判断wep的类型
*参数: type ： 密码类型
*返回: 0:fail   1:success
*---------------------------------------------*/
static int WifiPw_check_wep_type(char *pwd, int type)
{
	int ret = 0;
    int i,len;
    U8  wep_pwd_len[6]={5,13,10,26,16,32};
	
	if (!(WMI_CRYPTO_WEP_40 == gecpy_type || WMI_CRYPTO_WEP_104 == gecpy_type || WMI_CRYPTO_WEP_128 == gecpy_type))
	{
		return 1;
	}
	
    len = strlen(pwd);
     
    for (i=0; i<sizeof(wep_pwd_len); i++)
    {
        if (wep_pwd_len[i] == len) 
        {
        	ret = 1; 
            break;
        }    
    }
    
    return ret;
}


/*-------------------------------------------------
*函数: WifiPw_change_wep_pwd
*功能: 对16进制的wep密码进行重新输入
*参数: buf : 密码
*返回: 密码长度
*-------------------------------------------------*/
int WifiPw_change_wep_pwd(U8 *buf)
{
    U8 tempdata[33]={0};
    U8 validdata[3]={10,26,32};
    int i,j,k;
    int len;
    
    if (!(WMI_CRYPTO_WEP_40 == gecpy_type || WMI_CRYPTO_WEP_104 == gecpy_type || WMI_CRYPTO_WEP_128 == gecpy_type))
	{
		return 0;
	}

    len = strlen(buf);
    
    for (i=0,k=0; i<3; i++)
    {
        if (len == validdata[i])
        {
        hyUsbPrintf("key len = %d \r\n", len);
            strcpy(tempdata,buf);
            for (j=0; j<len; j+=2)
            {
                tempdata[k++] = (GETHEXDIGIT(buf[j]) << 4) | (GETHEXDIGIT(buf[j+1]) & 0xF);
            }
            
            memcpy(buf,tempdata, k); 
            
            len /= 2;
            
            break;               
        }
    }
    
    return len;
}					    


const tGrapViewAttr  WifiPwViewAttr[]= 
{
	{COMM_DESKBOX_ID,	0,0,240,320,	0,0,0,0,	TY_UNITY_BG1_EN, TY_UNITY_BG1_EN, 0, 0, 1,0,0,0x12345678},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确认
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确认
#endif	
	{BUTTON_ID_3,	10, 160,72,20,	0,0,0,0,	0,0,0,	0,1,0,0,0},//保存密码
	{BUTTON_ID_4,	10, 55, 220,20,	0,0,0,0,	0,0,0,	0,1,0,0,0},//显示AP名字
	{BUTTON_ID_5,	10, 100,72,20,	0,0,0,0,	0,0,0,	0,1,0,0,0},//输入密码:
	
	{EDIT_ID_1,   	10,  125,220, 30,      10,   125,  230,   145,   0,	0,	0xffff,0,1,1,1,0},//密码
	
	{BUTTON_ID_10,	100, 160, 50,20,	 90,160,155,185,	0,0,0,	0,1,0,0,0},//是
	{BUTTON_ID_11,  170, 160, 50,20,	170,160,225,185,	0,0,0,	0,1,0,0,0},//否
};

const tGrapButtonPriv  WifiPwBtnAttr[]=
{
#ifdef W818C
	{"确定",	 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN, NULL},//确认
	{"返回",	 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN, NULL},//返回
#else
	{"返回",	 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN, NULL},//返回
	{"确定",	 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN, NULL},//确认
#endif
	{"保存密码:",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0,TY_UNITY_BG1_EN, NULL},//保存密码
	
	{NULL,       0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,TY_UNITY_BG1_EN, NULL},//显示AP名字
	
	{"输入密码:",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_LEFT,0,TY_UNITY_BG1_EN, NULL},//密码
	
	{"是",   COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0,TY_UNITY_BG1_EN,NULL},
	{"否",   COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0,TY_UNITY_BG1_EN,NULL},
};

const tGrapEditPriv  WifiPwEditAttr[]=
{
 	{NULL,	COLOR_RED,FONTSIZE24X24,0,2,2,15,0,48,0,0,0,0, 0,	 1, 50,50,1,0,0,1,0},//密码
};


const RtcTime_st wifiRecBaseTime=
{	//2011.1.1 12:00:00
	0, 0, 12, 0, 1, 1, 2011
};

U32 Wifi_UpdateTime()
{
	RtcTime_st	kRtc;
	U32 kTime,kTimeBase;
	
	hyhwRtc_GetTime(&kRtc);
	
	kTimeBase = hyhwRtc_ConvertToSeconds(&wifiRecBaseTime);
//	hyUsbPrintf("basetime: %04d-%02d-%02d %02d:%02d:%02d \r\n", 
//				wifiRecBaseTime.year, wifiRecBaseTime.month, wifiRecBaseTime.day,
//				wifiRecBaseTime.hour, wifiRecBaseTime.minute, wifiRecBaseTime.second);
	kTime = hyhwRtc_ConvertToSeconds(&kRtc);
	kTime = kTime - kTimeBase;
	
	hyUsbPrintf("time = %d \r\n", kTime);
	return kTime;
}

void WifiPw_SaveInfo(tWMI_BECON_BUFF *pAp, U8 *pPw)
{
	U32		i,j,min=0xFFFFFFFF;
	
	//是否已经有保存信息
	for(i = 0; i < WIFI_SAVEPW_MAX; i++)
	{
		if(memcmp(gtHyc.apInfo[i].mac, pAp->apBssInfo.bssid, 6) == 0)
		{
			break;
		}
	}
	
	if(i >= WIFI_SAVEPW_MAX)
	{
		for(i = 0,j = 0; i < WIFI_SAVEPW_MAX; i++)
		{
			if(gtHyc.apInfo[i].mac[0] == 0x00 && gtHyc.apInfo[i].mac[1] == 0x00 && 
			   gtHyc.apInfo[i].mac[2] == 0x00 && gtHyc.apInfo[i].mac[3] == 0x00 && 
			   gtHyc.apInfo[i].mac[4] == 0x00 && gtHyc.apInfo[i].mac[5] == 0x00)
			{
				break;
			}
			if(gtHyc.apInfo[i].lastTime < min)
			{
				min = gtHyc.apInfo[i].lastTime;
				j = i;
			}
		}
		
		hyUsbPrintf("i = %d  j = %d   min = %d \r\n", i, j, min);
		
		if(i >= WIFI_SAVEPW_MAX) i = j;
	}
	
	strcpy(gtHyc.apInfo[i].apName, pAp->ap.apName);
	memcpy(gtHyc.apInfo[i].mac, pAp->apBssInfo.bssid, 6);
	gtHyc.apInfo[i].pwType = pAp->wmiEncryptionType;
	if(pPw != NULL)
	{
		strcpy(gtHyc.apInfo[i].pw, pPw);
	}
	gtHyc.apInfo[i].lastTime = Wifi_UpdateTime();
	
	glbVariable_SaveParameter();
}

/*------------------------------------------------------
* 函数: WifiPw_BtnEnter
* 功能: 
* 参数: 
* 返回: 0
-------------------------------------------------------*/
int WifiPw_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT		*pEdit;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确认
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			
			if(strlen(pEdit->edit_privattr.digt) == 0)
			{
				break;
			}
			
			//判断输入密码长度是否符合wep规定
			if(1 == WifiPw_check_wep_type(pEdit->edit_privattr.digt, 0))
			{
			hyUsbPrintf("save flag = %d gu8WifiSavePwIdx = %d \r\n", gu8WifiSaveFlag,gu8WifiSavePwIdx);
				//是否需要保存密码
				if(gu8WifiSaveFlag == 1)
				{
					WifiPw_SaveInfo(gpWifiInfo, pEdit->edit_privattr.digt);
				}
				else
				{
					if(gu8WifiSavePwIdx != 0xff)//之前有保存记录
					{
						memset((char *)&gtHyc.apInfo[gu8WifiSavePwIdx], 0, sizeof(tAPINFO));
						glbVariable_SaveParameter();
					}
				}
				hyUsbPrintf("set pw = %s \r\n", pEdit->edit_privattr.digt);
				WifiPw_SetPw(pEdit->edit_privattr.digt);
				
				pDesk->view_attr.reserve = 1;
				*pDesk->pQuitCnt = 1;
				return RETURN_QUIT;
			}
			else
			{
				Com_SpcDlgDeskbox("密码输入错误!",0,pDesk,1,NULL,NULL,150);
			}
			break;
		case BUTTON_ID_2://返回
			pDesk->view_attr.reserve = 0;
			return RETURN_QUIT;
			break;
		case BUTTON_ID_10:
			gu8WifiSaveFlag = 1;
			break;
		case BUTTON_ID_11:
			gu8WifiSaveFlag = 0;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}



S32 NetList_ButtonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton, *pOtehrBtn;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	if (pButton->view_attr.id == BUTTON_ID_10)
	{
		if (gu8WifiSaveFlag == 1)
		{
	         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
		}
	    else
	    {
	    	drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	    }
	}
	else if (pButton->view_attr.id == BUTTON_ID_11)
	{
		if (gu8WifiSaveFlag == 0)
		{
	         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
		}
	    else
	    {
	    	drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	    }
	}
    drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,viewY1+2,viewX1);
	
	if (pbtn_privattr->pName)//显示button上的字符
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	}
	
	return SUCCESS;
}


/*---------------------------------------------
*函数:WifiPw_Desk
*功能:
*参数:
*返回:0:返回   1:确认
*-----------------------------------------------*/
U8 WifiPw_Desk(void *pDeskFather, tWMI_BECON_BUFF *pApInfo)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	U8				scan,flag=0;
	int				i,idx=0;
	
	gpWifiInfo = pApInfo;
	
	gu8WifiSavePwIdx = 0xff;
	gu8WifiSaveFlag  = 1;
	
	//查找是否有保存记录
	for(i = 0; i < WIFI_SAVEPW_MAX; i++)
	{
		if(memcmp(gtHyc.apInfo[i].mac, pApInfo->apBssInfo.bssid, 6) == 0)//MAC地址相同
		{
			if(gtHyc.apInfo[i].pwType == pApInfo->wmiEncryptionType)//加密方式相同
			{
				gu8WifiSavePwIdx = i;//记录下当前idx
			}
			else //如果同一个AP的加密方式改变了, 则清空之前的信息
			{
				gu8WifiSavePwIdx = i;//记录下当前idx
				memset(gtHyc.apInfo[i].pw, 0, sizeof(gtHyc.apInfo[i].pw));//密码清空
			}
			break;
		}
	}
		
	pDesk = Grap_CreateDeskbox(&WifiPwViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	//按钮
	for(i = 0; i < 5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &WifiPwViewAttr[idx++], &WifiPwBtnAttr[i]);
    	if(NULL == pBtn ) return;
    	if(i < 3)
    	{
    		pBtn->pressEnter = WifiPw_BtnEnter;
    	}
    	else if(i == 3)//显示AP名字
    	{
    		pBtn->btn_privattr.pName = pApInfo->ap.apName;
    	}
    }
	
	pEdit = Grap_InsertEdit(pDesk, &WifiPwViewAttr[idx++], &WifiPwEditAttr[0]);
	pEdit->DrawInputCnt = Set_EditDrawInputCnt;
	if(pEdit == NULL) return;
	if(gu8WifiSavePwIdx != 0xff)
	{
	hyUsbPrintf("cur pw = %s \r\n", gtHyc.apInfo[gu8WifiSavePwIdx].pw);
		strcpy(pEdit->edit_privattr.digt, gtHyc.apInfo[gu8WifiSavePwIdx].pw);//把密码copy进去
	}
	
	for (i=0; i<2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &WifiPwViewAttr[idx++], &WifiPwBtnAttr[i+5]);
    	if(NULL == pBtn ) return;
    	pBtn->pressEnter = WifiPw_BtnEnter;
    	pBtn->draw = NetList_ButtonDraw;
	}
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    flag = pDesk->view_attr.reserve;
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return flag;
}



/*===============================WIFI 列表界面===================================*/
const tGrapViewAttr  WifiListViewAttr[]= 
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0x12345678},
#ifdef W818C
	{BUTTON_ID_1,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//搜索
	{BUTTON_ID_2,	   83,282,73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//IP信息
	{BUTTON_ID_3,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_3,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_2,	   83,282,73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//IP信息
	{BUTTON_ID_1,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//搜索
#endif	
	{BUTTON_ID_4,	   0,30,240,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//当前搜索到的网络

	{LISTER_ID_1,	10,60,220,220,	10,60,230,280,	0,0,0,0,1,1,1,0}, //列表	
};

const tGrapButtonPriv  WifiListBtnAttr[]=
{
#ifdef W818C
	{"搜索",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//搜索
	{"Ip信息",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//Ip信息
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,  TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,  TY_UNITY_BG1_EN,NULL},//返回
	{"Ip信息",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//Ip信息
	{"搜索",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//搜索
#endif
	{NULL,	COLOR_YELLOW,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//当前搜索到的网络
};

const tGrapListerPriv WifiListListPri[]=
{   
   {28,	7,0xffff,COLOR_YELLOW,	0,0,0,	TY_UNITY_BG1_EN,0,82,6,17,0,50,50,1,0,0,1,1,0},
};

S32 WifiList_ListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	U32 len,idx;
	U16 x1, x2, y1,y2;
	U8  *pSrcStr;
	tWMI_BECON_BUFF *pap;
	tGrapViewAttr   *pview_attr = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);

	pap = hy_get_ap_info_sort(i);
	if (NULL == pap)
	{
	    str[0] = 0;
	    str[1] = 0;
	    return SUCCESS;    
	}
	
	pSrcStr = (U8 *)pap->ap.apName;
	strcpy(str, pSrcStr);
#if 0
	x1 = pview_attr->viewX1;
	x2 = x1+23;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + 26;
	
	//信号
	idx = hy_get_ap_rssi_range(pap->apBssInfo.apRssi);
	Grap_ShowPicture(SZ_SIGNAL_NOTHING_EN+idx, x1+42, y1+5);
	
	x2 = x1+35;
	if(pap->wmiEncryptionType != 0)//加密
	{
		if(pap->apStatus&0x01)
		{
			Grap_ShowPicture(SZ_SIGNAL_LOCK_EN, x1, y1-2);
		}
		else
		{
			Grap_ShowPicture(SZ_SIGNAL_DISCONNECT_LOCK_EN, x1, y1-2);
		}
	}
	else
	{
		if(pap->apStatus&0x01)
		{
			Grap_ShowPicture(SZ_SIGNAL_EN, x1, y1-2);
		}
		else
		{
			Grap_ShowPicture(SZ_SIGNAL_DISCONNECT_EN, x1, y1-2);
		}
	}
#endif
	
	return FAIL;
}

S32 WifiList_DrawIcon(tGRAPLIST *pLister, U16 i)
{
	tWMI_BECON_BUFF *pap;
	tGrapViewAttr   *pview_attr = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);
	U32 len,idx;
	U16 x1, x2, y1,y2;
	
	
	pap = hy_get_ap_info_sort(i);
	if (NULL == pap)
	{
	    return SUCCESS;    
	}
	
	x1 = pview_attr->viewX1;
	x2 = x1+23;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + 26;
	
	//信号
	idx = hy_get_ap_rssi_range(pap->apBssInfo.apRssi);
	Grap_ShowPicture(SZ_SIGNAL_NOTHING_EN+idx, x1+42, y1+5);
	
	x2 = x1+35;
	if(pap->wmiEncryptionType != 0)//加密
	{
		if(pap->apStatus&0x01)
		{
			Grap_ShowPicture(SZ_SIGNAL_LOCK_EN, x1, y1-2);
		}
		else
		{
			Grap_ShowPicture(SZ_SIGNAL_DISCONNECT_LOCK_EN, x1, y1-2);
		}
	}
	else
	{
		if(pap->apStatus&0x01)
		{
			Grap_ShowPicture(SZ_SIGNAL_EN, x1, y1-2);
		}
		else
		{
			Grap_ShowPicture(SZ_SIGNAL_DISCONNECT_EN, x1, y1-2);
		}
	}
	
	
	return SUCCESS;
}


S32 WifiList_ListDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLIST *pLister;
	U16 i;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1+pview_attr->viewWidth), 
		(U16)(pview_attr->viewY1+pview_attr->viewHeight));
	
	//画个边框
	drvLcd_SetColor(0,0);
	//drvLcd_Rectangle(pview_attr->viewY1-2, pview_attr->viewX1-2, pview_attr->viewHeight+4, pview_attr->viewWidth+4, 2);
	
	if(plst_privattr->totalItem > plst_privattr->lstNum)
	{//需要显示滚动条
		if(plst_privattr->topItem > plst_privattr->totalItem - plst_privattr->lstNum)
		{
			plst_privattr->topItem = plst_privattr->totalItem - plst_privattr->lstNum;
		}
	}
	
	if(plst_privattr->totalItem <= plst_privattr->lstNum && plst_privattr->topItem > 0)
	{
		plst_privattr->topItem = 0;
	}
	if(plst_privattr->focusItem >= plst_privattr->totalItem && plst_privattr->totalItem > 0)
	{
		plst_privattr->focusItem = plst_privattr->totalItem-1;
	}
	
	for (i = 0; i < plst_privattr->lstNum; i++)
	{
		if(plst_privattr->topItem + i >= plst_privattr->totalItem)
			break;
		pLister->updateList(pLister, (U16)(plst_privattr->topItem + i));
	}
		
	return SUCCESS;
}

//对ap 进行断开和连接的操作
int WifiList_EnterList(tGRAPLIST *pLister, U16 i)
{
	tWMI_BECON_BUFF *pap,*ptemp;
	tGRAPDESKBOX	*pDesk;
	tGRAPLIST 		*pList;
	eDLG_STAT		kDlgState;
	eWMI_ENCRYPTION_TYPE ctype;
	U8	mac[6],flag;
	
	pDesk = (tGRAPDESKBOX *)pLister->pParent;	
	
	pap = hy_get_ap_info_sort(i);
	
	//判断该AP是否已经是连接状态
	if(pap == NULL)
	{
		return RETURN_CANCLE;
	}
	
	if(pap->apStatus&0x01)
	{//是连接状态
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否断开网络?",0,pDesk,3,NULL,NULL,500))
		{//断开当前连接
			kDlgState = ComWidget_ApEvent(pDesk, "正在断开网络,请稍候...", CMD_DISCONNECT, (void *)pap);
			if(kDlgState == DLG_CONFIRM_EN && kernal_app_get_stat() == 1)
			{
				//pap->apStatus &= (~0x01);
			}
			return RETURN_REDRAW;
		}
		else
		{
			return RETURN_REDRAW;
		}
	}
	
	ctype = pap->wmiEncryptionType;//加密类型
	if(ctype != WMI_CRYPTO_OPEN)//加密AP
	{
		gecpy_type = ctype;
		
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		flag = WifiPw_Desk(pDesk, pap);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		if(flag == 0)
		{
			return RETURN_REDRAW;
		}
	}
	
	if(hy_get_ap_link_status() == 1)
	{//已经有连接的AP,但不是本次点击的,提示是否断开当前连接,继续本次连接
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否断开当前连接,继续本次连接?",0,pDesk,3,NULL,NULL,500))
		{
			memcpy(mac, pap->apBssInfo.bssid, 6);
			ptemp = hy_ap_get_connect_infor();
			if(ptemp != NULL)
			{
				kDlgState = ComWidget_ApEvent(pDesk, "正在断开网络,请稍候...", CMD_DISCONNECT, (void *)ptemp);
				if(kDlgState == DLG_CONFIRM_EN && kernal_app_get_stat() == 1)
				{
					//ptemp->apStatus &= (~0x01);
				}
				else//断开失败
				{
					return RETURN_REDRAW;
				}
			}
			pap = hy_get_ap_info_bymac(mac);
		}
		else
		{
			return RETURN_REDRAW;
		}
	}
//hyUsbPrintf("ap name = %s  ap mac = %02x-%02x-%02x-%02x-%02x-%02x  Encryp = %d \r\n",pap->ap.apName,
//		pap->apBssInfo.bssid[0],pap->apBssInfo.bssid[1],pap->apBssInfo.bssid[2],
//		pap->apBssInfo.bssid[3],pap->apBssInfo.bssid[4],pap->apBssInfo.bssid[5],pap->wmiEncryptionType);

	
	hy_ap_setinfo(pap);
	
	//hyUsbPrintf("####statr connect net\r\n");
	
	kDlgState = ComWidget_ApEvent(pDesk, "正在连接网络,请稍候...", CMD_CONNECT, (void *)pap);
	
	//hyUsbPrintf("####end connect net\r\n");
	
	if(DLG_CONFIRM_EN == kDlgState)
	{
		if(kernal_app_get_stat() == 1)
		{
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			pList->lst_privattr.focusItem = 0;
	        pList->lst_privattr.topItem = 0;
			//hy_set_ap_link_status(1);
			//pap->apStatus |= 0x01;//连接状态
			Com_SpcDlgDeskbox("连接成功!",0,pDesk,1,NULL,NULL,100);
		}
		else
		{
			Com_SpcDlgDeskbox("网络连接失败!",0,pDesk,1,NULL,NULL,100);
		}
	}
	
	return RETURN_REDRAW;
}

S32 WifiList_BtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;
	U8 str[30];
	
	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 

	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	sprintf(str, "当前搜索到的网络 (共%d个)",gu8ApTotal);
	disp_info.string = str;
	disp_info.color = pbtn_privattr->cl_normal;
	disp_info.font_size = pbtn_privattr->font_size;
	disp_info.font_type = pbtn_privattr->font_type;
	disp_info.flag = pbtn_privattr->position;
	disp_info.keyStr = NULL;
	Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	
	return SUCCESS;
}

/*------------------------------------------------------
* 函数: WifiList_BtnEnter
* 功能: WIFI列表
* 参数: 
* 返回: 0
-------------------------------------------------------*/
int WifiList_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPLIST	 	*pList;
	int i;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://搜索
			ComWidget_ApEvent(pDesk, "正在搜索网络,请稍候...", CMD_SCAN, NULL);
	        
	        pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
	        gu8ApTotal = hy_get_scan_ap_count();
	        pList->lst_privattr.totalItem = gu8ApTotal;
	        pList->lst_privattr.focusItem = 0;
			break;
		case BUTTON_ID_2://IP信息
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			WifiIp_Desk(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			break;
		case BUTTON_ID_3://返回
			return RETURN_QUIT;			
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------
*函数:Wifi_ListDesk
*功能:WIFI列表
*参数:
*返回:
*-----------------------------------------------*/
void Wifi_ListDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST	 	*pList;
	int				i,idx=0;
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&WifiListViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	for(i = 0; i < 4; i++)
	{
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr*)&WifiListViewAttr[idx++], (tGrapButtonPriv*)&WifiListBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    if(i < 3)
	    {
	    	pBtn->pressEnter = WifiList_BtnEnter;
	    }
	    else if(i == 3)
	    {
	    	pBtn->draw = WifiList_BtnDraw;
	    }
    }
	
	pList = Grap_InsertLister(pDesk,(tGrapViewAttr*)(&WifiListViewAttr[idx++]),(tGrapListerPriv*)(&WifiListListPri[0]));
	if (NULL== pList) return ;
	pList->getListString = WifiList_ListStr;
	gu8ApTotal = hy_get_scan_ap_count();
	pList->lst_privattr.totalItem = gu8ApTotal;
	pList->drawIcon = WifiList_DrawIcon;
	pList->draw = WifiList_ListDraw;
	pList->enter = WifiList_EnterList;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
