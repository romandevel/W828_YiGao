#include "grap_api.h"
#include "grap_base.h"
#include "grap_event.h"
#include "grap_deskbox.h"
#include "hycT9.h"
#include "edit.h"

/*======================================================================*/
//直接返回主界面
ReturnToSwitchMode Grap_ReturnToSwitchMode = NULL;//kong

static tGRAPDESKBOX *ptCurDesk = NULL;


/*--------------------------------------------------------*/
U8 Grap_GetCurDesk_ScanEnable()
{
	tGRAPVIEW	*pWidget;
	tGRAPEDIT	*pEdit;
	U8			ret = 0;
	
	//还需要判断当前焦点项是否可以扫描
	if(ptCurDesk->scanEnable)
	{
		pWidget = (tGRAPVIEW *)Grap_GetCurFocus(ptCurDesk,NULL);
	
		if(pWidget->view_attr.id >= ptCurDesk->editMinId && pWidget->view_attr.id <= ptCurDesk->editMaxId)	
		{
			pEdit = (tGRAPEDIT *)pWidget;
			if(pEdit->edit_privattr.scanEnable)
			{
				ret = 1;
			}
		}
	}
	
	return ret;
}

//获取当前窗体类型  1:普通窗体   2:对话框
U8 Grap_GetCurDeskType()
{
	return ptCurDesk->view_attr.id;
}


U8 Grap_GetCurDesk_InputMode()
{
	return ptCurDesk->inputMode;
}

U8 Grap_GetCurDesk_EditMinId()
{
	return ptCurDesk->editMinId;
}

U8 Grap_GetCurDesk_EditMaxId()
{
	return ptCurDesk->editMaxId;
}

void *Grap_Get_Current_Desk(void)
{
    return ptCurDesk;
}

void Grap_Set_Current_Desk(void *pDesk)
{
    if (NULL == pDesk) return;
    
    ptCurDesk = pDesk; 
}

//直接返回到主界面 只能退一层
S32 Grap_Return2Switch(tGRAPDESKBOX *pDesk,U8 Pid_Flag)
{
    S32 flag = 0;
	
	if(Pid_Flag == RETURN_TOSWITCHMODE )
	{
		Grap_ReturnToSwitchMode = NULL;
		
		while (pDesk->view_attr.id != MAIN_DESKBOX_ID)
		{
			 flag = 1;
			 
			 *pDesk->pQuitCnt = 1;
		     pDesk = pDesk->pFather;
		     
		     if (pDesk == NULL)
		     {
		         break;
		     }
		}
	}
	
	return flag;
}

void Grap_DrawFocusRect(tGrapViewAttr *pView, U16 color)
{
	if(pView->FocusEnable && pView->curFocus)
	{
		drvLcd_SetColor(color,0x0);
		drvLcd_Rectangle(pView->viewY1, pView->viewX1, pView->viewHeight, pView->viewWidth, 1);
	}
}

//pView 窗体  pView2需要调整的控件
S32 Grap_UpdateZorder_Widget(void *pView, void *pView2) 
{
	tGRAPDESKBOX *pDesk ;
	tGRAPVIEW	*pWidget;
	tGRAPZORDER *pTemp1, *pTemp2 ;
	
	pDesk = (tGRAPDESKBOX *)pView ;
	pTemp1 = pDesk->pZorderList ;
	pWidget = (tGRAPVIEW *)pView2;
	
	while(pTemp1)
	{
		if((U32)(pTemp1->pMember) == (U32)pWidget)
			break ;
		pTemp2 = pTemp1 ;
		pTemp1 = pTemp1->pNext ;
	}
	if(pTemp1 && pTemp1->zOrder != (pDesk->pZorderList)->zOrder)
	{
		pTemp2->pNext = pTemp1->pNext ;
		pTemp1->pNext = pDesk->pZorderList ;
		pDesk->pZorderList = pTemp1 ;
	}
	return SUCCESS ;
}

S32 Grap_UpdateZorder(void *pView, tGRAPZORDER *pZorder) 
{
	tGRAPDESKBOX *pDesk ;
	tGRAPZORDER *pTemp1, *pTemp2 ;
	
	pDesk = (tGRAPDESKBOX *)pView ;
	pTemp1 = pDesk->pZorderList ;
	
	while(pTemp1)
	{
		if(pTemp1->zOrder == pZorder->zOrder)
			break ;
		pTemp2 = pTemp1 ;
		pTemp1 = pTemp1->pNext ;
	}
	if(pTemp1 && pTemp1->zOrder != (pDesk->pZorderList)->zOrder)
	{
		pTemp2->pNext = pTemp1->pNext ;
		pTemp1->pNext = pDesk->pZorderList ;
		pDesk->pZorderList = pTemp1 ;
	}
	return SUCCESS ;
}

S32 Grap_ZorderListDestroy(void *pView)
{
	tGRAPDESKBOX *pDesk ;
	tGRAPZORDER *pTemp ;

	pDesk = (tGRAPDESKBOX *)pView ;

	while((pDesk->pZorderList))
	{
		pTemp = pDesk->pZorderList ;
		pDesk->pZorderList = (pDesk->pZorderList)->pNext ;
		pTemp->pNext = NULL ;
		
		free((void *)pTemp) ;
	}
	
	return SUCCESS ;
}

//2008-11-4 kong
void Grap_ReversalZorderList(tGRAPZORDER **pHead)
{
	tGRAPZORDER *pTemp1, *pTemp2, *pTail = NULL;
	
    if(*pHead == NULL || (*pHead)->pNext == NULL)//没有节点或只有一个节点
       return ;
	pTemp1 = *pHead;
	pTemp2 = *pHead;
	
	while(pTemp1 != NULL)
	{
		pTemp2 = pTemp1;
		pTemp1 = pTemp1->pNext;
		pTemp2->pNext = pTail;
		pTail = pTemp2;
	}
	
	*pHead = pTail;
	
	return ;
}


// desk 默认handle。
S32 Grap_DeskboxHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPDESKBOX *pDesk;
	tGRAPVIEW    *pSubView,*pTempView,*pFocusView;
	tGRAPZORDER  *pSubZorder ;
	tT9KEY		 *pT9Key;
	tGRAPLABEL   *pLable;
	tCTR_PANEL   *pPanel;
	int		t9_ret = 0;
	U8  researchflag=0;
	int searchFlag = 1,rv;
	
	pDesk = (tGRAPDESKBOX *)pView;
	pSubView = pDesk->pNext;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;   
	
	pFocusView = (tGRAPVIEW *)Grap_GetCurFocus(pDesk, NULL);
	switch(message)
	{
	case VIEW_DRAW:		  
		pDesk->draw(pDesk, 0, 0); 
		if ( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
		{
			pPanel = (tCTR_PANEL *)pDesk->pdata;

			if (NULL != pPanel && 1 == pPanel->alphaDisp)
				alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata);
			else
				DisplayData2Screen();
		}
		return SUCCESS;
	    break;
	case VIEW_QUIT:
	case VIEW_ESC:
		*pDesk->pQuitCnt = 0xff;
		break;
	case VIEW_TAB://切换焦点
		break;
	case VIEW_QUITALL:
		*pDesk->pQuitCnt = 0xff;
		break;
	case VIEW_REV_PULLMSG:
	 research:   
	    if (pDesk->pPullChild == NULL)
        {//第一点    
            	
	        pPanel = (tCTR_PANEL *)pDesk->pdata;
            
            if (VIEW_PULL_END == pEvent->extParam) return SUCCESS;
            
            pEvent->extParam = VIEW_PULL_START;       
            if(pDesk->pT9 != NULL)
		 	{
		 		pT9Key = (tT9KEY *)pDesk->pT9;
		 		if(1 == t9_ifrecvclick(pT9Key, pEvent))
	 			{
		 			pLable = (tGRAPLABEL *)(pT9Key->pWidget);
					if(pLable)
						t9_ret = pLable->handle(pLable,pEvent);
		 			break;
	 			}
		 	}
            
            //先找输入法控件，没有则找普通控件
            //if (1 == pDesk->alphaDisp && NULL != pDesk->pRect)
            if (researchflag == 0 && NULL != pPanel && 1 == pPanel->alphaDisp)
            {
                 U16 row,col;
                 tGRAP_RECT  *pRect;
                 tGrapViewAttr *pViewAttr;
                 
                 row = HIWORD(lParam);
                 col = LOWORD(lParam);
            	 
            	 pRect = &pPanel->tRect;
            	 
            	 //pRect = pDesk->pRect;
            	 if (row >= pRect->row && row <= pRect->row+pRect->hig)
            	 {
            	    searchFlag = 0;
                    for (pSubZorder = pDesk->pZorderList; pSubZorder; pSubZorder = pSubZorder->pNext)
                    {
                    	pSubView = pSubZorder->pMember;
                    	
                    	pViewAttr = &pSubView->view_attr;
                    	
                    	if (1 != pViewAttr->enable) continue;
                    	     //LABEL_ID_3_EDIT																 BUTTON_ID_ALL_EDIT 
                    	if (!(1000 == pViewAttr->id || (pViewAttr->id >= BUTTON_ID_1_EDIT && pViewAttr->id <= BUTTON_ID_ALL_EDIT)))
                    	    continue;
                    	
                    	if (VIEW_PULL_START == pSubView->handle(pSubView,pEvent))
                        {
                            pDesk->pPullChild = pSubView;
                            pDesk->lastPoint = lParam;
                            pDesk->pullTimeOut = rawtime(NULL);
                            
                            searchFlag = 0;
                            break;    
                        }
                    }
                    
            	 }
            }
            
            pSubZorder = pDesk->pZorderList ;
            if (1 == searchFlag)
			{
				researchflag = 0;
				while (pSubZorder)
				{
					pSubView = pSubZorder->pMember ;
					
					if (1000 == pSubView->view_attr.id || (pSubView->view_attr.id >= BUTTON_ID_1_EDIT && pSubView->view_attr.id <= BUTTON_ID_ALL_EDIT))
	                {
	                	pSubZorder = pSubZorder->pNext;
	                	continue;
	                }
					if (pSubView->view_attr.enable == 1 && VIEW_PULL_START == pSubView->handle(pSubView,pEvent))
	                {
	                    pDesk->pPullChild = pSubView;
	                    pDesk->lastPoint = lParam;
	                    pDesk->pullTimeOut = rawtime(NULL);
	                    break;    
	                }
	                
					pSubZorder = pSubZorder->pNext;
				}
			}
        }
        else
        {//后续点
            
            pEvent->wParam = pDesk->lastPoint;
            
            pSubView = pDesk->pPullChild;
            rv = pSubView->handle(pSubView,pEvent);
            	        
            if (VIEW_PULL_END == rv)
            {
                pDesk->pPullChild = NULL;	        
            }
            else if (VIEW_PULL_ING == rv)
            {
                pDesk->lastPoint = lParam;
            }
            
            //特殊返回值
            if(rv == PARSED )
    		{
    			tGRAPZORDER  *pSubZorder ;
    	        pSubZorder = pDesk->pZorderList ;
    	        while(pSubZorder)
				{
					if(pSubView == pSubZorder->pMember)
					{
						break;
					}
					pSubZorder = pSubZorder->pNext;
				}
    			Grap_UpdateZorder(((tGRAPVIEW *)pView)->pParent, pSubZorder);	
    			pDesk->pPullChild = NULL;			
    		}
    		else if(RETURN_FOCUSMSG == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_FOCUSMSG;
    		}
    		else if(RETURN_QUIT == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_QUIT;
    		}
        }	    
        pDesk->pullTimeOut = rawtime(NULL);
	    break;	
	 case VIEW_KEY_MSG:   // 专门由于按键编辑的处理
	 	{
            pSubZorder = pDesk->pZorderList ;
			
			while (pSubZorder)
			{
				pSubView = pSubZorder->pMember ;
				
				if (1000 == pSubView->view_attr.id || (pSubView->view_attr.id >= BUTTON_ID_1_EDIT && pSubView->view_attr.id <= BUTTON_ID_ALL_EDIT))
                {
                	pSubZorder = pSubZorder->pNext;
                	continue;
                }
				if (pSubView->view_attr.enable == 1 && pSubView->view_attr.FocusEnable == 1 && pSubView->view_attr.curFocus)
                {
                    pSubView->handle(pSubView,pEvent);
                    break;    
                }
                
				pSubZorder = pSubZorder->pNext;
			}
	 	}
	 	break;
	 case VIEW_TIMER:
	    if (pDesk->pPullChild != NULL)
	    {
	        if (ABS(rawtime(NULL) - pDesk->pullTimeOut) > 100)//60
	        {
	            GRAP_EVENT event;
	            
	            event = *pEvent;
	            	            
	            event.message = VIEW_REV_PULLMSG;
	            event.extParam = VIEW_PULL_TIMEOUT;
	            
	            pSubView = pDesk->pPullChild; 
	            pSubView->handle(pSubView,&event);
	            pDesk->pPullChild = NULL;      
	        }
	    }
	    break;   
	    
	case VIEW_LOCK_SCR://使用按键进行锁屏
	    //cls_desk_recv_lock_msg(pDesk);	    
	    break;
	case VIEW_UNLOCK_SCR://解锁
		//cls_desk_recv_unlock_msg(pDesk);
		break;
	case VIEW_POPDIALOGUE:
		Com_SpcDlgDeskbox("请长按‘OK’键解锁!",6,pDesk,1,NULL,NULL,200);
		pDesk->draw(pDesk, 0, 0); 
		if ( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
		{			
			pPanel = (tCTR_PANEL *)pDesk->pdata;

			if (NULL != pPanel && 1 == pPanel->alphaDisp)
				alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata);
			else
				DisplayData2Screen();
		}
		return SUCCESS;
		break;
	case VIEW_PRESS:
		if(pDesk->pT9 != NULL)
		{
			pT9Key = (tT9KEY *)pDesk->pT9;
			pLable = (tGRAPLABEL *)(pT9Key->pWidget);
			if(pLable)
				t9_ret = pLable->handle(pLable,pEvent);
		}
		if(KEYEVENT_SHARP == lParam && pDesk->inputMode > 0 && pDesk->inputMode <= 5)//有输入法时按#号键切换输入法
		{
			if(pDesk->inputMode == 4)
			{
				edit_change_input_mode(pDesk,0,0);
			}
			//切换输入法
			do
			{
				pDesk->inputMode++;
				if(pDesk->inputMode > 5)
				{
					pDesk->inputMode = 1;
				}
				
			}while(!((0x01<<(pDesk->inputMode-1))&pDesk->inputTypes));
			
			if(pDesk->inputMode == 4)
			{
				edit_change_input_mode(pDesk,0,1);
			}
			
			pDesk->draw(pDesk, 0, 0);
			if ( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
			{	
				pPanel = (tCTR_PANEL *)pDesk->pdata;

				if (NULL != pPanel && 1 == pPanel->alphaDisp)
					alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata);
				else
					DisplayData2Screen();
			}
			return SUCCESS;
		}
	#ifdef W818C
		if(KEYEVENT_F2 == lParam)//使用F2做tab键
	#else
		if(KEYEVENT_TAB == lParam)//tab键
	#endif
		{
			Grap_ChangeFocus(pFocusView, 1);//向下切换焦点
		}
		break;
	default:
		break;
	}
	
	if(t9_ret == 0 && message != VIEW_QUIT && message != VIEW_ESC && message != VIEW_REV_PULLMSG && message != VIEW_KEY_MSG)
	{
		tGRAPZORDER  *pSubZorder ;
		S32		ret = 0 ;
		
		//先把消息传给焦点控件
		if(pFocusView)
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
			pSubZorder = pDesk->pZorderList ;
			while (pSubZorder)	// deskbox负责各个子控件的handle的调用。
			{
				pSubView = pSubZorder->pMember ;
				
				if( pSubView->view_attr.id != 0xFFEE && 
				    pSubView->view_attr.curFocus == 0 && 
				    (pSubView->view_attr.enable || message == VIEW_INIT))
				{
					ret = pSubView->handle(pSubView, pEvent);
					if(ret == PARSED )
					{
						if(pSubView->view_attr.FocusEnable)
						{
							Grap_UpdateZorder(pDesk, pSubZorder);
						}
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
	if(t9_ret == 1 && pLable != NULL)
	{
		pLable->draw(pLable,0,0);
	}

	if ( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
	{	
		pPanel = (tCTR_PANEL *)pDesk->pdata;

		if (NULL != pPanel && 1 == pPanel->alphaDisp)
			alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata);
		else
			DisplayData2Screen();
	}

	return SUCCESS;
}


void Grap_ReDrawFatherDesk(tGRAPDESKBOX *pDesk)
{
	tGRAPDESKBOX *pFather;

	if (pDesk->view_attr.viewHeight >= gLcd_Total_Row || pDesk->view_attr.viewWidth >= gu16Lcd_ColumnLimit) return;

	//radraw father deskbox
	pFather = pDesk->pFather;

	while((pFather != NULL && pFather->view_attr.viewHeight != cLCD_TOTAL_ROW && pFather->view_attr.viewWidth != cLCD_TOTAL_ROW))
	{
		pFather = pFather->pFather;
	}

	//根节点不是窗体不能刷新
	if (0 == pFather->view_attr.viewHeight || 0 == pFather->view_attr.viewWidth )
	{
		return;
	}

	if (NULL != pFather)
	{
		pFather->draw(pFather,0,0);
		//有输入法的界面需要特殊处理
		if (1 == edit_get_input_status(pFather))
		{
			alpha_disp_all_to_dispmem();
		}
	}
}


void Grap_ReDrawCurDeskBox(void)
{
    tGRAPDESKBOX *pDesk;
    
    pDesk = (tGRAPDESKBOX *)Grap_Get_Current_Desk();
    
    ReDraw(pDesk,0,0);
}

// Desk 默认draw。
S32 Grap_DeskboxDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX		*pDesk;
	tGRAPVIEW			*pSubView;
	tGRAPZORDER			*pSubZorder, *pTemp = NULL;
	GRAP_EVENT			event;
	tGRAPVIEW			*pTempL = NULL ;
	U16	x, y, width, heigh,id;
	
	pDesk = (tGRAPDESKBOX *)pView;
	
	ptCurDesk = pDesk;
	
	if(xy == 0 && wh == 0)
	{// 重绘全部
		x = pDesk->view_attr.viewX1;
		y = pDesk->view_attr.viewY1;
		width = pDesk->view_attr.viewWidth ;
		heigh = pDesk->view_attr.viewHeight;
				
		if (pDesk->view_attr.clNormal && width != 0 && heigh != 0)
		{ 
			Grap_ShowPicture(pDesk->view_attr.clNormal, x, y);
		}
	}
	else
	{// 重绘小片区域
		x = (U16)HIWORD(xy) ;
		y = (U16)LOWORD(xy) ;
		width = HIWORD(wh) ;
		heigh = LOWORD(wh) ;

		if (pDesk->view_attr.clNormal && width != 0 && heigh != 0)
		{ 
			Grap_ClearScreen(x, y, x+width, y+heigh);
		}
	}

	event.message = VIEW_DRAW;
	
	Grap_ReversalZorderList(&(pDesk->pZorderList));//反转链表
	pSubZorder = pDesk->pZorderList;
	
	while (pSubZorder)
	{
		pTemp = pSubZorder->pNext ;
		pSubView = pSubZorder->pMember;
		id = pSubView->view_attr.id;
		if(0xFFEC == id || (id >= T9_LABLE_BASE && id <= T9_PY_LABLE20) ||
		  (id >= BUTTON_ID_1_EDIT && id <= BUTTON_ID_ALL_EDIT))
		{
			pTempL = pSubView ;
			pSubZorder = pTemp ;
			continue ;
		}
		if (pSubView->view_attr.enable && pSubView->view_attr.id != 0xFFEE)
		{
			pSubView->handle(pSubView, &event);	//draw all widget
		}
		pSubZorder = pTemp ;
	}
	
	if(pDesk->pdata != NULL && pDesk->inputMode == 4)
	{
		pSubZorder = pDesk->pZorderList;
		while (pSubZorder)
		{
			pTemp = pSubZorder->pNext ;
			pSubView = pSubZorder->pMember;
			id = pSubView->view_attr.id;
			if(pSubView->view_attr.enable && id >= BUTTON_ID_1_EDIT && id <= BUTTON_ID_ALL_EDIT)
			{
				pSubView->handle(pSubView, &event);
			}
			pSubZorder = pTemp ;
		}
	}
	
	if(pDesk->pT9 != NULL && pDesk->inputMode == 5)
	{
		tT9KEY		*pT9Key;
		tGRAPLABEL  *pLable;
		
		pT9Key = (tT9KEY *)pDesk->pT9;
		pLable = (tGRAPLABEL *)(pT9Key->pWidget);
		if(pLable)
			pLable->draw(pLable,0,0);
	}
	
	if(NULL != pTempL && pTempL->view_attr.enable)pTempL->handle(pTempL, &event);
	
	Grap_ReversalZorderList(&(pDesk->pZorderList));//反转链表
	
	return SUCCESS;
}

// 释放窗体中所有的资源。
// NOTE: 各个子控件在插入desk后，不用自己来处理自己的销毁，在最后窗体销毁时统一释放。
S32 Grap_DeskboxDestroy(void *pView)
{
	tGRAPDESKBOX *pDesk;
    tGRAPVIEW *pSubView, *pTmpView;
    pDesk = (tGRAPDESKBOX *)pView;
    pSubView = pDesk->pNext;
    Grap_ZorderListDestroy(pDesk) ;
    while (pSubView) // 统一释放各子控件的内存空间。
    {
        pTmpView = pSubView->pNext;
        pSubView->destroy(pSubView);
        pSubView = pTmpView;
    }
    if (NULL != pDesk->pdata)
    {
        free(pDesk->pdata);
        pDesk->pdata = NULL;
    }
    if (NULL != pDesk->pT9)
    {
        free(pDesk->pT9);
        pDesk->pT9 = NULL;
    }
    ptCurDesk = pDesk->pFather;
    free((void *)pDesk);
    return SUCCESS;
}

#if 0///////////////////////

void grap_zoom_out_graphic(U16 *pSrc, U16 *pDst, U16 srcW, U16 srcH, U16 dstW, U16 dstH)
{                                                                                           
	U32		r,g,b,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4;                                      
	U32		u,v,p1,p2,p3,p4,index;                                                          
	U32		xratio,yratio;                                                                  
	S32		xError,yError;                                                                  
	S32		xSrc,ySrc;                                                                      
	S32		i,j;                                                                            
	U16		x,y;                                                                            
	U32     temp;                                                                       
                                                                                            
	if(srcW == 0 || srcH == 0 || dstW == 0 || dstH == 0)                                    
	{                                                                                       
		return ;                                                                            
	}                                                                                       
                                                                                                                                  
                                                                                            
	xratio	= (srcW << 16) / dstW+1;                                                        
	yratio	= (srcH << 16) / dstH+1;                                                        
	                                                                                        
	xError	= (xratio >> 1) - (1 << 15);                                                    
	yError	= (yratio >> 1) - (1 << 15);                                                    
	                                                                                        
	ySrc	= yError;                                                                       
    for(y = 0; y < dstH; y++)                                                               
    {                                                                                       
		j	= ySrc >> 16;                                                                   
		if(j < 0) j = 0;                                                                    
		else if(j >= srcH) j = srcH - 1;                                                    
                                                                                            
        xSrc	= xError;                                                                   
        for(x = 0; x < dstW; x++)                                                           
        {                                                                                   
			i	= xSrc >> 16;                                                               
			if(i < 0) i = 0;                                                                
			else if(i >= srcW) i = srcW - 1;                                                
                                                                                            
			index = j*srcW+i; 
			temp = pSrc[index];                                                          
			r1 = (temp>>11)&0x1f;                                                               
			g1 = (temp>>5)&0x3f;
			b1 = temp&0x1f;
			                                                                                
			if(i+1 >= srcW)                                                                 
			{                                                                               
				r2 = r1;
				g2 = g1;
				b2 = b1;
			}                                                                               
			else                                                                            
			{                                                                               
				index++;
				temp = pSrc[index];
				r2 = (temp>>11)&0x1f;
				g2 = (temp>>5)&0x3f;
				b2 = temp&0x1f;
			}

			if(j+1 >= srcH)
			{
				r3 = r1;
				g3 = g1;
				b3 = b1;
				r4 = r2;
				g4 = g2;
				b4 = b2;
			}                                                                               
			else                                                                            
			{
				index = (j+1)*srcW+i;
				temp = pSrc[index];
				r3 = (temp>>11)&0x1f;
				g3 = (temp>>5)&0x3f;
				b3 = temp&0x1f;

				if(i+1 >= srcW)                                                             
				{                                                                           
					r4 = r3;                                                                
					g4 = g3;                                                                
					b4 = b3;                                                                
				}                                                                           
				else                                                                        
				{                                                                           
					index++;
					temp = pSrc[index]; 
					r4 = (temp>>11)&0x1f;                                                               
					g4 = (temp>>5)&0x3f;
					b4 = temp&0x1f;                                                   
				}                                                                           
			}                                                                               
                                                                                            
			u		= (xSrc & 0xFFFF) >> 8;                                                 
			v		= (ySrc & 0xFFFF) >> 8;                                                 
			p4		= (u*v);                                                                
			p3		= (u*(256-v));                                                          
			p2		= (v*(256-u));                                                          
			p1		= ((256-u)*(256-v));                                                    
                                                                                            
			r = p1*r1 + p2*r3 + p3*r2 + p4*r4;                                              
			g = p1*g1 + p2*g3 + p3*g2 + p4*g4;                                              
			b = p1*b1 + p2*b3 + p3*b2 + p4*b4;                                              
			                                                                                
			r >>= 16;                                                                       
			g >>= 16;                                                                       
			b >>= 16; 
		    xSrc += xratio;
		    
		    *pDst++ = (r<<11)+(g<<5)+b;
        }
        ySrc += yratio;
     }  
}	

void grap_cpy_to_region(U16 *pDes, U16 *pSrc, U32 posX1, U32 posY1, U32 posX2, U32 posY2, U32 heigh, U32 width)
{
	U32 i,j;

	for (i=posY1; i<posY2; i++)
	{
		for (j=posX1; j<posX2; j++)
		{
			pDes[width*i+j] = *pSrc++;
		}
	}
}

/*---------------------------------------------------------
*函数: grap_zoom_out_effect
*功能: 缩小显示
*参数: none
*返回: 
*---------------------------------------------------------*/
void grap_zoom_out_effect(U32 lev)
{
     U16 *pTmpBuf,*pSrcBuf;
     U32 width,heigh;
     U32 posX1,posY1,posX2,posY2;
     U32 baseW,baseH;
	 
	 pTmpBuf = (U16 *)Get_Uncompress_Buffer_Offset();
	 pSrcBuf = (U16 *)Get_Display_DisplayMemory_Offset();

	 heigh = cLCD_TOTAL_ROW;
	 width = cLCD_TOTAL_COLUMN;
	 
#if 0	 
	 posX1 = width>>2;
	 posY1 = heigh>>2;
	 posX2 = posX1*3;
	 posY2 = posY1*3;
#else
     if (1 == lev)
     {
         baseW = width*3/4; 
         baseH = heigh*3/4;
	 }
	 else if (2 == lev)
	 {
	 	 baseW = width>>1; 
         baseH = heigh>>1;
	 }
	 else if (3 == lev)
	 {
	     baseW = width>>2; 
         baseH = heigh>>2;
	 }
#endif	 
	 
	 posX1 = (width-baseW)/2;
	 posY1 = (heigh-baseH)/2;
	 posX2 = posX1+baseW;
	 posY2 = posY1+baseH;
	 
	 grap_zoom_out_graphic(pSrcBuf,pTmpBuf, width, heigh,baseW,baseH);
	 
	 //把屏幕清除成黑色
	 memset(pSrcBuf,0,(cLCD_TOTAL_COLUMN*cLCD_TOTAL_ROW<<1));
	 
	 grap_cpy_to_region(pSrcBuf, pTmpBuf, posX1, posY1, posX2, posY2, heigh, width);
	 
     drvLcd_BMPFromDisplayMemory2Screen();
     
     //PhoneTrace(0,"%d,%d,%d,%d",posX1,posY1,posX2,posY2);
	 //syssleep(600);
}

/*---------------------------------------------------------
*函数: grap_back_resume_display_memory
*功能: 备份或者恢复显存
*参数: none
*返回: 
*---------------------------------------------------------*/
void grap_back_resume_display_memory(U32 type)
{
    U16 *pTmpBuf,*pSrcBuf;
    
    pTmpBuf = (U16 *)Get_Display_DisplayBuffer_Offset();
	pSrcBuf = (U16 *)Get_Display_DisplayMemory_Offset();
    
    if (0 == type)
    {
        memcpy(pTmpBuf,pSrcBuf,(cLCD_TOTAL_ROW*cLCD_TOTAL_COLUMN<<1));
    }
    else
    {
        memcpy(pSrcBuf,pTmpBuf,(cLCD_TOTAL_ROW*cLCD_TOTAL_COLUMN<<1));
    }
}

/*---------------------------------------------------------
*函数: grap_reverse_srceen
*功能: 反转显示屏
*参数: none
*返回: 
*---------------------------------------------------------*/
void grap_reverse_srceen(tGRAPDESKBOX *pDesk)
{
     
	ConSume_ChangeCoefTemp(360,0);
	
	//处理原来屏的数据
	grap_zoom_out_effect(1);
	grap_zoom_out_effect(2);
	grap_zoom_out_effect(3);
    
    if(0 == LcdModule_Get_ShowType())
    {
    	drvLcd_DisplayMode_L();
    }
    else
    {
    	drvLcd_DisplayMode_R();
    }

	//清除
	drvLcd_ClearQueue();
	ReDraw(pDesk,0,0);

	//back dismemory
	grap_back_resume_display_memory(0);     
	grap_zoom_out_effect(3);	        
	grap_back_resume_display_memory(1);

	grap_back_resume_display_memory(0);
	grap_zoom_out_effect(2);            
	grap_back_resume_display_memory(1); 

	grap_back_resume_display_memory(0);
	grap_zoom_out_effect(1);            
	grap_back_resume_display_memory(1); 
}
#endif////////////////////////

tGRAPDESKBOX *Grap_CreateDeskbox(tGrapViewAttr *pview_attr, U8 *pDisplayName)
{
	tGRAPDESKBOX *pDesk;

	pDesk = (tGRAPDESKBOX *)malloc(sizeof(tGRAPDESKBOX));
	if (!pDesk)
	{
		return NULL;
	}
	memset(pDesk,0,sizeof(tGRAPDESKBOX));
	pDesk->pNext   = NULL;
	pDesk->pPrev   = NULL;
	pDesk->pParent = (tGRAPVIEW *)pDesk;
	pDesk->pTail   = (tGRAPVIEW *)pDesk;
	
	pDesk->view_attr = *pview_attr;
	pDesk->pDisName  = pDisplayName;//窗体显示名称
	
	pDesk->draw   = Grap_DeskboxDraw;
	pDesk->handle = Grap_DeskboxHandle;
	pDesk->destroy= Grap_DeskboxDestroy;
	pDesk->inputTypes= 0;//默认不带输入法
	pDesk->inputMode = 0;
	pDesk->scanEnable= 0;   //默认不可扫描
	pDesk->editMinId = 0;
	pDesk->editMaxId = 0;
	
	pDesk->pZorderList = NULL ;
	
	common_insert_glob_lab(pDesk);	
		
	drvLcd_ClearQueue();
	return pDesk;
}

//不插入后台公共检测控件
tGRAPDESKBOX *Grap_CreateDeskboxNoCom(tGrapViewAttr *pview_attr, U8 *pDisplayName)
{
	tGRAPDESKBOX *pDesk;

	pDesk = (tGRAPDESKBOX *)malloc(sizeof(tGRAPDESKBOX));
	if (!pDesk)
	{
		return NULL;
	}
	memset(pDesk,0,sizeof(tGRAPDESKBOX));
	pDesk->pNext   = NULL;
	pDesk->pPrev   = NULL;
	pDesk->pParent = (tGRAPVIEW *)pDesk;
	pDesk->pTail   = (tGRAPVIEW *)pDesk;
	
	pDesk->view_attr = *pview_attr;
	pDesk->pDisName  = pDisplayName;
	
	pDesk->draw   = Grap_DeskboxDraw;
	pDesk->handle = Grap_DeskboxHandle;
	pDesk->destroy= Grap_DeskboxDestroy;
	pDesk->inputTypes= 0;//默认不带输入法
	pDesk->inputMode = 0;
	pDesk->scanEnable= 0;   //默认不可扫描
	pDesk->editMinId = 0;
	pDesk->editMaxId = 0;
	
	pDesk->pZorderList = NULL ;
	
	drvLcd_ClearQueue();
	return pDesk;
}

//继承父窗体的msg消息
void Grap_Inherit_Public(tGRAPDESKBOX *pDeskFather, tGRAPDESKBOX *pDesk)
{
	pDesk->pGetMsg = pDeskFather->pGetMsg;
	pDesk->pPutMsg = pDeskFather->pPutMsg;
	pDesk->pUIFocus= pDeskFather->pUIFocus;
	pDesk->pQuitCnt= pDeskFather->pQuitCnt;
	pDesk->ptGetMsgBody  = pDeskFather->ptGetMsgBody;
	pDesk->ptPutMsgParam = pDeskFather->ptPutMsgParam;
	pDesk->threadCallback = pDeskFather->threadCallback;
	pDesk->pFather = pDeskFather;
}

// 进入窗体消息循环。
S32 Grap_DeskboxRun(tGRAPDESKBOX *pDesk)
{
	S32		ret = -1;
	U16     i = 0 ;
	U16     id;
	U32     len;
	U32     game_run_flag = 0;
	GRAP_EVENT	event;
	tGRAPVIEW	*pSubView ;
	tGRAPZORDER		*pSubZorder, *pTempZorder;
	tMSG_PUT_PARAM *ptPutMsgPara; 
	
	//进入的时候已经不能做显示的则直接退出,不再进行刷屏,不然会出现问题
	if (*pDesk->pQuitCnt > 0)
	{
		(*pDesk->pQuitCnt)--;
	    return ret;
	}
	
	ptCurDesk = pDesk;

	ptPutMsgPara =  pDesk->ptPutMsgParam;	

	pSubView = pDesk->pNext ;
	while(pSubView)
	{
		pTempZorder = (tGRAPZORDER *)malloc(sizeof(tGRAPZORDER)) ;
		if(!pTempZorder)
		{
			return 0;
		}
		pTempZorder->pMember = pSubView ;
		pTempZorder->zOrder = i++;
		pTempZorder->pNext = NULL ;
		
		if(!(pDesk->pZorderList))
		{
			pDesk->pZorderList = pTempZorder ;
			pSubZorder = pDesk->pZorderList ;
		}
		else
		{
			pSubZorder->pNext = pTempZorder ;
			pSubZorder = pSubZorder->pNext ;
		}

		pSubView = pSubView->pNext ;
	}

	event.id = 0 ;
	event.message = VIEW_INIT ;
	event.wParam = 0 ;
	event.lParam = 0 ;
	Grap_SendMessage(pDesk, &event);
	
	if(TRUE == *pDesk->pUIFocus)
	{
    	event.message = VIEW_DRAW;
    	Grap_SendMessage(pDesk, &event);	//draw first
	}

	while (1)
	{
		event.message = 0;

		ret = msg_queue_get(pDesk->pGetMsg,(char *)pDesk->ptGetMsgBody,&len);
		
		if (ret < MSG_QUEUE_OK)	continue;
		
		//进程回调函数
		if (NULL != pDesk->threadCallback)
		{
		     pDesk->threadCallback(pDesk);
		}
		
		id = pDesk->ptGetMsgBody->id;
		
		if(TRUE == *pDesk->pUIFocus)
		{//处理界面程序

			ptCurDesk = pDesk;
			
			Grap_DealEvent(pDesk,&event,pDesk->ptGetMsgBody);
			
			ret = pDesk->handle(pDesk, &event);
			
			if(RETURN_FOCUSMSG == ret)
			{			                                  
	            if (USE_LOST_FOCUS_EVENT == pDesk->ptPutMsgParam->body.id)
	            {//如果只是丢失焦点消息,那么只把消息发送出,让进程阻塞
	            	msg_queue_put(*pDesk->pPutMsg,(char *)&pDesk->ptPutMsgParam->body,pDesk->ptPutMsgParam->length,pDesk->ptPutMsgParam->priority);
	           		pDesk->ptPutMsgParam->body.id = INVALID_EVENT;//把消息体类型去掉
	            }
	            *pDesk->pUIFocus = FALSE;                     
			}
			
			//一般的应用可以使用这个,但是像mp3,mp4,camera等必须要求停止后台服务进程
			//故这个run函数必须要重写，但是必须要求能够正常接受 SYS_REQ_QUIT_EVENT 消息
			if (SYS_REQ_QUIT_EVENT == id)
			{
			    *pDesk->pPutMsg = ptMsg_controlDesk;
					
			    ptPutMsgPara->body.id = USE_ACK_QUIT_EVENT;				    			
				ptPutMsgPara->body.msg.MsgQueAdd = (U32)pDesk->pGetMsg;			    			
				ptPutMsgPara->priority = 10;
				ptPutMsgPara->length =  sizeof(U32)+sizeof(eMSG_ID);					
				*pDesk->pQuitCnt = 0xff;
				*pDesk->pUIFocus = FALSE;
				break;
			}
			else if (SYS_DEPRIVE_FOCUS_EVENT == id)
			{
			   *pDesk->pUIFocus = FALSE;				   
			   
			   *pDesk->pPutMsg = ptMsg_controlDesk;						
			   ptPutMsgPara->body.id = USE_DEPRIVED_FOCUS_EVENT;
			   ptPutMsgPara->body.msg.MsgQueAdd = (U32)pDesk->pGetMsg; 		
			   ptPutMsgPara->priority = 10;
			   ptPutMsgPara->length =  sizeof(eMSG_ID)+sizeof(U32);	
			   msg_queue_put(*pDesk->pPutMsg,(char *)&pDesk->ptPutMsgParam->body,pDesk->ptPutMsgParam->length,pDesk->ptPutMsgParam->priority);
			   *pDesk->pPutMsg = NULL;
			   //continue;			   
			}
			else if (SYS_REQ_USE_KILL_SELF == id)//控制台要求进程自杀
			{
			    *pDesk->pPutMsg = ptMsg_controlDesk;
					
			    ptPutMsgPara->body.id = USE_ACK_KILL_SELF;	
			    ptPutMsgPara->body.msg.Pid.MyPid = AppCtrl_GetPidFromSystem();					    			
				ptPutMsgPara->priority = 10;
				ptPutMsgPara->length =  sizeof(tPIDINFOR)+sizeof(eMSG_ID);					
				*pDesk->pQuitCnt = 0xff;
				*pDesk->pUIFocus = FALSE;
			} 
			
		}
		else
		{ 		
			if (SYS_FOCUS_EVENT == id)
			{
				*pDesk->pUIFocus = TRUE;
				if(Grap_ReturnToSwitchMode)
				{//kong 直接返回主界面
					if(1==Grap_ReturnToSwitchMode(pDesk,pDesk->ptGetMsgBody->msg.Pid.Flag))
					{
						pDesk->ptGetMsgBody->msg.Pid.Flag = 0;
						continue ;
					}
				}				
								
				Grap_DealEvent(pDesk,&event,pDesk->ptGetMsgBody);//当前是focus消息时需要另外出来
				pDesk->handle(pDesk, &event);
				
				//081028 先收消息再显示				
				//event.message = VIEW_DRAW;
            	//pDesk->handle(pDesk, &event);	//draw all widget
            	//修改为使用redraw方式
            	ReDraw(pDesk,0,0);	//090413
				
			}
			else if (SYS_REQ_QUIT_EVENT == id)
			{   //一般的应用可以使用这个,但是像mp3,mp4,camera等必须要求停止后台服务进程
			    //故这个run函数必须要重写，但是必须要求能够正常接受 SYS_REQ_QUIT_EVENT 消息 				
			    *pDesk->pPutMsg = ptMsg_controlDesk;
					
			    ptPutMsgPara->body.id = USE_ACK_QUIT_EVENT;	
			    ptPutMsgPara->body.msg.MsgQueAdd = (U32)pDesk->pGetMsg;			    			
				ptPutMsgPara->priority = 10;
				ptPutMsgPara->length =  sizeof(U32)+sizeof(eMSG_ID);
				
				*pDesk->pQuitCnt = 0xff;
				break;
			}
			else if (SYS_DEPRIVE_FOCUS_EVENT == id)
			{
			   *pDesk->pUIFocus = FALSE;				   
			   
			   *pDesk->pPutMsg = ptMsg_controlDesk;						
			   ptPutMsgPara->body.id = USE_DEPRIVED_FOCUS_EVENT;
			   ptPutMsgPara->body.msg.MsgQueAdd = (U32)pDesk->pGetMsg; 		
			   ptPutMsgPara->priority = 10;
			   ptPutMsgPara->length =  sizeof(eMSG_ID)+sizeof(U32);	
			   msg_queue_put(*pDesk->pPutMsg,(char *)&pDesk->ptPutMsgParam->body,pDesk->ptPutMsgParam->length,pDesk->ptPutMsgParam->priority);
			   *pDesk->pPutMsg = NULL;
			   continue; 
			}
			else if (SYS_REQ_USE_KILL_SELF == id)//控制台要求进程自杀
			{
			    *pDesk->pPutMsg = ptMsg_controlDesk;
					
			    ptPutMsgPara->body.id = USE_ACK_KILL_SELF;	
			    ptPutMsgPara->body.msg.Pid.MyPid = AppCtrl_GetPidFromSystem();					    			
				ptPutMsgPara->priority = 10;
				ptPutMsgPara->length =  sizeof(tPIDINFOR)+sizeof(eMSG_ID);					
				*pDesk->pQuitCnt = 0xff;
				*pDesk->pUIFocus = FALSE;
			}
		}
		
		if (*pDesk->pQuitCnt > 0)
		{
			(*pDesk->pQuitCnt)--;
			break;
		}		
	}
	
	return ret;
}
