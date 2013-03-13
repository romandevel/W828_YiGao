#include "grap_view.h"
#include "grap_api.h"
#include "grap_base.h"
#include "grap_event.h"
#include "grap_deskbox.h"

/*===============================================================*/
/*----------------------------------------------
*函数: Grap_GetViewByID
*功能: 获取当前公共空间中指定ID的控件
*参数: view 公共控件指针
*返回: id: 要找的控件id
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByIDFromComm(void *view, U16 id)
{
	tGRAPVIEW	*pView = NULL;
	
	pView = (tGRAPVIEW *)view;
	for ( ; pView != NULL; pView = pView->pNext)
	{
		if (pView->view_attr.id == id)
		{
			break;
		}
	}

	return pView;
}

/*----------------------------------------------
*函数: Grap_GetViewByID
*功能: 获取当前窗体指定ID的控件
*参数: view 控件指针   id: 要找的控件id
*返回: 
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByID(void *view, U16 id)
{
	tGRAPVIEW	*pView = NULL;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	pView = ((tGRAPVIEW *)view)->pParent;
	for ( ; pView != NULL; pView = pView->pNext)
	{
		if (pView->view_attr.id == id)
		{
			break;
		}
		else if(Panel_IfPanel(pView))//panel控件标识
		{
			pPanel = (tPANEL *)pView;
			pPriv = &(pPanel->PanelPriv);
			pSub = pPriv->pSubView;
			while(pSub)
			{
				if(pSub->view_attr.id == id)
				{
					return pSub;
				}
				pSub = pSub->pNext;
			}
		}
	}

	return pView;
}

/*----------------------------------------------
*函数: Grap_GetPanelViewByID
*功能: 从panel上得到指定id的控件指针
*参数: panelid: panel控件的id号   widgetid: 要查找的控件的id
*返回: 
*----------------------------------------------*/
tGRAPVIEW *Grap_GetPanelViewByID(void *view, U16 panelid, U16 widgetid)
{
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	//先找到panel
	pPanel = (tPANEL *)Grap_GetViewByID(view, panelid);
	if(!pPanel)
	{
		return NULL;
	}
	
	//在该panel上找指定控件
	pPriv = &(pPanel->PanelPriv);
	pSub = pPriv->pSubView;
	
	while(pSub)
	{
		if(pSub->view_attr.id == widgetid)
		{
			break;
		}
		pSub = pSub->pNext;
	}

	return pSub;
}

/*----------------------------------------------
*函数: Grap_GetCurFocus
*功能: 获取当前窗体焦点控件,如果有panel则继续查找panel中的控件
*参数: pView 控件指针 pPanelid:如果焦点控件在panel上,则返回panel的id   不需要该参数时传NULL
*返回: pFocus: 焦点项控件指针,如果为找到焦点项则返回NULL
*----------------------------------------------*/
tGRAPVIEW *Grap_GetCurFocus(void *pView, U16 *pPanelid)
{
	tGRAPVIEW	*pFocus;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	if(pPanelid != NULL)
	{
		*pPanelid = 0;
	}
	pFocus = ((tGRAPVIEW *)pView)->pParent;
	
	while(pFocus)
	{
		if(pFocus->view_attr.enable && Panel_IfPanel(pFocus))//panel控件标识
		{
			pPanel = (tPANEL *)pFocus;
			pPriv = &(pPanel->PanelPriv);
			pSub = pPriv->pSubView;
			while(pSub)
			{
				if(pSub->view_attr.curFocus == 1)
				{
					if(pPanelid != NULL)
					{
						*pPanelid = pFocus->view_attr.id;
					}
					return pSub;
				}
				pSub = pSub->pNext;
			}
		}
		else
		{
			if(pFocus->view_attr.curFocus == 1)
			{
				break;
			}
		}
		
		pFocus = pFocus->pNext;
	}
	
	return pFocus;
}

/*----------------------------------------------
*函数: Grap_ChangeInputMode
*功能: 切换输入法模式
*参数: pEdit:控件指针
*返回: none
*----------------------------------------------*/
void Grap_ChangeInputMode(tGRAPEDIT *pEdit)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT2		*pEdit2;
	U8	inputMode;
	U16	id;
	
	pDesk = (tGRAPDESKBOX *)(pEdit->pParent);
	inputMode = pDesk->inputMode;
	id = pEdit->view_attr.id;
hyUsbPrintf("inputMode  =  %d  input = %d reserve = 0x%x \r\n",inputMode, pEdit->edit_privattr.input,pEdit->view_attr.reserve);	
	if(inputMode > 0 && inputMode <= 5 && id >= pDesk->editMinId && id <= pDesk->editMaxId)
	{
		if((pEdit->view_attr.reserve&0xFF) != 0xFF)//不是edit2
		{
			/*if(inputMode == pEdit->edit_privattr.input)
			{
				return ;
			}*/
			if(inputMode == 4)//当前是手写,则关闭
			{
				edit_change_input_mode(pDesk,0,0);
			}
			pDesk->inputMode = pEdit->edit_privattr.input;
		}
		else
		{
			pEdit2 = (tGRAPEDIT2 *)pEdit;
			/*if(inputMode == pEdit2->edit_privattr.input)
			{
				return ;
			}*/
			if(inputMode == 4)//当前是手写,则关闭
			{
				edit_change_input_mode(pDesk,0,0);
			}
			pDesk->inputMode = pEdit2->edit_privattr.input;
		}
		if(pDesk->inputMode == 4)//切换后是手写, 则打开
		{
			edit_change_input_mode(pDesk,0,1);
		}
		
		t9_closeshow(pDesk);
		ReDraw(pDesk, 0, 0);
	}
}

/*----------------------------------------------
*函数: Grap_ChangeFocusEnable
*功能: 切换焦点项
*参数: pView:控件指针
*返回: none
*----------------------------------------------*/
void Grap_ChangeFocusEnable(tGRAPVIEW *pView)
{
    tGRAPVIEW	*pTempView;
    tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	U16 id;
hyUsbPrintf("FocusEnable = %d   curFocus = %d \r\n",pView->view_attr.FocusEnable, pView->view_attr.curFocus);	
    if(pView->view_attr.FocusEnable)
    {
    	if(!pView->view_attr.curFocus)
    	{
    		pTempView = Grap_GetCurFocus(pView->pParent, NULL);
    		if(pTempView)
    		{
    			pTempView->view_attr.curFocus = 0;
				/*if(pTempView->view_attr.enable)
    			{
    				pTempView->draw(pTempView, 0, 0);
    			}*/
    		}
    		pView->view_attr.curFocus = 1;
    		//pView->draw(pView, 0, 0);
    		//如果是编辑框,则根据每个编辑框指定的输入法类型切换输入法
    		id = pView->view_attr.id;
    	hyUsbPrintf("changed focus id = %d \r\n", id);
    		if(id > EDIT_ID_BASE && id < BUTTON_ID_BASE)
    		{
    			Grap_ChangeInputMode((tGRAPEDIT *)pView);
    		}
    		t9_closeshow(pView->pParent);
    		ReDraw(pView->pParent, 0, 0);
    	}
    }    
}

/*-----------------------------------------
*函数: Grap_ChangeFocus
*功能: 控件切换焦点
*参数: type:向上切换   type：向下切换   当前焦点项
*返回: 
*------------------------------------------*/
S32 Grap_ChangeFocus(tGRAPVIEW *pView, int type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPZORDER  	*pSubZorder ;
	tGRAPVIEW    	*pNextView, *pFocusView;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	U16				panelId = 0;
	
	if(pView == NULL)
	{
		return RETURN_REDRAW;
	}
	pDesk = ((tGRAPDESKBOX *)pView)->pParent;
	pFocusView = Grap_GetCurFocus(pDesk, &panelId);
	if(pFocusView == NULL)
	{
		return RETURN_REDRAW;
	}
hyUsbPrintf("panel id = %d \r\n", panelId);	
	if(panelId != 0)//当前焦点在panel上, 则切换焦点时只在该panel上做变动
	{
		pPanel = (tPANEL *)Grap_GetViewByID(pDesk, panelId);
		if(pPanel == NULL)
		{
			return RETURN_REDRAW;
		}
		
		pPriv = &(pPanel->PanelPriv);
	}
	
	if(1 == type)//向下切换
	{
		pNextView = pFocusView->pNext;
		
		while(pNextView)
		{
			if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
			{
				break;
			}
			pNextView = pNextView->pNext;
		}
		
		if(!pNextView)//直到链表尾都没有找到
		{
		hyUsbPrintf("change focus down panelId = %d  0x%x\r\n",panelId,(U32)pFocusView);
			//重新从头开始找
			if(panelId == 0)
			{
				pNextView = pDesk->pNext;
			}
			else
			{
				pNextView = pPriv->pSubView;
			}
			while(pNextView && pNextView != pFocusView)
			{
			//hyUsbPrintf("pNextView = %x pFocusView = %x\r\n", pNextView, pFocusView);
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
				{
					break;
				}
				pNextView = pNextView->pNext;
			}
			if(pNextView == pFocusView)
			{
				pNextView = NULL;
			}
		}
	}
	else if(0 == type)//向上切换
	{
		pNextView = pFocusView->pPrev;
		
		if(panelId == 0)
		{
			while(pNextView != (tGRAPVIEW *)pDesk)
			{
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
				{
					break;
				}
				pNextView = pNextView->pPrev;
			}
			
			//找到头也没有找到
			if(pNextView == (tGRAPVIEW *)pDesk)
			{
				//从链表尾开始找
				pNextView = pDesk->pTail;
				while(pNextView != pFocusView)
				{
					if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
					{
						break;
					}
					pNextView = pNextView->pPrev;
				}
				
				if(pNextView == pFocusView)
				{
					pNextView = NULL;
				}
			}
		}
		else
		{
			while(pNextView != (tGRAPVIEW *)pPanel)
			{
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
				{
					break;
				}
				pNextView = pNextView->pPrev;
			}
			
			//找到头也没有找到
			if(pNextView == (tGRAPVIEW *)pPanel)
			{
				//从链表尾开始找
				pNextView = pPanel->pTail;
				while(pNextView != pFocusView)
				{
					if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//可以接收焦点
					{
						break;
					}
					pNextView = pNextView->pPrev;
				}
				if(pNextView == pFocusView)
				{
					pNextView = NULL;
				}
			}
		}
	}
	
	//找到
	if(pNextView)
	{
		pFocusView->view_attr.curFocus = 0;
		pFocusView->draw(pFocusView, 0, 0);
		pNextView->view_attr.curFocus = 1;
		pNextView->draw(pNextView, 0, 0);
		
		pFocusView = pNextView;
	hyUsbPrintf("========chaned focus id = %d \r\n", pFocusView->view_attr.id);	
		pSubZorder = pDesk->pZorderList ;
		while(pSubZorder)
		{
			pNextView = pSubZorder->pMember ;
			if (pNextView->view_attr.curFocus && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))
			{
				Grap_UpdateZorder(pDesk, pSubZorder);
				break ;
			}
			pSubZorder = pSubZorder->pNext;
		}
		
		if(pFocusView->view_attr.id > EDIT_ID_BASE && pFocusView->view_attr.id < BUTTON_ID_BASE)
		{
			Grap_ChangeInputMode((tGRAPEDIT *)pFocusView);
		}
		else
		{
			if(pDesk->inputMode == 4)
			{
				edit_change_input_mode(pDesk,0,0);
			}
		}
	}
			
	return RETURN_REDRAW;
}

/*----------------------------------------------
*函数: Grap_SendMessage
*功能: 给指定控件发送消息(阻塞方式)   id为0表示给窗体发送
*参数: pView:控件指针
       pEvent:消息
*返回: none
*----------------------------------------------*/
int Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent)
{
	tGRAPDESKBOX *pDesk;
	tGRAPVIEW	*view;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	if (pView == NULL) 	return -1;
	
	pDesk = (tGRAPDESKBOX *)(pView->pParent);
	if (pEvent->id == 0)
	{
		return pDesk->handle(pDesk, pEvent);
	}
	else if(pEvent->id == pView->view_attr.id)
	{
	//hyUsbPrintf("view id = %d  handle \r\n", pView->view_attr.id);
		return pView->handle(pView, pEvent);
	}
	else
	{
		view = (tGRAPVIEW *)pDesk;
		while (view)
		{
			if(Panel_IfPanel(view))
			{
				pPanel = (tPANEL *)view;
				pPriv = &(pPanel->PanelPriv);
				pSub = pPriv->pSubView;
				while(pSub)
				{
					if(pSub->view_attr.id == pEvent->id)
					{
						return pSub->handle(pSub, pEvent);
					}
					pSub = pSub->pNext;
				}
			}
			else
			{
				if (view->view_attr.id == pEvent->id)
				{
					return view->handle(view, pEvent);
				}
			}
			view = view->pNext;
		}
		return 0;
	}
}

/*----------------------------------------------
*函数: Grap_WriteBMPColor
*功能: 用指定的颜色画取模数据
*参数: x1:列
       y1:行
       width:取模数据宽
       height:取模数据高
       BmpColor:取模数据
       colorP:颜色
*返回: none
*----------------------------------------------*/
void Grap_WriteBMPColor(U16 x1, U16 y1, U16 width, U16 height, U8 *BmpColor, U16 colorP)
{
	drvLcd_SetColor(colorP,0x0000);
	drvLcd_WriteBMPColor(BmpColor,height,width,y1,x1);
	
	return ;
}

/*----------------------------------------------
*函数: Grap_BrushScreen
*功能: 用指定的颜色填充指定区域
*参数: x1:起始列
       y1:起始行
       x2:结束列
       y2:结束行
       colorP:颜色
*返回: none
*----------------------------------------------*/
void Grap_BrushScreen(U16 x1, U16 y1, U16 x2, U16 y2, U16 colorP)
{
	drvLcd_SetColor(0x0000,colorP);
	drvLcd_SetDisplayAddress(Get_Display_DisplayMemory_Offset());
	drvLcd_ClearBox(y1, x1, y2-y1, x2-x1);
	return;
}

/*----------------------------------------------
*函数: Grap_ClearScreen
*功能: 用当前背景图刷新指定区域
*参数: x1:起始列
       y1:起始行
       x2:结束列
       y2:结束行
*返回: none
*----------------------------------------------*/
void Grap_ClearScreen(U16 x1, U16 y1, U16 x2, U16 y2)
{
	drvLcd_Background2DisplayMemory(y2-y1, x2-x1, y1, x1);
	
	return ;
}

/*----------------------------------------------
*函数: Grap_ShowPicture
*功能: 在指定位置显示指定id号的图片
*参数: picId:图片枚举,要确保存在
	   posX:起始列
       posY:起始行
*返回: none
*----------------------------------------------*/
void Grap_ShowPicture(U16 picId, U16 posX, U16 posY)
{
    if (picId > 0)
    {
    	res_load_bmp_to_memory(picId,posX,posY);
	}
	
	return;
}

/*----------------------------------------------
*函数: Grap_Show_Part_BackPicture
*功能: 显示指定图片的一部分
*参数: pView1:底图所在的控件(不一定是大的背景图)
	   pView2:当前要刷新的控件,区域要在pView1中
*返回: none
*----------------------------------------------*/
void Grap_Show_Part_BackPicture(void *pView1,void *pView2)
{
	tGRAPVIEW *ptView,*pview_attr;
	U32 x1,y1,x2,y2,h,w;
	U16 posX,posY,posH,posW;
	U32 picId;
	
    ptView = (tGRAPVIEW *)pView1;
    pview_attr = (tGRAPVIEW *)pView2;
    picId = ptView->view_attr.clNormal;
    
    h = ptView->view_attr.viewHeight;
    w = ptView->view_attr.viewWidth;
    posX = pview_attr->view_attr.viewX1;
    posY = pview_attr->view_attr.viewY1;
    posH = pview_attr->view_attr.viewHeight;
    posW = pview_attr->view_attr.viewWidth;
    
    if (picId > 0)
    {
    	x1 = ptView->view_attr.viewX1; //图片的起始位置
    	y1 = ptView->view_attr.viewY1;
    	x2 = posX - x1;    //相对图片的位置
    	y2 = posY - y1;
    	if(x2 < 0 || y2 < 0 || x2+posW > w || y2+posH > h)
    	{
    		return ; // 区域超出图片区域
    	}

    	res_load_part_to_memory(picId,x2,y2,posX,posY,posH, posW);    	
	}
}
