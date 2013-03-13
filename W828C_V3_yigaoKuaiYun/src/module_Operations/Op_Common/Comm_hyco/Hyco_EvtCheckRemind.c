/*
后台事件监测和提醒
*/
#include "grap_api.h"
#include "glbVariable.h"
#include "Net_Evt.h"
#include "Rec_Info.h"
#include "hy_unifi.h"
#include "Op_Config.h"


#ifdef VER_CONFIG_HYCO

#define	CHECK_COMPANY_TIME		(4*60*60*100)


U8	gu8IfUpdateCompany;	//是否需要更新公司   0:不需要   1:需要   0xff:需要重新下载
U8	gu32LastUpdateCompany;//最后更新公司信息的时间
U8	gu8EvtWtVol_Abnormal;	//重量体积和异常类型更新   0:不需更新   1:只更新重量体积   2:只更新异常类型   3:2者都更新

U8 gu8BtErrorFlag;
U8 BtTmkdStr[50];

void Evt_VarInit()
{
	gu8IfUpdateCompany = 0;
	gu32LastUpdateCompany = 0xFFFFFFFF;
	
	gu8EvtWtVol_Abnormal = 0;
}

/*-----------------------------------------------
* 函数:Evt_SetUpdateCompanyLast
* 功能:设置最后一次更新公司信息的时间
* 参数:
* 返回:
------------------------------------------------*/
void Evt_SetUpdateCompanyLast(U32 tick)
{
	gu32LastUpdateCompany = tick;
}

/*-----------------------------------------------
* 函数:Evt_GetUpdateCompanyFlag
* 功能:获取更新公司事件
* 参数:
* 返回:
------------------------------------------------*/
U8 Evt_GetUpdateCompanyFlag()
{
	return gu8IfUpdateCompany;
}

/*-----------------------------------------------
* 函数:Evt_SetUpdateCompanyFlag
* 功能:设置更新公司事件
* 参数:
* 返回:
------------------------------------------------*/
void Evt_SetUpdateCompanyFlag(U8 flag)
{
	gu8IfUpdateCompany = flag;
}

/*-----------------------------------------------
* 函数:Evt_GetEvtWtVol_AbnormalFlag
* 功能:获取重量体积和异常件更新状态
* 参数:
* 返回:
------------------------------------------------*/
U8 Evt_GetEvtWtVol_AbnormalFlag()
{
	return gu8EvtWtVol_Abnormal;
}

/*-----------------------------------------------
* 函数:Evt_SetEvtWtVol_AbnormalFlag
* 功能:设置重量体积和异常件更新状态
* 参数:
* 返回:
------------------------------------------------*/
void Evt_SetEvtWtVol_AbnormalFlag(U8 flag)
{
	gu8EvtWtVol_Abnormal = flag;
}

/*-----------------------------------------------
* 函数:Evt_Event_Remind
* 功能:事件提醒
* 参数:
* 返回:
------------------------------------------------*/

/*-----------------------------------------------
* 函数:Evt_Event_Remind
* 功能:事件提醒
* 参数:
* 返回:
------------------------------------------------*/
void Evt_Event_Remind(tGRAPDESKBOX *pDesk)
{
	U8 type;
	
	if(GetUpDateState() == 2)//成功
	{
		SetUpDateState(0x82);//已经提示过
		Com_SpcDlgDeskbox("时间同步完成",0,pDesk,1,NULL,NULL,100);
		ReDraw(pDesk,0,0);
	}
	else if(GetUpDateState() == 3)//提示失败原因
	{
		SetUpDateState(0x83);//已经提示过
		Com_SpcDlgDeskbox(UpDateTime_GetFailReason(),6,pDesk,2,NULL,NULL,0xFFFFFFFF);
		ReDraw(pDesk,0,0);
	}
	
	if(gu8BtErrorFlag == 1)
	{
		gu8BtErrorFlag = 0;
		Com_SpcDlgDeskbox(BtTmkdStr,6,pDesk,1,NULL,NULL,200);
		ReDraw(pDesk,0,0);
	}
	
	//是否需要重新下载公司信息
	if(Evt_GetUpdateCompanyFlag() == 0xff)
	{
		Evt_SetUpdateCompanyFlag(0);
		Com_SpcDlgDeskbox("公司列表有较大变动,请及时更新!",6,pDesk,2,NULL,NULL,300);
		ReDraw(pDesk,0,0);
	}
}

/*----------------------------------------------------------
* 函数: Evt_CloseSocket_Check
* 功能: 检查是否需要断开socket或ppp等网络   
* 参数: 无
* 返回: 
------------------------------------------------------------*/
void Evt_CloseSocket_Check()
{
	static U8 	count = 0;
	static U32	lastTime = 0;
	U32 now;
	
	now = rawtime(NULL);
	if(ABS(now - lastTime) >= 200)
	{
		if(gprs_get_status() == HY_OK && Net_GetTcpFlag() == 1)
		{
			if (gtHyc.NetSelected == 1)//选择的是gsm
			{
				//ppp在连接状态,但 TCP进程退出,此时需要关闭ppp
				Net_AddEvt(NET_CLOSEPPP_EVT);
			}
			count = 0;
		}
		
		if(gtHyc.NetSelected == 0)//选择的是wifi
		{
			if((Net_GetEvt()&~NET_CLOSESOCKET_EVT) || (STATE_ASSOCIATED != hy_unifi_getConnectState()))
			{
				count = 0;
			}
		}
		else if (gtHyc.NetSelected == 1)//选择的是gsm
		{
			if((Net_GetEvt()&~NET_CLOSESOCKET_EVT) || ((int *)ppp_get_eventppp() != NULL))
			{
				count = 0;
			}
		}
		
		if(count == 5)
		{
			if(hyIE_GetHttpSocket() != -1)
		    {
		    	Net_AddEvt(NET_CLOSESOCKET_EVT);
		    }
		    count++;
	    }
	    else if(count == 6)
	    {
	    	if (gtHyc.NetSelected == 1)//选择的是gsm
	    	{
	    		Net_AddEvt(NET_CLOSEPPP_EVT);
	    	}
		    count = 0;
	    }
	    else
	    {
	    	count++;
	    }
		
		lastTime = rawtime(NULL);
	}
}


/*-----------------------------------------------
* 函数:Evt_GsmReset_Check
* 功能:gsm模块是否需要重启监测
* 参数:
* 返回:
------------------------------------------------*/
void Evt_GsmReset_Check()
{
	if(Ie_get_connectfailcnt() >= 5 && GsmReset_IfReset() == 0) /* IE网络连续5次连接失败 重启GSM */
	{
		if(gtHyc.NetSelected == 1)
		{
			GsmReset_WakeUp();
		}
		else if(gtHyc.NetSelected == 0)
		{
			Ie_clear_connectfailcnt();
		}
	}
	
	if(gsm_getmodestate() == 1 && GsmReset_IfReset() == 0)  /* GSM 不响应AT了 重启GSM */
	{//GSM模块出现异常  启动重启进程
		GsmReset_WakeUp();
	}
}

/*-----------------------------------------------
* 函数:Evt_RecLoadSend_Check
* 功能:记录加载,发送监测
* 参数:
* 返回:
------------------------------------------------*/
void Evt_RecLoadSend_Check()
{
	int		reg;
	U32		kQueCnt;
	
//hyUsbPrintf("back Net_GetEvt = 0x%x \r\n", Net_GetEvt());
	
	reg = Net_GetNetState();
		
	kQueCnt = Queue_GetNodeCnt();//判断队列中是否有记录
	if(reg == 1)//已注册
	{
		if(!(Net_GetEvt()&NET_FOREGROUND_EVT))
		{//没有前台事件

			if(GetUpDateState() == 0)
			{
				Net_AddEvt(NET_UPDATE_TIME_EVT);//设置为需要同步时间
			}
			
			if(Login_HasInfo() == 1/* && glbVar_GetSendType() == 0*/)//有登录信息  且选择的是自动发送
			{
				if(0 == (Net_GetEvt()&NET_SENDMULTI_EVT))
				{
					if(Rec_GetMultiCnt() > 0 && Rec_GetMultiSendType() == 0)//有一票多件要发送  且是后台发送
					{
						Net_AddEvt(NET_SENDMULTI_EVT);
					}
				}
				if(0 == (Net_GetEvt()&NET_SENDREC_EVT))//没有上传记录的标记
				{
				//hyUsbPrintf("send cnt = %d  %d  %d \r\n", gblVar_GetSendCnt(), gu32LastScanTime, gblVar_GetSendDelay());
					//if(kQueCnt >= gblVar_GetSendCnt() || 
					//  (kQueCnt > 0 && ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay()))//队列中有记录未上传
					if(kQueCnt > 0)
					{
						Net_AddEvt(NET_SENDREC_EVT);//设置上传标记
					}
				}
				else//有上传记录标志  需要唤醒进程
				{
					Net_WakeUp();
				}
			}
		}
	}
	
	if(Rec_GetMultiCnt() > 0 && Queue_GetMultiCodeCnt() == 0)
	{
		//磁盘上还有一票多件的记录,但对列中已经发送完,需要加载
		Net_AddEvt(NET_ADDMULTI_EVT);
	}
	
	if((Rec_GetUnSendImageCnt() > 0 || Rec_GetInNandCnt() > 0 || Rec_GetUnSendCnt() > Rec_GetMultiCnt()) && kQueCnt == 0)//在nand上还有未上传记录且队列中数据为空
	{
		Net_AddEvt(NET_ADDREC);
	}
}

/*-----------------------------------------------
* 函数:Evt_UpdateCompany_Event
* 功能:执行公司列表更新
* 参数:
* 返回:
------------------------------------------------*/


/*-----------------------------------------------
* 函数:Evt_UpdateCompany_Check
* 功能:检查公司列表更新
* 参数:
* 返回:
------------------------------------------------*/
void Evt_UpdateCompany_Check()
{

}


/*-----------------------------------------------
* 函数:Evt_UpdateWtVol_Check
* 功能:体积重量编码更新
* 参数:
* 返回:
------------------------------------------------*/
void Evt_UpdateWtVol_Check()
{

}


int Evt_CloseHttpSocket()
{
	struct protocol_http_request tRequest;
	U8  *pSend;
	int bufLen,sock;
	
	sock = hyIE_GetHttpSocket();
	if(sock != -1)
	{
	//hyUsbPrintf("close socket === \r\n");
		protocol_http_close(sock);
		hyIE_resetSocket();
	}
	
	return 0;
}


int Evt_FillErrStr(U8 *pData)
{
	U8 *pSub;
	
	pSub = strstr(pData, "False");
	if(pSub != NULL)
	{
		pSub += 5;
		if(*pSub == '\t')
		{
			pSub++;
		}
		else if(*pSub == '\r')
		{
			pSub+=2;
		}
		UpDateTime_SetFailReason(pSub);
	}
	else
	{
		UpDateTime_SetFailReason("其他错误!");
	}
}

#endif //#ifdef VER_CONFIG_HYCO