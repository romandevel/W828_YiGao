#include "hyTypes.h"
#include "AlphaDisplay.h"
#include "DisplayMemory.h"
#include "glbVariable_base.h"
#include "hyhwLcd.h"


static QUEUE_t sAlphaQue;

#define ALPHA_DEFAULT_COEF    (26)


/*define alpha mode coef, default is 30*/
static U8 gu8alphaCoef = ALPHA_DEFAULT_COEF;

/*---------alpha---memory---------------------------*\
 150K bak  |  150K disp | 150K tempBuf
\*--------------------------------------------------*/
	
/*-------------------------------------------*\
			|abcdef
    abc		|
    def		|
            |
			|
\*-------------------------------------------*/
void alpha_mem_to_buf(char *bakMem, char *outBuf, U16 row, U16 col, U16 hig, U16 wid)
{
	int i;
	char *srcBuf;
	
	wid = wid<<1;
	 
	for(i=0; i<hig; i++)
	{
		srcBuf = bakMem+(((row+i)*gLcd_Total_Column+col)<<1);
		memcpy(outBuf, srcBuf, wid);
		outBuf += wid;
	}
}

/*------------------------------------------------------*\
buf1: 		  |buf2:
			  |
     abcd	  |		abcd
     efgh     |		efgh
			  |
			  |
\*------------------------------------------------------*/
void alpha_rectmem_to_rectmem(char *src, char *des, U16 row, U16 col, U16 hig, U16 wid)
{
	 int i;
	 int sizeExcursion,sizeLen;
	 
	 sizeLen = wid<<1;
	 
	 for (i=0; i<hig; i++)
	 {
	      sizeExcursion = ((row+i)*gLcd_Total_Column+col)<<1;

	      memcpy(des+sizeExcursion,src+sizeExcursion,sizeLen);
	 }
}

/*-------------------------------------------------------*\
buf1: abcd   |buf2
 			 |
			 |      ab
			 |		cd
			 |
\*-------------------------------------------------------*/
void alpha_buf_to_rectmem(char *src, char *des, U16 row, U16 col, U16 hig, U16 wid)
{
    int i,sizeLen;
    char *tempSrc,*tempDes;
     
    sizeLen = wid<<1;
    
     //backup memory
	for(i=0; i<hig; i++)
	{
	    tempSrc = src+((i*wid)<<1);
	    tempDes = des+(((row+i)*gLcd_Total_Column+col)<<1);
	    memcpy(tempDes,tempSrc,sizeLen);
	}
}

void alpha_mix_two_buf(int percent,int len, U16 *pInput1, U16 *pInput2, U16 *outbuf)
{
    U16 put1,put2,OutPut;
	U32 i;	
	U16 r,g,b;
	
	for(i=0; i<len; i++)
	{	
		//R
		put1 = (*pInput1)>>11;
		put2 = (*pInput2)>>11;
		OutPut = (put1*(100-percent)+put2*percent)/100;
		r = OutPut<<11;
		
		//G
		put1 = ((*pInput1)>>5&0x3f);
		put2 = ((*pInput2)>>5&0x3f);
		OutPut = (put1*(100-percent)+put2*percent)/100;
		g = OutPut<<5;
		
		//B
		put1 = (*pInput1&0x1f);
		put2 = (*pInput2&0x1f);
		OutPut = (put1*(100-percent)+put2*percent)/100;
		b = OutPut;
		
		pInput1++;
		pInput2++;
		*outbuf++ = r+b+g;
	}
}

//mix bmp buf buf1:bmp buf buf2:background buf
void alpha_bmp_mix_two_buf(int percent,int len, U16 *pInput1, U16 *pInput2, U16 *outbuf)
{
    U16 put1,put2,OutPut;
	U32 i;	
	U16 r,g,b;
	
	for(i=0; i<len; i++)
	{	
		//filter black clock
		if (*pInput1 == 0)
		{
		    *outbuf = *pInput2; 
		    pInput1++;
		    pInput2++;
		    outbuf++;
		    continue;
		}
		
		//R
		put1 = (*pInput1)>>11;
		put2 = (*pInput2)>>11;
		OutPut = (put1*(100-percent)+put2*percent)/100;
		r = OutPut<<11;
		
		//G
		put1 = ((*pInput1)>>5&0x3f);
		put2 = ((*pInput2)>>5&0x3f);
		OutPut = (put1*(100-percent)+put2*percent)/100;
		g = OutPut<<5;
		
		//B
		put1 = (*pInput1&0x1f);
		put2 = (*pInput2&0x1f);
		OutPut = (put1*(100-percent)+put2*percent)/100;
		b = OutPut;
		
		pInput1++;
		pInput2++;
		*outbuf++ = r+b+g;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	alpha_bmp_cal_alpha_mix_buf
* 功能:	根据输入的高 宽 行 列
* 参数:	算出当前行的alpha，并混合buf输出
* 返回:	none
*----------------------------------------------------------------------------*/
static void alpha_bmp_cal_alpha_mix_buf(AREA_t *pArea,char *upTempMem, char *dispBuf,tCTR_PANEL *pCtrPan)
{
#if 1
     edit_bmp_mix_buf(pArea,upTempMem,dispBuf,pCtrPan);
#else    
     int flag = 1;
     int aVlue;
     int row,hig,wid,disHig;
     int len,mLen;
     
     row = pArea->kRow;
     hig = pArea->kHigh;
     wid = pArea->kWidth;
     
     do
     {
          flag = edit_get_next_alpha_row(row,hig);
          aVlue = edit_get_cur_alpha_value(row);
         
          if (flag > row+hig)
          {
              disHig = hig;
          }
          else
          {
              disHig = flag;
          }

          len = disHig*wid;
          
          alpha_bmp_mix_two_buf(aVlue,len,upTempMem,dispBuf,upTempMem);
          mLen = len<<1;
          upTempMem += mLen;
          dispBuf   += mLen;
          upTempMem += mLen;
          
          row += disHig;
          hig = hig-disHig;
           
     }while(hig > 0);
#endif     
}

static void alpha_disp_to_screen(U16 row,U16 col,U16 hig, U16 wid,char *dispBuf)
{
		
    U32 size,i; 
    U16 *pu16Sdram,*pTemp;
    
    drvLcd_SetRowColumn_Lcd(row, col, hig, wid);

    size = hig*wid;//16bit size
    pu16Sdram = (U16*)dispBuf;
    
    //pTemp = (U16*)Get_Display_RgbReversal_Offset();
	//hyhwRgb_AdjustBuf(pu16Sdram,pTemp,0,0,hig,wid,0);
	
#if 1    
    for (i = 0;i < size; i++)
	{
		//drvLcd_WriteData(*pTemp++);
		drvLcd_WriteData(*pu16Sdram++);
	}
#else
	drvLcd_WritePartData(pTemp);
#endif
    /*__asm
    {
    	mov		r0, size
    	mov		r1, #LCD_DATA_ADDRESS
    	mov		r2, pu16Sdram
    	
    sendLcd_loop:
    	ldrh	r3, [r2], #2
    	subs	r0, r0, #1
    	strh	r3, [r1]
    	bne		sendLcd_loop
    }*/
}

//return 1: in source range 2: in destination range 0:
__inline static int alpha_in_range(U16 row0, U16 col0, U16 hig0, U16 wid0,U16 row, U16 col, U16 hig, U16 wid)
{
    if (row >= row0 && row+hig <= row0+hig0 && col >= col0 && col+wid <= col0+wid0)
    {
        return 1;
    }
    else if (row0 >= row && row0+hig0 <= row+hig && col0 >= col && col0+wid0 <= col+wid)
    {
        return 2;
    }
    
    return 0;
} 

static void alpha_push_queue(QUEUE_t *que, U16 row, U16 col, U16 hig, U16 wid)
{
	int i;
	int totalLen;
	int rv;
	AREA_t *pArea;
	
	if (gRefreshNeedQue == FALSE) return;
	
	if(que->length < 0)
	{
		que->length = 0;
	}
	
	totalLen = que->length; 
	//进入队列的时候检查是否有重叠的区域
	for (i=0; i<totalLen; i++)
	{
        pArea = &que->data[i]; 
        
        rv = alpha_in_range(pArea->kRow,pArea->kColumn,pArea->kHigh,pArea->kWidth,row,col,hig,wid);
        if (1 == rv)
        {
            return;
        }
        else if (rv == 2)
        {
             pArea->kRow     = row; 
             pArea->kColumn  = col;
             pArea->kHigh    = hig;
             pArea->kWidth	 = wid;
             return;
        }
	}

	if(que->length >= QUESIZE)
	{		
		//gRefreshNeedQue = FALSE;//避免频闪 
		//que->length = 0;
		gRefreshNeedQue = FALSE;
		return; //jie 0312
	}

	que->data[que->length].kRow = row;
	que->data[que->length].kColumn = col;
	que->data[que->length].kHigh = hig;
	que->data[que->length].kWidth = wid;

	que->length++;
}

void alpha_set_push_queue(U16 row, U16 col, U16 hig, U16 wid)
{
     alpha_push_queue(&sAlphaQue,row,col,hig,wid);
}

#if 0
/*-----------------------------------------------------------------------------
* 函数:	alpha_nand_to_bakanddisp
* 功能:	把内容读到指定的buf中
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void alpha_nand_to_bakanddisp_mem(U32 BMPOffsetInNand, U16 kRow, U16 kColumn,U16 kBMPHigh, U16 kBMPWidth,  char *alpha)
{
	U32 kSize;
	char *tempBuf,*dispMem,*bakMem;
	
	bakMem = alpha;
	dispMem = alpha+ALPHA_BLOCK_SIZE;
	tempBuf = dispMem+ALPHA_BLOCK_SIZE;
	
	
	kSize = (U32)((kBMPHigh * kBMPWidth) * c1POINT_OCCUPY_BYTES);//16位色彩屏

	drvLcd_Nand2Sdram(BMPOffsetInNand, kSize, tempBuf);
    
    //backup memory
    alpha_buf_to_rectmem(tempBuf,bakMem,kRow,kColumn,kBMPHigh,kBMPWidth);
	
	//disp to display memory
	alpha_rectmem_to_rectmem(bakMem,dispMem,kRow,kColumn,kBMPHigh,kBMPWidth);
	
	alpha_push_queue(&sAlphaQue,kRow,kColumn,kBMPHigh,kBMPWidth);
}

/*-----------------------------------------------------------------------------
* 函数:	alpha_nand_to_disp_mem
* 功能:	把内容读到指定的buf中
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void alpha_nand_to_disp_mem(U32 BMPOffsetInNand, U16 kRow, U16 kColumn,U16 kBMPHigh, U16 kBMPWidth,  char *alpha)
{
	U32 kSize;
	char *tempBuf,*dispMem;
	
	dispMem = alpha+ALPHA_BLOCK_SIZE;
	tempBuf = dispMem+ALPHA_BLOCK_SIZE;
	
	kSize = (U32)((kBMPHigh * kBMPWidth) * c1POINT_OCCUPY_BYTES);//16位色彩屏

	drvLcd_Nand2Sdram(BMPOffsetInNand, kSize, tempBuf);

    //buf to disp memory
	alpha_buf_to_rectmem(tempBuf,dispMem,kRow,kColumn,kBMPHigh,kBMPWidth);
	
	alpha_push_queue(&sAlphaQue,kRow,kColumn,kBMPHigh,kBMPWidth);
}
#endif
void alpha_bmp_clr_block_mem(int type,U16 kRow,U16 kColumn,U16 kBMPHigh,U16 kBMPWidth)
{
    int i;
    int wid;
    char *des,*temp;
    
    if (!(type >= ALPHABACKGROUND && type <= ALPHABUFFER) ) return;
    
    i = type-ALPHABACKGROUND;
    
    des = ALPHA_MEMORY_OFFSET+i*ALPHA_BLOCK_SIZE; 
    
    wid = kBMPWidth<<1;
    
    for (i=0; i<kBMPHigh; i++)
    {
        temp = des+(((kRow+i)*gLcd_Total_Column+kColumn)<<1);
        memset(temp,0,wid);
    } 
}

//write bmp to alpha memroy  must set which block before write
void alpha_bmp_to_disp_mem(const U8 *pkBMPPointer,U16 kRow,U16 kColumn,U16 kBMPHigh,U16 kBMPWidth)
{
    drvLcd_WriteBMPColor(pkBMPPointer,kBMPHigh,kBMPWidth,kRow,kColumn);
}


//刷新背后的小图
static void alpha_disp_uplay_to_screen(QUEUE_t *que, char *upBakMem,tCTR_PANEL *pCtrPan)
{
     int i,totalLen,len;
     AREA_t *pArea;
	 char *upTempMem,*upDispMem;
	 char *dispBuf,*dispMem;
     
     totalLen = que->length;
     
     if (0 == totalLen) return;
     
     upDispMem = upBakMem+ALPHA_BLOCK_SIZE;
	 upTempMem = upDispMem+ALPHA_BLOCK_SIZE;
     
     dispBuf = (char *)Get_Display_DisplayBuffer_Offset();
     dispMem = (char *)Get_Display_DisplayMemory_Offset();
     
     for (i=0; i<totalLen; i++)
     {
         pArea = &que->data[i];
         
         len = pArea->kHigh * pArea->kWidth;  
         
         alpha_mem_to_buf(dispMem,dispBuf,pArea->kRow,pArea->kColumn,pArea->kHigh, pArea->kWidth);
         alpha_mem_to_buf(upDispMem, upTempMem, pArea->kRow,pArea->kColumn, pArea->kHigh, pArea->kWidth); 
         alpha_bmp_cal_alpha_mix_buf(pArea,upTempMem,dispBuf,pCtrPan);
         //alpha_bmp_mix_two_buf(gu8alphaCoef,len,upTempMem,dispBuf,upTempMem);	  
         alpha_disp_to_screen(pArea->kRow,pArea->kColumn,pArea->kHigh,pArea->kWidth,upTempMem);
     }
     
     //混合后刷屏     
     que->length = 0;
}

static void alpha_disp_partdispmem_to_screen(QUEUE_t *que, char *upBakMem,tCTR_PANEL *pCtrPan)
{
   	int i,totalLen,len;
	AREA_t *pArea;
	char *upTempMem,*upDispMem;
	char *dispBuf,*dispMem; 
   	
	totalLen = que->length;

	if(totalLen == 0)  return;
	
    upDispMem = upBakMem+ALPHA_BLOCK_SIZE;
	upTempMem = upDispMem+ALPHA_BLOCK_SIZE;
     
    dispBuf = (char *)Get_Display_DisplayBuffer_Offset();
    dispMem = (char *)Get_Display_DisplayMemory_Offset();	

	for	(i=0; i<totalLen; i++)
	{
		pArea = &que->data[i];
		
		len = pArea->kHigh * pArea->kWidth; 
				
		if ( len == 0) continue;
		
		//需要检查buf的有效性
		if (pArea->kRow >= ALPHA_SHOW_ROW+ALPHA_SHOW_HIG)
		{
			 //alpha_rectmem_to_rectmem(dispMem,upTempMem,pArea->kRow,pArea->kColumn,pArea->kHigh, pArea->kWidth);
			 alpha_mem_to_buf(dispMem,dispBuf,pArea->kRow,pArea->kColumn,pArea->kHigh, pArea->kWidth);
			 alpha_disp_to_screen(pArea->kRow,pArea->kColumn,pArea->kHigh,pArea->kWidth,dispBuf);
		}
		else
		{
            alpha_mem_to_buf(dispMem,dispBuf,pArea->kRow,pArea->kColumn,pArea->kHigh, pArea->kWidth);
            alpha_mem_to_buf(upDispMem, upTempMem, pArea->kRow,pArea->kColumn, pArea->kHigh, pArea->kWidth); 
            
            alpha_bmp_cal_alpha_mix_buf(pArea,upTempMem,dispBuf,pCtrPan);
            //alpha_bmp_mix_two_buf(gu8alphaCoef,len,upTempMem,dispBuf,upTempMem);	  
            
            alpha_disp_to_screen(pArea->kRow,pArea->kColumn,pArea->kHigh,pArea->kWidth,upTempMem);
        } 
	}

	que->length = 0; 
}



static void alpha_disp_alldispmem_to_screen(QUEUE_t *que, char *upBakMem, tCTR_PANEL *pCtrPan)
{
    char *upTempMem,*upDispMem;
	char *dispBuf,*dispMem; 
	char *buf1,*buf2;
	U16 row,hig;
	int len;
	AREA_t area;
	
	len =  ALPHA_SHOW_HIG*ALPHA_SHOW_WID;
	
    upDispMem = upBakMem+ALPHA_BLOCK_SIZE;
	upTempMem = upDispMem+ALPHA_BLOCK_SIZE;
     
    dispBuf = (char *)Get_Display_DisplayBuffer_Offset();
    dispMem = (char *)Get_Display_DisplayMemory_Offset();
    
    //
    alpha_mem_to_buf(dispMem,dispBuf,ALPHA_SHOW_ROW,ALPHA_SHOW_COL,ALPHA_SHOW_HIG, ALPHA_SHOW_WID);
    alpha_mem_to_buf(upDispMem, upTempMem, ALPHA_SHOW_ROW,ALPHA_SHOW_COL,ALPHA_SHOW_HIG, ALPHA_SHOW_WID); 
    
    area.kRow    = ALPHA_SHOW_ROW;
    area.kColumn = ALPHA_SHOW_COL;
    area.kHigh   = ALPHA_SHOW_HIG;
    area.kWidth  = ALPHA_SHOW_WID;
    
    alpha_bmp_cal_alpha_mix_buf(&area,upTempMem,dispBuf,pCtrPan);
    //alpha_bmp_mix_two_buf(gu8alphaCoef,len,upTempMem,dispBuf,upTempMem);
    
    buf1 = dispMem+ALPHA_SHOW_HIG*ALPHA_SHOW_WID*2;
    buf2 = upTempMem+ALPHA_SHOW_HIG*ALPHA_SHOW_WID*2;
    row = ALPHA_SHOW_ROW+ALPHA_SHOW_HIG;   
    hig = gLcd_Total_Row-row;
    
    //dispMem    
    alpha_rectmem_to_rectmem(dispMem,upTempMem,row,0,hig,ALPHA_SHOW_WID);
     	  
    alpha_disp_to_screen(0,0,gLcd_Total_Row, gLcd_Total_Column,upTempMem);
    
}


/*-------------------------------------------------------
 *函数: alpha_disp_all_to_screen  
 *功能: alpha模式下进行送屏
 *参数: none
 *返回: none
--------------------------------------------------------*/
void alpha_disp_all_to_screen(tCTR_PANEL *pCtrPan)
{
	//drvLcd_Change_BufTurnFlag();
	char  *alphaBuf = ALPHA_MEMORY_OFFSET;
	
	//队列送屏
	if (gRefreshNeedQue)
	{
	    alpha_disp_partdispmem_to_screen(drvLcd_GetQueList(),alphaBuf,pCtrPan);
	}
	else
	{//全屏刷
        alpha_disp_alldispmem_to_screen(&sAlphaQue,alphaBuf,pCtrPan);  

        drvLcd_ClearQueue(); 

        gRefreshNeedQue = TRUE;
        sAlphaQue.length = 0;
	}
	
	//判断alpha 
	alpha_disp_uplay_to_screen(&sAlphaQue,alphaBuf,pCtrPan);
}

/*-------------------------------------------------------
 *函数: alpha_disp_alldispmem_to_dispmem  
 *功能: 刷全屏到memory
 *参数: none
 *返回: none
--------------------------------------------------------*/
void  alpha_disp_alldispmem_to_dispmem(QUEUE_t *que, char *upBakMem, tCTR_PANEL *pCtrPan)
{
    char *upTempMem,*upDispMem;
	char *dispBuf,*dispMem; 
	char *buf1,*buf2;
	U16 row,hig;
	int len;
	AREA_t area;
	
	len =  ALPHA_SHOW_HIG*ALPHA_SHOW_WID;
	
    upDispMem = upBakMem+ALPHA_BLOCK_SIZE;
	upTempMem = upDispMem+ALPHA_BLOCK_SIZE;
     
    dispBuf = (char *)Get_Display_DisplayBuffer_Offset();
    dispMem = (char *)Get_Display_DisplayMemory_Offset();
    
    //
    alpha_mem_to_buf(dispMem,dispBuf,ALPHA_SHOW_ROW,ALPHA_SHOW_COL,ALPHA_SHOW_HIG, ALPHA_SHOW_WID);
    alpha_mem_to_buf(upDispMem, upTempMem, ALPHA_SHOW_ROW,ALPHA_SHOW_COL,ALPHA_SHOW_HIG, ALPHA_SHOW_WID); 
    
    area.kRow    = ALPHA_SHOW_ROW;
    area.kColumn = ALPHA_SHOW_COL;
    area.kHigh   = ALPHA_SHOW_HIG;
    area.kWidth  = ALPHA_SHOW_WID;
    
    alpha_bmp_cal_alpha_mix_buf(&area,upTempMem,dispBuf,pCtrPan);
    //alpha_bmp_mix_two_buf(gu8alphaCoef,len,upTempMem,dispBuf,upTempMem);
    
    buf1 = dispMem+ALPHA_SHOW_HIG*ALPHA_SHOW_WID*2;
    buf2 = upTempMem+ALPHA_SHOW_HIG*ALPHA_SHOW_WID*2;
    row = ALPHA_SHOW_ROW+ALPHA_SHOW_HIG;   
    hig = gLcd_Total_Row-row;
    
    //dispMem    
    alpha_rectmem_to_rectmem(dispMem,upTempMem,row,0,hig,ALPHA_SHOW_WID);
    
    alpha_rectmem_to_rectmem(upTempMem,dispMem,0,0,320,240); 
}

/*-------------------------------------------------------
 *函数: alpha_disp_all_to_dispmem  
 *功能: alpha模式下进行送显存
 *参数: none
 *返回: none
--------------------------------------------------------*/
void alpha_disp_all_to_dispmem(tCTR_PANEL *pCtrPan)
{
     char  *alphaBuf = ALPHA_MEMORY_OFFSET;
     
     alpha_disp_alldispmem_to_dispmem(&sAlphaQue,alphaBuf,pCtrPan);
     
     drvLcd_ClearQueue();
     sAlphaQue.length = 0;
     gRefreshNeedQue = FALSE;
}
		  
