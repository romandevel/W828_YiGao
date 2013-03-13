#include "grap_cartoon.h"
#include "grap_event.h"
#include "grap_deskbox.h"

S32 Grap_CartoonPress(void *pView, U16 state)
{
	
	return RETURN_CANCLE;
}

S32 Grap_CartoonRecvSysMsg(void *pView)
{
	
	return SUCCESS;
}

S32 Grap_CartoonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPCARTOON *pCartoon;
	tGrapCartoonPriv *pcartoon_privattr;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;
  
  	pCartoon = (tGRAPCARTOON *)pView;
	pcartoon_privattr = &(pCartoon->cartoon_privattr);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pCartoon))
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
		
		Grap_ChangeFocusEnable(pCartoon);
		pCartoon->draw(pCartoon, 0, 0);		
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	     if(GRAP_INWIDGET_RANGE(x,y,pCartoon))   ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pCartoon)) 
        {    
            return VIEW_PULL_END;
        }    
        
		ret = pCartoon->pressEnter(pView, STATE_NORMAL) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pCartoon, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pCartoon->pParent))->pQuitCnt = 1;
		}
		else if(ret == RETURN_FOCUSMSG)
		{
			return RETURN_FOCUSMSG;
		}

		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;    
}


S32 Grap_CartoonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPCARTOON *pCartoon;
	tGrapViewAttr *pview_attr;
	tGrapCartoonPriv *pcartoon_privattr;
	U8 str[MAX_CARTOON_STR_LEN];
	U16 viewX1,viewY1,viewX2,viewY2;

	
	pCartoon = (tGRAPCARTOON *)pView;
	pview_attr = &(pCartoon->view_attr);
	pcartoon_privattr = &(pCartoon->cartoon_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight;
	
	//draw back picture
	if (pcartoon_privattr->curFrame >= pcartoon_privattr->totFrame )	// loop
	{
		pcartoon_privattr->curFrame = 0;
	}
	
	if(pview_attr->FocusEnable)
	{
		if(pview_attr->curFocus == 0)
		{
			pcartoon_privattr->curFrame = 0;
			Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
		}
		else
		{
			Grap_ShowPicture(pcartoon_privattr->picArray[pcartoon_privattr->curFrame], viewX1, viewY1);
		}
	}
	else
	{
		if(pcartoon_privattr->focus == 0)
		{
			pcartoon_privattr->curFrame = 0;
			Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
		}
		else
		{
			Grap_ShowPicture(pcartoon_privattr->picArray[pcartoon_privattr->curFrame], viewX1, viewY1);
		}
	}

	pCartoon->getCartString(pView, pcartoon_privattr->curFrame, str);
	if (*str)
	{
		tStrDispInfo disp_info;
		disp_info.string = str;
		disp_info.color = 0x632C;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = 0;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	}
	
	//如果是当前焦点,则在外面画边框
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;
}

// Button 默认handle。
S32 Grap_CartoonHandle(void *pView, GRAP_EVENT *pEvent )
{
	U32 message, wParam, lParam ;
	tGRAPCARTOON *pCartoon;
	tGrapCartoonPriv *pcartoon_privattr;
	int ret = FAIL;
	
	pCartoon = (tGRAPCARTOON *)pView;
	pcartoon_privattr = &(pCartoon->cartoon_privattr);

	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_ENABLE:
		pCartoon->view_attr.enable = 1;
		pCartoon->draw(pCartoon, 0, 0);
		break;
	case VIEW_DISABLE:
		pCartoon->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pCartoon->draw(pCartoon, 0, 0);
		break;
	case VIEW_PRESS://当界面上有多个动画的时候这个函数需要传keycode值，在重载函数中需要再次判断
		if ((1 == pCartoon->view_attr.curFocus || pEvent->reserve == 1) && lParam == KEYEVENT_OK && cKEYB_EVENT_RELEASE == wParam)
		{
			ret = pCartoon->pressEnter(pView, lParam);
			if ( ret== RETURN_REDRAW)
			{
				ReDraw(pCartoon, 0, 0);
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pCartoon->pParent))->pQuitCnt = 1;
				return SUCCESS;
			}
			else if(ret == RETURN_FOCUSMSG)
			{
				return RETURN_FOCUSMSG;
			}
			else if (RETURN_CANCLE == ret)
			{
				break;
			}
			
			return PARSED;	
		}		
		break;
	case VIEW_TIMER:
		if(pCartoon->view_attr.FocusEnable == 1 && pCartoon->view_attr.curFocus == 0)
		{
			break ;
		}
		else if(pCartoon->view_attr.FocusEnable == 0 && pCartoon->cartoon_privattr.focus == 0)
		{
			break ;
		}

		if (pcartoon_privattr->lftTimeIntv == 0)
		{
			pcartoon_privattr->lftTimeIntv = pcartoon_privattr->timeIntv;	//reset timer
			pcartoon_privattr->curFrame++;
			pCartoon->draw(pCartoon, 0, 0);
			//如果是当前焦点,则在外面画边框
			Grap_DrawFocusRect(&(pCartoon->view_attr), COLOR_RED);
		}
		else
		{
			S32 timeleft;
			
			timeleft = pcartoon_privattr->lftTimeIntv - lParam;
			
			if (timeleft < 0) pcartoon_privattr->lftTimeIntv = 0;
			else  pcartoon_privattr->lftTimeIntv = timeleft;
		}
		break;
	case VIEW_CLICK:
		{
			int x, y ;
			tGRAPVIEW *pTempView;
			y = HIWORD(lParam) ;
			x = LOWORD(lParam) ;

			if(x >= pCartoon->view_attr.touchX1 && x < pCartoon->view_attr.touchX2 &&
			   y >= pCartoon->view_attr.touchY1 && y < pCartoon->view_attr.touchY2 )
			{
				if(pCartoon->view_attr.FocusEnable)
				{
					if(!pCartoon->view_attr.curFocus)
					{
						pTempView = pCartoon->pParent;
						while(pTempView)
						{
							if(pTempView->view_attr.curFocus)
							{
								pTempView->view_attr.curFocus = 0;
								if(pTempView->view_attr.enable)
									pTempView->draw(pTempView, 0, 0);
							}
							
							pTempView = pTempView->pNext ;
						}
						
						pCartoon->view_attr.curFocus = 1;
						pCartoon->draw(pCartoon, 0, 0);
					}
				}
				else
				{
					pcartoon_privattr->focus = !pcartoon_privattr->focus ;
					pCartoon->draw(pCartoon, 0, 0);
				}

				ret = pCartoon->pressEnter(pView, 0) ;
				if ( ret== RETURN_REDRAW)
				{
					ReDraw(pCartoon, 0, 0);
				}
				else if(ret == RETURN_QUIT)
				{
					*((tGRAPDESKBOX *)(pCartoon->pParent))->pQuitCnt = 1;
				}
				else if(ret == RETURN_FOCUSMSG)
				{
					return RETURN_FOCUSMSG;
				}

				return PARSED;
			}
		}
		break;
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pCartoon->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pCartoon->recvPullMsg(pCartoon,pEvent);
	    break;	
	
	}

	return ret;
}

// Cartoon 控件的销毁函数
S32 Grap_CartoonDestroy(void *pView)
{
	tGRAPCARTOON *pCartoon;
	pCartoon = (tGRAPCARTOON *)pView;

	free((void *)pCartoon);

	return SUCCESS;
}

// 得到第id桢要显示的字串，默认为空
S32 Grap_GetCartoonString(void *pView, U32 id, U8 *str)
{
	*str = 0;
	return SUCCESS;
}

/*S32 Grap_CartoonPopupMenu(void *pView)
{
	return SUCCESS;
}

S32 Grap_CartoonCopy(void *pView,tPOPUPINFO *pInfo)
{
	
	return SUCCESS;
}
S32 Grap_CartoonPaste(void *pView,tPOPUPINFO *pInfo)
{
	
	return SUCCESS;
}
S32 Grap_CartoonDelete(void *pView,tPOPUPINFO *pInfo)
{
	
	return SUCCESS;
}*/

tGRAPCARTOON *Grap_CreateCartoon(tGrapViewAttr *pview_attr, tGrapCartoonPriv *pcartoon_privattr)
{
	tGRAPCARTOON *pCartoon;

	pCartoon = (tGRAPCARTOON *)malloc(sizeof(tGRAPCARTOON));
	if (!pCartoon)
	{
		return NULL;
	}
	memset(pCartoon,0,sizeof(tGRAPCARTOON));
	pCartoon->view_attr = *pview_attr;
	pCartoon->cartoon_privattr = *pcartoon_privattr;
	
	pCartoon->draw = Grap_CartoonDraw;
	pCartoon->handle = Grap_CartoonHandle;
	pCartoon->destroy = Grap_CartoonDestroy;
	pCartoon->getCartString = Grap_GetCartoonString;
	pCartoon->pressEnter = Grap_CartoonPress ;
	pCartoon->recvSysMsg = Grap_CartoonRecvSysMsg;
	pCartoon->recvPullMsg = Grap_CartoonRecvPullMsg;
	//pCartoon->popupmenu = Grap_CartoonPopupMenu;
	//pCartoon->Copy			= Grap_CartoonCopy;
	//pCartoon->Paste			= Grap_CartoonPaste;
	//pCartoon->Delete		= Grap_CartoonDelete;
	
	return pCartoon;
}


tGRAPCARTOON *Grap_InsertCartoon(void *pView, tGrapViewAttr *pview_attr, tGrapCartoonPriv *pcartoon_privattr)
{
	tGRAPDESKBOX *pParentView;
	tGRAPCARTOON *pCartoon;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pCartoon = (tGRAPCARTOON *)malloc(sizeof(tGRAPCARTOON));
	if (!pCartoon)
	{
		return NULL;
	}
	memset(pCartoon,0,sizeof(tGRAPCARTOON));
	pCartoon->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pCartoon;
	pParentView->pTail = (tGRAPVIEW *)pCartoon;
	pCartoon->pParent = pParentView;
	
	//pCartoon->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pCartoon;
	//pCartoon->pParent = pParentView;
	
	pCartoon->view_attr = *pview_attr;
	pCartoon->cartoon_privattr = *pcartoon_privattr;
	
	pCartoon->draw = Grap_CartoonDraw;
	pCartoon->handle = Grap_CartoonHandle;
	pCartoon->destroy = Grap_CartoonDestroy;
	pCartoon->getCartString = Grap_GetCartoonString;
	pCartoon->pressEnter = Grap_CartoonPress ;
	pCartoon->recvSysMsg = Grap_CartoonRecvSysMsg;
	pCartoon->recvPullMsg = Grap_CartoonRecvPullMsg;
	//pCartoon->popupmenu = Grap_CartoonPopupMenu;
	//pCartoon->Copy			= Grap_CartoonCopy;
	//pCartoon->Paste			= Grap_CartoonPaste;
	//pCartoon->Delete		= Grap_CartoonDelete;
	
	return pCartoon;
}
