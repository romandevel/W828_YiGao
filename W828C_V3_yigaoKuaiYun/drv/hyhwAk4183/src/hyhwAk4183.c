/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/** 
 * @file hyhw2Wire.c
 * @brief The source code for the 2 Wire driver , simulated by GPIO. 
 *
 */
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*----------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "hyhwAk4183.h"
#include "hyhwIIC.h"
#include "hyhwDai.h"
#include "hyhwGpio.h"
#include "hardWare_Resource.h"


#define SLEEP_MODE			          		0x07 	/* 	sleep mode 		*/
#define ACTIVATE_NX_DRIVERS          		0x08 	/* 	Activate X– Drivers  		*/
#define ACTIVATE_NY_DRIVERS             	0x09 	/* 	Activate Y– Drivers    		*/
#define ACTIVATE_YNX_DRIVERS               	0x0A 	/* 	Activate Y+, X– Drivers     */
#define MEAS_XPOS                			0x0c 	/* 	Measure X Position          */
#define MEAS_YPOS            				0x0d 	/* 	Measure Y Position     		*/
#define MEAS_Z1POS             				0x0e	/* 	Measure Z1 Position      	*/
#define MEAS_Z2POS             				0x0f	/* 	Measure Z2 Position    		*/

#define PD_POWERDOWN						0	/* 	penirq 						*/
#define PD_IREFOFF_ADCON					1	/* 	no penirq 					*/

#define M_12BIT 							0	/*	12bit ADC MODEL				*/
#define M_8BIT 								1	/*	8BIT ADC MODEL				*/

#define TP_PEND_DET_PORT		AP_TP_PEND_DET_PORT
#define TP_PEND_DET_GPIO		AP_TP_PEND_DET_GPIO

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_sendCmd();
* 功能:	send command to Ak4183
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
//#define SOFT_I2C
U32 hyhwAk4183_sendCmd(unsigned short Cmd)
{
	unsigned char ucBuf[2];
	
	ucBuf[0] = AK4183_WR_Addr;			// slave address
	ucBuf[1] = Cmd & 0xFF;

#ifdef GPIO_AS_IIC0
	if (hyhwIIC_WriteBytes(ucBuf[0], &ucBuf[1], 1 ) == HY_ERROR)
#else
	if(hyhwI2c_writeByte(I2C_CH0,ucBuf[0], &ucBuf[1], 1 ) == HY_ERROR)
#endif
	{
		return HY_ERROR;
	}
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_readCmd();
* 功能:	read command to Ak4183
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_readCmd(unsigned char *dataBuff,unsigned short num)
{
	unsigned char ucBuf[2];

	ucBuf[0] = AK4183_WR_Addr;			// slave address

#ifdef GPIO_AS_IIC0
	if(hyhwIIC_ReadBytes(ucBuf[0], dataBuff, num) == HY_ERROR)
#else
	if(hyhwI2c_readByte(I2C_CH0, ucBuf[0], dataBuff, num ) == HY_ERROR)
#endif
	{
		return HY_ERROR;
	}
	return 1;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_Initial(void);
* 功能:	initial Ak4183
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
//U16 test_buff1[10],test_buff2[10],i= 0;
void  hyhwAk4183_Initial(void)
{
	hyhwGpio_setAsGpio(TP_PEND_DET_PORT, TP_PEND_DET_GPIO);
	hyhwGpio_setIn(TP_PEND_DET_PORT, TP_PEND_DET_GPIO);
	hyhwAk4183_SleepAndIntOn();
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4138_Get_12BitOut(void);
* 功能:	得到adc的值
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
int hyhwAk4138_Get_12BitOut(int type)
{
    int adc;
    
    if (type == 0)
    {    
        adc = hyhwAk4183_TSC_12bitout(MEAS_XPOS,PD_IREFOFF_ADCON);
    }
    else
    {
        adc = hyhwAk4183_TSC_12bitout(MEAS_YPOS,PD_IREFOFF_ADCON);
    }
    
    hyhwAk4183_SleepAndIntOn();
    
    return adc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_PendDetect(void);
* 功能:	detect pend interrupt
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_PenDetect(void)
{
	U32 pen_flag;
	
	pen_flag = hyhwGpio_Read(TP_PEND_DET_PORT, TP_PEND_DET_GPIO);	
	return pen_flag;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_PenDetectHi_z
* 功能:	置ak4183 PENIRQ脚为高阻态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_PenDetectHi_z(void)
{
	hyhwAk4183_sendCmd(0x72);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAk4183_SleepAndIntOn
* 功能:	置ak4183为节电模式，且允许PENIRQ脚在有触碰时置低
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_SleepAndIntOn(void)
{
	hyhwAk4183_sendCmd(0xc0);
}
/************************************************************************
 * Function            : hyhwAk4183_TSC_12bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 12bit result
*************************************************************/
U16 hyhwAk4183_TSC_12bitout(unsigned char cmd,unsigned char pd)
{
	U16 reuslt,buff1,buff2;
	hyhwAk4183_sendCmd((cmd<<4)|(pd<<2));
	hyhwAk4183_readCmd(&reuslt,2);
	
	buff2  = (reuslt<<8)&0xff00;
	buff1  = (reuslt>>8)&0x00ff;
	reuslt = ((buff2|buff1)>>4)&0x0fff;
	
	return reuslt;
}
/************************************************************************
 * Function            : hyhwAk4183_TSC_8bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 8bit result
*************************************************************/
U8 hyhwAk4183_TSC_8bitout(unsigned char cmd,unsigned char pd)
{
	U8 reuslt;
	hyhwAk4183_sendCmd((cmd<<4)|(pd<<2)|(M_8BIT<<1));
	hyhwAk4183_readCmd(&reuslt,1);
	
	return reuslt;
}
	
// end of filB