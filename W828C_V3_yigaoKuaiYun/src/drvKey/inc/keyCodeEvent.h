/*************************************************************
%
% Filename     : KeyCodeEvent.h
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
#ifndef _KEY_CODEEVENT_H_ //_KEY_CODEEVENT_H_
#define _KEY_CODEEVENT_H_

#ifdef __cplusplus
extern "C"
{
#endif



// the type of key event: normal, release
// "normal" is used to describe the repeat key event, which is coming 
//          after key is pressed but before key is released.
// "release" is used to describe the release event of key
#define cKEYB_EVENT_NORMAL      0
#define cKEYB_EVENT_RELEASE     1

// the value of key code, now, they are not depending one the hardware
typedef enum _Keyb_Code_e 
{

	Keyb_Code_Record = 0,
	Keyb_Code_Power,
	Keyb_Code_ScanL,
	Keyb_Code_ScanR,
	Keyb_Code_ScanM,
	Keyb_Code_Tab,
	Keyb_Code_0,
	Keyb_Code_1,
	Keyb_Code_2,
	Keyb_Code_3,
	Keyb_Code_4,
	Keyb_Code_5,
	Keyb_Code_6,
	Keyb_Code_7,
	Keyb_Code_8,
	Keyb_Code_9,
	Keyb_Code_Xing,
	Keyb_Code_Jing,
	Keyb_Code_Up,
	Keyb_Code_Down,
	Keyb_Code_Left,
	Keyb_Code_Right,
	Keyb_Code_OK,
	Keyb_Code_Esc,
	Keyb_Code_Del,
	Keyb_Code_F1,
    Keyb_Code_F2,
    
	Keyb_Code_Lock,//ËøÆÁ¼ü
	
// the following declarations are for the long press state of key
	Keyb_Code_RecordLongPressed = 32 + Keyb_Code_Record,
	Keyb_Code_PowerLongPressed,
	Keyb_Code_ScanLLongPressed,
	Keyb_Code_ScanRLongPressed,
	Keyb_Code_ScanMLongPressed,
	Keyb_Code_TabLongPressed,
	Keyb_Code_0LongPressed,
	Keyb_Code_1LongPressed,
	Keyb_Code_2LongPressed,
	Keyb_Code_3LongPressed,
	Keyb_Code_4LongPressed,
	Keyb_Code_5LongPressed,
	Keyb_Code_6LongPressed,
	Keyb_Code_7LongPressed,
	Keyb_Code_8LongPressed,
	Keyb_Code_9LongPressed,
	Keyb_Code_XingLongPressed,
	Keyb_Code_JingLongPressed,
	Keyb_Code_UpLongPressed,
	Keyb_Code_DownLongPressed,
	Keyb_Code_LeftLongPressed,
	Keyb_Code_RightLongPressed,
	Keyb_Code_OKLongPressed,
	Keyb_Code_EscLongPressed,
	Keyb_Code_DelLongPressed,
	Keyb_Code_F1LongPressed,
    Keyb_Code_F2LongPressed,
	
	Keyb_Code_LockLongPressed,//ËøÆÁ¼ü

} Keyb_Code_e, *pKeyb_Code_e;

#ifdef __cplusplus
}
#endif

#endif //_KEY_CODEEVENT_H_

/*============================ History List ================================
  1. Rev 1.00.01.0000, 2004/02/02 13:45             HYCZJ
============================================================================*/
