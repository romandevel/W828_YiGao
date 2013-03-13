/*************************************************************
%
% Filename     : LcdCPU.h
% Project name : part of the LCD driver library of Hyctron
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
%  This source file contains the control part of the CPU which control the LCD  
%
****************************************************************/
#ifndef _LCDCPU_H_ 
#define _LCDCPU_H_

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwLcd.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
#define 	LCD_CMD	                            *((volatile U16 *)(0x04000000))
#define		drvLcd_WriteInstruct(u16Instruct)	(LCD_CMD = u16Instruct)

#define 	LCD_DATA                            *((volatile U16 *)(0x04040000))
#define		drvLcd_WriteData(u16Data)			(LCD_DATA = u16Data)
#define		drvLcd_ReadData(pu16Data)			(*pu16Data = LCD_DATA)

#define		LCD_DATA_ADDRESS		    		(0x04040000)
#endif

/*************************************************************************
 * Function            : drvLcd_InitCPU
 * Description         : Initialize the LCD interface of 7750. Call this function once
 *						 before using any of the LCD driver functions.
 *						 After drvLcd_Initialise, LCD display is ON & cleared.
 * input parameters    : lcdcontrol: the control bits to configure the LCD
 *						  interface into various modes of parallel/serial,
 *						  8080/6800, 8bit/4bit, wait states, busy check,
 *						  chip select, E_RD output pin, MSB_FIRST, loopback.
 *						  See the SAA7750 manual for more information.
 * return parameter    : Returns value zero if all data successfully written
 *						 to FIFO, otherwise, returns value one.
 *
 *************************************************************************/
int drvLcd_InitCPU( void );


#ifdef __cplusplus
}
#endif

#endif // _LCDCPU_H 

/*============================ History List ================================
 1. Rev 0.1					 20040107             HYCZJ
============================================================================*/
