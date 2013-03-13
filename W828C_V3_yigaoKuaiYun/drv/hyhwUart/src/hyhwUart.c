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
#include "hyhwCkc.h"
#include "hyhwUart.h"
#include "hyhwGpio.h"



// Register and relative bits definition

/*UART INTERRUPT ENABLE REGISTER */	
	#define IER_BIT_EMSI            	BIT3
	#define IER_BIT_ELSI            	BIT2
	#define IER_BIT_ETXI            	BIT1
	#define IER_BIT_ERXI            	BIT0

/*UART INTERRUPT STATUS REGISTER */
	#define ISR_BIT_IID_MODEM       	~(BIT1|BIT2|BIT3)
	#define ISR_BIT_IID_TRAN       		BIT1
	#define ISR_BIT_IID_REC         	BIT2
	#define ISR_BIT_IID_ERR         	BIT1|BIT2
	#define ISR_BIT_IID_TIME        	BIT2|BIT3
	
	#define ISR_BIT_IP_PEND         	~BIT0
	#define ISR_BIT_IP_NPEND        	BIT0

/*UART_FIFO_CONTROL_REGISTER */	
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK   ~(BIT6|BIT7)
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_1	    0
	
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_4		BIT6
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_8      BIT7
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_14     BIT6|BIT7
	
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK   ~(BIT4|BIT5)
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_1		BIT4|BIT5
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_4		BIT5
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_8      BIT4
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_16     0
	
	#define FCR_BIT_DMA_MODE_NODEPEND_DISABLE   ~BIT3  /* DMA 传输依赖于RxDe和TxDe */
	#define FCR_BIT_DMA_MODE_NODEPEND_ENABLE	BIT3   /* DMA 传输使能, 不管RxDe和TxDe的值 */
	#define FCR_BIT_TX_FIFO_RESET               BIT2
	#define FCR_BIT_RX_FIFO_RESET               BIT1
	#define FCR_BIT_FIFO_ENABLE                 BIT0
	
/*UART LINE CONTROL REGISTER */		
	#define LCR_BIT_LATCH_DLAB      	BIT7
	#define LCR_BIT_SET_BREAK      	 	BIT6
	#define LCR_BIT_STICK_PARITY    	BIT5
	#define LCR_BIT_PARITY_ODD			~BIT4
	#define LCR_BIT_PARITYEVEN      	BIT4
	#define LCR_BIT_PARITY_EN       	BIT3
	#define LCR_BIT_STOP				BIT2
				
/*UART LINE STATUS REGISTER */	
	#define LSR_BIT_ERF            		BIT7
	#define LSR_BIT_TEMT     			BIT6	/* 1: Transmit FIFO and shift registers are all empty */
	#define LSR_BIT_THRE   				BIT5	/* 1: fifo has less then half data */
	#define LSR_BIT_BREAK             	BIT4
		
	#define LSR_BIT_FER             	BIT3
	#define LSR_BIT_PER             	BIT2
	#define LSR_BIT_OER            		BIT1
	#define LSR_BIT_DR              	BIT0
	
/* UART Control Register */
	#define UCR_BIT_RX_WORDACCESS_DISABLE			~BIT3   /* Rx FIFO access to word disable (1 bytes) */
	#define UCR_BIT_RX_WORDACCESS_ENABLE			BIT3	/* Rx FIFO access to word enable  (4 bytes) */
	#define UCR_BIT_TX_WORDACCESS_DISABLE			~BIT2	/* Tx FIFO access to word disable (1 bytes) */
	#define UCR_BIT_TX_WORDACCESS_ENABLE			BIT2	/* Tx FIFO access to word enable  (4 bytes) */
	#define UCR_BIT_RX_DMA_DISABLE					~BIT1	/* Rx DMA Disable */
	#define UCR_BIT_RX_DMA_ENABLE					BIT1	/* Rx DMA Enable */
	#define UCR_BIT_TX_DMA_DISABLE					~BIT0	/* Tx DMA Disable */
	#define UCR_BIT_TX_DMA_ENABLE					BIT0	/* Tx DMA Enable */
	
/* UART Modem Control Register(MCR) */

	#define MCR_BIT_RW_RTS_DISABLE_MASK		~BIT6		// RTS Deassert Condition Control Bit 掩码
	#define MCR_BIT_RW_RTS_DISABLE_RXSTOP	~BIT6		// nRTS is de-asserted at the Rx Stop Condition
	#define MCR_BIT_RW_RTS_DISABLE_RXSTART	BIT6        // nRTS is de-asserted at the Rx Start Condition
	
	#define MCR_BIT_RW_AFC_MASK				~BIT5		// Hardware Auto Flow control Enable bit mask
	#define MCR_BIT_RW_AFC_DISABLE			~BIT5		// Disable auto flow contrl 
	#define MCR_BIT_RW_AFC_ENABLE			BIT5		// Enable auto flow contrl 
	
	#define MCR_BIT_RW_LOOP_MASK			~BIT4		// loop back mode enable mask
	#define MCR_BIT_RW_LOOP_DISABLE			~BIT4		// disable local loop back feature
	#define MCR_BIT_RW_LOOP_ENABLE			BIT4		// enable local loop back feature
	
	#define MCR_BIT_RW_RTS_MASK				~BIT1		// request to send signal flag mask
	#define MCR_BIT_RW_RTS_HIGH				~BIT1		// set nRTS line to high state, 无效状态
	#define MCR_BIT_RW_RTS_LOW				BIT1		// reset  the nRTS line to low state
	
	#define MCR_BITMASK_FCC_ACTIVE_MASK		~(BIT1|BIT5) // flow control configuration
	#define MCR_BITMASK_FCC_ACTIVE_BOTH		BIT1|BIT5	 // both nCTS and nRTS automatic flow control is activated 
	#define MCR_BITMASK_FCC_ACTIVE_CTS		BIT5		 // only nCTS flow control is activated
	#define MCR_BITMASK_FCC_ACTIVE_NONE     			 // both unactived

/* UART	AFC 寄存器 */
	#define AFC_RTS_ASSERT_TRIGGER_LEVEL(i)       (i << 4)      
	#define AFC_RTS_DEASSERT_TRIGGER_LEVEL(i)      i
	
	
#define UART2_COMMUNICATION_TIMEOUT		1000//10000

// DMA2 通道1, 即channel 7 作为UART的DMA传输
#define UART1_DMA_CHANNEL			7

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaPause 
* 功能:	暂停 Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaPause(void)
{
	return hyhwDma_Pause(UART1_DMA_CHANNEL);
}
/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_Dmarestart 
* 功能:	继续暂停位置开始传输 Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_Dmarestart(void)
{
	return hyhwDma_restart(UART1_DMA_CHANNEL);
}
/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaisPause 
* 功能:	判断DMA是否是暂停状态
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
BOOL hyhwmUart1_DmaisPause(void)
{
	hyhwDma_isPause(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaEnable 
* 功能:	使能 Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaEnable(void)	
{
	return hyhwDma_Enable(UART1_DMA_CHANNEL);	
}

/*--------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaDisable 
* 功能:	disable Uart1 Dma 传输
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmUart1_DmaDisable(void)
{
	return hyhwDma_Disable(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 函数:	hyhwUart1_DmaIntClear
* 功能:	清除指定通道的DMA中断
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwUart1_DmaIntClear(void)
{
	return hyhwDma_IntClear(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaIntEnable 
* 功能:	enable dma rxtx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaIntEnable(void)	
{
	return hyhwDma_IntEnable(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 宏  :	hyhwmUart1_DmaIntDisable 
* 功能:	disable dma tx interrupt
* 参数:	none
* 返回: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmUart1_DmaIntDisable(void)
{
	return hyhwDma_IntDisable(UART1_DMA_CHANNEL);	
}
	
/*---------------------------------------------------------------------------
* 函数:	hyhwUart1_DmacheckInterruptState
* 功能:	判断是否中断使能
* 参数:	none
* 返回:	True 代表已经中断, FALSE 代表未中断
*--------------------------------------------------------------------------*/
BOOL hyhwUart1_DmacheckInterruptState(void)
{
	return hyhwDma_isDmaInterruptSet(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* 函数:	hyhwUart1_DmaGetStatus
* 功能:	获得指定通道的DMA的状态（空闲/busy）
*			先查看该通道是否启动---可用
*			数据传输完毕-----------可用
*			数据正在传输-----------不可用
* 参数:	none
* 返回:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwUart1_DmaGetStatus(void)
{
	return hyhwDma_GetStatus(UART1_DMA_CHANNEL);
}
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
UInt32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId
						   )
{
	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		switch (uartId)
		{
			case 0:
				hyhwCpm_busClkEnable(CKC_UART0);
				hyhwCpm_clearSoftReset(CKC_UART0);
				hyhwGpio_setUart0Function(0); 	/* set uart0 port0, 4线流控串口 */
				//hyhwUart_portMap(uartId, GSM_UART_PORT);
				break;
			case 1:
				hyhwCpm_busClkEnable(CKC_UART1);
				hyhwCpm_clearSoftReset(CKC_UART1);
				hyhwGpio_setUart1Function(0); 	/* set uart1 port1,  2线串口 */
				//hyhwUart_portMap(uartId, BT_UART_PORT);
				break;
			case 2:
				hyhwCpm_busClkEnable(CKC_UART2);
				hyhwCpm_clearSoftReset(CKC_UART2);
				hyhwGpio_setUart2Function(); 	/* set uart2 port2, 2线串口 */
				//hyhwUart_portMap(uartId, GPS_UART_PORT);
				break;
			case 3:
				hyhwCpm_busClkEnable(CKC_UART3);
				hyhwCpm_clearSoftReset(CKC_UART3);
				hyhwGpio_setUart3Function(); 	/* set uart3 port5, 2线串口 */
				//hyhwUart_portMap(uartId, STM102_UART_PORT);
				break;
		}
		
		hyhwCpm_setUartClk(uartId, PERI_CLK_48M); 				/* set uart clk */
			
			
		/* set baudrate  */
		/* Set the `Divisor Latch Access Bit` and enable so the DLL/DLM access*/
		/* Initialise the `Divisor latch LSB` and `Divisor latch MSB` registers */
		
		divisor		= sUartDivisor[baudrate];
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
		ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
		ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
		ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
		
	//	ptUart_Reg->UART_CONTROL 	  |= (UCR_BIT_RX_DMA_ENABLE|UCR_BIT_TX_DMA_ENABLE);
		
		if(uartMode == Uart_Fifo_Mode)
		{	
			 ptUart_Reg->FIFO_CONTROL = FCR_BIT_FIFO_ENABLE;   // 使能FIFO位
			//ptUart_Reg->FIFO_CONTROL &= ~FCR_BIT_FIFO_ENABLE;   // 使能FIFO位
			
		}	 
	
		ptUart_Reg->FIFO_CONTROL    &= 	FCR_BIT_DMA_MODE_NODEPEND_DISABLE;
		
#if 0
		/* 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function Start */
		
	//	ptUart_Reg->MODEM_CONTROL 	  |= BIT1;				// Reset the nRTS line to low state
	//	ptUart_Reg->MODEM_CONTROL 	  |= BIT5;				// Auto Flow Control Enable
//		ptUart_Reg->MODEM_CONTROL 	  &= ~BIT4;				// Disable LoopBack

		ptUart_Reg->FIFO_CONTROL 	  |= (BIT7|BIT4);		// TX FIFO Trigger Level(8 Bytes)
		ptUart_Reg->FIFO_CONTROL 	  &= ~(BIT3|BIT6|BIT5);
		
		ptUart_Reg->UART_CONTROL 	  &= ~BIT3;				// Rx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= ~BIT2;				// Tx FIFO access to byte
//		ptUart_Reg->UART_CONTROL 	  |= BIT1;				// Rx DMA enable
//		ptUart_Reg->UART_CONTROL 	  |= BIT0;				// Tx DMA enable
		/*- 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function End -*/

#else 
		/* 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function Start */
		/*
		AFE		RTS			Flow Control Configuration
		1		1		:	Both nCTS and nRTS automatic flow control is activated
		1		0		:	Only nCTS automatic flow control is activated
		0		X		:	Both nCTS and nRTS automatic flow control is de-activated
							(Same as TCC77x series, only support for S/W Flow Control)
		*/
		
		/* 只有uart0 和 uart1需要流控, 因此在此处需要加以区分 */
		
		if(uartId == UART0 || uartId == UART1)
		{
			// 需要流控控制
			// enable both CTS and RTS auto flow control
			ptUart_Reg->MODEM_CONTROL 	  &= MCR_BITMASK_FCC_ACTIVE_MASK;	// 位清零操作
			ptUart_Reg->MODEM_CONTROL     |= MCR_BITMASK_FCC_ACTIVE_BOTH;   // 赋值操作, 使能发送和接受流控
			
			/* 说明: 该值填写什么时候流控生效, 举例来说, 在流控情况下, 
			   当设成4和14时, 当dataready以后,将流控信号置高(无效时)
			   可以去读取缓冲区的数据, 共可以读出15个字节, 当读取数据到4时流控再次生效 
			   当设置成0和8时, dataready以后, 将流控置为无效, 共可以读出9个字节, 
			   读取到生0个字节时, 流控才会生效 
			   因此这里设置0/0的意思, 就是来一个字节就流控生效, 不再发送第二个数据, 等待这个数据读出以后
			   流控才自动生效, 这个在设置DMA时是非常有效的 */
			
			// disable loopback mode
			ptUart_Reg->MODEM_CONTROL     &= MCR_BIT_RW_LOOP_MASK;			// 位清零操作
			ptUart_Reg->MODEM_CONTROL 	  &= MCR_BIT_RW_LOOP_DISABLE;		// 赋值操作

			if(uartId == UART0)
			{
				// 一般流控串口使用, 用于ULC2, 设置14个字节中断
				ptUart_Reg->FIFO_CONTROL = ( FCR_BIT_FIFO_ENABLE             // FIFO 模式
				                            |FCR_BITMASK_RX_TRIGGRE_LEVEL_8  // triggerlevel -- 满足多少个字节进行DMA传输
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_8
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );
				
				
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA 不使能
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA 不使能
			}
			else
			{
				// UART1 用于tg3 3M 通信, 设置UART串口中断触发为1个字节
				// RX FIFO trigger level / Tx FIFO trigger level set, 设置一个字节触发
				/*
				ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK;
				ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_RX_TRIGGRE_LEVEL_1;
				
				ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK;
				ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_TX_TRIGGRE_LEVEL_1;
				
				*/
				ptUart_Reg->AFC_TRIGGER        = AFC_RTS_ASSERT_TRIGGER_LEVEL(0)
			                                | AFC_RTS_DEASSERT_TRIGGER_LEVEL(0);  // 设置触发流控的triggerlevel为14
				// 一般流控串口使用, 用于ULC2, 设置14个字节中断
				ptUart_Reg->FIFO_CONTROL = ( FCR_BIT_FIFO_ENABLE             // FIFO 模式
				                            |FCR_BITMASK_RX_TRIGGRE_LEVEL_1  // triggerlevel -- 满足多少个字节进行DMA传输
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_1
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );
				
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA 使能
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA 使能
			}
			//ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_TX_FIFO_RESET;
			//ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_RX_FIFO_RESET;
		}	
		else
		{
			// 不需要流控	// RX FIFO trigger level / Tx FIFO trigger level set, 设置8个字节触发
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK;
			ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_RX_TRIGGRE_LEVEL_14;
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK;
			ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_TX_TRIGGRE_LEVEL_8;
			
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_DMA_MODE_NODEPEND_DISABLE; // DMA 传输使能, 不依赖于RxDe 和 TxDe
			
			
		}
		
		ptUart_Reg->UART_CONTROL 	  &= UCR_BIT_RX_WORDACCESS_DISABLE;				// Rx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= UCR_BIT_TX_WORDACCESS_DISABLE;				// Tx FIFO access to byte
#endif
		// wait the setting is OK
	  	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
		    
		/* Line Control register, preparing for transmit and receive data
		   Clear the `Divisor Latch Access Bit (DLAB)*/	
		ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;
	} 
			
	return HY_OK;	

}
#endif
void hyhwUart_flowCtrl(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	ptUart_Reg->MODEM_CONTROL 	  |= (BIT1|BIT5);
}

void hyhwUart_DmaEffect(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	ptUart_Reg->FIFO_CONTROL 	  &= 0;
	
	ptUart_Reg->FIFO_CONTROL 	  |= 3;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_txIntEnable
* 功能:	打开Uart tx 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntEnable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	
		ptUart_Reg->INT_ENABLE|=(IER_BIT_ETXI);
	}
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_rxIntEnable
* 功能:	打开Uart rx 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntEnable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	
		ptUart_Reg->INT_ENABLE|=(IER_BIT_ERXI);
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_txIntDisable
* 功能:	关闭Uart tx 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntDisable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->INT_ENABLE &=~(IER_BIT_ETXI);
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_rxIntDisable
* 功能:	关闭Uart rx 中断使能
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntDisable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->INT_ENABLE &=~(IER_BIT_ERXI);
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_CheckRecDataReady(void)
* 功能:	判断是否有数据接收到
* 参数:	none
* 返回:	HY_OK(有数据），HY_ERROR(无数据）
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckRecDataReady(U8 uartId)
{
	/* read the character */
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);

		if(ptUart_Reg->LINE_STATUS & LSR_BIT_DR)
		{
			rc = HY_OK;
		}
		else
		{
			rc = HY_ERROR;
		}	
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_ReadFifo
* 功能:	从UART的fifo读出一个字节
* 参数:	none
* 返回:	读出的字节
*----------------------------------------------------------------------------*/
U32 hyhwUart_ReadFifo(U8 *data,U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);

	*data =(ptUart_Reg->RECEIVE);
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_WriteFifo
* 功能:	向UART的fifo写入一个字节
* 参数:	写入的字节
* 返回:	HY_OK		-- 成功
*		HY_ERROR	-- 超时
*----------------------------------------------------------------------------*/
U32 hyhwUart_WriteFifo(U8 data,U8 uartId)
{
	/* wait until the transmit fifo has less than half data	 */	
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		while(!(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE));
		ptUart_Reg->TRANSMIT	=	data ;	
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* 功能:	设置各个串口对应的端口号
* 参数:	空
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
* 注意: 4个参数不可以相等
*----------------------------------------------------------------------------*/
U32 hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
{
	U32 regBuffer;

	if(port0 > 5 || port1 > 5|| port2 > 5|| port3 > 5)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if(port0 == port1 || port0 == port2 || port0 == port3
		|| port1 == port2 || port1 == port3 || port2 == port3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	regBuffer = (port0 
				| port1 << 4
				| port2 << 8
				| port3 << 12
				);
				
	UART_MUX_CHSEL = regBuffer;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* 功能:	设置各个串口对应的端口号
* 参数:	空
* 返回:	HY_OK		-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
* 注意: 寄存器中的4个channel映射值不可以相同
*----------------------------------------------------------------------------*/
U32 hyhwUart_portMap(U32 uart, U32 port)
{
	U32 regBuffer;
	U32 offset;
	
	regBuffer = UART_MUX_CHSEL;
	offset = 4*uart;
	
	regBuffer &= ~(0x7<<offset);
	regBuffer |= port << offset;
				
	UART_MUX_CHSEL = regBuffer;
	
	return HY_OK;
}

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
UInt32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId, 					 // 映射的uart channnel
                           U8 port,   					 // 对应控制其的端口
                           e_UARTFLOW_CTRL isFlowSuppot, // 是否支持流控
                           e_UARTDMA_CTRL isDmaSupport  // 是否支持Dma
						   )
{
	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		switch (uartId)
		{
			case 0:
				hyhwCpm_busClkEnable(CKC_UART0);
				hyhwCpm_clearSoftReset(CKC_UART0);
				break;
			case 1:
				hyhwCpm_busClkEnable(CKC_UART1);
				hyhwCpm_clearSoftReset(CKC_UART1);
				break;
			case 2:
				hyhwCpm_busClkEnable(CKC_UART2);
				hyhwCpm_clearSoftReset(CKC_UART2);
				break;
			case 3:
				hyhwCpm_busClkEnable(CKC_UART3);
				hyhwCpm_clearSoftReset(CKC_UART3);
				break;
		}
		switch (port)
		{
			case 0:
				hyhwGpio_setUart0Function(0); 	/* set uart0 port0, 4线流控串口 */
				break;
			case 1:
				hyhwGpio_setUart1Function(0);	/* set uart0 port0, 4线流控串口 */
				break;
			case 2:
				hyhwGpio_setUart2Function(); 	/* set uart2 port2, 2线串口 */
				break;
			case 3:
				hyhwGpio_setUart3Function(); 	/* set uart3 port3, 2线串口 */
				break;
			case 4:
				hyhwGpio_setUart4Function(); 	/* set uart4 port4, 2线串口 */
				break;
			case 5:
				hyhwGpio_setUart5Function(); 	/* set uart5 port5, 2线串口 */
				break;
		}
		
		hyhwCpm_setUartClk(uartId, g_arrayBaudRate[baudrate].uart_clk); 	/* set uart clk */
		hyhwUart_portMap(uartId, port);				/* 把对应的控制器channel映射到对应的端口 */
			
	
		divisor = g_arrayBaudRate[baudrate].sDivisor;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
		ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
		ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
		ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
		
		if(uartMode == Uart_Fifo_Mode)
		{	
			 ptUart_Reg->FIFO_CONTROL  = FCR_BIT_FIFO_ENABLE;   // 使能FIFO位	
			 ptUart_Reg->FIFO_CONTROL |=  ( FCR_BITMASK_RX_TRIGGRE_LEVEL_8//FCR_BITMASK_RX_TRIGGRE_LEVEL_14  // triggerlevel -- 满足多少个字节进行DMA传输
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_8
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );		
		}	 
		
		// disable loopback mode
		ptUart_Reg->MODEM_CONTROL     &= MCR_BIT_RW_LOOP_MASK;			// 位清零操作
		ptUart_Reg->MODEM_CONTROL 	  &= MCR_BIT_RW_LOOP_DISABLE;		// 赋值操作
		
		// 流控设置
		ptUart_Reg->MODEM_CONTROL 	  	  &= MCR_BITMASK_FCC_ACTIVE_MASK;	// 位清零操作
		if( isFlowSuppot == FLOW_SUPPORT)
		{
			// enable both CTS and RTS auto flow control
			ptUart_Reg->MODEM_CONTROL     |= MCR_BITMASK_FCC_ACTIVE_BOTH;   // 赋值操作, 使能发送和接受流控
			ptUart_Reg->AFC_TRIGGER        = AFC_RTS_ASSERT_TRIGGER_LEVEL(0)
			                                | AFC_RTS_DEASSERT_TRIGGER_LEVEL(0);  // 设置触发流控的triggerlevel为14
		}
		
		// DMA设置
		ptUart_Reg->FIFO_CONTROL   		  &= FCR_BIT_DMA_MODE_NODEPEND_DISABLE;
		if( isDmaSupport == DMA_SUPPORT)
		{
			ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA 使能
			ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA 使能
		}
		
		// Rx FIFO access to byte // Tx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= (UCR_BIT_RX_WORDACCESS_DISABLE |UCR_BIT_TX_WORDACCESS_DISABLE);
				

		// wait the setting is OK
	  	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
		    
		/* Line Control register, preparing for transmit and receive data
		   Clear the `Divisor Latch Access Bit (DLAB) */	
		ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;
	} 
			
	return HY_OK;	

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_StateRead()
* 功能:	读取状态寄存器到整数中
* 参数:	空
* 返回:	状态寄存器中的值
*----------------------------------------------------------------------------*/
U32 hyhwUart_StateRead(void)
{
	return UART_MUX_CHST;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_CheckTransmitFifoEmpty
* 功能:	检查UART的发送fifo是否为空
* 参数:	写入串口id
* 返回:	Fifo的状态  
*			1	-- fifo为空； 
*			0	-- fifo不为空；
*			HY_ERR_BAD_PARAMETER -- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckTransmitFifoEmpty(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	}
	
	return (ptUart_Reg->LINE_STATUS & LSR_BIT_TEMT);
}	


#if 0
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_Ch1Config
* 功能:	配置UART1的流控控制, 用于传输数据
* 参数:	none
* 返回: none
* 备注: uart1 3M,使用DMA接收，同时设置uart 接收1个byte中断方式，中断服务程序
		中停止uart中断，并启动一个timer定时中断(暂定5ms)，产生定时中断时，读
		取DMA当前的写位置，将DMA已经接收到的数据copy出来，并重新配置DMA，停止
		该timer。如果产生DAM中断，则将已启动的timer停掉，并重新启动1byte的uart
		中断。DMA配置可暂为：8k bytes内存循环使用，2k产生中断。
*----------------------------------------------------------------------------*/
U32 hyhwUart_Ch1Config(
						   eUART_BAUDRATE   baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode
                      )
{

	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	U32 uartReg;
	
	/* 
	
		配置串口流控控制, 并设置波特率以及其他参数
	*/
	hyhwGpio_setUart0Function(); 	/* set uart1 port */
	hyhwCpm_setUart0Clk();
			
	/* set baudrate  */
	/* Set the `Divisor Latch Access Bit` and enable so the DLL/DLM access*/
	/* Initialise the `Divisor latch LSB` and `Divisor latch MSB` registers */
	
	divisor		= sUartDivisor[baudrate];
	ptUart_Reg 	= (tUART_REGISTER*)UART1_3M_TDADDR;
	
	ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
	ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
	ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
	ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
	
	// 设置缓冲区控制寄存器, 配置UART接收中断为1个字节中断
	uartReg    = ptUart_Reg->FIFO_CONTROL;
	uartReg   |= (  //FCR_BIT_FIFO_ENABLE       // FIFO 控制使能，因为要进行DMA传输
	             // | FCR_BIT_TXRX_ENABLE       // 设置DMA启动与RxDe/TxDe的关系
	               FCR_BIT_TXFIFO_RES        // reset FIFO计数器和缓冲区
	              | FCR_BIT_RXFIFO_RES
	             );
//	uartReg   &= FCR_BIT_RXTL_1;              // RxFIFO trigger level 为1个字节
	uartReg   |= FCR_BIT_RXTL_8;              // RxFIFO trigger level 为1个字节
	ptUart_Reg->FIFO_CONTROL       = uartReg; 

	
	// 设置流控
/*	ptUart_Reg->MODEM_CONTROL 	  |= BIT5;				// Auto Flow Control Enable
	ptUart_Reg->MODEM_CONTROL 	  &= ~BIT4;				// Disable LoopBack
	ptUart_Reg->MODEM_CONTROL 	  &= ~BIT1;
    
	ptUart_Reg->FIFO_CONTROL 	  |= (BIT7|BIT4);		// TX FIFO Trigger Level(8 Bytes)
	ptUart_Reg->FIFO_CONTROL 	  &= ~(BIT3|BIT6|BIT5);
	
	uartReg    = ptUart_Reg->UART_CONTROL;
	uartReg	  &= ~BIT3;				// Rx FIFO access to word (1 bytes)
	uartReg   &= ~BIT2;				// Tx FIFO access to word (1 bytes)
	uartReg   |= BIT1;				// Rx DMA enable
	uartReg	  |= BIT0;				// Tx DMA enable
	ptUart_Reg->UART_CONTROL      = uartReg;
*/	
	// wait the setting is OK
	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
	    
	/* Line Control register, preparing for transmit and receive data
	   Clear the `Divisor Latch Access Bit (DLAB)*/	
	ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;

return HY_OK;	
}
#endif
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_CheckInterrupt
* 功能:	判断指定uartId是否产生中断
* 参数:	uartId, 注: 这个id 是串口端口id, UART0/1/2/3
* 返回: none
* 示例: hyhwUart_CheckInterrupt(UART0);	
*----------------------------------------------------------------------------*/
BOOL hyhwUart_CheckInterrupt(U8 uartId)
{
	U32 tempVal;
	
	tempVal = UART_MUX_CHST;
	if(tempVal & (1<<uartId)) // bit1 为UART1的中断标志位, 现在默认chanle1 映射到UART1
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_sendData
* 功能:	接收数据
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_sendData(U8 uartId, void *pdata, U32 len)
{
	/* wait until the transmit fifo has less than half data	 */	
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut = 0;
	volatile U32 *pStatus, *pTransmit;
	
	pDataPnt = (U8 *)pdata;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);
		
		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_THRE))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return HY_ERROR;
					//break;
				}
			}
			
			*pTransmit = pDataPnt[i];
		}
	}
	return rc;

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_recvData
* 功能:	接收数据
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData(U8 uartId, void *pdata, U32 len)
{
	tUART_REGISTER *ptUart_Reg;
	//U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut;
	volatile U32 *pStatus, *pTransmit;

	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		pDataPnt = (U8 *)pdata;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);

		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_DR))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return HY_ERROR;
					//break;
				}
			}
			pDataPnt[i] = *pTransmit;
		}
	}
	return HY_OK;

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_recvData1
* 功能:	接收数据，该函数由高文中编写，和上面的函数功能类似，由不同模块调用
* 参数:	uartId------注: 这个id 是串口端口id, tg3应用中可选参数
					UART0/UART1/UART2, 其他通道号用不到。
		pdata ------数据缓冲区指针
		len   ------接收数据长度
* 返回: 接收数据长度	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData1(U8 uartId, void *pdata, U32 len)
{
	tUART_REGISTER *ptUart_Reg;
	//U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut;
	volatile U32 *pStatus, *pTransmit;

	
	if(uartId >UART_NUM)
	{
		return (-HY_ERR_BAD_PARAMETER);
	}
	else
	{
		pDataPnt = (U8 *)pdata;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);

		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_DR))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return i;
					//return HY_ERROR;
					//break;
				}
			}
			pDataPnt[i] = *pTransmit;
		}
	}
	return len;

}

/* 下面的程序为测试程序，暂时留着，过后删除 */

#if 0

#define BUFFSIZE	(8*1024) // UART1 DMA 缓冲区大小 8k
#define INTBUFFSIZE (2*1024) // UART1 DMA 中断缓冲区大小2k

U32 lastIntDstAddr = 0;
U32 lastIntSrcAddr = 0;

U32 writepos	   = 0;
U32 readpos        = 0;
U32 countBytes     = 0; // 计算此时缓冲区中的有效字节数
U32 lastDealedBytes = 0;
U32 readAddr;
U8 *pdataBufAddr;
U8 *pDmaRecvBufAddr;

U8 *data;
/* UART1的中断服务函数 */
void isr_Uart1(void)
{
	
	// 清中断, 这个由读数据自动清除中断标记, 待测
	//hyhwUart_rxIntDisable(0);			// 禁止UART1接收中断
	//hyhwTimer_SetFrequency(2, 5000); 	// 设置定时器, 5ms 中断一次, 启动定时中断
	//hyhwDma_Uart1DmaEnable();	        // 启动UART1 DMA传输
	hyhwUart_rxIntDisable(0);
	hyhwUart_ReadFifo(&data, 0);
	hyhwUart_WriteFifo('a', 0);
	hyhwUart_rxIntEnable(0);
	
}

/* 上面函数中设置的定时器中断 */
void isr_Timer2_count(void)
{	
	U32 wAddr = 0;
	hyhwmTimer0_clearIntFlag();
	// 获取DMA当前接收位置
	hyhwmUart1_DmaDisable();
	wAddr = hyhwDma_GetCurrentDstAddr(UART1_DMA_CHANNEL);
	// 拷贝数据到目的地址, 上次DMA中断时的地址到wAddr这个地址的数据
	
	if(wAddr > lastIntDstAddr)
	{
		memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, ((U32)wAddr-(U32)lastIntDstAddr));
	}
	else
	{
		U32 count = (U32)pDmaRecvBufAddr - (U32)lastIntDstAddr;
		memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, count);
		memcpy(pdataBufAddr+count, (U8 *)pDmaRecvBufAddr, (U32)wAddr-(U32)pDmaRecvBufAddr);
	}

}

// DMA 中断来了
void isr_Dma_Uart1(void)
{
	lastDealedBytes = INTBUFFSIZE;
/*	hyhwUart1_DmaIntClear();   // 清除中断标志
	hyhwTimer_intDisable(2);    // 关闭定时器中断
	hyhwmUart1_DmaDisable();   // 停掉DMA传输
#if 0	
	hyhwUart_rxIntDisable(1);  // 启动UART1的单字节接收中断
#else
	hyhwUart_rxIntDisable(0);  // 启动UART1的单字节接收中断	
#endif	
	memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, INTBUFFSIZE);
	AdjustBufferParameters();*/
}

void InitBufferParameters(U32 pBufAddr, U32 pDmaBufAddr)
{
	pdataBufAddr   = (U8 *)pBufAddr;
	pDmaRecvBufAddr= (U8 *)pDmaBufAddr;
	lastIntDstAddr = 0;
//	lastIntSrcAddr = 0;
	
	writepos	   = 0;
	readpos        = 0;
//	countBytes     = 0; // 计算此时缓冲区中的有效字节数
	
	lastDealedBytes = 0;
	
}
void AdjustBufferParameters(void)
{
	writepos += lastDealedBytes;
	if(writepos> BUFFSIZE)
	{
		writepos -= BUFFSIZE;
	}
	lastIntSrcAddr  = pDmaRecvBufAddr + writepos;
	
	readpos  += lastDealedBytes;
	if(readpos > BUFFSIZE)
	{	
		readpos -= BUFFSIZE;
	}
	
	lastIntDstAddr  = pDmaRecvBufAddr + readpos;
}

/*----------------------------------------------------------------------------

*-----------------------------------------------------------------------------*/
void hyhwDmaSubsys_Init(U8 *buffer, U32 bufSizeInSamples)
{
	/* 
	U8	*pBuffer;		
	U16	writePos;		/*
	U16	readPos; 		/*
							
	U32	sizeInBytes;	/*
	U16	curBytes;		/*
	U16	maxInputSamples;/*
	U16	lastDealedSamples;*/
/*    _tDMA_ROLL_BUFF_MANAGER  *pDmaBuffer;
	pDmaBuffer = &rxBuffer;
	
	 Initialise play buffers 
	pDmaBuffer->pBuffer			= buffer;
	
	pDmaBuffer->sizeInBytes	    = bufSizeInSamples;
	pDmaBuffer->writePos			= 0;
	pDmaBuffer->readPos			= 0;
	pDmaBuffer->maxInputSamples	= (U16)(bufSizeInSamples/4);
	pDmaBuffer->curBytes		= 0;
	pDmaBuffer->lastDealedSamples = 0;*/
	
	/*	初始化 COI (codec interface) 硬件 
	  	设置CODEC fifo 模式，AD/DA切换时必须清除buffer， 方法是同时disable TX 和RX 
	  	同时，必须清loop 位，这位设置时，fifo被disable，且中断也被禁止*/
	
	/*	禁止所有中断的产生 */
	

}


// 测试程序用例, for UART1 串口
void test_Uart1_TDMOD(void)
{

	U8 dataSend;
	U8 dataRecv;
	U32 i;
	char *dataSend1 = "I Love family home ";


	hyhwUart_Init( Uart_Baudrate_115200,//Uart_Baudrate_3000000,
	                    Uart_DataBits_8,
	                    Uart_StopBits_1,
	                    Uart_Parity_Disable,
	                    Uart_Fifo_Mode,
	                    0
				      );
	


	// 配置串口的传输速率
	hyhwUart_Ch1Config( Uart_Baudrate_115200,//Uart_Baudrate_3000000,
	                    Uart_DataBits_8,
	                    Uart_StopBits_1,
	                    Uart_Parity_Disable,
	                    Uart_Fifo_Mode
				      );
	hyhwInt_ConfigUart1_ISR(isr_Uart1);		
	hyhwInt_ConfigTimer2_ISR(isr_Timer2_count);	
	
	hyhwmUart1_DmaIntDisable();
	hyhwUart1_DmaIntClear();
#if 0	
	hyhwUart_txIntDisable(1);
	hyhwUart_rxIntDisable(1);
#else
	hyhwUart_txIntDisable(0);
	hyhwUart_rxIntDisable(0);
#endif	
	hyhwmUart1_DmaDisable();

/* 打开总中断 */	
	hyhwInt_setTriggerLevel(INT_BIT_UART);
	hyhwInt_setTriggerLevel(INT_BIT_DMA);
	hyhwInt_setIRQ(INT_BIT_UART);
	hyhwInt_setIRQ(INT_BIT_DMA); 
	hyhwInt_enable(INT_BIT_UART);
	hyhwInt_enable(INT_BIT_DMA);
	     
	// 启动UART中断	
#if 0	
	hyhwUart_rxIntEnable(1);
#else
	hyhwUart_rxIntEnable(0);
#endif	
	// DMA配置, DMA 中断
	// 初始化缓冲区
	InitBufferParameters((U8 *)0x20300000, (U8 *)0x20100000);
	hyhwDma_Uart1Config(1, BUFFSIZE, 0x20100000, INTBUFFSIZE);
#if 0	
	// 接收一个数据引起中断, 自己收发
	for(i=0; i<10; i++)
	if( HY_OK == hyhwUart_WriteFifo(dataSend, 1))
	{
		hyhwUart_ReadFifo(&dataRecv, 1);	
	}
#else
/*	// 接收一个数据引起中断, 自己收发
	for(i=0; i<20; i++)
	{
		dataSend = dataSend1[i];
		hyhwUart_WriteFifo(dataSend, 0);
	}
*/	
	//hyhwUart_ReadFifo(&dataRecv, 0);
#endif		
	while(1);
	hyhwUart_ReadFifo(&dataRecv, 0);;


}
#endif


/*-----------------------------------------------------------------------------
*	修改记录 1 :
20090817	杨立敬
问题:
	1\根据前面工作的反馈对串口部分进行以下修改
	添加了
	#define TD_DATA_UART		UART1	// 流控控制,  用于td数据通信使用的UART1, 3M速度通信
	#define TD_AT_STRUCT_UART 	UART2 // 一般串口
	#define GSM_UART			UART0   // 支持流控    

	#define GSM_UART_ADDR		 	TCC7901_UART_BASEADDR0 	
	#define TD_DATA_UART_ADDR		TCC7901_UART_BASEADDR1
	#define TD_AT_STRUCT_UART_ADDR 	TCC7901_UART_BASEADDR2
	删除了
	UART1_3M_TDADDR定义，并在函数调用时进行相应修改
	2\按照高文中要求修改了初始化的始终配置
-------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	修改记录 2 :
20090819	杨立敬
	添加了函数:
	hyhwmUart1_Dmarestart;hyhwmUart1_DmaisPause;
	主要是为了让UART1 在DMA时能够暂停，而不是停止DMA重新配置，暂停后再重新启动
	可以接着原来的地方进行传输。
	
	串口初始化函数没有更改接口，但根据硬件电路进行了分别配置, 比以前程序增加了串口流控和DMA是否使能控制
	
	另外为串口部分高文中的写的函数做了注释。
-------------------------------------------------------------------------------*/
/* 20100513
	重新编写串口初始化函数，因为102使用的串口需要用到端口5,即uart5，为了方便以后需改，重新编写了初始化函数
	增加了3个参数，确定是否支持mda和flow ctrl. 注意uart1和uart5是共用cts和rts引脚的，因此GPIO中引脚也作了修改
	所有调用初始化函数的地方需要作修改，明确是否需要流控河DMA
*//* 20100513
	重新编写串口初始化函数，因为102使用的串口需要用到端口5,即uart5，为了方便以后需改，重新编写了初始化函数
	增加了3个参数，确定是否支持mda和flow ctrl. 注意uart1和uart5是共用cts和rts引脚的，因此GPIO中引脚也作了修改
	所有调用初始化函数的地方需要作修改，明确是否需要流控河DMA
*/