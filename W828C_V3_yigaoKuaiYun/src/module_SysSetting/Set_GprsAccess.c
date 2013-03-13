#include "SysSetting.h"

/*============================================================================================*/
const tGrapViewAttr    gtViewSetGprsAccessAttr[] = 
{	
	{COMM_DESKBOX_ID,	0, 0, 240,320,			0, 0, 0, 0,     		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 1,0,0,0},/*背景   */
#ifdef W818C
	{BUTTON_ID_1,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
	{BUTTON_ID_2,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
#else
	{BUTTON_ID_2,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/
	{BUTTON_ID_1,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/	
#endif	
	{LISTER_ID_1,  		50, 54, 150,200,   		50,  54,200,254,   		0,0,0,0,1,1,1,0},/*list   */
};

const tGrapButtonPriv gtButnSetGprsAccessAttr[]=
{
#ifdef W818C
	{(U8*)"确定",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},
	{(U8*)"返回",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
#else
	{(U8*)"返回",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
	{(U8*)"确定",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},
#endif
};

const tGrapListerPriv	gtListSetGprsAccessAttr[]=
{
	{24, 4, 0xffff, COLOR_YELLOW,	0, 4, 0, TY_UNITY_BG1_EN, 0, 50, 4, 10, 0, 50, 50, 1, 0, 0, 1, 0,0},
};


/*
 * 确定,返回
 */
static S32 SetGprsAccess_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	U32             id;
	S32             rv = SUCCESS;
	
	pBtn 	= (tGRAPBUTTON *)pView;		
	id		= pBtn->view_attr.id;

	if (state == STATE_NORMAL)
	{
		switch( id )
		{
		case BUTTON_ID_1:/* 确定 */
			pDesk =  (tGRAPDESKBOX *)pBtn->pParent;

			if(gtHyc.GprsAccess != gu32SettingTemp)
			{
				gtHyc.GprsAccess = gu32SettingTemp;
				hyIE_clearDns();
				Net_AddEvt(NET_CLOSEPPP_EVT);
				glbVariable_SaveParameter();
			}
			
			Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,100);
			return RETURN_QUIT;
			break;
		case BUTTON_ID_2:/* 返回 */
			rv = RETURN_QUIT;				
			break;
		}	
	}
	
	return rv ;	
}


/**
 * lister 获得列表字符
 */
static S32 SetGprsAccess_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	switch(i)
	{
	case 0:
		strcpy(str, "cmnet");
		break;
	case 1:
		strcpy(str, "cmwap");
		break;
	case 2:
		strcpy(str, "uninet");
		break;
	case 3:
		strcpy(str, "uniwap");
		break;
	default:
		break;
	}
	
	return SUCCESS;
}
														
/*														
 * 点击lister各项										
 */
static S32 SetGprsAccess_ListEnter(tGRAPLIST *pLister, U16 i)
{
	gu32SettingTemp = i;
	pLister->draw(pLister,0,0);
	
	return SUCCESS;
}


/****************************************************************
*函数:SetGprsAccess_ListDrawIcon
*功能:list 画前面的点选图
*参数:
*返回:
****************************************************************/
static S32 SetGprsAccess_ListDrawIcon(tGRAPLIST *pLister, U16 i)
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
	
	if (gu32SettingTemp == i)
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
	drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+6,x1+20);
	
	return SUCCESS;
}

/*---------------------------------------------
*函数:Set_GprsAccess
*功能:GPRS接入点设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_GprsAccess(void *pDeskFather)
{
	tGRAPDESKBOX   *pDesk;
	tGRAPBUTTON    *pBtn;
	tGRAPLABEL     *pLabel;
	tGRAPLIST      *pList;
	U8             i;
	
	gu32SettingTemp = gtHyc.GprsAccess;
	pDesk = Grap_CreateDeskbox(&gtViewSetGprsAccessAttr[0], "接入点设置");
	if (NULL == pDesk ) return;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	/* 确定,返回 */
	for(i = 0; i < 2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &gtViewSetGprsAccessAttr[i+1], &gtButnSetGprsAccessAttr[i]);
		if (NULL == pBtn) return;
		pBtn->pressEnter = SetGprsAccess_BtnEnter;
	}
	
	pList =   Grap_InsertLister(pDesk, &gtViewSetGprsAccessAttr[3], &gtListSetGprsAccessAttr[0]);
	if (pList == NULL) return;
	pList->getListString          = SetGprsAccess_ListerGetString;
	pList->drawIcon               = SetGprsAccess_ListDrawIcon;
	pList->enter                  = SetGprsAccess_ListEnter;
	pList->lst_privattr.focusItem = gtHyc.GprsAccess; 

	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return;
}
