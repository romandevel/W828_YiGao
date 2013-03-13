#include "ppp_processor.h"
#include "ppp_api.h"

#include "app_shell.h"
#include "app_client_includes.h"


extern int *Eventppp;

S8 gu8GprsStatus = HY_ERROR;


int gprs_main(struct lgsm_handle *lh, char *getBuf, int *getLen);


//返回NULL  ppp在运行   否则ppp出于wait状态
int *ppp_get_eventppp()
{
	return Eventppp;
}

/*-----------------------------------------
* 函数：gprs_get_status
* 功能：得到GPRS连接的状态
* 参数：none
* 返回：>=0  
*-------------------------------------------*/
int gprs_get_status(void)
{
    return  gu8GprsStatus;
}

void gprs_set_status(S8 status)
{
	gu8GprsStatus = status;
}

/*void gprs_clear_status(void)
{
     gu8GprsStatus = HY_ERROR;
}*/


static  U8 gu8GprsSendMute = 0;
int gprs_send_data(char *buf, int len)
{
	 
	 if(GsmReset_IfReset() == 1)
	 {
	 	return -1;
	 }
	 
	 if (gu8GprsSendMute == 1)
	 {
	     while(1)
	     {
	         syssleep(1);
	         if (0 == gu8GprsSendMute) break;
	     }
	 }
	 
	 gu8GprsSendMute = 1;
	 lgsm_gprs_send(lgsmh_gprs,buf,len);
	 gu8GprsSendMute = 0;
	 
	 return 0;
}
int gprs_connect_intial(void)
{
    struct lgsm_data_connect_set connect_set;
				
    memset(&connect_set, 0, sizeof(connect_set));
    lgsm_gprs_connect_init(lgsmh_gprs, &connect_set);
}

/*
 * return type 0: 没有数据  1:有数据 2：连接上  3：连接失败
*/
int gprs_get_data(char *buf, int *len)
{
     int getLen;
     int rv;
     
     rv = gprs_main(lgsmh_gprs,buf,&getLen);
     
     if (rv == PPP_RT_DATA)
     {
         *len = getLen;
     }
     
     return rv;     
}

int gprs_main(struct lgsm_handle *lh, char *getBuf, int *getLen)
{
	char buf[2048];
	struct gsmd_msg_hdr *gmh;
	struct gsmd_evt_auxdata *aux;
	int len,*result,rc = 0;
	int gsm_fd = lgsm_fd(lh);

	//receive data on the gsmd socket
	len = u_recv(gsm_fd, buf, sizeof(buf), 0);
//hyUsbPrintf("ppp recv ==%d   =%s\r\n",len,buf);
	if(0<len)
	{
	//hyUsbPrintf("ppp recv ==%d\r\n",len);
		if (len < sizeof(*gmh))
			return PPP_RT_ERROR;
		gmh = (struct gsmd_msg_hdr *) buf;

		if (len - sizeof(*gmh) < gmh->len)
			return PPP_RT_ERROR;

		if (gmh->msg_type >= __NUM_GSMD_MSGS)
			return PPP_RT_ERROR;
//hyUsbPrintf("ppp gmh->msg_type ==%d  %d\r\n",gmh->msg_type,gmh->msg_subtype);
		switch(gmh->msg_type)
		{
		case GSMD_MSG_DATACALL:
			switch(gmh->msg_subtype)
			{
#if 0			
			case GSMD_DATACALL_DIAL:
				result = (int *) ((char *) gmh + sizeof(*gmh));
				if(0 == *result)
				{
					//GPRS CONNECT									//GPRS CONNECT
				}
				else
				{  //连接失败
				
				}
				break;
#endif				
			case GSMD_DATACALL_TRANSFER:
				if(getBuf != NULL)
				{
					memcpy(getBuf, gmh->data, gmh->len);	//一个完整GPRS数据包
				}
				*getLen = gmh->len;				
				return PPP_RT_DATA;
				break;
				
			case GSMD_DATACALL_CGDCONT:
			    result = (int *) ((char *) gmh + sizeof(*gmh));
				if(0 == *result)
				{
					 return PPP_RT_CGT;
				}
				else
				{  //连接失败
				     return PPP_RT_DIS;
				}
				break;
			}
			break;
		
		case GSMD_MSG_EVENT:
			aux = (struct gsmd_evt_auxdata *) gmh->data;
			switch(gmh->msg_subtype)
			{
			case GSMD_EVT_CIPHER:
				if(0 == aux->u.cipher.net_state_gprs)
				{
					return PPP_RT_DIS;
				}
				else if(1 == aux->u.cipher.net_state_gprs)
				{
					return PPP_RT_CON;
				}
				break;
				
			default:
			    return  PPP_RT_NONE;
				break;
			}
			break;
		}
	}
		
	return PPP_RT_NONE;
}

//--------------------得到外部user及password的外部接口------------------
/*-----------------------------------------
* 函数：ppp_get_password
* 功能：得到外部的用户名及密码
* 参数：none
* 返回：如果成功则返回 >=0 否则失败
*-------------------------------------------*/
int ppp_get_password(char *pueser, char *passwd)
{
	 int rv = -1;
	 
	 if (!pueser || !passwd)
	 {
	     return rv;
	 }
	 
#if 0
	 if (gtHyc.GprsAccess > 3)
	 {
    	 strcpy(pueser,"none");
    	 strcpy(passwd,"none");
	 }	 
	 else
	 {
    	 //strcpy(pueser,Info.user_name);
    	 //strcpy(passwd,Info.pwd);
    	 strcpy(pueser,"none");
    	 strcpy(passwd,"none");
	 }
#else
	 strcpy(pueser,"none");
	 strcpy(passwd,"none");
#endif
	 
	 return 0;
	 
}


//#define PPP_DEBUG

#define PPP_LOG_FILE  "C:/Ip.txt"

int pppDebugFile = -1;

void PPP_RecvIPPack(char *pbuf, int len)
{
#ifdef PPP_DEBUG
    int fp;
    char *str = "\npppLog";
    
    if (pppDebugFile < 0)
    {
        char *startstr = "\n\n\n-------------  log - start  ---------------\n";
        
        fp = AbstractFileSystem_Open(PPP_LOG_FILE,1);
    	if(0>fp)
    	{
    		fp = AbstractFileSystem_Create(PPP_LOG_FILE);
    		if (0>fp)
    		{
    		}
    		else
    		{
    			AbstractFileSystem_Seek(fp,0,0); //AFS_SEEK_OFFSET_FROM_START
    			AbstractFileSystem_Write(fp, startstr, strlen(startstr));
    		}
    	}
    	else
    	{
    		AbstractFileSystem_Seek(fp,0,2); //AFS_SEEK_OFFSET_FROM_END
    		AbstractFileSystem_Write(fp, startstr, strlen(startstr));
    	}

    	pppDebugFile = fp;
    }
    
    if (pppDebugFile < 0) return;
    
    
    AbstractFileSystem_Write(pppDebugFile, str, strlen(str));
    AbstractFileSystem_Write(pppDebugFile, pbuf, len);
#endif    
}

void PPP_RecvIpClose(void)
{
#ifdef PPP_DEBUG 
    if (pppDebugFile >= 0)
    {
        AbstractFileSystem_Close(pppDebugFile);
    }
    
    pppDebugFile = -1;
#endif    
}
