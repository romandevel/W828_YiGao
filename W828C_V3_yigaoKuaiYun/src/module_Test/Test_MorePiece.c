#include "Test_MainList.h"


#define TEST_MORE_MAX		3

U16 gu16TestMoreCnt,gu16TestMoreIdx;
const U16 gacTestMore[TEST_MORE_MAX]={100,500,1000};
/*=============================================一票多件界面===================================================*/
const tGrapViewAttr TestMorePiece_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//完成
	{BUTTON_ID_3,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_3,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//完成
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_10,	   5,  35 ,55, 20,   0,0,0,0,   0,0,0,0,1,0,0,0},//大包号
	{BUTTON_ID_11,	   5,  60 ,55, 20,   0,0,0,0,   0,0,0,0,1,0,0,0},//下一站
	{BUTTON_ID_12,	   5,  85 ,55, 20,   0,0,0,0,   0,0,0,0,1,0,0,0},//运单号
	
	{EDIT_ID_1,	   60, 35,170,22,   60,35,235, 57, 0,0,0xffff,0,1,0,1,0},//大包号
	{EDIT_ID_2,	   60, 60,170,22,   60,60,235, 82, 0,0,0xffff,0,1,0,1,0},//下一站
	{EDIT_ID_3,	   60, 85,170,22,   60,85,235,107, 0,0,0xffff,0,1,1,1,0},//运单号
};

const tGrapButtonPriv TestMorePiece_BtnPriv[]=
{
#ifdef W818C
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//完成
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//完成
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
#endif	
	{"大包号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"下一站",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运单号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};


const tGrapEditPriv TestMorePiece_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,20,  1, BARCODE_LEN-1,  			0,0,2,0,0,1,  50,50,0,  0,1,1,1},//
	{NULL,0x0,0,0,2,4,20,  1, COMPANY_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//下一站
	{NULL,0x0,0,0,2,4,20,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
};


int Op_TestMorePiece_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	int ret;
	tDATA_INFO 		Recd;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
				
		if (gpEdit_Packet==NULL || gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("查找控件失败!",0,pDesk,1,NULL,NULL,150);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			if(gpEdit_Packet->view_attr.curFocus == 1)//大包号
			{
				//先判断是否大包号
				if(0 == OpCom_CheckBarcode(gpEdit_Packet->edit_privattr.digt, TYPE_PACKET))
				{
					gpEdit_Packet->view_attr.curFocus = 0;
					gpEdit_Stat2->view_attr.curFocus = 1;
				}
				//不是大包号   判断是否站点
				else if(0 == OpCom_CheckBarcode(gpEdit_Packet->edit_privattr.digt, TYPE_COMPANYID))
				{
					strcpy(gtOpInfoGlb.station2, gpEdit_Packet->edit_privattr.digt);
					strcpy(gpEdit_Stat2->edit_privattr.digt, gpEdit_Packet->edit_privattr.digt);
				}
				else
				{
					Com_SpcDlgDeskbox("请输入大包号!",0,pDesk,1,NULL,NULL,150);
					return RETURN_REDRAW;
				}
			}
			else if(gpEdit_Stat2->view_attr.curFocus == 1)//下一站
			{
				//先判断是否站点
				if(0 == OpCom_CheckBarcode(gpEdit_Stat2->edit_privattr.digt, TYPE_COMPANYID))
				{
					if(strlen(gpEdit_Packet->edit_privattr.digt) > 0)//已经有大包号
					{
						gpEdit_Stat2->view_attr.curFocus = 0;
						gpEdit_Code->view_attr.curFocus = 1;
					}
					else
					{
						gpEdit_Stat2->view_attr.curFocus = 0;
						gpEdit_Packet->view_attr.curFocus = 1;
					}
				}
				//不是站点   判断是否大包号
				else if(0 == OpCom_CheckBarcode(gpEdit_Stat2->edit_privattr.digt, TYPE_PACKET))
				{
					strcpy(gpEdit_Packet->edit_privattr.digt, gpEdit_Stat2->edit_privattr.digt);
				}
				else
				{
					Com_SpcDlgDeskbox("请输站点!",0,pDesk,1,NULL,NULL,150);
					return RETURN_REDRAW;
				}
			}
			else if(gpEdit_Code->view_attr.curFocus == 1)//单号
			{
				if (strlen(gpEdit_Code->edit_privattr.digt) <= 0)
				{
					return RETURN_REDRAW;
				}
				if (OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE) == 0)
				{
					if (strlen(gpEdit_Packet->edit_privattr.digt) <= 0)
					{
						Com_SpcDlgDeskbox("大包号不能为空!",0,pDesk,1,NULL,NULL,150);
						gpEdit_Code->view_attr.curFocus = 0;
						gpEdit_Packet->view_attr.curFocus = 1;
						return RETURN_REDRAW;
					}
					if (strlen(gpEdit_Stat2->edit_privattr.digt) <= 0)
					{
						gpEdit_Code->view_attr.curFocus = 0;
						gpEdit_Stat2->view_attr.curFocus = 1;
						Com_SpcDlgDeskbox("站点不能为空!",0,pDesk,1,NULL,NULL,150);
						return RETURN_REDRAW;
					}
					
					//检查单号是否存在
					//if(0 != Op_ComCheckExist(pDesk, gpEdit_Code->edit_privattr.digt,OpCom_GetCurOpType()))
					if(gu16TestMoreCnt < gacTestMore[gu16TestMoreIdx])
					{
						//填充记录的公共部分
						OpCom_FillCommInfo(&Recd, gpEdit_Packet->edit_privattr.digt, gpEdit_Code->edit_privattr.digt, RECORD_WAITSEND);
			
						//业务部分
						strcpy(Recd.uData.testOp.Station, gtOpInfoGlb.station2);
						
						//写记录
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(gpEdit_Code->edit_privattr.digt);
							memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						}
						else
						{
							Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,150);
						}
						gu16TestMoreCnt++;
					}
					else
					{
						//填充记录的公共部分
						OpCom_FillCommInfo(&Recd, gpEdit_Packet->edit_privattr.digt, gpEdit_Packet->edit_privattr.digt, RECORD_WAITSEND);
						
						//业务部分
						strcpy(Recd.uData.testOp.Station, gtOpInfoGlb.station2);
						
						//写记录
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if(ret != 0)
						{
							Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,150);
							return RETURN_REDRAW;
						}
						
						//改变记录状态为 可传
						RecStore_ChangeMultiCodeState(Login_GetRight(), OpCom_GetCurOpType(), Recd.tComm.code, RECORD_ENABLESEND);
						
						//加载记录
						RecStore_GetUnreportedMulti(Login_GetRight(), gu16TestMoreIdx%2);//发送
						
						if(gu16TestMoreIdx%2)//前台
						{
							//if(0 == Net_ChangeEvt(pDesk, NET_SENDMULTI_F_EVT))
							{
								Net_SendMulti();
							}
						}
						
						gu16TestMoreCnt = 0;
						gu16TestMoreIdx++;
						if(gu16TestMoreIdx >= TEST_MORE_MAX)
						{
							gu16TestMoreIdx = 0;
						}
					}
				}
				else
				{
					Com_SpcDlgDeskbox("运单号错误!",0,pDesk,1,NULL,NULL,150);
					return RETURN_REDRAW;
				}
			}
			break;
		case BUTTON_ID_2://完成
			if (strlen(gpEdit_Packet->edit_privattr.digt) <= 0)
			{
				Com_SpcDlgDeskbox("大包号不能为空!",0,pDesk,1,NULL,NULL,150);
				gpEdit_Code->view_attr.curFocus = 0;
				gpEdit_Packet->view_attr.curFocus = 1;
				return RETURN_REDRAW;
			}
			
			//填充记录的公共部分
			OpCom_FillCommInfo(&Recd, gpEdit_Packet->edit_privattr.digt, gpEdit_Packet->edit_privattr.digt, RECORD_WAITSEND);

			//业务部分
			strcpy(Recd.uData.testOp.Station, gtOpInfoGlb.station2);
			
			//写记录
			ret = Rec_WriteNewRecode(&Recd, NULL);
			if(ret != 0)
			{
				Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,150);
				return RETURN_REDRAW;
			}
			
			//改变记录状态为 可传
			RecStore_ChangeMultiCodeState(Login_GetRight(), OpCom_GetCurOpType(), Recd.tComm.code, RECORD_ENABLESEND);
			
			//加载记录
			RecStore_GetUnreportedMulti(Login_GetRight(), 0);//后台发送
			break;
		case BUTTON_ID_3://返回
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否退出该操作?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*函数:TestFun_MorePiece
*功能:一票多件
*参数:
*返回:
*-----------------------------------------------*/
int TestFun_MorePiece(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
	
	//设置本次操作类型
	OpCom_SetCurOpType(HYCO_OP_TEST);
	
	gu16TestMoreIdx = 0;
	gu16TestMoreCnt = 0;
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&TestMorePiece_ViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_3;
	
	for (i=0; i<6; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestMorePiece_ViewAttr[idx++], &TestMorePiece_BtnPriv[i]);
		if(NULL == pBtn ) return;
		if(i < 3)
		{
			pBtn->pressEnter = Op_TestMorePiece_BtnPress;
		}
	}
	
	for (i=0; i<3; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &TestMorePiece_ViewAttr[idx++], &TestMorePiece_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return;
		if(i == 0)//大包
		{
			gpEdit_Packet = pEdit;
			strcpy(pEdit->edit_privattr.digt, gtOpInfoGlb.packet);
		}
		else if(i == 1)//下一站
		{
			gpEdit_Stat2 = pEdit;
			Company_GetNameById(gtOpInfoGlb.station2, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen);
		}
		else if(i == 2)//运单
		{
			gpEdit_Code = pEdit;
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 135, 220, 140);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

