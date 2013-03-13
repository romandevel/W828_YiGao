#include "hyTypes.h"
#include "hyhwGPIO.h"
#include "LedCtl.h"
#include "hardWare_Resource.h"



#define LED_PORT				LED1_PORT
#define RED_LED_GPIO			LED2_GPIO
#define GREEN_LED_GPIO			LED3_GPIO
#define BLUE_LED_GPIO			LED1_GPIO

const U32 cLedCtrlIO_info[] = { LED_PORT , RED_LED_GPIO } ;

#define LED_KEY_PAD_TIMEOUT		(500)//5秒
U32 gLedKeyPad_timeCtrl=0;
/*-------------------------------------------------------------------------
* 函数: Led_init
* 说明: 三个LED控制IO的初始化
* 参数: 无
* 返回: 无
-------------------------------------------------------------------------*/
void Led_init(void)
{
	hyhwGpio_setAsGpio(LED_PORT, RED_LED_GPIO);
	hyhwGpio_setAsGpio(LED_PORT, GREEN_LED_GPIO);
	hyhwGpio_setAsGpio(LED_PORT, BLUE_LED_GPIO);
	hyhwGpio_setOut(LED_PORT, RED_LED_GPIO|GREEN_LED_GPIO|BLUE_LED_GPIO);
	hyhwGpio_setHigh(LED_PORT, RED_LED_GPIO|GREEN_LED_GPIO|BLUE_LED_GPIO);
	
	//键盘灯
	hyhwGpio_setAsGpio(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	hyhwGpio_setOut(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	hyhwGpio_setLow(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	return;
}

/*-------------------------------------------------------------------------
* 函数: Led_ctrl
* 说明: LED控制
* 参数: color：0----7	0关闭，7白色
* 返回: 无
-------------------------------------------------------------------------*/
void Led_ctrl(int color)
{
	//RED
	if (color&LED_RED)
	{
		hyhwGpio_setLow(LED_PORT, RED_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, RED_LED_GPIO);
	}
	//GREEN
	if (color&LED_GREEN)
	{
		hyhwGpio_setLow(LED_PORT, GREEN_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, GREEN_LED_GPIO);
	}
	//BLUE
	if (color&LED_BLUE)
	{
		hyhwGpio_setLow(LED_PORT, BLUE_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, BLUE_LED_GPIO);
	}
}

/*-------------------------------------------------------------------------
* 函数: Led_keyPad_ctrl
* 说明: 键盘LED控制
* 参数: enable: 0----关闭，否则打开
* 返回: 无
-------------------------------------------------------------------------*/
void Led_keyPad_ctrl(int enable)
{
	if (enable)
	{
		hyhwGpio_setHigh(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
		gLedKeyPad_timeCtrl = rawtime(NULL);
	}
	else
	{
		hyhwGpio_setLow(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
		gLedKeyPad_timeCtrl = 0xFFFFFFFF;
	}
}

/*-------------------------------------------------------------------------
* 函数: Led_keyPad_timeCtrl
* 说明: 键盘LED时间控制，超时关闭
* 参数: 无
* 返回: 无
-------------------------------------------------------------------------*/
void Led_keyPad_timeCtrl(void)
{
	U32 now;
	
	if (gLedKeyPad_timeCtrl != 0xFFFFFFFF)
	{
		now = rawtime(NULL);
		if ((now-gLedKeyPad_timeCtrl) >= LED_KEY_PAD_TIMEOUT)
		{
			Led_keyPad_ctrl(0);
		}
	}
}