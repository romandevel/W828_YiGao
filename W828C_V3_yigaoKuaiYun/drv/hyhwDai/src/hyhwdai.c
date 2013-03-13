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
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
#include "hyhwChip.h"
#include "hyhwDai.h"
#include "hyhwCkc.h"
#include "hyhwDma.h"

/*---------------------------------------------------------------------------
   extern function 声明
----------------------------------------------------------------------------*/
//defined by GPIO 模块
extern void hyhwGpio_setDaiFunction(void);

#define CODEC_PLAY_DMA_LCHANNEL 3
#define CODEC_PLAY_DMA_RCHANNEL 4
#define CODEC_RECORD_DMA_CHANNEL 5
/*---------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaEnable 
* 功能:	使能 Dai Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaEnable(eDAI_TRANS_TYPE daiTransferType)	
{
	// DMA1 通道0/1 作为DAI DMA 通道，对应通道号为3/4
	//if(hyhwDma_Enable(4) == HY_OK)
	if(daiTransferType == DaiTxType)
	{
		if(hyhwDma_Enable(CODEC_PLAY_DMA_RCHANNEL) == HY_OK)
		{
			return hyhwDma_Enable(CODEC_PLAY_DMA_LCHANNEL);	
		}	
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_Enable(CODEC_RECORD_DMA_CHANNEL);	
	}
}

/*--------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaDisable 
* 功能:	disable Dai Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmDai_DmaDisable(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 通道0 作为DAI DMA 通道，对应通道号为3/4
	//if(hyhwDma_Disable(4) == HY_OK)
	if(daiTransferType == DaiTxType)
	{
		if(hyhwDma_Disable(CODEC_PLAY_DMA_RCHANNEL) == HY_OK)
		{
			return hyhwDma_Disable(CODEC_PLAY_DMA_LCHANNEL);
		}	
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_Disable(CODEC_RECORD_DMA_CHANNEL);
	}	
}

/*---------------------------------------------------------------------------
* 函数:	hyhwDai_DmaIntClear
* 功能:	清除指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwDai_DmaIntClear(eDAI_TRANS_TYPE daiTransferType)
{
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntClear(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntClear(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaIntEnable(eDAI_TRANS_TYPE daiTransferType)	
{
	// DMA1 通道0 作为DAI DMA 通道，对应通道号为3
	//if(hyhwDma_IntEnable(4))
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntEnable(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntEnable(CODEC_RECORD_DMA_CHANNEL);
	}

}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmDai_DmaIntDisable(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 通道0 作为DAI DMA 通道，对应通道号为3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntDisable(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntDisable(CODEC_RECORD_DMA_CHANNEL);
	}
	
}
	
/*---------------------------------------------------------------------------
* 函数:	hyhwDma_checkInterruptState
* 功能:	判断是否中断使能
* 参数:	none
* 返回:	True 代表已经中断, FALSE 代表未中断
*--------------------------------------------------------------------------*/
BOOL hyhwDai_DmacheckInterruptState(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 通道0 作为DAI DMA 通道，对应通道号为3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_isDmaInterruptSet(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_isDmaInterruptSet(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*---------------------------------------------------------------------------
* 函数:	hyhwDma_checkInterruptState
* 功能:	获得指定通道的DMA的状态（空闲/busy）
*			先查看该通道是否启动---可用
*			数据传输完毕-----------可用
*			数据正在传输-----------不可用
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwDai_DmaGetStatus(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 通道0 作为DAI DMA 通道，对应通道号为3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_GetStatus(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_GetStatus(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*----------------------------------------------------------------------------
* 函数:	hyhwDai_Init
* 功能:	initial dai 
* 参数:	none
* 返回:	HY_OK					--	成功
		HY_ERR_BAD_PARAMETER 	--	输入参数错误
*----------------------------------------------------------------------------*/
U32 hyhwDai_Init(void)
{
	//hyhwCpm_busClkEnable(CKC_DAI);		//enalbe the dai clk	
	hyhwGpio_setDaiFunction();
	//hyhwCpm_setDai0Clk();				//设置DAI 时钟，需要配置CKC_PCK_DAI_REGISTER
	
	return HY_OK;
}


/*----------------------------------------------------------------------------
* 函数:	hyhwmDai_setMaster
* 功能:	把DAI Set master mode
* 参数:	none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setMaster(void)
{
	DAI_DAVC_REGISTER  = DAVC_BIT_0dB;
	
	DAI_DAMR_REGISTER  = (//  DAI_DAMR_BIT_ENBLE		// DAI Master Enable
//						  | DAI_DAMR_BIT_TEN			// enable DAI transmitter
//						  | DAI_DAMR_BIT_REN			// enable Receiver
						    DAI_DAMR_BIT_SM				// 1 set that DAI system clock is generated by the clock generator block
						  | DAI_DAMR_BIT_BM				// bit clock is generated by dividing the system clock
						  | DAI_DAMR_BIT_FM				// frame clock is generated by dividing the bit clock
//						  | DAI_DAMR_BIT_BP				// bit clock polarity ,negtive edge effective 
//						  | DAI_DAMR_BIT_MM				// enable DAI monitor mode
//						  | DAI_DAMR_BIT_LB				// loopback mode enabled
						  | DAI_DAMR_BITMASK_BD16  		// DAI bit clock divider 4 (256->64)
						  | DAI_DAMR_BITMASK_FD32  		// DAI frame clock divider  00 select div 64 (64->1)
						  | DAI_DAMR_BITMASK_RXS_LSB16 	// DAI RX shift, LSB -16bit mode 根据newcleus 更改
						  | DAI_DAMR_BITMASK_TXS_LSB16 	// DAI TX shif, LSB-16bit mode 
						  
						 );
	// 下面的与操作基本是没有意义的，这里只是为了说明该配置的意义, 因为上面的赋值操作已经把这些位置零				 
	DAI_DAMR_REGISTER &= ~( DAI_DAMR_BIT_MS				// internal BCLK is used(Master mode )
						  | DAI_DAMR_BIT_PXE 			// disable (zero extension)
						  | DAI_DAMR_BIT_CC				// 0 disable CDIF clock master mode 
						  | DAI_DAMR_BIT_MD				// set dai bus as i2s bus mode
						  | DAI_DAMR_BIT_CM				// disable cdif monitor mode
						 );	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwDai_setSlave
* 功能:	把DAI Set slave mode 
* 参数:	none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setSlave(void)
{
	DAI_DAVC_REGISTER = DAVC_BIT_0dB;
	
	DAI_DAMR_REGISTER  = (  DAI_DAMR_BIT_MS				// 1 External BCLK used , Slave Mode 
						  | DAI_DAMR_BITMASK_RXS_LSB24 	// DAI RX shift, LSB -24bit mode
						  | DAI_DAMR_BITMASK_TXS_LSB16 	// DAI TX shif, LSB-16bit mode 
						  | DAI_DAMR_BIT_TEN			// enable DAI transmitter
						  | DAI_DAMR_BIT_REN			// enable Receiver
						  | DAI_DAMR_BITMASK_BD16  		// DAI bit clock divider 16 (512->32)
						  | DAI_DAMR_BITMASK_FD32  		// DAI frame clock divider  00 select div 32 (32->1)
						  | DAI_DAMR_BIT_BP				// bit clock polarity ,negtive edge effective
						  | DAI_DAMR_BIT_MM				// enable DAI monitor mode
						  | DAI_DAMR_BIT_LB				// loopback mode enabled
						 );
	// 下面的与操作基本是没有意义的，这里只是为了说明该配置的意义, 因为上面的赋值操作已经把这些位置零
	DAI_DAMR_REGISTER &= ~( DAI_DAMR_BIT_PXE 			// disable (zero extension)
						  & DAI_DAMR_BIT_ENBLE			// DAI Master Enable
						  & DAI_DAMR_BIT_MD				// set dai bus as i2s bus mode
						  & DAI_DAMR_BIT_SM				// 1 set that DAI system clock from extern
						  & DAI_DAMR_BIT_BM				// bit clock from extern
						  & DAI_DAMR_BIT_FM				// frame clock  from extern
						  & DAI_DAMR_BIT_CC				// 0 disable CDIF clock master mode 
						  & DAI_DAMR_BIT_CM				// disable cdif monitor mode
						 );			 
}

/*-----------------------------------------------------------------------------
* 函数:	U32 hyhwAudioCfg_SetSamplingRate( hyhwAudioCfg_SamplingRate_en)
* 功能:	设置采样率
* 参数:	samplingRate	--	枚举，以下几种采样率
*							hz_8000,
*							hz_11025,
*							hz_16000,
*							hz_22050,
*							hz_32000,
*							hz_44100,
*							hz_48000,
* 返回:	HY_OK,
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDai_SetSamplingRate(eHYHWAUDIOCFG_SAMPLINGRATE_EN samplingRate)
{
	/* 注释掉这两句的主要原因是配合应用程序部分，不用修改.
	   在应用程序第一次调用该函数以前, 有个lcdcpu的DMA配置, 
	   如果这里复位以后, DMA配置就失效了。导致程序不能DMA送屏.
	   当然如果应用程序在图像显示前再调用一次DMA配置函数也可以。
	   20090911, yanglijing
	 */  
	//hyhwCpm_setSoftReset(CKC_DMA);
	__asm{ nop };
	__asm{ nop };
	__asm{ nop };
	__asm{ nop };
	//hyhwCpm_clearSoftReset(CKC_DMA);

//	hyhwCpm_setDaiClk(512*samplingRate);			/* set dai system clk */
	hyhwCpm_setDaiDcoClk(512*samplingRate);			/* set dai system clk */
	return HY_OK;
}