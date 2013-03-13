/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "hyhwLcd.h"
#include "hyhwGpio.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "HycDeviceConfig.h"


#define DISPLAYMODE_TWO

#ifdef DISPLAYMODE_TWO
U8 gScreen_Display_Mode = 0;	//显示方式
#endif
/*
version index	显示屏厂家		驱动芯片				device code
	0			无显示屏			无						无
	1			信利显示屏		驱动芯片为ER61505V		0xB505
	2			比亚迪屏			驱动芯片为ILI9325C		0x9325
*/
const U16 lcdDeviceCode[]=
{
	0xB505,		//信利显示屏	驱动芯片为ER61505V
	0x9325,		//比亚迪屏	驱动芯片为ILI9325C
	0xFFFF
};
U8 lcdHwVersionIdx=0;



void drvLcd_ClearLcdRAM(void);
void LcdModulePowerOnOff(U8 u8Value);


void LcdModule_LightBack(void)
{
	LcdModulePowerOnOff(1);
}

/*-----------------------------------------------------------------------------
* 函数:	Delay
* 功能:	delay uint 1ms
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Delay1(U32 Time)
{
	U32 i,j;
	for(i=0;i<Time;i++)
	{
		j=50000;
    	while(j--);
    }
}

/*-----------------------------------------------------------------------------
* 函数:	LcdModulePowerOnOff
* 功能:	LCD模块电源控制
* 参数:	u8Value----0:关闭;1:开启 
* 返回:	none
*----------------------------------------------------------------------------*/
/**/
void LcdModulePowerOnOff(U8 u8Value)
{
	if(u8Value == 1)
	{
		//hyhwLcd_lightSet(10);
		hyhwLcd_lightSet(gtHyc.BackLight);
	}
	else
	{
		hyhwLcd_PWMReset();
	}
}


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_LCDModulePowerOn
* 功能:	打开LCD模块电源
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_LCDModulePowerOn(void)
{
	LcdModulePowerOnOff(1);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_LCDModulePowerOff
* 功能:	关闭LCD模块电源
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_LCDModulePowerOff(void)
{
	LcdModulePowerOnOff(0);
}

U8 drvLcd_GetVersionIndex(void)
{
	#if 0
	U16 deviceCode;
	U8  i, versionIndex;
	
	Delay1(18);
	drvLcd_WriteInstruct(0x0000);drvLcd_ReadData(&deviceCode);
	
	i = 0;
	versionIndex = 0;
	while(1)
	{
		if (lcdDeviceCode[i] == deviceCode)
		{
			versionIndex = (i+1);
			break;
		}
		else if (lcdDeviceCode[i] == 0xFFFF)
		{
			break;
		}
		i++;
	}
	
	return versionIndex;
	#else
	return 1;
	#endif
}

void drvLcd_InitLcdModuleInstructs(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_InitLcdModuleInstructs_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_InitLcdModuleInstructs_0x9325();
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_InitLCDModule
* 功能:	
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_InitLCDModule(void)
{
	hyhwLcd_lcdReset();
	LcdModulePowerOnOff(0);
	lcdHwVersionIdx = drvLcd_GetVersionIndex();
	drvLcd_InitLcdModuleInstructs();
	drvLcd_ClearLcdRAM();
}

#ifdef DISPLAYMODE_TWO
void drvLcd_176220DisplayMode(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_176220DisplayMode_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_176220DisplayMode_0x9325();
	}
}
void drvLcd_220176DisplayMode(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_220176DisplayMode_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_220176DisplayMode_0x9325();
	}
}
#endif

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayOn
* 功能:	打开显示
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOn(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_DisplayOn_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_DisplayOn_0x9325();
	}
}
/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayOff
* 功能:	关闭显示
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOff(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_DisplayOff_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_DisplayOff_0x9325();
	}
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetRowColumn
* 功能:	设置彩色显示屏的显示起始地址，地址为行序号和列序号row和Column,以及高度
*		和宽度
* 参数:	kRow 			- 行序号 , 起始行：0
*		kColumn 		- 列序号 ，起始列：0
*		U16 kHeight 	- 高度
*		U16 kWidth 		- 宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn_Lcd(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_SetRowColumn_Lcd_0xB505(kRow, kColumn, kHeight, kWidth);
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_SetRowColumn_Lcd_0x9325(kRow, kColumn, kHeight, kWidth);
	}
}
