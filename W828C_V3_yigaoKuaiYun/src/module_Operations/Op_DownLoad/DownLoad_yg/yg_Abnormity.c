/**********************************************************************
						异常类型下载
**********************************************************************/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "AbstractFileSystem.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"



U32		gu32AbnormalBufSize;
U32 	gu32AbnormalTotal;//下载异常类型总数
tABNORMAL *gptAbnormal;//存放异常类型

/*---------------------------------------------------------------------
*函数名：Op_Abnormity_Init
*功能：初始化异常信息列表信息
*参数：
*返回：
----------------------------------------------------------------------*/
int Op_Abnormity_Init(void)
{
	int fp,size;
	U16 crc;

	gu32AbnormalTotal = 0;
	gptAbnormal = (tABNORMAL *)hsaSdram_AbnormalBuf();
	gu32AbnormalBufSize = hsaSdram_AbnormalSize();
	
	fp = AbstractFileSystem_Open(ABNORMITY_FILE_PATH, AFS_READ);
	if (fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= 2 || size >= gu32AbnormalBufSize)
		{
			AbstractFileSystem_Close(fp);
			return -1;
		}
	
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		size -= 2;
		AbstractFileSystem_Read(fp,(char *)gptAbnormal,size);
		AbstractFileSystem_Close(fp);
		
		gu32AbnormalTotal = size/sizeof(tABNORMAL);
		
		if(crc != glbVariable_CalculateCRC((U8 *)gptAbnormal,size))
		{//文件出错
			AbstractFileSystem_DeleteFile(ABNORMITY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			gu32AbnormalTotal = 0;
		}
	}
	
	return 0;
}

/*****************************************************
* 函数：DownLoad_ParseAbnormal
* 功能：解析网络上返回的员工信息数据并存成文件
* 参数：pSend 网络返回的数据 utf-8编码
* 返回：0 成功  -1 失败
******************************************************/
int DownLoad_ParseAbnormal(U8 *pSend, int buflen)
{
	int ret = 0,len,size;
	int fp;
	U16 crc,readcrc;
	U8  *pStart,*pTmp;
	U8 *poS, *poE, *pdS, *pdE, *pdS1, *pdE1, *ps;
	int lenCode, lenName;
	
	gu32AbnormalTotal = 0;
	memset(&gptAbnormal[0],0,sizeof(tABNORMAL));
	
	pTmp = strstr(pSend,"st=\"ok");
	if(pTmp != NULL)
	{
		ps = pSend;
		poS = strstr(ps, "<o>");
		while(poS)
		{
			poE = strstr(ps, "</o>");
			if (poE)
			{
				pdS = strstr(ps, "<d>");
				if (pdS)
				{
					pdE = strstr(ps, "</d>");
					if (pdE)
					{
						ps = pdE+4;
						pdS1 = strstr(ps, "<d>");
						if (pdS1)
						{
							pdE1 = strstr(ps, "</d>");
							if (pdE1)
							{
								lenCode = pdE - pdS - 3;
								lenName = pdE1 - pdS1 - 3;
																
								if (lenCode > 0 && lenName > 0 && gu32AbnormalTotal < gu32AbnormalBufSize/sizeof(tABNORMAL))
								{
									if (lenCode > 6) lenCode = 6;
									memcpy(gptAbnormal[gu32AbnormalTotal].code, (pdS+3), lenCode);
									gptAbnormal[gu32AbnormalTotal].code[lenCode] = 0;
									
									if (lenName > 30) lenName = 30;
									memcpy(gptAbnormal[gu32AbnormalTotal].name, (pdS1+3), lenName);
									gptAbnormal[gu32AbnormalTotal].name[lenName] = 0;
									gu32AbnormalTotal++;
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
			
			ps = poE+4;
			poS = strstr(ps, "<o>");
		}
		
		//把解析出来的数据写入文件
		if(gu32AbnormalTotal > 0)
		{
			size = gu32AbnormalTotal*sizeof(tABNORMAL);
			crc  = glbVariable_CalculateCRC((U8 *)gptAbnormal,size);
			
			fp = AbstractFileSystem_Open(ABNORMITY_FILE_PATH,AFS_READ);
			if(fp >= 0)
			{
				AbstractFileSystem_Read(fp,(char *)&readcrc,sizeof(U16));
				AbstractFileSystem_Close(fp);
				if(crc == readcrc)//和之前保存的crc相同,不要更新
				{
					return 0;
				}
				AbstractFileSystem_DeleteFile(ABNORMITY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			}
			fp = AbstractFileSystem_Create(ABNORMITY_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Write(fp,(char *)gptAbnormal,size);
				AbstractFileSystem_Close(fp);
			}
		}
	}
	else
	{
		return -1;
	}
	
	return ret;
}


int DownLoad_DownAbnormal(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	U8				*pSub;
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
	
	NetReq_FillRequestHead(REQ_YICHANGSTR,Login_GetReqHead(),glbVar_GetCompanyId(),Login_GetUserId());
	sprintf(pSend, "<req op=\"%s\">%s</req>", Req_name[REQ_YICHANGSTR],Login_GetReqHead());
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 1;
	tRequest.gzip			= 1;
	
	hyUsbPrintf("gtHyc.url = %s  \r\n send (front) = %s \r\n", gtHyc.url, pSend);
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	if(recvLen > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		recvLen = Net_DecodeData(pSend,recvLen,pGbk);
		if(recvLen > 0)
		{
			memcpy(pSend,pGbk,recvLen);
			pSend[recvLen] = 0;
			pSend[recvLen+1] = 0;
			
			pSub = strstr(pSend,"</dta>");
			if(pSub == NULL)
			{//失败
				return 20;
			}
			
			pSub = strstr(pSend, "ok");
			if(pSub != NULL)//成功
			{
				//解析数据
				if(-1 == DownLoad_ParseAbnormal(pSend, bufLen))
				{
					return 11;
				}
				return 1;
			}
			else
			{//需要返回失败原因
				pSub = strstr(pSend,"ercode=\"");
				pSub += 8;
				ret = *pSub - 0x30;
				
				return (ret+20);
			}
		}
		else
		{
			ret = 4;
		}
	}
	else
	{
		ret = 3;
	}
	
	return ret;
}

/**************************************************************************************************
									异常类型下载列表
**************************************************************************************************/
const tGrapViewAttr Op_Abnormity_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
#endif	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapListerPriv Op_Abnormity_ListPriv[]=
{
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv Op_Abnormity_BtnPriv[]=
{
#ifdef W818C
	{"更新",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#else
	{"更新",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#endif
};


S32 Op_Abnormity_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://更新
		if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("确定更新异常类型列表?",0,pDesk,3,NULL,NULL,800))
		{
			break;
		}
		
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			break;
		}
		lastValue = BackLight_CloseScreenProtect();//关闭屏保
		
		pSend = (U8 *)hsaSdram_UpgradeBuf();
		tUp.pdata   = pSend;
		tUp.datalen = hsaSdram_UpgradeSize();
		
		cartoon_app_entry(DownLoad_DownAbnormal,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在下载,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		Net_DelEvt(NET_FOREGROUND_EVT);
		BackLight_SetScreenProtect(lastValue);//打开屏保
		
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)
			{
				pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
				if (gu32AbnormalTotal <= 0)
					pList->lst_privattr.totalItem = 1;
				else
					pList->lst_privattr.totalItem = gu32AbnormalTotal;
				
				Com_SpcDlgDeskbox("更新完成!",0,pDesk,1,NULL,NULL,100);
			}
			else if(err == 11)
			{
				Com_SpcDlgDeskbox("数据下载错误!",0,pDesk,1,NULL,NULL,100);
			}
			else if(err == 2)
			{
				Com_SpcDlgDeskbox("网络通讯错误!",6, pDesk, 2,NULL,NULL,100);
			}
			else if(err == 3)
			{
				Com_SpcDlgDeskbox("网络连接失败,请稍候再试!",6, pDesk, 2,NULL,NULL,100);
			}
			else
			{
				err -= 20;
				if(err < 1 || err > 6) err = 0;
				Com_SpcDlgDeskbox(Net_ErrorCode[err],6,pDesk,2,NULL,NULL,100);
			}
		}
		else
		{
			Com_SpcDlgDeskbox("网络通讯错误,请稍候再试!",6, pDesk, 2,NULL,NULL,100);
		}
		break;
	case BUTTON_ID_2://ESC
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Op_Abnormity_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if (gu32AbnormalTotal <= 0)
		strcpy(str, "请更新异常类型");
	else
		sprintf(str, "%s|%s", gptAbnormal[i].code, gptAbnormal[i].name);
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: Op_DownLoad_Abnormity_Desk
*功能: 异常类型下载界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Op_DownLoad_Abnormity_Desk(void *pDeskFather, U8 ifSearch)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tGRAPLIST       *pList ;
	U8              index  ;
	int 			i;
		
	index = 0 ;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&Op_Abnormity_ViewAttr[index++], "异常原因");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	for (i=0; i<2; i++)
	{
		pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&Op_Abnormity_ViewAttr[index++],(tGrapButtonPriv*)&Op_Abnormity_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		pBttn->pressEnter = Op_Abnormity_ButtonEnter;
	}
	
	pList = Grap_InsertLister(pDesk,(tGrapViewAttr*)(&Op_Abnormity_ViewAttr[index++]),(tGrapListerPriv*)(&Op_Abnormity_ListPriv[0]));
	if (pList == NULL) return;
	pList->getListString = Op_Abnormity_ListGetStr;
	if (gu32AbnormalTotal <= 0)
		pList->lst_privattr.totalItem = 1;
	else
		pList->lst_privattr.totalItem = gu32AbnormalTotal;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
