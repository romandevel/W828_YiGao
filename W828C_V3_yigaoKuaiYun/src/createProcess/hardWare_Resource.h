/*
此文件统一定义硬件资源,后续资源改动时只需修改该文件中的宏定义即可;
*/
#include "hyhwGpio.h"
#include "hyhwIIC.h"

/*===============================================================================*/


//振动马达功能接口      PWM  I/O口模拟
#define		VIBRATOR_CTRL_PORT			(PORT_A)
#define		VIBRATOR_CTRL_GPIO			(GPIO_6)

/*------ 电源管理功能接口 ------*/
//EINT（监测主电池的电量，设置为ADC功能）
//#define 	BAT_ADCIN_PORT				PORT_E
//#define		BAT_ADCIN_GPIO				GPIO_24

//DCIN 插入检测
#define 	DCIN_DET_PORT				PORT_E
#define		DCIN_DET_GPIO				GPIO_25//828

//PMU
#define PMU_I2C_CHANNEL					I2C_CH1

#define 	CHG_IRQ_PORT				PORT_F
#define		CHG_IRQ_GPIO				GPIO_26//828, 低电平表示PMU有中断

#define 	CHG_ACTIVE_PORT				PORT_E
#define		CHG_ACTIVE_GPIO				GPIO_31//828, 低电平表示在充电

//使能TCC7901最小系统之外的内部功能模块、控制寄存器的供电，高电平有效
//#define 	PWR_SYS_ON_PORT				PORT_D
//#define		PWR_SYS_ON_GPIO				GPIO_0//828没有

//USB-HOST对外供电使能，高电平有效
//#define 	HOST_POWER_EN_PORT			PORT_C
//#define		HOST_POWER_EN_GPIO			GPIO_19//828直接从PMU输出

//系统总电源开关控制口，运行后设置为输出高
#define 	PWR_HOLD_PORT				PORT_D
#define		PWR_HOLD_GPIO				GPIO_11//828

//PWR_STATUS     KEY_PWR_ON   高  关机键按下
//#define 	PWR_STATUS_PORT				PORT_A
//#define		PWR_STATUS_GPIO				GPIO_6

//摄像头
//#define 	CMOS_POWER_CTRL_PORT		PORT_F
//#define 	CMOS_POWER_CTRL_GPIO		GPIO_26//828直接从PMU输出

//闪光灯
#define		LED_FLASH_PORT				PORT_E
#define		LED_FLASH_GPIO				GPIO_26//W828 V3

//扫描
//#define 	POWER_SCANNER_CTRL_PORT		PORT_D
//#define 	POWER_SCANNER_CTRL_GPIO		GPIO_1//828直接从PMU输出

#define		SCANNER_UART_RX_PORT		PORT_E
#define		SCANNER_UART_RX_GPIO		GPIO_10

#define		SCANNER_UART_TX_PORT		PORT_E
#define		SCANNER_UART_TX_GPIO		GPIO_11

#define		SCANNER_MCU_IRQ_PORT		PORT_F
#define		SCANNER_MCU_IRQ_GPIO		GPIO_24

#define		SCANNER_SUCCESS_PORT		PORT_F
#define		SCANNER_SUCCESS_GPIO		GPIO_3

#define		SCANNER_MCU_RESET_PORT		PORT_F
#define		SCANNER_MCU_RESET_GPIO		GPIO_27

#define		SCANNER_MCU_BOOT_CTRL_PORT	PORT_F
#define		SCANNER_MCU_BOOT_CTRL_GPIO	GPIO_25

#define		SCANNER_MCU_WAKEUP_PORT		PORT_F
#define		SCANNER_MCU_WAKEUP_GPIO		GPIO_4

#define		SCANNER_TRIG_PORT			PORT_F
#define		SCANNER_TRIG_GPIO			GPIO_1

//讯宝 symbol
#define		STM102_UART					UART3
#define		STM102_UART_PORT			UART_PORT_3
#define		SCANNER_UART				(UART3)
#define		SCANNER_UART_PORT			(UART_PORT_3)

#define		SCANNER_UART_CTS_PORT		SCANNER_MCU_IRQ_PORT
#define		SCANNER_UART_CTS_GPIO		SCANNER_MCU_IRQ_GPIO

#define		SCANNER_UART_RTS_PORT		SCANNER_SUCCESS_PORT
#define		SCANNER_UART_RTS_GPIO		SCANNER_SUCCESS_GPIO

#define		SCANNER_PWRDWN_PORT			SCANNER_MCU_RESET_PORT
#define		SCANNER_PWRDWN_GPIO			SCANNER_MCU_RESET_GPIO

#define		SCANNER_BPR_PORT			SCANNER_MCU_BOOT_CTRL_PORT
#define		SCANNER_BPR_GPIO			SCANNER_MCU_BOOT_CTRL_GPIO

#define		SCANNER_AIM_WAKE_PORT		SCANNER_MCU_WAKEUP_PORT
#define		SCANNER_AIM_WAKE_GPIO		SCANNER_MCU_WAKEUP_GPIO

//欧光 opticon
#define		SCANNER_FRAME_PORT			SCANNER_SUCCESS_PORT
#define		SCANNER_FRAME_LINE			SCANNER_SUCCESS_GPIO		/* 帧信号，input */
#define		SCANNER_DATA_PORT			SCANNER_MCU_IRQ_PORT
#define		SCANNER_DATA_LINE			SCANNER_MCU_IRQ_GPIO		/* 数据信号，input */
#define		SCANNER_LASER_CTRL_PORT		SCANNER_UART_RX_PORT
#define		SCANNER_LASER_CTRL_LINE		SCANNER_UART_RX_GPIO		/* 激光控制，output */
#define		SCANNER_PENDULUM_CTRL_PORT	SCANNER_UART_TX_PORT
#define		SCANNER_PENDULUM_CTRL_LINE	SCANNER_UART_TX_GPIO		/* 摆动片控制，output */

//gps
#define		GPS_UART					UART2
#define		GPS_UART_PORT 				UART_PORT_2

#define 	POWER_GPS_CTRL_PORT			PORT_D
#define 	POWER_GPS_CTRL_GPIO			GPIO_2//828

#define		GPS_RESET_PORT				PORT_F
#define		GPS_RESET_GPIO				GPIO_0

#define		GPS_UART2_RXD_PORT  		PORT_E
#define		GPS_UART2_RXD_GPIO			GPIO_8

#define		GPS_UART2_TXD_PORT  		PORT_E
#define		GPS_UART2_TXD_GPIO			GPIO_9

/*----------- DISPLAY功能接口 -------------*/
//AP_LD_D0~AP_LD_D15   C0~C15
//LCD功能模块供电使能，高电平有效
#define 	PWR_LCD_ON_PORT				PORT_F
#define		PWR_LCD_ON_GPIO				GPIO_6//828

#define 	LCD_PWM_PORT				PORT_A
#define		LCD_PWM_GPIO				GPIO_7//828(GPIOA[7]PWM占用TIMER0)

//EINT(触控IC输出的中断信号，低电平有效)
#define 	AP_TP_PEND_DET_PORT			PORT_C
#define		AP_TP_PEND_DET_GPIO			GPIO_30//828

#define 	AP_LD_NRST_PORT				PORT_C
#define		AP_LD_NRST_GPIO				GPIO_29//828
	

/*--------- USB功能接口（资源同W900B/W818） -------------*/

/*--------- JTAG功能接口（资源同W900B/W818）-------------*/


/*--------- GSM功能接口（资源同W900B/W818） -------------*/
#define		GSM_UART					UART0   // 支持流控
#define		GSM_UART_PORT 				UART_PORT_0

#define 	GSM_SWITCH_ON_PORT			PORT_F
#define		GSM_SWITCH_ON_GPIO			GPIO_16//828

#define 	GSM_BB_RESET_PORT			PORT_F
#define		GSM_BB_RESET_GPIO			GPIO_8//828

#define 	GSM_UART0_DSR_PORT			PORT_F
#define		GSM_UART0_DSR_GPIO			GPIO_17//828

//GSM开机检测IO
#define 	GSM_DET_PORT				PORT_F
#define		GSM_DET_GPIO				GPIO_9//828

#define 	GSM_UART0_RI_PORT			PORT_F
#define		GSM_UART0_RI_GPIO			GPIO_11//828

#define 	GSM_UART0_RXD_PORT			PORT_E
#define		GSM_UART0_RXD_GPIO			GPIO_0//828

#define 	GSM_UART0_TXD_PORT			PORT_E
#define		GSM_UART0_TXD_GPIO			GPIO_1//828

#define 	GSM_UART0_RTS_PORT			PORT_E
#define		GSM_UART0_RTS_GPIO			GPIO_2//828

#define 	GSM_UART0_CTS_PORT			PORT_E
#define		GSM_UART0_CTS_GPIO			GPIO_3//828

#define 	GSM_POWER_ON_PORT			PORT_E
#define		GSM_POWER_ON_GPIO			GPIO_5//828

/*---------------- BT&WIFI功能接口（资源同W900B/W818）---------------*/
#define		BT_UART						UART1
#define		BT_UART_PORT 				UART_PORT_5

#define 	BT_UART_TX_PORT				PORT_E
#define		BT_UART_TX_GPIO				GPIO_7

#define 	BT_UART_RX_PORT				PORT_E
#define		BT_UART_RX_GPIO				GPIO_6

//复位BT功能模块，低电平有效，默认输出上拉
#define 	BT_RESET_PORT				PORT_F
#define		BT_RESET_GPIO				GPIO_5

#define 	BT_32K_CLK_PORT				PORT_A	//20121114
#define		BT_32K_CLK_GPIO				GPIO_3

//使能BT功能模块，高电平有效
//#define 	BT_EN_PORT					PORT_F
//#define		BT_EN_GPIO					GPIO_6//828上没有

//使能模组内部寄存器的供电，高电平有效，默认输入高阻
#define 	WL_VREG_EN_PORT				PORT_F
#define		WL_VREG_EN_GPIO				GPIO_7

//使能整个模块电源，高电平有效，默认输入高阻
#define 	PWR_WIFI_BT_ON_PORT			PORT_F
#define		PWR_WIFI_BT_ON_GPIO			GPIO_10

#define  	WIFI_RESET_PORT         	PORT_F
#define  	WIFI_RESET_GPIO          	GPIO_12

#define  	WIFI_SD_DAT3_PORT       	PORT_F
#define  	WIFI_SD_DAT3_GPIO        	GPIO_18

#define  	WIFI_SD_DAT2_PORT       	PORT_F
#define  	WIFI_SD_DAT2_GPIO        	GPIO_19

#define  	WIFI_SD_DAT1_PORT       	PORT_F
#define  	WIFI_SD_DAT1_GPIO        	GPIO_20

#define  	WIFI_SD_DAT0_PORT       	PORT_F
#define  	WIFI_SD_DAT0_GPIO        	GPIO_21

#define  	WIFI_SD_CMD_PORT        	PORT_F
#define  	WIFI_SD_CMD_GPIO         	GPIO_22

#define  	WIFI_SD_CLK_PORT        	PORT_F
#define  	WIFI_SD_CLK_GPIO         	GPIO_23

//#define  	WIFI_HOST_WAKEUP_PORT   	PORT_C
//#define  	WIFI_HOST_WAKEUP_GPIO    	GPIO_18//828上没有


/*-------------- COMS sensor ------------------*/
#define		COMS_BUS_PORT				PORT_E
	//数据端口(GPIOE12---GPIOE23)，共12个IO

//W828 cmos电源由PMU直接输出，没有PWR控制IO
//#define		COMS_PWR_PORT				PORT_F
//#define		COMS_PWR_GPIO				GPIO_26

#define		COMS_RESET_PORT				PORT_F
#define		COMS_RESET_GPIO				GPIO_2


/*-------------- 加密功能接口 ------------------*/

/*--------------- 触摸控制功能接口 ---------------*/

/*----------------- 蜂鸣器功能接口 ------------------*/
#define  	BUZZ_PWM_PORT   			PORT_A
#define  	BUZZ_PWM1_GPIO    			GPIO_5//828
#define		BEEPER_PWM1_TIMER			(TIMER2) /* GPIOA[5]PWM占用time2 */

#define  	BUZZ_PWM2_GPIO    			GPIO_4//828
#define		BEEPER_PWM2_TIMER			(TIMER3) /* GPIOA[4]PWM占用time3 */

/*---------------- 按键功能接口 ---------------------*/
//键盘LED指示灯，高电平有效
#define		LED_KEYPAD_PORT				PORT_E
#define		LED_KEYPAD_GPIO				GPIO_27

/*---------------- LD指示灯接口 ---------------------*/
//指示灯控制开关，高电平有效
#define LED1_PORT						PORT_D
#define LED1_GPIO						GPIO_3

#define LED2_PORT						PORT_D
#define LED2_GPIO						GPIO_1

#define LED3_PORT						PORT_D
#define LED3_GPIO						GPIO_4

/* g-sensor */
#define G_SENSOR_IIC_CHL				(I2C_CH0)

#define G_SENSOR_INT1_PORT				(PORT_D)
#define G_SENSOR_INT1_GPIO				(GPIO_9)

#define G_SENSOR_INT2_PORT				(PORT_D)
#define G_SENSOR_INT2_GPIO				(GPIO_8)

/*------------------ NC ----------------------
GPIOC[16]
GPIOC[17]
GPIOC[18]
GPIOC[19]
GPIOC[20]
GPIOC[21]
GPIOC[22]
GPIOC[23]

GPIOD[0]

GPIOE[26]~GPIOE[30]

GPIOF[13]
*/
