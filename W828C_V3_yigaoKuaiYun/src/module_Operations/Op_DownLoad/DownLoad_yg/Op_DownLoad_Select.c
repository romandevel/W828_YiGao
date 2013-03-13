#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_DownLoad.h"
#include "drvKeyArray.h"

extern int Op_DownLoad_Company_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Employee_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Problem_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Leave_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_WeiVol_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Class_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_DestAddr_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Flight_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Car_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Area(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Area_Desk(void *pDeskFather, U8 ifSearch);
extern int Op_DownLoad_Abnormity_Desk(void *pDeskFather, U8 ifSearch);

tDOWNLOAD_CFG	download_info[]=
{
	//显示名称				//执行函数
	{"网点列表",			Op_DownLoad_Company_Desk},//公司列表
	{"员工列表",			Op_DownLoad_Employee_Desk},//人员列表
	{"问题原因列表",		Op_DownLoad_Problem_Desk},//问题原因列表
	{"留仓原因列表",		Op_DownLoad_Leave_Desk},//留仓原因列表
	{"运输方式列表",		Op_DownLoad_Class_Desk},//运输方式列表更新
	{"目的地列表",			Op_DownLoad_DestAddr_Desk},//目的地列表更新
	{"航空信息列表",		Op_DownLoad_Flight_Desk},//航空信息表更新
	{"车辆信息列表",		Op_DownLoad_Car_Desk},//车辆信息表更新
	{"收派区域更新",		Op_DownLoad_Area},//收派区域更新
	//{"异常类型更新",		Op_DownLoad_Abnormity_Desk},//异常件类型更新
};



U16	*gpu16SearchInfo;


/*------------------------------------------
* 函数:Op_DownLoad_Init
* 功能:资料下载模块信息初始化
* 参数:none
* 返回:none
-------------------------------------------*/
void Op_DownLoad_Init()
{	
	Op_Company_Init();
	Op_Employee_Init();
	Op_Problem_Init();
	Op_Leave_Init();
	Op_Class_Init();
	Op_Flight_Init();
	Op_Car_Init();
	Op_DestAddr_Init();
	//Op_WeiVol_Init();
	//Op_Abnormity_Init();
	gpu16SearchInfo = NULL;
}

/**************************************************************************************************
									资料下载选择界面
**************************************************************************************************/
const tGrapViewAttr Op_InfoDownLoad_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapListerPriv Op_InfoDownLoad_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv Op_InfoDownLoad_BtnPriv[]=
{
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};


S32 OpInfoDownLoad_ButtonEnter(void *pView, U16 state)
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


S32 OpInfoDownLoad_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	
	pDesk  = (tGRAPDESKBOX *)pLister->pParent;
	if(download_info[i].fun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		download_info[i].fun(pDesk, 0);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
			
	return RETURN_REDRAW;
}

S32 OpInfoDownLoad_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	//strcpy(str, download_info[i].pName);
	sprintf(str, "%2d:%s",i+1,download_info[i].pName);
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: Op_DownLoadSelect_Desk
*功能: 资料下载选择界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Op_DownLoadSelect_Desk(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tGRAPLIST       *pList ;
	U8              index  ;
	int 			i;
	
	index = 0 ;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&Op_InfoDownLoad_ViewAttr[index++], NULL);
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&Op_InfoDownLoad_ViewAttr[index++],(tGrapButtonPriv*)&Op_InfoDownLoad_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = OpInfoDownLoad_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk,(tGrapViewAttr*)(&Op_InfoDownLoad_ViewAttr[index++]),(tGrapListerPriv*)(&Op_InfoDownLoad_ListPriv[0]));
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = sizeof(download_info)/sizeof(tDOWNLOAD_CFG);
	pList->getListString = OpInfoDownLoad_ListGetStr;
	pList->enter         = OpInfoDownLoad_ListEnter ;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}