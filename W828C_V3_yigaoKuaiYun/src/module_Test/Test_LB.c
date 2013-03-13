#include "Test_MainList.h"



/*=============================================二维码识别界面===================================================*/
const tGrapViewAttr TestLB_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//LB识别
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//LB识别
#endif	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,0,0},//显示识别到的LB码
};

const tGrapButtonPriv TestLB_BtnPriv[]=
{
#ifdef W818C
	{"LB识别",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//LB识别
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"识别",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//LB识别
#endif
};

const tGrapEdit2Priv  TestLB_Edit2Attr[]=
{
 	{NULL,	COLOR_YELLOW,0,0,0,TY_UNITY_BG1_EN,0,0,2048,1,1,0,0},//显示
};


int TestLB_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT2		*pEdit2;
	U32 len;
	U8 *pCode, *pUnic;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://LB识别
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			LB_Desk(pDesk,(U8 *)hsaSdram_UpgradeBuf());
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
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

/*-------------------------------------------------------------
* 函数:TestFun_LB
* 功能:二维码识别测试
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_LB(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int i, idx = 0;
	
	pDesk = Grap_CreateDeskbox(&TestLB_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestLB_ViewAttr[idx++], &TestLB_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		pBtn->pressEnter = TestLB_BtnEnter;
	}
	
	pEdit2 = Grap_InsertEdit2(pDesk, &TestLB_ViewAttr[idx++], &TestLB_Edit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"说明:按[确定]键或点击[识别]进入[浩创定制码]识别功能.\r\n识别时巴枪摄像头对准二维码即可自动识别,无需按键.");
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}
