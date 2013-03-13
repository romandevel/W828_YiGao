/*----------------------文件描述--------------------------------
*创建日期: 08/02/06 
*描述    : 总的UI调度进程，可以创建或者唤醒进程
*--------------------------------------------------------------*/
/*
*include local files
*/
#include "grap_api.h"
#include "Common_Widget.h"
#include "drvKeyb.h"
#include "app_access_sim.h"
#include "glbVariable.h"

//extern U32 gu32TestTotal,gu32TestSend;

static U8 gu8ShellPermit = 0;


//开启需要的服务
int Shell_StartService()
{
	ConSume_UpdataSpeedDirect(SHELL_AN, 120);
	
	t9_init();			//联想字库
	
	Net_BackEvt_CreateThread(); /* 创建网络事件处理进程0313 */
	syssleep(50);
	
	WifiConnect_Start();//wifi后台自动连接进程
	
	GsmReset_Start();	//gsm后台重启进程
	
	ppp_p_Start();		//ppp进程
	
	bt_create_process();//bt
	
	//Gps_Init();
	
	Remind_Fun(0,0);
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
}


//停止开机时启动的服务
int Shell_StopService()
{
	glbAllVariable_SaveParameter();
	
	t9_deinit();//联想字库
	
	Net_BackEvt_ExitThread();//后台网络事件处理进程
	
	WifiConnect_p_Exit();	//wifi后台自动连接进程
	
	WifiDriver_Exit();	//wifi进程
	
	GsmReset_p_Exit();	//gsm后台重启进程
	
	ppp_p_Exit();		//ppp进程
	
	bt_close();		//bt
	
	netdev_remove_gsm_dev();//tcp
	netdev_close_tcpip();
	
	gsmcdma_stop_service();//gsm
	
	//Gps_DeInit();
}

int Shell_main_handle(tGRAPDESKBOX *pDesk)
{
	int 		len;
	U16 		id;
	uMSG 		*pMsg;
	tGRAPVIEW	*pWidget;
	tGRAPEDIT	*pEdit;	
	U8 			code[100];
	U8          i;
	
	if(pDesk->scanEnable)
	{
		len = 0;
		memset(code, 0, 100);
		barBuf_readBarCode(&code[0], &len);
		
		if(len > 0)
		{
			//给扫描头发送低功耗命令
			//hwScanner_sendLowCmd(0);
			//BackLight_UpdataLightTime(10);
			
			drvKey_ClearScanTime();
			
			pWidget = (tGRAPVIEW *)Grap_GetCurFocus(pDesk,NULL);
			if(pWidget)
			{
				pEdit = (tGRAPEDIT *)pWidget;
				id = pEdit->view_attr.id;
				if(pEdit->edit_privattr.scanEnable && id >= pDesk->editMinId && id <= pDesk->editMaxId)
				{
					/*获取条码成功*/
					Remind_Fun(0, 2);
					
					memset(pEdit->edit_privattr.digt,0,pEdit->edit_privattr.containLen);
					if(len > pEdit->edit_privattr.containLen)
					{
						len = pEdit->edit_privattr.containLen;
					}
					memcpy(pEdit->edit_privattr.digt,code,len);
					pEdit->edit_privattr.Pos = len;
					pEdit->editKeyEnd(pEdit, 0);
					if(pEdit->edit_privattr.sendOK)//是否发送OK键
					{
						pDesk->ptGetMsgBody->id = KEY_INPUT_EVENT;
						pMsg = &(pDesk->ptGetMsgBody->msg);
						pMsg->Key.keycode  = Keyb_Code_OK;
						pMsg->Key.keyevent = 1;
					}
					else
					{
						ReDraw(pDesk, 0, 0);
						pDesk->ptGetMsgBody->id = 0;
					}
				}
			}
		}
	}
		
	return HY_ERROR ;
}


/*---------------------------------------------------------
*函数: Shell_ThreadMethod_Init
*功能: 初始化一些变量或overlay变量等
*参数: 
*返回: none
*---------------------------------------------------------*/
static void Shell_ThreadMethod_Init(void)
{

}

/*---------------------------------------------------------
*函数: Shell_Permit_Enter
*功能: 是否允许进入
*参数: 
*返回: 1允许  否则不能
*---------------------------------------------------------*/
int Shell_Permit_Enter(void)
{
     return gu8ShellPermit;
}

/*---------------------------------------------------------
*函数: Shell_ThreadMethod
*功能: 系统初始化后调用该进程，相当于以前的swichmode但是该进程
*      可以创建或或者唤醒其它UI进程
*参数: 
*返回: none
*---------------------------------------------------------*/
int Shell_ThreadMethod(void)
{
	tGRAPDESKBOX SwiDesk;
	U8  gbSwiFocus;
	U8  gu8SwiQuitAll;
	U8  MsgBody[SHELL_MSG_MAX_SIZE];
	U8  PutMsgBody[(SHELL_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];
	U8 	rv;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION *pPro;
    U32 putadd;
    	
	Shell_ThreadMethod_Init();	
	
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	memset(&SwiDesk,0,sizeof(tGRAPDESKBOX));
	
	gu8SwiQuitAll = 0;
	gbSwiFocus = FALSE;
	
	SwiDesk.pUIFocus = &gbSwiFocus;
	SwiDesk.pQuitCnt= &gu8SwiQuitAll;
	
	
	pPro = AppCtrl_GetAppInfor(SHELL_AN);
	
	SwiDesk.pPutMsg = &putadd;
	SwiDesk.pGetMsg = pPro->ptmsg;
	*SwiDesk.pPutMsg = NULL;
	SwiDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	SwiDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
        
    gu8ShellPermit = 0;	
	
    if(gtHyc.NetSelected == 1)//选择的gsm网络
	{
		//if((rv = Common_CheckPin(&SwiDesk)) == 0)
	    {
			
	    }
		SIM_set_pin_stat(1);
	}
	
    gu8ShellPermit = 1;	
	SwiDesk.threadCallback = Shell_main_handle;
	
	Shell_StartService();
	if (*SwiDesk.pQuitCnt == 0)
	{		
    	pgtComFirstChd = ComWidget_CreatAllObj();
    	*(SwiDesk.pUIFocus) = TRUE;	       
		
		if(Rec_CheckRecMaxSize() > 512)//超出限制
		{
			Com_SpcDlgDeskbox("记录大小超出范围.",0,&SwiDesk,2,NULL,NULL,0xFFFFFFFF);
		}
		
		//gu32TestTotal = 0;
		//gu32TestSend  = 0;
		
		//TestFun_Auto(&SwiDesk);
        Login_Desk(&SwiDesk);        
        
        ComWidget_DeleteAllObj(pgtComFirstChd);
        pgtComFirstChd = NULL;
    }

    if (NULL == *SwiDesk.pPutMsg)
    {
    	*SwiDesk.pPutMsg = ptMsg_controlDesk;
    	SwiDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	    SwiDesk.ptPutMsgParam->priority = 10;
	    SwiDesk.ptPutMsgParam->length = sizeof(eMSG_ID);
    }
    
    //不考虑发不出的情况
    msg_queue_put(*SwiDesk.pPutMsg,(char *)&SwiDesk.ptPutMsgParam->body,SwiDesk.ptPutMsgParam->length,SwiDesk.ptPutMsgParam->priority);
	
	return 1;
}

void shell_savescreen()
{
	tGRAPDESKBOX *ptCurDesk;
	char path[50];
	U16  *pData;
	
	
	ptCurDesk = Grap_Get_Current_Desk();
	
	pData = (U16 *)Get_Display_DisplayMemory_Offset();
	
	sprintf(path, "C:/%s.bmp", ptCurDesk->pDisName);
	
	BMP_SaveFile(pData, 240, 320, path);
}

void shell_log(char *name)
{
#if 0
	static U32 logtime=0;
	int  fp;
	U8   buf[100],date[22];
	char path[100];
	
	if(name == NULL && ABS(rawtime(NULL)-logtime) < 18000)
	{
		return ;
	}
	
	if(name != NULL)
	{
		sprintf(path, "C:/%s.txt", name);
	}
	else
	{
		strcpy(path, "C:/log.txt");
	}
	
	OpCom_GetTimeStr(date);
	sprintf(buf, "%s  bat=%d  T=%d;S=%d\r\n", date, Battery_Detect(),gu32TestTotal,gu32TestSend);
	
	fp = AbstractFileSystem_Open(path, 2);
	if(fp >= 0)
	{
		AbstractFileSystem_Seek(fp, 0, 2);
		AbstractFileSystem_Write(fp, buf, strlen(buf));
		AbstractFileSystem_Close(fp);
	}
	else
	{
		fp = AbstractFileSystem_Create(path);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp, buf, strlen(buf));
			AbstractFileSystem_Close(fp);
		}
	}
	
	logtime = rawtime(NULL);
#endif
}