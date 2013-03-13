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

#ifndef HY_HW_SPI_H_
#define HY_HW_SPI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PCM_INT_NUM  	6       // 发生中断的数据个数 

#define TD_BOOT_SPI_CTRL	0	// 对应GPSB 端口7
#define GSM_PCM_SPI_CTRL	2   // 对应GPSB 端口6
#define TD_PCM_SPI_CTRL		3   // 对应GPSB 端口8

#define TD_BOOT_SPI_CHN			7
#define GSM_PCM_SPI_CHN			6
#define TD_PCM_SPI_CHN			8


#if 0
#define TCC8300_SPI0_SPSTS_REGISTER		1
#define TCC8300_SPI0_SDI_REGISTER		*((volatile U32 *)(0x9000c000))
#define TCC8300_SPI0_SDO_REGISTER		*((volatile U32 *)(0x9000c000))
#define INTA_BIT_SPI_RX					1

#else
#define TCC8300_SPI0_SPSTS_REGISTER(m)	(((tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*m ))->STAT)
#define TCC8300_SPI0_SDI_REGISTER		*((volatile U32 *)(0xf0057200)) // 由57000改为57200
#define TCC8300_SPI0_SDO_REGISTER		*((volatile U32 *)(0xf0057200))
#define INTA_BIT_SPI_RX 				INT_BIT_GPSB	
#endif


#define SPI_IRQ_STATUS				    *((volatile unsigned *)(0xf0057804)) /* 中断状态寄存器 */
#define SPI_CONFIG_REGISTER				TCC7901_GPSB_PCFG_REGISTER		 	 /* GPSB PCFG register */

/* GPSB MODE register bitmask definition, for evaluate the tSPI_REGISTER->MODE */
#define SPI_MODE_REGISTER

	#define SPI_MODE_OFFSITE_CLK_DIVIDER		24
	//#define SPI_MODE_BITMASK_CLK_DIVIDER		(0x000000ff << SPI_MODE_OFFSITE_CLK_DIVIDER)	// clock divider load value
	#define SPI_MODE_BITMASK_CLK_DIVIDER		(0xFF000000)	// clock divider load value

	#define SPI_MODE_BIT_TRE_TIME_RECOVERY		BIT23			// master recoviry time
	#define SPI_MODE_BIT_THL_TIME_HOLD			BIT22			// master hold time
	#define SPI_MODE_BIT_TSU_TIME_SETUP			BIT21			// master setup time
	#define SPI_MODE_BIT_PCS_POLARITY_CS		BIT20			/* polarity control for CS(FRM), master only
																   0 for active low(default) /must be 0 for SSP
																   1 for active high */
	#define SPI_MODE_BIT_PCD_POLARITY_CMD		BIT19			/* polarity for CMD(FRM)-master only																   
																   0 for active low(default) /must be 1 for SSP
																   1 for active high */

	#define SPI_MODE_BIT_PWD_POLARITY_SEND		BIT18			/* polarity for transmitting data-master only																   
																   0 for falling edge of SCK
																   1 for rising edge of SCK, must be 1 for SSP */
	#define SPI_MODE_BIT_PRD_POLARITY_RECV		BIT17			/* polarity for receivving data-master only																   
																   0 for falling edge of SCK
																   1 for rising edge of SCK, must be 1 for SSP */
	
	#define SPI_MODE_BIT_PCK_POLARITY_PCK		BIT16			/* polarity control for serial clock:
																   0 for master, SCKO start from "low" for SPI timing 0,
																   	 and "high" for SSP timing
																   0 for slave, SCKI not inverted, for SPI timing 0 and 3 				
																   1 for master, SCKO starts from "1", for SPI timing 2 and 3
																   1 for slave, SCKI inverted, for SPI timing 1,2,and SSP timing */
	#define SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER	BIT15		// clear receive FIFO counter 
	#define SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER	BIT14		// clear transmit FIFO counter 


	/* bit width selection: Data bit width == BWS+1, valid range for BWS is 7~31, FIFOS are configured 
		according to BWS[4]as follows: "BWS[4] == 1, note 4*32 bit, BWS[4] ==0, 8*16bits */
	#define SPI_MODE_OFFSITE_BITWIDTH				8
	#define SPI_MODE_BITMASK_BWS_BITWIDTH			(0x1f << SPI_MODE_OFFSITE_BITWIDTH)
	#define SPI_MODE_BITMASK_BWS_BITWIDTH_8			(0x7  << SPI_MODE_OFFSITE_BITWIDTH) // 位宽8, FIFO 8*16 bits, FIFO 深度8		
	#define SPI_MODE_BITMASK_BWS_BITWIDTH_16		(0x15 << SPI_MODE_OFFSITE_BITWIDTH) // 位宽16, FIFO 8*16 bits, FIFO 深度8
	#define SPI_MODE_BITMASK_BWS_BITWIDTH_32		(0x31 << SPI_MODE_OFFSITE_BITWIDTH) // 位宽32, FIFO 4*32 bits, FIFO 深度4		
	
	#define SPI_MODE_BIT_SD_SHIFT_DIRECTION			BIT7		// data shift direction control: 0 for shift left (MSB first)
	#define SPI_MODE_BIT_LB_LOOPBACK_ENABLE			BIT6		/* data loop back enable, SDO is feedback to SDI internally 
																   and incoming data from external I/O is ignored */
	#define SPI_MODE_BIT_SDO_OUTPUT_DISABLE			BIT5		// SDO output disable(slave mode only)																   
	#define SPI_MODE_BIT_CTF_CONT_TRANSFER_ENABLE	BIT4		/* continuous transfer mode enable, 0 for single mode ,1 for 
																   continuous mode, if setto 1, CS 保持有效直到该位清除，并且
																   tranmit fifo is empty */
																   
	#define SPI_MODE_BIT_EN_OPERATION_ENABLE		BIT3		// operation enable bit 
	#define SPI_MODE_BIT_SLV_SLAVE_MODE				BIT2		// slave mode configuration: 1 for slave ,0 for master 

	#define SPI_MODE_BITMASK_MD_OPERATION			0x3			// operation mode
	#define SPI_MODE_BITMASK_MD_OPERATION_SPI		0x0			// SPI compatible 模式			
	#define SPI_MODE_BITMASK_MD_OPERATION_SSP		0x1			// SSP compatible 模式			
	#define SPI_MODE_BITMASK_MD_OPERATION_RESV		BIT1		// 保留			

typedef struct _tSPI_REGISTER 
{
  U32 DATAPORT ;
  U32 STAT ;
  U32 INTEN ;
  U32 MODE;
  U32 CTRL;
  U32 TXBASE;
  U32 RXBASE;
  U32 PACKET;
  U32 DMACTR;
  U32 DMASTR;
  U32 DMAICR;
  U32 EXTERNAL_DMA_REQUEST;
}tSPI_REGISTER;

typedef enum _eSPI_STATE
{
	 ReadFull,						// read fifo full
	 WriteEmpty,					// write fifo empty flag
	 ReadNEmpty,					// Read fifo not empty
	 WriteTH,						// write fifo valid entry count is under threshhold
	 ReadTH,						// Readfifo valid entry increased over threshhold
	 NoneState
} eSPI_STATE;

typedef enum _eSPI_INT_SRC
{
    TXFIFO_OVER_RUN_ERR   = BIT8,
	RXFIFO_UNDER_RUN_ERR  = BIT7,
	TXFIFO_UNDER_RUN_ERR  = BIT6,
	RXFIFO_OVER_RUN_ERR   = BIT5,
	RXFIFO_FULL           = BIT4,
	TXFIFO_EMPTY          = BIT3,
	RXFIFO_NOT_EMPTY      = BIT2,
	TXFIFO_ENTRY_UNDER_THRESHOLD = BIT1,
	RXFIFO_ENTYR_UPPER_THRESHOLD = BIT0	
}eSPI_INT_SRC;

#ifdef __cplusplus
}
#endif

/* 函数声明部分 */
/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaEnable 
* 功能:	使能 Spi Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaEnable();

/*--------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaDisable 
* 功能:	disable Spi Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaDisable();

/*---------------------------------------------------------------------------
* 函数:	hyhwSpi_DmaIntClear
* 功能:	清除指定通道的DMA中断
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwSpi_DmaIntClear();

/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaIntEnable();

/*---------------------------------------------------------------------------
* 宏  :	hyhwmSpi_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaIntDisable();
	
/*---------------------------------------------------------------------------
* 函数:	hyhwSpi_DmacheckInterruptState
* 功能:	判断是否中断使能
* 参数:	none
* 返回:	True 代表已经中断, FALSE 代表未中断
*--------------------------------------------------------------------------*/
BOOL hyhwSpi_DmacheckInterruptState();

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
U32 hyhwSpi_DmaGetStatus();

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_clearRXFifoCounter
* 功能:	清空接收缓冲区
* 参数:	Channel 选择映射的GPSB 通道
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter(U32 Channel);

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_clearRXFifoCounter1
* 功能:	清空接收缓冲区
* 参数:	Channel 选择映射的GPSB 通道
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter1(U32 Channel);

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_init(U32	Channel, U32 numSPI)
* 功能:	Map GPSB channel to numSPI 
* 参数:	Channel 选择映射的GPSB 通道
*		numSPI	选择映射的SPI端口序号	
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER 参数不对
*		HY_ERR_BYSY	所用通道或端口忙
* 示例：channel 0 Remap as SPI7: hyhwSpi_Map(0,7)
*------------------------------------------------------------------------------*/
U32 hyhwSpi_init( U32 Channel, U32 numSPI );

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Master()
* 功能:	set master spi mode 
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	HY_OK 正确
*		HY_ERR_BYSY	所用通道或端口忙
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Master( U32 Channel, U32 divider, U32 recovTime, U32 holdTIme, U32 SetupTime, U32 bitWidth);

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
U32 hyhwSpi_Slaver( U32 Channel, U32 bitWidth, int sendEnable);

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Wait_TX_End()
* 功能:	判断是否等待超时
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER	参数错误
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_TX_End( U32 Channel);

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
U32 hyhwSpi_Wait_RX_DataNum(U32 dataNum, U32 Channel, U32 deep);

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_Wait_RX_End()
* 功能:	判断是否等待超时
* 参数:	Channel 选择映射的GPSB 通道		
* 返回:	HY_OK 正确
*		HY_ERR_BAD_PARAMETER	参数错误
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_RX_End(U32 Channel);

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
U32 hyhwSpi_SendData(U32 Channel, void* pdata, U32 len, U32 bitDeep);

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
U32 hyhwSpi_RecvData(U32 Channel, void* pdata, U32 len, U32 bitDeep);

/*-----------------------------------------------------------------------------
* 函数:	hyhwSpi_ReturnState()
* 功能:	set slave spi mode 
* 参数:	Channel 选择映射的GPSB 通道	
* 返回:	返回现在的读写状态
*------------------------------------------------------------------------------*/
U32 hyhwSpi_ReturnState(U32 Channel);

#endif /* HY_HW_SPI_H_ */
