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

/*----------------------------------------------------------------------------
 * extern function 声明
 *----------------------------------------------------------------------------*/
//defined in hyhwCkc.c
extern U32 hyhwCpm_getTimerClk(void);

/*----------------------------------------------------------------------------
 * 以下是函数定义
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_getIntStat
* 功能:	get the timer32 int status
* 参数:	none
* 返回:	irq status
*----------------------------------------------------------------------------*/
U32 hyhwTimer32_getIntStat(void)
{
	return (TIMER32_INTCTRL_REGISTER & 0x0000001f);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_disableTimer
* 功能:	stop timer 32
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableTimer(void)
{
	TIMER32_INTCTRL_REGISTER	&= ~TC32EN_ENABLE_ON;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_enableTimer
* 功能:	enable timer
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableTimer(void)
{
	TIMER32_ENABLE_REGISTER		|= TC32EN_ENABLE_ON;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_enableIrq
* 功能:	enable the timer 32 irq
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableIrq(U32 irqSelect)
{
	TIMER32_INTCTRL_REGISTER |= irqSelect|0x60000000;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_disableIrq
* 功能:	disable the timer 32 irq
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableIrq(U32 irqSelect)
{
	
	TIMER32_INTCTRL_REGISTER &= (~irqSelect);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setMatch0
* 功能:	set match 0 value
* 参数:	matchValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch0(U32 matchValue)
{
	TIMER32_MATCH0_REGISTER  = matchValue;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setMatch1
* 功能:	set match 1 value
* 参数:	matchValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch1(U32 matchValue)
{
	TIMER32_MATCH1_REGISTER  = matchValue;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setLoadValue
* 功能:	set timer 32 load value
* 参数:	LoadValue 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setLoadValue(U32 LoadValue)
{
	TIMER32_LOAD_REGISTER   = LoadValue;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setPrescaleValue
* 功能:	set prescale value
* 参数:	preScaleValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setPrescaleValue(U32 preScaleValue)
{
	TIMER32_ENABLE_REGISTER = preScaleValue - 1;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setFrequency
* 功能:	设置TIMER32的中断频率，TIMER32有两个比较器，时钟源为12兆
* 参数:	cmp1Freq、cmp2Freq要小于preScaleFreq，而且需要满足整数倍
		exp:hyhwTimer32_setFrequency(6, 2, 3);
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setFrequency(U32 preScaleFreq, U32 cmp1Freq, U32 cmp0Freq)
{
	hyhwTimer32_setPrescaleValue(TC32_BASE_FREQ / preScaleFreq);
	hyhwTimer32_setLoadValue(1);
	
	
	cmp1Freq = preScaleFreq / cmp1Freq;
	cmp0Freq = preScaleFreq / cmp0Freq;
	
	hyhwTimer32_setMatch1(cmp1Freq);
	hyhwTimer32_setMatch0(cmp0Freq);
	
	if (cmp1Freq)
	{
		hyhwTimer32_enableIrq(TC32IRQ_EN1);
	}
	else
	{
		hyhwTimer32_disableIrq(TC32IRQ_EN1);
	}
	
	if (cmp0Freq)
	{
		hyhwTimer32_enableIrq(TC32IRQ_EN0);
	}
	else
	{
		hyhwTimer32_disableIrq(TC32IRQ_EN0);
	}
	
	if (cmp1Freq > cmp0Freq)
	{
		TIMER32_ENABLE_REGISTER	&= ~(RELOAD_CMP0|RELOAD_CMP1);
		TIMER32_ENABLE_REGISTER |= RELOAD_CMP1;
	}
	else
	{
		TIMER32_ENABLE_REGISTER &= ~(RELOAD_CMP0|RELOAD_CMP1);
		TIMER32_ENABLE_REGISTER |= RELOAD_CMP0;
	}
	
	hyhwTimer32_enableTimer();
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_SetFrequency
* 功能:	设置timer 时间
* 参数:	timerID		-- 通道index 
		uwFrequency	-- 唤醒时间中断的频率
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFrequency(U8 timerID,U32 uwFrequency)
{
	U32 timerValue,timerClk;
	
	tTIMER_REGISTER	*ptTimerReg;
	
	/* 检查参数 */
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	/* 检查通过，进行设置 */
//	timerClk	= (hyhwCpm_getTimerClk()>> 1);			/* 因为timer 内部分频默认为2分频  example timer = 1Mhz */
	timerClk 	= 750;	
	timerValue	= (timerClk *1000)/uwFrequency ;		/* 因为timer0\timer1\timer2 是16位记数器。如果输入时钟是1M那么最大延时是65.536ms */	
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	
	ptTimerReg->CONFIG	   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE	= timerValue;
	ptTimerReg->CONFIG     |= (TIMER_BIT_INT_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
							   
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_SetFreeFrequency
* 功能:	设置Timer的中断的频率
* 参数:	timerUnitID	-- timer 序号，系统中有两个时钟，序号为0~5
*		frequency	-- 唤醒时间中断的频率, 是float型，配合mpeg4的帧数/秒
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFreeFrequency (U32 timerID, float frequency)
{
	float loadValue;
	U32 timerValue,timerClk;
	tTIMER_REGISTER	*ptTimerReg;
	
	/* 检查参数 */
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		

	//timerClk	= (hyhwCpm_getTimerClk()>> 1);			/* 因为timer 内部分频默认为2分频  example timer = 1Mhz */	  		
	timerClk 	= 750;
	timerValue	= (timerClk *1000)/frequency + 0.5;		
	timerValue	= (U32) loadValue;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + timerID;
	
	ptTimerReg->CONFIG 	   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE  	= timerValue;
	ptTimerReg->CONFIG 	   |= (TIMER_BIT_INT_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
							  
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_readConter
* 功能:	读取指定Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		*conter	-- 带回指定timer的计数值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_readConter( U32 timerID, U32 *conter )
{
	tTIMER_REGISTER   *ptTimerReg ;
	U32 rc = HY_OK;
	
	if(timerID > TIMER_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
		*conter = ptTimerReg->COUNTER;
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setConter
* 功能:	设置指定Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		conter	-- 指定timer的计数值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setConter( U32 timerID, U32 conter )
{
	tTIMER_REGISTER   *ptTimerReg;
	U32 rc = HY_OK;
	
	if(timerID > TIMER_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
		ptTimerReg->COUNTER = conter;
	}
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setPwm
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	pwmID		-- 通道index 
*		Pwm_period	-- 设置Pwm output频率
*		duty		-- Pwm output full duty (high level)
*					   PWM输出的范围为是50HZ -- 10KHZ  占空比可调节为1/100
*					   超出了PWM输出的范围。输出为 low level
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setPwm(U8 pwmID,U32 pwmPeriod, U16 duty)
{
	U32 timerClk,pwmValue,waitValue,holdValue;
	tTIMER_REGISTER	*ptTimerReg;
	
	//参数合法性检查
	if(pwmID > PWM_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//timerClk = (hyhwCpm_getTimerClk()>> 1);  			/*因为timer 内部分频默认为2分频  */       
	timerClk 	= 750;
	pwmValue  = (timerClk * 1000) / pwmPeriod;
	waitValue = pwmValue * duty / 100; 		
	holdValue = pwmValue;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + pwmID;
	
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->CONFIG 		   |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4); 	

	return HY_OK;
}

U32 hyhwTimer_set2Pwm(U8 pwmID1,U8 pwmID2,U32 pwmPeriod, U16 duty)
{
	U32 timerClk,pwmValue,waitValue,holdValue;
	tTIMER_REGISTER	*ptTimerReg;
	U32 configReg1, configReg2;
	U32 *pConfigReg1, *pConfigReg2;
	
	//参数合法性检查
	if(pwmID1>PWM_NUM || pwmID2>PWM_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//timerClk = (hyhwCpm_getTimerClk()>> 1);  			/*因为timer 内部分频默认为2分频  */       
	timerClk 	= 750;
	pwmValue  = (timerClk * 1000) / pwmPeriod;
	waitValue = pwmValue * duty / 100;
	holdValue = pwmValue;
	
	//PWM ID1 config
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + pwmID1;
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->COUNTER			= 0;
	configReg1 = ptTimerReg->CONFIG;
	configReg1 |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
	pConfigReg1 = &ptTimerReg->CONFIG;
	
	//PWM ID2 config
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + pwmID2;
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->COUNTER			= holdValue/2;
	configReg2 = ptTimerReg->CONFIG;
	configReg2 |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
	pConfigReg2 = &ptTimerReg->CONFIG;
	
	//写入配置寄存器
	*pConfigReg1 = configReg1;
	*pConfigReg2 = configReg2;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_countEnable
* 功能:	使能Timer的计数
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/	
U32 hyhwTimer_countEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg 			 = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG	|= TIMER_BIT_COUNTER_ENABLE;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_countDisable
* 功能:	disable Timer的计数
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查	
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~TIMER_BIT_COUNTER_ENABLE;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_intEnable
* 功能:	使能Timer的中断
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG |= TIMER_BIT_INT_ENABLE;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_intDisable
* 功能:	disable Timer的中断
* 参数:	timerID	-- timer序号为0~5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查	
	if(timerID > TIMER_NUM)
	{
		return  HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~TIMER_BIT_INT_ENABLE;


	return	HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_pwmEnable
* 功能:	使能Timer pwm model
* 参数:	timerID	-- timer 序号为TIMER0~TIMER3
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查	
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE);

	return HY_OK;
}	

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_pwmDisable
* 功能:	disable Timer model
* 参数:	timerID	-- timer 序号为TIMER0~TIMER3
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~(TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE);
							
	return HY_OK;
}					 						 
				 						 
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setReference
* 功能:	设置Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		load	-- 要设置比较器的匹配值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setReference(U32 timerID, U32 load)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg				= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->REFERENCE	= load;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setMidReference
* 功能:	设置Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		load	-- 要设置比较器的匹配值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setMidReference(U32 timerID, U32 load)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//参数合法性检查
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg					= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->MID_REFERENCE	= load;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_init
* 功能:	initial watchdog  
* 参数:	watchdog_time  以秒为单位，最大不能超过357
*			max--357.9S  min--5.46mS
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_init(U32 watchdog_time)
{
	/*
		For example, 1 second Watchdog can be used with 183 WDTCNT value (12MHz clock).

		CountValue = WDTCNT * 2^16 = WDTCNT * 65536
		If you use 12MHz clock, 1 clock = 1/12000000 Second.

		1 sec. = 1/12000000 * WDTCNT * 65536
		WDTCNT = 1 sec. * 12000000 / 65536 = (about) 183
		
		max--357.9S  min--5.46mS
	BITCSET(HwWDTCTRL, 0xFFFF, usWDTCNT);
	BITSET(HwWDTCTRL, HwWDTCTRL_WS_1);
	BITSET(HwWDTCTRL, HwWDTCTRL_WE);
	 */
	U32 WDTCntReg;
	if (watchdog_time > 357) watchdog_time = 357;
	WDTCntReg = ((U32)watchdog_time * (1000 * EXTAL_CLK)) >> 16;
	if (WDTCntReg == 0) WDTCntReg = 1;//保证一个最小设置，12M时钟时最小5.2ms
	CKC_WDTCTRL_REGISTER = WDTCntReg | WDOG_ENABLE;
	
	return 	HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_initMs
* 功能:	initial watchdog  
* 参数:	watchdog_timeMs  以毫秒为单位，最大不能超过357913毫秒
*			max--357.9S  min--5.46mS
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_initMs(U32 watchdog_timeMs)
{
	U32 WDTCntReg;
	if (watchdog_timeMs > 357913) watchdog_timeMs = 357913;
	WDTCntReg = ((U32)watchdog_timeMs * EXTAL_CLK) >> 16;
	if (WDTCntReg == 0) WDTCntReg = 1;//保证一个最小设置，12M时钟时最小5.2ms
	CKC_WDTCTRL_REGISTER = WDTCntReg | WDOG_ENABLE;
	
	return 	HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_enable
* 功能:	enable watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_enable(void)
{
	CKC_WDTCTRL_REGISTER |= WDOG_ENABLE; 
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_disable
* 功能:	disable watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_disable(void)
{
	CKC_WDTCTRL_REGISTER &= ~WDOG_ENABLE;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_intEnable
* 功能:	enable watchdog interrupt
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intEnable(void)
{
	//CKC_WDTCTRL_REGISTER |= WTCHDOG_BIT_IEN;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_intDisable
* 功能:	disable watchdog interrupt
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intDisable(void)
{
	//CKC_WDTCTRL_REGISTER &= ~WTCHDOG_BIT_IEN;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_feed
* 功能:	feed watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_feed(void)
{
	CKC_WDTCTRL_REGISTER |= WDOG_CLEAR;	/*设置这个寄存器watchdog counter TIMER_WDCNT_REGISTER 被设置为0*/
	CKC_WDTCTRL_REGISTER &= ~WDOG_CLEAR;
}

/* end of file */