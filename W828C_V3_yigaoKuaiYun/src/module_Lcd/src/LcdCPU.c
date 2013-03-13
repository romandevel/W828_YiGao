/*************************************************************
%
% Filename     : LcdCPU.c
% Project name : LCD module definition, part of the LCD driver library of Hyctron
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040106  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the processor that control the LCD dirver
%  export function:
%  1.        drvLcd_InitCPU( void )
%  2. void drvLcd_WriteInstruct(unsigned char u8Instruct)
%  3. void drvLcd_WriteData(unsigned char u8Data)
****************************************************************/
//#include "HSA_API.h"

//#include "hyhwGpio.h"
//#include "hyhwEmi.h"

#include "LcdCPU.h"

//extern const struct StaticMemoryControl stMemoryCtrlCfg_Lcd;


/*************************************************************************
 * Function : 	drvLcd_InitCPU
 * Description:Initialize the LCD interface of the CPU. Call this function once
 *			before using any of the LCD driver functions.
 *				 
 * input  : none.
 * return : 0101 case if use memery map , bank0				           
 *************************************************************************/
int drvLcd_InitCPU( void )
{
	hyhwLcd_onChipInit(LCD_CPU,LCD_MODEL_80, LCD_BUS_WIDTH_16);
	//hyhwLcd_lcdcInit(LCD_MODEL_80, LCD_DATA_FORMAT_1);
	return 0;
}

/*----------------------------------------------------------
Version 0.1  2003-11-24		HYCZJ
Version 0.2  2004-01-06		HYCZJ
Version 0.3  2004-05-07		HYCZJ
-----------------------------------------------------------*/



