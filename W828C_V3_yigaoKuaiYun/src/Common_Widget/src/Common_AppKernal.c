/*
 * file name : common_AppKernal.c
 * 功能
*/

/*
 * include files
 */
 
#include "std.h"
#include "appControlProcess.h"
#include "grap_api.h"

//函数指针用于进程的实际执行
static void (*kernal_app_process)(void); 
static int (*cartoon_app_process)(void *p);
static void *pCApp;

static volatile U8 gu8KernalEventCpt = 0xff; //内核进程事件是否完成标志

/*--------------------------------------------------------
 * 函数 ；kernal_app_complete
 * 功能 : 判断内核进程是否完成
 * 返回 : 其他：完成 0xff：未完成 
 *-------------------------------------------------------*/
int kernal_app_get_stat(void)
{
    return  gu8KernalEventCpt;
}

/*--------------------------------------------------------
 * 函数 ；kernal_app_config_entry
 * 功能 : 注册进程运行函数
 * 参数 : 函数指针
 * 返回 : none 
 *-------------------------------------------------------*/
void kernal_app_config_entry(void (*process)(void))
{
    kernal_app_process =  process;
    gu8KernalEventCpt = 0xff;   
    
    cartoon_app_process = NULL;
    pCApp = NULL;
}

/*--------------------------------------------------------
 * 函数 ；cartoon_app_entry
 * 功能 : 注册进程运行函数
 * 参数 : 函数指针
 * 返回 : none 
 *-------------------------------------------------------*/
void cartoon_app_entry(int (*process)(void),void *p)
{
    cartoon_app_process =  process;
    pCApp = p;
    gu8KernalEventCpt = 0xff;
    
    kernal_app_process = NULL;
}

/*--------------------------------------------------------
 * 函数 ；Kernal_App_ThreadMethod
 * 功能 : 应用进程的内核进程
 * 返回 : 1 
 *-------------------------------------------------------*/
int Kernal_App_ThreadMethod(void)
{
	U8 MsgBody[KERNAL_APP_MSG_MAX_SIZE+1];
	U8 PutMsgBody[(50+MSG_PUT_HEAD_SIZE)];

	tGRAPDESKBOX LocalDesk;
	U8   focus;	
	U8   QuitAll = 0;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION   *pPro;
    U32 putadd;
    U32 startid;
    S32 ret;
    U32 len;
	
	gu8KernalEventCpt = 0xff;
	
	memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	
	QuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &QuitAll;
	
	pPro = AppCtrl_GetAppInfor(KERNAL_APP_AN);
	
	LocalDesk.pPutMsg = &putadd;
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
	
    if (NULL != kernal_app_process)
    {
        kernal_app_process();
        kernal_app_process = NULL;
        ret = 1;
    }
    else if(NULL != cartoon_app_process)
    {
    	ret = cartoon_app_process(pCApp);
    	cartoon_app_process = NULL;
    	pCApp = NULL;
    }
    gu8KernalEventCpt = ret; 
 	
	exit(1);
	
#if 0    
	*LocalDesk.pPutMsg = ptMsg_controlDesk;
	LocalDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	LocalDesk.ptPutMsgParam->body.msg.Pid.MyPid = KERNAL_APP_AN; 	
	LocalDesk.ptPutMsgParam->priority = 10;
    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
     
    //不考虑发不出的情况
    msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);
 #endif   
    return 1;
}