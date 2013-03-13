#include "grap_edit.h"
#include "grap_event.h"
#include "grap_deskbox.h"


extern const U8  gBmpUpData[];
extern const U8  gBmpDownData[];

U32 gu32PwdDisTimer;


/*-----------------------------------------
*函数: Edit_ChangeFocus
*功能: 输入法切换焦点
*参数: type:向上切换   type：向下切换
*返回: 
*------------------------------------------*/
S32 Edit_ChangeFocus(void *pView, int type)
{
    int id = 0, temp, mode;
	tGRAPDESKBOX	*pDesk;
	tGRAPVIEW		*pWidget;
	tGRAPEDIT       *pEdit ;
	
	pDesk = ((tGRAPDESKBOX *)pView)->pParent;
	pWidget = (tGRAPVIEW *)Grap_GetCurFocus(pDesk, NULL);
	if(pWidget == NULL)
	{
		return RETURN_REDRAW;
	}
	
	id = pWidget->view_attr.id;
	temp = id;
	if(id > pDesk->editMaxId || id < pDesk->editMinId)
	{
		return RETURN_REDRAW;
	}
	
	while(1)
	{
		if(0 == type)//上
		{
			if(id > pDesk->editMinId) id--;
			else id = pDesk->editMaxId;
			//else return RETURN_REDRAW;
		}
		else if(1 == type)//下
		{
			if(id < pDesk->editMaxId) id++;
			else id = pDesk->editMinId;
			//else return RETURN_REDRAW;
		}
		pWidget = (tGRAPVIEW *)Grap_GetViewByID(pDesk,id);

		if(id == temp || pWidget->view_attr.FocusEnable == 1)
		{
			break;
		}
	}
	Grap_ChangeFocusEnable(pWidget);
			
	return RETURN_REDRAW;
}

S32 Grap_EditDropListGetStr(tGRAPEDIT *pEdit, U16 i, U8 *pStr)
{

	return SUCCESS;
}

S32 Grap_EditDropListDrawScroll(tGRAPEDIT *pEdit)
{
	tDROPLIST	*pDropList;
	U16 		x1,x2,y1,y2;
	
	pDropList = &(pEdit->dropList);
	
	x1 = pEdit->view_attr.viewX1+pEdit->view_attr.viewWidth-18;
	x2 = x1+18;
	
	y1 = pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight;
	y2 = pDropList->listnum*pEdit->charh;
	Grap_WriteBMPColor(x1, y1, 18,16, (U8*)gBmpUpData, 0x00);//上
	
	Grap_WriteBMPColor(x1, y2-16, 18, 16, (U8*)gBmpDownData, 0x00);//下
	
	//中间部分的滑块
	
	return SUCCESS;
}

S32 Grap_EditDropListEnter(tGRAPEDIT *pEdit, U16 i)
{
	tDROPLIST	*pDropList;
	
	pDropList = &(pEdit->dropList);
	
	pEdit->dropListGetStr(pEdit, i, pEdit->edit_privattr.digt);
	pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
	if(pEdit->edit_privattr.firstCharPos+pEdit->edit_privattr.len < pEdit->edit_privattr.Pos)
	{
		pEdit->edit_privattr.firstCharPos = pEdit->edit_privattr.Pos-pEdit->edit_privattr.len;
	}
	pDropList->state = 0;//关闭
	
	
	return SUCCESS;
}


S32 Grap_EditDropListDrawOne(tGRAPEDIT *pEdit, U16 i)
{
	tDROPLIST	*pDropList;
	tStrDispInfo disp_info;
	U16			tempLimit;
	U16 		x1,x2,y1,y2;
	U8			str[30],idx;
	
	pDropList = &(pEdit->dropList);
	
	if(i >= pDropList->total)
	{
		return SUCCESS;
	}
	x1 = pEdit->view_attr.viewX1;
	x2 = x1+pEdit->view_attr.viewWidth;

	if(x2 > gLcd_Total_Column) x2 = gLcd_Total_Column;
	idx = i-pDropList->top;
	if(pDropList->direction == 0)//向下展开
	{
		y1 = idx*pEdit->charh+pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight;
		y2 = pEdit->charh+y1;
	}
	else//向上展开
	{
		y2 = pEdit->view_attr.viewY1-idx*pEdit->charh;
		y1 = y2-pEdit->charh;
	}
	
	if(i == pDropList->focus)//焦点项
	{
		disp_info.color  = pDropList->focusColor;
	}
	else
	{
		disp_info.color  = 0x00;
	}
	
	memset(str, 0, sizeof(str));
	pEdit->dropListGetStr(pEdit, i, str);
	if(str[0] != 0 || str[1] != 0)
	{
		disp_info.string = str;
		disp_info.font_size = pEdit->edit_privattr.font_size;
		disp_info.font_type = pEdit->edit_privattr.font_type;
		disp_info.flag      = 0;
		disp_info.keyStr = NULL;
		tempLimit = gu16Lcd_ColumnLimit;
		gu16Lcd_ColumnLimit = x2;
		Grap_WriteString(x1, y1, x2, y2, &disp_info);
		gu16Lcd_ColumnLimit = tempLimit;
	}
	
	return SUCCESS;
}

S32 Grap_EditDrawInputCnt(tGRAPEDIT *pEdit)
{
    return SUCCESS;
}


S32 Grap_EditRecvSysMsg(void *pView)
{

	return SUCCESS;
}

S32 Grap_EditGetFocus(tGRAPEDIT *pEdit)
{
    return SUCCESS;
}

S32 Grap_EditEnter(void *pView)
{
	return SUCCESS;
}

S32 Grap_DropListCheckRange(U16 x, U16 y, tGRAPEDIT *pEdit, tDROPLIST *pDropList)
{
	U16 y1,y2,cnt;
	
	cnt = (pDropList->total > pDropList->listnum)?pDropList->listnum:pDropList->total;
	if(pDropList->direction == 0)//向下
	{
		y1 = pEdit->view_attr.viewY1;
		y2 = y1+pEdit->view_attr.viewHeight+cnt*pEdit->charh;
	}
	else
	{
		y2 = pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight;
		y1 = pEdit->view_attr.viewY1-cnt*pEdit->charh;
	}
	
	if(x >= pEdit->view_attr.viewX1 && x <= pEdit->view_attr.viewX1+pEdit->view_attr.viewWidth &&
	   y >= y1 && y <= y2)
	{
		return 1;
	}
	
	return 0;
}

S32 Grap_EditRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPEDIT *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv  *pedit_privattr;
	tDROPLIST	   *pDropList;
	Character_Type_e kLycStr_Type;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL, rr;
	int x, y, lastx,lasty,pos ;
	int len, tempx=0;
	U16 date;
	U8  i,charwidth;


	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pDropList = &(pEdit->dropList);
	
	kLycStr_Type = pedit_privattr->font_type;

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	lasty = HIWORD(wParam) ;
	lastx = LOWORD(wParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(pDropList->state == 1)//展开
		{
			if(Grap_DropListCheckRange(x,y,pEdit,pDropList))
			{//在有效范围内
				ret = VIEW_PULL_START;
			}
			else
			{
				pDropList->state = 0;//关闭
		    	ReDraw(pEdit, 0, 0);
	        	return FAIL;
			}
		}
		else
		{
			if(GRAP_INWIDGET_RANGE(x,y,pEdit))
		    {//在有效范围内
		         ret = VIEW_PULL_START; 
		    }
		    else
		    {
		        return FAIL;
		    }
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		
		if(pEdit->view_attr.FocusEnable)
		{
			if(!pEdit->view_attr.curFocus)
			{
				Grap_ChangeFocusEnable(pEdit);
				pEdit->get_focus(pEdit);
			}
		}

		if(pDropList->state == 1)//展开
		{
			if(pDropList->direction == 0)//向下
			{
				for(i = 0; i < pDropList->listnum; i++)
				{
					if(pDropList->top + i >= pDropList->total)
						break;
					if(y >= pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight+i*pEdit->charh &&
					   y <= pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight+(i+1)*pEdit->charh)
					{
						pDropList->focus = i+pDropList->top;
						break ;
					}
				}
			}
			else
			{
				for(i = 0; i < pDropList->listnum; i++)
				{
					if(pDropList->top + i >= pDropList->total)
						break;
					if(y >= pEdit->view_attr.viewY1-(i+1)*pEdit->charh &&
					   y <= pEdit->view_attr.viewY1-i*pEdit->charh)
					{
						pDropList->focus = i+pDropList->top;
						break ;
					}
				}
			}
			
			pEdit->draw(pEdit, 0, 0);
		}
		else
		{
			charwidth = pEdit->charw;
			len = x-pEdit->view_attr.viewX1;
			pedit_privattr->Pos = pedit_privattr->firstCharPos;
			pos = pedit_privattr->firstCharPos;
			while(tempx<len)
			{
				if(pos >= (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt))	break;
				if(pedit_privattr->digt[pos] > 0x80 )//汉字
				{
					rr = charwidth*2 ;
					pos+=2;
					if(tempx+rr > charwidth*pedit_privattr->len)	break;
					pedit_privattr->Pos+=2;
				}
				else
				{
					rr = charwidth ;
					pos++;
					if(tempx+rr > charwidth*pedit_privattr->len)	break;
					pedit_privattr->Pos++;
				}	
				tempx += rr;
			}

			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;
			pEdit->draw(pEdit, 0, 0);
		}
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
		if(pDropList->state == 1)//展开
		{
			ret = VIEW_PULL_ING;
		}
		else
		{
			if(GRAP_INWIDGET_RANGE(x,y,pEdit))
			{
				len    = strlen( pedit_privattr->digt );
				
				if( len < pedit_privattr->len || ABS(x-lastx) < 4) /* 不够 */
				{
					return VIEW_PULL_ING;
				}
				else
				{
					U16   lenTem ;				
					S16   deart ;
					
					deart  = x-lastx ;				
					
					if( deart < 0 )/* 向前滑动 */
					{
						lenTem = pedit_privattr->firstCharPos + pedit_privattr->len;
						if( lenTem >= len) /* 托到最后了 */
						{
							return VIEW_PULL_ING;
						}
						else
						{
							if(pedit_privattr->digt[lenTem] > 0x80)//汉字
							{
								pedit_privattr->Pos += 2 ;				
							}
							else
							{
								pedit_privattr->Pos ++ ;				
							}
							
							if(pedit_privattr->digt[pedit_privattr->firstCharPos] > 0x80)
							{
								pedit_privattr->firstCharPos += 2 ;
							}
							else
							{
								pedit_privattr->firstCharPos ++ ;
							}
							pEdit->draw(pEdit, 0, 0);
						}					
					}
					else if( deart > 0 )
					{					
						if( pedit_privattr->firstCharPos <= 0 ) /* 托到最后了 */
						{
							return VIEW_PULL_ING;
						}
						else
						{
							if(pedit_privattr->digt[pedit_privattr->firstCharPos - 1] > 0x80)//汉字
							{
								pedit_privattr->firstCharPos -= 2 ;
								pedit_privattr->Pos -= 2 ;				
							}
							else
							{
								pedit_privattr->firstCharPos -- ;
								pedit_privattr->Pos -- ;				
							}
							
							pEdit->draw(pEdit, 0, 0);
						}					
						
					}				
				}		
				
				ret = VIEW_PULL_ING;
			}
		}
	}
	else if (VIEW_PULL_END == extParam)
	{//up
		pEdit->pullflag = 0;
		
		if(pDropList->state == 1)//展开
		{
			if(!Grap_DropListCheckRange(x,y,pEdit,pDropList))
			{//在有效范围内
				pDropList->state = 0;//关闭
				ReDraw(pEdit, 0, 0);

				return VIEW_PULL_END;
			}
		}
		else
		{
			if(!GRAP_INWIDGET_RANGE(x,y,pEdit))
			{
	            return VIEW_PULL_END;
	        }
	    }
		
		if(pDropList->state == 1)//展开
		{
			if(pDropList->direction == 0)//向下
			{
				for(i = 0; i < pDropList->listnum; i++)
				{
					if(pDropList->top + i >= pDropList->total)
						break;
					if(y >= pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight+i*pEdit->charh &&
					   y <= pEdit->view_attr.viewY1+pEdit->view_attr.viewHeight+(i+1)*pEdit->charh)
					{
						pDropList->focus = i+pDropList->top;
						break ;
					}
				}
			}
			else
			{
				for(i = 0; i < pDropList->listnum; i++)
				{
					if(pDropList->top + i >= pDropList->total)
						break;
					if(y >= pEdit->view_attr.viewY1-(i+1)*pEdit->charh &&
					   y <= pEdit->view_attr.viewY1-i*pEdit->charh)
					{
						pDropList->focus = i+pDropList->top;
						break ;
					}
				}
			}
			
			pEdit->dropListEnter(pEdit, pDropList->focus);
			
			//必须要redraw来重刷新下拉框的区域
			ReDraw(pEdit, 0, 0); 
		}
		else
		{
			ret = pEdit->editenter(pEdit);
	        
	        if(ret== RETURN_REDRAW)
			{
				ReDraw(pEdit, 0, 0);
	        	ret = VIEW_PULL_END;         
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pEdit->pParent))->pQuitCnt = 1;
			}
			else if (ret == RETURN_FOCUSMSG)
			{
				return ret;
			}
		}		
		return PARSED;     
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
		pEdit->pullflag = 0;
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;    
}


// Edit draw function
S32 Grap_EditDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT *pEdit;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	tDROPLIST	  *pDropList;	
	Character_Type_e kLycStr_Type;
	U8 tempdata[50]={0};
	U16 x,y,w,h,i,j;
	U8 temp[30], *pStr;
			
	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pDropList  = &(pEdit->dropList);
	
	kLycStr_Type = pedit_privattr->font_type;
	
	//clear edit screen
	if (pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
	}
	else if(pedit_privattr->coverPicId)
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	else if(pview_attr->color != pedit_privattr->color)
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}

	if(strlen(pedit_privattr->digt) == 0)
	{
		pedit_privattr->firstCharPos = 0 ;
		pedit_privattr->Pos          = 0 ;
	}
	else if(pedit_privattr->Pos > strlen(pedit_privattr->digt))
	{
		pedit_privattr->Pos = strlen(pedit_privattr->digt);
	}
	
	
	//检查是否处在半个汉字位置
	for(i = 0; i < pedit_privattr->firstCharPos; i++)
	{
		if(pedit_privattr->digt[i] > 0x80)//汉字
		{
			if(i+1 >= pedit_privattr->firstCharPos)
			{
				pedit_privattr->firstCharPos++;
				break;
			}
			else
			{
				i++;
			}
		}
	}

	// write string
	if (pedit_privattr->digt[pedit_privattr->firstCharPos])
	{
		U16 len;

		x = pview_attr->viewX1+pedit_privattr->xoffset;
		y = pview_attr->viewY1+pedit_privattr->yoffset;
		
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->firstCharPos);

		if(len <= pedit_privattr->len)
		{
			j = 0;
			pStr = pedit_privattr->digt+pedit_privattr->firstCharPos;
			if(kLycStr_Type == CHARACTER_LOCALCODE)
			{
				for(i=0; i<pedit_privattr->len;i++)
				{
					if(pStr[i]>0x80)
					{
						j+=2;i++;
						if(j > pedit_privattr->len)
						{j-=2;break;}
					}
					else
					{
						j++;
						if(j > pedit_privattr->len)
						{j--;break;}
					}
				}
				//hyUsbPrintf("edit j = %d \r\n", j);
			}
			else if(kLycStr_Type == CHARACTER_UNICODE)
			{
				j = pedit_privattr->len;
				if(j%2!=0)	j--;
			}
			memcpy(temp, (pedit_privattr->digt+pedit_privattr->firstCharPos), j);
			temp[j] = 0;
			temp[j+1] = 0;
			disp_info.string = temp;
			
			if (pedit_privattr->view_pwd == 1)
			{
			    memset(tempdata,0x2A,len);
			    if(ABS(rawtime(NULL)-gu32PwdDisTimer) < 150 && pview_attr->curFocus)
			    {//输入密码时先显示明码，1秒后再显示*
			    	if(pedit_privattr->Pos <= pedit_privattr->len)
			    	{
			    		tempdata[pedit_privattr->Pos-1] = pedit_privattr->digt[pedit_privattr->Pos-1];
			    	}
			    	else
			    	{
			    		tempdata[pedit_privattr->len-1] = pedit_privattr->digt[pedit_privattr->Pos-1];
			    	}
			    }
			    else
			    {
			    	gu32PwdDisTimer = 0;
			    }
			    disp_info.string = tempdata;    
			}

			disp_info.color = pedit_privattr->color;
			disp_info.font_size = pedit_privattr->font_size;
			disp_info.font_type = pedit_privattr->font_type;
			disp_info.flag = pedit_privattr->position;
			disp_info.keyStr = NULL;
			Grap_WriteString(x, y, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
		}
		else
		{
			j = 0;
			pStr = pedit_privattr->digt+pedit_privattr->firstCharPos;
			if(kLycStr_Type == CHARACTER_LOCALCODE)
			{
				for(i=0; i<pedit_privattr->len;i++)
				{
					if(pStr[i]>0x80)
					{
						j+=2;i++;
						if(j > pedit_privattr->len)
						{j-=2;break;}
					}
					else
					{
						j++;
						if(j > pedit_privattr->len)
						{j--;break;}
					}
				}
				//hyUsbPrintf("edit j = %d \r\n", j);
			}
			else if(kLycStr_Type == CHARACTER_UNICODE)
			{
				j = pedit_privattr->len;
				if(j%2!=0)	j--;
			}
			memcpy(temp, (pedit_privattr->digt+pedit_privattr->firstCharPos), j);
			temp[j] = 0;
			temp[j+1] = 0;
			disp_info.string = temp;			
			if (pedit_privattr->view_pwd == 1)
			{
			    memset(tempdata,0x2A,j);
			    if(ABS(rawtime(NULL)-gu32PwdDisTimer) < 150 && pview_attr->curFocus)
			    {//输入密码时先显示明码，1秒后再显示*
			    	tempdata[pedit_privattr->Pos-1] = pedit_privattr->digt[pedit_privattr->Pos-1];
			    }
			    else
			    {
			    	gu32PwdDisTimer = 0;
			    }
			    disp_info.string = tempdata;    
			}			
			disp_info.color = pedit_privattr->color;
			disp_info.font_size = pedit_privattr->font_size;
			disp_info.font_type = pedit_privattr->font_type;
			disp_info.flag = pedit_privattr->position;
			disp_info.keyStr = NULL;
			Grap_WriteString(x,y,(U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
		}
	}
	
	pEdit->DrawInputCnt(pEdit);
	
	if(pDropList->state == 0)//未展开
	{
		//如果是当前焦点,则在外面画边框
		Grap_DrawFocusRect(pview_attr, COLOR_RED);
	}
	else if(pview_attr->curFocus == 0)
	{
		//如果已经不是焦点项,则直接关闭
		pDropList->state = 0;
	}
	else//绘制下拉框
	{
		x = pview_attr->viewX1;
		w = pview_attr->viewWidth;
		if(pDropList->direction == 0)//向下
		{
			y = pview_attr->viewY1+pview_attr->viewHeight;
			//if(pDropList->listnum <= pDropList->total)
				h = pDropList->listnum*pEdit->charh;
			//else
			//	h = pDropList->total*pEdit->charh;
		}
		else
		{
			//if(pDropList->listnum <= pDropList->total)
				h = pDropList->listnum*pEdit->charh;
			//else
			//	h = pDropList->total*pEdit->charh;
			if(pview_attr->viewY1 >= h)
			{
				y = pview_attr->viewY1-h;
			}
			else
			{
				y = 0;
				h = pview_attr->viewY1;
			}
		}
		
		drvLcd_SetColor(pDropList->focusColor,pDropList->focusColor);
		drvLcd_Rectangle(y-1,x-1,h+2,w+2,1);
		Grap_BrushScreen(x,y,x+w,y+h, pDropList->backColor);
		
		//绘制滚动条
		if(pDropList->total > pDropList->listnum)
		{
			//pEdit->dropListDrawScroll(pEdit);//功能还没完全实现
		}
		
		//绘制每条信息
		for(i = 0; i < pDropList->listnum; i++)
		{
			if(pDropList->top+i >= pDropList->total)
				break;
			pEdit->dropListDrawOne(pEdit, pDropList->top+i);
		}
	}
	
	return SUCCESS;
}

S32 Grap_EditDestroy(void *pView)
{
	tGRAPEDIT *pEdit;
	
	pEdit = (tGRAPEDIT*)pView;
	
	if(0 == pEdit->edit_privattr.mallocFlag && pEdit->edit_privattr.digt != NULL )
		free((void*)(pEdit->edit_privattr.digt));
	free((void*)pEdit);
	
	return SUCCESS;
}

void Grap_EditRollShow(void *pView)
{
	tGRAPEDIT      *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv  *pedit_privattr;
	U16            len ;
	
	pEdit          = (tGRAPEDIT *)pView;
	pview_attr     = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	len  = strlen( pedit_privattr->digt );
	if( len < pedit_privattr->len ) /* 不够 */
 	{
		return ;
	}
	else
	{
		U16 lenTem ;
		
		lenTem = pedit_privattr->firstCharPos + pedit_privattr->len;
		if( lenTem >= len)
		{
			pedit_privattr->firstCharPos = 0 ;
			pedit_privattr->Pos          = 0 ;
		}
		else
		{
			if(pedit_privattr->digt[lenTem] > 0x80)//汉字
			{
				pedit_privattr->firstCharPos += 2 ;
				pedit_privattr->Pos += 2 ;				
			}
			else
			{
				pedit_privattr->firstCharPos ++ ;
				pedit_privattr->Pos ++ ;				
			}
			pEdit->draw(pEdit, 0, 0);
		}
	}
}

S32 Grap_EditCursorShow(void *pView)
{
	int i ;
	U16 date;
	S32 ret;
	U16 color,x1,y1;
	tGRAPEDIT *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	U8 charwidth, charheight;	
	
	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	if (pview_attr->curFocus == 0) return SUCCESS;
	
	charwidth  = pEdit->charw;
	charheight = pEdit->charh-2;
	
	x1 = pview_attr->viewX1 + pedit_privattr->xoffset;
	for(i = pedit_privattr->firstCharPos; i < pedit_privattr->Pos; i++)
	{
		if(pedit_privattr->digt[i] > 0x80)//汉字
		{
			ret = charwidth*2 ;
			i++;
		}
		else
		{
			ret = charwidth ;
		}	
		x1 += ret;
	}
	y1 = pview_attr->viewY1+pedit_privattr->yoffset;
	
	if(pedit_privattr->enable)
	{
		if(gu32PwdDisTimer != 0)
		{
			pEdit->draw(pEdit,0,0);
		}
		if(pedit_privattr->showState)	
		{
			color = COLOR_BLACK;
		}
		else//应该用背景色
		{
			color = COLOR_WHITE;
		}
		Grap_BrushScreen(x1, y1, (U16)(x1+1), (U16)(y1+charheight), color);
	}
	
	return SUCCESS;
}

extern U8  Keycount;
extern U32 Keylasttime;
extern U16 Keylastkey;
U32 Grap_EditKeyInput(void *pView, U32 *keycode)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT		*pEdit;
	tGrapViewAttr  	*pview_attr;
	tGrapEditPriv 	*pedit_privattr;
	U32 i,key;
	S8  ret;
	
	gu32PwdDisTimer = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	key = *keycode;
	
	pDesk = (tGRAPDESKBOX *)(pEdit->pParent);
	switch(key)
	{
	case KEYEVENT_0://0--9
	case KEYEVENT_1:
	case KEYEVENT_2:
	case KEYEVENT_3:
	case KEYEVENT_4:
	case KEYEVENT_5:
	case KEYEVENT_6:
	case KEYEVENT_7:
	case KEYEVENT_8:
	case KEYEVENT_9:
		if(pedit_privattr->containFlag == 1 && (pDesk->inputMode != 1 && pDesk->inputMode != 4))
		{//只允许输入数字时 要判断当前输入法状态(必须为 123 或 手写)
			ret = -1;
		}
		else
		{
			ret = KeyInputChar(keycode);
		}
		//hyUsbPrintf("ret = %d  key = %d  \r\n", ret, *keycode);
		if(ret == -1)//错误
		{
			return PARSED;
		}
		else if(ret == 0)//第一次输入
		{
			gu32PwdDisTimer = rawtime(NULL);
			return SUCCESS;
		}
		else
		{
			gu32PwdDisTimer = rawtime(NULL);
			pedit_privattr->digt[pedit_privattr->Pos-1] = *keycode;
			//pEdit->draw(pEdit,0,0);
			pEdit->editKeyEnd(pEdit, 1);
			pEdit->editKeyEnd(pEdit, 0);
			return PARSED;
		}
		break;
	case KEYEVENT_STAR://*  弹出符号   (现在暂作'.'符号)
		*keycode = '.';
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return SUCCESS;
		break;
	case KEYEVENT_UP:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		//if(pDesk->editMaxId > pDesk->editMinId)
		{
			//Edit_ChangeFocus(pEdit,0);
			Grap_ChangeFocus(pEdit,0);
			return PARSED;
		}
		/*else
		{
			return FAIL;
		}*/
		break;
	case KEYEVENT_DOWN:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		//if(pDesk->editMaxId > pDesk->editMinId)
		{
			//Edit_ChangeFocus(pEdit,1);
			Grap_ChangeFocus(pEdit,1);
			return PARSED;
		}
		/*else
		{
			return FAIL;
		}*/
		break;
	case KEYEVENT_LEFT:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	case KEYEVENT_RIGHT:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	case KEYEVENT_DELETE:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	default:
		Keylastkey  = 0xff;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	}
	
	return FAIL;
}

U32 Grap_EditKeyEnd(void *pView, U8 type)
{
	return SUCCESS ;
}


// default handle
//S32 Grap_EditHandle(void *pView, U32 message, U32 wParam, U32 lParam)
S32 Grap_EditHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	U16 len,tempLen;
	tGRAPEDIT *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	Character_Type_e kLycStr_Type;
	S32 ret = SUCCESS;

	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	kLycStr_Type = pedit_privattr->font_type;
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	if(message == VIEW_PRESS && pview_attr->curFocus && pEdit->editKeyInput != NULL)
	{
		U32 inputret;
		
		inputret = pEdit->editKeyInput(pEdit,&lParam);
		//hyUsbPrintf("inputret = %d  lParam = %d  \r\n", inputret, lParam);
		if(PARSED == inputret)
		{
			//pEdit->draw(pEdit,0,0);
			ReDraw(pEdit,0,0);
			return PARSED;
		}
		else if(inputret == VIEW_DELETE)
		{
			message = VIEW_DELETE;
			ret = PARSED;
		}
		else if(inputret == FAIL)
		{
			//return ret;
		}
		else if(lParam != 0xff)
		{
			message = VIEW_INPUT;
			ret = PARSED;
		}
	}
	
	switch(message)
	{
	case VIEW_INIT:
		if(strlen(pedit_privattr->digt) <= pedit_privattr->len)
		{
			pedit_privattr->Pos = pedit_privattr->len;
		}
		else
		{
			pedit_privattr->Pos = 0;//Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		}
		break;
	case VIEW_ENABLE:
		pEdit->view_attr.enable = 1;
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_DISABLE:
		pEdit->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_TIMER:
		if(pedit_privattr->TimerLft == 0)
		{
			pedit_privattr->TimerLft = pedit_privattr->TimerTot;
			pedit_privattr->showState = (1-pedit_privattr->showState);
			pEdit->editTimer(pEdit);
		}
		else
		{ 
			S32 timeleft;
			timeleft =  pedit_privattr->TimerLft-lParam;
			
			if (timeleft < 0)  pedit_privattr->TimerLft = 0;
			else pedit_privattr->TimerLft = timeleft;			
		}
		break;
	case VIEW_INPUT:
	case VIEW_DIGIT:
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		if (len < pedit_privattr->containLen)
		{	
			int i;
			U8 temp[MAX_DIGITEDIT_LEN];
			U8 hCode = 0, lCode = 0;

			memset(temp,0,MAX_DIGITEDIT_LEN);
			lCode = (U8)(lParam&0xff) ;
			hCode = (U8)((lParam>>8)&0xff) ;
		//hyUsbPrintf("edit input lcode  = %x  hcode = %x \r\n",lCode, hCode);
			if((kLycStr_Type == CHARACTER_UNICODE||hCode > 0) && len+2 > pedit_privattr->containLen)
				break;
			if(hCode > 0 && pedit_privattr->containFlag)
				break;
			if( (pedit_privattr->containFlag&CONTAIN_NUM) &&( (lCode>0x39 || lCode<0x30) || hCode > 0))
				break;
			if( (pedit_privattr->containFlag&CONTAIN_NUM1) &&( (lCode != '.' &&(lCode>0x39 || lCode<0x30)) || hCode > 0))
				break;
			//if( 0x3C == lCode || 0x3E == lCode || 0x26 == lCode) /* 韵达规定不允许输入<>& */
			//	break;
			tempLen = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->Pos);
			memcpy(temp, pedit_privattr->digt+pedit_privattr->Pos,tempLen+2);//光标后面的字符

			if(hCode || kLycStr_Type == CHARACTER_UNICODE)
				pedit_privattr->digt[pedit_privattr->Pos++] = hCode;

			pedit_privattr->digt[pedit_privattr->Pos++] = lCode;
			memcpy(pedit_privattr->digt+pedit_privattr->Pos,temp, tempLen+2);//光标后面的字符

			while((pedit_privattr->Pos-pedit_privattr->firstCharPos) > pedit_privattr->len)
			{
				if(pedit_privattr->digt[pedit_privattr->firstCharPos] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
					pedit_privattr->firstCharPos+=2;
				else
					pedit_privattr->firstCharPos++;
			}
			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;	
			gu32PwdDisTimer = rawtime(NULL);
			pEdit->draw(pEdit, 0, 0);
			pEdit->editKeyEnd(pEdit, 0);
		}
		else
		{
			gu32PwdDisTimer = 0;
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_LEFT:
		break;
	case VIEW_RIGHT:
		break;
	case VIEW_DELETE:
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		if (len > 0 && pedit_privattr->Pos>0)
		{
			int i,j;
			for(i=0;i<pedit_privattr->Pos;i++)
			{
				if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
					{i++;j=2;}
				else	j = 1;
			}
			tempLen = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->Pos);
			memcpy(pedit_privattr->digt+pedit_privattr->Pos-j,pedit_privattr->digt+pedit_privattr->Pos,tempLen+2);
			
			if(pedit_privattr->firstCharPos == pedit_privattr->Pos)
			{
				pedit_privattr->Pos-=j;
				pedit_privattr->firstCharPos-=j;
				for(i=0;i<pedit_privattr->Pos;i++)
				{
					if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
						{i++;j=2;}
					else	j = 1;
				}
				if(pedit_privattr->firstCharPos >= j)
					pedit_privattr->firstCharPos-=j;
			}
			else
			{
				pedit_privattr->Pos-=j;
				j = 0;
				while(pedit_privattr->firstCharPos > 0)
				{
					len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->firstCharPos);
					if(len < pedit_privattr->len)
					{
						for(i=0;i<pedit_privattr->firstCharPos;i++)
						{
							if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
								{i++;j=2;}
							else	j = 1;
						}
					}
					if(len + j < pedit_privattr->len)
					{
						pedit_privattr->firstCharPos-=j;
					}
					else
					{
						break;
					}
				}
			}
			
			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;
			pEdit->draw(pEdit, 0, 0);
			pEdit->editKeyEnd(pEdit, 1);
		}
		break;
	case VIEW_DELETEALL:
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		if (len > 0 )
		{
			memset(pedit_privattr->digt,0,pedit_privattr->containLen+2);
			pedit_privattr->Pos = 0;
			pedit_privattr->firstCharPos = 0;			
			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_QUIT:
		break;
	case VIEW_KEY_MSG:
	case VIEW_PRESS:
		//hyUsbPrintf("focus = %d  lparam  = %d   wparam  = %d  \r\n", pEdit->view_attr.curFocus,lParam,wParam);
		if(pEdit->view_attr.curFocus == 1 || pEvent->reserve == 1)//焦点时响应OK键
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				if(cKEYB_EVENT_RELEASE == wParam)
				{
					ret = pEdit->editenter(pEdit);
					if ( ret== RETURN_REDRAW)
					{
						ReDraw(pEdit, 0, 0);
						return PARSED;
					}
					else if(ret == RETURN_QUIT)
					{
						*((tGRAPDESKBOX *)(pEdit->pParent))->pQuitCnt = 1;
						pEdit->draw(pEdit, 0, 0);
					}
								
					return ret;
				}
				break;
			case KEYEVENT_LEFT:
				if(pedit_privattr->Pos > 0)
				{	
					U16 i,j=1;
					for(i=0;i<pedit_privattr->Pos;i++)
					{
						if(pedit_privattr->digt[i] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
						{i++;j=2;}
						else
						{j=1;}
					}
					pedit_privattr->Pos-=j;
					if(pedit_privattr->Pos < pedit_privattr->firstCharPos)
						pedit_privattr->firstCharPos = pedit_privattr->Pos;

					pedit_privattr->TimerLft = 1;
					pedit_privattr->showState = 0;
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_RIGHT:
				len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
				if(pedit_privattr->Pos < len)
				{
					if(pedit_privattr->digt[pedit_privattr->Pos] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
						pedit_privattr->Pos+=2;
					else
						pedit_privattr->Pos++;
					if(pedit_privattr->Pos > len)
						pedit_privattr->Pos = len;

					while((pedit_privattr->Pos-pedit_privattr->firstCharPos) > pedit_privattr->len)
					{
						if(pedit_privattr->digt[pedit_privattr->firstCharPos] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
							pedit_privattr->firstCharPos+=2;
						else
							pedit_privattr->firstCharPos++;
					}

					pedit_privattr->TimerLft = 1;
					pedit_privattr->showState = 0;
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_DELETE:
				len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
				if (len > 0 && pedit_privattr->Pos>0)
				{
					int i,j;
					for(i=0;i<pedit_privattr->Pos;i++)
					{
						if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
							{i++;j=2;}
						else	j = 1;
					}
					tempLen = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->Pos);
					memcpy(pedit_privattr->digt+pedit_privattr->Pos-j,pedit_privattr->digt+pedit_privattr->Pos,tempLen+2);
					
					if(pedit_privattr->firstCharPos == pedit_privattr->Pos)
					{
						pedit_privattr->Pos-=j;
						pedit_privattr->firstCharPos-=j;
						for(i=0;i<pedit_privattr->Pos;i++)
						{
							if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
								{i++;j=2;}
							else	j = 1;
						}
						if(pedit_privattr->firstCharPos >= j)
							pedit_privattr->firstCharPos-=j;
					}
					else
					{
						pedit_privattr->Pos-=j;
						j = 0;
						while(pedit_privattr->firstCharPos > 0)
						{
							len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->firstCharPos);
							if(len < pedit_privattr->len)
							{
								for(i=0;i<pedit_privattr->firstCharPos;i++)
								{
									if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
										{i++;j=2;}
									else	j = 1;
								}
							}
							if(len + j < pedit_privattr->len)
							{
								pedit_privattr->firstCharPos-=j;
							}
							else
							{
								break;
							}
						}
					}
					
					pedit_privattr->TimerLft = 1;
					pedit_privattr->showState = 0;
					pEdit->draw(pEdit, 0, 0);
					pEdit->editKeyEnd(pEdit, 1);
				}
				
				return PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_REV_SYSMSG:
		pEdit->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pEdit->recvPullMsg(pEdit,pEvent);
	    break;	
	}
	return ret;
}


tGRAPEDIT *Grap_CreateEdit(tGrapViewAttr  *pview_attr, tGrapEditPriv *pedit_privattr)
{
	tGRAPEDIT *pDigitEdit;

	pDigitEdit = (tGRAPEDIT *)malloc(sizeof(tGRAPEDIT));
	if (!pDigitEdit)
	{
		return NULL;
	}
	memset(pDigitEdit,0,sizeof(tGRAPEDIT));
	pDigitEdit->view_attr = *pview_attr;
	pDigitEdit->edit_privattr = *pedit_privattr;
	
	if(0 == pDigitEdit->edit_privattr.mallocFlag)
	{
		pDigitEdit->edit_privattr.digt = (U8 *)malloc(pDigitEdit->edit_privattr.containLen+2);
		memset(pDigitEdit->edit_privattr.digt,0,(pDigitEdit->edit_privattr.containLen+2));
	}
	
	pDigitEdit->disLenBack = pDigitEdit->edit_privattr.len;
	switch(pedit_privattr->font_size)
	{
	case FONTSIZE5X8:
		pDigitEdit->charw = 5;
		pDigitEdit->charh = 8;
		break;
	case FONTSIZE7X11:
		pDigitEdit->charw = 7;
		pDigitEdit->charh = 11;
		break;
	case FONTSIZE12X17:
		pDigitEdit->charw = 12;
		pDigitEdit->charh = 17;
		break;
	case FONTSIZE6X9:
		pDigitEdit->charw = 6;
		pDigitEdit->charh = 9;
		break;
	case FONTSIZE6X5:
	case FONTSIZE6X6:
		pDigitEdit->charw = 6;
		pDigitEdit->charh = 6;
		break;
	case FONTSIZE9x7:
		pDigitEdit->charw = 9;
		pDigitEdit->charh = 7;
		break;
	case FONTSIZE5X7:
		pDigitEdit->charw = 5;
		pDigitEdit->charh = 7;
		break;
	case FONTSIZE8X16:
		pDigitEdit->charw = 8;
		pDigitEdit->charh = 16;		
		break;	
	case FONTSIZE24X24:
		pDigitEdit->charw = 12;
		pDigitEdit->charh = 24;
		break;
	case FONTSIZE32X32:
		pDigitEdit->charw = 16;
		pDigitEdit->charh = 32;
		break;						
	default:
		pDigitEdit->charw = 8;
		pDigitEdit->charh = 16;
		break;			
	}
	
	pDigitEdit->pullflag = 0;
	pDigitEdit->draw = Grap_EditDraw;
	pDigitEdit->handle = Grap_EditHandle;
	pDigitEdit->destroy = Grap_EditDestroy;
	pDigitEdit->recvSysMsg = Grap_EditRecvSysMsg;
	pDigitEdit->get_focus =  Grap_EditGetFocus;
	pDigitEdit->recvPullMsg =  Grap_EditRecvPullMsg;
	
	pDigitEdit->editenter		= Grap_EditEnter;
	pDigitEdit->editKeyInput	= Grap_EditKeyInput;
	pDigitEdit->editKeyEnd		= Grap_EditKeyEnd;
	pDigitEdit->editTimer		= Grap_EditCursorShow;

	pDigitEdit->dropListGetStr  = Grap_EditDropListGetStr;
	pDigitEdit->dropListDrawScroll = Grap_EditDropListDrawScroll;
	pDigitEdit->dropListDrawOne    = Grap_EditDropListDrawOne;
	pDigitEdit->DrawInputCnt       = Grap_EditDrawInputCnt;
	pDigitEdit->dropListEnter	   = Grap_EditDropListEnter;
	
	pDigitEdit->dropList.listnum = 4;
	pDigitEdit->dropList.focus   = 0xff;
	pDigitEdit->dropList.high    = pDigitEdit->charh*pDigitEdit->dropList.listnum;
	pDigitEdit->dropList.backColor  = 0xffff;
	pDigitEdit->dropList.focusColor = 0xF800;
	
	return pDigitEdit;
}


tGRAPEDIT *Grap_InsertEdit(void *pView, tGrapViewAttr  *pview_attr, tGrapEditPriv *pedit_privattr)
{
	tGRAPDESKBOX *pParentView;
	tGRAPEDIT *pDigitEdit;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pDigitEdit = (tGRAPEDIT *)malloc(sizeof(tGRAPEDIT));
	if (!pDigitEdit)
	{
		return NULL;
	}
	memset(pDigitEdit,0,sizeof(tGRAPEDIT));
	pDigitEdit->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pDigitEdit;
	pParentView->pTail = (tGRAPVIEW *)pDigitEdit;
	pDigitEdit->pParent = pParentView;
	
	//pDigitEdit->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pDigitEdit;
	//pDigitEdit->pParent = pParentView;
	
	pDigitEdit->view_attr = *pview_attr;
	pDigitEdit->edit_privattr = *pedit_privattr;
	
	if(pDigitEdit->edit_privattr.containLen > MAX_DIGITEDIT_LEN)
	{//限制最大为MAX_DIGITEDIT_LEN
		pDigitEdit->edit_privattr.containLen = MAX_DIGITEDIT_LEN;
	}
	if(0 == pDigitEdit->edit_privattr.mallocFlag)
	{
		pDigitEdit->edit_privattr.digt = (U8 *)malloc(pDigitEdit->edit_privattr.containLen+2);
		memset(pDigitEdit->edit_privattr.digt,0,(pDigitEdit->edit_privattr.containLen+2));
	}
	
	pDigitEdit->disLenBack = pDigitEdit->edit_privattr.len;
	switch(pedit_privattr->font_size)
	{
	case FONTSIZE5X8:
		pDigitEdit->charw = 5;
		pDigitEdit->charh = 8;
		break;
	case FONTSIZE7X11:
		pDigitEdit->charw = 7;
		pDigitEdit->charh = 11;
		break;
	case FONTSIZE12X17:
		pDigitEdit->charw = 12;
		pDigitEdit->charh = 17;
		break;
	case FONTSIZE6X9:
		pDigitEdit->charw = 6;
		pDigitEdit->charh = 9;
		break;
	case FONTSIZE6X5:
	case FONTSIZE6X6:
		pDigitEdit->charw = 6;
		pDigitEdit->charh = 6;
		break;
	case FONTSIZE9x7:
		pDigitEdit->charw = 9;
		pDigitEdit->charh = 7;
		break;
	case FONTSIZE5X7:
		pDigitEdit->charw = 5;
		pDigitEdit->charh = 7;
		break;
	case FONTSIZE8X16:
		pDigitEdit->charw = 8;
		pDigitEdit->charh = 16;		
		break;	
	case FONTSIZE24X24:
		pDigitEdit->charw = 12;
		pDigitEdit->charh = 24;
		break;
	case FONTSIZE32X32:
		pDigitEdit->charw = 16;
		pDigitEdit->charh = 32;
		break;						
	default:
		pDigitEdit->charw = 8;
		pDigitEdit->charh = 16;
		break;			
	}
	
	pDigitEdit->pullflag = 0;
	pDigitEdit->draw = Grap_EditDraw;
	pDigitEdit->handle = Grap_EditHandle;
	pDigitEdit->destroy = Grap_EditDestroy;
	pDigitEdit->recvSysMsg = Grap_EditRecvSysMsg;
	pDigitEdit->get_focus =  Grap_EditGetFocus;
	pDigitEdit->recvPullMsg =  Grap_EditRecvPullMsg;
	
	pDigitEdit->editenter		= Grap_EditEnter;
	pDigitEdit->editKeyInput	= Grap_EditKeyInput;
	pDigitEdit->editKeyEnd		= Grap_EditKeyEnd;
	pDigitEdit->editTimer		= Grap_EditCursorShow;
	
	pDigitEdit->dropListGetStr  = Grap_EditDropListGetStr;
	pDigitEdit->dropListDrawScroll = Grap_EditDropListDrawScroll;
	pDigitEdit->dropListDrawOne    = Grap_EditDropListDrawOne;
	pDigitEdit->DrawInputCnt       = Grap_EditDrawInputCnt;
	pDigitEdit->dropListEnter	   = Grap_EditDropListEnter;
	
	pDigitEdit->dropList.listnum = 4;
	pDigitEdit->dropList.focus   = 0xff;
	pDigitEdit->dropList.high    = pDigitEdit->charh*pDigitEdit->dropList.listnum;
	pDigitEdit->dropList.backColor  = 0xffff;
	pDigitEdit->dropList.focusColor = 0xF800;
	
	return pDigitEdit;
}
