#include "hyTypes.h"
#include "hyhwTimer.h"
#include "hyhwGpio.h"
#include "hyErrors.h"
#include "HycDeviceConfig.h"
#include "hardWare_Resource.h"

#ifdef ACT6311
	#define PWM_TIMERID			TIMER0
	#define PWM_PORT			LCD_PWM_PORT
	#define PWM_BIT				LCD_PWM_GPIO
#endif  // ACT6311
#ifdef CP2142MM
	#define PWM_COUNTER_ID 		TIMER1
	#define PWM_TIMERID			TIMER0
	#define PWM_PORT			LCD_PWM_PORT
	#define PWM_BIT				LCD_PWM_GPIO
#endif


/*-----------------------------------------------------------------------------
* 函数:	void hyhwLcd_lightSet(U8 grade)
* 功能:	test pwm function
* 参数:	grade--输入背光强度设置从(0 到 31) grade =0 最大电流是20mA,
*										   grade =8	电流是15mA
*										   grade =16电流是10mA
*										   grade =24电流是5mA
*										   grade =31背光关闭
*----------------------------------------------------------------------------*/
U8 gu8LastGrade = 0xFF;
#ifdef ACT6311
void hyhwLcd_lightSet(U8 grade)
{
	if(grade >68 || grade < 40) grade = 40 ;
	
	if(grade == gu8LastGrade) return ;
	gu8LastGrade = grade;
	
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setPwmFunction();
	hyhwTimer_setPwm(PWM_TIMERID,1000,grade); 
}

void hyhwLcd_lightOff()
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
}

void hyhwLcd_lightReset(void)
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
	gu8LastGrade = 0xFF; 
}
#endif
#ifdef CP2142MM
static void hyhwLcd_isr(void)
{
/* 将PWM设置完成以后应当将该信号设置高电平保持点亮背光, 设置生效 */
	hyhwTimer_pwmDisable(PWM_TIMERID);
	
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setHigh(PWM_PORT, PWM_BIT);
	
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intDisable(PWM_COUNTER_ID);
}
void hyhwLcd_PWMReset(void)
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
	gu8LastGrade = 0xFF; 
}
void hyhwLcd_lightSet(U8 grade)
{
	U32 timePeriod;
	U8  tempGrade;
	
	if(grade>16)
		grade = 1;
	
	if (0 == grade) return;
		
	if (gu8LastGrade == 0xFF)
	{
		tempGrade = grade;
	}
	else
	{
		if (grade > gu8LastGrade)
		{
			tempGrade = grade-gu8LastGrade;
		}
		else if (grade < gu8LastGrade)
		{
			tempGrade = 16-gu8LastGrade+grade;
		}
		else
		{
			return;
		}
	}
	gu8LastGrade = grade;
	
	timePeriod = 1000000/(50*tempGrade);

	hyhwTimer_pwmDisable(PWM_TIMERID);
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setAsPwmFunction(PWM_PORT, PWM_BIT);
	
	hyhwInt_ConfigTimer1_ISR(hyhwLcd_isr);
	hyhwTimer_SetFrequency(PWM_COUNTER_ID, timePeriod);
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intEnable(PWM_COUNTER_ID);
	
	hyhwTimer_setConter(PWM_TIMERID, 0);
	hyhwTimer_setPwm(PWM_TIMERID, 20000, 70);
	
	hyhwTimer_setConter(PWM_COUNTER_ID, 0);
	hyhwTimer_countEnable(PWM_COUNTER_ID);
}
void hyhwLcd_lightSet_test(U8 cnt)
{
	U32 timePeriod;
	U8  tempGrade;
	
	//timePeriod = 1000000/(50*cnt);

	hyhwTimer_pwmDisable(PWM_TIMERID);
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setPwmFunction();
	
	hyhwInt_ConfigTimer1_ISR(hyhwLcd_isr);
	hyhwTimer_SetFrequency(PWM_COUNTER_ID, timePeriod);
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intEnable(PWM_COUNTER_ID);
	
	hyhwTimer_setPwm(PWM_TIMERID, 20000, 50);
	hyhwTimer_countEnable(PWM_COUNTER_ID);
}
#endif

// tg3项目中, 以下函数用不到 .. // 20090902 yanglijing
/* GPIO_F BIT14该信号设置可能会影响其他模块，暂时将其注释掉, tg3中没有闪光灯控制
   将这些函数设为空函数 */
/*-----------------------------------------------------------------------------
* 函数:	hyhwLcd_flashSet
* 功能:	设置闪光灯亮度
* 参数: none
* 返回:	none
*设置参数：0-16
*	RFSET(Ω)  IFLED=2880/RFSET  RFSET = 14.3K(IFLED =206mA Max)
*	
*	FENS Data	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	
*		
*	IFLX(%)	 	100	94	87	81	73	67	60	53	46	40	34	27	20	13	6	3
*----------------------------------------------------------------------------*/
#define FENS_Data_MAX 	16

U32 hyhwLcd_flashSet(U8 FENS_Data)
{
	U32 i,j;
	U8 intLev;

	if(FENS_Data > FENS_Data_MAX)
		return HY_ERR_BAD_PARAMETER;
	
	FENS_Data = (FENS_Data_MAX+1) - FENS_Data;
	
#if 0
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	
	intLev = InterruptDisable();
	for(i=0;i<FENS_Data;i++)
	{
		hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
		//delay 1us
		for(j=0;j<=20;j++);
		hyhwGpio_setLow(LED_FLASH_PORT,LED_FLASH_GPIO);
		//delay 1us
		for(j=0;j<=20;j++);
	}
	InterruptRestore(intLev);
#else
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
#endif
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwLcd_flashEnable
* 功能:	使能闪光灯
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_flashEnable(void)
{
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
	
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwLcd_flashDisable
* 功能:	关闭闪光灯  Flash闪光灯的关闭方式，VDD_LEDFLASH_EN，即GPIOA[7] ，
        输出为低电平是打开闪光灯，输出为高电平时不能完全关闭闪光灯，
        因此在对闪光灯关闭控制时需要将此I/O口设置为输入高阻态才行。
* 参数: none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_flashDisable(void)
{
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setLow(LED_FLASH_PORT,LED_FLASH_GPIO);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwLcd_flashStatus
* 功能:	闪光灯状态
* 参数:	none
* 返回:	0----关闭，1----开启
*----------------------------------------------------------------------------*/
int hyhwLcd_flashStatus(void)
{
	//return !hyhwGpio_Read(LED_FLASH_PORT,LED_FLASH_BIT);
}

void flash_test()
{
	static enable = 0  ;
	
	if(0 == enable){enable = 1 ; hyhwLcd_flashDisable();}
	else{enable = 0 ; hyhwLcd_flashEnable();}
	
}
