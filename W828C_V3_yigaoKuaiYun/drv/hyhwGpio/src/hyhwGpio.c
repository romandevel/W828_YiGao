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
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwGpio.h"
#include "hyhwUart.h"
#include "hyhwIntCtrl.h"


/*------------------------------------------------------------------------------
说明：
	1.由于硬件GPIO有4组端口所以在设置GPIO时必须选择通道参数，

设置方法：
		如故要设置端口A 1脚为I/O功能  为输出用： 并设置为高状态
		
	1. hyhwGpio_SetasGpio(channel,bit ); channel 参数是PORT_A, bit参数是BIT1; 
	
	2. hyhwGpio_SetOut(channel, bit ); channel 参数是PORT_A, bit参数是BIT1; 
  	
	3. hyhwGpio_SetHigh( channel,bit );channel 参数是PORT_A, bit参数是BIT1; 

    设置其他的功能：根据不同的功能设置 找对应的函数。
    
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Read
* 功能:	读取指定GPIO的状态（值）
* 参数:	指定GPIO的掩码（只能一个GPIO）
* 返回:	指定GPIO的状态（值）,1:high;0:low
*       位操作
        for example  如果置PORT_A位0的状态 unsigned char hyhwGpio_Read(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_Read(U8 port,U32 mask)  
{
 	tGPIO_REGISTER *ptGpioReg;

	U32 ret ;
	
	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}


	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + (port); 
	
	ret = ptGpioReg->DATA_REGISTER & mask ;
	if(ret > 0 )
	{ 
		return 1 ;
	}
	else
	{
		return 0 ;
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setLow
* 功能:	设置指定GPIO为low
* 参数:	指定GPIO的掩码
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
        for example  如果置PORT_A位0为0调用函数hyhwGpio_SetLow(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setLow(U8 port,U32 mask)
{
 	tGPIO_REGISTER *ptGpioReg;
 	
	//参数检查	
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
 	
	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + (port);
	ptGpioReg->DATA_REGISTER &= ~mask;

	return HY_OK;	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setHigh
* 功能:	设置指定GPIO为high
* 参数:	指定GPIO的掩码
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
		for example  如果置PORT_A位0为1调用函数hyhwGpio_SetHigh(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setHigh(U8 port,U32 mask)
{
 	tGPIO_REGISTER *ptGpioReg;
		
	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + (port);
	ptGpioReg-> DATA_REGISTER |= mask;	 

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setXOR
* 功能:	设置指定GPIO的异或值
* 参数:	指定GPIO的掩码
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
		for example  如果置PORT_A位0为异或值调用函数hyhwGpio_setXOR(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setXOR(U8 port,U32 mask)
{
 	tGPIO_REGISTER *ptGpioReg;
	
	//参数检查
 	if(port > GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	 
	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + port;
	ptGpioReg->XOR_REGISTER &= mask;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setOut
* 功能:	设置指定GPIO为输出IO
* 参数:	指定GPIO的掩码  direction control: 1 output and 0 input
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
		for example 如果置PORT_A位0为OUTPUT调用函数 hyhwGpio_SetOut(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setOut(U8 port,U32 mask)
{
 	tGPIO_REGISTER *ptGpioReg;
  	
  	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + (port); 
	ptGpioReg-> DIRECTION |= mask;	

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setIn
* 功能:	设置指定GPIO为输入IO
* 参数:	指定GPIO的掩码  direction control: 1 output and 0 input
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
		for example 如果置PORT_A位0为INTPUT调用函数 hyhwGpio_SetOut(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setIn(U8 port,U32 mask)
{
 	tGPIO_REGISTER *ptGpioReg;
 
 	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	} 

	ptGpioReg = (tGPIO_REGISTER*)(GPIO_GROUP_REGISTER_BASEADDR) + (port); 
	ptGpioReg-> DIRECTION &= ~mask;

	return HY_OK;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwGpio_setAsGpio
* 功能:	设置指定GPIO为IO口功能
* 参数:	指定GPIO的掩码
* 返回:	none
* NOTE:
		1、The Priority of this register is higher than Function Selection Register.
		2、If a certain bit of Function Selection Register is not set, the corresponding GPIO pin act as
		   Normal GPIO pin independent of This Register.
        位操作
        for example 如果置PORT_A位0为IO口功调用函数 hyhwGpio_SetasGpio(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setAsGpio(U8 port, U32 mask)
{
 	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	} 
	
	if(port == PORT_A )
	{
		if((GPIO_0 == mask)||(mask == GPIO_1))
			PORTCFG11 = GPIOA_0to1_GPIO;
		else if( GPIO_2 == mask )
			PORTCFG4 = GPIOA_2_GPIO;
		else if( GPIO_3 == mask )
			PORTCFG4 = GPIOA_3_GPIO;
		else if( GPIO_4 == mask )
			PORTCFG4 = GPIOA_4_GPIO;
		else if( GPIO_5 == mask )
			PORTCFG4 = GPIOA_5_GPIO;
		else if( GPIO_6 == mask )
			PORTCFG7 = GPIOA_6_GPIO;
		else if( GPIO_7 == mask )
			PORTCFG7 = GPIOA_7_GPIO;
		else if( GPIO_8 == mask )
			PORTCFG7 = GPIOA_8_GPIO;
		else if( GPIO_9 == mask )
			PORTCFG8 = GPIOA_9_GPIO;
		else if( GPIO_10 == mask )
			PORTCFG8 = GPIOA_10_GPIO;
		else if( GPIO_11 == mask )
			PORTCFG8 = GPIOA_11_GPIO;
		else if( GPIO_12 == mask )
			PORTCFG8 = GPIOA_12_GPIO;

		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_B )
	{
		if( GPIO_0 == mask  )
			PORTCFG5 = GPIOB_0_GPIO;
		else if( GPIO_1 == mask )
			PORTCFG5 = GPIOB_1_GPIO;
		else if( GPIO_2 == mask )
			PORTCFG5 = GPIOB_2_GPIO;
		else if( GPIO_3 == mask )
			PORTCFG5 = GPIOB_3_GPIO;
		else if( GPIO_4 == mask  )
			PORTCFG5 = GPIOB_4_GPIO;
		else if( GPIO_5 == mask )
			PORTCFG5 = GPIOB_5_GPIO;
		else if( GPIO_6 == mask  )
			PORTCFG5 = GPIOB_6_GPIO;
		else if( GPIO_7 == mask )
			PORTCFG6 = GPIOB_7_GPIO;
		else if( GPIO_8 == mask )
			PORTCFG6 = GPIOB_8_GPIO;
		else if( GPIO_9 == mask )
			PORTCFG6 = GPIOB_9_GPIO;
		else if( GPIO_10 == mask )
			PORTCFG6 = GPIOB_10_GPIO;
		else if( GPIO_11 == mask )
			PORTCFG6 = GPIOB_11_GPIO;
		else if( GPIO_12 == mask )
			PORTCFG6 = GPIOB_12_GPIO;
		else if( GPIO_13 == mask )
			PORTCFG6 = GPIOB_13_GPIO;
		else if( GPIO_14 == mask )
			PORTCFG6 = GPIOB_14_GPIO;
		else if( mask == GPIO_15 )
			PORTCFG7 = GPIOB_15_GPIO;
		else if( mask > GPIO_15 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_C )
	{
		if( (GPIO_0 <= mask)&&(mask <= GPIO_7 ))
			PORTCFG0 = GPIOC_0to7_GPIO;
		else if((GPIO_8 <= mask)&&(mask <= GPIO_15 ))
			PORTCFG0 = GPIOC_8to15_GPIO;
		else if((GPIO_16 <= mask)&&(mask <= GPIO_17 ))
			PORTCFG0 = GPIOC_16to17_GPIO;
		else if( (GPIO_18 <= mask)&&(mask <= GPIO_19 ))
			PORTCFG0 = GPIOC_18to19_GPIO;
		else if( mask == GPIO_20)
			PORTCFG0 = GPIOC_20_GPIO;
		else if( mask == GPIO_21)
			PORTCFG0 = GPIOC_21_GPIO;
		else if( mask == GPIO_22)
			PORTCFG0 = GPIOC_22_GPIO;
		else if( mask == GPIO_23)
			PORTCFG0 = GPIOC_23_GPIO;
		else if( mask == GPIO_24)
			PORTCFG4 = GPIOC_24_GPIO;
		else if( GPIO_25 == mask )
			PORTCFG1 = GPIOC_25_GPIO;
		else if( GPIO_26 == mask )
			PORTCFG1 = GPIOC_26_GPIO;
		else if( GPIO_27 == mask )
			PORTCFG1 = GPIOC_27_GPIO;
		else if( GPIO_28 == mask )
			PORTCFG1 = GPIOC_28_GPIO;
		else if( GPIO_29 == mask )
			PORTCFG1 = GPIOC_29_GPIO;
		else if( GPIO_30 == mask )
			PORTCFG1 = GPIOC_30_GPIO;
		else if( GPIO_31 == mask )
			PORTCFG1 = GPIOC_31_GPIO;

	}
	else if( port == PORT_D )
	{
		if( GPIO_0 == mask )
			PORTCFG11 = GPIOD_0_GPIO;
		else if( mask == GPIO_1)
		 	PORTCFG11 = GPIOD_1_GPIO;
		else if( mask == GPIO_2)
		 	PORTCFG11 = GPIOD_2_GPIO;
		else if( mask == GPIO_3)
		 	PORTCFG11 = GPIOD_3_GPIO;
		else if( mask == GPIO_4)
		 	PORTCFG11 = GPIOD_4_GPIO;
		else if( mask == GPIO_5)
			PORTCFG3 = GPIOD_5_GPIO;
		else if( GPIO_6 == mask )
			PORTCFG4 = GPIOD_6_GPIO;
		else if( GPIO_7 == mask )
			PORTCFG4 = GPIOD_7_GPIO;
		else if( GPIO_8 == mask )
			PORTCFG4 = GPIOD_8_GPIO;
		else if( GPIO_9 == mask )
			PORTCFG7 = GPIOD_9_GPIO;
		else if( GPIO_10 == mask )
			PORTCFG7 = GPIOD_10_GPIO;
		else if( GPIO_11 == mask )
			PORTCFG7 = GPIOD_11_GPIO;
		else if( GPIO_12 == mask )
			PORTCFG7 = GPIOD_12_GPIO;
		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_E )
	{
		if( GPIO_0 == mask )
			PORTCFG9 = GPIOE_0_GPIO;
		else if( GPIO_1 == mask )
			PORTCFG9 = GPIOE_1_GPIO;
		else if( GPIO_2 == mask )
			PORTCFG9 = GPIOE_2_GPIO;
		else if( GPIO_3 == mask )
			PORTCFG9 = GPIOE_3_GPIO;
		else if( GPIO_4 == mask )
			PORTCFG10 = GPIOE_4_GPIO;
		else if( GPIO_5 == mask )
			PORTCFG10 = GPIOE_5_GPIO;
		else if( GPIO_6 == mask )
			PORTCFG10 = GPIOE_6_GPIO;
		else if( GPIO_7 == mask )
			PORTCFG10 = GPIOE_7_GPIO;
		else if( GPIO_8 == mask )
			PORTCFG10 = GPIOE_8_GPIO;
		else if( GPIO_9 == mask )
			PORTCFG10 = GPIOE_9_GPIO;
		else if( GPIO_10 == mask )
			PORTCFG10 = GPIOE_10_GPIO;
		else if( GPIO_11 == mask )
			PORTCFG10 = GPIOE_11_GPIO;
		else if( GPIO_12 == mask )  
			PORTCFG12 = GPIOE_12_GPIO;
		else if( GPIO_13 == mask )  
			PORTCFG12 = GPIOE_13_GPIO;
		else if( GPIO_14 == mask )  
			PORTCFG12 = GPIOE_14_GPIO;
		else if( GPIO_15 == mask )  
			PORTCFG12 = GPIOE_15_GPIO;
		else if( GPIO_16 == mask )  
			PORTCFG12 = GPIOE_16_GPIO;
		else if( GPIO_17 == mask )
			PORTCFG13 = GPIOE_17_GPIO;
		else if( GPIO_18 == mask )
			PORTCFG13 = GPIOE_18_GPIO;
		else if( GPIO_19 == mask )
			PORTCFG13 = GPIOE_19_GPIO;
		else if( mask == GPIO_20)
		    PORTCFG11 = GPIOE_20_GPIO;
		else if( GPIO_21 == mask )  
			PORTCFG12 = GPIOE_21_GPIO;
		else if( GPIO_22 == mask )  
			PORTCFG12 = GPIOE_22_GPIO;
		else if( GPIO_23 == mask )  
			PORTCFG12 = GPIOE_23_GPIO;
		else if( GPIO_24 == mask )
			PORTCFG8 = GPIOE_24_GPIO;
		else if( GPIO_25 == mask )
			PORTCFG8 = GPIOE_25_GPIO;
		else if( GPIO_26 == mask )
			PORTCFG8 = GPIOE_26_GPIO;
		else if( GPIO_27 == mask )
			PORTCFG8 = GPIOE_27_GPIO;
		else if( GPIO_28 == mask )
			PORTCFG9 = GPIOE_28_GPIO;
		else if( GPIO_29 == mask )
			PORTCFG9 = GPIOE_29_GPIO;
		else if( GPIO_30 == mask )
			PORTCFG9 = GPIOE_30_GPIO;
		else if( GPIO_31 == mask )
			PORTCFG9 = GPIOE_31_GPIO;
	}
	else if( port == PORT_F )
	{
		if( GPIO_0 == mask )
			PORTCFG3 = GPIOF_0_GPIO;
		else if( GPIO_1 == mask )
			PORTCFG3 = GPIOF_1_GPIO;
		else if( GPIO_2 == mask )
			PORTCFG3 = GPIOF_2_GPIO;
		else if( GPIO_3 == mask )
			PORTCFG3 = GPIOF_3_GPIO;
		else if( GPIO_4 == mask )
			PORTCFG3 = GPIOF_4_GPIO;
		else if( GPIO_5 == mask )
			PORTCFG3 = GPIOF_5_GPIO;
		else if( GPIO_6 == mask )
			PORTCFG3 = GPIOF_6_GPIO;
		else if( GPIO_7 == mask )
			PORTCFG2 =GPIOF_7_GPIO;
		else if( (GPIO_8 <= mask)&&( mask<= GPIO_11 ) )
			PORTCFG2 = GPIOF_8to11_GPIO;
		else if( (GPIO_12 <= mask)&&( mask<= GPIO_15 ) ) 
			PORTCFG2 = GPIOF_12to15_GPIO;
		else if( (GPIO_16 <= mask ) && (mask <= GPIO_17) ) 
			PORTCFG2 = GPIOF_16to17_GPIO;
		else if( ( ( GPIO_18 <= mask)&&(mask <= GPIO_19 ) ) 
				|| ( ( GPIO_21 <= mask)&&(mask <= GPIO_23 ) ) )
			PORTCFG2 = GPIOF_18to19_21to23_GPIO;
		else if( mask == GPIO_20)
			PORTCFG13 = GPIOF_20_GPIO;
		else if(  GPIO_24 == mask )
			PORTCFG2 =GPIOF_24_GPIO;
		else if(  GPIO_25 == mask )
			PORTCFG2 =GPIOF_25_GPIO;
		else if(  GPIO_26 == mask )
			PORTCFG2 =GPIOF_26_GPIO;
		else if( mask == GPIO_27)
			PORTCFG1 = GPIOF_27_GPIO;
		else if( mask == GPIO_28)
			PORTCFG5 = GPIOF_28_GPIO;
		else if(( GPIO_29 <= mask )&&( mask <= GPIO_31 ))
			PORTCFG11 = GPIOF_29to31_GPIO;
	}
		
				
	return HY_OK;
}

/*****************************************************************************
***        说明：上下拉部分函数一次只能控制一个GPIO端口的一位            ***
*****************************************************************************/
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_pullDisable
* 功能:	设置指定GPIO pull disable --Neither pull-up nor pull-down is enabled.
* 参数:	指定GPIO的掩码  direction control: 1 output and 0 input
* 返回:	none
		for example 如果置PORT_A位0为Pull Disable调用函数 hyhwGpio_Pull_Disable(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_pullDisable(U8 port,U32 mask)
{
 	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	} 
	
	else
	{
		if(port == PORT_A )
		{
			if( GPIO_0 == mask )
				CPUD7 = CPUD7 & GPIOA_CPUD_0_DISABLE; 
			else if( GPIO_1 == mask )
				CPUD7 = CPUD7 & GPIOA_CPUD_1_DISABLE; 
			else if( GPIO_2 == mask )
				CPUD3 = CPUD3 &	GPIOA_CPUD_2_DISABLE;
			else if( GPIO_3 == mask )
				CPUD4 = CPUD4 & GPIOA_CPUD_3_DISABLE;
			else if( GPIO_4 == mask )
				CPUD4 = CPUD4 & GPIOA_CPUD_4_DISABLE;
			else if( GPIO_5 == mask )
				CPUD4 = CPUD4 & GPIOA_CPUD_5_DISABLE;
			else if( GPIO_6 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_6_DISABLE;
			else if( GPIO_7 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_7_DISABLE;
			else if( GPIO_8 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_8_DISABLE;
			else if( GPIO_9 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_9_DISABLE;
			else if( GPIO_10 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_10_DISABLE;
			else if( GPIO_11 == mask )
				CPUD5 = CPUD5 & GPIOA_CPUD_11_DISABLE;
			else if( GPIO_12 == mask )  
				CPUD5 = CPUD5 & GPIOA_CPUD_12_DISABLE;
			
			else if( mask > GPIO_12 )
				return HY_ERR_BAD_PARAMETER;
		}
		else if( port == PORT_B )
		{
			if( GPIO_0 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_0_DISABLE;
			else if( GPIO_1 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_1_DISABLE;
			else if( GPIO_2 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_2_DISABLE;
			else if( GPIO_3 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_3_DISABLE;
			else if( GPIO_4 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_4_DISABLE;
			else if( GPIO_5 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_5_DISABLE;
			else if( GPIO_6 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_6_DISABLE;
			else if( GPIO_7 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_7_DISABLE;
			else if( GPIO_8 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_8_DISABLE;
			else if( GPIO_9 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_9_DISABLE;
			else if( GPIO_10 == mask )
				CPUD4 = CPUD4 & GPIOB_CPUD_10_DISABLE;
			else if( GPIO_11 == mask )
				CPUD5 = CPUD5 & GPIOB_CPUD_11_DISABLE;
			else if( GPIO_12 == mask )  
				CPUD5 = CPUD5 & GPIOB_CPUD_12_DISABLE;
			else if( GPIO_13 == mask )  
				CPUD5 = CPUD5 & GPIOB_CPUD_13_DISABLE;
			else if( GPIO_14 == mask )  
				CPUD5 = CPUD5 & GPIOB_CPUD_14_DISABLE;
			else if( GPIO_15 == mask )  
				CPUD5 = CPUD5 & GPIOB_CPUD_15_DISABLE;
			
			else if( mask > GPIO_15 )
				return HY_ERR_BAD_PARAMETER;
		}
		else if( port == PORT_C )
		{
			if( GPIO_0 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_0_DISABLE;
			else if( GPIO_1 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_1_DISABLE;
			else if( GPIO_2 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_2_DISABLE;
			else if( GPIO_3 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_3_DISABLE;
			else if( GPIO_4 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_4_DISABLE;
			else if( GPIO_5 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_5_DISABLE;
			else if( GPIO_6 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_6_DISABLE;
			else if( GPIO_7 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_7_DISABLE;
			else if( GPIO_8 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_8_DISABLE;
			else if( GPIO_9 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_9_DISABLE;
			else if( GPIO_10 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_10_DISABLE;
			else if( GPIO_11 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_11_DISABLE;
			else if( GPIO_12 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_12_DISABLE;
			else if( GPIO_13 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_13_DISABLE;
			else if( GPIO_14 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_14_DISABLE;
			else if( GPIO_15 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_15_DISABLE;
			else if( GPIO_16 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_16_DISABLE;
			else if( GPIO_17 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_17_DISABLE;
			else if( GPIO_18 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_18_DISABLE;
			else if( GPIO_19 == mask )
				CPUD0 = CPUD0 & GPIOC_CPUD_19_DISABLE;
			else if( mask == GPIO_20)
			    CPUD0 = CPUD0 & GPIOC_CPUD_20_DISABLE;
			else if( GPIO_21 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_21_DISABLE;
			else if( GPIO_22 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_22_DISABLE;
			else if( GPIO_23 == mask )  
				CPUD0 = CPUD0 & GPIOC_CPUD_23_DISABLE;
			else if( GPIO_24 == mask )
				CPUD4 = CPUD4 & GPIOC_CPUD_24_DISABLE;
			else if( GPIO_25 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_25_DISABLE;
			else if( GPIO_26 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_26_DISABLE;
			else if( GPIO_27 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_27_DISABLE;
			else if( GPIO_28 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_28_DISABLE;
			else if( GPIO_29 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_29_DISABLE;
			else if( GPIO_30 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_30_DISABLE;
			else if( GPIO_31 == mask )
				CPUD1 = CPUD1 & GPIOC_CPUD_31_DISABLE;
		
		}
		else if( port == PORT_D )
		{
			if( GPIO_0 == mask )
				CPUD7 = CPUD7 & GPIOD_CPUD_0_DISABLE;
			else if( GPIO_1 == mask )
				CPUD7 = CPUD7 & GPIOD_CPUD_1_DISABLE;
			else if( GPIO_2 == mask )
				CPUD7 = CPUD7 & GPIOD_CPUD_2_DISABLE;
			else if( GPIO_3 == mask )
				CPUD7 = CPUD7 & GPIOD_CPUD_3_DISABLE;
			else if( GPIO_4 == mask )
				CPUD7 = CPUD7 & GPIOD_CPUD_4_DISABLE;
			else if( GPIO_5 == mask )
				CPUD3 = CPUD3 & GPIOD_CPUD_5_DISABLE;
			else if( GPIO_6 == mask )
				CPUD3 = CPUD3 & GPIOD_CPUD_6_DISABLE;
			else if( GPIO_7 == mask )
				CPUD3 = CPUD3 & GPIOD_CPUD_7_DISABLE;
			else if( GPIO_8 == mask )
				CPUD3 = CPUD3 & GPIOD_CPUD_8_DISABLE;
			else if( GPIO_9 == mask )
				CPUD5 = CPUD5 & GPIOD_CPUD_9_DISABLE;
			else if( GPIO_10 == mask )
				CPUD5 = CPUD5 & GPIOD_CPUD_10_DISABLE;
			else if( GPIO_11 == mask )
				CPUD5 = CPUD5 & GPIOD_CPUD_11_DISABLE;
			else if( GPIO_12 == mask )  
				CPUD5 = CPUD5 & GPIOD_CPUD_12_DISABLE;
			
			else if( mask > GPIO_12 )
				return HY_ERR_BAD_PARAMETER;
		}
		else if( port == PORT_E )
		{
			if( GPIO_0 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_0_DISABLE;
			else if( GPIO_1 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_1_DISABLE;
			else if( GPIO_2 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_2_DISABLE;
			else if( GPIO_3 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_3_DISABLE;
			else if( GPIO_4 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_4_DISABLE;
			else if( GPIO_5 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_5_DISABLE;
			else if( GPIO_6 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_6_DISABLE;
			else if( GPIO_7 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_7_DISABLE;
			else if( GPIO_8 == mask )
				CPUD7 = CPUD7 & GPIOE_CPUD_8_DISABLE; 
			else if( GPIO_9 == mask )
				CPUD7 = CPUD7 & GPIOE_CPUD_9_DISABLE; 
			else if( GPIO_10 == mask )
				CPUD7 = CPUD7 & GPIOE_CPUD_10_DISABLE; 
			else if( GPIO_11 == mask )
				CPUD7 = CPUD7 & GPIOE_CPUD_11_DISABLE; 
			else if( GPIO_12 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_12_DISABLE; 
			else if( GPIO_13 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_13_DISABLE; 
			else if( GPIO_14 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_14_DISABLE; 
			else if( GPIO_15 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_15_DISABLE; 
			else if( GPIO_16 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_16_DISABLE; 
			else if( GPIO_17 == mask )
				CPUD8 = CPUD8 & GPIOE_CPUD_17_DISABLE; 
			else if( GPIO_18 == mask )
				CPUD8 = CPUD8 & GPIOE_CPUD_18_DISABLE; 
			else if( GPIO_19 == mask )
				CPUD8 = CPUD8 & GPIOE_CPUD_19_DISABLE; 
			else if( GPIO_20 == mask )
			   CPUD7 = CPUD7 & GPIOE_CPUD_20_DISABLE; 
			else if( GPIO_21 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_21_DISABLE; 
			else if( GPIO_22 == mask )  
				CPUD8 = CPUD8 & GPIOE_CPUD_22_DISABLE; 
			else if( GPIO_23 == mask )  
				CPUD7 = CPUD7 & GPIOE_CPUD_23_DISABLE; 
			else if( GPIO_24 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_24_DISABLE;
			else if( GPIO_25 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_25_DISABLE;
			else if( GPIO_26 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_26_DISABLE;
			else if( GPIO_27 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_27_DISABLE;
			else if( GPIO_28 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_28_DISABLE;
			else if( GPIO_29 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_29_DISABLE;
			else if( GPIO_30 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_30_DISABLE;
			else if( GPIO_31 == mask )
				CPUD6 = CPUD6 & GPIOE_CPUD_31_DISABLE;
		}
		else if( port == PORT_F )
		{
			if( mask==GPIO_28 )
				CPUD4 = CPUD4 & GPIOF_CPUD_28_DISABLE;
			else
			return HY_ERR_BAD_PARAMETER;
		}
	}
	return HY_OK;

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setPullUp
* 功能:	设置指定GPIO pull up model
* 参数:	指定GPIO的掩码  direction control: 1 output and 0 input
* 返回:	none
		for example 如果置PORT_A位0为Pull Up 调用函数 hyhwGpio_Set_Pull_Up(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setPullUp(U8 port,U32 mask)
{
 	//参数检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	} 
	
	if(port == PORT_A )
	{
		if( GPIO_0 == mask )
			CPUD7 = (CPUD7 & GPIOA_CPUD_0_DISABLE)|BIT6; 
		else if( GPIO_1 == mask )
			CPUD7 = (CPUD7 & GPIOA_CPUD_1_DISABLE)|BIT4; 
		else if( GPIO_2 == mask )
			CPUD3 = (CPUD3 &	GPIOA_CPUD_2_DISABLE)|BIT0;
		else if( GPIO_3 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_3_DISABLE)|BIT30;
		else if( GPIO_4 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_4_DISABLE)|BIT28;
		else if( GPIO_5 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_5_DISABLE)|BIT26;
		else if( GPIO_6 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_6_DISABLE)|BIT12;
		else if( GPIO_7 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_7_DISABLE)|BIT10;
		else if( GPIO_8 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_8_DISABLE)|BIT8;
		else if( GPIO_9 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_9_DISABLE)|BIT6;
		else if( GPIO_10 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_10_DISABLE)|BIT4;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_11_DISABLE)|BIT2;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOA_CPUD_12_DISABLE)|BIT0;
		
		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_B )
	{
		if( GPIO_0 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_0_DISABLE)|BIT20;
		else if( GPIO_1 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_1_DISABLE)|BIT18;
		else if( GPIO_2 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_2_DISABLE)|BIT16;
		else if( GPIO_3 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_3_DISABLE)|BIT14;
		else if( GPIO_4 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_4_DISABLE)|BIT12;
		else if( GPIO_5 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_5_DISABLE)|BIT10;
		else if( GPIO_6 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_6_DISABLE)|BIT8;
		else if( GPIO_7 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_7_DISABLE)|BIT6;
		else if( GPIO_8 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_8_DISABLE)|BIT4;
		else if( GPIO_9 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_9_DISABLE)|BIT2;
		else if( GPIO_10 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_10_DISABLE)|BIT0;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOB_CPUD_11_DISABLE)|BIT30;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_12_DISABLE)|BIT28;
		else if( GPIO_13 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_13_DISABLE)|BIT26;
		else if( GPIO_14 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_14_DISABLE)|BIT24;
		else if( GPIO_15 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_15_DISABLE)|BIT22;
		
		else if( mask > GPIO_15 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_C )
	{
		if( GPIO_0 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_0_DISABLE)|BIT14;
		else if( GPIO_1 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_1_DISABLE)|BIT12;
		else if( GPIO_2 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_2_DISABLE)|BIT10;
		else if( GPIO_3 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_3_DISABLE)|BIT8;
		else if( GPIO_4 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_4_DISABLE)|BIT6;
		else if( GPIO_5 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_5_DISABLE)|BIT4;
		else if( GPIO_6 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_6_DISABLE)|BIT2;
		else if( GPIO_7 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_7_DISABLE)|BIT0;
		else if( GPIO_8 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_8_DISABLE)|BIT0;
		else if( GPIO_9 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_9_DISABLE)|BIT2;
		else if( GPIO_10 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_10_DISABLE)|BIT4;
		else if( GPIO_11 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_11_DISABLE)|BIT6;
		else if( GPIO_12 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_12_DISABLE)|BIT8;
		else if( GPIO_13 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_13_DISABLE)|BIT10;
		else if( GPIO_14 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_14_DISABLE)|BIT12;
		else if( GPIO_15 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_15_DISABLE)|BIT14;
		else if( GPIO_16 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_16_DISABLE)|BIT16;
		else if( GPIO_17 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_17_DISABLE)|BIT18;
		else if( GPIO_18 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_18_DISABLE)|BIT20;
		else if( GPIO_19 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_19_DISABLE)|BIT22;
		else if( mask == GPIO_20)
		    CPUD0 = (CPUD0 & GPIOC_CPUD_20_DISABLE)|BIT24;
		else if( GPIO_21 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_21_DISABLE)|BIT26;
		else if( GPIO_22 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_22_DISABLE)|BIT28;
		else if( GPIO_23 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_23_DISABLE)|BIT30;
		else if( GPIO_24 == mask )
			CPUD4 = (CPUD4 & GPIOC_CPUD_24_DISABLE)|BIT24;
		else if( GPIO_25 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_25_DISABLE)|BIT16;
		else if( GPIO_26 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_26_DISABLE)|BIT18;
		else if( GPIO_27 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_27_DISABLE)|BIT20;
		else if( GPIO_28 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_28_DISABLE)|BIT22;
		else if( GPIO_29 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_29_DISABLE)|BIT24;
		else if( GPIO_30 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_30_DISABLE)|BIT26;
		else if( GPIO_31 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_31_DISABLE)|BIT28;
	
	}
	else if( port == PORT_D )
	{
		if( GPIO_0 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_0_DISABLE)|BIT16;
		else if( GPIO_1 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_1_DISABLE)|BIT14;
		else if( GPIO_2 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_2_DISABLE)|BIT12;
		else if( GPIO_3 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_3_DISABLE)|BIT10;
		else if( GPIO_4 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_4_DISABLE)|BIT8;
		else if( GPIO_5 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_5_DISABLE)|BIT8;
		else if( GPIO_6 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_6_DISABLE)|BIT6;
		else if( GPIO_7 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_7_DISABLE)|BIT4;
		else if( GPIO_8 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_8_DISABLE)|BIT2;
		else if( GPIO_9 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_9_DISABLE)|BIT20;
		else if( GPIO_10 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_10_DISABLE)|BIT18;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_11_DISABLE)|BIT16;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOD_CPUD_12_DISABLE)|BIT14;
		
		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_E )
	{
		if( GPIO_0 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_0_DISABLE)|BIT14;
		else if( GPIO_1 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_1_DISABLE)|BIT12;
		else if( GPIO_2 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_2_DISABLE)|BIT10;
		else if( GPIO_3 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_3_DISABLE)|BIT8;
		else if( GPIO_4 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_4_DISABLE)|BIT6;
		else if( GPIO_5 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_5_DISABLE)|BIT4;
		else if( GPIO_6 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_6_DISABLE)|BIT2;
		else if( GPIO_7 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_7_DISABLE)|BIT0;
		else if( GPIO_8 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_8_DISABLE)|BIT30; 
		else if( GPIO_9 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_9_DISABLE)|BIT28; 
		else if( GPIO_10 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_10_DISABLE)|BIT26; 
		else if( GPIO_11 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_11_DISABLE)|BIT24; 
		else if( GPIO_12 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_12_DISABLE)|BIT26; 
		else if( GPIO_13 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_13_DISABLE)|BIT24; 
		else if( GPIO_14 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_14_DISABLE)|BIT22; 
		else if( GPIO_15 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_15_DISABLE)|BIT20; 
		else if( GPIO_16 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_16_DISABLE)|BIT18; 
		else if( GPIO_17 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_17_DISABLE)|BIT16; 
		else if( GPIO_18 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_18_DISABLE)|BIT14; 
		else if( GPIO_19 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_19_DISABLE)|BIT12; 
		else if( GPIO_20 == mask )
		    CPUD7 = (CPUD7 & GPIOE_CPUD_20_DISABLE)|BIT2; 
		else if( GPIO_21 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_21_DISABLE)|BIT30; 
		else if( GPIO_22 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_22_DISABLE)|BIT28; 
		else if( GPIO_23 == mask )  
			CPUD7 = (CPUD7 & GPIOE_CPUD_23_DISABLE)|BIT0; 
		else if( GPIO_24 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_24_DISABLE)|BIT30;
		else if( GPIO_25 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_25_DISABLE)|BIT28;
		else if( GPIO_26 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_26_DISABLE)|BIT26;
		else if( GPIO_27 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_27_DISABLE)|BIT24;
		else if( GPIO_28 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_28_DISABLE)|BIT22;
		else if( GPIO_29 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_29_DISABLE)|BIT20;
		else if( GPIO_30 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_30_DISABLE)|BIT18;
		else if( GPIO_31 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_31_DISABLE)|BIT16;
	}
	else if( port == PORT_F )
	{
		if( mask==GPIO_28 )
			CPUD4 = (CPUD4 & GPIOF_CPUD_28_DISABLE)|BIT22;
		else
		return HY_ERR_BAD_PARAMETER;
	}
	
	return HY_OK;

}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setPullDown
* 功能:	设置指定GPIO PULL DOWN model 
* 参数:	指定GPIO的掩码  direction control: 1 output and 0 input
* 返回:	none
		for example 如果置PORT_A位0为Pull_Down 调用函数 hyhwGpio_Set_Pull_Down(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setPullDown(U8 port,U32 mask)
{
 	//参数合法性检查
	if(port >= GPIO_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if(port == PORT_A )
	{
		if( GPIO_0 == mask )
			CPUD7 = (CPUD7 & GPIOA_CPUD_0_DISABLE)|BIT7; 
		else if( GPIO_1 == mask )
			CPUD7 = (CPUD7 & GPIOA_CPUD_1_DISABLE)|BIT5; 
		else if( GPIO_2 == mask )
			CPUD3 = (CPUD3 &	GPIOA_CPUD_2_DISABLE)|BIT1;
		else if( GPIO_3 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_3_DISABLE)|BIT31;
		else if( GPIO_4 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_4_DISABLE)|BIT29;
		else if( GPIO_5 == mask )
			CPUD4 = (CPUD4 & GPIOA_CPUD_5_DISABLE)|BIT27;
		else if( GPIO_6 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_6_DISABLE)|BIT13;
		else if( GPIO_7 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_7_DISABLE)|BIT11;
		else if( GPIO_8 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_8_DISABLE)|BIT9;
		else if( GPIO_9 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_9_DISABLE)|BIT7;
		else if( GPIO_10 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_10_DISABLE)|BIT5;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOA_CPUD_11_DISABLE)|BIT3;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOA_CPUD_12_DISABLE)|BIT1;
		
		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_B )
	{
		if( GPIO_0 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_0_DISABLE)|BIT21;
		else if( GPIO_1 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_1_DISABLE)|BIT19;
		else if( GPIO_2 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_2_DISABLE)|BIT17;
		else if( GPIO_3 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_3_DISABLE)|BIT15;
		else if( GPIO_4 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_4_DISABLE)|BIT13;
		else if( GPIO_5 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_5_DISABLE)|BIT11;
		else if( GPIO_6 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_6_DISABLE)|BIT9;
		else if( GPIO_7 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_7_DISABLE)|BIT7;
		else if( GPIO_8 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_8_DISABLE)|BIT5;
		else if( GPIO_9 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_9_DISABLE)|BIT3;
		else if( GPIO_10 == mask )
			CPUD4 = (CPUD4 & GPIOB_CPUD_10_DISABLE)|BIT1;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOB_CPUD_11_DISABLE)|BIT31;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_12_DISABLE)|BIT29;
		else if( GPIO_13 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_13_DISABLE)|BIT27;
		else if( GPIO_14 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_14_DISABLE)|BIT25;
		else if( GPIO_15 == mask )  
			CPUD5 = (CPUD5 & GPIOB_CPUD_15_DISABLE)|BIT23;
		
		else if( mask > GPIO_15 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_C )
	{
		if( GPIO_0 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_0_DISABLE)|BIT15;
		else if( GPIO_1 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_1_DISABLE)|BIT13;
		else if( GPIO_2 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_2_DISABLE)|BIT11;
		else if( GPIO_3 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_3_DISABLE)|BIT9;
		else if( GPIO_4 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_4_DISABLE)|BIT7;
		else if( GPIO_5 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_5_DISABLE)|BIT5;
		else if( GPIO_6 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_6_DISABLE)|BIT3;
		else if( GPIO_7 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_7_DISABLE)|BIT1;
		else if( GPIO_8 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_8_DISABLE)|BIT1;
		else if( GPIO_9 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_9_DISABLE)|BIT3;
		else if( GPIO_10 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_10_DISABLE)|BIT5;
		else if( GPIO_11 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_11_DISABLE)|BIT7;
		else if( GPIO_12 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_12_DISABLE)|BIT9;
		else if( GPIO_13 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_13_DISABLE)|BIT11;
		else if( GPIO_14 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_14_DISABLE)|BIT13;
		else if( GPIO_15 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_15_DISABLE)|BIT15;
		else if( GPIO_16 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_16_DISABLE)|BIT17;
		else if( GPIO_17 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_17_DISABLE)|BIT19;
		else if( GPIO_18 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_18_DISABLE)|BIT21;
		else if( GPIO_19 == mask )
			CPUD0 = (CPUD0 & GPIOC_CPUD_19_DISABLE)|BIT23;
		else if( mask == GPIO_20)
		    CPUD0 = (CPUD0 & GPIOC_CPUD_20_DISABLE)|BIT25;
		else if( GPIO_21 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_21_DISABLE)|BIT27;
		else if( GPIO_22 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_22_DISABLE)|BIT29;
		else if( GPIO_23 == mask )  
			CPUD0 = (CPUD0 & GPIOC_CPUD_23_DISABLE)|BIT31;
		else if( GPIO_24 == mask )
			CPUD4 = (CPUD4 & GPIOC_CPUD_24_DISABLE)|BIT25;
		else if( GPIO_25 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_25_DISABLE)|BIT17;
		else if( GPIO_26 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_26_DISABLE)|BIT19;
		else if( GPIO_27 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_27_DISABLE)|BIT21;
		else if( GPIO_28 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_28_DISABLE)|BIT23;
		else if( GPIO_29 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_29_DISABLE)|BIT25;
		else if( GPIO_30 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_30_DISABLE)|BIT27;
		else if( GPIO_31 == mask )
			CPUD1 = (CPUD1 & GPIOC_CPUD_31_DISABLE)|BIT29;
	
	}
	else if( port == PORT_D )
	{
		if( GPIO_0 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_0_DISABLE)|BIT17;
		else if( GPIO_1 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_1_DISABLE)|BIT15;
		else if( GPIO_2 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_2_DISABLE)|BIT13;
		else if( GPIO_3 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_3_DISABLE)|BIT11;
		else if( GPIO_4 == mask )
			CPUD7 = (CPUD7 & GPIOD_CPUD_4_DISABLE)|BIT9;
		else if( GPIO_5 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_5_DISABLE)|BIT9;
		else if( GPIO_6 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_6_DISABLE)|BIT7;
		else if( GPIO_7 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_7_DISABLE)|BIT5;
		else if( GPIO_8 == mask )
			CPUD3 = (CPUD3 & GPIOD_CPUD_8_DISABLE)|BIT3;
		else if( GPIO_9 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_9_DISABLE)|BIT21;
		else if( GPIO_10 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_10_DISABLE)|BIT19;
		else if( GPIO_11 == mask )
			CPUD5 = (CPUD5 & GPIOD_CPUD_11_DISABLE)|BIT17;
		else if( GPIO_12 == mask )  
			CPUD5 = (CPUD5 & GPIOD_CPUD_12_DISABLE)|BIT15;
		
		else if( mask > GPIO_12 )
			return HY_ERR_BAD_PARAMETER;
	}
	else if( port == PORT_E )
	{
		if( GPIO_0 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_0_DISABLE)|BIT15;
		else if( GPIO_1 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_1_DISABLE)|BIT13;
		else if( GPIO_2 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_2_DISABLE)|BIT11;
		else if( GPIO_3 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_3_DISABLE)|BIT9;
		else if( GPIO_4 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_4_DISABLE)|BIT7;
		else if( GPIO_5 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_5_DISABLE)|BIT5;
		else if( GPIO_6 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_6_DISABLE)|BIT3;
		else if( GPIO_7 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_7_DISABLE)|BIT1;
		else if( GPIO_8 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_8_DISABLE)|BIT31; 
		else if( GPIO_9 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_9_DISABLE)|BIT29; 
		else if( GPIO_10 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_10_DISABLE)|BIT27; 
		else if( GPIO_11 == mask )
			CPUD7 = (CPUD7 & GPIOE_CPUD_11_DISABLE)|BIT25; 
		else if( GPIO_12 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_12_DISABLE)|BIT27; 
		else if( GPIO_13 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_13_DISABLE)|BIT25; 
		else if( GPIO_14 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_14_DISABLE)|BIT23; 
		else if( GPIO_15 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_15_DISABLE)|BIT21; 
		else if( GPIO_16 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_16_DISABLE)|BIT19; 
		else if( GPIO_17 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_17_DISABLE)|BIT17; 
		else if( GPIO_18 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_18_DISABLE)|BIT15; 
		else if( GPIO_19 == mask )
			CPUD8 = (CPUD8 & GPIOE_CPUD_19_DISABLE)|BIT13; 
		else if( GPIO_20 == mask )
		    CPUD7 = (CPUD7 & GPIOE_CPUD_20_DISABLE)|BIT3; 
		else if( GPIO_21 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_21_DISABLE)|BIT31; 
		else if( GPIO_22 == mask )  
			CPUD8 = (CPUD8 & GPIOE_CPUD_22_DISABLE)|BIT29; 
		else if( GPIO_23 == mask )  
			CPUD7 = (CPUD7 & GPIOE_CPUD_23_DISABLE)|BIT1; 
		else if( GPIO_24 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_24_DISABLE)|BIT31;
		else if( GPIO_25 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_25_DISABLE)|BIT29;
		else if( GPIO_26 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_26_DISABLE)|BIT27;
		else if( GPIO_27 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_27_DISABLE)|BIT25;
		else if( GPIO_28 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_28_DISABLE)|BIT23;
		else if( GPIO_29 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_29_DISABLE)|BIT21;
		else if( GPIO_30 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_30_DISABLE)|BIT19;
		else if( GPIO_31 == mask )
			CPUD6 = (CPUD6 & GPIOE_CPUD_31_DISABLE)|BIT17;
	}
	else if( port == PORT_F )
	{
		if( mask==GPIO_28 )
			CPUD4 = (CPUD4 & GPIOF_CPUD_28_DISABLE)|BIT23;
		else
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	return HY_OK;

}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setNfc8Function
* 功能:	设置指定GPIO为指定功能口:NAND FLASH CONTROLLER
* 参数: none
* 返回:	none
* 2009.9.23 Zeet edited.由于GPIOF[26],GPIOF[27]用作Cmossensor的RST & PWD信号,
* 						如在此初始化成NAND相关的功能,会导致这两个IO口后续使用
*						出问题,所以注销GPIOF[26],GPIOF[27]在这个函数的端口功能设置.
*----------------------------------------------------------------------------*/
void hyhwGpio_setNfc8Function(void)
{
//	PORTCFG1 = (PORTCFG1 & (~(GPIOF_27))) | NFC_NANDRDY3_ENABLE;
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_24 | GPIOF_25 /*| GPIOF_26*/ )))
			    | (NFC_NANDCSN2_ENABLE | NFC_NANDCSN3_ENABLE /*| NFC_NANDRDY2_ENABLE*/);
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_0 | GPIOB_1 | GPIOB_2 | GPIOB_3 | GPIOB_4
				| GPIOB_5 | GPIOB_6))) | ( NFC_NANDXD_0_ENABLE|NFC_NANDXD_1_ENABLE
				|NFC_NANDXD_2_ENABLE|NFC_NANDXD_3_ENABLE|NFC_NANDXD_4_ENABLE
				|NFC_NANDXD_5_ENABLE|NFC_NANDXD_6_ENABLE );
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_7 | GPIOB_8 | GPIOB_9 | GPIOB_10 | GPIOB_11
				 | GPIOB_12 | GPIOB_13 | GPIOB_14))) | ( NFC_NANDXD_7_ENABLE
				 |NFC_NANDOEN_ENABLE|NFC_NANDWEN_ENABLE|NFC_NANDCSN0_ENABLE
				 |NFC_NANDCSN1_ENABLE|NFC_NANDCLE_ENABLE|NFC_NANDALE_ENABLE
				 |NFC_NANDRDY0_ENABLE );
	PORTCFG7 = (PORTCFG7 & (~(GPIOB_15))) | NFC_NANDRDY1_ENABLE;	
    
	   
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setNfc16Function
* 功能:	设置指定GPIO为指定功能口:NAND FLASH CONTROLLER
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setNfc16Function(void)
{
	PORTCFG1 = (PORTCFG1 & (~(GPIOF_27))) | NFC_NANDRDY3_ENABLE;
		PORTCFG2 = (PORTCFG2 & (~(GPIOF_8to11 | GPIOF_12to15 | GPIOF_24 | GPIOF_25
				| GPIOF_26 ))) | (NFC_NANDXD_8to11_ENABLE|NFC_NANDXD_12to15_ENABLE
				|NFC_NANDCSN2_ENABLE | NFC_NANDCSN3_ENABLE | NFC_NANDRDY2_ENABLE);
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_0 | GPIOB_1 | GPIOB_2 | GPIOB_3 | GPIOB_4
				| GPIOB_5 | GPIOB_6))) | ( NFC_NANDXD_0_ENABLE|NFC_NANDXD_1_ENABLE
				|NFC_NANDXD_2_ENABLE|NFC_NANDXD_3_ENABLE|NFC_NANDXD_4_ENABLE
				|NFC_NANDXD_5_ENABLE|NFC_NANDXD_6_ENABLE );
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_7 | GPIOB_8 | GPIOB_9 | GPIOB_10 | GPIOB_11
				 | GPIOB_12 | GPIOB_13 | GPIOB_14))) | ( NFC_NANDXD_7_ENABLE
				 |NFC_NANDOEN_ENABLE|NFC_NANDWEN_ENABLE|NFC_NANDCSN0_ENABLE
				 |NFC_NANDCSN1_ENABLE|NFC_NANDCLE_ENABLE|NFC_NANDALE_ENABLE
				 |NFC_NANDRDY0_ENABLE );
	PORTCFG7 = (PORTCFG7 & (~(GPIOB_15))) | NFC_NANDRDY1_ENABLE;		
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setLcdrgbFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 这里是Rgb 屏
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdrgbFunction(void)
{
	
	PORTCFG0 = PORTCFG0 & ( ~(GPIOC_0to7 | GPIOC_8to15 ) );
	PORTCFG1 = PORTCFG1 & ( ~(GPIOC_25 | GPIOC_26 | GPIOC_27 | GPIOC_28) );
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setLcdpipFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 这里是pip 屏
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdpipFunction(void)
{
	PORTCFG1 = (PORTCFG1 & (~(GPIOF_27))) | LDE_IN_ENABLE;
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_7 | GPIOF_8to11 | GPIOF_12to15 | GPIOF_16to17
			 | GPIOF_18to19_21to23 | GPIOF_24 | GPIOF_25 | GPIOF_26)))
			  |(LCD_LPDIN_7|LCD_LPDIN_8to11|LCD_LPDIN_12to15|LCD_LPDIN_16to17
			  |LCD_LPDIN_18to19_21to23|LCDPIP_LHS_IN_ENABLE|LCDPIP_LVS_IN_ENABLE
			  |LCDPIP_LCLK_IN_ENABLE);
	PORTCFG3 = (PORTCFG3 & (~(GPIOF_0 | GPIOF_1 | GPIOF_2 | GPIOF_3 | GPIOF_4 
				| GPIOF_5 | GPIOF_6))) | (LCD_LPDIN_0| LCD_LPDIN_1 | LCD_LPDIN_2 
				| LCD_LPDIN_3 | LCD_LPDIN_4 | LCD_LPDIN_5 | LCD_LPDIN_6);
				
	PORTCFG13 = (PORTCFG13 & (~(GPIOF_20))) | LCD_LPDIN_20;	
		 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setLcdcpuFunction
* 功能:	设置指定GPIO为指定功能口:LCD cpu
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdcpuFunction(void)
{
	
	
	PORTCFG0 = (PORTCFG0 & (~(GPIOC_0to7 | GPIOC_8to15 | GPIOC_16to17 ))) 
				| (LCD_0to7_LXD | LCD_8to15_LXD | LCD_16to17_LXD);
	PORTCFG1 = (PORTCFG1 & (~(GPIOC_25 | GPIOC_26 | GPIOC_27 | GPIOC_28 | GPIOC_29 )))
				| ( LCDCPU_LWEN_ENABLE | LCDCPU_LOEN_ENABLE | LCDCPU_LXA_ENABLE
				 | LCDCPU_LCSN0_ENABLE | LCDCPU_LCSN1_ENABLE );
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setAdcFunction
* 功能:	设置指定GPIO为指定功能口:ADC
* 参数: none     注意 : 复用的功能口,打开某一功能,同时也要把其他功能都关掉
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setAdcFunction(void)
{
	
	//PORTCFG8 = PORTCFG8 & (~(GPIOE_24 | GPIOE_25 | GPIOE_26 | GPIOE_27));
	//PORTCFG9 = PORTCFG9 & (~(GPIOE_28 | GPIOE_29 | GPIOE_30 | GPIOE_31));
	
	
	PORTCFG8 = PORTCFG8 & (~(GPIOE_24));
	PORTCFG8 = PORTCFG8 | (BIT8 | BIT4 | BIT0);
	PORTCFG9 = PORTCFG9 | (BIT28|BIT24|BIT20|BIT16);

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setI2c0Function
* 功能:	设置指定GPIO为指定功能口:I2C0
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setI2c0Function(void)
{	
	PORTCFG11 = (PORTCFG11 & (~(GPIOA_0to1))) | I2C_SCL0_SDA0_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setI2c1Function
* 功能:	设置指定GPIO为指定功能口:I2C1
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setI2c1Function(void)
{
	PORTCFG7 = (PORTCFG7 & (~(GPIOA_8)))|I2C_SCL1_ENABLE;
	PORTCFG8 = (PORTCFG8 & (~(GPIOA_9)))|I2C_SDA1_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_CIF_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setCifFunction(void)
{
	PORTCFG11 = PORTCFG11 & (~(GPIOE_20));
	PORTCFG12 = PORTCFG12 & (~(GPIOE_23 | GPIOE_21 | GPIOE_22 | GPIOE_12 | GPIOE_13
				 | GPIOE_14 | GPIOE_15 | GPIOE_16));
	//PORTCFG13 = PORTCFG13 & (~(GPIOE_17 | GPIOE_18 | GPIOE_19));			 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart0_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart0Function(int isFlow)
{
	if(isFlow == 1)
	{
		// 流控
		PORTCFG9 = PORTCFG9 & (~(GPIOE_0 | GPIOE_1 | GPIOE_2 | GPIOE_3 ));
	}else
	{
		// 不待流控
		PORTCFG9 = PORTCFG9 & (~(GPIOE_0 | GPIOE_1 ));
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart1_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart1Function(int isFlow)
{
	if(isFlow == 1)
	{
		// 使用流控
		PORTCFG10 = PORTCFG10 & (~(GPIOE_4 | GPIOE_5 | GPIOE_6 | GPIOE_7 ));
	}else
	{
		// 不使用流控
		PORTCFG10 = PORTCFG10 & (~(GPIOE_4 | GPIOE_5));
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart2_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart2Function(void)
{
	PORTCFG10 = PORTCFG10 & (~(GPIOE_8 | GPIOE_9));
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart3_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart3Function(void)
{
	PORTCFG10 = PORTCFG10 & (~(GPIOE_10 | GPIOE_11));
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart4_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart4Function(void)
{
	PORTCFG9 = (PORTCFG9 & (~(GPIOE_2 | GPIOE_3))) | (UART4_UTXD_ENABLE | UART4_URXD_ENABLE );
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_Set_Uart5_Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart5Function(void)
{
	//PORTCFG10 = (PORTCFG10 & (~(GPIOE_6 | GPIOE_7))) | (UART5_UTXD_ENABLE | UART5_URXD_ENABLE);
	PORTCFG10 = UART5_UTXD_ENABLE;
	PORTCFG10 = UART5_URXD_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setDaiFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setDaiFunction(void)
{
	PORTCFG11 = PORTCFG11 & (~(GPIOD_0 | GPIOD_1 | GPIOD_2 | GPIOD_3 | GPIOD_4));
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSpdiftxFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSpdiftxFunction(void)
{
	PORTCFG7 = (PORTCFG7 & (~(GPIOA_6))) | SPDIFTX_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setTimerFunction
* 功能:	设置指定GPIO为指定功能口：Output of Timer
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTimerFunction(void)
{
	
	PORTCFG7 = (PORTCFG7 & (~(GPIOA_6 | GPIOA_7 ))) | (TIM_TCO1 | TIM_TCO0);
	PORTCFG4 = (PORTCFG4 & (~(GPIOA_4 | GPIOA_5 ))) | (TIM_TCO3 | TIM_TCO2);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setTimerExtclkiFunction
* 功能:	设置指定GPIO为指定功能口：External Clock Input
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTimerExtclkiFunction(void)
{
	PORTCFG7 = (PORTCFG7 & (~(GPIOA_7)))|TIM_EXTCLKI_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setEmcFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setEmcFunction(void)
{
	PORTCFG4 = PORTCFG4 & (~(GPIOC_24));
	PORTCFG5 = PORTCFG5 & (~(GPIOF_28));
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setCkcFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setCkcFunction(void)
{
	PORTCFG4 = (PORTCFG4 & (~(GPIOA_2 | GPIOA_3 | GPIOA_4 ))) 
				|( CLK_OUT0|CLK_OUT1|CLK_WDTRSTO_ENABLE );
}

void hyhwGpio_setCkcFunction_gpioA3_32kHz(void)
{
	PORTCFG4 = (PORTCFG4 & (~GPIOA_3)) | CLK_OUT1;
	CKC_PCK_TCT_REGISTER = MODEL_BIT_ENABLE|(CLK_32k<<24);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setIdeFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setIdeFunction(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_7 | GPIOF_8to11 | GPIOF_12to15 | GPIOF_16to17
				 | GPIOF_18to19_21to23 | GPIOF_24 | GPIOF_26))) | (IDE_HDDXD_7
				 |IDE_HDDXD_8to11|IDE_HDDXD_12to15|IDE_HDDCSN_01_ENABLE
				 |IDE_HDD_RW_AD_ENABLE|IDE_HDDAK_ENABLE|IDE_HDDRDY_ENABLE);
	PORTCFG3 =  (PORTCFG3 & (~(GPIOF_0 | GPIOF_1 | GPIOF_2 | GPIOF_3 | GPIOF_4 
				| GPIOF_5 | GPIOF_6 ))) | (IDE_HDDXD_0|IDE_HDDXD_1|IDE_HDDXD_2
				|IDE_HDDXD_3|IDE_HDDXD_4|IDE_HDDXD_5|IDE_HDDXD_6);
	PORTCFG13 = (PORTCFG13 & (~(GPIOF_20))) | IDE_HDDXA0_ENABLE;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSFRamFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSFRamFunction(void)
{
	
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_0 | GPIOB_1 | GPIOB_2 | GPIOB_3 | GPIOB_4 
				| GPIOB_5 | GPIOB_6 ))) | ( SRAMIF_XD0|SRAMIF_XD1|SRAMIF_XD2
				|SRAMIF_XD3|SRAMIF_XD4|SRAMIF_XD5|SRAMIF_XD6 );	 
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_7 | GPIOB_8 | GPIOB_9 | GPIOB_10 | GPIOB_11
				| GPIOB_12 | GPIOB_13 ))) | (SRAMIF_XD7|SRAMIF_OEN|SRAMIF_WEN
				|SRAMIF_CSN0|SRAMIF_CSN1|SRAMIF_XA0|SRAMIF_XA1);
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD0Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD0Function(void)
{
	
	PORTCFG0 = (PORTCFG0 & (~(GPIOC_18to19 | GPIOC_20 | GPIOC_21 | GPIOC_22 
				| GPIOC_23))) | ( SD0_D3toD2|SD0_D1|SD0_D0|SD0_CLK|SD0_CMD );
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD1Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD1Function(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_18to19_21to23))) | SD1_SD1CTRL_ENABLE;
	PORTCFG13 = (PORTCFG13 & (~(GPIOF_20))) | SD1_D1;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD2Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD2Function(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_8to11 | GPIOF_12to15 | GPIOF_16to17))) 
				|( SD2_D0toD3|SD2_D4toD7|SD2_CMD_CLK_ENABLE );
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD3Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD3Function(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_7 | GPIOF_16to17))) |(SD3_D7|SD3_CMD_CLK_ENABLE);
	PORTCFG3 = (PORTCFG3 & (~(GPIOF_0 | GPIOF_1 | GPIOF_2 | GPIOF_3 
				| GPIOF_4 | GPIOF_5 | GPIOF_6))) | (SD3_D0|SD3_D1|SD3_D2|SD3_D3
				|SD3_D4|SD3_D5|SD3_D6);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD4Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD4Function(void)
{
	PORTCFG10 = (PORTCFG10 & (~(GPIOE_6 | GPIOE_7 | GPIOE_8 | GPIOE_9 
				| GPIOE_10 | GPIOE_11))) | (SD4_CLK_ENABLE|SD4_CMD_ENABLE
				|SD4_D0|SD4_D1|SD4_D2|SD4_D3);
				
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSF5amFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD5Function(void)
{
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_0 | GPIOB_1 | GPIOB_2 | GPIOB_3
				| GPIOB_4 | GPIOB_5 | GPIOB_6))) | (SD5_D0|SD5_D1|SD5_D2
				|SD5_D3|SD5_D4|SD5_D5|SD5_D6);
			 
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_7 | GPIOB_8 | GPIOB_9))) 
				| (SD5_D7|SD5_CMD_ENABLE|SD5_CLK_ENABLE);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD6Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD6Function(void)
{
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_10 | GPIOB_11 | GPIOB_12 | GPIOB_13 | GPIOB_14)))
				| (SD6_D0|SD6_D1|SD6_D2|SD6_D3|SD6_CMD_ENABLE);
	PORTCFG7 = (PORTCFG7 & (~(GPIOB_15))) | SD6_CLK_ENABLE;
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setSD7Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD7Function(void)
{
	PORTCFG8 = (PORTCFG8 & (~(GPIOE_26 | GPIOE_27))) | (SD7_CMD_ENABLE | SD7_CLK_ENABLE);
	PORTCFG9 = (PORTCFG9 & (~(GPIOE_28 | GPIOE_29 | GPIOE_30 | GPIOE_31 ))) 
				|( SD7_D0|SD7_D1|SD7_D2|SD7_D3);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setEhiFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setEhiFunction(void)
{
	PORTCFG13 = PORTCFG13 & (~(GPIOF_20));
	PORTCFG3 = PORTCFG3 & (~(GPIOF_0 | GPIOF_1 | GPIOF_2 | GPIOF_3 | GPIOF_4 | GPIOF_5 | GPIOF_6));
			 
	PORTCFG2 = PORTCFG2 & (~(GPIOF_7 | GPIOF_8to11 | GPIOF_12to15 | GPIOF_16to17
				 | GPIOF_18to19_21to23 | GPIOF_24));
			 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb0Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb0Function(void)
{
	PORTCFG9 =(PORTCFG9 & (~(GPIOE_28 | GPIOE_29 | GPIOE_30 | GPIOE_31)))
				 | ( GPSB0_SFRM_ENABLE|GPSB0_SCLK_ENABLE|GPSB0_SDI_ENABLE|GPSB0_SDO_ENABLE );
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb1Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb1Function(void)
{
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_10 | GPIOB_11 | GPIOB_12 | GPIOB_13)))
				| (GPSB1_SFRM_ENABLE|GPSB1_SCLK_ENABLE|GPSB1_SDI_ENABLE|GPSB1_SDO_ENABLE);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb2Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb2Function(void)
{
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_4 | GPIOB_5 | GPIOB_6)))
				| (GPSB2_SFRM_ENABLE|GPSB2_SCLK_ENABLE|GPSB2_SDI_ENABLE);
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_7))) | GPSB2_SDO_ENABLE;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb3Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb3Function(void)
{
	PORTCFG5 = (PORTCFG5 & (~(GPIOB_0 | GPIOB_1 | GPIOB_2 | GPIOB_3)))
				|(GPSB3_SFRM_ENABLE|GPSB3_SCLK_ENABLE|GPSB3_SDI_ENABLE|GPSB3_SDO_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb4Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb4Function(void)
{
	PORTCFG10 = (PORTCFG10 & (~( GPIOE_8 | GPIOE_9 | GPIOE_10
				| GPIOE_11))) | (GPSB4_SFRM_ENABLE|GPSB4_SCLK_ENABLE|GPSB4_SDI_ENABLE
				|GPSB4_SDO_ENABLE);
				
			 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb5Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb5Function(void)
{
	PORTCFG7 = PORTCFG7 & (~(GPIOD_9 | GPIOD_10 | GPIOD_11 | GPIOD_12));
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb6Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb6Function(void)
{
	PORTCFG3 = PORTCFG3 & (~(GPIOD_5));
	PORTCFG4 = PORTCFG4 & (~(GPIOD_6 | GPIOD_7 | GPIOD_8));
			 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb7Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb7Function(void)
{
	PORTCFG3 = (PORTCFG3 & (~(GPIOF_0 | GPIOF_1 | GPIOF_2 | GPIOF_3))) 
				| (GPSB7_SDO_ENABLE|GPSB7_SDI_ENABLE|GPSB7_SCLK_ENABLE
				|GPSB7_SFRM_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb8Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb8Function(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_7))) | GPSB8_SFRM_ENABLE;
	PORTCFG3 = (PORTCFG3 & (~(GPIOF_4 | GPIOF_5 | GPIOF_6))) 
				|(GPSB8_SDO_ENABLE|GPSB8_SDI_ENABLE|GPSB8_SCLK_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb9Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb9Function(void)
{
	PORTCFG1 = (PORTCFG1 & (~(GPIOF_27))) | GPSB9_SFRM_ENABLE;
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_24 | GPIOF_25 | GPIOF_26))) 
				|(GPSB9_SDO_ENABLE|GPSB9_SDI_ENABLE|GPSB9_SCLK_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setGpsb10Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb10Function(void)
{
	PORTCFG0 = (PORTCFG0 & (~(GPIOC_20 | GPIOC_21 | GPIOC_22 | GPIOC_23)))
				|(GPSB10_SDO_ENABLE|GPSB10_SDI_ENABLE|GPSB10_SCLK_ENABLE
				|GPSB10_SFRM_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setMmstick0Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick0Function(void)
{	
	PORTCFG0 = (PORTCFG0 & (~(GPIOC_18to19 | GPIOC_20 | GPIOC_21 | GPIOC_22
				| GPIOC_23))) | (MS0_D3toD2|MS0_D1|MS0_D0|MS0_CLK_ENABLE
				|MS0_BUS_ENABLE);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setMmstick1Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick1Function(void)
{
	PORTCFG2 = (PORTCFG2 & (~(GPIOF_18to19_21to23))) | MS1_MS1CTRL_ENABLE ;
	PORTCFG13 = (PORTCFG13 & (~(GPIOF_20))) | MS1_D1;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setMmstick2Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick2Function(void)
{
	PORTCFG10 = (PORTCFG10 & (~(GPIOE_6 | GPIOE_7 | GPIOE_8 | GPIOE_9 | GPIOE_10
				 | GPIOE_11 ))) | (MS2_CLK_ENABLE|MS2_BUS_ENABLE|MS2_D0|MS2_D1
				 |MS2_D2|MS2_D3);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setMmstick3Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick3Function(void)
{
	PORTCFG7 = (PORTCFG7 & (~(GPIOB_15)))|MS3_CLK_ENABLE;
	PORTCFG6 = (PORTCFG6 & (~(GPIOB_10 | GPIOB_11 | GPIOB_12 | GPIOB_13 
				| GPIOB_14))) | (MS3_D0|MS3_D1|MS3_D2|MS3_D3|MS3_BUS_ENABLE );
				
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setMmstick4Function
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick4Function(void)
{
	PORTCFG9 = (PORTCFG9 & (~(GPIOE_28 | GPIOE_29 | GPIOE_30 | GPIOE_31 ))) 
				|(MS4_D0|MS4_D1|MS4_D2|MS4_D3);
	PORTCFG8 = (PORTCFG8 & (~(GPIOE_26 | GPIOE_27))) | (MS4_BUS_ENABLE|MS4_CLK_ENABLE);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setBmselFunction
* 功能:	设置指定GPIO为指定功能口
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_settBmselFunction(void)
{
	PORTCFG11 = PORTCFG11 & (~(GPIOF_29to31));
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setInterruptEnable
* 功能:	设置指定的中断源为指定中断通道的中断源
* 参数: Eintsel---指定的中断通道
		intsources---指定的中断源
* 返回:	none
*----------------------------------------------------------------------------*
U32 hyhwGpio_setInterruptEnable(U8 Eintsel,U32 intsources)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|intsources;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(intsources << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(intsources << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(intsources << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|intsources;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(intsources << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(intsources << 16);
	}
	else if( Eintsel == EINT74SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(intsources << 24);
	}
	
	return HY_OK;
}
*********************************************************************/

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenUrxd3
* 功能:	使能中断源URXD3，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUrxd3(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_URXD3;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_URXD3 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_URXD3 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_URXD3 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_URXD3;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_URXD3 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_URXD3 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_URXD3 << 24);
	}
	
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenUtxd2
* 功能:	使能中断源Utxd2，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUtxd2(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_UTXD2;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_UTXD2 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_UTXD2 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_UTXD2 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_UTXD2;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_UTXD2 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_UTXD2 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_UTXD2 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenAin7
* 功能:	使能中断源Ain7，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin7(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_AIN7;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_AIN7 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_AIN7 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_AIN7 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_AIN7;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_AIN7 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_AIN7 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_AIN7 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenAin1
* 功能:	使能中断源Ain1，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin1(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_AIN1;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_AIN1 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_AIN1 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_AIN1 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_AIN1;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_AIN1 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_AIN1 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_AIN1 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenAin0
* 功能:	使能中断源Ain0，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin0(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_AIN0;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_AIN0 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_AIN0 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_AIN0 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_AIN0;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_AIN0 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_AIN0 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_AIN0 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa11
* 功能:	使能中断源Gpioa11，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa11(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA11;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA11 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA11 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA11 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA11;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA11 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA11 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA11 << 24);
	}
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa7
* 功能:	使能中断源Gpioa7，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa7(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA7;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA7 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA7 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA7 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA7;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA7 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA7 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA7 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa6
* 功能:	使能中断源Gpioa6，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa6(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA6;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA6 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA6 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA6 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA6;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA6 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA6 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA6 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenSdo1
* 功能:	使能中断源Sdo1，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenSdo1(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_SDO1;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_SDO1 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_SDO1 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_SDO1 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_SDO1;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_SDO1 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_SDO1 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_SDO1 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenScmd1
* 功能:	使能中断源SCMD1，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenScmd1(U8 Eintsel)
{
	//参数合法性检查	
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_SCMD1;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_SCMD1 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_SCMD1 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_SCMD1 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_SCMD1;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_SCMD1 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_SCMD1 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_SCMD1 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob15
* 功能:	使能中断源GPIOB15，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob15(U8 Eintsel)
{
	//参数合法性检查	
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB15;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB15 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB15 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB15 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB15;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB15 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB15 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB15 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob14
* 功能:	使能中断源GPIOB14，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob14(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB14;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB14 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB14 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB14 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB14;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB14 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB14 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB14 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob13
* 功能:	使能中断源GPIOB13，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob13(U8 Eintsel)
{
	//参数合法性
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB13;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB13 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB13 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB13 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB13;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB13 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB13 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB13 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob4
* 功能:	使能中断源GPIOB4，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob4(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB4;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB4 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB4 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB4 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB4;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB4 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB4 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB4 << 24);
	}
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob3
* 功能:	使能中断源GPIOB3，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob3(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB3;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB3 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB3 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB3 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB3;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB3 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB3 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB3 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiob0
* 功能:	使能中断源GPIOB0，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob0(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOB0;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOB0 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOB0 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOB0 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOB0;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOB0 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOB0 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOB0 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa5
* 功能:	使能中断源GPIOA5，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa5(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA5;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA5 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA5 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA5 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA5;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA5 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA5 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA5 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa3
* 功能:	使能中断源GPIOA3，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa3(U8 Eintsel)
{
	//参数合法性检查	
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA3;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA3 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA3 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA3 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA3;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA3 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA3 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA3 << 24);
	}
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioa2
* 功能:	使能中断GPIOA2源，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa2(U8 Eintsel)
{
	//参数合法性检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOA2;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOA2 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOA2 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOA2 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOA2;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOA2 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOA2 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOA2 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenSdo0
* 功能:	使能中断源Sdo0，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenSdo0(U8 Eintsel)
{
	//参数合法检查		
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_SDO0;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_SDO0 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_SDO0 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_SDO0 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_SDO0;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_SDO0 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_SDO0 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_SDO0 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenScmd0
* 功能:	使能中断源Scmd0，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenScmd0(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_SCMD0;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_SCMD0 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_SCMD0 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_SCMD0 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_SCMD0;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_SCMD0 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_SCMD0 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_SCMD0 << 24);
	}
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenHpxd3
* 功能:	使能中断源Hpxd3，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpxd3(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_HPXD3;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_HPXD3 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_HPXD3 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_HPXD3 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_HPXD3;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_HPXD3 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_HPXD3 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_HPXD3 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenHpxd11
* 功能:	使能中断源Hpxd11，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpxd11(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_HPXD11;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_HPXD11 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_HPXD11 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_HPXD11 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_HPXD11;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_HPXD11 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_HPXD11 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_HPXD11 << 24);
	}

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenHprdn
* 功能:	使能中断源Hprdn，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHprdn(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_HPRDN;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_HPRDN << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_HPRDN << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_HPRDN << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_HPRDN;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_HPRDN << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_HPRDN << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_HPRDN << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenHpcsn_l
* 功能:	使能中断源HPCSN_L，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpcsn_l(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_HPCSN_L;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_HPCSN_L << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_HPCSN_L << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_HPCSN_L << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_HPCSN_L;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_HPCSN_L << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_HPCSN_L << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_HPCSN_L << 24);
	}
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiof26
* 功能:	使能中断源GPIOF26，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiof26(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOF26;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOF26 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOF26 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOF26 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOF26;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOF26 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOF26 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOF26 << 24);
	}
		
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpiof27
* 功能:	使能中断源GPIOF27，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiof27(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOF27;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOF27 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOF27 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOF27 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOF27;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOF27 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOF27 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOF27 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenGpioc30
* 功能:	使能中断源GPIOC30，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioc30(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_GPIOC30;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_GPIOC30 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_GPIOC30 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_GPIOC30 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_GPIOC30;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_GPIOC30 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_GPIOC30 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_GPIOC30 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenLpd18
* 功能:	使能中断源LPD18，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenLpd18(U8 Eintsel)
{
	//参数检查		
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_LPD18;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_LPD18 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_LPD18 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_LPD18 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_LPD18;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_LPD18 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_LPD18 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_LPD18 << 24);
	}
	
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenLpd23
* 功能:	使能中断源LPD23，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenLpd23(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_LPD23;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_LPD23 << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_LPD23 << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_LPD23 << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_LPD23;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_LPD23 << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_LPD23 << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_LPD23 << 24);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenPmwkup
* 功能:	使能中断源PMWKUP，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenPmwkup(U8 Eintsel)
{
	//参数检查
	if(Eintsel >= EINT_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_PMKUP;
	}
	else if( Eintsel == EINT1SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_PMKUP << 8);
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_PMKUP << 16);
	}
	else if( Eintsel == EINT3SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_PMKUP << 24);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_PMKUP;
	}
	else if( Eintsel == EINT5SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_PMKUP << 8);
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_PMKUP << 16);
	}
	else if( Eintsel == EINT7SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_PMKUP << 24);
	}
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenUsb_vboff
* 功能:	使能中断源USB_VBOFF，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUsb_vboff(U8 Eintsel)
{
	//参数检查		
	if(Eintsel >= EINT_NUM
	 	|| Eintsel == EINT1SEL ||  Eintsel == EINT3SEL || Eintsel == EINT5SEL || Eintsel == EINT7SEL)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	
	if( Eintsel == EINT0SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_0SEL))|INTERRUPT_USB_VBOFF;
	}
	else if( Eintsel == EINT2SEL )
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_2SEL))|(INTERRUPT_USB_VBOFF << 16);
	}
	else if( Eintsel == EINT4SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_4SEL))|INTERRUPT_USB_VBOFF;
	}
	else if( Eintsel == EINT6SEL )
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_6SEL))|(INTERRUPT_USB_VBOFF << 16);
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenUsb_vbon
* 功能:	使能中断源USB_VBON，并指定中断通道
* 参数: Eintsel---指定的中断通道
		
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUsb_vbon(U8 Eintsel)
{
	//参数检查
	if (Eintsel >= EINT_NUM
		|| Eintsel == EINT0SEL ||  Eintsel == EINT2SEL || Eintsel == EINT4SEL || Eintsel == EINT6SEL)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if (Eintsel == EINT1SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_1SEL))|(INTERRUPT_USB_VBON << 8);
	}
	else if (Eintsel == EINT3SEL)
	{
		EINTSEL0 = (EINTSEL0 & (~EINTSEL0_3SEL))|(INTERRUPT_USB_VBON << 24);
	}
	else if (Eintsel == EINT5SEL)
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_5SEL))|(INTERRUPT_USB_VBON << 8);
	}
	
	else if (Eintsel == EINT7SEL)
	{
		EINTSEL1 = (EINTSEL1 & (~EINTSEL1_7SEL))|(INTERRUPT_USB_VBON << 24);
	}
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setTcxClkOut0Function
* 功能:	使能TCX clock输出到指定GPIO上，配合CKC   hyhwCpm_setTcxClk 使用
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTcxClkOut0Function(void)
{
	PORTCFG4 = (PORTCFG4 & (~BIT_MASK4)) | BIT16;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setTctClkOut1Function
* 功能:	使能TCT clock输出到指定GPIO上，配合CKC   hyhwCpm_setTcxClk 使用
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTctClkOut1Function(void)
{
	PORTCFG3 = (PORTCFG3 & (~BIT_MASK3)) | BIT12;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_IntenUsb_vbon
* 功能:	使能PWM输出到指定GPIO上
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setPwmFunction(void)
{
	PORTCFG4 = (PORTCFG4 & (~BIT_MASK2)) | BIT9;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setPwmPortA5Function
* 功能:	使能PWM输出到指定GPIOA5上
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setPwmPortA5Function(void)
{
	PORTCFG4 = (PORTCFG4 & (~BIT_MASK1)) | BIT5;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_setAsPwmFunction
* 功能:	设置GPIOA[4/5/6/7]为PWM功能
* 参数: port---端口，只有PORT_A有PWM功能口
*		gpio---GPIO_4(/5/6/7)
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setAsPwmFunction(U32 port, U32 gpio)
{
	if (port != PORT_A) return;
	
	switch (gpio)
	{
		case GPIO_4:
			//占用TIMER3
			PORTCFG4 = (PORTCFG4 & (~BIT_MASK2)) | BIT9;
			break;
		case GPIO_5:
			//占用TIMER2
			PORTCFG4 = (PORTCFG4 & (~BIT_MASK1)) | BIT5;
			break;
		case GPIO_6:
			//占用TIMER1
			PORTCFG7 = (PORTCFG7 & (~BIT_MASK2)) | BIT9;
			break;
		case GPIO_7:
			//占用TIMER0
			PORTCFG7 = (PORTCFG7 & (~BIT_MASK1)) | BIT5;
			break;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_3_3APowerEnable
* 功能:	打开3.3A 电源使能
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_3_3APowerEnable(void)
{
/*
	hyhwGpio_setAsGpio(PORT_E,BIT7);
	hyhwGpio_setOut(PORT_E,BIT7);
	hyhwGpio_setHigh(PORT_E,BIT7);
*/
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwGpio_3_3APowerDisable
* 功能:	关闭3.3A 电源使能
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_3_3APowerDisable(void)
{
/*	hyhwGpio_setAsGpio(PORT_E,BIT7);
	hyhwGpio_setOut(PORT_E,BIT7);
	hyhwGpio_setLow(PORT_E,BIT7);*/
}