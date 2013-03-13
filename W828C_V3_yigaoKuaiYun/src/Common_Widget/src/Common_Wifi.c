
/*
*include local files
*/
#include "std.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "TcpIp_p.h"
#include "wmi.h"
#include "wifi_api.h"
#include "wpa_api.h"
#include "Net_Evt.h"

static U32 gu32LockWifi;

extern S32 ComWidget_LabTimerISR(void *pView);
/*=================================================================================*/

/*---------------------------------------------------------
*函数: ComWidget_DisConnectAp
*功能: 断开AP       
*参数: none
*返回: none 
*---------------------------------------------------------*/ 
int ComWidget_DisConnectAp(void *p)
{
	int ret = 1;
	eWifi_cmd_state state;
	tWMI_BECON_BUFF  *pBuf = NULL;
	eWMI_ENCRYPTION_TYPE ctype;
	
	if(p == NULL || (gtHyc.NetSelected != 0 && gtHyc.NetSelected != 0xff))
	{
		return -1;
	}
#if 1	
	wlock(&gu32LockWifi);
	
hyUsbPrintf("DisConnect++++++++++++++ last state = %d \r\n",hy_unifi_getState());
	
	pBuf = (tWMI_BECON_BUFF *)p;
	
	ctype = pBuf->wmiEncryptionType;//加密类型
	if (ctype == WMI_WPA_PSK_TKIP ||
		ctype == WMI_WPA_PSK_AES ||
		ctype == WMI_WPA_PSK_TKIP_AES ||
		ctype == WMI_WPA2_PSK_TKIP ||
		ctype == WMI_WPA2_PSK_AES ||
		ctype == WMI_WPA2_PSK_TKIP_AES
		)
	{
		if(0 == Wpa_end())
		{
			netdev_stop_tcpip();
			netdev_remove_wifi_dev();
			pBuf->apStatus &= (~0x01);

			hy_set_ap_link_status(0);
			hy_sort_ap_rssi();
		}
		else
		{
			ret = 2;
		}
		
		unlock(&gu32LockWifi);
		return ret;
	}

hyUsbMessage("hy_unifi_cmd  start  ======== \r\n");	
	if(0 != hy_unifi_cmd(CMD_DISCONNECT))//设置命令
	{
		unlock(&gu32LockWifi);
		return 2;
	}
	
	
	//等待命令处理完
	while(1)
	{
		state = hy_unifi_getState();
		if(state == CMD_STA_DISCONNECTED)
		{//成功完成
			
			netdev_stop_tcpip();
			netdev_remove_wifi_dev();
			pBuf->apStatus &= (~0x01);
				
		//hyUsbPrintf("disconn ap name = %s  ap mac = %02x-%02x-%02x-%02x-%02x-%02x \r\n",pBuf->ap.apName,
		//	pBuf->apBssInfo.bssid[0],pBuf->apBssInfo.bssid[1],pBuf->apBssInfo.bssid[2],
		//	pBuf->apBssInfo.bssid[3],pBuf->apBssInfo.bssid[4],pBuf->apBssInfo.bssid[5]);
			
			hy_set_ap_link_status(0);
			hy_sort_ap_rssi();
			break;
		}
		else if(state == CMD_STA_DISCONNECT_ERR || state == CMD_STA_IDLE)
		{//超时失败
			ret = 2;
			break;
		}
		syssleep(1);
	}
	
	unlock(&gu32LockWifi);
#endif	
	return ret;
}

/*---------------------------------------------------------
*函数: ComWidget_ConnectAp
*功能: 连接AP       
*参数: none
*返回: none 
*---------------------------------------------------------*/ 
int ComWidget_ConnectAp(void *p)
{
	int ret = 1;
	eWifi_cmd_state state;
	tWMI_BECON_BUFF  *pBuf = NULL;
	eWMI_ENCRYPTION_TYPE ctype;
	tWPA_PARAM param;
	
	if(p == NULL || (gtHyc.NetSelected != 0 && gtHyc.NetSelected != 0xff))
	{
		return -1;
	}
#if 1	
	wlock(&gu32LockWifi);
	
	//等待gprs关闭
	while((int *)ppp_get_eventppp() == NULL)
	{
		//关闭gprs
		ppp_close(); 
		ppp_clear_brg_flag();
		
		syssleep(5);
	}
hyUsbPrintf("Connect++++++++++++++ last state = %d \r\n",hy_unifi_getState());	
	pBuf = (tWMI_BECON_BUFF *)p;
	
	ctype = pBuf->wmiEncryptionType;//加密类型
	
hyUsbPrintf("ap ctype  type  =  %d \r\n",ctype);
	
	//wpa 
	if (ctype == WMI_WPA_PSK_TKIP ||
		ctype == WMI_WPA_PSK_AES ||
		ctype == WMI_WPA_PSK_TKIP_AES ||
		ctype == WMI_WPA2_PSK_TKIP ||
		ctype == WMI_WPA2_PSK_AES ||
		ctype == WMI_WPA2_PSK_TKIP_AES
		)
	{
		hyUsbPrintf("ap type   WPA  \r\n");
		param.ApMac  = pBuf->apBssInfo.bssid;
		param.ApSsid = pBuf->ap.apName;
		param.ApPwd  = WifiPw_GetPw();
		param.ApChannel = pBuf->apBssInfo.channel;
		param.cpt_type = ctype;
		ret = Wpa_Setup(&param);
		if(ret != WPA_SUCCESS)
		{
			ret = 2;
		}
		else
		{
			ret = 1;
			if(netdev_add_ip() == 1)
			{
				pBuf->apStatus |= 0x01;//连接状态
				
				WifiPw_SetMac(pBuf->apBssInfo.bssid);
				hy_set_ap_link_status(1);
				hy_sort_ap_rssi();
				
				//连接成功后保存本次连接的AP信息
				WifiConnect_SaveApInfo(pBuf, WifiPw_GetPw());
			}
			else
			{
				hyUsbPrintf("tcp connect fail \r\n");
				//AP连接成功,但tcp连接失败,需要断开AP
				ComWidget_DisConnectAp(p);
				ret = 3;
			}
		}
	//hyUsbPrintf("connect wap ap  ret = %d \r\n", ret);
		
		unlock(&gu32LockWifi);
		
		return ret;
	}
	

	if(ctype != WMI_CRYPTO_OPEN)//加密AP (WEP)
	{
		U8 keyBuf[40];//={0x51, 0x69, 0x90, 0x11, 0xff};//密码是"51699011ff"，要做一次转换		
		int i,keyLen;
		
		strcpy(keyBuf, WifiPw_GetPw());
		
		//wep密码转换
		keyLen = WifiPw_change_wep_pwd(keyBuf);
	//for(i = 0; i < keyLen; i++)
	//	hyUsbPrintf("%x ",keyBuf[i]);
		
		hy_unifi_update_key_info(1, keyBuf, keyLen, 2/*WEP_CRYPT*/, 0, pBuf->apBssInfo.bssid);
		hy_unifi_cmd(CMD_SET_WEP_KEY);
		while(1)
		{
			int state;
			state = hy_unifi_getState();
			if (state == CMD_STA_SET_WEP_KEY_OK ||
				state == CMD_STA_SET_WEP_KEY_ERR)
			{
				//hyUsbPrintf("SET_WEP_KEY=%d\r\n", state);
				break;
			}
			syssleep(1);
		}
	}
	
	//hyUsbPrintf("\r\n set CMD_CONNECT \r\n");
	if(0 != hy_unifi_cmd(CMD_CONNECT))//设置命令
	{
		unlock(&gu32LockWifi);
		
		return 2;
	}
	
	//等待命令处理完
	while(1)
	{
		state = hy_unifi_getState();
		if(state == CMD_STA_CONNECTED)
		{//成功完成
			if(netdev_add_ip() == 1)
			{
				pBuf->apStatus |= 0x01;//连接状态
				
				//如果不是加密AP,则在这里需要保存信息   加密AP在输入密码确认后就已经保存了
				if(ctype == WMI_CRYPTO_OPEN)
				{
					WifiPw_SaveInfo(pBuf, NULL);
				}
				WifiPw_SetMac(pBuf->apBssInfo.bssid);
				hy_set_ap_link_status(1);
				hy_sort_ap_rssi();
				
				//连接成功后保存本次连接的AP信息
				WifiConnect_SaveApInfo(pBuf, WifiPw_GetPw());
			}
			else
			{
				hyUsbPrintf("tcp connect fail \r\n");
				//AP连接成功,但tcp连接失败,需要断开AP
				ComWidget_DisConnectAp(p);
				ret = 3;
			}
			break;
		}
		else if(state == CMD_STA_CONNECT_ERR || state == CMD_STA_IDLE)
		{//超时失败
			ret = 2;
			break;
		}
		syssleep(1);
	}
	
	unlock(&gu32LockWifi);
#endif	
	return ret;
}


/*----------------------------------------------------------
*函数: ComWidget_ScanConnectAp
*功能: (后台) 自动连接wifi  先扫描AP  然后连接之前连接过的AP  
       如果之前没有连接记录,则默认连接信号最好的 未加密 的AP
*参数: 
*返回: 1:成功  其他失败
------------------------------------------------------------*/
int ComWidget_ScanConnectAp(void *p)
{
	int ret = 1,conn=0;
	tAPINFO         *pLastAp;
	eWifi_cmd_state state;
	tWMI_BECON_BUFF  *pBuf = NULL;
	U8 mac[8];
	U32 lastTick, now;
	
	if(gtHyc.NetSelected != 0 && gtHyc.NetSelected != 0xff)
	{
		return -1;
	}
#if 1
	//等待wifi初始化完毕
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if (ABS(now-lastTick) > 1000)
		{
			//超时
			return 2;
		}
		if(hy_unifi_getState() >= CMD_STA_INITED)
		{
			break;
		}
		syssleep(1);
	}
hyUsbPrintf("ScanConnect++++++++++++++ last state = %d \r\n",hy_unifi_getState());	
	if (hy_get_ap_link_status() == 1)
	{
		//pBuf = hy_ap_get_connect_infor();
		//ComWidget_DisConnectAp(pBuf);
		//memcpy(mac, pBuf->apBssInfo.bssid, 6);
		conn = 1;
	}
hyUsbPrintf("conn flag = %d \r\n",conn);	
	hy_ap_del_list_all();
	
	if(0 != hy_unifi_cmd(CMD_SCAN))//设置扫描命令
	{		
		return 2;
	}
	
	//等待命令处理完
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if (ABS(now-lastTick) > 2000)
		{
			//超时
			return 2;
		}
		
		state = hy_unifi_getState();
		if(state == CMD_STA_SCANNED)
		{//成功完成
			
			if(hy_get_scan_ap_count() == 0)//没有扫描到AP
			{
				ret = 3;
			}
			else
			{
				if(conn == 1)//之前有连接
				{
					pBuf = hy_get_ap_info_bymac(WifiPw_GetMac());//找到之前连接的ap
					if(pBuf != NULL)
					{
						hyUsbPrintf("conn ap = %s \r\n", pBuf->ap.apName);
						pBuf->apStatus |= 0x01;//连接状态
						hy_set_ap_link_status(1);
						//hy_ap_setinfo(pBuf);
						//ComWidget_ConnectAp(pBuf);
					}
					else
					{
						hy_set_ap_link_status(0);
					}
				}
				else //从保存的信息中找到最近连接的AP,尝试连接
				{
					pLastAp = (tAPINFO *)p;
					if(pLastAp != NULL)
					{
						pBuf = hy_get_ap_info_bymac(pLastAp->mac);//找到之前连接的ap
						
					hyUsbPrintf("get ap mac = %02x-%02x-%02x-%02x-%02x-%02x \r\n",
					    	pLastAp->mac[0],pLastAp->mac[1],pLastAp->mac[2],
					    	pLastAp->mac[3],pLastAp->mac[4],pLastAp->mac[5]);
						if(pBuf != NULL)
						{
							if(pLastAp->pwType == pBuf->wmiEncryptionType)
							{
							hyUsbPrintf("-------ap name = %s pwType = %d \r\n", pBuf->ap.apName,pLastAp->pwType);
								if(pLastAp->pwType != WMI_CRYPTO_OPEN)
								{
									WifiPw_SetPwType(pLastAp->pwType);
								hyUsbPrintf("pw = %s \r\n", pLastAp->pw);
									WifiPw_SetPw(pLastAp->pw);
								}
								hy_ap_setinfo(pBuf);
								if(1 != ComWidget_ConnectAp(pBuf))
								{
									//连接失败
									hyUsbPrintf("connect fail 111111111\r\n");
								}
							}
						}
						else
						{
							hyUsbPrintf("connect fail 222222222\r\n");
						}
					}
				}
				hy_sort_ap_rssi();
			}
			break;
		}
		else if(state == CMD_STA_SCAN_ERR || state == CMD_STA_IDLE)
		{//超时失败
			ret = 2;
			break;
		}
		syssleep(1);
	}
#endif	
	return ret;
}

/*---------------------------------------------------------
*函数: ComWidget_SearchApList
*功能: 搜索ap list       
*参数: none
*返回: 1:成功 
*---------------------------------------------------------*/ 
int ComWidget_SearchApList(void *p)
{
	int ret = 1,conn=0;
	tAPINFO         *pLastAp;
	eWifi_cmd_state state;
	tWMI_BECON_BUFF  *pBuf = NULL;
	U8 mac[8];
	U32 lastTick, now;
	
	if(gtHyc.NetSelected != 0 && gtHyc.NetSelected != 0xff)
	{
		return -1;
	}
	
#if 1
	wlock(&gu32LockWifi);
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if (ABS(now-lastTick) > 1000)
		{
			//超时
			unlock(&gu32LockWifi);
			return 2;
		}
		if(hy_unifi_getState() >= CMD_STA_INITED)
		{
			break;
		}
		syssleep(1);
	}
	
	if (hy_get_ap_link_status() == 1)
	{
		//pBuf = hy_ap_get_connect_infor();
		//ComWidget_DisConnectAp(pBuf);
		//memcpy(mac, pBuf->apBssInfo.bssid, 6);
		conn = 1;
	}
	hy_ap_del_list_all();
	
	if(0 != hy_unifi_cmd(CMD_SCAN))//设置扫描命令
	{
		unlock(&gu32LockWifi);
		return 2;
	}
	
	//等待命令处理完
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if (ABS(now-lastTick) > 2000)
		{
			//超时
			unlock(&gu32LockWifi);
			return 2;
		}
		
		state = hy_unifi_getState();
		if(state == CMD_STA_SCANNED)
		{//成功完成
			
			if(hy_get_scan_ap_count() == 0)//没有扫描到AP
			{
				ret = 3;
			}
			else
			{
				if(conn == 1)//之前有连接
				{
					pBuf = hy_get_ap_info_bymac(WifiPw_GetMac());//找到之前连接的ap
					if(pBuf != NULL)
					{
						hyUsbPrintf("conn ap = %s \r\n", pBuf->ap.apName);
						pBuf->apStatus |= 0x01;//连接状态
						hy_set_ap_link_status(1);
						//ComWidget_ConnectAp(pBuf);
					}
					else
					{
						hy_set_ap_link_status(0);
					}
				}

				hy_sort_ap_rssi();
			}
			break;
		}
		else if(state == CMD_STA_SCAN_ERR || state == CMD_STA_IDLE)
		{//超时失败
			ret = 2;
			break;
		}
		syssleep(1);
	}
	
	unlock(&gu32LockWifi);
#endif
	return ret;
}

//处理scan connect discontinued
eDLG_STAT ComWidget_ApEvent(tGRAPDESKBOX *pDesk, U8 *pStr, eWifi_cmd_state cmd, void *pApInfo)
{
	eDLG_STAT kDlgState = DLG_CANCEL_EN;
	
	switch(cmd)
	{
	case CMD_SCAN:
		cartoon_app_entry(ComWidget_SearchApList,pApInfo);
        AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,pStr,NULL,NULL,ComWidget_LabTimerISR,50,0xFFFFFFFF);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		break;
	case CMD_CONNECT:
		cartoon_app_entry(ComWidget_ConnectAp,pApInfo);
        AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,pStr,NULL,NULL,ComWidget_LabTimerISR,50,0xFFFFFFFF);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		break;
	case CMD_DISCONNECT:
		cartoon_app_entry(ComWidget_DisConnectAp,pApInfo);
        AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,pStr,NULL,NULL,ComWidget_LabTimerISR,50,0xFFFFFFFF);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		break;
	default:
		break;
	}
	
	return kDlgState;
}


/*---------------------------------------------------------
* 函数:	ComWidget_ChangeNet_StopTrans
* 功能:	停止传输，并检查网络切换是否就绪(如果是GSM--->WIFI,必须确认GSM处于非重启状态)
* 参数:	lastSelect:上次网络类型---0:wifi  1:gsm
*		curSelect:当前选择的网络类型---0:wifi  1:gsm
* 返回:	HY_OK------网络切换就绪
*		HY_ERROR---网络切换未就绪
*---------------------------------------------------------*/
int ComWidget_ChangeNet_StopTrans(tGRAPDESKBOX *pDesk, U8 lastSelect, U8 curSelect)
{
	U32 lastTick, now;
	
	if (rawtime(NULL) < 1000)
	{
		Com_SpcDlgDeskbox("正在初始化网络,请稍候...",0,pDesk,1,NULL,NULL,200);
		return HY_ERROR;
	}
	
	Com_SpcDlgDeskbox("正在停止网络传输,请稍候...",0,pDesk,1,NULL,NULL,10);
	
	lastTick = rawtime(NULL);
	Net_AddEvt(NET_STOP);//切换前先停止后台上传进程
	while(Net_GetEvt()&(~NET_STOP))
	{
		syssleep(5);
		now = rawtime(NULL);
		if(ABS(now-lastTick) >= 2000)	//20秒超时
		{
			Net_DelEvt(NET_STOP);
			return HY_ERROR;
		}
	}
	
	Com_SpcDlgDeskbox("正在切换网络,请稍候...",0,pDesk,1,NULL,NULL,10);
	hyUsbPrintf("正在切换网络....\r\n");
	
	if (curSelect==0 && lastSelect!=curSelect)
	{
		//gsm--->wifi切换
		if (GsmReset_IfReset() == 1)
		{
			Net_DelEvt(NET_STOP);
			//GSM 正在重启，不允许切换
			return HY_ERROR;
		}
		else
		{
			//禁止GSM 重启
			GsmReset_WakeUp_Ctrl(0);
		}
	}
	if (curSelect == 0)
	{
		//wifi--->gsm切换
	}
	
	hyUsbPrintf("Net_DelEvt 0\r\n");
	Net_DelEvt(NET_STOP);
	hyUsbPrintf("Net_DelEvt 1\r\n");
	
	return HY_OK;
}

/*---------------------------------------------------------
* 函数:	ComWidget_ChangeNet_GprsToWifi
* 功能:	关闭PPP&TCPIP&GSM，启动WIFI&TCPIP
* 参数:	none
* 返回:	none
*---------------------------------------------------------*/
int ComWidget_ChangeNet_GprsToWifi(void)
{
	U32 lastTick, now;
	
	//1. 关闭gprs
	ppp_close();
	ppp_clear_brg_flag();
	lastTick = rawtime(NULL);
	while((int *)ppp_get_eventppp() == NULL)
	{
		now = rawtime(NULL);
		if(ABS(now-lastTick) >= 1000)	//10秒超时
		{
			return HY_ERROR;
		}
		syssleep(5);
	}
	//2. 关闭tcpip
	netdev_close_tcpip();
	//3. 关闭gsm
	gsmcdma_stop_service();
	syssleep(50);
	//4. 禁止wifi后台自动连接
	WifiConnect_SetLock(1);
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if(ABS(now-lastTick) >= 1000)	//10秒超时
		{
			return HY_ERROR;
		}
		if(WifiConnect_IfConnecting() == 0)
		{
			break;
		}
		syssleep(1);
	}
	
	//启动wifi
	WifiDriver_Start();
	AppCtrl_CreatPro(TCPIP_AN,0);
	
	return HY_OK;
}

/*---------------------------------------------------------
* 函数:	ComWidget_ChangeNet_WifiToGprs
* 功能:	关闭PPP&TCPIP&GSM，启动WIFI&TCPIP
* 参数:	none
* 返回:	none
*---------------------------------------------------------*/
int ComWidget_ChangeNet_WifiToGprs(void)
{
	U32 lastTick, now;
	int wifiDrv_initSta;
	
	//禁止WIFI后台连接运行
	WifiConnect_SetLock(1);
	
	hyUsbPrintf("ComWidget_ChangeNet_WifiToGprs\r\n");
	//等待WIFI初始化完成，等待WIFI进程处理完事务
	lastTick = rawtime(NULL);
	while(1)
	{
		now = rawtime(NULL);
		if(ABS(now-lastTick) >= 1000)	//10秒超时
		{
			return HY_ERROR;
		}
		wifiDrv_initSta = WiFiDriver_getInitState();
		if (wifiDrv_initSta == 1)
		{
			//wifi drv sdio接口初始化OK，判断unifi状态
			if (hy_unifi_getState() >= CMD_STA_INITED)
			{
				break;
			}
		}
		else if (wifiDrv_initSta == -1)
		{
			//wifi drv sdio接口初始化失败
			break;
		}
		syssleep(5);
		//hyUsbPrintf("wifi initing.....%d \r\n",hy_unifi_getState());
	}
	if (wifiDrv_initSta == 1)
	{
		tWMI_BECON_BUFF *pAp;
		//如果WIFI正在连接，等待WIFI进程处理完事务
		lastTick = rawtime(NULL);
		while(1)
		{
			now = rawtime(NULL);
			if(ABS(now-lastTick) >= 1000)	//10秒超时
			{
				return HY_ERROR;
			}
			if(WifiConnect_IfConnecting() == 0)
			{
				break;
			}
			syssleep(1);
		}
		
		pAp = hy_ap_get_connect_infor();
		hyUsbPrintf("Wifi ComWidget_DisConnectAp=0x%08X\r\n", pAp);
		ComWidget_DisConnectAp((void *)pAp);
		//关闭tcpip
		//hyUsbPrintf("Wifi netdev_close_tcpip\r\n");
		//netdev_close_tcpip();
		//netdev_remove_wifi_dev();
		syssleep(50);
	}
	hyUsbPrintf("WifiDriver_Exit()\r\n");
	//关闭wifi
	WifiDriver_Exit();
	
	hyUsbPrintf("Gsm_InitToSdram_buf()\r\n");
	//启动GSM
	Gsm_InitToSdram_buf();
	gsm_SetPinCode(getPINCode());
	hyhwGsm_BBreset();
	syssleep(50);

	//启动gsm进程(client&server)
	AppCtrl_CreatPro(GSMCDMA_CLIENT_AN,0);
	AppCtrl_CreatPro(GSMCDMA_SERVER_AN,0);

	hyUsbPrintf("GsmReset_WakeUp_Ctrl(1)\r\n");
	//允许GSM 重启
	GsmReset_WakeUp_Ctrl(1);

	AppCtrl_CreatPro(TCPIP_AN,0);

	syssleep(100);
	SIM_set_pin_stat(1);
	
	//允许WIFI后台连接运行
	WifiConnect_SetLock(0);
	
	return HY_OK;
}

/*---------------------------------------------------------
*函数: ComWidget_ChangeNetLogo
*功能: 切换网络信号图标
*参数: last:上次网络类型   type:0:wifi  1:gsm
*返回: 0:成功   -1:失败
*---------------------------------------------------------*/
int ComWidget_ChangeNetLogo(tGRAPDESKBOX *pDesk, U8 lastSelect, U8 curSelect)
{
	tGRAPLABEL		*pWifi, *pGsm;
	eDLG_STAT		kDlgState;
	U16				lastValue;
	int				ret;
	
	pWifi = (tGRAPLABEL *)Grap_GetViewByIDFromComm(pgtComFirstChd,COM_WIFISIG_ID);
	pGsm  = (tGRAPLABEL *)Grap_GetViewByIDFromComm(pgtComFirstChd,COM_GSMSIG_ID);
	hyUsbPrintf("pWifi = %x   gsm = %x  \r\n", (U32)pWifi, (U32)pGsm);
	
	//Com_SpcDlgDeskbox("正在切换网络,请稍候...",0,pDesk,1,NULL,NULL,10);
	lastValue = BackLight_CloseScreenProtect();//记录屏保信息 退出时还原
	ret = ComWidget_ChangeNet_StopTrans(pDesk, lastSelect, curSelect);
	BackLight_SetScreenProtect(lastValue);
	if (ret == HY_ERROR)
	{
		Com_SpcDlgDeskbox("网络切换失败,请重试!",0,pDesk,1,NULL,NULL,150);
		return -1;
	}

	if (curSelect == 0)
	{
		//切换到WIFI
		if(lastSelect != curSelect)//gsm--->wifi切换
		{
			ret = ComWidget_ChangeNet_GprsToWifi();
			if (ret == HY_ERROR)
			{
				Com_SpcDlgDeskbox("网络切换失败,请重试!",0,pDesk,1,NULL,NULL,150);
				return HY_ERROR;
			}
		}
		
		//显示用
		pWifi->view_attr.enable = 1;
		pGsm->view_attr.enable  = 0;
		//启动AP搜索
		kDlgState = ComWidget_ApEvent(pDesk, "正在搜索网络,请稍候...", CMD_SCAN, NULL);
		if(DLG_CONFIRM_EN == kDlgState)
		{
			if(kernal_app_get_stat() == 1)
			{
				ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
				Wifi_ListDesk(pDesk);
				ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			}
			else
			{//失败
				Com_SpcDlgDeskbox("没有找到网络!",0,pDesk,2,NULL,NULL,100);
			}
		}
		else
		{
			Com_SpcDlgDeskbox("没有找到网络!",0,pDesk,2,NULL,NULL,100);
		}
		
		//清除WIFI自动连接计数
		WifiConnect_SetDelay(0);
		//允许WIFI后台连接运行
		WifiConnect_SetLock(0);
	}//if (curSelect == 0)
	else //if (curSelect == 1)
	{
		//切换到GPRS
		ret = ComWidget_ChangeNet_WifiToGprs();
		if (ret == HY_ERROR)
		{
			Com_SpcDlgDeskbox("网络切换失败,请重试!",0,pDesk,1,NULL,NULL,150);
			return HY_ERROR;
		}
		
		//显示用
		pWifi->view_attr.enable = 0;
		pGsm->view_attr.enable  = 1;
	}
	
	return HY_OK;
}
