#include "grap_button.h"
#include "grap_event.h"
#include "grap_deskbox.h"

// button 默认 press。为空。
S32 Grap_ButtonPress(void *pView, U16 state)
{
	if (state == STATE_PRESS)
	{
	}
	else if (state == STATE_RELEASE)
	{
	}
	return RETURN_CANCLE;
}
S32 Grap_ButtonRecvSysMsg(void *pView)
{

	return SUCCESS ;
}

S32 Grap_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
    tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
    
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pButton))
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
		Grap_ChangeFocusEnable(pButton);

	    pButton->state = STATE_PRESS;
		pButton->draw(pButton, 0, 0);   		
	        
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//在有效范围内
	         pButton->state = STATE_PRESS;
		     pButton->draw(pButton, 0, 0);
		     
		     ret = pButton->pressEnter(pView, pButton->state) ;
             if ( ret== RETURN_REDRAW)
             {
            	 ReDraw(pButton, 0, 0);
             }
             else if(ret == RETURN_QUIT)
             {
            	 *((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
             }
             else if (ret == RETURN_FOCUSMSG)
             {
            	 return ret;
             }			
		     ret = VIEW_PULL_ING; 
	     }
	     else
	     {
	        pButton->state = STATE_NORMAL;
		    pButton->draw(pButton,0,0);
	     }

	    
	}
	else if (VIEW_PULL_END == extParam)
	{//up
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0);				

        if(!GRAP_INWIDGET_RANGE(x,y,pButton))
        {
            return VIEW_PULL_END;
        }    
        
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}
		else if (ret == RETURN_FOCUSMSG)
		{
			return ret;
		}			
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0); 
		
		ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;  
 
}

S32 Grap_ButtonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
		
	if(pview_attr->FocusEnable && pview_attr->curFocus)
	{
		if (pButton->state != STATE_PRESS)
		{//画button的背景图，弹起
			if(pview_attr->clNormal)
			{
				Grap_ShowPicture(pview_attr->clNormal, viewX1, viewY1);
			}
			else
			{
				Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
			}
		}
		else if (pButton->state == STATE_PRESS)
		{//按下图片
			Grap_ShowPicture(pview_attr->clFocus, viewX1, viewY1);
		}
	}
	else
	{//
		if (pButton->state != STATE_PRESS)
		{//画button的背景图，弹起
			if(pview_attr->clNormal)
			{
				Grap_ShowPicture(pview_attr->clNormal, viewX1, viewY1);
			}
			else
			{
				Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
			}
		}
		else if (pButton->state == STATE_PRESS)
		{//按下图片
			Grap_ShowPicture(pview_attr->clFocus, viewX1, viewY1);
		}
	}
	
	if (pbtn_privattr->pBufPic)	//buffer形式的pic。
	{
		U16 *pDisplayBuf;
		int i,j,x1,y1,x2,y2;
		U16 offset1,offset2;
		pbtnpic = pbtn_privattr->pBufPic;
		
		drvLcd_Memory2DisplayMemory(pbtnpic->rgb,  pview_attr->viewHeight, 	pview_attr->viewWidth,	viewY1 + pbtnpic->off_y1, viewX1 + pbtnpic->off_x1 );
	}
	
	if (pbtn_privattr->pName)//显示button上的字符
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	}
	
	//如果是当前焦点,则在外面画边框
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;
}


S32 Grap_ButtonHandle(void *pView, GRAP_EVENT *pEvent )
{
	tGRAPBUTTON *pButton;
	U32 message, wParam, lParam ;
	int ret = SUCCESS;

	pButton = (tGRAPBUTTON *)pView;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_ENABLE:
		pButton->view_attr.enable = 1;
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_DISABLE:
		pButton->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_PRESS:
		if ((lParam == pButton->btn_privattr.keycode && cKEYB_EVENT_RELEASE == wParam) || //该控件绑定的按键
		     ((pButton->view_attr.curFocus == 1 || pEvent->reserve == 1) && cKEYB_EVENT_RELEASE == wParam))//焦点时响应OK键
		{
			if(lParam == KEYEVENT_UP)
			{
				Grap_ChangeFocus(pButton, 0);
				return PARSED;
			}
			else if(lParam == KEYEVENT_DOWN)
			{
				Grap_ChangeFocus(pButton, 1);
				return PARSED;
			}
			else if(lParam != pButton->btn_privattr.keycode && lParam != KEYEVENT_OK)
			{
				return SUCCESS;
			}
				
			pButton->state = STATE_PRESS;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			syssleep(15);
			pButton->state = STATE_NORMAL;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			ret = pButton->pressEnter(pView, pButton->state) ;
			if ( ret== RETURN_REDRAW)
			{
				ReDraw(pButton, 0, 0);
				return PARSED;
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
			}
			else if (ret == RETURN_FOCUSMSG)
			{
				return ret;
			}	
		}
		break;
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pButton->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pButton->recvPullMsg(pButton,pEvent);
	    break;	
	}
	return ret;
}

// Label 控件的销毁函数
S32 Grap_ButtonDestroy(void *pView)
{
	free(pView);
	return SUCCESS;
}


tGRAPBUTTON *Grap_CreateButton(tGrapViewAttr  *pview_attr, tGrapButtonPriv *pbtn_privattr)
{
	tGRAPVIEW *pParentView;
	tGRAPBUTTON *pButton;

	pButton = (tGRAPBUTTON *)malloc(sizeof(tGRAPBUTTON));
	if (!pButton)
	{
		return NULL;
	}
	memset(pButton,0,sizeof(tGRAPBUTTON));

	pButton->view_attr = *pview_attr;
	pButton->btn_privattr = *pbtn_privattr;
	
	pButton->draw = Grap_ButtonDraw;
	pButton->handle = Grap_ButtonHandle;
	pButton->destroy = Grap_ButtonDestroy;
	pButton->pressEnter = Grap_ButtonPress;
	pButton->recvSysMsg = Grap_ButtonRecvSysMsg ;
	pButton->recvPullMsg = Grap_ButtonRecvPullMsg;
	
	pButton->state = STATE_NORMAL;

	return pButton;
}

tGRAPBUTTON *Grap_InsertButton(void *pView, tGrapViewAttr  *pview_attr, tGrapButtonPriv *pbtn_privattr)
{
	tGRAPDESKBOX *pParentView;
	tGRAPBUTTON *pButton;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pButton = (tGRAPBUTTON *)malloc(sizeof(tGRAPBUTTON));
	if (!pButton)
	{
		return NULL;
	}
	memset(pButton,0,sizeof(tGRAPBUTTON));
	pButton->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pButton;
	pParentView->pTail = (tGRAPVIEW *)pButton;
	pButton->pParent = pParentView;
	
	//pButton->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pButton;
	//pButton->pParent = pParentView;

	pButton->view_attr = *pview_attr;
	pButton->btn_privattr = *pbtn_privattr;
	
	pButton->draw = Grap_ButtonDraw;
	pButton->handle = Grap_ButtonHandle;
	pButton->destroy = Grap_ButtonDestroy;
	pButton->pressEnter = Grap_ButtonPress;
	pButton->recvSysMsg = Grap_ButtonRecvSysMsg ;
	pButton->recvPullMsg = Grap_ButtonRecvPullMsg;
	
	pButton->state = STATE_NORMAL;

	return pButton;
}
