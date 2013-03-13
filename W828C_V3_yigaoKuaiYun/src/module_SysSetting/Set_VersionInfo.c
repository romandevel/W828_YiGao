#include "SysSetting.h"


const tGrapViewAttr VersionViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//背景图片1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,0,0},
};

const tGrapButtonPriv  VersionBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapEdit2Priv  VersionEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,512,1,1,0,0},//显示查询信息
};

int Version_BtnPress(void *pView, U16 state)
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
*函数:Set_VersionInfo
*功能:版本信息
*参数:
*返回:
*-----------------------------------------------*/
void Set_VersionInfo(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	idx=0;
		
	pDesk = Grap_CreateDeskbox(&VersionViewAttr[idx++], "版本信息");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&VersionViewAttr[idx++],&VersionBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = Version_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &VersionViewAttr[idx++], &VersionEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	sprintf(pEdit2->edit_privattr.digt, "软件版本:\r\n%s\r\n巴枪编号:%s", appVersionInfo_all,gtHyc.MachineCode);
					
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
