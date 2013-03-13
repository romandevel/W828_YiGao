/*-----------------------------------------------
网点--->中转站    中转站---->中转站    中转站---->网点
发件操作
界面显示
运输方式
中转站
下一站
重量
运单号
------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//浩创

const U8 *const gcCarOrderFJQR[]=
{
	"汽运",	"航空",	"海运"
};

/*==========================================发件界面==========================================*/
const tGrapViewAttr FaJianQR_ViewAttr[] =
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
	{BUTTON_ID_4,	   165,111, 70,20,   165,111,238,121,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//蓝牙
	
	{BUTTON_ID_10,	   5,  35 ,64, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运输方式
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//中转站
	{BUTTON_ID_12,	   5,  85 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//下一站
	{BUTTON_ID_13,	   5, 110 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//重  量
	{BUTTON_ID_14,	 140, 110 ,16, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//Kg
	{BUTTON_ID_15,	   5, 135 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运单号
	
	{COMBOBOX_ID_1,	   70, 35,158,22,   70,35,228,57,  0,0,0xffff,0,1,1,1,0},//运输方式
	{EDIT_ID_1,	   	   55, 60,83, 22,   55,60,138,82,  0,0,0xffff,0,1,0,1,0},//中转站
	{EDIT_ID_2,	   	   55, 85,83, 22,   55,85,138,107,  0,0,0xffff,0,1,0,1,0},//下一站
	{EDIT_ID_3,   	   55,110,83, 22,   55,110,138,132, 0,0,0xCE59,0,1,0,1,0},//重量
	{EDIT_ID_4,	   	   55,135,172,22,   55,135,227,157,0,0,0xffff,0,1,0,1,0},//运单号 21
	
	{EDIT_ID_20,       140,60 ,100,22,  140,60,240,82,     0,0,0xCE59,0,1,0,0,0},//中转站名称
	{EDIT_ID_21,       140,85 ,100,22,  140,85,240,107,    0,0,0xCE59,0,1,0,0,0},//下一站名称
};

const tGrapButtonPriv FaJianQR_BtnPriv[]=
{
#ifdef W818C
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
	{"删除",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"删除",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
#endif	
	{"蓝牙",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 		TY_UNITY_BG1_EN,NULL},//
	
	{"运输方式",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"中转站",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"下一站",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"重  量",  0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"Kg",  	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运单号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv FaJianQR_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, COMPANY_ID_LEN-1,   0,0,2,0,0,1,  50,50,0,  0,1,1,1},//中转站
	{NULL,0x0,0,0,2,4,10,  1, COMPANY_ID_LEN-1,   0,0,2,0,0,1,  50,50,0,  0,1,1,1},//下一站
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//重量
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
	
	{gtOpInfoGlb.station1,0x0,0,0,2,4,12,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  1,0,0,0},//中转站
	{gtOpInfoGlb.station2,0x0,0,0,2,4,12,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  1,0,0,0},//下一站
};

const tComboboxPriv FaJianQR_ComAttr[]=
{
	{0,6,0,0,0,0,    1,138,16,158,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//运输方式
};

int FaJianQR_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEditWT;
	tDATA_INFO 		Recd;
	int ret;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if(gpEdit_Stat1==NULL || gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			if(gpEdit_Stat1->view_attr.curFocus == 1)//中转站
			{
				if(strlen(gtOpInfoGlb.station1) == 0)
				{
					gpEdit_Stat1->editenter(gpEdit_Stat1);
				}
				else
				{
					Grap_ChangeFocus(gpEdit_Stat1,1);
				}
				break;
			}
			else if(gpEdit_Stat2->view_attr.curFocus == 1)//下一站
			{
				if(strlen(gtOpInfoGlb.station2) == 0)
				{
					gpEdit_Stat2->editenter(gpEdit_Stat2);
				}
				else
				{
					Grap_ChangeFocus(gpEdit_Stat2,1);
				}
				break;
			}
			else if(gpEdit_Code->view_attr.curFocus == 1)
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
					if(strlen(gtOpInfoGlb.station2) == 0)
					{
						Com_SpcDlgDeskbox("请先输入下一站!",0,pDesk,1,NULL,NULL,100);
						memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						break;
					}
					
					//检查单号是否存在
					ret = Op_ComCheckExist(pDesk, pCode, OpCom_GetCurOpType());
					if(ret == 0)//已存在,未删除
					{
						
					}
					else if(ret == 1)//已存在,已删除
					{
						
					}
					else //不存在
					{
						//填充记录的公共部分
						OpCom_FillCommInfo(&Recd, pCode, NULL, RECORD_UNSEND);

						//业务部分
						strcpy(Recd.uData.saomiao.prevstat, gpEdit_Stat1->edit_privattr.digt);
						strcpy(Recd.uData.saomiao.nextstat, gpEdit_Stat2->edit_privattr.digt);
						sprintf(Recd.uData.saomiao.tran_types, "%d", gtOpInfoGlb.tempIndex);//运输方式
						strcpy(Recd.uData.saomiao.obj_wei, gtOpInfoGlb.weight);
						
						//写记录
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(pCode);
							//memset(gtOpInfoGlb.weight, 0, sizeof(gtOpInfoGlb.weight));
						}
						else
						{
							Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,100);
						}
					}
					memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				}
				else
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
			}
			else
			{
				//焦点下移
				//Edit_ChangeFocus(pDesk, 1);
				Grap_ChangeFocus(gpEdit_Code,1);
			}
			break;
		case BUTTON_ID_2://删除
			Op_ComDelCode(pDesk, OpCom_GetCurOpType());
			break;
		case BUTTON_ID_3://返回
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否退出该操作?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_4://蓝牙配对
			pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
			OpComm_ConnBT(pDesk, pEditWT);
			if(gpEdit_Code->view_attr.curFocus != 1)
			{
				Grap_ChangeFocusEnable(gpEdit_Code);
			}
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

U32 FaJianQR_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//中转站
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.station1, 0, sizeof(gtOpInfoGlb.station1));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	else if(pEdit->view_attr.id == EDIT_ID_2)//下一站
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
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_21);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	
	return SUCCESS ;
}

S32 FaJianQR_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//上一站
	{
		if(strlen(gtOpInfoGlb.station1) == 0)
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Op_DownLoad_Company_Desk(pDesk, 1);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			//这里没有判断ret, 因为Company_GetNameById里面有判断
			Company_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
			pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
				
			if(ret != -1)
			{
				Grap_ChangeFocus(pEdit,1);
			}
		}
		else
		{
			return SUCCESS;
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_20)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Op_DownLoad_Company_Desk(pDesk, 1);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		//这里没有判断ret, 因为Company_GetNameById里面有判断
		Company_GetIdNameByIdx(ret, gpEdit_Stat1->edit_privattr.digt, gpEdit_Stat1->edit_privattr.containLen, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
		gpEdit_Stat1->edit_privattr.Pos = strlen(gpEdit_Stat1->edit_privattr.digt);
		
		if(ret != -1)
		{
			Grap_ChangeFocusEnable(gpEdit_Stat2);
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_2)//目的站
	{
		if(strlen(gtOpInfoGlb.station2) == 0)
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Op_DownLoad_Company_Desk(pDesk, 1);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			//这里没有判断ret, 因为Company_GetNameById里面有判断
			Company_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
			pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
			
			if(ret != -1)
			{
				Grap_ChangeFocus(pEdit,1);
			}
		}
		else
		{
			return SUCCESS;
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_21)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Op_DownLoad_Company_Desk(pDesk, 1);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		//这里没有判断ret, 因为Company_GetNameById里面有判断
		Company_GetIdNameByIdx(ret, gpEdit_Stat2->edit_privattr.digt, gpEdit_Stat2->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
		gpEdit_Stat2->edit_privattr.Pos = strlen(gpEdit_Stat2->edit_privattr.digt);
		
		if(ret != -1)
		{
			Grap_ChangeFocusEnable(gpEdit_Code);
		}
	}
	
	return RETURN_REDRAW;
}

S32 FaJianQR_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	strcpy(pStr, gcCarOrderFJQR[i]);
	
	return SUCCESS;
}

S32 FaJianQR_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->list_opened == 1)
	{
		gtOpInfoGlb.tempIndex = i;
		Grap_ChangeFocus(pCombox, 1);//向下切换焦点
	}
	
	return SUCCESS;
}

/*-----------------------------------------------
*函数:Operation_FaJianQR_Desk
*功能:收件
*参数:
*返回:
*-----------------------------------------------*/
int Operation_FaJianQR_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,idx=0;
	
	gtOpInfoGlb.tempIndex = 0;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&FaJianQR_ViewAttr[idx++], "发件扫描");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_4;
	
	for (i=0; i<10; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = FaJianQR_BtnPress;
			if(i == 3)
			{
				pBtn->draw = OpComm_BtnBTDraw;
			}
		}
	}
	
	pComBox = Grap_InsertCombobox(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(gcCarOrderFJQR)/sizeof(U8 *);
	pComBox->comboxgetstr = FaJianQR_ComboboxGetStr;
	pComBox->comboxenter  = FaJianQR_ComboxEnter;
	
	//中转站  下一站   重量  运单  中转站名称  下一站名称
	for (i=0; i<6; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//中转站
		{
			pEdit->editenter  = FaJianQR_EditEnter;
			pEdit->editKeyEnd = FaJianQR_EditKeyEnd;
			gpEdit_Stat1 = pEdit;
		}
		else if(i == 1)//下一站
		{
			pEdit->editenter  = FaJianQR_EditEnter;
			pEdit->editKeyEnd = FaJianQR_EditKeyEnd;
			gpEdit_Stat2 = pEdit;
		}
		else if(i == 2)//重量
		{
			pEdit->handle = OpComm_EditHandle_Price;
			pEdit->editTimer = BT_WeightEditTimer;
			strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(i == 3)//运单号
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 4 || i == 5)
		{
			pEdit->editenter  = FaJianQR_EditEnter;
		}
	}
		
	Op_CommonInsertListShow(pDesk, 10, 160, 220, 120);
	
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
const U8* const opType_FaJianQR[]=//发件
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"员工编号:",
	"扫描时间:",
	
	"运输方式:",
	"中转站:",
	"下一站:",
	"单号:",
};


/*---------------------------------------------------------------
*函数: FaJianQR_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int FaJianQR_SearchDisplay(void *p)
{
	U8 *str[]={"未传", "已传", "已删", "未传", "未传"};
	U8 *pDispStr;
	tOP_DISPINFO 	*pOpCb;
	tDATA_INFO		*pData;
	tDATA_COMM 		*pComm;
	tSAOMIAO		*saomiao;
	int total,dispBufLen;
	int i, j, cnt,stat,len=0;
	
	pOpCb = (tOP_DISPINFO *)p;
	pData = (tDATA_INFO *)(pOpCb->pData);
	total = pOpCb->total;
	pDispStr = pOpCb->pDispBuf;
	dispBufLen = pOpCb->dispBufLen;
	
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		saomiao = &(pData[i].uData.saomiao);
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_FaJianQR[0], str[stat],
							opType_FaJianQR[1], pComm->opType,
							opType_FaJianQR[2], pComm->scanStat,
							opType_FaJianQR[3], pComm->userId,
							opType_FaJianQR[4], pComm->scanTime,
							
							opType_FaJianQR[5], gcCarOrderFJQR[atoi(saomiao->tran_types)],
							opType_FaJianQR[6], saomiao->prevstat,
							opType_FaJianQR[7], saomiao->nextstat,
							opType_FaJianQR[8], pComm->code
							);
		
		len += strlen(&pDispStr[len]);
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
*函数: FaJianQR_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int FaJianQR_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*pSaomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

return 0;//test
	
	//可以批量上传	
	//填充请求头
	dataLen = NetReq_FillHead_yida(pSend, "", "addScanData");
	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		pSaomiao = &(pNode[i].tData.uData.saomiao);
		
		sprintf(&pSend[dataLen], "<detailValue><scanType>%02d</scanType><scanUser>%s</scanUser><scanSite>%s</scanSite><scanTime>%s</scanTime>\
<scanHawb>%s</scanHawb><preNextSite>%s</preNextSite><shiftTimes>%s</shiftTimes><weight></weight><bagNumber></bagNumber>\
<exceptionCode></exceptionCode><exceptionMemo></exceptionMemo><signatureType></signatureType><signAture></signAture>\
<businessMan>%s</businessMan></detailValue>",
					pComm->opType,pComm->userId,pComm->scanStat,pComm->scanTime,pComm->code,pSaomiao->nextstat,pSaomiao->tran_types,pComm->userId);
		
		dataLen = strlen(pSend);
	}
	
	strcpy(&pSend[dataLen], "</senddata>");
	dataLen += strlen("</senddata>");
	
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
#if 0	
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
    tRequest.content_type   = "text/xml; charset=GBK";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("FaJianQR send = ");
hyUsbMessageByLen(pSend, strlen(pSend));
hyUsbPrintf("\r\n");
	cnt = http_send_request(pUrl, bufLen, &tRequest);
	if(cnt > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		
		cnt = Net_DecodeData(pSend,cnt,pGbk);
		if(cnt > 0)
		{
			memcpy(pSend,pGbk,cnt);
			pSend[cnt] = 0;
			pSend[cnt+1] = 0;
	hyUsbPrintf("FaJianQR recv = %s \r\n",pSend);	
			pTemp = strstr(pSend, "<result>");
			pE = strstr(pSend, "</result>");
			if(pTemp != NULL && pE != NULL)
			{
				pTemp += strlen("<result>");
				dataLen = pE - pTemp;
				if(dataLen > 4) dataLen = 4;
				memcpy(buf,pTemp, dataLen);
				buf[dataLen] = 0;
				ret = atoi(buf);
			}
		}
	}
hyUsbPrintf("FaJianQR ret = %d \r\n",ret);	
	
	return ret;
}

#endif //#ifdef VER_CONFIG_HYCO	//浩创