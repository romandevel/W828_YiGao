/******************************************************************************
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

#ifndef HY_HWLCD_H_
#define HY_HWLCD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	LCD_LCDSI_CTRL0_REGISTER 	TCC7901_LCD_LCDSI_CTRL0_REGISTER	/* Control register for LCDSI */
	#define	LCDSI_CTRL0_BIT_IMCPU	~BIT0								/* Input mode ,use cpu lcd, set this bit to 0, converts on-chip cpu signal to 68/80 interaface signal */
	#define LCDSI_CTRL0_BIT_IMRGB	BIT0								/* Input mode ,use rgb lcd, set this bit to 1 */
	#define LCDSI_CTRL0_BIT_OM80	~BIT1								/* Output mode, 0 output 80-system interface signal, 1 output 68-system interface signal */
	#define LCDSI_CTRL0_BIT_OM68	BIT1								/* Output mode, 0 output 80-system interface signal, 1 output 68-system interface signal */

#define	LCD_LCDSI_CTRL1_REGISTER 	TCC7901_LCD_LCDSI_CTRL1_REGISTER    /* Control register for LCSN[0] when LXA=0 (for core access) */
#define	LCD_LCDSI_CTRL2_REGISTER    TCC7901_LCD_LCDSI_CTRL2_REGISTER    /* Control register for LCSN[0] when LXA=1 (for core access) */
#define	LCD_LCDSI_CTRL3_REGISTER    TCC7901_LCD_LCDSI_CTRL3_REGISTER    /* Control register for LCSN[1] when LXA=0 (for core access) */
#define	LCD_LCDSI_CTRL4_REGISTER    TCC7901_LCD_LCDSI_CTRL4_REGISTER    /* Control register for LCSN[1] when LXA=1 (for core access) */
//	#define LCDSI_BITMASK_WIDTH_32		(1<<31)						/* 该位为1，声明 32bit memory mapped I/O devices */
//	#define LCDSI_BITMASK_WIDTH_16		((0x1<<15)|(0x0<<31)) 		/* 声明 16bit memory mapped I/O devices */
//	#define LCDSI_BITMASK_WIDTH_8		((0x0<<15)|(0x0<<31)) 		/* 声明 8bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_32		BIT31 		   				/* 该位为1，声明 32bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_16		BIT15 						/* 声明 16bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_8		~(0x80008000)				/* 声明 8bit memory mapped I/O devices */
	
	#define LCDSI_BITMASK_WSETUP_TIME		(0x7 << 28)		/* 写 操作建立时间 3 bits mask */
	#define LCDSI_BITMASK_WPULSE_WIDTH		(0x1ff << 19)	/* 写操作脉冲宽度  9 bits mask */
	#define	LCDSI_BITMASK_HOLD_TIME			(0x7 << 16)		/* 写操作持续时间  3 bits mask */
	#define LCDSI_BIT_READ_SETUP_TIME		(0x7 << 12)		/* 读 操作建立时间 3 bits mask */ 
	#define LCDSI_BIT_READ_PULSE_WIDTH		(0x1ff << 3)	/* 读操作脉冲宽度  9 bits mask */
	#define	LCDSI_BIT_READ_HOLD_TIME		0x7				/* 读操作持续时间  3 bits mask */


#if 1//RGB屏

	#define LCD_LCDSI_LCSN0													// 根据硬件电路图设备cs 片选信号接到LCSN0 引脚上
	#ifdef LCD_LCDSI_LCSN0	
		#define 	LCD_CMD		*((volatile U16 *)(0xf0000810))             // 写LCD命令
	#define		drvLcd_WriteInstruct(u16Instruct)	(LCD_CMD = u16Instruct)
#elif defined LCD_LCDSI_LCSN1
	#define 	LCD_CMD		*((volatile U16 *)(0xf0000820))
	#define		drvLcd_WriteInstruct(u16Instruct)	(LCD_CMD = u16Instruct)
#endif

#ifdef LCD_LCDSI_LCSN0	
		#define 	LCD_DATA	*((volatile U16 *)(0xf0000818))				// 写LCD 数据
	#define		drvLcd_WriteData(u16Data)			(LCD_DATA = u16Data)
	#define		drvLcd_ReadData(pu16Data)			(*pu16Data = LCD_DATA)
#elif defined LCD_LCDSI_LCSN1
	#define 	LCD_DATA	*((volatile U16 *)(0xf0000828))
	#define		drvLcd_WriteData(u16Data)			(LCD_DATA = u16Data)
	#define		drvLcd_ReadData(pu16Data)			(*pu16Data = LCD_DATA)
#endif	

#ifdef LCD_LCDSI_LCSN0	
	#define		LCD_DATA_ADDRESS		    	(0xf0000818)
#elif defined LCD_LCDSI_LCSN1
	#define		LCD_DATA_ADDRESS		    	(0xf0000828)
#endif	

#endif//RGB屏

typedef enum elcd_inmodel
{
	LCD_CPU = 0,
	LCD_RGB
}eLCD_INMODEL;

typedef enum elcd_outmodel
{
	LCD_MODEL_80= 0,
	LCD_MODEL_68

}eLCD_OUTMODEL; 

typedef enum elcd_buswidth
{
	LCD_BUS_WIDTH_8 = 0,
	LCD_BUS_WIDTH_16,
	LCD_BUS_WIDTH_32

}eLCD_BUSWIDTH;
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_onChipInit
* 功能:	初始化 on chip lcd 参数
* 参数:	inMode0--选择lcd为rgb 还是 cpu
*		outMode--选择输出为68系统接口还是80系统接口
*		busWidth--选择总线宽度8/16/32
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_onChipInit(eLCD_INMODEL inMode, eLCD_OUTMODEL outMode, eLCD_BUSWIDTH busWidth);
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_Set_Writ_SetupTime
* 功能:	set lcd_bus setup time  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritSetupTime(U32 setupTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_Set_Writ_HoldTime
* 功能: set lcd_bus hold time  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritHoldTime(U32 holdTime); 

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_Set_Writ_PulseWidth
* 功能:	set lcd_bus pulse width   
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritPulseWidth(U32 pulseWidth);

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadSetupTime
* 功能:	set lcd_bus setup time  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritReadTime(U32 setupTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadHoldTime
* 功能: set lcd_bus hold time  注意hold_time 时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadHoldTime(U32 holdTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadPulseWidth
* 功能:	set lcd_bus pulse width   时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadPulseWidth(U32 pulseWidth);

#ifdef __cplusplus
}
#endif

#endif //HY_HWLCD_H_