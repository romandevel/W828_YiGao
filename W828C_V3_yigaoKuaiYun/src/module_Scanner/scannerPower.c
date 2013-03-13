/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwGpio.h"
#include "scanner.h"



/*-----------------------------------------------------------------------------
* 函数:	hyhwScanner_TrigInit
* 功能:	扫描头trig IO初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigInit(void)
{
	hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setHigh(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwScanner_TrigDisable
* 功能:	扫描头trig IO初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigDisable(void)
{
	hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setIn(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_pullDisable(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwScanner_TrigCtrl
* 功能:	扫描头trig IO控制，低电平有效
* 参数:	isEnable----0,禁止；否则允许
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigCtrl(int isEnable)
{
	U8 scannerType;
	
	scannerType = scanner_getType();
	if (scannerType == SCANNER_TYPE_SYMBOL)
	{
		//symbol
		if (isEnable)
		{
			hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setLow(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		}
		else
		{
			hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setHigh(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		}
	}
	else
	{
		//opticon
		static U32 scanKeyCnt = 0;
		if (isEnable)
		{
			//扫描键按下
			scanKeyCnt++;
			if (scanKeyCnt > 6000) scanKeyCnt = 6000;//一分钟
		}
		else
		{
			//扫描键抬起
			scanKeyCnt = 0;
		}
		barScanKey_laserCtrl(scanKeyCnt);
	}
}

