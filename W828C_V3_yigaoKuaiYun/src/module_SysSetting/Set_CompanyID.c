#include "SysSetting.h"


U8 gau8CompanyName[80];

/*控件*/
const tGrapViewAttr Set_CompanyAtrr[]=
{
	{COMM_DESKBOX_ID,0,  0, 240, 320, 0,0,0,0,         	  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
#endif	
	{BUTTON_ID_3,	0, 110 ,240, 16,   0,0,0,0,   0,0,0,0,1,0,0,0},//网点名称
	{BUTTON_ID_4,	10, 134 ,64, 16,   0,0,0,0,   0,0,0,0,1,0,0,0},//网店编号
	
	{EDIT_ID_1,		80,130, 124,24,     80,130,204,154,   0,0,0xffff,0,1,1,1,0},/*输入文本框*/				
	
	{EDIT_ID_2,   10, 175,210, 100,      10,  175,  220,  275,   0,	0,	0,0,1,0,0,0},			
};

/*btn 属性*/
const tGrapButtonPriv Set_CompanyBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,            TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,            TY_UNITY_BG1_EN,NULL},
	{(U8 *)"确定", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
#endif	
	{gau8CompanyName, 0xffff,0x0,0,0,2,0, TY_UNITY_BG1_EN,NULL},
	{(U8 *)"网点编号", 0xffff,0x0,0,0,2,0, TY_UNITY_BG1_EN,NULL},
};

/*edt 属性*/
const tGrapEditPriv	  Set_CompanyEditAttr []=
{
	{NULL,0x0,0,0,2,6,12,  1,20,0,0,2,0,0,1,  50,50,0,  0,1,1,1},
};

const tGrapEdit2Priv  Set_CompanyEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,256,1,1,0,0},//显示
};

/*按钮事件*/
static S32 Set_Company_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
	S32  rv = SUCCESS;

	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
					
			if(strlen(pEdit->edit_privattr.digt) > 0)
			{
				strcpy(gtHyc.companyId,pEdit->edit_privattr.digt);
				strcpy(gtHycBak.companyId,pEdit->edit_privattr.digt);
				strcpy(gtHyc.companyName, gau8CompanyName);
				
				glbAllVariable_SaveParameter();
				Com_SpcDlgDeskbox("设置成功!",0,pDesk,1,NULL,NULL,100);
				return RETURN_QUIT;
			}
			
			rv = RETURN_REDRAW;
			break;
		case BUTTON_ID_2:
			rv = RETURN_QUIT;
			break;
		}
	}
	return rv;

}

U32 Set_Company_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPBUTTON		*pBtn;
	
	pEdit = (tGRAPEDIT *)pView;
	
	//通过公司编号获取公司名称
	Company_GetNameById(pEdit->edit_privattr.digt, gau8CompanyName, sizeof(gau8CompanyName));
	
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_3);
	pBtn->draw(pBtn, 0, 0);
	
	return SUCCESS ;
}


/*---------------------------------------------
*函数:Set_CompanyID
*功能:网点编码
*参数:
*返回:
*-----------------------------------------------*/
void Set_CompanyID(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT 	*pEdt;
	tGRAPEDIT2	*pEdit2;
	int len ;
	U8 i;
	int indx = 0;
		
	pDesk=Grap_CreateDeskbox(&Set_CompanyAtrr[indx++],"网点编码");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*消息继承*/
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable= 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	/*插入按钮*/
	for (i=0;i<4;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_CompanyAtrr[indx++], &Set_CompanyBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		
		pBtn->pressEnter=Set_Company_BtnEnter;
	}
	/*插入编辑框*/
	pEdt= Grap_InsertEdit(pDesk, &Set_CompanyAtrr[indx++], &Set_CompanyEditAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdt)
	{
		return;
	}
	pEdt->editKeyEnd = Set_Company_EditKeyEnd;
	strcpy(pEdt->edit_privattr.digt,gtHyc.companyId);
	
	/* 调整光标 */
	len = strlen(pEdt->edit_privattr.digt) ;
	len = len >= pEdt->edit_privattr.containLen ? pEdt->edit_privattr.containLen : len ;
	pEdt->edit_privattr.Pos = len ;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &Set_CompanyAtrr[indx++], &Set_CompanyEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"说明:使用该巴枪的网点编码.");
	pEdit2->get_default_str(pEdit2);
	
	//通过公司编号获取公司名称
	Company_GetNameById(gtHyc.companyId, gau8CompanyName, sizeof(gau8CompanyName));
	
	/*窗体消息循环和释放*/
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return;
}
