/*----------------------文件描述--------------------------------
*创建日期: 08/03/07
*描述    : 显示开机动画
*--------------------------------------------------------------*/

/*
*include files
*/
#include "grap_api.h"
#include "base.h"
#include "drvKeyb.h"
#include "glbVariable_base.h"
#include "LedCtl.h"

const U32 gu32PowerOnCtn[]={TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,
                     TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN};
/*
*各控件的公共属性
*/
const tGrapViewAttr gtPowerOnViewAttr[]=
{
	{COMM_DESKBOX_ID,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	
	 TY_ON_20_EN,	TY_ON_20_EN,0,	0,	1,	0,	0,	0},
	{CARTOON_ID_1,	24, 93,196, 78,0,0,0,0,
	 TY_ON_T_EN,	TY_ON_T_EN,	0,	0,	1,	0,	0,	0},

};

/*
*定义动画的私有属性
*/
const tGrapCartoonPriv gtPowerOnCtn[]=
{
	{0,1,8,0,10,0,(U32 *)&gu32PowerOnCtn,TY_ON_20_EN},
};
							 
/*---------------------------------------------------------
*函数: PowerOn_CartoonHandle
*功能: 开机动画handle 处理      
*参数: none
*返回: none
*---------------------------------------------------------*/
S32 PowerOn_CartoonHandle(void *pView, GRAP_EVENT *pEvent )
{
	U32 message,frame;
	tGRAPCARTOON *pCartoon;
	tGrapCartoonPriv *pcartoon_privattr;
	
	pCartoon = (tGRAPCARTOON *)pView;
	pcartoon_privattr = &(pCartoon->cartoon_privattr);

	message = pEvent->message ;
	switch(message)
	{
	case VIEW_TIMER:
		if (pcartoon_privattr->lftTimeIntv == 0)
		{
			pcartoon_privattr->lftTimeIntv = pcartoon_privattr->timeIntv;	//reset timer
			
			frame = pcartoon_privattr->curFrame;
			
			if (frame >= pcartoon_privattr->totFrame-1)
			{//开机结束,启动shell进程，进入主界面进行显示
				tMSG_PUT_PARAM *ptPutMsgPara;
				tGRAPDESKBOX *pDesk;
				
				Led_ctrl(0);//关
				
				pDesk = pCartoon->pParent;
				*pDesk->pPutMsg = ptMsg_controlDesk;
				
				ptPutMsgPara =  pDesk->ptPutMsgParam;
					
			    ptPutMsgPara->body.id = USE_REQ_START_TASK_EVENT;
			    ptPutMsgPara->body.msg.Pid.MyPid = POWER_ON_AN; 	
				ptPutMsgPara->body.msg.Pid.StartPid = SHELL_AN;				
				ptPutMsgPara->priority = 10;
				ptPutMsgPara->length =  sizeof(tPIDINFOR)+sizeof(eMSG_ID);
				*((tGRAPDESKBOX *)(pCartoon->pParent))->pQuitCnt = 0xff;
				
				return RETURN_QUIT;
			}
			else if(frame == pcartoon_privattr->totFrame-2)
			{
				Battery_NormalDetect();
			}
			pCartoon->draw(pCartoon, 0, 0);
			if (frame == 1)
			{
				Led_ctrl(LED_CLOSE);
				
				LcdModulePowerOnOff(1);//为将亮度设置为保存值
				
				drvKeyb_ClearKey();
	            hyhwInt_ConfigKeyScanTimer0_ISR(drvKeyb_GetEvent_Timer0ISR);
	            scanner_init();
			}
			
			pcartoon_privattr->curFrame++;
			
			Led_ctrl(pcartoon_privattr->curFrame%8);
		}
		else
		{
			pcartoon_privattr->lftTimeIntv--;
		}
		
		{
			tStrDispInfo disp_info;
			U8  showBuf[100];
			
			disp_info.color = 0xFFFF;
			disp_info.font_size = 0;
			disp_info.font_type = 0;
			disp_info.flag = 0;
			disp_info.string = showBuf;
			disp_info.keyStr = NULL;
			strcpy(showBuf,appVersionInfo_all);
			
			Grap_WriteStringMulti(72, 228, 240, 276, &disp_info);
		}
		break;
	default:
		break;	
	}
	
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: PowerOn_ShowDeskbox
*功能: 开机动画显示,这里只是简单的动画显示，之后可以做开机铃声等      
*参数: none
*返回: none
*---------------------------------------------------------*/
void PowerOn_ShowDeskbox(void *pDeskFather)
{

	tGRAPDESKBOX *pDesk;
	tGRAPCARTOON *pCartoon;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtPowerOnViewAttr[0], NULL);
	if (NULL == pDesk ) return;	
	
    Grap_Inherit_Public(pDeskFather,pDesk);
	
	pCartoon = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtPowerOnViewAttr[1],(tGrapCartoonPriv*)&gtPowerOnCtn[0]);
	if(NULL == pCartoon) return;
	pCartoon->handle = PowerOn_CartoonHandle;
	
	Grap_DeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
	
}

/*-------------------------修改记录--------------------------------
*
*-----------------------------------------------------------------*/