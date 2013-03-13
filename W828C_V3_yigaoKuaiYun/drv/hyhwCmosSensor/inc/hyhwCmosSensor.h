
#ifndef _HYHW_CMOSSENSOR_H_
#define _HYHW_CMOSSENSOR_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"

/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/
#define SLAVE_ADDR_OV9650			0x60

#define CMOSSENSOR_DATA_ADDRESS		(0x01000000)

//#ifdef AiMei_H0103
#if 1
//#define HREF						GPIO_13	// TXD
#define VSYNC						GPIO_12	// SSI_DI
#define SCCB_CLK					GPIO_10	// SSI_DO
#define SCCB_DATA					GPIO_3	// SSI_CLK
#define WAIT						GPIO_21
#define EGPIO_LE    				GPIO_11 // JTAG-RTCK
#define EGPIO_OUTPUTEN				GPIO_6 	// MD_BOOT   //开发板上由于使用了JTAG pin，直接使能

//extended GPIO
#define eDATAENABLE					GPIO_5	// BIT7
#define eCMOSSENSOR_RESET 			NULL	// BIT4
#define eCMOSSENSOR_POWERDOWN		GPIO_4	// BIT5

#else

//#define HREF						GPIO_13	// TXD
#define VSYNC						GPIO_1	// SSI_DI
#define SCCB_CLK					GPIO_2	// SSI_DO
#define SCCB_DATA					GPIO_3	// SSI_CLK
#define WAIT						GPIO_21
#define EGPIO_LE    				GPIO_6  // MD_BOOT
#define EGPIO_OUTPUTEN				GPIO_5 	// JTAG-RTCK    //开发板上由于使用了JTAG pin，直接使能

//extended GPIO
#define eDATAENABLE					BIT7
#define eCMOSSENSOR_RESET 			BIT4
#define eCMOSSENSOR_POWERDOWN		BIT5

#endif

#define COM1	0x04
#define COM6	0x0F
#define COM7	0x12
#define COM10	0x15
#define COM15	0x40
#define CLKRC	0x11

#define CMOSSENSOR_DVK
#ifdef CMOSSENSOR_DVK
	#define IIC_CLK					GPIO_2
	#define IIC_DAT					GPIO_3
#else
	#define IIC_CLK					GPIO_8
	#define IIC_DAT					GPIO_9
#endif



/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
//extern __align(4) char ComsSensorReadBuf_DC[0x1800];//6K

//extern __align(4) char ComsSensorReadBuf_DV[0x9800];//38K
extern __align(4) char ComsSensorReadBuffer[0x4B00];//38K


typedef enum
{
	hwCmosSensor_Scene_Auto_en,			//自动
	hwCmosSensor_Scene_Character_en,	//人物
	hwCmosSensor_Scene_Scenery_en,		//风景
	hwCmosSensor_Scene_Night_en			//夜景

} hyhwCmosSensor_mode_en;

/*************************************************************************
 * Function Prototypes:
 *************************************************************************/
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Init
* 功能:	初始化Cmos sensor。 
* 参数:	none          
* 返回:	0:success
*----------------------------------------------------------------------------*/
int hyhwCmosSensor_Init();

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_ReadRegister
* 功能:	read the register value 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
//U32 hyhwCmosSensor_ReadRegister(U8 Addr,U8* Value);
U32 hyhwCmosSensor_ReadRegister_MIC(U16 Addr,U16* Value);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_WriteRegister
* 功能:	write the value to the specified register 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
//U32 hyhwCmosSensor_WriteRegister(U8 Addr,U8 Value);
U32 hyhwCmosSensor_WriteRegister_MIC(U16 Addr,U16 Value);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetWindowArea
* 功能:	设置cmos sensor的window
* 参数:	         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_SetWindowArea(U16 RowStart,U16 RowEnd,U16 ColStart,U16 ColEnd);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_GetOneFrameQVGA
* 功能:	read frame and display 
* 参数:	none        
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_GetOneFrameQVGA(U8 *pFrameBuf);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_ReadRegister
* 功能:	read the register value 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_GetOneFrameSXGA(U8 *pFrameBuf,U16 Mode); //1280X1024

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Reset()
* 功能:	cmos sensor reset control; EnableOn :reset 
* 参数:	
* 返回:	
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_Reset(Enable_en EnalbeReset );

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_PowerDown()
* 功能:	power down control ,EnableOn :power down. //为了节约功耗，不使用sensor应进入powerdown
* 参数:	
* 返回:	
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_PowerDown(Enable_en EnalbePowerDown );

U32 hyhwCmosSensor_WriteRegister(U16 Addr,U16 Value);
U32 hyhwCmosSensor_ReadRegister(U16 Addr,U16* Value);

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_WriteRegister
* 功能:	write the value to the specified register 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_WriteRegister_OV(U16 Addr,U16 Value);
#endif /* _HYHW_CMOSSENSOR_H_ */

 /**@}*/
