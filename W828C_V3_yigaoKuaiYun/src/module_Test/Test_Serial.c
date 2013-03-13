#include "Test_MainList.h"



static U8 gsu8TestSerial[40];
/*=============================================烧录序列号界面===================================================*/
/*控件*/
const tGrapViewAttr TestSerialViewAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,320, 0,0,0,0,   TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*返回*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*保存*/
#endif	
	{BUTTON_ID_3,	10,80,220, 16,	0,0,0,0 ,	    0,0,0,0,1,0,0,0},
	
	{BUTTON_ID_4,	10,120,220, 16,	0,0,0,0 ,	    0,0,0,0,1,0,0,0},
	
	{EDIT_ID_1,		29,140,186,24,  29,100,215,164,   0,0,0xffff,0,1,1,1,0},/*输入文本框*/
};

/*btn 属性*/
const tGrapButtonPriv TestSerialBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
	{(U8 *)"确定", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
#endif
	{gsu8TestSerial, 0xffff,0x0,FONTSIZE8X16,0,2,0, TY_UNITY_BG1_EN,NULL},
	{gau8TestFunBuf, 0xffff,0x0,FONTSIZE8X16,0,2,0, TY_UNITY_BG1_EN,NULL},
};
/*edt 属性*/
const tGrapEditPriv	  TestSerialEdtAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,6,23,  1,30,0,0,2,0,0,1,   50,50,0,  0,1,1,1},
};


/*按钮事件*/
static S32 TestSerial_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
	S32  rv = SUCCESS;
	U8 flag = 0, tempId[16];

	pBtn  = (tGRAPBUTTON*)pView;
	pDesk = (tGRAPDESKBOX*)pBtn->pParent;
	
	id=pBtn->view_attr.id;
	
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
			if(strlen(pEdit->edit_privattr.digt) != 9)
			{
				Com_SpcDlgDeskbox("位数错误!",6,pDesk,1,NULL,NULL,200);
				break;
			}
			if(strcmp(&gau8TestFunBuf[9], pEdit->edit_privattr.digt) != 0)
			{
				sprintf(gau8TestFunBuf, "本次扫描:%s", pEdit->edit_privattr.digt);
				pEdit->edit_privattr.digt[0] = 0;
			}
			else
			{
				if(HY_OK == autDrv_ds28cn01_writeSN(pEdit->edit_privattr.digt))
				{
					syssleep(10);
					if (autDrv_ds28cn01_readSN(tempId)==HY_OK && strcmp(tempId, pEdit->edit_privattr.digt)==0)
					{
						sprintf(gsu8TestSerial, "当前序列号:%s", pEdit->edit_privattr.digt);
						strcpy(gtHyc.MachineCode, pEdit->edit_privattr.digt);		//终端编号
						Com_SpcDlgDeskbox("烧录成功!",0,pDesk,2,NULL,NULL,200);
					}
					else
					{
						//烧录失败
						flag = 1;
					}
					hyUsbPrintf("gtHyc.MachineCode = %s \r\n", gtHyc.MachineCode);
				}
				else
				{
					flag == 1;
				}
				if(flag == 1)
				{
					Com_SpcDlgDeskbox("烧录失败!",6,pDesk,1,NULL,NULL,100);
				}
				pEdit->edit_privattr.digt[0] = 0;
			}
			break;
		case BUTTON_ID_2:
			return RETURN_QUIT;
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return rv;
}



/*---------------------------------------------
*函数:TestFun_Serial
*功能:烧录序列号
*参数:
*返回:
*-----------------------------------------------*/
int TestFun_Serial(void *pDeskFather)
{
	tGRAPDESKBOX 	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdt;
	U8 				i, idx=0;
	
	strcpy(gau8TestFunBuf, "本次扫描:");
	
	strcpy(gsu8TestSerial,"当前序列号:");
	i = strlen(gsu8TestSerial);
	autDrv_ds28cn01_readSN(&gsu8TestSerial[i]);
hyUsbPrintf("gsu8TestSerial = %s \r\n", gsu8TestSerial);
	pDesk=Grap_CreateDeskbox(&TestSerialViewAtrr[idx++], NULL);
	if (NULL==pDesk)
	{
		return -1;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*消息继承*/
	pDesk->scanEnable = 1;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	/*插入按钮*/
	for (i=0; i<4; i++)
	{
		pBtn=Grap_InsertButton(pDesk, &TestSerialViewAtrr[idx++], &TestSerialBtnAttr[i]);
		if (NULL == pBtn)
		{
			return -1;
		}
		pBtn->pressEnter = TestSerial_BtnEnter;
	}
	/*插入编辑框*/
	pEdt= Grap_InsertEdit(pDesk, &TestSerialViewAtrr[idx++], &TestSerialEdtAttr[0]);
	pEdt->DrawInputCnt = OpCom_EditDrawInputCnt;
	if (NULL == pEdt)
	{
		return -1;
	}

	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

