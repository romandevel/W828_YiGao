#include "hyTypes.h"
#include "grap_api.h"
#include "edit.h"
#include "constEditData.h"
#include "selWord.h"
#include "AlphaDisplay.h"
#include "DisplayMemory.h"


const U16 gSelHBtnAttr[]=
{
 15, 17, 24, 24,  4, 15, 47, 43,
 55, 17, 24, 24, 48, 15, 87, 43,
 95, 17, 24, 24, 88, 15,127, 43,
135, 17, 24, 24,128, 15,167, 43,
175, 17, 24, 24,168, 15,207, 43,
 15, 46, 24, 24,  4, 44, 47, 72,
 55, 46, 24, 24, 48, 44, 87, 72,
 95, 46, 24, 24, 88, 44,127, 72,
135, 46, 24, 24,128, 44,167, 72,
175, 46, 24, 24,168, 44,207, 72,
 15, 75, 24, 24,  4, 73, 47,101,
 55, 75, 24, 24, 48, 73, 87,101,
 95, 75, 24, 24, 88, 73,127,101,
135, 75, 24, 24,128, 73,167,101,
175, 75, 24, 24,168, 73,207,101,
 15,104, 24, 24,  4,102, 47,130,
 55,104, 24, 24, 48,102, 87,130,
 95,104, 24, 24, 88,102,127,130,
135,104, 24, 24,128,102,167,130,
175,104, 24, 24,168,102,207,130,
 15,133, 24, 24,  4,131, 47,159,
 55,133, 24, 24, 48,131, 87,159,
 95,133, 24, 24, 88,131,127,159,
135,133, 24, 24,128,131,167,159,
175,133, 24, 24,168,131,207,159,
 15,162, 24, 24,  4,160, 47,188,
 55,162, 24, 24, 48,160, 87,188,
 95,162, 24, 24, 88,160,127,188,
135,162, 24, 24,128,160,167,188,
175,162, 24, 24,168,160,207,188,

209, 20, 23, 12,209,  3,240, 50, 
209,172, 23, 12,209,160,240,185,
};



/*---------------------------------------------------------
*函数: sel_ime_enable_panel   
*功能: 是否禁止或打开面板按键		 
*参数: none
*返回: none
*---------------------------------------------------------*/
static void sel_ime_enable_panel(tGRAPDESKBOX *pDesk, int act)
{
    tCTR_PANEL *pCtrPan;   
    int total;
    int i;
    int flag;
    
    pCtrPan = pDesk->pdata;
    
    if (NULL == pCtrPan) return; 
    
    if (act)
    {
        flag = 1;
    }
    else
    {
        flag = 0;
    }
    
    for (i=INPUT_IME_START; i<INPUT_IME_START+INPUT_IME_LEN-7; i++)
	{
	    *pCtrPan->pBtnEnable[i] = flag;
	}
    
    
}

/*---------------------------------------------------------
*函数: sel_ime_show_word   
*功能: 显示联想或备选字符串输入法		 
*参数: none
*返回: none
*---------------------------------------------------------*/
static void sel_ime_show_word(tGRAPBUTTON *pBtn)
{
    GRAP_EVENT	event;
    int i,len;    
    tCTR_PANEL *pCtrPan;   
    tGRAPDESKBOX *pDesk;
    tASSN_CTR *pAssn;
    //tPICINFO tPic;
    tRES_PARA tRes;
    char *bakM,*dispM,*tempM;
    U16 *pBkClr;
	U16 picAttr[]={YD_OPEN_BACKGROUND_EN,14, 7};
 
    pDesk = pBtn->pParent;
    pCtrPan = pDesk->pdata;

    pAssn = &pCtrPan->tAssnCtr;
    
    if (1 == pCtrPan->tIme.type)
    {
    	if (1 != pAssn->flag) return;
    }
    
    
    //加载背景
    //YD_OPEN_BACKGROUND_EN
    bakM = ALPHA_MEMORY_OFFSET;
    dispM = bakM+ALPHA_BLOCK_SIZE;
    tempM = dispM+ALPHA_BLOCK_SIZE;
    
    //Grap_GetPicInfoById(YD_OPEN_BACKGROUND_EN, &tPic);
    //drvLcd_Nand2Sdram(tRes.offset, tRes.width*tRes.high*2, tempM);       
	res_load_bmp_info(picAttr[0],&tRes);
    res_load_bmp_to_buf(picAttr[0],tempM);
    
    len = tRes.width*tRes.high;
    pBkClr = (U16 *)tempM;
    for (i=0; i<len; i++,pBkClr++)
    {
        if (*pBkClr == 0) *pBkClr = 1;
    }
    
    alpha_buf_to_rectmem(tempM,bakM,picAttr[1]+10,picAttr[2],tRes.high,tRes.width);
    alpha_rectmem_to_rectmem(bakM,dispM,picAttr[1]+10,picAttr[2],tRes.high,tRes.width);
    drvLcd_IsUseQueue(0);
    
    for(i=0; i < INPUT_SEL_WORD_LEN; i++)
    {
        event.id = gSelBtnShowId[i];
        event.message = VIEW_DRAW ;
        event.wParam = 0 ;
        event.lParam = 0 ;
        Grap_SendMessage(pBtn, &event);
    }
    
    for(i=0; i < 6; i++)
    {
        event.id = gImeBtnShowId[i];
        if (i==5)
        {
            event.id = BUTTON_ID_17_EDIT;
        }
        event.message = VIEW_DRAW ;
        event.wParam = 0 ;
        event.lParam = 0 ;
        Grap_SendMessage(pBtn, &event);
    }
    
    
}

/*---------------------------------------------------------
*函数: edit_write_show_assn   
*功能: 显示联想输入法		 
*参数: none
*返回: none
*---------------------------------------------------------*/
static void sel_write_show_assn_word(tGRAPBUTTON *pBtn)
{
    GRAP_EVENT	event;
    int i,len;    
    tCTR_PANEL *pCtrPan;   
    tGRAPDESKBOX *pDesk;
    tASSN_CTR *pAssn;
    //tPICINFO tPic;
    tRES_PARA tRes;
    char *bakM,*dispM,*tempM;
    U16 *pBkClr;
    U16 picAttr[]={YD_OPEN_BACKGROUND_EN,14, 7};
    
    pDesk = pBtn->pParent;
    pCtrPan = pDesk->pdata;

    pAssn = &pCtrPan->tAssnCtr;
    
    if (1 != pAssn->flag) return;
    
    //加载背景
    //YD_OPEN_BACKGROUND_EN
    bakM = ALPHA_MEMORY_OFFSET;
    dispM = bakM+ALPHA_BLOCK_SIZE;
    tempM = dispM+ALPHA_BLOCK_SIZE;
    
    //Grap_GetPicInfoById(YD_OPEN_BACKGROUND_EN, &tPic);
    //drvLcd_Nand2Sdram(tRes.offset, tRes.width*tRes.high*2, tempM);        
	res_load_bmp_info(picAttr[0],&tRes);
    res_load_bmp_to_buf(picAttr[0],tempM);
    
    len = tRes.width*tRes.high;
    pBkClr = (U16 *)tempM;
    for (i=0; i<len; i++,pBkClr++)
    {
        if (*pBkClr == 0) *pBkClr = 1;
    }
    
    alpha_buf_to_rectmem(tempM,bakM,picAttr[1],picAttr[2],tRes.high,tRes.width);
    alpha_rectmem_to_rectmem(bakM,dispM,picAttr[1],picAttr[2],tRes.high,tRes.width);
    drvLcd_IsUseQueue(0);
    
    for(i=0; i < INPUT_SEL_WORD_LEN; i++)
    {
        event.id = gSelBtnShowId[i];
        event.message = VIEW_DRAW ;
        event.wParam = 0 ;
        event.lParam = 0 ;
        Grap_SendMessage(pBtn, &event);
    }
}

#if 0
/*---------------------------------------------------------
*函数: sel_write_if_enable_sign   
*功能: enable or disable sign		 
*参数: none
*返回: none
*---------------------------------------------------------*/
static void sel_write_if_enable_sign(tCTR_PANEL *pCtrPan, int flag)
{
    int i;
    int start, length,threshold;
    extern const U8 gu16StartAttr[];
    
    for (i=0; ; i+=3)
	{
	     if (gu16StartAttr[i] == INPUT_CTR_CHI) break; 
	}
	
    start=gu16StartAttr[i+1];
	length=gu16StartAttr[i+2];
	threshold = start+length;
    
    if (flag)
    {
         for (i=start+INPUT_HANDW_LEN; i<threshold; i++)
         {
              *pCtrPan->pBtnEnable[i] = 1; 
         }
    }
    else
    {
         for (i=start+INPUT_HANDW_LEN; i<threshold; i++)
         {
              *pCtrPan->pBtnEnable[i] = 0; 
         }
    }
}
#endif

/*---------------------------------------------------------
*函数: sel_change_sel_attr
*功能: 根据当前的的模式改变下拉备选字的属性
*参数: none
*返回: none
*--------------------------------------------------------*/
static void sel_change_sel_attr(tGRAPDESKBOX *pDesk)
{
    tGRAPBUTTON *pBtn;
    tGrapViewAttr *pViewAttr,*pConView;
    tCTR_PANEL *pCtrPan;
    int i,j;
    int type;

    pCtrPan = pDesk->pdata;
    
    if (NULL == pCtrPan) return;
     
    type = pCtrPan->tSelWord.type;
    
    if (2 == type)
    {
        for (i=0,j=0; i<INPUT_SEL_WORD_LEN; i++)
        {
            pBtn = Grap_GetViewByID(pDesk,gSelBtnShowId[i]);
            
            if (NULL == pBtn) continue;
            
            pViewAttr = &pBtn->view_attr;
            
            pViewAttr->viewX1     = gSelHBtnAttr[j++];
            pViewAttr->viewY1     = gSelHBtnAttr[j++];
            pViewAttr->viewWidth  = gSelHBtnAttr[j++];
            pViewAttr->viewHeight = gSelHBtnAttr[j++];
            pViewAttr->touchX1    = gSelHBtnAttr[j++];
            pViewAttr->touchY1    = gSelHBtnAttr[j++];
            pViewAttr->touchX2    = gSelHBtnAttr[j++];
            pViewAttr->touchY2    = gSelHBtnAttr[j++];
        }
    }
    else
    { 
        pConView = &EditButtonStruct[INPUT_SEL_WORD_START];
         
        for (i=0; i<INPUT_SEL_WORD_LEN; i++)
        {
            pBtn = Grap_GetViewByID(pDesk,gSelBtnShowId[i]);
            
            if (NULL == pBtn) continue;
            
            pViewAttr = &pBtn->view_attr;
            
            pViewAttr->viewX1     = pConView->viewX1;
            pViewAttr->viewY1     = pConView->viewY1;
            pViewAttr->viewWidth  = pConView->viewWidth;
            pViewAttr->viewHeight = pConView->viewHeight;
            pViewAttr->touchX1    = pConView->touchX1;
            pViewAttr->touchY1    = pConView->touchY1;
            pViewAttr->touchX2    = pConView->touchX2;
            pViewAttr->touchY2    = pConView->touchY2;
            pConView++;
        }
    }   
}

/*---------------------------------------------------------
*函数: sel_clr_selword_ranger
*功能: 清除备选字区域
*参数: none
*返回: none
*--------------------------------------------------------*/
static void sel_clr_selword_ranger(tGRAPDESKBOX *pDesk)
{
    char *bakM,*dispM;
    tCTR_PANEL *pCtrPan;
    int viewY1,viewX1,viewH,viewW;

    pCtrPan = pDesk->pdata;

    bakM = ALPHA_MEMORY_OFFSET;
    dispM = bakM+ALPHA_BLOCK_SIZE;
    
    viewH = SEL_PANEL_HIG;
    viewW = SEL_PANEL_WID;
    
    if (2 == pCtrPan->tSelWord.type)
    {
        viewY1 = SEL_PANEL_WR_ROW;
        viewX1 = SEL_PANEL_WR_COL; 
    }
    else
    {
        viewY1 = SEL_PANEL_IM_ROW; 
        viewX1 = SEL_PANEL_IM_COL;
    }
      
    alpha_bmp_clr_block_mem(ALPHADISPLAY,   viewY1,viewX1,viewH,viewW);
    alpha_bmp_clr_block_mem(ALPHABACKGROUND,viewY1,viewX1,viewH,viewW); 
    alpha_set_push_queue(viewY1,viewX1,viewH,viewW);
}

/*---------------------------------------------------------
*函数: sel_if_enable_selword
*功能: 关闭或打开备选字
*参数: none
*返回: none
*--------------------------------------------------------*/
int sel_if_enable_selword(tGRAPDESKBOX *pDesk)
{
    int rc = -1;     
    tSEL_WORD_CTR *ptSelW;
    tCTR_PANEL *pCtrPan; 
    int i,total;
    int curMode;
    
    //加判断是否有备选字
    
    pCtrPan = pDesk->pdata; 
    
    if (NULL == pCtrPan) return rc;

    curMode = pCtrPan->tSysCtr.inputMode;
    ptSelW = &pCtrPan->tSelWord;
    
    total = INPUT_SEL_WORD_START+INPUT_SEL_WORD_LEN;

    if (ptSelW->enable)
    {//收起下拉框
		 
		 for (i=INPUT_SEL_WORD_START; i<total; i++)
		 {
		     *pCtrPan->pBtnEnable[i] = 0;
		 }
		 ptSelW->enable = 0;
		 sel_clr_selword_ranger(pDesk);	
		 
		 if (INPUT_CTR_IME == curMode)
		 {
		     sel_ime_enable_panel(pDesk,1);
		 }
		 else if (INPUT_CTR_CHI == curMode)
		 {
		     //sel_write_if_enable_sign(pCtrPan,1);
		 }	     
    }
    else
    {//打开下拉框
		 for (i=INPUT_SEL_WORD_START; i<total; i++)
		 {
		     *pCtrPan->pBtnEnable[i] = 1;
		 }
		 ptSelW->enable = 1;
		 
		 sel_change_sel_attr(pDesk);
		 
		 //显示备选字符
		 if (INPUT_CTR_CHI == curMode)
		 {
		     sel_write_show_assn_word(pDesk);
		     //sel_write_if_enable_sign(pCtrPan,0);
		 }
		 else if (INPUT_CTR_IME == curMode)
		 {//拼音下拉显示,是拼音字或联想下拉字
		     sel_ime_enable_panel(pDesk,0);
		     sel_ime_show_word(pDesk);
		 }   
    }
    
    rc = RETURN_REDRAW;
    
    return rc;
}

/*---------------------------------------------------------
*函数: sel_have_selword
*功能: 是否具有备选字
*参数: none
*返回: -1
*--------------------------------------------------------*/
int sel_have_selword(tGRAPDESKBOX *pDesk)
{
   	 int rc = -1;
   	 tCTR_PANEL *pCtrPan;
   	 int curMode; 
   	 
   	 pCtrPan = pDesk->pdata; 
    
    if (NULL == pCtrPan) return rc;
    
    curMode = pCtrPan->tSysCtr.inputMode;
    
    if (INPUT_CTR_CHI == curMode)
    {
	    if (pCtrPan->tAssnCtr.validTotal > 0)
	    {
	        rc = pCtrPan->tAssnCtr.validTotal;
	    }
    }
    else if (INPUT_CTR_IME == curMode)
    {
        if (0 == pCtrPan->tIme.type)
        {//是否有备选拼音字
             if (strlen(pCtrPan->tIme.pIme)> 10)
             {
                 rc = 1;
             }
        }
        else
        {//是否有联想字
            if (pCtrPan->tAssnCtr.validTotal > 0)
		    {
		        rc = pCtrPan->tAssnCtr.validTotal;
		    } 
        }
    }
    
    return rc;
}





