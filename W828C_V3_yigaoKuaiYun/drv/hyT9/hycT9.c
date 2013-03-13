#include "hycT9.h"
#include "glbVariable.h"
#include "grap_view.h"
#include "grap_deskbox.h"
#include "grap_api.h"
#include "AbstractFileSystem.h"

/*--------------------------------------------------------------------------*/

S8			 gu8T9HzPageUp;	//汉字需要向上翻页  0: 不动作   1:向上翻
U8			 gu8T9DisHzMax;	//本次该行可以显示汉字或词组的最大数目

S8			 gu8T9PyPageUp;	//
U8			 gu8T9DisPyMax;	//

U8		     gu8T9CurFocus;	//当前焦点项  0:拼音  1:汉字


//tGRAPLABEL	 *gT9pyLable[T9_DIS_PY_MAX], *gT9hzLable[T9_DIS_HZ_MAX], *gT9ArrowLable[2];
//tGRAPLABEL	 *gT9SignLable[T9_SIGN_DIS_MAX];

//tGRAPLABEL	 *gT9pyLable_Bak[3][T9_DIS_PY_MAX], *gT9hzLable_Bak[3][T9_DIS_HZ_MAX], *gT9ArrowLable_Bak[3][2];
//tGRAPLABEL	 *gT9SignLable_Bak[3][T9_SIGN_DIS_MAX];

const U8 T9_NumBmp[][16] = //1,2,3  8X16
{
	0x00,0x00,0x00,0x10,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,
	0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x04,0x04,0x08,0x10,0x20,0x42,0x7E,0x00,0x00,
	0x00,0x00,0x00,0x3C,0x42,0x42,0x04,0x18,0x04,0x02,0x02,0x42,0x44,0x38,0x00,0x00,
	0x00,0x00,0x00,0x04,0x0C,0x14,0x24,0x24,0x44,0x44,0x7E,0x04,0x04,0x1E,0x00,0x00,
	0x00,0x00,0x00,0x7E,0x40,0x40,0x40,0x58,0x64,0x02,0x02,0x42,0x44,0x38,0x00,0x00,
	0x00,0x00,0x00,0x1C,0x24,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x24,0x18,0x00,0x00,
	0x00,0x00,0x00,0x7E,0x44,0x44,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
	0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00,
	0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x26,0x1A,0x02,0x02,0x24,0x38,0x00,0x00
};

const U8 T9_FrontBmp[][16] =
{
	0x00,0x00,0x00,0x40,0x00,0xC0,0x01,0xC0,0x03,0xC0,0x07,0xC0,0x0F,0xC0,0x1F,0xC0,
	0x0F,0xC0,0x07,0xC0,0x03,0xC0,0x01,0xC0,0x00,0xC0,0x00,0x40,0x00,0x00,0x00,0x00,
};

const U8 T9_BackBmp[][16] =
{
	0x00,0x00,0x04,0x00,0x06,0x00,0x07,0x00,0x07,0x80,0x07,0xC0,0x07,0xE0,0x07,0xF0,
	0x07,0xE0,0x07,0xC0,0x07,0x80,0x07,0x00,0x06,0x00,0x04,0x00,0x00,0x00,0x00,0x00
};


//这里只是相对偏移
//一行最多显示T9_DIS_PY_MAX个拼音,总数超过时在右侧显示箭头   
const U8 t9_py_pos[T9_DIS_PY_MAX][4] = //y1 x1 y2 x2
{
	{T9_PY_YOFF,		0,		T9_PY_YOFF+T9_PY_H,	40},
	{T9_PY_YOFF,		50,		T9_PY_YOFF+T9_PY_H,	90},
	{T9_PY_YOFF,		100,	T9_PY_YOFF+T9_PY_H,	140},
	{T9_PY_YOFF,		150,	T9_PY_YOFF+T9_PY_H,	190}
};

//一屏最多显示T9_DIS_HZ_MAX个汉字     如果需要显示词组时,需要动态调整宽度
const U8 t9_hz_pos[T9_DIS_HZ_MAX][4] = //y1 x1 y2 x2
{
	{T9_HZ_YOFF,	0,		T9_HZ_YOFF+T9_HZ_H,		33},
	{T9_HZ_YOFF,	43,		T9_HZ_YOFF+T9_HZ_H,		76},
	{T9_HZ_YOFF,	86,		T9_HZ_YOFF+T9_HZ_H,		119},
	{T9_HZ_YOFF,	129,	T9_HZ_YOFF+T9_HZ_H,		162},
	{T9_HZ_YOFF,	172,	T9_HZ_YOFF+T9_HZ_H,		205},
	{T9_HZ_YOFF,	215,	T9_HZ_YOFF+T9_HZ_H,		248}
};



#define T9_PY_DISP_Y		260
#define T9_HZ_DISP_Y		290

const tGrapViewAttr t9_ViewAttr[]=
{
	//特殊的ID号，避免重复
	{0xFFEE, T9_DISPLAY_X,T9_DISPLAY_Y1,T9_DISPLAY_W,T9_DISPLAY_H, 0,0,0,0,0xffff,0,0,1, 0,0,0},
	
	//拼音
	{T9_PY_LABLE1,	0,/*T9_PY_DISP_Y*/0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE2,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE3,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE4,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE5,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE6,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE7,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE8,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE9,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE10,	0,0,0,T9_PY_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	
		
	//汉字
	{T9_PY_LABLE11,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE12,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE13,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE14,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE15,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE16,	0,0,0,T9_HZ_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	
	//右侧箭头
	{T9_PY_LABLE11,	T9_ARROWUP_X,	T9_ARROW_YOFF,  T9_ARROW_W,T9_ARROW_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	{T9_PY_LABLE12,	T9_ARROWDOWN_X, T9_ARROW_YOFF,	T9_ARROW_W,T9_ARROW_H, 0,0,0,0, 0xffff,0,0,1, 0,0,0},
	
	//符号
	{T9_PY_LABLE15,	0,0,T9_SIGN_W,T9_SIGN_H, 0,0,0,0, 0xffff,0,0,0, 0,0,0},
	
};

const tGrapLabelPriv t9_LabPri[]=
{
	{NULL,T9_UNSELECT_COLOR, T9_SELECT_COLOR, FONTSIZE24X24, 0, 2, 0,0},//拼音用
	{NULL,T9_UNSELECT_COLOR, T9_SELECT_COLOR, FONTSIZE24X24, 0, 0, 0,0},//汉字用
};

void t9_check_py_down(tT9KEY *pT9Key);
void t9_check_hz_down(tT9KEY *pT9Key);


///////////////////////////////////////////////////////////////////////////////////////////////////
/*************************************
* 函数: t9_loadbmp
* 功能: 加载汉字bmp数据
* 参数: pBuf:外部提供内存(60K)
* 返回: 0
*************************************/
int t9_loadbmp(U8 *pBuf)
{
	return 0;
}

/*************************************
* 函数: t9_savebmp
* 功能: 保存汉字bmp数据
* 参数: none
* 返回: 0
*************************************/
int t9_savebmp()
{
	return 0;
}

/*************************************
* 函数: t9_init
* 功能: 初始化
* 参数: 
* 返回: 1
*************************************/
int t9_init()
{
	InputRange_Init();
    
	return 1;
}

/*************************************
* 函数: t9_deinit
* 功能: 释放
* 参数: 
* 返回: 1
*************************************/
int t9_deinit()
{
	InputRange_CloseFd();
    
	return 1;
}

/*************************************
* 函数: t9_clear
* 功能: 部分全局变量清空
* 参数: none
* 返回: none
*************************************/
static void t9_clear(tT9KEY *pT9Key)
{	
	pT9Key->pystart	= 0;
	pT9Key->pytotal	= 0;
	pT9Key->pyselect= 0;

	pT9Key->hzstart	= 0;
	pT9Key->hOptal = 0;
	pT9Key->hzselect= 0;
	
	pT9Key->sistart	= 0;
	pT9Key->sitotal = 0;
	pT9Key->siselect= 0;
	
	pT9Key->assnflag = 0;
	pT9Key->signflag = 0;
	
	gu8T9DisPyMax = 0;
	gu8T9DisHzMax = 0;
	
	return ;
}



/*************************************
* 函数: t9_return_word
* 功能: 返回选中的汉字
* 参数: 
* 返回: 0
*************************************/
int t9_return_word(void *pView, U8 *pWord)
{
	tGRAPDESKBOX *pDesk;
	tGRAPVIEW	 *pFocus;
	GRAP_EVENT	 event;
	U16          i,len,id,word;	

	pDesk = (tGRAPDESKBOX *)pView;
	
	pFocus = (tGRAPVIEW *)Grap_GetCurFocus(pDesk, NULL);
	if(pFocus)
	{
		id = pFocus->view_attr.id;
		if(id >= pDesk->editMinId && id <= pDesk->editMaxId)
		{
			len = strlen(pWord);//判断是半角还是全角
			for(i = 0; i < len; i++)
			{
				if(pWord[i] > 0x80)//汉字
				{
					word = (pWord[i]<<8)|pWord[i+1];
					i++;
				}
				else
				{
					word = pWord[i];
				}
				
				event.id = id;
				event.message = VIEW_DIGIT;
				event.wParam = 0;
				event.lParam = word;
				
				Grap_SendMessage(pFocus, &event);
			}
		}
	}
	
	return 0;
}


void disable_allShowBtn(tT9KEY *pT9Key)
{
	int i;

	for (i=0; i<T9_DIS_HZ_MAX; i++)
	{
		pT9Key->pT9hzLable[i]->view_attr.enable = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchX1 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchY1 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchX2 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchY2 = 0;
	}
	
	for (i=0; i<T9_DIS_PY_MAX; i++)
	{
		pT9Key->pT9pyLable[i]->view_attr.enable = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchX1 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchY1 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchX2 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchY2 = 0;
	}
	
	pT9Key->pT9ArrowLable[0]->view_attr.enable = 0;
	pT9Key->pT9ArrowLable[1]->view_attr.enable = 0;
}

static void com_check_hz(tT9KEY *pT9Key)
{
	int i, len, tot=0;
	
	i = 0;
	len = FRONT_ARROW;
	
	//hyUsbPrintf("hOptal=%d\r\n", pT9Key->hOptal);
	
	while(i < pT9Key->hOptal)
	{
		len += 12*strlen(pT9Key->result.HanZi[i])+T9_HZ_STEP;
		if (len <= BACK_ARROW)
		{
			tot++;
		}
		else
		{
			break;
		}
		i++;
	}
	gu8T9DisHzMax = tot;
	
	//hyUsbPrintf("HzMax=%d\r\n", gu8T9DisHzMax);
}

/*************************************
* 函数: t9_gethz
* 功能: 根据拼音得到汉字
* 参数: pPinyi
* 返回: 0:成功   -1:失败
*************************************/
static int t9_gethz(tT9KEY *pT9Key, U8 *pPinyi)
{
	InputRange_GetExactlyWord(pPinyi,&pT9Key->result);
	pT9Key->hOptal = pT9Key->result.HZ_Num;
	com_check_hz(pT9Key);
	
	return 0;
}


/*************************************
* 函数: t9_inputkey
* 功能: 记录输入的按键值
* 参数: key:输入的按键值 2--9
* 返回: 0:成功   -2:按键已超过6个 -1:没有可以组合的拼音
*************************************/
static int t9_inputkey(tT9KEY *pT9Key, U8 key)
{
	int ret = 0;
	int i;
	
	if(pT9Key->keynum >= MAX_CODE_NUM)
	{
		ret = -2;
	}
	else
	{
		pT9Key->keynum++;
		InputRange_GetWord(key, &pT9Key->result);
		InputRange_GetExactlyWord(pT9Key->result.PinYing[0], &pT9Key->result);
		
		t9_clear(pT9Key);
		
		pT9Key->pytotal = pT9Key->result.PY_Num;
		pT9Key->hOptal = pT9Key->result.HZ_Num;
		
		t9_check_py_down(pT9Key);
		t9_check_hz_down(pT9Key);
	}
	
	return ret;
}

/*************************************
* 函数: t9_deletekey
* 功能: 删除最后输入的一个按键
* 参数: none
* 返回: 0:成功   -2:没有按键可删除  -1:没有可以组合的拼音
*************************************/
static int t9_deletekey(tT9KEY *pT9Key)
{
	int ret = 0;
	int i;
	
	if(pT9Key->keynum > 0)
	{
		if(pT9Key->keynum == 0)
		{
			ret = -2;
		}
		else
		{
			pT9Key->keynum--;
			
			InputRange_DeleteOneKey(&pT9Key->result);
					
			t9_clear(pT9Key);
			
			pT9Key->pytotal = pT9Key->result.PY_Num;
			pT9Key->hOptal = pT9Key->result.HZ_Num;
			
			t9_check_py_down(pT9Key);
			t9_check_hz_down(pT9Key);
			
			if(pT9Key->keynum == 0)
			{
				ret = -2;
			}
		}
	}
	else
	{
		ret = -2;
	}
	
	return ret;
}

/*************************************
* 函数: t9_draw_arrow_back
* 功能: 在显示屏上显示箭头
* 参数: flag:  1:拼音后箭头  2:汉字后箭头   3:两个都画
* 返回: 0
*************************************/
int t9_draw_arrow_back(U8 flag, U16 row)
{
	if (flag == 1 || flag == 3)
	{
		Grap_WriteBMPColor(BACK_ARROW, row, T9_ARROW_W, T9_ARROW_H, T9_BackBmp, 0xffff);
	}
	
	if (flag == 2 || flag == 3)
	{
		Grap_WriteBMPColor(BACK_ARROW, row, T9_ARROW_W, T9_ARROW_H, T9_BackBmp, 0xffff);
	}
	
	return 0;
}

/*************************************
* 函数: t9_draw_arrow_front
* 功能: 在显示屏上显示箭头
* 参数: flag:  1:拼音前箭头  2:汉字前箭头   3:两个都画
* 返回: 0
*************************************/
int t9_draw_arrow_front(U8 flag, U16 row)
{
	if (flag == 1 || flag == 3)
	{
		Grap_WriteBMPColor(0, row, T9_ARROW_W, T9_ARROW_H, T9_FrontBmp, 0xffff);
	}
	
	if (flag == 2 || flag == 3)
	{
		Grap_WriteBMPColor(0, row, T9_ARROW_W, T9_ARROW_H, T9_FrontBmp, 0xffff);
	}
	
	return 0;
}

void t9_check_py_down(tT9KEY *pT9Key)
{
	int i, len, tot=0;
	
	i = pT9Key->pystart + gu8T9DisPyMax;
	pT9Key->pystart  = i;
	pT9Key->pyselect = i;
	len = FRONT_ARROW;
	
	while(i < pT9Key->pytotal)
	{
		len += 12*strlen(pT9Key->result.PinYing[i])+T9_PY_STEP;
		
		//hyUsbPrintf("len=%d   i=%d\r\n", len, i);
		
		if (len <= BACK_ARROW)
		{
			tot++;
		}
		else
		{
			break;
		}
		i++;
	}
	
	gu8T9DisPyMax = tot;
}

void t9_check_py(tT9KEY *pT9Key)
{
	U16 i,j, len;
	
	if(gu8T9PyPageUp)//向上翻页
	{
		len = FRONT_ARROW;
		i = 0;
		while(i < pT9Key->pystart)
		{
			for (j=i; j<pT9Key->pystart; j++)
			{
				len += 12*strlen(pT9Key->result.PinYing[j])+T9_PY_STEP;
			}
			if (len <= BACK_ARROW)
			{
				break;
			}
			i++;
			len = FRONT_ARROW;
		}
		
		if(gu8T9PyPageUp == 2)//通过左移导致
		{
			pT9Key->pyselect--;
		}
		else
		{
			pT9Key->pyselect = i;
		}
		
		gu8T9PyPageUp = 0;
		gu8T9DisPyMax = pT9Key->pystart - i;
		pT9Key->pystart = i;
	}
}

/*************************************
* 函数: t9_draw_py
* 功能: 在显示屏上显示拼音组合
* 参数: none
* 返回: 0
*************************************/
int t9_draw_py(tT9KEY *pT9Key, U16 row)
{
	tStrDispInfo disp_info;
	U8  i,flag = 0;
	U16 x1,y1,x2,y2,len;
	
	//hyUsbPrintf("draw pinying MaxPy=%d  start=%d\r\n", gu8T9DisPyMax, pT9Key->pystart);
	
	for (i=0; i<T9_DIS_PY_MAX; i++)
	{
		pT9Key->pT9pyLable[i]->view_attr.enable = 0;
		pT9Key->pT9pyLable[i]->view_attr.viewX1 = 0;
		pT9Key->pT9pyLable[i]->view_attr.viewWidth = 0;
		
		pT9Key->pT9pyLable[i]->view_attr.touchX1 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchY1 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchX2 = 0;
		pT9Key->pT9pyLable[i]->view_attr.touchY2 = 0;
	}
		
	for (i=0; i<gu8T9DisPyMax; i++)
	{
		pT9Key->pT9pyLable[i]->view_attr.enable = 1;
		disp_info.font_size = pT9Key->pT9pyLable[i]->label_privattr.font_size;
		disp_info.font_type = 0;
		disp_info.flag = pT9Key->pT9pyLable[i]->label_privattr.position;
		disp_info.keyStr = NULL;
		if(i == pT9Key->pyselect-pT9Key->pystart && gu8T9CurFocus == 0)
		{
			disp_info.color = pT9Key->pT9pyLable[i]->label_privattr.cl_focus;
		}
		else
		{
			disp_info.color = pT9Key->pT9pyLable[i]->label_privattr.cl_normal;
		}
		disp_info.string = pT9Key->result.PinYing[i+pT9Key->pystart];
		
		len = strlen(disp_info.string);
		
		//hyUsbPrintf("py--len=%d   %s\r\n", len, pT9Key->result.PinYing[i+pT9Key->pystart]);
		
		if (i == 0)
		{
			pT9Key->pT9pyLable[i]->view_attr.viewX1 = FRONT_ARROW;
		}
		else
		{
			pT9Key->pT9pyLable[i]->view_attr.viewX1 = pT9Key->pT9pyLable[i-1]->view_attr.viewX1 + pT9Key->pT9pyLable[i-1]->view_attr.viewWidth + T9_PY_STEP;
		}
		pT9Key->pT9pyLable[i]->view_attr.viewWidth += 12 * len;
		
		//hyUsbPrintf("[%d]---PY:start=%d   width=%d\r\n", i, pT9Key->pT9pyLable[i]->view_attr.viewX1, pT9Key->pT9pyLable[i]->view_attr.viewWidth);
		
		x1 = pT9Key->pT9pyLable[i]->view_attr.viewX1;
		x2 = x1+pT9Key->pT9pyLable[i]->view_attr.viewWidth;
		y1 = row;//pT9Key->pT9pyLable[i]->view_attr.viewY1;
		y2 = y1+pT9Key->pT9pyLable[i]->view_attr.viewHeight;
		
		pT9Key->pT9pyLable[i]->view_attr.viewY1  = y1;
		pT9Key->pT9pyLable[i]->view_attr.touchX1 = x1;
		pT9Key->pT9pyLable[i]->view_attr.touchY1 = y1;
		pT9Key->pT9pyLable[i]->view_attr.touchX2 = x2;
		pT9Key->pT9pyLable[i]->view_attr.touchY2 = y2;
		
		Grap_WriteString(x1,y1,x2,y2,&disp_info);
	}
	
	if(gu8T9CurFocus == 0 && pT9Key->pytotal > gu8T9DisPyMax)
	{
		if(pT9Key->pystart > 0 )//需要画上箭头
		{
			t9_draw_arrow_front(1, row+3);
		}
		if(pT9Key->pystart + gu8T9DisPyMax < pT9Key->pytotal)//需要画下箭头
		{
			t9_draw_arrow_back(1, row+3);
		}
	}
	
	return 0;
}

void t9_check_hz_down(tT9KEY *pT9Key)
{
	int i, len, tot=0;
	
	i = pT9Key->hzstart + gu8T9DisHzMax;
	pT9Key->hzstart  = i;
	pT9Key->hzselect = i;
	len = FRONT_ARROW;
	
	while(i < pT9Key->hOptal)
	{
		len += 12*strlen(pT9Key->result.HanZi[i])+T9_HZ_STEP;
		if (len <= BACK_ARROW)
		{
			tot++;
		}
		else
		{
			break;
		}
		i++;
	}
	gu8T9DisHzMax = tot;
}

void t9_check_hz(tT9KEY *pT9Key)
{
	U16 i,j, len;
	
	if(gu8T9HzPageUp)//向上翻页
	{
		len = FRONT_ARROW;
		i = 0;
		
		//hyUsbPrintf("hzstart=%d\r\n", pT9Key->hzstart);
		
		while(i < pT9Key->hzstart)
		{
			for (j=i; j<pT9Key->hzstart; j++)
			{
				len += 12*strlen(pT9Key->result.HanZi[j])+T9_HZ_STEP;
			}
			//hyUsbPrintf("len=%d\r\n", len);
			if (len <= BACK_ARROW)
			{
				break;
			}
			i++;
			len = FRONT_ARROW;
		}
		
		//hyUsbPrintf("i=%d\r\n", i);
		
		if(gu8T9HzPageUp == 2)//通过左移导致
		{
			pT9Key->hzselect--;
		}
		else
		{
			pT9Key->hzselect = i;
		}
		gu8T9HzPageUp = 0;
		gu8T9DisHzMax = pT9Key->hzstart - i;
		pT9Key->hzstart = i;
		
		//hyUsbPrintf("HzMax=%d\r\n", gu8T9DisHzMax);
		//hyUsbPrintf("hzstart=%d\r\n\r\n", pT9Key->hzstart);
		
		if (gu8T9DisHzMax > T9_DIS_HZ_MAX)
			gu8T9DisHzMax = T9_DIS_HZ_MAX;
	}
}

/*************************************
* 函数: t9_draw_hz
* 功能: 在显示屏上显示汉字或词组  
* 参数: none
* 返回: 0
*************************************/
int t9_draw_hz(tT9KEY *pT9Key, U16 row)
{
	tStrDispInfo disp_info;
	int i;
	U8  flag=0;
	U16 x1,y1,x2,y2,len;
		
	for (i=0; i<T9_DIS_HZ_MAX; i++)
	{
		pT9Key->pT9hzLable[i]->view_attr.enable = 0;
		pT9Key->pT9hzLable[i]->view_attr.viewX1 = 0;
		pT9Key->pT9hzLable[i]->view_attr.viewWidth = 0;
		
		pT9Key->pT9hzLable[i]->view_attr.touchX1 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchY1 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchX2 = 0;
		pT9Key->pT9hzLable[i]->view_attr.touchY2 = 0;
	}
	
	//hyUsbPrintf("draw hanzi HzMax=%d   start=%d\r\n", gu8T9DisHzMax, pT9Key->hzstart);
	
	for (i=0; i<gu8T9DisHzMax; i++)
	{
		//hyUsbPrintf("i=%d\r\n", i);
		
		pT9Key->pT9hzLable[i]->view_attr.enable = 1;
		disp_info.font_size = pT9Key->pT9hzLable[i]->label_privattr.font_size;
		disp_info.font_type = 0;
		disp_info.flag = pT9Key->pT9hzLable[i]->label_privattr.position;
		disp_info.keyStr = NULL;
		if ((i+pT9Key->hzstart) == pT9Key->hzselect && gu8T9CurFocus == 1)
		{
			disp_info.color = pT9Key->pT9hzLable[i]->label_privattr.cl_focus;
		}
		else
		{
			disp_info.color = pT9Key->pT9hzLable[i]->label_privattr.cl_normal;
		}
		disp_info.string = pT9Key->result.HanZi[i+pT9Key->hzstart];
		
		len = strlen(disp_info.string);
		
		//hyUsbPrintf("hz---len=%d        %s\r\n", len, pT9Key->result.HanZi[i+pT9Key->hzstart]);
		
		if (i == 0)
		{
			pT9Key->pT9hzLable[i]->view_attr.viewX1 = FRONT_ARROW;
		}
		else
		{
			pT9Key->pT9hzLable[i]->view_attr.viewX1 = pT9Key->pT9hzLable[i-1]->view_attr.viewX1 + pT9Key->pT9hzLable[i-1]->view_attr.viewWidth + T9_HZ_STEP;
		}
		pT9Key->pT9hzLable[i]->view_attr.viewWidth = len*12;
		
		//hyUsbPrintf("[%d]---HZ:start=%d   width=%d\r\n", i, pT9Key->pT9hzLable[i]->view_attr.viewX1, pT9Key->pT9hzLable[i]->view_attr.viewWidth);
		
		x1 = pT9Key->pT9hzLable[i]->view_attr.viewX1;
		x2 = x1+pT9Key->pT9hzLable[i]->view_attr.viewWidth;
		y1 = row;//pT9Key->pT9hzLable[i]->view_attr.viewY1;
		y2 = y1+pT9Key->pT9hzLable[i]->view_attr.viewHeight;
		
		pT9Key->pT9hzLable[i]->view_attr.touchX1 = x1;
		pT9Key->pT9hzLable[i]->view_attr.touchY1 = y1;
		pT9Key->pT9hzLable[i]->view_attr.touchX2 = x2;
		pT9Key->pT9hzLable[i]->view_attr.touchY2 = y2;
		
		Grap_WriteString(x1,y1,x2,y2,&disp_info);
	}
	
	//是否需要画右侧箭头
	if(gu8T9CurFocus == 1 && pT9Key->hOptal > gu8T9DisHzMax)
	{
		if(pT9Key->hzstart > 0)//需要画上箭头
		{
			t9_draw_arrow_front(2, row+2);
		}
		if(pT9Key->hzstart + gu8T9DisHzMax < pT9Key->hOptal)//需要画下箭头
		{
			t9_draw_arrow_back(2, row+2);
		}
	}
	
	return 0;
}

/*************************************
* 函数: t9_change_sign
* 功能: 改变符号控件状态
* 参数: none
* 返回: 0
*************************************/
void t9_change_sign(tT9KEY *pT9Key)
{
	int i;
	
	for(i = 0; i < T9_SIGN_DIS_MAX; i++)
	{
		pT9Key->pT9SignLable[i]->view_attr.enable = pT9Key->signflag;
	}
}

/*************************************
* 函数: t9_draw_sign
* 功能: 在显示屏上显示符号
* 参数: none
* 返回: 0
*************************************/
static int t9_draw_sign(tT9KEY *pT9Key, U16 row)
{
	tStrDispInfo disp_info;
	int i,selected;
	U8  flag=0;
	U16 x1,y1,x2,y2;
	U16 left;
	U8  dis[3];
	
	for(i = 0; i < T9_DIS_PY_MAX; i++)
	{
		pT9Key->pT9pyLable[i]->view_attr.enable = 0;
	}
	
	for(i = 0; i < T9_DIS_HZ_MAX; i++)
	{
		pT9Key->pT9hzLable[i]->view_attr.enable = 0;
	}
	
	left  = T9_SIGN_MAX-pT9Key->sistart;
	left  = (left > T9_SIGN_DIS_MAX)?T9_SIGN_DIS_MAX:left;
	
	//hyUsbPrintf("sign left=%d   start=%d   select=%d\r\n", left, pT9Key->sistart, pT9Key->siselect);
	
	for(i = 0; i < left; i++)
	{
		disp_info.font_size = pT9Key->pT9SignLable[i]->label_privattr.font_size;
		disp_info.font_type = 0;
		disp_info.flag 		= pT9Key->pT9SignLable[i]->label_privattr.position;
		disp_info.keyStr = NULL;
		if(i == pT9Key->siselect-pT9Key->sistart)
		{
			disp_info.color = pT9Key->pT9SignLable[i]->label_privattr.cl_focus;
		}
		else
		{
			disp_info.color = pT9Key->pT9SignLable[i]->label_privattr.cl_normal;
		}
		
		dis[0] = t9_sign[i+pT9Key->sistart];
		dis[1] = 0;
		dis[2] = 0;
		disp_info.string = dis;
		
		x1 = pT9Key->pT9SignLable[i]->view_attr.viewX1;
		x2 = x1+pT9Key->pT9SignLable[i]->view_attr.viewWidth;
		y1 = row+pT9Key->pT9SignLable[i]->view_attr.viewY1;
		y2 = y1+pT9Key->pT9SignLable[i]->view_attr.viewHeight;
		
		pT9Key->pT9SignLable[i]->view_attr.touchX1 = x1;
		pT9Key->pT9SignLable[i]->view_attr.touchY1 = y1;
		pT9Key->pT9SignLable[i]->view_attr.touchX2 = x2;
		pT9Key->pT9SignLable[i]->view_attr.touchY2 = y2;
		
		
		Grap_WriteString(x1,y1,x2,y2,&disp_info);
	}
	
	drvLcd_SetColor(0,0);
	drvLcd_drawLine(row+30,0,row+30,240);
	
	for(i = 30; i < 240; i+=30)
	{
		drvLcd_drawLine(row,i,row+60,i);
	}
		
	return 0;
}

S32 t9_drawShouInput(tT9KEY *pT9Key, U16 row)
{
	tStrDispInfo disp_info;
	U8  dis[3];
	
	disp_info.font_size = FONTSIZE24X24;
	disp_info.font_type = 0;
	disp_info.flag 		= 0;
	disp_info.color = T9_UNSELECT_COLOR;
	disp_info.string = pT9Key->result.ShowInput;
	disp_info.keyStr = NULL;
	//Grap_ClearScreen(2, row, 238, 275);
	
	Grap_WriteString(2,row,238,row+T9_SHOW_YOFF,&disp_info);
	
	return 0;
}


/*************************************
* 函数: t9_labledraw
* 功能: 输入法控件刷新
* 参数: 
* 返回: 1
*************************************/
S32 t9_labledraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX *pDesk;
	tGRAPLABEL	 *pLable;
	tGRAPVIEW	 *pFocus;
	tT9KEY		 *pT9Key;
	U16			 row,col,hi,wi;
	U16			 focR,focH;
	
	pLable = (tGRAPLABEL *)pView;
	pDesk  = (tGRAPDESKBOX *)pLable->pParent;
	
	pT9Key = (tT9KEY *)pDesk->pT9;
	if(pT9Key->keynum == 0 && pT9Key->assnflag == 0 && pT9Key->signflag == 0)
	{
		return SUCCESS;
	}
	
	pFocus = (tGRAPVIEW *)Grap_GetCurFocus(pDesk, NULL);
	if(pFocus)
	{
		focR = pFocus->view_attr.viewY1;
		focH = pFocus->view_attr.viewHeight;
		
		if(focR >= T9_DISPLAY_Y1 || focR+focH > T9_DISPLAY_Y1)
		{//需要改变输入法位置
			pLable->view_attr.viewY1 = T9_DISPLAY_Y2;
			pLable->view_attr.reserve = 1 ;
		}
		else
		{
			pLable->view_attr.viewY1 = T9_DISPLAY_Y1;
			pLable->view_attr.reserve = 0 ;
		}
	}
	
	row = pLable->view_attr.viewY1;
	col = pLable->view_attr.viewX1;
	hi  = pLable->view_attr.viewHeight;
	wi  = pLable->view_attr.viewWidth;
	
	
	drvLcd_SetDisplayAddress(Get_Display_DisplayMemory_Offset());
	
	//清除背景
	if (pT9Key->signflag == 1)
	{
		drvLcd_Background2DisplayMemory(hi-T9_SHOW_YOFF,wi,row+T9_SHOW_YOFF,col);
	}
	else if (pT9Key->assnflag == 1)
	{
		drvLcd_Background2DisplayMemory(hi-T9_HZ_YOFF+3,wi,row+T9_HZ_YOFF-3,col);
	}
	else
	{
		drvLcd_Background2DisplayMemory(hi,wi,row,col);
	}
	
	//drvLcd_SetColor(0,0);
	//drvLcd_Rectangle(row, col, hi, wi, 1);
	
	if(pT9Key->signflag == 1)//符号
	{
		//刷新符号
		t9_draw_sign(pT9Key,row+T9_SHOW_YOFF);
	}
	else
	{
		if (pT9Key->result.Show_Num > 0)
		{
			//刷新显示字符
			t9_drawShouInput(pT9Key, row);
			drvLcd_SetColor(0,0);
			drvLcd_drawLine(row+T9_HLINE_YOFF_1,0,row+T9_HLINE_YOFF_1,240);
		}
		
	
		if(pT9Key->pytotal > 0)
		{
			//刷新拼音
			t9_draw_py(pT9Key,row + T9_PY_YOFF);
			drvLcd_SetColor(0,0);
			drvLcd_drawLine(row+T9_HLINE_YOFF_2,0,row+T9_HLINE_YOFF_2,240);
		}
		
		
		if(pT9Key->hOptal > 0)
		{
			//刷新汉字
			t9_draw_hz(pT9Key,row + T9_HZ_YOFF);
		}
	}
	
	drvLcd_SetColor(0,0);
	if (pT9Key->signflag == 1)
		drvLcd_Rectangle(row+T9_SHOW_YOFF, col, hi-T9_SHOW_YOFF, wi, 1);
	else if (pT9Key->assnflag == 1)
		drvLcd_Rectangle(row+T9_HZ_YOFF-3, col, hi-T9_HZ_YOFF+3, wi, 1);
	else
		drvLcd_Rectangle(row, col, hi, wi, 1);
				
	return SUCCESS;
}

/*************************************
* 函数: t9_ifrecvclick
* 功能: 输入法是否需要接收触摸消息
* 参数: none
* 返回: 1:需要   0:不需要
*************************************/
int t9_ifrecvclick(tT9KEY *pT9Key, GRAP_EVENT *pEvent)
{
	
#if 1

	if(pT9Key->keynum > 0 || pT9Key->assnflag == 1 || pT9Key->signflag == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
	
#else
	
	
	U32 extParam, message, wParam, lParam ;
	int x, y, ret;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;
	
	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;

	//hyUsbPrintf1("T9 recive pullmessage\r\n");
	//hyUsbPrintf1("x=%d  y=%d\r\n", x, y);

	if(pT9Key->keynum > 0)
	{
	//hyUsbPrintf1("keynum: %d\r\n", pT9Key->pT9pyLable[0]->view_attr.touchY1 + T9_SHOW_YOFF);
		if (y >= T9_DISPLAY_Y1)//pT9Key->pT9pyLable[0]->view_attr.touchY1 + T9_SHOW_YOFF)
		{
			ret = 1;
		}
		else
		{
			t9_clear(pT9Key);
			InputRange_ClearKeyArray();
			pT9Key->keynum  = 0;
			pT9Key->assnflag= 0;
			pT9Key->signflag= 0;
			pT9Key->result.Show_Num = 0;
			disable_allShowBtn(pT9Key);
			ret = 0;
		}
	}
	else if (pT9Key->assnflag == 1)
	{
	//hyUsbPrintf1("assnflag: %d\r\n", pT9Key->pT9pyLable[0]->view_attr.touchY1+3);
		if (y >= T9_DISPLAY_Y1 + T9_SHOW_YOFF +30)//pT9Key->pT9pyLable[0]->view_attr.touchY1+3)
		{
			ret = 1;
		}
		else
		{
			t9_clear(pT9Key);
			InputRange_ClearKeyArray();
			pT9Key->keynum  = 0;
			pT9Key->assnflag= 0;
			pT9Key->signflag= 0;
			pT9Key->result.Show_Num = 0;
			disable_allShowBtn(pT9Key);
			ret = 0;
		}
	}
	else if (pT9Key->signflag == 1)
	{
	//hyUsbPrintf1("signflag:\r\n");
		if (y >= T9_DISPLAY_Y1 + T9_SHOW_YOFF)//pT9Key->pT9SignLable[0]->view_attr.touchY1+1)
		{
			ret = 1;
		}
		else
		{
			t9_clear(pT9Key);
			InputRange_ClearKeyArray();
			pT9Key->keynum  = 0;
			pT9Key->assnflag= 0;
			pT9Key->signflag= 0;
			pT9Key->result.Show_Num = 0;
			disable_allShowBtn(pT9Key);
			ret = 0;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
#endif	
}


int t9_range(int col, int row, tGRAPLABEL *pLable)
{
	int ret = 0;
	int x1,y1,x2,y2;
	
	
	//hyUsbPrintf("col=%d   row=%d\r\n", col, row);
	
	y1 = pLable->view_attr.touchY1,
	x1 = pLable->view_attr.touchX1,
	x2 = pLable->view_attr.touchX2;
	y2 = pLable->view_attr.touchY2;
	
	//hyUsbPrintf("range:x1=%d  y1=%d  x2=%d  y2=%d\r\n\r\n", x1, y1, x2, y2);
		
	if(row >= y1 && row < y2 && col >= x1 && col < x2)
	{
		ret = 1;
	}
	
	return ret;
}


/*************************************
* 函数: t9_pullmsg
* 功能: 触摸消息处理
* 参数: none
* 返回: 
*************************************/
int t9_pullmsg(tGRAPLABEL *pLable, GRAP_EVENT *pEvent, tT9KEY *pT9Key)
{
	U32 extParam, message, wParam, lParam ;
	tGRAPDESKBOX *pDesk;
	tT9ASSN_CTR	 *pAssn;
	int x, y ;
	int ret = 0;
	U8  i,disnum;
	U16 total,left,cmpy;
	U8  *pStr, hz[3];
	
	pDesk  = (tGRAPDESKBOX *)pLable->pParent;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;
	
	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if(1 == pLable->view_attr.reserve)
	{
		cmpy = T9_DISPLAY_Y2 ;
	}
	else
	{
		cmpy = T9_DISPLAY_Y1 ;
	}
	
	if(VIEW_PULL_START == extParam)//只处理按下点
	{
		if(y < cmpy)
		{
			
			InputRange_ClearKeyArray();
			if(pT9Key->keynum || pT9Key->assnflag || pT9Key->signflag)
			{
				pT9Key->keynum  = 0;
				pT9Key->assnflag= 0;
				pT9Key->signflag= 0;
				
				t9_change_sign(pT9Key);
				disable_allShowBtn(pT9Key);
				ReDraw(pDesk,0,0);
			}
			return FAIL;
		}
		
		if (pT9Key->signflag == 1)//显示的是符号
		{
			left = T9_SIGN_MAX-pT9Key->sistart;
			left = (left > T9_SIGN_DIS_MAX)?T9_SIGN_DIS_MAX:left;
			for (i=0; i<left; i++)
			{
				if(t9_range(x,y,pT9Key->pT9SignLable[i]))
				{
					pT9Key->siselect = i + pT9Key->sistart;
				
					pT9Key->signflag = 0;
					hz[0] = t9_sign[pT9Key->siselect];
					hz[1] = 0;
					pT9Key->t9_return(pDesk,hz);
					t9_change_sign(pT9Key);
					//ReDraw(pDesk,0,0);
					ret = -1;
				}
			}
		}
		else
		{
			for(i = 0; i < gu8T9DisPyMax; i++)//拼音
			{
				if(t9_range(x,y,pT9Key->pT9pyLable[i]))
				{
					pT9Key->pyselect = i + pT9Key->pystart;
					
					pT9Key->hzstart  = 0;
					pT9Key->hzselect = 0;
					//获取汉字
					t9_gethz(pT9Key, pT9Key->result.PinYing[pT9Key->pyselect]);
					
					gu8T9CurFocus = 0;
					
					ret = 1;
				}
			}
			
			for(i = 0; i < gu8T9DisHzMax; i++)//汉字
			{
				if(t9_range(x,y,pT9Key->pT9hzLable[i]))
				{
					pT9Key->hzselect = i + pT9Key->hzstart;;
				
					pStr = pT9Key->result.HanZi[pT9Key->hzselect];
					pT9Key->t9_return(pDesk,pStr);
					pT9Key->keynum = 0;
					
					gu8T9CurFocus = 1; 
					
					InputRange_ChangePriority(pT9Key->result.HanZi[pT9Key->hzselect], 0, 1);
					
					InputRange_ClearKeyArray();
					if(0 == InputRange_GetNextWord(pStr,&pT9Key->result))
					{
						gu8T9CurFocus = 1;
						pT9Key->assnflag = 1;
						
						pT9Key->hOptal = pT9Key->result.HZ_Num;
						pT9Key->hzstart = 0;
						pT9Key->hzselect= 0;
						
						pT9Key->pytotal = 0;
						pT9Key->result.Show_Num = 0;
						
						com_check_hz(pT9Key);
						
						ret = 1;
					}
					else
					{
						gu8T9CurFocus = 0;
						memset(&pT9Key->result,0,sizeof(struct _return_word));
						t9_clear(pT9Key);
						
						pT9Key->keynum  = 0;
						pT9Key->assnflag= 0;
						pT9Key->signflag= 0;
						pT9Key->result.Show_Num = 0;
						
						disable_allShowBtn(pT9Key);
						
						ret = -1;
						//ReDraw(pDesk,0,0);
					}
				}
			}
		}
	}
	
	ReDraw(pDesk,0,0);
	
	return ret;
}

void t9_closeshow(tGRAPDESKBOX *pDesk)
{
	tT9KEY		 *pT9Key;
	
	pT9Key = (tT9KEY *)pDesk->pT9;
	if(pT9Key && pDesk->inputMode == 5)
	{
		InputRange_ClearKeyArray();
		if(pT9Key->keynum || pT9Key->assnflag || pT9Key->signflag)
		{
			pT9Key->keynum  = 0;
			pT9Key->assnflag= 0;
			pT9Key->signflag= 0;
			
			t9_change_sign(pT9Key);
			
			disable_allShowBtn(pT9Key);
		}
	}
}

/*************************************
* 函数: t9_handle
* 功能: 输入法消息处理
* 参数: none
* 返回: 1:输入法处理了该消息  0：未处理该消息  -1:需要ReDraw
*************************************/
int t9_handle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPDESKBOX *pDesk;
	tGRAPLABEL	 *pLable;
	tT9KEY		 *pT9Key;
	tT9ASSN_CTR	 *pAssn;
	U32			 keycode;
	U8		     *pStr,hz[3];
	int 		 i,len,ret = 0;
	
	pLable = (tGRAPLABEL *)pView;
	pDesk  = (tGRAPDESKBOX *)pLable->pParent;
	
	pT9Key = (tT9KEY *)pDesk->pT9;
	
	if(pDesk->inputMode == 5)
	{
		if(pEvent->message == VIEW_REV_PULLMSG && 1 == t9_ifrecvclick(pT9Key, pEvent))
		{
			ret = t9_pullmsg(pLable,pEvent,pT9Key);
			return ret;
		}
		
		keycode = pEvent->lParam;
		switch(keycode)
		{
		case KEYEVENT_OK:
			if(pT9Key->keynum > 0 || pT9Key->assnflag == 1)
			{
				if(gu8T9CurFocus == 0)//焦点在拼音
				{
					gu8T9CurFocus = 1;
					pT9Key->hzstart = 0;
					pT9Key->hzselect= 0;
					
					InputRange_ChangePriority(pT9Key->result.PinYing[pT9Key->pyselect], 0, 1);
					
					ret = 1;
				}
				else if(gu8T9CurFocus == 1)//焦点在汉字
				{
					pStr = pT9Key->result.HanZi[pT9Key->hzselect];
					len  = strlen(pStr);
					pT9Key->t9_return(pDesk,pStr);
					pT9Key->keynum = 0;
					
					InputRange_ChangePriority(pT9Key->result.HanZi[pT9Key->hzselect], 0, 1);
					
					InputRange_ClearKeyArray();
					if(0 == InputRange_GetNextWord(pStr,&pT9Key->result))
					{
						//gu8T9CurFocus = 0;
						
						pT9Key->assnflag = 1;
						
						pT9Key->hOptal = pT9Key->result.HZ_Num;
						pT9Key->hzstart = 0;
						pT9Key->hzselect= 0;
						
						pT9Key->pytotal = 0;
						pT9Key->result.Show_Num = 0;
						
						com_check_hz(pT9Key);
						
						ret = 1;
					}
					else
					{
						gu8T9CurFocus = 0;
						memset(&pT9Key->result,0,sizeof(struct _return_word));
						t9_clear(pT9Key);
						ret = -1;
						
						pT9Key->keynum  = 0;
						pT9Key->assnflag= 0;
						pT9Key->signflag= 0;
						
						disable_allShowBtn(pT9Key);
						
						//ReDraw(pDesk,0,0);
					}
				}
			}
			else if(pT9Key->signflag == 1)
			{
				pT9Key->signflag = 0;
				hz[0] = t9_sign[pT9Key->siselect];
				hz[1] = 0;
				pT9Key->t9_return(pDesk,hz);
				t9_change_sign(pT9Key);
				//ReDraw(pDesk,0,0);
				ret = -1;
			}
			ReDraw(pDesk,0,0);
			break;
		case KEYEVENT_ESC:
			InputRange_ClearKeyArray();
			if(pT9Key->keynum || pT9Key->assnflag || pT9Key->signflag)
			{
				pT9Key->keynum  = 0;
				pT9Key->assnflag= 0;
				pT9Key->signflag= 0;
				
				t9_change_sign(pT9Key);
				
				disable_allShowBtn(pT9Key);
				
				ReDraw(pDesk,0,0);
				ret = -1;
			}
			break;
		case KEYEVENT_SHARP://#
			InputRange_ClearKeyArray();
			if(pT9Key->keynum || pT9Key->assnflag || pT9Key->signflag)
			{
				pT9Key->keynum  = 0;
				pT9Key->assnflag= 0;
				pT9Key->signflag= 0;
				
				t9_change_sign(pT9Key);
				
				disable_allShowBtn(pT9Key);
				
				ReDraw(pDesk,0,0);
				ret = -1;
			}
			break;
		case KEYEVENT_STAR://符号 *
			InputRange_ClearKeyArray();
			if(pT9Key->signflag == 0)
			{
				pT9Key->keynum  = 0;
				pT9Key->assnflag= 0;
				pT9Key->signflag= 1;
				
				pT9Key->sitotal  = T9_SIGN_MAX;
				pT9Key->siselect = 0;
				pT9Key->sistart  = 0;
			}
			else
			{
				pT9Key->signflag = 0;
			}
			t9_change_sign(pT9Key);
			ReDraw(pDesk,0,0);
			ret = -1;
			break;
		case KEYEVENT_0:
			pT9Key->t9_return(pDesk," ");
			ReDraw(pDesk,0,0);
			ret = -1;
			break;
		case KEYEVENT_1:
			if(pT9Key->signflag == 1)
			{
				pT9Key->signflag = 0;
				hz[0] = t9_sign[pT9Key->siselect];
				hz[1] = 0;
				pT9Key->t9_return(pDesk,hz);
				ReDraw(pDesk,0,0);
				ret = -1;
			}
			else
			{
				ret = 1;
			}
			break;
		case KEYEVENT_2:
		case KEYEVENT_3:
		case KEYEVENT_4:
		case KEYEVENT_5:
		case KEYEVENT_6:
		case KEYEVENT_7:
		case KEYEVENT_8:
		case KEYEVENT_9:
			keycode -= KEYEVENT_0;
			if(pT9Key->signflag == 1)
			{
				pT9Key->signflag = 0;
				//hz[0] = t9_sign[pT9Key->siselect];
				//hz[1] = 0;
				//pT9Key->t9_return(pDesk,hz);
				//ReDraw(pDesk,0,0);
				//ret = -1;
			}
			//else
			{
				t9_inputkey(pT9Key, keycode);
				ret = 1;
				if (gu8T9CurFocus == 1)
					gu8T9CurFocus = 0;
			}
			break;
		case KEYEVENT_UP://时翻到上一行
			if(pT9Key->keynum > 0 || pT9Key->assnflag == 1)
			{
				if(gu8T9CurFocus == 0)
				{
					if(pT9Key->pystart > 0)
					{
						gu8T9PyPageUp = 1;
						t9_check_py(pT9Key);
						
						pT9Key->hzstart = 0;
						pT9Key->hzselect= 0;
						//获取汉字
						t9_gethz(pT9Key, pT9Key->result.PinYing[pT9Key->pyselect]);
					}
					ret = 1;
				}
				else if(gu8T9CurFocus == 1)
				{
					if(pT9Key->hzstart>0)
					{
						gu8T9HzPageUp = 1;
						t9_check_hz(pT9Key);
					}
					ret = 1;
				}
			}
			else if(pT9Key->signflag == 1)
			{
				if(pT9Key->siselect - pT9Key->sistart >= T9_SIGN_DIS_MAX/2)
				{
					pT9Key->siselect -= T9_SIGN_DIS_MAX/2;
				}
				else if(pT9Key->sistart > 0)
				{
					pT9Key->sistart -= T9_SIGN_DIS_MAX;
					pT9Key->siselect = pT9Key->sistart+T9_SIGN_DIS_MAX/2;
				}
				ret = 1;
			}
			break;
		case KEYEVENT_DOWN://翻到下一行
			if(pT9Key->keynum > 0 || pT9Key->assnflag == 1)
			{
				if(gu8T9CurFocus == 0)
				{
					if(pT9Key->pystart+gu8T9DisPyMax < pT9Key->pytotal)
					{
						t9_check_py_down(pT9Key);
						
						pT9Key->hzstart = 0;
						pT9Key->hzselect= 0;
						//获取汉字
						t9_gethz(pT9Key, pT9Key->result.PinYing[pT9Key->pyselect]);
					}
					ret = 1;
				}
				else if(gu8T9CurFocus == 1)
				{
					if(pT9Key->hzstart+gu8T9DisHzMax<pT9Key->hOptal)
					{
						t9_check_hz_down(pT9Key);
					}
					ret = 1;
				}
			}
			else if(pT9Key->signflag == 1)
			{
				if(pT9Key->siselect - pT9Key->sistart < T9_SIGN_DIS_MAX/2)
				{
					pT9Key->siselect += T9_SIGN_DIS_MAX/2;
				}
				else if(pT9Key->sistart+T9_SIGN_DIS_MAX < pT9Key->sitotal)
				{
					pT9Key->sistart += T9_SIGN_DIS_MAX;
					pT9Key->siselect = pT9Key->sistart;
				}
				ret = 1;
			}
			break;
		case KEYEVENT_LEFT://移动焦点
			if(pT9Key->keynum > 0 || pT9Key->assnflag == 1)
			{
				if(gu8T9CurFocus == 0)
				{
					if(pT9Key->pyselect > pT9Key->pystart)
					{
						pT9Key->pyselect--;
					}
					else
					{//向上翻一行
						if(pT9Key->pystart > 0)
						{
							gu8T9PyPageUp = 2;
							t9_check_py(pT9Key);
						}
					}
					pT9Key->hzstart = 0;
					pT9Key->hzselect= 0;
					//获取汉字
					t9_gethz(pT9Key, pT9Key->result.PinYing[pT9Key->pyselect]);
					ret = 1;
				}
				else if(gu8T9CurFocus == 1)
				{
					if(pT9Key->hzselect > pT9Key->hzstart)
					{
						pT9Key->hzselect--;
					}
					else
					{//向上翻一行
						if(pT9Key->hzstart > 0)
						{
							gu8T9HzPageUp = 2;
							t9_check_hz(pT9Key);
						}
					}
					ret = 1;
				}
			}
			else if(pT9Key->signflag == 1)
			{
				if(pT9Key->siselect > pT9Key->sistart)
				{
					pT9Key->siselect--;
				}
				else
				{//向上翻一行
					if(pT9Key->sistart > 0)
					{
						pT9Key->sistart -= T9_SIGN_DIS_MAX;
						pT9Key->siselect--;
					}
				}
				ret = 1;
			}
			break;
		case KEYEVENT_RIGHT://移动焦点
			if(pT9Key->keynum > 0 || pT9Key->assnflag == 1)
			{
				if(gu8T9CurFocus == 0)
				{
					if(pT9Key->pyselect+1 < pT9Key->pytotal)
					{
						pT9Key->pyselect++;
					}
					if(pT9Key->pyselect-pT9Key->pystart >= gu8T9DisPyMax)
					{
						t9_check_py_down(pT9Key);
					}
					pT9Key->hzstart = 0;
					pT9Key->hzselect= 0;
					//获取汉字
					t9_gethz(pT9Key, pT9Key->result.PinYing[pT9Key->pyselect]);
					ret = 1;
				}
				else if(gu8T9CurFocus == 1)
				{
					if(pT9Key->hzselect+1 < pT9Key->hOptal)
					{
						pT9Key->hzselect++;
					}
					if(pT9Key->hzselect-pT9Key->hzstart>=gu8T9DisHzMax)
					{
						t9_check_hz_down(pT9Key);
					}
					
					ret = 1;
				}
			}
			else if(pT9Key->signflag == 1)
			{
				if(pT9Key->siselect+1 < pT9Key->sitotal)
				{
					pT9Key->siselect++;
				}
				if(pT9Key->siselect-pT9Key->sistart>=T9_SIGN_DIS_MAX)
				{
					pT9Key->sistart+=T9_SIGN_DIS_MAX;
				}
				ret = 1;
			}
			break;
		case KEYEVENT_DELETE://删除一个输入的字符
			if(pT9Key->keynum > 0)
			{
				if (gu8T9CurFocus == 1)
				{
					gu8T9CurFocus = 0;
					ret = 1;
					break;
				}
				if(t9_deletekey(pT9Key) == -2)
				{
					pT9Key->keynum  = 0;
					pT9Key->assnflag= 0;
					pT9Key->signflag= 0;
					
					disable_allShowBtn(pT9Key);
				
					ReDraw(pDesk,0,0);
					ret = -1;
				}
				else
				{
					ret = 1;
				}
			}
			else if(pT9Key->assnflag == 1)
			{
				pT9Key->keynum  = 0;
				pT9Key->assnflag= 0;
				pT9Key->signflag= 0;
				disable_allShowBtn(pT9Key);
				ReDraw(pDesk,0,0);
				ret = -1;
			}
			else if(pT9Key->signflag == 1)
			{
				pT9Key->signflag = 0;
				ReDraw(pDesk,0,0);
				
				t9_change_sign(pT9Key);
				ret = -1;
			}
			break;
		default:
			break;
		}
	}
	
	return ret;
}

int t9_LabelDestroy(void *pView)
{
	free(pView);
	
	gu8T9HzPageUp	= 0;
    gu8T9PyPageUp   = 0;
	gu8T9CurFocus   = 0;
	gu8T9DisHzMax	= 0;	
	gu8T9DisPyMax	= 0;
	
	return SUCCESS;
}

/*************************************
* 函数: t9_insert
* 功能: 在需要输入法的窗体上插入输入法
* 参数: 
* 返回: 1
*************************************/
int t9_insert(tGRAPDESKBOX *pDesk, t9_Return tReturn)
{
	tGRAPLABEL	*pLable;
	tT9KEY		*pT9Key;
	int			ret,i,ind=0;
    U16			x,y;
    
    gu8T9HzPageUp	= 0;
    gu8T9PyPageUp   = 0;
	gu8T9CurFocus   = 0;
	gu8T9DisHzMax	= 0;	
	gu8T9DisPyMax	= 0;
	
	pDesk->pT9 = malloc(sizeof(tT9KEY));
	
	pT9Key = (tT9KEY *)pDesk->pT9;
	if(NULL == pT9Key)
	{
		return 0;
	}
	memset((char *)pT9Key,0,sizeof(tT9KEY));
	
	if(tReturn == NULL)
	{
		pT9Key->t9_return = t9_return_word;
	}
	else
	{
		pT9Key->t9_return = tReturn;
	}
	
	pLable = Grap_InsertLabel(pDesk,&t9_ViewAttr[ind++],&t9_LabPri[0]);
	if(pLable == NULL) return 0;
	pLable->handle = t9_handle;
	pLable->draw   = t9_labledraw;
	pLable->destroy= t9_LabelDestroy;
	
	pT9Key->pWidget = pLable;
	
	for(i = 0; i < T9_DIS_PY_MAX; i++)
	{
		pLable = Grap_InsertLabel(pDesk,&t9_ViewAttr[ind++],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		/*
		pLable->view_attr.viewX1	= t9_py_pos[i][1];
		pLable->view_attr.viewY1	= t9_py_pos[i][0];
		
		pLable->view_attr.touchX1	= t9_py_pos[i][1];
		pLable->view_attr.touchY1	= t9_py_pos[i][0];
		pLable->view_attr.touchX2	= t9_py_pos[i][3];
		pLable->view_attr.touchY2	= t9_py_pos[i][2];
		*/
		
		pT9Key->pT9pyLable[i] = pLable;
	}
	
	for(i = 0; i < T9_DIS_HZ_MAX; i++)
	{
		pLable = Grap_InsertLabel(pDesk,&t9_ViewAttr[ind++],&t9_LabPri[1]);
		if(pLable == NULL) return 0;
		/*
		pLable->view_attr.viewX1	= t9_hz_pos[i][1];
		pLable->view_attr.viewY1	= t9_hz_pos[i][0];
		
		pLable->view_attr.touchX1	= t9_hz_pos[i][1];
		pLable->view_attr.touchY1	= t9_hz_pos[i][0];
		pLable->view_attr.touchX2	= t9_hz_pos[i][3];
		pLable->view_attr.touchY2	= t9_hz_pos[i][2];
		*/
		
		pT9Key->pT9hzLable[i] = pLable;
	}
	
	for(i = 0; i < 2; i++)//汉字右侧箭头
	{
		pLable = Grap_InsertLabel(pDesk,&t9_ViewAttr[ind++],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		pT9Key->pT9ArrowLable[i] = pLable;
	}
	
	x = 0;
	y = 0;
	for(i = 0; i < T9_SIGN_DIS_MAX; i++)
	{
		if(i != 0 && i%8 == 0)
		{
			x = 0;
			y += T9_SIGN_H;
		}
		
		pLable = Grap_InsertLabel(pDesk,&t9_ViewAttr[ind],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		
		pLable->view_attr.viewX1	= x;
		pLable->view_attr.viewY1	= y;
		
		pLable->view_attr.touchX1	= x;
		pLable->view_attr.touchY1	= y;
		pLable->view_attr.touchX2	= x+T9_SIGN_W;
		pLable->view_attr.touchY2	= y+T9_SIGN_H;
		
		pT9Key->pT9SignLable[i] = pLable;
		
		x += T9_SIGN_W;
	}
	
	
	return 1;
}

#if 0

tGRAPVIEW *t9_linkativs ;
U8        t9totalatcivs ;

tGRAPVIEW *t9_CreatInsert(tGRAPDESKBOX *pDesk, t9_Return tReturn)
{
	tGRAPLABEL	*pLable;
	tT9KEY		*pT9Key;
	tGRAPVIEW   *pViewTem = NULL ;
	int			ret,i,ind=0;
    U16			x,y;
    
    gu8T9HzPageUp	= 0;
    gu8T9PyPageUp   = 0;
	gu8T9CurFocus   = 0;
	gu8T9DisHzMax	= 0;	
	gu8T9DisPyMax	= 0;
	
	
	pDesk->pT9 = malloc(sizeof(tT9KEY));
	
	pT9Key = (tT9KEY *)pDesk->pT9;
	if(NULL == pT9Key)
	{
		return 0;
	}
	memset((char *)pT9Key,0,sizeof(tT9KEY));
	
	if(tReturn == NULL)
	{
		pT9Key->t9_return = t9_return_word;
	}
	else
	{
		pT9Key->t9_return = tReturn;
	}
	
	pLable = Grap_CreateLabel(&t9_ViewAttr[ind++],&t9_LabPri[0]);
	if(pLable == NULL) return 0;
	pLable->handle = t9_handle;
	pLable->draw   = t9_labledraw;
	pLable->pNext  = pViewTem ;
	pViewTem       = (tGRAPVIEW *)pLable ;
	
	pT9Key->pWidget = pLable;
	
	for(i = 0; i < T9_DIS_PY_MAX; i++)
	{
		pLable = Grap_CreateLabel(&t9_ViewAttr[ind++],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		pLable->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pLable ;
		
		pT9Key->pT9pyLable[i] = pLable;
	}
	
	for(i = 0; i < T9_DIS_HZ_MAX; i++)
	{
		pLable = Grap_CreateLabel(&t9_ViewAttr[ind++],&t9_LabPri[1]);
		if(pLable == NULL) return 0;
		pLable->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pLable ;
		
		pT9Key->pT9hzLable[i] = pLable;
	}
	
	for(i = 0; i < 2; i++)//汉字右侧箭头
	{
		pLable = Grap_CreateLabel(&t9_ViewAttr[ind++],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		pLable->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pLable ;
		
		pT9Key->pT9ArrowLable[i] = pLable;
	}
	
	x = 0;
	y = 0;
	for(i = 0; i < T9_SIGN_DIS_MAX; i++)
	{
		if(i != 0 && i%8 == 0)
		{
			x = 0;
			y += T9_SIGN_H;
		}
		
		pLable = Grap_CreateLabel(&t9_ViewAttr[ind],&t9_LabPri[0]);
		if(pLable == NULL) return 0;
		pLable->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pLable ;
		
		pLable->view_attr.viewX1	= x;
		pLable->view_attr.viewY1	= y;
		
		pLable->view_attr.touchX1	= x;
		pLable->view_attr.touchY1	= y;
		pLable->view_attr.touchX2	= x+T9_SIGN_W;
		pLable->view_attr.touchY2	= y+T9_SIGN_H;
		
		pT9Key->pT9SignLable[i] = pLable;
		
		x += T9_SIGN_W;
	}
	
	ComWidget_Link2Desk(pDesk,pViewTem);
	
	t9_linkativs = pViewTem ;
	t9totalatcivs= 1 + T9_DIS_PY_MAX + T9_DIS_HZ_MAX + 2 + T9_SIGN_DIS_MAX;
	
	return pViewTem;
}

void t9_LeftFromDeskandDesdray(tGRAPDESKBOX *pDesk)
{
	tGRAPVIEW *pParentView;
	tGRAPVIEW *pSubChi,*pSubDele;
	 
	U32 i;
	
	if(NULL == pDesk)
	{
		return ;
	}
		
	pParentView = (tGRAPVIEW *)pDesk;
	pSubChi = (tGRAPVIEW *)t9_linkativs;
	
	for(i=0; i<t9totalatcivs; i++)
	{
		if(i == t9totalatcivs-1)
		{
			((tGRAPDESKBOX *)pParentView)->pNext = (tGRAPVIEW *)pSubChi->pNext;
			pSubChi->pNext = NULL;
			pSubChi->destroy(pSubChi);
			break;
		}
		pSubDele= (tGRAPVIEW *)pSubChi;
		pSubChi = (tGRAPVIEW *)pSubChi->pNext;
		
		pSubDele->destroy(pSubDele);
		
	}
	
	free(pDesk->pT9);
	pDesk->pT9 = NULL ;
	
	return ;
}

/*---------------------------------------------------------
*函数: t9_bak_panel
*功能: 进入下一个含有输入法的Desk前先将上一个Desk的输入法全局变量备份 (只支持2层Desk)
*参数: none
*返回: none
*---------------------------------------------------------*/
void t9_bak_panel(U8 index)
{
	U8  i ; 
	
	for(i = 0 ; i < T9_DIS_PY_MAX; i++)
	{
		gT9pyLable_Bak[index][i] = gT9pyLable[i];
	}
	
	for(i = 0 ; i < T9_DIS_HZ_MAX; i++)
	{
		gT9hzLable_Bak[index][i] = gT9hzLable[i];
	}
	
	for(i = 0 ; i < 2; i++)
	{
		gT9ArrowLable_Bak[index][i] = gT9ArrowLable[i];
	}
	
	for(i = 0 ; i < T9_SIGN_DIS_MAX; i++)
	{
		gT9SignLable_Bak[index][i] = gT9SignLable[i];
	}
}
/*---------------------------------------------------------
*函数: t9_set_panel
*功能: 退出一个含有输入法的Desk后要将上一个Desk的输入法全局变量复原 (只支持2层Desk)
*参数: none
*返回: none
*---------------------------------------------------------*/
void t9_set_panel(U8 index)
{
	U8  i ; 
	
	for(i = 0 ; i < T9_DIS_PY_MAX; i++)
	{
		gT9pyLable[i] = gT9pyLable_Bak[index][i];
	}
	
	for(i = 0 ; i < T9_DIS_HZ_MAX; i++)
	{
		gT9hzLable[i] = gT9hzLable_Bak[index][i];
	}
	
	for(i = 0 ; i < 2; i++)
	{
		gT9ArrowLable[i] = gT9ArrowLable_Bak[index][i];
	}
	
	for(i = 0 ; i < T9_SIGN_DIS_MAX; i++)
	{
		gT9SignLable[i] = gT9SignLable_Bak[index][i];
	}

	gu8T9HzPageUp	= 0;
    gu8T9PyPageUp   = 0;
	gu8T9CurFocus   = 0;
	gu8T9DisHzMax	= 0;	
	gu8T9DisPyMax	= 0;


}
#endif