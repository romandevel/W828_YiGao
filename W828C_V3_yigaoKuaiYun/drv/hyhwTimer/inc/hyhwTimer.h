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

#ifndef HY_HW_TIMER_H_
#define HY_HW_TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	TC32_BASE_FREQ	12000000

/*这个寄存器中断处理程序要用 所以在这定义 */
#define 	TIMER_INT_REQUEST_REGISTER		TCC7901_TIMER_TIREQ_REGISTER 		/* T/C Interrupt Request Register */
	#define TIMER_BIT_INT0					BIT0
	#define TIMER_BIT_INT1					BIT1
	#define TIMER_BIT_INT2					BIT2
	#define TIMER_BIT_INT3					BIT3
	#define TIMER_BIT_INT4					BIT4
	#define TIMER_BIT_INT5					BIT5
	#define WATCHDOG_BIT_INT				BIT6

	#define TIMER_BIT_FLAG0					BIT8
	#define TIMER_BIT_FLAG1					BIT9
	#define TIMER_BIT_FLAG2					BIT10
	#define TIMER_BIT_FLAG3					BIT11
	#define TIMER_BIT_FLAG4					BIT12
	#define TIMER_BIT_FLAG5					BIT13
	#define WATCHDOG_BIT_FLAG				BIT14
		
#define		TIMER_BASEADDR					TCC7901_TIMER_BASEADDR
		
//共5个通用的Timer
#define TIMER_NUM		5
#define PWM_NUM			3	

#define TIMER0			0
#define TIMER1			1
#define TIMER2			2
#define TIMER3			3
#define TIMER4			4
#define TIMER5			5
#define WATCHhDOG		6	
	
/*	Timer 寄存器的定义，每个timer占用4个寄存器  */
typedef volatile struct	_tTIMER_REGISTER
{
	U32 CONFIG;		  		/* Timer/Counter  Configuration Register		*/
	U32 COUNTER;			/* Timer/Counter  Counter Register				*/
	U32 REFERENCE;			/* Timer/Counter  Reference Register			*/
	U32 MID_REFERENCE;		/* Timer/Counter  Middle Reference Register		*/

}tTIMER_REGISTER;

/*---------------------------------------------------------------------------------------
* 宏  :	hyhwmTimer_checkIntFlag
* 功能:	check Timer 中断标志位
* 参数:	无
*---------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT0)
#define hyhwmTimer1_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT1)
#define hyhwmTimer2_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT2)
#define hyhwmTimer3_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT3)
#define hyhwmTimer4_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT4)
#define hyhwmTimer5_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT5)
#define hyhwmWatchDog_checkIntFlag()		(TIMER_INT_REQUEST_REGISTER & WATCHDOG_BIT_INT)

/*----------------------------------------------------------------------------------------
* 宏  :	hyhwmTimer_clearIntFlag
* 功能:	clear Timer 中断标志位
* 参数:	无
*----------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_clearIntFlag()			TIMER_INT_REQUEST_REGISTER  |=(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5) 
#define hyhwmWatchDog_clearIntFlag()		(TIMER_INT_REQUEST_REGISTER |= WATCHDOG_BIT_INT)

/*------------------------------------------------------------------------------------------
* 宏  :	hyhwmTimer_checkCountFlag
* 功能:	check Timer count flag
* 参数:	无
*------------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG0)
#define hyhwmTimer1_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG1)
#define hyhwmTimer2_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG2)
#define hyhwmTimer3_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG3)
#define hyhwmTimer4_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG4)
#define hyhwmTimer5_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG5)
#define hyhwmWatchDog_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & WATCHDOG_BIT_FLAG)

	#define TIMER_BIT_COUNTER_ENABLE		BIT0
	#define TIMER_BIT_CONTINUE_ENABLE		BIT1
	#define TIMER_BIT_PWMMODEL_ENABLE		BIT2
	#define TIMER_BIT_INT_ENABLE	 	 	BIT3
	//#define TIMER_BIT_INT_ENABLE	 	 	BIT3
	#define TIMER_BIT_POLARITY_ENABLE	 	BIT7
	
	
	#define TIMER_BIT_COUNT_CLEAR_ENABLE	BIT8
	#define TIMER_BIT_COUNT_STOP_ENABLE	 	BIT9   								/* If TCNTn is equal to the TREFn, the TCNTn counter stop to increment */

				
#define 	WATCHDOG_CONFIG_REGISTER 		TCC7901_TIMER_TWDCFG_REGISTER 		/* Watchdog Timer Configuration Register 	*/
#define 	WATCHDOG_CLEAR_REGISTER 		TCC7901_TIMER_TWDCLR_REGISTER 		/* Watchdog Timer Clear Register 			*/

#define 	TIMER32_ENABLE_REGISTER 		TCC7901_TIMER_TC32EN_REGISTER 		/* 32-bit Counter Enable / Pre-scale Value  */
	#define RELOAD_CMP1						BIT29
	#define RELOAD_CMP0						BIT28
	#define TC32EN_ENABLE_ON				BIT24
	
#define 	TIMER32_LOAD_REGISTER 			TCC7901_TIMER_TC32LDV_REGISTER 		/* 32-bit Counter Load Value				*/
#define 	TIMER32_MATCH0_REGISTER			TCC7901_TIMER_TC32CMP0_REGISTER 	/* 32-bit Counter Match Value 0 			*/
#define 	TIMER32_MATCH1_REGISTER 		TCC7901_TIMER_TC32CMP1_REGISTER 	/* 32-bit Counter Match Value 1 			*/
#define 	TIMER32_PRESCALE_VALUE_REGISTER	TCC7901_TIMER_TC32PCNT_REGISTER 	/* 32-bit Counter Current Value (pre-scale counter) */
#define 	TIMER32_MAIN_VALUE_REGISTER 	TCC7901_TIMER_TC32MCNT_REGISTER 	/* 32-bit Counter Current Value (main counter) 		*/
#define 	TIMER32_INTCTRL_REGISTER 		TCC7901_TIMER_TC32IRQ_REGISTER 		/* 32-bit Counter Interrupt Control 				*/
	#define	TC32IRQ_RSYNC_DIS				BIT30								// Disable Synchronization control
	#define	TC32IRQ_RSYNC_EN				~BIT30								// Enable Synchronization control
	#define TC32IRQ_SELECT_MASK				(BIT20|BIT19|BIT18|BIT17|BIT16)
	#define TC32IRQ_EN4						BIT20								// Enable Interrupt at the rising edge of a counter bit selected by BITSEL.
	#define TC32IRQ_EN3						BIT19								// Enable Interrupt at the end of pre-scale count
	#define TC32IRQ_EN2						BIT18								// Enable Interrupt at the end of count
	#define TC32IRQ_EN1						BIT17								// Enable Interrupt when the counter value matched with CMP1
	#define TC32IRQ_EN0						BIT16								// Enable Interrupt when the counter value matched with CMP0


#define hyhwmTimer32_clearInt()				(TIMER32_INTCTRL_REGISTER &= ~0x00001f00)

/*-----------------------------------------------------------------------------
* 以下是函数声明部分
*------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_getIntStat
* 功能:	get the timer32 int status
* 参数:	none
* 返回:	irq status
*----------------------------------------------------------------------------*/
U32 hyhwTimer32_getIntStat(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_disableTimer
* 功能:	stop timer 32
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableTimer(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_enableTimer
* 功能:	enable timer
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableTimer(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_enableIrq
* 功能:	enable the timer 32 irq
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableIrq(U32 irqSelect);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_disableIrq
* 功能:	disable the timer 32 irq
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableIrq(U32 irqSelect);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setMatch0
* 功能:	set match 0 value
* 参数:	matchValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch0(U32 matchValue);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setMatch1
* 功能:	set match 1 value
* 参数:	matchValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch1(U32 matchValue);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setLoadValue
* 功能:	set timer 32 load value
* 参数:	LoadValue 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setLoadValue(U32 LoadValue);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setPrescaleValue
* 功能:	set prescale value
* 参数:	preScaleValue
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setPrescaleValue(U32 preScaleValue);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer32_setFrequency
* 功能:	设置TIMER32的中断频率，TIMER32有两个比较器，时钟源为12兆
* 参数:	cmp1Freq、cmp2Freq要小于preScaleFreq，而且需要满足整数倍
		exp:hyhwTimer32_setFrequency(6, 2, 3);
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setFrequency(U32 preScaleFreq, U32 cmp1Freq, U32 cmp0Freq);
	
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_SetFrequency
* 功能:	设置Timer的中断的频率。Timer0 到Timer5 计数器，且溢出后，
*		如果使能了control/status寄存器的中断使能，就会自动加载
*		现在timer0 固定选用的是APB时钟的1/4作为时钟，所以，
*		如果每次改变完APB后，需要重新调用这个函数，以便保证timer0每次中断的时间大
*		致一样
* 参数:	timerUnitID	-- timer 序号，系统中有两个时钟，序号为0~5
*		uwFrequency	-- 唤醒时间中断的频率
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFrequency(U8 timerID, U32 time);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_SetFreeFrequency
* 功能:	设置Timer的中断的频率
* 参数:	timerUnitID	-- timer 序号，系统中有两个时钟，序号为0~5
*		frequency	-- 唤醒时间中断的频率, 是float型，配合mpeg4的帧数/秒
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFreeFrequency (U32 timerUnitID, Float frequency);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setPwm
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel		-- 通道index 
*		Pwm_period	-- 设置Pwm output频率
*		duty		-- Pwm output full duty (high level)
*		PWM输出的范围为是50HZ -- 10KHZ  占空比可调节为1/100
*		超出了PWM输出的范围。输出为 low level
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setPwm(U8 channel,U32 pwm_period, U16 duty);		

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_countEnable
* 功能:	使能Timer的计数
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countEnable(U32 timerID);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_countDisable
* 功能:	disable Timer的计数
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countDisable(U32 timerID);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_intEnable
* 功能:	使能Timer的中断
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intEnable(U32 timerID);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_IntEnable
* 功能:	disable Timer的中断
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intDisable(U32 timerID);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_pwmEnable
* 功能:	使能Timer pwm model
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmEnable(U32 timerID);
	
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_IntEnable
* 功能:	disable Timer model
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmDisable(U32 timerID);
					 						 
/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setReference
* 功能:	设置Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		load	-- 要设置比较器的匹配值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setReference(U32 timerID, U32 load);

/*-----------------------------------------------------------------------------
* 函数:	hyhwTimer_setMidReference
* 功能:	设置Timer的计数值
* 参数:	timerID	-- timer 序号为TIMER0~TIMER5
*		load	-- 要设置比较器的匹配值
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setMidReference(U32 timerID, U32 load);

/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_init
* 功能:	initial watchdog
* 参数:	watchdog_time  以秒为单位，最大不能超过357
*			max--357.9S  min--5.46mS
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_init(U32 watchdog_time);

/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_initMs
* 功能:	initial watchdog
* 参数:	watchdog_timeMs  以毫秒为单位，最大不能超过357913毫秒
*			max--357.9S  min--5.46mS
* 返回:	HY_OK				-- 成功
*		HY_ERR_BAD_PARAMETER-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_initMs(U32 watchdog_timeMs);

/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_enable
* 功能:	enable watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_enable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_disable
* 功能:	disable watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_disable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_intEnable
* 功能:	enable watchdog interrupt
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intEnable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_intDisable
* 功能:	disable watchdog interrupt
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intDisable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwWatchdog_feed
* 功能:	feed watchdog
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_feed(void);

#ifdef __cplusplus
}
#endif

#endif //TM_HWTIMER_H_