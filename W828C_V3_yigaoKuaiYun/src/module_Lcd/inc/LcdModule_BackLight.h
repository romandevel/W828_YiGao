#ifndef _LCDMODULE_BACKLIGHT_H_
#define _LCDMODULE_BACKLIGHT_H_

#include "HycDeviceConfig.h"


#ifdef ACT6311
	#define LCD_PWM_GRADE_1	   68
	#define LCD_PWM_GRADE_2	   65
	#define LCD_PWM_GRADE_3	   60
	#define LCD_PWM_GRADE_4	   50
	#define LCD_PWM_GRADE_5	   40 
#endif

#ifdef CP2142MM
	#define LCD_PWM_GRADE_1	   12//15
	#define LCD_PWM_GRADE_2	   8
	#define LCD_PWM_GRADE_3	   5
	#define LCD_PWM_GRADE_4	   3
	#define LCD_PWM_GRADE_5	   2 
#endif 
	
extern S32  gu32ScreenPro;
//lcd PWM背光控制级数
extern const U8 gu8Lcd_BackLight[5];

/*---------------------------------------------------------------------------
 函数：BackLight_EventTurnOnLcd
 功能：由于处于屏保状态下，来了事件需要点亮背光 如:来电话了，按键按下等
 参数：无
 返回：1：点亮 0：维持
* -----------------------------------------------------------------------------*/ 
U32  BackLight_EventTurnOnLcd(void *pView);

/*---------------------------------------------------------------------------
 函数：BackLight_UpdataLightTime
 功能：更新背光时间
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void BackLight_UpdataLightTime(U32 t);

/*---------------------------------------------------------------------------
 函数：BackLight_CalculateTime
 功能：对背光计时,计时为0时则关闭背光
 参数：无
 返回：none
* -----------------------------------------------------------------------------*/ 
void  BackLight_CalculateTime(void);

/*---------------------------------------------------------------------------
 函数：BackLight_AvoidThis
 功能：在当前模式下屏蔽此项功能
 参数：无
 返回：1:屏蔽 0:不屏蔽
* -----------------------------------------------------------------------------*/ 
U32 BackLight_AvoidThis(void);

#endif