/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwChip.h"
#include "hyhwUart.h"

//#include "scratch.h"
#include "grap_api.h"
#include "DisplayMemory.h"
#include "editBmpData.h"
#include "constEditData.h"
#include "AlphaDisplay.h"
#include  "glbVariable.h"
#include  "edit.h"

#include "InputRange_api.h"


U8 const DeleteBmp[][32]=
{
	0x00,0x00,0x00,0x00,0x02,0x00,0x06,0x00,0x0E,0x00,0x1F,0xFE,0x3F,0xFE,0x7F,0xFE,
	0x7F,0xFE,0x3F,0xFE,0x1F,0xFE,0x0E,0x00,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////


//U8 Is_WriteHand;//0:联想的汉字   1：手写识别的汉字
//tGRAPBUTTON *gu_ShowHZ_sx[MAX_SHOW_HZ];
//tGRAPBUTTON *gu_ShowHZ_arrow[2];
//tGRAPBUTTON *gu_ShowHZ_identify[MAX_IDENTIFY_HZ];
//tGRAPBUTTON *gu_DeleteBtn;

//tGRAPBUTTON *gu_ShowHZ_sx_Bak[3][MAX_SHOW_HZ];
//tGRAPBUTTON *gu_ShowHZ_arrow_Bak[3][2];
//tGRAPBUTTON *gu_ShowHZ_identify_Bak[3][MAX_IDENTIFY_HZ];
//tGRAPBUTTON *gu_DeleteBtn_Bak[3];
int Show_Max_HzNum;
int Show_start;
int Show_IdentifyNum;
U8  BufferCopy_Temp[MAX_IDENTIFY_HZ][4]={0};
tReturnWord gu_Result;

U16 gu16EditClrInput[4];//输入法颜色
U8  gu8EditAlphaInput[2];//输入法的alpha值
U8  gu8EdittypeChi;//保存的上次的汉字输入状态

/////////////////////////////////////////////////
/////////////////////////////////////////////////


// COLOR_BLUE						 //COLOR_GREEN  //COLOR_RED
#define BMP_DEFAULT_CLR_PRESS  0x660 
#define BMP_DEFAULT_CLR_NORMAL COLOR_WHITE
#define BMP_DEFAULT_CLR_WRITE  COLOR_RED//COLOR_BLUE

#define WRITE_TOTAL_BTN         5
#define WRITE_ASSN_TOTAL_BTN    5
#define WRITE_ASSN_BUF_INDEX    5


const U16 gHandBtnShowId[10]=
{
BUTTON_ID_1_EDIT,BUTTON_ID_2_EDIT,BUTTON_ID_3_EDIT,BUTTON_ID_4_EDIT,	
BUTTON_ID_5_EDIT,BUTTON_ID_6_EDIT,BUTTON_ID_7_EDIT,BUTTON_ID_8_EDIT,
BUTTON_ID_9_EDIT,BUTTON_ID_10_EDIT,
};

const U16 gSelBtnShowId[32]=
{
BUTTON_ID_30_EDIT,BUTTON_ID_31_EDIT,BUTTON_ID_32_EDIT,BUTTON_ID_33_EDIT,BUTTON_ID_34_EDIT,
BUTTON_ID_35_EDIT,BUTTON_ID_36_EDIT,BUTTON_ID_37_EDIT,BUTTON_ID_38_EDIT,BUTTON_ID_39_EDIT,
BUTTON_ID_40_EDIT,BUTTON_ID_41_EDIT,BUTTON_ID_42_EDIT,BUTTON_ID_43_EDIT,BUTTON_ID_44_EDIT,
BUTTON_ID_45_EDIT,BUTTON_ID_46_EDIT,BUTTON_ID_47_EDIT,BUTTON_ID_48_EDIT,BUTTON_ID_49_EDIT,
BUTTON_ID_50_EDIT,BUTTON_ID_51_EDIT,BUTTON_ID_52_EDIT,BUTTON_ID_53_EDIT,BUTTON_ID_54_EDIT,
BUTTON_ID_55_EDIT,BUTTON_ID_56_EDIT,BUTTON_ID_57_EDIT,BUTTON_ID_58_EDIT,BUTTON_ID_59_EDIT,
BUTTON_ID_60_EDIT,BUTTON_ID_61_EDIT
};

const U16 gImeBtnShowId[5]=
{
   BUTTON_ID_11_EDIT,BUTTON_ID_12_EDIT,BUTTON_ID_13_EDIT,BUTTON_ID_14_EDIT,BUTTON_ID_15_EDIT
};


tINPUT_BMP_CTR gtBtnBmpCtr = 
{
	BMP_DEFAULT_CLR_PRESS,
	BMP_DEFAULT_CLR_NORMAL,
	BMP_DEFAULT_CLR_WRITE,
	INPUT_CTR_BIGABC,
	0
};  
  

const U8 gu16StartAttr[]=
{
    INPUT_CTR_WWW,    INPUT_WWW_START,      INPUT_WWW_LEN,
    INPUT_CTR_BIGABC, INPUT_BIG_ABS_START,  INPUT_BIG_ABS_LEN,
    INPUT_CTR_SMAABC, INPUT_SMAL_ABS_START, INPUT_SMAL_ABS_LEN,
    INPUT_CTR_DIGIT,  INPUT_DIGIT_START,    INPUT_DIGIT_LEN,
    INPUT_CTR_IME,    INPUT_IME_START,      INPUT_IME_LEN,    
    INPUT_CTR_CHI,    INPUT_HANDW_START,    INPUT_HANDW_LEN,
    INPUT_CTR_SIGN,   INPUT_SIGN_START,     INPUT_SIGN_LEN,
    INPUT_CTR_PREVFOCUS,    0,      0,
    INPUT_CTR_NEXTFOCUS,    0,      0,    
};



static const U8 * const gu8EditSpcStr[]=
{
	(const U8 *)"http://",(const U8 *)"wap.",(const U8 *)"www.",(const U8 *)".com",
	(const U8 *)".cn",(const U8 *)".net",(const U8 *)".com.cn",(const U8 *)".gov",
	(const U8 *)"smtp",(const U8 *)"pop",(const U8 *)"pop3",(const U8 *)"..."	
};

static void edit_enable_input_mode(tGRAPDESKBOX *pDesk,U8 num);
static void edit_write_hand_range(tCTR_PANEL *pCtrPan, U16 color );

void edit_DisableIdentifyBtn(tCTR_PANEL *pCtrPan);



extern const U8 T9_FrontBmp[][16];

extern const U8 T9_BackBmp[][16];


void edit_init()
{
	gu8EditAlphaInput[0] = 30;
    gu16EditClrInput[0] = COLOR_RED;
    gu16EditClrInput[1] = COLOR_YELLOW;
    
    //输入法颜色设置成默认值
    gu8EditAlphaInput[1] = 30;
    gu16EditClrInput[2] = COLOR_RED;
    gu16EditClrInput[3] = COLOR_YELLOW;
    gu8EdittypeChi = INPUT_CTR_CHI;
    
    edit_set_alfaValue(gu8EditAlphaInput[0], gu8EditAlphaInput[1]);
}

void edit_check_hz(tReturnWord *pT9Key)
{
	int i, len, tot=0;
	
	i = 0;
	len = ARROW_F_TAIL;
	
	while(i < pT9Key->HZ_Num)
	{
		len += 12*strlen(pT9Key->HanZi[i])+8;
		
		//hyUsbPrintf("check:len=%d\r\n", len);
		
		if (len <= ARROW_T_FRONT)
		{
			tot++;
		}
		else
		{
			break;
		}
		i++;
	}
	Show_Max_HzNum = tot;
}

/*************************************
* 函数: edit_draw_arrow
* 功能: 在显示屏上显示箭头
* 参数: flag:  0:前箭头   1：后箭头   2：两个箭头
* 返回: 0
*************************************/
int  edit_draw_arrow(U8 flag, U16 row)
{
	if (flag == 0)
	{
		Grap_WriteBMPColor(4, row, 16, 16, T9_FrontBmp, 0xffff);
	}
	else if (flag == 1)
	{
		Grap_WriteBMPColor(220, row, 16, 16, T9_BackBmp, 0xffff);
	}
	else if (flag == 2)
	{
		Grap_WriteBMPColor(4, row, 16, 16, T9_FrontBmp, 0xffff);
		Grap_WriteBMPColor(220, row, 16, 16, T9_BackBmp, 0xffff);
	}
	
	return 0;
}

int edit_range(int col, int row, tGRAPBUTTON *pBtn)
{
	int ret = 0;
	int x1,y1,x2,y2;
	
	y1 = pBtn->view_attr.touchY1,
	x1 = pBtn->view_attr.touchX1,
	x2 = pBtn->view_attr.touchX2;
	y2 = pBtn->view_attr.touchY2;
	
	if(row >= y1 && row < y2 && col >= x1 && col < x2)
	{
		ret = 1;
	}
	
	return ret;
}


/*---------------------------------------------------------
*函数: edit_disable_show_buf   
*功能: 禁止buf使能，并清除buf 
*参数: none
*返回: none
*---------------------------------------------------------*/
static void edit_disable_show_buf(tCTR_PANEL *pCtrPan)
{
    int i;
    
    pCtrPan->tSelWord.enable = 0;
    
    for (i=0; i<INPUT_SHOW_BUF_TOTAL; i++)
    {
        memset((char *)pCtrPan->BtnCharShow[i],0,3);
    }
}


/*---------------------------------------------------------
*函数: edit_lab_destroy
*功能: 销毁label对象      
*参数: none
*返回: none
*---------------------------------------------------------*/
static S32 edit_lab_destroy(void *pView)
{
	tGRAPLABEL *pLabel;
	tCTR_PANEL *pCtrPan;  
	tGRAPDESKBOX *pDesk,*pDeskFather;
	pLabel = (tGRAPLABEL *)pView;
	
	pDesk = pLabel->pParent;
	
	pDeskFather = pDesk->pFather;
	
	if(pDeskFather != NULL && pDeskFather->pdata != NULL)//上层窗体也有手写输入,不能释放内存
	{
		free((void *)pLabel);
	}
	else
	{
		pCtrPan = pDesk->pdata;
		
		if (1 == *pCtrPan->pHandLabEnable)
	    {
	        hyHWRecog_EnableFlag(0);
	        ts_set_valid_point_step(15);
	    }
		free((void *)pLabel);
		
		pCtrPan->alphaDisp = 0;
		pCtrPan->tSysCtr.status = 0;
		
		//090408 退出的时候清空内存
	    memset(ALPHA_MEMORY_OFFSET,0,ALPHA_BLOCK_SIZE*3);
	}
	
	Show_Max_HzNum = 0;
	Show_start = 0;
	Show_IdentifyNum = 0;
	
	return SUCCESS;
}



static S32 edit_lab_recv_pull_msg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y,x0,y0 ;
	tCTR_PANEL *pCtrPan;   
    tGRAPDESKBOX *pDesk;
  
	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	y0 = HIWORD(wParam) ;
	x0 = LOWORD(wParam) ;
	
	pDesk = pLabel->pParent;
    pCtrPan = pDesk->pdata;
        
    pLabel->view_attr.viewHeight = 250;
    pLabel->view_attr.touchY2 = pLabel->view_attr.viewY1 + pLabel->view_attr.viewHeight;
    
    //有下拉备选字符的时候不能写字
    if (pCtrPan->tSelWord.enable) return FAIL;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pLabel))
	    {//在有效范围内
	         ts_set_valid_point_step(2);
	         pCtrPan->HandWordShowFlag = 1;
	         ret = VIEW_PULL_START; 
	    }
	    else
	    {
	        return FAIL;
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	     if(GRAP_INWIDGET_RANGE(x,y,pLabel)) 
	     {  	        
            int row,col,hig,wid,row1,col1;
            int tempY,tempX; 
            
            //可能上次画线位置出界
            if (!GRAP_INWIDGET_RANGE(x0,y0,pLabel))
            {
                return VIEW_PULL_ING;
            } 
            
            //输入手写的位置
            #define EDIT_WRITE_ROW   7//14
            #define EDIT_WRITE_COL	 7//15
            #define EDIT_WRITE_HIG	 248//186//152//178//165
            #define EDIT_WRITE_WID	 226//210
	        
	        //修改090512
	        tempY = MIN(y0,y)-2;
	        
	        row =  tempY < EDIT_WRITE_ROW ? EDIT_WRITE_ROW:tempY;
	        row =  row > (EDIT_WRITE_ROW+EDIT_WRITE_HIG) ? (EDIT_WRITE_ROW+EDIT_WRITE_HIG-2): row;
	        
	        tempX = MIN(x0,x)-2;
	        
	        col = tempX < 3 ? 3:tempX;
	        col = col > (EDIT_WRITE_COL+EDIT_WRITE_WID)? (EDIT_WRITE_COL+EDIT_WRITE_WID-2):col;
	        
	        tempY = MAX(y0,y)+2;
	        row1 =  tempY < EDIT_WRITE_ROW ? EDIT_WRITE_ROW:tempY;
	        row1 =  row1 > (EDIT_WRITE_ROW+EDIT_WRITE_HIG) ? (EDIT_WRITE_ROW+EDIT_WRITE_HIG-2): row1;
	        
	        tempX = MAX(x0,x)+2; 	        
	        col1 = tempX < 3 ? 3:tempX;
	        col1 = col1 > (EDIT_WRITE_COL+EDIT_WRITE_WID)? (EDIT_WRITE_COL+EDIT_WRITE_WID-2):col1;
	        
	              
	        hig = row1-row;
	        wid = col1-col;
	        
	        hig = hig <= 0 ? 1:hig;
	        wid = wid <= 0 ? 1:wid;
	        
	        /* 
	        row = MIN(y0,y)-2 < EDIT_WRITE_ROW ? EDIT_WRITE_ROW: MIN(y0,y)-2;
	        col = MIN(x0,x)-2 < 3 ? 3 : MIN(x0,x)-2;
	        hig = row+ABS(y-y0)+4 > EDIT_WRITE_ROW+EDIT_WRITE_HIG ? EDIT_WRITE_ROW+EDIT_WRITE_HIG-row : ABS(y-y0)+4;
	        wid = col+ABS(x-x0)+4 > EDIT_WRITE_COL+EDIT_WRITE_WID ? EDIT_WRITE_COL+EDIT_WRITE_WID-col : ABS(x-x0)+4;     
	        */ 
	        
	        alpha_set_push_queue(row,col,hig,wid);
	        alpha_set_mem_type(ALPHADISPLAY); 
	        scratch_sdram_draw_line(y0,x0,y,x,2,pCtrPan->tSysCtr.writeClr);    
	     }
	     else
	     {//取消本次识别
	        //pCtrPan->HandWordShowFlag = 0;
	     }
	     ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pLabel))
        {//取消本次识别
            //pCtrPan->HandWordShowFlag = 0;
        }
               
        ret = VIEW_PULL_END;
         
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;
	    pCtrPan->HandWordShowFlag = 0;
	}
	
	return ret;
}


S32 edit_Arrow_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 lParam ;
    tGRAPBUTTON *pButton;
	int x, y, ret;

	pButton = (tGRAPBUTTON *)pView;
    
	lParam = pEvent->lParam ;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;

	if(GRAP_INWIDGET_RANGE(x,y,pButton))
    {  
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}		
    }
	
	return PARSED;  
}


static S32 Arrow_btn_enter(void *pView,U16 state)
{
	int ret, i,j, len, total;
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tCTR_PANEL  *pCtrPan;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pBtn = (tGRAPBUTTON *)pView;
	
	pDesk = (tGRAPDESKBOX *)(pBtn->pParent);
	pCtrPan = (tCTR_PANEL *)pDesk->pdata;
	//hyUsbPrintf("arrow button\r\n");
	
	if (pBtn->view_attr.id == BUTTON_ID_7_EDIT)//left
	{
	//hyUsbPrintf("left\r\n");
		if (Show_start == 0)
		{
			return SUCCESS;
		}
		len = ARROW_F_TAIL;
		i = 0;
		while(i < Show_start)
		{
			for (j=i; j<Show_start; j++)
			{
				len += 12*strlen(gu_Result.HanZi[j])+8;
			}
			if (len <= ARROW_T_FRONT)
			{
				break;
			}
			i++;
			len = ARROW_F_TAIL;
		}
		
		//drvLcd_Background2DisplayMemory(30, 240, 288, 0);
		
		Show_Max_HzNum = Show_start - i;
		Show_start = i;
		
		if (Show_start == 0)
		{
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
			edit_draw_arrow(1, 294);
		}
		else
		{
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 1;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
			edit_draw_arrow(2, 294);
		}
		
		for (i=0; i<MAX_SHOW_HZ; i++)
		{
		 	pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
		}
		
		if (Show_Max_HzNum > 0)
		{
			for (i=0; i<Show_Max_HzNum; i++)
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 1;
 				pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName = gu_Result.HanZi[i+Show_start];
 				
 				len = strlen(gu_Result.HanZi[i+Show_start]);
 				
 				if (i == 0)
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = ARROW_F_TAIL;
 				}
 				else
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewWidth + 8;
 				}
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth = len*12;
 				
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 320;//pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight;
 			
 				pCtrPan->gu_ShowHZ_sx[i]->draw(pCtrPan->gu_ShowHZ_sx[i], 0, 0);
			}
		}
	}
	else if (pBtn->view_attr.id == BUTTON_ID_8_EDIT)//right
	{
	//hyUsbPrintf("right\r\n");
		if ((Show_start + Show_Max_HzNum) >= gu_Result.HZ_Num)
		{
			return ;
		}
		Show_start += Show_Max_HzNum;
		
		total = 0;
		len = ARROW_F_TAIL;
		i = Show_start;
		while(i < gu_Result.HZ_Num)
		{
			len += 12*strlen(gu_Result.HanZi[i])+8;
			if (len <= ARROW_T_FRONT)
			{
				total++;
			}
			else
			{
				break;
			}
			i++;
		}
		Show_Max_HzNum = total;
		
		//hyUsbPrintf("ShowStart=%d   ShowMaxNum=%d  HZ_Num=%d\r\n", Show_start, Show_Max_HzNum, gu_Result.HZ_Num);
		
		//drvLcd_Background2DisplayMemory(30, 240, 288, 0);
		
		if ((Show_start + Show_Max_HzNum) >= gu_Result.HZ_Num)
		{
		//hyUsbPrintf("draw left\r\n");
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 1;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 0;
			edit_draw_arrow(0, 294);
		}
		else
		{
		//hyUsbPrintf("draw two\r\n");
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 1;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
			edit_draw_arrow(2, 294);
		}
		
		for (i=0; i<MAX_SHOW_HZ; i++)
		{
		 	pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
		}
		
		if (Show_Max_HzNum > 0)
		{
			for (i=0; i<Show_Max_HzNum; i++)
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 1;
 				pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName = gu_Result.HanZi[i+Show_start];
 				
 				len = strlen(gu_Result.HanZi[i+Show_start]);
 				
 				if (i == 0)
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = ARROW_F_TAIL;
 				}
 				else
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewWidth + 8;
 				}
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth = len*12;
 				
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 320;//pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight;
 			
 				pCtrPan->gu_ShowHZ_sx[i]->draw(pCtrPan->gu_ShowHZ_sx[i], 0, 0);
			}
		}
	}
	
	return RETURN_REDRAW;
}

S32 edit_Assin_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
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
	    pButton->state = STATE_PRESS;
		pButton->draw(pButton, 0, 0);   		
	        
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//在有效范围内
	         pButton->state = STATE_PRESS;
		     pButton->draw(pButton, 0, 0);		
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

static S32 edit_btn_enter_Assin(void *pView,U16 state)
{
    tGRAPBUTTON *pButton;
	tCTR_PANEL  *pCtrPan;
	tGRAPVIEW *pTempView,*pSubView;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	S32 ret = FAIL;
	int i, len;
	tGRAPDESKBOX *pDesk;
	U8 pStr[36];
	GRAP_EVENT	event;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pButton = (tGRAPBUTTON *)pView;
	
	pDesk = pButton->pParent;
	pCtrPan = (tCTR_PANEL *)pDesk->pdata;
	
	pTempView = pDesk->pNext;
    while(pTempView)
    {
    	//先判断是不是panel
    	if(pTempView->view_attr.enable && Panel_IfPanel(pTempView))
    	{
    		pPanel = (tPANEL *)pTempView;
    		pPriv  = &(pPanel->PanelPriv);
    		pSubView = pPriv->pSubView;
    		while(pSubView)
    		{
    			if(pSubView->view_attr.enable && pSubView->view_attr.curFocus)
    			{
    				event.id =pSubView->view_attr.id;
		            event.message = VIEW_DIGIT;
		            event.wParam = 0 ;
		            
		            len = strlen(pButton->btn_privattr.pName);
		            
		            for (i=0; i<len; i+=2)
		            {
		                if(pButton->btn_privattr.pName[i+1] > 0)
		                {
		                    event.lParam =(pButton->btn_privattr.pName[i]<<8)|(pButton->btn_privattr.pName[i+1]) ;
		                }
		                else
		                {
		                   event.lParam =pButton->btn_privattr.pName[i]; 
		                }
		                Grap_SendMessage(pSubView, &event);
		            }
		            
		            memset(pStr, 0, sizeof(pStr));
		            strcpy(pStr, pButton->btn_privattr.pName);
		            
		            break;
    			}
    			pSubView = pSubView->pNext;
    		}
    		if(pSubView != NULL)
    		{
    			break;
    		}
    	}
    	else if(1 == pTempView->view_attr.enable && pTempView->view_attr.curFocus)
    	{
            event.id =pTempView->view_attr.id;
            event.message = VIEW_DIGIT;
            event.wParam = 0 ;
            
            len = strlen(pButton->btn_privattr.pName);
            
            for (i=0; i<len; i+=2)
            {
                if(pButton->btn_privattr.pName[i+1] > 0)
                {
                    event.lParam =(pButton->btn_privattr.pName[i]<<8)|(pButton->btn_privattr.pName[i+1]) ;
                }
                else
                {
                   event.lParam =pButton->btn_privattr.pName[i]; 
                }
                Grap_SendMessage(pTempView, &event);
            }
            
            memset(pStr, 0, sizeof(pStr));
            strcpy(pStr, pButton->btn_privattr.pName);
            
            break;
    	}
    	pTempView = pTempView->pNext;
    }
	
	edit_DisableIdentifyBtn(pCtrPan);
    for (i=0; i<MAX_SHOW_HZ; i++)
	{
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 0;
	}
    pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
	pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 0;
	
    ReDraw(pDesk, 0, 0);
	
	InputRange_ChangePriority(pStr, 0, 1);
	
	//hyUsbPrintf("##str=%s\r\n", pStr);
	
	if(0 == InputRange_GetNextWord(pStr,&gu_Result))
	{
		Show_start = 0;
		edit_check_hz(&gu_Result);
		
		//drvLcd_Background2DisplayMemory(30, 240, 288, 0);
				
		for (i=0; i<Show_Max_HzNum; i++)
		{
			len = strlen(gu_Result.HanZi[i]);
			
			if (i == 0)
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = ARROW_F_TAIL;
			}
			else
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewWidth + 8;
			}
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth = len*12;
		//hyUsbPrintf("len = %d  str = %s  x1 = %d  width = %d \r\n", len,gu_Result.HanZi[i],
		//			pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1,pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth);	
			
			if(pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1+pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth > ARROW_T_FRONT)
			{
				break;
			}
			
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 1;
			pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName = gu_Result.HanZi[i];
			
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 320;//pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight;
		
			pCtrPan->gu_ShowHZ_sx[i]->draw(pCtrPan->gu_ShowHZ_sx[i], 0, 0);
		}
		if (Show_start == 0 && gu_Result.HZ_Num > Show_Max_HzNum)
		{
			//hyUsbPrintf("##draw arrow\r\n");
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
			edit_draw_arrow(1, 294);
		}
	}
		
	return RETURN_REDRAW;
}
static S32 edit_btn_enter_Delete(void *pView,U16 state)
{
    tGRAPBUTTON *pButton;
	tGRAPVIEW *pTempView, *pSubView;
	tGRAPDESKBOX *pDesk;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	GRAP_EVENT	event;

	pButton = (tGRAPBUTTON *)pView;
	pDesk = pButton->pParent;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pTempView = pDesk->pNext;
    while(pTempView)
    {
    	//先判断是不是panel
    	if(pTempView->view_attr.enable && Panel_IfPanel(pTempView))
    	{
    		pPanel = (tPANEL *)pTempView;
    		pPriv  = &(pPanel->PanelPriv);
    		pSubView = pPriv->pSubView;
    		while(pSubView)
    		{
    			if(pSubView->view_attr.enable && pSubView->view_attr.curFocus)
    			{
    				event.id =pSubView->view_attr.id;
		            event.message = VIEW_DELETE;
		            event.wParam = 0 ;
		            Grap_SendMessage(pSubView, &event);
		            return SUCCESS;
    			}
    			pSubView = pSubView->pNext;
    		}
    	}
    	else if(1 == pTempView->view_attr.enable && pTempView->view_attr.curFocus)
    	{
            event.id =pTempView->view_attr.id;
            event.message = VIEW_DELETE;
            event.wParam = 0 ;
            Grap_SendMessage(pTempView, &event);
    	}
    	pTempView = pTempView->pNext;
    }
    
    return SUCCESS;
}


static S32 edit_btn_enter_Identify(void *pView,U16 state)
{
    tGRAPBUTTON *pButton;
    tCTR_PANEL  *pCtrPan;
	tGRAPVIEW 	*pTempView,*pSubView;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	S32 ret = FAIL;
	int i, len;
	tGRAPDESKBOX *pDesk;
	U8 pStr[36];
	GRAP_EVENT	event;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pButton = (tGRAPBUTTON *)pView;
	
	pDesk = pButton->pParent;
	pCtrPan = (tCTR_PANEL *)pDesk->pdata;
	
	pTempView = pDesk->pNext;
    while(pTempView)
    {
    	//先判断是不是panel
    	if(pTempView->view_attr.enable && Panel_IfPanel(pTempView))
    	{
    		pPanel = (tPANEL *)pTempView;
    		pPriv  = &(pPanel->PanelPriv);
    		pSubView = pPriv->pSubView;
    		while(pSubView)
    		{
    			if(pSubView->view_attr.enable && pSubView->view_attr.curFocus)
    			{
    				event.id =pSubView->view_attr.id;
		            event.message = VIEW_DELETE;
		            event.wParam = 0 ;
		            Grap_SendMessage(pSubView, &event);
		            
		            event.id =pSubView->view_attr.id;
		            event.message = VIEW_DIGIT;
		            event.wParam = 0 ;
		            
		            len = strlen(pButton->btn_privattr.pName);
		            //hyUsbPrintf("##len=%d  pName=%s\r\n", len, pButton->btn_privattr.pName);
		            
		            for (i=0; i<len; i+=2)
		            {
		                if(pButton->btn_privattr.pName[i+1] > 0)
		                {
		                    event.lParam =(pButton->btn_privattr.pName[i]<<8)|(pButton->btn_privattr.pName[i+1]) ;
		                }
		                else
		                {
		                   event.lParam =pButton->btn_privattr.pName[i]; 
		                }
		                Grap_SendMessage(pSubView, &event);
		            }
		            
		            memset(pStr, 0, sizeof(pStr));
		            strcpy(pStr, pButton->btn_privattr.pName);
		            break;
    			}
    			pSubView = pSubView->pNext;
    		}
    		if(pSubView != NULL)
    		{
    			break;
    		}
    	}
    	else if(1 == pTempView->view_attr.enable && pTempView->view_attr.curFocus)
    	{
            event.id =pTempView->view_attr.id;
            event.message = VIEW_DELETE;
            event.wParam = 0 ;
            Grap_SendMessage(pView, &event);
            
            event.id =pTempView->view_attr.id;
            event.message = VIEW_DIGIT;
            event.wParam = 0 ;
            
            len = strlen(pButton->btn_privattr.pName);
            //hyUsbPrintf("##len=%d  pName=%s\r\n", len, pButton->btn_privattr.pName);
            
            for (i=0; i<len; i+=2)
            {
                if(pButton->btn_privattr.pName[i+1] > 0)
                {
                    event.lParam =(pButton->btn_privattr.pName[i]<<8)|(pButton->btn_privattr.pName[i+1]) ;
                }
                else
                {
                   event.lParam =pButton->btn_privattr.pName[i]; 
                }
                Grap_SendMessage(pView, &event);
            }
            
            memset(pStr, 0, sizeof(pStr));
            strcpy(pStr, pButton->btn_privattr.pName);
            
            break;
    	}
    	pTempView = pTempView->pNext;
    }
    
    edit_DisableIdentifyBtn(pCtrPan);
    for (i=0; i<MAX_SHOW_HZ; i++)
	{
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = 0;
		pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 0;
	}
    pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
	pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 0;
    
    ReDraw(pDesk, 0, 0);
	
	InputRange_ChangePriority(pStr, 0, 1);
	
	//hyUsbPrintf("##str=%s\r\n", pStr);
	
	if(0 == InputRange_GetNextWord(pStr,&gu_Result))
	{
		Show_start = 0;
		edit_check_hz(&gu_Result);
				
		//drvLcd_Background2DisplayMemory(30, 240, 288, 0);
		
		//hyUsbPrintf("##Total=%d   Show=%d\r\n", gu_Result.HZ_Num, Show_Max_HzNum);
		
		for (i=0; i<Show_Max_HzNum; i++)
		{
			len = strlen(gu_Result.HanZi[i]);
			
			if (i == 0)
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = ARROW_F_TAIL;
			}
			else
			{
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewWidth + 8;
			}
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth = len*12;
			
			if(pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1+pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth > ARROW_T_FRONT)
			{
				break;
			}
			
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 1;
			pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName = gu_Result.HanZi[i];
			
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 320;//pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight;
		
			pCtrPan->gu_ShowHZ_sx[i]->draw(pCtrPan->gu_ShowHZ_sx[i], 0, 0);
			
		}
		if (Show_start == 0 && gu_Result.HZ_Num > Show_Max_HzNum)
		{
			//hyUsbPrintf("##draw arrow\r\n");
			pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
			pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
			edit_draw_arrow(1, 294);
		}
	}
			
	return RETURN_REDRAW;
}



static S32 edit_lab_draw_none(void *pView, U32 xy, U32 wh)
{
    return SUCCESS;
} 




/*---------------------------------------------------------
*函数: edit_change_default_bk
*功能: 程序中各个地方强制修改默认背景，直接调用函数  以此为 ABC abc 123 符号 拼 手 .com
*参数: none
*返回: none
*---------------------------------------------------------*/
static void  edit_change_default_bk(tGRAPDESKBOX *pDesk,U8 BKnum) 
{
     U8 i;
     tCTR_PANEL *pCtrPan;
     
     pCtrPan = pDesk->pdata;
     
     if (NULL == pCtrPan) return;
  
    if (BKnum < INPUT_CTR_BIGABC || BKnum > INPUT_CTR_WWW)
    {
        BKnum =  INPUT_CTR_SMAABC;
    }

    if (INPUT_CTR_CHI == BKnum)
    {
        *pCtrPan->pHandLabEnable = 1;
        hyHWRecog_EnableFlag(1);    
    }
    else
    {
        *pCtrPan->pHandLabEnable = 0;
        hyHWRecog_EnableFlag(0); 
    }

    memset(ALPHA_MEMORY_OFFSET,0,ALPHA_BLOCK_SIZE*3);
    edit_enable_input_mode(pDesk,BKnum);
    pCtrPan->tSysCtr.inputMode = BKnum;
    pCtrPan->tSysCtr.status = 1; 
}


/*---------------------------------------------------------
*函数: edit_ctrl_input_mode
*功能:  <=0 临时关闭输入法，disable 否则 打开之前的输入法
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_ctrl_input_mode(int swi,tGRAPDESKBOX *pDesk, U16 color)
{
     int i;
     tCTR_PANEL *pCtrPan;
     
     pCtrPan = pDesk->pdata;
     
     if (NULL == pCtrPan) return;
     
     //090311 修改切换时备选字没有清除
     edit_disable_show_buf(pCtrPan);
     
     if (swi <= 0)
     {
           *pCtrPan->pHandLabEnable = 0;
           
           if (INPUT_CTR_CHI == pCtrPan->tSysCtr.inputMode)
           {
               hyHWRecog_EnableFlag(0);
           }
           
           pCtrPan->tSysCtr.status = 0;
           pCtrPan->alphaDisp = 0;
     }
     else
     {
          edit_change_default_bk(pDesk,pCtrPan->tSysCtr.inputMode);
          pCtrPan->alphaDisp = 1;
          
          if (INPUT_CTR_CHI == pCtrPan->tSysCtr.inputMode)
          {
              edit_write_hand_range(pCtrPan, color);
          }    
     } 
}


/*---------------------------------------------------------
*函数: edit_get_input_status
*功能: get edit status 
*参数: none
*返回: 1 : open 0:close
*---------------------------------------------------------*/
int  edit_get_input_status(tGRAPDESKBOX *pDesk)
{
    tCTR_PANEL *pCtrPan;
    int rv = 0;
    
    pCtrPan = pDesk->pdata;
    
    if (NULL != pCtrPan)
    {
         rv = pCtrPan->tSysCtr.status; 
    }
    
    return rv;
}

/*---------------------------------------------------------
*函数: edit_auto_change_input_mode
*功能: aoto change mode: close or open
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_auto_change_input_mode(tGRAPDESKBOX *pDesk, U16 color)
{
    int swi = 1;
    tCTR_PANEL *pCtrPan;
     
    pCtrPan = pDesk->pdata;
    
   // hyUsbPrintf("###########\r\n");
    
    if (NULL == pCtrPan) return;
    
    if (1 == pCtrPan->tSysCtr.status)
    {
        swi = 0;
        pCtrPan->alphaDisp = 0;
    }
    else
    {
        pCtrPan->alphaDisp = 1;
    }
       
    edit_ctrl_input_mode(swi,pDesk, gu16EditClrInput[0]);
}
/*---------------------------------------------------------
*函数: edit_disenable_input_mode
*功能: 关闭输入界面
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_change_input_mode(tGRAPDESKBOX *pDesk, U16 color,U8 onoff)
{
    int i, swi = 1;
    tCTR_PANEL *pCtrPan;
     
    pCtrPan = pDesk->pdata;
    
    //hyUsbPrintf("@@@@@@@@@@\r\n");
    
    if (NULL == pCtrPan) return;
    
    if (0 == onoff)
    {
        swi = 0;
        pCtrPan->alphaDisp = 0; 
        
        for (i=0; i<MAX_SHOW_HZ; i++)
	    {
	    	pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
	    }
	    
	    for (i=0; i<MAX_IDENTIFY_HZ; i++)
	    {
	    	pCtrPan->gu_ShowHZ_identify[i]->view_attr.enable = 0;
	    }
	    
	    for (i=0; i<2; i++)
	    {
	    	pCtrPan->gu_ShowHZ_arrow[i]->view_attr.enable = 0;
	    }
	    
	    pCtrPan->gu_DeleteBtn->view_attr.enable = 0;
	    
        edit_ctrl_input_mode(swi,pDesk, gu16EditClrInput[0]);
        ReDraw(pDesk,0,0);
    }
    else
    {
    	pCtrPan->gu_DeleteBtn->view_attr.enable = 1;
    	pCtrPan->alphaDisp = 1;
    	edit_ctrl_input_mode(swi,pDesk, gu16EditClrInput[0]);
    }  
}
/*---------------------------------------------------------
*函数: edit_write_hand_range
*功能: 话手写区域位置
*参数: none
*返回: none
*---------------------------------------------------------*/
static void edit_write_hand_range(tCTR_PANEL *pCtrPan, U16 color)  //color = gtHyc.ClrInput[0];//gtBtnBmpCtr.normal;
{
    U16 srow,erow,scol,ecol,mrow,mcol;

    //srow =  3;
	//erow =  255;
	//scol =  3;
	//ecol =  237;  
	
	srow =  3;
	erow =  317;
	scol =  3;
	ecol =  237; 	
	
	alpha_set_mem_type(ALPHADISPLAY);

	
	// -
	scratch_sdram_draw_line(srow,scol,srow,ecol,1,color);
	
	// -
	scratch_sdram_draw_line(erow,scol,erow,ecol,1,color);  
	
	// |
	scratch_sdram_draw_line(srow,scol,erow,scol,1,color); 
	// |
	scratch_sdram_draw_line(srow,ecol,erow,ecol,1,color);
	
	//增加一横线
	//scratch_sdram_draw_line(163,3,163,237,1,color);


	//scratch_sdram_draw_line(197,3,197,237,1,color);
	//scratch_sdram_draw_line(226,3,226,237,1,color);

	scratch_sdram_draw_line(258,3,258,237,1,color);

	scratch_sdram_draw_line(287,3,287,237,1,color);
	
	
	
	//1 201		20	89
    //121	19 1	153	

	//画虚线
	//drvLcd_SetColor(color,COLOR_BLACK);
	//alpha_bmp_to_disp_mem(gu8BmpHdHer,99,20,1,201);
	//alpha_bmp_to_disp_mem(gu8BmpHdVertical,23,121,153,1);
	
	//画虚线
	//drvLcd_SetColor(color,COLOR_BLACK);
	//alpha_bmp_to_disp_mem(gu8BmpHdHer,99,20,1,201);
	//alpha_bmp_to_disp_mem(gu8BmpHdVertical,23,121,153,1);
	
	//画虚线
	drvLcd_SetColor(color,COLOR_BLACK);
	alpha_bmp_to_disp_mem(gu8BmpHdHer,113,20,1,201);
	alpha_bmp_to_disp_mem(gu8BmpHdVertical,23,121,217,1);
}
void edit_DisableIdentifyBtn(tCTR_PANEL *pCtrPan)
{
	int i;
	
	for (i=0; i<MAX_IDENTIFY_HZ; i++)
	{
		pCtrPan->gu_ShowHZ_identify[i]->btn_privattr.pName = NULL;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.enable = 0;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchX1 = 0;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchY1 = 0;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchX2 = 0;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchY2 = 0;
	}
}

void edit_EnableIdentifyBtn(tCTR_PANEL *pCtrPan)
{
	int i;
	
	for (i=0; i<Show_IdentifyNum; i++)
	{
		pCtrPan->gu_ShowHZ_identify[i]->btn_privattr.pName = BufferCopy_Temp[i];
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.enable = 1;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewX1;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewY1;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewWidth;
		pCtrPan->gu_ShowHZ_identify[i]->view_attr.touchY2 = pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_identify[i]->view_attr.viewHeight;
		pCtrPan->gu_ShowHZ_identify[i]->draw(pCtrPan->gu_ShowHZ_identify[i], 0, 0);
	}
}



/*---------------------------------------------------------
*函数: edit_label_timerisr_reload
*功能: 使面板中的控件时能
*参数: none
*返回: none
*---------------------------------------------------------*/
static void edit_label_timerisr_reload(void *pView)
{

	GRAP_EVENT	event;
	int i,j, len;
	U8  BufferCopy[20]={0};
	U8  pStr[4];
	U8  tempChar;
	U8  ShowWordFlag=0;
	tCTR_PANEL *pCtrPan;
	tGRAPDESKBOX *pDesk;
	tGRAPVIEW *pTempView, *pSubView;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	int numChar;
	
	
	pDesk = ((tGRAPLABEL *)pView)->pParent;
	
	if (NULL == pDesk->pdata) return;
	
	pCtrPan = (tCTR_PANEL *)pDesk->pdata;
	
	if (pCtrPan->tSelWord.enable) return;
	    
    numChar = hyHWRecog_GetLetter(BufferCopy);
    
    if (numChar < 0) return;
    
   
 	if(1 == pCtrPan->HandWordShowFlag && BufferCopy[0] > 0) 
 	{
 	    ShowWordFlag=1;
 	    
 	    for (i=0; i<MAX_SHOW_HZ; i++)
		{
		 	pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 0;
		 	pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = 0;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = 0;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = 0;
			pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 0;
		}
		
		pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
		pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 0;
 	}
 	
 	if (1 == pCtrPan->HandWordShowFlag)
 	{
 		Show_IdentifyNum = 0;
	    memset(BufferCopy_Temp, 0, MAX_IDENTIFY_HZ*4);
	        
	    i = 0;
	    while(BufferCopy[i] != 0)
	    {
	    	BufferCopy_Temp[Show_IdentifyNum][0]=BufferCopy[i++];
	    	BufferCopy_Temp[Show_IdentifyNum][1]=BufferCopy[i++];
	    	Show_IdentifyNum++;
	    	if (Show_IdentifyNum >= MAX_IDENTIFY_HZ) break;
	    }
	    edit_DisableIdentifyBtn(pCtrPan);
	    edit_EnableIdentifyBtn(pCtrPan);
	    ReDraw(pDesk, 0, 0);
 	}
 	
 	if (BufferCopy[0] > 0x80 && ShowWordFlag == 1)
 	{
 		memset(pStr, 0, sizeof(pStr));
 		memcpy(pStr, BufferCopy, 2);
 	
 		if(0 == InputRange_GetNextWord(pStr,&gu_Result))
 		{
 			Show_start = 0;
 			edit_check_hz(&gu_Result);
 			
 			//drvLcd_Background2DisplayMemory(30, 240, 288, 0);
 			
 			//hyUsbPrintf1("Total=%d   Show=%d\r\n", gu_Result.HZ_Num, Show_Max_HzNum);
 			
 			for (i=0; i<Show_Max_HzNum; i++)
 			{
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.enable = 1;
 				pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName = gu_Result.HanZi[i];
 				
 				len = strlen(gu_Result.HanZi[i]);
 				
 				if (i == 0)
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = ARROW_F_TAIL;
 				}
 				else
 				{
 					pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 = pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i-1]->view_attr.viewWidth + 8;
 				}
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth = len*12;
 				
 				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX2 = pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth;
				pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2 = 320;//pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1 + pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight;
 			
 				pCtrPan->gu_ShowHZ_sx[i]->draw(pCtrPan->gu_ShowHZ_sx[i], 0, 0);
 			/*
 				hyUsbPrintf("pCtrPan->gu_ShowHZ_sx[%d]:area--%d  %d  %d  %d\r\n", i, pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewX1,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewY1,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewWidth,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_attr.viewHeight);
 				hyUsbPrintf("pCtrPan->gu_ShowHZ_sx[%d]:touch--%d  %d  %d  %d\r\n", i,pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchX1,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY1,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_ttr.touchX2,
 																		   	pCtrPan->gu_ShowHZ_sx[i]->view_attr.touchY2);
 				hyUsbPrintf("pCtrPan->gu_ShowHZ_sx[%d]=%s\r\n\r\n", i,pCtrPan->gu_ShowHZ_sx[i]->btn_privattr.pName);
 			*/
 			}
 			
 			
 			//hyUsbPrintf1("Show_start=%d   gu_Result.HZ_Num=%d  Show_Max_HzNum=%d\r\n", Show_start, gu_Result.HZ_Num, Show_Max_HzNum);
 			if (Show_start == 0 && gu_Result.HZ_Num > Show_Max_HzNum)
 			{
 				pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable = 0;
 				pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable = 1;
 				//hyUsbPrintf1("draw arrow\r\n");
 				edit_draw_arrow(1, 294);
 			}
 		}
 		/*
 		else
 		{
 			drvLcd_Background2DisplayMemory(30, 240, 288, 0);
 		}
 		*/
 	}
 	/*
 	else if (ShowWordFlag == 1)
 	{
 		drvLcd_Background2DisplayMemory(30, 240, 288, 0);
 	}
 	*/

    alpha_bmp_clr_block_mem(ALPHADISPLAY,0,0,257,240);
    alpha_set_push_queue(0,0,257,240);
	edit_write_hand_range(pCtrPan, ((tGRAPLABEL *)pView)->view_attr.color);
	  
	//找到焦点控件输入GBK 
    if(ShowWordFlag==1)
    {
        pTempView = pDesk->pNext;
        while(pTempView)
        {
	    	//先判断是不是panel
	    	if(pTempView->view_attr.enable && Panel_IfPanel(pTempView))
	    	{
	    		pPanel = (tPANEL *)pTempView;
	    		pPriv  = &(pPanel->PanelPriv);
	    		pSubView = pPriv->pSubView;
	    		while(pSubView)
	    		{
	    			if(pSubView->view_attr.enable && pSubView->view_attr.curFocus)
	    			{
	    				event.id =pSubView->view_attr.id;
			            event.message = VIEW_DIGIT;
			            event.wParam = 0;
			            if(BufferCopy[1] > 0)
		                {
		                    event.lParam =(BufferCopy[0]<<8)|(BufferCopy[1]) ;
		                }
		                else
		                {
		                   event.lParam =BufferCopy[0] ; 
		                }
			            Grap_SendMessage(pSubView, &event);
			            break;
	    			}
	    			pSubView = pSubView->pNext;
	    		}
	    		if(pSubView != NULL)
	    		{
	    			break;
	    		}
	    	}
    		else if(1 == pTempView->view_attr.enable && pTempView->view_attr.curFocus)
        	{
                event.id =pTempView->view_attr.id;
                event.message = VIEW_DIGIT;
                event.wParam = 0 ;
                if(BufferCopy[1] > 0)
                {
                    event.lParam =(BufferCopy[0]<<8)|(BufferCopy[1]) ;
                }
                else
                {
                   event.lParam =BufferCopy[0] ; 
                }
                Grap_SendMessage(pTempView, &event);
                break;
        	}
        	pTempView = pTempView->pNext;
        }
    }     
    	  
	pCtrPan->HandWordShowFlag = 0;
	  
	return;
}

/*---------------------------------------------------------
*函数: edit_enable_input_mode
*功能: 使面板中的控件时能
*参数: none
*返回: none
*---------------------------------------------------------*/
static void edit_enable_input_mode(tGRAPDESKBOX *pDesk,U8 num)
{
    U8 start;
	U8 length;
	int i;
	int threshold;
	tCTR_PANEL *pCtrPan;
	
	return;
	
	if (NULL == pDesk->pdata) return; 
	
	pCtrPan = pDesk->pdata;
	
	for (i=0; ; i+=3)
	{
	     if (gu16StartAttr[i] == num) break; 
	}

    start=gu16StartAttr[i+1];
	length=gu16StartAttr[i+2];	
    threshold = start+length;
    
    for (i=0; i<7; i++)
    {
        *pCtrPan->pBtnEnable[i] = 1;
    } 

    for(i=INPUT_BIG_ABS_START;i<INPUT_BTN_TOTAL;i++)
	{
        if(i >= start && i < threshold)
        {
            if(INPUT_CTR_CHI != num)
            {
                *pCtrPan->pBtnEnable[i] = 1;
            }
            else
            {
                if(i >= start+INPUT_HANDW_LEN-2 || i == start+4)
                {
                    *pCtrPan->pBtnEnable[i] = 1;  //使删除键有效
                }                               
                else
                {
                    *pCtrPan->pBtnEnable[i] = 0;//0
                }      
            }
        }
        else
        {
           *pCtrPan->pBtnEnable[i] = 0;
        }
		
	}
}

/*---------------------------------------------------------
*函数: edit_get_cur_mode
*功能: pDesk
*参数: none
*返回: none
*---------------------------------------------------------*/
int edit_get_cur_mode(tGRAPDESKBOX *pDesk)
{
      tCTR_PANEL *pCtrPan;
     
      if (NULL == pDesk->pdata) return;
     
      pCtrPan = (tCTR_PANEL *)pDesk->pdata;
      
      return pCtrPan->tSysCtr.inputMode;
}


/*---------------------------------------------------------
*函数: edit_modify_input_mode
*功能: 改变输入方式
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_modify_input_mode(tGRAPDESKBOX *pDesk, int mode, U16 color)
{
      tCTR_PANEL *pCtrPan;
     
      if (NULL == pDesk->pdata) return;
     
      pCtrPan = (tCTR_PANEL *)pDesk->pdata;
     
      //090311 防止切换时没有收起下来框
      edit_disable_show_buf(pCtrPan);
      
      if (mode < INPUT_CTR_BIGABC || mode > INPUT_CTR_WWW)
      {
          mode =  INPUT_CTR_SMAABC;
      }
      
      if (0 == pCtrPan->tSysCtr.status)
      {
           pCtrPan->tSysCtr.inputMode = mode;
           return;
      }
      
       if (INPUT_CTR_CHI == mode)
       {
	      *pCtrPan->pHandLabEnable = 1;
	      hyHWRecog_EnableFlag(1); 
	      pCtrPan->tSelWord.type = 2;
	      edit_make_alpha_value();
       }    
  
      memset(ALPHA_MEMORY_OFFSET,0,ALPHA_BLOCK_SIZE*3);
      pCtrPan->tSysCtr.inputMode = mode;
      pCtrPan->tSysCtr.status = 1;
      edit_write_hand_range(pCtrPan,gu16EditClrInput[0]);
}



S32 Edit_ButtonDraw(void *pView, U32 xy, U32 wh)
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
	
	return SUCCESS;
}


S32 Edit_DeleteButtonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	U16 viewX1,viewY1;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	
	Grap_ClearScreen(viewX1, viewY1, viewX1+pview_attr->viewWidth,viewY1+pview_attr->viewHeight);
	
	if (pButton->state == STATE_NORMAL)
	{
		Grap_WriteBMPColor(viewX1, viewY1, pview_attr->viewWidth, pview_attr->viewHeight, DeleteBmp, pbtn_privattr->cl_normal);
	}
	else if (pButton->state == STATE_PRESS)
	{
		Grap_WriteBMPColor(viewX1, viewY1, pview_attr->viewWidth, pview_attr->viewHeight, DeleteBmp, pbtn_privattr->cl_focus);
	}
	
	return SUCCESS;
}


S32 edit_ArrowButtonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGRAPDESKBOX *pDesk;
	tCTR_PANEL  *pCtrPan;
	
	pButton = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)(pButton->pParent);
	pCtrPan = (tCTR_PANEL *)pDesk->pdata;
	
	if (pCtrPan->gu_ShowHZ_arrow[0]->view_attr.enable == 1)
	{
		edit_draw_arrow(0, 294);
	}
	if (pCtrPan->gu_ShowHZ_arrow[1]->view_attr.enable == 1)
	{
		edit_draw_arrow(1, 294);
	}
	

	return SUCCESS;
}



/*---------------------------------------------------------
*函数: edit_creat_panel   
*功能: 创建输入法			 
*参数: pDesk:附着窗体 mode:初始显示状态 ...
*返回: 创建
*---------------------------------------------------------*/
void edit_creat_panel(tGRAPDESKBOX *pDesk, int mode, Button_Return_Word returnWord, edit_change_focus changeFocus)
{
    tGRAPBUTTON *pBtn;
    tGRAPLABEL *pLabel;
    int i;
    int index;
    tCTR_PANEL *pCtrPan;
    int id,reserve;
    U16 cNormal,cPress,cSpcNormal,cSpcPress;
 	
 	edit_init();
 	
    pDesk->pdata = (char *)malloc(sizeof(tCTR_PANEL));

    pCtrPan = (tCTR_PANEL *)pDesk->pdata;
    
    if (NULL == pCtrPan) return;
    
    memset(pCtrPan,0,sizeof(tCTR_PANEL));
    
    pCtrPan->tRect.row = ALPHA_SHOW_ROW;
    pCtrPan->tRect.col = ALPHA_SHOW_COL;
    pCtrPan->tRect.hig = ALPHA_SHOW_HIG;
    pCtrPan->tRect.wid = ALPHA_SHOW_WID;
    
    pCtrPan->alphaDisp = 1;
    pCtrPan->edit_return_word = returnWord;
    pCtrPan->edit_change_widget_focus = changeFocus;
    
    pCtrPan->tSysCtr = gtBtnBmpCtr;
    
    cNormal    = gu16EditClrInput[0];
    cPress     = gu16EditClrInput[1];
    cSpcNormal = gu16EditClrInput[2];
    cSpcPress  = gu16EditClrInput[3];
    
    pLabel=Grap_InsertLabel(pDesk,&EditLabelStruct[0],&LabelPriStruct[0]);
    pCtrPan->pHandLabEnable = &pLabel->view_attr.enable;
    pLabel->draw = edit_lab_draw_none;
    pLabel->destroy = edit_lab_destroy;       
    pLabel->recvPullMsg = edit_lab_recv_pull_msg;
    pLabel->labelTimerISR = edit_label_timerisr_reload;
    pLabel->view_attr.color	= gu16EditClrInput[0];	 /* 这个参数 上面的函数要使用*/
    
    pLabel->label_timer.enable = 1;
    pLabel->label_timer.TimerLft = 1;
    pLabel->label_timer.TimerTot = 1;
    
    Show_Max_HzNum = 0;
	Show_start = 0;
    
    for(i=0; i<6; i++)
    {
        pBtn = Grap_InsertButton(pDesk,&EditButtonStruct[i],&gtBtnPriAttr[0]);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        pBtn->draw			= Edit_ButtonDraw; 
        pBtn->pressEnter	= edit_btn_enter_Assin;
        //pBtn->pressEnter_param = ptHyc;
        pBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
        
        pBtn->btn_privattr.cl_focus	 = cSpcPress;
        pBtn->btn_privattr.cl_normal = cSpcNormal;
        pBtn->btn_privattr.font_size = FONTSIZE24X24;
        pBtn->btn_privattr.position = SHOWSTRING_LIAN_MID;
      
        /*
        id = pBtn->view_attr.id; 
        reserve = pBtn->view_attr.reserve;
        
        if (reserve == INPUT_CTR_CHI || reserve == INPUT_CTR_IME)
        {
            //以防出错
            if (ptHyc->typeChi != INPUT_CTR_CHI && ptHyc->typeChi != INPUT_CTR_IME )
            {
                ptHyc->typeChi = INPUT_CTR_CHI;
            }
            
            pBtn->view_attr.reserve = ptHyc->typeChi;
        }
        pCtrPan->pBtnEnable[i] = 1;
       */
        
        pCtrPan->gu_ShowHZ_sx[i] = pBtn;
    }
    
    for (i=0; i<2; i++)
    {
    	pBtn = Grap_InsertButton(pDesk,&EditButtonStruct[i+6],&gtBtnPriAttr[0]);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        //pBtn->pressEnter_param = ptHyc;
        //pBtn->recvPullMsg	= edit_Arrow_ButtonRecvPullMsg;
        pBtn->pressEnter = Arrow_btn_enter;
        pBtn->draw = edit_ArrowButtonDraw;
        
    	//*pCtrPan->pBtnEnable[i+6] = 1;
    	pCtrPan->gu_ShowHZ_arrow[i] = pBtn;
    }
    
    for(i=0; i<6; i++)
    {
        pBtn = Grap_InsertButton(pDesk,&EditButtonStruct[i+8],&gtBtnPriAttr[0]);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        pBtn->draw			= Edit_ButtonDraw; 
        pBtn->pressEnter	= edit_btn_enter_Identify;
        //pBtn->pressEnter_param = ptHyc;
        pBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
        
        pBtn->btn_privattr.cl_focus	 = cSpcPress;
        pBtn->btn_privattr.cl_normal = cSpcNormal;
        pBtn->btn_privattr.font_size = FONTSIZE24X24;
        pBtn->btn_privattr.position = SHOWSTRING_LIAN_MID;
        
        pCtrPan->gu_ShowHZ_identify[i] = pBtn;
    }
    
    
    pCtrPan->gu_DeleteBtn = Grap_InsertButton(pDesk,&EditButtonStruct[14],&gtBtnPriAttr[0]);
    pCtrPan->gu_DeleteBtn->draw			= Edit_DeleteButtonDraw; 
    pCtrPan->gu_DeleteBtn->pressEnter	= edit_btn_enter_Delete;
    //pCtrPan->gu_DeleteBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
    pCtrPan->gu_DeleteBtn->btn_privattr.cl_focus	 = cSpcPress;//0xFFE0;
    pCtrPan->gu_DeleteBtn->btn_privattr.cl_normal = cSpcNormal;//0xF800;
    
    
    pCtrPan->tSysCtr.status = 1;
    edit_modify_input_mode(pDesk,mode, gu16EditClrInput[0]);
    
    return ;
}

#if 0

tGRAPVIEW *editlinkativs ;
U8         edittotalativs;

void edit_creatInsert_panel(tGRAPDESKBOX *pDesk, int mode, Button_Return_Word returnWord, edit_change_focus changeFocus)
{
    tGRAPBUTTON *pBtn;
    tGRAPLABEL *pLabel;
    tGRAPVIEW   *pViewTem = NULL ;
    int i;
    int index;
    tCTR_PANEL *pCtrPan;
    int id,reserve;
    U16 cNormal,cPress,cSpcNormal,cSpcPress;
 
    pDesk->pdata = (char *)malloc(sizeof(tCTR_PANEL));

    pCtrPan = (tCTR_PANEL *)pDesk->pdata;
    
    if (NULL == pCtrPan) return;
    
    memset(pCtrPan,0,sizeof(tCTR_PANEL));
    
    pCtrPan->tRect.row = ALPHA_SHOW_ROW;
    pCtrPan->tRect.col = ALPHA_SHOW_COL;
    pCtrPan->tRect.hig = ALPHA_SHOW_HIG;
    pCtrPan->tRect.wid = ALPHA_SHOW_WID;
    
    pCtrPan->alphaDisp = 1;
    pCtrPan->edit_return_word = returnWord;
    pCtrPan->edit_change_widget_focus = changeFocus;
    
    pCtrPan->tSysCtr = gtBtnBmpCtr;   
    
    cNormal    = gu16EditClrInput[0];
    cPress     = gu16EditClrInput[1];
    cSpcNormal = gu16EditClrInput[2];
    cSpcPress  = gu16EditClrInput[3];
    
    pLabel=Grap_CreateLabel(&EditLabelStruct[0],&LabelPriStruct[0]);
    pCtrPan->pHandLabEnable = &pLabel->view_attr.enable;
    pLabel->draw = edit_lab_draw_none;
    pLabel->destroy = edit_lab_destroy;       
    pLabel->recvPullMsg = edit_lab_recv_pull_msg;
    pLabel->labelTimerISR = edit_label_timerisr_reload;
    pLabel->view_attr.color	= gu16EditClrInput[0];	 /* 这个参数 上面的函数要使用*/
    
    pLabel->label_timer.enable = 1;
    pLabel->label_timer.TimerLft = 1;
    pLabel->label_timer.TimerTot = 1;
    
    pLabel->pNext  = pViewTem ;
	pViewTem       = (tGRAPVIEW *)pLabel ;
    
    
    Show_Max_HzNum = 0;
	Show_start = 0;
    
    for(i=0; i<6; i++)
    {
        pBtn = Grap_CreateButton(&EditButtonStruct[i],&gtBtnPriAttr[0],NULL);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        pBtn->draw			= Edit_ButtonDraw; 
        pBtn->pressEnter	= edit_btn_enter_Assin;
        //pBtn->pressEnter_param = ptHyc;
        pBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
        
        pBtn->btn_privattr.cl_focus	 = cSpcPress;
        pBtn->btn_privattr.cl_normal = cSpcNormal;
        pBtn->btn_privattr.font_size = FONTSIZE24X24;
        pBtn->btn_privattr.position = SHOWSTRING_LIAN_MID;
      
        pBtn->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pBtn ;
        
        pCtrPan->gu_ShowHZ_sx[i] = pBtn;
    }
    
    for (i=0; i<2; i++)
    {
    	pBtn = Grap_CreateButton(&EditButtonStruct[i+6],&gtBtnPriAttr[0],NULL);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        //pBtn->pressEnter_param = ptHyc;
        //pBtn->recvPullMsg	= edit_Arrow_ButtonRecvPullMsg;
        pBtn->pressEnter = Arrow_btn_enter;
        pBtn->draw = edit_ArrowButtonDraw;
        
        pBtn->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pBtn ;
		
    	pCtrPan->gu_ShowHZ_arrow[i] = pBtn;
    }
    
    for(i=0; i<6; i++)
    {
        pBtn = Grap_CreateButton(&EditButtonStruct[i+8],&gtBtnPriAttr[0],NULL);
        //pCtrPan->pBtnEnable[i] = &pBtn->view_attr.enable;        
        pBtn->draw			= Edit_ButtonDraw; 
        pBtn->pressEnter	= edit_btn_enter_Identify;
        //pBtn->pressEnter_param = ptHyc;
        pBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
        
        pBtn->btn_privattr.cl_focus	 = cSpcPress;
        pBtn->btn_privattr.cl_normal = cSpcNormal;
        pBtn->btn_privattr.font_size = FONTSIZE24X24;
        pBtn->btn_privattr.position = SHOWSTRING_LIAN_MID;
        
        pBtn->pNext  = pViewTem ;
		pViewTem       = (tGRAPVIEW *)pBtn ;
        
        pCtrPan->gu_ShowHZ_identify[i] = pBtn;
    }
    
    
    pCtrPan->gu_DeleteBtn = Grap_CreateButton(&EditButtonStruct[14],&gtBtnPriAttr[0],NULL);
    pCtrPan->gu_DeleteBtn->draw			= Edit_DeleteButtonDraw; 
    pCtrPan->gu_DeleteBtn->pressEnter	= edit_btn_enter_Delete;
    //pCtrPan->gu_DeleteBtn->recvPullMsg	= edit_Assin_ButtonRecvPullMsg;
    pCtrPan->gu_DeleteBtn->btn_privattr.cl_focus	 = cSpcPress;//0xFFE0;
    pCtrPan->gu_DeleteBtn->btn_privattr.cl_normal = cSpcNormal;//0xF800;
    
    pCtrPan->gu_DeleteBtn->pNext  = pViewTem ;
	pViewTem       = (tGRAPVIEW *)pCtrPan->gu_DeleteBtn ;
    
    pCtrPan->tSysCtr.status = 1;
    edit_modify_input_mode(pDesk,mode, gu16EditClrInput[0]);
    
    ComWidget_Link2Desk(pDesk,pViewTem);
	
	editlinkativs = pViewTem ;
	edittotalativs= 1 + 6 + 6 + 2 + 1;
    
    
    return ;
}

void edit_LeftFromDeskandDesdray(tGRAPDESKBOX *pDesk)
{
	tGRAPVIEW *pParentView;
	tGRAPVIEW *pSubChi,*pSubDele;
	 
	U32 i;
	
	if(NULL == pDesk)
	{
		return ;
	}
		
	pParentView = (tGRAPVIEW *)pDesk;
	pSubChi = (tGRAPVIEW *)editlinkativs;
	
	for(i=0; i<edittotalativs; i++)
	{
		if(i == edittotalativs-1)
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
	free(pDesk->pdata);
	pDesk->pdata = NULL ;
	
	return ;
}


/*---------------------------------------------------------
*函数: edit_get_special_string
*功能: 程序中是长字符串的时候返回这个值，否则返回none
*参数: none
*返回: none
*---------------------------------------------------------*/
char *edit_get_special_string(U16 word, void *pView)
{

    tGRAPDESKBOX *pDesk = ((tGRAPVIEW * )pView)->pParent;
    char *pStr = NULL;
    tINPUT_BMP_CTR *pSysC;  
        
    pSysC = &((tCTR_PANEL *)(pDesk->pdata))->tSysCtr;
    
    if (INPUT_CTR_WWW == pSysC->inputMode && word >= INPUT_NET_VALUE_1 && word <= INPUT_NET_VALUE_B)
    {
        pStr = gu8EditSpcStr[word-INPUT_NET_VALUE_1];
    }
    else if ((INPUT_CTR_SIGN == pSysC->inputMode || INPUT_CTR_CHI == pSysC->inputMode) && word == 0x85)
    {
        pStr = gu8EditSpcStr[11]; 
    }
  
    
    return pStr;
}

/*---------------------------------------------------------
*函数: edit_bak_panel
*功能: 进入下一个含有输入法的Desk前先将上一个Desk的输入法全局变量备份 (只支持2层Desk)
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_bak_panel(U8 index)
{
	U8  i; 
	
	for(i = 0 ; i < MAX_SHOW_HZ ; i++)
	{
		gu_ShowHZ_sx_Bak[index][i] = gu_ShowHZ_sx[i];
	}
	
	gu_ShowHZ_arrow_Bak[index][0] = gu_ShowHZ_arrow[0] ;
	gu_ShowHZ_arrow_Bak[index][1] = gu_ShowHZ_arrow[1] ;
	
	for(i = 0 ; i < MAX_IDENTIFY_HZ ; i++)
	{
		gu_ShowHZ_identify_Bak[index][i] = gu_ShowHZ_identify[i];
	}
	gu_DeleteBtn_Bak[index] = gu_DeleteBtn ;
}
/*---------------------------------------------------------
*函数: edit_last_panel
*功能: 退出一个含有输入法的Desk后要将上一个Desk的输入法全局变量复原 (只支持2层Desk)
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_set_panel(U8 index)
{
	U8  i; 
	
	for(i = 0 ; i < MAX_SHOW_HZ ; i++)
	{
		gu_ShowHZ_sx[i] = gu_ShowHZ_sx_Bak[index][i];
	}
	
	gu_ShowHZ_arrow[0] = gu_ShowHZ_arrow_Bak[index][0] ;
	gu_ShowHZ_arrow[1] = gu_ShowHZ_arrow_Bak[index][1] ;
	
	for(i = 0 ; i < MAX_IDENTIFY_HZ ; i++)
	{
		gu_ShowHZ_identify[i] = gu_ShowHZ_identify_Bak[index][i];
	}
	gu_DeleteBtn = gu_DeleteBtn_Bak[index] ;
	
	Show_Max_HzNum = 0;
	Show_start = 0;
}

#endif


/*---------------------------------------------------------------------------------
 * 090506 主要改动为第一次读取35个汉字,之后读取30个汉字，导致某些宏不再一致，具体需
 *        参考源代码

---------------------------------------------------------------------------------*/