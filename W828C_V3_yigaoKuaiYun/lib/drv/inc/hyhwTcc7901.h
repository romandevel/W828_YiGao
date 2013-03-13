/*************************************************************
%
% Filename     : hyhwTcc7901.h
% Project name : wifi multimedia phone
%
% Copyright 2005 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   081226  1001	       Primary version 
% -------------------------------------------------------------
*/
#ifndef __HYHWTCC7901_H__
#define __HYHWTCC7901_H__

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/

/* ADC Controller Register Map */
#define	TCC7901_ADC_BASEADDR				0xf3004000													/* ADC base address */
#define	TCC7901_ADC_ADCCON_REGISTER			(*(volatile U32 *)(TCC7901_ADC_BASEADDR + 0x00))			/* ADC Control Register */
#define	TCC7901_ADC_ADCDATA_REGISTER		(*(volatile U32 *)(TCC7901_ADC_BASEADDR + 0x04))			/* ADC Data Register */
#define	TCC7901_ADC_ADCCONA_REGISTER		(*(volatile U32 *)(TCC7901_ADC_BASEADDR + 0x80))			/* ADC Control Register */
#define	TCC7901_ADC_ADCSTATUS_REGISTER		(*(volatile U32 *)(TCC7901_ADC_BASEADDR + 0x84))			/* ADC Status Register */
#define	TCC7901_ADC_ADCCFG_REGISTER			(*(volatile U32 *)(TCC7901_ADC_BASEADDR + 0x88))			/* ADC Configuration Register */

/* CKC Register Map */
#define	TCC7901_CKC_BASEADDR				0xF3000000													/* Clock Controller BASE ADDRESS */
#define TCC7901_CKC_CLKCTRL_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR))					/* CPU & Bus Clock Control Register */
#define TCC7901_CKC_PLL0CFG_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x04))			/* PLL0 Configuration Register */
#define TCC7901_CKC_PLL1CFG_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x08))			/* PLL1 Configuration Register */
#define TCC7901_CKC_CLKDIVC_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x0c))			/* Clock Divider Configuration Register */
#define TCC7901_CKC_CLKDIVC1_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x10))			/* PLL1 Divider Configuration Register */
#define TCC7901_CKC_MODECTR_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x14))			/* Operating Mode Control Register */
#define TCC7901_CKC_BCLKCTR0_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x18))			/* Bus Clock Mask Control Register for Group 0 */
#define TCC7901_CKC_BCLKCTR1_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x1c))			/* Bus Clock Mask Control Register for Group 1 */
#define TCC7901_CKC_SWRESET0_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x20))			/* Software Reset Control Register for Group 0 */
#define TCC7901_CKC_SWRESET1_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x24))			/* Software Reset Control Register for Group 1 */
#define TCC7901_CKC_WDTCTRL_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x28))			/* Watchdog Control Register */
#define TCC7901_CKC_PCK_USB11H_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x80))			/* USB11H Clock Control Register */
#define TCC7901_CKC_PCK_SDMMC_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x84))			/* SD/MMC Clock Control Register */
#define TCC7901_CKC_PCK_MSTICK_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x88))			/* Memory Stick Clock Control Register */
#define TCC7901_CKC_PCK_I2C_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x8c))			/* I2C Clock Control Register */
#define TCC7901_CKC_PCK_LCD_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x90))			/* LCD Clock Control Register */
#define TCC7901_CKC_PCK_CAM_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x94))			/* Camera Clock Control Register */
#define TCC7901_CKC_PCK_UART0_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x98))			/* UART0 Clock Control Register */
#define TCC7901_CKC_PCK_UART1_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0x9c))			/* UART1 Clock Control Register */
#define TCC7901_CKC_PCK_UART2_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xa0))			/* UART2 Clock Control Register */
#define TCC7901_CKC_PCK_UART3_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xa4))			/* UART3 Clock Control Register */
#define TCC7901_CKC_PCK_TCT_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xa8))			/* Clock Output via GPIO_A[3] during CLK_OUT1 mode */
#define TCC7901_CKC_PCK_TCX_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xac))			/* Clock Output via GPIO_A[2] during CLK_OUT0 mode */
#define TCC7901_CKC_PCK_TCZ_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xb0))			/* Timer32 T-Clock Control Register */
#define TCC7901_CKC_PCK_DAI_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xb4))			/* DAI Clock Control Register */
#define TCC7901_CKC_PCK_GPSB0_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xb8))			/* GPSB0 Clock Control Register */
#define TCC7901_CKC_PCK_GPSB1_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xbc))			/* GPSB1 Clock Control Register */
#define TCC7901_CKC_PCK_GPSB2_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xc0))			/* GPSB2 Clock Control Register */
#define TCC7901_CKC_PCK_GPSB3_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xc4))			/* GPSB3 Clock Control Register */
#define TCC7901_CKC_PCK_ADC_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xc8))			/* ADC Clock Control Register */
#define TCC7901_CKC_PCK_SPDIF_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xcc))			/* SPDIF TX Clock Control Register */
#define TCC7901_CKC_PCK_RFREQ_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xd0))			/* SDRAM Refresh Clock Control Register */
#define TCC7901_CKC_PCK_SCALER_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xd4))			/* CIF Scaler Clock Control Register */
#define TCC7901_CKC_PCK_EHI0_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xd8))			/* EHI0 Clock Control Register */
#define TCC7901_CKC_PCK_EHI1_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xdc))			/* EHI1 Clock Control Register */
#define TCC7901_CKC_BCKVCFG_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xe0))			/* Bus Clock Mask Register for Video Block */
#define TCC7901_CKC_MCLKCTRL_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xe8))			/* Main Processor Clock Control Register */
#define TCC7901_CKC_SCLKCTRL_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xec))			/* Sub Processor Clock Control Register */
#define TCC7901_CKC_BCLKMSKE0_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xf0))			/* Bus Clock Mask Enable Register for Group 0 */
#define TCC7901_CKC_BCLKMSKE1_REGISTER  	(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xf4))			/* Bus Clock Mask Enable Register for Group 1 */
#define TCC7901_CKC_OPTION0_REGISTER  		(*(volatile U32 *)(TCC7901_CKC_BASEADDR + 0xfc))			/* Option Register */

/* DAI controller */
#define TCC7901_DAI_BASEADDR			0xf0059000
#define TCC7901_DAI_DADI_L0_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x00)) 			/* digital audio left input register 0 */
#define	TCC7901_DAI_DADI_R0_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x04))			/* digital audio right input register 0 */
#define	TCC7901_DAI_DADI_L1_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x08))			/* digital audio left input register 1 */
#define	TCC7901_DAI_DADI_R1_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x0c))			/* digital audio right input register 1 */
#define	TCC7901_DAI_DADI_L2_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x10))			/* digital audio left input register 2 */
#define	TCC7901_DAI_DADI_R2_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x14))   		/* digital audio right input register 2 */
#define	TCC7901_DAI_DADI_L3_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x18))  			/* digital audio left input register 3 */
#define	TCC7901_DAI_DADI_R3_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x1c))			/* digital audio right input register 3 */

#define TCC7901_DAI_DADO_L0_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x20)) 			/* digital audio left output register 0 */
#define	TCC7901_DAI_DADO_R0_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x24))			/* digital audio right output register 0 */
#define	TCC7901_DAI_DADO_L1_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x28))			/* digital audio left output register 1 */
#define	TCC7901_DAI_DADO_R1_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x2c))			/* digital audio right output register 1 */
#define	TCC7901_DAI_DADO_L2_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x30))			/* digital audio left output register 2 */
#define	TCC7901_DAI_DADO_R2_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x34))   		/* digital audio right output register 2 */
#define	TCC7901_DAI_DADO_L3_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x38))   		/* digital audio left output register 3 */
#define	TCC7901_DAI_DADO_R3_REGISTER	*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x3c))			/* digital audio right output register 3 */

#define TCC7901_DAI_DAMR_REGISTER		*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x40))			/* digital audio mode register */
#define TCC7901_DAI_DAVC_REGISTER		*((volatile unsigned *)(TCC7901_DAI_BASEADDR + 0x44))			/* digital audio volume control register */

/* CD Interface 20120619 Zeet added. */
#define TCC7901_DAI_CICR_REGISTER		*((volatile U32 *)(TCC7901_DAI_BASEADDR + 0x90))			/* digital audio volume control register */

/* DMA controller */
#define TCC7901_GDMA0_BASEADDR			0xf0040000														/* GDMA0 base address */
#define TCC7901_DMA0_CONFIG_REGISTR		*((volatile U32 *)(TCC7901_GDMA0_BASEADDR+0x0090))				/* channel configuration register */
#define TCC7901_GDMA1_BASEADDR			0xf0040100														/* GDMA1 base address */
#define TCC7901_DMA1_CONFIG_REGISTR		*((volatile U32 *)(TCC7901_GDMA1_BASEADDR+0x0090))				/* channel configuration register */
#define TCC7901_GDMA2_BASEADDR			0xf0040200														/* GDMA2 base address */
#define TCC7901_DMA2_CONFIG_REGISTR		*((volatile U32 *)(TCC7901_GDMA2_BASEADDR+0x0090))				/* channel configuration register */

/*ECC REGISTER*/
#define TCC7901_ECC_REGISTER_BASEADDR		0xF005B000	
#define TCC7901_ECC_CTRL_REGISTER  			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0000))
#define TCC7901_ECC_BASE_REGISTER  			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0004))
#define TCC7901_ECC_MASK_REGISTER			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0008))
#define TCC7901_ECC_CLEAR_REGISTER			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x000C))

#define TCC7901_ECC_SECC_0_REGISTER			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0010))
#define TCC7901_ECC_SECC_1_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0014))
#define TCC7901_ECC_SECC_2_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0018))
#define TCC7901_ECC_SECC_3_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x001C))
#define TCC7901_ECC_SECC_4_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0020))
#define TCC7901_ECC_SECC_5_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0024))
#define TCC7901_ECC_SECC_6_REGISTER			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0028))
#define TCC7901_ECC_SECC_7_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x002C))
#define TCC7901_ECC_SECC_8_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0030))
#define TCC7901_ECC_SECC_9_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0034))
#define TCC7901_ECC_SECC_10_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0038))
#define TCC7901_ECC_SECC_11_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x003C))
#define TCC7901_ECC_SECC_12_REGISTER		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0040))
#define TCC7901_ECC_SECC_13_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0044))
#define TCC7901_ECC_SECC_14_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0048))
#define TCC7901_ECC_SECC_15_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x004C))

#define TCC7901_ECC_MECC4_0_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0010))
#define TCC7901_ECC_MECC4_1_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0014))
#define TCC7901_ECC_MECC4_2_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0018))


#define TCC7901_ECC_MECC8_0_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0010))
#define TCC7901_ECC_MECC8_1_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0014))
#define TCC7901_ECC_MECC8_2_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0018))
#define TCC7901_ECC_MECC8_3_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x001C))
#define TCC7901_ECC_MECC8_4_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0020))


#define TCC7901_ECC_SECC_EADDR0_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0050))
#define TCC7901_ECC_SECC_EADDR1_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0054))
#define TCC7901_ECC_SECC_EADDR2_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0058))
#define TCC7901_ECC_SECC_EADDR3_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x005C))
#define TCC7901_ECC_SECC_EADDR4_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0060))
#define TCC7901_ECC_SECC_EADDR5_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0064))
#define TCC7901_ECC_SECC_EADDR6_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0068))
#define TCC7901_ECC_SECC_EADDR7_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x006C))
#define TCC7901_ECC_SECC_EADDR8_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0070))
#define TCC7901_ECC_SECC_EADDR9_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0074))
#define TCC7901_ECC_SECC_EADDR10_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0078))
#define TCC7901_ECC_SECC_EADDR11_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x007C))
#define TCC7901_ECC_SECC_EADDR12_REGISTER 	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0080))
#define TCC7901_ECC_SECC_EADDR13_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0084))
#define TCC7901_ECC_SECC_EADDR14_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0088))
#define TCC7901_ECC_SECC_EADDR15_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x008C))

#define TCC7901_ECC_MECC4_EADDR0_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0050))
#define TCC7901_ECC_MECC4_EADDR1_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0054))
#define TCC7901_ECC_MECC4_EADDR2_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0058))
#define TCC7901_ECC_MECC4_EADDR3_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x005C))

#define TCC7901_ECC_MECC4_EDATA0_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0070))
#define TCC7901_ECC_MECC4_EDATA1_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0074))
#define TCC7901_ECC_MECC4_EDATA2_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0078))
#define TCC7901_ECC_MECC4_EDATA3_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x007C))


#define TCC7901_ECC_MECC8_EADDR0_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0050))
#define TCC7901_ECC_MECC8_EADDR1_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0054))
#define TCC7901_ECC_MECC8_EADDR2_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0058))
#define TCC7901_ECC_MECC8_EADDR3_REGISTER 	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x005C))
#define TCC7901_ECC_MECC8_EADDR4_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0060))
#define TCC7901_ECC_MECC8_EADDR5_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0064))
#define TCC7901_ECC_MECC8_EADDR6_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0068))
#define TCC7901_ECC_MECC8_EADDR7_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x006C))


#define TCC7901_ECC_MECC8_EDATA0_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0070))
#define TCC7901_ECC_MECC8_EDATA1_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0074))
#define TCC7901_ECC_MECC8_EDATA2_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0078))
#define TCC7901_ECC_MECC8_EDATA3_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x007C))
#define TCC7901_ECC_MECC8_EDATA4_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0080))
#define TCC7901_ECC_MECC8_EDATA5_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0084))
#define TCC7901_ECC_MECC8_EDATA6_REGISTER 	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0088))
#define TCC7901_ECC_MECC8_EDATA7_REGISTER  	*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x008C))


#define TCC7901_ECC_ERRNUM_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0090))
#define TCC7901_ECC_IREQ_REGISTER  			*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0094))
#define TCC7901_ECC_FSMSTATE_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x0098))

#define TCC7901_ECC_ENCSEED_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x00F0))
#define TCC7901_ECC_ENCMASK_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x00F4))
#define TCC7901_ECC_ENCDATA_REGISTER  		*((volatile U32 *)(TCC7901_ECC_REGISTER_BASEADDR+0x00F8))


/* EMI SDRAM Registers */
#define	TCC7901_EMI_BASEADDR				0xF1000000													/* emi base address */
#define TCC7901_EMI_SDCFG_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0000))
#define TCC7901_EMI_SDFSM_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0004))
#define TCC7901_EMI_MCFG_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0008))
#define TCC7901_EMI_TST_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x000C))
#define TCC7901_EMI_CSCFG0_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0010))
#define TCC7901_EMI_CSCFG3_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x001C))
#define TCC7901_EMI_CLKCFG_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0020))
#define TCC7901_EMI_SDCMD_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0024))
#define TCC7901_EMI_SDCFG1_REGISTER			*((volatile U32 *)(TCC7901_EMI_BASEADDR+0x0028))

/* GPIO Registers */
#define	TCC7901_GPIO_BASEADDR				0xF005A000																/* gpio base address */

#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER0 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x00))    	/* GPIOC[23:0] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER1 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x04))		/* GPIOC[31:25],GPIOF[27]*/
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER2 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x08))		/* GPIOF[26:22],GPIOF[19:7] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER3 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x0c))		/* GPIOF[6:0],,GPIOD[5] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER4 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x10))		/* GPIOA[5:2],GPIOD[8:6],GPIOC[24] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER5 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x14))		/* GPIOB[6:0],GPIOF[28] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER6 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x18))		/* GPIOB[14:7] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER7 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x1c))		/* GPIOA[8:6],GPIOB[15],GPIOD[12:9] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER8 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x20))		/* GPIOA[12:9],GPIOE[27:24] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER9 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x24))		/* GPIOE[31:28],GPIOE[3:0] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER10 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x28))		/* GPIOE[11:4],GPIOE[3:0] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER11 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x2c))		/* GPIOF[31:29],GPIOE[20],GPIOD[4:0],GPIOA[1:0] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER12 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x30))		/* GPIOE[16:12],GPIOE[23:21] */
#define	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER13 			(*(volatile U32 *)(TCC7901_GPIO_BASEADDR + 0x34))		/* GPIOE[20:17] */

#define	TCC7901_GPIO_GROUP_REGISTER_BASEADDR				0xF005A040	

/* A组GPIO */
#define	TCC7901_GPIO_PORTA_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x00))	/* GPA Data Register */
#define	TCC7901_GPIO_PORTA_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x04))	/* GPA Output Enable Register */
#define	TCC7901_GPIO_PORTA_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x08))	/* OR function on GPA Output Data */
#define	TCC7901_GPIO_PORTA_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x0c))	/* BIC function on GPA Output Data */
#define	TCC7901_GPIO_PORTA_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x10))	/* XOR function on GPA Output Data */
/* B组GPIO */
#define	TCC7901_GPIO_PORTB_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x20))	/* GPB Data Register */
#define	TCC7901_GPIO_PORTB_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x24))	/* GPB Output Enable Register */
#define	TCC7901_GPIO_PORTB_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x28))	/* OR function on GPB Output Data */
#define	TCC7901_GPIO_PORTB_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x2c))	/* BIC function on GPB Output Data */
#define	TCC7901_GPIO_PORTB_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x30))	/* XOR function on GPB Output Data */
/* C组GPIO */
#define	TCC7901_GPIO_PORTC_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x40))	/* GPC Data Register */
#define	TCC7901_GPIO_PORTC_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x44))	/* GPC Output Enable Register */
#define	TCC7901_GPIO_PORTC_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x48))	/* OR function on GPC Output Data */
#define	TCC7901_GPIO_PORTC_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x4c))	/* BIC function on GPC Output Data */
#define	TCC7901_GPIO_PORTC_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x50))	/* XOR function on GPC Output Data */
/* D组GPIO */
#define	TCC7901_GPIO_PORTD_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x60))	/* GPD Data Register */
#define	TCC7901_GPIO_PORTD_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x64))	/* GPD Output Enable Register */
#define	TCC7901_GPIO_PORTD_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x68))	/* OR function on GPD Output Data */
#define	TCC7901_GPIO_PORTD_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x6c))	/* BIC function on GPD Output Data */
#define	TCC7901_GPIO_PORTD_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x70))	/* XOR function on GPD Output Data */
/* E组GPIO */
#define	TCC7901_GPIO_PORTE_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x80))	/* GPE Data Register */
#define	TCC7901_GPIO_PORTE_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x84))	/* GPE Output Enable Register */
#define	TCC7901_GPIO_PORTE_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x88))	/* OR function on GPE Output Data */
#define	TCC7901_GPIO_PORTE_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x8c))	/* BIC function on GPE Output Data */
#define	TCC7901_GPIO_PORTE_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0x90))	/* XOR function on GPE Output Data */
/* F组GPIO */
#define	TCC7901_GPIO_PORTF_DATA_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0xA0))	/* GPF Data Register */
#define	TCC7901_GPIO_PORTF_OUTPUT_ENABLE_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0xA4))	/* GPF Output Enable Register */
#define	TCC7901_GPIO_PORTF_OR_FUNCTION_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0xA8))	/* OR function on GPF Output Data */
#define	TCC7901_GPIO_PORTF_BIC_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0xAc))	/* BIC function on GPF Output Data */
#define	TCC7901_GPIO_PORTF_XOR_FUNCTION_REGISTER			(*(volatile U32 *)(TCC7901_GPIO_GROUP_REGISTER_BASEADDR + 0xB0))	/* XOR function on GPF Output Data */

#define	TCC7901_GPIO_PULL_CONTROL_BASEADDR					0xF005A100

#define	TCC7901_GPIO_PULL_CONTROL0_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x00))		/* Pull-Up/Down Control Register 0 */		
#define	TCC7901_GPIO_PULL_CONTROL1_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x04))		/* Pull-Up/Down Control Register 1 */
#define	TCC7901_GPIO_PULL_CONTROL2_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x08))		/* Pull-Up/Down Control Register 2 */
#define	TCC7901_GPIO_PULL_CONTROL3_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x0c))		/* Pull-Up/Down Control Register 3  GPIOA[2] */
#define	TCC7901_GPIO_PULL_CONTROL4_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x10))		/* Pull-Up/Down Control Register 4  GPIOB[10:0]*/
#define	TCC7901_GPIO_PULL_CONTROL5_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x14))		/* Pull-Up/Down Control Register 5 	GPIOB[15:11] GPIOA[12:6]*/
#define	TCC7901_GPIO_PULL_CONTROL6_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x18))		/* Pull-Up/Down Control Register 6 */
#define	TCC7901_GPIO_PULL_CONTROL7_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x1c))		/* Pull-Up/Down Control Register 7 */
#define	TCC7901_GPIO_PULL_CONTROL8_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x20))		/* Pull-Up/Down Control Register 8 */
#define	TCC7901_GPIO_PULL_CONTROL9_REGISTER					(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x24))		/* Pull-Up/Down Control Register 9 */
#define	TCC7901_GPIO_DRIVER_STRENGTH_CONTROL0_REGISTER		(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x28))		/* Driver strength Control Register 0 */
#define	TCC7901_GPIO_DRIVER_STRENGTH_CONTROL1_REGISTER		(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x2c))		/* Driver strength Control Register 1 */
#define	TCC7901_GPIO_ANALOG_INPUT_PAD_CONTROL_REGISTER		(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x30))		/* Analog Input Pad Control Register */
#define	TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT0_REGISTER	(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x34))		/* External Interrupt Select Register 0 */
#define	TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT1_REGISTER	(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x38))		/* External Interrupt Select Register 1 */
#define	TCC7901_GPIO_INTERRUPT_SELECT_REGISTER				(*(volatile U32 *)(TCC7901_GPIO_PULL_CONTROL_BASEADDR + 0x3c))		/* Interrupt Select Register */


/* 这个是为CIF 模块 场信号检测使用，所以单独定义*/
#define	TCC7901_GPIO_PORTE_DIRECTION_REGISTER		(*(volatile U32 *)(0xF005A0C4))

/* GPSB module */
#define TCC7901_GPSB_BASEADDR			0xf0057000	/* GPSB base address */																						
#define	TCC7901_GPSB_OFFSETADDR			0x100	

#define	TCC7901_GPSBCH0_BASEADDR		0xf0057000	/* channel 0 base address */
#define	TCC7901_GPSBCH1_BASEADDR		0xf0057100	/* channel 1 base address */	
#define	TCC7901_GPSBCH2_BASEADDR		0xf0057200	/* channel 2 base address */	
#define	TCC7901_GPSBCH3_BASEADDR		0xf0057300	/* channel 3 base address */

//channel 0
#define TCC7901_GPSBCH0_PORT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x000)) 	/* 	channel 0 data port register */
#define TCC7901_GPSBCH0_STAT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x004)) 	/* 	channel 0 status register */
#define TCC7901_GPSBCH0_INTEN_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x008)) 	/* 	channel 0 interrupt enable register */
#define TCC7901_GPSBCH0_MODE_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x00c)) 	/* 	channel 0 mode register */
#define TCC7901_GPSBCH0_CTRL_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x010)) 	/* 	channel 0 Control register */
#define TCC7901_GPSBCH0_TXBASE_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x020)) 	/* 	channel 0 Tx base address  register */
#define TCC7901_GPSBCH0_RXBASE_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x024)) 	/* 	channel 0 Rx base address  register */
#define TCC7901_GPSBCH0_PACKET_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x028)) 	/* 	channel 0 Packet  register */
#define TCC7901_GPSBCH0_DMACTR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x02c)) 	/* 	channel 0 DMA control register */
#define TCC7901_GPSBCH0_DMASTR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x030)) 	/* 	channel 0 DMA status register */
#define TCC7901_GPSBCH0_DMAICR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x034)) 	/* 	channel 0 DMA interrupt control register */

//channel 1
#define TCC7901_GPSBCH1_PORT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x100)) 	/* 	channel 1 data port register */
#define TCC7901_GPSBCH1_STAT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x104)) 	/* 	channel 1 status register */
#define TCC7901_GPSBCH1_INTEN_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x108)) 	/* 	channel 1 interrupt enable register */
#define TCC7901_GPSBCH1_MODE_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x10c)) 	/* 	channel 1 mode register */
#define TCC7901_GPSBCH1_CTRL_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x110)) 	/* 	channel 1 Control register */
#define TCC7901_GPSBCH1_TXBASE_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x120)) 	/* 	channel 1 Tx base address  register */
#define TCC7901_GPSBCH1_RXBASE_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x124)) 	/* 	channel 1 Rx base address  register */
#define TCC7901_GPSBCH1_PACKET_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x128)) 	/* 	channel 1 Packet  register */
#define TCC7901_GPSBCH1_DMACTR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x12c)) 	/* 	channel 1 DMA control register */
#define TCC7901_GPSBCH1_DMASTR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x130)) 	/* 	channel 1 DMA status register */
#define TCC7901_GPSBCH1_DMAICR_REGISTER *((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x134)) 	/* 	channel 1 DMA interrupt control register */


//channel 2
#define TCC7901_GPSBCH2_PORT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x200)) 	/* 	channel 2 data port register */
#define TCC7901_GPSBCH2_STAT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x204)) 	/* 	channel 2 status register */
#define TCC7901_GPSBCH2_INTEN_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x208)) 	/* 	channel 2 interrupt enable register */
#define TCC7901_GPSBCH2_MODE_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x20c)) 	/* 	channel 2 mode register */
#define TCC7901_GPSBCH2_CTRL_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x210)) 	/* 	channel 2 Control register */

//channel 3
#define TCC7901_GPSBCH3_PORT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x300)) 	/* 	channel 3 data port register */
#define TCC7901_GPSBCH3_STAT_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x304)) 	/* 	channel 3 status register */
#define TCC7901_GPSBCH3_INTEN_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x308)) 	/* 	channel 3 interrupt enable register */
#define TCC7901_GPSBCH3_MODE_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x30c)) 	/* 	channel 3 mode register */
#define TCC7901_GPSBCH3_CTRL_REGISTER	*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x310)) 	/* 	channel 3 Control register */

#define TCC7901_GPSB_PCFG_REGISTER		*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0x800)) 	/* 	Port config register */
#define TCC7901_GPSB_PIDT_REGISTER		*((volatile unsigned *)(TCC7901_GPSB_BASEADDR + 0xf00)) 	/* 	PID table register */


/* LCD module register definition */
#define	TCC7901_LCD_BASEADDR				0xf0000000												/* LCD Controller BASE ADDRESS */

/* LCDC	lcd interface Register map */
#define	TCC7901_LCDC_LCTRL_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x00))	/* LCD Control Register */
#define	TCC7901_LCDC_LBC_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x04))	/* LCD Background Color Register */
#define	TCC7901_LCDC_LCLKDIV_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x08))	/* LCD Clock Divider Register */
#define	TCC7901_LCDC_LHTIME1_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x0c))	/* LCD Horizontal Timing Register 1 */
#define	TCC7901_LCDC_LHTIME2_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x10))	/* LCD Horizontal Timing Register 2 */
#define	TCC7901_LCDC_LVTIME1_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x14))	/* LCD Vertical Timing Register 1 */
#define	TCC7901_LCDC_LVTIME2_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x18))	/* LCD Vertical Timing Register 2 */
#define	TCC7901_LCDC_LVTIME3_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x1c))	/* LCD Vertical Timing Register 3 */
#define	TCC7901_LCDC_LVTIME4_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x20))	/* LCD Vertical Timing Register 4 */
#define	TCC7901_LCDC_LLUTR_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x24))	/* LCD Lookup Register for Red */
#define	TCC7901_LCDC_LLUTG_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x28))	/* LCD Lookup Register for Green */
#define	TCC7901_LCDC_LLUTB_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x2c))	/* LCD Lookup Register for Blue */
#define	TCC7901_LCDC_LDP7L_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x30))	/* LCD Modulo 7 Dithering Pattern (low) */
#define	TCC7901_LCDC_LDP7H_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x34))	/* LCD Modulo 7 Dithering Pattern (high) */
#define	TCC7901_LCDC_LDP5_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x38))	/* LCD Modulo 5 Dithering Pattern Register */
#define	TCC7901_LCDC_LDP4_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x3c))	/* LCD Modulo 4 Dithering Pattern Register */
#define	TCC7901_LCDC_LDP3_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x40))	/* LCD 3-bit Dithering Pattern Register */
#define	TCC7901_LCDC_LCP1_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x44))	/* LCD Clipping Register1 */
#define	TCC7901_LCDC_LCP2_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x48))	/* LCD Clipping Register2 */
#define	TCC7901_LCDC_LK1_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x4c))	/* LCD Keying Register 1 */
#define	TCC7901_LCDC_LK2_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x50))	/* LCD Keying Register 2 */
#define	TCC7901_LCDC_LKM1_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x54))	/* LCD Keying Mask Register 1 */
#define	TCC7901_LCDC_LKM2_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x58))	/* LCD Keying Mask Register 2 */
#define	TCC7901_LCDC_LDS_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x5c))	/* LCD Display Size Register */
#define	TCC7901_LCDC_LSTATUS_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x60))	/* LCD Status Register */
#define	TCC7901_LCDC_LIM_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x64))	/* LCD Interrupt Register */
#define	TCC7901_LCDC_LI0C_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x68))	/* LCD Image 0 Control Register */
#define	TCC7901_LCDC_LI0P_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x6c))	/* LCD Image 0 Position Register */
#define	TCC7901_LCDC_LI0S_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x70))	/* LCD Image 0 Size Register */
#define	TCC7901_LCDC_LI0BA0_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x74))	/* LCD Image 0 Base Address 0 Register */
#define	TCC7901_LCDC_LI0CA_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x78))	/* LCD Image 0 Current Address Register */
#define	TCC7901_LCDC_LI0BA1_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x7c))	/* LCD Image 0 Base Address 1 Register */
#define	TCC7901_LCDC_LI0BA2_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x80))	/* LCD Image 0 Base Address 2 Register */
#define	TCC7901_LCDC_LI0O_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x84))	/* LCD Image 0 Offset Register */
#define	TCC7901_LCDC_LI0SR_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x88))	/* LCD Image 0 scale ratio */
#define	TCC7901_LCDC_LI1C_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x8c))	/* LCD Image 1 Control Register */
#define	TCC7901_LCDC_LI1P_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x90))	/* LCD Image 1 Position Register */
#define	TCC7901_LCDC_LI1S_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x94))	/* LCD Image 1 Size Register */
#define	TCC7901_LCDC_LI1BA0_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x98))	/* LCD Image 1 Base Address 0 Register */
#define	TCC7901_LCDC_LI1CA_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x9c))	/* LCD Image 1 Current Address Register */
#define	TCC7901_LCDC_LI1O_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xa8))	/* LCD Image 1 Offset Register */
#define	TCC7901_LCDC_LI1SR_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xac))	/* LCD Image 1 Scale ratio */
#define	TCC7901_LCDC_LI2C_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xb0))	/* LCD Image 2 Control Register */
#define	TCC7901_LCDC_LI2P_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xb4))	/* LCD Image 2 Position Register */
#define	TCC7901_LCDC_LI2S_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xb8))	/* LCD Image 2 Size Register */
#define	TCC7901_LCDC_LI2BA0_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xbc))	/* LCD Image 2 Base Address 0 Register */
#define	TCC7901_LCDC_LI2CA_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xc0))	/* LCD Image 2 Current Address Register. */
#define	TCC7901_LCDC_LI2O_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xcc))	/* LCD Image 2 Offset Register */
#define	TCC7901_LCDC_LI2SR_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xd0))	/* LCD Image 2 Scale ratio */
#define	TCC7901_LCDC_DLCTRL_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xd4))	/* Dual LCD Control Register */
#define	TCC7901_LCDC_DLCSA0_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xdc))	/* Dual LCD Configuration Start Address 0 */
#define	TCC7901_LCDC_DLCSA1_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xe0))	/* Dual LCD Configuration Start Address 1 */
#define	TCC7901_LCDC_Y2RC_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xe4))	/* YCbCr2RGB Control register */
#define	TCC7901_LCDC_S2LC_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xec))	/* Scaler to LCDC control register */
#define	TCC7901_LCDC_LPMCTRL_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xf0))	/* LCD PIP control register */
#define	TCC7901_LCDC_LPMW0_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xf4))	/* LCD PIP window register 0 */
#define	TCC7901_LCDC_LPMW1_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xf8))	/* LCD PIP window register 1 */
#define	TCC7901_LCDC_LPMH_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xfc))	/* LCD PIP Height register */
#define	TCC7901_LCDC_LPMC_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x100))	/* LCD PIP Chroma-keying register */
#define	TCC7901_LCDC_LPMMC_REGISTER			(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x104))	/* LCD PIP Chroma-keying Mask register */
#define	TCC7901_LCDC_LCDLUT_REGISTER		(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0xc00))	/* LCD lookup table */

/* LCDSI lcd system interface Register map */
#define TCC7901_LCD_LCDSI_CTRL0_REGISTER 	(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x400))	/* Control register for LCDSI */
#define TCC7901_LCD_LCDSI_CTRL1_REGISTER 	(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x800))	/* Control register for LCSN[0] when LXA=0 (for core access) */
#define TCC7901_LCD_LCDSI_CTRL2_REGISTER 	(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x804))	/* Control register for LCSN[0] when LXA=1 (for core access) */
#define TCC7901_LCD_LCDSI_CTRL3_REGISTER 	(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x808))	/* Control register for LCSN[1] when LXA=0 (for core access) */
#define TCC7901_LCD_LCDSI_CTRL4_REGISTER 	(*(volatile U32 *)(TCC7901_LCD_BASEADDR + 0x80c))	/* Control register for LCSN[1] when LXA=1 (for core access) */


/*  SDIO Host Controller 接口寄存器定义 */ 
#define	TCC7901_SDPORTCTRL_REGISTER			 *((volatile U32 *)(0xF0090800)) 	

#define	TCC7901_SDIO_BASEADDR				0xF0090100 	//0xF0090000/

#define	TCC7901_SDIO_SDMA_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x00))	/* SDMA System Address */
#define	TCC7901_SDIO_BSIZE_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x04))	/* Block Size */
#define	TCC7901_SDIO_BCNT_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x06))	/* Block Count */
#define	TCC7901_SDIO_ARG_REGISTER			*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x08))	/* Argument */
#define	TCC7901_SDIO_TMODE_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x0C))	/* Transfer Mode */
#define	TCC7901_SDIO_CMD_REGISTER			*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x0E))	/* Command */
#define	TCC7901_SDIO_RESP0_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x10))	/* Response0 */
#define	TCC7901_SDIO_RESP1_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x12))	/* Response1 */
#define	TCC7901_SDIO_RESP2_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x14))	/* Response2 */
#define	TCC7901_SDIO_RESP3_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x16))	/* Response3 */
#define	TCC7901_SDIO_RESP4_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x18))	/* Response4 */
#define	TCC7901_SDIO_RESP5_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x1A))	/* Response5 */
#define	TCC7901_SDIO_RESP6_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x1C))	/* Response6 */
#define	TCC7901_SDIO_RESP7_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x1E))	/* Response7 */
#define	TCC7901_SDIO_DATAL_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x20))	/* Buffer Data Port(Low) */
#define TCC7901_SDIO_DATA_REGISTER_REG8     *((volatile  U8 *)(TCC7901_SDIO_BASEADDR + 0x20))
#define	TCC7901_SDIO_DATAH_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x22))	/* Buffer Data Port(High) */
#define	TCC7901_SDIO_STATEL_REGISTER		*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x24))	/* Present State(Low) */
#define	TCC7901_SDIO_STATEH_REGISTER		*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x26))	/* Present State(Low) */
#define	TCC7901_SDIO_CONTL_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x28))	/* Power Control / Host Control */
#define	TCC7901_SDIO_CONTH_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x2A))	/* Wakeup Control / Block Gap Control */
#define	TCC7901_SDIO_CLK_REGISTER			*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x2C))	/* Clock Control */
#define	TCC7901_SDIO_TIME_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x2E))	/* Software Reset / Timeout Control */
#define	TCC7901_SDIO_STSL_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x30))	/* Normal Interrupt Status(Low) */
#define	TCC7901_SDIO_STSH_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x32))	/* Normal Interrupt Status(High) */
#define	TCC7901_SDIO_STSENL_REGISTER		*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x34))	/* Normal Interrupt Status Enable(Low) */
#define	TCC7901_SDIO_STSENH_REGISTER		*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x36))	/* Normal Interrupt Status Enable(High) */
#define	TCC7901_SDIO_INTENL_REGISTER		*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x38))	/* Normal Interrupt Signal Enable(Low) */
#define	TCC7901_SDIO_INTENH_REGISTER		*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x3A))	/* Normal Interrupt Signal Enable(High) */
#define	TCC7901_SDIO_CMD12ERR_REGISTER	    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x3C))	/* Auto Cmd12 Error Status */
#define	TCC7901_SDIO_CAPL_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x40))	/* Capabilities(Low) */
#define	TCC7901_SDIO_CAPH_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x42))	/* Capabilities(High) */
#define	TCC7901_SDIO_CURL_REGISTER		    *((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x48))	/* Maximum Current Capabilities(Low) */
#define	TCC7901_SDIO_CURH_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x4A))	/* Maximum Current Capabilities(High) */
#define	TCC7901_SDIO_FORCEL_REGISTER		*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x50))	/* Force event for AutoCmd12 Error */
#define	TCC7901_SDIO_FORCEH_REGISTER		*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x52))	/* Force event for Error Interrupt Status */
#define	TCC7901_SDIO_ADMAERR_REGISTER		*((volatile U32 *)(TCC7901_SDIO_BASEADDR + 0x54))	/* ADMA Error Status */
#define	TCC7901_SDIO_ADDR0_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x58))	/* ADMA Address[15:0] */
#define	TCC7901_SDIO_ADDR1_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x5A))	/* ADMA Address[31:16] */
#define	TCC7901_SDIO_ADDR2_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x5C))	/* ADMA Address[47:32] */
#define	TCC7901_SDIO_ADDR3_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0x5E))	/* ADMA Address[63:48] */
#define	TCC7901_SDIO_SLOT_REGISTER		    *((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0xFC))	/* Slot Interrupt Status */
#define	TCC7901_SDIO_VERSION_REGISTER		*((volatile U16 *)(TCC7901_SDIO_BASEADDR + 0xFE))	/* Host Controller Version */

/*NandFlash Controller register*/
#define TCC7901_NFC_BASEADDR				0xF00B0000			
#define TCC7901_NFC_CMD_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0000))
#define TCC7901_NFC_LADDR_REGISTER  		*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0004))
#define TCC7901_NFC_BADDR_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0008))
#define TCC7901_NFC_SADDR_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x000C))
#define TCC7901_NFC_WDATA_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0010))
#define TCC7901_NFC_LDATA_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0020))
#define TCC7901_NFC_SDATA_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0040))
#define TCC7901_NFC_CTRL_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0050))
#define TCC7901_NFC_PSTART_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0054))
#define TCC7901_NFC_RSTART_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0058))
#define TCC7901_NFC_DSIZE_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x005C))
#define TCC7901_NFC_IREQ_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0060))
#define TCC7901_NFC_RST_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0064))
#define TCC7901_NFC_CTRL1_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0068))
#define TCC7901_NFC_MDATA_REGISTER			*((volatile U32 *)(TCC7901_NFC_BASEADDR+0x0070))



/* I2C Register Map */

#define TCC7901_I2C_BASEADDR				0xF0052000

/* I2C slave */
#define TCC7901_I2C_DPORT_REGISTER			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x80))	/* Data Access port (TX/RX FIFO) */
#define TCC7901_I2C_CTL_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x84))	/* Control register */
#define TCC7901_I2C_ADDR_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x88))	/* Address register */
#define TCC7901_I2C_INT_REGISTER			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x8c))	/* Interrupt Enable Register */
#define TCC7901_I2C_STAT_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x90))	/* Status Register */
#define TCC7901_I2C_MBF_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0x9c))	/* Buffer Valid Flag */
#define TCC7901_I2C_MB0_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0xa0))	/* Data Buffer 0 (Byte 3 ~ 0) */
#define TCC7901_I2C_MB1_REGISTER 			(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0xa4))	/* Data Buffer 1 (Byte 7 ~ 4) */

#define TCC7901_I2C_IRQSTR_REGISTER 		(*(volatile U32 *)(TCC7901_I2C_BASEADDR + 0xc0))	/* IRQ Status Register */


/*	Programmble Interrupt Controller (PIC) 可编程中断控制器的寄存器定义 */
#define	TCC7901_PIC_BASEADDR			0xF3001000

#define	TCC7901_PIC_IEN_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR))			/*	中断使能寄存器  	R/W, Interrupt Enable Register */		
#define	TCC7901_PIC_CLR_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x4))	/*	中断清除寄存器  	R/W, Interrupt Clear Register*/
#define	TCC7901_PIC_STS_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x8))	/*	中断状态寄存器  	R, Interrupt Status Register */
#define	TCC7901_PIC_SEL_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0xC))	/*	IRQ/FIQ选择寄存器	R/W, IRQ/FIQ Selection Register */
#define	TCC7901_PIC_SRC_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x10))	/*	源中断状态寄存器	R, Source Interrupt Status Register */
#define	TCC7901_PIC_MSTS_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x14))	/*	屏蔽状态寄存器 		R, Masked Status Register */
#define	TCC7901_PIC_TIG_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x18))	/*	测试中断产生寄存器	R/W, Test Interrupt Generation Register*/	
#define	TCC7901_PIC_POL_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x1C))	/*	中断极性寄存器		R/W, Interrupt Polarity Register*/	
#define	TCC7901_PIC_IRQ_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x20))	/*	IRQ原始状态寄存器	R, IRQ Raw Status Register*/	
#define	TCC7901_PIC_FIQ_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x24))	/*	FIQ状态寄存器  		R, FIQ Status Register*/
#define	TCC7901_PIC_MIRQ_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x28))	/*	屏蔽IRQ状态寄存器	R, Masked IRQ Status Register*/
#define	TCC7901_PIC_MFIQ_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x2C))	/*	屏蔽FIQ状态寄存器	R, Masked FIQ Status Register*/
#define	TCC7901_PIC_MODE_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x30))	/*	中断触发模式寄存器	R/W, Trigger Mode Register 0:edge, 1:level */
#define	TCC7901_PIC_SYNC_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x34))	/*	同步使能寄存器  	R/W, Synchronizer Enable Register  */
#define	TCC7901_PIC_WKEN_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x38))	/*	唤醒使能寄存器  	R/W, Wakeup Enable Register  */
#define	TCC7901_PIC_MODEA_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x3C))	/*	双边或单边寄存器	R/W, Both Edge or Single Edge Register */
#define	TCC7901_PIC_INTMSK_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x40))	/*	中断输出屏蔽寄存器	R/W, Interrupt Output Masking Register */
#define	TCC7901_PIC_ALLMSK_REGISTER		*((volatile unsigned *)(TCC7901_PIC_BASEADDR + 0x44))	/*	中断全屏蔽寄存器	R/W, All Mask Register */

/* Programmble Interrupt Controller (PIC)可编程中断控制器的寄存器定义 */
#define	TCC7901_VIC_BASEADDR			0xF3001080

#define	TCC7901_VIC_VAIRQ_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x00))	/*	IRQ向量寄存器		R, IRQ Vector Register */		
#define	TCC7901_VIC_VAFIQ_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x04))	/*	FIQ向量寄存器		R, FIQ Vector Register*/
#define	TCC7901_VIC_VNIRQ_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x08))	/*	IRQ向量个数寄存器	R, IRQ Vector Number Register */
#define	TCC7901_VIC_VNFIQ_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x0C))	/*	FIQ向量个数寄存器	R, FIQ Vector Number Register */
#define	TCC7901_VIC_VCTRL_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x10))	/*	向量控制寄存器		R/W, Vector control Register */
#define	TCC7901_VIC_PRI04_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x20))	/*	中断0-3的优先级  	R/W, Priorityes for Interrupt 0~3*/
#define	TCC7901_VIC_PRI08_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x24))	/*	中断4-7的优先级  	R/W, Priorityes for Interrupt 4~7*/
#define	TCC7901_VIC_PRI12_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x28))	/*	中断8-11的优先级  	R/W, Priorityes for Interrupt 8~11*/
#define	TCC7901_VIC_PRI16_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x2C))	/*	中断12-15的优先级  	R/W, Priorityes for Interrupt 12~15*/
#define	TCC7901_VIC_PRI20_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x30))	/*	中断16-19的优先级  	R/W, Priorityes for Interrupt 16~19*/
#define	TCC7901_VIC_PRI24_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x34))	/*	中断20-23的优先级  	R/W, Priorityes for Interrupt 20~23*/
#define	TCC7901_VIC_PRI28_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x38))	/*	中断24-27的优先级  	R/W, Priorityes for Interrupt 24~27*/
#define	TCC7901_VIC_PRI32_REGISTER		*((volatile unsigned *)(TCC7901_VIC_BASEADDR + 0x3C))	/*	中断28-31的优先级  	R/W, Priorityes for Interrupt 28~31*/

/*	复用端口的寄存器，和中断有关的部分 */
#define	TCC7901_PORT_EINTSEL0_REGISTER	*((volatile unsigned *)(0xF005A134))/*	外部中断选择寄存器0，负责外部中断1-3的源的选择	R/W, External Interrupt select0 Register */
#define	TCC7901_PORT_EINTSEL1_REGISTER	*((volatile unsigned *)(0xF005A138))/*	外部中断选择寄存器1，负责外部中断4-7的源的选择	R/W, External Interrupt select0 Register */
#define	TCC7901_PORT_IRQSEL_REGISTER	*((volatile unsigned *)(0xF005A13C))/*	IRQ选择寄存器，R/W, External Interrupt select0 Register */

/*RTC Register Map */
#define	TCC7901_RTC_BASEADDR				0xF3002000			/* RTC base address */
#define	TCC7901_RTC_RTCCON_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x00))		/* RTC Control Register */
#define	TCC7901_RTC_INTCON_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x04))		/* RTC Interrupt Control Register */
#define	TCC7901_RTC_RTCALM_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x08))		/* RTC Alarm Control Register */
#define	TCC7901_RTC_ALMSEC_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x0c))		/* Alarm Second Data Register */
#define	TCC7901_RTC_ALMMIN_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x10))		/* Alarm Minute Data Register */
#define	TCC7901_RTC_ALMHOUR_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x14))		/* Alarm Hour Data Register */
#define	TCC7901_RTC_ALMDATE_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x18))		/* Alarm Date Data Register */
#define	TCC7901_RTC_ALMDAY_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x1c))		/* Alarm Day of Week Data Register */
#define	TCC7901_RTC_ALMMON_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x20))		/* Alarm Month Data Register */
#define	TCC7901_RTC_ALMYEAR_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x24))		/* Alarm Year Data Register */
#define	TCC7901_RTC_BCDSEC_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x28))		/* BCD Second Register */
#define	TCC7901_RTC_BCDMIN_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x2c))		/* BCD Minute Register1 */
#define	TCC7901_RTC_BCDHOUR_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x30))		/* BCD Hour Register */
#define	TCC7901_RTC_BCDDATE_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x34))		/* BCD Date Register */
#define	TCC7901_RTC_BCDDAY_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x38))		/* BCD Day of Week Register */
#define	TCC7901_RTC_BCDMON_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x3c))		/* BCD Month Register */
#define	TCC7901_RTC_BCDYEAR_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x40))		/* BCD Year Register */
#define	TCC7901_RTC_RTCIM_REGISTER			(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x44))		/* RTC Interrupt Mode Register */
#define	TCC7901_RTC_RTCPEND_REGISTER		(*(volatile U32 *)(TCC7901_RTC_BASEADDR + 0x48))		/* RTC Interrupt Pending Register */


//sd interface
#define	TCC7901_SD_BASEADDR				0xF0090000
#define	TCC7901_SD_SDMA_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x00))	
#define	TCC7901_SD_BSIZE_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x04))	
#define	TCC7901_SD_BCNT_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x06))	
#define	TCC7901_SD_ARG_REGISTER			*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x08))	
#define	TCC7901_SD_TMODE_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x0C))	
#define	TCC7901_SD_CMD_REGISTER			*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x0E))	
#define	TCC7901_SD_RESP0_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x10))	
#define	TCC7901_SD_RESP1_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x12))	
#define	TCC7901_SD_RESP2_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x14))	
#define	TCC7901_SD_RESP3_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x16))	
#define	TCC7901_SD_RESP4_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x18))	
#define	TCC7901_SD_RESP5_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x1A))	
#define	TCC7901_SD_RESP6_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x1C))	
#define	TCC7901_SD_RESP7_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x1E))	
#define	TCC7901_SD_DATAL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x20))	
#define	TCC7901_SD_DATAH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x22))	
#define	TCC7901_SD_STATEL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x24))	
#define	TCC7901_SD_STATEH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x26))	
#define	TCC7901_SD_CONTL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x28))	
#define	TCC7901_SD_CONTH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x2A))	
#define	TCC7901_SD_CLK_REGISTER			*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x2C))	
#define	TCC7901_SD_TIME_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x2E))	
#define	TCC7901_SD_STSL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x30))	
#define	TCC7901_SD_STSH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x32))	
#define	TCC7901_SD_STSENL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x34))	
#define	TCC7901_SD_STSENH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x36))	
#define	TCC7901_SD_INTENL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x36))	
#define	TCC7901_SD_INTENH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x38))	
#define	TCC7901_SD_CMD12ERR_REGISTER	*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x3C))	
#define	TCC7901_SD_CAPL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x40))	
#define	TCC7901_SD_CAPH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x42))	
#define	TCC7901_SD_CURL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x48))	
#define	TCC7901_SD_CURH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x4A))	
#define	TCC7901_SD_FORCEL_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x50))	
#define	TCC7901_SD_FORCEH_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x52))	
#define	TCC7901_SD_ADMAERR_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x54))	
#define	TCC7901_SD_ADDR0_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x58))	
#define	TCC7901_SD_ADDR1_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x5A))	
#define	TCC7901_SD_ADDR2_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x5C))	
#define	TCC7901_SD_ADDR3_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0x5E))	
#define	TCC7901_SD_SLOT_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0xFC))	
#define	TCC7901_SD_VERSION_REGISTER		*((volatile unsigned *)(TCC7901_SD_BASEADDR + 0xFE))

/* Timer Registers Map*/		
#define TCC7901_TIMER_BASEADDR				0xF3003000											/* TIMER BASE ADDRESS */
#define TCC7901_TIMER_TCFG0_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x00)	/* Timer/Counter 0 Configuration Register */
#define TCC7901_TIMER_TCNT0_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x04))	/* Timer/Counter 0 Counter Register */
#define TCC7901_TIMER_TREF0_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x08))	/* Timer/Counter 0 Reference Register */
#define TCC7901_TIMER_TMREF0_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x0c))	/* Timer/Counter 0 Middle Reference Register */
#define TCC7901_TIMER_TCFG1_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x10))	/* Timer/Counter 1 Configuration Register */
#define TCC7901_TIMER_TCNT1_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x14))	/* Timer/Counter 1 Counter Register */
#define TCC7901_TIMER_TREF1_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x18))	/* Timer/Counter 1 Reference Register */
#define TCC7901_TIMER_TMREF1_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x1c))	/* Timer/Counter 1 Middle Reference Register */
#define TCC7901_TIMER_TCFG2_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x20))	/* Timer/Counter 2 Configuration Register */
#define TCC7901_TIMER_TCNT2_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x24))	/* Timer/Counter 2 Counter Register */
#define TCC7901_TIMER_TREF2_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x28))	/* Timer/Counter 2 Reference Register */
#define TCC7901_TIMER_TMREF2_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x2c))	/* Timer/Counter 2 Middle Reference Register */
#define TCC7901_TIMER_TCFG3_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x30))	/* Timer/Counter 3 Configuration Register */
#define TCC7901_TIMER_TCNT3_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x34))	/* Timer/Counter 3 Counter Register */
#define TCC7901_TIMER_TREF3_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x38))	/* Timer/Counter 3 Reference Register */
#define TCC7901_TIMER_TMREF3_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x3c))	/* Timer/Counter 3 Middle Reference Register */
#define TCC7901_TIMER_TCFG4_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x40))	/* Timer/Counter 4 Configuration Register */
#define TCC7901_TIMER_TCNT4_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x44))	/* Timer/Counter 4 Counter Register */
#define TCC7901_TIMER_TREF4_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x48))	/* Timer/Counter 4 Reference Register */
#define TCC7901_TIMER_TCFG5_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x50))	/* Timer/Counter 5 Configuration Register */
#define TCC7901_TIMER_TCNT5_REGISTER  		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x54))	/* Timer/Counter 5 Counter Register */
#define TCC7901_TIMER_TREF5_REGISTER   		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x58))	/* Timer/Counter 5 Reference Register */

#define TCC7901_TIMER_TIREQ_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x60))	/* Timer/Counter n Interrupt Request Register */
#define TCC7901_TIMER_TWDCFG_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x70))	/* Watchdog Timer Configuration Register */ 
#define TCC7901_TIMER_TWDCLR_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x74))	/* Watchdog Timer Clear Register */

	/*offset 0x78 没有寄存器 */
#define TCC7901_TIMER_TC32EN_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x80))	/* 32-bit Counter Enable / Pre-scale Value */
#define TCC7901_TIMER_TC32LDV_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x84))	/* 32-bit Counter Load Value */
#define TCC7901_TIMER_TC32CMP0_REGISTER 	(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x88))	/* 32-bit Counter Match Value 0 */
#define TCC7901_TIMER_TC32CMP1_REGISTER 	(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x8c))	/* 32-bit Counter Match Value 1 */ 
#define TCC7901_TIMER_TC32PCNT_REGISTER 	(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x90))	/* 32-bit Counter Current Value (pre-scale counter) */
#define TCC7901_TIMER_TC32MCNT_REGISTER 	(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x94))	/* 32-bit Counter Current Value (main counter) */
#define TCC7901_TIMER_TC32IRQ_REGISTER 		(*(volatile U32 *)(TCC7901_TIMER_BASEADDR + 0x98))	/* 32-bit Counter Interrupt Control */


/*UART Registers Map*/
#define	TCC7901_UART_BASEADDR				0xF0055000											/* uart base address */
#define	TCC7901_UART_OFFSETADDR				0x100	

#define	TCC7901_UART_BASEADDR0				0xF0055000											/* uart0 base address */
#define	TCC7901_UART_BASEADDR1				0xF0055100											/* uart1 base address */	
#define	TCC7901_UART_BASEADDR2				0xF0055200											/* uart2 base address */	
#define	TCC7901_UART_BASEADDR3				0xF0055300											/* uart3 base address */	


#define	TCC7901_UART_IIR_REGISTER0			(*(volatile U32 *)(TCC7901_UART_BASEADDR0 + 0x04))	/* UART0  Interrupt Register */
#define	TCC7901_UART_IIR_REGISTER1			(*(volatile U32 *)(TCC7901_UART_BASEADDR1 + 0x04))	/* UART1  Interrupt Register */
#define	TCC7901_UART_IIR_REGISTER2			(*(volatile U32 *)(TCC7901_UART_BASEADDR2 + 0x04))	/* UART2  Interrupt Register */
#define	TCC7901_UART_IIR_REGISTER3			(*(volatile U32 *)(TCC7901_UART_BASEADDR3 + 0x04))	/* UART3  Interrupt Register */


#define	TCC7901_UART0_SCCR_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x60))	/* SmartCard Configuration Register */
#define	TCC7901_UART0_STC_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x64))	/* SmartCard Tx Countr */
#define	TCC7901_UART0_IRDA_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x80))	/* SmartCard Tx Countr */
		
#define	TCC7901_UART1_SCCR_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x160))	/* SmartCard Configuration Register */
#define	TCC7901_UART1_STC_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x164))	/* SmartCard Tx Countr */
#define	TCC7901_UART1_IRDA_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x180))	/* SmartCard Tx Countr */

#define	TCC7901_UART2_SCCR_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x260))	/* SmartCard Configuration Register */
#define	TCC7901_UART2_STC_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x264))	/* SmartCard Tx Countr */
#define	TCC7901_UART2_IRDA_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x280))	/* SmartCard Tx Countr */

#define	TCC7901_UART3_SCCR_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x360))	/* SmartCard Configuration Register */
#define	TCC7901_UART3_STC_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x364))	/* SmartCard Tx Countr */
#define	TCC7901_UART3_IRDA_REGISTER			(*(volatile U32 *)(TCC7901_UART_BASEADDR + 0x380))	/* SmartCard Tx Countr */


/*USB Registers*/
#define TCC7901_USB_BASEADDRESS				0xF0010000	
/*NOINDEX part*/
#define TCC7901_USB_INDEX_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0000))
#define TCC7901_USB_EIR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0004))
#define TCC7901_USB_EIER_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0008))
//#define TCC7901_USB_FAR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x000C))
//#define TCC7901_USB_FNR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0010))
#define TCC7901_USB_EDR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0014))
#define TCC7901_USB_TEST_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0018))
#define TCC7901_USB_SSR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x001C))
#define TCC7901_USB_SCR1_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0020))
#define TCC7901_USB_EP0SR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0024))
#define TCC7901_USB_EP0CR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0028))
#define TCC7901_USB_SCR2_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0058))
#define TCC7901_USB_EP0BUF_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0060))
#define TCC7901_USB_EP1BUF_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0064))
#define TCC7901_USB_EP2BUF_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0068))
#define TCC7901_USB_EP3BUF_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x006C))
#define TCC7901_USB_PLICR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00A0))
#define TCC7901_USB_PCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00A4))
/*INDEX part(应该是需要通过INDEX寄存器去设置所要读取的寄存器)*/
#define TCC7901_USB_ESR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x002C))
#define TCC7901_USB_ECR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0030))
#define TCC7901_USB_BRCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0034))
#define TCC7901_USB_BWCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0038))
#define TCC7901_USB_MPR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x003C))
#define TCC7901_USB_DCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0040))
#define TCC7901_USB_DTCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0044))
#define TCC7901_USB_DFCR_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0048))
#define TCC7901_USB_DTTCR1_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x004C))
#define TCC7901_USB_DTTCR2_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0050))
#define TCC7901_USB_ESR2_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x0054))
/*USB Configuration Register*/
#define TCC7901_USB_UPCR0_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00C8))
#define TCC7901_USB_UPCR1_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00CC))
#define TCC7901_USB_UPCR2_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00D0))
#define TCC7901_USB_UPCR3_REGISTER			*((volatile U32 *)(TCC7901_USB_BASEADDRESS+0x00D4))

/* CIF Register Map */
#define TCC7901_CIF_BASEADDR				0xF0060000
#define	TCC7901_CIF_ICPCR1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x00))		/* Input Image Color/Pattern Configuration */
#define	TCC7901_CIF_656FCR1_REGISTER		(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x04))		/* CCIR656 Format Configuration Register 1 */
#define	TCC7901_CIF_656FCR2_REGISTER		(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x08))		/* CCIR656 Format Configuration Register 2 */
#define	TCC7901_CIF_IIS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x0c))		/* Input Image Size */
#define	TCC7901_CIF_IIW1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x10))		/* Input Image Windowing 1 */
#define	TCC7901_CIF_IIW2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x14))		/* Input Image Windowing 2 */
#define	TCC7901_CIF_CDCR1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x18))		/* DMA Configuration Register 1 */
#define	TCC7901_CIF_CDCR2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x1c))		/* DMA Configuration Register 2 */
#define	TCC7901_CIF_CDCR3_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x20))		/* DMA Configuration Register 3 */
#define	TCC7901_CIF_CDCR4_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x24))		/* DMA Configuration Register 4 */
#define	TCC7901_CIF_CDCR5_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x28))		/* DMA Configuration Register 5 */
#define	TCC7901_CIF_CDCR6_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x2c))		/* DMA Configuration Register 6 */
#define	TCC7901_CIF_CDCR7_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x30))		/* DMA Configuration Register 7 */
#define	TCC7901_CIF_FIFOSTATE_REGISTER		(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x34))		/* FIFO Status Register */
#define	TCC7901_CIF_CIRQ_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x38))		/* Interrupt & Status register */
#define	TCC7901_CIF_OCTRL1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x3c))		/* Overlay Control 1 */
#define	TCC7901_CIF_OCTRL2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x40))		/* Overlay Control 2 */
#define	TCC7901_CIF_OCTRL3_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x44))		/* Overlay Control 3 */
#define	TCC7901_CIF_OCTRL4_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x48))		/* Overlay Control 4 */
#define	TCC7901_CIF_OIS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x4c))		/* Overlay Image Size */
#define	TCC7901_CIF_OIW1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x50))		/* Overlay Image Windowing 1 */
#define	TCC7901_CIF_OIW2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x54))		/* Overlay Image Windowing 2 */
#define	TCC7901_CIF_COBA_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x58))		/* Overlay Base Address */
#define	TCC7901_CIF_CDS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x5c))		/* Camera Down Scaler */
#define	TCC7901_CIF_CCM1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x60))		/* Capture Mode Configuration 1 */
#define	TCC7901_CIF_CCM2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x64))		/* Capture Mode Configuration 2 */
#define	TCC7901_CIF_CESA_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x68))		/* Point Encoding Start Address */
#define	TCC7901_CIF_CR2Y_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x6c))		/* RGB2YUV Format converter Configuration */
#define	TCC7901_CIF_CCYA_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x70))		/* Current Y Address *//
#define	TCC7901_CIF_CCYU_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x74))		/* Current U Address */
#define	TCC7901_CIF_CCYV_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x78))		/* Current V Address */
#define	TCC7901_CIF_CCLC_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x7c))		/* Current line couter */

/* Effect Register Map */
#define	TCC7901_CIF_CEM_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x100))		/* Effect mode register */
#define	TCC7901_CIF_CSUV_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x104))		/* Sepia UV setting */
#define	TCC7901_CIF_CCS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x108))		/* Color selection register */
#define	TCC7901_CIF_CHFC_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x10c))		/* H-filter coefficent0 */
#define	TCC7901_CIF_CST_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x110))		/* Sketch threshold register */
#define	TCC7901_CIF_CCT_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x114))		/* Clamp threshold register */
#define	TCC7901_CIF_CBR_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x118))		/* BIAS register */
#define	TCC7901_CIF_CEIS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x11c))		/* Image size register */

//#define	TCC7901_CIF_CISA1_REGISTER		(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x140))		/* npath configuration */
#define	TCC7901_CIF_CISA1_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x144))		/* Source address in Y channel */
#define	TCC7901_CIF_CISA2_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x148))		/* Source address in U channel */
#define	TCC7901_CIF_CISA3_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x14c))		/* Source address in V channel */
#define	TCC7901_CIF_CISS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x150))		/* Source image size */
#define	TCC7901_CIF_CISO_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x154))		/* Source image offset */
#define	TCC7901_CIF_CIDS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x158))		/* Destination image size */
#define	TCC7901_CIF_CIS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x15C))		/* Target scale1 */

/* Scaler Register Map  */
#define	TCC7901_CIF_CSC_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x200))		/* Scaler configuration */
#define	TCC7901_CIF_CSSF_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x204))		/* Scale factor */
#define	TCC7901_CIF_CSSO_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x208))		/* Image offset */
#define	TCC7901_CIF_CSSS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x20c))		/* Source image size */
#define	TCC7901_CIF_CSDS_REGISTER			(*(volatile U32 *)(TCC7901_CIF_BASEADDR + 0x210))		/* Destination image size */

/* miscellaneous controller */
#define TCC7901_ECFG0_REGISTER              (*(volatile U32 *)(0xF300500c))		/* exceptional configuration register 0 */

#define	TCC7901_SDMMC_BASEADDR				0xF0090000 	//slot 0

#define	TCC7901_SDMMC_SDMA_REGISTER		    *((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x00))	/* SDMA System Address */
#define	TCC7901_SDMMC_BSIZE_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x04))	/* Block Size */
#define	TCC7901_SDMMC_BCNT_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x06))	/* Block Count */
#define	TCC7901_SDMMC_ARG_REGISTER			*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x08))	/* Argument */
#define	TCC7901_SDMMC_TMODE_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x0C))	/* Transfer Mode */
#define	TCC7901_SDMMC_CMD_REGISTER			*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x0E))	/* Command */
#define	TCC7901_SDMMC_RESP0_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x10))	/* Response0 */
#define	TCC7901_SDMMC_RESP1_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x12))	/* Response1 */
#define	TCC7901_SDMMC_RESP2_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x14))	/* Response2 */
#define	TCC7901_SDMMC_RESP3_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x16))	/* Response3 */
#define	TCC7901_SDMMC_RESP4_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x18))	/* Response4 */
#define	TCC7901_SDMMC_RESP5_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x1A))	/* Response5 */
#define	TCC7901_SDMMC_RESP6_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x1C))	/* Response6 */
#define	TCC7901_SDMMC_RESP7_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x1E))	/* Response7 */
#define	TCC7901_SDMMC_DATAL_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x20))	/* Buffer Data Port(Low) */
#define TCC7901_SDMMC_DATA_REGISTER_REG8    *((volatile  U8 *)(TCC7901_SDMMC_BASEADDR + 0x20))
#define	TCC7901_SDMMC_DATAH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x22))	/* Buffer Data Port(High) */
#define	TCC7901_SDMMC_STATEL_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x24))	/* Present State(Low) */
#define	TCC7901_SDMMC_STATEH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x26))	/* Present State(Low) */
#define	TCC7901_SDMMC_CONTL_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x28))	/* Power Control / Host Control */
#define	TCC7901_SDMMC_CONTH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x2A))	/* Wakeup Control / Block Gap Control */
#define	TCC7901_SDMMC_CLK_REGISTER			*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x2C))	/* Clock Control */
#define	TCC7901_SDMMC_TIME_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x2E))	/* Software Reset / Timeout Control */
#define	TCC7901_SDMMC_STSL_REGISTER		    *((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x30))	/* Normal Interrupt Status(Low) */
#define	TCC7901_SDMMC_STSH_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x32))	/* Normal Interrupt Status(High) */
#define	TCC7901_SDMMC_STSENL_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x34))	/* Normal Interrupt Status Enable(Low) */
#define	TCC7901_SDMMC_STSENH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x36))	/* Normal Interrupt Status Enable(High) */
#define	TCC7901_SDMMC_INTENL_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x38))	/* Normal Interrupt Signal Enable(Low) */
#define	TCC7901_SDMMC_INTENH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x3A))	/* Normal Interrupt Signal Enable(High) */
#define	TCC7901_SDMMC_CMD12ERR_REGISTER	    *((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x3C))	/* Auto Cmd12 Error Status */
#define	TCC7901_SDMMC_CAPL_REGISTER		    *((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x40))	/* Capabilities(Low) */
#define	TCC7901_SDMMC_CAPH_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x42))	/* Capabilities(High) */
#define	TCC7901_SDMMC_CURL_REGISTER		    *((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x48))	/* Maximum Current Capabilities(Low) */
#define	TCC7901_SDMMC_CURH_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x4A))	/* Maximum Current Capabilities(High) */
#define	TCC7901_SDMMC_FORCEL_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x50))	/* Force event for AutoCmd12 Error */
#define	TCC7901_SDMMC_FORCEH_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x52))	/* Force event for Error Interrupt Status */
#define	TCC7901_SDMMC_ADMAERR_REGISTER		*((volatile U32 *)(TCC7901_SDMMC_BASEADDR + 0x54))	/* ADMA Error Status */
#define	TCC7901_SDMMC_ADDR0_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x58))	/* ADMA Address[15:0] */
#define	TCC7901_SDMMC_ADDR1_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x5A))	/* ADMA Address[31:16] */
#define	TCC7901_SDMMC_ADDR2_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x5C))	/* ADMA Address[47:32] */
#define	TCC7901_SDMMC_ADDR3_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0x5E))	/* ADMA Address[63:48] */
#define	TCC7901_SDMMC_SLOT_REGISTER		    *((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0xFC))	/* Slot Interrupt Status */
#define	TCC7901_SDMMC_VERSION_REGISTER		*((volatile U16 *)(TCC7901_SDMMC_BASEADDR + 0xFE))	/* Host Controller Version */

/* NTSC/PAL Encoder Composite Output.20120618 Zeet added */
#define	TCC7901_TVOUT_BASEADDR				0xF9000000 	
#define	TCC7901_TVOUT_ECMDA_REGISTER		*((volatile U16 *)(TCC7901_TVOUT_BASEADDR + 0x04))	/* BTI7 "1" Power down mode for the entire digital logic circuits of encoder digital core */
#define	TCC7901_TVOUT_DACSEL_REGISTER		*((volatile U16 *)(TCC7901_TVOUT_BASEADDR + 0x40))	/* BTI0 "0" DAC digital output is disabled, Output is code 0 */
#define	TCC7901_TVOUT_DACPD_REGISTER	    *((volatile U16 *)(TCC7901_TVOUT_BASEADDR + 0x50))	/* BTI0 "1" DAC Power down */

/* GRAPHIC ENGINE 20120621 Zeet added */
#define	TCC7901_GE_BASEADDR				0xF6000000 	
#define	TCC7901_GE_CTRL_REGISTER		*((volatile U32 *)(TCC7901_GE_BASEADDR + 0xB0))	/* BTI7 "1" Power down mode for the entire digital logic circuits of encoder digital core */


#endif//#ifndef __HYHWTCC7901_H__

