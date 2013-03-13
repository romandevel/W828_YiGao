#include "grap_edit2.h"
#include "grap_event.h"
#include "grap_deskbox.h"

//箭头颜色   0x3186
const U8 UpArrow[16]=//w=16 h=8
{
	0x01,0x80,0x03,0xC0,0x07,0xE0,0x0F,0xF0,0x1F,0xF8,0x3F,0xFC,0x7F,0xFE,0xFF,0xFF
};

const U8 DownArrow[16]=//w=16 h=8
{
	0xFF,0xFF,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x01,0x80
};

/*const U16 Edit2ScrollBarColor[16]=
{
	0xA75F,0x86FF,0x86FF,0x769F,0x661F,0x559F,0x453F,0x34BF,0x1C5E,0x39C,0x39C,0x35A,0x318,0xB7A,
	0x3BB,0x45F 
};*/

//横线取模   颜色Ox632c
const U16 Edit2ScrollBarColor[]=
{
	//0xad55,0xad55,0xa534,0x9cf3,0x9cd3,0x9492,0x8c51,0x8410,0x7bcf,0x73ae,0x6b6d,0x632c,0x5aeb,0x5acb,
	//0x528a,0x4a49,0x4208,0x39e7,0x31a6
	
	/*0x640,0x600,0x5c0,0x580,0x560,0x520,0x4e0,0x4c0,0x480,
    0x440,0x420,0x3e0,0x3a0,0x380,0x340,0x300,0x2e0,0x2a0,
    0x260,*/
    
    //换成歌华风格
    0x867e,0x865e,0x7e1d,0x75fc,0x75dc,0x6dbb,0x657a,0x655a,0x5d3a,
	0x5cf9,0x54d8,0x4cb8,0x4c97,0x4457,0x4436,0x3c16,0x33d5,0x33b5,
	0x2b94 
};

static void moveCursor(void *pView, U16 x, U16 y);

U16 keyValue[10][5] = 
{
	{'@',	':',	'/',	0,	KEYEVENT_1},
	{'a',	'b',	'c',	0,	KEYEVENT_2},
	{'d',	'e',	'f',	0,	KEYEVENT_3},
	{'g',	'h',	'i',	0,	KEYEVENT_4},
	{'j',	'k',	'l',	0,	KEYEVENT_5},
	{'m',	'n',	'o',	0,	KEYEVENT_6},
	{'p',	'q',	'r',	's',KEYEVENT_7},
	{'t',	'u',	'v',	0,	KEYEVENT_8},
	{'w',	'x',	'y',	'z',KEYEVENT_9},
	{',',	'.',	'?',	'!',KEYEVENT_0}
};

U8  Keycount = 0;
U32 Keylasttime = 0;
U16 Keylastkey = 0xff;

/*
 *更新节点信息
 */
static S32 LineInfo_UpdateNode(tCHARPOS **pHead, U16 LineNo, U32 firstCharPos)
{
	tCHARPOS *pSub;
	
	pSub = *pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			pSub->firstCharPos = firstCharPos;
			break;
		}

		pSub = pSub->pNext;
	}
	
	return 0;
}

/*
 *插入一个记录行信息的节点
 */
static S32 LineInfo_InsertNode(tCHARPOS **pHead, tCHARPOS **pTail, U16 LineNo, U32 firstCharPos)
{
	tCHARPOS *pNewNode = NULL, *pSub = NULL;
	
	if(*pTail != NULL && (*pTail)->LineNo >= LineNo)
	{
		LineInfo_UpdateNode(pHead, LineNo, firstCharPos);
	}
	else
	{
		pNewNode = (tCHARPOS *)malloc(sizeof(tCHARPOS));
		if(pNewNode == NULL)
			return -1;//失败
		pNewNode->LineNo = LineNo;
		pNewNode->firstCharPos = firstCharPos;
		pNewNode->pNext = NULL;
		*pTail = pNewNode;
		
		if(*pHead == NULL)
		{
			*pHead = pNewNode;
		}
		else
		{
			pSub = *pHead;
			while(pSub->pNext != NULL)
			{
				pSub = pSub->pNext;
			}
			pSub->pNext = pNewNode;
		}
	}
	
	return 0;
}
/*
 *删除一个记录行信息的节点
 */
static S32 LineInfo_DeleteNode(tCHARPOS **pHead, tCHARPOS **pTail)
{
	tCHARPOS *pTemp;
	
	if(*pHead == NULL)	return 0;
	if(*pHead == *pTail)
	{
		pTemp = *pHead;
		*pHead = NULL;
		*pTail = NULL;
		free((void *)pTemp);
	}
	else
	{
		pTemp = *pHead;
		while(pTemp->pNext != *pTail)
		{
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = NULL;
		free((void *)(*pTail));
		*pTail = pTemp;
	}
	
	return 0;
}

/*
 *查找一个记录行信息的节点
 */
static S32 LineInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos)
{
	tCHARPOS *pSub;
	
	*firstCharPos = 0;
	pSub = pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			*firstCharPos = pSub->firstCharPos;
			break;
		}
		pSub = pSub->pNext;
	}
	
	return 0;
}

/*
 *释放该链表
 */
static void LineInfo_Destroy(tCHARPOS *pHead)
{
	tCHARPOS *pSub, *pTemp;
	
	pSub = pHead;
	while(pSub != NULL)
	{
		pTemp = pSub->pNext;
		free((void *)pSub);
		pSub = pTemp;
	}
	
	return;
}

/*
 *flag:0:直接跳到指定位置 1:下移 -1:上移
 */
int moveBar(tGRAPEDIT2 *pEdit, U16 y, int flag)
{
	tEdit2Scroll *pEdit2Scr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	float temp;
	U16 barH, tempH, linenum;
	
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
	temp = (float)pEdit2Scr->barScrollLen / pedit_ctrl->totLine;//一行应占的高度
	barH = pEdit2Scr->end_y_pos - pEdit2Scr->start_y_pos;//滑块高
	
	if(flag == 0)
	{
		if(y > pEdit2Scr->end_y_pos)//下方
		{
			pEdit2Scr->end_y_pos = y + barH/2;
			if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)
			{
				pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
				pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
			}
			else
			{
				pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
				tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
				linenum = tempH/temp;
				if(linenum+pedit_ctrl->line_screen >= pedit_ctrl->totLine)
	   			{
	   				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
	   			}
	   			else
	   			{
	   				pedit_ctrl->topLine_idx = linenum;
	   			}
			}	
		}
		else //上方
		{
			if(y >= barH/2)
			{
				pEdit2Scr->start_y_pos = y - barH/2;
				if(pEdit2Scr->start_y_pos < pEdit2Scr->bar_start_y)
					pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
			}
			else
			{
				pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
			}
			pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			
			tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
			linenum = tempH/temp;
			pedit_ctrl->topLine_idx = linenum;
		}
		
		return -1;
	}
	else if(flag == 1)//下
	{
		if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)	return -1;

		pEdit2Scr->end_y_pos += y;
		if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)
		{
			pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
			pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
			pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
		}
		else
		{
			pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
			tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
			linenum = tempH/temp;
			if(linenum+pedit_ctrl->line_screen >= pedit_ctrl->totLine)
   			{
   				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
   			}
   			else
   			{
   				pedit_ctrl->topLine_idx = linenum;
   			}
		}
	}
	else//上
	{
		if(pEdit2Scr->start_y_pos <= pEdit2Scr->bar_start_y)	return -1;
		if(pEdit2Scr->start_y_pos > y)
		{
			pEdit2Scr->start_y_pos -= y;
			if(pEdit2Scr->start_y_pos < pEdit2Scr->bar_start_y)
				pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
		}
		else
		{
			pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
		}
		pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			
		tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
		linenum = tempH/temp;
		pedit_ctrl->topLine_idx = linenum;
	}

	return 0;
}

int Grap_Edit2Enter(void *pView)
{
	
	return SUCCESS;
}

S32 Grap_Edit2GetFocus(void *pView)
{
	return SUCCESS;
}

S32 Grap_Edit2RecvPullMsg(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll *pEdit2Scr;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	U16 x,y,lastX, lastY;
	
	y = HIWORD(pEvent->lParam) ;
	x = LOWORD(pEvent->lParam) ;
	lastY = HIWORD(pEvent->wParam) ;
	lastX = LOWORD(pEvent->wParam) ;//上次点
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
	if(pEvent->extParam == VIEW_PULL_START)//开始点
	{
		if(x >= pview_attr->touchX1 && x < (pview_attr->touchX2+pEdit->edit2Scr.bar_end_x) &&
		   y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//在该空间内
		{ 	
			ret = VIEW_PULL_START;
		}
		else
		{
			return FAIL;
		}
	}
	
	if(VIEW_PULL_START == pEvent->extParam)
	{
		if(pEdit->view_attr.FocusEnable)
		{
			if(!pEdit->view_attr.curFocus)
			{
				Grap_ChangeFocusEnable(pEdit);
				pEdit->get_focus(pEdit);
			}
		}
		
		//分区域
		if(x >= pview_attr->touchX1 && x < pview_attr->touchX2 &&
	       y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//写字区
	    {
	    	if(pedit_privattr->cursor_en)
	    	{
		    	moveCursor(pEdit,x,y);
		    	pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
		    }
	    }
	    else if(x >= pEdit->edit2Scr.bar_start_x && x < pEdit->edit2Scr.bar_end_x
			 && y >= pEdit->edit2Scr.arrow1_areaY1 && y < pEdit->edit2Scr.arrow2_areaY2
			 && pEdit->edit2Scr.need == 1)//滚动条
		{
			if(y <= pEdit->edit2Scr.arrow1_areaY2)//上箭头
			{
				if(pedit_ctrl->cursor_line > 0)
					pedit_ctrl->cursor_line-=1;
			}
			else if(y >= pEdit->edit2Scr.arrow2_areaY1)//下箭头
			{
				if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
					pedit_ctrl->cursor_line+=1;
			}
			else//滚动区域
			{
				if(y >= pEdit->edit2Scr.start_y_pos && y<= pEdit->edit2Scr.end_y_pos)
		   		{
		   			return VIEW_PULL_START ;
		   		}
		   		else
		   		{
		   			moveBar(pEdit,y,0);
		   		}
		   		
		   		pedit_ctrl->cursor_line = pedit_ctrl->topLine_idx;
		   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
	   		
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
					
				return VIEW_PULL_START ;
			}
			
			if(pedit_ctrl->cursor_line < pedit_ctrl->topLine_idx)//调整光标
	   		{
	   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
	   		}
	   		else if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
	   		{
	   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line-pedit_ctrl->line_screen+1;
	   		}
	   		
	   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
	   		if(pedit_privattr->cursor_en)
			{
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
			}
		}
	}
	else if (VIEW_PULL_ING == pEvent->extParam)
	{//拖拽中
		if(pedit_ctrl->totLine <= pedit_ctrl->line_screen)
		{
			return FAIL;
		}
		if(x >= pEdit->edit2Scr.bar_start_x && x < pEdit->edit2Scr.bar_end_x
		&& y >= pEdit->edit2Scr.arrow1_areaY1 && y < pEdit->edit2Scr.arrow2_areaY2
		&& pEdit->edit2Scr.need == 1)
		{

			if(lastX >= pEdit->edit2Scr.bar_start_x && lastX < pEdit->edit2Scr.bar_end_x
			&& lastY >= pEdit->edit2Scr.arrow1_areaY1 && lastY < pEdit->edit2Scr.arrow2_areaY2)
			{
				if(lastY <= pEdit->edit2Scr.arrow1_areaY2 && y <= pEdit->edit2Scr.arrow1_areaY2)//上箭头
				{
					if(pedit_ctrl->cursor_line > 0)
						pedit_ctrl->cursor_line-=1;
				}
				else if(lastY >= pEdit->edit2Scr.arrow2_areaY1 && y >= pEdit->edit2Scr.arrow2_areaY1)//下箭头
				{
					if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
						pedit_ctrl->cursor_line+=1;
				}
				else//滚动区域
				{
					int temp,re=-1;
					
					temp = y - lastY;
					
					if(temp > 0)//下
					{
						re = moveBar(pEdit,ABS(temp),1);
					}
					else//上
					{
						re = moveBar(pEdit,ABS(temp),-1);
					}
			   		
			   		if(re == -1)
			   		{
			   			return FAIL;
			   		}
			   		
			   		pedit_ctrl->cursor_line = pedit_ctrl->topLine_idx;
			   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
		   		
					pedit_ctrl->TimerLft = 1;
					pedit_ctrl->showState = 0;
					pEdit->draw(pEdit, 0, 0);
						
					return VIEW_PULL_ING ;
			   	}
			   	
			   	if(pedit_ctrl->cursor_line < pedit_ctrl->topLine_idx)//调整光标
		   		{
		   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
		   		}
		   		else if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
		   		{
		   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line-pedit_ctrl->line_screen+1;
		   		}
		   		
		   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
		   		
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
				
				return VIEW_PULL_ING ;
			}
		} 
		else if(x >= pview_attr->touchX1 && x < (pview_attr->touchX2+pEdit->edit2Scr.bar_end_x) &&
		        y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//在该空间内
		{
			S16 moveP;
			
			moveP = y - lastY;
	
			if(ABS(moveP) >= pedit_ctrl->line_height)
			{
				if(moveP > 0)//向下拖
				{
					if(pedit_ctrl->topLine_idx > 0)
					{
						pedit_ctrl->topLine_idx--;
						if(pedit_privattr->cursor_en)
						{
							pedit_ctrl->cursor_line--;
							LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
							pedit_ctrl->TimerLft = 1;
							pedit_ctrl->showState = 0;
						}
						pEdit->draw(pEdit, 0, 0);
					}
				}
				else//向上拖
				{
					if(pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
					{
						pedit_ctrl->topLine_idx++;
						if(pedit_privattr->cursor_en)
						{
							pedit_ctrl->cursor_line++;
							LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
							pedit_ctrl->TimerLft = 1;
							pedit_ctrl->showState = 0;
						}
						pEdit->draw(pEdit, 0, 0);
					}
				}
				ret = VIEW_PULL_ING;
			}
		}
	}
	else if (VIEW_PULL_END == pEvent->extParam)
	{//up			
		ret = VIEW_PULL_END;
	}
	else if (VIEW_PULL_TIMEOUT == pEvent->extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;
	}
	
	return ret; 
}


/*输入或删除后重绘*/
void Grap_Edit2Rest_AfterInputOrDel(void *pView, Character_Type_e kLycStr_Type)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll *pEdit2Scr;
	U8 *pCurPageStr = NULL;
	U32 curPos, i;
	U32 charContent;//一行能容的字符
	U32 lineContent;//一页能容的行数
	U32 kLineNum, kCharNum, totalCharLen;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
	pCurPageStr = pedit_privattr->digt + curPos;//光标所在行首字符开始的字符串
	
	pedit_ctrl->totLine = pedit_ctrl->cursor_line;//之后每写完一行再加1,并更新line_pos
	
	charContent = pedit_ctrl->char_line;
	lineContent = pedit_ctrl->line_screen;
	
	/*计算出之后字符的总数*/
	totalCharLen = Get_StringLength(kLycStr_Type,pCurPageStr);//字符个数(UNICODE半角的也按2计)

	i = 0;
	if(kLycStr_Type == CHARACTER_UNICODE)
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
		pedit_ctrl->totLine++;//行数加1
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//字符写完
				{
					if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
						LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
					return ;
				}
				if(pCurPageStr[i] == 0x0D && pCurPageStr[i+1] == 0x0A)
				{
					i+=2;
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
						pedit_ctrl->totLine++;//行数加1
						if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
							LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
						return;
					}
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 <= charContent)//全角
				{
					i+=2;
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 > charContent)//全角
				{
					break;
				}
				else//半角
				{
					i+=2;
					curPos += 2;
					kCharNum += 1;
					totalCharLen -= 2;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
			pedit_ctrl->totLine++;//行数加1
		}
	}
	else//本地码
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
		pedit_ctrl->totLine++;//行数加1
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//字符写完
				{
					if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
						LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
					return ;
				}
				if(pCurPageStr[i] == 0x0D && pCurPageStr[i+1] == 0x0A)
				{
					i+=2;
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
						pedit_ctrl->totLine++;//行数加1
						if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
							LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
						return;
					}
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 <= charContent)//全角
				{
					i+=2;
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 > charContent)//全角
				{
					break;
				}
				else//半角
				{
					i+=1;
					curPos += 1;
					kCharNum += 1;
					totalCharLen -= 1;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
			pedit_ctrl->totLine++;//行数加1
		}
	}
	if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
		LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
	
	return ;
}

static U16 Grap_Edit2_MoveCursor(U8 *pStr, U16 len, Character_Type_e kLycStr_Type)
{
	U16 i, j = 0;
	
	for(i=0; i<len; )
	{
		if(pStr[i] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
		{
			i+=2; j = 2;
		}
		else
		{
			i+=1; j = 1;
		}
	}
	
	return j;
}


/*删除光标前的字符*/
S32 Grap_Edit2_DeleteChar(void *pView)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U32 curPos, strLen, tempPos;
	U16 len, movenum = 0;
	U8 *temp = NULL, *pStr = NULL;
	Character_Type_e kLycStr_Type;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	kLycStr_Type = pedit_privattr->font_type;//字符类型
	
	temp = (U8 *)malloc(pedit_privattr->containLen);
	if (NULL == temp) return -1;
	memset(temp, 0, pedit_privattr->containLen);
	
	strcpy(temp, pedit_privattr->digt + pedit_ctrl->cursor_pos);//光标后面的字符
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
	pStr = pedit_privattr->digt + curPos;//光标所在行首字符开始的字符串
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line+1, &tempPos);
	/*要分光标在一行尾部还是在一行头部*/
	if(pedit_ctrl->cursor_pos == tempPos)//光标在行尾部
	{
		len = pedit_ctrl->cursor_pos - curPos;//光标离本行开头的距离
	}
	else if(pedit_ctrl->cursor_pos == curPos)//光标在行头部
	{//此时光标需要换行处理(光标至少在第2行),此时还要判断是否需要翻页
		pedit_ctrl->cursor_line -= 1;//光标上移
		if(pedit_ctrl->topLine_idx > pedit_ctrl->cursor_line)//需要向上翻
		{
			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
		}
		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
		pStr = pedit_privattr->digt + curPos;//光标所在行首字符开始的字符串
		len = pedit_ctrl->cursor_pos - curPos;//光标离本行的距离
	}
	else//光标在行中
	{
		len = pedit_ctrl->cursor_pos - curPos ;//光标离本行的距离
	}
	strLen = Get_StringLength(kLycStr_Type,pStr);//字符个数(UNICODE半角的也按2计)
	
	if(pedit_privattr->digt[pedit_ctrl->cursor_pos-2]==0x0D
		&& pedit_privattr->digt[pedit_ctrl->cursor_pos-1]==0x0A)
	{
		movenum = 2;
	}
	else
	{
		movenum = Grap_Edit2_MoveCursor(pStr,len,kLycStr_Type);
	}
	pedit_ctrl->cursor_pos -= movenum;//光标前移
	strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos,temp);//把之前光标后的字符copy过去
	
	free((void *)temp);
	if(pedit_ctrl->topLine_idx > 0 && pedit_ctrl->topLine_idx+pedit_ctrl->line_screen > pedit_ctrl->cursor_line+1)
	{
		pedit_ctrl->topLine_idx-=1;
	}
	Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
	return 0;
}

/*获取默认的字符串----可重载,现在只是测试用*/
S32 Grap_Edit2GetDefaultStr(void *pView)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	S32 curPos=0,totalCharLen;
	U8  *pCurPageStr;
	U32 charContent;//一行能容的字符
	U32 lineContent;//一页能容的行数
	U32 kLineNum, kCharNum;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	if(pedit_ctrl->pLineInfoH != NULL)
	{
		LineInfo_Destroy(pedit_ctrl->pLineInfoH);
		pedit_ctrl->pLineInfoH = NULL;
		pedit_ctrl->pLineInfoT = pedit_ctrl->pLineInfoH;
	}
	
	kLycStr_Type = pedit_privattr->font_type;//字符类型
	
	totalCharLen = Get_StringLength(kLycStr_Type,pedit_privattr->digt);//字符个数(UNICODE半角的也按2计)

	charContent = pedit_ctrl->char_line;
	lineContent = pedit_ctrl->line_screen;
	pedit_ctrl->topLine_idx = 0;
	pedit_ctrl->totLine = 0;
	pedit_ctrl->cursor_pos = 0;
	pedit_ctrl->cursor_line = 0;
	pCurPageStr = pedit_privattr->digt;
	/*因为是测试,下面只处理本地码*/
	/*2008-0809增加回车处理*/
	if(kLycStr_Type == CHARACTER_UNICODE)
	{
		//......
	}
	else//本地码
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
		pedit_ctrl->totLine++;
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//字符写完
				{
					return 0;
				}
				if(pCurPageStr[curPos] == 0x0D && pCurPageStr[curPos+1] == 0x0A)//回车换行
				{
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
						pedit_ctrl->totLine++;//行数加1
						return;
					}
				}
				else if(pCurPageStr[curPos] == 0x0d || pCurPageStr[curPos] == 0x0a)
				{
					curPos += 1;
					totalCharLen -= 1;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
						pedit_ctrl->totLine++;//行数加1
						return;
					}
				}
				else if(pCurPageStr[curPos] > 0x80 && kCharNum+2 <= charContent)//全角
				{
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[curPos] > 0x80 && kCharNum+2 > charContent)//全角
				{
					break;
				}
				else//半角
				{
					curPos += 1;
					kCharNum += 1;
					totalCharLen -= 1;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
			pedit_ctrl->totLine++;//行数加1
		}
	}
	return 0;
}

S32 Grap_Edit2RecvSysMsg(void *pView)
{

	return SUCCESS;
}
/*****************************************************/
void Grap_Edit2CursorShow(void *pView)
{
	int i ;
	U16 date;
	S32 ret;
	U16 color,x1,y1;
	U32 pos;   //x方向的pos
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U16 font_H, font_W;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
	pos = pedit_ctrl->cursor_pos - pos;
	
	font_H = pedit_ctrl->line_height-4;
	font_W = font_H/2;
	
	y1 = pview_attr->viewY1 + ( (pedit_ctrl->cursor_line-pedit_ctrl->topLine_idx) * pedit_ctrl->line_height) + 1;
	x1 = pview_attr->viewX1 + (pos* font_W)+pedit_privattr->xoffset;
	y1 += pedit_privattr->yoffset;
	if (pedit_privattr->cursor_en)
	{
		if(pedit_ctrl->showState)	
		{
			color = COLOR_BLACK;		
		}
		else//应该用背景色
		{
			color = COLOR_WHITE;		
		}
		Grap_BrushScreen(x1, y1, (U16)(x1+1), (U16)(y1+font_H-1), color);
	}
}

//画指定行
S32 Grap_Edit2DrawOneLine(void *pView, U32 line_idx, U16 x_start, U16 y_start)
{
	tGRAPEDIT2 *pEdit;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tGrapEdit2Priv *pedit_privattr;
	tGrapViewAttr  *pview_attr;
	U32 totalCharLen, pos, tempPos;      //要显示行首字符在字符串中的位置
	tStrDispInfo disp_info;
	Character_Type_e kLycStr_Type;
	U32 curDrawLen;
	U8  *pTempStr;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx, &pos);
	
	if(line_idx == pedit_ctrl->totLine-1)//最后一行
	{
		kLycStr_Type = pedit_privattr->font_type;//字符类型
		totalCharLen = Get_StringLength(kLycStr_Type,(pedit_privattr->digt+pos));//字符个数(UNICODE半角的也按2计)
		curDrawLen = totalCharLen;
	}
	else
	{
		LineInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx+1, &tempPos);
		curDrawLen = tempPos - pos;
	}
	pTempStr = (U8 *)malloc(curDrawLen+2);
	if(NULL == pTempStr) return -1;
	
	memcpy(pTempStr,(U8*)(&pedit_privattr->digt[pos]),curDrawLen);
	pTempStr[curDrawLen] = 0;
	pTempStr[curDrawLen+1] = 0;
	disp_info.string = pTempStr;
	disp_info.color = pedit_privattr->color;
	disp_info.font_size = pedit_privattr->font_size;
	disp_info.font_type = pedit_privattr->font_type;
	disp_info.flag = pedit_privattr->position;
	disp_info.keyStr = NULL;
	disp_info.keyStrColor=COLOR_YELLOW;
	Grap_WriteString(x_start+pedit_privattr->xoffset, y_start+pedit_privattr->yoffset, 
				(U16)(x_start+pview_attr->viewWidth),
				(U16)(y_start + pview_attr->viewHeight), &disp_info);
	free((void *)pTempStr);
	return SUCCESS;
}

// Edit draw function
S32 Grap_Edit2Draw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll   *pEdit2Scr;
	U32 i = 0;
	U32 line;  //一屏显示的行数
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
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
	else
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}
	
	if(1)
	{
		if(pedit_ctrl->line_screen < pedit_ctrl->totLine)//需要画滑动块
		{
			float temp;
			U16 barH, barW;
			U16 row,column,i,j;
			
			pEdit2Scr->need = 1;
			
			/*画滚动条底图*/
			Grap_BrushScreen(pEdit2Scr->bar_start_x, pEdit2Scr->bar_start_y, 
						pEdit2Scr->bar_end_x,pEdit2Scr->bar_end_y, pEdit2Scr->barBGColor);
			/*上箭头底图*/
			Grap_BrushScreen(pEdit2Scr->arrow1_areaX1, pEdit2Scr->arrow1_areaY1, 
						pEdit2Scr->arrow1_areaX2,pEdit2Scr->arrow1_areaY2, pEdit2Scr->barBGColor);
			/*下箭头底图*/
			Grap_BrushScreen(pEdit2Scr->arrow2_areaX1, pEdit2Scr->arrow2_areaY1, 
						pEdit2Scr->arrow2_areaX2,pEdit2Scr->arrow2_areaY2, pEdit2Scr->barBGColor);
			/*上箭头*/
			Grap_WriteBMPColor(pEdit2Scr->arrow1_areaX1,pEdit2Scr->arrow1_areaY1+1,16,8, (U8*)UpArrow, ARROWCOLOR);
			/*下箭头*/
			Grap_WriteBMPColor(pEdit2Scr->arrow2_areaX1,pEdit2Scr->arrow2_areaY1+1,16,8, (U8*)DownArrow, ARROWCOLOR);

			
			//temp = (float)pview_attr->viewHeight / pedit_ctrl->totLine;//一行应占的高度
			temp = (float)pEdit2Scr->barScrollLen / pedit_ctrl->totLine;//一行应占的高度
			//barH = pview_attr->viewHeight - temp*(pedit_ctrl->totLine - pedit_ctrl->line_screen);
			barH = pEdit2Scr->barScrollLen - temp*(pedit_ctrl->totLine - pedit_ctrl->line_screen);
			if(barH <= 2)	barH = 2;
			barW = pEdit2Scr->bar_end_x - pEdit2Scr->bar_start_x;
			pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y + (U16)(pedit_ctrl->topLine_idx*temp);
			pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			if(pEdit2Scr->end_y_pos > pEdit2Scr->bar_end_y)
			{
				pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
				barH = pEdit2Scr->end_y_pos - pEdit2Scr->start_y_pos;
			}
			row  = pEdit2Scr->start_y_pos;
			column = pEdit2Scr->bar_start_x;
			for(i=0; i < barW; i++)
			{
				Memory_SetRowColumn(row, column++, barH, 1);
				
				for(j=0; j < barH; j++)
				{
					if(j == 0 || j == barH-1)	drvLcd_WritePoint(0x632c);
					else drvLcd_WritePoint(Edit2ScrollBarColor[i]);
				}
			}
		}
		else
		{
			pEdit2Scr->need = 0;
		}
	}
	//draw lines  from top line
	for ( i = pedit_ctrl->topLine_idx; i < pedit_ctrl->topLine_idx + pedit_ctrl->line_screen; i++ )
	{
		if ( i < pedit_ctrl->totLine )
		{
			pEdit->edit2DrawOneLine(pView, i, pview_attr->viewX1, 
					pview_attr->viewY1 + (i-pedit_ctrl->topLine_idx)*pedit_ctrl->line_height);
		}
	}
	
	//如果是当前焦点,则在外面画边框
	//Grap_DrawFocusRect(pview_attr, COLOR_RED);
	if(pEdit2Scr->need == 1)
	{
		if(pview_attr->FocusEnable && pview_attr->curFocus)
		{
			drvLcd_SetColor(COLOR_RED,0x0);
			drvLcd_Rectangle(pview_attr->viewY1, pview_attr->viewX1, pview_attr->viewHeight, pview_attr->viewWidth+18, 1);
		}
	}
	else
	{
		if(pview_attr->FocusEnable && pview_attr->curFocus)
		{
			drvLcd_SetColor(COLOR_RED,0x0);
			drvLcd_Rectangle(pview_attr->viewY1, pview_attr->viewX1, pview_attr->viewHeight, pview_attr->viewWidth, 1);
		}
	}
	
	return SUCCESS;
}


static void moveCursor(void *pView, U16 x, U16 y)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	int pos ; //
	U32 new_line; //当前坐标处于的新行
	U32 len, templen; //temp variable
	U8 *pStr;
			
			
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	kLycStr_Type = pedit_privattr->font_type;//字符类型
	
	
	//将y对应到行
	new_line = (y - pview_attr->viewY1) / pedit_ctrl->line_height;//当前页第几行
	if(new_line >= pedit_ctrl->line_screen && pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
	{
		new_line-=1;
		pedit_ctrl->topLine_idx+=1;
	}
	new_line += pedit_ctrl->topLine_idx;//总的第几行
	if(new_line >= pedit_ctrl->totLine)
	{
		if(pedit_ctrl->totLine > 0)
		{
			pedit_ctrl->cursor_line = pedit_ctrl->totLine-1;
			LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
			pStr = pedit_privattr->digt + pos;//最后一行字符
			templen = (U16)Get_StringLength(kLycStr_Type,pStr);
			if(templen>=2 && pStr[templen-2]==0x0D && pStr[templen-1]==0x0A)
				templen-=2;
			pedit_ctrl->cursor_pos = templen + pos;
		}
		else
		{
			pedit_ctrl->cursor_line = 0;
			pedit_ctrl->cursor_pos = 0;
		}

		return ;
	}
	pedit_ctrl->cursor_line = new_line;
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,new_line, &pos);
	pStr = pedit_privattr->digt + pos;
	len = (x - pview_attr->viewX1)/((pedit_ctrl->line_height-4)/2);
	
	if(new_line == pedit_ctrl->totLine-1)//最后一行
	{
		templen = (U16)Get_StringLength(kLycStr_Type,pStr);
	}
	else//中间某行
	{
		LineInfo_FindNode(pedit_ctrl->pLineInfoH,new_line+1, &templen);
		templen -= pos;//该行字符数
	}
	
	if(len > templen)
	{
		if(templen>=2 && pStr[templen-2]==0x0D && pStr[templen-1]==0x0A)
			templen-=2;
		pedit_ctrl->cursor_pos = templen + pos;
	}
	else
	{
		int j=0;
		for(templen=0; templen<len; )
		{
			if(pStr[templen]==0x0D && pStr[templen+1]==0x0A)
			{
				len = templen;
				j=0;
				break;
			}
			else if(pStr[templen]>0x80 && templen+2<=len)
			{
				templen+=2;
				j=0;
			}
			else if(pStr[templen]>0x80 && templen+2>len)
			{
				j = 1;
				break;
			}
			else
			{
				templen+=1;
			}
		}
		len -= j;

		pedit_ctrl->cursor_pos = len + pos;
	}
	
	return;
}


//返回-1: 失败 0:新输入字符  1:应替换之前输入
S8 KeyInputChar(U32 *inputkey)
{
	U8	inputMode;
	S8  ret = -1;
	U32 i,key;
	
	key = *inputkey;
	inputMode = Grap_GetCurDesk_InputMode();
	//hyUsbPrintf("inputMode  == %d \r\n",inputMode);
	if(inputMode == 1 || inputMode == 4)//数字  手写 
	{
		key = key - KEYEVENT_0 + 0x30;
		*inputkey = key;
		return 0;
	}
	
	for(i = 0; i < 10; i++)
	{
		if(keyValue[i][4] == key)
		{
			break;
		}
	}
	if(i == 10)//错误
		return ret;
	
	if(key == Keylastkey && rawtime(NULL)-Keylasttime < 100)//1秒内按的同一个键
	{
		Keycount++;
		if(Keycount >= 4 || keyValue[i][Keycount] == 0)
		{
			Keycount = 0;
		}
		ret = 1;
	}
	else
	{
		Keycount = 0;
		ret = 0;
	}
	Keylastkey  = key;
	Keylasttime = rawtime(NULL);
	
	if(inputMode == 2)//小写字母
	{
		*inputkey = keyValue[i][Keycount];
	}
	else if(inputMode == 3)//大写字母
	{
		if(i >= 1 && i <= 8)//字母 需要转成大写
		{
			*inputkey = keyValue[i][Keycount]-0x20;
		}
		else
		{
			*inputkey = keyValue[i][Keycount];
		}
		hyUsbPrintf("key = %c \r\n", *inputkey);
	}
	
	return ret;
}

U32 Grap_Edit2KeyInput(void *pView, U32 *keycode)
{
	
	tGRAPEDIT2		*pEdit;
	tGrapViewAttr  	*pview_attr;
	tGrapEdit2Priv 	*pedit_privattr;
	tGRAPEDIT2CTRL 	*pedit_ctrl;
	U32 i,key;
	S8  ret;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);	
	
	key = *keycode;
	
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
		ret = KeyInputChar(keycode);
		if(ret == -1)//错误
		{
			return PARSED;
		}
		else if(ret == 0)//第一次输入
		{
			return SUCCESS;
		}
		else
		{
			pedit_privattr->digt[pedit_ctrl->cursor_pos-1] = *keycode;
			pEdit->draw(pEdit,0,0);
			return FAIL;
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
		//Grap_ChangeFocus(pEdit,0);
		/*if(pedit_ctrl->cursor_line > 0)
			pedit_ctrl->cursor_line-=1;
   		
   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
   		if(pedit_privattr->cursor_en)
		{
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;
			//pEdit->draw(pEdit, 0, 0);
		}*/
		
		return FAIL;
		break;
	case KEYEVENT_DOWN:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		
		/*if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
			pedit_ctrl->cursor_line+=1;
   		
   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
   		if(pedit_privattr->cursor_en)
		{
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;
			//pEdit->draw(pEdit, 0, 0);
		}*/
		//hyUsbPrintf("edit2 cursor_line = %d \r\n",pedit_ctrl->cursor_line);
		//Grap_ChangeFocus(pEdit,1);
		return FAIL;
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
		if(pedit_ctrl->cursor_pos > 0)//光标前有字符,可以进行删除
		{
			Grap_Edit2_DeleteChar(pEdit);
		
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);
		}
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		break;
	default:
		Keylastkey  = 0xff;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	}
	
	return FAIL;
}

// default handle
S32 Grap_Edit2Handle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	S32 len ,ret = SUCCESS;
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	kLycStr_Type = pedit_privattr->font_type;//字符类型
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	if(message == VIEW_PRESS && pview_attr->curFocus && pEdit->edit2KeyInput != NULL)
	{
		U32 inputret;
		
		inputret = pEdit->edit2KeyInput(pEdit,&lParam);
		if(PARSED == inputret)
		{
			pEdit->draw(pEdit,0,0);
			return PARSED;
		}
		else if(inputret == FAIL)
		{
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
		pEdit->get_default_str(pView);
		break;
	case VIEW_DRAW:
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_INPUT: //在光标所在处插入一个字符
	case VIEW_DIGIT:
		len = strlen(pedit_privattr->digt);
		if (len < pedit_privattr->containLen)
		{
			U8 *temp = NULL;
			U8 *pStr = NULL;
			U32 pos = 0, num = 0, i;
			U8 hCode = 0, lCode = 0;

			lCode = (U8)(lParam&0xff) ;
			hCode = (U8)((lParam>>8)&0xff) ;
			
			temp = (U8 *)malloc(pedit_privattr->containLen);
			if (!temp) break;
			memset(temp, 0, pedit_privattr->containLen);

			strcpy(temp, pedit_privattr->digt + pedit_ctrl->cursor_pos);//光标后面的字符
			
			if(hCode == 0x0D && lCode == 0x0A)//回车换行
			{
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = hCode;
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = lCode;
				
				strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos, temp);
				free((void *)temp);

				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
				Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;	
				pEdit->draw(pEdit, 0, 0);
				
				break;
			}
			//要考虑以下情况，光标所在处是不是最后一个，能不能放下一个汉字，根据这些判断光标是否换行，
			//若光标行不是最后一行，则后面的每行首字符都要进行处理
			LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
			pStr = pedit_privattr->digt + pos;  //当前光标行首字符
			pos = pedit_ctrl->cursor_pos - pos; //光标相对于行首的距离
			
			pos = pedit_ctrl->char_line - pos;//本行光标后面能容的字符数
			if(pos == 0)
			{
				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
			}
			else if(pos == 1 && hCode > 0)
			{
				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
			}

			if(hCode)
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = hCode;
			pedit_privattr->digt[pedit_ctrl->cursor_pos++] = lCode;
			strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos, temp);
			free((void *)temp);

			Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);			
		}
		break;
	case VIEW_DELETE:
		if(pedit_ctrl->cursor_pos > 0)//光标前有字符,可以进行删除
		{
			Grap_Edit2_DeleteChar(pEdit);
		
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_KEY_MSG:
	case VIEW_PRESS:
		if(pEdit->view_attr.curFocus == 1 || pEvent->reserve == 1)//焦点时响应OK键
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				if(cKEYB_EVENT_RELEASE == wParam)
				{
					ret = pEdit->edit2Enter(pEdit);
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
				}
				return ret;
				break;
			case KEYEVENT_DELETE:
				if(pedit_ctrl->cursor_pos > 0)//光标前有字符,可以进行删除
				{
					Grap_Edit2_DeleteChar(pEdit);
				
					pedit_ctrl->TimerLft = 1;
					pedit_ctrl->showState = 0;	
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_UP:
				if(pedit_ctrl->topLine_idx > 0)
				{
					pedit_ctrl->topLine_idx--;
					if(pedit_privattr->cursor_en)
					{
						pedit_ctrl->cursor_line--;
						LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
						pedit_ctrl->TimerLft = 1;
						pedit_ctrl->showState = 0;
					}
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_DOWN:
				if(pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
				{
					pedit_ctrl->topLine_idx++;
					if(pedit_privattr->cursor_en)
					{
						pedit_ctrl->cursor_line++;
						LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
						pedit_ctrl->TimerLft = 1;
						pedit_ctrl->showState = 0;
					}
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_DELETEALL:
		if(pedit_ctrl->pLineInfoH != NULL)
		{
			LineInfo_Destroy(pedit_ctrl->pLineInfoH);
			pedit_ctrl->pLineInfoH = NULL;
			pedit_ctrl->pLineInfoT = pedit_ctrl->pLineInfoH;
		}
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
		pedit_ctrl->topLine_idx = 0;
		pedit_ctrl->totLine = 0;
		pedit_ctrl->cursor_pos = 0;
		pedit_ctrl->cursor_line = 0;
		
		pedit_ctrl->TimerLft = pedit_privattr->TimerTot;
		break;
	case VIEW_TIMER:
		if (pview_attr->curFocus == 0) break; 

		if ( pedit_ctrl->TimerLft == 0 )
		{
			pedit_ctrl->TimerLft = pedit_privattr->TimerTot;
			pedit_ctrl->showState = (1 - pedit_ctrl->showState);
			Grap_Edit2CursorShow(pEdit);			
		}
		else
		{
			S32 timeleft;
			timeleft =  pedit_ctrl->TimerLft - lParam;
			
			if (timeleft < 0)  pedit_ctrl->TimerLft = 0;
			else pedit_ctrl->TimerLft = timeleft;
		}
		break;
	case VIEW_CLICK:
		break;
	case VIEW_REV_PULLMSG:
		ret = pEdit->recvPullMsg(pEdit,pEvent);
		break;
	default:
		break;
	}

	return ret;
}

S32 Grap_Edit2Destroy(void *pView)
{
	tGRAPEDIT2 *pEdit;
	
	pEdit = (tGRAPEDIT2*)pView;
	
	if(0 == pEdit->edit_privattr.mallocFlag &&
			pEdit->edit_privattr.digt != NULL )
		free((void*)(pEdit->edit_privattr.digt));
	if(pEdit->edit_ctrl.pLineInfoH != NULL)
	{
		LineInfo_Destroy(pEdit->edit_ctrl.pLineInfoH);
		pEdit->edit_ctrl.pLineInfoH = NULL;
		pEdit->edit_ctrl.pLineInfoT = NULL;
	}
	
	free((void*)pEdit);
	return SUCCESS;
}

tGRAPEDIT2 *Grap_InsertEdit2(void *pView, tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor)
{
	tGRAPDESKBOX *pParentView;
	tGRAPEDIT2 *pEdit;
	U32 lineNum, font_H, font_W;
	
	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pEdit = (tGRAPEDIT2 *)malloc(sizeof(tGRAPEDIT2));
	if (!pEdit)
	{
		return NULL;
	}
	memset(pEdit, 0, sizeof(tGRAPEDIT2));
	pEdit->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pEdit;
	pParentView->pTail = (tGRAPVIEW *)pEdit;
	pEdit->pParent = pParentView;
	
	//pEdit->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pEdit;
	//pEdit->pParent = pParentView;
	
	pEdit->view_attr = *pview_attr;
	pEdit->edit_privattr = *pedit_privattr;
	
	pEdit->view_attr.reserve |= 0xFF;
	
	if(pEdit->edit_privattr.containLen == 0)//如果没有赋值,取默2k
		pEdit->edit_privattr.containLen = 2048;
	else if(pEdit->edit_privattr.containLen > EDIT2_DATA_SIZE)
		pEdit->edit_privattr.containLen = EDIT2_DATA_SIZE;
	if(0 == pEdit->edit_privattr.mallocFlag)
	{
		pEdit->edit_privattr.digt = (U8 *)malloc(pEdit->edit_privattr.containLen+2);
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
	}
	if(pEdit->edit_privattr.font_size == FONTSIZE24X24)
	{
		font_H = 24;
		font_W = 12;//单字节字符宽
	}
	else if(pEdit->edit_privattr.font_size == FONTSIZE32X32)
	{
		font_H = 32;
		font_W = 16;
	}
	else
	{
		font_H = 16;
		font_W = 8;
	}
	
	pEdit->edit_ctrl.line_height = font_H+4;
	pEdit->edit_ctrl.line_screen = pEdit->view_attr.viewHeight/pEdit->edit_ctrl.line_height;  //一屏能容纳的行数
	pEdit->edit_ctrl.char_line = pEdit->view_attr.viewWidth / font_W;  //一行能容纳的字符数
	
	pEdit->edit_ctrl.topLine_idx = 0;
	pEdit->edit_ctrl.totLine = 0;
	pEdit->edit_ctrl.cursor_pos = 0;
	pEdit->edit_ctrl.cursor_line = 0;
	
	pEdit->edit_ctrl.TimerLft = pedit_privattr->TimerTot;

	pEdit->edit_ctrl.pLineInfoH = NULL;
	pEdit->edit_ctrl.pLineInfoT = pEdit->edit_ctrl.pLineInfoH;
	
	pEdit->edit2Scr.bar_start_x	= pEdit->view_attr.viewX1 + pEdit->view_attr.viewWidth;//滚动条不包括在编辑框中
	pEdit->edit2Scr.bar_end_x	= pEdit->edit2Scr.bar_start_x + SCROLLBARWIDTH/*font_W*2*/;
	
	pEdit->edit2Scr.arrow1_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow1_areaY1= pEdit->view_attr.viewY1;
	pEdit->edit2Scr.arrow1_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow1_areaY2= pEdit->edit2Scr.arrow1_areaY1+SCROLLBARHEIGHT;

	pEdit->edit2Scr.arrow2_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow2_areaY2= pEdit->view_attr.viewY1 + pEdit->view_attr.viewHeight;
	pEdit->edit2Scr.arrow2_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow2_areaY1= pEdit->edit2Scr.arrow2_areaY2-SCROLLBARHEIGHT;
	
	pEdit->edit2Scr.barScrollLen= pEdit->view_attr.viewHeight-2*SCROLLBARHEIGHT-2;
	pEdit->edit2Scr.bar_start_y	= pEdit->edit2Scr.arrow1_areaY2+1;
	pEdit->edit2Scr.bar_end_y	= pEdit->edit2Scr.bar_start_y+pEdit->edit2Scr.barScrollLen;
	
	pEdit->edit2Scr.start_y_pos	= pEdit->edit2Scr.bar_start_y;
	pEdit->edit2Scr.end_y_pos	= pEdit->edit2Scr.bar_start_y;
	
	pEdit->edit2Scr.need	= 0;//默认不需要画滚动快
	pEdit->edit2Scr.barBGColor	= 0xB596;//barBgColor;//

	pEdit->draw = Grap_Edit2Draw;
	pEdit->handle = Grap_Edit2Handle;
	pEdit->destroy = Grap_Edit2Destroy;
	pEdit->recvSysMsg = Grap_Edit2RecvSysMsg;
	pEdit->get_default_str = Grap_Edit2GetDefaultStr;
	pEdit->recvPullMsg = Grap_Edit2RecvPullMsg;
	pEdit->get_focus    = Grap_Edit2GetFocus;
	pEdit->edit2KeyInput= Grap_Edit2KeyInput;
	
	pEdit->edit2Enter = Grap_Edit2Enter;
	pEdit->edit2DrawOneLine = Grap_Edit2DrawOneLine;
	
	return pEdit;
}

tGRAPEDIT2 *Grap_CreateEdit2(tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor)
{
	tGRAPEDIT2 *pEdit;
	U32 lineNum, font_H, font_W;
	
	pEdit = (tGRAPEDIT2 *)malloc(sizeof(tGRAPEDIT2));
	if (!pEdit)
	{
		return NULL;
	}
	memset(pEdit, 0, sizeof(tGRAPEDIT2));
	
	pEdit->view_attr = *pview_attr;
	pEdit->edit_privattr = *pedit_privattr;
	
	pEdit->view_attr.reserve |= 0xFF;
	
	if(pEdit->edit_privattr.containLen == 0)//如果没有赋值,取默2k
		pEdit->edit_privattr.containLen = 2048;
	else if(pEdit->edit_privattr.containLen > EDIT2_DATA_SIZE)
		pEdit->edit_privattr.containLen = EDIT2_DATA_SIZE;

	if(0 == pEdit->edit_privattr.mallocFlag)
	{
		pEdit->edit_privattr.digt = (U8 *)malloc(pEdit->edit_privattr.containLen+2);
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
	}

	if(pEdit->edit_privattr.font_size == FONTSIZE24X24)
	{
		font_H = 24;
		font_W = 12;//单字节字符宽
	}
	else if(pEdit->edit_privattr.font_size == FONTSIZE32X32)
	{
		font_H = 32;
		font_W = 16;
	}
	else
	{
		font_H = 16;
		font_W = 8;
	}
	
	pEdit->edit_ctrl.line_height = font_H+4;
	pEdit->edit_ctrl.line_screen = pEdit->view_attr.viewHeight/pEdit->edit_ctrl.line_height;  //一屏能容纳的行数
	pEdit->edit_ctrl.char_line = pEdit->view_attr.viewWidth / font_W;  //一行能容纳的字符数
	
	pEdit->edit_ctrl.topLine_idx = 0;
	pEdit->edit_ctrl.totLine = 0;
	pEdit->edit_ctrl.cursor_pos = 0;
	pEdit->edit_ctrl.cursor_line = 0;
	
	pEdit->edit_ctrl.TimerLft = pedit_privattr->TimerTot;

	pEdit->edit_ctrl.pLineInfoH = NULL;
	pEdit->edit_ctrl.pLineInfoT = pEdit->edit_ctrl.pLineInfoH;
	
	pEdit->edit2Scr.bar_start_x	= pEdit->view_attr.viewX1 + pEdit->view_attr.viewWidth;//滚动条不包括在编辑框中
	pEdit->edit2Scr.bar_end_x	= pEdit->edit2Scr.bar_start_x + SCROLLBARWIDTH/*font_W*2*/;
	
	pEdit->edit2Scr.arrow1_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow1_areaY1= pEdit->view_attr.viewY1;
	pEdit->edit2Scr.arrow1_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow1_areaY2= pEdit->edit2Scr.arrow1_areaY1+SCROLLBARHEIGHT;

	pEdit->edit2Scr.arrow2_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow2_areaY2= pEdit->view_attr.viewY1 + pEdit->view_attr.viewHeight;
	pEdit->edit2Scr.arrow2_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow2_areaY1= pEdit->edit2Scr.arrow2_areaY2-SCROLLBARHEIGHT;
	
	pEdit->edit2Scr.barScrollLen= pEdit->view_attr.viewHeight-2*SCROLLBARHEIGHT-2;
	pEdit->edit2Scr.bar_start_y	= pEdit->edit2Scr.arrow1_areaY2+1;
	pEdit->edit2Scr.bar_end_y	= pEdit->edit2Scr.bar_start_y+pEdit->edit2Scr.barScrollLen;
	
	pEdit->edit2Scr.start_y_pos	= pEdit->edit2Scr.bar_start_y;
	pEdit->edit2Scr.end_y_pos	= pEdit->edit2Scr.bar_start_y;
	
	pEdit->edit2Scr.need	= 0;//默认不需要画滚动快
	pEdit->edit2Scr.barBGColor	= barBgColor;//0xB596

	pEdit->draw = Grap_Edit2Draw;
	pEdit->handle = Grap_Edit2Handle;
	pEdit->destroy = Grap_Edit2Destroy;
	pEdit->recvSysMsg = Grap_Edit2RecvSysMsg;
	pEdit->get_default_str = Grap_Edit2GetDefaultStr;
	pEdit->recvPullMsg = Grap_Edit2RecvPullMsg;
	pEdit->get_focus    = Grap_Edit2GetFocus;
	pEdit->edit2KeyInput= Grap_Edit2KeyInput;
	
	pEdit->edit2Enter = Grap_Edit2Enter;
	pEdit->edit2DrawOneLine = Grap_Edit2DrawOneLine;
	
	return pEdit;
}
