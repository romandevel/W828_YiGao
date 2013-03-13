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

#ifndef __DS28CN01_H
#define __DS28CN01_H


//#include "std.h"
//#include "string.h"
#include "hyhwIIC.h"
#include "hyTypes.h"
#include "hyErrors.h"


#define DS28CN01_I2C_CHANNEL		0x1

#define E2PROM_Secret1 	0x1234567890123456
#define E2PROM_Secret2	0x2345678901234567
#define E2PROM_Secret3	0x3456789012345678
#define Wrong_Secret	0x3434564532408978

#define DS28CN01_SLAVE_ADDR      0xA0  
#define I2C_CLK_DELAY      		10

/* 以下为DS28CN01寄存器的地址*/
#define DS28CN01_MEM_SERIAL					0xA0 /* 芯片的Bit64 的唯一序列号 的rom address*/
#define DS28CN01_MEM_COMMUNICATE_MODE		0xA8 /* 得到通讯模式  1：SM bus  0：IIC*/
#define DS28CN01_MEM_COMMAND				0xA9 /* 写入命令 */
	#define DS28CN01_COMMAND_LOAD_FIRST_SECRET	0xA0	/* 写入64bit的基本密钥*/
	#define ComputeNextSecret  0xB0
	#define ComputeFinalSecret 0xC0
	#define DS28CN01_COMMAND_COMPUTE_PAGE_MAC	0xD0
	#define ComputeAnonymouslyPageMAC 0xE0
#define DS28CN01_MEM_MAC					0xB0				

//define Command register address
#define Bit64RomAddress  0xA0
#define CommandAddress  0xA9
#define MAC_Address 0xB0




//define DS28CN01 Command mode



	//define DS28CN01 Command mode
#define LoadFirstSecret  0xA0
#define ComputeNextSecret  0xB0
#define ComputeFinalSecret 0xC0
#define ComputePageMAC  0xD0
#define ComputeAnonymouslyPageMAC 0xE0

  
typedef enum Key_status
{
 	KeyMatch,
 	KeyUnMatch
}eKEY_STATUS;

typedef enum Secret_type
{
 	SSecret = 0,
 	ESecret1,
 	ESecret2,
 	ESecret3
}eSECRET_TYPE; 

typedef struct tds28cn01_serialNum
{
 	U8 serialNum[8];
 	
}tDS28CN01_SERIAL_NUM;
  

extern tDS28CN01_SERIAL_NUM  ds28cn01SerialNum;  
  


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01Init
* 功能:	 初始化ds2460，并读取序列号
* 参数:	 none
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01Init(void);

/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_reset
* 功能:	reset the ds28cn01
* 参数:	none
* 返回:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_reset( void );

/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_readSerialNumber
* 功能:	读28cn01 唯一的序列号
* 参数:	none	
* 返回:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSerialNumber( void );

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_writeSN
* 功能:	 写入产品序列号
* 参数:	 pbuffer-- SN Str，最大16字节长度(含字符串结束符)
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeSN(U8* pSerialNum);

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_readSN
* 功能:	 读取产品序列号
* 参数:	 pbuffer-- SN Str，至少16字节长度(含字符串结束符)
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSN(U8* pSerialNum);

#endif