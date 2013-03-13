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

#ifndef _HY_HWRTC_H_
#define _HY_HWRTC_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**
* \defgroup tmhwRtc
* @brief  The timer driver allows user configure rtc timer and alarm interrupt and register interrupt 
*		  handlers for them.
*
* @{
*/

/**
 * @file   
 * @brief hereunder is the sequency to set timer and register timer handler
 *  - call tmhwTimerInit to initialise Timer registers
 *	- call tmhwRtcSetTime to set real time clock timer. It will generate interrupt every second
 *	- call tmhwRtcSetAlarm to set alarm. When alarm register value matchs time counters value, an 
 *	  interrupt is generated.
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
//#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*************************************************************************
 * INCLUDES
 *************************************************************************/
/*************************************************************************
 * DEFINES
 *************************************************************************/
#define		RTC_CONTROL_REGISTER		TCC7901_RTC_RTCCON_REGISTER			/* RTC Control Register */
	#define RTC_BIT_WAKE_UP_EN			BIT7
	#define RTC_BIT_ALARM_INT_EN		BIT6
	#define RTC_BIT_OSC_EN				BIT5
	#define RTC_BIT_CLK_SET				BIT4
	#define RTC_BIT_WRITE_EN			BIT1
	#define RTC_BIT_HALT_EN				BIT0
	
#define		INT_CONTROL_REGISTER		TCC7901_RTC_INTCON_REGISTER			/* RTC Interrupt Control Register */
	#define INT_BIT_ENABLE				BIT0
	
#define		ALARM_CONTROL_REGISTER		TCC7901_RTC_RTCALM_REGISTER			/* RTC Alarm Control Register */
	#define ALARM_BIT_GLOBAL_EN			BIT7
	#define ALARM_BIT_YEAR_EN			BIT6
	#define ALARM_BIT_MONTH_EN			BIT5
	#define ALARM_BIT_WEEK_EN			BIT4
	#define ALARM_BIT_DATE_EN			BIT3
	#define ALARM_BIT_HOUR_EN			BIT2
	#define ALARM_BIT_MINUTE_EN			BIT1
	#define ALARM_BIT_SECOND_EN			BIT0
	
	#define ALARM_ALL_EN				(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7)
	
#define		ALARM_SECOND_REGISTER		TCC7901_RTC_ALMSEC_REGISTER			/* Alarm Second Data Register */
#define		ALARM_MINUTE_REGISTER		TCC7901_RTC_ALMMIN_REGISTER			/* Alarm Minute Data Register */
#define		ALARM_HOUR_REGISTER			TCC7901_RTC_ALMHOUR_REGISTER		/* Alarm Hour Data Register */
#define		ALARM_DATE_REGISTER			TCC7901_RTC_ALMDATE_REGISTER		/* Alarm Date Data Register */
#define		ALARM_WEEK_REGISTER			TCC7901_RTC_ALMDAY_REGISTER			/* Alarm Day of Week Data Register */
#define		ALARM_MONTH_REGISTER		TCC7901_RTC_ALMMON_REGISTER			/* Alarm Month Data Register */
#define		ALARM_YEAR_REGISTER			TCC7901_RTC_ALMYEAR_REGISTER		/* Alarm Year Data Register */
#define		BCD_SECOND_REGISTER			TCC7901_RTC_BCDSEC_REGISTER			/* BCD Second Register */
#define		BCD_MINUTE_REGISTER			TCC7901_RTC_BCDMIN_REGISTER			/* BCD Minute Register1 */
#define		BCD_HOUR_REGISTER			TCC7901_RTC_BCDHOUR_REGISTER		/* BCD Hour Register */
#define		BCD_DATE_REGISTER			TCC7901_RTC_BCDDATE_REGISTER		/* BCD Date Register */
#define		BCD_WEEK_REGISTER			TCC7901_RTC_BCDDAY_REGISTER			/* BCD Day of Week Register */
#define		BCD_MONTH_REGISTER			TCC7901_RTC_BCDMON_REGISTER			/* BCD Month Register */
#define		BCD_YEAR_REGISTER			TCC7901_RTC_BCDYEAR_REGISTER		/* BCD Year Register */
#define		RTC_INT_MODE_REGISTER		TCC7901_RTC_RTCIM_REGISTER			/* RTC Interrupt Mode Register */
	#define INT_BIT_NORMAL				BIT3
	#define INT_BIT_PMWKUP_HIGH			BIT2
	#define INT_BIT_ALARM_DIS			~(BIT0|BIT1)
	#define INT_BIT_ALARM_EN			BIT1
	#define INT_BIT_MODE_EDGE			BIT1
	#define INT_BIT_MODE_LEVEL			(BIT0|BIT1)

#define		RTC_INT_PEND_REGISTER		TCC7901_RTC_RTCPEND_REGISTER		/* RTC Interrupt Pending Register */
	#define INTPEND_BIT_PENDING			BIT0



#define CALENDAR_START_YEAR				1950 
#define CALENDAR_END_YEAR				2085 
#define	CALENDAR_START_YEAR_FIRST_DATE	0    								// 1950年元月一日是星期天

#define CALENDAR_TOTAL_YEARS			136
// 1950 年到2085年

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
                           

/************************************************************************
*	RTC Controller
************************************************************************/
typedef volatile struct _tRtcTime_st
{
	U8	second;		// (0 ~ 59)
	U8	minute;		// (0 ~ 59)
	U8	hour;		// (0 ~ 23)
	U8	week;		// Day of Week (SUN=0, MON, TUE, WED, THR, FRI, SAT=6)

	U8	day;		// (1 ~ 28,29,30,31)
	U8	month;		// (1 ~ 12)
	U16	year;
} RtcTime_st,*pRtcTime_st;


//阴历年月日
typedef struct RtcLunarDate_st
{
	UInt8  day;
	UInt8  month;
	//UInt16 chinese_era;//天干地支(高8位:天干，低8位:地支)
	UInt8  chinese_eraT;//天干
	UInt8  chinese_eraD;//地支
}RtcLunarDate_st, *pRtcLunarDate_st;

//******************************************************************************************
//
//		Definition
//
//******************************************************************************************
#define COMMON_YEAR			0	/* 平年 */
#define LEAP_YEAR			1	/* 闰年 */


//******************************************************************************************
//		ENUM
//******************************************************************************************
typedef enum eNumOfDays
{
	RTC_DATE_SUNDAY = 0,
	RTC_DATE_MONDAY,
	RTC_DATE_TUESDAY,
	RTC_DATE_WEDNESDAY,
	RTC_DATE_THURSDAY,
	RTC_DATE_FRIDAY,
	RTC_DATE_SATURDAY
} eRTC_DATE_DAYS;	

typedef enum eNumOfMonths
{
	RTC_DATE_JANUARY = 1,
	RTC_DATE_FEBRUARY,
	RTC_DATE_MARCH,
	RTC_DATE_APRIL,
	RTC_DATE_MAY,
	RTC_DATE_JUNE,
	RTC_DATE_JULY,
	RTC_DATE_AUGUST,
	RTC_DATE_SEPTEMBER,
	RTC_DATE_OCTOBER,
	RTC_DATE_NOVEMBER,
	RTC_DATE_DECEMBER
} eRTC_DATE_MONTHS;

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getBcdTime
* 功能:	
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_getBcdTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_IsValidTime
* 功能:	
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_IsValidTime(RtcTime_st *ptTimeTest);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_SetTime
* 功能:	set timer
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_SetTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetTime
* 功能:	get timer
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_setBCDTime
* 功能:	set BCD Timer
* 参数:	
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_setBCDTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_SetAlarm
* 功能:	set alarm
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_SetAlarm(RtcTime_st *ptTime);
 
/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_setBcdAlarm
* 功能:	set BCD Timer
* 参数:	
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_setBcdAlarm(RtcTime_st *ptTime, U32 uCon);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetAlarm
* 功能:	
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetAlarm(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getBcdAlarm
* 功能:	
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getBcdAlarm(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* 函数: hyhwRTC_readIntPending
* 功能: read interrupt
* 参数: none
* 返回: 0----没有产生Alarm信号
*		1----有Alarm信号，定时时间到
*----------------------------------------------------------------------------*/
U32 hyhwRTC_readIntPending(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwRTC_clearIntPending
* 功能:	initial rtc
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRTC_clearIntPending(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwRTC_setWakeupOutput
* 功能:	initial rtc
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRTC_setWakeupOutput(void);

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_Init
* 功能:	initial rtc
* 参数:	 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwRtc_Init(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_AlarmIntEnable
* 功能:	使能闹钟功能，使能闹钟中断
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntEnable(void );

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_AlarmIntDisable
* 功能:	禁止闹钟中断
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntDisable(void );

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_EnableAlarm
* 功能:	使能RTC时钟的闹钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_EnableAlarm( void );

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_DisableAlarm
* 功能:	禁止RTC时钟的闹钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_DisableAlarm( void );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_Bcd2Dec
* 功能:	把BCD码换成十进制数转
* 参数:	nBCD	-- 输入的BCD码
* 返回:	十进制数
*----------------------------------------------------------------------------*/
U32  hyhwRtc_Bcd2Dec( U32 nBCD );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_Dec2Bcd
* 功能:	把十进制数转换成BCD码
* 参数:	uDEC	-- 输入的数
* 返回:	BCD码
*----------------------------------------------------------------------------*/
U32 hyhwRtc_Dec2Bcd( U32 uDEC );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getWeekDayOfToday
* 功能:	获得输入的年份、月份、日得到的星期
* 参数:	nThisYear				-- 输入的年份
*		nMonth					-- 输入的月份
*		nDay					-- 输入的日子
* 返回:	计算出的星期
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getWeekDayOfToday( U32 nThisYear, U32 nMonth, U32 nDay );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getTotalDaysUntilTodayInThisYear
* 功能:	获得输入的年份到目前为止总的天数
* 参数:	nThisYear				-- 输入的年份
*		nMonth					-- 输入的月份
* 返回:	输入的年份年到目前为止总的天数
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysUntilTodayInThisYear( U32 nThisYear, U32 nMonth, U32 nDay );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getTotalDaysInMonth
* 功能:	获得输入的月份的天数
* 参数:	nThisYear				-- 输入的年份
*		nMonth					-- 输入的月份
* 返回:	得输入的月份的天数
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysInMonth( U32 nThisYear, U32 nMonth );

/*----------------------------------------------------------------------------
* 函数:	hyhwRtc_getLeapYear
* 功能:	判断是否是闰年
* 参数:	nThisYear		-- 输入的年份
* 返回:	LEAP_YEAR		-- 闰年
*		COMMON_YEAR		-- 平年
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getLeapYear( U32 nThisYear );

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_ConvertToSeconds
* 功能:	将当前的时间，包括年月日星期，时分秒， 转化为秒数
* 参数:	pTime	-- 输入， 时间结构的指针
* 返回:	转化好的秒数
*----------------------------------------------------------------------------*/
U32 hyhwRtc_ConvertToSeconds( RtcTime_st *pTime );

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetCurrentMonthDays
* 功能:	获得指定阳历年月的总天数
* 参数:	solarYear ----阳历年
*		solarMonth ---阳历月
* 返回:	指定年月的总天数
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetCurrentMonthDays(U32 solarYear, U32 solarMonth);

#ifdef __cplusplus
}
#endif

#endif //TM_HWRTC_H_