#include "Rec_Store.h"
#include "Rec_Queue.h"
#include "glbVariable_base.h"
#include "Common_Widget.h"
#include "base.h"
#include "grap_api.h"
#include "Op_Common.h"
#include "Op_Config.h"

/*++++++++++++++++++++++++++++++++++++++++++++业务删除错扫界面++++++++++++++++++++++++++++++++++++++++++++++*/
const tGrapViewAttr Rec_DelErrScanOpViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_3,  10,62,64,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//删除单号
	
	{EDIT_ID_2,   10, 90,210, 186,      10,  90,  220,  276,   0,	0,	0x0,0,1,0,0,0},
	
	{EDIT_ID_1,	  80, 60,133,24,  80, 60,213,84, 0,0,0xffff,0,1,1,1,0},
};

const tGrapButtonPriv  Rec_DelErrScanOpBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//确定
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
#else
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
	{(U8 *)"确定",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//确定
#endif		
	{(U8 *)"删除单号",0xffff,0xffff,0,0,1,0,TY_UNITY_BG1_EN, NULL},//删除单号
};

//edit
const tGrapEditPriv Rec_DelErrScanOpEditPri[] = 
{
	{NULL,0x0,0,0,2,4,16,  1,BARCODE_LEN-1,0,0,2,0,0,1,  50,50,0,  0,1,1,1},
};

//edit2
const tGrapEdit2Priv  Rec_DelErrScanOpEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,100,2,2,0,0},//显示查询信息
};


int Rec_DelErrScanOp_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT2		*pEdit2;
	tGRAPEDIT		*pEdit;
	tGRAPDESKBOX	*pDesk;
	int delCnt;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)(pBtn->pParent);	
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			pEdit2   = (tGRAPEDIT2 *)Grap_GetViewByID(pDesk,EDIT_ID_2);
			pEdit    = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			
			if(strlen(pEdit->edit_privattr.digt) > 0)
			{
				delCnt = RecStore_DelErrorScan(Login_GetRight(), pDesk->view_attr.reserve, pEdit->edit_privattr.digt);
				if(delCnt > 0)
				{
					sprintf(pEdit2->edit_privattr.digt, "删除成功\r\n单号:%s\r\n数量:%d",pEdit->edit_privattr.digt, delCnt);
					pEdit2->get_default_str(pEdit2);
				}
				else
				{
					Com_SpcDlgDeskbox("删除失败,记录不存在或已上传.",6,pDesk,2,NULL,NULL,300);
				}
			}
			break;
		case BUTTON_ID_2://退出
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
*函数:Rec_DelErrScanOpDesk
*功能:指定业务类型删除错扫
*参数:type:类型
*返回:
*-----------------------------------------------*/
void Rec_DelErrScanOpDesk(void *pDeskFather, U16 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	tGRAPEDIT		*pEdit;
	int				i,idx=0,len;
	
	pDesk = Grap_CreateDeskbox(&Rec_DelErrScanOpViewAttr[idx++], "删除错扫");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->view_attr.reserve = type;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	//按钮
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Rec_DelErrScanOpViewAttr[idx++], &Rec_DelErrScanOpBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    if(i < 2)
	    {
	    	pBtn->pressEnter = Rec_DelErrScanOp_BtnEnter;
	    }
    }

	//显示查询信息
	pEdit2 = Grap_InsertEdit2(pDesk, &Rec_DelErrScanOpViewAttr[idx++], &Rec_DelErrScanOpEdit2Attr[0],0);
	if(NULL == pEdit2 ) return;
	
	pEdit = Grap_InsertEdit(pDesk, &Rec_DelErrScanOpViewAttr[idx++], &Rec_DelErrScanOpEditPri[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(NULL == pEdit ) return;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return;
}
/*++++++++++++++++++++++++++++++++++++++++++++删除错扫主界面++++++++++++++++++++++++++++++++++++++++++++++*/
const tGrapViewAttr Rec_DelErrScanViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  Rec_DelErrScanBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

/*const tGrapListerPriv Rec_DelErrScanListAttr[]=
{   
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv Rec_DelErrScanSupListPriv[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,26,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

/*int Rec_DelErrScan_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "% 2d:%s错扫", i+1, gaOp_Config[i+OP_CONFIG_OFFSET].pName);
	
	return SUCCESS;
}

int Rec_DelErrScan_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tOP_CALLBACK	tOpCb;
	int ret;
	
	ret = Op_Config_GetOpTypeByRight(0, i+OP_CONFIG_OFFSET);//跳过统计
	if(ret != -1)
	{
		pDesk = (tGRAPLIST *)pLister->pParent;
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		Rec_DelErrScanOpDesk(pDesk, (U8)ret);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}*/

int Rec_DelErrScan_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	sprintf(str, "% 2d:%s错扫", itemNo+1, gaOp_Config[itemNo+OP_CONFIG_OFFSET].pName);
	
	return SUCCESS;
}

int Rec_DelErrScan_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	tOP_CALLBACK	tOpCb;
	int ret;
	
	ret = Op_Config_GetOpTypeByRight(0, i+OP_CONFIG_OFFSET);//跳过统计
	if(ret != -1)
	{
		pSupList = (tSUPPERLIST *)pView;
		pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		Rec_DelErrScanOpDesk(pDesk, (U8)ret);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}

int Rec_DelErrScan_BtnEnter(void *pView, U16 state)
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
*函数:Rec_DelErrScanDesk
*功能:删除错扫主界面
*参数:
*返回:
*-----------------------------------------------*/
int Rec_DelErrScanDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
			
	pDesk = Grap_CreateDeskbox(&Rec_DelErrScanViewAttr[idx++], NULL);
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, &Rec_DelErrScanViewAttr[idx++], &Rec_DelErrScanBtnAttr[0]);
    if(NULL == pBtn ) return -1;
    pBtn->pressEnter = Rec_DelErrScan_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &Rec_DelErrScanViewAttr[idx++], &Rec_DelErrScanListAttr[0]);
    if(pList == NULL) return -1;
    pList->lst_privattr.totalItem = Op_Config_GetCntByRight(0)-OP_CONFIG_OFFSET;//去掉业绩统计
	pList->enter = Rec_DelErrScan_ListEnter;
	pList->getListString = Rec_DelErrScan_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &Rec_DelErrScanViewAttr[idx++], &Rec_DelErrScanSupListPriv[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = Rec_DelErrScan_SupListEnter;
	pSupList->SupListGetStr= Rec_DelErrScan_SupListGetStr;
	pSupList->SupList_priv.totalItem = Op_Config_GetCntByRight(0)-OP_CONFIG_OFFSET;//去掉业绩统计
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
