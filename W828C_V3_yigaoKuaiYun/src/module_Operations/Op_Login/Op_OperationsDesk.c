#include "Op_Common.h"


extern int Op_OperationSelect_Desk(void *pDeskFather);
extern int Rec_SearchDesk(void *pDeskFather);
extern int Rec_DelErrScanDesk(void *pDeskFather);
extern int Op_DownLoadSelect_Desk(void *pDeskFather);
extern int Express_InquireDesk(void *pDeskFather);
extern int Delivery_InquireArea(void *pDeskFather);
extern int Set_SysSettingDesk(void *pDeskFather);

extern int Operation_YGLuDan_Desk(void *pDeskFather);
extern void App_SJMain_Desk(void *pDeskFather);
extern void App_FJMain_Desk(void *pDeskFather);
extern void App_DJMain_Desk(void *pDeskFather);
extern int Operation_YGPaiJian_Desk(void *pDeskFather);
extern int Operation_YGQianShou_Desk(void *pDeskFather);
extern int Operation_YGWenTi_Desk(void *pDeskFather);

/*====================================================================*/
typedef int (*YWMainSelectFun)(void *pView);


typedef struct _download_cfg_t
{
	U8				*pName;	//显示名称
	YWMainSelectFun	fun;	//执行函数
	
}tYWMAINSEL_CFG;



tYWMAINSEL_CFG	YWmainselect_info[]=
{
	//显示名称				//执行函数
	{"录单操作",			Operation_YGLuDan_Desk},
	{"收件操作",			App_SJMain_Desk},
	{"发件操作",			App_FJMain_Desk},
	{"到件操作",			App_DJMain_Desk},
	{"派件操作",			Operation_YGPaiJian_Desk},
	{"签收操作",			Operation_YGQianShou_Desk},
	{"问 题 件",			Operation_YGWenTi_Desk},
};


/**************************************************************************************************
									主界面
**************************************************************************************************/
const tGrapViewAttr YWMainDesk_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//退出
	
	{LISTER_ID_1,	10,40,224,220,	10,40,234,260,	0,0,0,0,1,1,1,0},
	
	{BUTTON_ID_2,  10,265,220,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//
};

/*const tGrapListerPriv YWMainDesk_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv YWMainDesk_SupListPriv[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,26,0,50,50,FONTSIZE8X16,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

const tGrapButtonPriv YWMainDesk_BtnPriv[]=
{
	{(U8 *)"退出",0xffff,0x0000,0,0,2,KEYEVENT_ESC, 0,0},
	
	{NULL,COLOR_YELLOW,COLOR_YELLOW,0,0,2,0, 0,0},
};


S32 YWMainDesk_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	tGRAPDESKBOX 	*pDesk;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ESC
		
		return RETURN_QUIT;
		
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

/*
S32 MainDesk_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	//strcpy(str, mainselect_info[i].pName);
	sprintf(str, "%2d:%s",i+1,mainselect_info[i].pName);
	return SUCCESS;
}


S32 MainDesk_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	
	pDesk  = (tGRAPDESKBOX *)pLister->pParent;
	if(mainselect_info[i].fun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		mainselect_info[i].fun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
			
	return RETURN_REDRAW;
}*/

S32 YWMainDesk_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	//strcpy(str, mainselect_info[i].pName);
	sprintf(str, "%2d:%s",itemNo+1,YWmainselect_info[itemNo].pName);
	
	return SUCCESS;
}


S32 YWMainDesk_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
	if(YWmainselect_info[i].fun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		YWmainselect_info[i].fun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
			
	return RETURN_REDRAW;
}

/*---------------------------------------------------------
*函数: App_YWMain_Desk
*功能: 主界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
void App_YWMain_Desk(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tSUPPERLIST		*pSupList;
	U8              index= 0;
	int 			i;
		
	pDesk = Grap_CreateDeskbox(&YWMainDesk_ViewAttr[index++], NULL);
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &YWMainDesk_ViewAttr[index++], &YWMainDesk_BtnPriv[0]);
	if (pBttn == NULL)  return ;
	pBttn->pressEnter = YWMainDesk_ButtonEnter;
	
	/*pList = Grap_InsertLister(pDesk, &MainDesk_ViewAttr[index++], &MainDesk_ListPriv[0]);
	if (pList == NULL) return;
	pList->lst_privattr.totalItem = sizeof(mainselect_info)/sizeof(tMAINSEL_CFG);
	pList->getListString = MainDesk_ListGetStr;
	pList->enter         = MainDesk_ListEnter;*/
	
	pSupList = Grap_InsertSupList(pDesk, &YWMainDesk_ViewAttr[index++], &YWMainDesk_SupListPriv[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = YWMainDesk_SupListEnter;
	pSupList->SupListGetStr= YWMainDesk_SupListGetStr;
	pSupList->SupList_priv.totalItem = sizeof(YWmainselect_info)/sizeof(tYWMAINSEL_CFG);
	
	pBttn = Grap_InsertButton(pDesk, &YWMainDesk_ViewAttr[index++], &YWMainDesk_BtnPriv[1]);
	if (pBttn == NULL)  return ;
	pBttn->btn_privattr.pName = Login_GetName();
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
}