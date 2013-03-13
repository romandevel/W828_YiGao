/*
/*
以下12条设置只在管理员界面中，只有管理员才可以设置。
1.	服务器设置((服务器地址)(服务器端口)(服务器DNS))
2.	SIM卡号
3.	网点编码
4.	巴枪编号 
5.	GPS设置
6.	蓝牙配对
7.	系统设置
8.	网络选择(WIFI(WIFI网络的选择)，GPRS(GPRS接入点的选择))
9.	自动上传设置
10.	打印次数设置
11.	记录查询
12.	出厂预设
13: 磁盘格式化


系统设置每一个普通用户都可以进行设置
系统设置
1.	声音设置
2.	背光设置
3.	触摸屏校正
4.	屏保设置
5.	时间同步
6.	软件升级(软件升级的等待界面做成卷圈圈的等待模式)
7.	使用说明
8.	版本信息(序列号)版本信息中已包含序列号，可以省略序列号设置
9.	巴枪清空
*/
#include "SysSetting.h"





/*---------------------------------------------
*函数:Rec_SearchDesk
*功能:查询记录主界面
*参数:
*返回:
*-----------------------------------------------*/
extern void Rec_SearchDesk(void *pDeskFather);



U32	gu32SettingTemp;
tSETTING_INFO	*gptSetInfo;


//管理员权限配置
const tSETTING_INFO	setting_0[]=//管理员
{
	//显示字符			执行函数
	{"服务器设置",		Set_Url},
	{"SIM卡号",			Set_SIM},
	{"网点编码",		Set_CompanyID},
	{"巴枪编号",		Set_DeviceID},
	//{"GPS设置",			Set_Gps},
	//{"打印次数",		Set_PrintTime},
	{"记录查询",		Rec_SearchDesk},
	{"出厂预设",		Set_FactorySetting},
	{"巴枪清空",		Set_RecClear},
	{"磁盘格式化",		Set_Format},
	{"系统设置",		Set_SysSettingDesk},
	//{"业务选择",		Set_OperationsSelect}
};

//普通员工权限配置
const tSETTING_INFO	setting_1[]=//系统设置
{
	//显示字符			执行函数
	{"声音设置",		Set_Sound},
	{"背光设置",		Set_BackLight},
	//{"触摸屏校正",	Set_ScreenVerify},
	{"屏保设置",		Set_ScreenProtect},
	//{"参数设置",		Set_ParamConfig},
	{"上传设置",		Set_AutoSend},
	{"扫描模式",		Set_ScannerDesk},
	{"接入点设置",		Set_GprsAccess},
	{"工作日设置",		Set_WorkDay},
	{"蓝牙配对",		Set_BT},
	{"WIFI设置",		Set_WIFI},
	{"网络选择",		Set_NetSelect},
	{"时间同步",		Set_UpdateTime},
	{"软件升级",		Set_UpdateSoft},
	{"使用说明",		Set_Instructions},
	{"版本信息",		Set_VersionInfo},
	{"巴枪清空",		Set_RecClear}
};

//权限数组   目前只放了2中权限  如需增加权限,在此处增加即可,并配置其对应的数组
const tSETTING_INFO	*setting_array[]=
{
	setting_0,setting_1
};


/*====================================设置主界面================================*/
const tGrapViewAttr  SysSetViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv   SysSetBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

/*const tGrapListerPriv  SysSetListAttr[]=
{   
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv SysSetSupListAttr[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,20,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

/*int SysSet_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", i+1, gptSetInfo[i].pName);
	//sprintf(str, gptSetInfo[i].pName);
	return SUCCESS;
}

int SysSet_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}*/

int SysSet_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", itemNo+1, gptSetInfo[itemNo].pName);

	return SUCCESS;
}

int SysSet_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}

int SysSet_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
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


/*---------------------------------------------
*函数:Set_SysSettingDesk
*功能:设置主界面
*参数:
*返回:
*-----------------------------------------------*/
int Set_SysSettingDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
	
	gptSetInfo = setting_array[1];
	
	pDesk = Grap_CreateDeskbox(& SysSetViewAttr[idx++], "系统设置");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, & SysSetViewAttr[idx++], &SysSetBtnAttr[0]);
    if(NULL == pBtn ) return -1;
    pBtn->pressEnter =  SysSet_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &SysSetViewAttr[idx++], & SysSetListAttr[0]);
    if(pList == NULL) return -1;
    pList->lst_privattr.totalItem = sizeof(setting_1)/sizeof(tSETTING_INFO);
	pList->enter =  SysSet_ListEnter;
	pList->getListString =  SysSet_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &SysSetViewAttr[idx++], &SysSetSupListAttr[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = SysSet_SupListEnter;
	pSupList->SupListGetStr= SysSet_SupListGetStr;
	pSupList->SupList_priv.totalItem = sizeof(setting_1)/sizeof(tSETTING_INFO);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


/*=============================================管理员设置界面===================================================*/
const tGrapViewAttr  AdminSetViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  AdminSetBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

/*const tGrapListerPriv  AdminSetListAttr[]=
{   
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv AdminSetSupListAttr[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,20,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};


/*int AdminSet_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", i+1, gptSetInfo[i].pName);
	//strcpy(str, gptSetInfo[i].pName);
	return SUCCESS;
}

int AdminSet_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		gptSetInfo = setting_array[0];
	}
	
	return RETURN_REDRAW;
}*/

int AdminSet_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", itemNo+1, gptSetInfo[itemNo].pName);
	return SUCCESS;
}

int AdminSet_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		gptSetInfo = setting_array[0];
	}
	
	return RETURN_REDRAW;
}

int AdminSet_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
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


/*---------------------------------------------
*函数:AdminSettingDesk
*功能:管理员设置主界面
*参数:
*返回:
*-----------------------------------------------*/
void AdminSettingDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
	
	gptSetInfo = setting_array[0];
	
	pDesk = Grap_CreateDeskbox(&AdminSetViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, &AdminSetViewAttr[idx++], &AdminSetBtnAttr[0]);
    if(NULL == pBtn ) return;
    pBtn->pressEnter = AdminSet_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &AdminSetViewAttr[idx++], &AdminSetListAttr[0]);
    if(pList == NULL) return ;
    pList->lst_privattr.totalItem = sizeof(setting_0)/sizeof(tSETTING_INFO);
	pList->enter = AdminSet_ListEnter;
	pList->getListString = AdminSet_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &AdminSetViewAttr[idx++], &AdminSetSupListAttr[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = AdminSet_SupListEnter;
	pSupList->SupListGetStr= AdminSet_SupListGetStr;
	pSupList->SupList_priv.totalItem = sizeof(setting_0)/sizeof(tSETTING_INFO);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}


///////////////////////////////////////////////////////////////

S32 Set_EditDrawInputCnt(tGRAPEDIT *pEdit)
{
	tStrDispInfo disp_info;
	
	if(pEdit->view_attr.curFocus)
	{//显示个数
		U8 buf[8];
		
		Grap_ClearScreen(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H);
		
		memset(buf,0,8);
		sprintf(buf,"%d/%d",strlen(pEdit->edit_privattr.digt),pEdit->edit_privattr.containLen);
		
		disp_info.string = buf;
		disp_info.color  = COLOR_RED;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H, &disp_info);
	}
	
    return SUCCESS;
}

/*---------------------------------------------
*函数:Help_GetInfo
*功能:帮助
*参数:pStr:存放帮助信息  
      limitSize:pStr的大小
      pHelpName:帮助文件名
*返回:
*-----------------------------------------------*/
void Help_GetInfo(U8 *pStr, int limitSize, U8 *pHelpName)
{
	int fp,size;
	char path[100];
	
	sprintf(path, "C:/系统/help/%s.txt", pHelpName);
	fp = AbstractFileSystem_Open(path, AFS_READ);
	if(fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		if(size >= limitSize) size = limitSize-2;
		
		AbstractFileSystem_Seek(fp, 0, AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp, pStr, size);
		AbstractFileSystem_Close(fp);
		
		pStr[size] = 0;
	}
	else
	{
		strcpy(pStr, "对不起,没有使用说明文件.");
	}
	
	return ;
}