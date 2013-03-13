#include "edit.h"
#include "glbVariable.h" 
#include "alphaMake.h"
#include "selWord.h"
#include "AlphaDisplay.h"
#include "DisplayMemory.h"


static U8 sAlfaInput0, sAlfaInput1;

void edit_set_alfaValue(U8 alfa0, U8 alfa1)
{
	sAlfaInput0 = alfa0;
	sAlfaInput1 = alfa1;
}

/*-----------------------------------------------------------------------------
* 函数:	edit_make_alpha_value
* 功能:	设置alpha值
* 参数:	type 类型
* 返回:	base offset
*----------------------------------------------------------------------------*/
void edit_make_alpha_value(void)
{
    U8 *pHd,*pIm,*pHdNormal,*pImNormal;
    int i,j;
    int alVNormal,alVSpc;
    
    alVNormal = sAlfaInput0;
    alVSpc    = sAlfaInput1;
    
    alVNormal = alVNormal>100 ? 100:alVNormal;
    alVSpc    = alVSpc>100 ? 100:alVSpc;
    
    pHd = ALPHA_MEMORY_VALUE_OFFSET;
    pIm = pHd+ALPHA_BLOCK_VALUE_SIZE;
    pHdNormal = pIm+ALPHA_BLOCK_VALUE_SIZE;
    pImNormal = pHdNormal+ALPHA_BLOCK_VALUE_SIZE;
    
    for (i=0; i<320; i++)
    {
        for (j=0; j<240; j++)
        {
            if (i>=SEL_PANEL_WR_ROW && i<SEL_PANEL_WR_ROW+SEL_PANEL_HIG && j >= SEL_PANEL_WR_COL && j<= SEL_PANEL_WR_COL+SEL_PANEL_WID)
            {
                *pHd++ = 0;
                *pHdNormal++ = alVNormal;
            }
            else if (i>=194 && i<251 && j>= 4 && j < 236)
            {
                *pHd++ = alVSpc;
                *pHdNormal++ = alVSpc;
            }
            else
            {
                *pHd++ = alVNormal;
                *pHdNormal++ = alVNormal;
            }
            
            if (i>=SEL_PANEL_IM_ROW && i<SEL_PANEL_IM_ROW+SEL_PANEL_HIG && j >= SEL_PANEL_IM_COL && j<= SEL_PANEL_IM_COL+SEL_PANEL_WID)
            {
                *pIm++ = 0;
                *pImNormal++ = alVNormal;
            }
            else if (i>=208 && i<256 && j>= 4 && j < 236) //if (i>=215 && i<256 && j>= 4 && j < 236)  090312 修改位置
            {
                *pIm++ = alVSpc;
                *pImNormal++ = alVSpc;
            }
            else
            {
                *pIm++ = alVNormal;
                *pImNormal++ = alVNormal;
            }
        }
    }
}

/*-----------------------------------------------------------------------------
* 函数:	edit_get_alpha_offset
* 功能:	根据类型得到base值
* 参数:	type 类型
* 返回:	base offset
*----------------------------------------------------------------------------*/
static char *edit_get_alpha_offset(tCTR_PANEL *pCtrPan)
{
     char *pBase = NULL;
     U8 *pHd;
     int curMode,selType;
     
     curMode = pCtrPan->tSysCtr.inputMode;
     selType = pCtrPan->tSelWord.enable;
     
     pHd = ALPHA_MEMORY_VALUE_OFFSET;
      
     if (INPUT_CTR_IME == curMode)
     {
          if (selType)
          {
              pBase = pHd+ALPHA_BLOCK_VALUE_SIZE;
          }
          else
          {
              pBase = pHd+ALPHA_BLOCK_VALUE_SIZE*3;   
          }
           
     }
     else if (INPUT_CTR_CHI == curMode)
     {
     	  if (selType)
          {
              pBase = pHd; 
          }
          else
          {
              pBase = pHd+ALPHA_BLOCK_VALUE_SIZE*2;
          }
     }    
     
     return pBase;
}


/*-----------------------------------------------------------------------------
* 函数:	edit_bmp_mix_buf
* 功能:	混合两种buf输出
* 参数:	算出当前行的alpha，并混合buf输出
* 返回:	none
*----------------------------------------------------------------------------*/
void edit_bmp_mix_buf(AREA_t *pArea,char *upTempMem, char *dispBuf,tCTR_PANEL *pCtrPan)
{
     int row,col,hig,wid;
     int i,j;
     U8 *pAlpha,*pValue;
     int alValue;
     int curMode,selType;
     int len;

     if (NULL == pCtrPan) return;
     
     curMode = pCtrPan->tSysCtr.inputMode;
     selType = pCtrPan->tSelWord.enable;
     
     row = pArea->kRow;
     col = pArea->kColumn;
     hig = pArea->kHigh;
     wid = pArea->kWidth;
     
     pAlpha = edit_get_alpha_offset(pCtrPan);
     
     //其他模式下统一的alpha值
     if (pAlpha == NULL)
     {
     	  len = hig*wid;
     	  alValue = sAlfaInput0;
     	  alValue = alValue>100 ? 100:alValue;
     	  alpha_bmp_mix_two_buf(alValue,len,upTempMem,dispBuf,upTempMem);
     	  return;
     }     
     
     for (i=row; i<row+hig; i++)
     {
         for (j=col; j<col+wid; j++)
         {
             pValue = pAlpha+i*240+j; 
             alValue = *pValue; 
             
             if (alValue>100) alValue = 100;
             
             alpha_bmp_mix_two_buf(alValue,1,upTempMem,dispBuf,upTempMem);
             upTempMem += 2;
             dispBuf   += 2;
              
         }
     } 
}


