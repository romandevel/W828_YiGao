#include "Op_Login.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"

#ifdef VER_CONFIG_HYCO

//0xCE59 灰色

extern int DownLoad_DownProblem(void *p);

void Login_ClearLoginInfo();
int DownLoad_DownCompany(void *p);
int DownLoad_DownEmployee(void *p);
void Login_ShowUserInfo_Desk(void *pDeskFather);
/*==============================================================================*/
#define		RELAND_TIME				(24*60*60*100)

U8		gu8LoginFlag;
U8		gau8UserName[30];
U8		gau8Md5Str[40];
U8		gau8ReqHead[300];
tLOGOINFO gtLogoInfo;

/*===============================================================*/
const tGrapViewAttr Login_DeskAttr[] =
{
	{MAIN_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//登录
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//联系方式
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//联系方式
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//登录
#endif	
	{BUTTON_ID_3,	   50, 240 ,50, 16,   50,230,100,260,   0,0, 0,0,1,0,0,0},//WIFI
	{BUTTON_ID_4,	   140, 240 ,50, 16,   140,230,200,260,   0,0, 0,0,1,0,0,0},// GSM
	
	{BUTTON_ID_5,	   25, 200 ,100, 24,   25 ,200,120,224,   0,0, 0,0,1,0,0,0},//电信网络
	{BUTTON_ID_6,	   130,200 ,100, 24,   130,200,230,224,   0,0, 0,0,1,0,0,0},//联通网络
	
	
	{BUTTON_ID_7,	   0, 70 ,240, 16,   0,0,0,0,   0,0, 0,0,1,0,0,0},//对应账号的名称
	
	{BUTTON_ID_8,	   10, 110 ,40, 16,   0,0,0,0,   0,0,0,0,1,0,0,0},//账号
	{BUTTON_ID_9,	   10, 150 ,40, 16,   0,0,0,0,   0,0,0,0,1,0,0,0},//密码
	
	{EDIT_ID_1,	  52,  110,164,24,  50, 110,216,134, 0,0,0xffff,0,1,0,1,0},//
	{EDIT_ID_2,	  52,  150,164,24,  50, 150,216,174, 0,0,0xffff,0,1,0,1,0},//
};

const tGrapButtonPriv Login_BtnAttr[]=
{
#ifdef W818C
	{"登录",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
	{"联系方式",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{"联系方式",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
	{"登录",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
#endif	
	{"WIFI",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"GPRS",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	
	{"电信网络",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//网点名称
	{"联通网络",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//网点名称
	
	{"易高快运",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//账号对应的用户名
	
	{"账号:",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_LEFT,0, TY_UNITY_BG1_EN,NULL},//
	{"密码:",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_LEFT,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv Login_EditPri[] = 
{
	{NULL,0x0,0,0,2,6,19,  1,20,0,0,2,0,0,1,  50,50,0,  0,1,1,1},//用户名
	{NULL,0x0,0,0,2,6,19,  1,20,0,0,2,0,0,1,  50,50,1,  0,0,1,0},//密码
};


/*-----------------------------------------------------------------
*函数:Login_VarInit
*功能:初始化
*参数:
*返回:
*-----------------------------------------------------------------*/
void Login_VarInit()
{
	Op_CommVar_Init();
	Op_DownLoad_Init();//资料下载模块初始化
	
	Evt_VarInit();
	
	gu8LoginFlag = 0;
	memset(gau8UserName,0, sizeof(gau8UserName));
	memset(gau8Md5Str,  0, sizeof(gau8Md5Str));
	memset(&gtLogoInfo, 0, sizeof(tLOGOINFO));
	memset(gau8ReqHead, 0, sizeof(gau8ReqHead));
	
	Login_ClearLoginInfo();

	//通过公司编号获取公司名称
	Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
}

/*-----------------------------------------------------------------
*函数:Login_LoginInfo
*功能:登录信息
*参数:
*返回:0:没有登录信息   1:有登录信息
*-----------------------------------------------------------------*/
int Login_HasInfo()
{
	if(strlen(gtHycBak.userID) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*-----------------------------------------------------------------
*函数:Login_GetLoginFlag
*功能:检查是否登录
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 Login_GetLoginFlag()
{
	return gu8LoginFlag;
}


/*-----------------------------------------------------------------
*函数:Login_GetLoginId
*功能:获取登录人的ID
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 *Login_GetUserId()
{
	if(gu8LoginFlag == 1)
	{
		return gtLogoInfo.userID;
	}
	else
	{
		return gtHycBak.userID;
	}
}

U8 *Login_GetUserPw()
{
	if(gu8LoginFlag == 1)
	{
		return gtLogoInfo.userPw;
	}
	else
	{
		return gtHycBak.userPw;
	}
}

void Login_SetUserName(U8 *pName)
{
	int len,size;
	
	size = sizeof(gau8UserName);
	memset(gau8UserName, 0, size);
	
	len = strlen(pName);
	if(len >= size) len = size-1;
	memcpy(gau8UserName, pName, len);
	
	strcpy(gtLogoInfo.userName, gau8UserName);
}

/*-----------------------------------------------------------------
*函数:Login_GetName
*功能:获取登录人的姓名
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 *Login_GetName()
{
	if(gu8LoginFlag == 1)
	{
		return gtLogoInfo.userName;
	}
	else
	{
		return NULL;
	}
}

/*-----------------------------------------------------------------
*函数:Login_GetPw_MD5
*功能:获取加密后的密码
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 *Login_GetPw_MD5()
{
	return gau8Md5Str;
}


/*-----------------------------------------------------------------
*函数:Login_GetRight
*功能:获取登录人的权限
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 Login_GetRight()
{
	return gtLogoInfo.userRight;
}

/*-----------------------------------------------------------------
*函数:Login_GetGroupId
*功能:获取登录人分组编号
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 Login_GetGroupId()
{
	return (U8)atoi(gtLogoInfo.pGroup[gtLogoInfo.groupSelect].groupId);
	//return gtLogoInfo.groupId;
}


/*-----------------------------------------------------------------
*函数:Login_GetReqHead
*功能:请求头
*参数:
*返回:
*-----------------------------------------------------------------*/
U8 *Login_GetReqHead()
{
	return gau8ReqHead;
}

//清空登录信息
void Login_ClearLoginInfo()
{
	memset((char *)&gtLogoInfo,0,sizeof(tLOGOINFO));
	gtLogoInfo.pGroup = (tGROUPINFO *)hsaSdram_UserGroupBuf();
}

/*
离线验证
return 0:success     other:fail
*/
int Login_OutLine(tGRAPDESKBOX	*pDesk, U8 *id, U8 *pw)
{
	if (strcmp(id, gtHyc.lastLogin.userId)==0 && strcmp(pw, gtHyc.lastLogin.userPw)==0)
	{
		strcpy(gtLogoInfo.userID, gtHyc.lastLogin.userId); 
		strcpy(gtLogoInfo.userPw, gtHyc.lastLogin.userPw); 
		strcpy(gtLogoInfo.userName, gtHyc.lastLogin.userName); 
		gtLogoInfo.userRight = gtHyc.lastLogin.userRight;
		gtLogoInfo.userType = gtHyc.lastLogin.userType;
		gtLogoInfo.groupTotal = gtHyc.lastLogin.groupTotal;
		gtLogoInfo.groupDef = gtHyc.lastLogin.groupDefault;
		gtLogoInfo.groupSelect = gtHyc.lastLogin.groupSelect;
		strcpy(gtLogoInfo.pGroup[0].groupId, gtHyc.lastLogin.groupId);
		strcpy(gtLogoInfo.pGroup[0].groupName, gtHyc.lastLogin.groupName);
		gtLogoInfo.landTime = rawtime(NULL);
	
		return 0;
	}
	else
	{
		Com_SpcDlgDeskbox("账号或密码和最后一次登录不一致!",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
	}

	
	return 1;
}

/*-----------------------------------------
* 函数: Login_FillGroup
* 功能: 解析分组信息
* 参数: 
* 返回: 分组数
*-----------------------------------------*/
U8 Login_FillGroup(U8 *pBuf, tGROUPINFO *pGroup)
{
	U8 *pS,*pE,t=0;

	pS = strstr(pBuf,"<o><d>");
	while(pS != NULL)
	{
		memset((char *)&pGroup[t],0,sizeof(tGROUPINFO));
		pS += 6;
		pE  = strstr(pS,"</d><d>");
		memcpy(pGroup[t].groupId,pS,pE-pS);
		
		pS = pE+7;
		pE  = strstr(pS,"</d></o>");
		memcpy(pGroup[t].groupName,pS,pE-pS);
		
		t++;
		pS = strstr(pE,"<o><d>");
	}
	
	return t;
}


int Login_GetOtherInfo(void *p)
{

	Comm_ClearSoftVer();

	//检查程序版本
	Common_NetUpgrade(1, NULL);
#if 0
	//查手机号
	if(gtHyc.NetSelected == 1)//gsm
	{
		YD_GetSIMID(gsm_getIMSI());
	}
	else//wifi
	{
		YD_GetSIMID(NULL);
	}
	
	//更新重量体积  异常原因  2012-08-15 孔祥文  韵达要求每次登录时更新一次
	Evt_UpdateWtVol_Check();
#endif
	return 0;
}

/*----------------------------------------------------
* 函数:Login_TestID
* 功能:测试账号登录
* 参数:
* 返回:-1:失败   0:成功
-----------------------------------------------------*/
int Login_TestID(tGRAPDESKBOX *pDesk, U8 *pId, U8 *pPw)
{
	Login_ClearLoginInfo();
	strcpy(gtLogoInfo.userID,pId);
	strcpy(gtLogoInfo.userPw,pPw);
	strcpy(gtLogoInfo.userName, gau8UserName);
	gtLogoInfo.userRight = 0;//测试账号
	gtLogoInfo.userType  = 0;//类型
	gtLogoInfo.groupTotal  = 1;
	gtLogoInfo.groupSelect = 0;
	gtLogoInfo.groupDef    = 0;
	memset(gtLogoInfo.pGroup,0,sizeof(tGROUPINFO));
	strcpy(gtLogoInfo.pGroup[0].groupId,"0");
	strcpy(gtLogoInfo.pGroup[0].groupName,"测试账号默认分组");
	gtLogoInfo.landTime = rawtime(NULL);
	
	//保存登录信息
	if(strcmp(gtHycBak.userID,gtLogoInfo.userID) != 0 || strcmp(gtHycBak.userPw,gtLogoInfo.userPw) != 0)
	{
		strcpy(gtHycBak.userID,gtLogoInfo.userID);
		strcpy(gtHycBak.userPw,gtLogoInfo.userPw);
		
		glbBakVariable_SaveToPersistentArea();
	}
	
	return 0;
}

/*----------------------------------------------------
* 函数:Login_OldID
* 功能:之前登录过的账号登录
* 参数:
* 返回:-1:失败   0:成功
-----------------------------------------------------*/
int Login_OldID(tGRAPDESKBOX *pDesk, U8 *pId, U8 *pPw)
{
	
	return 0;
}


int Op_Login_Online(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	U8				*pSub;
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=GBK";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
hyUsbPrintf("login send = %s \r\n", pSend);
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
hyUsbPrintf("login recv = %s \r\n", pSend);
	if(recvLen > 0)
	{
		pSub = strstr(pSend,"<result>0");
		if(pSub != NULL)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 3;
	}
	
	return ret;
}

/*----------------------------------------------------
* 函数:Login_NewID
* 功能:新账号登录
* 参数:
* 返回:0:成功   -1:失败(数据错误,不能离线登录)    -2:失败 (更新其他资料是失败)
-----------------------------------------------------*/
int Login_NewID(tGRAPDESKBOX *pDesk, U8 *pId, U8 *pPw)
{
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	U8				*pSend,*pTmp,*pEnd,tmbuf[10];
	int				i,err,ret = 0,len;
	
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	
	//更新公司信息
	/*if(DownLoad_Company_GetTotal() == 0)
	{
		cartoon_app_entry(DownLoad_DownCompany,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新网点信息,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err != 1 && err != 11)
			{
				Com_SpcDlgDeskbox("更新网点信息失败,请登录后手动更新!",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			}
		}
	}
	
	//更新公司下面的用户信息
	if(Op_Employee_Total() == 0)
	{
		cartoon_app_entry(DownLoad_DownEmployee,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新用户信息,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err != 1)
			{
				Com_SpcDlgDeskbox("更新用户信息失败,请登录后手动更新!",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			}
		}
	}
	
	//问题原因
	if(Op_DownLoad_GetProblemCnt() == 0)
	{
		cartoon_app_entry(DownLoad_DownProblem,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新问题件资料,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err != 1)
			{
				Com_SpcDlgDeskbox("更新问题件资料失败,请登录后手动更新.",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			}
		}
	}*/

#if 1	
	//登录
	sprintf(pSend, "<?xml version=\"1.0\" encoding=\"GBK\"?><senddata><accessMethod>loginPDA</accessMethod><versionTime>0</versionTime>\
<baseValue><dev>%s</dev><userName>%s</userName><password>%s</password><belongSite>%s</belongSite></baseValue></senddata>",
				glbVar_GetMachineId(),pId,pPw,glbVar_GetCompanyId());
	
	
	cartoon_app_entry(Op_Login_Online,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在验证信息,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err != 1)
		{
			Com_SpcDlgDeskbox("账号或密码错误.",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			return -1;
		}
	}
#else//离线验证
	
	if(1 != Op_DownLoad_CheckIdPw(NULL, pId, pPw))
	{
		Com_SpcDlgDeskbox("账号或密码错误.",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		return -1;
	}
	
#endif
	Employee_GetNameById(pId,gau8UserName, sizeof(gau8UserName));
	Employee_GetCompanyById(pId, gtHyc.companyId, sizeof(gtHyc.companyId));
	//通过公司编号获取公司名称
	Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
	
	Login_ClearLoginInfo();
	strcpy(gtLogoInfo.userID,pId);
	strcpy(gtLogoInfo.userPw,pPw);
	strcpy(gtLogoInfo.userName, gau8UserName);
	gtLogoInfo.userRight = 0;//测试账号
	gtLogoInfo.userType  = 0;//类型
	gtLogoInfo.groupTotal  = 1;
	gtLogoInfo.groupSelect = 0;
	gtLogoInfo.groupDef    = 0;
	memset(gtLogoInfo.pGroup,0,sizeof(tGROUPINFO));
	strcpy(gtLogoInfo.pGroup[0].groupId,"0");
	strcpy(gtLogoInfo.pGroup[0].groupName,"默认分组");
	gtLogoInfo.landTime = rawtime(NULL);
	
	//保存登录信息,做离线登录用
	strcpy(gtHyc.lastLogin.userId, pId);
	strcpy(gtHyc.lastLogin.userPw, pPw);
	strcpy(gtHyc.lastLogin.userName, gtLogoInfo.userName);
	gtHyc.lastLogin.userRight = gtLogoInfo.userRight;
	gtHyc.lastLogin.userType  = gtLogoInfo.userType;
	gtHyc.lastLogin.groupTotal= gtLogoInfo.groupTotal;
	gtHyc.lastLogin.groupDefault  = gtLogoInfo.groupDef;
	gtHyc.lastLogin.groupSelect= gtLogoInfo.groupSelect;
	strcpy(gtHyc.lastLogin.groupId, gtLogoInfo.pGroup[0].groupId);
	strcpy(gtHyc.lastLogin.groupName, gtLogoInfo.pGroup[0].groupName);
	
hyUsbPrintf("%s   %s  \r\n",gtHycBak.companyId, gtHyc.companyId);
	
	//保存登录信息
	if(strcmp(gtHycBak.companyId, gtHyc.companyId) != 0 || 
	   strcmp(gtHycBak.userID,gtLogoInfo.userID) != 0 || 
	   strcmp(gtHycBak.userPw,gtLogoInfo.userPw) != 0)
	{
		strcpy(gtHycBak.companyId, gtHyc.companyId);
		strcpy(gtHycBak.userID,gtLogoInfo.userID);
		strcpy(gtHycBak.userPw,gtLogoInfo.userPw);
		
		glbBakVariable_SaveToPersistentArea();
	}
		
	return ret;
}

/*----------------------------------------------------

/*-----------------------------------------------------------------
*函数:Login_Action
*功能:执行登录事件
*参数:pEditID:工号控件
      pEditPW:密码控件
      flag:标志   0:测试账号   1:有登录记录的账号   2:新登录账号
*返回:0:成功  -1:失败
*-----------------------------------------------------------------*/
int Login_Action(tGRAPEDIT *pEditID, tGRAPEDIT *pEditPW, U8 flag)
{
	tGRAPDESKBOX	*pDesk;
	U8				*pId, *pPw;
	int				i,ret = 0,len;
	
	pDesk = (tGRAPDESKBOX *)(pEditID->pParent);
	
	pId = pEditID->edit_privattr.digt;
	pPw = pEditPW->edit_privattr.digt;
	
	if(flag == 0)//测试账号
	{
		Login_TestID(pDesk, pId, pPw);
		gu8LoginFlag = 1;
	}
	else if(flag == 1)//以前有登录
	{
		Login_OldID(pDesk, pId, pPw);
		gu8LoginFlag = 1;
	}
	else if(flag == 2)//新账号
	{
		if(Net_GetNetState() != 1)//正在注册网络
		{
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("正在注册网络,是否离线登录?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				if(0 != Login_OutLine(pDesk, pId, pPw))
				{
					return -1;
				}
				gu8LoginFlag = 1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			//设置事件,等待后台暂停
			if(-1 == Net_ChangeEvt(pDesk, NET_LOGIN_EVT))
			{
				return -1;
			}
			
			ret = Login_NewID(pDesk, pId, pPw);
			if(ret == -1)//失败
			{
				return -1;
			}
			
			gu8LoginFlag = 1;
		}
	}
	
	//清除前台标识
	Net_DelEvt(NET_FOREGROUND_EVT);
	
	//设置当前权限和ID
    RecStore_SetRight(gtLogoInfo.userRight);
    RecStore_SetId(gtLogoInfo.userID);
		
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	//Login_ShowUserInfo_Desk(pDesk);
	App_Main_Desk(pDesk);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	
	gu8LoginFlag = 0;
	
	/*if(strlen(gtHyc.companyId) == 0)
	{
		//找所属公司
		if(1 == Employee_GetCompanyById(pId, gtHyc.companyId, sizeof(gtHyc.companyId)))
		{
			//通过公司编号获取公司名称
			Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
		}
	}*/
	
	memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
	pEditID->view_attr.curFocus = 0;
	pEditPW->view_attr.curFocus = 1;
	
	return ret;
}


int Login_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEditID,*pEditPW;
	U8				lastSelect;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		
		case BUTTON_ID_1://登录
			pEditID = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			pEditPW = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
			
			if(pEditID->view_attr.curFocus == 1 && strlen(pEditPW->edit_privattr.digt) == 0)//如果当前焦点在 工号 上, 则切换焦点项
			{
				if(strlen(pEditID->edit_privattr.digt) == 0)
				{
					break;
				}
				else
				{
					pEditID->view_attr.curFocus = 0;
					pEditPW->view_attr.curFocus = 1;
				}
			}
			else//如果焦点在 密码 上  则执行登录动作
			{
				if(strlen(pEditID->edit_privattr.digt) == 0 || 
			   	   strlen(pEditPW->edit_privattr.digt) == 0)
				{
					break;
				}
				
				hyUsbPrintf("id = %s   pw = %s  \r\n", pEditID->edit_privattr.digt, pEditPW->edit_privattr.digt);
				//是否测试账号
				if(strcmp(pEditID->edit_privattr.digt,ADMIN_ID) == 0 && strcmp(pEditPW->edit_privattr.digt,ADMIN_PW) == 0)
				{
					//管理员
					Login_ClearLoginInfo();
					strcpy(gtLogoInfo.userID,HARDWARE_ID);
					strcpy(gtLogoInfo.userPw,HARDWARE_PW);
					strcpy(gtLogoInfo.userName, gau8UserName);
					gtLogoInfo.userRight = 0;//测试账号
					gtLogoInfo.userType  = 0;//类型
					gtLogoInfo.groupTotal  = 1;
					gtLogoInfo.groupSelect = 0;
					gtLogoInfo.groupDef    = 0;
					memset(gtLogoInfo.pGroup,0,sizeof(tGROUPINFO));
					strcpy(gtLogoInfo.pGroup[0].groupId,"0");
					strcpy(gtLogoInfo.pGroup[0].groupName,"管理员组");
					
					//设置当前权限和ID
				    RecStore_SetRight(gtLogoInfo.userRight);
				    RecStore_SetId(gtLogoInfo.userID);
				    
				    gu8LoginFlag = 0;
					ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
					//Login_ShowUserInfo_Desk(pDesk);
					AdminSettingDesk(pDesk);
					ComWidget_Link2Desk(pDesk,pgtComFirstChd);
					
					memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
					pEditID->view_attr.curFocus = 0;
					pEditPW->view_attr.curFocus = 1;
				}
				else if(strcmp(pEditID->edit_privattr.digt,HARDWARE_ID) == 0 && strcmp(pEditPW->edit_privattr.digt,HARDWARE_PW) == 0)
				{
					//硬件测试账号
					Login_ClearLoginInfo();
					strcpy(gtLogoInfo.userID,HARDWARE_ID);
					strcpy(gtLogoInfo.userPw,HARDWARE_PW);
					strcpy(gtLogoInfo.userName, gau8UserName);
					gtLogoInfo.userRight = 0;//测试账号
					gtLogoInfo.userType  = 0;//类型
					gtLogoInfo.groupTotal  = 1;
					gtLogoInfo.groupSelect = 0;
					gtLogoInfo.groupDef    = 0;
					memset(gtLogoInfo.pGroup,0,sizeof(tGROUPINFO));
					strcpy(gtLogoInfo.pGroup[0].groupId,"0");
					strcpy(gtLogoInfo.pGroup[0].groupName,"硬件测试组");
					
					//设置当前权限和ID
				    RecStore_SetRight(gtLogoInfo.userRight);
				    RecStore_SetId(gtLogoInfo.userID);
					
					gu8LoginFlag = 1;
					ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
					//Login_ShowUserInfo_Desk(pDesk);
					Test_MainDesk(pDesk);
					ComWidget_Link2Desk(pDesk,pgtComFirstChd);
					gu8LoginFlag = 0;
					
					memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
					pEditID->view_attr.curFocus = 0;
					pEditPW->view_attr.curFocus = 1;
				}
				else if(strcmp(pEditID->edit_privattr.digt,OPTEST_ID) == 0 && strcmp(pEditPW->edit_privattr.digt,OPTEST_PW) == 0)
				{
					//业务测试账号
					if(0 == Login_Action(pEditID,pEditPW,0))
					{
						memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
					}
				}
				else if(strcmp(pEditID->edit_privattr.digt,gtLogoInfo.userID) == 0 &&
				        strcmp(pEditPW->edit_privattr.digt,gtLogoInfo.userPw) == 0 && 
				        rawtime(NULL)-gtLogoInfo.landTime < RELAND_TIME)//有登录记录，不用再次验证
				{
					if(0 == Login_Action(pEditID,pEditPW,1))
					{
						memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
					}
				}
				else
				{
					if(0 == Login_Action(pEditID,pEditPW,2))
					{
						memset(pEditPW->edit_privattr.digt,0,pEditPW->edit_privattr.containLen);
					}
					//清除前台标识
					Net_DelEvt(NET_FOREGROUND_EVT);

					pEditID->view_attr.curFocus = 0;
					pEditPW->view_attr.curFocus = 1;
				}
			}
			break;
		case BUTTON_ID_2://联系方式	
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			Contact_Desk(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			break;
		case BUTTON_ID_3://WIFI
			//if(gtHyc.NetSelected != 0)
			{
				lastSelect = gtHyc.NetSelected;
				gtHyc.NetSelected = 0xff;//值为网络切换中
				if(0 == ComWidget_ChangeNetLogo(pDesk, lastSelect, 0))
				{
					gtHyc.NetSelected = 0;
				}
				else
				{
					gtHyc.NetSelected = lastSelect;
				}
			}
			break;
		case BUTTON_ID_4://GSM
			if(gtHyc.NetSelected != 1)
			{
				lastSelect = gtHyc.NetSelected;
				gtHyc.NetSelected = 0xff;//值为网络切换中
				if(0 == ComWidget_ChangeNetLogo(pDesk, lastSelect, 1))
				{
					gtHyc.NetSelected = 1;
				}
				else
				{
					gtHyc.NetSelected = lastSelect;
				}
			}
			break;
		case BUTTON_ID_5://电信网络
			if(gtHyc.UrlSelected != 0)
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否选择电信网络?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
				{
					strcpy(gtHyc.url, "http://120.90.2.126:8099/pdaMsSql_WebService/PDAService.asmx");
				    hyUsbPrintf("电信网络  gtHyc.url = %s\r\n",gtHyc.url);
					gtHyc.UrlSelected = 0;
				}
			}
			break;
		case BUTTON_ID_6://联通网络
			if(gtHyc.UrlSelected != 1)
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否选择联通网络?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
				{
					strcpy(gtHyc.url, "http://120.90.2.126:8099/PDAService/PDAService.asmx");
					hyUsbPrintf("联通网络  gtHyc.url = %s\r\n",gtHyc.url);
					gtHyc.UrlSelected = 1;
				}
			}
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

//edit input end
U32 Login_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPBUTTON		*pBtn;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//工号
	{
		if(strcmp(pEdit->edit_privattr.digt,ADMIN_ID) == 0)
		{
			strcpy(gau8UserName, "管理员账号");
		}
		else if(strcmp(pEdit->edit_privattr.digt,HARDWARE_ID) == 0)
		{
			strcpy(gau8UserName, "硬件测试账号");
		}
		else if(strcmp(pEdit->edit_privattr.digt,OPTEST_ID) == 0)
		{
			strcpy(gau8UserName, "业务测试账号");
	hyUsbPrintf("login  gau8UserName = %s\r\n",gau8UserName);
			//strcpy(gtHyc.companyId, "000000");
			//strcpy(gtHyc.companyName, "测试网点");
		}
		else
		{
			Employee_GetNameById(pEdit->edit_privattr.digt, gau8UserName, sizeof(gau8UserName));
			
			/*memset(gtHyc.companyId, 0, sizeof(gtHyc.companyId));
			memset(gtHyc.companyName, 0, sizeof(gtHyc.companyName));	
			//找所属公司
			if(1 == Employee_GetCompanyById(pEdit->edit_privattr.digt, gtHyc.companyId, sizeof(gtHyc.companyId)))
			{
				//通过公司编号获取公司名称
				Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
			}*/
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_5);
		pBtn->draw(pBtn, 0, 0);
		pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_6);
		pBtn->draw(pBtn, 0, 0);
		pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_7);
		pBtn->draw(pBtn, 0, 0);
	}
	
	return SUCCESS ;
}


S32 Login_WifiGsmBtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;
	U8 id;
	
	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	id = pview_attr->id - BUTTON_ID_3;
	if(id == gtHyc.NetSelected)
	{
		drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
    drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,pview_attr->viewY1+2,pview_attr->viewX1);
	
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
	
	//如果是当前焦点,则在外面画边框
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;
}

//网络选择
S32 Login_NetBtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;
	U8 id;
	
	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	id = pview_attr->id - BUTTON_ID_5;
	if(id == gtHyc.UrlSelected)
	{
		drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
    drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,pview_attr->viewY1+4,pview_attr->viewX1);
	
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
	
	//如果是当前焦点,则在外面画边框
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;
}

//韵达需要拼接url
void Login_FillUrl()
{
	//U8	*dns;
	
	//dns = gtHyc.serverDNS[gtHyc.defaultId-1];
	
	//sprintf(gtHyc.url,"http://%s:%s/ws/ws.jsp",dns,gtHyc.serverPort1);
}


/*-----------------------------------------
*函数: Login_Desk
*功能: 登录页面
*参数: none
*返回: none
*------------------------------------------*/
void Login_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT 		*pEdit;
	U8 i,idx = 0,focus=0;
	
	//strcpy(gtHyc.key, "EA0A7D38D905145D");
	//strcpy(gtHyc.MachineCode,"190000391");
	
	Login_VarInit();
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&Login_DeskAttr[idx++], NULL);
	if(!pDesk)	return ;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->scanEnable= 1;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for(i = 0; i < 9; i++)
	{
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr*)(&Login_DeskAttr[idx++]), (tGrapButtonPriv*)(&Login_BtnAttr[i]));
		if (NULL == pBtn) return ;
		pBtn->pressEnter = Login_BtnEnter;
		if(i == 2 || i == 3)//wifi  gsm
		{
			pBtn->draw = Login_WifiGsmBtnDraw;
		}
		if(i == 4 || i == 5)
		{
			pBtn->draw = Login_NetBtnDraw;
		}
		
	}
	
	for (i=0; i<2; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &Login_DeskAttr[idx++], &Login_EditPri[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if (NULL == pEdit) return ;
		if(i == 0)//账号
		{
			pEdit->editKeyEnd = Login_EditKeyEnd;
			pEdit->view_attr.curFocus = 1;
			//strcpy(pEdit->edit_privattr.digt ,gtHycBak.userID);
			//if(strlen(gtHycBak.userID) > 0)
			//{
				
			//}
		}
		else if(i == 1)//密码
		{
			//if(strlen(gtHycBak.userID) > 0)
			//{
			//	pEdit->view_attr.curFocus = 1;
			//}
		}
		
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}

/*==========================================联系方式=====================================================*/
const tGrapViewAttr ContactViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//背景图片1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{EDIT_ID_1,   20, 40,200, 240,      20,  40,  220,  280,   0,	0,	0,0,1,0,0,0},
};

const tGrapButtonPriv  ContactBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapEdit2Priv  ContactEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,512,1,1,0,0},//显示查询信息
};

int Contact_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}

void Contact_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	idx=0;
		
	pDesk = Grap_CreateDeskbox(&ContactViewAttr[idx++], "联系方式");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&ContactViewAttr[idx++],&ContactBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = Contact_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &ContactViewAttr[idx++], &ContactEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	sprintf(pEdit2->edit_privattr.digt, "浩创信息科技有限公司\r\n电话:021-34290323\r\n地址:上海市大虹桥开发区蟠中路388弄1号4楼\r\n软件版本:%s\r\n巴枪编号:%s", 
					appVersionInfo,gtHyc.MachineCode);
					
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}

/*-----------------------------------------------------------------------------------------------
---------------------------------------用户账号显示界面--------------------------------------
------------------------------------------------------------------------------------------------*/
const tGrapViewAttr gtNameVAttr[] =
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},
	{BUTTON_ID_1,	  10, 50,210,24,  0, 0,0,0,   0, 0,0,0,1,0,0,0},//工号
	{BUTTON_ID_2,	  10, 80,210,24,  0, 0,0,0,   0, 0,0,0,1,0,0,0},//姓名
	{BUTTON_ID_3,	  85,282, 73,34,     85,282, 158,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	
	{EDIT_ID_1,	  	  40, 120,200,104,  0, 32,240,280,   0,0,0,0,1,0,0,0},//
	
	{BUTTON_ID_4,	  85,230,75, 36,   85,230, 157,266, YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//
};

const tGrapButtonPriv gtNameBPri[]=
{
	{NULL,		COLOR_YELLOW,0,FONTSIZE24X24,0,2,0,TY_UNITY_BG1_EN,NULL},
	{"进入系统",0xffff,0,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
	{"升级",	0xffff,0,0,0,2,0,TY_UNITY_BG1_EN,NULL},
};

const tGrapEdit2Priv  gtNameEdit2Attr[] =
{
	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,512,1,1,0,0},
};

/*---------------------------------------------------------------
* function declare
----------------------------------------------------------------*/
/*---------------------------------------------------------
*函数: Login_ShowUserInfo_BtnEnter
*功能: 重载函数
*参数: 
*返回: none
*---------------------------------------------------------*/
S32 Login_ShowUserInfo_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;

	if (state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_3://退出	
			return RETURN_QUIT;
			break;
		case BUTTON_ID_4://升级
			Common_NetUpgrade(3, pDesk);
			return RETURN_REDRAW;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}


/*---------------------------------------------------------
*函数: Login_ShowUserInfo_Desk
*功能: 显示登录人员信息
*参数: 
*返回: 无
*---------------------------------------------------------*/
void Login_ShowUserInfo_Desk(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tGRAPEDIT2      *pEdit2;
	U8              i      ;
	U8              index  ;
	U8				flag=0,notice[50];
	U8   			hycId[18] ;
	
	index = 0 ;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtNameVAttr[index++], "用户信息");
	if (pDesk == NULL)  return ;
	
	Grap_Inherit_Public(pDeskFather,pDesk);
	pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&gtNameVAttr[index++],(tGrapButtonPriv*)&gtNameBPri[0]);
	if (pBttn == NULL)  return ;
	pBttn->btn_privattr.pName = gtLogoInfo.userID ;
	
	pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&gtNameVAttr[index++],(tGrapButtonPriv*)&gtNameBPri[0]);
	if (pBttn == NULL)  return ;
	pBttn->btn_privattr.pName = gtLogoInfo.userName ;
	
	pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&gtNameVAttr[index++],(tGrapButtonPriv*)&gtNameBPri[1]);
	if (pBttn == NULL)  return ;
	pBttn->pressEnter = Login_ShowUserInfo_BtnEnter;
	
	if(gtLogoInfo.userRight != 0)//不是管理员
	{
		memset(hycId,0,18);
	hyUsbPrintf("autDrv_ds28cn01_readSN  \r\n");
		if(HY_OK == autDrv_ds28cn01_readSN(hycId))
		{
			hycId[9] = 0;
		}
	hyUsbPrintf("ver  = %s    %s  \r\n", appVersionInfo+10,  Comm_GetNewestVer());	
		memset(notice, 0, sizeof(notice));
		//当前版本比服务器版本老
		if(strlen(Comm_GetNewestVer()) == 0 || strcmp(Comm_GetNewestVer(), "版本获取失败") == 0)
		{
			strcpy(notice, "获取程序版本失败");
			flag = 0;
		}
		else if(strcmp(appVersionInfo+10,Comm_GetNewestVer()) < 0)
		{
			flag = 1;
			strcpy(notice, "程序版本过旧,请升级.");
		}
		else
		{
			flag = 0;
			strcpy(notice, "程序已是最新版本");
		}
		
		pEdit2  = Grap_InsertEdit2(pDesk, (tGrapViewAttr*)(&gtNameVAttr[index++]),(tGrapEdit2Priv*)(&gtNameEdit2Attr[0]),0xB596);	
		if (NULL == pEdit2)	return;
		sprintf(pEdit2->edit_privattr.digt, "手机号:%s\r\n序列号:%s\r\n本机程序版本:%s\r\n服务器程序版本:%s\r\n%s",
					glbVar_GetSimCode(), hycId, appVersionInfo+10, Comm_GetNewestVer(), notice);
		
		pEdit2->get_default_str(pEdit2);
		
		if(flag == 1)
		{
			pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&gtNameVAttr[index++],(tGrapButtonPriv*)&gtNameBPri[2]);
			if (pBttn == NULL)  return ;
			pBttn->pressEnter = Login_ShowUserInfo_BtnEnter;
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
}

#if 0
//字模放大(注意:目前只支持放大)  目前字库有 16 24 32 字模
void grap_zoom_font(U8 *pSrc, U8 *pDst, U16 srcW, U16 srcH, U16 dstW, U16 dstH, U8 *pWork)
{
	U8		bit,val_s,val_d,multiple;
	S32		i,j,k,m,n,b;                                                                            
	U32     size;
	U8		buf[100],*pTemp;
                                                                                        
	if(srcW == 0 || srcH == 0 || dstW == 0 || dstH == 0)                                    
	{                                                                                       
		return ;                                                                            
	}                                                                                       
	
	pTemp = pWork;
	//宽度
	multiple = dstW/srcW;
	size = (srcW+7)/8 * srcH;//原数据字节数
	val_d = 0;
	b = 7;
	for(i = 0; i < size; i++)
	{
		val_s = pSrc[i];
		for(n = 7; n >= 0; n--)
		{
			bit = (val_s>>n)&0x01;
			
			for(m = 0; m < multiple; m++)
			{
				val_d |= (bit<<b);
				b--;
				if(b < 0)
				{
					*pTemp++ = val_d;
					val_d = 0;
					b = 7;
				}
			}
		}
	}
	
	//高度
	multiple = dstH/srcH;
	n = (dstW+7)/8;
	for(i = 0; i < srcH; i++)
	{
		memcpy(buf, &pWork[i*n], n);
		for(m = 0; m < multiple; m++)
		{
			memcpy(pDst, buf, n);
			pDst += n;
		}
	}
}
#endif

#endif //VER_CONFIG_HYCO