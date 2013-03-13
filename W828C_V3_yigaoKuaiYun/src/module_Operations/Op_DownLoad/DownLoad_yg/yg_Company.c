#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "AbstractFileSystem.h"
#include "Net_Evt.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"


char	ga8CompanyVer[COMPANY_VER_LEN];
U32		gu32CompanyBufSize;
U32 	gu32CompanyTotal;//下载的总个数

tCOMPANY	*gptCompany;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//全量下载时
//成功返回解析出的条数   失败 <=0
int DownLoad_CompanyAll(U8 *pGbk, tCOMPANY *pCompany, int curCnt)
{
	int			i,total;
	U8			*pStart,*pTmp;
	U8			companyId[COMPANY_ID_LEN], companyName[COMPANY_NAME_LEN];
	int			flag,len;
	
	total = curCnt;
	
	pStart = strstr(pGbk,"<usable>");
	while(pStart != NULL)
	{
		//标志
		pStart += strlen("<usable>");
		flag = pStart[0]-0x30;
		
		//编号
		memset(companyId, 0, sizeof(companyId));
		pStart = strstr(pStart,"<siteId>");
		pStart += strlen("<siteId>");
		pTmp   = strstr(pStart,"</siteId>");
		if(pTmp == NULL)
			return -1 ;
		len = pTmp - pStart;
		if(len >= COMPANY_ID_LEN) len = COMPANY_ID_LEN-1;
		if(len > 0)
			memcpy(companyId,pStart,len);
		
		//名称
		memset(companyName, 0, sizeof(companyName));
		pStart = strstr(pTmp,"<siteName>");
		pStart += strlen("<siteName>");
		pTmp   = strstr(pStart,"</siteName>");
		if(pTmp == NULL)
			return -1;
		len = pTmp - pStart;
		if(len >= COMPANY_NAME_LEN) len = COMPANY_NAME_LEN-1;
		if(len > 0)
			memcpy(companyName,pStart,len);	
	hyUsbPrintf("flag = %d  id = %s  name = %s \r\n", flag, companyId, companyName);	
		if(flag == 1)//新增
		{
			memset((char *)&pCompany[total],0,sizeof(tCOMPANY));
			
			//编号
			strcpy(pCompany[total].companyid,companyId);
			
			//名称
			strcpy(pCompany[total].company,companyName);			
			
			total++;
			if(total >= (gu32CompanyBufSize/sizeof(tCOMPANY))) break;
		}
		else if(flag == 2)//删除
		{
			for(i = 0; i < total; i++)
			{
				if(strcmp(pCompany[i].companyid, companyId) == 0)//找到要删除的公司
				{
					//把最后一个提到该位置即可
					memcpy(&pCompany[i], &pCompany[total-1], sizeof(tCOMPANY));
					total--;
					break;
				}
			}
		}
		else if(flag == 3)//修改
		{
			for(i = 0; i < total; i++)
			{
				if(strcmp(pCompany[i].companyid, companyId) == 0)//找到要修改的公司
				{
					strcpy(pCompany[i].companyid,companyId);
					strcpy(pCompany[i].company,companyName);
					break;
				}
			}
		}
		
		pStart = strstr(pTmp,"<usable>");
	}
	
	//找版本号
	pStart = strstr(pGbk,"<lastModifyTime>");
	pStart += strlen("<lastModifyTime>");
	pTmp   = strstr(pStart,"</lastModifyTime>");
	if(pStart && pTmp)
	{
		len = pTmp - pStart;
		if(len >= COMPANY_VER_LEN) len = COMPANY_VER_LEN-1;
		if(len > 0)
			memcpy(ga8CompanyVer, pStart, len);
		
		ga8CompanyVer[len] = 0;
	}
hyUsbPrintf("company ver = %s \r\n", ga8CompanyVer);	
	return total;
}



//1:成功   3:网络连接失败   11:已是最新版本  12:服务器返回数据格式有误    >20:服务器返回的错误
int DownLoad_DownCompany(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA *pUp;
	U8		*pSend, *pGbk;
	U8		*pS, *pE,range[20], ver[COMPANY_VER_LEN];
	int		bufLen, recvLen, total, len;
	int		ret = 1, err;
	int		start,end,step,getCnt=0,tempCnt;
	
	if(Net_ConnectNetWork() != 1)
	{
	hyUsbPrintf("sendRequest 连接失败\r\n");
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//网络连接失败
		return 3;
	}
		
	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
	
	sprintf(pSend,"<req op=\"UpdataStation\"><h><user>%s</user><stat>%s</stat><dev>%s</dev><flag>pda</flag></h><data><o><ver>%s</ver><pda>W818C</pda></o></data></req>", 
	        Login_GetUserId(),glbVar_GetCompanyId(),glbVar_GetMachineId(),LOAD_VER_NULL);
	strcat(&pSend[recvLen], "</senddata>");
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	hyUsbPrintf("login send = %s \r\n", pSend);

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
			
			pS = strstr(pSend,"<result>0");
			if(pS == NULL)
			{//失败
				OpCom_SetLastErrCode(ERR_OTHER+(pS[8]-'0'));//其他错误
				
				return 20+(pS[8]-'0');
			}
			
			pS = strstr(pSend,"<usable>");
			if(pS == NULL)
			{
			hyUsbPrintf("已经是最新  \r\n");
				OpCom_SetLastErrCode(ERR_NEWEST);
				return 11;
			}
			
			getCnt = DownLoad_CompanyAll(pSend, gptCompany, gu32CompanyTotal);
		hyUsbPrintf("company cnt = %d \r\n", getCnt);
			if(-1 == getCnt)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);
				return 12;
			}
			
			if(getCnt > 0)
			{
				DownLoad_Company_WriteFile(gptCompany, getCnt, ga8CompanyVer);
			}
			
			//更新站点名称
			Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
			
			OpCom_SetLastErrCode(ERR_SUCCESS);
			return 1;
		}
		else
		{
			OpCom_SetLastErrCode(ERR_SENDFAIL);
			return 2;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_SENDFAIL);
		return 2;
	}
	
	return ret;
}

int DownLoadWholeFile(void *p);
/*--------------------------------------------------
* 函数:DownLoad_Company_Action
* 功能:下载公司列表的界面执行函数
* 参数:
* 返回: 0:成功   其他失败,具体失败原因可以调用OpCom_GetLastErrCode得到
----------------------------------------------------*/
int DownLoad_Company_Action(tGRAPDESKBOX *pDesk)
{
	int				ret = -1;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	lastValue = BackLight_CloseScreenProtect();//关闭屏保
		
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	*pSend = 0;
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);				
	cartoon_app_entry(DownLoadWholeFile,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新资料,请稍候...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);//
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//打开屏保
	
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err == 1)//成功
		{
			ret = 0;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
	}
	
	return ret;
}

#if 0
/*========================================UI===================================================*/
/*----------------------------------------------------------
* 函数:Op_Company_Init
* 功能:用户信息初始化
* 参数:
* 返回:
------------------------------------------------------------*/
int Op_Company_Init()
{
	int fp,size;
	U16 crc;
	
	gu32CompanyTotal = 0;
	strcpy(ga8CompanyVer,"0");
	gptCompany = (tCOMPANY *)hsaSdram_CompanyBuf();
	gu32CompanyBufSize = hsaSdram_CompanySize();
	
	fp = AbstractFileSystem_Open(COMPANY_FILE_PATH, AFS_READ);
	if (fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= COMPANY_VER_LEN || size >= gu32CompanyBufSize)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(COMPANY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
		}
		else
		{
			AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
			AbstractFileSystem_Read(fp,(char *)ga8CompanyVer,COMPANY_VER_LEN);
			AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
			size -= (COMPANY_VER_LEN+2);
			AbstractFileSystem_Read(fp,(char *)gptCompany,size);
			AbstractFileSystem_Close(fp);
			
			if(crc != glbVariable_CalculateCRC((U8 *)gptCompany,size))
			{//文件出错
				AbstractFileSystem_DeleteFile(COMPANY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			}
			else
			{
				gu32CompanyTotal = size/sizeof(tCOMPANY);
			}
		}
	}
	
	return 0;
}
#else
/*****************************************************
* 函数：YD_ReadCompany
* 功能：从文件中读取公司数据
* 参数：none
* 返回：0 成功  -1 失败
******************************************************/
 U8	gaCompanyVer[32];

int Op_Company_Init()
{
	int ret = 0;
	int fp,size;
	U16 crc;
	
	memset(gaCompanyVer,0,32);
	gu32CompanyTotal = 0;
	gptCompany = (tCOMPANY *)hsaSdram_CompanyBuf();
	fp = AbstractFileSystem_Open(COMPANY_FILE_PATH,AFS_READ);	
	if(fp >= 0)
	{hyUsbPrintf("open ok\r\n");
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= 34 || size >= hsaSdram_CompanySize())
		{
			AbstractFileSystem_Close(fp);
			return -1;
		}
	
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)gaCompanyVer,32);
		size -= 34;
		AbstractFileSystem_Read(fp,(char *)gptCompany,size);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Close(fp);
		
		gu32CompanyTotal = size/sizeof(tCOMPANY);
		hyUsbPrintf("gu32CompanyTotal = %d\r\n",gu32CompanyTotal);
		if(crc != glbVariable_CalculateCRC((U8 *)gptCompany,size))
		{//文件出错
			gu32CompanyTotal = 0;
			memset(gaCompanyVer,0,32);
			strcpy(gaCompanyVer,LOAD_VER_NULL);
		}
	}
	else
	{hyUsbPrintf("open fail\r\n");
		strcpy(gaCompanyVer,LOAD_VER_NULL);
	}
	
	return ret;
}
#endif
int DownLoad_Company_WriteFile(tCOMPANY *pCompany, int cnt, U8 *ver)
{
	int fp;
	int size;
	U16 crc;
	
	if(cnt <= 0)
	{
		return -1;
	}
	
	gu32CompanyTotal = 0;
	fp = AbstractFileSystem_Open(COMPANY_FILE_PATH,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(COMPANY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
	}
	
	fp = AbstractFileSystem_Create(COMPANY_FILE_PATH);
	if(fp >= 0)
	{		
		//先写版本号
		AbstractFileSystem_Write(fp, ver, COMPANY_VER_LEN);

		//写crc
		size = cnt*sizeof(tCOMPANY);
		crc = glbVariable_CalculateCRC((U8 *)pCompany,size);
		AbstractFileSystem_Write(fp, (char *)&crc, sizeof(U16));
		
		//写数据
		AbstractFileSystem_Write(fp, (char *)pCompany, size);
		AbstractFileSystem_Close(fp);
		
		gu32CompanyTotal = cnt;
		memcpy((char *)gptCompany,(char *)pCompany,size);
	}
	
	return 0;
}

U32 DownLoad_Company_GetTotal()
{
	return gu32CompanyTotal;
}

/*--------------------------------------
*函数：Company_GetIdNameByIdx
*功能：通过索引获取公司ID和name
*参数：pID:(out)公司编号   pName:(out)名称    nameBufLen:pName  buf的长度
       如果pId为NULL  则不获取id  同样pName为NULL也不获取name
*返回：1:成功    0:失败
*--------------------------------------*/
int Company_GetIdNameByIdx(int idx, U8 *pId, int idBufLen, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	
	if(idx >= 0 && idx < gu32CompanyTotal)
	{
		if(pId != NULL)
		{
			if(strlen(gptCompany[idx].companyid) >= idBufLen)
			{
				memcpy(pId, gptCompany[idx].companyid, idBufLen-1);
			}
			else
			{
				strcpy(pId, gptCompany[idx].companyid);
			}
		}
		else
		{
			hyUsbPrintf("pId is null\r\n");
		}
		if(pName != NULL)
		{
			if(strlen(gptCompany[idx].company) >= nameBufLen)
			{
				memcpy(pName, gptCompany[idx].company, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptCompany[idx].company);
			}
		}
		else
		{
			hyUsbPrintf("pName is null\r\n");
		}
		ret = 1;
	}
	
	return ret;
}


/*--------------------------------------
*函数：Company_GetNameById
*功能：通过公司id得到对应的名称
*参数：pID:(in)公司编号   pName:(out)名称    nameBufLen:pName  buf的长度
*返回：1:成功    0:失败
*--------------------------------------*/
int Company_GetNameById(U8 *pId, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	if(strlen(pId) == 0)
	{
		return 0;
	}
	for(i = 0; i < gu32CompanyTotal; i++)
	{
		if(strcmp(pId, gptCompany[i].companyid) == 0)
		{
			if(strlen(gptCompany[i].company) >= nameBufLen)
			{
				memcpy(pName, gptCompany[i].company, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptCompany[i].company);
			}
			ret = 1;
			break;
		}
	}
	
	return ret;
}
U8 *Company_GetNameById2(U8 *pId)
{
	int i;
	if(strlen(pId) == 0)
	{
		return NULL;
	}
	for(i = 0; i < gu32CompanyTotal; i++)
	{
		if(strcmp(pId, gptCompany[i].companyid) == 0)
		{
			return gptCompany[i].company;
		}
	}
	
	return NULL;
}
/**************************************************************************************************
									公司列表下载择界面
**************************************************************************************************/
const tGrapViewAttr Op_DownLoadCompany_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//更新
	{BUTTON_ID_2,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//更新
#endif	
	{BUTTON_ID_10,	   5,  38,72,16,    0,0,0,0,   0,0,0,0,1,0,0,0},//编号/名称
	{EDIT_ID_1,	   	   78, 35,150,22,    78,35,230,57, 0,0,0xffff,0,1,1,1,0},
	
	{LISTER_ID_1,	   10,64,224,212,	10,64,234,276,	0,0,0,0,1,0,1,0},
	
	{BUTTON_ID_1,	   83,282,73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
};

const tGrapListerPriv Op_DownLoadCompany_ListPriv[]=
{
	{24,8,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv Op_DownLoadCompany_BtnPriv[]=
{
#ifdef W818C
	{"更新",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#else
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
	{"更新",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
#endif	
	{"编号/名称",0xffff,0x0000,0,0,1,0,0,0},
	
	{"更新",0xffff,0x0000,0,0,2,0 ,0,0},
};

const tGrapEditPriv Op_DownLoadCompany_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,15,  1, COMPANY_NAME_LEN-1,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//编号/名称
};


S32 Op_DownLoad_Company_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	int				err;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://更新
		if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("确定更新网点列表?",0,pDesk,3,NULL,NULL,800))
		{
			break;
		}
		
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			break;
		}
		err = DownLoad_Company_Action(pDesk);
		
		gpu16SearchInfo[0] = 0xffff;
		if(0 == err)
		{
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			if (gu32CompanyTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32CompanyTotal;
			
			Company_GetNameById(gtHyc.companyId, gtHyc.companyName, sizeof(gtHyc.companyName));
			Com_SpcDlgDeskbox("更新完成!",0,pDesk,1,NULL,NULL,100);
		}
		else
		{
			OpCom_ErrorRemind(pDesk);
		}
		break;
	case BUTTON_ID_2://返回
		pDesk->view_attr.reserve = 0xffffffff;
		return RETURN_QUIT;
		break;
	case BUTTON_ID_3://确定
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		/*
		if (gu32CompanyTotal <= 0)
		{
			pDesk->view_attr.reserve = 0xffffffff;
		}
		else
		{
			if(gpu16SearchInfo[0] == 0xffff)//没有进行搜索
			{
				pDesk->view_attr.reserve = pList->lst_privattr.focusItem;
			}
			else
			{
				pDesk->view_attr.reserve = gpu16SearchInfo[pList->lst_privattr.focusItem];
			}
		}
		*/
		pList->enter(pList,pList->lst_privattr.focusItem);
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

S32 Op_DownLoad_Company_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if (gu32CompanyTotal <= 0)
	{
		strcpy(str, "请更新网点信息");
	}
	else
	{
		if(gpu16SearchInfo[0] == 0xffff)//没有进行搜索
		{
			sprintf(str, "%s|%s", gptCompany[i].companyid, gptCompany[i].company);
		}
		else
		{
			sprintf(str, "%s|%s", gptCompany[gpu16SearchInfo[i]].companyid, gptCompany[gpu16SearchInfo[i]].company);
		}
	}
	
	return SUCCESS;
}

U32 Op_DownLoadCompany_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPLIST       *pList;
	U8	*pId, *pNa;
	int i,ret = 0,total = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1 && gu32CompanyTotal > 0)//
	{
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);//ID号一定确认好
		
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
		//hyUsbPrintf("digt = %s   %d \r\n", pEdit->edit_privattr.digt,pList->lst_privattr.totalItem);
			if(type == 0 && gpu16SearchInfo[0] != 0xffff)//输入
			{
				for(i = 0; i < pList->lst_privattr.totalItem; i++)
				{
				//hyUsbPrintf("id = %s  name = %s \r\n", gptCompany[gpu16SearchInfo[i]].companyid, gptCompany[gpu16SearchInfo[i]].company);
					pId = hy_strstr(gptCompany[gpu16SearchInfo[i]].companyid, pEdit->edit_privattr.digt);
					if(pId == NULL)//通过ID没有匹配到
					{
					//hyUsbPrintf("id cmp fail \r\n");
						pNa = hy_strstr(gptCompany[gpu16SearchInfo[i]].company, pEdit->edit_privattr.digt);
						if(pNa != NULL)//匹配到名称
						{
						//hyUsbPrintf("name cmp ok \r\n");
							ret = 1;
						}
						else
						{
							ret = 0;
						}
					}
					else
					{
						ret = 1;
					}
					
					if(ret == 1)
					{
					//hyUsbPrintf("total = %d   i = %d \r\n", total, gpu16SearchInfo[i]);
						gpu16SearchInfo[total++] = gpu16SearchInfo[i];
					}
				}
			}
			else
			{
				for(i = 0; i < gu32CompanyTotal; i++)
				{
					pId = hy_strstr(gptCompany[i].companyid, pEdit->edit_privattr.digt);
					if(pId == NULL)//通过ID没有匹配到
					{
						pNa = hy_strstr(gptCompany[i].company, pEdit->edit_privattr.digt);
						if(pNa != NULL)//匹配到名称
						{
							ret = 1;
						}
						else
						{
							ret = 0;
						}
					}
					else
					{
						ret = 1;
					}
					
					if(ret == 1)
					{
						gpu16SearchInfo[total++] = i;
					}
				}
			}
			pList->lst_privattr.topItem   = 0;
			pList->lst_privattr.focusItem = 0;
			pList->lst_privattr.totalItem = total;
		}
		else
		{
			pList->lst_privattr.topItem   = 0;
			pList->lst_privattr.focusItem = 0;
			gpu16SearchInfo[0] = 0xffff;//标志没有进行搜索
			if (gu32CompanyTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32CompanyTotal;
		}
		
		if(pList) pList->draw(pList, 0, 0);
	}
	
	return SUCCESS ;
}
S32 Op_DownLoad_Company_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	
	if(i < pLister->lst_privattr.totalItem)
	{
		pDesk = (tGRAPDESKBOX *)pLister->pParent ;
		if (gu32CompanyTotal <= 0)
		{
			pDesk->view_attr.reserve = 0xffffffff;
		}
		else
		{
			if(gpu16SearchInfo[0] == 0xffff)//没有进行搜索
			{
				pDesk->view_attr.reserve = i;
			}
			else
			{
				pDesk->view_attr.reserve = gpu16SearchInfo[i];
			}
		}
	}
	
	return RETURN_QUIT;
	
}
/*---------------------------------------------------------
*函数: Op_DownLoad_Company_Desk
*功能: 员工信息下载界面
*参数: ifSearch:是否查询  1:是   查询时会调整下方按钮
*返回: 
*---------------------------------------------------------*/
int Op_DownLoad_Company_Desk(void *pDeskFather, U8 ifSearch)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	tGRAPEDIT 		*pEdit;
	U8              index = 0;
	int 			i,sel;
	
	gpu16SearchInfo = (U16 *)hsaSdram_UpgradeBuf();
	gpu16SearchInfo[0] = 0xffff;
	
	pDesk = Grap_CreateDeskbox(&Op_DownLoadCompany_ViewAttr[index++], "网点列表");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for(i = 0; i < 3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadCompany_ViewAttr[index++],&Op_DownLoadCompany_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = Op_DownLoad_Company_BtnEnter;
		}
	#ifdef W818C
		if(i == 0 && ifSearch == 1)
	#else	
		if(i == 1 && ifSearch == 1)
	#endif
		{
			pBttn->view_attr.id = BUTTON_ID_3;
			pBttn->btn_privattr.pName = "确定";
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &Op_DownLoadCompany_ViewAttr[index++], &Op_DownLoadCompany_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	pEdit->editKeyEnd = Op_DownLoadCompany_EditKeyEnd;
	
	pList = Grap_InsertLister(pDesk,&Op_DownLoadCompany_ViewAttr[index++],&Op_DownLoadCompany_ListPriv[0]);
	if (pList == NULL) return;
	pList->getListString = Op_DownLoad_Company_ListGetStr;
	if(ifSearch == 1)
		pList->enter = Op_DownLoad_Company_ListEnter;
	if (gu32CompanyTotal <= 0)
		pList->lst_privattr.totalItem = 1;
	else
		pList->lst_privattr.totalItem = gu32CompanyTotal;
	
	if(ifSearch == 1)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadCompany_ViewAttr[index++],&Op_DownLoadCompany_BtnPriv[3]);
		if (pBttn == NULL)  return ;
		pBttn->pressEnter = Op_DownLoad_Company_BtnEnter;
	}
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	sel = pDesk->view_attr.reserve;
	Grap_DeskboxDestroy(pDesk);
	
	return sel;
}

/*-------------------------------------------------------------------------
* 函数: file_parse_site
* 说明: 网点列表的解析和保存
* 参数: flag : 1 : 从0更新  0: 增量更新
* 返回: 
* -------------------------------------------------------------------------*/
int file_parse_site(U8 flag,U8 *pdata,U32 datalen,U8 *ver)
{
#if 1
	tCOMPANY *pCompany ;
	U8   *pTempTap, *pTab, *pEnter, *pStart;
	int  fp ,len1   ;
	U32  total,size ;
	U16  crc;
	
	//1. 临时buff的申请和初始化
	pCompany = (tCOMPANY *)(((U32)hsaSdram_UpgradeBuf() + 4) & 0xfffffffc);
	
	memset((U8 *)pCompany,0,LOAD_COMPANY_MAX * sizeof(tCOMPANY));
	
	total = 0 ;
	/*
	if(flag == 0)
	{
		memcpy((U8 *)pCompany,(U8 *)gptCompany,gu32CompanyTotal * sizeof(tCOMPANY));
		total = gu32CompanyTotal ;
	}
	*/
	
	//2. 解析
	pStart = pdata;
	pTempTap = strchr(pStart, '	');
	while(pTempTap)
	{
		if(total>=LOAD_COMPANY_MAX) break;//在这里限制不要超出
		
		pTab = pTempTap;
		pTempTap++;
		pEnter = strchr(pTempTap, '	');
		if(pEnter[3]=='0')//判断该网点是否删除
		{
			if (pEnter)
			{
				len1 = pTab - pStart;
				if (len1 > COMPANY_ID_LEN-2)
					len1 = COMPANY_ID_LEN-2;
				memcpy(pCompany[total].companyid, pStart, len1);//编码
				len1 = pEnter - pTempTap;
				if (len1 > COMPANY_NAME_LEN-2)
					len1 = COMPANY_NAME_LEN-2;
				memcpy(pCompany[total].company, pTempTap, len1);//名称
				total++;
			}
			else
			{
				break;
			}
		}
		
		pEnter = strchr(pTempTap, '\r');
		if (NULL == pEnter)break;
		
		pStart = pEnter+2;
		pTempTap = strchr(pStart, '	');
	}
	
	//3. 写文件
	{
		if(total <= 0)
		{
			return HY_ERROR;
		}
		
		fp = AbstractFileSystem_Open(COMPANY_FILE_PATH,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(COMPANY_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			
			size = total*sizeof(tCOMPANY);
			crc = glbVariable_CalculateCRC((U8 *)pCompany,size);
			fp = AbstractFileSystem_Create(COMPANY_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pCompany,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		else
		{
			size = total*sizeof(tCOMPANY);
			crc = glbVariable_CalculateCRC((U8 *)pCompany,size);
			fp = AbstractFileSystem_Create(COMPANY_FILE_PATH);
			if(fp >= 0)
			{
				hyUsbPrintf("size = %d\r\n",size);
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pCompany,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
			else
			{
				hyUsbPrintf("创建文件失败\r\n");
			}
		}
		
		memset(gaCompanyVer,0,32);
		strcpy(gaCompanyVer,ver);
		gu32CompanyTotal = total;
		memcpy((char *)gptCompany,(char *)pCompany,size);
	}
	
	return HY_OK ;
	
#endif
}