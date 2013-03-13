

/*****************************************************************************
				各业务操作时，底部显示当前操作的单号
*****************************************************************************/

#include "Op_Common.h"
#include "base.h"
#include "grap_lister.h"
#include "grap_deskbox.h"
#include "grap_api.h"
#include "Rec_Info.h"
#include "Op_Types.h"

tGRAPLIST	*gpListShow;	//显示已操作的记录   直接使用exist里面的数据
int OpListShowNum;//单号总数

const tGrapViewAttr Op_ListShow_ViewAttr[] =
{
	{LISTER_ID_20,         0,0,0,0,    0,0,0,0,	0, 0,  0, 0, 1,0,0, 0},
};

const tGrapListerPriv Op_ListShow_ListPriv[]=
{
	{20,0,0xffff,COLOR_RED,0,0,0, TY_UNITY_BG1_EN,0,    10,2,0,0,50,50,1,0,0,  0,0},
};


S32 Op_ListShow_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	tEXIST_CHECK *pExist;
//hyUsbPrintf("getstr---------------------------\r\n");	
	//strcpy(str, pOpListShow[i].barcode);
	pExist = Rec_GetExistCodeByIdx(i);
	if(pExist != NULL)
	{
		//strcpy(str, pExist->code);
		sprintf(str, "%s  %s", pExist->code, pExist->other);
	}
	else
	{
		str[0] = 0;
		//strcpy(str, "fail");
	}
	
	return SUCCESS;
}

S32 Op_ListShow_ListerDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLIST *pLister;
	U16 i;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1-20, (U16)(pview_attr->viewX1+pview_attr->viewWidth), 
		(U16)(pview_attr->viewY1+pview_attr->viewHeight));

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
	//hyUsbPrintf("topItem  = %d  i = %d   existcnt  = %d  \r\n", plst_privattr->topItem, i, Rec_GetExistCnt());
		if(plst_privattr->topItem + i >= Rec_GetExistCnt())
			break;
		pLister->updateList(pLister, (U16)(plst_privattr->topItem + i));
	}
	
	drvLcd_SetColor(0xFFFF,0x0000);
	drvLcd_Rectangle(pview_attr->viewY1-21,pview_attr->viewX1-1,pview_attr->viewHeight+22,pview_attr->viewWidth+2,1);
	
	{//显示票数
		U8 buf[20];
		tStrDispInfo disp_info;
		
		memset(buf,0,20);
		sprintf(buf,"%d票",OpListShowNum);
		
		disp_info.string = buf;
		disp_info.color = COLOR_YELLOW;
		disp_info.font_size = plst_privattr->font_size;
		disp_info.font_type = plst_privattr->font_type;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1-20, 
					pview_attr->viewX1+pview_attr->viewWidth,pview_attr->viewY1, &disp_info);
	}
	
	return SUCCESS;

}


/*
20个像素显示当前的票数
*/
tGRAPLIST *Op_CommonInsertListShow(tGRAPDESKBOX *pDesk, U16 statrX, U16 startY, U16 width, U16 height)
{	
	if (pDesk==NULL || statrX<0 || startY<0 || width<0 || height<0)
	{
		return NULL;
	}
	
	if(Rec_GetExistCnt() == 0)
	{
		OpListShowNum = 0;
	}
	
	gpListShow = Grap_InsertLister(pDesk,&Op_ListShow_ViewAttr[0],&Op_ListShow_ListPriv[0]);
	if (gpListShow == NULL) return;
	
	gpListShow->view_attr.viewX1 = statrX;
	gpListShow->view_attr.viewY1 = startY+20;
	gpListShow->view_attr.viewWidth = width;
	gpListShow->view_attr.viewHeight = height-20;
	
	gpListShow->view_attr.touchX1 = statrX;
	gpListShow->view_attr.touchY1 = startY+20;
	gpListShow->view_attr.touchX2 = statrX+width;
	gpListShow->view_attr.touchY2 = startY+height;
		
	gpListShow->lst_privattr.totalItem = Rec_GetExistCnt();
	gpListShow->lst_privattr.lstNum = (gpListShow->view_attr.viewHeight / gpListShow->lst_privattr.lstHeight);
	gpListShow->lst_privattr.x_start= statrX+5;
	gpListShow->lst_privattr.curLen = (width / 8 - 2);
	
	gpListShow->getListString = Op_ListShow_ListGetStr;
	gpListShow->draw = Op_ListShow_ListerDraw;
	
	return gpListShow;
}

tGRAPLIST *Op_CommonInsertListShow_noclear(tGRAPDESKBOX *pDesk, U16 statrX, U16 startY, U16 width, U16 height)
{	
	if (pDesk==NULL || statrX<0 || startY<0 || width<0 || height<0)
	{
		return NULL;
	}
	
	gpListShow = Grap_InsertLister(pDesk,&Op_ListShow_ViewAttr[0],&Op_ListShow_ListPriv[0]);
	if (gpListShow == NULL) return;
	
	gpListShow->view_attr.viewX1 = statrX;
	gpListShow->view_attr.viewY1 = startY+20;
	gpListShow->view_attr.viewWidth = width;
	gpListShow->view_attr.viewHeight = height-20;
	
	gpListShow->view_attr.touchX1 = statrX;
	gpListShow->view_attr.touchY1 = startY;
	gpListShow->view_attr.touchX2 = statrX+width;
	gpListShow->view_attr.touchY2 = startY+height;
		
	gpListShow->lst_privattr.totalItem = Rec_GetExistCnt();
	gpListShow->lst_privattr.lstNum = (gpListShow->view_attr.viewHeight / gpListShow->lst_privattr.lstHeight);
	gpListShow->lst_privattr.x_start= statrX+5;
	gpListShow->lst_privattr.curLen = (width / 8 - 2);
	
	gpListShow->getListString = Op_ListShow_ListGetStr;
	gpListShow->draw = Op_ListShow_ListerDraw;
	
	return gpListShow;
}

void Op_CommonList_Clear()
{
	OpListShowNum = 0;
	Rec_ClearExist();
	gpListShow->lst_privattr.totalItem = 0;
}

void Op_CommonList_SetTotal(int total)
{
	Rec_ClearExist();
	OpListShowNum = total;
	gpListShow->lst_privattr.totalItem = OpListShowNum;
}

/*
添加一条单号
return 0:success   other:fail
*/
int Op_CommonListShow_Add(U8 *pCode)
{
	if (pCode == NULL || gpListShow == NULL) return 1;
	
	OpListShowNum++;
	gpListShow->lst_privattr.totalItem = Rec_GetExistCnt();
	gpListShow->lst_privattr.focusItem = 0;
	gpListShow->lst_privattr.topItem = 0;
	
	return 0;
}


/*
删除一条单号
return 0:success   other:fail
*/
int Op_CommonListShow_Delete()
{
	if (OpListShowNum <= 0 || gpListShow == NULL) return 2;
	
	OpListShowNum--;
	gpListShow->lst_privattr.totalItem = Rec_GetExistCnt();
	
	return 0;
}



tGRAPLIST *Op_CommonGetListShowWeight(tGRAPDESKBOX *pDesk)
{
	return gpListShow;
}

void Op_CommonGetCode(U8 *code)
{
	if(gpListShow != NULL)
		strcpy(code, Rec_GetExistCodeByIdx(gpListShow->lst_privattr.focusItem));
}

/*--------------------------------------
*函数：Op_ComCheckExist
*功能：检查单号是否已经存在
*参数：pCode:带检查的条码
       opType:操作类型
*返回：0:已存在,未删除    1:已存在,删除    -1:不存在
*--------------------------------------*/
int Op_ComCheckExist(tGRAPDESKBOX *pDesk, U8 *pCode, U16 opType)
{
	int ret = -1;
	U8  tempBuf[50];
	
	//检查单号是否存在
	if (Rec_CheckExist(pCode) == 0)
	{
		sprintf(tempBuf, "%s已存在,是否删除?", pCode);
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox(tempBuf,6,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
		{
			ret = RecStore_DelErrorScan(Login_GetRight(), opType, pCode);
			//hyUsbPrintf("delerrscan ret = %d  \r\n", ret);
			if (ret > 0)
			{
				Rec_DelExist(pCode);
				/*if(opType == HYCO_OP_QS)//不在这里执行  到所属业务中执行
				{
					PicStore_Delete(pCode);
				}*/
				Op_CommonListShow_Delete();
				
				Com_SpcDlgDeskbox("删除成功!",0,pDesk,1,NULL,NULL,100);
				
				ret = 1;
			}
			else if (ret == -1)
			{
				Com_SpcDlgDeskbox("记录删除失败!",0,pDesk,1,NULL,NULL,150);
				ret = 0;
			}
			else if (ret == -2)
			{
				Com_SpcDlgDeskbox("记录正在上传,无法删除!",0,pDesk,1,NULL,NULL,150);
				ret = 0;
			}
			else if (ret == 0)
			{
				Rec_DelExist(pCode);
				Op_CommonListShow_Delete();
				Com_SpcDlgDeskbox("记录已上传,只能从巴枪删除!",0,pDesk,1,NULL,NULL,150);
				ret = 0;
			}
		}
		else
		{
			ret = 0;
		}
		//pCode[0] = 0;
	}
	
	return ret;
}


int Op_ComDelCode(tGRAPDESKBOX *pDesk, U16 type)
{
	U8 tempBuf[40];
	int ret = 0;
	
	if(OpListShowNum <= 0 || Rec_GetExistCnt() == 0)
	{
		return 0;
	}
	
	if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("是否删除该票面单?",6, pDesk, 3, NULL, NULL, 3000))
	{
		return ret;
	}

	memset(tempBuf, 0, sizeof(tempBuf));
	Op_CommonGetCode(tempBuf);
					
	ret = RecStore_DelErrorScan(Login_GetRight(), type, tempBuf);
	hyUsbPrintf("del code = %s   ret = %d \r\n", tempBuf, ret);
	if (ret > 0)
	{
		Rec_DelExist(tempBuf);
		/*if(type == HYCO_OP_QS)//不再这里执行,到所属业务中执行
		{
			PicStore_Delete(tempBuf);
		}*/
		Op_CommonListShow_Delete();
		Com_SpcDlgDeskbox("删除成功!",0,pDesk,1,NULL,NULL,100);
	}
	else if (ret == -1)
	{
		Com_SpcDlgDeskbox("记录删除失败!",0,pDesk,1,NULL,NULL,150);
	}
	else if (ret == -2)
	{
		Com_SpcDlgDeskbox("记录正在上传,无法删除!",0,pDesk,1,NULL,NULL,150);
	}
	else if (ret == 0)
	{
		Rec_DelExist(tempBuf);
		Op_CommonListShow_Delete();
		Com_SpcDlgDeskbox("记录已上传,只能从巴枪删除!",0,pDesk,1,NULL,NULL,150);
	}
	
	return ret;
}