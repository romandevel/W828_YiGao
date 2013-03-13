
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwLcd.h"
#include "hyhwGpio.h"
#include "hyhwCkc.h"

/*----------------------------------------------------------------------------
* 函数:	void hyhwLcd_GpiopinSet(void)
* 功能:	设置GPIO引脚功能为cpu lcd
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

void hyhwLcd_GpiopinSet(void)
{
	hyhwGpio_setLcdcpuFunction(); 	
}								 

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_onChipInit
* 功能:	初始化 on chip lcd 参数
* 参数:	inMode0--选择lcd为rgb 还是 cpu
*		outMode--选择输出为68系统接口还是80系统接口
*		busWidth--选择总线宽度8/16/32
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_onChipInit(eLCD_INMODEL inMode, eLCD_OUTMODEL outMode, eLCD_BUSWIDTH busWidth)  
{
	hyhwCpm_busClkEnable(CKC_LCD);
	hyhwCpm_setLcdClk();					/* 初始化lcd 时钟 */
	hyhwLcd_GpiopinSet(); 					/* initial lcd port */
	
	//设置输入模式
	if(inMode == LCD_CPU)
	{
		LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	// 设置为cpu lcd 屏
	}
	else
	{
		LCD_LCDSI_CTRL0_REGISTER |= LCDSI_CTRL0_BIT_IMRGB;	// 设置为cpu rgb 屏
	}
	
	//设置数据输出
	if(outMode == LCD_MODEL_80)
	{
		LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_OM80;	// 设置为80 system interface
	}		
	else
	{
		LCD_LCDSI_CTRL0_REGISTER |= LCDSI_CTRL0_BIT_OM68;	// 设置为68 system interface
	}
	
	//设置总线宽度
	if(busWidth == LCD_BUS_WIDTH_8)
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// 设置总线宽度8
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
	}
	else if(busWidth == LCD_BUS_WIDTH_16)
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// 清零相关位
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		
		LCD_LCDSI_CTRL1_REGISTER |= LCDSI_BITMASK_WIDTH_16;	// 设置总线宽度16
		LCD_LCDSI_CTRL2_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		LCD_LCDSI_CTRL3_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		LCD_LCDSI_CTRL4_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		
	}
	else
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// 清零相关位
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		
		LCD_LCDSI_CTRL1_REGISTER |= LCDSI_BITMASK_WIDTH_32;	// 设置总线宽度32
		LCD_LCDSI_CTRL2_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		LCD_LCDSI_CTRL3_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		LCD_LCDSI_CTRL4_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		
	}
}	

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setWritSetupTime
* 功能:	set lcd_bus setup time  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritSetupTime(U32 setupTime)  
{
	U32 lcdClk,clkTime,setupValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* 获得LCD CLK */
	setupValue = (lcdClk /1000) * setupTime ;				/* 如果 clk = 1Mhz ,time = 1us 那么 setupValue =1 */	
	
	if(setupValue > 7)	setupValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (setupValue << 28);
	LCD_LCDSI_CTRL2_REGISTER = (setupValue << 28);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 28);	
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 28);
#endif
}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setWritHoldTime
* 功能: set lcd_bus hold time  注意hold_time 时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritHoldTime(U32 holdTime)  
{
	U32 lcdClk,clkTime,holdValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* 获得LCD CLK */
	holdValue = (lcdClk /1000) * holdTime ;		 			/* 如果 clk = 1Mhz ,time = 1us 那么 holdValue =1 */	
	
	if(holdValue > 7)	holdValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (holdValue << 16);
	LCD_LCDSI_CTRL2_REGISTER = (holdValue << 16);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (holdValue << 16);	
	LCD_LCDSI_CTRL4_REGISTER = (holdValue << 16);
#endif
}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setWritPulseWidth
* 功能:	set lcd_bus pulse width   时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritPulseWidth(U32 pulseWidth)
{
	U32 lcdClk,clkTime,pulseValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                  		/* 获得LCD CLK */
	pulseValue = (lcdClk /1000) * pulseWidth ;		 		/* 如果 clk = 1Mhz ,time = 1ms 那么 pulseValue =1000 */	
	
	if(pulseValue > 0x1ff)	pulseValue = 0x1ff;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (pulseValue << 19);
	LCD_LCDSI_CTRL2_REGISTER = (pulseValue << 19);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (pulseValue << 19);	
	LCD_LCDSI_CTRL4_REGISTER = (pulseValue << 19);
#endif

}



/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadSetupTime
* 功能:	set lcd_bus setup time  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritReadTime(U32 setupTime)  
{
	U32 lcdClk,clkTime,setupValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* 获得LCD CLK */
	setupValue = (lcdClk /1000) * setupTime ;				/* 如果 clk = 1Mhz ,time = 1us 那么 setupValue =1 */	
	
	if(setupValue > 7)	setupValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (setupValue << 12);
	LCD_LCDSI_CTRL2_REGISTER = (setupValue << 12);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 12);	
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 12);
#endif
}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadHoldTime
* 功能: set lcd_bus hold time  注意hold_time 时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadHoldTime(U32 holdTime)  
{
	U32 lcdClk,clkTime,holdValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* 获得LCD CLK */
	holdValue = (lcdClk /1000) * holdTime ;		 			/* 如果 clk = 1Mhz ,time = 1us 那么 holdValue =1 */	
	
	if(holdValue > 7)	holdValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (holdValue << 0);
	LCD_LCDSI_CTRL2_REGISTER = (holdValue << 0);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (holdValue << 0);	
	LCD_LCDSI_CTRL4_REGISTER = (holdValue << 0);
#endif
}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_setReadPulseWidth
* 功能:	set lcd_bus pulse width   时间单位为 us
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadPulseWidth(U32 pulseWidth)
{
	U32 lcdClk,clkTime,pulseValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                  		/* 获得LCD CLK */
	pulseValue = (lcdClk /1000) * pulseWidth ;				/* 如果 clk = 1Mhz ,time = 1ms 那么 pulseValue =1000 */	
	
	if(pulseValue > 0x1ff)	pulseValue = 0x1ff;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (pulseValue << 3);
	LCD_LCDSI_CTRL2_REGISTER = (pulseValue << 3);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (pulseValue << 3);	
	LCD_LCDSI_CTRL4_REGISTER = (pulseValue << 3);
#endif

}


void hyhwLcd_setBusTime( U32  hClk )
{
	
	if( hClk <= 48 )
	{
		//LCD_CONTROL_REGISTER_2 = 0x90099021;
		//LCD_LCDSI_CTRL1_REGISTER = 0xA0099021;
//			LCD_LCDSI_CTRL2_REGISTER = 0x20429021;
		LCD_LCDSI_CTRL2_REGISTER = 0x90099021;
	}
	else if( hClk <= 96 )
	{
		//LCD_CONTROL_REGISTER_2 = 0xa0399021;
		//LCD_LCDSI_CTRL1_REGISTER = 0xB0399021;
		LCD_LCDSI_CTRL2_REGISTER = 0xa0399021;
	}
	else
	{
		//LCD_CONTROL_REGISTER_2 = 0xb04a9021;0xA0089021
		//LCD_LCDSI_CTRL1_REGISTER = 0xC04a9021;
		LCD_LCDSI_CTRL2_REGISTER = 0xb04a9021;
	}	
}

/* end of file */	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		