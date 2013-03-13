#include "SysSetting.h"


const tGrapViewAttr InstructionsViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//背景图片1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,1,0},
};

const tGrapButtonPriv  InstructionsBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapEdit2Priv  InstructionsEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,2048,1,1,0,0},//显示
};

int Instructions_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
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
*函数:Set_Instructions
*功能:使用说明
*参数:
*返回:
*-----------------------------------------------*/
void Set_Instructions(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	idx=0;
		
	pDesk = Grap_CreateDeskbox(&InstructionsViewAttr[idx++], "使用说明");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&InstructionsViewAttr[idx++],&InstructionsBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = Instructions_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &InstructionsViewAttr[idx++], &InstructionsEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	Help_GetInfo(pEdit2->edit_privattr.digt, pEdit2->edit_privattr.containLen, "使用说明");
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
