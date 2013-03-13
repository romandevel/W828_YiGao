#include "SysSetting.h"


U32 srceenpro[]={cSCREENPROTECT_DEFAULT,cSCREENPROTECT_1MIN,cSCREENPROTECT_5MIN, cSCREENPROTECT_10MIN,cSCREENPROTECT_CLOSE};

/*===============================================================================================*/

const tGrapViewAttr    gtViewSetScreenAttr[] = 
{	
	{COMM_DESKBOX_ID, 	0,0, 240,320,  	0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},/*背景   */
#ifdef W818C
	{BUTTON_ID_1,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
	{BUTTON_ID_2,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
#else
	{BUTTON_ID_2,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
	{BUTTON_ID_1,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
#endif	
	{LISTER_ID_1,  		50, 54, 150,200,    50, 54,200,254,          0,0,0,0,1,1,1,0},/*list   */
};

const tGrapButtonPriv gtButnSetScreenAttr []=
{
#ifdef W818C
	{(U8*)"确定",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
	{(U8*)"返回",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
#else
	{(U8*)"返回",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
	{(U8*)"确定",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
#endif
};

const tGrapListerPriv	gtListSetScreenAttr[]=
{
    {29,5,0xffff,COLOR_YELLOW,	0,5,0,	TY_UNITY_BG1_EN,0,30,3,10,0,50,50,1,0,0, 2, 0,0},
};


/*------------------------------------------------------
* 函数: SetProtocol_BtnEnter
* 功能: 屏保设置
* 参数: 
* 返回: 0
-------------------------------------------------------*/
int SetProtocol_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPLIST		*pList;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确认
			pDesk = (tGRAPDESKBOX *)pBtn->pParent;
			
			if(gtHyc.ScreenProtect != gu32SettingTemp)
			{
				gtHyc.ScreenProtect = gu32SettingTemp;
				glbVariable_SaveParameter();
			}
    
    		Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,100);
			return RETURN_QUIT;
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

int SetProtocol_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	
	switch(i)
	{
	case 0:
		strcpy(str, "30秒");
		break;
	case 1:
		strcpy(str, "60秒");
		break;
	case 2:
		strcpy(str, "5分");
		break;
	case 3:
		strcpy(str, "10分");
		break;
	case 4:
		strcpy(str, "关闭");
		break;
	default:
		break;
	}
	
	return SUCCESS;
}

int SetProtocol_ListEnter(tGRAPLIST *pLister, U16 i)
{
	
	gu32SettingTemp = srceenpro[i];
	pLister->draw(pLister,0,0);
	
	return SUCCESS;
}

/****************************************************************
*函数:SetProtocol_ListDrawIcon
*功能:list 画前面的点选图
*参数:
*返回:
****************************************************************/
static S32 SetProtocol_ListDrawIcon(tGRAPLIST *pLister, U16 i)
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
	
	if (gu32SettingTemp == srceenpro[i])
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
	drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+8,x1+20);
	
	return SUCCESS;
}

/*---------------------------------------------
*函数:Set_ScreenProtect
*功能:屏保设置
*参数:
*返回:
*-----------------------------------------------*/
void Set_ScreenProtect(void *pDeskFather)
{
	tGRAPDESKBOX   *pDesk;
	tGRAPBUTTON    *pBtn;
	tGRAPLIST      *pList;
	U8             i;
	int idex=0;
	
	gu32SettingTemp = gtHyc.ScreenProtect;
	
	pDesk = Grap_CreateDeskbox(&gtViewSetScreenAttr[idex++], "屏保设置");
	if (NULL == pDesk ) return;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	/* 确定,返回 */
	for(i = 0; i < 2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &gtViewSetScreenAttr[idex++],&gtButnSetScreenAttr[i]);
		if (NULL == pBtn) return;
		pBtn->pressEnter = SetProtocol_BtnEnter;
	}
	
	pList =   Grap_InsertLister(pDesk, &gtViewSetScreenAttr[idex++],&gtListSetScreenAttr[0]);
	if (pList == NULL) return;
	pList->getListString = SetProtocol_GetListStr;
	pList->drawIcon      = SetProtocol_ListDrawIcon;
	pList->enter         = SetProtocol_ListEnter;
	
	for(i = 0; i < 5; i++)
	{
		if(gu32SettingTemp == srceenpro[i])
		{
			pList->lst_privattr.focusItem = i;
			break;
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
}
