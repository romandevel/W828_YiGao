#include "ospriority.h"
#include "hyhwCkc.h"
#include "std.h"
#include "sys.h"
#include "net_api.h"
#include "wmi.h"
#include "semaphore.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_AppKernal.h"
#include "Net_Evt.h"
#include "Rec_Queue.h"
#include "Op_Config.h"
#include "grap_api.h"
#include "http.h"
#include "hy_unifi.h"







extern S32 ComWidget_LabTimerISR(void *pView);
/*===============================================================*/
#define NET_BACKEVT_NAME			"gprs_evt"
#define NET_BACKEVT_SIZE			4096
#define NET_BACKEVT_PRIOPITY		PRIORITY_6_OSPP


U32 		gu32NetEvt = 0;
U8			*gpNetSendBuf;//用来与服务器通讯  发送记录

void 		*pNetThreadWait = NULL;
SEMAPHORE 	*pNetThread_ExitSem = NULL;



/*------------------------------------------------------------------*/
/*----------------------------------------
*函数:Net_GsmReset
*功能:当使用GSM网络时,GSM模块进入重启则返回1
*参数:none
*返回:1:GSM重启中
*------------------------------------------*/
U8 Net_GsmReset()
{
	if(GsmReset_IfReset() == 1 && gtHyc.NetSelected == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*----------------------------------------
*函数:Net_WakeUp
*功能:唤醒网络后台事件处理进程
*参数:none
*返回:
*------------------------------------------*/
void Net_WakeUp()
{
	if(pNetThreadWait != NULL)
	{
		ewakeup(&pNetThreadWait);
	}
}

/*----------------------------------------
*函数:Net_ClearEvt
*功能:清空网络事件
*参数:none
*返回:none
*------------------------------------------*/
void Net_ClearEvt()
{
	gu32NetEvt = 0;
}

/*----------------------------------------
*函数:Net_GetEvt
*功能:获取网络事件
*参数:none
*返回:网络事件
*------------------------------------------*/
U32 Net_GetEvt()
{
	return gu32NetEvt;
}

/*----------------------------------------
*函数:Net_GetEvtStr
*功能:获取网络事件字符串
*参数:direction:(out)给应用返回网络箭头方向   0:下  1:上
*返回:网络事件字符串
*------------------------------------------*/
void Net_GetEvtStr(U8 *pDisBuf, U8 *direction)
{
	pDisBuf[0] = 0;
	if(pNetThreadWait) // 后台进程进入wait状态
	{
		return ;
	}
	if(gu32NetEvt&NET_LOGIN_EVT)//登录
	{
		*direction = 1;
		strcpy(pDisBuf, "登录");
	}
	else if(gu32NetEvt&NET_UPDATE_SOFT_EVT)//程序升级
	{
		*direction = 0;
		strcpy(pDisBuf, "程序升级");
	}
	else if(gu32NetEvt&NET_DOWNDATA_EVT)//下载资料
	{
		*direction = 0;
		strcpy(pDisBuf, "下载资料");
	}
	else if(gu32NetEvt&NET_UPDATE_TIME_EVT)//时间同步
	{
		*direction = 0;
		strcpy(pDisBuf, "时间同步");
	}
	else if( (gu32NetEvt&NET_SENDREC_EVT) || (gu32NetEvt&NET_SENDMULTI_EVT) )//上传记录
	{
		*direction = 1;
		strcpy(pDisBuf, "上传记录");
	}
	else if(gu32NetEvt&NET_UPDATE_DATA_EVT)//更新资料
	{
		*direction = 0;
		strcpy(pDisBuf, "更新资料");
	}
	else if(gu32NetEvt&NET_CLEARREC_EVT)//清空记录
	{
		*direction = 0;
		strcpy(pDisBuf, "清空记录");
	}
	
	return ;
}

/*----------------------------------------
*函数:Net_AddEvt
*功能:增加指定类型的网络事件
*参数:event待增加的事件
*返回:none
*------------------------------------------*/
void Net_AddEvt(U32 event)
{
	gu32NetEvt |= event;//增加事件
	
	//如果本次增加的是前台事件,先把之前的前台标志去掉
	if(event == NET_LOGIN_EVT ||
	   event == NET_UPDATE_SOFT_EVT ||
	   event == NET_DOWNDATA_EVT ||
	   event == NET_SENDMULTI_F_EVT)
	{
		gu32NetEvt &= ~NET_FOREGROUND_EVT;
	}
	
	//如果本次增加的不是前台标志位 且之前没有前台标志位, 则唤醒gprs后台进程
	if(event != NET_FOREGROUND_EVT && !(gu32NetEvt&NET_FOREGROUND_EVT))
	{
		Net_WakeUp();
	}
}

/*----------------------------------------
*函数:Net_AddSendEvt
*功能:添加记录发送事件
*参数:
*返回:none
*------------------------------------------*/
void Net_AddSendEvt()
{
	gu32LastScanTime = 0x7fffffff;
	if(0 == (Net_GetEvt()&NET_SENDMULTI_EVT))
	{
		if(Rec_GetMultiCnt() > 0 && Rec_GetMultiSendType() == 0)//有一票多件要发送  且是后台发送
		{
			Net_AddEvt(NET_SENDMULTI_EVT);
		}
	}
	if(Queue_GetNodeCnt() > 0 && 0 == (Net_GetEvt()&NET_SENDREC_EVT))//没有上传记录的标记
	{
		Net_AddEvt(NET_SENDREC_EVT);//设置上传标记
	}
	else
	{
		Net_WakeUp();
	}
}

/*----------------------------------------
*函数:Net_DelEvt
*功能:删除指定类型的网络事件
*参数:event待删除的事件
*返回:none
*------------------------------------------*/
void Net_DelEvt(U32 event)
{
	gu32NetEvt &= ~event;//删除指定事件
	
	//如果删除的是前台标志位,则唤醒进程
	if(event == NET_FOREGROUND_EVT)
	{
		Net_WakeUp();
	}
}


/*-----------------------------------------
* 函数: Net_WaitEvt
* 功能: 等待切换网络事件
* 参数: 
* 返回: 1:成功   -1:失败
*-----------------------------------------*/
int Net_WaitEvt(void *p)
{
	tCARTOONPARA	*pUp;
	int ret = 1;
	U16 event;
	
	pUp = (tCARTOONPARA *)p;
	
	event = pUp->datalen;

	while(Net_GetEvt()&event)//等待后台进程处理事件
	{
		Net_WakeUp();
		syssleep(10);
		if(Net_GsmReset() == 1)
		{
			ret = 2;
			break;
		}
	}
	
	return ret;
}

/*-----------------------------------------
* 函数: Net_ChangeEvt
* 功能: 切换网络事件
* 参数: 
* 返回: 0:成功   -1:失败
*-----------------------------------------*/
int Net_ChangeEvt(tGRAPDESKBOX *pDesk, U32 event)
{
	tCARTOONPARA	tUp;
	
	if(Net_GetEvt() != 0)//网络正在使用
	{
		Net_AddEvt(event);//设置网络事件

		tUp.datalen = event;
		tUp.pdata   = NULL;
		cartoon_app_entry(Net_WaitEvt,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		Com_CtnDlgDeskbox(pDesk,2,"设备已占用,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);

		if(kernal_app_get_stat() != 1)//失败
		{
			Net_DelEvt(event);
			Com_SpcDlgDeskbox("网络连接失败,请稍候再试!",6,pDesk,2,NULL,NULL,500);
			return -1;
		}
	}
	else//不在使用中,只需设置为有前台事件即可
	{
		Net_AddEvt(NET_FOREGROUND_EVT);
	}
	
	return 0;
}

/*----------------------------------------
*函数:Net_IfSendMulti
*功能:是否在发送一票多件
*参数:none
*返回:0:否  !=0 是
*------------------------------------------*/
int Net_IfSendMulti()
{
	return (gu32NetEvt&NET_SENDMULTI_EVT);
}

/*----------------------------------------
*函数:Net_StartGprs
*功能:启动gprs
*参数:none
*返回:1:成功
*------------------------------------------*/
int Net_StartGprs(void *p)
{
	int		ret = 1, count = 0;
	
	//gsm在重启时不能连接
	if(GsmReset_IfReset() == 1)
	{
		return 11;
	}
	
	//电量低时不能连接
	/*if(Battery_PermitRun() == 0)
	{
		return 11;
	}*/
	
	//移除wifi网络
	netdev_remove_wifi_dev();
	
	//已经在连接状态时 不需连接
	if(NULL == (int *)ppp_get_eventppp() && gprs_get_status() == HY_OK)
	{
		return 1;
	}
	
	while(count++ < 2)
	{
		ret = gprs_start_connect();
		if(ret >= 0)
		{
			break;
		}
		syssleep(50);
	}
	
	if(ret >= 0 && ret != 2 && ret != 3)
	{
		ret = 1;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

/*----------------------------------------------------------
*函数: Net_ConnectNetWork
*功能: 根据选择方式连接不同网络
*参数: pDesk:当前窗体
*返回: 1:成功,-1:失败
------------------------------------------------------------*/
int Net_ConnectNetWork(tGRAPDESKBOX *pDesk)
{
	int ret = -1;
	
	if(gtHyc.NetSelected == 0)//wifi
	{
		if(netdev_get_tcpip_status())
		{
			//wifi已启动
			ret = 1;
		}
	}
	else if(gtHyc.NetSelected == 1)//使用gprs
	{
		if(1 == Net_StartGprs(NULL))
		{
			ret = 1;
		}
	}

	return ret;
}

/*----------------------------------------------------------
*函数: Net_GetNetState
*功能: 判断当前网络状态
*参数: 
*返回: 
------------------------------------------------------------*/
U8 Net_GetNetState()
{
	U8 state = 0;
	
	//hyUsbPrintf("gtHyc.NetSelected = %d \r\n", gtHyc.NetSelected);
	
	if(gtHyc.NetSelected == 0)//wifi
	{
		if(netdev_get_tcpip_status())
		{
			state = 1;
		}
		//hyUsbPrintf("wifi state = %d \r\n", state);
	}
	else if(gtHyc.NetSelected == 1)//gsm
	{
		if(GsmReset_IfReset() == 0)
		{
			state = net_onlyRead_regStatus();
		}
		else
		{
			state = 0;
		}
		//hyUsbPrintf("gsm state = %d \r\n", state);
	}
	
	return state;
}


//返回数据长度     <=0 失败
int Net_DecodeData(U8 *pNetData, int len, U8 *pGbk)
{
	int ret = 0;
	int gzip=0;
	U8	*pGzip,*pUnic;
	
	if(http_GzipFlag() == 1)
	{
		while(1)
		{
			//0x1f 0x8b  是gzip压缩数据的头
			if(*pNetData == 0x1f && *(pNetData+1) == 0x8b)
			{
				break;
			}
			pNetData++;
			len--;
			if(len <= 0)
			{
			//hyUsbPrintf("gizp flag err!!\r\n");
				return 0;
			}
		}
		gzip   = 0;
		pGzip  = (U8 *)hsaSdramReusableMemoryPool();//用来gzip解压
		Gzip_Uncompress(pNetData,len,pGzip,&gzip,1);
		//hyUsbPrintf("gizp decode len = %d\r\n",gzip);
		pGzip[gzip] = 0;
		pGzip[gzip+1] = 0;
	}
	else
	{
		pGzip = pNetData;
		gzip  = len;
	}

	if(gzip > 0)
	{
		if(hyIE_GetEnType() == CHARSET_UTF8)
		{
			//hyUsbPrintf("%s \r\n",pGzip);
			pUnic = (U8 *)hsaSdramReusableMemoryPool()+0x100000;
			//utf-8 转 unicode
			DecodeUTF8String(CHARACTER_UNICODE,pGzip,pUnic,0x100000);
			ret = Hyc_Unicode2Gbk(pUnic, pGbk, 1);
		}
		else
		{
			memcpy(pGbk, pGzip, gzip);
			ret = gzip;
		}
		
		pGbk[ret] = 0;
		pGbk[ret+1] = 0;
		//hyUsbPrintf("ret = %d GBK = %s \r\n",ret, pGbk);
	}
	else
	{
		ret = 0;
	}
hyUsbPrintf("DecodeData ret__%d \r\n",ret);
	return ret;
}

/*----------------------------------------
*函数:Net_SendMulti
*功能:发送一票多件
*参数:
*返回:0:成功   其他:错误
*------------------------------------------*/
int Net_SendMulti()
{
	int				ret = 1;
	tOP_CALLBACK	tOpCb;
	tMULTI_CODE	*pMulti;
	
	pMulti = Queue_GetMultiBuf();
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
	tOpCb.right = 0;
	tOpCb.funType = 5;
	tOpCb.opType = pMulti->tData.tComm.opType;
	tOpCb.uOpcfg.tSend.pSend = gpNetSendBuf;
	tOpCb.uOpcfg.tSend.bufLen = hsaSdram_RecSendSize();
	tOpCb.uOpcfg.tSend.pNode  = (U8 *)pMulti;
	tOpCb.uOpcfg.tSend.cnt    = 1;
	tOpCb.uOpcfg.tSend.pUrl   = gtHyc.url;
	ret = Op_Config_CallBack(&tOpCb);
	
	if(0 == ret)
	{
		//发送成功,改变状态   为  已传
		RecStore_ChangeMultiCodeState(pMulti->tData.tComm.right, pMulti->tData.tComm.opType, pMulti->tData.tComm.code, RECORD_SEND);
		//清空本票信息
		Queue_ClearMulti();
		//票数减1
		Rec_DelMultiCnt();
	}
	
	return ret;
}

/*----------------------------------------
*函数:Net_SendRequest_F
*功能:共前台发送请求使用
*参数:
*返回:1:成功   >1:错误 (2:发送失败   3:网络连接失败   4:接收到数据类型错误)
*------------------------------------------*/
int Net_SendRequest_F(void *p)
{
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	struct protocol_http_request tRequest;
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    //tRequest.content_type   = "multipart/form-data;charset=UTF8";
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
			
			hyUsbPrintf("recv (front)\r\n");
			hyUsbMessageByLen(pSend, recvLen);
			hyUsbPrintf("\r\n");
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

/*----------------------------------------
*函数:Net_SendRequest
*功能:发送请求
*参数:
*返回:0:成功   >0:错误
*------------------------------------------*/
int Net_SendRequest(tQUE_NODE *pNode, int cnt)
{
	int				ret = 1;
	tOP_CALLBACK	tOpCb;
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
	tOpCb.right = 0;
	tOpCb.funType = 1;
	tOpCb.opType = pNode[0].opType;
	tOpCb.uOpcfg.tSend.pSend = gpNetSendBuf;
	tOpCb.uOpcfg.tSend.bufLen = hsaSdram_RecSendSize();
	tOpCb.uOpcfg.tSend.pNode  = (U8 *)pNode;
	tOpCb.uOpcfg.tSend.cnt    = cnt;
	tOpCb.uOpcfg.tSend.pUrl   = gtHyc.url;
	ret = Op_Config_CallBack(&tOpCb);
	
	return ret;
}

/*----------------------------------------
*函数:Net_LoadFromNand
*功能:从nand上加载未上传的记录
*参数:none
*返回:
*------------------------------------------*/
void Net_LoadFromNand()
{
	if(Queue_GetNodeCnt() == 0)
	{
		Queue_Clear();
		RecStore_GetAllUnreported(Login_GetRight());
		if(Queue_GetNodeCnt() == 0)
		{
			gu32RecUnSendTotal = 0;
		}
	}
}

int Net_AutoConnect()
{
	int ret = 0;
	
	if(gtHyc.NetSelected == 0)
	{
		WifiConnect_BackConn();
	}
	else
	{
		if(phone_detect_simcard_nrt() != 1 || net_onlyRead_regStatus() != 1)//GSM 没有网络
		{
		hyUsbPrintf("gsm not net \r\n");
			gu32NetEvt = 0;//把所有事件清除
			ret = -1;
		}
	}
	
	return ret;
}

/*----------------------------------------
*函数:Net_ThreadMain
*功能:网络后台事件处理进程
*参数:none
*返回:
*------------------------------------------*/
int Net_ThreadMain(void)
{
	int 		ret,cnt;
	U32			intLevel;
	U32			kQueCnt, sendCnt;
	tQUE_NODE	*pNodes;
	tSTORE_CFG  tStoreCfg;
	tCARTOONPARA tUp;
	
	gu32NetEvt	= 0;
	pNetThreadWait = NULL;
	gu32RecUnSendTotal = 0;
	gu32RecInNandTotal = 0;
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	tStoreCfg.groupMax		= 50;//由于buf的限制,不要超过50,如果必须增加时,需对应调整hsaSdram_RecGroupBuf()大小
	tStoreCfg.delaySecond	= 28;
	tStoreCfg.pSmallRecBuf	= (U8 *)hsaSdram_SmallRecBuf();
	tStoreCfg.smallBufSize	= hsaSdram_SmallRecSize();
	tStoreCfg.pLargeRecBuf	= (U8 *)hsaSdram_LargeRecBuf();
	tStoreCfg.largeBufSize	= hsaSdram_LargeRecSize();
	tStoreCfg.pQueBuf		= (U8 *)hsaSdram_RecQueBuf();
	tStoreCfg.queBufSize	= hsaSdram_RecQueSize();
	tStoreCfg.pMultiCodeBuf	= (U8 *)hsaSdram_MultiRecBuf();
	tStoreCfg.multiBufSize	= hsaSdram_MultiRecSize();
	tStoreCfg.pExistBuf	    = (U8 *)hsaSdram_BarCompareBuf();
	tStoreCfg.existBufSize	= hsaSdram_BarCompareSize();
	if(-1 == Rec_InfoInit(&tStoreCfg))
	{
		return -1;
	}
	
	//加载所有记录
	RecStore_GetAllUnreported(Login_GetRight());
	if(Queue_GetNodeCnt() == 0)
	{
		gu32RecUnSendTotal = 0;
	}
	
	//清空dns
	hyIE_clearDns();
	
	pNodes = (tQUE_NODE *)hsaSdram_RecGroupBuf();//供打包上传时使用
	
	pNetThread_ExitSem = semaphore_init(0);
	
	while(1)
	{
		ret = semaphore_trywait(pNetThread_ExitSem);
		if (ret == 0)
		{
			//响应退出要求，退出进程
			break;
		}
		
		if(gu32NetEvt&NET_STOP)//停止后台上传进程
		{
		hyUsbPrintf("stop upload proc \r\n");
			gu32NetEvt = NET_STOP;//清除其他标志,只保留 停止标志
		}
	hyUsbPrintf("wait -----gu32NetEvt = %d \r\n",gu32NetEvt);
		if( Net_GetNetState() == 0 ||	//没有网络
		   (gu32NetEvt&NET_STOP) != 0 || //要求停止后台服务
		   (gu32NetEvt&NET_EVENT_MASK) == 0 ||	//没有任何事件
		   (gu32NetEvt&NET_FOREGROUND_EVT) != 0	//此时有前台在使用GPRS,  后台需要进入wait状态
		   )
		{
			ConSume_UpdataAppCoef(UPLOAD_APP_AN, 0);
			
			intLevel = setil(_ioff);
			(void) ewait(&pNetThreadWait);
			(void) setil(intLevel);
			
			ConSume_UpdataAppCoef(UPLOAD_APP_AN, 120);
		}
	hyUsbPrintf("wakeup cur gu32NetEvt = %x \r\n",gu32NetEvt);	
		ret = semaphore_trywait(pNetThread_ExitSem);
		if (ret == 0)
		{
			//响应退出要求，退出进程
			break;
		}

		//按优先级检测GPRS事件
		if(gu32NetEvt&NET_STOP)//停止后台上传进程
		{
		hyUsbPrintf("stop upload proc \r\n");
			gu32NetEvt = NET_STOP;//清除其他标志,只保留 停止标志
		}
		else if(gu32NetEvt&NET_CLOSEPPP_EVT)//关闭ppp
		{
			ppp_close(); 
	    	ppp_clear_brg_flag();
	    	Net_DelEvt(NET_CLOSESOCKET_EVT);
	    	Net_DelEvt(NET_CLOSEPPP_EVT);
		}
		else if(gu32NetEvt&NET_CLOSESOCKET_EVT)//关闭socket
		{
			Evt_CloseHttpSocket();
	    	Net_DelEvt(NET_CLOSESOCKET_EVT);
		}
		else if(gu32NetEvt&NET_LOGIN_EVT)//登录
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_LOGIN_EVT);
		}
		else if(gu32NetEvt&NET_UPDATE_SOFT_EVT)//程序升级
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_UPDATE_SOFT_EVT);
		}
		else if(gu32NetEvt&NET_DOWNDATA_EVT)//下载资料
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_DOWNDATA_EVT);
		}
		else if(gu32NetEvt&NET_ADDREC)//从nand加载记录
		{
			Net_LoadFromNand();
			Net_DelEvt(NET_ADDREC);
		}
		else if(gu32NetEvt&NET_UPDATE_TIME_EVT)//时间同步
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			/*if(Battery_PermitRun() == 0)
			{//电量不足,不能使用网络!
				
			}
			else*/
			{
				ret = Net_ConnectNetWork(NULL);//连接GPRS
				if(ret == 1)
				{
					SetUpDateState(1);//设置为正在同步
					ret = SetSyn_NetTime();//2:成功   3:失败(需要提示失败原因)   0:其他原因失败
				hyUsbPrintf("SetSyn_NetTime   ret  = %d \r\n", ret);
					if(ret == 2 || ret == 3)
					{
						SetUpDateState(ret);
						syssleep(50);
					}
					else
					{
						SetUpDateState(0);//如果同步其他原因失败,则修改为未同步
					}
				}
			}
			Net_DelEvt(NET_UPDATE_TIME_EVT);
		}
		else if(gu32NetEvt&NET_UPDATE_DATA_EVT)//更新资料
		{
			U8 flag;
			
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			/*if(Battery_PermitRun() == 0)
			{//电量不足,不能使用网络!
				
			}
			else*/
			{
				ret = Net_ConnectNetWork(NULL);//连接GPRS
				if(ret == 1)
				{
					tUp.pdata   = (U8 *)hsaSdram_UpgradeBuf();
					tUp.datalen = hsaSdram_UpgradeSize();
					
					//是否需要更新公司信息
					if(Evt_GetUpdateCompanyFlag() == 1)
					{
						if(DownLoad_Company_GetTotal() == 0)//需要重新下载公司信息
						{
							Evt_SetUpdateCompanyFlag(0xff);
						}
						else
						{
							//更新公司信息
							if(1 == DownLoad_DownCompany(&tUp))
							{//需要全部更新公司列表
								Evt_SetUpdateCompanyFlag(0xff);
							}
							else
							{
								Evt_SetUpdateCompanyFlag(0);
							}
						}
					}
					
					//是否需要更新重量体积和异常件
					flag = Evt_GetEvtWtVol_AbnormalFlag();
					Evt_SetEvtWtVol_AbnormalFlag(0);
					ret = 0x80;
					if(flag&0x01)//更新重量体积
					{
						if(1 == DownLoad_DownWeiVol(&tUp))
						{
							//成功
							ret |= 0x01;
						}
					}
					if(flag&0x2)//更新异常件
					{
						if(1 == DownLoad_DownAbnormal(&tUp))
						{
							//成功
							ret |= 0x02;
						}
					}
					
					Evt_SetEvtWtVol_AbnormalFlag(ret);
				}
				
				//检查程序版本
				if(Comm_GetUpFlag() == 0)
				{
					ret = Net_ConnectNetWork(NULL);
					if(ret == 1)
					{
						Common_NetUpgrade(1, NULL);
					}
				}
			}
			Net_DelEvt(NET_UPDATE_DATA_EVT);
		}
		else if(gu32NetEvt&NET_CLEARREC_EVT)//清空记录
		{
			gu32RecUnSendTotal = 0;
			gu32RecUnSendImage = 0;
			gu32RecInNandTotal = 0;
			gu32MultiInNand    = 0;
			Net_DelEvt(NET_SENDREC_EVT);//删除发送记录事件
			
			RecStore_Clear();//清空nand中记录
			PicStore_Format();//图片
			Queue_Clear();//清空队列信息
			Queue_ClearMulti();
			Net_DelEvt(NET_CLEARREC_EVT);//删除清空记录事件
			hyUsbPrintf("gu32RecUnSendTotal = %d  gu32RecUnSendImage = %d  gu32RecInNandTotal = %d \r\n", 
					gu32RecUnSendTotal, gu32RecUnSendImage, gu32RecInNandTotal);
		}
		else if(gu32NetEvt&NET_ADDMULTI_EVT)//加载一票多件
		{
			RecStore_GetUnreportedMulti(Login_GetRight(), 0);//后台发送
			
			Net_DelEvt(NET_ADDMULTI_EVT);
		}
		else if(gu32NetEvt&NET_SENDMULTI_EVT)//发送一票多件
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			Net_SendMulti();
			
			Net_DelEvt(NET_SENDMULTI_EVT);
		}
		else if(gu32NetEvt&NET_SENDREC_EVT)//发送记录
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			if(Login_HasInfo() == 0)//没有登录信息
			{
				Net_DelEvt(NET_SENDREC_EVT);
				continue;
			}
			//判断电量
			/*if(Battery_PermitRun() == 0)
			{
				syssleep(500);
			}
			else*/
			{
				kQueCnt = Queue_GetNodeCnt();
				if(kQueCnt == 0)//没有记录需要上传
				{
					Net_DelEvt(NET_SENDREC_EVT);
					continue;
				}
				
				//if(/*glbVar_GetSendType() == 1 || */kQueCnt >= gblVar_GetSendCnt() || 
				//   ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay())	//超过设置延迟时间没有再次扫描
				
				//if(kQueCnt > 5 || ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay())
				{
					gu32LastScanTime = 0x7fffffff;
					ret = Net_ConnectNetWork(NULL);//连接网路
					if(ret == 1)//连接成功
					{
						/*
						if(gtHyc.NetSelected == 0)//wifi
						{
							sendCnt = (kQueCnt >= Rec_GetGroupMax())? Rec_GetGroupMax():kQueCnt;
						}
						else if(gtHyc.NetSelected == 1)//gsm
						{
							sendCnt = (kQueCnt >= (Rec_GetGroupMax()/2))? (Rec_GetGroupMax()/2):kQueCnt;
						}
						else
						{
							syssleep(100);
							continue;
						}
						
						if(sendCnt == 0) sendCnt = 1;
						*/
						sendCnt = 1;
						cnt = Queue_GetNodesDate(pNodes, sendCnt);
						hyUsbPrintf("  cnt = %d que cnt = %d  send cnt = %d  cnt = %d\r\n",cnt,kQueCnt, sendCnt, cnt);
						
						//cnt = 2;
						if(cnt > 0)
						{
							ret = Net_SendRequest(pNodes, cnt);//发送数据
							if(ret == -1)//网络通讯错误
							{
								syssleep(50);
							}
							else if(ret == 0)//成功
							{
								Queue_DelNodes(1,cnt);
								syssleep(20);
							}
							else//服务器返回其他错误
							{
								Queue_DelNodes(0,1);//先从队列中删除一条
								syssleep(20);
							}
						}
						else//没有从队列中取到数据
						{
							syssleep(100);
						}
						
						if(cnt >= kQueCnt)//队列中的数据全部加载出来, 可能此时又有新扫描的
						{
							hyUsbPrintf("sendcnt(%d) > quecnt(%d) claer \r\n",cnt,kQueCnt);
							Net_DelEvt(NET_SENDREC_EVT);
						}
					}
					else//gprs连接失败
					{
						syssleep(100);
					}
				}
				/*else
				{
					Net_DelEvt(NET_SENDREC_EVT);
				}*/
			}
		}
	}
	
	semaphore_destroy(pNetThread_ExitSem, OS_DEL_NO_PEND);
	pNetThread_ExitSem = NULL;
	pNetThreadWait = NULL;
	exit(1);
	return 1;	
}

/*----------------------------------------
*函数:Net_BackEvt_CreateThread
*功能:创建网络后台事件处理进程
*参数:none
*返回:none
*------------------------------------------*/
int Net_BackEvt_CreateThread()
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(NET_BACKEVT_NAME);
	if (pid>0 && pid<30)
	{
		//进程存在，判断状态
		if (PSPROC(pid)->state == P_INACT)
		{
			pidStatus = HY_OK;//进程存在且处于exit状态，则直接启动
		}
		else
		{
			return HY_ERROR;//进程还在运行，直接退出，当前设置无效
		}
	}
	
	if (pidStatus != HY_OK)
	{
		pid = createp(	Net_ThreadMain,
		                NET_BACKEVT_NAME,
		                NET_BACKEVT_SIZE,
		                NET_BACKEVT_PRIOPITY,
		                NULL,NULL,NULL);
	}
	
	startValue = pstart(pid, (void *)Net_ThreadMain);
	AppCtrl_CreatPro_Virtual(UPLOAD_APP_AN, pid);
}

/*----------------------------------------
*函数:Net_BackEvt_ExitThread
*功能:销毁网络后台事件处理进程
*参数:none
*返回:none
*------------------------------------------*/
void Net_BackEvt_ExitThread(void)
{
	int pid;
	
	pid = ntopid(NET_BACKEVT_NAME);
	if (pid<0 || pid>30) return;
	
	if(pNetThread_ExitSem != NULL)
		semaphore_post(pNetThread_ExitSem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
		if(pNetThreadWait)
			ewakeup(&pNetThreadWait);
	    
	    syssleep(10);
	}
	
	deletep(pid);
	AppCtrl_DeletePro(UPLOAD_APP_AN);
}