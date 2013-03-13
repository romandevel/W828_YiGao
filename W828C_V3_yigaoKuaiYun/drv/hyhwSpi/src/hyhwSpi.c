

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwSpi.h"
#include "hyhwGpio.h"
#include "hyhwCkc.h"
//#include "gsmd.h"
//#define spiMast

/* tg3 GPSB 映射说明 
   TDBOOT SPI 接口, GPSB port7, 用到DMA, 映射channle 0
   TD PCM传输, GPSB通道 port8, 不用DMA, 映射到channel 3

   GSB PCM传输, GPSB通道 port6, 不用DMA, 映射到channel 2
*/

/*----------------------------------------------------------------------------
   macro defines:
-----------------------------------------------------------------------------*/

#define SPI_FIFO_DEEP_8BIT		(8)
#define SPI_FIFO_DEEP_16BIT		(8)
#define SPI_FIFO_DEEP_32BIT		(4)

#define SPI_TX_TIMEOUT			(10000000)

// DMA2 通道0, 即channel 6 作为SPI的DMA传输
#define SPI_DMA_CHANNEL			6

/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaEnable 
* 功能:	使能 Spi Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaEnable()
{
	return hyhwDma_Enable(SPI_DMA_CHANNEL);	
}

/*--------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaDisable 
* 功能:	disable Spi Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaDisable()
{
	return hyhwDma_Disable(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 函数:	hyhwSpi_DmaIntClear
* 功能:	清除指定通道的DMA中断
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwSpi_DmaIntClear()
{
	return hyhwDma_IntClear(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaIntEnable()	
{
	return hyhwDma_IntEnable(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaIntDisable()
{
	return hyhwDma_IntDisable(SPI_DMA_CHANNEL);	
}
	
/*---------------------------------------------------------------------------
* 函数:	hyhwSpi_DmacheckInterruptState
* 功能:	判断是否中断使能
* 参数:	none
* 返回:	True 代表已经中断, FALSE 代表未中断
*--------------------------------------------------------------------------*/
BOOL hyhwSpi_DmacheckInterruptState()
{
	return hyhwDma_isDmaInterruptSet(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 函数:	hyhwSpi_DmaGetStatus
* 功能:	获得指定通道的DMA的状态（空闲/busy）
*			先查看该通道是否启动---可用
*			数据传输完毕-----------可用
*			数据正在传输-----------不可用
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwSpi_DmaGetStatus()
{
	return hyhwDma_GetStatus(SPI_DMA_CHANNEL);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_clearRXFifoCounter
* 功能:	清空接收缓冲区
* 参数:	Channel 选择映射的GPSB 通道
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	ptSpiReg->MODE &= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_clearRXFifoCounter1
* 功能:	清空接收缓冲区
* 参数:	Channel 选择映射的GPSB 通道
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter1(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	ptSpiReg->MODE |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_init
* 功能:	Map GPSB channel to numSPI 
* 参数:	Channel 选择映射的GPSB 通道
*		numSPI	选择映射的SPI端口序号	
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*		HY_ERR_BYSY	所用通道或端口忙
* 示例：channel 0 Remap as SPI7: hyhwSpi_init(0,7)
*------------------------------------------------------------------------------*/
U32 hyhwSpi_init( U32 Channel, U32 numSPI )
{
	//tSPI_REGISTER *ptSpiReg;
	U32 temp;
	U32 reg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	reg = SPI_CONFIG_REGISTER;
	switch(Channel)	//enable the clock
	{
		case 0:
			hyhwCpm_busClkEnable(CKC_GPSB0);
			hyhwCpm_setGpsb0Clk();			
			break;
		case 1:
			hyhwCpm_busClkEnable(CKC_GPSB1);
			hyhwCpm_setGpsb1Clk();			
			break;
		case 2:
			hyhwCpm_busClkEnable(CKC_GPSB2);
			hyhwCpm_setGpsb2Clk();			
			break;
		case 3:
			hyhwCpm_busClkEnable(CKC_GPSB3);
			hyhwCpm_setGpsb3Clk();			
			break;
		default:
			break;	
	}
	
	switch(numSPI) /* 设置GPIO为功能引脚 */
	{
		case 0:
			hyhwGpio_setGpsb0Function();		
			break;
		case 1:
			hyhwGpio_setGpsb1Function();		
			break;
		case 2:
			hyhwGpio_setGpsb2Function();		
			break;
		case 3:
			hyhwGpio_setGpsb3Function();		
			break;
		case 4:
			hyhwGpio_setGpsb4Function();		
			break;
		case 5:
			hyhwGpio_setGpsb5Function();		
			break;
		case 6:
			hyhwGpio_setGpsb6Function();		
			break;
		case 7:
			hyhwGpio_setGpsb7Function();		
			break;
		case 8:
			hyhwGpio_setGpsb8Function();		
			break;
		case 9:
			hyhwGpio_setGpsb9Function();		
			break;
		case 10:
			hyhwGpio_setGpsb10Function();		
			break;
		default:
			break;	
	}
	
	temp = numSPI << 8*Channel; // 配置端口寄存器到通道的映射
	reg &= ~(0xff << 8*Channel);
	SPI_CONFIG_REGISTER = 	reg | temp;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Master()
* 功能:	set master spi mode 
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	HY_OK 正确
*		HY_ERR_BYSY	所用通道或端口忙
* 修改：添加了bitWidth参数，用于设置位宽， 使用时需要注意 2009.07.15
		可选8/16/32位宽, 用宏定义取代了BIT16等标示
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Master( U32 Channel, U32 divider, U32 recovTime, 
					U32 holdTIme, U32 SetupTime, U32 bitWidth)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//检查参数
	if(divider > 255)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );   

	spiRegModeValue  = ptSpiReg->MODE;
	// master mode
	spiRegModeValue &= ~SPI_MODE_BIT_SLV_SLAVE_MODE;	
		
	// set master relative time, 此处默认清零
	spiRegModeValue &= ~( SPI_MODE_BIT_TRE_TIME_RECOVERY
						|SPI_MODE_BIT_THL_TIME_HOLD
						|SPI_MODE_BIT_TSU_TIME_SETUP
						);								
	
	// SPI 收发数据极性在SCK的上升沿,只在master 模式设置
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV // 
					   |SPI_MODE_BIT_PCK_POLARITY_PCK //SCK 的空闲为高电平
					  ); 								
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );											
	
	spiRegModeValue &= ~SPI_MODE_BIT_SD_SHIFT_DIRECTION;				// shif left
	spiRegModeValue &= ~SPI_MODE_BIT_CTF_CONT_TRANSFER_ENABLE;		// CTF single mode
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;				// operation enable
	
	// 设置SPI模式 bit1| bit0,  SPI compatible 模式			
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;						
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// 设置SPI 通信位宽
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;		
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);		
	
	// set 时钟分频数 DIVLDV
	spiRegModeValue &= ~SPI_MODE_BITMASK_CLK_DIVIDER;					
	spiRegModeValue |= divider << SPI_MODE_OFFSITE_CLK_DIVIDER;
	
	ptSpiReg->MODE   = spiRegModeValue;
	
	
	return HY_OK;
}

void enableSpi(U32 Channel,U8 flag)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );   

	spiRegModeValue  = ptSpiReg->MODE;
	
	if(flag)
	{
		spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_EN_OPERATION_ENABLE;
	}
	ptSpiReg->MODE = spiRegModeValue;
	
	return ;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Slaver()
* 功能:	set slave spi mode 
* 参数:	Channel 选择映射的GPSB 通道
*		bitWidth----数据宽度(8/16/32)
*		sendEnable--是否允许发送(0---禁止发送，1---允许发送)
* 返回:	HY_OK 正确
*		HY_ERR_BYSY	所用通道或端口忙
* 修改：添加了bitWidth参数，用于设置位宽， 使用时需要注意 2009.07.15
		可选8/16/32位宽， 用宏定义取代了BIT16等标示
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Slaver( U32 Channel, U32 bitWidth, int sendEnable)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	spiRegModeValue = ptSpiReg->MODE;
	// slave mode
	spiRegModeValue |= SPI_MODE_BIT_SLV_SLAVE_MODE;

	// operation enable
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	
	// 设置SPI模式 bit1| bit0,  SPI compatible 模式
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );
	// slave mod SDO output disable
	if (sendEnable == 0)
	{
		spiRegModeValue |= SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	// SPI 收发数据极性在SCK的上升沿,只在master 模式设置
	/*
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV
					   |SPI_MODE_BIT_PCK_POLARITY_PCK
					  ); 								
	*/ //yanglijing 注释掉
	//#define TIMING1
	#ifdef TIMING1 
	spiRegModeValue &= ~SPI_MODE_BIT_PCK_POLARITY_PCK; // 上降沿采样数据
	
	#else
	spiRegModeValue |= SPI_MODE_BIT_PCK_POLARITY_PCK;  // 下升沿采样数据
	#endif
	// 设置SPI 通信位宽
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);
	
	#if 0
	ptSpiReg->MODE = spiRegModeValue
					|SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
					|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;
	#endif				
	ptSpiReg->MODE = spiRegModeValue;
	//ptSpiReg->MODE = 0x0007DF04;
	//	ptSpiReg->MODE = 0x00071F0C;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Slaver()
* 功能:	set slave spi mode 
* 参数:	Channel 选择映射的GPSB 通道
*		bitWidth----数据宽度(8/16/32)
*		sendEnable--是否允许发送(0---禁止发送，1---允许发送)
* 返回:	HY_OK 正确
*		HY_ERR_BYSY	所用通道或端口忙
* 修改：这个函数增加主要是为了调试CHannel 3, PORT 8
		设置成SSP模式后程序可以收发32bit数据, 但是channel 0, port 6不可以用这种模式，因此另外加一个函数。
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Slaver_setSSP( U32 Channel, U32 bitWidth, int sendEnable)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	spiRegModeValue = ptSpiReg->MODE;
	// slave mode
	spiRegModeValue |= SPI_MODE_BIT_SLV_SLAVE_MODE;

	// operation enable
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	
	// 设置SPI模式 bit1| bit0,  SPI compatible 模式
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );
	// slave mod SDO output disable
	if (sendEnable == 0)
	{
		spiRegModeValue |= SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	// SPI 收发数据极性在SCK的上升沿,只在master 模式设置
	#if 0
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV
					   |SPI_MODE_BIT_PCK_POLARITY_PCK
					  ); 								
	#else
	//#define TIMING1
	#ifdef TIMING1 
	spiRegModeValue &= ~SPI_MODE_BIT_PCK_POLARITY_PCK; // 上降沿采样数据
	
	#else
	spiRegModeValue |= SPI_MODE_BIT_PCK_POLARITY_PCK;  // 下升沿采样数据
	#endif//#ifdef TIMING1
	#endif
	// 设置SPI 通信位宽
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);
	
	// 设置SSP 模式
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SSP;
	
	#if 0
	ptSpiReg->MODE = spiRegModeValue
					|SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
					|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;
	#endif
	ptSpiReg->MODE = spiRegModeValue;
	//ptSpiReg->MODE = 0x0007DF04;
	//	ptSpiReg->MODE = 0x00071F0C;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Wait_TX_End()
* 功能:	判断是否等待超时
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER	参数错误
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_TX_End( U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );    
	status	 = ptSpiReg->STAT;
	
	while((status & BIT3)==0)
	{
		i++;
		if( i > SPI_TX_TIMEOUT )
		{
		  	return HY_ERR_TIMEOUT;
		}
		status	 = ptSpiReg->STAT;
	}
	
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Wait_RX_DataNum()
* 功能:	判断是否等待超时
* 参数:	dataNum 指定接收多少个字节的数据
*		Channel 选择映射的GPSB 通道	
*		deep	FIFO 深度
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER	参数错误
* 注释: 该函数是由于在测试中发现发送并接收8个数据以后，第9个数据并不是想得到的数据
*		原因可能是数据发送和接受速率的匹配，该问题由高文中修改。
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_RX_DataNum(U32 dataNum, U32 Channel, U32 deep)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel ); 
	status	 = ptSpiReg->STAT;
	
#if 0
	if (dataNum >= deep)
	{
		/*	当需要等待的数据个数大于等于FIFO深度时，
			直接判断接收full位
		*/
		while((status & BIT4)==0)
		{
			i++;
			if( i > SPI_TX_TIMEOUT )
			{
			  	return HY_ERR_TIMEOUT;
			}  	
		}
	}
#else
	if (dataNum > deep)
	{
		/*	当需要等待的数据个数大于等于FIFO深度时，
			返回错误
		*/
		return HY_ERROR;
	}
#endif
	else
	{
		/*	等待接收够指定数据个数，直到超时 */
		while((status & (0x0F<<16)) < (dataNum<<16))
		//while(((CPSB_STATUS>>16)&0x0F) < dataNum)
		{
			i++;
			if( i > SPI_TX_TIMEOUT )
			{
			  	return HY_ERR_TIMEOUT;
			} 	
		}
	}
	
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Wait_RX_End()
* 功能:	判断是否等待超时
* 参数:	Channel 选择映射的GPSB 通道		
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER	参数错误
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_RX_End(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	status	 = ptSpiReg->STAT;    
	
	while((status & BIT2)==0)
	{
		i++;
		if( i > SPI_TX_TIMEOUT )
		{
		  	return HY_ERR_TIMEOUT;
		}
		status	 = ptSpiReg->STAT;
	}
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_SendData()
* 功能:	发送数据
* 参数:	Channel 选择映射的GPSB 通道	
*		pdata 缓冲区
*		len   数据字节数
*		depp  fifo深度
*		bitDeep 位宽，取值8,16,32, 分别代表8/16/32位宽
* 返回:	正确返回接收到的数据
*		HY_ERR_BAD_PARAMETER	参数错误
*------------------------------------------------------------------------------*/
U32 hyhwSpi_SendData(U32 Channel, void* pdata, U32 len, U32 bitDeep)
{
	U32 i,j,k;
	tSPI_REGISTER *ptSpiReg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	
	//等待发送完成
	hyhwSpi_Wait_TX_End(Channel);

	j=0;
	
	//8 位宽度，即字节收发
	if(bitDeep == 8)		
	{
		U8 *pU8SpiData = (U8 *)pdata;
		//enableSpi(0, 0);
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = pU8SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
			//enableSpi(0, 1);
		}
		
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = pU8SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	// 16 位宽度，即半字收发
	else if(bitDeep == 16)	
	{
		U16 *pU16SpiData = (U16 *)pdata;
	
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = pU16SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
		}
	
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = pU16SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	// 32 位宽度，即字收发
	else if(bitDeep == 32 || bitDeep == 24) 
	{
		
		U32 *pU32SpiData = (U32 *)pdata;
		
		for (i=0; i<(len/4); i++)
		{
			for (k=0; k<4; k++)
			{
				ptSpiReg->DATAPORT = pU32SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
		}
		
		for (i=0; i<(len%4); i++)
		{
			ptSpiReg->DATAPORT = pU32SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_RecvData()
* 功能:	set 接收数据 
* 参数:	Channel 选择映射的GPSB 通道	
*		pdata 缓冲区
*		len   数据字节数
*		bitDeep 位宽，取值8,16,32, 分别代表8/16/32位宽
* 返回:	正确返回接收到的数据
*		HY_ERR_BAD_PARAMETER	参数错误

*------------------------------------------------------------------------------*/
U32 hyhwSpi_RecvData(U32 Channel, void* pdata, U32 len, U32 bitDeep)
{
	U32 i,j,k,recvUnit;
	tSPI_REGISTER *ptSpiReg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	//等待发送完成
	hyhwSpi_Wait_TX_End(Channel);

	j=0;
 
	if(bitDeep == 8)	//位宽为8
	{	
		U8 *pU8SpiData = (U8 *)pdata;
		recvUnit = 8;

		//清除fifo内数据
		for(i=0; i<8; i++)
		{
			pU8SpiData[i] = ptSpiReg->DATAPORT;
		}
		
		for (i=0; i<(len/8); i++)							
		{
			for (k=0; k<8; k++)
			{
			ptSpiReg->DATAPORT = 0xFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit , Channel, 8);		//等待接受完(len/8)个字节数据
			
			for (k=0; k<8; k++)
			{
				pU8SpiData[j++] = ptSpiReg->DATAPORT;
			}
				
		}
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = 0xFF;
		}
		
		recvUnit = len%8;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);			//等待接受完(len%8)个字节数据
		
		for (i=0; i<(len%8); i++)
		{
			pU8SpiData[j++] = ptSpiReg->DATAPORT;
		}
	}
	else if(bitDeep == 16) //位宽为16
	{
		U16 *pU16SpiData = (U16 *)pdata;
		recvUnit 		 = 8;
			
		//清除fifo内数据
		for(i=0; i<8; i++)
		{
			pU16SpiData[i] = ptSpiReg->DATAPORT;
		}
			
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = 0xFFFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);		//等待接受完(len/8)个字节数据
			
			for (k=0; k<8; k++)
			{
				pU16SpiData[j++] = ptSpiReg->DATAPORT;
			}
				
		}
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = 0xFFFF;
		}
		
		recvUnit	= len%8;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);			//等待接受完(len%8)个字节数据				
		
		for (i=0; i<(len%8); i++)
		{
			pU16SpiData[j++] = ptSpiReg->DATAPORT;
		}
	}		
	else if(bitDeep == 32 || bitDeep == 24) //32 bit width tx
	{
		
		U32 *pU32SpiData = (U32 *)pdata;
		recvUnit		 = len/4;							
		
		//清除fifo内数据
		for(i=0; i<4; i++)
		{
			pU32SpiData[i] = ptSpiReg->DATAPORT;
		}
		
		for (i=0; i<(len/4); i++)
		{
			for (k=0; k<4; k++)
			{
				ptSpiReg->DATAPORT = 0xFFFFFFFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 4);		//等待接受完(len/4)个字节数据
			
			for (k=0; k<4; k++)
			{
				pU32SpiData[j++] = ptSpiReg->DATAPORT;
			}
			
		}
		for (i=0; i<(len%4); i++)
		{
			ptSpiReg->DATAPORT = 0xFFFFFFFF;
		}
		
		recvUnit	= len%4;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 4);			//等待接受完(len%4)个字节数据
		
		for (i=0; i<(len%4); i++)
		{
			pU32SpiData[j++]  = ptSpiReg->DATAPORT;
		}
	}
	
	i = 0;
	j = 0;
	k = 0;
	return HY_OK;
}

#if 0
/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_ReturnState()
* 功能:	set slave spi mode 
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	返回现在的读写状态
*------------------------------------------------------------------------------*/
U32 hyhwSpi_ReturnState(U32 Channel)
{
	U32 status;
	tSPI_REGISTER *ptSpiReg;
	
	//检查参数
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	status	 = ptSpiReg->STAT;
	
	if(status & BIT2)
	{
		return ReadNEmpty; 			//读缓冲区不为空
	}
	else if(status & BIT3)
	{
		return WriteEmpty; 			//写缓冲区为空，可以继续发送数据
	}
	else
	{
		return NoneState; 			//返回无效状态
	}
}
#endif


/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_CheckInterrupt
* 功能:	判断是否产生中断
* 参数:	channel Id
* 返回: none
* 备注: 这里spi端口应用到得主要是GPSB的通道号, tg3 中用到的通道号分别是
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL , 
* 示例: hyhwSpi_CheckInterrupt();	
	
*----------------------------------------------------------------------------*/
BOOL hyhwSpi_CheckInterrupt(U8 channelId)
{
	U32 tempVal;
	
	tempVal = SPI_IRQ_STATUS;
	if(tempVal & (1<<(2*channelId))) 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_IntEnable
* 功能:	使能中断
* 参数:	channel Id
		IntSource 表示使能的中断源, 注意: 这里的中断源设置可以是多个中断源的或值
		IntSource 是BIT0-BIT8的或值
* 返回: none
* 备注: 这里spi端口应用到得主要是GPSB的通道号, tg3 中用到的通道号分别是
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL, 
		这里的寄存器值只包括最后的9个中断, 不包括前面的DMA相关的设置。
* 示例: hyhwSpi_CheckInterrupt(GSM_PCM_SPI_CTRL, RXFIFO_FULL);// 打开接收中断
*----------------------------------------------------------------------------*/
void hyhwSpi_IntEnable(U8 channelId, eSPI_INT_SRC IntSource)
{
	tSPI_REGISTER *ptSpiReg;
	
	ptSpiReg        = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*channelId );
	ptSpiReg->INTEN |= IntSource;
}

void hyhwSpi_IntDisable(U8 channelId, eSPI_INT_SRC IntSource)
{
	tSPI_REGISTER *ptSpiReg;
	
	ptSpiReg        = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*channelId );
	ptSpiReg->INTEN &= ~IntSource;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_IntClear
* 功能:	使能中断
* 参数:	channel Id
* 返回: none
* 备注: 这里spi端口应用到得主要是GPSB的通道号, tg3 中用到的通道号分别是
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL , 
* 示例: hyhwSpi_CheckInterrupt();	
	
*----------------------------------------------------------------------------*/
void hyhwSpi_IntClear(U8 channelID)
{
	// 读寄存器就可以清除中断
	// 该函数设置为空, 起始可以通过设置一个全局变量来保存状态寄存器的值就可以清除中断了。
}

#include "hyhwIntCtrl.h"
#if 0

U8 pData1[32];
void spi_rxInt_Isr(void)
{
	// 清除中断
	
	// 中断处理
	U32 spiStatus , i, num;
	
	spiStatus = TCC8300_SPI0_SPSTS_REGISTER(0);
	num = ((spiStatus>>16) & 0x0f);
	//while( TCC8300_SPI0_SPSTS_REGISTER(pcmSourceSpiChn) & BIT4 ) //RX FIFO CNT IS 12
	{
 		for( i = 0; i < num; i ++ )
		{
			//PhoneTrace(0, "pcm data");

			pData1[i] = (SPI_PCM_DATA_SDI(0));//从spi中读取pcm数据(高16bit有效)
		}
	}
	
}
#endif



U32 hyhwSpi_Send(U32 Channel, char* pdata, U32 len, U32 bitDeep)
{
	U32 i;
	for(i=0; i<len; i++)
	{
		while(0 != hyhwGpio_Read(PORT_E, BIT6));
		hyhwSpi_SendData(0, &pdata[i], 1, bitDeep);	
	
	}
	return 0;
}

/*mode 	区分写给primary还是secondary, 
  rwflag 区分是读还是写, 0 表示接收到数据，读, 1表示发送数据，写
  rs 寄存器选择, 1表示写指令, 0表示索引寄存器的写和状态寄存器读
发送之前注意要把片选置一下，结束后再返回原来状态 */
void hyhwSpi_LcdSend(unsigned short Data, unsigned int mode
	,unsigned int rs, unsigned int rwflag)
{
	unsigned int data;
	if(mode == 0)
	{
		// primary register
		/* 需要确定RS和RW的值 */
		data = Data | rs << 17 | rwflag << 16 | 0x16 << 18;
	}else
	{
		// secondary register
		data = Data | rs << 17 | rwflag << 16 | 0x17 << 18;
	}

	hyhwSpi_SendData(0, &data, 1, 24);
}

void hyhwSpi_LcdCmd(unsigned int cmd, unsigned int mode)
{
		// rw = 1, rs = 1
	hyhwSpi_LcdSend(cmd, mode, 1, 1);
}

void hyhwSpi_LcdData(unsigned int data, unsigned int mode)
{
	// rw = 1, rs = 0
	hyhwSpi_LcdSend(data, mode, 0, 1);
}

U32 hyhwSpi_LcdRecv(void)
{
	// can read state register use this function
	unsigned int data;
	hyhwSpi_LcdCmd(0x0, 0);
	hyhwSpi_RecvData(0, &data, 1, 24);
	return data;
}
/*-----------------------------------------------------------------------------
*	修改记录 1 :
2009 07 07 杨立敬
*	问题：
	1、程序测试时问题： 当发送并接受8个字节数据时，接受到第9个数据不为空，
	且不是想得到的数据，高文中添加了hyhwSpi_Wait_RX_DataNum()这个函数，由杨立敬
	整理

*	修改地方:
	头文件函数声明、源文件中文件定义，和该函数的6处调用
*------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	修改记录 2 :
2009 07 15	杨立敬
问题:
	1\slave 模式设置不能正常通信, 用BIT15等表示时意义不明确
	2\hyhwSpi_clearRXFifoCounter()hyhwSpi_clearRXFifoCounter1(),这两个宏定义是根据以前固定通道时写的，
		为了通用，这里将两个宏以函数形式定义
处理:
	函数:	hyhwSpi_Master()和hyhwSpi_Slave()中
	添加了bitWidth参数，用于设置位宽， 使用时需要注意
		可选8/16/32位宽, 用宏定义取代了BIT16等标示
修改位置:
	1\头文件中SPI_MODE寄存器中的位用宏进行定义
	2\头文件中和hyhwSpi_Master()和hyhwSpi_Slave()	相关的定义或声明处
		函数定义中相关寄存器配置值进行了部分修改，
	3\头文件中增加了hyhwSpi_clearRXFifoCounter()hyhwSpi_clearRXFifoCounter1()函数声明，
		源文件中注释掉了两个宏，定义了同名的函数，但添加了参数，需要在使用时注意
	
*------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
*	修改记录 3 :
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
	3\hyhwSpi.h 中的如下函数的声明和源文件中函数定义
	hyhwmSpi_DmaEnable,hyhwmSpi_DmaDisable,hyhwSpi_DmaIntClear,hyhwmSpi_DmaIntEnable,
	hyhwmSpi_DmaIntDisable,hyhwSpi_DmacheckInterruptState,hyhwSpi_DmaGetStatus;
	
补充, 按 Gao 的意见, 将程序中出现寄存器赋值语句多条时, 最好用一个中间变量来改变效率
,hyhwSpi_Slaver模式添加了参数sendEnable, 程序也进行了修改。	
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	修改记录 4 :
20090820
	按照高文中要求进行SPI8 即TD模块的测试，在测试过程中发现有些设置有问题，另外
	把前面集成工程时添加的函数进行了整理和注释
	:
	hyhwSpi_init, hyhwSpi_Slaver进行了修改
	新添加了函数:  hyhwSpi_Slaver_setSSP/hyhwSpi_IntClear/hyhwSpi_IntEnable/
	hyhwSpi_CheckInterrupt
------------------------------------------------------------------------------*/