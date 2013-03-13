
#ifndef BEEPER_CTRL_H_
#define BEEPER_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwTimer.h"
#include "hyhwGpio.h"


/*
使用timer，用作鸣叫时间控制，使用timer6
*/

typedef struct beep_music_data_st
{
	U16	tone;			/* 音调，这里就是pwm的周期 */
	U16	rhythm;			/* 节奏，以10ms为单位 */
} BEEP_MUSIC_DATA_st;

typedef enum
{
	SCAN_OK = 0,
	SCANNER_POWERON_OK,
	WAIT_TRANS_DATA,
	LOW_POWER,
	LOW_POWER_DOWN,
	SETTING_OK,
	SETTING_ERR,
	KEY_SOUND,
	
	RESEVER1,
	RESEVER2,
	RESEVER3,
	RESEVER4
}BEEP_TYPE_en;


extern const U16 beepTone[];
extern const BEEP_MUSIC_DATA_st* const beepData[];


/*----------------------------------------------------------------------------
* 函数:	beeper_start
* 功能:	开始鸣叫指定旋律
* 输入:	pBeep_data	----- 鸣叫的旋律结构数组
* 输出:	none
*-----------------------------------------------------------------------------*/
void beeper_start(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable);

/*----------------------------------------------------------------------------
* 函数:	beeper_stop
* 功能:	停止鸣叫旋律
* 输入:	none
* 输出:	none
*-----------------------------------------------------------------------------*/
void beeper_stop(void);

/*----------------------------------------------------------------------------
* 函数:	beeper_getState
* 功能:	获取鸣叫状态
* 输入:	none
* 输出:	0	----- 鸣叫已经完成
*		1	----- 正在鸣叫
*-----------------------------------------------------------------------------*/
int beeper_getState(void);

/*----------------------------------------------------------------------------
* 函数:	beeper_ctrl_ISR
* 功能:	鸣叫控制，该函数在timer中断中调用(timer6)
* 输入:	none
* 输出:	none
*-----------------------------------------------------------------------------*/
void beeper_ctrl_ISR(void);


#endif	//#ifndef BEEPER_CTRL_H_