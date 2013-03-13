#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwCkc.h"


#include "LcdCPU.h"
#include "LedCtl.h"

#include "USBHandler_p.h"

#include "grap_api.h"
#include "base.h"
#include "USB_ShowDeskbox.h"

#include "DisplayMemory.h"
#include "net_api.h"
#include "glbVariable_base.h"
#include "LcdModule_BackLight.h"



/*
(USB_PLUG_IN_SCAN_CYCLE * USB_PLUG_IN_CNT * systick) = USB Vbus扫描总时间
*/
#define USB_PLUG_IN_SCAN_CYCLE      (10)//USB Vbus扫描周期(以系统tick计)
#define USB_PLUG_IN_CNT             (3)//USB Vbus插入计数


extern void hyudsDcd_connectInit(void);



/*---------------------------------------------------------
*函数: usb_is_active
*功能: 判读USB状态
*参数: none
*返回: -1：不是USB状态  0: 是
*---------------------------------------------------------*/
int usb_is_active(void)
{
    tAPPLICATION   *pPro;
    int rv = -1;
    
    pPro = AppCtrl_GetAppInfor(USB_HANDLER_AN);
    
    if (1 == pPro->isFocus)
    {
        rv = 0;
    }
    
    return rv;
}

int USBHandler_ThreadMethod(void)
{

	U8   focus = FALSE;
	U8   UsbQuitAll = 0;
    char MsgBody[USB_MSG_MAX_SIZE];
    char PutMsgBody[(USB_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];
    tGRAPDESKBOX   LocalDesk;    
    tAPPLICATION   *pPro;
    U32 putadd;
    U32 len;
    S32 ret;
    int count ;
    extern void drvKeyb_GetEvent_Timer0ISR(void);
    
    LcdModulePowerOnOff(0);
    
    drvKeyb_ClearKey();
	hyhwInt_ConfigKeyScanTimer0_ISR(drvKeyb_GetEvent_Timer0ISR); 
       
    
    memset(PutMsgBody,0,sizeof(PutMsgBody));
    memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
    
    UsbQuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &UsbQuitAll;
	
	LocalDesk.pPutMsg = &putadd;
	
	pPro = AppCtrl_GetAppInfor(USB_HANDLER_AN);
	
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
	
	Led_ctrl(LED_RED);
	//gsensor_destroy(0xf);
    //PhoneTrace(0,"testtt");
    //syssleep(1000);
    //避免内存泄漏
    //phb_if_save();
    //while(1)
	{
	    //ret = msg_queue_get(LocalDesk.pGetMsg,(char *)LocalDesk.ptGetMsgBody,&len);
	
		//if (ret >= MSG_QUEUE_OK && SYS_FOCUS_EVENT == LocalDesk.ptGetMsgBody->id)
		{ 
            bUSB_Status = FALSE;
            gs32VolumeFd = -1;
            
        	USB_ShowDeskbox(&LocalDesk);
            
            //break;
        }
    }
    
#if 0

	if (USBHandler_USBConnect() == 0)
    {//再次检测一下USB状态     
	    USBHandler_SetScsiFlag(0);
	}
    if (NULL == *LocalDesk.pPutMsg)
    {
    	*LocalDesk.pPutMsg = ptMsg_controlDesk;
    	LocalDesk.ptPutMsgParam->body.id = USE_REQ_START_TASK_EVENT;
    	LocalDesk.ptPutMsgParam->body.msg.Pid.MyPid =  USB_HANDLER_AN;
    	LocalDesk.ptPutMsgParam->body.msg.Pid.StartPid = POWER_ON_AN;
	    LocalDesk.ptPutMsgParam->priority = 10;
	    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
    }
	hyudsDcd_HwDeinit();
	hyudsDcd_StopSpeedDetect();
#endif

	LcdModulePowerOnOff(0);
	while(1)
	{
		hyhwBatteryPowerOnOff(0);
		for (count=0; count < 5000; count++);
	}
	
	if ((GetResouceBmpOffset() == -1) ||				//没有资源文件
		(0 != AppCtrl_IsValidPro(GSMCDMA_SERVER_AN)))	//GSM集成已经运行
	{
		//系统直接复位
		Vector_Init_Block();
#if 0		
		LcdModulePowerOnOff(0);
		InterruptDisable();
		while(1)
		{
			hyhwBatteryPowerOnOff(0);
		}
#endif		
	}

    LcdModule_LightBack();
    
    drvKeyb_ClearKey();
	hyhwInt_ConfigKeyScanTimer0_ISR(NULL); 
    
    //不考虑发不出的情况
    ret = msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);

    return 1;
}


void DisplayUSBInterface(void)
{
   // char string[10];
    
    drvLcd_Background2DisplayMemory(16,64,40,0);
    drvLcd_SetColor(0x001F, 0x0000);
    drvLcd_DisplayString(40,0,0,(unsigned char *)"USB", NULL, 0);
    DisplayData2Screen();
}
