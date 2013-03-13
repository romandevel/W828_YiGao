/*----------------------文件描述--------------------------------
*创建日期: 08/03/07
*描述    : 开机进程
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "appControlProcess.h"
#include "grap_api.h"

/*---------------------------------------------------------
*函数: PowerDown_ThreadMethod_Init
*功能: 初始化一些变量或overlay变量等
*参数: 
*返回: none
*---------------------------------------------------------*/
static void PowerDown_ThreadMethod_Init(void)
{
}

/*---------------------------------------------------------
*函数: PowerDown_ThreadMethod
*功能: 系统关机时调用该进程，显示关机动画
*参数: 
*返回: none
*---------------------------------------------------------*/
int PowerDown_ThreadMethod(void)
{
	U8 MsgBody[POWER_DOWN_MSG_MAX_SIZE];
	U8 PutMsgBody[(POWER_DOWN_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];

	tGRAPDESKBOX LocalDesk;
	U8   focus;	
	U8   QuitAll = 0;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION   *pPro;
    U32 putadd;
	
	PowerDown_ThreadMethod_Init();	
	
	memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	
	QuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &QuitAll;
	
	
	pPro = AppCtrl_GetAppInfor(POWER_DOWN_AN);
	
	LocalDesk.pPutMsg = &putadd;
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
	
	PowerDown_ShowDeskbox(&LocalDesk);
	
	if (NULL == *LocalDesk.pPutMsg)
    {
    	*LocalDesk.pPutMsg = ptMsg_controlDesk;
    	LocalDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	    LocalDesk.ptPutMsgParam->priority = 10;
	    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID);
    }
    
    //不考虑发不出的情况
    msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);

	return 1;
	
}