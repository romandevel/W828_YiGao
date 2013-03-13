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
#include "hyhwCkc.h"
#include "hyhwGpio.h"



#define I2C_USE_WLOCK				//不带操作系统时注释掉

#ifdef I2C_USE_WLOCK
U32 lockI2cChannel0 = 0;
U32 lockI2cChannel1 = 0;
U32 lockI2cInit = 0;
#endif

/*-----------------------------------------------------------------------------
* 函数:	wLock_I2cInit
* 功能:	I2C初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void wLock_I2cInit(void)
{
#ifdef I2C_USE_WLOCK
	lockI2cInit = 1;
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	wLock_I2c
* 功能:	共享资源Spi申请并lock
* 		在使用I2c之前调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void wLock_I2c(U8 channel)
{
#ifdef I2C_USE_WLOCK
	U32 *pI2c;
	if (channel == I2C_CH0)
	{
		pI2c = (U32*)&lockI2cChannel0;
	}
	else
	{
		pI2c = (U32*)&lockI2cChannel1;
	}
	if (lockI2cInit == 1)
		(void)wlock((U32) pI2c);
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	unLock_I2c
* 功能:	共享资源申请并lock
* 		在使用I2c之后调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void unLock_I2c(U8 channel)
{
#ifdef I2C_USE_WLOCK
	U32 *pI2c;
	if (channel == I2C_CH0)
	{
		pI2c = (U32*)&lockI2cChannel0;
	}
	else
	{
		pI2c = (U32*)&lockI2cChannel1;
	}
	if (lockI2cInit == 1)
		(void)unlock((U32) pI2c);
#endif
}

/*以下部分为硬件IIC部分*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_Init
* 功能:	set i2c 通讯速率
* 参数:	channel	--- 通道index 
*		rate    --- 波特率	
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_Init(U8 channel,eI2c_Rate rate)
{
	tI2C_REGISTER *ptI2cReg;
	U32  i2cClk,Prescale;
	  
	
	//参数检查	
	if(channel >I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//局部变量首次赋值部分
	i2cClk = I2C_CLK;		
	
	//函数处理	
	//hyhwCpm_setI2cClk();
	/* initial i2c port */
	if (channel == I2C_CH0)
	{
		hyhwGpio_setI2c0Function();
	}
	else
	{
		hyhwGpio_setI2c1Function();
	}
	
	Prescale = i2cClk / rate - 1;
		
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + channel; 
	
	ptI2cReg->I2C_PRESCLE	= Prescale;
	ptI2cReg->I2C_TIMING   &= I2C_BIT_I2CCLK; 			/* select i2c clk from Clock controller */
	
	ptI2cReg->I2C_CONTROL  &= I2C_BIT_DATA_WIDTH_8;		/* set data width 8 bit */
	ptI2cReg->I2C_CONTROL  |= ( I2C_BIT_CORE_ENABLE		/* enable i2c core */
							   |I2C_BIT_INT_ENABLE		/* enable i2c core interrupt */
							  ); 	
	//函数结束返回
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
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
U32 hyhwI2c_readData(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
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
	
#if 0	
//由于g-sensor mma8452的IIC读时序中不能有stop，故在这里去除---gao 2012.10.11
//经过初步测试，没有影响其他IIC器件读功能
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
#endif

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

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData_16bit(U8 channel, U8 SlaveAddr, U16 subaddr, U16 *buf, U16 num)
/* 2009.8.5 Zeet edited for OV3640 8Bit Regsiter Data Write Operation */
{
	tI2C_REGISTER *ptI2cReg;
	unsigned int i,j,loopcount = 0;
	U32 status;
	U32 rc = HY_OK;
	U8 subaddr_low,subaddr_high,data_high,data_low;
	
	subaddr_high = (subaddr & 0xff00)>>8;
	subaddr_low  = (subaddr & 0x00ff);
	
	data_high = (*buf & 0xff00)>>8;
	data_low  = (*buf & 0x00ff);
	
	if(channel >I2C_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		wLock_I2c(channel);
		
		ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);	 
		ptI2cReg->I2C_TRANSMIT = SlaveAddr;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

		loopcount = 0;
		// 这里把高8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_high;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		// 这里把低8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_low;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		for(j=0;j<num;j++)
		{
#if 0
			ptI2cReg->I2C_TRANSMIT = data_high;
			ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
				
			loopcount = 0;
#endif
			ptI2cReg->I2C_TRANSMIT = data_low;
			ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;		
 // 2009.5.25 Zeet edited
			loopcount = 0;
			while((ptI2cReg->I2C_STATUS & I2C_BIT_RXACK))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
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
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
	}	
	
	unLock_I2c(channel);
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData_16bit_OV(U8 channel, U8 SlaveAddr, U16 subaddr, U16 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	U32 j,loopcount;
	U8 subaddr_low,subaddr_high,data_high,data_low;
	
	//参数合法性检查
	if(channel >I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//局部变量首次赋值部分
	loopcount = 0;
	subaddr_high = (subaddr & 0xff00)>>8;
	subaddr_low  = (subaddr & 0x00ff);
	data_high = (*buf & 0xff00)>>8;
	data_low  = (*buf & 0x00ff);
	
	//函数处理
	wLock_I2c(channel);
	
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR + (channel*0x40));	 
	ptI2cReg->I2C_TRANSMIT = SlaveAddr;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

	loopcount = 0;
	// 这里把高8位地址发出去
	ptI2cReg->I2C_TRANSMIT = subaddr_high;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	loopcount = 0;
	// 这里把低8位地址发出去
	ptI2cReg->I2C_TRANSMIT = subaddr_low;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	for(j=0;j<num;j++)
	{
#if 0 //2010.10.26 Zeet write off.
		ptI2cReg->I2C_TRANSMIT = data_high;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

		loopcount = 0;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
			
		loopcount = 0;
#endif
		ptI2cReg->I2C_TRANSMIT = data_low;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		loopcount = 0;
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;		
		while((ptI2cReg->I2C_STATUS & I2C_BIT_RXACK))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
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
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
	
	
	unLock_I2c(channel);
	
	//函数返回值处理部分
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readData_16bit(U8 channel,U8 SlaveAddr,U16 subaddr, U16 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	unsigned int j,loopcount = 0;
	U32 status;
	U32 rc = HY_OK;
	U8 subaddr_low,subaddr_high,data_high,data_low;
	
	subaddr_high = (subaddr & 0xff00)>>8;
	subaddr_low  = (subaddr & 0x00ff);
	
	if(channel >I2C_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		wLock_I2c(channel);
		
		ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);
	 
	ptI2cReg->I2C_TRANSMIT = SlaveAddr;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY)
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
	loopcount = 0;
	// 这里把高8位地址发出去
	ptI2cReg->I2C_TRANSMIT = subaddr_high;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	loopcount = 0;
	// 这里把低8位地址发出去
	ptI2cReg->I2C_TRANSMIT = subaddr_low;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

//这里用不用STOP 需要测试
/*	
	loopcount = 0;
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY)
		{
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
*/// 2009.5.25 Zeet edited		
		loopcount = 0;
		ptI2cReg->I2C_TRANSMIT = (SlaveAddr|I2C_RD);
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
		
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		for(j=0;j<num;j++)
		{
			ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE;
			//ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE|I2C_BIT_NACK; //2010.8.19 Zeet added.
#if 1//2010.8.16 Zeet write off.			 	 
			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY)
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			data_high = ptI2cReg->I2C_RECEIVE;
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
			
			if((num - 1) == j)								/*last data no ack  */
				ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE|I2C_BIT_NACK;  
			else 
			 	ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE;
#endif
#if 1			 	 
			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY)
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			data_low = ptI2cReg->I2C_RECEIVE;
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
#endif
		}
//		*buf = (data_high<<8)|data_low;
		*buf = data_high;//2010.8.19 Zeet added.
#if 1
		loopcount = 0;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;				
#endif
	}
	
	unLock_I2c(channel);
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeByte
* 功能:	这个函数是写AK4183 操作
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeByte(U8 channel, U8 SlaveAddr, U8 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	U32 i,j,loopcount;
	static U8 test = 1;
	
	//参数检查
	if(channel > I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//处理	
	loopcount = 0;
	
	wLock_I2c(channel);
	
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR + (channel*0x40));	 
	ptI2cReg->I2C_TRANSMIT = SlaveAddr;
	
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY) 
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

	//loopcount = 0;
	for(j=0;j<num;j++)
	{
		loopcount = 0;
		ptI2cReg->I2C_TRANSMIT = buf[j];
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;			
		loopcount = 0;
		while((ptI2cReg->I2C_STATUS & I2C_BIT_RXACK))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
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
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
	
	unLock_I2c(channel);	

	//函数结束返回值处理
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readByte
* 功能:	这个函数是读AK4183 操作
* 参数:	channel	 -- 通道index 
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readByte(U8 channel, U8 SlaveAddr, U8 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	U32 j,loopcount;
	
	// 参数合法性检查
	if(channel >I2C_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	// 处理
	loopcount = 0;
	wLock_I2c(channel);
	
	ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR + (channel*0x40));

	ptI2cReg->I2C_TRANSMIT = (SlaveAddr|I2C_RD);
	ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
	
	while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
	{
		loopcount++;
		if(loopcount >= I2C_ACK_DELAY)
		{
			unLock_I2c(channel);
			return HY_ERROR;
		}
	}
	
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
	
	//loopcount = 0;
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
				return HY_ERROR;
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
			return HY_ERROR;
		}
	}
	ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;				
		
	unLock_I2c(channel);
	
	//函数结束返回值处理
	return HY_OK;
}
	
/*以下部分为模拟IIC部分*/
#define IIC_ACK			0
#define IIC_NACK		1

#if 1
#define IIC_DELAYTIME_20us 	20		//20uS
#define IIC_DELAYTIME_10us 	4		//10uS
#define IIC_DELAYTIME_5us 	2		//5uS
#define IIC_DELAYTIME		20		//10uS
#else
#define IIC_DELAYTIME_20us 	2000		//20uS
#define IIC_DELAYTIME_10us 	400		//10uS
#define IIC_DELAYTIME_5us 	200		//5uS
#define IIC_DELAYTIME		2000		//10uS
#endif

static U8 IIC_CLK_Port = 0; 		//设置I2C用那个组的I/O，要对应的组的通道。
static U8 IIC_DAT_Port = 0;

static U32 sIIC_CLK_Gpio = 0;		//设置I2C用那个的I/O
static U32 sIIC_DAT_Gpio = 0;

extern tSYSCLKREGISTER_ST  SysClkRegister;
/*************************************************************************
 * Function            : hyhwIIC_DelayuS
 * Description         : delay ?? uS
 * parameter		   : time to delay
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_DelayuS( U16 time )
{
	U32 clk;
	clk = (SysClkRegister.mCpuClk)*time;
	clk /= 5;
	while( clk --);
}

/*************************************************************************
 * Function            : hyhwIIC_Init
 * Description         : Initialize the GPIO that simulate the IIC driver
  * Return 			   : NON
 *************************************************************************/
U32 hyhwIIC_Init(U8 kIIC_CLK_Port,U8 kIIC_DAT_Port,U32 kIIC_CLK_Gpio,U32 kIIC_DAT_Gpio )
{	
	sIIC_CLK_Gpio 	= kIIC_CLK_Gpio;
	sIIC_DAT_Gpio 	= kIIC_DAT_Gpio;
	
	IIC_CLK_Port = kIIC_CLK_Port;
	IIC_DAT_Port = kIIC_DAT_Port;
	
	//参数检查
	if ((sIIC_CLK_Gpio==0 || sIIC_DAT_Gpio==0)||(IIC_CLK_Port>=GPIO_NUM || IIC_DAT_Port>=GPIO_NUM))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	hyhwGpio_setAsGpio(IIC_CLK_Port, sIIC_CLK_Gpio);
	hyhwGpio_setOut(IIC_CLK_Port, sIIC_CLK_Gpio);
	hyhwGpio_setHigh(IIC_CLK_Port, sIIC_CLK_Gpio);
	
	hyhwGpio_setAsGpio(IIC_DAT_Port, sIIC_DAT_Gpio);
	hyhwGpio_setOut(IIC_DAT_Port, sIIC_DAT_Gpio);
	hyhwGpio_setHigh(IIC_DAT_Port, sIIC_DAT_Gpio);
	
	return HY_OK;
}

/*************************************************************************
 * Function            : hyhwIIC_Start
 * Description         : Iic start
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_Start(void)
{
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_20us);
	
	hyhwGpio_setLow(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
	hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);				//08-08-09
	
}

/*************************************************************************
 * Function            : hyhwIIC_RepeatedStart
 * Description         : Iic Repeated Start
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_RepeatedStart(void)
{
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	hyhwIIC_DelayuS(IIC_DELAYTIME_20us);
	hyhwIIC_DelayuS(IIC_DELAYTIME_20us);
	hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_20us);
	hyhwIIC_DelayuS(IIC_DELAYTIME_20us);
	hyhwGpio_setLow(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
	hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
}

/*************************************************************************
 * Function            : hyhwIIC_Stop
 * Description         : Iic stop
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_Stop(void)
{
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
	hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
}

/*************************************************************************
 * Function            : hyhwIIC_SendAck
 * Description         : send ack to slave
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_SendAck(void)
{
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setLow(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME);
	hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
}

/*************************************************************************
 * Function            : hyhwIIC_SendNack
 * Description         : send nack to slave
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_SendNack(void)
{
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME);
	hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
}

/*************************************************************************
 * Function            : hyhwIIC_CheckAck
 * Description         : to check the slave ack or nack
 * Return 			   : ACK,NACK
 *************************************************************************/
U8 hyhwIIC_CheckAck(void)
{
	U8 ack;
	
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return (0xFF);
	}
	hyhwGpio_setIn(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setPullUp(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_5us);
	hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_5us);
	ack = hyhwGpio_Read(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwIIC_DelayuS(IIC_DELAYTIME_5us);
	hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
	
	return ack;

}

/*************************************************************************
 * Function            : hyhwIIC_WriteByte
 * Description         : write one byte to the slave
 * parameter:		   : cha to send
 * Return 			   : none
 *************************************************************************/
void hyhwIIC_WriteByte(U8 cha)
{
	U8 i;
	
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return;
	}
	hyhwGpio_setOut(IIC_DAT_Port,sIIC_DAT_Gpio);
	for(i=0;i<8;i++)
	{
		if(cha&0x80)
		{
			hyhwGpio_setHigh(IIC_DAT_Port,sIIC_DAT_Gpio);
			hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
			hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
			hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
			hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
		}
		else
		{
			hyhwGpio_setLow(IIC_DAT_Port,sIIC_DAT_Gpio);
			hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
			hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
			hyhwIIC_DelayuS(IIC_DELAYTIME_10us);
			hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
		}
		cha = cha<<1;
	}
}

/*************************************************************************
 * Function            : hyhwIIC_ReadByte
 * Description         : read one byte from the slave
 * parameter:		   : none
 * Return 			   : byte read
 *************************************************************************/
U8 hyhwIIC_ReadByte(void)
{
	U8 ret,j;
	
	if (sIIC_CLK_Gpio == 0 || sIIC_DAT_Gpio == 0)
	{
		return 0;
	}
	ret = 0;
	hyhwGpio_setIn(IIC_DAT_Port,sIIC_DAT_Gpio);
	hyhwGpio_setPullUp(IIC_DAT_Port,sIIC_DAT_Gpio);
	
	for(j=0;j<8;j++)
	{
		ret 	 = ret<<1;
		
		hyhwIIC_DelayuS(IIC_DELAYTIME);
		hyhwGpio_setHigh(IIC_CLK_Port,sIIC_CLK_Gpio);
		hyhwIIC_DelayuS(IIC_DELAYTIME);
		if(hyhwGpio_Read(IIC_CLK_Port,sIIC_DAT_Gpio))
		{
			ret |= 0x01;
		}
		hyhwGpio_setLow(IIC_CLK_Port,sIIC_CLK_Gpio);
		
	}
	
	return(ret);
}

/*************************************************************************
 * Function            : hyhwIIC_WriteData
 * Description         : write n bytes to slave
 * parameter:		   : slave address,data to write ,bytes to write
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_WriteData(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
{
	U8 ack;
	U16 i,j;
	
	wLock_I2c(1);
	hyhwIIC_Start();
	SlaveAddr &= 0xfe; //write
	hyhwIIC_WriteByte(SlaveAddr);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}	
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}	
		
	hyhwIIC_WriteByte(subaddr);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}	
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}
	
	for(j=0;j<num;j++)
	{
		hyhwIIC_WriteByte(buf[j]);
		for(i =0;i<200;i++)
		{
			ack = hyhwIIC_CheckAck();
			if(ack == IIC_ACK)
			{
				break;
			}	
		}
		if(ack == IIC_NACK)
		{
			unLock_I2c(1);
			return HY_ERROR;
		}	
	}		
	hyhwIIC_Stop();
	
	unLock_I2c(1);
	return HY_OK;
}

/*************************************************************************
 * Function            : hyhwIIC_ReadData
 * Description         : read n bytes from slave
 * parameter:		   : slave address,data buffer for reading ,bytes to read
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_ReadData(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num)
{
	U8 ack;
	U16 i,j;
	
	wLock_I2c(1);
	hyhwIIC_Start();
	hyhwIIC_WriteByte(SlaveAddr);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}	
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}
		
	hyhwIIC_WriteByte(subaddr);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}	
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}	
//2008-08-09		
	hyhwIIC_RepeatedStart();
		
	hyhwIIC_WriteByte(SlaveAddr|0x01);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}	
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}
		
	for(j=0;j<num-1;j++)
	{
		buf[j] = hyhwIIC_ReadByte();
		hyhwIIC_SendAck();
	}
	buf[j] = hyhwIIC_ReadByte();
	hyhwIIC_SendNack();
	hyhwIIC_Stop();
	
	unLock_I2c(1);
	return HY_OK;
}

/*************************************************************************
 * Function            : hyhwIIC_WriteBytes
 * Description         : write n bytes to slave
 * parameter:		   : slave address,data to write ,bytes to write
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_WriteBytes(U8 SlaveAddr, U8 *buf, U16 num)
{
	U8 ack;
	U16 i,j;
	
	wLock_I2c(1);
	hyhwIIC_Start();
	SlaveAddr &= 0xfe; //write
	hyhwIIC_WriteByte(SlaveAddr);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}
	
	for(j=0;j<num;j++)
	{
		hyhwIIC_WriteByte(buf[j]);
		for(i =0;i<200;i++)
		{
			ack = hyhwIIC_CheckAck();
			if(ack == IIC_ACK)
			{
				break;
			}
		}
		if(ack == IIC_NACK)
		{
			unLock_I2c(1);
			return HY_ERROR;
		}
	}
	hyhwIIC_Stop();
	
	unLock_I2c(1);
	return HY_OK;
}

/*************************************************************************
 * Function            : hyhwIIC_ReadBytes
 * Description         : read n bytes from slave
 * parameter:		   : slave address,data buffer for reading ,bytes to read
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_ReadBytes(U8 SlaveAddr, U8 *buf, U16 num)
{
	U8 ack;
	U16 i,j;
	
	wLock_I2c(1);
	hyhwIIC_Start();
	hyhwIIC_WriteByte(SlaveAddr|0x01);
	for(i =0;i<200;i++)
	{
		ack = hyhwIIC_CheckAck();
		if(ack == IIC_ACK)
		{
			break;
		}
	}
	if(ack == IIC_NACK)
	{
		unLock_I2c(1);
		return HY_ERROR;
	}
	
	for(j=0;j<num-1;j++)
	{
		buf[j] = hyhwIIC_ReadByte();
		hyhwIIC_SendAck();
	}
	buf[j] = hyhwIIC_ReadByte();
	hyhwIIC_SendNack();
	hyhwIIC_Stop();
	
	unLock_I2c(1);
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_CIF_writeData_16bit(U8 channel,unsigned char SlaveAddr, U16 subaddr, U16 *buf,unsigned short num)
//U32 hyhwI2c_writeData_16bit(U8 channel,unsigned char SlaveAddr, U16 subaddr, U16 *buf,unsigned short num)
{
	tI2C_REGISTER *ptI2cReg;
	unsigned int i,j,loopcount = 0;
	U32 status;
	U32 rc = HY_OK;
	U8 subaddr_low,subaddr_high,data_high,data_low;
	
	subaddr_high = (subaddr & 0xff00)>>8;
	subaddr_low  = (subaddr & 0x00ff);
	
	data_high = (*buf & 0xff00)>>8;
	data_low  = (*buf & 0x00ff);
	
	if(channel >I2C_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		wLock_I2c(channel);
		
		ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);	 
		ptI2cReg->I2C_TRANSMIT = SlaveAddr;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

		loopcount = 0;
		// 这里把高8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_high;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		// 这里把低8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_low;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		for(j=0;j<num;j++)
		{
/*			ptI2cReg->I2C_TRANSMIT = data_high;
			ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
				
			loopcount = 0;
*/			ptI2cReg->I2C_TRANSMIT = data_low;
			ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;

			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;		
 // 2009.5.25 Zeet edited
			loopcount = 0;
			while((ptI2cReg->I2C_STATUS & I2C_BIT_RXACK))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY) 
				{
					unLock_I2c(channel);
					return HY_ERROR;
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
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
	}	
	
	unLock_I2c(channel);
	return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_CIF_readData_16bit(U8 channel,unsigned char SlaveAddr,U16 subaddr, U16 *buf,unsigned short num)
{
	tI2C_REGISTER *ptI2cReg;
	unsigned int j,loopcount = 0;
	U32 status;
	U32 rc = HY_OK;
	U8 subaddr_low,subaddr_high,data_high,data_low;
	
	subaddr_high = (subaddr & 0xff00)>>8;
	subaddr_low  = (subaddr & 0x00ff);
	
	if(channel >I2C_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		wLock_I2c(channel);
		
		ptI2cReg = (tI2C_REGISTER*)(I2C_BASEADDR) + (channel);
		 
		ptI2cReg->I2C_TRANSMIT = SlaveAddr;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
		
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
			
		loopcount = 0;
		// 这里把高8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_high;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		// 这里把低8位地址发出去
		ptI2cReg->I2C_TRANSMIT = subaddr_low;
		
		ptI2cReg->I2C_COMMMAND = I2C_BIT_WR_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY) 
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;

//这里用不用STOP 需要测试
		
		loopcount = 0;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
/**/// 2009.5.25 Zeet edited		
		loopcount = 0;
		ptI2cReg->I2C_TRANSMIT = (SlaveAddr|I2C_RD);
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STA_ENABLE|I2C_BIT_WR_ENABLE;
		
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;
		
		loopcount = 0;
		for(j=0;j<num;j++)
		{
			ptI2cReg->I2C_COMMMAND = I2C_BIT_RD_ENABLE;
			 	 
			while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
			{
				loopcount++;
				if(loopcount >= I2C_ACK_DELAY)
				{
					unLock_I2c(channel);
					return HY_ERROR;
				}
			}
			data_high = ptI2cReg->I2C_RECEIVE;
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
			
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
					return HY_ERROR;
				}
			}
			data_low = ptI2cReg->I2C_RECEIVE;
			ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;	
		}
		*buf = (data_high<<8)|data_low;
		loopcount = 0;
		ptI2cReg->I2C_COMMMAND = I2C_BIT_STP_ENABLE;
		while( !(I2C_IRQSTR_REGISTER & (1 << channel)))
		{
			loopcount++;
			if(loopcount >= I2C_ACK_DELAY)
			{
				unLock_I2c(channel);
				return HY_ERROR;
			}
		}
		ptI2cReg->I2C_COMMMAND |= I2C_BIT_IACK;				
	}
	
	unLock_I2c(channel);
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwIIC_writeData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwIIC_writeData_16bit(U8 SlaveAddr, U16 subaddr, U16 *buf, U16 num)
{
	tI2C_REGISTER *ptI2cReg;
	unsigned int i,j;
	U32 rc = HY_OK;
	U8 subaddr_arr[2],data[2];
	
	subaddr_arr[0] = (subaddr & 0xff00)>>8;
	subaddr_arr[1] = (subaddr & 0x00ff);
	
	{
		wLock_I2c(1);
		
		hyhwIIC_Start();
		hyhwIIC_WriteByte(SlaveAddr);
		for(i=0;i<200;i++)
		{
			rc = hyhwIIC_CheckAck();
			if(rc == IIC_ACK)
			{
				break;
			}
		}
		if(rc == IIC_NACK)
		{
			unLock_I2c(1);
			return HY_ERROR;
		}
		//发送subaddr高8bit，再发低8bit
		for (j=0; j<2; j++)
		{
			hyhwIIC_WriteByte(subaddr_arr[j]);
			for(i =0;i<200;i++)
			{
				rc = hyhwIIC_CheckAck();
				if(rc == IIC_ACK)
				{
					break;
				}
			}
			if(rc == IIC_NACK)
			{
				unLock_I2c(1);
				return HY_ERROR;
			}
		}
		for(j=0;j<num;j++)
		{
			data[0] = (*buf & 0xff00)>>8;
			data[1] = (*buf & 0x00ff);
#if 0
			//发高8bit
			hyhwIIC_WriteByte(data[0]);
#endif
			//发低8bit
			hyhwIIC_WriteByte(data[1]);
			for(i=0;i<200;i++)
			{
				rc = hyhwIIC_CheckAck();
				if(rc == IIC_ACK)
				{
					break;
				}
			}
			if(rc == IIC_NACK)
			{
				unLock_I2c(1);
				return HY_ERROR;
			}
		}
		hyhwIIC_Stop();
	}
	
	unLock_I2c(1);
	return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwIIC_readData_16bit(U8 SlaveAddr,U16 subaddr, U16 *buf, U16 num)
{
	unsigned int i, j;
	U32 rc = HY_OK;
	U8 subaddr_arr[2], data[2], data_high, data_low;
	
	subaddr_arr[0] = (subaddr & 0xff00)>>8;
	subaddr_arr[1] = (subaddr & 0x00ff);
	
	{
		wLock_I2c(1);
		
		hyhwIIC_Start();
		hyhwIIC_WriteByte(SlaveAddr);
		for(i=0;i<200;i++)
		{
			rc = hyhwIIC_CheckAck();
			if(rc == IIC_ACK)
			{
				break;
			}
		}
		if(rc == IIC_NACK)
		{
			unLock_I2c(1);
			return HY_ERROR;
		}
		//发送subaddr高8bit，再发低8bit
		for (j=0; j<2; j++)
		{
			hyhwIIC_WriteByte(subaddr_arr[j]);
			for(i=0;i<200;i++)
			{
				rc = hyhwIIC_CheckAck();
				if(rc == IIC_ACK)
				{
					break;
				}
			}
			if(rc == IIC_NACK)
			{
				unLock_I2c(1);
				return HY_ERROR;
			}
		}

		hyhwIIC_RepeatedStart();
		hyhwIIC_WriteByte(SlaveAddr|0x01);
		for(i =0;i<200;i++)
		{
			rc = hyhwIIC_CheckAck();
			if(rc == IIC_ACK)
			{
				break;
			}
		}
		if(rc == IIC_NACK)
		{
			unLock_I2c(1);
			return HY_ERROR;
		}
		
		for(j=0;j<num-1;j++)
		{
			#if 0
			data_high = hyhwIIC_ReadByte();
			data_low = hyhwIIC_ReadByte();
			hyhwIIC_SendAck();
			//buf[j] = (data_high<<8)|data_low;
			buf[j] = data_high;
			#else
			buf[j] = (U16)hyhwIIC_ReadByte();
			hyhwIIC_SendAck();
			#endif
		}
		buf[j] = (U16)hyhwIIC_ReadByte();
		hyhwIIC_SendNack();
		hyhwIIC_Stop();
	}
	
	unLock_I2c(1);
	return rc;
}
