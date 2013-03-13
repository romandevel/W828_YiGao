#include "hyhwWifiBt.h"
#include "hardWare_Resource.h"

U8	gu8WifiBtPowerCtrl=0;


/*----------------------------------------------------------------------------
* 函数:	hyhwWifiBt_Init
* 功能:	WIFI BT初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWifiBt_Init(void)
{
	gu8WifiBtPowerCtrl = 0;
	
	hyhwGpio_setAsGpio(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	hyhwGpio_setOut(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	hyhwGpio_setLow(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	
	//bt disable
	#if 0	//828上没有这个IO
	hyhwGpio_setAsGpio(BT_EN_PORT, BT_EN_GPIO);
	hyhwGpio_setOut(BT_EN_PORT, BT_EN_GPIO);
	hyhwGpio_setLow(BT_EN_PORT, BT_EN_GPIO);
	#endif
	//bt reset
	hyhwGpio_setAsGpio(BT_RESET_PORT, BT_RESET_GPIO);
	hyhwGpio_setOut(BT_RESET_PORT, BT_RESET_GPIO);
	hyhwGpio_setLow(BT_RESET_PORT, BT_RESET_GPIO);
	
	hyhwGpio_setAsGpio(BT_UART_TX_PORT,BT_UART_TX_GPIO);
	hyhwGpio_setIn(BT_UART_TX_PORT,BT_UART_TX_GPIO);
	
	hyhwGpio_setAsGpio(BT_UART_RX_PORT,BT_UART_RX_GPIO);
	hyhwGpio_setIn(BT_UART_RX_PORT,BT_UART_RX_GPIO);
	
	
	//wifi reset
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setOut(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setLow(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	
	//wifi vreg disable
	hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setOut(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setLow(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);

	hyhwGpio_setAsGpio(WIFI_SD_DAT3_PORT,WIFI_SD_DAT3_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT3_PORT,WIFI_SD_DAT3_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT2_PORT,WIFI_SD_DAT2_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT2_PORT,WIFI_SD_DAT2_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT1_PORT,WIFI_SD_DAT1_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT1_PORT,WIFI_SD_DAT1_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT0_PORT,WIFI_SD_DAT0_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT0_PORT,WIFI_SD_DAT0_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_CMD_PORT,WIFI_SD_CMD_GPIO);
	hyhwGpio_setIn(WIFI_SD_CMD_PORT,WIFI_SD_CMD_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_CLK_PORT,WIFI_SD_CLK_GPIO);
	hyhwGpio_setIn(WIFI_SD_CLK_PORT,WIFI_SD_CLK_GPIO);
	
	#if 0	//828上没有这个IO
	hyhwGpio_setAsGpio(WIFI_HOST_WAKEUP_PORT,WIFI_HOST_WAKEUP_GPIO);
	hyhwGpio_setIn(WIFI_HOST_WAKEUP_PORT,WIFI_HOST_WAKEUP_GPIO);
	#endif
}

void hyhwWifiBt_powerOn(void)
{
	int i, j, delay;
	volatile U32 *WifiBt_Port;
	
	WifiBt_Port = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (PWR_WIFI_BT_ON_PORT*sizeof(tGPIO_REGISTER)));
	hyhwGpio_setAsGpio(PWR_WIFI_BT_ON_PORT,PWR_WIFI_BT_ON_GPIO);
	hyhwGpio_setOut(PWR_WIFI_BT_ON_PORT,PWR_WIFI_BT_ON_GPIO);
	
	i = 40;
	j = 0;
	delay = 0;
	while(i--)
	{
		//hyhwGpio_setHigh(PWR_WIFI_BT_ON_PORT,PWR_WIFI_BT_ON_GPIO);
		(*WifiBt_Port) |= PWR_WIFI_BT_ON_GPIO;
		while(delay--);
		//hyhwGpio_setLow(PWR_WIFI_BT_ON_PORT,PWR_WIFI_BT_ON_GPIO);
		(*WifiBt_Port) &= ~PWR_WIFI_BT_ON_GPIO;
		//syssleep(1);
		hyhwNand_delayUs(100);
		j++;
		delay = j;
	}
	hyhwGpio_setHigh(PWR_WIFI_BT_ON_PORT,PWR_WIFI_BT_ON_GPIO);
	//syssleep(10);	//驱动集成时打开
}

/*----------------------------------------------------------------------------
* 函数:	hyhwWifiBt_powerCtrl
* 功能:	WIFI BT电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwWifiBt_powerCtrl(U8 enable)
{
	hyhwGpio_setAsGpio(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	hyhwGpio_setOut(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	if(1 == enable)
	{
		if(gu8WifiBtPowerCtrl == 0)//之前没有打开过电源
		{
			//hyhwWifiBt_powerOn();
			hyhwGpio_setHigh(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
		}
		gu8WifiBtPowerCtrl++;//记录已经打开过一次电源
	}
	else
	{
		if(gu8WifiBtPowerCtrl > 0)
		{
			gu8WifiBtPowerCtrl--;//计数-1
		}
		if(gu8WifiBtPowerCtrl == 0)//计数到0时真正关闭电源
		{
			hyhwGpio_setLow(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
			
			//bt disable
			#if 0	//828上没有这个IO
			hyhwGpio_setAsGpio(BT_EN_PORT, BT_EN_GPIO);
			hyhwGpio_setOut(BT_EN_PORT, BT_EN_GPIO);
			hyhwGpio_setLow(BT_EN_PORT, BT_EN_GPIO);
			#endif
			//bt reset
			hyhwGpio_setAsGpio(BT_RESET_PORT, BT_RESET_GPIO);
			hyhwGpio_setOut(BT_RESET_PORT, BT_RESET_GPIO);
			hyhwGpio_setLow(BT_RESET_PORT, BT_RESET_GPIO);
			
			//wifi vreg disable
			hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
			hyhwGpio_setOut(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
			hyhwGpio_setLow(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
		}
	}
}
