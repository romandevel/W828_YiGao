/*--------------------------------------------------------------
运单到达该站点/网点
到件操作
界面显示
上一站
运单号
---------------------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//浩创版本

/*==========================================到件界面==========================================*/
const tGrapViewAttr DaoJian_ViewAttr[] =
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
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//上一站
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运单号
	
	{EDIT_ID_1,	   55, 35,83,22,    55,35, 138,57, 0,0,0xffff,0,1,1,1,0},//上一站
	{EDIT_ID_2,	   55, 60,172,22,    55,60, 227,82, 0,0,0xffff,0,1,0,1,0},//运单号
	
	{EDIT_ID_20,   140,35 ,100,22,   0,0,0,0,     0,0,0xCE59,0,1,0,0,0},
};

const tGrapButtonPriv DaoJian_BtnPriv[]=
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
	
	{"上一站",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"运单号",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv DaoJian_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, COMPANY_ID_LEN-1,   0,0,2,0,0,1,  50,50,0,  0,1,1,1},//上一站
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//运单号
	
	{gtOpInfoGlb.station1,0x0,0,0,2,4,12,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0,0},//
};


int DaoJian_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tDATA_INFO 		Recd;
	int ret;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if(gpEdit_Stat1==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			if(gpEdit_Stat1->view_attr.curFocus == 1)
			{
				gpEdit_Stat1->editenter(gpEdit_Stat1);
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
					if(strlen(gtOpInfoGlb.station1) == 0)
					{
						Com_SpcDlgDeskbox("请先输入正确的站点!",0,pDesk,1,NULL,NULL,100);
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
				else
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
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

U32 DaoJian_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//网点
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
	
	return SUCCESS ;
}

S32 DaoJian_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	pCode = pEdit->edit_privattr.digt;
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
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*函数:Operation_DaoJian_Desk
*功能:收件
*参数:
*返回:
*-----------------------------------------------*/
int Operation_DaoJian_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&DaoJian_ViewAttr[idx++], "到件扫描");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0; i<5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &DaoJian_ViewAttr[idx++], &DaoJian_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 3)
		{
			pBtn->pressEnter = DaoJian_BtnPress;
		}
	}
	
	//上一站编号  运单号   上一站名称
	for (i=0; i<3; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &DaoJian_ViewAttr[idx++], &DaoJian_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//上一站
		{
			pEdit->editenter  = DaoJian_EditEnter;
			pEdit->editKeyEnd = DaoJian_EditKeyEnd;
			gpEdit_Stat1 = pEdit;
		}
		else if(i == 1)//运单号
		{
			gpEdit_Code = pEdit;
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 90, 220, 180);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*-----------------------------------------记录查询显示------------------------------------------------*/
const U8* const opType_DaoJian[]=//发件
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"员工编号:",
	//"员工分组:",
	"扫描时间:",
	
	"上一站:",
	"单号:",
};


/*---------------------------------------------------------------
*函数: DaoJian_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int DaoJian_SearchDisplay(void *p)
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
%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_DaoJian[0], str[stat],
							opType_DaoJian[1], pComm->opType,
							opType_DaoJian[2], pComm->scanStat,
							opType_DaoJian[3], pComm->userId,
							opType_DaoJian[4], pComm->scanTime,
							
							opType_DaoJian[5], saomiao->prevstat,
							opType_DaoJian[6], pComm->code
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
*函数: DaoJian_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int DaoJian_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*pSaomiao;
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
		pSaomiao = &(pNode[i].tData.uData.saomiao);
				
		sprintf(&pSend[dataLen], "<data>%02d%-14s%-14s%-1s%-1s%-16s%-15s%-8s%-1s%-7s%-8s%-1s%-10s%-10s</data>",
					pComm->opType,pSaomiao->prevstat,pComm->scanTime," "," ",pComm->code,pComm->userId,da,
					" "," "," "," "," "," ");
		
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

hyUsbPrintf("daojian send = ");
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
	hyUsbPrintf("daojian recv = %s \r\n",pSend);	
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
hyUsbPrintf("daojian ret = %d \r\n",ret);	
	
	return ret;
}

#endif //浩创版本