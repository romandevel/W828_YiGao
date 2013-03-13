#include "Test_MainList.h"
#include "drvKeyArray.h"

U8	gu8CallState=0;
U8  gau8Call112Disp[20];
U8	gau8TestFunBuf[50];


void TestFun_Call112(void *pDeskFather);

const tTEST_FUN	gcTestFunList[]=
{
	//显示字符			执行函数
	{"烧录序列号",		TestFun_Serial},
	{"扫描测试",		TestFun_Scan},
	{gau8Call112Disp,	TestFun_Call112},
	{"出厂设置",		TestFun_Factory},
	{"LED测试(红绿蓝)",	TestFun_LED},
	{"震动测试(2秒)",	TestFun_Vibrator},
	{"蜂鸣器测试",		TestFun_Beeper},
	{"蓝牙测试",		TestFun_BT},
	{"同步时间",		TestFun_Time},
	{"GPS测试",			TestFun_GPS},
	{"清空记录",		TestFun_ClearRec},
	{"网络切换测试",	TestFun_NetChange},
	{"拍照测试",		TestFun_DC},
	{"QR码识别测试",	TestFun_QR},
	{"定制码识别",		TestFun_LB},
	{"导出记录",		TestFun_LoadRec},
	//{"一票一件测试",	TestFun_OnePiece},
	//{"一票多件测试",	TestFun_MorePiece},
	//{"记录查询测试",	Rec_SearchDesk},
	//{"删除错扫测试",	Rec_DelErrScanDesk},

};

/*=============================================功能测试界面===================================================*/
const tGrapViewAttr  TestMainViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  TestMainBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapListerPriv  TestMainListAttr[]=
{   
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};


int TestMain_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	//strcpy(str, gcTestFunList[i].pName);
	sprintf(str, "%2d:%s",i+1,gcTestFunList[i].pName);
	return SUCCESS;
}

int TestMain_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	if(gcTestFunList[i].testfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gcTestFunList[i].testfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}


int TestMain_BtnEnter(void *pView, U16 state)
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
*函数:Test_MainDesk
*功能:功能测试主界面
*参数:
*返回:
*-----------------------------------------------*/
void Test_MainDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST		*pList;
	int				i,idx=0;
		
	gu8CallState = 0;
	if(gtHyc.NetSelected == 1)//GSM
	{
		strcpy(gau8Call112Disp, "拨打112");
	}
	else
	{
		strcpy(gau8Call112Disp, "拨打112(不能使用)");
	}
	
	pDesk = Grap_CreateDeskbox(&TestMainViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, &TestMainViewAttr[idx++], &TestMainBtnAttr[0]);
    if(NULL == pBtn ) return;
    pBtn->pressEnter = TestMain_BtnEnter;

    pList = Grap_InsertLister(pDesk, &TestMainViewAttr[idx++], &TestMainListAttr[0]);
    if(pList == NULL) return ;
    pList->lst_privattr.totalItem = sizeof(gcTestFunList)/sizeof(tTEST_FUN);
	pList->enter = TestMain_ListEnter;
	pList->getListString = TestMain_GetListStr;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}


void TestFun_Call112(void *pDeskFather)
{
	if(gtHyc.NetSelected == 1)//GSM
	{
		gsm_dail_test(gu8CallState);
		gu8CallState = !gu8CallState;
		
		if(gu8CallState == 1)
			strcpy(gau8Call112Disp, "112拨号中...");
		else
			strcpy(gau8Call112Disp, "拨打112");
	}
}

/*--------------------------------------------------------------*/
void Test_WriteLog(U8 *pData, int len)
{
	int fp;
	char path[30];
	
	sprintf(path, "C:/%d.bin", rawtime(NULL));
	
	fp = AbstractFileSystem_Open(path, 0);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(path,0);
	}
	fp = AbstractFileSystem_Create(path);
	if(fp >= 0)
	{
		AbstractFileSystem_Write(fp, pData, len);
		AbstractFileSystem_Close(fp);
	}
	
	return ;
}
