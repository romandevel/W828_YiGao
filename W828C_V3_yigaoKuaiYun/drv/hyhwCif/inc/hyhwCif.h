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

#ifndef _HY_HWCIF_H_
#define _HY_HWCIF_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
 * INCLUDES
 *************************************************************************/
/*************************************************************************
 * DEFINES
 *************************************************************************/
/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
 #define	INPUT_IMAGE_CFG_REGISTER		TCC7901_CIF_ICPCR1_REGISTER				/* Input Image Color/Pattern Configuration */
	#define	IICR_BIT_CIF_ON					BIT31									/* 0 : Can't operate CIF, 1 : Operating CIF */
	#define	IICR_BIT_TV_SYNC				BIT29									/* TV sync signal */
	#define	IICR_BIT_VI_INSERT				BIT28									/* Insert vertical blank in 1 line */
	#define	IICR_BIT_BPS					BIT23									/* Bypass Scaler */
	#define	IICR_BIT_POL					BIT21									/* PXCLK Polarity */
	#define	IICR_BIT_NOT_SKIP				~(BIT18|BIT19|BIT20)					/* Not Skip */
	#define	IICR_BIT_M420_NOT_CVT			~(BIT17|BIT16)							/* Not Convert */
	#define	IICR_BIT_M420_ODD_SKIP			BIT17									/* Converted in odd line skip */
	#define	IICR_BIT_M420_EVEN_SKIP			(BIT17|BIT16)							/* Converted in even line skip */
	#define	IICR_BIT_BP						BIT15									/* Bypass */
	#define	IICR_BIT_C656					BIT13									/* Convert 656 format */
	#define	IICR_BIT_CP_RGB					BIT12									/* RGB(555,565,bayer) color pattern */
	#define	IICR_BIT_PF_444					~(BIT10|BIT11)							/* 4:4:4 format */
	#define	IICR_BIT_PF_422					BIT10									/* 4:2:2 format */
	#define	IICR_BIT_PF_420					BIT11									/* 4:2:0 format or RGB(555,565,bayer) mode */
	#define	IICR_BIT_RGBM_BAYER				~(BIT8|BIT9)							/* Bayer RGB Mode */
	#define	IICR_BIT_RGBM_RGB555			BIT8									/* RGB555 Mode */
	#define	IICR_BIT_RGBM_RGB565			BIT9									/* RGB565 Mode */
	#define	IICR_BIT_RGBBM_16				~(BIT6|BIT7)							/* 16bit mode */
	#define	IICR_BIT_RGBBM_8DISYNC			BIT6									/* 8bit disable sync */
	#define	IICR_BIT_RGBBM_8				BIT7									/* 8bit mode */
	#define	IICR_BIT_CS_RGBMG				~(BIT5|BIT4)							/* 555RGB:RGB(MG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:BG->GR, CCIR656:YCbYCr */
	#define	IICR_BIT_CS_RGBLG				BIT4									/* 555RGB:RGB(LG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:GR->BG, CCIR656:YCrYCb */
	#define	IICR_BIT_CS_BGRMG				BIT5									/* 555RGB:BGR(MG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:RG->GB, CCIR656:CbYCrY */
	#define	IICR_BIT_CS_BGRLG				(BIT5|BIT4)								/* 555RGB:BGR(LG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:GB->RG, CCIR656:CrYCbY */
	#define	IICR_BIT_BO_SW					BIT2									/* Switch the MSB/LSB 8bit Bus */
	#define	IICR_BIT_HSP_HIGH				BIT1									/* Active high */
	#define	IICR_BIT_VSP_HIGH				BIT0									/* Active high */

#define	FORMAT_CFG_REGISTER1				TCC7901_CIF_656FCR1_REGISTER			/* CCIR656 Format Configuration Register 1 */
	#define	CCIR656FCR1_BIT_PSL_1ST			~(BIT26|BIT25)							/* The status word is located the first byte of EAV & SAV */
	#define	CCIR656FCR1_BIT_PSL_2ND			BIT25									/* The status word is located the second byte of EAV & SAV */
	#define	CCIR656FCR1_BIT_PSL_3RD			BIT26									/* The status word is located the third byte of EAV & SAV */
	#define	CCIR656FCR1_BIT_PSL_4TH			(BIT26+BIT25)							/* The status word is located the forth byte of EAV & SAV */

#define	FORMAT_CFG_REGISTER2				TCC7901_CIF_656FCR2_REGISTER			/* CCIR656 Format Configuration Register 2 */
#define	INPUT_IMAGE_SIZE_REGISTER			TCC7901_CIF_IIS_REGISTER				/* Input Image Size */
#define	INPUT_IMAGE_WINDOW1_REGISTER		TCC7901_CIF_IIW1_REGISTER				/* Input Image Windowing 1 */
#define	INPUT_IMAGE_WINDOW2_REGISTER		TCC7901_CIF_IIW2_REGISTER				/* Input Image Windowing 2 */
#define	DMA_CFG_REGISTER1					TCC7901_CIF_CDCR1_REGISTER				/* DMA Configuration Register 1 */
	#define	DCR1_BIT_TM_INC					BIT3									/* INC Transfer */
	#define	DCR1_BIT_LOCK_ON				BIT2									/* Lock Transfer */
	#define	DCR1_BIT_BS_1					~(BIT1|BIT0)							/* The DMA transfers the image data as 1 word to memory */
	#define	DCR1_BIT_BS_2					BIT0									/* The DMA transfers the image data as 2 word to memory */
	#define	DCR1_BIT_BS_4					BIT1									/* The DMA transfers the image data as 4 word to memory */
	#define	DCR1_BIT_BS_8					(BIT1|BIT0)								/* The DMA transfers the image data as 8 word to memory (default) */

#define	DMA_CFG_REGISTER2					TCC7901_CIF_CDCR2_REGISTER				/* DMA Configuration Register 2 */
#define	DMA_CFG_REGISTER3					TCC7901_CIF_CDCR3_REGISTER				/* DMA Configuration Register 3 */
#define	DMA_CFG_REGISTER4					TCC7901_CIF_CDCR4_REGISTER				/* DMA Configuration Register 4 */
#define	DMA_CFG_REGISTER5					TCC7901_CIF_CDCR5_REGISTER				/* DMA Configuration Register 5 */
#define	DMA_CFG_REGISTER6					TCC7901_CIF_CDCR6_REGISTER				/* DMA Configuration Register 6 */
#define	DMA_CFG_REGISTER7					TCC7901_CIF_CDCR7_REGISTER				/* DMA Configuration Register 7 */
#define	FIFO_STATUS_REGISTER				TCC7901_CIF_FIFOSTATE_REGISTER			/* FIFO Status Register */
	#define	FIFOSTATE_BIT_CLR				BIT21									/* 1:Clear, 0:Not Clear */
	#define	FIFOSTATE_BIT_REO				BIT19									/* 1:The empty signal of input overlay FIFO and read enable signal are High, 0:The empty signal of overlay FIFO is low, or empty is High and read enable signal is Low. */
	#define	FIFOSTATE_BIT_REV				BIT18									/* 1:The empty signal of input V(B) channel FIFO and read enable signal are High, 0:The empty signal of V(B) channel FIFO is Low, or empty is High and read enable signal is Low. */
	#define	FIFOSTATE_BIT_REU				BIT17									/* 1:The empty signal of input U(R) channel FIFO and read enable signal are High, 0:The empty signal of U(R) channel FIFO is Low, or empty is High and read enable signal is Low. */
	#define	FIFOSTATE_BIT_REY				BIT16									/* 1:The empty signal of input Y(G) channel FIFO and read enable signal are High, 0:The empty signal of Y(G) channel FIFO is Low, or empty is High and read enable signal is Low. */
	#define	FIFOSTATE_BIT_WEO				BIT13									/* 1:The full signal of overlay FIFO and write enable signal are High, 0:The full signal of overlay FIFO is Low, or full is High and write enable signal is Low. */
	#define	FIFOSTATE_BIT_WEV				BIT12									/* 1:The full signal of V(B) channel FIFO and write enable signal are High, 0:The full signal of V(B) channel FIFO is Low, or full is High and write enable signal is Low. */
	#define	FIFOSTATE_BIT_WEU				BIT11									/* 1:The full signal of U(R) channel FIFO and write enable signal are High, 0:The full signal of U(R) channel FIFO is Low, or full is High and write enable signal is Low. */
	#define	FIFOSTATE_BIT_WEY				BIT10									/* 1:The full signal of Y channel FIFO and write enable signal are High, 0:The full signal of Y channel FIFO is Low, or full is High and write enable signal is Low. */
	#define	FIFOSTATE_BIT_EO				BIT8									/* 1:The state of overlay FIFO is empty, 0:The state of overlay FIFO is non-empty. */
	#define	FIFOSTATE_BIT_EV				BIT7									/* 1:The state of V(B) channel FIFO is empty, 0:The state of V(B) channel FIFO is non-empty. */
	#define	FIFOSTATE_BIT_EU				BIT6									/* 1:The state of U(R) channel FIFO is empty, 0:The state of U(R) channel FIFO is non-empty. */
	#define	FIFOSTATE_BIT_EY				BIT5									/* 1:The state of Y channel FIFO is empty, 0:The state of Y channel FIFO is non-empty. */
	#define	FIFOSTATE_BIT_FO				BIT3									/* 1:The state of overlay FIFO is full, 0:The state of overlay FIFO is non-full. */
	#define	FIFOSTATE_BIT_FV				BIT2									/* 1:The state of V(B) channel FIFO is full, 0:The state of V(B) channel FIFO is non-full. */
	#define	FIFOSTATE_BIT_FU				BIT1									/* 1:The state of U(R) channel FIFO is full, 0:The state of U(R) channel FIFO is non-full */
	#define	FIFOSTATE_BIT_FY				BIT0									/* 1:The state of Y channel FIFO is full, 0:The state of Y channel FIFO is non-full. */

#define	CIF_INT_STATUS_REGISTER				TCC7901_CIF_CIRQ_REGISTER				/* Interrupt & Status register */
	#define	ISR_BIT_IEN_EN					BIT31									/* Interrupt Enable */
	#define	ISR_BIT_URV						BIT30									/* Update Register in VSYNC */
	#define	ISR_BIT_ITY						BIT29									/* Interrupt Type */
	#define	ISR_BIT_ICR						BIT28									/* Interrupt Clear */
	#define	ISR_BIT_MVN						BIT26									/* Mask Interrupt of VS negative edge */
	#define	ISR_BIT_MVP						BIT25									/* Mask Interrupt of VS positive edge */
	#define	ISR_BIT_MVIT					BIT24									/* Mask Interrupt of VCNT INterrupt */
	#define	ISR_BIT_MSE						BIT23									/* Mask Interrupt of Scaler Error */
	#define	ISR_BIT_MSF						BIT22									/* Mask Interrupt of Scaler finish */
	#define	ISR_BIT_MENS					BIT21									/* Mask Interrupt of Encoding start */
	#define	ISR_BIT_MRLV					BIT20									/* Mask Interrupt of Rolling V address */
	#define	ISR_BIT_MRLU					BIT19									/* Mask Interrupt of Rolling U address */
	#define	ISR_BIT_MRLY					BIT18									/* Mask Interrupt of Rolling Y address */
	#define	ISR_BIT_MSCF					BIT17									/* Mask Interrupt of Capture frame */
	#define	ISR_BIT_MSOF					BIT16									/* Mask Interrupt of Stored one frame */
	#define	ISR_BIT_VSS						BIT12									/* Status of vertical sync  */
	#define	ISR_BIT_VN						BIT10									/* VS negative */
	#define	ISR_BIT_VP						BIT9									/* VS positive */
	#define	ISR_BIT_VIT						BIT8									/* VCNT Interrupt */
	#define	ISR_BIT_SE						BIT7									/* Scaler Error */
	#define	ISR_BIT_SF						BIT6									/* Scaler Finish */
	#define	ISR_BIT_ENS						BIT5									/* Encoding start stauts */
	#define	ISR_BIT_ROLV					BIT4									/* Rolling V address status */
	#define	ISR_BIT_ROLU					BIT3									/* Rolling U address status */
	#define	ISR_BIT_ROLY					BIT2									/* Rolling Y address status */
	#define	ISR_BIT_SCF						BIT1									/* Stored captured frame */
	#define	ISR_BIT_SOF						BIT0									/* Stored one frame */

#define	OVER_CONTROL_REGISTER1				TCC7901_CIF_OCTRL1_REGISTER				/* Overlay Control 1 */
	#define	OCTRL1_BIT_OCNT_MAX				(BIT28|BIT27|BIT26|BIT25|BIT24)			
	#define	OCTRL1_BIT_OM_BLOCK				BIT16									/* Block image overlay */
	#define	OCTRL1_BIT_OE_EN				BIT12									/* Overlay enable */
	#define	OCTRL1_BIT_XR1_100				BIT10									/* 100% */
	#define	OCTRL1_BIT_XR0_100				BIT9									/* 100% */
	#define	OCTRL1_BIT_AP1_25				~(BIT7|BIT6)							/* 25% */			
	#define	OCTRL1_BIT_AP1_50				BIT6									/* 50% */
	#define	OCTRL1_BIT_AP1_75				BIT7									/* 75% */				
	#define	OCTRL1_BIT_AP1_100				(BIT7|BIT6)								/* 100% */		
	#define	OCTRL1_BIT_AP0_25				~(BIT5|BIT4)							/* 25% */			
	#define	OCTRL1_BIT_AP0_50				BIT4									/* 50% */
	#define	OCTRL1_BIT_AP0_75				BIT5									/* 75% */				
	#define	OCTRL1_BIT_AP0_100				(BIT5|BIT4)								/* 100% */
	#define OCTRL1_BIT_AEN_EN				BIT2									/* Alpha enable */
	#define OCTRL1_BIT_CEN_EN				BIT0									/* Chroma key enable */

#define	OVER_CONTROL_REGISTER2				TCC7901_CIF_OCTRL2_REGISTER				/* Overlay Control 2 */
	#define	OCTRL2_BIT_CONV_EN				BIT3									/* Color Converter Enable */
	#define	OCTRL2_BIT_RGB_565				~(BIT2+BIT1)							/* 565RGB */
	#define	OCTRL2_BIT_RGB_555				BIT1									/* 555RGB */
	#define	OCTRL2_BIT_RGB_444				BIT2									/* 444RGB */
	#define	OCTRL2_BIT_RGB_332				(BIT2+BIT1)								/* 332RGB */
	#define	OCTRL2_BIT_MD_RGB				BIT0									/* Color Mode, 0 = YUB, 1 = RGB */
	#define	OCTRL2_BIT_MD_YUV				~BIT0
	
#define	OVER_CONTROL_REGISTER3				TCC7901_CIF_OCTRL3_REGISTER				/* Overlay Control 3 */
	#define	OCTRL3_BIT_KEYR_MAX				(0xff<<BIT16)
	#define	OCTRL3_BIT_KEYG_MAX				(0xff<<BIT8)
	#define	OCTRL3_BIT_KEYB_MAX				0xff
                                    		
#define	OVER_CONTROL_REGISTER4				TCC7901_CIF_OCTRL4_REGISTER				/* Overlay Control 4 */
	#define	OCTRL4_BIT_MKEYR_MAX			(0xff<<BIT16)
	#define	OCTRL4_BIT_MKEYG_MAX			(0xff<<BIT8)
	#define	OCTRL4_BIT_MKEYB_MAX			0xff

#define	OVER_IMAGE_SIZE_REGISTER			TCC7901_CIF_OIS_REGISTER				/* Overlay Image Size */
#define	OVER_IMAGE_WINDOW1_REGISTER			TCC7901_CIF_OIW1_REGISTER				/* Overlay Image Windowing 1 */
#define	OVER_IMAGE_WINDOW2_REGISTER			TCC7901_CIF_OIW2_REGISTER				/* Overlay Image Windowing 2 */
#define	OVER_BASEADDR_REGISTER				TCC7901_CIF_COBA_REGISTER				/* Overlay Base Address */
#define	CAMERA_SCALER_REGISTER				TCC7901_CIF_CDS_REGISTER				/* Camera Down Scaler */
	#define	CDS_BIT_SFH_1					~(BIT5|BIT4)							/* 1/1 down scale */
	#define	CDS_BIT_SFH_2					BIT4									/* 1/2 down scale */
	#define	CDS_BIT_SFH_4					BIT5									/* 1/4 down scale */
	#define	CDS_BIT_SFH_8					(BIT5|BIT4)								/* 1/8 down scale */
	#define	CDS_BIT_SFV_1					~(BIT3|BIT2)							/* 1/1 down scale */
	#define	CDS_BIT_SFV_2					BIT2									/* 1/2 down scale */
	#define	CDS_BIT_SFV_4					BIT3									/* 1/4 down scale */
	#define	CDS_BIT_SFV_8					(BIT3|BIT2)								/* 1/8 down scale */
	#define CDS_BIT_SEN_EN					BIT0									/* Scale enable */
  
#define	CAPTURE_MODE_CFG_REGISTER1			TCC7901_CIF_CCM1_REGISTER				/* Capture Mode Configuration 1 */
	#define	CMC_BIT_CB						BIT10									/* Capture Busy */
	#define	CMC_BIT_EIT						BIT9									/* Encoding INT count */
	#define	CMC_BIT_UES_EN					BIT8									/* Using Encoding Start Address */
	#define	CMC_BIT_RLV_EN					BIT3									/* Rolling address V */
	#define	CMC_BIT_RLU_EN					BIT2									/* Rolling address U */
	#define	CMC_BIT_RLY_EN					BIT1									/* Rolling address Y */
	#define	CMC_BIT_CAP						BIT0									/* Image Capture */

#define	CAPTURE_MODE_CFG_REGISTER2			TCC7901_CIF_CCM2_REGISTER				/* Capture Mode Configuration 2 */
#define	ENCODE_STARTADDR_REGISTER			TCC7901_CIF_CESA_REGISTER				/* Point Encoding Start Address */
#define	RGB2YUV_FORMAT_CFG_REGISTER			TCC7901_CIF_CR2Y_REGISTER				/* RGB2YUV Format converter Configuration */
	#define	R2YFCR_BIT_565RGB_SEQ16			~(BIT4|BIT3|BIT2|BIT1)					/* 16bit 565RGB (RGB sequence) */
	#define	R2YFCR_BIT_565BGR_SEQ16			BIT1									/* 16bit 565RGB (BGR sequence) */
	#define	R2YFCR_BIT_555RGB_GAR16			BIT3									/* 16bit 555RGB	(RGB-garbage) */
	#define	R2YFCR_BIT_555BGR_GAR16			(BIT3|BIT1)								/* 16bit 555RGB (BGR-garbage) */
	#define	R2YFCR_BIT_555GAR_RGB16			(BIT3|BIT2)								/* 16bit 555RGB	(garbage-RGB) */
	#define	R2YFCR_BIT_555GAR_BGR16			(BIT3|BIT2+BIT1)						/* 16bit 555RGB (garbage-BGR) */
	#define	R2YFCR_BIT_565RGB_SEQ8			BIT4									/* 8bit 565RGB (RGB sequence) */
	#define	R2YFCR_BIT_565BGR_SEQ8			(BIT4|BIT1)								/* 8bit 565RGB (BGR sequence) */
	#define	R2YFCR_BIT_555RGB_GAR8			(BIT4|BIT3)								/* 8bit 555RGB (RGB-garbage) */
	#define	R2YFCR_BIT_555BGR_GAR8			(BIT4|BIT3|BIT1)						/* 8bit 555RGB (BGR-garbage) */
	#define	R2YFCR_BIT_555GAR_RGB8			(BIT4|BIT3|BIT2)						/* 8bit 555RGB (garbage-RGB) */
	#define	R2YFCR_BIT_555GAR_BGR8			(BIT4|BIT3|BIT2|BIT1)					/* 8bit 555RGB (garbage-BGR) */
	#define	R2YFCR_BIT_EN					BIT0									/* R2Y Enable */
#define	CURRENT_YADDR_REGISTER				TCC7901_CIF_CCYA_REGISTER				/* Current Y Address */
#define	CURRENT_UADDR_REGISTER				TCC7901_CIF_CCYU_REGISTER				/* Current U Address */
#define	CURRENT_VADDR_REGISTER				TCC7901_CIF_CCYV_REGISTER				/* Current V Address */
#define	CURRENT_LINECOUNT_REGISTER			TCC7901_CIF_CCLC_REGISTER				/* Current line count */

/* Effect Register Map */
#define	EFFECT_MODE_REGISTER				TCC7901_CIF_CEM_REGISTER				/* Effect mode register */
	#define EMR_BIT_UVS_ENABLE				BIT15
	#define EMR_BIT_VB_ENABLE				BIT14
	#define EMR_BIT_UB_ENABLE				BIT13
	#define EMR_BIT_YB_ENABLE				BIT12
	#define EMR_BIT_YCS_ENABLE				BIT11
	#define EMR_BIT_IVY_ENABLE				BIT10
	#define EMR_BIT_STC_ENABLE				BIT9
	#define EMR_BIT_YCL_ENABLE				BIT8
	#define EMR_BIT_CS_ENABLE				BIT7
	#define EMR_BIT_SKT_ENABLE				BIT6
	#define EMR_BIT_EMM_ENABLE				BIT5
	#define EMR_BIT_EMB_ENABLE				BIT4
	#define EMR_BIT_NEGA_ENABLE				BIT3
	#define EMR_BIT_GRAY_ENABLE				BIT2
	#define EMR_BIT_SEPI_ENABLE				BIT1
	#define EMR_BIT_NOR_ENABLE				BIT0

#define	SEPIA_UV_SET_REGISTTER				TCC7901_CIF_CSUV_REGISTER				/* Sepia UV setting */
#define	COLOR_SELECT_REGISTER				TCC7901_CIF_CCS_REGISTER				/* Color selection register */
#define	FILTER_COEFFICIENT_REGISTER			TCC7901_CIF_CHFC_REGISTER				/* H-filter coefficent0 */
#define	SKETCH_THRESHOLD_REGISTER			TCC7901_CIF_CST_REGISTER				/* Sketch threshold register */
#define	CLAMP_THRESHOLD_REGISTER			TCC7901_CIF_CCT_REGISTER				/* Clamp threshold register */
#define	BIAS_REGISTER						TCC7901_CIF_CBR_REGISTER				/* BIAS register */
#define	IMAGE_SIZE_REGISTER					TCC7901_CIF_CEIS_REGISTER				/* Image size register */
//#define	NPATH_CFG_REGISTER				TCC7901_CIF_INPATH_CTRL_REGISTER		/* npath configuration */
#define	SOURCE_YADDR_REGSITER				TCC7901_CIF_CISA1_REGISTER				/* Source address in Y channel */
#define	SOURCE_UADDR_REGSITER				TCC7901_CIF_CISA2_REGISTER				/* Source address in U channel */
#define	SOURCE_VADDR_REGSITER				TCC7901_CIF_CISA3_REGISTER				/* Source address in V channel */
#define	SOURCE_IMAGE_SIZE_REGSITER			TCC7901_CIF_CISS_REGISTER				/* Source image size */
#define	SOURCE_IMAGE_OFFSET_REGSITER		TCC7901_CIF_CISO_REGISTER				/* Source image offset */
#define	DEST_IMAGE_SIZE_REGSITER			TCC7901_CIF_CIDS_REGISTER				/* Destination image size */
#define	TARGET_SCALE_REGISTER				TCC7901_CIF_CIS_REGISTER				/* Target scale1 */

/* Scaler Register Map  */
#define	SCALER_CFG_REGISTER					TCC7901_CIF_CSC_REGISTER				/* Scaler configuration */
	#define	SCALER_BIT_EN					BIT0
		
#define	SCALER_FACTOR_REGISTER				TCC7901_CIF_CSSF_REGISTER				/* Scale factor */
#define	IMAGE_OFFSET_REGISTER				TCC7901_CIF_CSSO_REGISTER				/* Image offset */
#define	SCALER_SOURCE_IMAGE_SIZE_REGISTER	TCC7901_CIF_CSSS_REGISTER				/* Source image size */
#define	SCALER_DEST_IMAGE_SIZE_REGISTER		TCC7901_CIF_CSDS_REGISTER				/* Destination image size */


/*CVS GPIO Define*/
#define PORTF_DATA_REGISTER					TCC7901_GPIO_PORTE_DATA_REGISTER
	#define PORTF_DATA_BIT_CVS				BIT21

#define PORTE_DIR_CTL_REGISTER				TCC7901_GPIO_PORTE_DIRECTION_REGISTER
	#define PORTF_DIR_BIT_CVS				BIT21

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_enable
* 功能:	enalbe cif on
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_enable()	INPUT_IMAGE_CFG_REGISTER |=IICR_BIT_CIF_ON

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_disable
* 功能:	enable cif off
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_disable()	INPUT_IMAGE_CFG_REGISTER &=~IICR_BIT_CIF_ON

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_clearFifoStatus
* 功能:	clear fifo status flag
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_clearFifoStatus()	FIFO_STATUS_REGISTER |=FIFOSTATE_BIT_CLR

/*-----------------------------------------------------------------------------
* 宏:	hyhwCif_intEnable
* 功能:	enable cif interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intEnable()	CIF_INT_STATUS_REGISTER |=(ISR_BIT_IEN_EN|ISR_BIT_ITY)

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_intDisable
* 功能:	disable cif interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intDisable() 	CIF_INT_STATUS_REGISTER &= ~(ISR_BIT_IEN_EN|ISR_BIT_ITY) 

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_frameIntEnable
* 功能:	enable cif frame interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_frameIntEnable()	CIF_INT_STATUS_REGISTER &=~(ISR_BIT_MSOF)

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_frameIntDisable
* 功能:	disable cif frame interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_frameIntDisable() 	CIF_INT_STATUS_REGISTER |= (ISR_BIT_MSOF)

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_clearIntFlag
* 功能:	clear interrupt flag
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_clearIntFlag()		CIF_INT_STATUS_REGISTER |=ISR_BIT_ICR

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_intVsNegativeEdgeEnable
* 功能:	enable VS negative edge interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intVsNegativeEdgeEnable()	CIF_INT_STATUS_REGISTER &= ~ISR_BIT_MVN

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_intVsNegativeEdgeDisable
* 功能:	disable VS negative edge interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intVsNegativeEdgeDisable()	CIF_INT_STATUS_REGISTER |= ISR_BIT_MVN

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_intVsPositiveEdgeEnable
* 功能:	enable VS positive edge interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intVsPositiveEdgeEnable()	CIF_INT_STATUS_REGISTER &= ~ISR_BIT_MVP

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_intVsPositiveEdgeDisable
* 功能:	disable VS positive edge interrupt
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_intVsPositiveEdgeDisable()	CIF_INT_STATUS_REGISTER |= ISR_BIT_MVP

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_setCVSin()
* 功能:	将CVS信号设置成输入
* 参数:	none 
*----------------------------------------------------------------------------*/
//#define hyhwmCif_setCVSin()				PORTF_DIR_CTL_REGISTER &= ~PORTF_DIR_BIT_CVS
#define hyhwmCif_setCVSin()			PORTE_DIR_CTL_REGISTER &= ~PORTF_DIR_BIT_CVS

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_setCVSin()
* 功能:	将CVS信号设置成输入
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_readCVS()			PORTF_DATA_REGISTER &PORTF_DATA_BIT_CVS

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_clearFrameFlag()
* 功能:	清除一帧标志位
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_clearFrameFlag()	CIF_INT_STATUS_REGISTER |= ISR_BIT_SOF

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_checkStoredOneFrame()
* 功能:	check 是否结束一帧数据
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_checkStoredOneFrame()	CIF_INT_STATUS_REGISTER & ISR_BIT_SOF

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_scalerEnable
* 功能:	enalbe scaler on
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_scalerEnable()		SCALER_CFG_REGISTER |=SCALER_BIT_EN

/*-----------------------------------------------------------------------------
* 宏:	hyhwmCif_scalerDisable
* 功能:	enable scaler off
* 参数:	none 
*----------------------------------------------------------------------------*/
#define hyhwmCif_scalerDisable()	SCALER_CFG_REGISTER &=~SCALER_BIT_EN

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_init
* 功能:	initial cif model
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_init();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setScaler
* 功能:	initial Scaler model
* 参数:	INPUT_IMAGE_SIZE		输入图象尺寸大小
*		OUTPUT_IMAGE_SIZE		输出图象尺寸大小
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setScaler(U32 INPUT_IMAGE_SIZE,U32 OUTPUT_IMAGE_SIZE);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setQVGAMode
* 功能:	set output QVGA MODE
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setQVGAMode();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setVGAMode
* 功能:	set output QVGA MODE
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setVGAMode();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setSXGAMode
* 功能:	set output SXGA MODE
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setSXGAMode();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setQXGAMode
* 功能:	set output QXGA MODE
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setQXGAMode();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_frameStart
* 功能:	得到一帧数据
* 参数:	YUV 420数据存储的位置
* 返回:	HYOK or HY_ERROR, 超时错误
*		note: QVGA数据长度: Y = 76800 / U = 19200 / V= 19200 
			  SXGA数据长度: Y =  / U =  / V=  
*----------------------------------------------------------------------------*/
U32 hyhwCif_frameStart(U32 yAddr,U32 uAddr,U32 vAddr);

/*-----------------------------------------------------------------------------
* 宏:	hyhwCif_checkStoredOneFrame()
* 功能:	check 是否结束一帧数据
* 参数:	none 
* 返回:	HYOK or HY_ERROR, 超时错误
*----------------------------------------------------------------------------*/
U32 hyhwCif_getStoredOneFrame(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCif_setOverlay
* 功能:	initial cif model
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCif_setOverlay(void);

#ifdef __cplusplus
}
#endif

#endif //TM_HWVCI_H_