/*******************************************************************************
*  (C) Copyright 2009 Shanghai Hyctron Electronic Ltd, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*  说明：telechip TCC7901 芯片的中断控制器驱动函数
*******************************************************************************/

#ifndef HYHW_INTA_CTRL_H_
#define HYHW_INTA_CTRL_H_

#ifdef __cplusplus
extern "C" {	/*  Assume C declarations for C++ */
#endif



#define ISR_TIMER0_NUM	5

typedef void (* isr_function)(void);		//uart1扫描


extern isr_function pisr_timer0[];



/*  由于只有 Audio Subsystem (codec) 的handler可能会变，所以只用一个函数指针即可
	在视频情况下，音频的解码也是通过中断处理，所以也有一个函数指针，当这个函数
	指针不为零时，就进行音频解码 */
extern void (* isr_audio_codec ) ( void );

extern void (* isr_audio_decoder ) ( void );
extern void (* isr_timer1)(void);
extern void (* isr_timer2)(void);
extern void (* isr_timer3)(void);
extern void (* isr_timer5)(void); 
extern void (* isr_key_scan_timer0)(void);//按键扫描

extern void (* isr_gpio)(void);
extern void (* isr_uart0)(void);
extern void (* isr_uart1)(void);
extern void (* isr_uart2)(void);
extern void (* isr_uart3)(void);
extern void (* isr_sdio0)(void);
extern void (* isr_sdio1)(void);
extern void (* isr_usb)(void);

extern void (* isr_camera)(void);
extern void (* isr_enable_sdio)(void);


extern void (* isr_ppp_time)(void);//ppp定时器

extern void (* isr_uartDma)(void);
extern void (* isr_spiDma)(void);

extern void (* isr_spiTDBoot)(void);
extern void (* isr_spiTDPcm)(void);
extern void (* isr_spiGsmPcm)(void);

extern void (* isr_spi)(void);
extern void (* isr_rtc)(void);//rtc中断 alarmclock RTC
extern void (* isr_adc)(void);
extern void (* isr_afs_check)(void);

extern void (* isr_scanner_frame)(void);
extern void (* isr_scanner_data)(void);
	
/*	PIC(programble Interrupt control)中断寄存器 */
#define	INT_ENABLE_REGISTER			TCC7901_PIC_IEN_REGISTER	/*	中断使能寄存器 	R/W, Interrupt Enable Register */			
	/*	通用的位定义，适用于以下的寄存器，具体意义解释如下
	中断使能寄存器(IEN)			bit置1使能中断		bit清0禁止中断 
	中断清除寄存器(CLR)			bit置1清除中断
	中断状态寄存器(STS)			bit置1中断激活
	中断选择寄存器(SEL)			bit置1选择IRQ		bit清0选择FIQ 
	中断源状态寄存器(SRC)		和中断输入和极性寄存器XOR ，表示中断源的状态
	屏蔽状态寄存器(MSTS)		对使能的中断源，表示中断状态，对于禁止的中断，相应位置0 
	测试中断产生寄存器(TST)		对于SRC寄存器中置1的中断源，将此寄存器相应位写1，将产生中断
	中断极性寄存器(POL)			bit置1表示低有效，bit清0表示高有效
	IRQ原始状态寄存器(IRQ)
	FIQ原始状态寄存器(FIQ)
	屏蔽IRQ状态寄存器(MIRQ) 	IRQ寄存器的屏蔽状态
	屏蔽FIQ状态寄存器(MFIQ) 	FIQ寄存器的屏蔽状态
	中断触发模式寄存器(MODE)	bit置1表示选择电平触发，bit清0表示选择edge触发
	同步使能寄存器(SYNC)		bit置1表示中断源和bus时钟HCLK同步
	唤醒使能寄存器(WKEN) 		bit清0表示CPU clock或 bus clock可被相应的中断唤醒
	双边或单边寄存器(MODEA)		当在edge触发模式下，bit清0表示中断在single edge时传至FIQ或IRQ，bit置1表示
								both edge 时传至IRQ或FIQ
	中断输出屏蔽寄存器(INTMSK)	bit置1表示相应的中断转发至IRQ或FIQ	*/						
	#define INT_BIT_SELECT4		BIT31	/* refer to IRQSEL3 				*/
	#define	INT_BIT_ADC			BIT30	/* ADC interrupt enable				*/		
	#define	INT_BIT_LCD			BIT29   /* LCD controller interrupt enable 	*/		
	#define	INT_BIT_CAMERA		BIT28   /* Camera interrupt enable 			*/		
	#define	INT_BIT_SD		    BIT27   /* SD/MMC interrupt enable 			*/		
	#define	INT_BIT_NANDFLASH   BIT26   /* Nandflash controller Interrupt enable*/		
	#define	INT_BIT_SELECT3    	BIT25   /* refer to IRQSEL3  				*/		
	#define	INT_BIT_HDD	  	    BIT24   /* HDD contrller Interrupt enable	*/	
	#define	INT_BIT_DMA		    BIT23   /* DMA contrller interrupt enable 	*/		
	#define	INT_BIT_USB_HOST	BIT22   /* USB 1.1 host interrupt enable 	*/	
	#define	INT_BIT_USB_DEVICE	BIT21   /* USB 2.0 device interrupt enable 	*/		
	#define	INT_BIT_GRAPHIC_2D	BIT20   /* Graphic 2D Interrupt enable, 注意 POL寄存器相应位应设为1 */		
	#define	INT_BIT_SELECT2		BIT19   /* refer to IRQSEL2  				*/		
	#define	INT_BIT_UART		BIT18   /* UART interrupt enable 			*/		
	#define	INT_BIT_HPI			BIT17   /* EHI Channel 0 interrupt enable 	*/		
	#define	INT_BIT_CDRX		BIT16   /* CDIF receive interrupt enable	*/		
	#define	INT_BIT_DAITX		BIT15   /* DAI transmit interrupt enable 	*/		
	#define	INT_BIT_DAIRX		BIT14   /* DAI receive interrupt enable 	*/		
	#define	INT_BIT_I2C			BIT13   /* I2C interrupt enable				*/		
	#define	INT_BIT_SC		    BIT12   /* Mem-to Mem scalerinterrupt enable*/		
	#define	INT_BIT_SELECT1	    BIT11   /* refer to IRQSEL1 				*/		
	#define	INT_BIT_SELECT0	    BIT10   /* refer to IRQSEL0 				*/		
	#define	INT_BIT_SPD_TX	    BIT9    /* SPDIF Transmitter interrupt enable*/		
	#define	INT_BIT_SCORE	   	BIT8    /* Sub Processor Interrupt enable	*/		
	#define	INT_BIT_TIMER1		BIT7    /* Timer 1 interrupt enable		*/		
	#define	INT_BIT_TIMER0		BIT6    /* Timer 0 interrupt enable		*/		
	#define	INT_BIT_GPSB		BIT5    /* GPSB interrupt enable 		*/		
	#define	INT_BIT_RTC			BIT4	/* RTC interrupt enable 		*/		
	#define	INT_BIT_EI3			BIT3	/* External Interrupt 3 enable	*/ 											
	#define	INT_BIT_EI2			BIT2	/* External Interrupt 2 enable	*/ 											
	#define	INT_BIT_EI1			BIT1	/* External Interrupt 1enable	*/ 		
	#define	INT_BIT_EI0			BIT0	/* External Interrupt 0 enable	*/ 							
#define	INT_CLEAR_REGISTER			TCC7901_PIC_CLR_REGISTER	/*	中断清除寄存器  	R/W, Interrupt Clear Register*/
#define	INT_STATUS_REGISTER			TCC7901_PIC_STS_REGISTER	/*	中断状态寄存器  	R, Interrupt Status Register */
#define	INT_IRQFIQ_SELECT_REGISTER	TCC7901_PIC_SEL_REGISTER	/*	IRQ/FIQ选择寄存器	R/W, IRQ/FIQ Selection Register */
#define	INT_SOURCE_REGISTER			TCC7901_PIC_SRC_REGISTER	/*	源中断状态寄存器	R, Source Interrupt Status Register */
#define	INT_MASKED_STATUS_REGISTER	TCC7901_PIC_MSTS_REGISTER	/*	屏蔽状态寄存器 		R, Masked Status Register */		
#define	INT_TEST_REGISTER			TCC7901_PIC_TIG_REGISTER	/*	测试中断产生寄存器	R/W, Test Interrupt Generation Register*/		
#define	INT_POLARITY_REGISTER		TCC7901_PIC_POL_REGISTER	/*	中断极性寄存器		R/W, Interrupt Polarity Register*/	 
#define	INT_RAWIRQ_REGISTER			TCC7901_PIC_IRQ_REGISTER	/*	IRQ原始状态寄存器	R, IRQ Raw Status Register*/	
#define	INT_RAWFIQ_REGISTER			TCC7901_PIC_FIQ_REGISTER	/*	FIQ原始状态寄存器  	R, FIQ Status Register*/	
#define	INT_MASKIRQ_REGISTER		TCC7901_PIC_MIRQ_REGISTER	/*	屏蔽IRQ状态寄存器	R, Masked IRQ Status Register*/	 
#define	INT_MASKFIQ_REGISTER		TCC7901_PIC_MFIQ_REGISTER	/*	屏蔽FIQ状态寄存器	R, Masked FIQ Status Register*/	 
#define	INT_TRIGGER_MODE_REGISTER	TCC7901_PIC_MODE_REGISTER	/*	中断触发模式寄存器	R/W, Trigger Mode Register 0:edge, 1:level */	    
#define	INT_SYNC_REGISTER			TCC7901_PIC_SYNC_REGISTER	/*	同步使能寄存器  	R/W, Synchronizer Enable Register  */  
#define	INT_WAKEUP_REGISTER			TCC7901_PIC_WKEN_REGISTER	/*	唤醒使能寄存器  	R/W, Wakeup Enable Register  */	
#define	INT_MODEA_REGISTER			TCC7901_PIC_MODEA_REGISTER	/*	双边或单边寄存器	R/W, Both Edge or Single Edge Register */	  
#define	INT_INTMASK_REGISTER		TCC7901_PIC_INTMSK_REGISTER	/*	中断输出屏蔽寄存器	R/W, Interrupt Output Masking Register */
#define	INT_ALLMASK_REGISTER		TCC7901_PIC_ALLMSK_REGISTER	/*	中断全屏蔽寄存器	R/W, All Mask Register */
	#define INT_BIT_ALLMASK_FIQ		BIT1	/* 清0表示 IRQ中断被屏蔽	*/
	#define	INT_BIT_ALLMASK_IRQ		BIT0	/* 清0表示 FIQ中断被屏蔽	*/		
	

/* VIC（Vectored Interrupt Controller） 寄存器*/									
#define	INT_IRQ_VECTOR_REGISTER		TCC7901_VIC_VAIRQ_REGISTER/*	IRQ向量寄存器		R, IRQ Vector Register */		
	/* 4个向量及向量数目寄存器通用bit定义，*/
	#define INT_BIT_VECTOR_INV			BIT31	/* 1/0 - invalid/valid for current Interrupt Source */
	#define	INT_BITMASK_VECTOR_VAVN		(BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)		/* interrupt Vector Address Offset in word-addrss, 例如 0、4、8	*/		
#define	INT_FIQ_VECTOR_REGISTER		TCC7901_VIC_VAFIQ_REGISTER/*	FIQ向量寄存器		R, FIQ Vector Register*/	 
#define	INT_IRQ_VECTOR_NUM_REGISTER	TCC7901_VIC_VNIRQ_REGISTER/*	IRQ向量个数寄存器	R, IRQ Vector Number Register */	  
#define	INT_FIQ_VECTOR_NUM_REGISTER	TCC7901_VIC_VNFIQ_REGISTER/*	FIQ向量个数寄存器	R, FIQ Vector Number Register */	  

#define	INT_VECTOR_CONTROL_REGISTER	TCC7901_VIC_VCTRL_REGISTER/*	向量控制寄存器		R/W, Vector control Register */  
	#define INT_BIT_VEC_CTRL_RCL			BIT31	
	#define INT_BIT_VEC_CTRL_FPOL			BIT30	
	#define INT_BIT_VEC_CTRL_FFLG			BIT29	
	#define INT_BIT_VEC_CTRL_IFLG			BIT28	
	#define INT_BIT_VEC_CTRL_FHD			BIT27	
	#define INT_BIT_VEC_CTRL_IHD			BIT26	

#define	INT_PRIORITY0_3_REGISTER	TCC7901_VIC_PRI04_REGISTER/*	中断0-3的优先级  	R/W, Priorityes for Interrupt 0~3*/	 
	/* 8个优先级寄存器的通用位定义 */
	#define	INT_BITMASK_PRIORITY_PRI3		(BIT28|BIT27|BIT26|BIT25|BIT24)				
	#define	INT_BITMASK_PRIORITY_PRI2		(BIT20|BIT19|BIT18|BIT17|BIT16)				
	#define	INT_BITMASK_PRIORITY_PRI1		(BIT12|BIT11|BIT10|BIT9 |BIT8 )				
	#define	INT_BITMASK_PRIORITY_PRI0		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0)				
#define	INT_PRIORITY4_7_REGISTER	TCC7901_VIC_PRI08_REGISTER/*	中断4-7的优先级  	R/W, Priorityes for Interrupt 4~7*/	 
#define	INT_PRIORITY8_11_REGISTER	TCC7901_VIC_PRI12_REGISTER/*	中断8-11的优先级  	R/W, Priorityes for Interrupt 8~11*/	  
#define	INT_PRIORITY12_15_REGISTER	TCC7901_VIC_PRI16_REGISTER/*	中断12-15的优先级  	R/W, Priorityes for Interrupt 12~15*/	  
#define	INT_PRIORITY16_19_REGISTER	TCC7901_VIC_PRI20_REGISTER/*	中断16-19的优先级  	R/W, Priorityes for Interrupt 16~19*/	  
#define	INT_PRIORITY20_23_REGISTER	TCC7901_VIC_PRI24_REGISTER/*	中断20-23的优先级  	R/W, Priorityes for Interrupt 20~23*/	  
#define	INT_PRIORITY24_27_REGISTER	TCC7901_VIC_PRI28_REGISTER/*	中断24-27的优先级  	R/W, Priorityes for Interrupt 24~27*/	   
#define	INT_PRIORITY28_31_REGISTER	TCC7901_VIC_PRI32_REGISTER/*	中断28-31的优先级  	R/W, Priorityes for Interrupt 28~31*/	   


/* PORT的、和中断相关的寄存器 */
#define	INT_EXTERNEL_SELECT0_REGISTER	TCC7901_PORT_EINTSEL0_REGISTER	/*	外部中断选择寄存器0，负责外部中断1-3的源的选择	R, External Interrupt select0 Register */
	#define	INT_BITMASK_EXT_INT0		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0 )		
	#define	INT_BITMASK_EXT_INT1		(BIT12|BIT11|BIT10|BIT9 |BIT8 )		
	#define	INT_BITMASK_EXT_INT2		(BIT20|BIT19|BIT18|BIT17|BIT16)		
	#define	INT_BITMASK_EXT_INT3		(BIT28|BIT27|BIT26|BIT25|BIT24)		
#define	INT_EXTERNEL_SELECT1_REGISTER	TCC7901_PORT_EINTSEL1_REGISTER	/*	外部中断选择寄存器1，负责外部中断4-7的源的选择	R, External Interrupt select0 Register */
	#define	INT_BITMASK_EXT_INT4		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0 )		
	#define	INT_BITMASK_EXT_INT5		(BIT12|BIT11|BIT10|BIT9 |BIT8 )		
	#define	INT_BITMASK_EXT_INT6		(BIT20|BIT19|BIT18|BIT17|BIT16)		
	#define	INT_BITMASK_EXT_INT7		(BIT28|BIT27|BIT26|BIT25|BIT24)		
	
	/* 外部中断的具体的值,即设到2个外部选择中断寄存器中的相对于0-7个外部选择位置的具体的值 */
	#define	INT_EXT_SELECT_USBVB_OFF		31	/*external int号 为偶数，int号为0-7*/		
	#define	INT_EXT_SELECT_USBVB_ON			31	/*external int号 为奇数，int号为0-7*/	
	#define	INT_EXT_SELECT_PMKUP			30		
	#define	INT_EXT_SELECT_LPD23			29		
	#define	INT_EXT_SELECT_LPD18			28		
	#define	INT_EXT_SELECT_GPIOC30			27		
	#define	INT_EXT_SELECT_GPIOC27			26		
	#define	INT_EXT_SELECT_GPIOC26			25		
	#define	INT_EXT_SELECT_HPCSN_L			24		
	#define	INT_EXT_SELECT_HPRDN			23		
	#define	INT_EXT_SELECT_HPXD11			22		
	#define	INT_EXT_SELECT_HPXD3			21		
	#define	INT_EXT_SELECT_SCMD0			20	
	#define	INT_EXT_SELECT_SDO0				19	
	#define	INT_EXT_SELECT_GPIOA2			18		
	#define	INT_EXT_SELECT_GPIOA3			17		
	#define	INT_EXT_SELECT_GPIOA5			16		
	#define	INT_EXT_SELECT_GPIOB0			15		
	#define	INT_EXT_SELECT_GPIOB3			14		
	#define	INT_EXT_SELECT_GPIOB4			13		
	#define	INT_EXT_SELECT_GPIOB13			12		
	#define	INT_EXT_SELECT_GPIOB14			11		
	#define	INT_EXT_SELECT_GPIOB15			10		
	#define	INT_EXT_SELECT_SCMD1			9	
	#define	INT_EXT_SELECT_SDO1				8	
	#define	INT_EXT_SELECT_GPIOA6			7		
	#define	INT_EXT_SELECT_GPIOA7			6		
	#define	INT_EXT_SELECT_GPIOA11			5		
	#define	INT_EXT_SELECT_AIN0				4		
	#define	INT_EXT_SELECT_AIN1				3		
	#define	INT_EXT_SELECT_AIN7				2		
	#define	INT_EXT_SELECT_UTXD2			1		
	#define	INT_EXT_SELECT_URXD3			0		
	
#define	INT_IRQ_SELECT_REGISTER			TCC7901_PORT_IRQSEL_REGISTER	/*	IRQ选择寄存器，R/W, External Interrupt select0 Register */
	#define	INT_BITMASK_IRQSEL_0		(BIT1|BIT0)		
		/* IRQ SELECT0 具体的值 */	
		#define	INT_IRQSEL0_VIDEO_CORE		0		
		#define	INT_IRQSEL0_EHICH0_CTRL		1		
		#define	INT_IRQSEL0_EI4				2		
	#define	INT_BITMASK_IRQSEL_1		(BIT3|BIT2)	
		/* IRQ SELECT1 具体的值 */	
		#define	INT_IRQSEL1_GPSB			0		
		#define	INT_IRQSEL1_EHICH1_CTRL		1		
		#define	INT_IRQSEL1_EI5				2		
	#define	INT_BITMASK_IRQSEL_2		(BIT5|BIT4)	
		/* IRQ SELECT2 具体的值 */	
		#define	INT_IRQSEL2_UART			0		
		#define	INT_IRQSEL2_EHICH1_INTERNAL	1		
	#define	INT_BITMASK_IRQSEL_3		(BIT7|BIT6)		
		/* IRQ SELECT3 具体的值 */	
		#define	INT_IRQSEL3_MEM_STICK		0		
		#define	INT_IRQSEL3_EI6				1		
	#define	INT_BITMASK_IRQSEL_4		(BIT9|BIT8)	
		/* IRQ SELECT4 具体的值 */	
		#define	INT_IRQSEL4_GPSB			0		
		#define	INT_IRQSEL4_ECC				1		
		#define	INT_IRQSEL4_EI7				2		
		#define	INT_IRQSEL4_USB20_DMA		3		

/*-----------------------------------------------------------------------------------*/

/*  由于只有 Audio Subsystem (codec) 的handler可能会变，所以只用一个函数指针即可
	在视频情况下，音频的解码也是通过中断处理，所以也有一个函数指针，当这个函数
	指针不为零时，就进行音频解码 */
extern void (* isr_audio_codec ) ( void );

extern void (* isr_audio_decoder ) ( void );
extern void (* isr_timer0)(void);
extern void (* isr_timer1)(void);
extern void (* isr_timer2)(void);
extern void (* isr_key_scan_timer0)(void);//按键扫描

extern void (* isr_gpio)(void);
extern void (* isr_uart0)(void);
extern void (* isr_uart1)(void);
extern void (* isr_uart2)(void);
extern void (* isr_uart3)(void);
extern void (* isr_sdio0)(void);
extern void (* isr_sdio1)(void);


extern void (* isr_spi)(void);
extern void (* isr_camera)(void);
extern void (* isr_usb)(void);
extern void (* isr_usbHost)(void);//usb host stack 20100427
//extern void (* isr_lcdDmaShow)(void);//RGB lcd Show dma intp

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_init
* 功能:	初始化，禁止所用中断
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_init ( void );

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_enable
* 功能:	使能的指定源的中断
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_enable(intSourceMask) 	INT_ENABLE_REGISTER |= intSourceMask;

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_disable
* 功能:	禁止指定源的中断
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_disable(intSourceMask)	INT_ENABLE_REGISTER &= ~(intSourceMask);

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_clear
* 功能:	清除指定源的中断标记
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_clear(intSourceMask)	INT_CLEAR_REGISTER |= intSourceMask;

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_getStatus
* 功能:	读出指定源的中断的请求
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_getStatus(intSourceMask)	(INT_STATUS_REGISTER & intSourceMask);


/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_setIRQ
*		hyhwInt_setFIQ
* 功能:	设置指定源的中断的为IRQ/FIQ类型
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_setIRQ(intSourceMask)	INT_IRQFIQ_SELECT_REGISTER |= intSourceMask;
#define	hyhwInt_setFIQ(intSourceMask)	INT_IRQFIQ_SELECT_REGISTER &= ~(intSourceMask);

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_getMaskedRequest
* 功能:	读出指定源的中断的请求
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwIntA_getMaskedStatus(intSourceMask)		(INT_MASKED_STATUS_REGISTER & intSourceMask)

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_setTriggerLevel
*		hyhwInt_setTriggerEdge
* 功能:	设置指定源的中断的触发类型为edge/level类型
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_setTriggerLevel(intSourceMask)	INT_TRIGGER_MODE_REGISTER |= intSourceMask
#define	hyhwInt_setTriggerEdge(intSourceMask)	INT_TRIGGER_MODE_REGISTER &= ~(intSourceMask)

/*-----------------------------------------------------------------------------
* 宏  :	hyhwInt_setActiveLow
*		hyhwInt_setActiveHigh
* 功能:	设置A组/B组的指定源的中断的有效类型为low/high类型，默认为active high
* 参数:	intSourceMask--中断源的掩码,可以是多个中断的或值
*----------------------------------------------------------------------------*/
#define	hyhwInt_setActiveLow(intSourceMask)		INT_POLARITY_REGISTER |= intSourceMask
#define	hyhwInt_setActiveHigh(intSourceMask)	INT_POLARITY_REGISTER &= ~(intSourceMask)

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigCodecISR
* 功能:	由于现在Audio Subsubsystem 在播放、录音情况下有不同的ISR函数，并且，播放
*		情况不同，ISR也会有不同，所以，需要设置AudioSubsytem的ISR函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCodecISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigAudioDecISR
* 功能:	播放解码器的中断函数
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioDecISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer1_ISR
* 功能:	设置 timer1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigKeyScanTimer0_ISR
* 功能:	设置 KeyScanTimer0 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigKeyScanTimer0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigGpio_ISR
* 功能:	设置 Gpio 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigGpio_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart0_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart1_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart2_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart2_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart_ISR(U8 id,  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSdio0_ISR
* 功能:	设置 Sdio0 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSdio1_ISR
* 功能:	设置 Sdio1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigCamera_ISR
* 功能:	设置 Sdio1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCamera_ISR(  void (*pIntVector) (void));

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* HYHW_INTA_CTRL_H_ */
