/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/


/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwTcc7901.h"
#include "hyhwSdram.h"
#include "hyhwGpio.h"
/*************************************************************************
 * DEFINES
 *************************************************************************/

// register define
#define		EMI_SD_CONFIG_REGISTER						TCC7901_EMI_SDCFG_REGISTER	
	#define DMCR_BIT_CAS_LATENCY_3CYCLE					BIT31	
	#define	DMCR_BIT_CAS_LATENCY_2CYCLE					~BIT31	
	#define DMCR_BIT_DATA_BUS_WIDTH_16					BIT30	
	#define DMCR_BIT_DATA_BUS_WIDTH_32					~BIT30	
	#define	DMCR_BIT_CAS_WIDTH_MASK						~(BIT29|BIT28)				/* the first step is setthe mask-bit to zero	*/
	#define DMCR_BIT_CAS_WIDTH_8						BIT28
	#define DMCR_BIT_CAS_WIDTH_9						BIT29
	#define DMCR_BIT_CAS_WIDTH_10						BIT28|BIT29
	#define DMCR_BIT_DELAY_REFRESH_MASK					~(BIT27|BIT26|BIT25|BIT24)	/*	BIT[27:24] -- Delay of Refresh to Idle	*/
	#define DELAY_REFRESH_SHIFT							24					
	#define DMCR_BIT_DELAY_REFRESH(X)					(X<<DELAY_REFRESH_SHIFT) 
	#define DMCR_BIT_RAS_TO_CAS_DELAY_MASK				~(BIT23|BIT22|BIT21|BIT20) 	/*	BIT[23:20] -- RAS-CAS Delay */
	#define RAS_TO_CAS_DELAY_SHIFT						20
	#define DMCR_BIT_RAS_TO_CAS_DELAY(X)				(X<<RAS_TO_CAS_DELAY_SHIFT)
	#define DMCR_BIT_WRITE_TO_READY_DELAY_MASK			~(BIT19|BIT18) 				/*	BIT[19:18] -- RAS-CAS Delay */
	#define WRITE_TO_READY_DELAY_SHIFT					18
	#define DMCR_BIT_WRITE_TO_READY_DELAY_1				0<<WRITE_TO_READY_DELAY_SHIFT
	#define DMCR_BIT_WRITE_TO_READY_DELAY_2				1<<WRITE_TO_READY_DELAY_SHIFT
	#define DMCR_BIT_WRITE_TO_READY_DELAY_3				2<<WRITE_TO_READY_DELAY_SHIFT
	#define DMCR_BIT_WRITE_TO_READY_DELAY_4				3<<WRITE_TO_READY_DELAY_SHIFT
	#define DMCR_BIT_PRECHARGE_TO_REFRESH_MASK		    ~(BIT15|BIT14|BIT13|BIT12)	/*  BIT[15:12] -- Precharge to Refresh Cycle */
	#define PRECHARGE_TO_REFRESH_SHIFT			      	12	
	#define DMCR_BIT_PRECHARGE_TO_REFRESH(X)	    	(X<<PRECHARGE_TO_REFRESH_SHIFT)
	#define DMCR_BIT_PRECHARGE_TO_REFRESH_15		    15<<PRECHARGE_TO_REFRESH_SHIFT
	#define DMCR_BIT_RAS_WIDTH_MASK						~(BIT11|BIT10) 				/* 	BIT[11:10]		-- RAS Width	*/
	#define RAS_WIDTH_SHIFT								10
	#define DMCR_BIT_RAS_WIDTH_11						BIT10	
	#define DMCR_BIT_RAS_WIDTH_12						~(BIT11|BIT10)
	#define DMCR_BIT_RAS_WIDTH_13						BIT11
	#define DMCR_BIT_ADDR_MATCH_CONFIG_BANK_RAS_CAS		~BIT3	
	#define DMCR_BIT_ADDR_MATCH_CONFIG_RAS_BANK_CAS		BIT3
	#define DMCR_BIT_PRECHARGE_POWER_DOWN_ENABLE		BIT1
	#define	DMCR_BIT_SELF_REFRESH_ENABLE				BIT0
		
#define		EMI_SD_MISCELL_CONFIG_REGISTER				TCC7901_EMI_MCFG_REGISTER
	#define DMCR_BIT_EXTERNAL_READY_STATUS				BIT15						/* Read-only registers for external configuration */
	#define DMCR_BIT_DATA_BUS_OUTPUT_MODE_OUTPUT		BIT14	 
	#define DMCR_BIT_DATA_BUS_OUTPUT_MODE_INPUT			~BIT14	 
	#define DMCR_BIT_BAND_WIDTH							BIT12|BIT11					/* Read-only registers for external configuration */
	#define	DMCR_BIT_MAIN_PROCESS_CLK_SELECTION_MASK	~(BIT10|BIT9|BIT8)			//3bit config, may be there are 8 CLK CONTER to be selelct
	#define MAIN_PROCESS_CLK_SELECTION_SHIFT			8
	#define	DMCR_BIT_MAIN_PROCESS_CLK_SELECTION(X)		(X<<MAIN_PROCESS_CLK_SELECTION_SHIFT)	
	#define	DMCR_BIT_SDRAM_HIGH_FREQUENCY_WAIT_ENABLE	BIT7	
	#define	DMCR_BIT_SDCLK_DYNAMIC_CONTROL_DIRECT_OUTPUT	~BIT6
	#define	DMCR_BIT_SDCLK_DYNAMIC_CONTROL_GATE_CONTROL		BIT6	
	#define	DMCR_BIT_SDCLK_LOW							~BIT5	
	#define	DMCR_BIT_SDCLK_CONTROLLED					BIT5	
	#define	DMCR_BIT_SD_CONTROLLER_ENABLE				BIT4
	#define	DMCR_BIT_REFRESH_EMERGENT_ENABLE			BIT3
	#define	DMCR_BIT_REFRESH_PENDING_ENABLE				BIT2
	#define	DMCR_BIT_SD_CLK_UNMASKED					BIT1
	#define	DMCR_BIT_EXTERNAL_REFRESH_CLK_ENABLE		BIT0

#define		EMI_SD_SDRAM_CLK_CONFIG_REGISTER			TCC7901_EMI_CLKCFG_REGISTER
	#define DMCR_BIT_MASKED_PERIOD_OF_SDCLK_MASK		~(BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
	#define DMCR_BIT_MASKED_PERIOD_OF_SDCLK(X)			X	
	
#if 0
#define		EMI_SD_CMD_REGISTER							TCC7901_MEMORY_CTRL_SDCMD_REGISTER
	#define DMCR_BIT_CLK_ENABLE							BIT17
	#define DMCR_BIT_A10_PORT							BIT16
	#define DMCR_BIT_CSN								BIT15
	#define DMCR_BIT_RAW_ADDR_STROBE					BIT14
	#define DMCR_BIT_COLUMN_ADDR_STROBE					BIT13
	#define DMCR_BIT_WRITE_ENABLE						BIT12
	#define DMCR_BIT_BANK_ADDR_MASK						BIT11|BIT10
	#define BANK_ADDR_SHIFT								10
	#define	DMCR_BIT_BANK_ADDR(X)						(X<<BANK_ADDR_SHIFT)	
	#define DMCR_BIT_CAS_ADDR_MASK						(BIT9|BIT8|BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
	#define CAS_ADDR_SHIFT								0
	#define	DMCR_BIT_CAS_ADDR(X)						(X<<CAS_ADDR_SHIFT)	
#endif
	
#define DMCR_MASK_CLEAR									0
#define SDRAM_BASE_ADDRESS								0x20000000	
#define SDRAM_SCAN_STEP									(0x00200000)

U32 sgSdramCapability = 8;	//Sdram的容量（以Mbytes为单位）

static const U32 scSdramScanFirstValueTable[5] =
{
	//检测Sdram时，(SDRAM_BASE_ADDRESS + SDRAM_SCAN_STEP - 2) 的值
	16,15,13,9,1
};
static const U32 scSdramCapabilityTable[5] =
{
	//Sdram的容量表（以Mbytes为单位）
	2,4,8,16,32
};

//------------------------------------------------------
// define SDRAM timing parameters, 这个表应该放在另一个h 文件中？ 有两个变化因素，一个是sdram型号，另一个是MCLK
// tRCD > 20 ns		corresponding to DMCR.RCD
// tRP  > 20 ns		corresponding to DMCR.TPC
// tWR  > 1CLK+7.5 ns	corresponding to DMCR.TRWL
// tRAS > 50 ns		corresponding to DMCR.TRAS
// tRC  > 70 ns		corresponding to DMCR.TRC
//------------------------------------------------------
/*-----------------------------------------------------------------------------
* 函数:	hySdram_init
* 功能:	初始化EMI中控制SDRAM的寄存器, 主要是SDRAM的读写wait Cycle等
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
#if 0
void hyhwSdram_init(void)
{
	TCC7901_SCFG_GP_XA0_DRV_REGISTER	= 0xffffffff;
	TCC7901_SCFG_GP_XA1_DRV_REGISTER	= 0xffffffff;
	TCC7901_SCFG_GP_XD0_DRV_REGISTER	= 0xffffffff;
	TCC7901_SCFG_GP_XD1_DRV_REGISTER	= 0xffffffff;
	hyhwCpm_setSdramClk(133);
//	
	hyhwGpio_setEmiFunction();
	

	
	EMI_SD_CONFIG_REGISTER	= (	DMCR_BIT_CAS_LATENCY_3CYCLE|DMCR_BIT_DELAY_REFRESH(7)|DMCR_BIT_RAS_TO_CAS_DELAY(2)|	\
								DMCR_BIT_WRITE_TO_READY_DELAY_3|DMCR_BIT_PRECHARGE_TO_REFRESH(2) );
	
#ifdef SD_4M32BIT	
	EMI_SD_CONFIG_REGISTER |= DMCR_BIT_CAS_WIDTH_8;
	EMI_SD_CONFIG_REGISTER &= DMCR_BIT_RAS_WIDTH_12;
#elif  defined(SD_8M32BIT)				
	EMI_SD_CONFIG_REGISTER |= (DMCR_BIT_CAS_WIDTH_9);
	EMI_SD_CONFIG_REGISTER &= DMCR_BIT_RAS_WIDTH_12;
#elif  defined(SD_16M32BIT)
	EMI_SD_CONFIG_REGISTER |= (DMCR_BIT_RAS_WIDTH_13|DMCR_BIT_CAS_WIDTH_9);
#endif 

	EMI_SD_MISCELL_CONFIG_REGISTER =  (	DMCR_BIT_SDCLK_CONTROLLED|DMCR_BIT_SD_CONTROLLER_ENABLE|DMCR_BIT_SDRAM_HIGH_FREQUENCY_WAIT_ENABLE|	\
										DMCR_BIT_SD_CLK_UNMASKED|DMCR_BIT_EXTERNAL_REFRESH_CLK_ENABLE );		
	
	
}


/*-----------------------------------------------------------------------------
* 函数:	hySdram_standby
* 功能:	SDRAM进入keeping模式，这时不能读写SDRAM，但是SDRAM中的数据保持，这个函数主要用于
*		在nandFlash读写时，设置了CE, 如果对SDRAM不进行这样的保护，SDRAM中的数据可能被破坏
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_standby(void)	
{
	EMI_SD_MISCELL_CONFIG_REGISTER	&=	(~DMCR_BIT_SDCLK_CONTROLLED);	
	EMI_SD_CONFIG_REGISTER			|= 	DMCR_BIT_SELF_REFRESH_ENABLE;
}
/*-----------------------------------------------------------------------------
* 函数:	hySdram_wakeup
* 功能:	SDRAM离开keeping模式，和hySdram_Standby配合使用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_wakeup(void)	
{	
	EMI_SD_MISCELL_CONFIG_REGISTER	|=	(DMCR_BIT_SDCLK_CONTROLLED);	
	EMI_SD_CONFIG_REGISTER			&= 	~DMCR_BIT_SELF_REFRESH_ENABLE;
}
#endif
/*-----------------------------------------------------------------------------
* 函数:	hySdram_powerDown
* 功能:	SDRAM AUTO POWERDOWN Enable or disable
* 参数:	EnableOff or EnableOn
* 返回:	none
*----------------------------------------------------------------------------*/
/*
void hyhwSdram_powerDown(eENABLE enablePowerDown)
{
	if(enablePowerDown == EnableOff)
	{
		EMI_SD_CONFIG_REGISTER		&=	(~DMCR_BIT_PRECHARGE_POWER_DOWN_ENABLE);
	}
	else
	{
		EMI_SD_CONFIG_REGISTER		|=	(DMCR_BIT_PRECHARGE_POWER_DOWN_ENABLE);
	}


}	
*/
/*-----------------------------------------------------------------------------
* 函数:	hySdram_autoPowerSave
* 功能:	SDRAM AUTO POWER save
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hySdram_autoPowerSave( void )
{
	EMI_SD_MISCELL_CONFIG_REGISTER 	  =  (	DMCR_BIT_SDCLK_CONTROLLED|
											DMCR_BIT_SD_CONTROLLER_ENABLE|
											//DMCR_BIT_SDRAM_HIGH_FREQUENCY_WAIT_ENABLE|
											DMCR_BIT_SD_CLK_UNMASKED|
											DMCR_BIT_EXTERNAL_REFRESH_CLK_ENABLE );		
	
	EMI_SD_MISCELL_CONFIG_REGISTER	 |=	DMCR_BIT_SDCLK_DYNAMIC_CONTROL_GATE_CONTROL;
	
	//EMI_SD_CONFIG_REGISTER			|= 	DMCR_BIT_SELF_REFRESH_ENABLE;
	
	//EMI_SD_SDRAM_CLK_CONFIG_REGISTER &= DMCR_BIT_MASKED_PERIOD_OF_SDCLK_MASK;
//	EMI_SD_SDRAM_CLK_CONFIG_REGISTER |= BIT1;

	TCC7901_EMI_SDCFG1_REGISTER		= 0x03030303;
}	
#if 1
/*-----------------------------------------------------------------------------
* 函数:	hyhwScanSdramCapability
* 功能:	扫描Sdram的大小（扫描每个2M bytes的最后一个short）
*		现在假定Sdram只会用到2，4，8，16，32M bytes
*		注：这里将Sdram的起始地址固定为0x20000000
*			此函数必须在MMU disable后使用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwScanSdramCapability(void)
{
	U32 i;
	U32 buffer[16];
	U32 *pSdram;
	
	for (i = 1;i <= 16;i++)
	{
		pSdram = (U32 *)(SDRAM_BASE_ADDRESS + i*SDRAM_SCAN_STEP - 4);
		buffer[i-1] = *pSdram;
		*pSdram = i;
	}
	pSdram = (U32 *)(SDRAM_BASE_ADDRESS + SDRAM_SCAN_STEP - 4);
	for (i = 0;i < 5;i++)
	{
		if (*pSdram == scSdramScanFirstValueTable[i])
		{
			sgSdramCapability = scSdramCapabilityTable[i];
			break;
		}
	}
#if 1
	//回写
	for (i = 1;i <= sgSdramCapability/2;i++)
	{
		pSdram = (U32 *)(SDRAM_BASE_ADDRESS + i*SDRAM_SCAN_STEP - 4);
		*pSdram = buffer[i-1];
	}
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSdramGetCapability
* 功能:	获得Sdram的容量
* 参数:	none
* 返回:	容量（以Mbytes为单位）
*----------------------------------------------------------------------------*/
U32 hyhwSdramGetCapability(void)
{
	return sgSdramCapability;
}
#endif

void hyhwNotUsedBlockShortDown(void)
{
//	*( volatile U32* )(0x90006014) &= ~(BIT4|BIT6|BIT8|BIT11|BIT14|BIT18|BIT19|BIT20|BIT21|BIT23|BIT24|BIT25|BIT30);
// 	*( volatile U32* )(0x90006018) |= (BIT4|BIT6|BIT8|BIT11|BIT14|BIT18|BIT19|BIT20|BIT21|BIT23|BIT24|BIT25|BIT30);

}
