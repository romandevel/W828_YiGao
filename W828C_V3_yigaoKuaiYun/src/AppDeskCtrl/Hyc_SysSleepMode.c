#include "hyTypes.h"
#include "hyhwTcc7901.h"
#include "hyhwCkc.h"
#include "hyhwintCtrl.h"
#include "hyhwGpio.h"
#include "hyhwRtc.h"
#include "Hyc_SysSleepMode.h"
#include "drvKeybScan.h"
#include "hardWare_Resource.h"

//#define SLEEP_MODE_DEBUG	//调试时，使用12M，32k无法调试
//#define SLEEP_MODE_TEST		//5秒自动唤醒一次

/*************************************************************************
 * DEFINES
 *************************************************************************/
//RTC
#define	RTC_CONTROL_REGISTER		TCC7901_RTC_RTCCON_REGISTER			/* RTC Control Register */
#define RTC_BIT_WRITE_EN		BIT1
	
#define	RTC_INT_CONTROL_REGISTER	TCC7901_RTC_INTCON_REGISTER			/* RTC Interrupt Control Register */
#define INT_BIT_ENABLE			BIT0
	
#define	BCD_SECOND_REGISTER			TCC7901_RTC_BCDSEC_REGISTER			/* BCD Second Register */

//RTC ALARM
#define	RTC_INT_PEND_REGISTER		TCC7901_RTC_RTCPEND_REGISTER		/* RTC Interrupt Pending Register */
#define INTPEND_BIT_PENDING		BIT0
	
//WIFI SDIO
#define	NORMAL_INTERRUPT_STATUS_LOW_REGISTER			TCC7901_SDIO_STSL_REGISTER
#define NIS_BIT_CARD_INTERRUPT			BIT8	// card interrupt

//USB
#define INT_SRC_REG		TCC7901_PIC_SRC_REGISTER

//SDRAM
#define	EMI_SD_CONFIG_REGISTER						TCC7901_EMI_SDCFG_REGISTER	
#define	DMCR_BIT_SELF_REFRESH_ENABLE				BIT0
#define	EMI_SD_MISCELL_CONFIG_REGISTER				TCC7901_EMI_MCFG_REGISTER
#define DMCR_BIT_DATA_BUS_OUTPUT_MODE_OUTPUT		BIT14	 
#define	DMCR_BIT_SDCLK_DYNAMIC_CONTROL_GATE_CONTROL	BIT6	
#define	DMCR_BIT_SDCLK_CONTROLLED					BIT5
#define	DMCR_BIT_SD_CONTROLLER_ENABLE				BIT4
#define	DMCR_BIT_SD_CLK_UNMASKED					BIT1
#define	DMCR_BIT_EXTERNAL_REFRESH_CLK_ENABLE		BIT0

#define BCDtoDEC(x)		(\
						((x & 0xF000) >> 12) * 1000 + \
						((x & 0x0F00) >> 8)  * 100  + \
						((x & 0x00F0) >> 4)  * 10   + \
						( x & 0x000F)				  \
						)

#define TIME_WAKEUP_CNT_NORMAL			(30)//秒，系统自动唤醒一次，检查各种状态(记事本等)
#define TIME_WAKEUP_CNT_WIFI			(5)//(30)//秒，系统自动唤醒一次，为主动发送网络电话信息

#define CYCLE_TIME_NORMAL		(46875)//(20000)//us，在32k频率下运行
#define CYCLE_TIME_WIFI			(100)//us，在12M频率下运行

#define KEY_DETECT_CNT_NORMAL		(1)//(50*1000/CYCLE_TIME_NORMAL)//(100*1000/CYCLE_TIME_NORMAL)//100ms的循环次数
#define KEY_DETECT_CNT_WIFI			(1)//(1000/CYCLE_TIME_WIFI)//(30*1000/CYCLE_TIME_WIFI)//(100*1000/CYCLE_TIME_WIFI)//100ms的循环次数
#define POWER_KEY_LONG_NORMAL		(1000*1000/CYCLE_TIME_NORMAL)//1s的循环次数
#define POWER_KEY_LONG_WIFI			(1000*1000/CYCLE_TIME_WIFI)//1s的循环次数

#define USB_DETECT_CNT_NORMAL		(100*1000/CYCLE_TIME_NORMAL)//100ms的循环次数
#define USB_DETECT_CNT_WIFI			(100*1000/CYCLE_TIME_WIFI)//100ms的循环次数

//循环指定次数后恢复一次12M时钟，用于检测USB Vbus
#define RESUME_12M_CNT			(1000/CYCLE_TIME_NORMAL)

/*	该函数中所有的程序变量都overlay到Sram中，
	在进入待机循环时，程序要求不使用局部变量，
	不调用子函数，不操作Sdram上的任何资源(函数和内存)
*/

static U32 bakModeCtrlReg, bakPLL0CfgReg, bakPLL1CfgReg, bakClkCtrlReg, bakClkDivReg;
static U32 bakBusClkCtrlReg0, bakBusClkCtrlReg1;
static U32 bakIntEnableCfg_A;
static U32 tempPClk, bakPClk[24], regCount;
static U32 *pPClk;

static S8 wakeUpSecond;
static U8 lastSecond;
static U8 second;//当局部变量使用   
static U8 timeWakeUpCnt;

#define WAKEUP_DETECT_KEY_TOTAL		5//1//

static int wakeIndex = 0;
static tCTR_WAKE_KEY tWakeKey[WAKEUP_DETECT_KEY_TOTAL];

static volatile U32 *GSM_CDMA_Uart_RI_Port;
static volatile U32 GSM_CDMA_Uart_RI_Gpio;

static volatile U32 *TestDCINport;
static volatile U32 TestDCINGpio;

static const U32 keyValidCnt[2] = {KEY_DETECT_CNT_NORMAL, KEY_DETECT_CNT_WIFI};
static const U32 powerKeyValidCnt[2] = {POWER_KEY_LONG_NORMAL, POWER_KEY_LONG_WIFI};
static const U32 usbValidCnt[2] = {USB_DETECT_CNT_NORMAL, USB_DETECT_CNT_WIFI};
//循环次数换算为系统tick的系数
static const U32 runningCntCoef[2] = {CYCLE_TIME_NORMAL, CYCLE_TIME_WIFI};

static U32 usbDetectCnt;		//USB VBus扫描计数
static U32 runningCnt;
static U32 resume12MCnt;		//用于恢复12M时钟的计数

static U32 netSelectFlag = 0;
static U32 wakeUpFlag;

//外部程序检测到按键没有来得及处理让内部来处理
static U32 keyExtFlag = 0;


extern U32 gTime;

//#define TEST_USE_GPIO
#ifdef TEST_USE_GPIO
#define TEST_PORT		PORT_E
#define TEST_GPIO		GPIO_30

static volatile U32 *test_Port;
static volatile U32 test_Gpio;
static U32 sysSleepTestFlag;
#endif

#ifdef SLEEP_MODE_TEST
#define LED_LOOP_CNT		(50)//50*20=1000ms,一个循环20ms
#define LED_ENABLE_CNT		(1)//亮1*20=20ms
#else
#ifdef SLEEP_MODE_DEBUG
#define LED_LOOP_CNT		(0x11940)//(200*360)//(200)//200*20=4000ms,一个循环20ms
#define LED_ENABLE_CNT		(720)//(1*360)//(2)//亮1*20=20ms
#else
#define LED_LOOP_CNT		(100)//(0x11940)//(200*360)//200*20=4000ms,一个循环20ms
#define LED_ENABLE_CNT		(2)//(360)//(1*360)//亮1*20=20ms
#endif
#endif

extern const U32 cLedCtrlIO_info[];
static volatile U32 *led_Port;
static volatile U32 led_Gpio;
static U32 ledFlashCnt;


__inline static void Hyc_SysStartKey(void)
{
    int i;
    U32 *pSysKeyInfo;
    U32 sizeRegister;
    
	memset(&tWakeKey,0,sizeof(tCTR_WAKE_KEY)*WAKEUP_DETECT_KEY_TOTAL);
	
	sizeRegister = sizeof(tGPIO_REGISTER);
	pSysKeyInfo = &cWakeUpSysKeyInfo[0];
	
	for (i=0; i<WAKEUP_DETECT_KEY_TOTAL; i++)
	{
	    if (0 != pSysKeyInfo[0] || 0 != pSysKeyInfo[1])
	    {
	        tWakeKey[i].pOutPort = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + ((U32)pSysKeyInfo[0]*sizeRegister));
	        tWakeKey[i].outGpio = pSysKeyInfo[1];
	    }
	    else
	    {
	        tWakeKey[i].pOutPort = (volatile U32 *)0; 
	        tWakeKey[i].outGpio = 0;
	    }
	    
	    if (0 != pSysKeyInfo[2] || 0 != pSysKeyInfo[3])
	    {
	        tWakeKey[i].pInPort = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (((U32)pSysKeyInfo[2])*sizeRegister));
	        tWakeKey[i].inGpio =  pSysKeyInfo[3];
	        tWakeKey[i].valid  =  pSysKeyInfo[4];
	    }
	    else
	    {
	        tWakeKey[i].pInPort = (volatile U32 *)0;
	        tWakeKey[i].inGpio = 0;
	    }
	    
	    
	    if (i == 0)
	    {
	        tWakeKey[i].wakeType = POWER_OFF_WAKEUP;
	        tWakeKey[i].maxKeyTotal = keyValidCnt[0];//powerKeyValidCnt[WiFi_Ready];
	    } 	    
	    else
	    {
	        tWakeKey[i].wakeType = KEY_WAKEUP;
	        tWakeKey[i].maxKeyTotal = keyValidCnt[0];
	    }  
	    
	    //增加外部检测
	    if (i != 0 && keyExtFlag == tWakeKey[i].wakeType)
	    {
	         tWakeKey[i].detectCnt = keyValidCnt[0];
	    }
	    
	    pSysKeyInfo += 5;
	}
	
	wakeIndex = 0;
}

void Hyc_Set_Key(int key)
{
     keyExtFlag = key;
}

void Hyc_Set_netFlag(int netFlag)
{
	netSelectFlag = netFlag;
}

int Hyc_SysSleepMain(void)
{
#ifdef TEST_USE_GPIO
	hyhwGpio_setAsGpio(TEST_PORT, TEST_GPIO);
	hyhwGpio_setOut(TEST_PORT, TEST_GPIO);
	hyhwGpio_setLow(TEST_PORT, TEST_GPIO);
	
	test_Port = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (TEST_PORT*sizeof(tGPIO_REGISTER)));
	test_Gpio = TEST_GPIO;
#endif
	//如果WiFi已上电并初始化，则维持SDIO的12M供给
	Hyc_SysStartKey();
	
	GSM_CDMA_Uart_RI_Port	= (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (GSM_UART0_RI_PORT*sizeof(tGPIO_REGISTER)));
	GSM_CDMA_Uart_RI_Gpio	= GSM_UART0_RI_GPIO;
	
	TestDCINport = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (DCIN_DET_PORT*sizeof(tGPIO_REGISTER)));
	TestDCINGpio = DCIN_DET_GPIO ;
	
	led_Port	= (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (cLedCtrlIO_info[0]*sizeof(tGPIO_REGISTER)));
	led_Gpio	= cLedCtrlIO_info[1];
	ledFlashCnt = 0;
	
	/* 初始化RTC使用的变量 */
	second = 0;
	lastSecond = 0;
	wakeUpSecond = -1;


//	(*led_Port) |= GPIO_1|GPIO_3|GPIO_4;//high//关闭三个灯
#if 0
	/* 进去power down之前清除cache内容 */
	hyhwArm926e_writeBackDcache();
	hyhwArm926ejs_drainWriteBuffer();
	hyhwArm926ejs_invalidIcacheDcache();
#endif
	hyhwArm926ejs_enableIcache();
	hyhwArm926ejs_enableDcache();
	
	/* 保存中断(FIQ/IRQ)允许配置 */
	bakIntEnableCfg_A = INT_ENABLE_REGISTER;
	
	/* 禁止所有模块的中断 */
	INT_ENABLE_REGISTER = 0;

	//INT_ENABLE_REGISTER |= INT_BIT_UART;		//GSM(Uart1)中断
	
	/* EnterStandbyMode */
	INT_WAKEUP_REGISTER = 0xffffffff;
	
	// set CPU enter & exit standby mode
	// backup releated registers
	bakModeCtrlReg = CKC_MODECTR_REGISTER;
	bakPLL0CfgReg  = CKC_PLL0CFG_REGISTER;
	bakPLL1CfgReg  = CKC_PLL1CFG_REGISTER;
	bakClkCtrlReg = CKC_CLKCTRL_REGISTER;
	bakClkDivReg  = CKC_CLKDIVC_REGISTER;
	bakBusClkCtrlReg0 = CKC_BCLKMSKE0_REGISTER;
	bakBusClkCtrlReg1 = CKC_BCLKMSKE1_REGISTER;
	
	
#if 1
	//save BCLKCTRL register and enable all of Bus Clock
	//backup all of PCLK_XXX at here, 18 registers
	//setting all peripheral clock in XTIN mode and close them at here
	// the count of register is defferent between 7901 and 8300 
	//,and the register map is differ too
	/*
		CKC_USBH, CKC_SDMMC, CKC_MSTICK, CKC_I2C, CKC_LCD,
		CKC_CAM, CKC_UART0, CKC_UART1, CKC_UART2, CKC_UART3,
		CKC_TIMER, CKC_TIMER, CKC_TIMER, 
		CKC_DAI, CKC_GPSB0, CKC_GPSB1, CKC_GPSB2, CKC_GPSB3,
		CKC_ADC, CKC_SPDIF, CKC_EMC, CKC_MTM, CKC_EHI0, CKC_EHI1,
	*/
	pPClk = (unsigned long *)(&(CKC_PCK_USB11H_REGISTER));
	for(regCount=0; regCount<24; regCount++)
	{
		tempPClk = *pPClk;
		bakPClk[regCount] = tempPClk; //backup PCLK_XXX
		
		if ((tempPClk&MODEL_BIT_ENABLE) != 0)
		{
			//if (regCount != 20)//Sdram clk不设置为32k
			{
				//如果clk处于使能状态，将其clk原切换到32k上，不分频
				tempPClk = ((CLK_32k<<24)|MODEL_BIT_ENABLE);
				*pPClk = tempPClk;
			}
		}
		pPClk++;
	}
	//CKC_PCK_RFREQ_REGISTER = bakPClk[20];//sdram 时钟开
#endif
#ifdef SLEEP_MODE_TEST
	timeWakeUpCnt = TIME_WAKEUP_CNT_WIFI;
#else
	timeWakeUpCnt = TIME_WAKEUP_CNT_NORMAL;
#endif
	
	// Disable PLLs steps:
	//1.set HwPLLCFG's S=3 to make stable when system clock is changed
	CKC_PLL0CFG_REGISTER |= 0x00030000;
	CKC_PLL1CFG_REGISTER |= 0x00030000;
	
	//2.clear CPU0 source, HwCLKCTRL setting bit0~2
	//  clear bus divider, Fbus=Fxindiv/2
	//CLK_DIVIDE_REGISTER |= 0x0000EF00;
	#ifdef SLEEP_MODE_DEBUG
	{//12M
		CKC_CLKCTRL_REGISTER |= MCPUCLK_BIT_DIV_MASK;
		CKC_CLKCTRL_REGISTER |= (CKC_CLKCTRL_REGISTER & ~0x0F) | (BIT2);
	}
	#else
	{
    	/* 使用32768晶振作为系统时钟 */
    	CKC_CLKCTRL_REGISTER |= MCPUCLK_BIT_DIV_MASK;
    	CKC_CLKCTRL_REGISTER |= (CKC_CLKCTRL_REGISTER & ~0x0F) | (BIT2|BIT1);
    	CKC_CLKCTRL_REGISTER &= ~0x1F0;// clear bus divider
		__asm
		{
			nop
			nop
		}
	}
	#endif
	
	
	/* 禁止 PLL */
	CKC_PLL0CFG_REGISTER |= (BIT31|0x030000);
	CKC_PLL1CFG_REGISTER |= BIT31;
	
	//clear all IRQ
	INT_CLEAR_REGISTER  = 0xffffffff;
	#ifndef SLEEP_MODE_DEBUG
	{
		//设置 CKC_MODECTR_REGISTER 寄存器，关闭12M
		//enter power down mode
		//CKC_MODECTR_REGISTER &= MODE_BIT_PWDOWNCLK_XTIN;
		CKC_MODECTR_REGISTER |= MODE_BIT_PWDOWNENABLE;
		__asm 
		{
			nop
			nop
		}
		//停外部12M
		CKC_CLKCTRL_REGISTER &= (~BIT31);
	}
	#endif
	
	//(*led_Port) |= led_Gpio;//high,亮
	(*led_Port) &= ~led_Gpio;//low,灭
	//sdram_standby
	//if ((EMI_SD_MISCELL_CONFIG_REGISTER) & DMCR_BIT_SD_CONTROLLER_ENABLE)
	{
		// enter SDRAM self-refresh mode
		EMI_SD_CONFIG_REGISTER |= DMCR_BIT_SELF_REFRESH_ENABLE;
		// wait until self-refresh mode is done.
		while ((EMI_SD_MISCELL_CONFIG_REGISTER&0x000A0000) != 0x000A0000);
		// Disable SDCLK
		EMI_SD_MISCELL_CONFIG_REGISTER &= ~(DMCR_BIT_SDCLK_DYNAMIC_CONTROL_GATE_CONTROL|
											DMCR_BIT_SD_CLK_UNMASKED);
		// Disable SD-XD[31:0]
		EMI_SD_MISCELL_CONFIG_REGISTER |= DMCR_BIT_DATA_BUS_OUTPUT_MODE_OUTPUT;
	}
	
	#ifdef TEST_USE_GPIO
	sysSleepTestFlag = 0;
	#endif
	
	usbDetectCnt = 0;
	
	runningCnt = 0;
	resume12MCnt = 0;
	RTC_CONTROL_REGISTER |= RTC_BIT_WRITE_EN;
	RTC_INT_CONTROL_REGISTER |= INT_BIT_ENABLE;
	
//	(*led_Port) |= led_Gpio;//high,亮
	ledFlashCnt = 0;
	while(1)
	{
		wakeUpFlag = 0;
		#ifdef TEST_USE_GPIO
		if (sysSleepTestFlag == 0)
		{
			(*test_Port) &= ~test_Gpio;//low
			sysSleepTestFlag = 1;
		}
		else
		{
			(*test_Port) |= test_Gpio;//high
			sysSleepTestFlag = 0;
		}
		#endif
		//闪烁led
		ledFlashCnt++;
		if (ledFlashCnt == LED_ENABLE_CNT)
		{
			//(*led_Port) &= ~led_Gpio;//low,灭
			(*led_Port) |= led_Gpio;//hig,亮
		}
		else if (ledFlashCnt >= LED_LOOP_CNT)
		{
			ledFlashCnt = 0;
			//(*led_Port) |= led_Gpio;//hig,亮
			(*led_Port) &= ~led_Gpio;//low,灭
		}


		if (netSelectFlag == 0)//wifi 待机
		{
			if(NORMAL_INTERRUPT_STATUS_LOW_REGISTER&NIS_BIT_CARD_INTERRUPT)
			{
				wakeUpFlag |= SDIO_WAKEUP;
				break;//立即后台唤醒
			}
		}
		else
		{
			//检测GSM是否有来电、短信
			if (((*GSM_CDMA_Uart_RI_Port) & GSM_UART0_RI_GPIO) != 0)
			{
				wakeUpFlag |= UART1_WAKEUP;
			}
		}

		//扫描键盘
		//只允许power和OK键唤醒 2011-05-26
	#if 1
		for (wakeIndex=0; wakeIndex<WAKEUP_DETECT_KEY_TOTAL; wakeIndex++)
		{
			if (0 != tWakeKey[wakeIndex].outGpio)
			{
				/* 设置output IO输出指定电平(high) */
				//( *tWakeKey[wakeIndex].pOutPort) |= tWakeKey[wakeIndex].outGpio;
				/* 设置output IO输出指定电平(low) */
				( *tWakeKey[wakeIndex].pOutPort) &= ~tWakeKey[wakeIndex].outGpio;
			}

			if (0 != tWakeKey[wakeIndex].inGpio)
			{
				/* 读取input IO状态 */
				if ((*tWakeKey[wakeIndex].pInPort)&tWakeKey[wakeIndex].inGpio)
				{
					tWakeKey[wakeIndex].gpioStatus = 1;
				}
				else
				{
					tWakeKey[wakeIndex].gpioStatus = 0;
				}
			}

			if (0 != tWakeKey[wakeIndex].outGpio)
			{
				/* 恢复output IO输出指定电平(low) */
				//( *tWakeKey[wakeIndex].pOutPort) &= ~tWakeKey[wakeIndex].outGpio;
			/* 恢复output IO输出指定电平(high) */
				( *tWakeKey[wakeIndex].pOutPort) |= tWakeKey[wakeIndex].outGpio;
			}

			if (tWakeKey[wakeIndex].gpioStatus == tWakeKey[wakeIndex].valid)
			{
				if (++tWakeKey[wakeIndex].detectCnt >= tWakeKey[wakeIndex].maxKeyTotal)
				{
					tWakeKey[wakeIndex].detectCnt = tWakeKey[wakeIndex].maxKeyTotal;
				}
			}
			else
			{
				/* 指定的按键释放后，才退出待机模式 */
				if (tWakeKey[wakeIndex].detectCnt >= tWakeKey[wakeIndex].maxKeyTotal)
				{
					//wakeUpFlag |= tWakeKey[wakeIndex].wakeType;
					wakeUpFlag |= KEY_WAKEUP;
				}

				tWakeKey[wakeIndex].detectCnt = 0;
			}
		}
	#endif

		if (netSelectFlag == 0)//wifi 待机
		{
			if(NORMAL_INTERRUPT_STATUS_LOW_REGISTER&NIS_BIT_CARD_INTERRUPT)
			{
				wakeUpFlag |= SDIO_WAKEUP;
				break;//立即后台唤醒
			}
		}
		else
		{
			//检测GSM是否有来电、短信
			if (((*GSM_CDMA_Uart_RI_Port) & GSM_UART0_RI_GPIO) != 0)
			{
				wakeUpFlag |= UART1_WAKEUP;
			}
		}
		
		/*	读取RTC，直接读取寄存器
			1. SIP规定当VoIP注册上后，需要每30秒向服务器发送数据，表示在线
			2. 因为停止了所有中断，更新系统tick，保持系统计时
			3. 定时启动ADC，读取电池电量，不足时唤醒系统提示(该条可以不要)
		*/
		second = BCD_SECOND_REGISTER;
		second = BCDtoDEC(second);
		if (wakeUpSecond == -1)
		{
			wakeUpSecond = second;
		}
		
		if (second != lastSecond)
		{
			if (second > wakeUpSecond)
			{
				if ((second - wakeUpSecond) >= timeWakeUpCnt)
				{
					wakeUpSecond = second;
					wakeUpFlag |= TIME_30S_WAKEUP;
				}
			}
			else if (second < wakeUpSecond)
			{
				if (((60-wakeUpSecond) + second) >= timeWakeUpCnt)
				{
					wakeUpSecond = second;
					wakeUpFlag |= TIME_30S_WAKEUP;
				}
			}
			
			lastSecond = second;
		}
		
		if (netSelectFlag == 0)//wifi 待机
		{
			if(NORMAL_INTERRUPT_STATUS_LOW_REGISTER&NIS_BIT_CARD_INTERRUPT)
			{
				wakeUpFlag |= SDIO_WAKEUP;
				break;//立即后台唤醒
			}
		}
		else
		{
			//检测GSM是否有来电、短信
			if (((*GSM_CDMA_Uart_RI_Port) & GSM_UART0_RI_GPIO) != 0)
			{
				wakeUpFlag |= UART1_WAKEUP;
			}
		}

		#if 0
		/*	读取RTC，直接读取ALARM寄存器 */
		if (RTC_INT_PEND_REGISTER & INTPEND_BIT_PENDING)
		{
			wakeUpFlag |= ALARM_WAKEUP;
		}
		#endif
		
		/* 检测 USB 是否插入(有可能是适配器) */
		if ((INT_SRC_REG&INT_BIT_SELECT4) ||
			((*TestDCINport) & DCIN_DET_GPIO) != 0)
		{
			usbDetectCnt++;
			if (usbDetectCnt >= usbValidCnt[0])
			{
				wakeUpFlag |= USB_VBUS_WAKEUP;
				usbDetectCnt = 0;
			}
		}
		else
		{
			/* 清除USB VBUS检测计数 */
			usbDetectCnt = 0;
		}
		
		runningCnt++;
		//wakeUpFlag = 2;
		/* 唤醒系统 */
		if (wakeUpFlag != 0)
		//if (wakeUpFlag & USB_VBUS_WAKEUP)
		{
			break;
		}
	}/* while(1) */
	
	//(*led_Port) |= led_Gpio;//hig,亮
	(*led_Port) &= ~led_Gpio;//low,灭
	//使能12M外部时钟
	CKC_CLKCTRL_REGISTER |= BIT31;
	second = 40;
	while(second--);
	CKC_MODECTR_REGISTER		= bakModeCtrlReg;
	//(*led_Port) &= ~led_Gpio;//low,灭
	(*led_Port) |= led_Gpio;//hig,亮
	
	CKC_CLKCTRL_REGISTER |= MCPUCLK_BIT_DIV_MASK;
	CKC_CLKCTRL_REGISTER |= (CKC_CLKCTRL_REGISTER & ~0x0F) | (BIT2);
	second = 40;
	while(second--);
	
	#ifndef SLEEP_MODE_DEBUG
	{
		//设置 CKC_MODECTR_REGISTER 寄存器，关闭12M
		//enter power down mode
		//CKC_MODECTR_REGISTER |= MODE_BIT_PWDOWNCLK_XTIN;
		//CKC_MODECTR_REGISTER &= (~MODE_BIT_PWDOWNENABLE);
	}
	#endif
	
	/* 恢复寄存器设置(releated) */
	//CKC_MODECTR_REGISTER		= bakModeCtrlReg;
	CKC_PLL0CFG_REGISTER 		= bakPLL0CfgReg;
	CKC_PLL1CFG_REGISTER 		= bakPLL1CfgReg;
	CKC_CLKCTRL_REGISTER 		= bakClkCtrlReg;
	__asm 
	{
		nop
		nop
	}
	CKC_CLKDIVC_REGISTER		= bakClkDivReg;
	__asm 
	{
		nop
		nop
	}
	
	/* 恢复寄存器设置(PCLKCFG) */
	#if 1
	pPClk = (unsigned long *)(&(CKC_PCK_USB11H_REGISTER));
	for(regCount=0; regCount<24; regCount++)
	{
		*pPClk++ = bakPClk[regCount];
	}
	#endif
	
	//restore BCLKCTRL register
	CKC_BCLKMSKE0_REGISTER = bakBusClkCtrlReg0;
	CKC_BCLKMSKE1_REGISTER = bakBusClkCtrlReg1;
	
	/* 恢复中断(FIQ/IRQ)允许配置 */
	INT_ENABLE_REGISTER = bakIntEnableCfg_A;
	
	/* 禁止系统Wakeup */
	INT_WAKEUP_REGISTER = 0xffffffff;

#if 1
	//sdram_wakeup
	//if ((EMI_SD_MISCELL_CONFIG_REGISTER) & DMCR_BIT_SD_CONTROLLER_ENABLE)
	{
		// Enable SD-XD[31:0]
		EMI_SD_MISCELL_CONFIG_REGISTER &= ~DMCR_BIT_DATA_BUS_OUTPUT_MODE_OUTPUT;
		// Enable SDCLK
		
		//(*led_Port) |= led_Gpio;//hig,亮
		(*led_Port) &= ~led_Gpio;//low,灭
		second = 40;
		while(second--);
		//(*led_Port) &= ~led_Gpio;//low,灭
		(*led_Port) |= led_Gpio;//hig,亮
		EMI_SD_MISCELL_CONFIG_REGISTER |= (DMCR_BIT_SDCLK_DYNAMIC_CONTROL_GATE_CONTROL|
											DMCR_BIT_SD_CLK_UNMASKED);
		
		// Release SDRAM from self-refresh mode
		EMI_SD_CONFIG_REGISTER &= ~DMCR_BIT_SELF_REFRESH_ENABLE;
		
		second = 40;
		while(second--);
		//(*led_Port) |= led_Gpio;//hig,亮
		(*led_Port) &= ~led_Gpio;//low,灭
		// dummy read
		usbDetectCnt	= *(volatile unsigned *)0x20000000;
		usbDetectCnt	= *(volatile unsigned *)0x20000000;
		usbDetectCnt	= *(volatile unsigned *)0x20000000;
		usbDetectCnt	= *(volatile unsigned *)0x20000000;
	}
#endif
	//(*led_Port) &= ~led_Gpio;//low,灭
	(*led_Port) |= led_Gpio;//hig,亮
	
	RTC_INT_CONTROL_REGISTER &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER &= ~RTC_BIT_WRITE_EN; 
	
	//将runningCnt换算为系统tick数(10ms)
	runningCnt = runningCnt*runningCntCoef[0]/(10*1000);
	gTime += runningCnt;
	
	
	return wakeUpFlag;
}
