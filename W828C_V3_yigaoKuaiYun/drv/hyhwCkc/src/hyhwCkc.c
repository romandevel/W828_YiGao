
/*----------------------------------------------------------------------------
   Standard include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwIIC.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwPmu.h"

#define	tca_wait()				{ }//volatile S32 i; for (i=0; i<0x10000; i++); }
tSYSCLKREGISTER_ST  SysClkRegister;

/* add begin
   added by yanglijing, 20090910, suggested by gaowenzhong
   for system clk changing stable 
注:   200M 以上，这些位要为1, 在复位状态下这些位已经为1
*/   
#define ECFG0_REGISTER     TCC7901_ECFG0_REGISTER
    #define SDW_SUBCORE_DTCM_WAIT_BIT  BIT14
    #define IW_MAINCORE_ITCM_WAIT_BIT  BIT13
    #define DW_MAINCORE_DTCM_WAIT_BIT  BIT12
// add end

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_minSubCpu()
* 功能:	set sub process clock divider 为12 M 直接提供，不经过分频
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_minSubCpu(void)
{
	CKC_CLKCTRL_REGISTER	&= ~0xf000;
	CKC_CLKCTRL_REGISTER	|= (1 << 12);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_closeSubCpu()
* 功能:	close processor
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_closeSubCpu(void )
{
	CKC_BCLKCTR0_REGISTER	&= ~(IO_CKC_BUS_VideoC|IO_CKC_BUS_VideoH|IO_CKC_BUS_VideoP);
	CKC_BCLKCTR1_REGISTER	&= ~BIT5;	 					// Video Encoder
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_closeSubCpu()
* 功能:	open sub processor
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCmp_openSubCpu(void)
{
	CKC_BCLKCTR0_REGISTER	|= (BIT19|BIT23|BIT25);
	CKC_BCLKCTR1_REGISTER	|= BIT5;
	
	CKC_SWRESET0_REGISTER	|= (BIT19|BIT23|BIT25);
	CKC_SWRESET1_REGISTER	|= BIT5;	
	
	CKC_SWRESET0_REGISTER	&= ~(BIT19|BIT23|BIT25);	
	CKC_SWRESET1_REGISTER	&= ~BIT5;
}


/* 
   TCC7901
   32768Hz      0.9V, 这个不能从EXTERCLK 分频，应当选择 CLK_32k
   <12M         1.1V
   12M~266M     1.2V
   266~332      1.3V
   332~399      1.4V
   399~498      1.55V
*/
/*------------------------------------------------------------------------------
* 函数:	hyhwSysClk_getVoltage
* 功能:	根据cpu和bus频率获取cpu和mem的电压目标值
* 参数:	cpuClk----cpu频率以MHz为单位----in
*		busClk----bus频率以MHz为单位----in
*		pCpuVoltage---cpu电压----out
*		pMemVoltage---mem电压----out
* 返回:	HY_OK		-- 成功
*		HY_ERRORR	-- 参数错误
-----------------------------------------------------------------------------*/
int hyhwSysClk_getVoltage(U32 cpuClk, U32 busClk, 
						eACT8600_VOLTAGE *pCpuVoltage,
						eACT8600_VOLTAGE *pMemVoltage)
{
	if (pCpuVoltage==NULL || pMemVoltage==NULL) return HY_ERROR;
	
	if(cpuClk >= 399000)
	{
		*pCpuVoltage = ACT8600_1V6;
	}
	else if(cpuClk >= 332000)
	{
		*pCpuVoltage = ACT8600_1V4;
	}
	else if(cpuClk >= 266000)
	{
		*pCpuVoltage = ACT8600_1V3;
	}
	else
	{
		*pCpuVoltage = ACT8600_1V2;
	}

	if(busClk >= 133000)
	{
		// Sdram 总线速度高于133MHz时，LDO3输出电压需要提升至1.95V
		*pMemVoltage = ACT8600_1V95;
	}
	else
	{
		*pMemVoltage = ACT8600_1V8;
	}
	
	return HY_OK;
}

/*------------------------------------------------------------------------------
* 函数:	hyhwSysClk_Set
* 功能: 设置系统时钟
*			PLL的时钟来源为硬件晶振
*				PLL1为系统提供时钟
*			ICLK,HCLK,PCLK,MCLK的时钟源为PLL
*			此四种CLK的设置是独立的，但因为有外部器件的读写操作，
*			故必须遵从以下规则：（否则可能会造成对外联系的失败）
*				1.ICLK必须是HCLK的整数倍
*				2.HCLK必须是PCLK的整数倍
*				3.HCLK必须是MCLK的一倍或两倍
*			   
* 参数: 
*		sysclk系统时钟，枚举
    	cfgExtal        外部时钟 EXTAL_CLK 
    	例：hyhwSysClk_Set(ICLK266M_AHB133M,EXTAL_CLK)
    	
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
-----------------------------------------------------------------------------*/
U32 hyhwSysClk_Set(eSYS_Clk_EN sysclk, U32 cfgExtal)
{
	U8 interrupt;
	U32 regBuff,pllOut;
	U32 hdiv;
	U32 mCpuClk, fSysClk, busClk;
	U32 cpuVoltageFlag, memVoltageFlag;		//0---后调整电压，1---先调整电压，2---不调整
	eACT8600_VOLTAGE cpuVoltage=ACT8600_1V4, memVoltage=ACT8600_1V8;
	eACT8600_VOLTAGE cpuVoltage_last=ACT8600_1V4, memVoltage_last=ACT8600_1V8;
	U32 rc = HY_OK;
	
	pllOut = PLLFREQ((sysclk>>24),((sysclk>>16)&0xff),((sysclk>>8)&0xff));
	
	mCpuClk = pllOut/1000;
	fSysClk = pllOut/(sysclk&0xff)/1000;
	
	hdiv   = ((sysclk&0xff)-1);
	busClk = (pllOut/(hdiv+1));
	
	/*	关于频率调整和电压设置关系，目标是尽量减少电压调整次数(IIC通讯速度较低)
		升频，先调整电压到目标值，再调整频率
		降频，先调整频率，再调整电压到目标值
		注：
		在进入低功耗时，频率调整在低功耗函数做，
		对SysClkRegister.mCpuClk赋值为0(32kHz),表示<1MHz
		所以在退出低功耗调整频率时，必须先调整电压
	*/
	
	//计算core电压目标值
	hyhwSysClk_getVoltage(mCpuClk, busClk, &cpuVoltage, &memVoltage);
	hyhwSysClk_getVoltage(SysClkRegister.mCpuClk, SysClkRegister.busClk,
						&cpuVoltage_last, &memVoltage_last);
	//判断先调整电压还是后调整电压
#if 1
	//CPU
	cpuVoltageFlag = 2;	//默认为不调整电压
	if (cpuVoltage > cpuVoltage_last)
	{
		//CPU 先调整电压
		cpuVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	else if (cpuVoltage < cpuVoltage_last)
	{
		//CPU 后调整电压
		cpuVoltageFlag = 0;
	}
	
	//MEM
	memVoltageFlag = 2;	//默认为不调整电压
	if (memVoltage > memVoltage_last)
	{
		//MEM 先调整电压
		memVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
	}
	else if (memVoltage < memVoltage_last)
	{
		//MEM 后调整电压
		memVoltageFlag = 0;
	}
	
#else
	if (mCpuClk > SysClkRegister.mCpuClk)
	{
		//CPU 目标频率比当前频率高，先调整电压
		cpuVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	
	memVoltageFlag = 0;	//默认为后调整电压
	if ((busClk>=133000 && SysClkRegister.busClk<133000) ||
		(busClk<133000 && SysClkRegister.busClk>=133000))
	{
		//需要调整 MEM 电压
		if (busClk > SysClkRegister.busClk)
		{
			//MEM 目标频率比当前频率高，先调整电压
			memVoltageFlag = 1;
			act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
		}
	}
	else
	{
		//不需要调整 MEM 电压
		memVoltageFlag = 2;
	}
#endif

	SysClkRegister.mCpuClk = mCpuClk;		//以MHz为单位
	SysClkRegister.fSysClk = fSysClk;		//以MHz为单位
	SysClkRegister.busClk = busClk;			//以MHz为单位
	
/* 调整主频，禁止中断 */
	interrupt = InterruptDisable();
	hyhwNand_setIoSpeed(fSysClk);
	hyhwLcd_setBusTime(fSysClk);

	if(pllOut > EXTAL_CLK)	// multiply the clk	
	{
		if(pllOut >= 200000)
		{
			ECFG0_REGISTER |= ( SDW_SUBCORE_DTCM_WAIT_BIT
							   |IW_MAINCORE_ITCM_WAIT_BIT
		   					   |DW_MAINCORE_DTCM_WAIT_BIT
		   					  );
		}
		
		/* 设置频率之前先把频率设置到12M CLK*/
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((CLK_12M << 0)							/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)					/*MCUP CLK == FCLK; BUS CLK  == FCLK/2*/
				   |(1  <<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  = regBuff;

		rc =hyhwCpm_setPll0(sysclk,cfgExtal);

		if(rc == HY_OK)
		{
			regBuff  = CKC_CLKCTRL_REGISTER;
			regBuff &= ~(MCPUCLK_BIT_DIV_MASK   			/* clear clk divider / bus divider */ 
						|BUSCLK_BIT_DIV_MASK
						|SYSCLK_BIT_SOU_MASK
						);
			regBuff |= ((PLL0_CLK << 0)						/* set cpu clk and bus clk divider,selection*/
					   |(15 <<MCPUCLK_BIT_DIV)
					   |((hdiv)<< BUSCLK_BIT_DIV)
					   );		   
			CKC_CLKCTRL_REGISTER  = regBuff;
		}
	}
	else if(pllOut == EXTAL_CLK) //neither divide nor multiply
	{
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((CLK_12M << 0)							/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)
				   |((hdiv)<<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  = regBuff;
	}
	else	//divide clk
	{
		regBuff  = CKC_CLKDIVC_REGISTER;
		regBuff &= ~(XIN_BIT_DIV_MASK );  					/* clear clk divider / bus divider */ 
		
		
		regBuff |= (XIN_BIT_DIV_ENABLE						/* set cpu clk and bus clk divider,selection*/
				   |(((EXTAL_CLK / pllOut)-1)<<XIN_BIT_DIV)
				   );
		CKC_CLKDIVC_REGISTER  = regBuff;	
		
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((DIV_12M_CLK << 0)						/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)
				   |((hdiv)<<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  |= regBuff;				   
   		CKC_PLL0CFG_REGISTER |= PLL0_BIT_DISABLE;     		/* disable PLL */  
	}
/* 完成调整，恢复中断 */
	InterruptRestore(interrupt);
	
	if (cpuVoltageFlag == 0)
	{
		//CPU 目标频率比当前频率低，后调整电压
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	if (memVoltageFlag == 0)
	{
		//MEM 目标频率比当前频率低，后调整电压
		act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
	}
	
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwSysClk_GetCurValue
* 功能:	得到当前系统的频率
* 参数:	none
* 返回: 系统频率
*----------------------------------------------------------------------------*/
U32 hyhwSysClk_GetCurValue(void)
{
     return SysClkRegister.mCpuClk;
}

/*-----------------------------------------------------------------------------
* 函数:	void hyhwSystemPower_stepInit(void)
* 功能:	对系统电源等级控制初始化(默认电压是1.2V)
* 参数:	none
*----------------------------------------------------------------------------*/
void hyhwSystemPower_stepInit(void)
{
/*未找到对应IO口
	hyhwGpio_setAsGpio(PORT_B,BIT9);
	hyhwGpio_setAsGpio(PORT_C,BIT3);
	hyhwGpio_setOut(PORT_B,BIT9);
	hyhwGpio_setOut(PORT_C,BIT3);
*/
}

/*----------------------------------------------------------------------------
* 函数:	hyhwSubSysClk_Set()
* 功能:	set sub processor's clock
* 参数:	sCpuClk------sub process's clock value 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwSubSysClk_Set(U32 sCpuClk)
{	//先做scpu clk是mcpu
	U32 scupDiv;
	SysClkRegister.sCpuClk 	 = sCpuClk;
	scupDiv = ((SysClkRegister.sCpuClk - SysClkRegister.busClk)<<4)/(SysClkRegister.fSysClk - SysClkRegister.busClk)-1;
	
	CKC_CLKCTRL_REGISTER	&= ~0xf000;
	CKC_CLKCTRL_REGISTER	|= (scupDiv << 12);

}

/*----------------------------------------------------------------------------
* 函数:	tca_ckc_getsystemsource()
* 功能:	获取时钟源
* 参数:	none
* 返回:	返回时钟选择序数，可据此数值查询对应时钟源
*----------------------------------------------------------------------------*/
S32 tca_ckc_getsystemsource(void)
{
	return (CKC_CLKCTRL_REGISTER & 0x7);
}

/*----------------------------------------------------------------------------
* 函数:	tca_ckc_setsystemsource()
* 功能:	设置时钟源
* 参数:	ClockSource--时钟源序号，据此序号可查对应时钟源
* 返回:	none
*----------------------------------------------------------------------------*/
void tca_ckc_setsystemsource( U32 ClockSource )
{
	CKC_CLKCTRL_REGISTER	&= ~0x7;
	CKC_CLKCTRL_REGISTER	|= ClockSource;
}

/*----------------------------------------------------------------------------
* 函数:	tca_ckc_setpll()
* 功能:	设置PLL的通用函数
* 参数:	P--PLL 的P值
*		M--PLL 的M值
*		S--PLL 的S值
*		uCH--PLL序号 0 或者1
* 返回:	none
*----------------------------------------------------------------------------*/
void tca_ckc_setpll(U8 P, U8 M, U8 S, U32 uCH)
{
    volatile U32 *pPLLCFG;
	if(uCH)
	{
		pPLLCFG		 = (&CKC_PLL1CFG_REGISTER);	
	}
	else
	{
		pPLLCFG 	 = (&CKC_PLL0CFG_REGISTER);	
	}

	//Change PLL Clock
	//Disable PLL 
	*pPLLCFG			|= PLL1CFG_PD_DIS;
    //Set P
    *pPLLCFG			&= ~0x3f;
    *pPLLCFG			|= P;
	//Set M
	*pPLLCFG			&= ~0xff00;
    *pPLLCFG			|= (M << 8);
	//Set S
	*pPLLCFG			&= ~0x70000;
    *pPLLCFG			|= (S << 16);
    //Enable PLL
    *pPLLCFG			&= ~PLL1CFG_PD_DIS;
	
	tca_wait();	
}

/*---------------------------------------------------------------------------
* 函数:	hyhwCpm_setPll0
* 功能:	set PLL output frequency 
* 参数:	pllclk	-- PLL 0 输出频率 
*		cfgExtal-- 外部时钟
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll0(U32 pllclk, U32 cfgExtal)
{
	tca_ckc_setpll((pllclk>>24),((pllclk>>16)&0xff),((pllclk>>8)&0xff),0);
	return HY_OK;
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setPll1
* 功能:	set PLL output frequency 
* 参数:	pllclk	-- PLL 1 输出频率 
*		cfgExtal-- 外部时钟
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll1(ePLL_CLK_EN pllclk, U32 cfgExtal)
{
	tca_ckc_setpll(((pllclk>>16)&0xff),((pllclk>>8)&0xff),((pllclk)&0xff),1);
	return HY_OK;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getPllOut0
* 功能:	get PLL output frequency 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getPllOut0(void)
{
	U32 m, p, s, pllout0;
		
	pllout0 = CKC_PLL0CFG_REGISTER;
	m 		= (pllout0&PLL0_BIT_MDIV_MASK)>>PLL0_BIT_MDIV;
	p	 	= (pllout0&PLL0_BIT_PDIV_MASK)>>PLL0_BIT_PDIV;
	s 		= (pllout0&PLL0_BIT_SDIV_MASK)>>PLL0_BIT_SDIV;
	
	pllout0 = ((EXTAL_CLK * m) / (p * (2<<(s))))<<1;

	return pllout0;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getPllOut1
* 功能:	get PLL output frequency 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getPllOut1(void)
{
	U32 m, p, s, pllout1;
		
	pllout1 = CKC_PLL1CFG_REGISTER;
	m 		= (pllout1&PLL0_BIT_MDIV_MASK)>>PLL0_BIT_MDIV;
	p	 	= (pllout1&PLL0_BIT_PDIV_MASK)>>PLL0_BIT_PDIV;
	s 		= (pllout1&PLL0_BIT_SDIV_MASK)>>PLL0_BIT_SDIV;
	
	pllout1 = ((EXTAL_CLK * m) / (p * (2<<(s))))<<1;

	return pllout1;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUSB11HClk
* 功能:	设置USB 1.1 Host 时钟 20100427
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误 
*----------------------------------------------------------------------------*/
void hyhwCpm_setUSB11HClk(void)
{	
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / USB11H_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_USBH);

	regBuff  = CKC_PCK_USB11H_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_USB11H_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_USBH);
	
	return ;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSdmmcClk
* 功能:	设置SD\MMC 时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdmmcClk(void)
{	
	U32 regBuff, divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / SDMMC_CLK)) - 1;			//分频数计算
	hyhwCpm_setSoftReset(CKC_SDMMC);
	
	regBuff  = CKC_PCK_SDMMC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_SDMMC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SDMMC);

}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setMstickClk
* 功能:	设置Memory stick 时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setMstickClk(void)
{	
	U32 regBuff, divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue

	divValue = ((EXTAL_CLK / MSTICK_CLK)) - 1;			//分频数计算
	hyhwCpm_setSoftReset(CKC_MSTICK);
	
	regBuff  = CKC_PCK_MSTICK_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
				
	CKC_PCK_MSTICK_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_MSTICK);

}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setI2cClk
* 功能:	设置I2C时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setI2cClk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / I2C_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_I2C);

	regBuff  = CKC_PCK_I2C_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_I2C_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_I2C);

}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_enableTimer
* 功能:	使能定时器
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_enableTimer(void)
{
	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_disableTimer
* 功能:	关闭定时器
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_disableTimer(void)
{
	hyhwCpm_setSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTimer32Clk
* 功能:	设置定时器频率
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTimer32Clk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / TCZ_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	？？？是否适合现在的系统

	regBuff  = CKC_PCK_TCZ_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_I2C_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);

}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTcxClk
* 功能:	设置Tcx时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTcxClk(void)
{
	U32 regBuff,divValue;		//寄存器值存储临时变量regBuff, 分频值变量divValue
	divValue = ((EXTAL_CLK / TCX_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	？？？是否适合现在的系统

	regBuff  = CKC_PCK_TCX_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_TCX_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_SetTdClk32K
* 功能:	设置Tcx时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_SetTdClk32K(void)
{
     // 设置TCX输出32.768时钟, 用于TD模块
     /* 开始该模块使用32.768晶振, 但该晶振容易损坏, 因此改从cpu直接输出*/
     // added by yanglijing, 20090909
	U32 regBuff,divValue;		//寄存器值存储临时变量regBuff, 分频值变量divValue

	hyhwGpio_setTcxClkOut0Function();
	regBuff  = CKC_PCK_TCX_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_32k << 24			
				|0					
				);
	
	CKC_PCK_TCX_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setTctClk
* 功能:	设置Tct时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTctClk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / TCT_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	？？？是否适合现在的系统

	regBuff  = CKC_PCK_TCT_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_TCT_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setLcdClk
* 功能:	设置LCD时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setLcdClk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / LCD_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_LCD);

	regBuff  = CKC_PCK_LCD_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_LCD_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_LCD);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setCifClk
* 功能:	设置CIF PXclock时钟 //camera pixel clock
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setCifClk(U32 cifClk)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	divValue = ((pllOut / cifClk)) - 1;
	hyhwCpm_setSoftReset(CKC_CAM);

	regBuff  = CKC_PCK_CAM_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_CAM_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_CAM);
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setScalerClk
* 功能:	设置ScalerClk 时钟 //camera pixel clock
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setScalerClk(U32 scalerClk)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	divValue = ((pllOut / scalerClk)) - 1;
	//hyhwCpm_setSoftReset(CKC_MTM);

	regBuff  = CKC_PCK_SCALER_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_SCALER_REGISTER  = regBuff;
	//hyhwCpm_clearSoftReset(CKC_MTM);
	
}

U32 hyhwCpm_setUartRegister(U32 UartId, U32 divValue)
{
	volatile U32 *pUart_CkcPckRegister;
	U32 regBuff;
	
	if (UartId > 3) return HY_ERR_BAD_PARAMETER;
	
	pUart_CkcPckRegister = (volatile U32 *)(TCC7901_CKC_BASEADDR + 0x98 + UartId*4);
	regBuff  = *pUart_CkcPckRegister;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24
				|divValue
				);
	
	*pUart_CkcPckRegister  = regBuff;
	
	return HY_OK;
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUartClk
* 功能:	set uart clk 
* 参数:	uartclk -- uart设备需要的时钟 单位：MHZ 
*				UnitId  -- 是设备号，spi 为(0 \ 1 \ 2 \ 3)
* NOTE: 设置 UART CLK 如果系统时钟不是整数倍 那么uart clk 设置失败。。请改变系统时钟 
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setUartClk(U32 UartId, ePERI_CLK uartClk)
{
	U32 regBuff,pllclk,divValue;
	UInt32 rc = HY_OK;
	
	//if((UartId > 3)||(uartClk > UART_CLK_MAX))
	if (UartId > 3)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		pllclk = hyhwCpm_getPllOut1();
		if((pllclk % (uartClk)) == 0)
		{
			divValue = ((pllclk / uartClk)) - 1;
			
			switch (UartId)
			{
				case 0:
					hyhwCpm_setSoftReset(CKC_UART0);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART0);
					break;
				case 1:
					hyhwCpm_setSoftReset(CKC_UART1);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART1);
					break;
				case 2:
					hyhwCpm_setSoftReset(CKC_UART2);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART2);
					break;
				case 3:
					hyhwCpm_setSoftReset(CKC_UART3);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART3);
					break;
				default:
					break;
			}
		}
	}
	
	return rc;
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart0Clk
* 功能:	设置UART0时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart0Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART0_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART0);

	regBuff  = CKC_PCK_UART0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART0);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart1Clk
* 功能:	设置UART1时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart1Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART1_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART1);

	regBuff  = CKC_PCK_UART1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART1);
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart2Clk
* 功能:	设置UART2时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart2Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART2_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART2);

	regBuff  = CKC_PCK_UART2_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART2_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART2);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setUart3Clk
* 功能:	设置UART3时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart3Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART3_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART3);

	regBuff  = CKC_PCK_UART3_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART3_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART3);
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setDaiClk
* 功能:	设置DAI Controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setMmcClk(ePERI_CLK clk)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	pllOut = hyhwCpm_getPllOut1();
	divValue = ((pllOut / clk)) - 1;
	hyhwCpm_setSoftReset(CKC_SDMMC);

	regBuff  = CKC_PCK_SDMMC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_SDMMC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SDMMC);
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setDaiClk
* 功能:	设置DAI Controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setDaiClk(U32 daiClk)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	pllOut = hyhwCpm_getPllOut1()*1000;
	divValue = ((pllOut / daiClk)) - 1;
	hyhwCpm_setSoftReset(CKC_DAI);

	regBuff  = CKC_PCK_DAI_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (DAI_DIV_SELECT
				|MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_DAI_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_DAI);
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setDaiDcoClk
* 功能:	设置DAI clk dco 模式，频率会准确一些
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setDaiDcoClk(U32 daiClk)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	U32 pllclk;
	U64 clk;
	
	
	
	pllOut = hyhwCpm_getPllOut1()*1000;
	clk = daiClk;
	clk = (clk *65536)/pllOut;
	
	divValue = 65536 - clk;
	hyhwCpm_setSoftReset(CKC_DAI);

	regBuff  = CKC_PCK_DAI_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_DAI_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_DAI);
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb0Clk
* 功能:	设置GPSB0时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb0Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB0_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB0);

	regBuff  = CKC_PCK_GPSB0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB0);
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb1Clk
* 功能:	设置GPSB1时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb1Clk(void)
{
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	divValue = ((pllOut / (GPSB1_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB1);
	
	
	regBuff  = CKC_PCK_GPSB1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB1);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb2Clk
* 功能:	设置GPSB2时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb2Clk(void)
{

	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB2_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB2);

	regBuff  = CKC_PCK_GPSB2_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB2_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB2);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setGpsb3Clk
* 功能:	设置GPSB3时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb3Clk(void)
{
	
	U32 regBuff,divValue,pllOut;						//寄存器值存储临时变量regBuff, 分频值变量divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB3_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB3);

	regBuff  = CKC_PCK_GPSB3_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB3_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB3);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setAdcClk
* 功能:	设置Adc时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setAdcClk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue

	divValue = ((EXTAL_CLK / ADC_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_ADC);

	regBuff  = CKC_PCK_ADC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_ADC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_ADC);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSpdifClk
* 功能:	设置 S/PDIF Tx Controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSpdifClk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / SPDIF_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_SPDIF);

	regBuff  = CKC_PCK_SPDIF_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_SPDIF_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SPDIF);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setEhi0Clk
* 功能:	设置 EHI 0 controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi0Clk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	divValue = ((EXTAL_CLK / EHI0_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EHI0);

	regBuff  = CKC_PCK_EHI0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_EHI0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EHI0);
	
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setEhi1Clk
* 功能:	设置 EHI 1 controller时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi1Clk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	divValue = ((EXTAL_CLK / EHI1_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EHI1);

	regBuff  = CKC_PCK_EHI1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_EHI1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EHI1);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSdramRefreshClk
* 功能:	设置 SDRAM 刷新时钟
* 参数:	none
* 返回:	HY_OK					-- 成功
*		HY_ERR_BAD_PARAMETER	-- 参数错误
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdramRefreshClk(void)
{
	U32 regBuff,divValue;								//寄存器值存储临时变量regBuff, 分频值变量divValue
	
	divValue = ((EXTAL_CLK / RFREQ_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EMC);

	regBuff  = CKC_PCK_EHI0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//时钟使能位清零
				|MODEL_BIT_SOURCE_MASK					//时钟源选择位
				|MODEL_BIT_DIV_MASK						//分频或倍频数位
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M		//|( DIV_PLL0_DIV0_CLK<< 24	)	// 			
				|divValue					
				);
	
	CKC_PCK_RFREQ_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EMC);
	
}


/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_busClkEnable
* 功能:	打开外围设备时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkEnable(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_BCLKCTR1_REGISTER |= (1 << UnitId);
	}
	else
	{
		CKC_BCLKCTR0_REGISTER |= (1 << UnitId);
	} 	
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_busClkDisable
* 功能:	关闭外围设备时钟
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkDisable(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_BCLKCTR1_REGISTER &= ~(1 << UnitId);
	}
	else
	{
		CKC_BCLKCTR0_REGISTER &= ~(1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_setSoftReset
* 功能:	软件复位外围设备
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setSoftReset(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_SWRESET1_REGISTER |= (1 << UnitId);
	}
	else
	{
		CKC_SWRESET0_REGISTER |= (1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_clearSoftReset
* 功能:	清除外围设备复位
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_clearSoftReset(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_SWRESET1_REGISTER &= ~(1 << UnitId);
	}
	else
	{
		CKC_SWRESET0_REGISTER &= ~(1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getSysClkSourceo
* 功能:	get sysClk Sourceo  
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

U32 hyhwCpm_getSysClkSourceo(void)
{

	return(CKC_CLKCTRL_REGISTER & SYSCLK_BIT_SOU_MASK);
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getClk
* 功能:	get clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/

U32 hyhwCpm_getClk(U32 div,U32 clkSour)
{


	if(clkSour == PLL0_CLK)							  		/*  选择 PLL0 */
	{
		if((CKC_PLL0CFG_REGISTER & PLL0_BIT_DISABLE)==0) 	/* 判断 PLL0 是否ENABLE */
		{
			return hyhwCpm_getPllOut0() / div;
		}
		else 
		{
			return 0;	          						 	/*  note : 选择PLL输出但是没有enable 那PLLOUT 是多少？？ */
		}
	}
	else if(clkSour == PLL1_CLK)							/*  选择 PLL1  */
	{
		if((CKC_PLL1CFG_REGISTER & PLL1_BIT_DISABLE)==0)
		{
			return hyhwCpm_getPllOut1() / div;
		}
		else 
		{
			return 0;										/*  note : 选择PLL输出但是没有enable 那PLLOUT 是多少？？ */
		}
	}
	else if(clkSour == CLK_12M)								/*  选择 12M  */
	{
		return EXTAL_CLK / div;
	}
	else if(clkSour == CLK_32k)								/*  选择 32.768K  */
	{
		return 32768 / div;
	}
	else
	{
		return HY_ERR_BAD_PARAMETER;
	}
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getBusclk
* 功能:	get bus  clock
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getBusClk(void)
{
	U32 ratio,clkSour;
	
	ratio    = hyhwCpm_getBusClkDiv() + 1;
	clkSour = hyhwCpm_getSysClkSourceo();
	return (hyhwCpm_getClk(ratio,clkSour));

}


/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getCpuClk
* 功能:	get CPU core clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getCpuClk(void)
{
	U32 ratio,busClk,pllClk,cpuClk;
	
	pllClk = hyhwCpm_getPllOut0();
	busClk = hyhwCpm_getBusClk();
	ratio  = hyhwCpm_getCpuClkDiv() + 1;
	cpuClk = busClk + ((pllClk - busClk) * ratio) / 16;
	return cpuClk;
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getTimerClk
* 功能:	get timer clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getTimerClk(void)
{
	U32 div,clkSour;
	
	clkSour = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	div   = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_DIV_MASK) + 1;
	
	return (hyhwCpm_getClk(div,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getTimer32Clk
* 功能:	get timer clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getTimer32Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_TCZ_REGISTER & MODEL_BIT_DIV_MASK) ) + 1;
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getI2cClk
* 功能:	get i2c clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getI2cClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_I2C_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_I2C_REGISTER & MODEL_BIT_DIV_MASK) ) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getLcdClk
* 功能:	get lcd clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getLcdClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_LCD_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_LCD_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getAdcClk
* 功能:	get adc clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getAdcClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_ADC_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_ADC_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getSpi0Clk
* 功能:	get SPI0 clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getSpiClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_SDMMC_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_SDMMC_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getUart0Clk
* 功能:	get UART0 clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart0Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART0_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART0_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getUart1Clk
* 功能:	get UART1 clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart1Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART1_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART1_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getUart2Clk
* 功能:	get UART2 clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart2Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART2_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART2_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getUart2Clk
* 功能:	get UART3 clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart3Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART3_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART3_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getDaiClk
* 功能:	get DAI clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getDaiClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_DAI_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_DAI_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getUsbClk
* 功能:	get USB clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUsbClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_USB11H_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_USB11H_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* 函数:	hyhwCpm_getMmcClk
* 功能:	get MMC clock 
* 参数:	none 
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getMmcClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (SDMMC_CLK & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((SDMMC_CLK & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}


U32 hyhwCpm_Get_LcdClk()
{
	return LCD_CLK;
}


/* end of file */

