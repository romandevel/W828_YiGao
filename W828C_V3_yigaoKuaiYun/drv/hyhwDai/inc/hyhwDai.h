/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/
#include "hyhwDma.h"

#ifndef HY_HW_DAI_H_
#define HY_HW_DAI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define DAI_BASEADDR			TCC7901_DAI_BASEADDR

#define DAI_DADI_L0_REGISTER	TCC7901_DAI_DADI_L0_REGISTER	/* digital audio left input register 0  */
#define	DAI_DADI_R0_REGISTER    TCC7901_DAI_DADI_R0_REGISTER    /* digital audio right input register 0 */
#define	DAI_DADI_L1_REGISTER    TCC7901_DAI_DADI_L1_REGISTER    /* digital audio left input register 1  */
#define	DAI_DADI_R1_REGISTER    TCC7901_DAI_DADI_R1_REGISTER    /* digital audio right input register 1 */
#define	DAI_DADI_L2_REGISTER    TCC7901_DAI_DADI_L2_REGISTER    /* digital audio left input register 2  */
#define	DAI_DADI_R2_REGISTER    TCC7901_DAI_DADI_R2_REGISTER    /* digital audio right input register 2 */
#define	DAI_DADI_L3_REGISTER    TCC7901_DAI_DADI_L3_REGISTER    /* digital audio left input register 3  */
#define	DAI_DADI_R3_REGISTER    TCC7901_DAI_DADI_R3_REGISTER    /* digital audio right input register 3 */

#define DAI_DADO_L0_REGISTER    TCC7901_DAI_DADO_L0_REGISTER    /* digital audio left output register 0  */
#define	DAI_DADO_R0_REGISTER    TCC7901_DAI_DADO_R0_REGISTER    /* digital audio right output register 0 */
#define	DAI_DADO_L1_REGISTER    TCC7901_DAI_DADO_L1_REGISTER    /* digital audio left output register 1  */
#define	DAI_DADO_R1_REGISTER    TCC7901_DAI_DADO_R1_REGISTER    /* digital audio right output register 1 */
#define	DAI_DADO_L2_REGISTER    TCC7901_DAI_DADO_L2_REGISTER    /* digital audio left output register 2  */
#define	DAI_DADO_R2_REGISTER    TCC7901_DAI_DADO_R2_REGISTER    /* digital audio right output register 2 */
#define	DAI_DADO_L3_REGISTER    TCC7901_DAI_DADO_L3_REGISTER    /* digital audio left output register 3  */
#define	DAI_DADO_R3_REGISTER    TCC7901_DAI_DADO_R3_REGISTER    /* digital audio right output register 3 */

#define DAI_DAMR_REGISTER	    TCC7901_DAI_DAMR_REGISTER	    /* digital audio mode register */
	//Master Clock selection Register */
	#define DAI_DAMR_BIT_MS		BIT31							/* 0 Internal BCLK is used (Master Mode) , 1External BCLK used , Slave Mode */
	// DAI RX data sign Extention		
	#define DAI_DAMR_BIT_PXE	BIT22							/* 0 Disable ( zero extension ) ,1 Enable (sign bit extension) */
	// DAI RX shift
	#define DAI_DAMR_BITMASK_RXS_MSB24	(0x0 << 20)				/* 00 Bit-pack MSB-24 bitmode */
	#define DAI_DAMR_BITMASK_RXS_MSB16	(0x1 << 20)				/* 01 MSB-16bitmode */
	#define DAI_DAMR_BITMASK_RXS_LSB24	(0x2 << 20)				/* 10 LSB 24bit */
	#define DAI_DAMR_BITMASK_RXS_LSB16	(0x3 << 20)				/* 10/11 LSB-16bit mode */
	// DAI TX shif
	#define DAI_DAMR_BITMASK_TXS_MSB	(0x0 << 19)	 			/* 0x MSB */
	#define DAI_DAMR_BITMASK_TXS_LSB24	(0x2 << 18)				/* 10 LSB-24 bitmode */
	#define DAI_DAMR_BITMASK_TXS_LSB16	(0x3 << 18)				/* 11 LSB-16bit mode */
	
	// DAI Master Enable
	#define DAI_DAMR_BIT_ENBLE	BIT15							/* 0 disable dai module, 1 enable dai module */
	// DAI Transmitter Enable
	#define DAI_DAMR_BIT_TEN	BIT14							/* 0 disable dai transmitter, 1 enable dai transmitter */
	// DAI Receiver Enable
	#define DAI_DAMR_BIT_REN	BIT13							/* as upsides */
	// DAI Bus mode 
	#define DAI_DAMR_BIT_MD		BIT12							/* 0 set dai bus as i2s bus mode ,1 set as MSB justified mode */
	// DAI System clock Master select
	#define DAI_DAMR_BIT_SM		BIT11							/* 0 set that DAI system clock is come from external pin */
																/* 1 set that DAI system clock is generated by the clock generator block */
	// DAI bit clock master select
	#define DAI_DAMR_BIT_BM		BIT10							/* 0 set DAI bit clock from external pin, 1 clock is generated by dividing DAI system clock */
	// DAI frame clock master select
	#define DAI_DAMR_BIT_FM		BIT9							/* 0 set DAI frame clock from external pin, 1 clock is generated by dividing DAI bit clock */ 
	// CDIF clock select		
	#define DAI_DAMR_BIT_CC		BIT8							/* 0 disable CDIF clock master mode, 1 enable */
	// DAI bit clock divider select
	#define DAI_DAMR_BITMASK_BD4		(0x0 << 6)				/* 00  00 select div 4 (256fs->64fs), */
	#define DAI_DAMR_BITMASK_BD6		(0x1 << 6)				/* 01 div 6(384->64) */
	#define DAI_DAMR_BITMASK_BD8		(0x2 << 6)				/* 10 div 8(512->64, 384->48, 256->32,  */
	#define DAI_DAMR_BITMASK_BD16		(0x3 << 6) 				/* 11 div 16 (512->32)	*/
												   						
	// DAI frame clock divider select
	
	#define DAI_DAMR_BITMASK_FD32		(0x0 << 4) 				/* 00 select div 32 (32->1)*/
	#define DAI_DAMR_BITMASK_FD48		(0x1 << 4)				/* 01 div 48(48->1) */ 
	#define DAI_DAMR_BITMASK_FD64		(0x2 << 4)				/* 10 div 64(64->fs) */
	
	// DAI bit clock polarity
	#define DAI_DAMR_BIT_BP		BIT3							/* 0 set that data is captured at positive edge of bit clock, 1 negative */
	// DAI CDIF monitor mode 
	#define DAI_DAMR_BIT_CM		BIT2							/* 0 disable cdif monitor mode ,1 enable */
	// DAI monitor mode 
	#define DAI_DAMR_BIT_MM		BIT1							/* 0 disable monitor mode, 1 enable, transmitter should be enabled (TE = 1) */
	// DAI loop-backk mode 
	#define DAI_DAMR_BIT_LB		BIT0							/* disalbe dai lloop back mode, 1 enable */
	
#define DAI_DAVC_REGISTER	    TCC7901_DAI_DAVC_REGISTER	    /* digital audio volume control register */
	#define	DAVC_BIT_0dB				0						/* 0dB Volume */
	#define	DAVC_BIT_6dB				1						/* -6dB Volume */
	#define	DAVC_BIT_12dB				2						/* -12dB Volume */
	#define	DAVC_BIT_18dB				3						/* -18dB Volume */
	#define	DAVC_BIT_24dB				4						/* -24dB Volume */
	#define	DAVC_BIT_30dB				5						/* -30dB Volume */
	#define	DAVC_BIT_36dB				6						/* -36dB Volume */
	#define	DAVC_BIT_42dB				7						/* -42dB Volume */
	#define	DAVC_BIT_48dB				8						/* -48dB Volume */
	#define	DAVC_BIT_54dB				9						/* -54dB Volume */
	#define	DAVC_BIT_60dB				10						/* -60dB Volume */
	#define	DAVC_BIT_66dB				11						/* -66dB Volume */
	#define	DAVC_BIT_72dB				12						/* -72dB Volume */
	#define	DAVC_BIT_78dB				13						/* -78dB Volume */
	#define	DAVC_BIT_84dB				14						/* -84dB Volume */
	#define	DAVC_BIT_90dB				15						/* -90dB Volume */
	#define	DAVC_BIT_96dB				16						/* -96dB Volume */


typedef enum hyhwAudioCfg_SamplingRate_en
{
	hz_8000  = 8000,
	hz_11025 = 11025,
	hz_12000 = 12000,
	hz_16000 = 16000,
	hz_22050 = 22050,
	hz_24000 = 24000,
	hz_32000 = 32000,
	hz_44100 = 44100,
	hz_48000 = 48000
}eHYHWAUDIOCFG_SAMPLINGRATE_EN;

typedef enum hyhwAudioCfg_AudioDataLength_en
{
	AudioDataLength16,
	AudioDataLength18,
	AudioDataLength20,
	AudioDataLength24
}eHYHWAUDIOCFG_AUDIODATALENGTH_EN;


/*----------------------------------------------------------------------------
* 宏:	hyhwmDai_enableDai
* 功能:	enable dai tx 
* 参数:	none
*-----------------------------------------------------------------------------*/
#define	hyhwmDai_enableDai()		DAI_DAMR_REGISTER	 |= DAI_DAMR_BIT_ENBLE

/*----------------------------------------------------------------------------
* 宏:	hyhwmDai_disableDai
* 功能:	disable dai tx 
* 参数:	none
*----------------------------------------------------------------------------*/
#define hyhwmDai_disableDai()		DAI_DAMR_REGISTER	 &= ~DAI_DAMR_BIT_ENBLE
		
/*----------------------------------------------------------------------------
* 宏:	hyhwDai_enableDaiTx
* 功能:	enable dai tx 
* 参数:	none
*-----------------------------------------------------------------------------*/
#define	hyhwmDai_enableDaiTx()		DAI_DAMR_REGISTER	 |= DAI_DAMR_BIT_TEN

/*----------------------------------------------------------------------------
* 宏:	hyhwDai_disableDaiTx
* 功能:	disable dai tx 
* 参数:	none
*----------------------------------------------------------------------------*/
#define hyhwmDai_disableDaiTx()		DAI_DAMR_REGISTER	 &= ~DAI_DAMR_BIT_TEN

/*---------------------------------------------------------------------------
* 宏:	hyhwDai_enableDaiRx
* 功能:	enable dai rx 
* 参数:	none
*----------------------------------------------------------------------------*/
#define hyhwmDai_enableDaiRx()		DAI_DAMR_REGISTER	 |= DAI_DAMR_BIT_REN

/*---------------------------------------------------------------------------
* 宏:	hyhwmDai_disableDaiRx
* 功能:	disable dai rx 
* 参数:	none
*----------------------------------------------------------------------------*/
#define hyhwmDai_disableDaiRx()		DAI_DAMR_REGISTER	 &= ~DAI_DAMR_BIT_REN

/*-----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaEnable 
* 功能:	使能 Dai Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaEnable(eDAI_TRANS_TYPE daiTransferType);

/*----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaDisable 
* 功能:	disable Dai Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwmDai_DmaDisable(eDAI_TRANS_TYPE daiTransferType);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDai_DmaIntClear
* 功能:	清除指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDai_DmaIntClear(eDAI_TRANS_TYPE daiTransferType);

/*-----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaIntEnable(eDAI_TRANS_TYPE daiTransferType);

/*----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwmDai_DmaIntDisable(eDAI_TRANS_TYPE daiTransferType);
	
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_checkInterruptState
* 功能:	判断是否中断使能
* 参数:	none
* 返回:	True 代表已经中断, FALSE 代表未中断
*----------------------------------------------------------------------------*/
BOOL hyhwDai_DmacheckInterruptState(eDAI_TRANS_TYPE daiTransferType);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_checkInterruptState
* 功能:	获得指定通道的DMA的状态（空闲/busy）
*			先查看该通道是否启动---可用
*			数据传输完毕-----------可用
*			数据正在传输-----------不可用
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDai_DmaGetStatus(eDAI_TRANS_TYPE daiTransferType);

/*----------------------------------------------------------------------------
* 函数:	hyhwDai_Init
* 功能:	initial dai 
* 参数:	none
* 返回:	HY_OK					--	成功
		HY_ERR_BAD_PARAMETER 	--	输入参数错误
*----------------------------------------------------------------------------*/
U32 hyhwDai_Init(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwmDai_setMaster
* 功能:	把DAI Set master mode
* 参数:	none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setMaster(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwDai_setSlave
* 功能:	把DAI Set slave mode 
* 参数:	none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setSlave(void);

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
U32 hyhwDai_SetSamplingRate(eHYHWAUDIOCFG_SAMPLINGRATE_EN samplingRate);

#ifdef __cplusplus
}
#endif

#endif //TM_HWTIMER_H_