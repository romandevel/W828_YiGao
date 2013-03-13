/**********************************************************************
						重量体积编码下载
**********************************************************************/


#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "AbstractFileSystem.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"


U32			gu32WeiVolTotal;
U32			gu32WeiVolBufSize;
U16			gu16VolIndex;
tVOLWEI		*gptWeiVol;


/*---------------------------------------------------------------------
*函数名：Op_WeiVol_Init
*功能：初始化重量体积编码列表信息
*参数：
*返回：
----------------------------------------------------------------------*/
void Op_WeiVol_Init(void)
{
	int fp,size;
	U16 i,crc;
	
	//重量体积公用一块buf
	gu32WeiVolTotal = 0;
	gu32WeiVolBufSize= hsaSdram_WeiVolSize();
	gu16VolIndex  = 0;
	gptWeiVol = (tVOLWEI *)hsaSdram_WeiVolBuf();
	
	fp = AbstractFileSystem_Open(WEIVOL_FILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		if(size <= 4)//crc+index+data
		{
			AbstractFileSystem_Close(fp);
			return ;
		}
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		
		AbstractFileSystem_Read(fp,(char *)&gu16VolIndex,sizeof(U16));
		
		size-=4;
		AbstractFileSystem_Read(fp,(char *)gptWeiVol,size);
		AbstractFileSystem_Close(fp);
		
		gu32WeiVolTotal = size/sizeof(tVOLWEI);
		
		if(crc != glbVariable_CalculateCRC((U8 *)gptWeiVol,size))
		{
			AbstractFileSystem_DeleteFile(WEIVOL_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			return ;
		}
	}
	
	return ;
}


/*****************************************************
* 函数：DownLoad_ParseWeiVol
* 功能：解析网络上返回的信息
* 参数：pData 网络返回的数据 utf-8编码
* 返回：0 成功  -1 失败
******************************************************/
int DownLoad_ParseWeiVol(U8 *pData, int buflen)
{
	int ret = 0,len;
	int subLen,size;
	int fp;
	U16 crc,readcrc;
	U8  *pTmp,*pS,*pE;
	
	//先清空内存
	memset((char *)gptWeiVol, 0, gu32WeiVolBufSize);
	gu32WeiVolTotal = 0;
	gu16VolIndex    = 0;
	
	pTmp = strstr(pData,"st=\"ok");
	if(pTmp != NULL)
	{
		/* 体积信息 */
		pS = strstr(pTmp,"<d>vol=1:");
		if(pS == NULL) return -2 ;
		pE = strstr(pTmp,";");
		if(pE == NULL) return -2 ;
		
		pS += 9 ;
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		gu16VolIndex++;
		
		pS = pE + 3 ;
		pE = strstr(pS,";");
		if(pE == NULL) return -2 ;
		
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		gu16VolIndex++;
		
		pS = pE + 3 ;
		pE = strstr(pS,";");
		if(pE == NULL) return -2 ;
		
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		gu16VolIndex++;
		
		/* 重量信息 */
		pS = strstr(pE,"<d>wei=1:");
		if(pS == NULL) return -2 ;
		pE = strstr(pS,";");
		if(pE == NULL) return -2 ;
		
		pS += 9 ;
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		
		pS = pE + 3 ;
		pE = strstr(pS,";");
		if(pE == NULL) return -2 ;
		
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		
		pS = pE + 3 ;
		pE = strstr(pS,";");
		if(pE == NULL) return -2 ;
		
		len = pE - pS ;
		if(len <= 0 || len > 100) return -2 ;
		memcpy(gptWeiVol[gu32WeiVolTotal].name,pS,len);
		gptWeiVol[gu32WeiVolTotal].name[len] = 0 ;
		gu32WeiVolTotal++;
		
		//保存文件
		size = gu32WeiVolTotal*sizeof(tVOLWEI);
		crc  = glbVariable_CalculateCRC((U8 *)gptWeiVol,size);
		
		fp = AbstractFileSystem_Open(WEIVOL_FILE_PATH,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Read(fp,(char *)&readcrc,sizeof(U16));
			AbstractFileSystem_Close(fp);
			if(crc == readcrc)//和之前保存的crc相同,不要更新
			{
				return 0;
			}
			AbstractFileSystem_DeleteFile(WEIVOL_FILE_PATH,AFS_SINGLE_DELETE_FDM);
		}
		fp = AbstractFileSystem_Create(WEIVOL_FILE_PATH);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
			AbstractFileSystem_Write(fp,(char *)&gu16VolIndex,sizeof(U16));
			AbstractFileSystem_Write(fp,(char *)gptWeiVol,size);
			AbstractFileSystem_Close(fp);
		}
	}
	else
	{
		return -1;
	}
	
	return ret;
}

int DownLoad_DownWeiVol(void *p)
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
	
	NetReq_FillRequestHead(REQ_GETWEIVOL,Login_GetReqHead(),glbVar_GetCompanyId(),Login_GetUserId());
	sprintf(pSend, "<req op=\"%s\">%s</req>", Req_name[REQ_GETWEIVOL],Login_GetReqHead());
	
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
				if(-1 == DownLoad_ParseWeiVol(pSend, bufLen))
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
