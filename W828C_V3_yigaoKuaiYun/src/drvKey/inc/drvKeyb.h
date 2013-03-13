/*************************************************************
%
% Filename     : drvKeyb.h
% Project name : part of the lower key driver of GUI part of hyctron, manage the input key scanning 
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
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

#ifndef _DRVKEYB_H_ //-----------------
#define _DRVKEYB_H_

#include "HSA_API.h"
#include "keyCodeEvent.h"

#ifdef __cplusplus
extern "C"
{
#endif

//change the Hold key properity, button / switch key
#define KEYB_HOLD_PUSH_BUTTON	0
#define KEYB_HOLD_SWITCH		1

// BIT PATTERN of static S8   s8LockedState
#define MAIN_KEYBOARD_LOCK    0x80
#define LINE_KEYBOARD_LOCK    0x40

#define KEY_7901_PCB        0x0fff  //该值不要和键值有冲突  

/*-----------------------------------------------------------------------------
* 函数:	drvKeyb_Initialize
* 功能:	键盘驱动初始化
* 参数:	none
* 返回:	初始化成功，返回cSSA_OK
*----------------------------------------------------------------------------*/
U32 drvKeyb_Initialize( void );

/*------------------------------------------------------------------------
* 函数: PowerOnKeyPressed
* 功能: 检测开机键是否按下
* 参数: none
* 返回: 0------开机键按下
*		非零------开机键没有按下
-------------------------------------------------------------------------*/
U32 drvKeybPowerOnKeyPressed(void);

/*-----------------------------------------------------------------------------
* 函数:	drvKeyb_IsLocked
* 功能:	检查键盘是否被锁定
* 参数:	none
* 返回:	TRUE, 键盘已被锁定
	FALSE, 键盘处于正常状态
*----------------------------------------------------------------------------*/
U32 drvKeyb_IsLocked( void );

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_GetEvent
* 功能: 返回检测到的键盘事件
* 参数: pKeyb_Code_e    pkeKeyCode,   // O: 键码
   	pU32            pku32Event    // O: 按键动作， 释放或按下
* 返回: cSSA_OK/cSSA_NOT_OK/cSSA_BAD_PARAMETER/cSSA_NOT_INITIALIZED
---------------------------------------------------------------------------*/
U32 drvKeyb_GetEvent( pKeyb_Code_e   pkeKeyCode, U32 * pku32Event );

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_GetPos
* 功能: 得到触摸点的坐标
* 参数: *row：行 *column:列
* 返回: 1:成功取得 0:失败
---------------------------------------------------------------------------*/
U32 drvKeyb_GetPos(S16 *row,S16 *column);

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_ClearPos
* 功能: 清除触摸点避免多次得到坐标点
* 参数: 
* 返回: 
---------------------------------------------------------------------------*/
void drvKeyb_ClearPos(void);

/*-----------------------------------------------------------------------------
* 函数: drvKeyb_GetEvent_Timer0ISR
* 功能: irq中断扫描按键
* 参数: none
* 返回: none
---------------------------------------------------------------------------*/
void drvKeyb_GetEvent_Timer0ISR(void);

#ifdef __cplusplus
}
#endif

#endif // DRVKEYB_H_H //---------------

/*============================ History List ================================
 1. Rev 1.00.00.0000, 2003/07/14 10:00             Xuefeng Yang
============================================================================*/
