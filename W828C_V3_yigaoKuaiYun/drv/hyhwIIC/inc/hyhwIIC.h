
#ifndef _HYHW_IIC_H_
#define _HYHW_IIC_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"

#include "hyhwGpio.h"
/*************************************************************************
 * DEFINES
 *************************************************************************/

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
 /*	共2个Master的I2C */
 

#define I2C_ACK_DELAY 		500000

#define I2C_WR				0
#define I2C_RD				1


#define I2C_NUM				1
	#define I2C_CH0			0
	#define I2C_CH1			1

#define I2C_BASEADDR 					TCC7901_I2C_BASEADDR

	#define I2C_MBF_REGISTER			TCC7901_I2C_MBF_REGISTER 
	#define I2C_MB1_REGISTER 			TCC7901_I2C_MB1_REGISTER	
	
	/* Control Register */
	#define I2C_BIT_CORE_ENABLE 		BIT7		/*  I2C Core enable bit */
	#define I2C_BIT_INT_ENABLE 			BIT6		/*  I2C Core interrupt enable bit */
	#define I2C_BIT_DATA_WIDTH_16 		BIT5		/*  I2C Data Width 16 bit */
	#define I2C_BIT_DATA_WIDTH_8 	   ~BIT5		/*  I2C Data Width 8 bit */

	/* Command Register */
	#define I2C_BIT_STA_ENABLE			BIT7		/*  Start Condition Generation */
	#define I2C_BIT_STP_ENABLE			BIT6		/*  Stop  Condition Generation */
	#define I2C_BIT_RD_ENABLE			BIT5		/*  Read From Slave */
	#define I2C_BIT_WR_ENABLE			BIT4		/*  Write to Slave */
	#define I2C_BIT_ACK				   ~BIT3		/*  Sent ACK */
	#define I2C_BIT_NACK			    BIT3		/*  Sent not ACK */
	#define I2C_BIT_IACK				BIT0		/*  Interrupt Acknowledge */

	/* Status Register */
	#define I2C_BIT_RXACK				BIT7		/*  Received acknowledge from slave */
	#define I2C_BIT_BUSY				BIT6		/*  I2C Bus Busy */
	#define I2C_BIT_AL					BIT5		/*  Arbitration lost */
	#define I2C_BIT_TIP					BIT1		/*  Transfer in progress */
	#define I2C_BIT_INT_FLAG			BIT0		/*  Interrupt Flag */

	/* Timing Control Register */
	#define I2C_BIT_I2CCLK			   ~BIT5  		/*  I2CCLK from Clock controller */
	#define I2C_BIT_PCLK				BIT5		/*  PCLK (HCLK) divided by 2 */

#define I2C_IRQSTR_REGISTER				TCC7901_I2C_IRQSTR_REGISTER   /*  IRQ Status register*/

//#define IIC_DELAYTIME_20us 	20	//20uS
//#define IIC_DELAYTIME_10us 	4	//10uS
//#define IIC_DELAYTIME_5us 	2	//5uS
//#define IIC_DELAYTIME		20	//10uS


//#define GPIO_AS_IIC0	//GPIO模拟IIC
#ifdef GPIO_AS_IIC0
	#define IIC0_CLK_PORT		PORT_A
	#define IIC0_CLK_GPIO		GPIO_0
	#define IIC0_DAT_PORT		PORT_A
	#define IIC0_DAT_GPIO		GPIO_1
#endif




typedef volatile struct	ti2c_register
{
	U32 I2C_PRESCLE;		/* Clock Prescale register */
	U32 I2C_CONTROL; 		/* Control register */
	U32 I2C_TRANSMIT; 		/* Transmit Register */
	U32 I2C_COMMMAND;		/* Command Register */
	U32 I2C_RECEIVE; 		/* Receive Register */
	U32 I2C_STATUS;	 		/* Status Register */
	U32 I2C_TIMING; 		/* Timing Control Register */
	U32 I2C_RESEVE0;
	U32 I2C_RESEVE1;
	U32 I2C_RESEVE2;
	U32 I2C_RESEVE3;
	U32 I2C_RESEVE4;
	U32 I2C_RESEVE5;
	U32 I2C_RESEVE6;
	U32 I2C_RESEVE7;
	U32 I2C_RESEVE8;
}tI2C_REGISTER;

typedef enum ei2c_rate
{
	Rate_50k = 250,
	Rate_100k = 500,
	Rate_400k = 2000
	
}eI2c_Rate;


//函数声明
/*-----------------------------------------------------------------------------
* 函数:	wLock_I2cInit
* 功能:	I2C初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void wLock_I2cInit(void);

/*-----------------------------------------------------------------------------
* 函数:	wLock_I2c
* 功能:	共享资源Spi申请并lock
* 		在使用I2c之前调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void wLock_I2c(U8 channel);

/*-----------------------------------------------------------------------------
* 函数:	unLock_I2c
* 功能:	共享资源申请并lock
* 		在使用I2c之后调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void unLock_I2c(U8 channel);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_Init
* 功能:	set i2c 通讯速率
* 参数:	channel	--- 通道index 
*		rate    --- 波特率	
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_Init(U8 channel,eI2c_Rate rate);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData
* 功能:	初始化PWM部分的寄存器和static变量
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readData(U8 channel, U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeData_16bit(U8 channel, U8 SlaveAddr, U16 subaddr, U16 *buf, U16 num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readData_16bit
* 功能:	
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readData_16bit(U8 channel,U8 SlaveAddr,U16 subaddr, U16 *buf, U16 num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_writeByte
* 功能:	这个函数是写AK4183 操作
* 参数:	channel	 -- 通道index 
*		SlaveAddr-- 设备地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_writeByte(U8 channel, U8 SlaveAddr, U8 *buf, U16 num);

/*-----------------------------------------------------------------------------
* 函数:	hyhwI2c_readByte
* 功能:	这个函数是读AK4183 操作
* 参数:	channel	 -- 通道index 
*		subaddr  -- 寄存器地址
*		buf		 -- 源地址		
*		num      -- 长度
* 返回:	HY_OK  / HY_ERR_BAD_PARAMETER
*------------------------------------------------------------------------------*/
U32 hyhwI2c_readByte(U8 channel, U8 SlaveAddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_DelayuS
 * Description         : delay ?? uS
 * parameter		   : time to delay
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_DelayuS ( U16 time );

/*************************************************************************
 * Function            : hyhwIIC_Init
 * Description         : Initialize the GPIO that simulate the IIC driver
  * Return 			   : NON
 *************************************************************************/
U32 hyhwIIC_Init(U8 kIIC_CLK_Port,U8 kIIC_DAT_Port,U32 kIIC_CLK_Gpio,U32 kIIC_DAT_Gpio );

/*************************************************************************
 * Function            : hyhwIIC_Start
 * Description         : Iic start
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_Start (void);

/*************************************************************************
 * Function            : hyhwIIC_RepeatedStart
 * Description         : Iic Repeated Start
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_RepeatedStart (void);

/*************************************************************************
 * Function            : hyhwIIC_Stop
 * Description         : Iic stop
  * Return 			   : NON
 *************************************************************************/
void hyhwIIC_Stop (void);

/*************************************************************************
 * Function            : hyhwIIC_SendAck
 * Description         : send ack to slave
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_SendAck(void);

/*************************************************************************
 * Function            : hyhwIIC_SendNack
 * Description         : send nack to slave
 * Return 			   : NON
 *************************************************************************/
void hyhwIIC_SendNack(void);

/*************************************************************************
 * Function            : hyhwIIC_CheckAck
 * Description         : to check the slave ack or nack
 * Return 			   : ACK,NACK
 *************************************************************************/
U8 hyhwIIC_CheckAck(void);

/*************************************************************************
 * Function            : hyhwIIC_WriteByte
 * Description         : write one byte to the slave
 * parameter:		   : cha to send
 * Return 			   : none
 *************************************************************************/
void hyhwIIC_WriteByte(U8 cha);

/*************************************************************************
 * Function            : hyhwIIC_ReadByte
 * Description         : read one byte from the slave
 * parameter:		   : none
 * Return 			   : byte read
 *************************************************************************/
U8 hyhwIIC_ReadByte(void);

/*************************************************************************
 * Function            : hyhwIIC_WriteNbytes
 * Description         : write n bytes to slave
 * parameter:		   : slave address,data to write ,bytes to write
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_WriteNbytes(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_ReadNbytes
 * Description         : read n bytes from slave
 * parameter:		   : slave address,data buffer for reading ,bytes to read
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_ReadNbytes(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_WriteData
 * Description         : write n bytes to slave
 * parameter:		   : slave address,data to write ,bytes to write
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_WriteData(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_ReadData
 * Description         : read n bytes from slave
 * parameter:		   : slave address,data buffer for reading ,bytes to read
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_ReadData(U8 SlaveAddr, U8 subaddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_WriteBytes
 * Description         : write n bytes to slave
 * parameter:		   : slave address,data to write ,bytes to write
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_WriteBytes(U8 SlaveAddr, U8 *buf, U16 num);

/*************************************************************************
 * Function            : hyhwIIC_ReadBytes
 * Description         : read n bytes from slave
 * parameter:		   : slave address,data buffer for reading ,bytes to read
 * Return 			   : HY_OK,HY_ERROR
 *************************************************************************/
U32 hyhwIIC_ReadBytes(U8 SlaveAddr, U8 *buf, U16 num);

#endif /* _HYHW_IIC_H_ */

 /**@}*/
