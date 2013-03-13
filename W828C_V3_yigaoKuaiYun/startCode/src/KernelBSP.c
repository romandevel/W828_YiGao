
/**
 * \addtogroup tmAppDemoApplication
 * @{
 */

/** 
 * @file TODO
 * @brief TODO
 *
 * TODO
 */

/*------------------------------------------------------------------------------
   Standard include files:
   These should always be included !
   ---------------------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
/*------------------------------------------------------------------------------
   Project include files:
   ---------------------------------------------------------------------------*/
#include "hyhwRgb.h"
#include "std.h"
#include "sys.h"

#include "hyhwIntCtrl.h"
#include "hyhwChip.h"
#include "hyhwCkc.h"
#include "hyhwTimer.h"
#include "hyhwIntCtrl.h"
#include "hyhwIIC.h"
#include "hyhwUart.h"
#include "hyhwDma.h"
#include "hyhwSpi.h"

#include "BoardSupportPackage.h"
#include "appControlProcess.h"
#include "HycDeviceConfig.h"

/*------------------------------------------------------------------------------
   Types & Defines
   ---------------------------------------------------------------------------*/
#define _ILEVF	0x000000c0		/* disable FIQ and IRQ */
#define _ILEVI	0x00000080		/* disable IRQ */

#define OFREQ 100 /* 100 Hz Timer requency */

#define  INTA_ENABLE_REGISTER				INT_ENABLE_REGISTER
#define  INTA_CONTROLLER_PENDING_REGISTER	INT_STATUS_REGISTER 		// 即status寄存器，读此寄存器判断中断
#define  INTA_CONTROLLER_CONFIG_REGISTER	INT_IRQFIQ_SELECT_REGISTER		// 相应的位设为1时，为FIQ；初始值为0，IRQ



#define INT_FIQ_VALID(enableReg, status, ctrlConfig, intBit)	((enableReg&intBit) && (status&intBit) && ((~ctrlConfig)&intBit))
#define INT_IRQ_VALID(enableReg, status, ctrlConfig, intBit)	((enableReg&intBit) && (status&intBit) && (ctrlConfig&intBit))
/*------------------------------------------------------------------------------
   Global variables
   ---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Local variables
   ---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Function protoypes:
   ---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/


/* Low Level IRQ Interupt handler routine, called by kernel code */
void user_fiq_handler(void)
{
    UInt32 status,ctrlConfig,enableReg;
    
#if 1
	enableReg = INTA_ENABLE_REGISTER;
	status = INTA_CONTROLLER_PENDING_REGISTER;
	ctrlConfig = INTA_CONTROLLER_CONFIG_REGISTER;

	if (INT_FIQ_VALID(enableReg, status, ctrlConfig, INT_BIT_EI0))
	{
		hyhwInt_clear(INT_BIT_EI0);
		if (isr_scanner_frame)
		{
			isr_scanner_frame();
		}
	}
	if (INT_FIQ_VALID(enableReg, status, ctrlConfig, INT_BIT_EI1))
	{
		hyhwInt_clear(INT_BIT_EI1);
		if (isr_scanner_data)
		{
			isr_scanner_data();
		}
	}
#endif
	
	return;
    
}


/*----------------------------------------------------------------------------*/
/* Low Level IRQ Interupt handler routine, called by kernel code */
UInt32	gTime =0;
UInt32	gLastTime =0;
void user_irq_handler(void)
{
    UInt32 status,ctrlConfig,enableReg;
	U32 ctrlflag = 0,i;

    /* 处理中断A */
    enableReg = INTA_ENABLE_REGISTER;
    status = INTA_CONTROLLER_PENDING_REGISTER;
    ctrlConfig = INTA_CONTROLLER_CONFIG_REGISTER;

	if(INT_IRQ_VALID(enableReg, status, ctrlConfig, INT_BIT_CAMERA))
	{
		hyhwInt_clear(INT_BIT_CAMERA);
		
		if(isr_camera)
		{
			isr_camera();
		}
	}
	
   if (INT_IRQ_VALID(enableReg, status, ctrlConfig, INT_BIT_UART))
   {
   		hyhwInt_clear(INT_BIT_UART);
   		if(hyhwUart_CheckInterrupt(UART0))  // UART1 中断, 总中断中判断中断通道
	   	{
	   		if(isr_uart0)
	   		{
	   			isr_uart0(); // 在中断服务子程序中处理通道收中断还是发中断
	   		}
	   	}
	   	if(hyhwUart_CheckInterrupt(UART1))  
	   	{
	   		if(isr_uart1)
	   		{
	   			isr_uart1();
	   		}
	   	}
	   	if(hyhwUart_CheckInterrupt(UART2))
	   	{
	   		if(isr_uart2)
	   		{
	   			isr_uart2();
	   		}
	   	}
	   	if(hyhwUart_CheckInterrupt(UART3))
	   	{
	   		if(isr_uart3)
	   		{
	   			isr_uart3();
	   		}
	   	}
   }

   if (INT_IRQ_VALID(enableReg, status, ctrlConfig, INT_BIT_USB_DEVICE))
    {
		hyhwInt_clear(INT_BIT_USB_DEVICE);
		if(isr_usb)
		{
			isr_usb();
		}
    }
	
    if (INT_IRQ_VALID(enableReg, status, ctrlConfig, INT_BIT_TIMER0))
    {
		int timerIntRequest;
		hyhwInt_clear(INT_BIT_TIMER0);
		timerIntRequest = TIMER_INT_REQUEST_REGISTER;
		hyhwmTimer0_clearIntFlag();
		if (timerIntRequest & TIMER_BIT_INT4)
		{
			gTime++;
			
			/* Treat the timer intterupt by calling the appropriate handler. */
			_clock();
			/* the following call for undating profiling statistics
			** should appear in the profiling clock driver
			**/
			if (_tms_utime)		/* clock interrupts still disabled */
			{
				_inc_prof();
			}

			if (isr_key_scan_timer0)
			{
				isr_key_scan_timer0();
				
				//have key code wakeup AppCtrlPro 
			    if (drvKeyb_HaveKey() == 1)
			    {			    	
				    if (gtkernelEvent.wr < KERNEL_EVENT_NUM)
				    { 				    	
						gtkernelEvent.event[gtkernelEvent.wr++] |= KERNEL_EVENT_KEY_INPUT; 				    	
				    }
			    }
			}
			
			if (ABS(gTime-gLastTime) >= 2)
			{
			    gLastTime = gTime;
			    
			    if (gtkernelEvent.wr < KERNEL_EVENT_NUM)
    			{			   
    			   gtkernelEvent.event[gtkernelEvent.wr++] |= KERNEL_EVENT_TIMER;
    			}			        
			}			
			
			if (gtkernelEvent.wr > 0) ctrlflag = 1;
		
			if (isr_enable_sdio)
			{
			    isr_enable_sdio();
			} 

			for(i=0; i<ISR_TIMER0_NUM; i++)
			{
				if(pisr_timer0[i])
					(pisr_timer0[i])();
			}

			if (isr_ppp_time)
			{
			    isr_ppp_time();
			} 
			
			if (isr_adc)
			{
			    isr_adc();
			}
			
			if (isr_afs_check)
			{
			    isr_afs_check();
			}
		}//if (timerIntRequest & TIMER_BIT_INT4)
		
		if (timerIntRequest & TIMER_BIT_INT0)
		{
			if (isr_timer0)
			{
				isr_timer0();
			}
		}
		
		if (timerIntRequest & TIMER_BIT_INT1)
		{
			if (isr_timer1)
			{
				isr_timer1();
			}
		}
		
		if (timerIntRequest & TIMER_BIT_INT2)
		{
		    if (isr_timer2)
			{
				isr_timer2();
			} 
		}
		
		if (timerIntRequest & TIMER_BIT_INT3)
		{
		    if (isr_timer3)
			{
				isr_timer3();
			} 
		}
		
		if (timerIntRequest & TIMER_BIT_INT5)
		{
		    if (isr_timer5)
			{
				isr_timer5();
			} 
		}
	}

    if (1 == ctrlflag && pvEventWait != NULL)
    {//启动控制台进程    	
    	wakeup(&pvEventWait);
    }
    
    barScanner_checkValidFrame();//检测是否有有效的帧数据，如果有有效帧，则直接唤醒
    
    return;
}


/*----------------------------------------------------------------------------*/
/* Low Level FIQ Interupt handler routine, called by kernel code */
void _iinit(void)
{
    hyhwInt_init();
}
/*----------------------------------------------------------------------------*/

void _iclock(void)
{
    // Timer initialisation routine
    /* initialize # ticks/second, and # microseconds/tick */
    _clk_tck	= OFREQ; // Output_frequency [Number of OS clock ticks per second ];
    _tick		= 1;       // Counter Frequency / _clk_tck, [Number of timer ticks for 1 OS tick];
    _clkpri		= _ILEVI;	/* disable irq interrupts */
	// Timer Needs stable known clocks
//	hyhwTimer_Init(0); //The OS claims timer0
//	hyhwTimer_SetFrequency(0, _clk_tck); //start in this function already
//	hyhwInt_Enable(INT_MASK_TIMER0);
//	hyhwCpm_setTimerClk();
//	hyhwTimer_setTimer(0, _clk_tck);
 	INT_TRIGGER_MODE_REGISTER |= INT_BIT_TIMER0;	/* 电平触发 */
 	INT_MODEA_REGISTER &= ~INT_BIT_TIMER0;			/* 上升沿&下降沿触发 */
 	INT_INTMASK_REGISTER |= INT_BIT_TIMER0;
	
	INT_TRIGGER_MODE_REGISTER |= INT_BIT_UART;	/* 电平触发 0610*/ 
 	INT_MODEA_REGISTER &= ~INT_BIT_UART;			/* 上升沿&下降沿触发 */
 	INT_INTMASK_REGISTER |= INT_BIT_UART;
	
	hyhwTimer_SetFrequency(4, (unsigned int)_clk_tck);
	hyhwTimer_intEnable(4);
	hyhwInt_setIRQ(INT_BIT_TIMER0);
	hyhwInt_enable(INT_BIT_TIMER0);
}

//获取随机数，读取系统timer的conter值作为随机数
int hy_rand(void)
{
	U32 conter;
	if (hyhwTimer_readConter(4, &conter) == HY_OK)
	{
		return conter;
	}
	return -1;
}
/*----------------------------------------------------------------------------*/

/* All initialisation to be done before the Kernel is started */
int _iuser(void)
{
	U32 rc;
	
	hyhwCpm_setI2cClk();
	hyhwI2c_Init(I2C_CH0,Rate_100k);
	hyhwI2c_Init(I2C_CH1,Rate_100k);
	hyhwCpm_setPll1(PLL_CKC_96M, EXTAL_CLK);
	hyhwPeripheralCtrl_Init();
   
    /* Initialise the audio config */
	rc = autDrv_ds28cn01Init();
	if (rc != HY_OK)
	{
		//加密芯片验证错误，返回
		return HY_ERROR;
	}

#if 0
	/*	nandflash init part, 仅仅是一些不要在nand_init中的准备 
		注意，所有Gpio已经转换成mask的了*/
    if( internalStorageMedia.bank0.Gpio_nWP != NULL )
    {
       hyhwGpio_SetasGpio(internalStorageMedia.bank0.Gpio_nWP);
    }
    
    if( internalStorageMedia.bank1.Gpio_nWP != NULL )
    {
       hyhwGpio_SetasGpio(internalStorageMedia.bank1.Gpio_nWP);
    }
#endif
    return 0;
}

/*-----------------------------------------------------------------------------
* 函数  :	hyc_cache_write_back()
* 描述  :	回写cache
* 参数  :	none
* 返回  :	none
*----------------------------------------------------------------------------*/
void hyc_cache_write_back(void)
{
	U8 intLev;
	intLev = InterruptDisable();
	hyhwArm926e_writeBackDcache();
	hyhwArm926ejs_drainWriteBuffer();
	hyhwArm926ejs_invalidIcacheDcache();
	InterruptRestore(intLev);
}

/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------
以下程序是测试abort时的类型，在显示屏的左上角显示一个10×10小方块
红色：undefined
绿色：PrefetchAbort
蓝色：DataAbort
紫色：SWI
2007-06-07 高
---------------------------------------------------------------*/
#define ABORT_DISPLAY
#ifdef ABORT_DISPLAY
	#include "hyhwLcd.h"
    extern void drvLcd_SetRowColumn_Lcd(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth);
    
	//#define 	LCD_DATA					*((volatile U16 *)(0x04040000))
	//#define		drvLcd_WriteData(u16Data)	(LCD_DATA = u16Data)

static void DisplayInfo(U16 DispColor)
{
	int i;

#ifdef HYC_DEV_USE_WATCHDOG
    while(1)
    {
        LcdModulePowerOnOff(0);
        hyhwWatchdog_initMs(1);
        //hyhwBatteryPowerOnOff(0);
    }
#else
    drvLcd_SetRowColumn_Lcd(0,0,10,10);
	for (i = 0; i < 100; i++)
	{
		drvLcd_WriteData(DispColor);
	}
#endif
}
	//#undef 	LCD_DATA

void DisplayInfo_Undefined(void)
{
	DisplayInfo(0xF800);
	//while(1);
}
void DisplayInfo_PrefetchAbort(void)
{
	DisplayInfo(0x07E0);
	//while(1);
}
void DisplayInfo_DataAbort(void)
{
	DisplayInfo(0x001F);
	//while(1);
}
void DisplayInfo_SWI(void)
{
	DisplayInfo(0xF81F);
	while(1);
}
#endif//#ifdef ABORT_DISPLAY

