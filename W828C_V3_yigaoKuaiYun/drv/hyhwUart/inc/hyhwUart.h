

#ifndef HY_HW_UART_H_
#define HY_HW_UART_H_

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#ifdef __cplusplus
/*  Assume C declarations for C++ */
extern "C" {
#endif

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Types and defines:
------------------------------------------------------------------------------*/
//共4个通用的UART
#define UART_NUM		3
#define UART0			0
#define UART1			1
#define UART2			2
#define UART3			3

/* 可以映射的UART 端口为6个，但同时只能有4个端口在工作 */
#define UART_PORT_NUM	6
#define UART_PORT_0		0
#define UART_PORT_1		1
#define UART_PORT_2		2
#define UART_PORT_3		3
#define UART_PORT_4		4
#define UART_PORT_5		5


#ifdef DEBUG //开发板是串口3
#define SCRATCH_UART   UART3
#else
#define SCRATCH_UART   UART0
#endif

#define	UART_BASEADDR		TCC7901_UART_BASEADDR
#define	UART_OFFESTADDR		TCC7901_UART_OFFSETADDR

#define TD_DATA_UART		UART1	// 流控控制,  用于td数据通信使用的UART1, 3M速度通信
#define TD_AT_STRUCT_UART 	UART2 // 一般串口

/* 用于通信的几个串口的基地址定义 */
#define GSM_UART_ADDR		 	TCC7901_UART_BASEADDR0 	
#define TD_DATA_UART_ADDR		TCC7901_UART_BASEADDR1
#define TD_AT_STRUCT_UART_ADDR 	TCC7901_UART_BASEADDR2


/*tcc 7901 added*/ 
#define	UART_MUX_BASEADDR	0xF0055400											/* port mux register base address */	
#define	UART_MUX_CHSEL		(*(volatile U32 *)(UART_MUX_BASEADDR + 0x00))		/* channel selection Register read/write*/
#define	UART_MUX_CHST		(*(volatile U32 *)(UART_MUX_BASEADDR + 0x04))		/* channel state Register read only */ 


typedef	volatile struct tuart_register
{
	U32	UART_RBR;
		#define	RECEIVE				UART_RBR
		#define	TRANSMIT			UART_RBR
		#define	DIVISOR_LATCH_LOW	UART_RBR
	U32	UART_IER;
		#define	INT_ENABLE 			UART_IER
		#define	DIVISOR_LATCH_HIGH	UART_IER
	U32	UART_IIR;
		#define	INT_IDENT			UART_IIR
		#define	FIFO_CONTROL		UART_IIR
	U32	UART_LCR;
		#define	LINE_CONRROL		UART_LCR
	U32	UART_MCR;
		#define	MODEM_CONTROL		UART_MCR
	U32	UART_LSR;
		#define	LINE_STATUS			UART_LSR
	U32	UART_MSR;
		#define	MODEM_STATUS		UART_MSR
	U32	UART_SCR;
		#define	SCRATCH				UART_SCR
	U32	UART_AFT;
		#define	AFC_TRIGGER			UART_AFT
	U32	UART_UCR;	
		#define	UART_CONTROL		UART_UCR
} tUART_REGISTER;

/**
 * @brief UART IP3106 baud rate enumeration
 *
 * The UART IP3106 can be configured for different baud rates.
 */
typedef enum euart_Baudrate
{
  Uart_Baudrate_3000000=0,
  Uart_Baudrate_1000000,
  Uart_Baudrate_500000,
  Uart_Baudrate_230400,
  Uart_Baudrate_115200,
  Uart_Baudrate_57600,
  Uart_Baudrate_38400,
  Uart_Baudrate_19200,
  Uart_Baudrate_9600,
  Uart_Baudrate_4800,
  //Uart_Baudrate_3600,
  Uart_Baudrate_2400
} eUART_BAUDRATE;

static const UInt16 sUartDivisor[] = 
{
	/* fuart = 3M */
//	39,      /* 4800 */

	/* fuart = 6M */
//	39,     /* 9600 */

	/* BaudRate = 12 Mhz / (16  * Divisor) */   
//	7,		/*  115200  baud */   clk tolerance is unbearable
//	13,		/*  57600  baud  */
//	20, 	/*  38400  baud  */	   clk tolerance is unbearable
//	39, 	/*  19200  baud  */
//	78, 	/*   9600  baud  */    ---

	/* BaudRate = 24 Mhz / (16  * Divisor) */   
//	13,		/*  115200  baud  */   
// 	26,		/*  57600  baud  */
//  39, 	/*  38400  baud  */
//  78, 	/*  19200  baud  */ --
//  156, 	/*   9600  baud  */ -- 
//  416, 	/*   4800  baud  */ --
//  625, 	/*   2400  baud  */

	/* BaudRate = 48 Mhz / (16  * Divisor) */   
	1,		/*	3000000	baud */
	3,		/*	1000000	baud */
	6,		/*	500000	baud */
	26,		/*  115200  baud */  //--
	52,		/*  57600  baud  */  //--
	78, 	/*  38400  baud  */ //--
	156, 	/*  19200  baud  */ //--
	313, 	/*   9600  baud  */ //--

};
#include "hyhwCkc.h"

typedef struct{
	ePERI_CLK  uart_clk;
	UInt16 sDivisor;
} tUART_BAUDCONFIG;


static const tUART_BAUDCONFIG g_arrayBaudRate[] = 
{
	/* 这里修改定义方式的主要目的是为了分频时的系数接近整数，减少偏差 */
	{PERI_CLK_48M,  1},
	{PERI_CLK_48M,  3},
	{PERI_CLK_48M,  6},
	{PERI_CLK_48M,  13},
	{PERI_CLK_24M,  13},
	{PERI_CLK_24M,  26},
	{PERI_CLK_24M,  39},
	{PERI_CLK_12M,  39},
	{PERI_CLK_6M,   39},
	{PERI_CLK_3M,   39},	
	{PERI_CLK_24M,  625},
};



/* 说明: 上面的变化主要是基于调试模块频率的选择 
	     看选择的是多少的12M/24M/48M 来决定选择的波特率分频数值 */
/**
 * @brief UART IP3106 data bits enumeration
 *
 * The UART IP3106 can be configured for different data bits length.
 */
typedef enum euart_data_bits
{
  Uart_DataBits_5 = 0x00, 
  Uart_DataBits_6 = 0x01, 
  Uart_DataBits_7 = 0x02, 
  Uart_DataBits_8 = 0x03
} eUART_DATA_BITS;

/**
 * @brief UART IP3106 stop bits enumeration
 *
 * The UART IP3106 can be configured for different stop bit length.
 */
typedef enum euart_stop_bits
{
  Uart_StopBits_1 = 0x00, 
  Uart_StopBits_2 = 0x04 
} eUART_STOP_BITS;

/**
 * @brief UART IP3106 parity enumeration
 *
 * The UART IP3106 has the ability to configure parity checking.
 */
typedef enum euart_parity
{
  Uart_Parity_Disable  = 0x00, /* parity checking disabled */
  Uart_Parity_Odd      = 0x08, /* data+parity bit will have and odd  numer of 1s.*/
  Uart_Parity_Even     = 0x18, /* data+parity bit will have and even numer of 1s.*/
  Uart_Parity_Mark     = 0x28, /* parity is always 1 */
  Uart_Parity_Space    = 0x38  /* parity is always 0 */  
} eUART_PARITY;

typedef enum euart_mode
{
  Uart_No_Fifo_Mode		= 0x00, /* UART Work is no fifo mode */
  Uart_Fifo_Mode		= 0x01  /* UART Work is fifo mode.*/
} eUart_Mode;

typedef enum
{
	FLOW_UNSUPPORT,
	FLOW_SUPPORT
}e_UARTFLOW_CTRL;

typedef enum
{
	DMA_UNSUPPORT,
	DMA_SUPPORT
}e_UARTDMA_CTRL;


/*------------------------------------------------------------------------------
Exported Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported Function protoypes:
------------------------------------------------------------------------------*/
#if 0
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_Init
* 功能:	初始化UART, 应用层可以配置不同的UART参数
* 参数:	baudrate	-- UART的波特率
*		databits	-- 数据位数
*		stopbits	-- 停止位数
*		parity		-- 奇偶校验类型
* 返回:	HY_OK		-- 成功
*		HY_ERROR	-- 超时
*----------------------------------------------------------------------------*/
U32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId
						   );
#endif
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_txIntEnable
* 功能:	打开Uart 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntEnable(U8 uartId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_rxIntEnable
* 功能:	打开Uart 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntEnable(U8 uartId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_txIntDisable
* 功能:	关闭Uart 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntDisable(U8 uartId);

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaPause 
* 功能:	暂停 Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaPause(void);

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_Dmarestart 
* 功能:	继续暂停位置开始传输 Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_Dmarestart(void);

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaisPause 
* 功能:	判断DMA是否是暂停状态
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
BOOL hyhwmUart1_DmaisPause(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_rxIntDisable
* 功能:	关闭Uart 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntDisable(U8 uartId);
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_CheckRecDataReady(void)
* 功能:	判断是否有数据接收到
* 参数:	none
* 返回:	HY_OK(有数据），HY_ERROR(无数据）
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckRecDataReady(U8 uartId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_ReadFifo
* 功能:	从UART的fifo读出一个字节
* 参数:	none
* 返回:	读出的字节
*----------------------------------------------------------------------------*/
U32 hyhwUart_ReadFifo(U8 * data,U8 uartId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_WriteFifo
* 功能:	向UART的fifo写入一个字节
* 参数:	写入的字节
* 返回:	HY_OK		-- 成功
*		HY_ERROR	-- 超时
*----------------------------------------------------------------------------*/
U32 hyhwUart_WriteFifo(U8 data,U8 uartId);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* 功能:	设置各个串口对应的端口号
* 参数:	空
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_StateRead(void)
* 功能:	读取状态寄存器到整数中
* 参数:	空
* 返回:	状态寄存器中的值
*----------------------------------------------------------------------------*/
U32 hyhwUart_StateRead(void);


/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_sendData
* 功能:	接收数据
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_sendData(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_recvData
* 功能:	接收数据
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_recvData1
* 功能:	接收数据，该函数由高文中编写，和上面的函数功能类似，由不同模块调用
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData1(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_Initx
* 功能:	初始化UART, 应用层可以配置不同的UART参数
* 参数:	baudrate	-- UART的波特率
*		databits	-- 数据位数
*		stopbits	-- 停止位数
*		parity		-- 奇偶校验类型
* 返回:	HY_OK		-- 成功
*		HY_ERROR	-- 超时
*----------------------------------------------------------------------------*/
UInt32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId, // 映射的uart channnel
                           U8 port,    // 对应控制其的端口
                           e_UARTFLOW_CTRL isFlowSuppot, // 是否支持流控
                           e_UARTDMA_CTRL isDmaSupport  // 是否支持Dma
						   );

#ifdef __cplusplus
}
#endif

#endif /* TM_LLIC_API_UART_H_ */




