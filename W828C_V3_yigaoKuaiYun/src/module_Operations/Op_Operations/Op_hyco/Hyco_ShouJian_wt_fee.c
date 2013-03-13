/*----------------------------------
收件操作 (带运费  称重)
界面显示:
收件员
运费
重量
运单号
-----------------------------------*/

#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//浩创版本

/*==========================================收件界面==========================================*/
const tGrapViewAttr ShouJianWtFee_ViewAttr[] =
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
	{BUTTON_ID_4,	   165,86, 70,20,    165,86,238,106,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//蓝牙
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//收件员编号
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运  费
	{BUTTON_ID_12,	 140,  60 ,16, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//元
	{BUTTON_ID_13,	   5,  85 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//重  量
	{BUTTON_ID_14,	 140,  85 ,16, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//Kg
	{BUTTON_ID_15,	   5, 110 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运单号
	
	{EDIT_ID_1,	   55, 35,83,22,    55,35,138,57, 0,0,0xffff,0,1,0,1,0},//收件员 8
	{EDIT_ID_2,	   55, 60,83,22,    55,60,138,82, 0,0,0xffff,0,1,1,1,0},//运费
	{EDIT_ID_3,    55, 85,83,22,    55,85,138,107, 0,0,0xCE59,0,1,0,0,0},//重量
	{EDIT_ID_4,	   55,110,172,22,   55,110,227,132, 0,0,0xffff,0,1,0,1,0},//运单号 21
	
	{EDIT_ID_20,   140,35 ,100,22,   140,35,240,57,     0,0,0xCE59,0,1,0,0,0},//收件员名称,与编号对应   12
};

const tGrapButtonPriv ShouJianWtFee_BtnPriv[]=
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
	{"蓝牙",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//
	
	{"收件员",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运  费",  0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"元",  	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"重  量",  0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"Kg",  	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运单号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv ShouJianWtFee_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, EMPLOYEE_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//网点/业务员
	{gtOpInfoGlb.fee1,     0x0,0,0,2,4,9,  1, FEE_LEN-1,		0,0,1,0,0,1,  50,50,0,  1,0,1, 0},//运费
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//重量
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
	
	{gtOpInfoGlb.employee,0x0,0,0,2,4,12,  1, EMPLOYEE_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  1,0,0,0},//
};


int ShouJianWtFee_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEditWT, *pEditFee;
	tDATA_INFO 		Recd;
	int ret;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		pEditFee= (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
		pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
		if(gpEdit_Employee==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			if(gpEdit_Employee->view_attr.curFocus == 1)
			{
				if(strlen(gtOpInfoGlb.employee) == 0)
				{
					gpEdit_Employee->editenter(gpEdit_Employee);
				}
				else
				{
					Grap_ChangeFocus(gpEdit_Employee,1);
				}
				break;
			}
			
			if(gpEdit_Code->view_attr.curFocus == 1 || (strlen(gtOpInfoGlb.weight) && strlen(gtOpInfoGlb.fee1)))
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
					break;
				}
				
				//判断单号
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					if(strlen(gtOpInfoGlb.employee) == 0)
					{
						Com_SpcDlgDeskbox("请先输入正确的员工信息!",0,pDesk,1,NULL,NULL,100);
						memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						break;
					}
					
					if(strlen(gtOpInfoGlb.fee1) == 0)
					{
						Com_SpcDlgDeskbox("费用不能为空!",0,pDesk,1,NULL,NULL,100);
						break;
					}
					
					if(strlen(gtOpInfoGlb.weight) == 0)
					{
						Com_SpcDlgDeskbox("重量不能为空!",0,pDesk,1,NULL,NULL,100);
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
						strcpy(Recd.uData.saomiao.daishou_fee, gtOpInfoGlb.fee1);
						strcpy(Recd.uData.saomiao.obj_wei, gtOpInfoGlb.weight);
						strcpy(Recd.uData.saomiao.deliverer, gpEdit_Employee->edit_privattr.digt);
					
						//写记录
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(pCode);
						}
						else
						{
							Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,100);
						}
					}
					memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				}
				else if(strlen(pCode) > 0)
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
				if(gpEdit_Code->view_attr.curFocus != 1)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
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

U32 ShouJianWtFee_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//业务员
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Employee_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
		}
		
		if(ret == 0)
		{
			memset(gtOpInfoGlb.employee, 0, sizeof(gtOpInfoGlb.employee));
		}

		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	
	return SUCCESS ;
}

S32 ShouJianWtFee_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	if(pEdit->view_attr.id == EDIT_ID_1)
	{
		if(strlen(gtOpInfoGlb.employee) == 0)
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Op_DownLoad_Employee_Desk(pDesk, 1);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			//这里没有判断ret, 因为Employee_GetIdNameByIdx里面有判断
			Employee_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
			pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
			
			if(ret != -1)
			{
				Grap_ChangeFocus(pEdit,1);
			}
		}
		else
		{
			return FAIL;
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_20)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Op_DownLoad_Employee_Desk(pDesk, 1);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		//这里没有判断ret, 因为Employee_GetIdNameByIdx里面有判断
		Employee_GetIdNameByIdx(ret, gpEdit_Employee->edit_privattr.digt, gpEdit_Employee->edit_privattr.containLen, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
		gpEdit_Employee->edit_privattr.Pos = strlen(gpEdit_Employee->edit_privattr.digt);
		
		if(ret != -1)
		{
			Grap_ChangeFocus(gpEdit_Employee,1);
			//Grap_ChangeFocusEnable(gpEdit_Code);
		}
	}
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*函数:Operation_ShouJianWtFee_Desk
*功能:收件
*参数:
*返回:
*-----------------------------------------------*/
int Operation_ShouJianWtFee_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&ShouJianWtFee_ViewAttr[idx++], "收件扫描");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_4;
	
	for (i=0; i<10; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &ShouJianWtFee_ViewAttr[idx++], &ShouJianWtFee_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = ShouJianWtFee_BtnPress;
			if(i == 3)
			{
				pBtn->draw = OpComm_BtnBTDraw;
			}
		}
	}
	
	//员工编号   运费  重量  运单号  员工姓名
	for (i=0; i<5; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &ShouJianWtFee_ViewAttr[idx++], &ShouJianWtFee_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//收件员
		{
			pEdit->editenter  = ShouJianWtFee_EditEnter;
			pEdit->editKeyEnd = ShouJianWtFee_EditKeyEnd;
			gpEdit_Employee = pEdit;
			//默认是登录人
			strcpy(pEdit->edit_privattr.digt, Login_GetUserId());
		}
		else if(i == 1)//运费
		{
			pEdit->handle = OpComm_EditHandle_Price;
			//strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(i == 2)//重量
		{
			pEdit->handle = OpComm_EditHandle_Price;
			pEdit->editTimer = BT_WeightEditTimer;
			//strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(i == 3)//运单号
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 4)//员工姓名
		{
			pEdit->editenter = ShouJianWtFee_EditEnter;
			pEdit->editTimer = OpComm_EditTimer;
			strcpy(pEdit->edit_privattr.digt, Login_GetName());
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 135, 220, 140);
	
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
const U8* const opType_ShouJianWtFee[]=//收件
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"扫描员工:",
	//"员工分组:",
	"扫描时间:",
	
	"收件员:",
	"运费:",
	"重量:",
	"单号:",
};


/*---------------------------------------------------------------
*函数: ShouJianWtFee_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int ShouJianWtFee_SearchDisplay(void *p)
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
							opType_ShouJianWtFee[0], str[stat],
							opType_ShouJianWtFee[1], pComm->opType,
							opType_ShouJianWtFee[2], pComm->scanStat,
							opType_ShouJianWtFee[3], pComm->userId,
							opType_ShouJianWtFee[4], pComm->scanTime,
							
							opType_ShouJianWtFee[5], saomiao->deliverer,
							opType_ShouJianWtFee[6], saomiao->daishou_fee,
							opType_ShouJianWtFee[7], saomiao->obj_wei,
							opType_ShouJianWtFee[8], pComm->code
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
//http://120.90.2.126:8888/yiDaSystem/getAccessMethod.do
/*
<?xml version= "1.0" encoding="utf-8"?>
<senddata><accessMethod>loginPda</accessMethod>
<baseValue>
<dev>0</dev>
<userName></userName>
<password></userName>
<belongSite></belongSite>
</baseValue>
<detailValue><scanType></scanType><scanUser></scanUser><scanSite></scanSite><scanTime></scanTime><scanHawb></scanHawb>\
<preNextSite></preNextSite><shiftTimes></shiftTimes><weight></weight><bagNumber></bagNumber><exceptionCode></exceptionCode>\
<exceptionMemo></exceptionMemo><signatureType></signatureType><signAture></signAture><businessMan></businessMan></detailValue>
</senddata>
*/

/*---------------------------------------------------------------
*函数: ShouJianWtFee_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int ShouJianWtFee_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*saomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[20],da[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

return 0;//test

	//可以批量上传	
	//填充请求头
	pComm = &(pNode[0].tData.tComm);
	dataLen = NetReq_FillHead_DHL(pSend, pComm->opType, NULL);
	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		memcpy(da, pComm->scanTime, 8);
		da[8] = 0;
		saomiao = &(pNode[i].tData.uData.saomiao);

		sprintf(&pSend[dataLen], "<data>%02d%-14s%-14s%-1s%-1s%-16s%-15s%-8s%-1s%-7s%-8s%-1s%-10s%-10s</data>",
					pComm->opType, saomiao->deliverer, pComm->scanTime, " "," ",pComm->code, pComm->userId,da,
					" ",saomiao->obj_wei,saomiao->daishou_fee," "," "," ");
		
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
    tRequest.content_length = dataLen;
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=GBK";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("ShouJianWtFee send = ");
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
	hyUsbPrintf("ShouJianWtFee recv = %s \r\n",pSend);	
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
hyUsbPrintf("ShouJianWtFee ret = %d \r\n",ret);	
	
	return ret;//先测试通过
}

#endif //浩创版本