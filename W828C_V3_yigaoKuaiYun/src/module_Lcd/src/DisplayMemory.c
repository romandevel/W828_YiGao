
/*************************************************************
%
% Filename     : DisplayMemory.c
% Project name : by use of displaymemory, part of the LCD driver library of Hyctron
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   060424  HYCJB		Primary version
% -------------------------------------------------------------
%
%  This source file contains low function of the Lcd Library test test you are the best
%
****************************************************************/

#include "HSA_API.h"
#include "string.h"

#include "glbVariable_base.h"
#include "LcdAPIFunc.h"
#include "LcdCPU.h"
#include "DisplayMemory.h"

#include "AbstractFileSystem.h"
#include "grap_api.h"
#include "resManage.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define TRANSFERDATA_USE_ASSEMBLY_LANGUAGE		//使用汇编指令传输数据到显示屏


static MemoryType_e sRWMemoryType = MEMORYDISPLAY;	//背景图存储区、显存、缓冲存储区

static U16 sRow = 0;
static U16 sColumn = 0;
static U16 sHeight = 0;
static U16 sWidth = 0;
static U16 sTempColumn = 0;

static QUEUE_t sque;   

static U8  sLcd_DmaRefresh = 0;//0：lcd 1:dma


/*++++++++++++++++++++++++++++++++++++++++++发送队列++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* 函数:	EnQueue
* 功能:	增加队列元素,刷新界面
* 参数: que-----队列
* 返回:	TRUE----成功	FALSE---失败
*----------------------------------------------------------------------------*/
static void EnQueue(QUEUE_t *que)
{
	int i,length;
	AREA_t *pArea;
	int row,col,endRow,endCol;
	
	if(sHeight*sWidth >= gLcd_Total_Row*gLcd_Total_Column || que->length >= QUESIZE)
	{
		gRefreshNeedQue = FALSE;
		que->length = 0;
		return;
	}
	
	if(que->length < 0)
	{
		que->length = 0;
	}

	length = que->length;
	for (i=0; i<length; i++)
	{
	     pArea = &que->data[i];
	     
	     row = pArea->kRow;
	     col = pArea->kColumn;
	     endRow = pArea->kRow+pArea->kHigh;
	     endCol = pArea->kColumn+pArea->kWidth;
	     
	     if (sRow >= row && sColumn>= col && sRow+sHeight <= endRow && sColumn+sWidth <= endCol)
	     {
	         return;
	     }
	     else if (sRow <=row && sColumn <= col && sRow+sHeight >= endRow && sColumn+sWidth >= endCol)
	     {
	         pArea->kRow = sRow;
	         pArea->kColumn = sColumn;
	         pArea->kHigh = sHeight;
	         pArea->kWidth = sWidth;
	         return;
	     }
	}

	que->data[que->length].kRow = sRow;
	que->data[que->length].kColumn = sColumn;
	que->data[que->length].kHigh = sHeight;
	que->data[que->length].kWidth = sWidth;

	que->length++;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetQueList
* 功能:	获得送屏队列指针
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
QUEUE_t *drvLcd_GetQueList(void)
{
    return &sque;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_IsUseQueue
* 功能:	是否使用队列
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_IsUseQueue(int type)
{
	if (1 == type)
	{
		gRefreshNeedQue = TRUE;
	}
	else
	{
		gRefreshNeedQue = FALSE;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearQueue
* 功能:	清除显示队列
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearQueue(void)
{
	sque.length = 0;
}

/*++++++++++++++++++++++++++++++++++++++++++内存++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* 功能:	得到文件管理的区域,以及大小
*----------------------------------------------------------------------------*/
U32 Get_Display_Buffer_Size(void)
{
	return ((U32)hsaDisplayMemoryPool_GetBufferSize()/6);
}

/*
* 获取当前正在用作显示的那块RGBbuf的地址
*/
U32 Get_Display_RgbBuf_Offset()//kong rgb  显示屏配置地址
{
	U32 addess;

	addess = (U32)hsaDisplayMemoryPool_GetBuffer();
	
	return (U32)hsaConvertUnCacheMemory((char *)addess);
}

U32 Get_Display_RgbReversal_Offset(void)//调整rgb方向
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size());
}

U32 Get_Display_DisplayMemory_Offset(void)//上层操作的buf
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*2);
}

U32 Get_Display_DisplayBuffer_Offset(void)
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*3);
}

U32 Get_Display_BackGround_Offset(void)
{
    return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*4);
}

U32 Get_Uncompress_Buffer_Offset(void)
{
    return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*5);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetMemoryOffset
* 功能:	得到memory的offset
* 参数: x,y memory的列行
* 返回:	none
*----------------------------------------------------------------------------*/
char *drvLcd_GetMemoryOffset(U16 x, U16 y, U32 type)
{
	char *pOff;

	switch(type)
	{
	case MEMORYBACKGROUND:        
		pOff = (char *)((Get_Display_BackGround_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	case MEMORYDISPLAY:
		pOff = (char *)((Get_Display_DisplayMemory_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	case MEMORYBUFFER:
		pOff = (char *)((Get_Display_DisplayBuffer_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	default:
		pOff = NULL;
		break;             
	}

	return pOff;
}

/*-----------------------------------------------------------------------------
* 函数:	GetSDRAMOffset
* 功能:	获得行和列在SDRAM中的地址
* 参数:	gMemory-----背景图存储区或显存
*		kRow--------屏幕显示行
*		kColumn-----屏幕显示列
* 返回:	offset------SDRAM地址
*----------------------------------------------------------------------------*/
U32 GetSDRAMOffset(U16 kRow, U16 kColumn)
{
	U32 offset;

	if (sRWMemoryType == MEMORYBACKGROUND)
	{
		offset = (U32)(Get_Display_BackGround_Offset() + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if (sRWMemoryType == MEMORYDISPLAY)
	{
		offset = (U32)(Get_Display_DisplayMemory_Offset() + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if(sRWMemoryType == MEMORYBUFFER)
	{
		offset = (U32)(Get_Display_DisplayBuffer_Offset() + (kRow*sWidth+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if (sRWMemoryType == ALPHABACKGROUND)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES); 
	}
	else if (sRWMemoryType == ALPHADISPLAY)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET+ALPHA_BLOCK_SIZE + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES); 
	}
	else if (sRWMemoryType == ALPHABUFFER)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET+2*ALPHA_BLOCK_SIZE + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}

	return offset;
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetDisplayMemoryArea
* 功能:	设置SDRAM显存中对应位置的信息
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDisplayMemoryArea(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	if(kRow > gLcd_Total_Row || kRow+kHigh > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || kColumn+kWidth > gLcd_Total_Column)
	{
		return;
	} 
	
	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);
	if(gRefreshNeedQue)	EnQueue(&sque);
	sRWMemoryType = MEMORYDISPLAY;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetDisplayArea
* 功能:	设置显示地址
* 参数: DisplayOffset------显示
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetDisplayAddress(U32 DisplayOffset)
{	
	if(DisplayOffset==Get_Display_BackGround_Offset())
	{
		sRWMemoryType = MEMORYBACKGROUND;
	}
	else if(DisplayOffset==Get_Display_DisplayMemory_Offset())
	{
		sRWMemoryType = MEMORYDISPLAY;
	}
	else if(DisplayOffset==Get_Display_DisplayBuffer_Offset())
	{
		sRWMemoryType = MEMORYBUFFER;
	}	
}

/******************************************************
* 函数: hyhwRgb_GetAddressOffset
* 功能: 获得行和列在RGBbuf中的地址
* 参数: kRow:行    kColumn:列
* 返回: 地址
*******************************************************/
U32 hyhwRgb_GetAddressOffset(U16 kRow, U16 kColumn)
{
	U32 offset;

	offset = (U32)(Get_Display_RgbBuf_Offset() + (kRow*gLcd_Total_Row+kColumn)*c1POINT_OCCUPY_BYTES);	
	
	return offset;
}

/***********************************
* 函数:hyhwRgb_AdjustBuf
* 功能:调整显示buf 来完成转屏动作
* 参数:pSrc:源  pDst:目标  flag:0:pSrc中的数据为连续的数据(不是按屏幕位置存放)  1:对应屏幕位置存放的数据
* 返回:none
***********************************/
void hyhwRgb_AdjustBuf(U16 *pSrc, U16 *pDst, U16 kRow, U16 kCol, U16 kHight, U16 kWidth, U8 flag)
{
	int i,j,displaymode;
	U16 endRow,endCol,dataWidth;
	
	if(flag == 0)
	{
		dataWidth = kWidth;
	}
	else
	{
		dataWidth = gLcd_Total_Column;
	}
	displaymode = 0;//LcdModule_Get_ShowType();

	endRow = kRow+kHight;
	endCol = kCol+kWidth;
	if(displaymode == 0)//竖屏
	{
		for(j = endCol-1; j >= kCol; j--)
		{
			for(i = kRow; i < endRow; i++)
			{
				*pDst++ = pSrc[i*dataWidth+j];
			}
		}
	}
	else //和原始刷屏方向一致 可以不用调整
	{
		memcpy((U8 *)pDst, (U8 *)pSrc, gLcd_Total_Row*gLcd_Total_Column*c1POINT_OCCUPY_BYTES);
	}
	
	return ;
}

/*++++++++++++++++++++++++++++++++++++++送屏++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetRowColumn
* 功能:	设置彩色显示屏的显示起始地址,地址为行序号和列序号row和Column,以及高度和宽度
* 参数:	kRow------行序号 kColumn---列序号  kHeight---高度  kWidth----宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
{
	sRow = kRow;
	sColumn = kColumn;
	sTempColumn = 0;
}

void Memory_SetRowColumn(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth)
{
	sRow = kRow;
	sColumn = kColumn;
	sHeight = kHeight;
	sWidth = kWidth;
	sTempColumn = 0;

	if(kHeight == gLcd_Total_Row && kWidth == gLcd_Total_Column)
	{
		sRWMemoryType = MEMORYDISPLAY;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DMARefreshEnable
* 功能:	使用DMA送屏时，是这个样子的
* 参数:	1：利用DMA送屏 0:cpu 送屏
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DMARefreshEnable(U32 flag)
{
    if (1 == flag)   sLcd_DmaRefresh = 1;
    else sLcd_DmaRefresh = 0;   
}


void Set_DispMemory2Queue(U16 h, U16 w, U16 y, U16 x)
{
	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(y, x, h, w);	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_AddDispRect
* 功能:	把显示区域加入队列，以便能够进行送屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void  drvLcd_AddDispRect(U16 y, U16 x, U16 h, U16 w)
{
	Memory_SetRowColumn(y, x, h, w);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}	


/*-----------------------------------------------------------------------------
* 函数:	alpha_set_mem_type
* 功能:	设置绘图的buf区域
* 参数:	type：buf类型
* 返回:	none
*----------------------------------------------------------------------------*/
void alpha_set_mem_type(U32 type)
{
     if (type >ALPHABUFFER) return;
          
     sRWMemoryType = type;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearBox
* 功能:	清除指定区域，彩屏时，以背景清除
* 参数:	kPage----页序号（彩屏时为行序号）
*		kColumn--列序号
*		kHeight--高度（单色屏时为页高度，彩屏时为行高度）
*		kWidth---列宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearBox(U16 kPage, U16 kColumn, U16 kHeight, U16 kWidth)
{
	U16 i, j;
	U16 kRow;
	U32 EndRow,EndCol;

	kRow = kPage;
	
	EndRow = kHeight + kRow;
	EndCol = kColumn+kWidth;

	if(kRow > gLcd_Total_Row || EndRow > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || EndCol > gLcd_Total_Column)
	{
		return;
	}   

	for(i=kRow;i<EndRow;i++)
	{
		for(j=kColumn;j<EndCol;j++)
		{
			drvLcd_SetRowColumn(i,j,1,kWidth);
			drvLcd_WritePoint(gBackColor);
		}
	}
	
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearBox
* 功能:	用指定的颜色画图
* 参数:	kPage----页序号（彩屏时为行序号）
*		kColumn--列序号
*		kHeight--高度（单色屏时为页高度，彩屏时为行高度）
*		kWidth---列宽度
*       pcolor---颜色数组
*       注意：颜色数组的大小一定要>=kWidth，否则会死机，调用该函数请小心使用
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearBoxUseColor(U16 kPage, U16 kColumn, U16 kHeight, U16 kWidth, const U16 *pcolor)
{
	U16 i, j,k;
	U16 kRow;
	U32 EndRow,EndCol;

	kRow = kPage;
	
	EndRow = kHeight + kRow;
	EndCol = kColumn+kWidth;
	
	if(kRow > gLcd_Total_Row || EndRow > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || EndCol > gLcd_Total_Column)
	{
		return;
	}   
	
	for(i=kRow;i<EndRow;i++)
	{
		for(k=0,j=kColumn;j<EndCol;j++,k++)
		{
			drvLcd_SetRowColumn(i,j,1,kWidth);
			drvLcd_WritePoint(pcolor[k]);
		}
	}
	
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DrawRectUseColor
* 功能:	用指定的颜色画图
* 参数:	pRect：画图的范围  pcolor：颜色指正 mode 0：水平画 1:垂直画
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DrawRectUseColor(RECT *pRect, const U16 *pcolor, U32 mode)
{
	U16 i, j,k;
	U16 h,w,r,c;
	
	h = pRect->h;
	w = pRect->w;
	r = pRect->y;
	c = pRect->x;
	
	if(r+h > gLcd_Total_Row || c+w > gLcd_Total_Column)
	{
		return;
	}
	
	if (1 == mode)
	{
	    for (k=0,i=r; i<r+h; i++,k++)
	    {
	        for (j=c; j<c+w; j++)
	        {
	            drvLcd_SetRowColumn(i,j,1,1);
			    drvLcd_WritePoint(pcolor[k]);    
	        }
	    }    
	}
	else
	{
	    for (i=r; i<r+h; i++)
	    {
	        for (k=0,j=c; j<c+w; j++,k++)
	        {
	            drvLcd_SetRowColumn(i,j,1,1);
			    drvLcd_WritePoint(pcolor[k]);    
	        }
	    }
	}   
	
	Memory_SetRowColumn(r,c,h,w);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	WritePoint2SDRAM
* 功能:	写一个彩色的点到SDRAM
* 参数:	u16Data-----16位彩色数据
* 返回:	none
*----------------------------------------------------------------------------*/
void WritePoint2SDRAM(U16 u16Data)
{
	U16 *offset;

	offset = (U16 *)GetSDRAMOffset(sRow, (U16)(sColumn+sTempColumn));
	*offset = (U16)u16Data;

	if (sTempColumn>=sWidth-1)
	{
		sRow++;
		sTempColumn = 0;
	}
	else
	{
		sTempColumn++;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2Memory
* 功能:	从NandFlash中读取界面上的图片写入指定的SDRAM显存
* 参数:	BMPOffsetInNand---界面图片在NandFlash资源图片中的偏移地址
*		kBMPHigh----------图片高度
*       kBMPWidth---------图片宽度
*       kRow--------------屏幕显示行
*		kColumn-----------屏幕显示列
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2Memory(char *pSrc, char *pDest,U16 kBMPHigh, U16 kBMPWidth, U16 kRow, U16 kColumn)
{
    U16 i;
	U32 realWid;

    realWid = kBMPWidth<<1;
   
	for(i=0; i<kBMPHigh; i++)
	{
		memcpy(pDest+(((i+kRow)*gLcd_Total_Column+kColumn)<<1),pSrc,realWid );
		pSrc += realWid;
	}	
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_BMPFromDisplayMemory2Screen
* 功能:	SDRAM显存数据DMA方式直接送入显示屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayMemory2Screen(void)
{	
	U32 size; 
	int i;
	U16 data;
	U16 *pu16Sdram;
	U16 *pRgb,*pTemp;

	drvLcd_SetRowColumn_Lcd(0, 0, gLcd_Total_Row, gLcd_Total_Column);

	size = gLcd_Total_Row*gLcd_Total_Column;
	pu16Sdram = (U16 *)Get_Display_DisplayMemory_Offset(); 
	
	
#ifdef TRANSFERDATA_USE_ASSEMBLY_LANGUAGE
	__asm
	{
		mov		r0, size
		mov		r1, #LCD_DATA_ADDRESS
		mov		r2, pu16Sdram
		
	Lcd_loop:
		ldrh	r3, [r2], #2
		subs	r0, r0, #1
		strh	r3, [r1]
		bne		Lcd_loop
	}
#else
	for (i = 0;i < size;i++)
	{
		drvLcd_WriteData(*p16Sdram++);
	}
#endif
	
	return ;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Background2DisplayMemory
* 功能:	从SDRAM背景图缓存中读取需要刷新位置的数据到SDRAM显存中对应位置
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Background2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetbg, *offsetDisplay;
	int i;

	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYBACKGROUND;
		offsetbg = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetbg, kWidth*c1POINT_OCCUPY_BYTES);
		offsetDisplay += kWidth;
		offsetbg += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2DisplayMemory
* 功能:	从SDRAM其他缓存中读取数据到SDRAM显存中指定位置
* 参数:	pBuf------数据源地址
*		kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemory(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetBuffer, *offsetDisplay;
	int i;
	U32 cpySize;


	offsetBuffer = (U16 *)pBuf;
	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	cpySize = kWidth*c1POINT_OCCUPY_BYTES;
	sRWMemoryType = MEMORYDISPLAY;
	
	for(i=0; i<kHigh; i++)
	{
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetBuffer, cpySize);
		offsetBuffer += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_PartMemory2DisplayMemory
* 功能:	从SDRAM其他缓存中读取部分数据到SDRAM显存中指定位置
* 参数:	pBuf------数据源地址
*		klcdRow------在屏上的显示位置
*		klcdColumn
*   	kfactHigh-----数据实际高度
*       kfactWidth----数据实际宽度

*		kdisRow------从该行开始取数据--相对于该数据
*		kdisColumn---从该列开始取数据
*   	kdisHigh-----本次应取数据的高
*       kdisWidth----本次应取数据的宽
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_PartMemory2DisplayMemory(void *pBuf, U16 kfactHigh, U16 kfactWidth, U16 klcdRow, U16 klcdColumn,
									U16 kdisHigh, U16 kdisWidth, U16 kdisRow, U16 kdisColumn)
{
	U16	*offsetBuffer, *offsetDisplay;
	int i, offset_y;

	offsetBuffer = (U16 *)pBuf;
	
	if(kdisHigh == 0 || kdisWidth == 0)
		return ;
	if(kdisHigh==gLcd_Total_Row && kdisWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((klcdColumn+kdisWidth)>gLcd_Total_Column)
	{
		kdisWidth=gLcd_Total_Column-klcdColumn;
	}
	
	if((klcdRow+kdisHigh)>gLcd_Total_Row)
	{
		kdisHigh=gLcd_Total_Row-klcdRow;
	}
	
	offsetBuffer += kdisRow*kfactWidth;
	for(i=0; i<kdisHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(klcdRow+i), klcdColumn);

		memcpy((char *)offsetDisplay, (char *)(offsetBuffer+kdisColumn), kdisWidth*c1POINT_OCCUPY_BYTES);
		offsetBuffer += kfactWidth;
	}

	Memory_SetRowColumn(klcdRow, klcdColumn, kdisHigh, kdisWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_InDisplayMemoryMove
* 功能:	在SDRAM内移动数据
* 参数: Offset_dest-----目的地址
*		Offset_src------源地址
*		size------------数据大小
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_InDisplayMemoryMove(U32 Offset_dest, U32 Offset_src, U32 size)
{
	memcpy((char*)(Get_Display_DisplayMemory_Offset()+Offset_dest), (char*)(Get_Display_DisplayMemory_Offset()+Offset_src), size);
	sRWMemoryType = MEMORYDISPLAY;
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_JPGBackground2DisplayMemory
* 功能:	从SDRAM背景图缓存中读取需要刷新的数据到SDRAM显存中的位置
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_JPGBackground2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetbg, *offsetDisplay;
	int i;

	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	offsetbg=(U16 *)Get_Display_BackGround_Offset();
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetbg, kWidth*c1POINT_OCCUPY_BYTES);
		offsetDisplay += kWidth;
		offsetbg += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayMemory2DisplayBuffer
* 功能:	部分刷新的数据从SDRAM显存到SDRAM数据缓存中
* 参数: kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayMemory2DisplayBuffer(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16 *offsetDisplay, *offsetBuffer;
	int i;

	if(kHigh*kWidth > Get_Display_Buffer_Size())	//超过BUFFER，显示一部分
	{
		kHigh=Get_Display_Buffer_Size()/(kWidth*c1POINT_OCCUPY_BYTES);
	}

	sRWMemoryType = MEMORYBUFFER;
	offsetBuffer = (U16 *)GetSDRAMOffset(0, 0);

	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetBuffer, (char *)offsetDisplay, kWidth*c1POINT_OCCUPY_BYTES);
		offsetBuffer += kWidth;
		offsetDisplay += kWidth;
	}

	sRWMemoryType = MEMORYBUFFER;
	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);
	
	if(1 == gu8EnableRefreshSrc)
	{
		drvLcd_SetRowColumn_Lcd(kRow, kColumn, kHigh, kWidth);
	}
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_BMPFromDisplayBuffer2Screen
* 功能:	SDRAM缓存数据DMA方式直接送入显示屏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayBuffer2Screen(void)
{
	U16* pu16Sdram,*pTemp;
	U32  u16size,i;

    if (1 != gu8EnableRefreshSrc) return;
    
#ifdef TRANSFERDATA_USE_ASSEMBLY_LANGUAGE
	//使用16bit送数据
	u16size = sHeight*sWidth;//16bit size
	if(u16size == 0) return; //孔
	pu16Sdram = (U16*)Get_Display_DisplayBuffer_Offset();
	__asm
	{
		mov		r0, u16size
		mov		r1, #LCD_DATA_ADDRESS
		mov		r2, pu16Sdram
		
	sendLcd_loop:
		ldrh	r3, [r2], #2
		subs	r0, r0, #1
		strh	r3, [r1]
		bne		sendLcd_loop
	}
#else
	u16size = sHeight*sWidth;//16bit size
	if(u16size == 0) return; //孔
	pu16Sdram = (U16*)Get_Display_DisplayBuffer_Offset();
	for (i = 0;i < u16size;i++)
	{
		drvLcd_WriteData(*pu16Sdram++);
	}
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayMemory2DisplayBufferAndScreen
* 功能:	查找队列，是否存在部分刷新的数据，
*		若存在部分刷新的数据，将SDRAM显存中部分刷新的数据到SDRAM数据缓存中，
*		完成后SDRAM缓存数据DMA方式到显示屏的对应位置
*		直到队列为空
* 参数: que-----队列
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayMemory2DisplayBufferAndScreen(QUEUE_t *que)
{
	int i;
	U32 hig,wid,row,col;

	if(que->length == 0)
		return;
		
	if (1 == sLcd_DmaRefresh)
	{//DMA 送屏
	    
	    while(lcdCpuDmaOK() != TRUE) syssleep(1);
        drvLcd_SetRowColumn_Lcd(0,0,gLcd_Total_Row,gLcd_Total_Column);
	    lcdCpuEnable((char *)Get_Display_DisplayMemory_Offset(), gLcd_Total_Row*gLcd_Total_Column);
	    
	    que->length = 0;	    
	    return;    
	} 	

	for	(i=0; i<que->length; i++)
	{
		hig = que->data[i].kHigh;
		wid = que->data[i].kWidth;
		row = que->data[i].kRow;
		col = que->data[i].kColumn;
				
		if ( hig*wid == 0) continue;
		
		drvLcd_DisplayMemory2DisplayBuffer(hig, wid, row, col);
		drvLcd_BMPFromDisplayBuffer2Screen();
	}

	que->length = 0;
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ClearDisplayMemory
* 功能:	清楚显示缓冲区域
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearDisplayMemory(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth)
{
	U16 i,j;

	if(kHeight==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}

	if(gRefreshNeedQue)	EnQueue(&sque);

	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	for(i=0;i<kHeight;i++)
	{
		for(j=0;j<kWidth;j++)
		{
			drvLcd_SetRowColumn(kRow+i,kColumn+j,1,1);
			drvLcd_WritePoint(gFrontColor);
		}
	}
}    


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Memory2DisplayMemoryClarity
* 功能:	从SDRAM其他缓存中读取数据到SDRAM显存中指定位置
* 参数:	pBuf------数据源地址
*		kRow------屏幕显示行
*		kColumn---屏幕显示列
*   	kHigh-----屏幕高度
*       kWidth----屏幕宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemoryClarity(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn, U16 startH, U16 startW, U16 filterColor)
{
	U16	*offsetBuffer, *offsetDisplay;
	U16 dispColor;
	int i,j;

	offsetBuffer = (U16 *)pBuf;
	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);
		
		for (j=0; j<kWidth; j++)
		{ 		    
		    
		    if ((j<startW || j+startW > kWidth) || (i<startH || i+startH > kHigh))		    
		    {
		         if (*offsetBuffer == filterColor)
		         {
		             //*offsetDisplay =  *offsetDisplay;
		         }
		         else  
		         {
		             *offsetDisplay = *offsetBuffer;
		         }    
		    }
		    else
		    {
		       *offsetDisplay = *offsetBuffer; 
		    }		    
		   
		    offsetDisplay++;
		    offsetBuffer++;
		}		
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------
* 函数: res_write_to_bgdisp_memory
* 功能: 把显示数据送入到bg and disp memory 中
* 参数: 
* 返回: 
-------------------------------------------------------------------*/
void drvLcd_write_to_bgdisp_memory(U16 *pbuf, tRES_PARA *pRes, int posX, int posY)
{
	int i,j,high,width;
	U32 *pData;
	int bufSize;
	
	high  = pRes->high;
	width = pRes->width;
	
	bufSize = high*width*c1POINT_OCCUPY_BYTES;
	
	//buf 显示
    if (high == gLcd_Total_Row && width == gLcd_Total_Column)
    {
    	 
    	 memcpy((char*)Get_Display_BackGround_Offset(),    pbuf,  bufSize);
    	 memcpy((char*)Get_Display_DisplayMemory_Offset(), pbuf,  bufSize);
    	 
    	 gRefreshNeedQue = FALSE;
    }
    else
    {
        Memory_SetRowColumn(posY, posX, high, width);
        sRWMemoryType = MEMORYDISPLAY;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}
    	
    	Memory_SetRowColumn(posY, posX, high, width);
    	
    	if(gRefreshNeedQue)	EnQueue(&sque);
    }  
}

void drvLcd_write_to_bganddisp_memory(U16 *pbuf, tRES_PARA *pRes, int posX, int posY)
{
	
	int i,j,high,width;
	U32 *pData;
	int bufSize;
	
	high  = pRes->high;
	width = pRes->width;
	
	bufSize = high*width*c1POINT_OCCUPY_BYTES;
	
	//buf 显示
    if (high == gLcd_Total_Row && width == gLcd_Total_Column)
    {
    	 
    	 memcpy((char*)Get_Display_BackGround_Offset(),    pbuf,  bufSize);
    	 memcpy((char*)Get_Display_DisplayMemory_Offset(), pbuf,  bufSize);
    	 
    	 gRefreshNeedQue = FALSE;
    }
    else
    {
        Memory_SetRowColumn(posY, posX, high, width);
    	sRWMemoryType = MEMORYBACKGROUND;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}
        
        
        Memory_SetRowColumn(posY, posX, high, width);
        sRWMemoryType = MEMORYDISPLAY;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}    	
    	
    	Memory_SetRowColumn(posY, posX, high, width);
    	
    	if(gRefreshNeedQue)	EnQueue(&sque);
    }
}

/*----------------------------------------------------------------------------
* 函数:	DisplayData2Screen
* 功能:	将显存数据送显示屏，增加是否dma送屏的判断
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void DisplayData2Screen(void)
{
	if(gRefreshNeedQue)
	{
		drvLcd_DisplayMemory2DisplayBufferAndScreen(&sque);
	}
	else
	{
		if (1 == sLcd_DmaRefresh)
		{
		    while(lcdCpuDmaOK() != TRUE)
		    {
		    	syssleep(1);
		    }
            drvLcd_SetRowColumn_Lcd(0,0,gLcd_Total_Row,gLcd_Total_Column);
    	    lcdCpuEnable((char *)Get_Display_DisplayMemory_Offset(), gLcd_Total_Row*gLcd_Total_Column*(c1POINT_OCCUPY_BYTES>>1));
		}
		else if (1 == gu8EnableRefreshSrc)
		{
    	    drvLcd_BMPFromDisplayMemory2Screen();
		}
		
		gRefreshNeedQue = TRUE;
	}
}


/*++++++++++++++++++++++++++++++++++++++图形绘制++++++++++++++++++++++++++++++++++++++++++++++++++*/

//不直接送屏，把显示数据写入sdram中后统一进行送屏
void scratch_sdram_draw_line(U16 y1,U16 x1,U16 y2,U16 x2, U16 wid, U32 ColorN)
{
	U16 u1,v1,u2,v2;
	S16 deltaX=x2-x1;
	S16 deltaY=y2-y1;
	int dx=ABS(x1-x2),dy=ABS(y1-y2);
	unsigned char m;

	S16 du,dv;
	S16 u,v,uEnd,p;
	S16 twoDv,twoDvDu;
/////////////////
    U8  i;
    U32 totalpix;
    U16 row,col;
////////////////////    

	if((deltaX*deltaY)>=0) 
	{
		if(dx>=dy)
		{
			m=1; //斜率在[0,1] 范围内
			u1=x1;v1=y1;u2=x2;v2=y2;
		}
		else
		{
			m=2; //斜率在(1, ∞) 范围内
			u1=y1;v1=x1;u2=y2;v2=x2;
		}
	}
	else
	{
		if(dx>=dy)
		{
			m=3; //斜率在[?1,0) 范围内
			u1=x1;v1=y1;u2=x2;v2=2*y1-y2;
		}
		else
		{
			m=4; //斜率在(?∞,?1) 范围内
			u1=y1;v1=x1;u2=2*y1-y2;v2=x2;
		}
	}

	du=ABS(u1-u2);
	dv=ABS(v1-v2);
	p=2*dv-du;
	twoDv=2*dv;
	twoDvDu=2*(dv-du);

	if(u1>u2)
	{
		u=u2;
		v=v2;
		uEnd=u1;
	}
	else
	{
		u=u1;
		v=v1;
		uEnd=u2;
	}

	switch (m)
	{
	case 1:
		row = v;
		col = u;
		break;
	case 2:
  		row = u;
		col = v;
		break;
	case 3:
  		row = 2*y1-v;
		col = u;
		break;
	case 4:
	    row = 2*y1-u;
		col = v;
		break;
	default:
	    break;
	}
	
	if (1 == m || 2 == m || 3 == m || 4== m)
	{
	    Memory_SetRowColumn(row, col, wid, wid);
	    totalpix = wid*wid;
	    
	    if (col + totalpix >= gLcd_Total_Column)
	    {
	         totalpix = gLcd_Total_Column-1-col;
	    }
	    
	    while(totalpix--)   drvLcd_WritePoint(ColorN);   
	}
	
	while(u<uEnd)
	{
		u++;
		if(p<0)
		{
			p+=twoDv;
		}
		else
		{
			v++;
			p+=twoDvDu;
		}		
		
    	switch (m)
    	{
    	case 1:
    		row = v;
    		col = u;
    		break;
    	case 2:
      		row = u;
    		col = v;
    		break;
    	case 3:
      		row = 2*y1-v;
    		col = u;
    		break;
    	case 4:
    	    row = 2*y1-u;
    		col = v;
    		break;
    	default:
    	    break;
    	}
		
		if (1 == m || 2 == m || 3 == m || 4== m)
        {
        	Memory_SetRowColumn(row, col, wid, wid);
        	totalpix = wid*wid;
        	
        	//add 090414
        	if (col + totalpix >= gLcd_Total_Column)
    	    {
    	         totalpix = gLcd_Total_Column-1-col;
    	    }
            	
        	while(totalpix--)   drvLcd_WritePoint(ColorN);   
        }
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_drawLine()
* 功能:	画线
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_drawLine(U16 y1,U16 x1,U16 y2,U16 x2)
{
	U16 u1,v1,u2,v2;
	S16 deltaX=x2-x1;
	S16 deltaY=y2-y1;
	int dx=ABS(x1-x2),dy=ABS(y1-y2);
	unsigned char m;

	S16 du,dv;
	S16 u,v,uEnd,p;
	S16 twoDv,twoDvDu;


	if((deltaX*deltaY)>=0)
	{
		if(dx>=dy)
		{
			m=1; //斜率在[0,1] 范围内
			u1=x1;v1=y1;u2=x2;v2=y2;
		}
		else
		{
			m=2; //斜率在(1, ∞) 范围内
			u1=y1;v1=x1;u2=y2;v2=x2;
		}
	}
	else
	{
		if(dx>=dy)
		{
			m=3; //斜率在[?1,0) 范围内
			u1=x1;v1=y1;u2=x2;v2=2*y1-y2;
		}
		else
		{
			m=4; //斜率在(?∞,?1) 范围内
			u1=y1;v1=x1;u2=2*y1-y2;v2=x2;
		}
	}

	du=ABS(u1-u2);
	dv=ABS(v1-v2);
	p=2*dv-du;
	twoDv=2*dv;
	twoDvDu=2*(dv-du);

	if(u1>u2)
	{
		u=u2;
		v=v2;
		uEnd=u1;
	}
	else
	{
		u=u1;
		v=v1;
		uEnd=u2;
	}
	switch (m)
	{
	case 1:
		drvLcd_ClearBox(v,u,1,1);
		break;
	case 2:
		drvLcd_ClearBox(u,v,1,1);
		break;
	case 3:
		drvLcd_ClearBox(2*y1-v,u,1,1);
		break;
	case 4:
		drvLcd_ClearBox(2*y1-u,v,1,1);
		break;
	default:break;
	}
	while(u<uEnd)
	{
		u++;
		if(p<0)
		{
			p+=twoDv;
		}
		else
		{
			v++;
			p+=twoDvDu;
		}
		switch (m)
		{
		case 1:
			drvLcd_ClearBox(v,u,1,1);
			break;
		case 2:
			drvLcd_ClearBox(u,v,1,1);
			break;
		case 3:
			drvLcd_ClearBox(2*y1-v,u,1,1);
			break;
		case 4:
			drvLcd_ClearBox(2*y1-u,v,1,1);
			break;
		default:
			break;
		}
	}
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_Rectangle
* 功能:	画矩形
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_Rectangle(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth, U16 FrameWidth)
{
	U16 i,j;

	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	for(i=0;i<FrameWidth;i++)
	{
		for(j=0;j<kWidth;j++)
		{
			drvLcd_SetRowColumn(kRow+i,kColumn+j,1,kWidth);//-
			drvLcd_WritePoint(gFrontColor);

			drvLcd_SetRowColumn(kRow+kHeight-1-i,kColumn+j,1,kWidth);//-
			drvLcd_WritePoint(gFrontColor);
		}
	}

	for(i=0;i<FrameWidth;i++)
	{
		for(j=0;j<kHeight;j++)
		{
			drvLcd_SetRowColumn(kRow+j,kColumn+i,kHeight,1);//|
			drvLcd_WritePoint(gFrontColor);

			drvLcd_SetRowColumn(kRow+j,kColumn+kWidth-1-i,kHeight,1);//|
			drvLcd_WritePoint(gFrontColor);
		}
	}

	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_WriteBMPColor
* 功能:	只针对彩色屏，画单色图片，用前景色和背景色扩充
*		（如果是单色屏，就用drvLcd_WriteBMP()）
* 参数:	*pkBMPPointer--单色图片数据
*		kBMPHigh-------图片高度
*		kBMPWidth------图片宽度
*		kRow-----------行序号
*		kColumn--------列序号
* 返回:	none
*----------------------------------------------------------------------------*/
//使用横排的彩色bmp图，bit7...bit0, bit7在左， 扩展到彩图
void drvLcd_WriteBMPColor(const U8 *pkBMPPointer,U16 kBMPHigh,U16 kBMPWidth,U16 kRow,U16 kColumn)
{
	int i,j,m,n;
	U16 tempData,tempBMPWidth;
	U8 u8nBit;
	
	tempBMPWidth =(U16)(kBMPWidth/8);
	if (kBMPWidth % 8) tempBMPWidth++;

	for(i=0;i<kBMPHigh;i++)
	{
		drvLcd_SetRowColumn((U16)(kRow+i),kColumn,1,kBMPWidth);
		for(j=0;j<tempBMPWidth;j++)
		{
			tempData=*(pkBMPPointer+i*tempBMPWidth+j);
			if((kBMPWidth % 8)&&(j==(tempBMPWidth-1)))
			{
				u8nBit = (U8)(kBMPWidth % 8);
			}
			else
			{
				u8nBit = 8;
			}

			//画透明图片(只写前景色)
			m=0;
			for(n=7 ; n>=(8-u8nBit) ; n--)
			{
				if((tempData>>n)&(0x01))
				{
					drvLcd_SetRowColumn((U16)(kRow+i),(U16)(kColumn+j*8+m),1,1);
					drvLcd_WritePoint(gFrontColor);
				}
				m++;
			}
		}
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_GetFontHighWidth
* 功能:	输入字体大小，得到字体显示的高、宽
* 参数:	fontsize:字体大小 *BMPHigh *BMPWidth:取字符显示的高、宽
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetFontHighWidth(U16 fontsize,U16 *BMPHigh,U16 *BMPWidth)
{
	switch(fontsize)
	{
	case FONTSIZE8X16:
		*BMPWidth = 8;
		*BMPHigh = 16;
		break;
	case FONTSIZE24X24:
		*BMPWidth = 12;
		*BMPHigh = 24;
		break;	
	case FONTSIZE32X32:
		*BMPWidth = 16;
		*BMPHigh = 32;
		break;	
	default:
		*BMPWidth = 8;
		*BMPHigh = 16;
		break;			
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DispString
* 功能:	显示字符串
* 参数:	pos----显示位置
*		kDisplayFlag--显示方式
*		*pkString-----待显示的字符串
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DispString(TEXTOUT *text)
{
	U32 WidthStr;
	U16 BMPWidth;
	U16 BMPHigh;
	Character_Type_e kStr_Type;	
	
	if(text->pkString == NULL) return;//如果是空指针则直接返回
	
	drvLcd_GetFontHighWidth(text->kDisplayFlag & FONTSIZE_BIT,&BMPHigh,&BMPWidth);

	kStr_Type = text->kDisplayFlag>>8;
	WidthStr = BMPWidth*Get_StringLength(kStr_Type,(char*) text->pkString);	
	
	if(text->x +WidthStr > gu16Lcd_ColumnLimit ) WidthStr = gu16Lcd_ColumnLimit-text->x;	
	
	drvLcd_SetColor(text->FrontColor,text->BackColor);
	switch(text->EraseBack)
	{
	case 0:	
		sRWMemoryType = MEMORYDISPLAY;
		break;
	case 1:
		if(text->prect != NULL)
		drvLcd_Background2DisplayMemory(text->prect->h,text->prect->w,text->prect->y,text->prect->x);
		else
		drvLcd_Background2DisplayMemory(BMPHigh,WidthStr,text->y,text->x);
		break;
	case 2:
		if(text->prect == NULL) return;
		drvLcd_ClearBox(text->prect->y,text->prect->x,text->prect->h,text->prect->w);
		break;	
	case 3:
	    sRWMemoryType = ALPHADISPLAY;	
		break;	    		
	default:
		sRWMemoryType = MEMORYDISPLAY;
		break;		
	}
	
	drvLcd_DisplayString(text->y, text->x, text->kDisplayFlag, text->pkString, text->pKeyStr, text->keyStrColor);	

	if (text->EraseBack != 3)
	{
		Memory_SetRowColumn(text->y, text->x, BMPHigh, WidthStr);

		if(gRefreshNeedQue)	EnQueue(&sque);
	}
	else
	{
		alpha_set_push_queue(text->y, text->x, BMPHigh, WidthStr); 
	}
}

