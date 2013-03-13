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

/**
 * \addtogroup hyhwAdc
 * @{
 */

/** 
 * @file hyhwAdc.c
 * @brief The source code for the ADC (hardware layer).
 *
 * The source code for the ADC (hardware layer).
 * 
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/
#include "hyhwCkc.h"
#include "hyhwAdc.h"
#include "hyhwGpio.h"


/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/

static volatile U16 sgADC_Value[8];			//用于保存各个通道的ADC值
static tADC_CTRL gtAdcCtrl;

#define ADC_COMPULSORYREAD_LIMIT		100000
#define ADC_ERR_VALUE				    0x3ff//错误的ADC值
/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Static prototypes:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_HwInit
* 功能:	初始化A/D部分的寄存器
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_HwInit(void)
{
	/* 打开ADC模块并设置ADC clk =2.0M */
	hyhwCpm_setAdcClk();
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	CONFIG_REGISTER   |= (0x00000f00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_Init
* 功能:	初始化A/D部分的寄存器和static变量
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_Init(void)
{
	U8 i;

	for(i=0;i<8;i++)
	{
		sgADC_Value[i] = ADC_ERR_VALUE; 
	}
	/* 打开ADC模块并设置ADC clk =2.0M */
	hyhwCpm_setAdcClk();
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	CONFIG_REGISTER   |= (0x00000f00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	
	memset(&gtAdcCtrl,0,sizeof(tADC_CTRL));
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_PowerIdle();
* 功能:	set adc power idle
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
void hyhwAdc_PowerIdle(void)
{
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	//关闭ADC时钟
	CKC_PCK_ADC_REGISTER &= (~MODEL_BIT_ENABLE);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_start
* 功能:	启动AD转换
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_start(void)
{
	U32 rc = HY_OK;
	U32 i,j,reg;
	U32 stausReg;
	U32 curTick;
	S32 index;
	//static U8 tempCnt = 0;
	
	if (0 == gtAdcCtrl.curChl && 0 == gtAdcCtrl.converFlag)
	{
	    CONFIG_REGISTER   &= ~(CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	}
	
	curTick = rawtime(NULL);
	
	if (gtAdcCtrl.lastTick != curTick && gtAdcCtrl.converFlag == 0)
	{		 
	    CONTROL0_REGISTER = gtAdcCtrl.curChl;
    	gtAdcCtrl.converFlag = 1;
    	gtAdcCtrl.cvtTick = curTick;
    	return;
	}
	
    if (!(ADC_DATA_REGISTER & DATA_BIT_STATUS_FLAG)) 
    {
        S32 diff;
        
        diff = (curTick>=gtAdcCtrl.cvtTick) ? (curTick-gtAdcCtrl.cvtTick):(gtAdcCtrl.cvtTick-curTick);
        
        if (ABS(diff) > 20)
        {
        	 hyhwAdc_Init();
        }
                
        return;    
	}
	
	stausReg = STATUS_REGISTER;
	
	index = (stausReg>>16) & 0x0f; 
	
	if (index < ADCNUM)
	{
	    sgADC_Value[index] = stausReg&0x3ff;
	    
	    //if (2 == index && tempCnt == 0) 
	    //PhoneTrace(0,"chl:%x",sgADC_Value[index]);
	    
	    //tempCnt++;
	    //if (tempCnt > 100) tempCnt = 0;
	}
	
	gtAdcCtrl.lastTick = rawtime(NULL);	

    gtAdcCtrl.curChl = 0;//(gtAdcCtrl.curChl==0) ? 7:0;

	gtAdcCtrl.converFlag = 0;
	
	//PhoneTrace(0,"---%x,  %x",sgADC_Value[0],sgADC_Value[7]);

	//return rc;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_Read
* 功能:	读取指定通道的ADC，每次只能读取一个通道的值，而且是已经转换好的值
*       对于没有转换或者已经读过的channel，AD值为ADC_ERR_VALUE
* 参数:	channel--指定的通道(ADC_CH0 ~ ADC_CH3)
* 返回:	ADC值
*----------------------------------------------------------------------------*/
U16 hyhwAdc_Read(U8 channel)
{
	//参数检查		
	if(channel > ADCNUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
//	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
//	CONFIG_REGISTER   |= (0x00000F00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	//sgADC_Value[channel] = ADC_ERR_VALUE; 
	
	return (sgADC_Value[channel] & 0x3ff);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_SetDefault
* 功能:	把指定的通道设置成默认的值
* 参数:	channel--指定的通道(ADC_CH0 ~ ADC_CH3)
* 返回:	ADC值
*----------------------------------------------------------------------------*/
void hyhwAdc_SetDefault(U8 channel)
{
     sgADC_Value[channel] = 0x3ff;
}



