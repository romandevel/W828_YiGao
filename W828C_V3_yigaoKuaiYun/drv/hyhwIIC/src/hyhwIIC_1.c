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
#include "hyhwIIC.h"
//#include "hyhwCkc.h"
//#include "hyhwGpio.h"


/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData_DS2460_1
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData1(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	U32 j,loopcount;
	U32 testRet, testFlag;
	
	//参数检查
	if(channel >I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	wLock_I2c(channel);
	
	loopcount = 0;
	testFlag = 0;
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);	 
	ptI2cReg->I2C_TRANSMIT = SlaveAddr;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	
	
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 1;
			testRet = HY_ERROR;
			goto quitIIC;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	loopcount = 0;
	ptI2cReg->I2C_TRANSMIT = subaddr;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 2;
			testRet = HY_ERROR;
			goto quitIIC;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	for(j=0;j<num;j++)
	{
		loopcount = 0;
		ptI2cReg->I2C_TRANSMIT = buf[j];
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= (I2C_ACK_DELAY<<1)) 
			{
				unLock_I2c(channel);
				//return HY_ERROR;
				testFlag = 3;
				testRet = HY_ERROR;
				goto quitIIC;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		while((ptI2cReg->I2C_STATUS & I2C_BIT_RXACK))
		{
			loopcount++;
			if(loopcount >= (I2C_ACK_DELAY<<1)) 
			{
				unLock_I2c(channel);
				//return HY_ERROR;
				testFlag = 4;
				testRet = HY_ERROR;
				goto quitIIC;
			}
		}
	}
		
	loopcount = 0;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 5;
			testRet = HY_ERROR;
			goto quitIIC;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
		
	unLock_I2c(channel);
	
	return HY_OK;
quitIIC:
	testFlag = 6;
	return testRet;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readData1(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	U32 j,loopcount;
	U32 testRet, testFlag;

	//参数检查
	if(channel >I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	wLock_I2c(channel);
	
	loopcount = 0;
	testFlag = 0;
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);
	 
	ptI2cReg->I2C_TRANSMIT = SlaveAddr;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;

	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 1;
			testRet = HY_ERROR;
			goto quitIIC_r;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
	loopcount = 0;
	ptI2cReg->I2C_TRANSMIT = subaddr;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 2;
			testRet = HY_ERROR;
			goto quitIIC_r;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	loopcount = 0;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 3;
			testRet = HY_ERROR;
			goto quitIIC_r;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

	loopcount = 0;
	ptI2cReg->I2C_TRANSMIT = (SlaveAddr|I2C_RD);
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 4;
			testRet = HY_ERROR;
			goto quitIIC_r;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	for(j=0;j<num;j++)
	{
		loopcount = 0;
		if((num - 1) == j)								/*last data no ack  */
			ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE|I2C_BIT_NACK;  
		else 
		 	ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE;
		 	 
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				//return HY_ERROR;
				testFlag = 5;
				testRet = HY_ERROR;
				goto quitIIC_r;
			}
		}
		buf[j] = ptI2cReg->I2C_RECEIVE;
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
	}
	loopcount = 0;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			//return HY_ERROR;
			testFlag = 6;
			testRet = HY_ERROR;
			goto quitIIC_r;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;				
	
	unLock_I2c(channel);
	
	return HY_OK;
quitIIC_r:
	testFlag = 7;
	return testRet;
}
