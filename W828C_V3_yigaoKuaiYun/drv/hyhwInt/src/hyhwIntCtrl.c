
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
  
#include "hyhwChip.h"
#include "hyhwIntCtrl.h"

#ifdef __cplusplus
  /* Assume C declarations for C++ */
  extern "C" {
#endif

/*---------------------------------------------------------------------------
   Global variables:
   --------------------------------------------------------------------------*/


isr_function pisr_timer0[ISR_TIMER0_NUM];


/*  由于只有 Audio Subsystem (codec) 的handler可能会变，所以只用一个函数指针即可
	在视频情况下，音频的解码也是通过中断处理，所以也有一个函数指针，当这个函数
	指针不为零时，就进行音频解码 */
void (* isr_audio_codec ) ( void );
void (* isr_audio_decoder ) ( void );
void (* isr_audio_encoder )(void);
void (* isr_timer0)(void);
void (* isr_timer1)(void);
void (* isr_timer2)(void);
void (* isr_timer3)(void);
void (* isr_timer5)(void);
void (* isr_key_scan_timer0)(void);//按键扫描

void (* isr_gpio)(void);
void (* isr_uart0)(void);
void (* isr_uart1)(void);
void (* isr_uart2)(void);
void (* isr_uart3)(void);
void (* isr_sdio0)(void);
void (* isr_sdio1)(void);
void (* isr_usb)(void);
void (* isr_enable_sdio)(void);

void (* isr_camera)(void);

void (* isr_ppp_time)(void);//ppp协议的定时器

void (* isr_uartDma)(void);	// 这两个函数用到DMA传输
void (* isr_spiDma)(void);
void (* isr_spi)(void);
void (* isr_rtc)(void);//rtc
void (* isr_adc)(void);//rtc
void (* isr_afs_check)(void);
void (* isr_spiTDBoot)(void);
void (* isr_spiTDPcm)(void);
void (* isr_spiGsmPcm)(void);
void (* isr_usbHost)(void);//usb host stack ISR 20100427
//void (* isr_lcdDmaShow)(void);//RGB lcd show

void (* isr_scanner_frame)(void);
void (* isr_scanner_data)(void);

static void hyhwInt_ISR_null ( void );


/*---------------------------------------------------------------------------
   exported function:
   --------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_Init
* 功能:	初始化，禁止所用中断
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_init( void )
{
	int i;
	
	/*初始化时，所有的中断禁止 */
	INT_ENABLE_REGISTER	= 0;
	hyhwInt_clear(0xFFFFFFFF);
	
	/*需要打开总的中断*/
		 
	 
	for(i=0; i<ISR_TIMER0_NUM; i++)
		pisr_timer0[i] = NULL;
	 
	 
	/*	为安全起见，为isr_audio_codec 赋个初值 */
	isr_audio_codec		= hyhwInt_ISR_null;
	
	/* 这个要判断是否是空才调用 */
	isr_audio_decoder	= NULL;
	isr_audio_encoder   = NULL;
	
	isr_timer1			= hyhwInt_ISR_null;
	isr_timer3			= hyhwInt_ISR_null;
	isr_key_scan_timer0	= NULL;
	isr_gpio			= NULL;
	isr_uart0			= NULL;
	isr_uart1			= NULL;
	isr_uart2			= NULL;
	isr_uart3			= NULL;
	isr_sdio0			= NULL;
	isr_sdio1			= NULL;
	isr_usb				= NULL;
	isr_enable_sdio     = NULL;
	isr_timer5          = NULL;
	
	isr_camera			= NULL;
	
	isr_ppp_time        = NULL;
	
	isr_spi				= NULL;
	isr_rtc               =NULL;
	isr_adc	            = NULL;
	isr_afs_check       = NULL;
	isr_uartDma		    = NULL;
	isr_spiDma			= NULL;
	
	isr_spiTDBoot		= NULL;
	isr_spiTDPcm        = NULL;
	isr_spiGsmPcm       = NULL;
	isr_usbHost           =NULL; //20100427
	
	isr_timer0			= NULL;
	
	//isr_lcdDmaShow      = NULL ;
	return ;
	
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUsbHost_ISR  20100427
* 功能:	设置 usb Host 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUsbHost_ISR(  void (*pIntVector) (void))
{
	isr_usbHost = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_LcdDmaShow_ISR
* 功能:	设置 RGB送屏 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
/*void hyhwInt_LcdDmaShow_ISR(  void (*pIntVector) (void))
{
	isr_lcdDmaShow = pIntVector;
}*/


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_disableAll
* 功能:	禁止所有中断
* 参数:	group			--  A组中断还是B组中断
*		intSourceMask	--  相应的中断位，可以是多个中断的或值
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_disableAll( void )
{
	/*  抄来的，是否HwMREQ 是MREQA，需要核实 什么意思 
	unsigned long __HwMREQ
	
	HwIEN	= 0 ;

		__HwMREQ = HwMREQ;				// Flush the interrupt Queue
		HwIEN	 = 0x0;		//&= ~HwINT_MEN;		
		HwMREQ	 = __HwMREQ;
	}  */

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigCodecISR
* 功能:	由于现在Audio Subsubsystem 在播放、录音情况下有不同的ISR函数，并且，播放
*		情况不同，ISR也会有不同，所以，需要设置AudioSubsytem的ISR函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCodecISR(  void (*pIntVector) (void))
{
	isr_audio_codec = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigAudioDecISR
* 功能:	播放解码器的中断函数
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioDecISR(  void (*pIntVector) (void))
{
	isr_audio_decoder = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigAudioEncISR
* 功能:	播放编码器的中断函数
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioEncISR(  void (*pIntVector) (void))
{
	isr_audio_encoder = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer0Hw_ISR
* 功能:	设置 timer1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer0Hw_ISR(  void (*pIntVector) (void))
{
	isr_timer0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer1_ISR
* 功能:	设置 timer1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer1_ISR(  void (*pIntVector) (void))
{
	isr_timer1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer2_ISR
* 功能:	设置 timer2 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer2_ISR(  void (*pIntVector) (void))
{
	isr_timer2 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer3_ISR
* 功能:	设置 timer2 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer3_ISR(  void (*pIntVector) (void))
{
	isr_timer3 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer5_ISR
* 功能:	设置 timer5 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer5_ISR(  void (*pIntVector) (void))
{
	isr_timer5 = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigKeyScanTimer0_ISR
* 功能:	设置 KeyScanTimer0 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigKeyScanTimer0_ISR(  void (*pIntVector) (void))
{
	isr_key_scan_timer0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigGpio_ISR
* 功能:	设置 Gpio 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigGpio_ISR(  void (*pIntVector) (void))
{
	isr_gpio = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart0_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart0_ISR(  void (*pIntVector) (void))
{
	isr_uart0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart1_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart1_ISR(  void (*pIntVector) (void))
{
	isr_uart1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart2_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart2_ISR(  void (*pIntVector) (void))
{
	isr_uart2 = pIntVector;
}


#include "hyhwUart.h"
/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart_ISR
* 功能:	设置 Uart 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart_ISR(U8 id,  void (*pIntVector) (void))
{
	switch(id)
	{
	case UART0:
		isr_uart0 = pIntVector;
		break;
	case UART1:
		isr_uart1 = pIntVector;
		break;
	case UART2:
		isr_uart2 = pIntVector;
		break;
	case UART3:
		isr_uart3 = pIntVector;
		break;
	default:
		break;
	}
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSdio0_ISR
* 功能:	设置 Sdio0 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio0_ISR(  void (*pIntVector) (void))
{
	isr_sdio0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSdio1_ISR
* 功能:	设置 Sdio1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio1_ISR(  void (*pIntVector) (void))
{
	isr_sdio1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUsb_ISR
* 功能:	设置 usb 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUsb_ISR(  void (*pIntVector) (void))
{
	isr_usb = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigCamera_ISR
* 功能:	设置 Sdio1 的中断函数。
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCamera_ISR(  void (*pIntVector) (void))
{
	isr_camera = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigTimer0_ISR
* 功能:	
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer0_ISR(U8 id, void (*pIntVector) (void))
{
	if(id < ISR_TIMER0_NUM)
		pisr_timer0[id] = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigEnableSdio
* 功能:	允许sdio中断
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigEnableSdio(void (*pIntVector) (void))
{
    isr_enable_sdio = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigPppTime
* 功能:	注册ppp定时器
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigPppTime(void (*pIntVector) (void))
{
    isr_ppp_time = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ISR_null
* 功能:	为codec 的ISR的函数指针做的空函数，为isr_audio_codec函数指针赋空值使用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void hyhwInt_ISR_null ( void )
{
   return;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSpi
* 功能:	注册spi中断函数
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpi(void (*pIntVector) (void))
{
    isr_spi = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUart1Scan
* 功能:	得到TRC中断,处理事件
* 参数:	pIntVector	-- 具体的RTC 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_RTC_intermit(void (*pIntVector) (void))
{
    isr_rtc = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigADC
* 功能:	注册ADC扫描函数
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigADC(void (*pIntVector) (void))
{
    isr_adc = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSpiDma
* 功能:	配置spiDMA的中断
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiDma(void (*pIntVector) (void))
{
    isr_spiDma = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUartDma
* 功能:	uart串口DMA中断配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUartDma(void (*pIntVector) (void))
{
    isr_uartDma = pIntVector;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUartDma
* 功能:	spi6 Gsm Pcm 传输中断配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiGsmPcm(void (*pIntVector) (void))
{
    isr_spiGsmPcm = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigSpiDma
* 功能:	spi8 TD Pcm 传输中断配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiTDPcm(void (*pIntVector) (void))
{
    isr_spiTDPcm = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigUartDma
* 功能:	spi7 TD-BOOT 传输中断配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiTDBoot(void (*pIntVector) (void))
{
    isr_spiTDBoot = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigScannerFrame
* 功能:	扫描frame中断服务函数配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigScannerFrame(void (*pIntVector) (void))
{
    isr_scanner_frame = pIntVector;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwInt_ConfigScannerData
* 功能:	扫描data中断服务函数配置
* 参数:	pIntVector	-- 具体的ISR 处理函数的入口地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigScannerData(void (*pIntVector) (void))
{
    isr_scanner_data = pIntVector;
}

/* 
修改记录: 
1\ 添加了
 hyhwInt_ConfigSpiDma
 hyhwInt_ConfigUartDma
 用来处理这两个通道DMA的情况
 yanglijing, 20090817
*/ 
 
#ifdef __cplusplus
  }
#endif
