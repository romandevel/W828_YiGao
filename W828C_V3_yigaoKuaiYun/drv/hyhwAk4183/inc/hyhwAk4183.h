
#ifndef _HYHW_AK4183_H_
#define _HYHW_AK4183_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"

#if 1  //CAD0 = 0
#define AK4183_WR_Addr			(0x90)
#define AK4183_RD_Addr			(0x91)
#else  //CAD0 = 1
#define AK4183_WR_Addr			(0x92)
#define AK4183_RD_Addr			(0x93)
#endif

#endif

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_sendCmd();
* 功能:	send command to Ak4183
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
//#define SOFT_I2C
U32 hyhwAk4183_sendCmd(unsigned short Cmd);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_readCmd();
* 功能:	read command to Ak4183
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_readCmd(unsigned char *dataBuff,unsigned short num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_Initial(void);
* 功能:	initial Ak4183
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void  hyhwAk4183_Initial(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_PendDetect(void);
* 功能:	detect pend interrupt
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_PenDetect(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_PenDetectHi_z
* 功能:	置ak4183 PENIRQ脚为高阻态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_PenDetectHi_z(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_SleepAndIntOn
* 功能:	置ak4183为节电模式，且允许PENIRQ脚在有触碰时置低
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_SleepAndIntOn(void);

/************************************************************************
 * Function            : hyhwAk4183_TSC_12bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 12bit result
*************************************************************/
U16 hyhwAk4183_TSC_12bitout(unsigned char cmd,unsigned char pd);

/************************************************************************
 * Function            : hyhwAk4183_TSC_8bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 8bit result
*************************************************************/
U8 hyhwAk4183_TSC_8bitout(unsigned char cmd,unsigned char pd);
