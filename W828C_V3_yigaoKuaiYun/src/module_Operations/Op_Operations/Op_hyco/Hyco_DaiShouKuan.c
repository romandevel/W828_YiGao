/*----------------------------------
代收货款
-----------------------------------*/

#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//浩创版本

const U8 *const gcPayType[]=
{
	"现付",	"到付",	"月结"
};

/*==========================================代收货款界面==========================================*/
const tGrapViewAttr DaiShou_ViewAttr[] =
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
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//收件员编号
	{BUTTON_ID_11,	   5,  60 ,64, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//支付方式
	{BUTTON_ID_12,	   5,  85 ,64, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//代收金额
	{BUTTON_ID_13,	   5, 110 ,32, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运费
	{BUTTON_ID_14,	 120, 110 ,32, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//保价
	{BUTTON_ID_15,	   5, 135 ,64, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//客户编号
	{BUTTON_ID_16,	   5, 160 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运单号
	
	{EDIT_ID_1,	   		55, 35,83,22,    55,35,138,57, 0,0,0xffff,0,1,1,1,0},//收件员 8
	{COMBOBOX_ID_1,	    75, 60,158,22,   75,60,233,82,  0,0,0xffff,0,1,0,1,0},//支付方式
	{EDIT_ID_2,	   		75, 85,158,22,   75,85,233,107, 0,0,0xffff,0,1,0,1,0},//代收金额 8
	{EDIT_ID_3,	  		40,110,74,22,    40,110,115,132, 0,0,0xffff,0,1,0,1,0},//运费
	{EDIT_ID_4,	  	   159,110,74,22,   150,110,233,132, 0,0,0xffff,0,1,0,1,0},//保价
	{EDIT_ID_5,	   		75,135,158,22,   75,135,233,157, 0,0,0xffff,0,1,0,1,0},//客户编号
	{EDIT_ID_6,	   		75,160,158,22,   75,160,233,182, 0,0,0xffff,0,1,0,1,0},//运单号 21
	
	{EDIT_ID_20,   140,35 ,100,22,   0,0,0,0,     0,0,0xCE59,0,1,0,0,0},
};

const tGrapButtonPriv DaiShou_BtnPriv[]=
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
	
	{"收件员",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"支付方式",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"代收金额",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运费",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"保价",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"客户编号",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运 单 号",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv DaiShou_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, EMPLOYEE_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//收件员
	{NULL,0x0,0,0,2,4,8,   1, 8,  				0,0,1,0,0,1,  50,50,0,  0,0,1,0},//代收金额
	{NULL,0x0,0,0,2,4,8,   1, 8,  				0,0,1,0,0,1,  50,50,0,  0,0,1,0},//运费
	{NULL,0x0,0,0,2,4,8,   1, 8,  				0,0,1,0,0,1,  50,50,0,  0,0,1,0},//保价
	{NULL,0x0,0,0,2,4,19,  1, CUSTOMER_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//客户编号
	{NULL,0x0,0,0,2,4,19,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
	
	{gtOpInfoGlb.employee,0x0,0,0,2,4,12,  1, EMPLOYEE_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0},//
};

const tComboboxPriv DaiShou_ComAttr[]=
{
	{0,6,0,0,0,0,    1,138,16,158,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//运输方式
};

int DaiShou_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT		*pEditCost,*pEditTranCost,*pEditProtectCost,*pEditCustomer;
	tGRAPCOMBOBOX	*pCombox;
	tDATA_INFO 		Recd;
	int ret;
	U8	*pCode,*pTemp,buf[32];
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		pEditCost = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
		pEditTranCost = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
		pEditProtectCost = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
		pEditCustomer = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_5);
		
		if(gpEdit_Employee==NULL || gpEdit_Code==NULL || pEditCost == NULL || 
		   pEditTranCost==NULL || pEditProtectCost==NULL || pEditCustomer==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			if(gpEdit_Employee->view_attr.curFocus == 1)
			{
				gpEdit_Employee->editenter(gpEdit_Employee);
				break;
			}
			
			if(gpEdit_Code->view_attr.curFocus == 1)
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				
				//代收金额
				if(strlen(pEditCost->edit_privattr.digt) == 0)
				{
					gpEdit_Code->view_attr.curFocus = 0;
					pEditCost->view_attr.curFocus = 0;
					Com_SpcDlgDeskbox("请输入代收金额!",0,pDesk,1,NULL,NULL,100);
					break;
				}
				//运费
				if(strlen(pEditTranCost->edit_privattr.digt) == 0)
				{
					gpEdit_Code->view_attr.curFocus = 0;
					pEditTranCost->view_attr.curFocus = 0;
					Com_SpcDlgDeskbox("请输入运费!",0,pDesk,1,NULL,NULL,100);
					break;
				}
				
				//保价
				if(strlen(pEditProtectCost->edit_privattr.digt) == 0)
				{
					gpEdit_Code->view_attr.curFocus = 0;
					pEditProtectCost->view_attr.curFocus = 0;
					Com_SpcDlgDeskbox("请输入保价!",0,pDesk,1,NULL,NULL,100);
					break;
				}
				
				//客户编号
				if(strlen(pEditCustomer->edit_privattr.digt) == 0)
				{
					gpEdit_Code->view_attr.curFocus = 0;
					pEditCustomer->view_attr.curFocus = 0;
					Com_SpcDlgDeskbox("请输入客户编号!",0,pDesk,1,NULL,NULL,100);
					break;
				}
				
				//判断单号
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					if(strlen(gtOpInfoGlb.employee) == 0)
					{
						Com_SpcDlgDeskbox("请先输入正确的员工!",0,pDesk,1,NULL,NULL,100);
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
						strcpy(Recd.uData.daishou.collect_id, gpEdit_Employee->edit_privattr.digt);
						sprintf(Recd.uData.daishou.pay_type, "%d", gtOpInfoGlb.tempIndex);
						strcpy(Recd.uData.daishou.pay_name,gcPayType[gtOpInfoGlb.tempIndex]);
						strcpy(Recd.uData.daishou.cost_trans, pEditTranCost->edit_privattr.digt);
						strcpy(Recd.uData.daishou.money, pEditCost->edit_privattr.digt);
						strcpy(Recd.uData.daishou.cost_protect, pEditProtectCost->edit_privattr.digt);
						strcpy(Recd.uData.daishou.customer_id, pEditCustomer->edit_privattr.digt);
						
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
					//memset(pEditTranCost->edit_privattr.digt, 0, pEditTranCost->edit_privattr.containLen);
					//memset(pEditCost->edit_privattr.digt, 0, pEditCost->edit_privattr.containLen);
					//memset(pEditProtectCost->edit_privattr.digt, 0, pEditProtectCost->edit_privattr.containLen);
					memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				}
				else
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
			}
			else
			{
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
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

S32 DaiShou_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	strcpy(pStr, gcPayType[i]);
	
	return SUCCESS;
}

S32 DaiShou_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->list_opened == 1)
	{
		gtOpInfoGlb.tempIndex = i;
		Grap_ChangeFocus(pCombox,1);
	}
	
	return SUCCESS;
}


U32 DaiShou_EditKeyEnd(void *pView, U8 type)
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

S32 DaiShou_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	//编辑框没有输入任何数据
	pCode = pEdit->edit_privattr.digt;
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
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*函数:Operation_DaiShou_Desk
*功能:收件
*参数:
*返回:
*-----------------------------------------------*/
int Operation_DaiShou_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&DaiShou_ViewAttr[idx++], "代收付款");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_6;
	
	for (i=0; i<10; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &DaiShou_ViewAttr[idx++], &DaiShou_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 3)
		{
			pBtn->pressEnter = DaiShou_BtnPress;
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &DaiShou_ViewAttr[idx++], &DaiShou_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(NULL == pEdit ) return -1;
	pEdit->editenter  = DaiShou_EditEnter;
	pEdit->editKeyEnd = DaiShou_EditKeyEnd;
	gpEdit_Employee = pEdit;
	//默认是登录人
	strcpy(pEdit->edit_privattr.digt, Login_GetUserId());
	
	pComBox = Grap_InsertCombobox(pDesk, &DaiShou_ViewAttr[idx++], &DaiShou_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(gcPayType)/sizeof(U8 *);
	pComBox->comboxgetstr = DaiShou_ComboboxGetStr;
	pComBox->comboxenter  = DaiShou_ComboxEnter;
	
	for (i=1; i<7; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &DaiShou_ViewAttr[idx++], &DaiShou_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i < 4)
		{
			pEdit->handle = OpComm_EditHandle_Price;
			strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(i == 5)//运单号
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 6)
		{
			strcpy(pEdit->edit_privattr.digt, Login_GetName());
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 190, 220, 80);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*-----------------------------------------记录查询显示------------------------------------------------*/
const U8* const opType_DaiShou[]=//代收付款
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"员工编号:",
	"扫描时间:",
	
	"收件员:",
	"支付类型:",
	"代收金额:",
	"运费:",
	"保价:",
	"客户编号:",
	"单号:",
};


/*---------------------------------------------------------------
*函数: DaiShou_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int DaiShou_SearchDisplay(void *p)
{
	U8 *str[]={"未传", "已传", "已删", "未传", "未传"};
	U8 *pDispStr;
	tOP_DISPINFO 	*pOpCb;
	tDATA_INFO		*pData;
	tDATA_COMM 		*pComm;
	tDAISHOU		*daishou;
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
		daishou = &(pData[i].uData.daishou);
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_DaiShou[0], str[stat],
							opType_DaiShou[1], pComm->opType,
							opType_DaiShou[2], pComm->scanStat,
							opType_DaiShou[3], pComm->userId,
							opType_DaiShou[4], pComm->scanTime,
							
							opType_DaiShou[5], daishou->collect_id,
							opType_DaiShou[6], daishou->pay_name,
							opType_DaiShou[7], daishou->money,
							opType_DaiShou[8], daishou->cost_trans,
							opType_DaiShou[9], daishou->cost_protect,
							opType_DaiShou[10], daishou->customer_id,
							opType_DaiShou[11], pComm->code
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
/*
S:单号
L:扫描人
W:扫描网点
D:扫描时间
PS: 收件业务员
PE: 问题件类型/代收付款 支付类型：现付，到付，月结
PD: 问题件描述/代收付款运费	为空empty
LS: 留仓业务员/代收付款代收金额 为空empty
LD: 留仓件原因/代收付款保价金额为空empty
PN: 袋号/代收付款 客户编号
CT=ZTO*VE=100*DE=0*VT=1*T=13*S=123456789012*L=57171.001*W=57171*D=20110819092710*
PS =57171.001*PE=现付*PD=100.00*LS=99.00*LD=empty*PN=20012061511012451012

*/
/*---------------------------------------------------------------
*函数: DaiShou_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int DaiShou_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tDAISHOU	*daishou;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;
	
return 0;
	
	//需要一个临时的buf先把数据段组织起来,可以批量上传,最大50包,包之间用\r\n分割
	dataLen = 0;
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		daishou = &(pNode[i].tData.uData.daishou);
		
		sprintf(&pTemp[dataLen], "CT=HYCO*VE=100*DE=0*VT=1*T=%02d*S=%s*L=%s*W=%s*D=%s*PS=%s*PE=%s*PD=%s*LS=%s*LD=%s*PN=%s\r\n",
					pComm->opType,pComm->code,pComm->userId,pComm->scanStat,pComm->scanTime,
					daishou->collect_id,daishou->pay_name,daishou->cost_trans,daishou->money,daishou->cost_protect,daishou->customer_id);
		
		dataLen = strlen(pTemp);
	}
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
	dataLen-=2;//去掉最后的\r\n
	pTemp[dataLen] = 0;
	pTemp[dataLen+1] = 0;
	
	i = NetReq_FillHead_ZTO(pSend, "G006");
	sprintf(&pSend[i], "<d><h>%d</h><p><a>%s</a></p></d></root>",dataLen,pTemp);
	
	//编码转换
	//发送前需要先转成unicode  再转成utf-8
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	//1 转unicode
	Hyc_Gbk2Unicode(pSend,pTemp,0);
	//2 转utf-8
	Unicode2UTF8(pTemp, pSend);
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "multipart/form-data;charset=UTF8";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("paijian send = ");
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
	hyUsbPrintf("paijian recv = %s \r\n",pSend);
			pTemp = strstr(pSend, "<Success>");
			if(pTemp != NULL)
			{
				pTemp = strstr(pSend, "<d><h>");
				pE = strstr(pSend, "</h></d>");
				if(pTemp && pE)
				{
					pTemp += 6;
					sprintf(buf,"%d",dataLen);
				hyUsbPrintf("buf = %s   %d  ret = %d \r\n",buf,dataLen,ret);
					if(memcmp(buf,pTemp,strlen(buf)) == 0)//发送数据长度和返回长度不一致
					{
						ret= 0;
					}
				}
			}
		}
	}
hyUsbPrintf("paijian ret = %d \r\n",ret);	
	return ret;
}

#endif //浩创版本