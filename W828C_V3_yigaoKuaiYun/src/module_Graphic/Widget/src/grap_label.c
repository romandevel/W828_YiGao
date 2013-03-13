#include "grap_label.h"
#include "grap_event.h"
#include "grap_deskbox.h"

void Grap_LabelTimerISR(void *pView)
{
	return;
}

S32 Grap_LabelEnter(void *pView, U16 viewX, U16 viewY)
{
	return SUCCESS;
}

S32 Grap_LabelRecvSysMsg(void *pView)
{
	return SUCCESS;
}

S32 Grap_LabelRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;
  
	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pLabel))
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
		
		Grap_ChangeFocusEnable(pLabel);
		pLabel->draw(pLabel, 0, 0);
		
		if(!pLabel->view_attr.FocusEnable) plabel_privattr->focus = !plabel_privattr->focus ; 
		
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	     if(GRAP_INWIDGET_RANGE(x,y,pLabel)) ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pLabel)) 
        {
            return VIEW_PULL_END;
        }    
        
        ret = pLabel->labelEnter(pLabel,x,y);
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pLabel, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pLabel->pParent))->pQuitCnt = 1;
		}
		
		pLabel->draw(pLabel, 0, 0);
		
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;
}

S32 Grap_GetNewString(void *pView)
{
	return SUCCESS;	
}

// Label 默认draw。
S32 Grap_LabelDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL *pLabel;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapLabelPriv *plabel_privattr;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);
	
	//draw back picture
	if(pview_attr->FocusEnable)//可以接收焦点
	{
		if (pLabel->view_attr.curFocus == 0 && pview_attr->clNormal)
		{
			Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
		}
		else if (pLabel->view_attr.curFocus == 1 && pview_attr->clFocus)
		{
			Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
		}
		else if(plabel_privattr->backgndPicId)
		{
			Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		}
	}
	else
	{
		if (plabel_privattr->focus == 0 && pview_attr->clNormal)
		{
			Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
		}
		else if (plabel_privattr->focus == 1 && pview_attr->clFocus)
		{
			Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
		}
		else if(plabel_privattr->backgndPicId)
		{
			Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		}
	}
	pLabel->getnewstring(pLabel);

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
	
	//如果是当前焦点,则在外面画边框
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;
}

// Label 默认handle。
//S32 Grap_LabelHandle(void *pView, U32 message, U32 wParam, U32 lParam)
S32 Grap_LabelHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	int ret = SUCCESS;

	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_ENABLE:
		pLabel->view_attr.enable = 1;
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_DISABLE:
		pLabel->view_attr.enable = 0;
		break;
	case VIEW_INIT:
		//pLabel->getnewstring(pLabel);
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_TIMER:
		if (pLabel->label_timer.enable == 1)
		{
			if (plabel_timer->TimerLft == 0)
			{
				plabel_timer->TimerLft = plabel_timer->TimerTot;
				pLabel->labelTimerISR(pLabel);
				//如果是当前焦点,则在外面画边框
				Grap_DrawFocusRect(&(pLabel->view_attr), COLOR_RED);
			}
			else
			{
				S32 timeleft;				
				timeleft =  plabel_timer->TimerLft-lParam;
				
				if (timeleft < 0) 
				{				    
				    pLabel->labelTimerISR(pLabel);
				    plabel_timer->TimerLft = plabel_timer->TimerTot;//plabel_timer->TimerTot;
				}
				else 
				{
				    plabel_timer->TimerLft = timeleft;
				}
			}
		}
		break;
	case VIEW_PRESS:
		if((pLabel->view_attr.curFocus == 1 || pEvent->reserve == 1) && lParam == KEYEVENT_OK && cKEYB_EVENT_RELEASE == wParam)//焦点时响应OK键
		{
			ret = pLabel->labelEnter(pLabel,0,0);
			if ( ret== RETURN_REDRAW)
			{
				ReDraw(pLabel, 0, 0);
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pLabel->pParent))->pQuitCnt = 1;
				pLabel->draw(pLabel, 0, 0);
			}
						
			return PARSED;
		}
		break;
	case VIEW_SETLABELFOCUS:
		plabel_privattr->focus = 1;
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_LOSELABELFOCUS:
		plabel_privattr->focus = 0;
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_LABELFOCUS:	// 支持触摸
		break;
		break;	
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pLabel->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pLabel->recvPullMsg(pLabel,pEvent);
	    break;	
	default:
		break;
	}

	return ret;
}

// Label 控件的销毁函数
S32 Grap_LabelDestroy(void *pView)
{
	
	free(pView);

	return SUCCESS;
}


tGRAPLABEL *Grap_CreateLabel(tGrapViewAttr *pview_attr, tGrapLabelPriv *plabel_privattr)
{
	int urlStrLen;
	tGRAPLABEL *pLabel;

	pLabel = (tGRAPLABEL *)malloc(sizeof(tGRAPLABEL));
	if (!pLabel)
	{
		return NULL;
	}
	memset(pLabel,0,sizeof(tGRAPLABEL));
	pLabel->view_attr = *pview_attr;
	pLabel->label_privattr = *plabel_privattr;
	
	pLabel->draw = Grap_LabelDraw;
	pLabel->handle = Grap_LabelHandle;
	//pLabel->updateString = Grap_LabelUpdateString;
	pLabel->destroy = Grap_LabelDestroy;
	pLabel->getnewstring = Grap_GetNewString;

	pLabel->labelTimerISR = Grap_LabelTimerISR;
	pLabel->labelEnter = Grap_LabelEnter;
	pLabel->recvSysMsg = Grap_LabelRecvSysMsg;
	pLabel->recvPullMsg = Grap_LabelRecvPullMsg;
	
	{ // 暂时固定值,可在应用层进行修改(default: 1s中断一次)
		pLabel->label_timer.enable = 0;
		pLabel->label_timer.TimerLft = 20;
		pLabel->label_timer.TimerTot = 20;
	}

	return pLabel;
}


tGRAPLABEL *Grap_InsertLabel(void *pView, tGrapViewAttr *pview_attr, tGrapLabelPriv *plabel_privattr)
{
	int urlStrLen;
	tGRAPDESKBOX *pParentView;
	tGRAPLABEL *pLabel;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pLabel = (tGRAPLABEL *)malloc(sizeof(tGRAPLABEL));
	if (!pLabel)
	{
		return NULL;
	}
	memset(pLabel,0,sizeof(tGRAPLABEL));
	pLabel->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pLabel;
	pParentView->pTail = (tGRAPVIEW *)pLabel;
	pLabel->pParent = pParentView;
	
	//pLabel->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pLabel;
	//pLabel->pParent = pParentView;
	
	pLabel->view_attr = *pview_attr;
	pLabel->label_privattr = *plabel_privattr;
	
	pLabel->draw		= Grap_LabelDraw;
	pLabel->handle		= Grap_LabelHandle;
	//pLabel->updateString 	= Grap_LabelUpdateString;
	pLabel->destroy		= Grap_LabelDestroy;
	pLabel->getnewstring	= Grap_GetNewString;
	pLabel->labelTimerISR	= Grap_LabelTimerISR;
	pLabel->labelEnter		= Grap_LabelEnter;
	pLabel->recvSysMsg		= Grap_LabelRecvSysMsg;
	pLabel->recvPullMsg	= Grap_LabelRecvPullMsg;

	
	{ // 暂时固定值,可在应用层进行修改(default: 1s中断一次)
		pLabel->label_timer.enable = 0;
		pLabel->label_timer.TimerLft = 20;
		pLabel->label_timer.TimerTot = 20;
	}

	return pLabel;
}
