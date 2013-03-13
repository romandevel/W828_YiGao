#include "SysSetting.h"



/*===============================WIFI GSM 切换界面===================================*/
const tGrapViewAttr Set_NetSelectAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0x12345678},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*确定*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*确定*/
#endif

	{BUTTON_ID_3,	   0, 80 ,240, 32,   0,0,0,0,   0,0, 0,0,1,0,0,0},//网络切换
	
	{LISTER_ID_1,  		50, 140, 150,60,   		50,  140,200,200,   		0,0,0,0,1,1,1,0},/*list   */
};

const tGrapButtonPriv Set_NetSelectBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"返回",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"返回",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"确定",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
#endif	
	{"网络切换",0xffff,0x0,FONTSIZE32X32,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},
};

const tGrapListerPriv	Set_NetSelectListAttr[]=
{
	{24, 2, 0xffff, COLOR_YELLOW,	0, 2, 0, TY_UNITY_BG1_EN, 0, 70, 4, 10, 0, 50, 50, 1, 0, 0, 1, 0,0},
};

int Set_NetSelect_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPLIST 		*pList;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			pList->enter(pList, pList->lst_privattr.focusItem);
			break;
		case BUTTON_ID_2://返回
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}


S32 Set_NetSelect_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX *pDesk;
	U8			 lastSelect;
	
    pDesk = (tGRAPDESKBOX *)pLister->pParent;

	if(i == 0)//wifi
	{
		lastSelect = gtHyc.NetSelected;
		gtHyc.NetSelected = 0xff;//值为网络切换状态
		if(0 == ComWidget_ChangeNetLogo(pDesk, lastSelect, 0))
		{
			gtHyc.NetSelected = 0;
		}
		else
		{
			gtHyc.NetSelected = lastSelect;
		}
	}
	else if(i == 1)//gprs
	{
		if(gtHyc.NetSelected != 1)
		{
			lastSelect = gtHyc.NetSelected;
			gtHyc.NetSelected = 0xff;//值为网络切换状态
			if(0 == ComWidget_ChangeNetLogo(pDesk, lastSelect, 1))
			{
				gtHyc.NetSelected = 1;
			}
			else
			{
				gtHyc.NetSelected = lastSelect;
			}
		}
	}
   
    return RETURN_QUIT;
}

S32 Set_NetSelect_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if(i == 0)
	{
		strcpy(str, "WIFI");
	}
	else
	{
		strcpy(str, "GPRS");
	}	
	
	return SUCCESS;
}

S32 Set_NetSelect_ListDrawIcon(tGRAPLIST *pLister, U16 i)
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
	
	if (plst_privattr->focusItem == i)
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
    drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+6,x1+40);
	
	return SUCCESS;
}

/*---------------------------------------------
*函数:Set_NetSelect
*功能:网络选择
*参数:
*返回:
*-----------------------------------------------*/
void Set_NetSelect(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST     	*pList;
	int	i,idx=0;
		
	pDesk = Grap_CreateDeskbox(&Set_NetSelectAttr[idx++], "网络选择");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	for(i = 0; i < 3; i ++)
	{
		pBtn = Grap_InsertButton(pDesk,&Set_NetSelectAttr[idx++],&Set_NetSelectBtnAttr[i]);
		if(pBtn == NULL) return ;
		if(i < 2)
		{
			pBtn->pressEnter = Set_NetSelect_BtnPress;
		}
	}
	
	pList =   Grap_InsertLister(pDesk, &Set_NetSelectAttr[idx++], &Set_NetSelectListAttr[0]);
	if (pList == NULL) return;
	pList->getListString          = Set_NetSelect_ListerGetString;
	pList->drawIcon               = Set_NetSelect_ListDrawIcon;
	pList->enter                  = Set_NetSelect_ListEnter;
	pList->lst_privattr.focusItem   = gtHyc.NetSelected;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
