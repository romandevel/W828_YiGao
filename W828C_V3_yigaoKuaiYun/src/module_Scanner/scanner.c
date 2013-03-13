/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwIntCtrl.h"

#include "AbstractFileSystem.h"
#include "LcdAPIFunc.h"
#include "glbVariable_base.h"

#include "scanner.h"


static U8 gu8ScannerType = SCANNER_TYPE_SYMBOL;

extern const U8 suffixCmd[][9];
extern const U8 Enable_ack[][9];
extern const U8 Disable_ack[][9];

void scannerUart_intIsr(void);
static void scanner_checkType(void);
static void scanner_set(void);


/*-----------------------------------------------------------------------------
* 函数:	scanner_init
* 功能:	初始化扫描头的电源控制端口
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void scanner_init(void)
{
	hyhwScanner_powerCtrl(EnableOn);
	syssleep(1);
	
	hyhwCpm_busClkEnable(CKC_UART3);
	hyhwCpm_clearSoftReset(CKC_UART3);
	hyhwUart_Init(Uart_Baudrate_9600, Uart_DataBits_8, Uart_StopBits_1, 
					Uart_Parity_Disable, Uart_Fifo_Mode, SCANNER_UART,
					SCANNER_UART_PORT, FLOW_UNSUPPORT, DMA_UNSUPPORT);
	hyhwUart_rxIntDisable(SCANNER_UART);
	hyhwInt_disable(INT_BIT_UART);
	hyhwInt_ConfigUart_ISR(SCANNER_UART, scannerUart_intIsr);
	hyhwUart_rxIntEnable(SCANNER_UART);
	hyhwInt_setIRQ(INT_BIT_UART);
	hyhwInt_enable(INT_BIT_UART);
	
	barBuf_init();
	hyhwScanner_TrigInit();
	scanner_checkType();
	
	scanner_set();
}

/*-----------------------------------------------------------------------------
* 函数:	scanner_reinit
* 功能:	重新初始化扫描头的电源控制端口
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void scanner_reinit(void)
{
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		/* symbol 扫描头设置*/
		hyhwScanner_powerCtrl(EnableOn);
		
		hyhwUart_Init(Uart_Baudrate_9600, Uart_DataBits_8, Uart_StopBits_1, 
						Uart_Parity_Disable, Uart_Fifo_Mode, SCANNER_UART,
						SCANNER_UART_PORT, FLOW_UNSUPPORT, DMA_UNSUPPORT);
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwInt_disable(INT_BIT_UART);
		hyhwInt_ConfigUart_ISR(SCANNER_UART, scannerUart_intIsr);
		hyhwUart_rxIntEnable(SCANNER_UART);
		hyhwInt_setIRQ(INT_BIT_UART);
		hyhwInt_enable(INT_BIT_UART);
	}
	else //if (gu8ScannerType == SCANNER_TYPE_OPTION)
	{
		#if 0
		/* opticon 扫描头，102/102解码，设置*/
		hyhwApi_stm102Wake();
		#else
		/* opticon 扫描头，7901解码，设置*/
		#endif
	}
	
	barBuf_init();
	hyhwScanner_TrigInit();
}

/*-----------------------------------------------------------------------------
* 函数:	scanner_deinit
* 功能:	反初始化扫描头，关闭扫描头
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void scanner_deinit(void)
{
	hyhwScanner_TrigDisable();
	
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwCpm_busClkDisable(CKC_UART3);
		hyhwInt_ConfigUart_ISR(SCANNER_UART, NULL);
		
		hyhwGpio_setAsGpio(SCANNER_UART_TX_PORT, SCANNER_UART_TX_GPIO);
		hyhwGpio_setIn(SCANNER_UART_TX_PORT, SCANNER_UART_TX_GPIO);
		hyhwGpio_setAsGpio(SCANNER_UART_RX_PORT, SCANNER_UART_RX_GPIO);
		hyhwGpio_setIn(SCANNER_UART_RX_PORT, SCANNER_UART_RX_GPIO);
		
		/* CTS 置高，允许SE-955发送数据，关闭时，设为输入浮空*/
		hyhwGpio_setAsGpio(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setIn(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_pullDisable(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
	}
	else
	{
		#if 0
		hyhwApi_stm102ResetOn();
		hyhwApi_stm102HwInterfaceDeinit();
		#else
		#endif
	}
	hyhwScanner_powerCtrl(EnableOff);
}

/*-----------------------------------------------------------------------------
* 函数:	scanner_checkType
* 功能:	检查扫描头类型
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void scanner_checkType(void)
{
	int ret;
	scannerUart_sendwkup();
	syssleep(10);
	
	/* symbol设置命令 */
	ret = scannerUart_sendCmd(Enable_ack[0]);
	if (ret == HY_OK)
	{
		/* symbol设置命令响应成功 */
		gu8ScannerType = SCANNER_TYPE_SYMBOL;
	}
	else
	{
		/* symbol设置命令响应失败 */
		gu8ScannerType = SCANNER_TYPE_OPTICON;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	scanner_getType
* 功能:	获取扫描头类型
* 参数:	none
* 返回:	扫描头类型
*		0---symbol scanner(讯宝扫描头)
*		1---option scanner(欧光扫描头)
*----------------------------------------------------------------------------*/
int scanner_getType(void)
{
	return gu8ScannerType;
}

#if 0
#define IMAGE_SCANNER_FILE		("C:/image102.hyc")
#define UPDATE_SCANNER_STR		("Updating scanner...")
/*-----------------------------------------------------------------------------
* 函数:	scanner_update_init_102
* 功能:	升级并初始化102程序(欧光扫描头才有102)
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void scanner_update_init_102(void)
{
	int resourceHandle, len;
	
	resourceHandle = AbstractFileSystem_Open(IMAGE_SCANNER_FILE, AFS_READ);
	if(resourceHandle >= 0)
	{
		AbstractFileSystem_Close(resourceHandle);
		
		len = strlen(UPDATE_SCANNER_STR);
		//drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
		//drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
		//drvLcd_Background2DisplayMemory(16,len*8,140,50);
		drvLcd_Background2DisplayMemory(78, 196, 93, 24);
		drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
		drvLcd_DisplayString(140,50,0,(U8*)UPDATE_SCANNER_STR);
		DisplayData2Screen();

		LcdModulePowerOnOff(1);//为将亮度设置为保存值
		
		hyhwUart_stm32FlashLoader(IMAGE_SCANNER_FILE);
	}
	hyhwApi_CommInit();
}
#endif

/*-----------------------------------------------------------------------------
* 函数:	scanner_set
* 功能:	设置扫描头
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void scanner_set(void)
{
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		/* symbol 扫描头设置*/
		scanner_symbol_set();
		/* 初始化其他IO */
		/* CTS 置高，允许SE-955发送数据 */
		hyhwGpio_setAsGpio(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setOut(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setHigh(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		/* RTS 置为输入浮空 */
		hyhwGpio_setAsGpio(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		hyhwGpio_setIn(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		hyhwGpio_pullDisable(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		/* PWRDWN 置为输入浮空 */
		hyhwGpio_setAsGpio(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		hyhwGpio_setIn(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		hyhwGpio_pullDisable(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		/* Beeper 置为输入浮空 */
		hyhwGpio_setAsGpio(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		hyhwGpio_setIn(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		hyhwGpio_pullDisable(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		/* AIM_WAKE 置为输入浮空 */
		hyhwGpio_setAsGpio(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
		hyhwGpio_setIn(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
		hyhwGpio_pullDisable(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
	}
	else //if (gu8ScannerType == SCANNER_TYPE_OPTICON)
	{
		/* opticon 扫描头，7901解码，设置*/
		hyhwInt_disable(INT_BIT_UART);
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwCpm_busClkDisable(CKC_UART3);
		/* 初始化无用IO */
		/* MCU_RESET 置低 */
		hyhwGpio_setAsGpio(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		/* MCU_BOOT_CTRL 置低 */
		hyhwGpio_setAsGpio(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		/* MCU_WAKEUP 置低 */
		hyhwGpio_setAsGpio(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		/* TRIG 置低 */
		hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		hyhwGpio_setLow(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		
		barDecode_p_Start();
	}
	/* 重新初始化barBuf */
	barBuf_init();
}

void scan_clear(void)
{
}