#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Struct.h"
#include "Op_Config.h"
#include "drvKeyArray.h"


/*==========================================业务选择界面==============================================*/
const tGrapViewAttr OpSelectSub_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tSupListPriv OpSelectSub_SupListPriv[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,26,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

const tGrapButtonPriv OpSelectSub_BtnPriv[]=
{
	{(U8 *)"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 OpSelectSub_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
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

S32 OpSelectSub_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	sprintf(str, "%d:%s", itemNo+1, gaOp_Config[itemNo+pSupList->view_attr.reserve].pName);
		
	return SUCCESS;
}

S32 OpSelectSub_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX 	*pDesk;
	tSUPPERLIST		*pSupList;
	tOP_CALLBACK	tOpCb;
	int ret;
	
	pSupList = (tSUPPERLIST *)pView;
	ret = Op_Config_GetOpTypeByRight(0, i+pSupList->view_attr.reserve);//跳过上面两个统计
	if(ret != -1)
	{
		pDesk  = (tGRAPDESKBOX *)pSupList->pParent;

		//设置本次业务类型
		OpCom_SetCurOpType((U8)ret);
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		//调用对应的解析显示接口
		memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
		tOpCb.right = 0;
		tOpCb.funType = 0;
		tOpCb.opType = (U8)ret;
		tOpCb.uOpcfg.p = pDesk;
		Op_Config_CallBack(&tOpCb);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
			
	return RETURN_REDRAW;
}


/*---------------------------------------------------------
*函数: Op_OperationSelect_SubDesk
*功能: 业务选择下级界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Op_OperationSelect_SubDesk(void *pDeskFather, U8 startIdx, U8 total)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tSUPPERLIST		*pSupList;
	U8              index  ;
	
	index = 0 ;
	pDesk = Grap_CreateDeskbox(&OpSelectSub_ViewAttr[index++], NULL);
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &OpSelectSub_ViewAttr[index++], &OpSelectSub_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = OpSelectSub_ButtonEnter;
	
	pSupList = Grap_InsertSupList(pDesk, &OpSelectSub_ViewAttr[index++], &OpSelectSub_SupListPriv[0]);
	if (pSupList == NULL) return;
	pSupList->view_attr.reserve = startIdx;
	pSupList->SupListEnter = OpSelectSub_SupListEnter;
	pSupList->SupListGetStr= OpSelectSub_SupListGetStr;
	pSupList->SupList_priv.totalItem = total;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*======================================================================================*/
const U8 * const OpMainStr[]=
{
	"录单操作",	"收件操作",	"发件操作",	"到件操作",	"派件操作","签收操作","问 题 件",
};
const tGrapViewAttr OpSelect_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tGrapListerPriv OpSelect_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv OpSelect_BtnPriv[]=
{
	{(U8 *)"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 OpSelect_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
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


S32 OpSelect_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "%d:%s", i+1, OpMainStr[i]);
		
	return SUCCESS;
}

S32 OpSelect_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk;
	U8 config[7][2]={{1,1},{2,2},{4,3},{7,4},{11,1},{12,1},{13,1}};//起始  总数
	
	pDesk  = (tGRAPDESKBOX *)pLister->pParent;
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Op_OperationSelect_SubDesk(pDesk, config[i][0], config[i][1]);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			
	return RETURN_REDRAW;
}
/*---------------------------------------------------------
*函数: Op_OperationSelect_Desk
*功能: 业务选择主界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Op_OperationSelect_Desk(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tGRAPLIST       *pList ;
	U8              index  ;
	
	index = 0 ;
	pDesk = Grap_CreateDeskbox(&OpSelect_ViewAttr[index++], NULL);
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &OpSelect_ViewAttr[index++], &OpSelect_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = OpSelect_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk, &OpSelect_ViewAttr[index++], &OpSelect_ListPriv[0]);
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = 7;
	pList->getListString = OpSelect_ListGetStr;
	pList->enter         = OpSelect_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}