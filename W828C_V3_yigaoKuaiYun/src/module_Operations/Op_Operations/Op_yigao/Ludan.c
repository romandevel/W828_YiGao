/*-----------------------------------------------
快件单号*
目的地*
重量(Kg)*
件数*     扫描子单
支付方式*  (现付,到付....)
代收签单
代收货款
费用
客户

------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO

extern U8 gu8BtErrorFlag;
extern U8 BtTmkdStr[50];

//支付方式
const U8 *const gauYGPayType[]=
{
	"现付","到付","月结"
};

/*==========================================发件界面==========================================*/
const tGrapViewAttr YGLuDan_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//删除
	{BUTTON_ID_3,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_3,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//删除
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	
	{BUTTON_ID_4,	   5,  33 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//快件单号*
	{BUTTON_ID_5,	   5,  58 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//目的地*
	{BUTTON_ID_6,	   5,  83 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//重量(Kg)*
	{BUTTON_ID_7,	   165,84,70, 20,   165,84,238,104,   0,0,0,0,1,0,0,0},//蓝牙
	{BUTTON_ID_8,	   5, 110 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//件数*
	{BUTTON_ID_9,	   165,105,73, 34,  165,105,238,139,   YD_AN1_EN,YD_AN2_EN,0,0,0,0,0,0},//扫描子单
	{BUTTON_ID_10,	   5, 140 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//支付方式*
	{BUTTON_ID_11,	   5, 165 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//费用
	{BUTTON_ID_12,	   5, 190 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//取件员
	{BUTTON_ID_13,	   5, 215, 72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//超长费
	{BUTTON_ID_14,	   5, 240 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//超重费
	
	{EDIT_ID_1,	   	   78, 33,160, 22,   78,33,238,55,  0,0,0xffff,0,1,1,1,0},//快件单号
	{EDIT_ID_2,	   	   78, 58,83, 22,   78,58,161,80,   0,0,0xffff,0,1,0,1,0},//目的地
	{EDIT_ID_20,	  162, 58,76, 22,  162,58,238,80,   0,0,0xCE59,0,1,0,0,0},//目的地名称
	{EDIT_ID_3,	   	   78, 83,83, 22,   78,83,161,105,  0,0,0xCE59,0,1,0,1,0},//重量
	{EDIT_ID_4,	   	   78, 110,83, 22,  78,110,161,132, 0,0,0xffff,0,1,0,1,0},//件数
	{COMBOBOX_ID_1,	   78, 140,160,22,  78,140,238,162,0,0,0xffff,0,1,0,1,0},//支付方式
	{EDIT_ID_5,	   	   78, 165,160,22,  78,165,238,187,0,0,0xCE59,0,1,0,0,0},//费用
	{EDIT_ID_6,	   	   78, 190,83, 22,   78,190,161,212,   0,0,0xffff,0,1,0,1,0},//取件员
	{EDIT_ID_21,	  162, 190,76, 22,  162,190,238,212,   0,0,0xCE59,0,1,0,0,0},//取件员名称
	{EDIT_ID_7,	   	   78, 215,160,22,  78,215,238,237,0,0,0xCE59,0,1,0,0,0},//超长费
	{EDIT_ID_8,	   	   78, 240,160,22,  78,240,238,262,0,0,0xCE59,0,1,0,0,0},//超重费
	
	
};


const tGrapButtonPriv YGLuDan_BtnPriv[]=
{
#ifdef W818C
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
#endif	
		
	{"运 单 号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//快件单号*
	{"目 的 地",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//目的地*
	{"重量(Kg)",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//重量(Kg)*
	{"蓝牙",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//蓝牙
	{"件   数 ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//件数*
	{"扫描子单",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//扫描子单
	{"支付方式",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//支付方式*
	{" 费  用 ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//费用
	{"取 件 员",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//取 件 员
	{"超 长 费",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//超 长 费
	{"超 重 费",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//超 重 费
	
};

const tGrapEditPriv YGLuDan_EditPriv[] = 
{
	//第一页 0-8
	{NULL,0x0,0,0,2,4,19,  1, BARCODE_LEN-1,	0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
	{NULL,0x0,0,0,2,4,8,   1, COMPANY_ID_LEN-1, 0,0,2,0,0,1,  50,50,0,  0,1,1,1},//目的地
	{gtOpInfoGlb.station2,0x0,0,0,2,4,9,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0,0},//目的地名称
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//重量
	{NULL,  0x0,0,0,2,4,9,  1, 3,	0,0,1,0,0,1,  50,50,0,  0,0,1, 1},//件数
	
	{NULL,	0x0,0,0,2,4,7,  1, FEE_LEN-1,0,0,2,0,0,1,  50,50,0,  0,1,1,1},//费用
	{NULL,  0x0,0,0,2,4,8,  1, COMPANY_ID_LEN-1,	0,0,1,0,0,1,  50,50,0,  0,0,1,1},//取件员
	{gtOpInfoGlb.employee,  0x0,0,0,2,4,9,  1, COMPANY_NAME_LEN-1,	0,0,0,0,0,0,  50,50,0,  1,0,0,0},//取件员名称
	{NULL,	0x0,0,0,2,4,7, 1, FEE_LEN-1,0,0,0,0,0,1,  50,50,0,  0,1,1,1},//超长费
	{NULL,	0x0,0,0,2,4,7, 1, FEE_LEN-1,0,0,0,0,0,1,  50,50,0,  0,1,1,1},//超重费
	
};

const tComboboxPriv YGLuDan_ComAttr[]=
{
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//支付方式
	
};

int YGLuDan_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn, *pBtnSub;
	tGRAPEDIT 		*pEditWT, *pEditNum;
	tGRAPEDIT 		*pEdit;
	tGRAPEDIT2 		*pEdit2;
	tGRAPCOMBOBOX	*pCombox;
	tDATA_INFO 		Recd, *pSub;
	int ret,i;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if(gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_2://完成
			if(strcmp(gpEdit_Code->edit_privattr.digt, SubCode_GetMainCode()) != 0)
			{
				Com_SpcDlgDeskbox("快件单号不一致!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			if(SubCode_GetSubTotal() > 1 && SubCode_GetSubTotal() == SubCode_GetSubScan())
			{
				if(SubCode_GetFinalState() != 0)
				{
					Com_SpcDlgDeskbox("该单已完成,不可操作!",6,pDesk,1,NULL,NULL,100);
					break;
				}
				if(strlen(gtOpInfoGlb.station2) == 0)
				{
					Com_SpcDlgDeskbox("请输入正确的站点!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(gpEdit_Stat2);
					break;
				}
				
				pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
				if(strlen(pEditWT->edit_privattr.digt) == 0)
				{
					Com_SpcDlgDeskbox("请输入正确的重量!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(pEditWT);
					break;
				}
				
				pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
				if(strlen(pEditNum->edit_privattr.digt) == 0 || atoi(pEditNum->edit_privattr.digt) < 1)
				{
					Com_SpcDlgDeskbox("请输入正确的件数!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(pEditNum);
					break;
				}
				
				OpCom_FillCommInfo(&Recd, gpEdit_Code->edit_privattr.digt,NULL, RECORD_UNSEND);
				
				strcpy(Recd.uData.ludan.bchId, gpEdit_Code->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.bchId = %s \r\n",Recd.uData.ludan.bchId);
				strcpy(Recd.uData.ludan.deststat, gpEdit_Stat2->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.deststat = %s \r\n",Recd.uData.ludan.deststat);
				strcpy(Recd.uData.ludan.obj_wei, gtOpInfoGlb.weight);
				hyUsbPrintf("Recd.uData.ludan.obj_wei = %s \r\n",Recd.uData.ludan.obj_wei);
				strcpy(Recd.uData.ludan.num, pEditNum->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.num = %s \r\n",Recd.uData.ludan.num);
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_1);
				Recd.uData.ludan.payType  = pCombox->comboPriv.cur_index;
				strcpy(Recd.uData.ludan.daishouCode,gauYGPayType[pCombox->comboPriv.cur_index]);
				hyUsbPrintf("Recd.uData.ludan.daishouCode = %s \r\n",Recd.uData.ludan.daishouCode);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_5);
				strcpy(Recd.uData.ludan.feeTotal, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.feeTotal = %s \r\n",Recd.uData.ludan.feeTotal);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_6);
				strcpy(Recd.uData.ludan.customer_id, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.customer_id = %s \r\n",Recd.uData.ludan.customer_id);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_7);
				strcpy(Recd.uData.ludan.feeDaishou, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.feeDaishou = %s \r\n",Recd.uData.ludan.feeDaishou);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_8);
				strcpy(Recd.uData.ludan.sendName, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.sendName = %s \r\n",Recd.uData.ludan.sendName);
												
				//保存
				ret = Rec_WriteNewRecode(&Recd, NULL);
				if(ret != 0)
				{
					Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,150);
					return RETURN_REDRAW;
				}
								
				Com_SpcDlgDeskbox("保存成功!",0,pDesk,1,NULL,NULL,100);
				return RETURN_QUIT;
			}
			else
			{
				Com_SpcDlgDeskbox("此单还未操作完!",0,pDesk,1,NULL,NULL,100);
			}
			break;
		case BUTTON_ID_1://确定
			if(gpEdit_Stat2->view_attr.curFocus == 1)//目的地
			{
				gpEdit_Stat2->editenter(gpEdit_Stat2);
				break;
			}
			
			if(gpEdit_Code->view_attr.curFocus == 1)
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					break;
				}
				
				//判断单号
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
				
					SubCode_SetMainCode(gpEdit_Code->edit_privattr.digt);
				/*
					//加载该主单下面的子单
					ret = RecStore_GetSubCodeByMainCode(Login_GetRight(), OpCom_GetCurOpType(), NULL, 
													pCode, (U8 *)hsaSdram_UpgradeBuf(), hsaSdram_UpgradeSize());
					
					hyUsbPrintf("get subcode cnt = %d \r\n", ret);
					//有加载到子单
					if(ret > 0)
					{
						pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
						sprintf(pEditNum->edit_privattr.digt, "%d", ret);
						pBtnSub = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_9);
						SubCode_SetSubTotal(ret);
						SubCode_SetSubScan(ret);
						pBtnSub->view_attr.enable = 1;
						
						//判断是否已完成的单 (找主单和子单相同的记录),如果已完成,则不可再修改
						pSub = (tDATA_INFO *)hsaSdram_UpgradeBuf();
						for(i = 0; i < ret; i++)
						{
							if(strcmp(pSub->tComm.code,pSub->tComm.subCode) == 0)
							{
								//已经完成的单,总数不可再修改
								pEditNum->edit_privattr.enable = 0;
								pEditNum->view_attr.FocusEnable = 0;
								pEditNum->view_attr.color=0xCE59;
								SubCode_SetFinalState(1);
								break;
							}
							pSub++;
						}
					}
				*/
					Grap_ChangeFocus(gpEdit_Code,1);
				}
				else
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
			}
			break;
		case BUTTON_ID_3://返回
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否退出该操作?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_7://蓝牙配对
			pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
			OpComm_ConnBT(pDesk, pEditWT);
			if(gpEdit_Code->view_attr.curFocus != 1)
			{
				Grap_ChangeFocusEnable(gpEdit_Code);
			}
			break;
		case BUTTON_ID_9://扫描子单
		hyUsbPrintf("maincode = %s \r\n", gpEdit_Code->edit_privattr.digt);
			if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
			{
				Com_SpcDlgDeskbox("请先扫描主单号!",0,pDesk,1,NULL,NULL,100);
				if(gpEdit_Code->view_attr.curFocus != 1)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
				}
				break;
			}
			else
			{
				ret = OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE);
				if(ret == -1)
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
					if(gpEdit_Code->view_attr.curFocus != 1)
					{
						Grap_ChangeFocusEnable(gpEdit_Code);
					}
					break;
				}
			}
			
			pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
			ret = atoi(pEditNum->edit_privattr.digt);
			if(ret > 500)
			{
				Com_SpcDlgDeskbox("件数超出范围(最大500件)!",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
				break;
			}
			SubCode_SetSubTotal(ret);
			SubCode_SetMainCode(gpEdit_Code->edit_privattr.digt);
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			Operation_SubCode_Desk(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

U32 YGLuDan_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0,num,scan;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_2)//目的地
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.station2, 0, sizeof(gtOpInfoGlb.station2));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	else if(pEdit->view_attr.id == EDIT_ID_4)//件数
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			pDesk= (tGRAPDESKBOX *)pEdit->pParent;
			pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_9);
			
			num = atoi(pEdit->edit_privattr.digt);
			/*scan = SubCode_GetSubScan();
			if(num < scan)
			{
				//小于实际已扫描数,提示需要删除子单
				Com_SpcDlgDeskbox("小于已扫描子单数,请先删除子单!",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
				sprintf(pEdit->edit_privattr.digt, "%d", scan);
				ReDraw(pDesk, 0, 0);
				return SUCCESS ;
			}
			
			SubCode_SetSubTotal(num);*/
			if(num > 1)
			{
				pBtn->view_attr.enable = 1;
				if(pBtn) pBtn->draw(pBtn, 0, 0);
			}
			else
			{
				pBtn->view_attr.enable = 0;
				ReDraw(pDesk, 0, 0);
			}
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_6)//取件员
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.employee, 0, sizeof(gtOpInfoGlb.employee));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_21);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	
	return SUCCESS ;
}

S32 YGLuDanMDD_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEdit1;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	pEdit1 = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_2);
	pCode = pEdit1->edit_privattr.digt;

	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ret = Op_DownLoad_Company_Desk(pDesk, 1);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	//这里没有判断ret, 因为Company_GetNameById里面有判断
	Company_GetIdNameByIdx(ret, pEdit1->edit_privattr.digt, pEdit1->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
	pEdit1->edit_privattr.Pos = strlen(pEdit1->edit_privattr.digt);
	
	if(ret != -1)
	{
		Grap_ChangeFocus(pEdit,1);
	}
	
	return RETURN_REDRAW;
}

S32 YGLuDanQJY_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEdit1;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	pEdit1 = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_6);
	pCode = pEdit1->edit_privattr.digt;

	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ret = Op_DownLoad_Company_Desk(pDesk, 1);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	//这里没有判断ret, 因为Company_GetNameById里面有判断

	Company_GetIdNameByIdx(ret, pEdit1->edit_privattr.digt, pEdit1->edit_privattr.containLen, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
	pEdit1->edit_privattr.Pos = strlen(pEdit1->edit_privattr.digt);
	
	if(ret != -1)
	{
		Grap_ChangeFocus(pEdit,1);
	}
	
	return RETURN_REDRAW;
}

S32 YGLuDan_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	tGRAPCOMBOBOX	*pCombox;
	AREAnode		*pCity;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	switch(pCombox->view_attr.id)
	{
	case COMBOBOX_ID_1://第一页   支付方式
		strcpy(pStr, gauYGPayType[i]);
		break;
	default:
		break;
	}
	
	
	return SUCCESS;
}

/*-----------------------------------------------
*函数:Operation_YGLuDan_Desk
*功能:录单
*参数:
*返回:
*-----------------------------------------------*/
int Operation_YGLuDan_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,j,idx=0;
	
	SubCode_Init();
	memset(gtOpInfoGlb.station2,0,sizeof(gtOpInfoGlb.station2));
	memset(gtOpInfoGlb.weight,0,sizeof(gtOpInfoGlb.weight));
	memset(gtOpInfoGlb.employee,0,sizeof(gtOpInfoGlb.employee));
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&YGLuDan_ViewAttr[idx++], "录单操作");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_8;

	for (i=0; i<3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &YGLuDan_ViewAttr[idx++], &YGLuDan_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		pBtn->pressEnter = YGLuDan_BtnPress;
	}

	
	for(j = 0; j < 11; j++)//btn
	{
		pBtn = Grap_InsertButton(pDesk,&YGLuDan_ViewAttr[idx++],&YGLuDan_BtnPriv[j+3]);
		if(NULL == pBtn) return;
		if(j == 3 || j == 5)//蓝牙   扫描子单
		{
			pBtn->pressEnter = YGLuDan_BtnPress;
			if(j == 3)
			{
				pBtn->draw = OpComm_BtnBTDraw;
			}
		}
	}
	
	for(j = 0; j < 5; j++)
	{
		pEdit = Grap_InsertEdit(pDesk,&YGLuDan_ViewAttr[idx++], &YGLuDan_EditPriv[j]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(j == 0)//单号
		{
			gpEdit_Code = pEdit;
		}
		else if(j == 1)//目的地
		{
			pEdit->editKeyEnd = YGLuDan_EditKeyEnd;
			gpEdit_Stat2 = pEdit;
		}
		else if(j == 2)//目的地名称
		{
			pEdit->editenter  = YGLuDanMDD_EditEnter;
		}
		else if(j == 3)//重量
		{
			pEdit->handle = OpComm_EditHandle_Price;
			pEdit->editTimer = BT_WeightEditTimer;
			//strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(j == 4)//件数
		{
			pEdit->editKeyEnd = YGLuDan_EditKeyEnd;
			strcpy(pEdit->edit_privattr.digt, "1");
		}
	}
	
	//支付方式
	pComBox = Grap_InsertCombobox(pDesk,&YGLuDan_ViewAttr[idx++], &YGLuDan_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(gauYGPayType)/sizeof(U8 *);
	pComBox->comboxgetstr = YGLuDan_ComboboxGetStr;
	
	for(j = 0; j < 5; j++)
	{
		pEdit = Grap_InsertEdit(pDesk,&YGLuDan_ViewAttr[idx++], &YGLuDan_EditPriv[j+5]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(j == 0 || j == 3 || j == 4)//费用
		{
			pEdit->handle = OpComm_EditHandle_Price;
		}
		else if(j == 1)//取件员
		{
			pEdit->editKeyEnd = YGLuDan_EditKeyEnd;
			gpEdit_Employee = pEdit;
		}
		else if(j == 2)//取件员名称
		{
			pEdit->editenter  = YGLuDanQJY_EditEnter;
		}
		
	}
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	//退出就断开BT
	if (hyco_getConnectState() == TRUE)
	{
		bt_disconnect(pDeskFather);
	}
	
	return 0;
}

/*-----------------------------------------记录查询显示------------------------------------------------*/
const U8* const opType_YGLuDan[]=//录单
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"员工编号:",
	"扫描时间:",
	
	"主单号:",
	"目的站:",
	"重量:",
	"件数:",
	"支付方式:",
	"费用:",
	"取件员:",
	"超长费:",
	"超重费:",
};


/*---------------------------------------------------------------
*函数: YGLuDan_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int YGLuDan_SearchDisplay(void *p)
{
	U8 *str[]={"未传", "已传", "已删", "未传", "未传"};
	U8 *pDispStr;
	tOP_DISPINFO 	*pOpCb;
	tDATA_INFO		*pData;
	tDATA_COMM 		*pComm;
	tLUDAN			*ludan;
	tDATA_INFO 		*pSub;
	U8				*pSubCode;
	int total,dispBufLen, subCnt;
	int i, j, cnt,stat,len=0;
	
	pOpCb = (tOP_DISPINFO *)p;
	pData = (tDATA_INFO *)(pOpCb->pData);
	total = pOpCb->total;
	pDispStr = pOpCb->pDispBuf;
	dispBufLen = pOpCb->dispBufLen;
	
	pSubCode = (U8 *)hsaSdram_UpgradeBuf()+0x3D0000;
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		ludan = &(pData[i].uData.ludan);
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s\r\n",
							opType_YGLuDan[0], str[stat],
							opType_YGLuDan[1], pComm->opType,
							opType_YGLuDan[2], pComm->scanStat,
							opType_YGLuDan[3], pComm->userId,
							opType_YGLuDan[4], pComm->scanTime,
							
							opType_YGLuDan[5], pComm->code,
							opType_YGLuDan[6], ludan->deststat,
							opType_YGLuDan[7], ludan->obj_wei,
							opType_YGLuDan[8], ludan->num,
							opType_YGLuDan[9], gauYGPayType[ludan->payType],
							opType_YGLuDan[10], ludan->feeTotal,
							opType_YGLuDan[11], ludan->customer_id,
							opType_YGLuDan[12], ludan->feeDaishou,
							opType_YGLuDan[13], ludan->sendName
							);
		
		
		len += strlen(&pDispStr[len]);
		//加载该主单下面的子单
		subCnt = RecStore_GetSubCodeByMainCode(Login_GetRight(), OpCom_GetCurOpType(), NULL, 
											pComm->code, pSubCode, 0x30000);
		if(subCnt > 0)
		{
			pSub = (tDATA_INFO *)pSubCode;
			for(j = 0; j < subCnt; j++)
			{
				if(strcmp(pSub[j].tComm.code,pSub[j].tComm.subCode) != 0)
				{
					sprintf(&pDispStr[len], "    %s\r\n", pSub[j].tComm.subCode);
					len += (strlen(pSub[j].tComm.subCode)+6);
				}
			}
		}
		strcat(&pDispStr[len], "-----------------------\r\n");
		
		len += strlen("-----------------------\r\n");
		//需要判断长度是否超出
		if(len + 512 >= dispBufLen)
		{
			strcpy(&pDispStr[len], "显示空间不足,后续记录无法显示.");
		}
	}
	
	return 0;
}


/*-----------------------------------------组织请求数据------------------------------------------------*/
/*---------------------------------------------------------------
*函数: YGLuDan_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int YGLuDan_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt,len;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tLUDAN		*pLuDan;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pTmp,url[256];
	U8	buf[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

	memset(url,0,256);
	pComm = &(pNode[0].tData.tComm);
	pLuDan = &(pNode[0].tData.uData.ludan);
	
	sprintf(pSend, "StrJson=[Request][data][no Code='02'][a]%s[/a][b]%s[/b][c]%s[/c][d]%s[/d][e]%s[/e][f]%s[/f][g]%s[/g]\
[h]%s[/h][i]%s[/i][j]%s[/j][k]%s[/k][l]%s[/l][m]%s[/m][n]%s[/n][/no][/data][/Request]&Checking_Key=002",
				pComm->userId,pComm->scanStat,gtHyc.MachineCode,pLuDan->bchId,pLuDan->deststat,pLuDan->obj_wei,pLuDan->num,
				pLuDan->daishouCode,pLuDan->feeTotal,pLuDan->customer_id,pLuDan->feeDaishou,pLuDan->sendName,pComm->scanTime,pLuDan->sub_code);
	
	dataLen = strlen(pSend);
	
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
#if 1	
	//编码转换
	//发送前需要先转成unicode  再转成utf-8
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	//1 转unicode
	Hyc_Gbk2Unicode(pSend,pTemp,0);
	//2 转utf-8
	Unicode2UTF8(pTemp, pSend);
#endif
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "application/x-www-form-urlencoded";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("LuDan send = ");
hyUsbMessageByLen(pSend, strlen(pSend));
hyUsbPrintf("\r\n");
	sprintf(url,"%s/RequestInvoke",gtHyc.url);
hyUsbPrintf("url = %s\r\n",url);
	cnt = http_send_request(url, bufLen, &tRequest);
	if(cnt > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		
		cnt = Net_DecodeData(pSend,cnt,pGbk);
		if(cnt > 0)
		{
			cnt = html_decode(pGbk, cnt, pSend);
					
			hyUsbPrintf("recv (front)\r\n");
			hyUsbMessageByLen(pSend, cnt);
			hyUsbPrintf("\r\n");
			
			pTmp = strstr(pSend,"<result>");
			pSend = pTmp + 8;
			if(memcmp(pSend, "true", 4) == 0)
			{
				
			}
			else if(memcmp(pSend,"false",5) == 0)
			{
				memset(BtTmkdStr,0,50);
				pTmp = strstr(pSend,"<reason>");
				pSend = pTmp + 8;
				pTmp = strstr(pSend,"</reason>");
				if(pTmp != NULL)
				{
					gu8BtErrorFlag = 1;
					len = (int)(pTmp-pSend);
					if(len >= 50) len = 50;
					memcpy(BtTmkdStr,pSend,len);
				}
			}
			ret = 0;
		}
	}
hyUsbPrintf("LuDan ret = %d \r\n",ret);	
	
	return ret;
}

#endif //VER_CONFIG_HYCO