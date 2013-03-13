/*************************************************************
%
% Filename     : drvKeyb.c
% Project name : part of the lower key driver of GUI part of hyco, manage the input key scanning 
%
% Copyright 2010 hyco,
% hyco, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    	Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040128  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the key process part of the GUI   
% 
****************************************************************/
#include "HSA_API.h"  //ARM basic data type definition
#include "glbStatusDef.h"

#include "drvKeyb.h"
#include "glbVariable.h"
#include "LcdCPU.h"

#define DEFAULT_TIMER0_FREQ       		100
#define KEYB_DURATION_MINIMUM			1//2       // 0.10s
#define KEYB_DURATION_LONG_PRESS		(20)//(DEFAULT_TIMER0_FREQ)//100//jie 20      // 1.00s
#define KEYB_DURATION_REPEAT			(KEYB_DURATION_LONG_PRESS + 2)//jie modfy(KEYB_DURATION_LONG_PRESS + 2) // 0.10s

#define KEYB_BASE_LONGPRESS			0X01
//#define KEYB_BASE_PREVLONGPRESS		0X02
#define KEYB_BASE_LONGPRESS_RELEASE		0X04
#define KEYB_BASE_TOMP3BEGIN			0X08


// BIT PATTERN of static S8   s8InvalidState
#define MAIN_KEYBOARD_INVALID		0x80
#define LINE_KEYBOARD_INVALID		0x40

// Local global data to store the key board status:
static U8 u8KeyTimer;

//static S8 s8DCState = FALSE;
static S8 s8InvalidState;
static S8 s8LastValidKeyIdx;
static U8 u8LastKeyLongPressed;

// main and line control lock status

// main key lock
U8 u8HoldTimer;

static U8 su8KeyInterFlag = 0;
static U16 ss16PosCol=0xffff;
static U16 ss16PosRow=0xffff;
static U8 su8PermitLowPs = 1;

#ifdef SCREEN_LOCK
static U8 su8TouchLock = 0;//0没有锁定 1锁定
#endif

// drvKeybScan.c 中定义
extern const U8	gdrvKeyb_u8KeycodeTable[];

/*------------------------------------------------------------------------
  临时定义
--------------------------------------------------------------------------*/
//extern U8 edit_buf[20];
extern U32 drvKeyb_ScanInitialize( void );
extern S32 drvKeyb_Scan( void );

void drvKey_TouchScreen(void);

/*------------------------------------------------------------------------
   FUNCTION:    drvKeyb_Initialize
   DESCRIPTION: Initialize the keyboard driver
   RETURN:      cSSA_OK, initialization is successful
   NOTES:       normally, no error is reported.
--------------------------------------------------------------------------*/
U32 drvKeyb_Initialize( void )
{
	S32 slKeyBits;
	
	su8KeyInterFlag = 0;
	drvKeyb_ScanInitialize();
	
	s8LastValidKeyIdx = 0;
	slKeyBits = drvKeyb_Scan();
	
	if(slKeyBits!= 0)
	{  // something wrong in keyboard
		s8InvalidState = (S8)MAIN_KEYBOARD_INVALID;
	}
	
	u8KeyTimer = u8LastKeyLongPressed = 0;
	
	return 0;	//successful , 0 means no error
	
} // drvKeyb_Initialize()

int drvKeyb_PermitPowerSave(void)
{
    return su8PermitLowPs; 
}

/*-----------------------------------------------------------------------------
   FUNCTION:    drvKeyb_GetEvent
   DESCRIPTION: return the detected key event
   INPUT:  	pKeyb_Code_e    pkeKeyCode, // O: the keycode
   			pU32            pku32Event       // O: key event, released or press
   RETURN:      cSSA_OK/cSSA_NOT_OK/cSSA_BAD_PARAMETER/cSSA_NOT_INITIALIZED
   NOTES:       normally, no error is reported.
---------------------------------------------------------------------------*/
static Keyb_Code_e	keyCode;
static U32			ku32Event = -1;
U32 drvKeyb_GetEvent( pKeyb_Code_e    pkeKeyCode, U32 * pku32Event )
{
	if (ku32Event != -1)
	{//有效按键
		*pkeKeyCode = keyCode;
		*pku32Event = ku32Event;
		ku32Event = -1;
		return cSSA_OK;
	}
	
	return (cSSA_NOT_OK);
}

void drvKeyb_SetEvent( Keyb_Code_e pkeKeyCode, U32 pku32Event )
{
	keyCode = pkeKeyCode;
	ku32Event = pku32Event;
	su8KeyInterFlag = 1;
	su8PermitLowPs = 1;
}

// 是否有按键按下 0 ：无 1:有
U32 drvKeyb_HaveKey(void)
{
	U32 ret = 0;

	if ( 1 == su8KeyInterFlag )
	{ 
	    su8KeyInterFlag = 0;
	    ret = 1;
	}    
	
	//0 == BackLight_LcdIsReset() && 
	if (ts_get_point_num() > 0) ret =1;

	return ret;
}

void drvKeyb_ClearKey( void )
{
	ku32Event = -1;
	s8InvalidState=0;
	s8LastValidKeyIdx=0;
	u8LastKeyLongPressed=0;
	u8KeyTimer = 0;
}

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_GetPos
* 功能: 得到触摸点的坐标
* 参数: *row：行 *column:列
* 返回: 1:成功取得 0:失败
---------------------------------------------------------------------------*/
U32 drvKeyb_GetPos(S16 *row,S16 *column)
{
	
	if(ss16PosCol == 0xffff || ss16PosRow == 0xffff) return 0;
	else 
	{
		*row =  ss16PosRow;
		*column = ss16PosCol;		
		return 1;
	}
	
	
}

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_ClearPos
* 功能: 清除触摸点避免多次得到坐标点
* 参数: 
* 返回: 
---------------------------------------------------------------------------*/
void drvKeyb_ClearPos(void)
{
	ss16PosCol = 0xffff;
	ss16PosRow = 0xffff;
}

void drvKey_TouchScreen(void)
{
	 //int x,y;
	 ts_wake_collect_position();
	 
	 //ts_get_cur_position(&x,&y);
	 
	 //hzrecog_rec_trace(x,y);   	 
}

//判断是否需要强制关机
static void drvKeyb_long_poweroff(U32 key_bit)
{
#if 1
   	static U32  poweroff_time = 0;

    if (0 != (key_bit&0x01))
    {
        if (++poweroff_time > 120) //6s自动关机
        {
#ifdef HYC_DEV_USE_WATCHDOG
            Led_ctrl(1);
            LcdModulePowerOnOff(0);
            hyhwBatteryPowerOnOff(0);//关闭电源维持
            
            hyhwWatchdog_initMs(1);
            while(1);
#else
            while(1)
            {
                LcdModulePowerOnOff(0);
                hyhwBatteryPowerOnOff(0);
            }   
#endif 
        }
    }
    else
    { 
        poweroff_time = 0;
    }
#endif
} 

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_GetEvent_Timer0ISR
* 功能: irq中断扫描按键
* 参数: none
* 返回: none
---------------------------------------------------------------------------*/
void drvKeyb_GetEvent_Timer0ISR(void)
{
	U32 rc = cSSA_NOT_OK;
	S32 slKeyBits, slIdx, powerkey;
	
    Keyb_Code_e tempKeyCode;
    U32			tempKeyEvent;
	
	
    drvKey_TouchScreen();
	slKeyBits = drvKeyb_Scan();
	
	{
		static U8 keyScanCnt = 0;
		//每DEFAULT_TIMER0_FREQ/KEYB_DURATION_LONG_PRESS个中断扫描一次按键
		//(DEFAULT_TIMER0_FREQ/KEYB_DURATION_LONG_PRESS)=5
		keyScanCnt++;
		//if (keyScanCnt < (DEFAULT_TIMER0_FREQ/KEYB_DURATION_LONG_PRESS))
		if (keyScanCnt < 2)
		{
			return;
		}
		keyScanCnt = 0;
	}
	
	
	powerkey  =  slKeyBits;

	if (slKeyBits == 0)
	{//无按键按下，这时可能是按键后抬起
		if(s8LastValidKeyIdx > 0)
		{//上一次是否有按键按下
			
			//PhoneTrace3(0,"key up %d",u8KeyTimer);
			if(u8KeyTimer >= KEYB_DURATION_MINIMUM)
			{//
				tempKeyCode = (Keyb_Code_e)(gdrvKeyb_u8KeycodeTable[s8LastValidKeyIdx] + u8LastKeyLongPressed);
				tempKeyEvent = cKEYB_EVENT_RELEASE;
				rc = cSSA_OK;
			}
			s8LastValidKeyIdx = 0;
		}
		s8InvalidState = 0; 
	}
	else
	{// some keys are pressed down
		su8PermitLowPs = 0;
		
		if(s8LastValidKeyIdx > 0)
		{// there is last key
			slIdx = (S32)(1 << (s8LastValidKeyIdx - 1));  // get the last key bit pattern from the index
			if((slKeyBits & slIdx) == 0)
			{// current pressed key is not same as last key or key released
				if(u8KeyTimer >= KEYB_DURATION_MINIMUM)
				{// 2 ticks
					tempKeyCode = (Keyb_Code_e)(gdrvKeyb_u8KeycodeTable[s8LastValidKeyIdx] + u8LastKeyLongPressed);
					tempKeyEvent = cKEYB_EVENT_RELEASE;
					rc = cSSA_OK;
				}
				s8LastValidKeyIdx = 0;
			}
			else if(!s8InvalidState)
			{  // now main board in the valid state
				
				if(slKeyBits == slIdx)
				{  // current key is same as last key, count the time
					u8KeyTimer ++;
					if(u8KeyTimer == KEYB_DURATION_LONG_PRESS)
					{// 20 ticks
						u8KeyTimer = KEYB_DURATION_LONG_PRESS-2; //-2为防止长时间产生按键
						u8LastKeyLongPressed = 32;
						tempKeyCode = (Keyb_Code_e)(gdrvKeyb_u8KeycodeTable[s8LastValidKeyIdx] + u8LastKeyLongPressed);
						tempKeyEvent = cKEYB_EVENT_NORMAL;
						rc = cSSA_OK;
					}					
				}
				else
				{  // current key is not same as last key, take as an invalid state
					s8InvalidState = (S8)MAIN_KEYBOARD_INVALID;
				}
			}
		}
		
		//没有前序键，记录当前键
		if(!(s8InvalidState) && s8LastValidKeyIdx == 0)
		{
			for(slIdx = 0; slKeyBits > 0 ; slKeyBits >>= 1)
			{
				slIdx ++;   // get the index of the key
				if(slKeyBits & 1) break;
			}
			if(slKeyBits > 1)
			{
				s8InvalidState = (S8)MAIN_KEYBOARD_INVALID;  // there must be error key bits, like 0x 00101 etc.
 			
 			}
 			else
 			{
				s8LastValidKeyIdx = (S8)slIdx;
				u8LastKeyLongPressed = 0;
				u8KeyTimer = 0;
			}
		}
	}//if(slKeyBits == 0)
	if (rc == cSSA_OK)
	{
		keyCode = tempKeyCode;
		ku32Event = tempKeyEvent;
		su8KeyInterFlag = 1;
		su8PermitLowPs = 1;
	}	
	
	drvKeyb_long_poweroff(powerkey);
		
} // drvKeyb_GetEvent()
/*============================ History List ================================
 1. Rev 1.00.01.0000, 2004/02/02 13:45             HYCZJ
 2. 2004/05/25  del check of Line lock
 3. 2004/05/27  add base machine check, if there is IIC data, set the base machine exist bit
                else  clear it, include battery.h
 4. 20040527  add code, when base machine on, 008 on, and now USB in, 008 can't get v-bus signal
    but instead will get a IIC data to indicate U disk on
 5. 20041025  add code to support pc simulation
 ============================================================================*/
