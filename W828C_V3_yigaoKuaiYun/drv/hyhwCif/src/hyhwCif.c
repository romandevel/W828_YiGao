/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*----------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "hyhwTimer.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwCif.h"
#include "hyhwCmosSensor.h"
#include "hyhwIntCtrl.h"

/*----------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/
 
//hyhwCif 是07-15版本
//hyhwCif2是07-16版本（闪屏已消除）

/* 输入图象和窗口图象的定义 */

#define QXGA_IMAGE_HSIZE	2048			/* 定义 QXGA SIZE */
#define QXGA_IMAGE_VSIZE	1536

#define XSGA_IMAGE_HSIZE	1280			/* 定义 XSGA SIZE */
#define XSGA_IMAGE_VSIZE	1024

#define VGA_IMAGE_HSIZE		640				/* 定义 VGA SIZE  */
#define VGA_IMAGE_VSIZE		480

#define QVGA_IMAGE_HSIZE	320				/* 定义 QVGA SIZE */
#define QVGA_IMAGE_VSIZE	240

#define IMAGE_HWINDOW	0
#define IMAGE_VWINDOW	0

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_init
* 功能:	initial cif model
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_init()
{

	U32 bitSet;

	hyhwCpm_setScalerClk(PERI_CLK_48M);
	
	/* cmos interface effect mode set */
	bitSet = EFFECT_MODE_REGISTER;
	bitSet &=~(EMR_BIT_UVS_ENABLE|EMR_BIT_YCS_ENABLE);		/* set uyvy sequence*/
	bitSet |=( EMR_BIT_NOR_ENABLE							/* set normal mode*/
			  |EMR_BIT_YCS_ENABLE);							/* 这里设置是不起作用的，具体为什么没查出来，所以默认为UYVY*/
			  
	EFFECT_MODE_REGISTER = bitSet;							
	
	DMA_CFG_REGISTER1 = DCR1_BIT_BS_8;						/* set DMA transfers the image data as 8 words to memory */
	DMA_CFG_REGISTER1 &=~(DCR1_BIT_TM_INC|DCR1_BIT_LOCK_ON);
	
	
	bitSet = INPUT_IMAGE_CFG_REGISTER;
	bitSet &=~(IICR_BIT_CIF_ON								/* disable cif */
			  |IICR_BIT_TV_SYNC								/* set CIF sync signal */
			  |IICR_BIT_POL									/* set PXCLK Polarity Positive edge */
			  );
	
	bitSet |= IICR_BIT_M420_ODD_SKIP;						/* set Convert YUV420 */	
	bitSet &= ~IICR_BIT_BP;									/* set bypass. output format is YUV420*/	
	
	
	bitSet &= ~(IICR_BIT_HSP_HIGH|IICR_BIT_VSP_HIGH);		/* 2009.8.29 Zeet added */

	
#if 0//HYC_DEV_SENSOR_OV2655
		bitSet |= (IICR_BIT_HSP_HIGH							/* set Horizontal Sync Polarity Active high */
				  //|IICR_BIT_VSP_HIGH							/* set Vertical Sync Polarity Active high */
				  |IICR_BIT_PF_422								/* set input format is YUV422 */
				  |IICR_BIT_BPS									/* Bypass. CIF scaler is not used */
			  //|IICR_BIT_POL	/* 2009.5.27 Zeet edited(Assert while OV3640 in use) */
			  );	  
#else//HYC_DEV_SENSOR_HM2055
	bitSet |= (IICR_BIT_HSP_HIGH							/* set Horizontal Sync Polarity Active high */
			  |IICR_BIT_VSP_HIGH							/* set Vertical Sync Polarity Active high */
			  |IICR_BIT_PF_422								/* set input format is YUV422 */
			  |IICR_BIT_BPS									/* Bypass. CIF scaler is not used */
			  //|IICR_BIT_POL	/* 2009.5.27 Zeet edited(Assert while OV3640 in use) */
			  );	  
#endif
	INPUT_IMAGE_CFG_REGISTER = bitSet;
	hyhwmCif_clearFrameFlag();								/* clear one frame flag */
	hyhwmCif_intEnable();									/* enable cif interrupt */
	hyhwmCif_frameIntEnable();								/* enable cif frame interrupt */
	
	return;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setScaler
* 功能:	initial Scaler model
* 参数:	INPUT_IMAGE_SIZE		输入图象尺寸大小
*		OUTPUT_IMAGE_SIZE		输出图象尺寸大小
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCif_setScaler(U32 INPUT_IMAGE_SIZE,U32 OUTPUT_IMAGE_SIZE)
{
	U16 SRC_HSIZE,SRC_VSIZE;
	U16 DST_HSIZE,DST_VSIZE;
	
	SCALER_SOURCE_IMAGE_SIZE_REGISTER	= INPUT_IMAGE_SIZE;		/* Source image size */
	SCALER_DEST_IMAGE_SIZE_REGISTER		= OUTPUT_IMAGE_SIZE;	/* Destination image size */
	
	
	SRC_HSIZE = (INPUT_IMAGE_SIZE>>16)& 0x0fff;
	SRC_VSIZE = (INPUT_IMAGE_SIZE & 0x0fff);
	
	DST_HSIZE = (OUTPUT_IMAGE_SIZE>>16)& 0x0fff;
	DST_VSIZE = (OUTPUT_IMAGE_SIZE & 0x0fff);
	

	SCALER_FACTOR_REGISTER  = ((SRC_HSIZE<<8) / DST_HSIZE)<<16; 
	SCALER_FACTOR_REGISTER |= (SRC_VSIZE<<8) / DST_VSIZE;
			
	INPUT_IMAGE_CFG_REGISTER	&= ~(IICR_BIT_BPS);		/* enable CIF scaler is used */
	hyhwmCif_scalerEnable();
	
	return;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setQVGAMode
* 功能:	set output QVGA MODE
* 参数:	320 X 240
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setQVGAMode()
{
	INPUT_IMAGE_SIZE_REGISTER		= (QVGA_IMAGE_HSIZE) <<16;	/* set input image size Horizonta*/
	INPUT_IMAGE_SIZE_REGISTER	   |= (QVGA_IMAGE_VSIZE);		/* set input image size Vertical*/
	INPUT_IMAGE_WINDOW1_REGISTER	= IMAGE_HWINDOW;			/* set input Window image size Horizonta*/
	INPUT_IMAGE_WINDOW2_REGISTER	= IMAGE_VWINDOW;			/* set input Window image size Vertical*/
	
	IMAGE_SIZE_REGISTER				= (QVGA_IMAGE_HSIZE) <<16;	/* set image size Horizonta*/
	IMAGE_SIZE_REGISTER	   		   |= (QVGA_IMAGE_VSIZE);		/* set image size Vertical*/
	
	hyhwCmosSensor_SetQVGAMode();
	
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setVGAMode
* 功能:	set output QVGA MODE
* 参数:	640 X 480 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setVGAMode()
{
	INPUT_IMAGE_SIZE_REGISTER		= ((VGA_IMAGE_HSIZE <<16)|(VGA_IMAGE_VSIZE));	/* set input image size Horizonta*/
//	INPUT_IMAGE_SIZE_REGISTER	   |= (VGA_IMAGE_VSIZE);		/* set input image size Vertical*/
	INPUT_IMAGE_WINDOW1_REGISTER	= IMAGE_HWINDOW;			/* set input Window image size Horizonta*/
	INPUT_IMAGE_WINDOW2_REGISTER	= IMAGE_VWINDOW;			/* set input Window image size Vertical*/
	
	IMAGE_SIZE_REGISTER				= ((VGA_IMAGE_HSIZE <<16)|(VGA_IMAGE_VSIZE));	/* set image size Horizonta*/
//	IMAGE_SIZE_REGISTER	   		   |= (VGA_IMAGE_VSIZE);		/* set image size Vertical*/
	
	hyhwCmosSensor_SetVGAMode();
	
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setSXGAMode
* 功能:	set output SXGA MODE
* 参数:	1280X1024 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setSXGAMode()
{
	
	INPUT_IMAGE_SIZE_REGISTER		= (XSGA_IMAGE_HSIZE) <<16;	/* set input image size Horizonta*/
	INPUT_IMAGE_SIZE_REGISTER	   |= (XSGA_IMAGE_VSIZE);		/* set input image size Vertical*/
	INPUT_IMAGE_WINDOW1_REGISTER	= IMAGE_HWINDOW;			/* set input Window image size Horizonta*/
	INPUT_IMAGE_WINDOW2_REGISTER	= IMAGE_VWINDOW;			/* set input Window image size Vertical*/
	
	IMAGE_SIZE_REGISTER				= (XSGA_IMAGE_HSIZE) <<16;	/* set image size Horizonta*/
	IMAGE_SIZE_REGISTER	   		   |= (XSGA_IMAGE_VSIZE);		/* set image size Vertical*/

	hyhwCmosSensor_SetSXGAMode();
	
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setQXGAMode
* 功能:	set output QXGA MODE
* 参数:	2048X11536 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setQXGAMode()
{
	
	INPUT_IMAGE_SIZE_REGISTER		= (QXGA_IMAGE_HSIZE) <<16;	/* set input image size Horizonta*/
	INPUT_IMAGE_SIZE_REGISTER	   |= (QXGA_IMAGE_VSIZE);		/* set input image size Vertical*/
	INPUT_IMAGE_WINDOW1_REGISTER	= IMAGE_HWINDOW;			/* set input Window image size Horizonta*/
	INPUT_IMAGE_WINDOW2_REGISTER	= IMAGE_VWINDOW;			/* set input Window image size Vertical*/
	
	IMAGE_SIZE_REGISTER				= (QXGA_IMAGE_HSIZE) <<16;	/* set image size Horizonta*/
	IMAGE_SIZE_REGISTER	   		   |= (QXGA_IMAGE_VSIZE);		/* set image size Vertical*/

	hyhwCmosSensor_SetQXGAMode();
	
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_frameStart
* 功能:	得到一帧数据
* 参数:	YUV 420数据存储的位置
* 返回:	none
*		note: QVGA数据长度: Y = 76800 / U = 19200 / V= 19200 
			  SXGA数据长度: Y =  / U =  / V=  
*----------------------------------------------------------------------------*/
U32 hyhwCif_frameStart(U32 yAddr,U32 uAddr,U32 vAddr)
{
	U32 loopcount =0x0;
#define YUVLEN  	640*480	

	DMA_CFG_REGISTER2 = yAddr;						/* set Y(G) Channel Base Address */
	DMA_CFG_REGISTER3 = uAddr;						/* set U(R) Channel Base Address */
	DMA_CFG_REGISTER4 = vAddr;						/* set V(B) Channel Base Address */

	DMA_CFG_REGISTER5 = yAddr + YUVLEN;
	DMA_CFG_REGISTER6 = uAddr + YUVLEN/4;
	DMA_CFG_REGISTER7 = vAddr + YUVLEN/4;
//	hyhwmCif_setCVSin();							/* set cvs in */				
//	hyhwGpio_setIn(PORT_E, BIT21);
//	while(!hyhwmCif_readCVS())						
	{
		loopcount++;								/* 这里设置的延时要根据主频设定*/
		if(loopcount >= 5000000) 						
		{
	//		return HY_ERROR;
		}
	}
	hyhwmCif_enable();
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 宏:	hyhwCif_checkStoredOneFrame()
* 功能:	check 是否结束一帧数据
* 参数:	none 
*----------------------------------------------------------------------------*/
U32 hyhwCif_getStoredOneFrame(void)	
{	
	U32 loopcount = 0x0;
	
	while(!(hyhwmCif_checkStoredOneFrame()))						
	{
		loopcount++;								/* 这里设置的延时要根据主频设定 */
		if(loopcount >= 5000000) 					
		{
			return HY_ERROR;
		}
	}
	hyhwmCif_disable();
	hyhwmCif_clearFrameFlag();
	
	return HY_OK;
}	
/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setOverlay
* 功能:	initial cif model
* 参数:	timerID	--- 通道index 
		time    --- 输入时间
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCif_setOverlay(void)
{
	U32 bitSet,overAddr;
	
	OVER_IMAGE_SIZE_REGISTER	= 320<<16;			/* set overlay input image size Horizonta*/
	OVER_IMAGE_SIZE_REGISTER   |= 240;				/* set overlay input image size Vertical*/
	OVER_IMAGE_WINDOW1_REGISTER	= 0;				/* set overlay Window image size Horizonta*/
	OVER_IMAGE_WINDOW2_REGISTER	= 0;				/* set overlay Window image size Horizonta*/
	
//	OVER_BASEADDR_REGISTER = overAddr;
	
	bitSet|= (OCTRL1_BIT_OM_BLOCK					/* Block image overlay */
			 |OCTRL1_BIT_XR1_100					/* 100 % */
			 |OCTRL1_BIT_XR0_100					/* 100 % */
			 |OCTRL1_BIT_OE_EN						/* enable overlay */
			 );
	
	OVER_CONTROL_REGISTER1= bitSet;
}


/* end of file */

