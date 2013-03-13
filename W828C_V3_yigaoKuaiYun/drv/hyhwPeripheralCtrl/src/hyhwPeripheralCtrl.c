
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"

/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwDma.h"
#include "hyhwUart.h"
#include "hyhwIntCtrl.h"
#include "hyhwPmu.h"
#include "hyhwPeripheralCtrl.h"
#include "hardWare_Resource.h"

//TVOUT 20120618 Zeet added.
#define TVOUT_ECMDA_REGISTER	TCC7901_TVOUT_ECMDA_REGISTER
#define POWER_DOWN_MODE			BIT7
#define	TVOUT_DACSEL_REGISTER	TCC7901_TVOUT_DACSEL_REGISTER
#define DAC_OUTPUT_DISABLE		BIT0
#define	TVOUT_DACPD_REGISTER	TCC7901_TVOUT_DACPD_REGISTER
#define DAC_POWER_DOWN			BIT0


void hyhwCmosSensor_powerCtrl(Enable_en enable);
void hyhwGsm_powerDown(void);
//void hyhwPrinter_powerCtrl(Enable_en enable);
void hyhwGps_powerCtrl(Enable_en enable);
void hyhwScanner_powerCtrl(Enable_en enable);
void hyhwLcd_powerCtrl(Enable_en enable);

void hyhwLcd_resetOn(void);
void hyhwLcd_resetOff(void);
void hyhwWifiBt_Init();
void hyhwUSBHOST_powerCtrl(Enable_en enable);

/*-----------------------------------------------------------------------------
* 函数:	hyhwPeripheralCtrl_Init
* 功能:	根据应用需求，关闭不常用的外围设备，以节省功耗。
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwPeripheralCtrl_Init(void)
{
	hyhwCpm_setSoftReset(CKC_USBH);//?
	hyhwCpm_busClkDisable(CKC_USBH);

//	hyhwCpm_setSoftReset(CKC_USBF);//?
//	hyhwCpm_busClkDisable(CKC_USBF);

	hyhwCpm_setSoftReset(CKC_LCD);//?
	hyhwCpm_busClkDisable(CKC_LCD);

	hyhwCpm_setSoftReset(CKC_CAM);//?
	hyhwCpm_busClkDisable(CKC_CAM);

	hyhwCpm_setSoftReset(CKC_HDD);//?
	hyhwCpm_busClkDisable(CKC_HDD);

	hyhwCpm_setSoftReset(CKC_DMA);
	hyhwCpm_busClkDisable(CKC_DMA);

	hyhwCpm_setSoftReset(CKC_SDMMC);
	hyhwCpm_busClkDisable(CKC_SDMMC);

//	hyhwCpm_setSoftReset(CKC_MSTICK);
//	hyhwCpm_busClkDisable(CKC_MSTICK);

//	hyhwCpm_setSoftReset(CKC_I2C);
//	hyhwCpm_busClkDisable(CKC_I2C);

//	hyhwCpm_setSoftReset(CKC_NAND);
//	hyhwCpm_busClkDisable(CKC_NAND);

	hyhwCpm_setSoftReset(CKC_EHI0);
	hyhwCpm_busClkDisable(CKC_EHI0);
	
	hyhwCpm_setSoftReset(CKC_UART0);
	hyhwCpm_busClkDisable(CKC_UART0);
	
	hyhwCpm_setSoftReset(CKC_UART1);
	hyhwCpm_busClkDisable(CKC_UART1);

	hyhwCpm_setSoftReset(CKC_GPSB0);
	hyhwCpm_busClkDisable(CKC_GPSB0);

	hyhwCpm_setSoftReset(CKC_DAI);//Checked
	hyhwCpm_busClkDisable(CKC_DAI);
	
	hyhwCpm_setSoftReset(CKC_UART2);
	hyhwCpm_busClkDisable(CKC_UART2);
	
	hyhwCpm_setSoftReset(CKC_UART2);
	hyhwCpm_busClkDisable(CKC_UART2);
	
//	hyhwCpm_setSoftReset(CKC_ECC);
//	hyhwCpm_busClkDisable(CKC_ECC);
	
	hyhwCpm_setSoftReset(CKC_SPDIF);
	hyhwCpm_busClkDisable(CKC_SPDIF);
	
	hyhwCpm_setSoftReset(CKC_GPSB1);
	hyhwCpm_busClkDisable(CKC_GPSB1);
	
	hyhwCpm_setSoftReset(CKC_SUBP);
	hyhwCpm_busClkDisable(CKC_SUBP);
	
	hyhwCpm_setSoftReset(CKC_G2D);
	hyhwCpm_busClkDisable(CKC_G2D);
	
//	hyhwCpm_setSoftReset(CKC_RTC);
//	hyhwCpm_busClkDisable(CKC_RTC);
	
//	hyhwCpm_setSoftReset(CKC_EMC);
//	hyhwCpm_busClkDisable(CKC_EMC);
	
	hyhwCpm_setSoftReset(CKC_SUB);
	hyhwCpm_busClkDisable(CKC_SUB);
	
//	hyhwCpm_setSoftReset(CKC_ADC);
//	hyhwCpm_busClkDisable(CKC_ADC);
	
	hyhwCpm_setSoftReset(CKC_VCH);
	hyhwCpm_busClkDisable(CKC_VCH);
	
//	hyhwCpm_setSoftReset(CKC_TIMER);
//	hyhwCpm_busClkDisable(CKC_TIMER);
	
//	hyhwCpm_setSoftReset(CKC_MAIN);
//	hyhwCpm_busClkDisable(CKC_MAIN);
	
//	hyhwCpm_setSoftReset(CKC_VIC);
//	hyhwCpm_busClkDisable(CKC_VIC);
	
	hyhwCpm_setSoftReset(CKC_MTM);
	hyhwCpm_busClkDisable(CKC_MTM);
	
	hyhwCpm_setSoftReset(CKC_MAILBOX);
	hyhwCpm_busClkDisable(CKC_MAILBOX);
	
//	hyhwCpm_setSoftReset(CKC_MBC);
//	hyhwCpm_busClkDisable(CKC_MBC);
	
	hyhwCpm_setSoftReset(CKC_UART3);
	hyhwCpm_busClkDisable(CKC_UART3);
	
//	hyhwCpm_setSoftReset(CKC_SRAM);
//	hyhwCpm_busClkDisable(CKC_SRAM);
	
	hyhwCpm_setSoftReset(CKC_GPSB2);
	hyhwCpm_busClkDisable(CKC_GPSB2);
	
	hyhwCpm_setSoftReset(CKC_GPSB3);
	hyhwCpm_busClkDisable(CKC_GPSB3);
	
	hyhwCpm_setSoftReset(CKC_EHI1);
	hyhwCpm_busClkDisable(CKC_EHI1);
	
//	hyhwCpm_setSoftReset(CKC_VE);//Done!
//	hyhwCpm_busClkDisable(CKC_VE);

	hyhwCpm_clearSoftReset(CKC_VE);//
	hyhwCpm_busClkEnable(CKC_VE);//
//20120618 Zeet added for TVOUT function disable
	TVOUT_DACSEL_REGISTER |= 0x0000;
	TVOUT_DACPD_REGISTER  |= 0x0001;
	TVOUT_ECMDA_REGISTER  |= 0x0008;
	hyhwCpm_busClkDisable(CKC_VE);//
/**/
	//PMU init
	act8600_probe_init();

	hyhwGsm_powerDown();
	
	#if 1
	{
		int i;
		hyhwCmosSensor_powerCtrl(EnableOn);
		hyhwCmosSensor_Reset(EnableOn);
	    i=4000;          //200000
	    while(i--);//Delayms(20);
	    //syssleep(2);
		hyhwCmosSensor_Reset(EnableOff);
		
		hyhwCmosSensor_powerCtrl(EnableOff);
	}
	#else
		hyhwCmosSensor_powerCtrl(EnableOff);
	#endif
	
	hyhwScanner_powerCtrl(EnableOff);
	Gps_HwDeInit();
	hyhwGps_powerCtrl(EnableOff);
	hyhwLcd_powerCtrl(EnableOff);
	hyhwLcd_resetOn();
	hyhwLcd_flashDisable();//disable 闪光灯
	
	hyhwWifiBt_Init();
	hyhwUSBHOST_powerCtrl(EnableOff);
	
	return;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwSystem_powerCtrl
* 功能:	VDD_3D0_SYS电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSystem_powerCtrl(Enable_en enable)
{
	//act8600_output_enable(ACT8600_OUT2, enable);
}


/*----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_powerCtrl
* 功能:	Cmos Sensor电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_powerCtrl(Enable_en enable)
{
	act8600_output_setVoltage(ACT8600_OUT5, ACT8600_2V8);
	act8600_output_enable(ACT8600_OUT5, enable);
	
	act8600_output_setVoltage(ACT8600_OUT8, ACT8600_1V5);
	act8600_output_enable(ACT8600_OUT8, enable);
	
	if (enable == EnableOff)
	{
		hyhwCmosSensor_port_powerSave();
	}
}


/*----------------------------------------------------------------------------
* 函数:	hyhwScanner_powerCtrl
* 功能:	扫描头电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_powerCtrl(Enable_en enable)
{
	act8600_output_enable(ACT8600_OUT6, enable);
}


/*----------------------------------------------------------------------------
* 函数:	hyhwGps_powerCtrl
* 功能:	VDD_3D0_GPS电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGps_powerCtrl(Enable_en enable)
{
	hyhwGpio_setAsGpio(POWER_GPS_CTRL_PORT, POWER_GPS_CTRL_GPIO);
	hyhwGpio_setOut(POWER_GPS_CTRL_PORT, POWER_GPS_CTRL_GPIO);
	if (enable == EnableOn)
	{
		hyhwGpio_setHigh(POWER_GPS_CTRL_PORT, POWER_GPS_CTRL_GPIO);
	}
	else
	{
		hyhwGpio_setLow(POWER_GPS_CTRL_PORT, POWER_GPS_CTRL_GPIO);
	}
}

/*----------------------------------------------------------------------------
* 函数:	hyhwUSBHOST_powerCtrl
* 功能:	USB host电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwUSBHOST_powerCtrl(Enable_en enable)
{
	act8600_output_enable(ACT8600_OUT4,enable);
	act8600_set_q1(enable);
}


/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_powerCtrl
* 功能:	VDD_3D3_SCAN电源控制
* 参数:	enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_powerCtrl(Enable_en enable)
{
	hyhwGpio_setAsGpio(PWR_LCD_ON_PORT, PWR_LCD_ON_GPIO);
	hyhwGpio_setOut(PWR_LCD_ON_PORT, PWR_LCD_ON_GPIO);
	if (enable == EnableOn)
	{
		hyhwGpio_setHigh(PWR_LCD_ON_PORT, PWR_LCD_ON_GPIO);
	}
	else
	{
		hyhwGpio_setLow(PWR_LCD_ON_PORT, PWR_LCD_ON_GPIO);
	}
	
	
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_lcdReset
* 功能:	对LCD RESET
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_lcdReset(void)
{
	U32 i;
	
	hyhwGpio_setAsGpio(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setOut(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setLow(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	for(i=0;i<0x100000;i++);
	hyhwGpio_setHigh(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);

}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_resetOn
* 功能:	Lcd reset
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_resetOn(void)
{
	hyhwGpio_setAsGpio(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setOut(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setLow(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	
	//关闭内部LCD模块clk
//	hyhwCpm_setSoftReset(CKC_LCD);
//	hyhwCpm_busClkDisable(CKC_LCD);
	hyhwLcd_disableCtrler();
	hyhwDma_Disable(0);//lcd dma
}
/*----------------------------------------------------------------------------
* 函数:	hyhwLcd_resetOff
* 功能:	清除Lcd reset
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_resetOff(void)
{
	//开启内部LCD模块clk
//	hyhwCpm_busClkEnable(CKC_LCD);
//	hyhwCpm_clearSoftReset(CKC_LCD);
	
	hyhwGpio_setAsGpio(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setOut(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	hyhwGpio_setHigh(AP_LD_NRST_PORT, AP_LD_NRST_GPIO);
	
	hyhwDma_lcdRgbConfig(LcdBusType16, Get_Display_RgbBuf_Offset());
	hyhwLcd_enableCtrler();
}

/*----------------------------------------------------------------------------
* 函数:	hyhwJpegG2d_Ctrl
* 功能:	打开或关闭硬件jpeg和2D加速
* 参数:	OnOff---非0打开；0关闭
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwJpegG2d_Ctrl(int OnOff)
{
	if (OnOff != 0)
	{
		hyhwCpm_busClkEnable(CKC_MTM);
		hyhwCpm_clearSoftReset(CKC_MTM);
		
		//打开硬件解码设备
		hyhwCpm_busClkEnable(CKC_G2D);
	    hyhwCpm_clearSoftReset(CKC_G2D);
    }
    else
    {
		hyhwCpm_busClkDisable(CKC_MTM);
		
		//关闭硬件解码设备
        hyhwCpm_setSoftReset(CKC_G2D);
		hyhwCpm_busClkDisable(CKC_G2D);
    }
}