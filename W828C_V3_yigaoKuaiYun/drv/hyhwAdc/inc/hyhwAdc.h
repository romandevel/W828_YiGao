#ifndef TM_HW_API_ADC_H_
#define TM_HW_API_ADC_H_

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#ifdef __cplusplus
	/*  Assume C declarations for C++ */
	extern "C" {
#endif

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
Types and defines:
------------------------------------------------------------------------------*/


#define	CONTROL0_REGISTER				TCC7901_ADC_ADCCON_REGISTER					/* ADC Control Register */
	#define	CON_BIT_STANDBY_MODEL		BIT4
	#define CON_BIT_IN_CH0				0x0
	#define CON_BIT_IN_CH1				0x1
	#define CON_BIT_IN_CH2				0x2
	#define CON_BIT_IN_CH3				0x3
	#define CON_BIT_IN_CH4				0x4
	#define CON_BIT_IN_CH5				0x5
	#define CON_BIT_IN_CH6				0x6
	#define CON_BIT_IN_CH7				0x7
	
	
#define	ADC_DATA_REGISTER				TCC7901_ADC_ADCDATA_REGISTER				/* ADC Data Register */
	#define DATA_BIT_STATUS_FLAG		BIT0
	
#define	CONTROL1_REGISTER				TCC7901_ADC_ADCCONA_REGISTER				/* ADC Control Register */
#define	STATUS_REGISTER					TCC7901_ADC_ADCSTATUS_REGISTER				/* ADC Status Register */
#define	CONFIG_REGISTER					TCC7901_ADC_ADCCFG_REGISTER					/* ADC Configuration Register */
	#define	CFGR_BIT_NEOC				~BIT7
	#define	CFGR_BIT_IRQE_EN			BIT3	
	#define	CFGR_BIT_R8_8BIT			BIT2
	#define	CFGR_BIT_APD_EN				BIT1
	#define	CFGR_BIT_SM_EN				BIT0	

typedef struct _tADC_CTRL
{
    unsigned char  converFlag; //状态0 ：开始转换 1：转换完成
    unsigned char  curChl; //当前通道
    unsigned int   lastTick; //上次的tick
    unsigned int   cvtTick;//开始转换的tick数
    
}tADC_CTRL;


//ADC 通道
#define ADCNUM		8			

#define ADC_CH7		7
#define ADC_CH6		6
#define ADC_CH5		5
#define ADC_CH4		4
#define ADC_CH3		3
#define ADC_CH2		2
#define ADC_CH1		1
#define ADC_CH0		0

/*------------------------------------------------------------------------------
Exported Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported Function protoypes:
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_Init
* 功能:	初始化A/D部分的寄存器和static变量
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_Init(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_PowerIdle();
* 功能:	set adc power idle
* 参数:	none
* 返回: none
*			
*----------------------------------------------------------------------------*/
void hyhwAdc_PowerIdle(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_start
* 功能:	启动AD转换
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_start(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwAdc_Read
* 功能:	读取指定通道的ADC，每次只能读取一个通道的值，而且是已经转换好的值
*       对于没有转换或者已经读过的channel，AD值为ADC_ERR_VALUE
* 参数:	channel--指定的通道(ADC_CH0 ~ ADC_CH3)
* 返回:	ADC值
*----------------------------------------------------------------------------*/
unsigned short hyhwAdc_Read(unsigned char channel);

#ifdef __cplusplus
}
#endif

#endif /* TM_HW_API_ADC_H_ */
