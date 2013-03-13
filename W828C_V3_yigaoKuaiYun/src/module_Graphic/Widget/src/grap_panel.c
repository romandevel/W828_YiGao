#include "grap_panel.h"
#include "grap_event.h"
#include "grap_deskbox.h"
#include "grap_api.h"

//是否panel控件  1:是  0:否
int Panel_IfPanel(tGRAPVIEW *pView)
{
	if(pView->view_attr.id >= PANEL_ID_1 && pView->view_attr.id <= PANEL_ID_20)	
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int Panel_UpdataZorder(tPANEL *pPanel, tGRAPZORDER *pZorder)
{
	tGRAPZORDER *pTemp1, *pTemp2 ;
	
	pTemp1 = pPanel->pPanelZ;
	
	while(pTemp1)
	{
		if(pTemp1->zOrder == pZorder->zOrder)
			break ;
		pTemp2 = pTemp1 ;
		pTemp1 = pTemp1->pNext ;
	}
	if(pTemp1 && pTemp1->zOrder != (pPanel->pPanelZ)->zOrder)
	{
		pTemp2->pNext = pTemp1->pNext ;
		pTemp1->pNext = pPanel->pPanelZ ;
		pPanel->pPanelZ = pTemp1 ;
	}
	
	return SUCCESS;
}

static int Panel_DispenseMsg(tPANEL *pPanel,GRAP_EVENT *pEvent)
{
	int ret = SUCCESS;
	U32 message;
	U16 panelId;
	tGRAPVIEW		*pSubView, *pFocusView;
	tGRAPZORDER		*pSubZorder;
	
	pSubZorder = pPanel->pPanelZ;
	message = pEvent->message;
	
	pFocusView = (tGRAPVIEW *)Grap_GetCurFocus(pPanel, &panelId);
	switch(message)
	{
	case VIEW_REV_PULLMSG:
		if(pPanel->pPullChild == NULL)
		{//第一个点
			if(VIEW_PULL_END == pEvent->extParam) return SUCCESS;
			
			pEvent->extParam = VIEW_PULL_START;
			
			while(pSubZorder)
			{
				pSubView = pSubZorder->pMember;
				if(pSubView->view_attr.enable == 1 && VIEW_PULL_START == pSubView->handle(pSubView,pEvent))
				{
					ret = VIEW_PULL_START;
					pPanel->pPullChild  = pSubView;
					break;
				}
				pSubZorder = pSubZorder->pNext;
			}
		}
		else
		{//后续点
			pSubView = pPanel->pPullChild;
			ret = pSubView->handle(pSubView, pEvent);
			
			if(VIEW_PULL_END == ret)
            {
                pPanel->pPullChild = NULL;	        
            }
            else if(VIEW_PULL_ING == ret)
            {
            	
            }
            
            if(ret == PARSED )
			{
		        while(pSubZorder)
				{
					if(pSubView == pSubZorder->pMember)
					{
						break;
					}
					pSubZorder = pSubZorder->pNext;
				}
				Panel_UpdataZorder(pPanel, pSubZorder);
				pPanel->pPullChild = NULL;
			}
			else if(RETURN_FOCUSMSG == ret)
			{
				pPanel->pPullChild = NULL;
				return RETURN_FOCUSMSG;
			}
			else if(RETURN_QUIT == ret)
			{
				pPanel->pPullChild = NULL;
				return RETURN_QUIT;
			}
		}
		break;
	default:
		break;
	}

	if(message != VIEW_QUIT && message != VIEW_ESC && message != VIEW_REV_PULLMSG)
	{
		if(panelId == pPanel->view_attr.id)//当前面板上有焦点项
		{
			ret = pFocusView->handle(pFocusView, pEvent);
		}
		if(ret == PARSED)
		{
		}
		else if(RETURN_FOCUSMSG == ret)
		{
			return RETURN_FOCUSMSG;
		}
		else if(RETURN_QUIT == ret)
		{
			return RETURN_QUIT;
		}
		else//传递给其他控件
		{
			while(pSubZorder)
			{
				pSubView = pSubZorder->pMember;
				if(pSubView->view_attr.curFocus == 0 && (pSubView->view_attr.enable || message == VIEW_INIT))//初始化时全部发
				{
					ret = pSubView->handle(pSubView, pEvent);
					if(ret == PARSED )
					{
						Panel_UpdataZorder(pPanel, pSubZorder);
						break ;
					}
					else if(RETURN_FOCUSMSG == ret)
					{
						return RETURN_FOCUSMSG;
					}
					else if(RETURN_QUIT == ret)
					{
						return RETURN_QUIT;
					}
				}
				pSubZorder = pSubZorder->pNext;
			}
		}
	}
	
	return ret;
}

int Grap_PanelDraw(void *pView, U32 xy, U32 wh)
{
	tPANEL			*pPanel;
	tGrapViewAttr	*pview_attr;
	tPANELPRIV		*pPriv;
	GRAP_EVENT		tEvent;
	tGRAPVIEW		*pSubView;
	tGRAPZORDER		*pSubZorder, *pTemp = NULL;
	U16 x1,y1,x2,y2;
	
	pPanel   	= (tPANEL *)pView;
	pview_attr  = &(pPanel->view_attr);
	pPriv	 	= &(pPanel->PanelPriv);
	
	x1 = pview_attr->viewX1;
	y1 = pview_attr->viewY1;
	x2 = x1 + pview_attr->viewWidth;
	y2 = y1 + pview_attr->viewHeight;
	
	if(pview_attr->clFocus)
	{
		Grap_ShowPicture(pview_attr->clFocus, x1, y1);
	}
	else if(pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, x1, y1);
	}
	else if(pPriv->backgndPicId)
	{
		Grap_ClearScreen(x1, y1, x2, y2);
	}
	else
	{
		Grap_BrushScreen(x1, y1, x2, y2, pPriv->backgndColor);
	}

	//绘其他附属控件
	tEvent.message = VIEW_DRAW;
	//Panel_DispenseMsg(pPanel,&tEvent);

	Grap_ReversalZorderList(&(pPanel->pPanelZ));//反转链表
	pSubZorder = pPanel->pPanelZ;
	
	while (pSubZorder)
	{
		pTemp = pSubZorder->pNext ;
		pSubView = pSubZorder->pMember;
		if(pSubView->view_attr.enable)
		{
			pSubView->handle(pSubView, &tEvent);
		}
		pSubZorder = pTemp ;
	}
	Grap_ReversalZorderList(&(pPanel->pPanelZ));//反转链表
	
	
	return SUCCESS;
}

S32 Grap_Paneltimer(void *pView, GRAP_EVENT *pEvent)
{
	int ret;
	tPANEL		*pPanel;
	
	pPanel = (tPANEL *)pView;
	
	ret = Panel_DispenseMsg(pPanel,pEvent);
	
	return ret;
}

S32 Grap_Panelsysmsg(void *pView, GRAP_EVENT *pEvent)
{
	int ret;
	tPANEL		*pPanel;
	
	pPanel = (tPANEL *)pView;
	
	ret = Panel_DispenseMsg(pPanel,pEvent);
	
	return ret;
}

S32 Grap_Panelpullmsg(void *pView, GRAP_EVENT *pEvent)
{
	int ret;
	tPANEL		*pPanel;
	
	pPanel = (tPANEL *)pView;
	
	ret = Panel_DispenseMsg(pPanel,pEvent);
	
	return ret;
}

int Grap_PanelHandle(void *pView, GRAP_EVENT *pEvent)
{
	int ret = SUCCESS;
	U32 message, wParam, lParam;
	tPANEL			*pPanel;
	tGRAPVIEW		*pSubView;
	tGRAPZORDER		*pSubZorder;
	
	pPanel   = (tPANEL *)pView;
	pSubView = pPanel->PanelPriv.pSubView;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	switch(message)
	{
	case VIEW_INIT:
		pPanel->panelinit(pPanel);
		break;
	case VIEW_ENABLE:
		pPanel->view_attr.enable = 1;
		pPanel->draw(pPanel, 0, 0);
		break;
	case VIEW_DISABLE:
		pPanel->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
		if(pPanel->view_attr.enable)
		{
			pPanel->draw(pPanel, 0, 0);
		}
		break;
	case VIEW_TIMER:
		ret = pPanel->paneltimer(pPanel,pEvent);
		break;
	case VIEW_PRESS:
		ret = pPanel->panelpullmsg(pPanel,pEvent);
		break;
	case VIEW_REV_SYSMSG:
		ret = pPanel->panelsysmsg(pPanel,pEvent);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pPanel->panelpullmsg(pPanel,pEvent);
	    break;
	default:
		break;
	}
	
	return ret;
}

int Grap_PanelInit(void *pView)
{
	tPANEL			*pPanel;
	tGRAPVIEW		*pSubView;
	tGRAPZORDER		*pSubZorder, *pTempZorder;
	GRAP_EVENT		tEvent;
	U16				i = 0;
	
	pPanel   = (tPANEL *)pView;
	pSubView = pPanel->PanelPriv.pSubView;
	
	if(pPanel->pPanelZ != NULL)
	{//之前已经初始化过了
		return SUCCESS;
	}
	
	while(pSubView)
	{
		pTempZorder = (tGRAPZORDER *)malloc(sizeof(tGRAPZORDER)) ;
		if(!pTempZorder)
		{
			return FAIL;
		}
		
		pTempZorder->pMember = pSubView;
		pTempZorder->zOrder = i++;
		pTempZorder->pNext = NULL ;
		
		if(!(pPanel->pPanelZ))
		{
			pPanel->pPanelZ = pTempZorder ;
			pSubZorder = pPanel->pPanelZ ;
		}
		else
		{
			pSubZorder->pNext = pTempZorder ;
			pSubZorder = pTempZorder ;
		}

		pSubView = pSubView->pNext ;
	}
	
	//初始化panel上的控件
	tEvent.message = VIEW_INIT;
	Panel_DispenseMsg(pPanel,&tEvent);
	
	return SUCCESS;
}

int Grap_PanelDestroy(void *pView)
{
	tPANEL		*pPanel;
	tGRAPVIEW	*pSubView,*pTmp;
	
	pPanel = (tPANEL *)pView;
	
	pSubView = pPanel->PanelPriv.pSubView;
	while(pSubView)
	{
		pTmp = pSubView->pNext;
		pSubView->destroy(pSubView);
		pSubView = pTmp;
	}
	free(pPanel);
	
	return SUCCESS;
}

S32 Grap_PanelInsertWidget(void *pView, tGRAPVIEW *pWidget)
{
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub,*pTmp;
	tPANELPRIV	*pPriv;
	
	pPanel = (tPANEL *)pView;
	pPanel->pTail = pWidget;
	pPriv = &(pPanel->PanelPriv);
	
	if(pPriv->pSubView == NULL)
	{
		pPriv->pSubView = pWidget;
		((tGRAPVIEW *)pWidget)->pParent = pPanel->pParent;//依然指向大窗体
		pWidget->pPrev = pPanel;
		pWidget->pNext = NULL;
	}
	else
	{
		pSub = pPriv->pSubView;
		//pTmp = pSub;
		while(pSub->pNext)
		{
			pSub = pSub->pNext;
			//pTmp = pSub;
		}
		
		pSub->pNext = pWidget;
		((tGRAPVIEW *)pWidget)->pParent = pPanel->pParent;//依然指向大窗体
		pWidget->pPrev = pSub;
		pWidget->pNext = NULL;
	}
	
	return SUCCESS;
}

tPANEL *Grap_InsertPanel(void *pView, tGrapViewAttr  *pview_attr, tPANELPRIV *pPanel_priv)
{
	tGRAPDESKBOX *pParentView;
	tPANEL		*pPanel;
	
	pParentView = (tGRAPVIEW *)pView;
	if (NULL == pParentView) return NULL;
	pPanel = (tPANEL *)malloc(sizeof(tPANEL));
	if (NULL == pPanel)	return NULL;
	memset((char *)pPanel,0,sizeof(tPANEL));

	pPanel->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pPanel;
	pParentView->pTail = (tGRAPVIEW *)pPanel;
	pPanel->pParent = pParentView;
	
	//pPanel->pNext = pParent->pNext;
	//pParent->pNext  = (tGRAPVIEW *)pPanel;
	//pPanel->pParent = pParent;
	
	pPanel->view_attr = *pview_attr;
	pPanel->PanelPriv = *pPanel_priv;
		
	pPanel->handle			= Grap_PanelHandle;
	pPanel->draw			= Grap_PanelDraw;
	pPanel->destroy			= Grap_PanelDestroy;
	pPanel->panelinit		= Grap_PanelInit;
	pPanel->panelInsert		= Grap_PanelInsertWidget;
	pPanel->paneltimer		= Grap_Paneltimer;
	pPanel->panelsysmsg		= Grap_Panelsysmsg;
	pPanel->panelpullmsg	= Grap_Panelpullmsg;
	
	return pPanel;
}

tPANEL *Grap_CreatePanel(tGrapViewAttr *pview_attr, tPANELPRIV *pPanel_priv)
{
	tPANEL		*pPanel;
	
	pPanel = (tPANEL *)malloc(sizeof(tPANEL));
	if (NULL == pPanel)	return NULL;
	memset((char *)pPanel,0,sizeof(tPANEL));
	
	pPanel->view_attr = *pview_attr;
	pPanel->PanelPriv = *pPanel_priv;
		
	pPanel->handle			= Grap_PanelHandle;
	pPanel->draw			= Grap_PanelDraw;
	pPanel->destroy			= Grap_PanelDestroy;
	pPanel->panelinit		= Grap_PanelInit;
	pPanel->panelInsert		= Grap_PanelInsertWidget;
	pPanel->paneltimer		= Grap_Paneltimer;
	pPanel->panelsysmsg		= Grap_Panelsysmsg;
	pPanel->panelpullmsg	= Grap_Panelpullmsg;
	
	return pPanel;
}