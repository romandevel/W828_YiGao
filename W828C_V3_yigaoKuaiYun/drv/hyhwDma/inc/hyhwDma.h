/**
 * \defgroup vhApiSdma
 * @brief Describes the vhApi for the DMA controller.
 *
 * The dma driver provides an interface to configure and enable the DMA 
 * engine. The services are implemented. This functionality can be
 * enhanced in order to make full use of the services provided by the DMA engine.
 * This is the only interface that upper layers may use in order to access the
 * services of the SDMA engine.
 **/


#ifndef _HYHW_DMA_H_
#define _HYHW_DMA_H_

#define CODEC_BUFF_MAX	0xFFFF0									/* DAI模块使用的编解码空间缓冲区大小 */
#define CODEC_BUFF_MIN	0x10

#define LCD_CPU_INTERFACE_ID		0xf0000818
#define LCD_DMA_REGISTER_ADDR		0xf0040000 					/* dma0 channel 0 */
#define DAI_INTERFACE_IDO			0xf0059020					/* DAI DADO 发送寄存器首地址 */
#define DAI_INTERFACE_IDOR			0xf0059024					/* 右声道首地址 */
#define DAI_INTERFACE_IDI			0xf0059000					/* DAI DADO 接收寄存器首地址 */
#define DAI_INTERFACE_IDIR			0xf0059004					/* DAI DADO 接收寄存器首地址 */
#define DAI_DMA_REGISTER_ADDR		0xf0040100 					/* dma1 channel 0 */
#define DAI_DMA_LEFT_REGISTER_ADDR	0xf0040100					/* dma1 channel 0 */
#define DAI_DMA_RIGHT_REGISTER_ADDR	0xf0040130					/* dma1 channel 1 */
#define DAI_DMA_RECORD_REGISTER_ADDR 0xf0040160

/*  20090805
	SPI通道配置, 选择DMA通道为 DMA2的channel 0, 
	added by yanglijing, 20090805 
	------------------------------------------>
*/
#define SPI_DMA_REGISTER_ADDR		0xf0040200					/* 用于SPI通道DMA传输的通道 dma2 channel 0 */
#define SPI_DMA_TX_DST_REGISTER		0xf0057000					/* GPSB的通道0的数据读写端口地址 */
#define SPI_DMA_RX_SRC_REGISTER		0xf0057000					/* GPSB的通道0的数据读写端口地址 */
/* <-------------------------------------------
	SPI通道配置, 选择DMA通道为 DMA2的channel 0, 
	end added20090805 
*/

/*  20090806
	UART通道配置, 选择DMA通道为 DMA2的channel 1, 
	选择UART channel 1 映射到 port1，即UART1
	added by yanglijing, 20090805 
	------------------------------------------>
*/
#define UART1_DMA_REGISTER_ADDR		0xf0040230					/* 用于3M UART1通道DMA传输的通道 dma2 channel 1 */
#define UART1_DMA_TX_DST_REGISTER	0xf0055100					/* UART1的写数据寄存器地址 THR, CPU写入, 外设来读取 */
#define UART1_DMA_RX_SRC_REGISTER	0xf0055100					/* UART1的读数据寄存器地址 RBR, 与THR复用一个寄存器 */
/* <-------------------------------------------
	SPI通道配置, 选择DMA通道为 DMA2的channel 0, 
	end added20090805 
*/


#define	DMA_TOTAL_CHANNEL_NUM		9
#define DMA_CHANNEL_NUM_PER_DMA		3
#define DMA_CHANNEL_OFFSET			0x30

#define DMA_DAI_LRBUF_OFFSET		8							/* DAI 寄存器左/右通道的偏移量, 该值主要是为了DAI/DMA传输设置 */
#define DMA_PORT_TYPE_WORD 			4
#define DMA_PORT_TYPE_HALF_WORD 	2
#define DMA_PORT_TYPE_BYTE 			1
#define DMA_PORT_TYPE_STATIC 		0

#define DMA0_BASEADDR			TCC7901_GDMA0_BASEADDR
#define DMA1_BASEADDR			TCC7901_GDMA1_BASEADDR
#define DMA2_BASEADDR			TCC7901_GDMA2_BASEADDR
#define DMA0_CONFIG_REGISTER	TCC7901_DMA0_CONFIG_REGISTR		//channel configuration register
#define DMA1_CONFIG_REGISTER	TCC7901_DMA1_CONFIG_REGISTR
#define DMA2_CONFIG_REGISTER	TCC7901_DMA2_CONFIG_REGISTR

//DMA control bit define 
	#define CHCTRL_BIT_ISSUE_CONTINU_TRANSFER					BIT15
	#define CHCTRL_BIT_ISSUE_STARTADDR_TRANSFER					~BIT15
	#define CHCTRL_BIT_DIFFERENTIAL_TRANSFER_MODE_ENABLE		BIT14
	#define CHCTRL_BIT_DIFFERENTIAL_TRANSFER_MODE_DISABLE		~BIT14
	#define CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE					BIT13
	#define CHCTRL_BIT_HW_REQUEST_SYNC_DISABLE					~BIT13
	#define CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE				BIT12
	#define CHCTRL_BIT_HW_REQUEST_DIRECTION_READ				0
	#define CHCTRL_BIT_TRANSFER_LOCKED							BIT11
	#define CHCTRL_BIT_TRANSFER_UNLOCK							~BIT11
	#define CHCTRL_BIT_TRANSFER_BURST_MODE						BIT10
	#define CHCTRL_BIT_TRANSFER_ARBITRATION_MODE				~BIT10
	
	#define CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE				(0x0 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_HARDWARE					(0x1 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE					(0x2 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_SINGLE_LEVEL				(0x3 << 8)

	
	

	#define CHCTRL_BIT_BURST_SIZE_1								(0x0 << 6)
	#define CHCTRL_BIT_BURST_SIZE_2								(0x1 << 6)
	#define CHCTRL_BIT_BURST_SIZE_4								(0x2 << 6)
	#define CHCTRL_BIT_BURST_SIZE_8								(0x3 << 6)
	
	#define CHCTRL_BIT_WORD_SIZE_8BIT							(0x0 << 4)
	#define CHCTRL_BIT_WORD_SIZE_16BIT							(0x1 << 4)
	#define CHCTRL_BIT_WORD_SIZE1_32BIT							(0x2 << 4)
	#define CHCTRL_BIT_WORD_SIZE2_32BIT							(0x3 << 4)
	
	#define CHCTRL_BIT_DMA_DONE_FLAG							BIT3
	#define CHCTRL_BIT_DMA_INT_ENABLE							BIT2
	#define CHCTRL_BIT_DMA_INT_DISABLE							~BIT2
	#define CHCTRL_BIT_REPEAT_MODE_ENABLE						BIT1
	#define CHCTRL_BIT_REPEAT_MODE_DISABLE						~BIT1
	#define CHCTRL_BIT_DMA_START								BIT0
	#define CHCTRL_BIT_DMA_STOP									~BIT0

//start external DMA request register bit define
	#define	EXTREQ_BIT_UART1RX									BIT30	// UART channel 1 receiver
	#define	EXTREQ_BIT_UART1TX									BIT29	// UART channel 1 transmitter
	#define	EXTREQ_BIT_UART0RX									BIT27	// UART channel 0 receiver
	#define	EXTREQ_BIT_UART0TX									BIT26	// UART channel 0 transmitter
	#define	EXTREQ_BIT_I2C1										BIT25	// I2C channel 1
	#define EXTREQ_BIT_DAIRX									BIT24	// DAI Receiver
	#define EXTREQ_BIT_DAITX									BIT23	// DAI transmitter
	#define EXTREQ_BIT_CDIF										BIT22	// CD interface 
	#define EXTREQ_BIT_SPDIFU									BIT21	// SPDIF User data
	#define EXTREQ_BIT_SPDIFP									BIT20	// SPDIF Paceket(audio) data
	#define EXTREQ_BIT_I2C0										BIT19	// I2C channel 0
	#define EXTREQ_BIT_NFC										BIT18	// Nand Flash Controller
	#define EXTREQ_BIT_MSTICK									BIT17	// memory stick
	#define EXTREQ_BIT_ECC										BIT12	// ECC
	#define EXTREQ_BIT_UART3RX									BIT11	// UART channel 3 receiver
	#define EXTREQ_BIT_UART3TX									BIT10	// UART channel 3 transmitter
	#define EXTREQ_BIT_UART2RX									BIT9	// UART channel 2 receiver
	#define EXTREQ_BIT_UART2TX									BIT8	// UART channel 2 transmitter
	#define EXTREQ_BIT_GPSB3RX									BIT7	// GPSB channel 3 RX
	#define EXTREQ_BIT_GPSB2RX									BIT6	// GPSB channel 2 RX
	#define EXTREQ_BIT_GPSB1RX									BIT5	// GPSB channel 1 RX
	#define EXTREQ_BIT_GPSB0RX									BIT4	// GPSB channel 0 RX
	#define EXTREQ_BIT_GPSB3TX									BIT3	// GPSB channel 3 TX
	#define EXTREQ_BIT_GPSB2TX									BIT2	// GPSB channel 2 TX
	#define EXTREQ_BIT_GPSB1TX									BIT1	// GPSB channel 1 TX
	#define EXTREQ_BIT_GPSB0TX									BIT0	// GPSB channel 0 TX	 
// end external DMA request register bit define

// start DMA channel configuration register  bit define
	#define CHCONFIG_BIT_CH2_INT_STATUS							BIT22
	#define CHCONFIG_BIT_CH1_INT_STATUS							BIT21
	#define CHCONFIG_BIT_CH0_INT_STATUS							BIT20
	// 与上面status的区别是这些bit只有在INT ENABLE 下，才有效
	#define CHCONFIG_BIT_CH2_INT_MASKED_STATUS					BIT18
	#define CHCONFIG_BIT_CH1_INT_MASKED_STATUS					BIT17
	#define CHCONFIG_BIT_CH0_INT_MASKED_STATUS					BIT16 
	
	#define CHCONFIG_BIT_CH2_DATA_SWAP_ENABLE					BIT10
	#define CHCONFIG_BIT_CH1_DATA_SWAP_ENABLE					BIT9
	#define CHCONFIG_BIT_CH0_DATA_SWAP_ENABLE					BIT8
	
	#define CHCONFIG_BIT_CH_PRIORITY_012						(0x0 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_021						(0x1 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_102						(0x2 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_120						(0x3 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_210						(0x4 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_201						(0x5 << 4)
	
	#define CHCONFIG_BIT_CH_PRIORITY_FIXED						~BIT0
	#define CHCONFIG_BIT_CH_PRIORITY_CYCLIC						BIT0
// end 	DMA channel configuration register  bit define


// start DMA channel Repeat control 寄存器 bit 定义	
	#define RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE   BIT31
	#define RPTCTRL_BIT_EOT_REPEAT_INT_DISABLE   BIT30

// end DMA channel Repeat control 寄存器 bit 定义	  


/***************************for test*********************************/	 
#define ST_SADR			0x00
#define SPARAM			0x04
#define C_SADR			0x0C
#define ST_DADR			0x10
#define DPARAM			0x14
#define C_DADR			0x1C
#define HCOUNT			0x20
#define CHCTRL			0x24
#define RPTCTRL			0x28
#define EXTREQ			0x2C

#define GDMA0_CH0		0xf0040000
#define GDMA0_CH1		0xf0040030
#define GDMA0_CH2		0xf0040060

#define GDMA1_CH0		0xf0040100
#define GDMA1_CH1		0xf0040130
#define GDMA1_CH2		0xf0040160

#define GDMA2_CH0		0xf0040200
#define GDMA2_CH1		0xf0040230
#define GDMA2_CH2		0xf0040260


#define DMA_REG(X,Y)		*( volatile U32 *)(X+Y)

/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/
typedef struct _tDMA_REGISTER 
{
  U32 SOURCE_START_ADDR ;
  U32 SOURCE_PARAMETER ;
  U32 _SOURCE_PARAMETER ;
  U32 SOURCE_CURRENT_ADDR;
  U32 DESTINATION_START_ADDR;
  U32 DESTINATION_PARAMETER;
  U32 _DESTINATION_PARAMETER;
  U32 DESTINATION_CURRENT_ADDR;
  U32 HOP_COUNT;
  U32 CHANNEL_CONTROL;
  U32 REPEAT_CONTROL;
  U32 EXTERNAL_DMA_REQUEST;
}tDMA_REGISTER;

typedef struct  
{
  tDMA_REGISTER CH0;
  tDMA_REGISTER CH1;
  tDMA_REGISTER CH2;
}tDMA_CH;

typedef struct  
{
  tDMA_REGISTER CH0;
  tDMA_REGISTER CH1;
  tDMA_REGISTER CH2;
}tDMA;

typedef enum 
{
	channle0,
	channle1,
	channle2,
	channle3,
	channle4,
	channle5,
	channle6,
	channle7,
	channle8
	
} eDMA_CHANNEL;


typedef enum _eDmaPortWidth_en
{
	DmaPortWidth32bits,
	DmaPortWidth08bits,
	DmaPortWidth16bits,
	DmaPortWidthReserved	//错误的端口宽度
	
} eDMAPORTWIDTH_EN;

typedef enum eDmaHardwareModule_en
{
	UT1_RX	=	BIT30	,	// UART channel 1 receiver
	UT1_TX	=	BIT29	,	// UART channel 1 transmitter
	UT0_RX	=	BIT27	,	// UART channel 0 receiver
	UT0_TX	=	BIT26	,	// UART channel 0 transmitter
	I2C_1	=	BIT25	,	// I2C channel 1
	DAI_RX	=	BIT24	,   // DAI Receiver
	DAI_TX	=	BIT23	,   // DAI transmitter
	CD_IF	=	BIT22	,	// CD interface 
	SPDIF_U	=	BIT21	,   // SPDIF User data
	SPDIF_P	=	BIT20	,   // SPDIF Paceket(audio) data
	I2C_0	=	BIT19	,	// I2C channel 0
	NFC_N	=	BIT18	,	// Nand Flash Controller
	MS_TICK	=	BIT17	,   // memory stick
	ECC_E	=	BIT12	,	// ECC
	UT3_RX	=	BIT11	,	// UART channel 3 receiver
	UT3_TX	=	BIT10	,	// UART channel 3 transmitter
	UT2_RX	=	BIT9	,	// UART channel 2 receiver
	UT2_TX	=	BIT8	,	// UART channel 2 transmitter
	GSB3_RX	=	BIT7	,	// GPSB channel 3 RX
	GSB2_Rx	=	BIT6	,	// GPSB channel 2 RX
	GSB1_RX	=	BIT5	,	// GPSB channel 1 RX
	GSB0_RX	=	BIT4	,	// GPSB channel 0 RX
	GSB3_TX	=	BIT3	,	// GPSB channel 3 TX
	GSB2_TX	=	BIT2	,	// GPSB channel 2 TX
	GSB1_TX	=	BIT1	,	// GPSB channel 1 TX
	GSB0_TX	=	BIT0		// GPSB channel 0 TX	 
	
} eDMAHARDWAREMODULE_EN;


typedef enum 
{
	dmaPortTypeStatic,
	dmaPortTypeByte,
	dmaPortTypeHalfWord,
	dmaPortTypeWord = 4
} eDMAPORTTYPE;

typedef enum eDmaHardwareModuleRW_en
{
	HardwareRead,
	HardwareWrite
} eDMAHARDWAREMODULERW_EN;

typedef enum eLcdBusType_en
{
	LcdBusType16,
	LcdBusType32

}eLCDBUSTYPE_EN;

typedef enum eDaiTransType
{
	DaiTxType,	// DAI 发送
	DaiRxType	// DAI 接收
}eDAI_TRANS_TYPE;


typedef enum eSpiTransType
{
	TxType,	// DMA 发送
	RxType	// DMA 接收
}eSPI_TRANS_TYPE, e_TRANS_TYPE;

/*---------------------------------------------------------------------------
   Function prototypes:
   --------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Init
* 功能:	初始化DMA
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwDma_Init(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Init
* 功能:	关闭DMA电源，以节省用电
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwDma_DeInit(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Config
* 功能:	设置DMA
* 参数:	sa------数据源地址
*		da------数据目的地址
*		size--需要传输的字数( 32 bit word )
* 返回:	可用的通道号，根据优先级返回最先可用的通道
*		HY_ERR_BUSY:无可用的DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size);

/*-----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntEnable 
* 功能:	使能 Dai Dma 传输
* 参数:	ChannelId DMA通道
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwDma_Enable(U8 ChannelId);

/*----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntDisable 
* 功能:	disable Dai Dma 传输
* 参数:	ChannelId DMA通道
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwDma_Disable(U8 ChannelId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_IntClear
* 功能:	清除指定通道的DMA中断
* 参数:	ChannelId--通道号
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntClear(U8 ChannelId);

/*-----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	ChannelId DMA通道
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwDma_IntEnable(U8 ChannelId);

/*----------------------------------------------------------------------------
* 宏  :	hyhwmDai_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	ChannelId DMA通道
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwDma_IntDisable(U8 ChannelId);

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
U32 hyhwDma_GetStatus(U8 ChannelId);	
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_isDmaInterruptSet
* 功能:	判断是否中断使能
* 参数:	ChannelId
*		ChannelId--通道号
* 返回:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId);

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
						 	);

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
						);
	
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart0TxConfig
* 功能:	设置DMA uart0 Tx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0TxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart0RxConfig
* 功能:	设置DMA uart0 Rx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0RxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart2TxConfig
* 功能:	设置DMA uart2 Tx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2TxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart2RxConfig
* 功能:	设置DMA uart2 Rx DMA 使能传输
* 参数:	ChannelId------选择用于传输的DMA通道
*		sa------数据源地址地址
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2RxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* 函数:	isDaiDmaDmaOK
* 功能:	判断dai DMA是否完成，如果完成则清中断，返回TRUE，否则返回FAULSE
* 参数:	none
* 返回:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL isDaiDmaDmaOK(eDAI_TRANS_TYPE daiTransferType);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_lcdCpuConfig
* 功能:	设置lcd 屏DMA
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_lcdCpuConfig(eLCDBUSTYPE_EN  busType);

/*-----------------------------------------------------------------------------
* 函数:	lcdCpuEnable
* 功能:	使能lcd 屏DMA
* 参数:	sa		--	数据源地址
*		size	--	需要传输的半字数
* 返回:	none		
*----------------------------------------------------------------------------*/
U32 lcdCpuEnable(U16 * pdata, U32 size);

/*-----------------------------------------------------------------------------
* 函数:	lcdCpuDmaOK
* 功能:	判断LCD DMA是否完成，如果完成则清中断，返回TRUE，否则返回FAULSE
* 参数:	none
* 返回:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL lcdCpuDmaOK(void);		

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_isDmaInterruptSet
* 功能:	判断是否中断使能
* 参数:	ChannelId
*		ChannelId--通道号
* 返回:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId);


			
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_Config
* 功能:	设置DMA
* 参数:	sa------数据源地址
*		da------数据目的地址
*		size--需要传输的字节数
* 返回:	HY_OK 
*		HY_ERR_BUSY:无可用的DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size);


/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaConfig
* 功能:	设置DMA DAI DMA 使能传输, 指定固定通道DAI_DMA_REGISTER_ADDR
*		该函数和下一个函数均指定特定通道用于DMA传输, 以前的函数太过麻烦（上面3个)
*		该函数由杨健建议,后来改动主要是基于和codec模块左右通道的搭配，配置2个DMA
*		,在高文中帮助下实现。
* 参数:	daiTransferType------指定传输类型, 可选值(0,1): 0表示发送， 1 表示接收
*		size------缓冲区大小, 用于指定DMA传输的缓冲区大小,该缓冲区设置环形区域
*		pBufLeftAddr---------音频缓冲区左声道指针，即本次DMA传输的首地址
*		pBufRightAddr--------音频缓冲区右声道指针，即本次DMA传输的首地址
*		sampleCount----------采样子个数，该参数决定HOPCOUNT值
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
						U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaEnable
* 功能:	启动DAI DMA的DMA传输
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaEnable(eDAI_TRANS_TYPE daiTransferType);

/*----------------------------------------------------------------------------
* 函数:	hyhwDma_SetDaiSampleCount
* 功能:	 
* 参数:	sampleCount				--  Tx/RX sample count 
* 返回:	HY_OK					--	成功
		HY_ERR_BAD_PARAMETER 	--	输入参数错误
*----------------------------------------------------------------------------*/
U32 hyhwDma_SetDaiSampleCount(U32 sampleCount, eDAI_TRANS_TYPE daiTransferType);

// 开始SPI DMA 操作函数接口声明
/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_DaiDmaConfig	added by yanglijing, 2009-08-05
* 功能:	设置DMA SPI DMA 使能传输, 指定固定通道SPI_DMA_REGISTER_ADDR
* 参数:	daiTransferType------指定传输类型, 可选值(0,1): 0表示发送， 1 表示接收
*		size-----------------缓冲区大小
*		pBufAddr-------------DMA传输首地址
*		count----------------传输中断数值, 该参数决定HOPCOUNT值, 字节表示
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaConfig(eSPI_TRANS_TYPE TransferType, U32 size, U32 *pBufAddr, U32 Count);						

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_SpiDmaEnable
* 功能:	启动SPI DMA的DMA传输
* 参数:	none
* 返回:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaEnable();

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_uart1Config
* 功能:	设置DMA uart1 Tx DMA 使能传输
* 参数:	transType---------------DMA传输方向, 可选参数TxType/RxType
*		buffsize----------------环形缓冲区大小, 以字节计
*		sourceAddr--------------数据源地址地址
*		bytesCount--------------DMA一次中断的字节数
* 返回: none
* 用例: hyhwDma_uart1Confit(TxType, 512, 0x2010000, 128)
*		配置串口DMA通道参数为环形缓冲区大小512字节，发送模式，每128字节一次DMA中断
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1Config(e_TRANS_TYPE transType, U32 buffsize, U32 sourceAddr, U32 bytesCount);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_GetCurrentSrcAddr 
* 功能:	查询当前DMA通道当前传输的源地址
* 参数:	ChannelId--通道号
* 返回:	当前DMA传输源地址值/ 或者HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentSrcAddr(U32 ChannelId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwDma_GetCurrentDstAddr 
* 功能:	查询当前DMA通道当前传输目的地址
* 参数:	ChannelId--通道号
* 返回:	当前DMA传输目的地址值
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentDstAddr();

#endif  //_HYHW_DMA_H_

