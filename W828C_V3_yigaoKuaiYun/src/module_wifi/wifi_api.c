/*
*给应用层提供接口
*/

#include "hyTypes.h"
#include "hyErrors.h"
#include "wifi_user.h"
#include "wmi.h"
#include "wifi_api.h"
#include "hyhwCkc.h"
#include "hyhwSDIO.h"
#include "hymwSDIO.h"
#include "hyhwIntCtrl.h"
#include "hyhwGpio.h"
#include "glbVariable.h"
//#include "EventEncumberInit.h"

U8 gu8Wifi_ConnectState;//wifi连接状态 0:未连接  1:已连接
struct llist_head tApList;
//U8 gApSortedInfo[MAX_AP_COUNT];

/**************************************************************
*函数名：hy_stop_scan_ap
*功能：停止扫描
*参数：none
*返回：none
***************************************************************/
void hy_stop_scan_ap(void)
{
	return ;
}

/**************************************************************
*函数名：hy_get_ap_rssi
*功能：获取指定AP的信号信号等级
*参数：index:排序后的AP索引
*返回：信号等级
***************************************************************/
S32 hy_get_ap_rssi(U8 index)
{
	S32 currssi = 0;

	return currssi;
}

/**************************************************************
*函数名：hy_get_connectap_rssi
*功能：更新当前wifi所连接的AP的信号等级
*参数：none
*返回：信号等级
***************************************************************/
S32 hy_get_connected_ap_rssi(void)
{
	S32 currssi = 0;
	
	currssi = csr_getconnect_rssi();
	
	return currssi;	
}

/**************************************************************
*函数名：hy_get_scan_ap_count
*功能：获取扫描到的AP个数
*参数：none
*返回：扫描到的AP数
***************************************************************/
U16 hy_get_scan_ap_count(void)
{
	U16 apcount = 0;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        apcount++;  
    }
	
	return apcount;
}



/**************************************************************
*函数名：hy_reconnect_ap
*功能：重新连接上次连接的AP
*参数：none
*返回：HY_OK:连接成功 HY_ERROR:连接失败
***************************************************************/
U32 hy_reconnect_ap(void)
{
	
	return HY_OK;
}

/**************************************************************
*函数名：hy_connect_anoter_ap
*功能：断开当前AP,连接另一个指定的AP
*参数：apMac
*返回：HY_OK:连接成功 HY_ERROR:连接失败
***************************************************************/
U32 hy_connect_anoter_ap(U8 *apMac)
{
	
	return HY_OK;
}

/**************************************************************
*函数名：hy_get_ap_info_sort
*功能：获取指定AP信息
*参数：index:排序后的AP序号
*返回：AP信息
***************************************************************/
tWMI_BECON_BUFF *hy_get_ap_info_sort(U8 index)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	int count = 0;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if (count == index)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
        
        count++;  
    }
		
	return pApinfo;
}

tWMI_BECON_BUFF *hy_get_ap_info_noEncryp()
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(pPos->beconBuff.wmiEncryptionType == 0)//未加密
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}

tWMI_BECON_BUFF *hy_get_ap_info_bymac(U8* pMac)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(memcmp(pMac, pPos->beconBuff.apBssInfo.bssid, 6) == 0)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}


tWMI_BECON_BUFF *hy_get_ap_info_byName(U8 *pName)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(strcmp(pName, pPos->beconBuff.ap.apName) == 0)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}

/**************************************************************
*函数名：hy_get_ap_module_status
*功能：获取wifi模块是否出错
*参数：none
*返回：0==正常，1==出错
***************************************************************/
U8 hy_get_ap_module_status(void)
{
	U8 status;
	
	status = 0;//现在认为总是正确的

	return status;
}

/**************************************************************
*函数名：hy_get_ap_link_status
*功能：获取连接状态
*参数：none
*返回：0:未连接  1:已连接
***************************************************************/
U8 hy_get_ap_link_status()
{
	return gu8Wifi_ConnectState;
}

void hy_set_ap_link_status(U8 state)
{
	gu8Wifi_ConnectState = state;
}

/**************************************************************
*函数名：hy_get_ap_ready_status
*功能：获取wifi模块准备状态
*参数：none
*返回：见MV_READY_STATUS
***************************************************************/
MV_READY_STATUS hy_get_ap_ready_status()
{
	MV_READY_STATUS status;
	
	status = adapter.mvReadyStatus;
	
	return status;
}

/**************************************************************
*函数名：hy_get_ap_scan_status
*功能：获取wifi模块扫苗AP状态
*参数：none
*返回：见
***************************************************************/
U8 hy_get_ap_scan_status()
{
	U8 status;
	
	return status;
}

/**************************************************************
*函数名：hy_get_ap_scan_status
*功能：获取是否取到信号的状态
*参数：none
*返回：0 = 未取到，1=已取到
***************************************************************/
U8 hy_get_ap_rssi_status()
{
	U8 status;
	
	//drv_MutexProtect();

	//drv_MutexUnprotect();
	
	return status;
}

/**************************************************************
*函数名：hy_sort_ap_rssi
*功能：对ap列表进行排序,排序规则:连接上的位于第一个 其次为信号强度排序
*参数：none
*返回：none
***************************************************************/
void hy_sort_ap_rssi(void)
{
	tWMI_BECON_BUFF beconBuff;
	tWMI_PARSE_BECON *pPos1,*pPos2;
	
	drv_MutexProtect();
	
	for (pPos1=(tWMI_PARSE_BECON *)tApList.next; pPos1->list.next != &tApList; pPos1 = pPos1->list.next)
	{
	     for (pPos2=pPos1->list.next; (struct llist_head *)pPos2 != &tApList; pPos2 = pPos2->list.next)
	     {
	         
	          if (pPos2->beconBuff.apStatus&0x1)
	          {
	              beconBuff = pPos2->beconBuff;
	              pPos2->beconBuff = pPos1->beconBuff;
	              pPos1->beconBuff = beconBuff;	                
	          }
	          else if (pPos2->beconBuff.apBssInfo.apRssi > pPos1->beconBuff.apBssInfo.apRssi && !(pPos1->beconBuff.apStatus&0x1))
	          {
	              beconBuff = pPos2->beconBuff;
	              pPos2->beconBuff = pPos1->beconBuff;
	              pPos1->beconBuff = beconBuff;	  
	          }
	     }
	}
	
	drv_MutexUnprotect();
	
	return ;
}


/**************************************************************
*函数名：hy_get_ap_rssi_range
*功能：根据传入的rssi进行分6个等级
*参数：rssi:信号强度值
*返回：信号等级
***************************************************************/ 
U32 hy_get_ap_rssi_range(S32 rssi)
{
	U32 index = 0;

    //if(rssi < -90) index = 0;
	//else 
	if(rssi < -82) index = 1;
	else if(rssi < -76) index = 2;
	else if(rssi < -68) index = 3;
	else if(rssi < -60) index = 4;
	else index = 5;
	
	//G40的AP信号图标只有4格
	if (index>4) index = 4;

	return index;
}

/**************************************************************
*函数名：hy_ap_wait_link_success
*功能：等待wifi连接上
*参数：none
*返回：成功: SUCCESS 失败:FAIL
***************************************************************/ 
U32 hy_ap_wait_link_success(void)
{
    U32 timecnt = 0;
    U32 rc = 0;
    
    while(0 == hy_get_ap_link_status())
    {
        
        
        syssleep(1);
        if (timecnt++ > 700)
        {
            break;
        }        
    }
    
    if ( 1 == hy_get_ap_link_status())
    {
//        mvWmi_cmdRssi();
        rc = 1;
    }
    
    return rc;
}

/**************************************************************
*函数名：hy_adhoc_wait_discon
*功能：等待ap回断开信息
*参数：none
*返回：HY_OK:成功 HY_ERROR:失败
***************************************************************/
static int hy_adhoc_wait_discon(void)
{
    U32 timecnt = 0;
    int rc = HY_ERROR;
    
    while(1 == hy_get_ap_link_status())
    {
        syssleep(1);
        if (timecnt++ > 700)
        {
            break;
        }        
    }
    
    if ( 1 != hy_get_ap_link_status())
    {
        rc = HY_OK;
    }
    
    return rc; 
}

/**************************************************************
*函数名：hy_ap_discon_adhoc
*功能：发送命令后清除ap信息
*参数：eHY_APCMD中的一些命令
*返回：HY_OK:成功 HY_ERROR:失败
***************************************************************/
int hy_ap_discon_adhoc(void)
{
	U32 ret = HY_OK;
    tWMI_PARSE_BECON *pPos,*pPos1;	         
    int flag = 0;
    U8 status;

	if( adapter.mvWlanStatus ==  WLAN_STATUS_CONNECT)
	{
		
         llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
         {
             status = pPos->beconBuff.apStatus;
             
             if ((status&0x1) && (status&0x4))
             {
                 llist_del(&pPos->list);
                 unifi_free(NULL, pPos);
                 break;
             }
             else if ((status&0x1))
             {
                 pPos->beconBuff.apStatus &= (~1); 
                 break;
             }
         }	
   	
//    	 mvWmi_cmdAdHocStop();

		// do not know if event will come back!			
	}
	//adapter.mvWlanStatus =  WLAN_STATUS_NOT_CONNECT;

    return ret;
}

/*-------------------------------------------------------------
*函数名：hy_ap_get_spc_becon
*功能：根据mac地址得到becon地址
*参数：pMac：mac地址
*返回：不成功返回NULL
*------------------------------------------------------------*/
tWMI_BECON_BUFF *hy_ap_get_spc_becon(U8 *pMac)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
      
    llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(memcmp( pPos->beconBuff.apBssInfo.bssid,pMac,6) == 0 )
    	 {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }  
      
    return pBuf;
}

/*-------------------------------------------------------------
*函数名：hy_ap_del_list_all
*功能：删除ap list 的内容
*参数：
*返回：
*------------------------------------------------------------*/
void hy_ap_del_list_all(void)
{
    tWMI_PARSE_BECON *pPos,*pPos1;
    //int i=0;
    
    llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
        llist_del(&pPos->list);
        //hyUsbPrintf("ies buf = %x \r\n", (U32)pPos->beconBuff.ies.buf);
        unifi_free(NULL, pPos->beconBuff.ies.buf);
        unifi_free(NULL, pPos);
    } 
}

/*-------------------------------------------------------------
*函数：hy_ap_get_curnet_type
*功能：得到当前网络的类型
*参数：none
*返回：
*------------------------------------------------------------*/
int hy_ap_get_curnet_type(void)
{
	tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	int type = AP_NET_TYPE_UNKNOW;
	int status;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         status = pPos->beconBuff.apStatus; 
         if(status&0x1)
         {
    		 if (status&0x2)
    		 {
    		     type = AP_NET_TYPE_ADHOC;
    		 }
    		 else
    		 {
    		     type = AP_NET_TYPE_WIFI;
    		 }
    		 break;
    	 }
    }
    
    return type;
}

/*-------------------------------------------------------------
*函数：hy_ap_get_connect_infor
*功能：得到连接上的ap的信息
*参数：none
*返回：
*------------------------------------------------------------*/
tWMI_BECON_BUFF *hy_ap_get_connect_infor(void)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(pPos->beconBuff.apStatus&0x1)
         {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }
    
    if(pBuf == NULL)
    {
    	//如果没有从ap列表中找到ap,则可能是没有经过扫描就直接连接的上次AP
    	pBuf = WifiConnect_GetCurAp();
    }
    
    return pBuf;
}

/*-------------------------------------------------------------
*函数：hy_ap_updata_ap_list
*功能：更新ap list 信息
*参数：none
*返回：none
*------------------------------------------------------------*/
void hy_ap_updata_ap_list(int type)
{
    tWMI_PARSE_BECON *pPos = NULL, *pPos1 = NULL;
	int status;
	
	if (gu8Wifi_ConnectState == 1)  return;
	
	llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
         status = pPos->beconBuff.apStatus; 
         if(status&0x1)
         {

    		 if (AP_LIST_REFRESH == type)
    		 {
    		     pPos->beconBuff.apStatus &= (~1);
    		 }
    		 else if (AP_LIST_PART_DEL == type)
    		 {
    		     llist_del(pPos); 
    		     unifi_free(NULL, pPos);
    		 }   
    		 break;
    	 }
    }
    
    return ;
}

/*-------------------------------------------------------------
*函数：hy_ap_rescan_tidy_list
*功能：扫描时重新整理一下ap list,把没有连接ap信息都删除掉
*参数：none
*返回：none
*------------------------------------------------------------*/
void hy_ap_rescan_tidy_list(void)
{
    tWMI_PARSE_BECON *pPos = NULL, *pPos1 = NULL;

    llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
         if(!(pPos->beconBuff.apStatus&0x1))
         {
    		 llist_del(pPos);
    		 unifi_free(NULL, pPos);
    	 }
    }
}/*-------------------------------------------------------------
*函数：hy_ap_get_cur_encryptionType()
*功能：得到连接上的ap的加密类型
*参数：none
*返回：
*------------------------------------------------------------*/
eWMI_ENCRYPTION_TYPE hy_ap_get_cur_encryptionType(void)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(pPos->beconBuff.apStatus&0x1)
         {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }
    
    return pBuf->wmiEncryptionType;
}

int hy_ap_setinfo_byidx(U8 idx)
{
	int ret = -1;
	tWMI_BECON_BUFF *pap;
	
	pap = hy_get_ap_info_sort(idx);
	if(pap != NULL)
	{
		mlme_update_scan_info(&(pap->ies), &(pap->msi), pap->wmiEncryptionType);
		ret = 0;
	}
	
	return ret;
}

int hy_ap_setinfo(tWMI_BECON_BUFF *pap)
{
	int ret = 0;

	mlme_update_scan_info(&(pap->ies), &(pap->msi), pap->wmiEncryptionType);
	
	return ret;
}

