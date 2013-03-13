
/*
*include local files
*/
#include "std.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "Battery.h"
#include "LcdModule_BackLight.h"
#include "TcpIp_p.h"

#include "app_access_sim.h"
#include "Net_Evt.h"

/**************************************** 界面顶部公共控件 *******************************************/
tGRAPLABEL *pgtComFirstChd=NULL;

static U8  	gsu8GprsStep;
static U8 	su8ComCount,su8Comflag;
static U8	gsu8DateShowType;//当前显示的是日期还是时间

static U32 gsm_sigq_level = 0;

static const U8 keyBmp[][30] =  /* 20*10 */
{
	{0x00,0x00,0x00,0x18,0x71,0xC0,0x38,0xFB,0xE0,0x18,0x18,0x60,0x18,0x19,0xC0,0x18,
	0x31,0xC0,0x18,0x60,0x60,0x3C,0xFB,0xE0,0x7E,0xFB,0xC0,0x00,0x00,0x00},//123
	{0x00,0x00,0x00,0x3E,0x00,0x00,0x7F,0x00,0x00,0x03,0x00,0x00,0x1F,0x00,0x00,0x3F,
	0x00,0x00,0x73,0x00,0x00,0x7F,0x00,0x00,0x3F,0x80,0x00,0x00,0x00,0x00},//a
	{0x00,0x00,0x00,0x0C,0x00,0x00,0x1E,0x00,0x00,0x1E,0x00,0x00,0x33,0x00,0x00,0x33,
	0x00,0x00,0x3F,0x00,0x00,0x7F,0x80,0x00,0x61,0x80,0x00,0x00,0x00,0x00},//A
	{0x7F,0xC0,0x00,0x50,0x40,0x00,0x1F,0x00,0x00,0x10,0x00,0x00,0x1F,0xC0,0x00,0x00,
	0xC0,0x00,0x7F,0xC0,0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,0x01,0x80,0x00},//写
	{0x22,0x40,0x00,0x22,0x80,0x00,0x77,0xE0,0x00,0x22,0x40,0x00,0x22,0x40,0x00,0xF7,
	0xF0,0x00,0x22,0x40,0x00,0x22,0x40,0x00,0x24,0x40,0x00,0x68,0x40,0x00},//拼
};

const U8 Gprs_BMP[][9] = //Gprs  8X9
{
	0x18,0x18,0xFF,0x7E,0x3C,0x18,0x00,0x00,0x00,//下载1
	0x00,0x00,0x00,0x18,0x18,0xFF,0x7E,0x3C,0x18,//下载2
	
	0x00,0x00,0x00,0x18,0x3C,0x7E,0xFF,0x18,0x18,//上传1
	0x18,0x3C,0x7E,0xFF,0x18,0x18,0x00,0x00,0x00,//上传2
};

/*
logo(或登录人信息)   记录条数+GPRS工作箭头+GPRS当前事件    日期时间   GSM信号   WIFI信号  电池    输入法
*/
const tGrapViewAttr gtComView[]=
{
	
	{COM_LOGO_ID,	LOGO_X,	LOGO_Y,	LOGO_W,	LOGO_H,	0,0,0,0,	TY_YUNDA_LOGO_EN,TY_YUNDA_LOGO_EN, 0,0, 1,0,0,0},//LOGO
	{COM_GPRSEVT_ID,GPRS_X,	GPRS_Y,	GPRS_W,	GPRS_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//GPRS事件
	{COM_DATE_ID,	DATE_X,	DATE_Y,	DATE_W,	DATE_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//日期时间
	{COM_GSMSIG_ID,	GSM_X,	GSM_Y,	GSM_W,	GSM_H,	GSMWIFI_X1,GSMWIFI_Y1,GSMWIFI_X2,GSMWIFI_Y2,	TY_G_NO_SIGNAL_EN,TY_G_NO_SIGNAL_EN,	0,0,	1,0,0,0},//gsm信号
	{COM_WIFISIG_ID,WIFI_X,	WIFI_Y,	WIFI_W,	WIFI_H,	GSMWIFI_X1,GSMWIFI_Y1,GSMWIFI_X2,GSMWIFI_Y2,	TY_W_NO_SIGNAL_EN,TY_W_NO_SIGNAL_EN,	0,0,	1,0,0,0},//wifi信号
	{COM_BAT_ID,	BAT_X,	BAT_Y,	BAT_W,	BAT_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//电池
	{COM_INPUT_ID,	INPUT_X,INPUT_Y,INPUT_W,INPUT_H,0,0,0,0,	0,0,	0,0,	1,0,0,0},//输入法
	
};

const tGrapLabelPriv gtComLabPriv[]=
{
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//logo
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//GPRS
	{NULL, COLOR_YELLOW, COLOR_YELLOW, FONTSIZE6X9, 0, 0, 0, 0},//日期时间
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//gsm
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//wifi
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//电池
	{NULL, COLOR_YELLOW, COLOR_YELLOW, 0, 0, 0, 0, 0},//输入法
};

/*---------------------------------------------------------
*函数: ComWidget_Init
*功能: 
*参数: none
*返回: 1
*---------------------------------------------------------*/
void ComWidget_Init(void)
{
	gsu8GprsStep = 0;
	gsu8DateShowType = 0;
	gsm_sigq_level = 0;
	SetUpDateState(0);
	
	su8ComCount = 0;
	su8Comflag = 1;	
}

/*---------------------------------------------------------
*函数: Com_DlgButtonPress
*功能: btn按键按下后的操作
*参数: 
*返回: none
*---------------------------------------------------------*/
static S32 ComWidget_EnterBtnPress(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		Com_DlgUpdataStat(DLG_CONFIRM_EN);
		
		if(FALSE == hyudsDcd_Connected())
		{
			Com_DlgUpdataStat(DLG_CANCEL_EN);
		}
		
		pDesk->kDlgState = Com_DlgGetStat();
		
		return RETURN_QUIT;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*函数: ComWidget_RevUSBMsg
*功能: 使用公共空间接收USB消息       
*参数: none
*返回: 
*---------------------------------------------------------*/ 
void ComWidget_RevUSBMsg(void *pView)
{
    tGRAPLABEL *pLabel = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    tMSG_BODY *pMsgBody;
    U8 tempdata[40]={0};
    tMSG_PUT_PARAM	*ptPutMsg = tempdata;
    
    pDesk = pLabel->pParent;
    
    pMsgBody = pDesk->ptGetMsgBody;    
    
    if (USE_USB_IS_ACT == pMsgBody->id)
    {
        ptPutMsg->body.id = USE_USB_IGNORE;         
        
        BackLight_EventTurnOnLcd(pDesk); 
	#if 0
		if(Gprs_getMode() > 0 && 0 == (YDget_gprsevent()&YD_GPRS_FOREGROUND)) /* 只是后台事件才去等待 */
		{	
	        if(DLG_CONFIRM_EN == Com_ShutDownDlgDeskbox(pDesk,"数据正在传输,完成本次传输巴枪将自动进入USB!", Com_Usb_ShutDownLabelTimerISR, DLG_ALLWAYS_SHOW))
	        {
	        	YDreset_gprsevent();
	            ptPutMsg->body.id = USE_USB_ENTER_ACT;
	            pLabel->view_attr.enable = 0 ;  /* 关闭该comGolb控件避免进入labelTimerISR */
	        }
        }
        else 
    #endif
       	{
       		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否连接USB?",0, pDesk, 3,ComWidget_EnterBtnPress,NULL,DLG_DEFAULT_TIME))
	        //if (DLG_CONFIRM_EN == Com_DlgDeskbox(pDesk,8,cDIALOG_IS_ENTER_USB,ComWidget_EnterBtnPress,NULL,DLG_DEFAULT_TIME))
	        {        	
	            ptPutMsg->body.id = USE_USB_ENTER_ACT;
	        }
        } 
        ptPutMsg->length  = sizeof(eMSG_ID);
        ptPutMsg->priority = 10;
       
       	//图片模块未处理
		ReDraw(pDesk,0,0);
		
        msg_queue_put(ptMsg_controlDesk, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
    }
	
	return ;
}


/*---------------------------------------------------------
*函数: ComWidget_CheckObjAttar
*功能: 检测信号和电话等公共图标，并更新     
*参数: pDesk 窗体指针
*返回: 
*---------------------------------------------------------*/
void ComWidget_CheckObjAttar(void *pView)
{
	tGRAPLABEL 		*pLab_Gsm,*pLab_Wifi,*pLab_Bat;
	tGRAPDESKBOX	*pDesk; 
	U32 lev,idx;
    
    pDesk = ((tGRAPVIEW *)pView)->pParent;
    
    pLab_Gsm	= Grap_GetViewByID(pDesk,COM_GSMSIG_ID);
    pLab_Wifi	= Grap_GetViewByID(pDesk,COM_WIFISIG_ID);
    pLab_Bat	= Grap_GetViewByID(pDesk,COM_BAT_ID);

    if(NULL == pLab_Gsm || NULL == pLab_Wifi || NULL == pLab_Bat)
    {
    	return ;
    }
	
	if(gtHyc.NetSelected == 1)//选择的gsm
	{
		//gsm
		if(BackLight_HaveLight() == 1)
		{//没有屏保时
			pLab_Gsm->view_attr.clNormal = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
			pLab_Gsm->view_attr.clFocus  = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
		}
	}	
	else
	{
		//wifi  如果已经连接  需要判断信号
		if(SUCCESS == netdev_get_tcpip_status())
		{
			idx = hy_get_ap_rssi_range(hy_get_connected_ap_rssi());  			 

			if (idx >= 4) idx = TY_W_SIGNAL4_EN;
			else idx = TY_W_NO_SIGNAL_EN+idx;
			
			pLab_Wifi->view_attr.clNormal = idx;
			pLab_Wifi->view_attr.clFocus  = idx;
		}
		else
		{
			pLab_Wifi->view_attr.clNormal = TY_W_NO_SIGNAL_EN;
			pLab_Wifi->view_attr.clFocus  = TY_W_NO_SIGNAL_EN;
		}
	}
	
	//battery
	lev = Battery_Level();
	if(lev > 4) lev = 4;
	
	if(1 == Battery_DcInCheck())
	{
	    if (0 == usb_need_charge())
	    {
            pLab_Bat->view_attr.clNormal = TY_BATTERY5_EN;
            pLab_Bat->view_attr.clFocus  = TY_BATTERY5_EN;
	    }
	    else
	    {
    	    pLab_Bat->view_attr.clNormal = TY_BATTERY1_CHARGE_EN+lev;
    	    pLab_Bat->view_attr.clFocus =  TY_BATTERY1_CHARGE_EN+lev;
	    }
	}
	else
	{
	    pLab_Bat->view_attr.clNormal = TY_BATTERY1_EN+lev;
	    pLab_Bat->view_attr.clFocus  = TY_BATTERY1_EN+lev;
	}

	return ;
}

/*---------------------------------------------------------
*函数: ComWidget_LableEnter
*功能: 点击公共图标当作快捷键进入各功能界面       
*参数: none
*返回:  
*---------------------------------------------------------*/ 
int ComWidget_LableEnter(void *pView, U16 viewX, U16 viewY)
{
	tGRAPDESKBOX 	*pDesk;
	tGRAPLABEL 		*pLable;
	eDLG_STAT		kDlgState;
	U16				id1,id2;
	
	pLable = (tGRAPLABEL *)pView;
	switch(pLable->view_attr.id)
	{
	case COM_LOGO_ID://LOGO
        break;
    case COM_GPRSEVT_ID://GPRS事件
        break;
    case COM_DATE_ID://日期时间
        break;
    case COM_GSMSIG_ID://GSM信号
    case COM_WIFISIG_ID://WIFI信号
    	//进入网络选择
        pDesk = (tGRAPDESKBOX *)(pLable->pParent);
        
    	if(0x12345678 == pDesk->view_attr.reserve)//wifi相关界面
    	{
    		break;
    	}
    	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		Set_NetSelect(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
        break;
    case COM_BAT_ID://电池
        break;
    default:
        break;
	}
    
    return RETURN_REDRAW;
}

/*---------------------------------------------------------
*函数: ComWidget_LabelRecvPullMsg
*功能: 拖拽处理函数       
*参数: none
*返回: 
*---------------------------------------------------------*/
int ComWidget_LabelRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLABEL *pLable;
	tGrapLabelPriv *pLable_privattr;
	tLabelTimer *pLable_timer;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;
	U32 father_id;
	U32 id;
  
	pLable = (tGRAPLABEL *)pView;
	pLable_privattr = &(pLable->label_privattr);
	pLable_timer = &(pLable->label_timer);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	id = pLable->view_attr.id;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pLable))
	    {//在有效范围内
	         
	         ret = VIEW_PULL_START; 
	    }
	    else
	    {
	        return FAIL;
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down 
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	     ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pLable)) 
        {
             return VIEW_PULL_END;
        }
        
        father_id = ((tGRAPDESKBOX *)pLable->pParent)->view_attr.id;
		
		ret = pLable->labelEnter(pLable,x,y);
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pLable, 0, 0);    				
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pLable->pParent))->pQuitCnt = 1;
		}
		else
		{
			pLable->draw(pLable, 0, 0);
		}
		ret = VIEW_PULL_END;

	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;
}


/*--------------------------------------
logo 控件刷新  有登录信息需要显示时显示登录信息  否则显示logo
----------------------------------------*/
int ComWidget_LogoDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U8	disBuf[10];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	//显示本页面名称
	disp_info.string = pDesk->pDisName;
	if(disp_info.string == NULL)
	{//4,  3, 51, 21
		if(pview_attr->clNormal)
		{
			Grap_ShowPicture(pview_attr->clNormal, 4, 3);
		}
		else
		{
			Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		}
	}
	else//有登录信息
	{
		if(strlen(disp_info.string) > 8)
		{
			memcpy(disBuf,disp_info.string, 8);
			disBuf[8] = 0;
			disp_info.string = disBuf;
		}
		
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		
		disp_info.color = pLable_privattr->cl_normal;
		disp_info.font_size = pLable_privattr->font_size;
		disp_info.font_type = pLable_privattr->font_type;
		disp_info.flag = pLable_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	}
	
	return SUCCESS;
}

/*--------------------------------------
gprs 事件控件刷新  包括 记录条数  箭头  当前事件
----------------------------------------*/
int ComWidget_GprsEvtDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U32				recTotal, gprsEvt;
	U8				direct,disBuf[20];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight*2));
	
	disp_info.string 	= disBuf;
	disp_info.font_size = 0;
	disp_info.font_type = 0;
	disp_info.flag = 1;
	
	//获取当前位上传记录条数  最多显示4位
	recTotal = Rec_GetUnSendCnt();
	//if(recTotal > 0)
	{
		disp_info.color = pLable_privattr->cl_normal;
		if(recTotal > 9999)
		{
			recTotal = 9999;
			disp_info.color = COLOR_RED;
		}
		sprintf(disBuf, "记录%d", recTotal);
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1+8, pview_attr->viewY1, 
					pview_attr->viewX1+pview_attr->viewWidth, pview_attr->viewY1+pview_attr->viewHeight, &disp_info);
	}
	recTotal = Rec_GetUnSendImageCnt();
	//if(recTotal > 0)
	{
		disp_info.color = COLOR_YELLOW;
		if(recTotal > 9999)
		{
			recTotal = 9999;
			disp_info.color = COLOR_RED;
		}
		sprintf(disBuf, "图片%d", recTotal);
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1+8, pview_attr->viewY1+pview_attr->viewHeight, 
					pview_attr->viewX1+pview_attr->viewWidth, pview_attr->viewY1+pview_attr->viewHeight*2, &disp_info);
	}
	
	//获取GPRS当前事件 从事件类型可以知道箭头方向
	Net_GetEvtStr(disBuf, &direct);
	if(disBuf[0] != 0 && (direct == 0 || direct == 1))//有事件
	{
		direct *= 2;
		//gprs 箭头  104
		drvLcd_SetColor(0xffff,0xffff);    
    	drvLcd_WriteBMPColor((const U8 *)Gprs_BMP[direct+gsu8GprsStep],9,8,pview_attr->viewY1,pview_attr->viewX1);
		gsu8GprsStep = !gsu8GprsStep;
		
		//事件
//		disp_info.color  	= pLable_privattr->cl_normal;
//		Grap_WriteString(117, 3, 181, 19, &disp_info);
	}
	
	return SUCCESS;
}

/*--------------------------------------
日期时间 控件刷新
----------------------------------------*/
int ComWidget_DateDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U8				disBuf[20];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
				
	//该位置如果有输入框的界面就显示输入框可输入字符和已输入字符数,否则显示登录人姓名
	if(pDesk->inputMode != 0)//有输入法
	{
		//此处不做绘制,由各控件自己重载draw函数绘制
		//Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
		//	(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	/*else
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight+18));
		
		disp_info.string = Login_GetName();
		if(disp_info.string != NULL)
		{
			if(strlen(disp_info.string) > 8)
			{
				memcpy(disBuf,disp_info.string, 8);
				disBuf[8] = 0;
				disp_info.string = disBuf;
			}
			
			disp_info.color = 0xffff;
			disp_info.font_size = 0;
			disp_info.font_type = 0;
			disp_info.flag = 2;
			
			disp_info.keyStr = NULL;
			Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1 + pview_attr->viewHeight+1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight+17), &disp_info);
		}
	}*/
		
	if(gsu8DateShowType == 0)//显示日期
	{
		//sprintf(disBuf, "%02d-%02d-%02d", (gtRtc_solar.year%100), gtRtc_solar.month, gtRtc_solar.day);
		sprintf(disBuf, "%02d-%02d", gtRtc_solar.month, gtRtc_solar.day);
	}
	else if(gsu8DateShowType == 1)//显示时间
	{
		sprintf(disBuf, "%02d:%02d", gtRtc_solar.hour, gtRtc_solar.minute);
	}
	
	disp_info.string 	= disBuf;
	disp_info.color  	= pLable_privattr->cl_normal;
	disp_info.font_size = pLable_privattr->font_size;
	disp_info.font_type = 0;
	disp_info.flag = 2;
	disp_info.keyStr = NULL;
	Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	
	return SUCCESS;
}

/*--------------------------------------
gsm 信号 控件刷新
----------------------------------------*/
int ComWidget_GsmSigDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
gsm 信号 timer
----------------------------------------------*/
void ComWidget_GsmSigTimer(void *pView)
{
	U32 sigq = 0;
	tGRAPLABEL *pLable;
	U16 sigq_id;
	int simcard=1,regStat;
	static U8  hasCard = 0;
	static U16 sigq_id_last = 0;
	static U32 sigqCnt=0,nocard = 0,nosigg=0,noreg = 0;
	
	if(hasCard == 0 && strlen(gsm_ReadSimCard()) > 0)
	{
		hasCard = 1;
	}
	
	if ((BackLight_HaveLight() == 0 && Rec_GetUnSendCnt() == 0 && 0 == Net_GetEvt()) || GsmReset_IfReset() == 1)
	{
		return ;
	}
	    
	pLable = (tGRAPLABEL *)pView;

	//检测是否有SIM卡
	simcard = phone_detect_simcard_nrt();	
	if(simcard == 1)
	{		
		nocard = 0;
		//查询注册状态成功
		regStat = net_query_reg_nrt();
		if(regStat == 1)
		{
			noreg = 0;
			sigq = net_query_sigq_nrt();
			sigq_id = (U16)(sigq>>16);

			if(sigq_id_last != sigq_id)
				sigq_id_last = sigq_id;
			
			sigq = (U32)(sigq & 0x0000FFFF);
		    if(sigq > 31)
		    {
		    	sigqCnt++;
		    	if(sigqCnt>=3)
		    	{
		    		sigq=0;
		    		sigqCnt=0;
		    	}
		    	else
		    	{
		    	    sigq = gsm_sigq_level;//return FAIL;
		    	}
		    }
		    else
		    {
		    	if(sigq==0)sigq = 0;
				else if(sigq < 11)sigq = 1;
				else if(sigq < 16)sigq = 2;
				else if(sigq < 21)sigq = 3;
				else sigq = 4;
				sigqCnt = 0;
			}
			
			if(sigq == 0)
			{
				nosigg++;
				if(nosigg >= 15)
				{
					nosigg = 0;
					PhoneTrace(0, "sigq");
					GsmReset_WakeUp();
				}
			}
			else
			{
				nosigg = 0;
			}
		}
		else
		{
			noreg++;
			if(noreg >= 15)
			{
				noreg = 0;
				PhoneTrace(0, "noreg");
				GsmReset_WakeUp();
			}
			sigq = 0;
		}
		
	}
	else
	{
		noreg = 0;
		nosigg= 0;
		sigq = 0;
		if(hasCard == 1)
		{
			nocard++;
			if(nocard >= 20)
			{
				nocard = 0;
				PhoneTrace(0, "nocard");
				GsmReset_WakeUp();
			}
		}
	}
	gsm_sigq_level = sigq;
	
	pLable->view_attr.clNormal = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
	pLable->view_attr.clFocus  = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
	
	pLable->draw(pLable,0,0);

	return ;
}

/*--------------------------------------
wifi 信号 控件刷新
----------------------------------------*/
int ComWidget_WifiSigDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
wifi 信号 timer
----------------------------------------------*/
void ComWidget_WifiSigTimer(void *pView)
{
	tGRAPLABEL		*pLable;
	U32 idx;
	
	pLable = (tGRAPLABEL *)pView;
	if (SUCCESS == netdev_get_tcpip_status())
	{
		idx = hy_get_ap_rssi_range(hy_get_connected_ap_rssi());  			 

		if (idx >= 4) idx = TY_W_SIGNAL4_EN;
		else idx = TY_W_NO_SIGNAL_EN+idx;
		
		pLable->view_attr.clNormal = idx;
		pLable->view_attr.clFocus  = idx;
	}
	else
	{
		pLable->view_attr.clNormal = TY_W_NO_SIGNAL_EN;
		pLable->view_attr.clFocus  = TY_W_NO_SIGNAL_EN;
	}
	
	pLable->draw(pLable,0,0);
}

/*--------------------------------------
电池 控件刷新
----------------------------------------*/
int ComWidget_BatDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
Bat 信号 timer
----------------------------------------------*/
void ComWidget_BatTimer(void *pView)
{
	tGRAPLABEL		*pLable;
	U32 			lev, batIndex;
	static U8 lastframe = 0;
	static U8 batterLowCnt = 10;
	
	pLable = (tGRAPLABEL *)pView;
	
	lev = Battery_Level();
	if (lev > 4) lev = 4;
	if (1 == Battery_DcInCheck())
	{
        if(1 == usb_need_charge())
        {
	        lastframe++;
	        if (lastframe >= 5)
	        {
	            if(lev == 4) lastframe = 3;
	            else  lastframe = lev;
	        }
	        
	        batIndex = TY_BATTERY1_CHARGE_EN+lastframe;
		    
		    pLable->view_attr.clNormal = batIndex;
		    pLable->view_attr.clFocus  = batIndex;
		}
	}
	else
	{
    	pLable->view_attr.clNormal = TY_BATTERY1_EN+lev;
    	pLable->view_attr.clFocus  = TY_BATTERY1_EN+lev;
	}
	
	pLable->draw(pLable,0,0);
}

//输入法
int ComWidget_InputDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);

	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	
	if(pDesk->inputMode > 0 && pDesk->inputMode <= 5)
	{
		drvLcd_SetColor(COLOR_YELLOW,COLOR_YELLOW);
		drvLcd_WriteBMPColor(keyBmp[pDesk->inputMode-1],10,20,pview_attr->viewY1,pview_attr->viewX1);
	}
	
	return SUCCESS;
}

//公共控件的timer定时处理函数
void ComWidget_TimerISR(void *pView)
{
	tGRAPLABEL *pLable;
	
	pLable = (tGRAPLABEL *)pView;
	
	switch(pLable->view_attr.id)
	{
	case COM_LOGO_ID://LOGO
		pLable->draw(pLable, 0, 0);
        break;
    case COM_GPRSEVT_ID://GPRS事件(记录条数  箭头  当前事件)
        pLable->draw(pLable, 0, 0);
        break;
    case COM_DATE_ID://日期时间
        pLable->draw(pLable, 0, 0);
        gsu8DateShowType++;
		if(gsu8DateShowType > 1)
		{
			gsu8DateShowType = 0;
		}
        break;
    case COM_GSMSIG_ID://GSM信号
        ComWidget_GsmSigTimer(pLable);
        break;
    case COM_WIFISIG_ID://WIFI信号
        ComWidget_WifiSigTimer(pLable);
        break;
    case COM_BAT_ID://电池
        ComWidget_BatTimer(pLable);
        break;
    default:
        break;
	}
}

/*---------------------------------------------------------
*函数: ComWidget_ObjHandle
*功能: 公共对象的handle对象处理句柄，都重载这个函数       
*参数: none
*返回: 
*---------------------------------------------------------*/
int ComWidget_ObjHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPLABEL 		*pLable;
	tGrapLabelPriv 	*pLable_privattr;
	tGRAPDESKBOX	*pDesk;
	U32 id;
    S32 ret = SUCCESS;
	
	pLable = (tGRAPLABEL *)pView;
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	id = pLable->view_attr.id;
	
	switch(pEvent->message)
	{
	case VIEW_DRAW:
	case VIEW_UPDATE:
		ComWidget_CheckObjAttar(pLable);
		pLable->draw(pLable, 0, 0);
		break;	
	case VIEW_TIMER:
		if (pLable->label_timer.enable == 1)
		{
			tLabelTimer *pLable_timer = &pLable->label_timer;
			
			if (pLable_timer->TimerLft == 0)
			{
				pLable_timer->TimerLft = pLable_timer->TimerTot;
				pLable->labelTimerISR(pLable);
			}
			else
			{
				S32 timeleft;
				
				timeleft = pLable_timer->TimerLft - pEvent->lParam;
				
				if (timeleft < 0) pLable_timer->TimerLft = 0;
				else  pLable_timer->TimerLft = timeleft;
			}
		}
		break;
	case VIEW_REV_SYSMSG:    
	    break;
	case VIEW_REV_PULLMSG:
	    ret = pLable->recvPullMsg(pLable,pEvent);
	    break;	
	default:
		break;
	}	
	
	return ret;
}

/*---------------------------------------------------------
*函数: ComWidget_DestroyObj
*功能: 释放对象指针，并没有真正释放对象，因为这是全局的对象
*      指针，程序启动后，其一直驻留在内存中       
*参数: none
*返回: 1
*---------------------------------------------------------*/
static S32 ComWidget_DestroyObj(void *pView)
{
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: ComWidget_CreatAllObj
*功能: 创建公共的电池对象链表      
*参数: none
*返回: 第一个公共控件的指针
*---------------------------------------------------------*/
tGRAPLABEL *ComWidget_CreatAllObj(void)
{
	tGRAPLABEL *pLastObj=NULL,*pCreatObj=NULL;
	U32 i;
//LOGO GPRS 日期时间  gsm  wifi  电池  输入法
	U32 timetotal[]={100,//logo				     
					 100,//GPRS
					 500,//时间和日期				 
					 200,//gsm
					 200,//wifi
					 200,//电池
					 0
					 };

	for(i=0; i<COM_WIDGET_NUM; i++)//
	{
		pCreatObj = Grap_CreateLabel((tGrapViewAttr*)&gtComView[i],(tGrapLabelPriv*)&gtComLabPriv[i]);
		
		if(pCreatObj != NULL)
		{						
			pCreatObj->destroy = ComWidget_DestroyObj;
			pCreatObj->handle  = ComWidget_ObjHandle;
			pCreatObj->labelEnter = ComWidget_LableEnter;
			pCreatObj->recvPullMsg= ComWidget_LabelRecvPullMsg;
			
			if(timetotal[i] > 0)
				pCreatObj->label_timer.enable = 1;
			else
				pCreatObj->label_timer.enable = 0;
			pCreatObj->label_timer.TimerLft = 0;
			pCreatObj->label_timer.TimerTot = timetotal[i];
			pCreatObj->labelTimerISR = ComWidget_TimerISR;
			
			pCreatObj->pNext = (tGRAPVIEW *)pLastObj;
			
			pLastObj = pCreatObj;
			
			switch(i)
			{
			case 0://logo
				pCreatObj->draw = ComWidget_LogoDraw;
				break;
			case 1://gprs
				pCreatObj->draw = ComWidget_GprsEvtDraw;
				break;
			case 2://date
				pCreatObj->draw = ComWidget_DateDraw;
				break;
			case 3://gsm
				if(gtHyc.NetSelected != 1)//未选gsm网络
				{
					pCreatObj->view_attr.enable = 0;
				}
				pCreatObj->draw = ComWidget_GsmSigDraw;
				break;
			case 4://wifi
				if(gtHyc.NetSelected != 0)//未选wifi网络
				{
					pCreatObj->view_attr.enable = 0;
				}
				pCreatObj->draw = ComWidget_WifiSigDraw;
				break;
			case 5://bat
				pCreatObj->draw = ComWidget_BatDraw;
				break;
			case 6://input
				pCreatObj->draw = ComWidget_InputDraw;
				break;
			default:
				break;
			}
		}
	}
	
	return pCreatObj;
}
/*---------------------------------------------------------
*函数: ComWidget_DeleteAllObj
*功能: 释放公共对象链表,只有在关机时才真正使用这个函数来释放所有公共对象      
*参数: none
*返回: 第一个公共控件的指针
*---------------------------------------------------------*/
void ComWidget_DeleteAllObj(void *pView)
{
	tGRAPLABEL *pSubChi,*pSubLast;
	
	if(NULL == pView)
	{
		return ;
	}
	
	pSubChi = (tGRAPLABEL *)pView;
	pSubLast = pSubChi;
	while(pSubChi)
	{
		pSubLast = pSubChi;
		pSubChi = (tGRAPLABEL *)pSubChi->pNext;
		free((void *)pSubLast);
	}
	
	return ;	
}


/*---------------------------------------------------------
*函数: ComWidget_Link2Desk
*功能: 把公共空件连接到desk上      
*参数: pDesk：要连接的desk指针 pFirstCom:第一个控件的指针
*返回: 
*---------------------------------------------------------*/
void ComWidget_Link2Desk(void *pDesk,void *pFirstCom)
{
	tGRAPVIEW *pParentView;
	tGRAPVIEW *pSubChi;
	
	if(NULL == pDesk || NULL == pFirstCom)
	{
		return ;
	}	
	
	pParentView = (tGRAPVIEW *)pDesk;	
	
	pSubChi = (tGRAPVIEW *)pFirstCom;
	while(pSubChi)
	{
		
		pSubChi->pParent = pParentView;
		
		if(pSubChi->pNext == NULL) 
		{	
			pSubChi->pNext = ((tGRAPDESKBOX *)pParentView)->pNext;
			break;
		}
		pSubChi = pSubChi->pNext; 
	}	
	
	((tGRAPDESKBOX *)pParentView)->pNext = (tGRAPVIEW *)pFirstCom;
	
	return ;
}

/*---------------------------------------------------------
*函数: ComWidget_Link2Desk
*功能: 把公共空件连接到desk上      
*参数: pDesk：要连接的desk指针 pFirstCom:第一个控件的指针 TotalChild:脱离窗体控件个数
*返回: 
*---------------------------------------------------------*/
void ComWidget_LeftFromDesk(void *pDesk,void *pFirstCom,U32 TotalChild)
{
	tGRAPVIEW *pParentView;
	tGRAPLABEL *pSubChi;
	 
	U32 i;
	
	if(NULL == pDesk || NULL == pFirstCom || TotalChild == 0)
	{
		return ;
	}
		
	pParentView = (tGRAPVIEW *)pDesk;	
	pSubChi = (tGRAPLABEL *)pFirstCom;
	
	for(i=0; i<TotalChild; i++)
	{
		if(i == TotalChild-1) 
		{
			((tGRAPDESKBOX *)pParentView)->pNext = (tGRAPVIEW *)pSubChi->pNext;
			pSubChi->pNext = NULL;
			break;
		}
		pSubChi = (tGRAPLABEL *)pSubChi->pNext;
		
	}
	
	return ;
}

