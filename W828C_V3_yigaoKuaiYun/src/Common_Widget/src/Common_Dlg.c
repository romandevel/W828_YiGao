/*----------------------文件描述--------------------------------
*创建日期: 08/02/06 
*描述    : 总的UI调度进程，可以创建或者唤醒其它UI模块进程
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyTypes.h"
#include "LcdAPIFunc.h"

/*
*include local files
*/
#include "grap_api.h"
#include "base.h"

#include "Common_Dlg.h" 
#include "glbVariable_base.h"

#include "beeperCtrl.h"
#include "drvKeyArray.h"

//当显示屏为横屏时显示的对话框
#define COM_DLG_BCK_X  50 //对话框显示的总的背景 10, 83,
#define COM_DLG_BCK_Y  60

#define COM_DLG_TIPICON_X   57
#define COM_DLG_TIPICON_Y   65

#define COM_DLG_ONLYCONFORM_X   84
#define COM_DLG_ONLYCONFORM_Y   128


#define COM_DLG_CONFORM_X  59
#define COM_DLG_CONFORM_Y  127
 
#define COM_DLG_CANCEL_X  163
#define COM_DLG_CANCEL_Y  127

static eDLG_STAT gu8DlgState = DLG_CANCEL_EN;

static U32 gu32DlgTimeOutTotal = 0;
static U32 gu32DlgStartTime = 0;

int gs32PercentTotal=0,gs32PercentRecv=0;

/*
*各控件的公共属性
*/
const tGrapViewAttr gtComDlgViewAttr[]=
{
 //     id                  viewX1         viewY1   viewWidth  viewHeight touchX1  touchY1      touchX2          touchX2               color                          clNormal                          clFocus            *pLangIndex      enable   curFocus  FocusEnable reserve
	
	{DLG_DESKBOX_ID, 10, 102, 220, 110, 0, 0, 0, 0,  TY_DIALOGBAX_BACKGROUND1_EN,TY_DIALOGBAX_BACKGROUND1_EN,0, 0, 1,0,0,0},//带提示对话框
	{DLG_DESKBOX_ID, 10, 102, 220, 110, 0, 0, 0, 0,  TY_DIALOGBAX_BACKGROUND2_EN,TY_DIALOGBAX_BACKGROUND2_EN,0, 0, 1,0,0,0},//动画框
	
	//取消按键	 2
	{BUTTON_ID_1, 45, 180, 151, 24, 45, 180, 196, 204, TY_NOSELECT_AFFIRM_LONG_EN,TY_SELECT_AFFIRM_LONG_EN,0, 0, 1,0,0,0},//单一按钮
  	
  	//标题 3
  	{LABEL_ID_1,  10, 107, 220, 16,   0,0,0,0,    0,0,0,0, 1,0,0,0},
  	//内容 4
  	{LABEL_ID_2, 30,133, 180, 32,   0,0,0,0,    0,0,0,0, 1,0,0,0},
#ifdef W818C
  	{BUTTON_ID_2,30, 180, 83, 24, 30, 180, 113, 204, TY_NOSELECT_AFFIRM_EN,TY_SELECT_AFFIRM_EN,0, 0, 1,0,0,0},
  	//取消 6
  	{BUTTON_ID_3,127, 180, 83, 24,127,180, 210, 204, TY_NOSELECT_CANCEL_EN,TY_SELECT_CANCEL_EN,0, 0, 1,0,0,0},
#else
  	//确定 5
  	{BUTTON_ID_2,127, 180, 83, 24,127,180, 210, 204, TY_NOSELECT_AFFIRM_EN,TY_SELECT_AFFIRM_EN,0, 0, 1,0,0,0},
  	//取消 6
  	{BUTTON_ID_3,30, 180, 83, 24, 30, 180, 113, 204, TY_NOSELECT_CANCEL_EN,TY_SELECT_CANCEL_EN,0, 0, 1,0,0,0},
#endif  	
  	//动画 7
  	{CARTOON_ID_1,102, 107, 36, 35, 0, 0, 0, 0, TY_SEARCH_AP1_EN,TY_SEARCH_AP1_EN,0, 0, 1,0,0,0},

  	//内容 8 显示动画时用
  	{LABEL_ID_2, 30,144, 180, 36,   0,0,0,0,    0,0,0,0, 1,0,0,0},
};


const tGrapButtonPriv gtComDlgBtn[]=
{
	{(U8 *)"确定",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_DIALOGBAX_BACKGROUND1_EN,NULL},
	{(U8 *)"取消",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_DIALOGBAX_BACKGROUND1_EN,NULL},
};


const tGrapLabelPriv gtComDlgLabPri[]=
{
	//0xBDF7	
	//提示 ---带按钮
	{(U8 *)"提示",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//警告
	{(U8 *)"警告",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//询问
	{(U8 *)"询问",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//软件版本
	{(U8 *)"软件版本",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//磁盘信息
	{(U8 *)"磁盘信息",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},	
   
    //校正
	{(U8 *)"校正",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},	
	
	//显示信息  6
    {NULL,COLOR_BLACK,COLOR_BLACK,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
};


const U32 gu32ComDlgCtn[]=
{
    TY_SEARCH_AP8_EN,TY_SEARCH_AP7_EN,TY_SEARCH_AP6_EN,TY_SEARCH_AP5_EN,
    TY_SEARCH_AP4_EN,TY_SEARCH_AP3_EN,TY_SEARCH_AP2_EN,TY_SEARCH_AP1_EN   
};


const tGrapCartoonPriv gtComDlgCtn[]=
{
	{0,1,25,0,8,0,(U32 *)&gu32ComDlgCtn,TY_DIALOGBAX_BACKGROUND2_EN},
};



int Com_GetPercentTotal()
{
	return gs32PercentTotal;
}

void Com_SetPercentTotal(int total)
{
	gs32PercentTotal = total;
}

int Com_GetPercentRecv()
{
	return gs32PercentRecv;
}

void Com_SetPercentRecv(int recvsize)
{
	gs32PercentRecv = recvsize;
}

/*---------------------------------------------------------
*函数: Com_DlgUpdataStat
*功能: 更新对话框状态
*参数: stat ： 对话框状态
*返回: none
*---------------------------------------------------------*/
void Com_DlgUpdataStat(eDLG_STAT stat)
{
    gu8DlgState = stat;     
}

U8 Com_DlgGetStat()
{
    return gu8DlgState;     
}

/*---------------------------------------------------------
*函数: Com_DlgStartTime
*功能: 对话框开始的时间
*参数: none
*返回: 开始时间
*---------------------------------------------------------*/
U32 Com_DlgStartTime(void)
{
    return gu32DlgStartTime;    
}

/*---------------------------------------------------------
*函数: Com_DlgTotalLiveTime
*功能: 对话框生存的时间
*参数: none
*返回: 生存时间
*---------------------------------------------------------*/
U32 Com_DlgTotalLiveTime(void)
{
    return gu32DlgTimeOutTotal;
}

/*---------------------------------------------------------
*函数: Com_DlgTimeOut
*功能: 对话框是否超时
*参数: none
*返回: 1: 超时 0：未超时
*---------------------------------------------------------*/
int Com_DlgTimeOut(void)
{
    int rc = FAIL;
    
    if (ABS(rawtime(NULL) - gu32DlgStartTime) >= gu32DlgTimeOutTotal) rc = SUCCESS;
    
    return rc;
}


/*---------------------------------------------------------
*函数: ComWidget_LabTimerISR
*功能: 点击信号图标时产生的中断事件   
*参数: pView ： lable point
*返回: none 
*---------------------------------------------------------*/ 
S32 ComWidget_LabTimerISR(void *pView)
{
    S32 rc;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
    if ( 0xff != kernal_app_get_stat())
    {         
        Com_DlgUpdataStat(DLG_CONFIRM_EN);
        *pDesk->pQuitCnt = 1;       
    }
    else
    {
        if ( SUCCESS == Com_DlgTimeOut())
        {            
            Com_DlgUpdataStat(DLG_TIMEROUT_EN);
            *pDesk->pQuitCnt = 1;
        }
    }
    
    pDesk->kDlgState = Com_DlgGetStat();
    
    rc = SUCCESS;    
}


/*---------------------------------------------------------
*函数: ComWidget_UpdataTimerMsg
*功能: 把消息队列中的timer消息去空后    
*参数: pDeskFather ：父指针
*返回: none 
*---------------------------------------------------------*/  
void ComWidget_UpdataTimerMsg(void *pDeskFather)
{
    U32 len,i,meslen[10],totallen=0;
    U32 num = 0;
    U32 sendMsgStatus;
    tGRAPDESKBOX *pDesk;
    char *pTempBuf = (char *)malloc(1000);
    
    if (NULL == pTempBuf) return;
    
    pDesk = (tGRAPDESKBOX *)(((tGRAPVIEW *)pDeskFather)->pParent);
    
    for (i=0; i<10; i++)
    {
    
        sendMsgStatus = msg_queue_tryget(pDesk->pGetMsg, (char *)pDesk->ptGetMsgBody, &len);
    
        if (sendMsgStatus != HY_OK)
        {
            break;
        }
        
        if (SYS_TIMER_EVENT  != pDesk->ptGetMsgBody->id)
        {
            memcpy(pTempBuf+totallen,(char *)pDesk->ptGetMsgBody,len); 
            
            meslen[num] = len;
            num++; 
            totallen += len;
        }
    
    }
    
    totallen = 0;
    for(i=0; i<num; i++)
    {
        msg_queue_tryput(pDesk->pGetMsg, pTempBuf+totallen, meslen[i], 6);
        totallen += meslen[i];  
        //i++;
    }
    
    Grap_UpdataLastTick();
    
    free(pTempBuf);
}

/*---------------------------------------------------------
*函数: Com_DlgButtonPress
*功能: btn按键按下后的操作
*参数: 
*返回: none
*---------------------------------------------------------*/
static S32 Com_DlgButtonPress(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CONFIRM_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*函数: Com_DlgButtonCancel
*功能: btn按键按下后的操作
*参数: 
*返回: none
*---------------------------------------------------------*/
static S32 Com_DlgButtonCancel(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CANCEL_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*函数: Com_DlgLabelDrawInfor
*功能: 显示提示标题
*参数: none
*返回: 
*---------------------------------------------------------*/
static S32 Com_DlgLabelDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL *pLabel;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapLabelPriv *plabel_privattr;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);	

	if (plabel_privattr->pStr)
	{
		disp_info.string = plabel_privattr->pStr;

		if(plabel_privattr->focus)
			disp_info.color = plabel_privattr->cl_focus;
		else
			disp_info.color = plabel_privattr->cl_normal;
		disp_info.font_size = plabel_privattr->font_size;
		disp_info.font_type = plabel_privattr->font_type;
		disp_info.flag = plabel_privattr->position;	
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	}

	return SUCCESS;
}

/*---------------------------------------------------------
*函数: Com_DlgLabelDrawInfor
*功能: 显示提示信息内容
*参数: none
*返回: 
*---------------------------------------------------------*/
static S32 Com_DlgLabelDrawInfor(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL *pLabel;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapLabelPriv *plabel_privattr;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);
	
	if(plabel_privattr->focus)
		disp_info.color = plabel_privattr->cl_focus;
	else
		disp_info.color = plabel_privattr->cl_normal;

	disp_info.font_size = plabel_privattr->font_size;
	disp_info.font_type = plabel_privattr->font_type;
	disp_info.flag = plabel_privattr->position;	
	
	disp_info.string = pLabel->label_privattr.pStr ;
	
	disp_info.keyStr = NULL;
	Grap_WriteStringMulti(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
					pview_attr->viewY1+48, &disp_info);
	
	return SUCCESS;
}

S32 ComWidget_LabTimerTimeOutISR(void *pView)
{
    S32 rc;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
   
    if ( SUCCESS == Com_DlgTimeOut())
    {            
        Com_DlgUpdataStat(DLG_TIMEROUT_EN);
        *pDesk->pQuitCnt = 1;   
    }
    
    pDesk->kDlgState = Com_DlgGetStat();
    
    rc = SUCCESS;    
}

static void Com_DlgLabelTimerISR(void *pView)
{
	tGRAPDESKBOX *pDesk;
	
	pDesk = (tGRAPDESKBOX *)((tGRAPLABEL *)pView)->pParent;
	pDesk->kDlgState = DLG_TIMEROUT_EN;
	*pDesk->pQuitCnt = 1;
	
	return;
}


/*---------------------------------------------------------
*函数: Com_DlgLabelCheckUSBTimerISR
*功能: pView： 检测是否已经断开USB
*参数: none
*返回: none
*---------------------------------------------------------*/
static void Com_DlgLabelCheckUSBTimerISR(void *pView)
{
	if (FALSE == hyudsDcd_Connected())
	{
	    *((tGRAPDESKBOX *)((tGRAPLABEL *)pView)->pParent)->pQuitCnt = 1;
	}

	return;
}

/*---------------------------------------------------------
*函数: Com_DlgSrcLabelHandle
*功能: 主要用于检测是否点击到锁图标位置
*参数: none
*返回: none
*---------------------------------------------------------*/
static S32 Com_DlgSrcLabelHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	int ret ;

	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_SETLABELFOCUS:
		plabel_privattr->focus = 1;
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_LOSELABELFOCUS:
		plabel_privattr->focus = 0;
		pLabel->draw(pLabel, 0, 0);
		break; 	
	case VIEW_TIMER:
		if (pLabel->label_timer.enable == 1)
		{
			if (plabel_timer->TimerLft == 0)
			{
				plabel_timer->TimerLft = plabel_timer->TimerTot;
				pLabel->labelTimerISR(pLabel);
			}
			else
			{
				S32 timeleft;
				
				timeleft =  plabel_timer->TimerLft-lParam;
				if (timeleft < 0) plabel_timer->TimerLft = 0;
				else plabel_timer->TimerLft = timeleft;
			}
		}
		break;		
	case VIEW_REV_SYSMSG:
		pLabel->recvSysMsg(pView);
		break;
	case VIEW_ANTICLICK:
	    break;	
	}

	return SUCCESS;
}

/*
*功能：处理对话框信息
*参数: pDeskFather:父窗体指针 DlgType:对话框类型 1提示2警告3询问 id:显示的字符的id 
*      DlgPressEnter :确定按钮的函数指针 DlgCancelEnter：取消按钮的函数指针
*      DispTime : 对话框显示的时间		 
*/
eDLG_STAT Com_SpcDlgDeskbox(U8 *str,U8 soundType,void *pDeskFather,U32 DlgType,PressEnter DlgPressEnter,PressEnter DlgCancelEnter,U32 DispTime)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPLABEL *pLable;
	U16 	lastValue;
	U16 index;

	index = 0;
  
    gu8DlgState = DLG_CANCEL_EN;  

	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtComDlgViewAttr[index+0], NULL);
	if (NULL == pDesk ) return DLG_CANCEL_EN;	
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	ComWidget_UpdataTimerMsg(pDesk);	
	
	switch(DlgType)
	{
	case 1://提示			

		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[0]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		break;
		
	case 2://警告
		//pDesk->pKeyCodeArr = keyComArray;
	    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[0]);
		if (NULL == pBtn ) return;
		
		if(DlgPressEnter != NULL)	pBtn->pressEnter = DlgPressEnter;
		else	pBtn->pressEnter = Com_DlgButtonPress;
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[1]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		
		break;
	case 3://3询问
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+5], (tGrapButtonPriv *)&gtComDlgBtn[0]);
		if (NULL == pBtn ) return;			
		if(DlgPressEnter != NULL)	pBtn->pressEnter = DlgPressEnter;
		else	pBtn->pressEnter = Com_DlgButtonPress;
		
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+6], (tGrapButtonPriv *)&gtComDlgBtn[1]);
		if (NULL == pBtn ) return;			
		if(DlgCancelEnter != NULL)	pBtn->pressEnter = DlgCancelEnter;
		else	pBtn->pressEnter = Com_DlgButtonCancel;			
			
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[2]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		break;
	default:
		break;		
	}
	
	Remind_Fun(soundType, 0);
		
	if (DLG_ALLWAYS_SHOW != DispTime)
	{
    	pLable->label_timer.enable = 1;
    	pLable->label_timer.TimerLft = DispTime;
    	pLable->label_timer.TimerTot = DispTime;
    	pLable->labelTimerISR = Com_DlgLabelTimerISR;
    }

	lastValue = BackLight_CloseScreenProtect();
	
    pDesk->kDlgState = gu8DlgState;
    
	Grap_DeskboxRun(pDesk); 
	gu8DlgState = pDesk->kDlgState;
	Grap_DeskboxDestroy(pDesk);
	
	BackLight_SetScreenProtect(lastValue);

	return gu8DlgState;
}

/*---------------------------------------------------------
*函数: Com_PercentLabTimerISR
*功能: 动画提示框中显示百分比  
*参数: pView ： lable point
*返回: none 
*---------------------------------------------------------*/ 
void Com_PercentLabTimerISR(void *pView)
{
	tGRAPLABEL 		*pLabel;
	tStrDispInfo 	disp_info;
	tGrapViewAttr  	*pview_attr;
	tGrapLabelPriv 	*plabel_privattr;
	tGRAPDESKBOX	*pDesk;
	int total,percent;
	U8  buf[6];
	U8  req ;
	
    pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);   
    pDesk = (tGRAPDESKBOX *)pLabel->pParent;    
        
    if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if( 0xff != kernal_app_get_stat() ) /* 该请求标志位已处理完毕被清除 */
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
    	 *pDesk->pQuitCnt = 1;
    }
    else
    {
		Grap_Show_Part_BackPicture(pLabel->pParent,pLabel);
		
		total   = Com_GetPercentTotal();
		
		memset(buf,0,6);
		if(total > 0)
		{
			percent = (Com_GetPercentRecv()*100)/total ;
			if(percent >= 0 && percent <= 100)
			{
				sprintf(buf,"%d%s",percent,"%");		
			}
		}
		else
		{
			strcpy(buf,"0%");
		}
		
		disp_info.string = buf;
		disp_info.color = 0;
		disp_info.font_size = plabel_privattr->font_size;;
		disp_info.font_type = plabel_privattr->font_type;;
		disp_info.flag = 2;	
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1+12, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1+12 + pview_attr->viewHeight), &disp_info);

		pLabel->draw(pLabel,0,0);		
    }	
}

/*
*功能：处理对话框信息
*参数: pDeskFather:父窗体指针 DlgType:对话框类型 1动画取消 id:显示的字符的id  
*      DlgPressEnter :确定按钮的函数指针 DlgCancelEnter：取消按钮的函数指针
*      DlgLabTimer: 传入的lableTimer判断函数指针 DispTime : lab中断的时间
*/
eDLG_STAT Com_CtnDlgDeskbox(void *pDeskFather,U32 DlgType,U8 *str,
							PressEnter DlgPressEnter,PressEnter DlgCancelEnter,
							Label_TimerISR DlgLabTimer,U32 DispTime , U32 TimeOut)
{
    
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPLABEL *pLable;
	tGRAPCARTOON *pCtn;
	U16 	lastValue;
	U16 index;

	index = 0;
        
    gu8DlgState = DLG_CANCEL_EN;
    
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtComDlgViewAttr[index+1], NULL);
	if (NULL == pDesk ) return;	
	Grap_Inherit_Public(pDeskFather,pDesk);
	//pDesk->pKeyCodeArr = keyComArray;
	
	ComWidget_UpdataTimerMsg(pDesk);
	
    switch(DlgType)
    {
    case 1:
        pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
    	if(NULL == pCtn) return;
    	
	    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[1]);
		if (NULL == pBtn ) return;
		if(DlgCancelEnter != NULL)	pBtn->pressEnter = DlgCancelEnter;
		else	pBtn->pressEnter = Com_DlgButtonCancel;

    	
    	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
    	if (NULL == pLable ) return;
    	pLable->draw = Com_DlgLabelDrawInfor;
    	pLable->label_privattr.pStr = str; 
    	
    	if (DLG_ALLWAYS_SHOW != DispTime)
    	{
    		pLable->label_timer.enable = 1;
    	}
    	
    	if (NULL != DlgLabTimer)
    	{
        	pLable->label_timer.TimerLft = 0;
        	pLable->label_timer.TimerTot = DispTime;
    	    pLable->labelTimerISR = DlgLabTimer;
    	}
    	else
    	{
    	    pLable->label_timer.TimerLft = TimeOut;
        	pLable->label_timer.TimerTot = TimeOut;
    	    pLable->labelTimerISR = Com_DlgLabelTimerISR;    
    	}    
    	break;
    case 2: //无按键
        pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
    	if(NULL == pCtn) return;   	
 	
    	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
    	if (NULL == pLable ) return;
    	pLable->draw = Com_DlgLabelDrawInfor;
    	pLable->label_privattr.pStr = str; 
    	if (DLG_ALLWAYS_SHOW != DispTime)
    	{
    		pLable->label_timer.enable = 1;
    	}
    	
    	if (NULL != DlgLabTimer)
    	{
        	pLable->label_timer.TimerLft = 0;
        	pLable->label_timer.TimerTot = DispTime;
    	    pLable->labelTimerISR = DlgLabTimer;
    	}
    	else
    	{
    	    pLable->label_timer.TimerLft = TimeOut;
        	pLable->label_timer.TimerTot = TimeOut;
    	    pLable->labelTimerISR = Com_DlgLabelTimerISR;    
    	}    
    	break;	
	}
	
    gu32DlgStartTime = rawtime(NULL);
    gu32DlgTimeOutTotal = TimeOut;

	lastValue = BackLight_CloseScreenProtect();
	
    pDesk->kDlgState = gu8DlgState;
	
	Grap_DeskboxRun(pDesk); 
	gu8DlgState = pDesk->kDlgState;
	Grap_DeskboxDestroy(pDesk); 
	
	BackLight_SetScreenProtect(lastValue);
	 
    return gu8DlgState;    
}


/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*---------------------------------------等待网络处理完毕---------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
static U8 gu8ShutDownDglWait ;


void Com_ShutDown_SetWait(U8 set)
{
	gu8ShutDownDglWait = set ;
}

/*---------------------------------------------------------
*函数: Com_ShutDownLabelTimerISR
*功能: 
*参数: none
*返回: 
*---------------------------------------------------------*/
static void Com_ShutDownLabelTimerISR(void *pView)
{
    U8  req ;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
  
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
        /* 因为该对话框的出现不走ComGlob */
    if(1 == gu8ShutDownDglWait || gsm_getmodestate() == 1) /* 标志被置或GSM异常退出 */
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
       
}


/*---------------------------------------------------------
*函数: Com_ShutDownLabelTimerISR
*功能: 
*参数: none
*返回: 
*---------------------------------------------------------*/
void Com_Usb_ShutDownLabelTimerISR(void *pView)
{
    U8  req ;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
  
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
        
    App_SetVbsInvalidTime();
    
    if(1 == gu8ShutDownDglWait)
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if(FALSE == hyudsDcd_Connected())
	{
		pDesk->kDlgState = DLG_CANCEL_EN;
        *pDesk->pQuitCnt = 1;
	}
    else if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
       
}

/*---------------------------------------------------------
*函数: Com_NetButtonCancel
*功能: btn按键按下后的操作
*参数: 
*返回: none
*---------------------------------------------------------*/
static S32 Com_ShutDownButtonCancel(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CANCEL_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*------------------yu---------------------------------------
* 函数: Com_ShutDownDlgDeskbox
* 功能: 对话框，关闭后台进程用 主要是强制关机和强制进入USB时候调用
* 参数: pDeskFather --- 父窗体
* 返回: 
------------------------------------------------------------*/
U8 Com_ShutDownDlgDeskbox(void *pDeskFather,U8 *str,Label_TimerISR DlgLabTimer,U32 timeOut)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON  *pBtn;
	tGRAPLABEL   *pLable;
	tGRAPCARTOON *pCtn;
	int          ret  ;
	U16          index = 0;
	
    /* 不带公共控件的 */
	pDesk = Grap_CreateDeskboxNoCom((tGrapViewAttr*)&gtComDlgViewAttr[index+1], NULL);
	if (NULL == pDesk ) return;
	
	pDesk->kDlgState = DLG_CONFIRM_EN;
	
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	ComWidget_UpdataTimerMsg(pDesk);
	
    pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
	if(NULL == pCtn) return;
	
    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[1]);
	if (NULL == pBtn ) return;
	pBtn->pressEnter = Com_ShutDownButtonCancel;

	
	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
	if (NULL == pLable ) return;
	pLable->draw = Com_DlgLabelDrawInfor;
	pLable->label_privattr.pStr = str;//"数据正在传输,完成本次传输巴枪将自动关机!"; 
	//pLable->label_privattr.pStr = "数据正在传输,完成本次传输巴枪";
	pLable->label_timer.enable = 1;
    
    pLable->label_timer.TimerLft = 0;
	pLable->label_timer.TimerTot = 20;
	if(NULL == DlgLabTimer)
	{
		pLable->labelTimerISR = Com_ShutDownLabelTimerISR ;
	}
	else
	{
    	pLable->labelTimerISR = DlgLabTimer;
    }

	
    gu32DlgStartTime = rawtime(NULL);
    gu32DlgTimeOutTotal = timeOut;
    
	gu8ShutDownDglWait = 0;
		
#if 0
	if(!(YDget_gprsevent()&YD_GPRS_FOREGROUND)) /* 没有前台事件,有就直接退出 */
	{
		YDclear_gprsevent(YD_GPRS_SENDREC); /* 上传标志清了,当上传进程暂时无事件的时候,
		                     对话框可能一直等待gu8ShutDownDglWait被置1 所以要唤醒上传进程*/
		YDupdata_Wakeup();
		Grap_DeskboxRun(pDesk);
	}
#endif
	ret = pDesk->kDlgState ;
	Grap_DeskboxDestroy(pDesk);
    
    return ret;    
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/*-------------------修改记录------------------------------------
090216  ： 修改id index不对的Bug
090309  :  修改对话框长度记录
---------------------------------------------------------------*/