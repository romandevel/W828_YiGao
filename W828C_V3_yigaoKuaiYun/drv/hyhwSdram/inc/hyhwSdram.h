/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

#ifndef HY_HW_SDRAM_H_
#define HY_HW_SDRAM_H_

// 使用的SDRAM的型号，以后需要放在整个系统的配置文件中, 现在放在target中
//#define 	SD_4M32BIT
#define 	SD_8M32BIT
//#define 	SD_16M32BIT




/*-----------------------------------------------------------------------------
* 函数:	hyhwSdram_Init
* 功能:	初始化EMI中控制SDRAM的寄存器, 主要是SDRAM的读写wait Cycle等
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_init(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwSdram_PowerDown
* 功能:	SDRAM AUTO POWERDOWN Enable or disable
* 参数:	EnableOff or EnableOn
* 返回:	none
*----------------------------------------------------------------------------*/
//void hyhwSdram_powerDown(eENABLE enablePowerDown);
/*-----------------------------------------------------------------------------
* 函数:	hySdram_Standby
* 功能:	SDRAM进入keeping模式，这时不能读写SDRAM，但是SDRAM中的数据保持，这个函数主要用于
*		在nandFlash读写时，设置了CE, 如果对SDRAM不进行这样的保护，SDRAM中的数据可能被破坏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_standby(void);
/*-----------------------------------------------------------------------------
* 函数:	hySdram_Wakeup
* 功能:	SDRAM离开keeping模式，和hySdram_Standby配合使用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_wakeup(void);
/*-----------------------------------------------------------------------------
* 函数:	hySdram_powerDown
* 功能:	SDRAM AUTO POWERDOWN Enable or disable
* 参数:	EnableOff or EnableOn
* 返回:	none
*----------------------------------------------------------------------------*/
//void hyhwSdram_powerDown(eENABLE enablePowerDown);
/*-----------------------------------------------------------------------------
* 函数:	hySdram_autoPowerSave
* 功能:	SDRAM AUTO POWER save
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hySdram_autoPowerSave( void );

/*-----------------------------------------------------------------------------
* 函数:	hyhwScanSdramCapability
* 功能:	扫描Sdram的大小（扫描每个2M bytes的最后一个short）
*		现在假定Sdram只会用到2，4，8，16，32M bytes
*		注：这里将Sdram的起始地址固定为0x20000000
*			此函数必须在MMU disable后使用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanSdramCapability(void);
	

#endif //HY_HW_SDRAM_H_