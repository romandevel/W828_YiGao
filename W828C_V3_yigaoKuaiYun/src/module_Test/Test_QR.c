#include "Test_MainList.h"



/*=============================================二维码识别界面===================================================*/
const tGrapViewAttr TestQR_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//QR识别
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//QR识别
#endif	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,0,0},//显示识别到的QR码
};

const tGrapButtonPriv TestQR_BtnPriv[]=
{
#ifdef W818C
	{"QR识别",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//QR识别
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"QR识别",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//QR识别
#endif
};

const tGrapEdit2Priv  TestQR_Edit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,2048,1,1,0,0},//显示
};


int TestQR_BtnEnter(void *pView, U16 state)
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
		case BUTTON_ID_1://QR识别
			len = 0x700000;
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			pCode = QR_Desk(pDesk, (U8 *)hsaSdram_UpgradeBuf(), (U8 *)hsaSdram_UpgradeBuf()+0x280000,&len);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			if(len > 0)
			{
				pCode[len] = 0;
				
				pUnic = (U8 *)hsaSdram_FrontBuf2();
				DecodeUTF8String(CHARACTER_UNICODE,pCode,pUnic,hsaSdram_Front2Size());
				len = Hyc_Unicode2Gbk(pUnic, pCode, 1);
				pCode[len] = 0;
				pCode[len+1] = 0;
				
				
				hyUsbPrintf("code = %s \r\n", pCode);
				pEdit2 = (tGRAPEDIT2 *)Grap_GetViewByID(pDesk, EDIT_ID_1);
				strcpy(pEdit2->edit_privattr.digt, pCode);
				pEdit2->get_default_str(pEdit2);
			}
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
* 函数:TestFun_QR
* 功能:二维码识别测试
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_QR(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int i, idx = 0;
	
	pDesk = Grap_CreateDeskbox(&TestQR_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestQR_ViewAttr[idx++], &TestQR_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		pBtn->pressEnter = TestQR_BtnEnter;
	}
	
	pEdit2 = Grap_InsertEdit2(pDesk, &TestQR_ViewAttr[idx++], &TestQR_Edit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}
