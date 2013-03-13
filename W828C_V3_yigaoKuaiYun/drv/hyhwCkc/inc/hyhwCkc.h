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

#ifndef HY_HW_CPM_H_
#define HY_HW_CPM_H_

#define 	CKC_CLKCTRL_REGISTER		TCC7901_CKC_CLKCTRL_REGISTER
	#define OSCCLK_BIT_ENABLE			BIT31							// XIN oscillator enable register
	#define MCPUCLK_BIT_MB_ENABLE		BIT29							// Main processor clock selection register,If ‘1’, FMCPU = FBUS
	#define SCPUCLK_BIT_SB_ENABLE		BIT28							// Slave processor clock selection register,If ‘1’, FSCPU = FBUS
	#define MCPUCLK_BIT_DIV				16
	#define MCPUCLK_BIT_DIV_MASK		(0x0f <<MCPUCLK_BIT_DIV)
	#define SCPUCLK_BIT_DIV				12
	#define SCPUCLK_BIT_DIV_MASK		(0x0f <<SCPUCLK_BIT_DIV)
	#define BUSCLK_BIT_DIV				4
	#define BUSCLK_BIT_DIV_MASK			(0x1f << BUSCLK_BIT_DIV)
	#define SYSCLK_BIT_SOU_MASK			0x07

#define		CKC_CLKDIVC_REGISTER		TCC7901_CKC_CLKDIVC_REGISTER  		
	#define PLL0_BIT_DIV_ENABLE			BIT31	
	#define P0P_BIT_DIV					24
	#define P0P_BIT_DIV_MASK			(0x3f << P0P_BIT_DIV)
	#define PLL1_BIT_DIV_ENABLE			BIT23							/* PLL 1 Divider Enable or Disabled 	*/
	#define P1P_BIT_DIV					16
	#define P1P_BIT_DIV_MASK			(0x3f << P1P_BIT_DIV)
	
	#define XIN_BIT_DIV_ENABLE			BIT15							/* XIN Divider Enable or Disabled 		*/ 
	#define XIN_BIT_DIV					8
	#define XIN_BIT_DIV_MASK			(0x3f << XIN_BIT_DIV)
	#define XTIN_BIT_DIV_ENABLE			BIT7							/* XTIN Divider Enable or Disabled 		*/ 
	#define XTIN_BIT_DIV_MASK			0x3f
#define 	CKC_CLKDIVC1_REGISTER		TCC7901_CKC_CLKDIVC1_REGISTER
	#define PLL0_BIT_DIV1_ENABLE		BIT15	
	#define P0P_BIT_DIV1				8
	#define P0P_BIT_DIV1_MASK			(0x3f << P0P_BIT_DIV)
	#define PLL1_BIT_DIV1_ENABLE			BIT7						/* PLL 1 Divider Enable or Disabled 	*/
	#define P1P_BIT_DIV1				0
	#define P1P_BIT_DIV1_MASK			(0x3f << P1P_BIT_DIV)
	
#define 	CKC_PLL0CFG_REGISTER		TCC7901_CKC_PLL0CFG_REGISTER	/* PLL0 Configuration Register			*/
	#define PLL0_BIT_DISABLE			BIT31
	#define PLL0_BIT_SDIV				16
	#define PLL0_BIT_SDIV_MASK			(0x07 << PLL0_BIT_SDIV)
	#define PLL0_BIT_MDIV				8
	#define PLL0_BIT_MDIV_MASK			(0xff << PLL0_BIT_MDIV)
	#define PLL0_BIT_PDIV				0
	#define PLL0_BIT_PDIV_MASK			(0x3f << PLL0_BIT_PDIV)
	
#define 	CKC_PLL1CFG_REGISTER		TCC7901_CKC_PLL1CFG_REGISTER	/* PLL1 Configuration Register 			*/
	#define PLL1_BIT_DISABLE			BIT31
	#define PLL1_BIT_SDIV				16
	#define PLL1_BIT_SDIV_MASK			(0x07 << PLL1_BIT_SDIV)
	#define PLL1_BIT_MDIV				8
	#define PLL1_BIT_MDIV_MASK			(0xff << PLL1_BIT_MDIV)
	#define PLL1_BIT_PDIV				0
	#define PLL1_BIT_PDIV_MASK			(0x3f << PLL1_BIT_PDIV)

#define		CKC_MODECTR_REGISTER 		TCC7901_CKC_MODECTR_REGISTER 	/* Operating Mode Control Register 		*/
	#define MODE_BIT_PWDOWNCLK_XIN		BIT9							/* power dwon mode clk selects XIN 		*/
	#define MODE_BIT_PWDOWNCLK_XTIN		~BIT9							/* power dwon mode clk selects XTIN 	*/
	#define MODE_BIT_PWDOWNENABLE		BIT8							/* power dwon mode enable 				*/
	#define MODE_BIT_HALTENABLE			BIT0							/* halt mode enable 					*/
	
#define		CKC_BCLKCTR0_REGISTER		TCC7901_CKC_BCLKCTR0_REGISTER	/* Bus Clock Mask Control Register for Group 0 */	
#define		CKC_BCLKCTR1_REGISTER		TCC7901_CKC_BCLKCTR1_REGISTER	/* Bus Clock Mask Control Register for Group 1 */
#define		CKC_SWRESET0_REGISTER		TCC7901_CKC_SWRESET0_REGISTER	/* Software Reset Control Register for Group 0 */
#define		CKC_SWRESET1_REGISTER		TCC7901_CKC_SWRESET1_REGISTER	/* Software Reset Control Register for Group 1 */

#define		CKC_WDTCTRL_REGISTER 		TCC7901_CKC_WDTCTRL_REGISTER 	/* Watchdog Control Register			*/
	#define WDOG_ENABLE					BIT31							/* Watchdog Enable register			  --bit 31 */						
	#define WDOG_CLEAR					BIT30							/* Watchdog Clear register			  --bit 30 */
	#define WDOG_STATE					BIT16							/* Watchdog state indication register --bit 16 */
	#define WDOG_WDTCNT_MASK			0xffff							/* Watchdog count regisger 			  --bit 0 - 15 */


//followed is the clk control regiter mask for every module
#define 	DAI_DIV_SELECT			BIT31
#define 	MODEL_BIT_ENABLE		BIT28								// clock divider enable register --bit 28
#define 	MODEL_BIT_SOURCE_MASK		(0x0f << 24)					// clk source select register 	--bit 24-27
#define 	MODEL_BIT_DIV_MASK			(0xffff)						// clk diviser					--bit 0	-11
//follewd is the clk control register for every module
#define		CKC_PCK_USB11H_REGISTER 	TCC7901_CKC_PCK_USB11H_REGISTER	/* USB11H Clock Control Register 		*/
#define		CKC_PCK_SDMMC_REGISTER 		TCC7901_CKC_PCK_SDMMC_REGISTER 	/* SD/MMC Clock Control Register 		*/
#define		CKC_PCK_MSTICK_REGISTER		TCC7901_CKC_PCK_MSTICK_REGISTER	/* Memory Stick Clock Control Register 	*/
#define		CKC_PCK_I2C_REGISTER  		TCC7901_CKC_PCK_I2C_REGISTER  	/* I2C Clock Control Register 			*/
#define		CKC_PCK_LCD_REGISTER  		TCC7901_CKC_PCK_LCD_REGISTER  	/* LCD Clock Control Register 			*/
#define		CKC_PCK_CAM_REGISTER  		TCC7901_CKC_PCK_CAM_REGISTER  	/* Camera Clock Control Register 		*/
#define		CKC_PCK_UART0_REGISTER 		TCC7901_CKC_PCK_UART0_REGISTER 	/* UART0 Clock Control Register 		*/
#define		CKC_PCK_UART1_REGISTER 		TCC7901_CKC_PCK_UART1_REGISTER 	/* UART1 Clock Control Register 		*/
#define		CKC_PCK_UART2_REGISTER 		TCC7901_CKC_PCK_UART2_REGISTER 	/* UART2 Clock Control Register 		*/
#define		CKC_PCK_UART3_REGISTER 		TCC7901_CKC_PCK_UART3_REGISTER 	/* UART3 Clock Control Register 		*/
#define		CKC_PCK_TCT_REGISTER  		TCC7901_CKC_PCK_TCT_REGISTER  	/* Clock Output via GPIO_A[3] during CLK_OUT1 mode */
#define		CKC_PCK_TCX_REGISTER  		TCC7901_CKC_PCK_TCX_REGISTER  	/* Clock Output via GPIO_A[2] during CLK_OUT0 mode */
#define		CKC_PCK_TCZ_REGISTER  		TCC7901_CKC_PCK_TCZ_REGISTER    /* Timer32 T-Clock Control Register 	*/
#define		CKC_PCK_DAI_REGISTER  		TCC7901_CKC_PCK_DAI_REGISTER  	/* DAI Clock Control Register 			*/
#define		CKC_PCK_GPSB0_REGISTER 		TCC7901_CKC_PCK_GPSB0_REGISTER 	/* GPSB0 Clock Control Register 		*/
#define		CKC_PCK_GPSB1_REGISTER 		TCC7901_CKC_PCK_GPSB1_REGISTER 	/* GPSB1 Clock Control Register 		*/
#define		CKC_PCK_GPSB2_REGISTER 		TCC7901_CKC_PCK_GPSB2_REGISTER 	/* GPSB2 Clock Control Register 		*/
#define		CKC_PCK_GPSB3_REGISTER 		TCC7901_CKC_PCK_GPSB3_REGISTER 	/* GPSB3 Clock Control Register 		*/
#define		CKC_PCK_ADC_REGISTER  		TCC7901_CKC_PCK_ADC_REGISTER  	/* ADC Clock Control Register			*/
#define		CKC_PCK_SPDIF_REGISTER 		TCC7901_CKC_PCK_SPDIF_REGISTER 	/* SPDIF TX Clock Control Register 		*/
#define		CKC_PCK_RFREQ_REGISTER 		TCC7901_CKC_PCK_RFREQ_REGISTER 	/* SDRAM Refresh Clock Control Register */
#define		CKC_PCK_SCALER_REGISTER		TCC7901_CKC_PCK_SCALER_REGISTER	/* CIF Scaler Clock Control Register 	*/
#define		CKC_PCK_EHI0_REGISTER  		TCC7901_CKC_PCK_EHI0_REGISTER  	/* EHI0 Clock Control Register 			*/
#define		CKC_PCK_EHI1_REGISTER  		TCC7901_CKC_PCK_EHI1_REGISTER  	/* EHI1 Clock Control Register 			*/

	
#define		CKC_BCKVCFG_REGISTER		TCC7901_CKC_BCKVCFG_REGISTER	/* Bus Clock Mask Register for Video Block 			*/
	#define BCKVCFG_BCKMV_ENABLE		16								/* Bus clock dynamic mask enable for video hardware */
	#define BCKVCFG_BCKMV_ENABLE_MASK	(0xffff << BCKVCFG_BCKMVEN)
	#define BCKVCFG_BCKMV				0								/* Bus clock mask for video hardware		*/
	#define BCKVCFG_BCKMV_MASK			(0xffff << BCKVCFG_BCKMV)

#define		CKC_MCLKCTRL_REGISTER 		TCC7901_CKC_MCLKCTRL_REGISTER  	/* Main Processor Clock Control Register 	*/
	#define MCLKCTRL_ACCESS_ENABLE		BIT31							/* Access enabled */
	#define MCLKCTRL_MB					BIT8							/* Main processor clock selection register 	*/
	#define MCLKCTRL_MDIV_MASK			0x0f							/* 分频数掩码 */
#define		CKC_SCLKCTRL_REGISTER 		TCC7901_CKC_SCLKCTRL_REGISTER  	/* Sub Processor Clock Control Register 	*/
	#define SCLKCTRL_ACCESS_ENABLE		BIT31							/* Access enabled */
	#define SCLKCTRL_SB					BIT8							/* slave processor clock selection register */
	#define SCLKCTRL_SDIV_MASK			0x0f							/* 分频数掩码 */

#define		CKC_BCLKMSKE0_REGISTER		TCC7901_CKC_BCLKMSKE0_REGISTER 	/* Bus Clock Mask Enable Register for Group 0 */
#define		CKC_BCLKMSKE1_REGISTER		TCC7901_CKC_BCLKMSKE1_REGISTER 	/* Bus Clock Mask Enable Register for Group 1 */

#define		CKC_OPTION0_REGISTER  		TCC7901_CKC_OPTION0_REGISTER  	/* Option Register */
	#define OPTION_BUS_DUTY				BIT0							/* 占空比寄存器 */
	
//#define CKC_PCK_LCD_REGISTER	TCC7901_CKC_PCK_LCD_REGISTER	
/* clk input Source select */
#define 	PLL0_CLK  			0	 
#define 	PLL1_CLK  			1
#define 	DIV_PLL0_DIV0_CLK 	2 
#define 	DIV_PLL1_DIV0_CLK	3
#define 	CLK_12M				4
#define 	DIV_12M_CLK			5
#define 	CLK_32k				6
#define 	DIV_32k_CLK			7
#define 	DIV_PLL0_DIV1_CLK	8
#define 	DIV_PLL1_DIV1_CLK	9
#define 	EXTERN_CLK			10
#define 	TIMER_COUNTER1_CLK	11
#define 	TIMER_COUNTER2_CLK	12
#define 	TIMER_COUNTER3_CLK	13
#define 	TIMER_COUNTER4_CLK	14
#define 	USB_PHY_48M			15




#define 	EXTAL_CLK 		12000	//12000								/* set extal osc clk is 12M UNIT IS khz*/

/* 下面定义这些设备CLK是从12M晶体直接提供的。所以不需要考虑由于主频的改动而变化*/
/* 外设 CLK 定义 单位是 KHz 所以12000 代表12M */

#define		USB11H_CLK		48000		/* set USB11H_CLK clk is 48M 20100427*/				
#define		SDMMC_CLK		3000		/* set SDMMC_CLK clk is 3M 	*/			
#define		MSTICK_CLK		3000		/* set MSTICK_CLK clk is 3M */				
#define		I2C_CLK			3000		/* set I2C_CLK clk is 3M	*/
#define		LCD_CLK			6000		/* set LCD_CLK clk is 3M 	*/								
#define		CAM_CLK			3000		/* set CAM_CLK clk is 3M 	*/

#define		UART0_CLK		48000		/* set UART0_CLK clk is 48M	*/
#define		UART1_CLK		48000		/* set UART1_CLK clk is 3M 	*/
#define		UART2_CLK		48000		/* set UART2_CLK clk is 3M 	*/
#define		UART3_CLK		48000		/* set UART3_CLK clk is 3M 	*/
#define		TCT_CLK		    3000		/* set TCT_CLK clk is 3M 	*/
#define		TCX_CLK		    3000		/* set TCX_CLK clk is 3M 	*/
#define		TCZ_CLK			3000		/* set TCZ_CLK clk is 3M 	*/
#define		DAI_CLK			3000		/* set DAI_CLK clk is 3M 	*/
#define		GPSB0_CLK		3000		/* set GPSB0_CLK clk is 3M 	*/
#define		GPSB1_CLK		8000		/* set GPSB2_CLK clk is 3M	*/
#define		GPSB2_CLK		3000		/* set GPSB2_CLK clk is 3M 	*/
#define		GPSB3_CLK		3000		/* set GPSB3_CLK clk is 3M	*/
#define		ADC_CLK			3000		/* set ADC_CLK clk is 3M 	*/
#define		SPDIF_CLK		3000		/* set SPDIF_CLK clk is 3M	*/
#define		RFREQ_CLK		3000		/* set RFREQ_CLK clk is 3M 	*/
#define		SCALER_CLK		3000		/* set SCALER_CLK clk is 3M	*/
#define		EHI0_CLK		3000		/* set EHI0_CLK clk is 3M 	*/
#define		EHI1_CLK		3000		/* set EHI1_CLK clk is 3M 	*/

typedef enum _eSYS_CLK_EN
{	

	ICLK1M_AHB512K	= 0x03010202,
	ICLK2M_AHB1M	= 0x03010102,
	ICLK6M_AHB3M	= 0x01010102,
	ICLK12M_AHB6M	= 0x01010002,
	
    	   
    ICLK24M_AHB12M	= 0x03600402,
     
    ICLK36M_AHB18M	= 0x04600302,
    ICLK36M_AHB9M	= 0x04600304,
    
    ICLK48M_AHB24M	= 0x03600302,
    ICLK48M_AHB16M	= 0x03600303,
    
	ICLK50M_AHB25M	= 0x094b0102,
    ICLK52M_AHB26M	= 0x03680302,
    ICLK54M_AHB27M	= 0x036c0302,
    ICLK56M_AHB28M	= 0x0f8c0102,
    ICLK58M_AHB29M	= 0x0f910102,
    
	ICLK60M_AHB30M	= 0x04280102,
    ICLK60M_AHB20M	= 0x04280103,
    
    ICLK72M_AHB36M	= 0x03900302,
    ICLK72M_AHB24M	= 0x03900303,
    
	ICLK84M_AHB42M	= 0x03540202,
    ICLK84M_AHB28M	= 0x03540203,
         
    ICLK96M_AHB48M	= 0x03600202,
    ICLK96M_AHB32M	= 0x03600203,
    ICLK96M_AHB24M	= 0x03600204,
    ICLK96M_AHB12M	= 0x03600208,
    
	ICLK108M_AHB54M	= 0x036c0202,
    ICLK108M_AHB36M	= 0x036c0203,
    
    ICLK120M_AHB60M	= 0x04280002,
    ICLK120M_AHB40M	= 0x04280003,
    
    ICLK132M_AHB66M	= 0x03840202,
    ICLK132M_AHB44M	= 0x03840203,
    
    ICLK144M_AHB72M	= 0x03900202,
    ICLK144M_AHB48M	= 0x03900203,
    
    ICLK156M_AHB78M	= 0x034e0102,
    ICLK156M_AHB52M	= 0x034e0103,
    
    ICLK168M_AHB84M	= 0x03540102,
    ICLK168M_AHB56M	= 0x03540103,
    
    ICLK180M_AHB90M	= 0x035a0102,
    ICLK180M_AHB60M	= 0x035a0103,
        
    ICLK192M_AHB96M	= 0x03600102,
    ICLK192M_AHB64M	= 0x03600103,
    ICLK192M_AHB48M = 0x03600104,
    
    ICLK240M_AHB120M = 0x03780102,
    ICLK250M_AHB125M = 0x037d0102,
    ICLK260M_AHB130M = 0x03820102,
    ICLK264M_AHB132M = 0x03840102,   
    ICLK266M_AHB133M = 0x03850102,
    
    ICLK300M_AHB100M = 0x034b0003,
    ICLK332M_AHB166M = 0x03530002,
    ICLK360M_AHB120M = 0x011e0003,
    ICLK399M_AHB133M = 0x02850103,

    ICLK420M_AHB105M = 0x01230003,
    ICLK456M_AHB114M = 0x024C0003,
    ICLK480M_AHB120M = 0x01280003,
    
    ICLK498M_AHB166M = 0x02530003
} eSYS_Clk_EN;

typedef enum 
{
	PLL_CKC_1M	= 0x030102,
	PLL_CKC_2M	= 0x030101,
	PLL_CKC_6M	= 0x010101,
	PLL_CKC_12M	= 0x010100,
	
    	   
    PLL_CKC_24M	= 0x036004,
     
    PLL_CKC_36M	= 0x046003,
    
    PLL_CKC_48M	= 0x036003,
     
	PLL_CKC_50M	= 0x094b01,
    PLL_CKC_52M	= 0x036803,
    PLL_CKC_54M	= 0x036c03,
    PLL_CKC_56M	= 0x0f8c01,
    PLL_CKC_58M	= 0x0f9101,
    
	PLL_CKC_60M	= 0x042801,
    
    PLL_CKC_72M	= 0x039003,
    
	PLL_CKC_84M	= 0x035402,
         
    PLL_CKC_96M	= 0x036002,
    
	PLL_CKC_108M = 0x036c02,
    
    PLL_CKC_120M = 0x042800,
    
    PLL_CKC_132M = 0x038402,
    
    PLL_CKC_144M = 0x039002,
    
    PLL_CKC_156M = 0x034e01,
    
    PLL_CKC_168M = 0x035401,
    
    PLL_CKC_180M = 0x035a01,
        
    PLL_CKC_192M = 0x036001,
    
    PLL_CKC_240M = 0x037801,
    PLL_CKC_250M = 0x037d01,
    PLL_CKC_260M = 0x038201,
    PLL_CKC_264M = 0x038401,   
    PLL_CKC_266M = 0x038501,
    
    PLL_CKC_300M = 0x034b00,
    PLL_CKC_332M = 0x035300,
    PLL_CKC_360M = 0x011e00,
    PLL_CKC_399M = 0x028501,
    
    PLL_CKC_420M = 0x024C00,
    PLL_CKC_456M = 0x024C00,
    PLL_CKC_480M = 0x012800,
    PLL_CKC_498M = 0x025300
}ePLL_CLK_EN;


typedef enum _eMODEL_ID
{
	/*SWRESET0 */
	CKC_USBH 	= 0,		/* 0  USB 1.1 Host 					*/
	CKC_USBF 	= 1,		/* 1  USB2.0 Function Controller 	*/ 
	CKC_LCD 	= 2,		/* 2  LCD Controller 				*/	
	CKC_CAM 	= 3,		/* 3  Camera Interface 				*/
	CKC_HDD 	= 4,		/* 4  HDD Controller 				*/	
	CKC_DMA 	= 5,		/* 5  DMA Controller				*/ 
	CKC_SDMMC 	= 6,		/* 6  SD/MMC Controller Channel 	*/
	CKC_MSTICK 	= 7,		/* 7  Memory Stick Controller 		*/
	CKC_I2C 	= 8,		/* 8  I2C Controller 				*/
	CKC_NAND 	= 9,		/* 9  NAND Flash 					*/ 
	CKC_EHI0 	= 10,		/* 10 EHI0 Controller 				*/ 
	CKC_UART0 	= 11,		/* 11 UART Controller Channel 0 	*/	
	CKC_UART1	= 12,		/* 12 UART Controller Channel 1 	*/ 	
	CKC_GPSB0	= 13,		/* 13 GPSB Channel 0 				*/ 
	CKC_DAI 	= 14,		/* 14 DAI Controller 				*/ 
	CKC_UART2 	= 15,		/* 15 UART Controller Channel 2 	*/
	CKC_ECC 	= 16,		/* 16 ECC Controller 				*/
	CKC_SPDIF 	= 17,		/* 17 S/PDIF Tx Controller 			*/ 
	CKC_GPSB1	= 18,		/* 18 GPSB Channel 1 				*/ 
	CKC_SUBP	= 19,		/* 19 Sub processor peripherals 	*/ 
	CKC_G2D 	= 20,		/* 20 Graphic 2D 					*/ 	
	CKC_RTC 	= 21,		/* 21 RTC 							*/	
	CKC_EMC 	= 22,		/* 22 External Memroy Controller 	*/	
	CKC_SUB		= 23,		/* 23 Sub processor peripherals 	*/ 
	CKC_ADC 	= 24,		/* 24 ADC 							*/ 	
	CKC_VCH		= 25,		/* 25 Video Controller Hardware 	*/ 
	CKC_TIMER 	= 26,		/* 26 Timer 						*/ 
	CKC_MAIN 	= 27,		/* 27 Main processor 				*/ 
	CKC_VIC 	= 28,		/* 28 Vectored Interrupt Controller */ 
	CKC_MTM 	= 29,		/* 29 Memory-to-Memory Scaler 		*/ 
	CKC_MAILBOX = 30,		/* 30 Mailbox 						*/
	CKC_MBC 	= 31,		/* 31 Main Bus Components 			*/
	
	/*SWRESET1 */	
	CKC_UART3 	= 32,		/* 32 UART Channel 3				*/
	CKC_SRAM 	= 33,		/* 33 SRAM Interface 				*/
	CKC_GPSB2	= 34,		/* 34 GPSB Channel 2				*/ 
	CKC_GPSB3	= 35,		/* 35 GPSB Channel 3 				*/ 
	CKC_EHI1	= 36,		/* 36 EHI 1 Controller 				*/ 
	CKC_VE		= 37		/* 37 Video Encoder 				*/ 

}eMODEL_ID;

#define	PLL1CFG_PD_DIS				BIT31					   // PLL Disable
#define	PLL1CFG_PD_EN				~BIT31					   // PLL Enable
//Done....
	#define	IO_CKC_BUS_RSVD		0x88000000
	#define	IO_CKC_BUS_INT			BIT28
	
	#define	IO_CKC_BUS_VideoH		BIT25
	#define	IO_CKC_BUS_VideoC		BIT23
	#define	IO_CKC_BUS_VideoP		BIT19
	



#define	PLLFREQ(P, M, S)		(( 12000 * (M) )  / (P) ) >> (S) // 100Hz Unit..
#define	FPLL_t(P, M, S)			PLLFREQ(P,M,S), P, M, S

typedef	volatile struct _tSYSCLKREGISTER_ST
{
	U32 mCpuClk;
	U32 sCpuClk;
	U32 busClk;
	U32 fSysClk;
}tSYSCLKREGISTER_ST;

typedef	volatile struct _tFPLL_ST
{
	U32		uFpll;
	U8		P, M, S, dummy;
}tFPLL_ST;



typedef enum _eCLK_SOURCE_SELECT
{
	Direct_PLL0 = 0, 
	Direct_PLL1, 
	Divided0_PLL0, 
	Divided0_PLL1,
	Direct_12M,
	Divided_12M,
	Direct_32k,
	Divided_32k,
	Divided1_PLL0, 
	Divided1_PLL1,	
	Extern_clock,
	TimerCounter1,
	TimerCounter2,
	TimerCounter3,
	TimerCounter4,
	Usb_PHY_48M	

}eCLK_SOURCE_SELECT;

typedef enum _eFSYS_CLK_SOURCE_SELECT
{ /* FSystem Clock Source */
	DIRECTPLL0=0,
	DIRECTPLL1,
	DIVIDEPLL0,
	DIVIDEPLL1,
	DIRECTXIN,
	DIVIDEXIN,
	DIRECTXTIN,
	DIVIDEXTIN
}eFSYS_CLK_SOURCE_SELECT;



typedef enum eperi_clk
{
	PERI_CLK_1M = 1000,
	PERI_CLK_2M = 2000,
	PERI_CLK_4M = 4000,
	PERI_CLK_3M = 3000,
	PERI_CLK_6M = 6000,
	PERI_CLK_12M = 12000,
	PERI_CLK_16M = 16000,
	PERI_CLK_24M = 24000,
	PERI_CLK_48M = 48000	
}ePERI_CLK;
extern tSYSCLKREGISTER_ST  SysClkRegister;

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getCpuClkDiv
* 功能:	get	Divider for CPU Clock Frequency ratio
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getCpuClkDiv() \
 ((CKC_CLKCTRL_REGISTER & MCPUCLK_BIT_DIV_MASK) >> MCPUCLK_BIT_DIV)

/*	宏定义 */
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getSCpuClkDiv
* 功能:	get	Divider for slave CPU Clock Frequency ratio
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getSCpuClkDiv() \
 ((CKC_CLKCTRL_REGISTER & SCPUCLK_BIT_DIV_MASK) >> SCPUCLK_BIT_DIV)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getBusClkDiv
* 功能:	get	Divider for Bus Clock Frequency ratio
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getBusClkDiv() \
 ((CKC_CLKCTRL_REGISTER & BUSCLK_BIT_DIV_MASK) >> BUSCLK_BIT_DIV)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll0m
* 功能:	get	feedback divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll0m() \
	((CKC_PLL0CFG_REGISTER & PLL0_BIT_MDIV_MASK) >> PLL0_BIT_MDIV)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll0p
* 功能:	get	PLL input divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll0p() \
	((CKC_PLL0CFG_REGISTER & PLL0_BIT_PDIV_MASK) >> PLL0_BIT_PDIV)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll0s
* 功能:	get	PLL input divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll0s() \
	((CKC_PLL0CFG_REGISTER & PLL0_BIT_SDIV_MASK) >> PLL0_BIT_SDIV)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll1m
* 功能:	get	feedback divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll1m() \
	((CKC_PLL1CFG_REGISTER & PLL1_BIT_MDIV_MASK) >> PLL1_BIT_MDIV)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll1p
* 功能:	get	PLL input divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll1p() \
	((CKC_PLL1CFG_REGISTER & PLL1_BIT_PDIV_MASK) >> PLL1_BIT_PDIV)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_getPll1s
* 功能:	get	PLL input divider
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwCpm_getPll1s() \
	((CKC_PLL1CFG_REGISTER & PLL1_BIT_SDIV_MASK) >> PLL1_BIT_SDIV)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_oscEnable
* 功能:	enable 12m osc  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_oscEnable() (CKC_CLKCTRL_REGISTER |= OSCCLK_BIT_ENABLE)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_oscDisable
* 功能:	disable 12m osc  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_oscDisable() (CKC_CLKCTRL_REGISTER &= ~OSCCLK_BIT_ENABLE)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_pll0Enable
* 功能:	enable pll0  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_pll0Enable() (CKC_PLL0CFG_REGISTER &= ~PLL0_BIT_DISABLE)

/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_pll0Disable
* 功能:	disable pll0 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_pll0Disable() (CKC_PLL0CFG_REGISTER |= PLL0_BIT_DISABLE)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_pll1Enable
* 功能:	enable pll1  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_pll1Enable() (CKC_PLL1CFG_REGISTER &= ~PLL1_BIT_DISABLE)
/*----------------------------------------------------------------------------
* 宏:	hyhwCpm_pll1Disable
* 功能:	disable pll1 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#define hyhwmCpm_pll1Disable() (CKC_PLL1CFG_REGISTER |= PLL1_BIT_DISABLE)

/* 函数声明 */
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_minSubCpu()
* 功能:	set sub process clock divider
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_minSubCpu(void );

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_closeSubCpu()
* 功能:	close processor
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_closeSubCpu(void );

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_openSubCpu()
* 功能:	open sub processor
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_openSubCpu(void );

/*----------------------------------------------------------------------------
* 函数:	hyhwSubSysClk_Set()
* 功能:	set sub processor's clock
* 参数:	sCpuClk------sub process's clock value 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSubSysClk_Set(U32  sCpuClk);
/*------------------------------------------------------------------------------
* 函数:	hyhwSysClk_Set
* 功能: 设置系统时钟
*			PLL的时钟来源为硬件晶振
*				PLL1为系统提供时钟
*			ICLK,HCLK,PCLK,MCLK的时钟源为PLL
*			此四种CLK的设置是独立的，但因为有外部器件的读写操作，
*			故必须遵从以下规则：（否则可能会造成对外联系的失败）
*				1.ICLK必须是HCLK的整数倍
*				2.HCLK必须是PCLK的整数倍
*				3.HCLK必须是MCLK的一倍或两倍
*			   
				ICLK1M_AHB512K	= 0x000110,
				ICLK2M_AHB1M	= 0x000210,
				ICLK6M_AHB3M	= 0x000610,
				ICLK12M_AHB6M	= 0x000c10,
				
			    ICLK24M_AHB12M	= 0x001810,
			     
			    ICLK36M_AHB18M	= 0x002410,	
			    ICLK36M_AHB9M	= 0x002430,
			    
			    ICLK48M_AHB24M	= 0x003c10,
			    ICLK48M_AHB16M	= 0x003c20,
			    
			    ICLK50M_AHB25M	= 0x003210,	
			    ICLK52M_AHB26M	= 0x003410,
			    ICLK54M_AHB27M	= 0x003610,
			    ICLK56M_AHB28M	= 0x003810,
			    ICLK58M_AHB29M	= 0x003a10,
			    
			    ICLK60M_AHB30M	= 0x003010,
			    ICLK60M_AHB20M	= 0x003020,
			    
			    ICLK72M_AHB36M	= 0x004810,
			    ICLK72M_AHB24M	= 0x004820,
			         
			    ICLK96M_AHB48M	= 0x006010,
			    ICLK96M_AHB32M	= 0x006020,
			    ICLK96M_AHB24M	= 0x006030,
			    ICLK96M_AHB12M	= 0x006070,
		        
		        ICLK108M_AHB54M	= 0x006c10,
			    ICLK108M_AHB36M	= 0x006c20,
			    
			    ICLK120M_AHB60M	= 0x007810,
			    ICLK120M_AHB40M	= 0x007820,
			    
			    ICLK132M_AHB66M	= 0x008410,
			    ICLK132M_AHB44M	= 0x008420,
			    
			    ICLK144M_AHB72M	= 0x009010,
			    ICLK144M_AHB48M	= 0x009020,
			    
			    ICLK156M_AHB78M	= 0x009c10,
			    ICLK156M_AHB52M	= 0x009c20,
			    
			    ICLK168M_AHB84M	= 0x00a810,
			    ICLK168M_AHB56M	= 0x00a820,
			    
			    ICLK180M_AHB90M	= 0x00b410,
			    ICLK180M_AHB60M	= 0x00b420,
		        
		        
			    ICLK192M_AHB96M	= 0x00c010,
			    ICLK192M_AHB64M	= 0x00c020,
			    ICLK192M_AHB48M = 0x00c030,
			    
			    ICLK240M_AHB120M = 0x00f010,
			    ICLK250M_AHB125M = 0x00fa10,
			    ICLK260M_AHB130M = 0x010410,
			    ICLK264M_AHB132M = 0x010810,   
			    ICLK266M_AHB133M = 0x010a10,
			    
			    ICLK300M_AHB100M = 0x012c20,
			    ICLK360M_AHB120M = 0x016820,//不稳定
			    ICLK399M_AHB133M = 0x018f20,//不稳定可能是供电不足引起的
			    
			   
			    ICLK498M_AHB166M = 0x018f20,
			    
			    ICLK332M_AHB166M = 0x014c10
			    要支持到166
* 参数: 
*		sysclk系统时钟，枚举
    	cfgExtal        外部时钟 EXTAL_CLK 
    	例：hyhwSysClk_Set(ICLK266M_AHB133M,EXTAL_CLK)
    	
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
-----------------------------------------------------------------------------*/
U32 hyhwSysClk_Set(eSYS_Clk_EN sysclk, U32 cfgExtal);

/*----------------------------------------------------------------------------
* 函数:	tca_ckc_setpll()
* 功能:	设置PLL的通用函数
* 参数:	P--PLL 的P值
*		M--PLL 的M值
*		S--PLL 的S值
*		uCH--PLL序号 0 或者1
* 返回:	none
*----------------------------------------------------------------------------*/
void tca_ckc_setpll(U8 P, U8 M, U8 S, U32 uCH);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setPll0
* 功能:	set PLL output frequency 
* 参数:	pllclk	-- PLL 0 输出频率 
				cfgExtal-- 外部时钟
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll0(U32 pllclk,U32 cfgExtal);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setPll1
* 功能:	set PLL output frequency 
* 参数:	pllclk	-- PLL 1 输出频率 
				cfgExtal-- 外部时钟
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll1(ePLL_CLK_EN pllclk, U32 cfgExtal);

/*----------------------------------------------------------------------------
* 函数:	tca_ckc_getsystemsource()
* 功能:	获取时钟源
* 参数:	none
* 返回:	返回时钟选择序数，可据此数值查询对应时钟源
*----------------------------------------------------------------------------*/
S32 tca_ckc_getsystemsource(void);
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUSB11HClk
* 功能:	设置USB 1.1 Host 时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUSB11HClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSdmmcClk
* 功能:	设置SD\MMC 时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdmmcClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setMstickClk
* 功能:	设置Memory stick 时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setMstickClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setI2cClk
* 功能:	设置I2C时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setI2cClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_enableTimer
* 功能:	使能定时器
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_enableTimer(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_disableTimer
* 功能:	关闭定时器
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_disableTimer(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTimer32Clk
* 功能:	设置定时器频率
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTimer32Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTcxClk
* 功能:	设置Tcx时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTcxClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTctClk
* 功能:	设置Tct时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTctClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setLcdClk
* 功能:	设置LCD时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setLcdClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setCamClk
* 功能:	设置CIF PXclock时钟 //camera pixel clock
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setCamClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart0Clk
* 功能:	设置UART0时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart0Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart1Clk
* 功能:	设置UART1时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart1Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart2Clk
* 功能:	设置UART2时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart2Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart3Clk
* 功能:	设置UART3时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart3Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setDaiClk
* 功能:	设置DAI Controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setDaiClk(U32 daiClk);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb0Clk
* 功能:	设置GPSB0时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb0Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb1Clk
* 功能:	设置GPSB1时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb1Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb2Clk
* 功能:	设置GPSB2时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb2Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb3Clk
* 功能:	设置GPSB3时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb3Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setAdcClk
* 功能:	设置Adc时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setAdcClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSpdifClk
* 功能:	设置 S/PDIF Tx Controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSpdifClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setEhi0Clk
* 功能:	设置 EHI 0 controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi0Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setEhi1Clk
* 功能:	设置 EHI 1 controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi1Clk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSdramRefreshClk
* 功能:	设置 SDRAM 刷新时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdramRefreshClk(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSoftReset
* 功能:	软件复位外围设备
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setSoftReset(eMODEL_ID UnitId);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_clearSoftReset
* 功能:	清除外围设备复位
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_clearSoftReset(eMODEL_ID UnitId);

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_busClkEnable
* 功能:	打开外围设备时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkEnable(eMODEL_ID UnitId);
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_busClkDisable
* 功能:	关闭外围设备时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkDisable(eMODEL_ID UnitId);

#endif
