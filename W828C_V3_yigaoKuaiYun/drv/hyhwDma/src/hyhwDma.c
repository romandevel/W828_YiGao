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
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
#include "hyhwChip.h"
#include "hyhwDma.h"
#include "hyhwCkc.h"

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Init
* 功能:	初始化DMA 1~8,dma0为lcd_CPU DMA，不加初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwDma_Init(void)
{
	U8 i;
	tDMA_REGISTER *ptDmaReg;
	
	hyhwCpm_busClkEnable( CKC_DMA ); 											// enable the dam clock
		
	for(i=1;i<DMA_TOTAL_CHANNEL_NUM;i++)
	{
		if(i < DMA_CHANNEL_NUM_PER_DMA) 										// channel 1-2
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*i );   
		}	
		else if( i < 2*DMA_CHANNEL_NUM_PER_DMA) 								// channel 3-5							
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(i-DMA_CHANNEL_NUM_PER_DMA) ); 
		}
		else																	// channel 6-8																	
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(i-2*DMA_CHANNEL_NUM_PER_DMA) ); 
		}
		
		ptDmaReg->SOURCE_PARAMETER 		= DMA_PORT_TYPE_WORD; 					// DMA时source 步长为1个字
		ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_WORD; 					// DMA时Destination 步长为1个字
		ptDmaReg->CHANNEL_CONTROL  		=(  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE 	// SW transfer
										  | CHCTRL_BIT_BURST_SIZE_1				// 1 burst transfer include 1 read and 1 write circles
										  | CHCTRL_BIT_WORD_SIZE1_32BIT			// WSIZE, Each cycle read or write 32bit data
										  | CHCTRL_BIT_DMA_DONE_FLAG			// FLAG , Represents that all hop of transfers are fulfilled.
										 );
	}
		
	// channel control register inialize						
	DMA0_CONFIG_REGISTER |= 	CHCONFIG_BIT_CH_PRIORITY_CYCLIC;				// set bit 0 of channel configuration register as fixed priority_mode
	DMA0_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA0_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA0_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
	DMA1_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_CYCLIC;
	DMA1_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA1_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA1_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
	DMA2_CONFIG_REGISTER |= 	CHCONFIG_BIT_CH_PRIORITY_CYCLIC;
	DMA2_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA2_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA2_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Init
* 功能:	关闭DMA电源，以节省用电
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
Void hyhwDma_DeInit(Void)
{
	//关闭DMA时钟
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Config
* 功能:	设置DMA
* 参数:	sa------数据源地址
*		da------数据目的地址
*		size--需要传输的hop数( 32 bit word )
* 返回:	可用的通道号，根据优先级返回最先可用的通道
*		HY_ERR_BUSY:无可用的DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size)
{

	U8 ChannelId;
	tDMA_REGISTER *ptDmaReg;
	
	/* channel 0 被 LCD 占用*/
	for( ChannelId = 1; ChannelId < DMA_TOTAL_CHANNEL_NUM; ChannelId ++ )
	{
		if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
		{
			ptDmaReg  = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
		}	
		else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
		{
			 ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );     
		}
		else
		{
			ptDmaReg  = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );     
		}
		
		if(( ptDmaReg->HOP_COUNT >> 16 ) == 0 )															/* 只能用当前的计数器值判断，不能用中断标志位判断*/
		{ 
			ptDmaReg->SOURCE_START_ADDR 	 = sa;
			ptDmaReg->DESTINATION_START_ADDR = da;
			ptDmaReg->HOP_COUNT				 = size;
			ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
			ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;	
			return ChannelId;
		}
	}	
	return HY_ERR_BUSY;	
}

/*-----------------------------------------------------------------------------
* 函数:	lcdCpuDmaStop
* 功能:	停止lcdCpuDma
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
Void lcdCpuDmaStop(Void)
{
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Enable 
* 功能:	启动DMA
* 参数:	ChannelId--通道号
*		
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_Enable(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->CHANNEL_CONTROL |= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_InterruptSet 
* 功能:	停止DMA
* 参数:	ChannelId--通道号
*	
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_Disable(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Pause 
* 功能:	暂停DMA传输，当再次开始传输时不需要重新配置DMA。调用hyhw_restart函数即可
* 参数:	ChannelId--通道号
*	
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_Pause(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_restart 
* 功能:	暂停DMA传输，当再次开始传输时不需要重新配置DMA。
* 参数:	ChannelId--通道号
*	
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_restart(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_isPause 
* 功能:	判断DMA是否处于暂停状态
* 参数:	ChannelId--通道号
*	
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isPause(U8 ChannelId)
{
	U32 temp;
	tDMA_REGISTER *ptDmaReg;
	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	temp = ptDmaReg->CHANNEL_CONTROL;
	if(temp & CHCTRL_BIT_DMA_START)
	{
		return FALSE;
	}
	
	if(ptDmaReg->HOP_COUNT == 0)
	{
		return FALSE;
	}
	
   	return TRUE;
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_IntClear
* 功能:	清除指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntClear(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_DONE_FLAG;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_IntEnable
* 功能:	使能指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntEnable(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_IntDisable
* 功能:	禁止指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntDisable(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->CHANNEL_CONTROL &= CHCTRL_BIT_DMA_INT_DISABLE;
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_GetStatus
* 功能:	获得指定通道的DMA的状态（空闲/busy）
*			先查看该通道是否启动---可用
*			数据传输完毕-----------可用
*			数据正在传输-----------不可用
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetStatus(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数合法性检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	if( ( ptDmaReg->HOP_COUNT >> 16) == 0 )
	{ 
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return HY_OK;
	}
	else
	{
		return HY_ERR_BUSY;	
	}
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_ParameterSet 
* 功能:	用于设置指定通道的参数，可以设置成循环DMA，DMA每次增加/减小的地址可以设置，
*       地址也可以固定
* 参数:	ChannelId--通道号
*		SourceAddrMask--DMA过程中哪些source地址线固定不变，可以利用此特性进行循环/反复DMA
*		SourceIncreaseStep--DMA过程中source地址的变化设置，8位有符号，bit7表示增/减
*		DestinationAddrMask--DMA过程中哪些Destination地址线固定不变，可以利用此特性进行循环/反复DMA
*		DestinationIncreaseStep--DMA过程中Destination地址的变化设置，8位有符号，bit7表示增/减
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:	ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_ParameterSet(
						 	U8 ChannelId,
						 	U32 SourceAddrMask,
						 	S8 SourceIncreaseStep,
						 	U32 DestinationAddrMask,
						 	S8 DestinationIncreaseStep
						 	)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数合法性检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM ) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->SOURCE_PARAMETER 		= (SourceAddrMask<<4) & (~0xff) + (U8)(SourceIncreaseStep);
	ptDmaReg->DESTINATION_PARAMETER = (DestinationAddrMask<<4) & (~0xff)  + (U8)(DestinationIncreaseStep);
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_InterruptSet 
* 功能:	设置dma中断使能
* 参数:	ChannelId--通道号
*		Enable_en IntCtrl--EnableOff 中断禁止
*						 --EnableOn  中断允许
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId的范围0~5，0～2使用DMA0，3～5使用DMA1,其余为错误参数
*----------------------------------------------------------------------------*/
/*U32 hyhwDma_InterruptSet(U8 ChannelId, eENABLE IntCtrl)
{
	tDMA_REGISTER	*ptDmaReg;
	if(	 ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if(ChannelId >= DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}	
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );   
	}
	if(IntCtrl == EnableOff)
	{
		ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_INT_ENABLE;
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	}
	return HY_OK;
}
*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_InterruptSet 
* 功能:	设置dma中断使能
* 参数:	ChannelId--通道号
*		Enable_en IntCtrl--EnableOff 中断禁止
*						 --EnableOn  中断允许
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note: 	ChannelId的范围0~8，0～2使用DMA0，3～5使用DMA1, 6~~8使用DMA2其余为错误参数
*----------------------------------------------------------------------------*/
U32 hyhwDma_HWModeSet(
							U8 ChannelId,
							eDMAHARDWAREMODULERW_EN ReadWrite,
							eDMAHARDWAREMODULE_EN HardwareType
						)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数合法性检查
	if(ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	if(ReadWrite == HardwareRead)
	{
		ptDmaReg->CHANNEL_CONTROL 	&= ~CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE;
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL 	|= CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE;
	}
	
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE;
	
	ptDmaReg->CHANNEL_CONTROL 		&= ~(0x3 << 8);
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_TRANSFER_TYPE_HARDWARE;
	
	ptDmaReg->EXTERNAL_DMA_REQUEST 	 = 0; 									//clear
	ptDmaReg->EXTERNAL_DMA_REQUEST  |= HardwareType;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart0TxConfig
* 功能:	设置DMA uart0 Tx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0TxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	// 参数检查 确保通道处于可用且空闲状态
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	// 根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 		= sa;
	ptDmaReg->SOURCE_PARAMETER 			= 1;								// address added step is 1, every time when operation is finished ,address += 1;
	ptDmaReg->DESTINATION_START_ADDR 	= 0xf0055000; 						// UART0 THR 发送保持寄存器
	ptDmaReg->DESTINATION_PARAMETER 	= 0;
	ptDmaReg->HOP_COUNT 				= 16;								// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 			= (BIT31|BIT30); 					/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST 		= BIT26;				 			/* UART channel 0 transmitter DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		   &= ~(BIT9|BIT8|BIT5|BIT4); 			/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		   |= (BIT8|BIT1);						/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		   |= (BIT0); 							/* UART的interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		   |= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
	ptDmaReg->CHANNEL_CONTROL 		   |= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart0RxConfig
* 功能:	设置DMA uart0 Rx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0RxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查 确保通道处于可用且空闲状态
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = 0xf0055000;	 						// UART0 RBR 接收缓冲区寄存器;
	ptDmaReg->SOURCE_PARAMETER 		 = 0;									// address added step is 0, every time when operation is finished ,address += 0;
	ptDmaReg->DESTINATION_START_ADDR = sa;
	ptDmaReg->DESTINATION_PARAMETER  = 1;
	ptDmaReg->HOP_COUNT 			 = 16;									// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 		 = (BIT31|BIT30); 						/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT27;				 				/* UART channel 0 receiver DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4); 				/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								/* UART的interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart2TxConfig
* 功能:	设置DMA uart2 Tx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2TxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查 确保通道处于可用且空闲状态
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = sa;
	ptDmaReg->SOURCE_PARAMETER 		 = 1;									 // address added step is 1, every time when operation is finished ,address += 1;
	ptDmaReg->DESTINATION_START_ADDR = 0xf0055200; 	 					 	 // UART2 THR 发送保持寄存器
	ptDmaReg->DESTINATION_PARAMETER  = 0;
	ptDmaReg->HOP_COUNT 			 = 16;									 // or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 	     = (BIT31|BIT30); 	 				 	 /* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT8;				 				 /* UART channel 2 transmitter DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4);				 /* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							 /* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								 /* UART的interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart2RxConfig
* 功能:	设置DMA uart2 Rx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2RxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查 确保通道处于可用且空闲状态
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//根据通道选择控制寄存器
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = 0xf0055200; 							// UART2 RBR 接收缓冲区寄存器
	ptDmaReg->SOURCE_PARAMETER 		 = 0;									// address added step is 0, every time when operation is finished ,address += 0;
	ptDmaReg->DESTINATION_START_ADDR = sa;
	ptDmaReg->DESTINATION_PARAMETER  = 1;
	ptDmaReg->HOP_COUNT 			 = 16;									// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 		 = (BIT31|BIT30); 						/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT9;				 				/* UART channel 2 receiver DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4); 				/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								/* UART的interrupt flag generate DMA requeset */
	
	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* 清中断,使能中断 */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_lcdCpuConfig
* 功能:	设置lcd 屏DMA
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
Void hyhwDma_lcdCpuConfig(eLCDBUSTYPE_EN  busType)
{
	
	tDMA_REGISTER *ptDmaReg;
	U32 buffAddr;
	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
//	buffAddr = ~(((262144)-1)<<4);
//	buffAddr = ~(((1048576)-1)<<4);	//1M
//	ptDmaReg->SOURCE_PARAMETER	 = ((0xffffff00 & buffAddr) | DMA_PORT_TYPE_HALF_WORD);
	ptDmaReg->SOURCE_PARAMETER	  	= DMA_PORT_TYPE_HALF_WORD;

	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_STATIC; 				/* DMA时Destination 步长为0 */	
		
	if( busType == LcdBusType16 )
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
					//				   | CHCTRL_BIT_REPEAT_MODE_ENABLE
					//				   | CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									  );
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE1_32BIT	
									  );
	}
	
//	ptDmaReg->REPEAT_CONTROL		= 0;
	ptDmaReg->DESTINATION_START_ADDR= LCD_CPU_INTERFACE_ID;

}

/*-----------------------------------------------------------------------------
* 函数:	lcdCpuEnable
* 功能:	使能lcd 屏DMA
* 参数:	sa		--	数据源地址
*		size	--	需要传输的半字数
* 返回:	none		
*----------------------------------------------------------------------------*/
U32 lcdCpuEnable(U16 * pdata, U32 size)
{
	tDMA_REGISTER *ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_START_ADDR = ( U32 )pdata;

	// 应用中传入的是半字的个数, 因此是右移2位
	ptDmaReg->HOP_COUNT			= size>>2;
	
	/* 
	  送图像时, 每次都是一帧, 不需要重复模式, 也不用处理中断
	  因此这里不用清除中断
	*/ 
	//ptDmaReg->CHANNEL_CONTROL  |= CHCTRL_BIT_DMA_INT_ENABLE;
	
	ptDmaReg->CHANNEL_CONTROL  |= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	lcdCpuDmaOK
* 功能:	判断LCD DMA是否完成，如果完成则清中断，返回TRUE，否则返回FAULSE
* 参数:	none
* 返回:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL lcdCpuDmaOK(Void)
{
	tDMA_REGISTER *ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	if((ptDmaReg->HOP_COUNT >> 16) == 0)
	{
		/* 
		  送图像时, 每次都是一帧, 不需要重复模式, 也不用处理中断
		  因此这里不用清除中断
		*/   
	//	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
		
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_isDmaInterruptSet
* 功能:	判断是否中断使能
* 参数:	ChannelId
*		ChannelId--通道号
* 返回:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId)
{
	
	tDMA_REGISTER *ptDmaReg;
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}

	if( ptDmaReg->CHANNEL_CONTROL & CHCTRL_BIT_DMA_DONE_FLAG )
	{
		//ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaConfig
* 功能:	设置DMA DAI DMA 使能传输, 指定固定通道DAI_DMA_REGISTER_ADDR
*		该函数和下一个函数均指定特定通道用于DMA传输, 以前的函数太过麻烦（上面3个)
*		该函数由杨健建议,后来改动主要是基于和codec模块左右通道的搭配，配置2个DMA
*		,在高文中帮助下实现, 后又重新添加了录音DMA通道
* 参数:	daiTransferType------指定传输类型, 可选值(0,1): 0表示发送， 1 表示接收
*		size------缓冲区大小, 用于指定DMA传输的缓冲区大小,该缓冲区设置环形区域
*		pBufLeftAddr---------音频缓冲区左声道指针，即本次DMA传输的首地址
*		pBufRightAddr--------音频缓冲区右声道指针，即本次DMA传输的首地址
*		sampleCount----------采样子个数，该参数决定HOPCOUNT值
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
							U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount)
{
	if(daiTransferType == DaiTxType)								// DAI DMA发送
	{
		tDMA_REGISTER *ptDmaRegLeft;
		tDMA_REGISTER *ptDmaRegRight;
		ptDmaRegLeft  = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaRegRight = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		
		
		/* 因左右两个缓冲区传输的同步，这里中断只使能一个DMA通道 */
		ptDmaRegLeft->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
											| CHCTRL_BIT_DMA_INT_ENABLE
										    );
										   
		ptDmaRegRight->CHANNEL_CONTROL    = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
										    );
										   								   
		/* 
			Hopcount计算方法： sampleCount(以字节计)/(burstsize*bitWidth/8) 
			1 sampleCount == 2 bytes, burstsize = 4, bitWidth = 16 bits 
			Hopcount = sampleCount/16
		*/								   
		ptDmaRegLeft->HOP_COUNT				  = (sampleCount >> 3);
		ptDmaRegRight->HOP_COUNT			  = (sampleCount >> 3);
	
	 // left :
	 	// 缓冲区源地址的配置, 以及DMA源地址参数寄存器配置
	 	ptDmaRegLeft->SOURCE_START_ADDR  	  = (U32)pBufLeftAddr;
		ptDmaRegLeft->SOURCE_PARAMETER 		  = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD; 
		
		ptDmaRegLeft->DESTINATION_START_ADDR  = DAI_INTERFACE_IDO;
		ptDmaRegLeft->DESTINATION_PARAMETER   = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
		
		ptDmaRegLeft->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_DAITX;
		
	 // right:	
	 	ptDmaRegRight->SOURCE_START_ADDR  	  = (U32)pBufRightAddr;
	 	ptDmaRegRight->SOURCE_PARAMETER 	  = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;  
		
		ptDmaRegRight->DESTINATION_START_ADDR = DAI_INTERFACE_IDOR;
		ptDmaRegRight->DESTINATION_PARAMETER  = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
		
		ptDmaRegRight->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_DAITX;
	}
	else if(daiTransferType == DaiRxType)					 // DAI DMA接收, 录音情况
	/* 因为在打电话时必须同时进行录音和播放，因此修改代码为录音程序单独加一个DMA通道负责录音 */
	{
		tDMA_REGISTER *ptDmaRegRecord;
		ptDmaRegRecord = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaRegRecord->CHANNEL_CONTROL   = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
											| CHCTRL_BIT_DMA_INT_ENABLE
										    );
		ptDmaRegRecord->HOP_COUNT			    = (sampleCount >> 3);

		/*if (0!=Test_TellingChl(0) || 0!=Test_TellingChl(1))
		{   
			// 打电话状态下 该函数基于打电话时有可能插拔耳机, 需要修改这部分代码 20090910, yanglijing
			if(skr_soft_detect_earphone() == 1)//耳机mic输入时
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDIR;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufRightAddr;
				
			}
			else // 不带耳机时
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			}
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}
		else*/
		{
			// 单独录音时, 该函数基于打电话时有可能插拔耳机, 需要修改这部分代码 20090910, yanglijing
			ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
			ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}	
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaConfig
* 功能:	设置DMA DAI DMA 使能传输, 指定固定通道DAI_DMA_REGISTER_ADDR
*		该函数和下一个函数均指定特定通道用于DMA传输, 以前的函数太过麻烦（上面3个)
*		该函数由杨健建议,后来改动主要是基于和codec模块左右通道的搭配，配置2个DMA
*		,在高文中帮助下实现, 后又重新添加了录音DMA通道
* 参数:	daiTransferType------指定传输类型, 可选值(0,1): 0表示发送， 1 表示接收
*		size------缓冲区大小, 用于指定DMA传输的缓冲区大小,该缓冲区设置环形区域
*		pBufLeftAddr---------音频缓冲区左声道指针，即本次DMA传输的首地址
*		pBufRightAddr--------音频缓冲区右声道指针，即本次DMA传输的首地址
*		sampleCount----------采样子个数，该参数决定HOPCOUNT值
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaReConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
							U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount)
{
	if(daiTransferType == DaiTxType)								// DAI DMA发送
	{
		tDMA_REGISTER *ptDmaRegLeft;
		tDMA_REGISTER *ptDmaRegRight;
		ptDmaRegLeft  = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaRegRight = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
						   
		ptDmaRegLeft->HOP_COUNT				  = (sampleCount >> 3);
		ptDmaRegRight->HOP_COUNT			  = (sampleCount >> 3);
	}
	else
	{
		tDMA_REGISTER *ptDmaRegRecord;
		ptDmaRegRecord = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		
		ptDmaRegRecord->HOP_COUNT			    = (sampleCount >> 3);

		/*if (0!=Test_TellingChl(0) || 0!=Test_TellingChl(1))
		{   
			// 打电话状态下 该函数基于打电话时有可能插拔耳机, 需要修改这部分代码 20090910, yanglijing
			if(skr_soft_detect_earphone() == 1)//耳机mic输入时
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDIR;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufRightAddr;
				
			}
			else // 不带耳机时
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			}
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}*/
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaEnable
* 功能:	启动DAI DMA的DMA传输
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaEnable(eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER *ptDmaReg;
	if(daiTransferType == DaiTxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;

		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	}
	else
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	}	
}

/*-----------------------------------------------------------------------------
* 函数:	isDaiDmaDmaOK
* 功能:	判断dai DMA是否完成，如果完成则清中断，返回TRUE，否则返回FAULSE
* 参数:	none
* 返回:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL isDaiDmaDmaOK(eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER	*ptDmaReg;
	if(daiTransferType == DaiTxType)
	{	
		ptDmaReg = (tDMA_REGISTER *)DAI_DMA_REGISTER_ADDR;
		
		if(  ( ptDmaReg->HOP_COUNT >> 16) == 0  )
		{
			ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}
	else if(daiTransferType == DaiRxType)
	{
		ptDmaReg = (tDMA_REGISTER *)DAI_DMA_RECORD_REGISTER_ADDR;
		
		if(  ( ptDmaReg->HOP_COUNT >> 16) == 0  )
		{
			ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwDai_setTxSampleCount
* 功能:	 
* 参数:	sampleCount				--  Tx sample count 
* 返回:	HY_OK					--	成功
		HY_ERR_BAD_PARAMETER 	--	输入参数错误
*----------------------------------------------------------------------------*/
U32 hyhwDma_SetDaiSampleCount(U32 sampleCount, eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER *ptDmaReg;
	
	if((sampleCount > CODEC_BUFF_MAX)||(sampleCount < CODEC_BUFF_MIN))	
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if(daiTransferType == DaiTxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03);
		
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03);
	}	
	else if(daiTransferType == DaiRxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03); 
	}
	
	
	return HY_OK;	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaConfig	added by yanglijing, 2009-08-05
* 功能:	设置DMA SPI DMA 使能传输, 指定固定通道SPI_DMA_REGISTER_ADDR
* 参数:	daiTransferType------指定传输类型, 可选值(0,1): 0表示发送， 1 表示接收
*		size-----------------缓冲区大小
*		pBufAddr-------------DMA传输首地址
*		count----------------传输中断数值, 该参数决定HOPCOUNT值, 字节表示
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaConfig(eSPI_TRANS_TYPE TransferType, U32 size, U32 *pBufAddr, U32 Count)
{
	tDMA_REGISTER *ptDmaReg;
	
	ptDmaReg = ( tDMA_REGISTER * )SPI_DMA_REGISTER_ADDR;
	
	/* SPI DMA 控制寄存器配置 */
	ptDmaReg->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
									| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
									| CHCTRL_BIT_REPEAT_MODE_ENABLE
									| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
									| CHCTRL_BIT_BURST_SIZE_4
									| CHCTRL_BIT_WORD_SIZE1_32BIT
									| CHCTRL_BIT_DMA_INT_ENABLE
									);
									   
	/* 
		注释: SPI 传输宽度在系统设置为32位, 缓冲区深度为4, 也就是DMA一跳传输的最大字节数
			是16, 为了同步这里的寄存器配置成: 跳宽度为4, 位宽32bit.
	/* 
		Hopcount计算方法： count 值这里指字节数, 即以此DMA中断传输的字节数, hopCount = count / 16
	*/								   
	ptDmaReg->HOP_COUNT				  	  = (Count >> 4);
	
	if(TransferType == TxType)						// SPI DMA发送
	{
		/*  // 传输方向, 吧要写入到发送寄存器的值（内存地址写到寄存器
			发送源地址应当是传入的缓冲区首地址, 该值按照位宽来存储, 32Bit系统里面
			32Bit数据存储速度会比较快, 和SPI设置的位宽一致
		*/	
		ptDmaReg->SOURCE_START_ADDR       = (U32)pBufAddr;
		ptDmaReg->SOURCE_PARAMETER        = ((~((size-1)<<4))&0xffffff00) |DMA_PORT_TYPE_WORD;
		
		/*
	 		发送目的地址应当是SPI的数据寄存器, 所以偏移量为DMA_PORT_TYPE_STATIC
	 		DMA 传输时源地址并不发生变化 
	 	*/	
	 	ptDmaReg->DESTINATION_START_ADDR  = (U32)SPI_DMA_TX_DST_REGISTER;
		ptDmaReg->DESTINATION_PARAMETER   = DMA_PORT_TYPE_STATIC;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_GPSB0TX;
		
	 
	}
	else if(TransferType == RxType)					 // SPI DMA接收
	{
		/*
	 		接收时源地址应当是SPI的数据寄存器, 所以偏移量为DMA_PORT_TYPE_STATIC
	 		DMA 传输时源地址并不发生变化 
	 	*/	
		ptDmaReg->SOURCE_START_ADDR       = SPI_DMA_RX_SRC_REGISTER;
		ptDmaReg->SOURCE_PARAMETER        = DMA_PORT_TYPE_STATIC;
		
		/*
			接收时目的地址应当是传入的缓冲区首地址, 该值按照位宽来存储, 32Bit系统里面
			32Bit数据存储速度会比较快, 和SPI设置的位宽一致
		*/	
	 	ptDmaReg->DESTINATION_START_ADDR  = (U32)pBufAddr;
	 	ptDmaReg->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00) |DMA_PORT_TYPE_WORD;
	 	
	 	
		
		ptDmaReg->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_GPSB0RX;
	}	
}							

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_SpiDmaEnable
* 功能:	启动SPI DMA的DMA传输
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaEnable()
{
	tDMA_REGISTER *ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )SPI_DMA_REGISTER_ADDR;
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
}


/*---------------------------------------------------------------------------->
added start by yanglijing , 20090806
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart1Config
* 功能:	设置DMA uart1 Tx DMA 使能传输, 用于TD
* 参数:	transType---------------DMA传输方向, 可选参数TxType/RxType
*		buffsize----------------环形缓冲区大小, 以字节计
*		sourceAddr--------------数据源地址地址
*		bytesCount--------------DMA一次中断的字节数
* 返回: none
* 用例: hyhwDma_uart1Confit(TxType, 512, 0x2010000, 128)
*		配置串口DMA通道参数为环形缓冲区大小512字节，发送模式，每128字节一次DMA中断
*		按照串口部分进行了修改，BSIZE 改为ile1，即每次DMA请求传1个字节
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1Config(e_TRANS_TYPE transType, U32 buffsize, U32 sourceAddr, U32 bytesCount)
{
	tDMA_REGISTER *ptDmaReg;
	
	ptDmaReg = UART1_DMA_REGISTER_ADDR;
	
	/* UART1 DMA 控制寄存器配置 */
	ptDmaReg->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									| CHCTRL_BIT_TRANSFER_TYPE_HARDWARE//CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGECHCTRL_BIT_TRANSFER_TYPE_HARDWARE//|CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
									| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
									| CHCTRL_BIT_REPEAT_MODE_ENABLE
									| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
									| CHCTRL_BIT_BURST_SIZE_1
									| CHCTRL_BIT_WORD_SIZE_8BIT
									| CHCTRL_BIT_DMA_INT_ENABLE
									);
									   
	/* 
		注释: UART1 传输宽度在系统设置为8位, 缓冲区深度为16, 但一跳的最大8个读写周期, 因此一次最多传输
			是8个字节
	/* 
		Hopcount计算方法： count 值这里指字节数, 即以此DMA中断传输的字节数, hopCount = count / 8
	*/								   
	ptDmaReg->HOP_COUNT				  	  = (1);
	ptDmaReg->REPEAT_CONTROL              = (  bytesCount 
	                                         | RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE
	                                         | RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE
	                                        );
	if(transType == TxType)						// SPI DMA发送
	{
	 	/*	// 传输方向, 吧要写入到发送寄存器的值（内存地址写到寄存器
			发送源地址应当是传入的缓冲区首地址, 该值按照位宽来存储, UART寄存器宽度为8bit
	 	*/	
	 	ptDmaReg->SOURCE_START_ADDR  	  = (U32)sourceAddr;
		ptDmaReg->SOURCE_PARAMETER 		  = ((~((buffsize-1)<<4))&0xffffff00) |DMA_PORT_TYPE_BYTE;
		
		/*
			发送目的地址应当UART的THT数据寄存器, 所以偏移量为DMA_PORT_TYPE_STATIC
	 		DMA 传输时源地址并不发生变化 
		*/	
		ptDmaReg->DESTINATION_START_ADDR  = (U32)UART1_DMA_TX_DST_REGISTER;
		ptDmaReg->DESTINATION_PARAMETER   = DMA_PORT_TYPE_STATIC;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_UART1TX;
		
	 
	}
	else if(transType == RxType)					 // SPI DMA接收
	{
		/*
			接收源地址应当UART的RBR数据寄存器, 所以偏移量为DMA_PORT_TYPE_STATIC
	 		DMA 传输时源地址并不发生变化
		*/	
	 	ptDmaReg->SOURCE_START_ADDR  	  = (U32)UART1_DMA_RX_SRC_REGISTER;
	 	ptDmaReg->SOURCE_PARAMETER   	  = DMA_PORT_TYPE_STATIC;
	 		
	 	/*
	 		接收时目的地址应当是传入的缓冲区首地址, 该值按照位宽来存储, UART寄存器宽度为8bit
	 	*/	
		ptDmaReg->DESTINATION_START_ADDR  = (U32)sourceAddr;
		ptDmaReg->DESTINATION_PARAMETER   = ((~((buffsize-1)<<4))&0xffffff00) |DMA_PORT_TYPE_BYTE;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_UART1RX;
		
	}	
}
/*added end, by yanglijing , 20090806
<-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_SpiDmaEnable
* 功能:	启动UART1 DMA的DMA传输
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1DmaEnable()
{
	tDMA_REGISTER *ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )UART1_DMA_REGISTER_ADDR;
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_GetCurrentSrcAddr 
* 功能:	查询当前DMA通道当前传输的源地址
* 参数:	ChannelId--通道号
* 返回:	当前DMA传输源地址值/ 或者HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentSrcAddr(U32 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	return ptDmaReg->SOURCE_CURRENT_ADDR;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_GetCurrentDstAddr 
* 功能:	查询当前DMA通道当前传输目的地址
* 参数:	ChannelId--通道号
* 返回:	当前DMA传输目的地址值
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentDstAddr(U32 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//参数检查
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	return ptDmaReg->DESTINATION_CURRENT_ADDR;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwLcd_RGBEnable
* 功能:	Setting the  800*480 color image,
* 参数:	* color	--  the data point
* 返回:	none		
*----------------------------------------------------------------------------*/
U32 hyhwLcd_RGBEnable( U32 sorceAddr, U32 destAddr, U32 size )
{
	tDMA_REGISTER	*ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_PARAMETER = DMA_PORT_TYPE_HALF_WORD; 	/*DMA时source 步长为1*/
	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_HALF_WORD; /*DMA时Destination 步长为0*/		
	
	ptDmaReg->SOURCE_START_ADDR = ( U32 )sorceAddr;
	ptDmaReg->DESTINATION_START_ADDR = ( U32 )destAddr;
	
	ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
									  );
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	ptDmaReg->HOP_COUNT	= (size>>1);
	
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	lcdCpuDmaOK
* 功能:	判断RGB DMA是否完成，如果完成则清中断，返回TRUE，否则返回FAULSE
* 参数:	none
* 返回:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL hyhwLcd_RGBDmaOK( void )
{
	tDMA_REGISTER	*ptDmaReg;
	( U32 * )ptDmaReg = ( U32 * )LCD_DMA_REGISTER_ADDR;
	if(( ptDmaReg->HOP_COUNT >> 16) == 0 )
	{
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
*	修改记录 1 :
2009 08 05	杨立敬
问题:
	通过SPI传输启动数据时, 大概需要3s的时间, 这个时间用DMA传输一是可以提高速度, 二可以
	CPU可以做其他事情, 不用等待传输的完成. 按照高文中要求更改添加
处理:
	在hyhwDma文件夹模块中添加了两个函数hyhwDma_SpiDmaConfig 和 hyhwDma_SpiDmaEnable
	, 一个是用于SPI　DMA配置的, 一个用来使能DMA传输, 并添加了几个宏定义, 在hyhwDma.h文件
修改位置:
	1\hyhwDma.h 头文件中的添加标记20090805, 主要是宏定义, 和 函数声明
		hyhwDma_SpiDmaConfig 和 hyhwDma_SpiDmaEnable
	2\hyhwDma.c hyhwDma_SpiDmaConfig 和 hyhwDma_SpiDmaEnable 函数定义
	
// 删除了以前保留的已经用#if 0 注掉的函数定义, 	
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
*	修改记录 2 :
2009 08 06	杨立敬
问题:
	需要配置UART1为串口流控DMA传输, 用于TD数据的传输，不是很明白, 3M串口很少用
	uart1 3M,使用DMA接收，同时设置uart 接收1个byte中断方式，中断服务程序中停止uart中断
	，并启动一个timer定时中断(暂定5ms)，产生定时中断时，读取DMA当前的写位置，将DMA已经接收到的数据copy出来
	，并重新配置DMA，停止该timer。如果产生DAM中断，则将已启动的timer停掉，并重新启动1byte的uart中断。DMA配
	置可暂为：8k bytes内存循环使用，2k产生中断。
处理:
	在hyhwDma文件夹模块中添加了两个函数hyhwDma_Uart1Config 和 hyhwDma_Uart1DmaEnable
	, 一个是用于UART1　DMA配置的, 一个用来使能DMA传输, 并添加了几个宏定义, 在hyhwDma.h文件
修改位置:
	1\hyhwDma.h 函数声明
		hyhwDma_Uart1Config 和 hyhwDma_Uart1DmaEnable
	2\hyhwDma.c hyhwDma_Uart1Config 和 hyhwDma_Uart1DmaEnable 函数定义
*//*-----------------------------------------------------------------------------
*	修改记录 3 :
2009 08 19	杨立敬
问题:
	需要让DMA暂停，但不用重新配置DMA，可从原来被暂停的位置恢复运行
处理:
	
修改位置:
	头文件中增加函数原型声明, 源文件中添加函数定义
	U32 hyhwDma_Pause(U8 ChannelId)
	U32 hyhwDma_restart(U8 ChannelId)
	BOOL hyhwDma_isPause(U8 ChannelId)
	这三个函数为新添加的, 当hyhwDma_isPause返回TRUE时，调用hyhwDma_restart这个函数。
-------------------------------------------------------------------------------*//* 

*/