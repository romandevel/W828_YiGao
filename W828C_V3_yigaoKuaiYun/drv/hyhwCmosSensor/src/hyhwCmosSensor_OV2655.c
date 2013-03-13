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

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*----------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "hyhwTimer.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwIIC.h"
#include "hyhwCmosSensor.h"
#include "HycDeviceConfig.h"

#ifdef HYC_DEV_SENSOR_OV2655
/*----------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/
#define WRITE_ADDR_OV3640	0x60  // 0x60 For write operation, 0x61 for read. 
#define READ_ADDR_OV3640	0x61

#define STEP_STATE_NO			0x80
#define MASK_MODECHANGE			0x40
#define MASK_CAPTURECMD			0x20
#define RESERVED_INVALID		0x10

#define MODE_IDLE				0x00
#define MODE_SINGLE				0x04
#define MODE_CONTINUE			0x08
#define MODE_STEP				0x0c

#define MODE_STEP_INSTRUCTION	0x00
#define MODE_STEP_FOCUSING		0x01
#define MODE_STEP_FOCUSED		0x02
#define MODE_STEP_CAPTURE		0x03

#define FOCUS_STATE_IDLE		0x00
#define FOCUS_STATE_FOCUSING	0x01
#define FOCUS_STATE_FOCUSED		0x02

/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/
#if 0
	#define STATE_INF 		 (MODE_IDLE|MODE_STEP_INSTRUCTION)
	#define STATE_SINGLE 	 (MODE_SINGLE|MODE_STEP_FOCUSING|MASK_MODECHANGE|MASK_CAPTURECMD)
	#define STATE_SUCCESS_S  (MODE_SINGLE|MODE_STEP_FOCUSED|MASK_MODECHANGE)
	#define STATE_FAIL_S 	 (MODE_SINGLE|MODE_STEP_FOCUSED|MASK_MODECHANGE|STEP_STATE_NO)
	#define STATE_CAPTURE_S  (MODE_SINGLE|MODE_STEP_CAPTURE|MASK_MODECHANGE)
#else
	#define STATE_UNINIT 	 (MODE_IDLE|MODE_STEP_INSTRUCTION|STEP_STATE_NO)
	#define STATE_INF 		 (MODE_IDLE|MODE_STEP_INSTRUCTION)
	#define STATE_SINGLE 	 (MODE_SINGLE|MODE_STEP_FOCUSING|MASK_MODECHANGE|MASK_CAPTURECMD)
	#define STATE_SUCCESS_S  (MODE_SINGLE|MODE_STEP_FOCUSED|MASK_MODECHANGE)
	#define STATE_FAIL_S 	 (MODE_SINGLE|MODE_STEP_FOCUSED|MASK_MODECHANGE|STEP_STATE_NO)
	#define STATE_CAPTURE_S  (MODE_SINGLE|MODE_STEP_CAPTURE|MASK_MODECHANGE)
	#define STATE_CONTINUOUS (MODE_CONTINUE|MODE_STEP_FOCUSING|MASK_MODECHANGE|MASK_CAPTURECMD)
	#define STATE_SUCCESS_C  (MODE_CONTINUE|MODE_STEP_FOCUSED|MASK_MODECHANGE)
	#define STATE_FAIL_C 	 (MODE_CONTINUE|MODE_STEP_FOCUSED|MASK_MODECHANGE|STEP_STATE_NO)
	#define STATE_CAPTURE_C  (MODE_CONTINUE|MODE_STEP_CAPTURE|MASK_MODECHANGE)
	#define STATE_STEP 		 (MODE_STEP|MODE_STEP_FOCUSED|MASK_MODECHANGE)
	#define STATE_UNKOWN	 (RESERVED_INVALID)
#endif
U16 ReadData = 0;
/*------------------------------------------------------------------------------
Static Variables:
------------------------------------------------------------------------------*/
#define YUV_640x480
//#define YUV_1280x1024
const U16 Inition_Sensor[][2]=
{
// OV2655_SVGA_YUV 20 fps
// 24 MHz input clock, SVGA
// initial settings

//software reset
0x3012, 0x80,

0x308c, 0x80,
0x308d, 0x0e,
0x360b, 0x00,
0x30b0, 0xff,
0x30b1, 0xff,
0x30b2, 0x2c,
0x300e, 0x34,
0x300f, 0xa6,
0x3010, 0x81,
0x3082, 0x01,
0x30f4, 0x01,
0x3090, 0x33,
0x3091, 0xc0,
0x30ac, 0x42,
0x30d1, 0x08,
0x30a8, 0x56,
0x3015, 0x03,
0x3093, 0x00,
0x307e, 0xe5,
0x3079, 0x00,
0x30aa, 0x42,
0x3017, 0x40,
0x30f3, 0x82,
0x306a, 0x0c,
0x306d, 0x00,
0x336a, 0x3c,
0x3076, 0x6a,

0x30d9, 0x8c,
0x3016, 0x82,
0x3601, 0x30,
0x304e, 0x88,
0x30f1, 0x82,
0x306f, 0x14,
0x302a, 0x02,
0x302b, 0x6a,
0x3012, 0x10,
0x3011, 0x01,
// AEC/AGC
0x3013, 0xf7,
0x301c, 0x05,
0x301d, 0x07,
0x3070, 0x5d,
0x3072, 0x4d,
// D5060  
0x3014, 0x84,
0x30af, 0x10,
0x304a, 0x00,
0x304f, 0x00,
// Window Setup
0x300e, 0x38,
0x3020, 0x01,
0x3021, 0x18,
0x3022, 0x00,
0x3023, 0x06,
0x3024, 0x06,
0x3025, 0x58,
0x3026, 0x02,
0x3027, 0x5e,
0x3088, 0x03,
0x3089, 0x20,
0x308a, 0x02,
0x308b, 0x58,
0x3316, 0x64,
0x3317, 0x25,
0x3318, 0x80,
0x3319, 0x08,
0x331a, 0x64,
0x331b, 0x4b,
0x331c, 0x00,
0x331d, 0x38,
0x3100, 0x00,

//0x3600, 0x65,//2011.4.7 Zeet added for PCLK.
//0x3600, 0x65,
// AWB
0x3320, 0xfa,
0x3321, 0x11,
0x3322, 0x92,
0x3323, 0x01,
0x3324, 0x97,
0x3325, 0x02,
0x3326, 0xff,
0x3327, 0x0c,
0x3328, 0x10,
0x3329, 0x13,
0x332a, 0x58,
0x332b, 0x5f,
0x332c, 0xbe,
0x332d, 0x9b,
0x332e, 0x3a,
0x332f, 0x36,
0x3330, 0x4d,
0x3331, 0x44,
0x3332, 0xf0,
0x3333, 0x0a,
0x3334, 0xf0,
0x3335, 0xf0,
0x3336, 0xf0,
0x3337, 0x40,
0x3338, 0x40,
0x3339, 0x40,
0x333a, 0x00,
0x333b, 0x00,
// Color Matrix
0x3380, 0x28,
0x3381, 0x48,
0x3382, 0x10,
0x3383, 0x24,
0x3384, 0xc9,
0x3385, 0xed,
0x3386, 0xb4,
0x3387, 0xc7,
0x3388, 0x13,
0x3389, 0x98,
0x338a, 0x00,
// Gamma
0x3340, 0x0a,
0x3341, 0x18,
0x3342, 0x31,
0x3343, 0x45,
	    	 
0x3344, 0x55,
0x3345, 0x65,
0x3346, 0x70,
0x3347, 0x7c,
0x3348, 0x86,
0x3349, 0x96,
0x334a, 0xa3,
0x334b, 0xaf,
0x334c, 0xc4,
0x334d, 0xd7,
0x334e, 0xe8,
0x334f, 0x20,
// Lens correction
0x3300, 0xFC,
0x3350, 0x2f,
0x3351, 0x25,
0x3352, 0x08,
0x3353, 0x1d,
0x3354, 0x00,
0x3355, 0x85,
// G    
0x3356, 0x30,
0x3357, 0x25,
0x3358, 0x08,
0x3359, 0x1c,
0x335a, 0x00,
0x335b, 0x85,
// B    
0x335c, 0x2e,
0x335d, 0x25,
0x335e, 0x08,
0x335f, 0x1b,
0x3360, 0x00,
0x3361, 0x85,
0x3363, 0x01,
0x3364, 0x03,
0x3365, 0x02,
0x3366, 0x00,
// UVadjust
//0x3301, 0xff,
0x338b, 0x08,
0x338c, 0x10,
	    	 
0x338d, 0x40,
// Sharpness/De-noise
0x3306, 0x00,
0x3370, 0xd0,
0x3371, 0x00,
0x3372, 0x00,
0x3373, 0x30,
0x3374, 0x10,
0x3375, 0x10,
0x3376, 0x05,
0x3377, 0x00,
0x3378, 0x04,
0x3379, 0x40,
// BLC
0x3069, 0x84,
0x307c, 0x10,
0x3087, 0x02,
// Other functions
0x3300, 0xfc,
0x3302, 0x11,
0x3400, 0x00,
0x3606, 0x20,
0x3601, 0x30,
0x300e, 0x36, //25fps svga, 30Mhz pclk
0x3011, 0x00,
0x30f3, 0x83,
0x304e, 0x88,
0x3086, 0x0f,
0x3086, 0x00,
//0x3373, 0x30,
	    	 
0x3303, 0x00,
0x3308, 0x00,//////////
0x3069, 0x80,
	    	 
0x30a1, 0x01,
0x30a3, 0x00,
0x30a8, 0x56,
0x30aa, 0x42,
0x30af, 0x10,
0x30b2, 0x2c,
0x30d9, 0x8c,
	  
	  
//preview
0x3010, 0x81,
0x300e, 0x38, //20fps svga
0x3011, 0x00,
0x3012, 0x10,
0x3014, 0x84,
0x3015, 0x03, 
0x3016, 0x82,
0x301c, 0x05,//0x05,
0x301d, 0x07,//0x07,
0x3023, 0x06, 
0x3026, 0x02, 
0x3027, 0x5e,
0x302a, 0x02, 
0x302b, 0x6a,
0x306f, 0x14,
0x3088, 0x03, 
0x3089, 0x20,
0x308a, 0x02, 
0x308b, 0x58,
0x308e, 0x00, 
0x3090, 0x33,
0x30a1, 0x01, 
0x30a3, 0x00, 
0x30d9, 0x8c,
0x3302, 0x11,
0x3317, 0x25,
0x3318, 0x80,
0x3319, 0x08, 
0x331d, 0x38,
0x3340, 0x0e, 
0x3341, 0x1a,
0x3342, 0x31,
//0x3373, 0x40,
0x361e, 0x02, 
0x363b, 0x41,
0x363c, 0xa2,
0x3362, 0x80, 
0x300f, 0xa6,

//0x3600, 0x65,//2011.4.7 Zeet added for PCLK.
0x3600, 0x65,

0x3013, 0xf7,
0x301c, 0x05,
0x301d, 0x07,
0x3070, 0x5d,
0x3072, 0x4d,


0xff,0xff	
};		


//const U16 Inition_AF_FW[][2]=
//{};


/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_WriteRegister_OV
* 功能:	write the value to the specified register 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_WriteRegister_OV(U16 Addr,U16 Value)
{
	if(hyhwI2c_writeData_16bit(I2C_CH1,WRITE_ADDR_OV3640, Addr, &Value, 1 ) == HY_ERROR)
	{
		return HY_ERROR;
	}
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_ReadRegister_OV
* 功能:	read the register value 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_ReadRegister_OV(U16 Addr,U16* Value)
{
	if(hyhwI2c_readData_16bit(I2C_CH1,READ_ADDR_OV3640, Addr, Value, 1 ) == HY_ERROR)
	{
		return HY_ERROR;
	}
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_WriteRegister_OV
* 功能:	write the value to the specified register 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_WriteRegister(U16 Addr,U16 Value)
{
	if(hyhwI2c_writeData_16bit(I2C_CH1,WRITE_ADDR_OV3640, Addr, &Value, 1 ) == HY_ERROR)
	{
		return HY_ERROR;
	}
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_ReadRegister_OV
* 功能:	read the register value 
* 参数:	register address and value         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_ReadRegister(U16 Addr,U16* Value)
{
	if(hyhwI2c_readData_16bit(I2C_CH1,READ_ADDR_OV3640, Addr, Value, 1 ) == HY_ERROR)
	{
		return HY_ERROR;
	}
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Init
* 功能:	初始化Cmos sensor。 
* 参数:	none          
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_AF_Init()
{
	/*U32 i;
	
	i = 0;
	while(1)
	{
		int kl;
		if(Inition_AF_FW[i][0]==0xff && Inition_AF_FW[i][1]==0xff) break;
		else
		{
			hyhwCmosSensor_WriteRegister_OV(Inition_AF_FW[i][0],Inition_AF_FW[i][1]);
		}
		i++;
		kl=20000;                  
		
	}
	hyhwCmosSensor_WriteRegister_OV(0x3F00, 0x08);
	syssleep(20);*/
}
//extern U8 testvalue;
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Init
* 功能:	初始化Cmos sensor。 
* 参数:	none          
* 返回:	0:success
*----------------------------------------------------------------------------*/
int hyhwCmosSensor_Init()
{
	U32 i,kl;
	U16 inta = 0,intb = 0;
	int ret = 0;//默认成功
	
	hyhwGpio_setCifFunction();  		/* set cif port */
 	hyhwCpm_setCifClk(PERI_CLK_24M);	/* set cif master clk is 24M*/
 	
	hyhwCmosSensor_Reset(EnableOn);

    i=400000;                   //200000
    while(i--);//Delayms(20);
    //syssleep(2);

	hyhwCmosSensor_Reset(EnableOff);
	hyhwCmosSensor_powerCtrl(EnableOn);

	i=4000000;                   //200000
	while(i--);//Delayms(200);
    //syssleep(2);
/* 2009.7.28 Zeet 	added to Check the Version Num */
	//hyhwCmosSensor_ReadRegister_OV(0x300a, &inta);
	//hyhwCmosSensor_ReadRegister_OV(0x300b, &intb);
	/*if(0x36 != inta || 0x4c != intb)
	{		
		ret = 1 ;
		return ret;
	}*/
	while(1)
	{
		if(Inition_Sensor[i][0]==0xff && Inition_Sensor[i][1]==0xff)
		{
			break;
		}
		else
		{
			hyhwCmosSensor_WriteRegister_OV(Inition_Sensor[i][0],Inition_Sensor[i][1]);
		}
		
		/*if(Inition_Sensor[i][0] == 0x3600||Inition_Sensor[i+1][0] == 0x3600)
		{
			kl=20000;                  //200000
			while(kl--);//Delayms(2s);
		}*/
		i++;
	}
	
	return ret;
}


int set_OV3640_AF_enable(void)
{
	hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x03);
	return 0;
}

#define Bus_Port				PORT_E
#define PowerDown_Port			PORT_A
#define PowerDown_Bit			BIT1
#define Reset_Port				PORT_F
#define Reset_Bit				BIT22
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Reset()
* 功能:	cmos sensor reset control; EnableOn :reset 
* 参数:	
* 返回:	2009.3.28 Zeet added
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_Reset(Enable_en EnalbeReset )
{	 
	hyhwGpio_setAsGpio(Reset_Port,Reset_Bit);
	hyhwGpio_setOut(Reset_Port,Reset_Bit);
	 
	if(EnalbeReset == EnableOff )
	{
		hyhwGpio_setHigh(Reset_Port,Reset_Bit);
	}
	else
	{
		hyhwGpio_setLow(Reset_Port,Reset_Bit);
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_port_powerSave
* 功能:	控制sensor 总线功耗
* 参数:	none       
* 返回:	none 2009.3.28 Zeet added
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_port_powerSave(void)
{
	U32 i;
	//这里是Sensor 的数据端口(GPIOE12---GPIOE23)
	for(i=0;i<=11;i++)
	{
		hyhwGpio_setAsGpio(Bus_Port,BIT12<<i);
		hyhwGpio_setOut(Bus_Port,BIT12<<i);
		hyhwGpio_setLow(Bus_Port,BIT12<<i);
	}
	//这里是Sensor 复位管脚
	hyhwGpio_setAsGpio(Reset_Port,Reset_Bit);
	hyhwGpio_setOut(Reset_Port,Reset_Bit);
	hyhwGpio_setLow(Reset_Port,Reset_Bit);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_standby_enable
* 功能:	启动 standby
* 参数:	none       
* 返回:	none 2009.3.28 Zeet added(need to correct)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_standby_enable(void)
{
	return HY_ERROR;
}
/*
U32 hyhwCmosSensor_standby_enable(void)
{
	U32 rc = HY_ERROR;
	U32 i,standby_flag;
	
	hyhwCmosSensor_WriteRegister_OV(0x09,0x10);

	for(i=0;i<10;i++)
	{
		hyhwCmosSensor_ReadRegister_OV(0x09,&standby_flag);
		if(standby_flag==0x10)
		{
//			hyhwGpio_setAsGpio(Bus_Port,BIT8);
//			hyhwGpio_setOut(Bus_Port,BIT8);
//			hyhwGpio_setLow(Bus_Port,BIT8);
			rc= HY_OK;
		}
	}
	hyhwCmosSensor_port_powerSave();
	return rc;
}
*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetScene
* 功能:	设置cmossensor 场镜模式
* 参数:		mode	--	各种场镜模式的枚举，包括以下场镜模式
*			hwCmosSensor_Scene_Auto_en,
*			hwCmosSensor_Scene_Scenery_en,
*			hwCmosSensor_Scene_Character_en,
*			hwCmosSensor_Scene_Night_en,
* 返回:	none
*----------------------------------------------------------------------------*/

void hyhwCmosSensor_SetScene(hyhwCmosSensor_mode_en mode)
{
}
/**/
#if 0
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_standby_disable
* 功能:	退出 standby
* 参数:	none       
* 返回:	none 2009.3.28 Zeet added(need to correct)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_standby_disable(void)
{
	U32 i,standby_flag;
	hyhwCmosSensor_WriteRegister_OV(0x09,0x00);

//for test
#if 0	
	for(i=0;i<10;i++)
	{
		
		hyhwCmosSensor_ReadRegister_OV(0x0018,&standby_flag);
		{
		#include "PhoneDebug.h"
 		//PhoneTrace(0,"%x",standby_flag);
 		standby_flag = 0;
 		
 		}	
	}
#endif	
}
#endif
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Af_enable
* 功能:	启动自动对焦
* 参数:	none       
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_Af_enable(void)
{
//	hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x03);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_AF_Done
* 功能:	读取自动对焦是否完成
* 参数:	none        
* 返回:	HY_OK--完成，HY_ERROR--没有完成
*----------------------------------------------------------------------------*/
U16 hyhwCmosSensor_AF_Done(void)
{
/*	int ret = HY_ERROR;
	U16 readVal;
	
	hyhwCmosSensor_ReadRegister_OV(0x3f07, &readVal);
	if (readVal == FOCUS_STATE_FOCUSED)
	{
		ret = HY_OK;
	}
	return (U16)ret;*/
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_AF_Release
* 功能:	释放AF，否则功耗较大，焦距越小，功耗越大，最小焦距时，AF约耗电100mA
*		该函数在对焦完成且取出图像后调用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
int hyhwCmosSensor_AF_Release(void)
{
//	hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x08);
}

//调整焦距位置步进
int ov3640_stepPosition(int direction)
{
/*	int cnt;
	
	if (direction == 1)
	{
		//step 近
		hyhwCmosSensor_WriteRegister_OV(0x3f01, 0x01);
		hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x05);
	}
	else
	{
		//step 远
		hyhwCmosSensor_WriteRegister_OV(0x3f01, 0x02);
		hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x05);
	}
	cnt = 1000;
	while(cnt--)
	{
		U16 readVal;
		hyhwCmosSensor_ReadRegister_OV(0x3f07, &readVal);//文档上没有找到0x3F07描述
		if (readVal == 0x20)//?????,是判断这个寄存器和这个值吗？
		{
			//break;
			return 0;
		}
	}*/
	return -1;
}


void hyhwCmosSensor_Set(U8 flag)
{
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Write_Position
* 功能:	设置手动对焦的位置
* 参数:	Position :min: 0x00 max: 0xff        
* 返回:	none
*----------------------------------------------------------------------------*/
U16 hyhwCmosSensor_Write_Position(U16 Position)
{
	int cnt;
/*	
	hyhwCmosSensor_WriteRegister_OV(0x3f01, 0x01);
	hyhwCmosSensor_WriteRegister_OV(0x3f00, 0x05);
	hyhwCmosSensor_WriteRegister_OV(0x3F05, (U8)Position);
	
	cnt = 1000;
	while(cnt--)
	{
		U16 readVal;
		hyhwCmosSensor_ReadRegister_OV(0x3f07, &readVal);
		if (readVal == 0x20)
		{
			//break;
			return 0;
		}
	}*/
	return 0;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Read_Position
* 功能:	读取手动对焦当前的位置
* 参数:	none        
* 返回:	none 2009.3.28 Zeet added(need to correct)
*----------------------------------------------------------------------------*/
U16 hyhwCmosSensor_Read_Position(void)
{
	return HY_ERROR;
/*	U16 Position;
	
	hyhwCmosSensor_WriteRegister_OV(0x30,0x00);
	hyhwCmosSensor_ReadRegister_OV(0x31,&Position);
	
	return Position;
*/
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetWindowArea
* 功能:	设置cmos sensor的window
* 参数:	         
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_SetWindowArea(U16 RowStart,U16 RowEnd,U16 ColStart,U16 ColEnd)
{
//	return HY_ERROR;
/*	U8 addr;
	U8 HRef,VRef;
	
	addr = (ColStart>>3)&0xff;
	hyhwCmosSensor_WriteRegister_OV(0x19,addr); //Vertical start high 8bit
	addr = (ColEnd>>3)&0xff;
	hyhwCmosSensor_WriteRegister_OV(0x1a,addr); //Vertical end high 8bit
	HRef = ((ColStart&0x07)+((ColEnd&0x07)<<3))|0x80; //|0x80:bit7,bit6 reset value
	hyhwCmosSensor_WriteRegister_OV(0x32,HRef); //Vertical start and end low bits
	
	addr = (RowStart>>2)&0xff;
	hyhwCmosSensor_WriteRegister_OV(0x17,addr); //Horizontal start high 8bit
	addr = (RowEnd>>2)&0xff;
	hyhwCmosSensor_WriteRegister_OV(0x18,addr); //Horizontal end high 8bit
	VRef = ((RowStart&0x03)+((RowEnd&0x03)<<2))|0x40; //|0x40:bit7,bit6 reset value
	hyhwCmosSensor_WriteRegister_OV(0x03,VRef); //Horizontal start and end low bits
*/
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_Reset()
* 功能:	cmos sensor reset control; EnableOn :reset 
* 参数:	
* 返回:	2009.3.28 Zeet edited
*----------------------------------------------------------------------------*/
/*void hyhwCmosSensor_Reset(Enable_en EnalbeReset )
{
	if(EnalbeReset ==EnableOff )
	{
		hyhwGpio_setLow(PORT_F,BIT15);
	}
	else
	{
		hyhwGpio_setHigh(PORT_F,BIT15);
	}
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_PowerDown()
* 功能:	power down control ,EnableOn :power down. //为了节约功耗，不使用sensor应进入powerdown
* 参数:	
* 返回:	2009.3.28 Zeet edited
*----------------------------------------------------------------------------*/
/*void hyhwCmosSensor_PowerDown(Enable_en EnalbePowerDown )
{
	if(EnalbePowerDown ==EnableOff )
	{
		hyhwGpio_setLow(PORT_F,BIT14);
	}
	else
	{
		hyhwGpio_setHigh(PORT_F,BIT14);
	}
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetQVGAMode
* 功能:	将cmos sensor的输出设置成QVGA输出
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

const U16  QVGAWriteRegister[][2] = 
{
//[Sensor.YUV.320x240]
//99 320 240
0x306f, 0x14,
0x302a, 0x02,
0x302b, 0x6a,
0x3012, 0x10,
0x3011, 0x01,
0x3070, 0x5d,
0x3072, 0x4d,
	  		  
0x300e, 0x38,
0x3020, 0x01,
0x3021, 0x18,
0x3022, 0x00,
0x3023, 0x06,
0x3024, 0x06,
0x3025, 0x58,
0x3026, 0x02,
0x3027, 0x61,
0x3088, 0x01,
0x3089, 0x40,
0x308a, 0x00,
0x308b, 0xf0,
0x3316, 0x64,
0x3317, 0x25,
0x3318, 0x80,
0x3319, 0x08,
0x331a, 0x14,
0x331b, 0x0f,
0x331c, 0x00,
0x331d, 0x38,

0x3302, 0x11,

0xff,0xff
};	
void hyhwCmosSensor_SetQVGAMode(void)
{

	U32 i = 0;
	
	while(1)
	{
		if(QVGAWriteRegister[i][0]==0xff && QVGAWriteRegister[i][1]==0xff) break;
		
		hyhwCmosSensor_WriteRegister_OV(QVGAWriteRegister[i][0],QVGAWriteRegister[i][1]);
		
		i++;
	}
	
	return;
}
/*2009.3.30 Zeet edited
void hyhwCmosSensor_SetQVGAMode(void)
{
//	return HY_ERROR;
}
*/
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetVGAMode
* 功能:	将cmos sensor的输出设置成VGA输出
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
const U16  VGAWriteRegister[][2] = 
{
/**/
//[Sensor.YUV.640x480]
//99 640 480
0x306f, 0x14,
0x302a, 0x02,
0x302b, 0x6a,
0x3012, 0x10,
0x3011, 0x01,
0x3070, 0x5d,
0x3072, 0x4d,
	  
0x300e, 0x38,
0x3020, 0x01,
0x3021, 0x18,
0x3022, 0x00,
0x3023, 0x06,
0x3024, 0x06,
0x3025, 0x58,
0x3026, 0x02,
0x3027, 0x61,
0x3088, 0x02,
0x3089, 0x80,
0x308a, 0x01,
0x308b, 0xe0,
0x3316, 0x64,
0x3317, 0x25,
0x3318, 0x80,
0x3319, 0x08,
0x331a, 0x28,
0x331b, 0x1e,
0x331c, 0x00,
0x331d, 0x38,

0x3302, 0x11,

0x3011, 0x00, //image output height L
0x300e, 0x34, //<5:0>plldiv
0x302E, 0x00,
0x301c, 0x02,
0x301d, 0x03,
0x3070, 0xb9,
0x3071, 0x00,
0x3072, 0x9a,
0x3073, 0x00,

0xff,0xff
};
void hyhwCmosSensor_SetVGAMode(void)
{
	U32 i = 0;
	
	while(1)
	{
		if(VGAWriteRegister[i][0]==0xff && VGAWriteRegister[i][1]==0xff) break;
		
		hyhwCmosSensor_WriteRegister_OV(VGAWriteRegister[i][0],VGAWriteRegister[i][1]);
		{
			int j=1000;
		//	while(j--);
		}
		i++;
	}
	
	return;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetSXGAMode
* 功能:	将cmos sensor的输出设置成SXGA输出
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
const U16  SXGAWriteRegister[][2] = 
{
//3.5 SXGA Capture, 7.5fps, 24 Mhz input clock
/**/
//[Sensor.YUV.1280x1024]
//99 1280 1024
0x306f, 0x54,
0x302a, 0x04,
0x302b, 0xd4,
0x3012, 0x00,
0x3011, 0x02,
	  
0x300e, 0x34,
0x3020, 0x01,
0x3021, 0x18,
0x3022, 0x00,
0x3023, 0x0a,
0x3024, 0x06,
0x3025, 0x58,
0x3026, 0x04,
0x3027, 0xbc,
0x3088, 0x05,
0x3089, 0x00,
0x308a, 0x04,
0x308b, 0x00,
0x3316, 0x64,
0x3317, 0x4b,
0x3318, 0x00,
0x3319, 0x6c,
0x331a, 0x50,
0x331b, 0x40,
0x331c, 0x00,
0x331d, 0x6c,
	  
0x3302, 0x11,
	  
0x3011, 0x01,
0x3088, 0x05,  //image output width H
0x3089, 0x00,  //image output width L
0x308a, 0x04,  //image output height H
0x308b, 0x00,  //image output height L
0x331a, 0x50,
0x331b, 0x40,
0x3302, 0x11,  // scale_en

0xff, 0xff,
};

void hyhwCmosSensor_SetSXGAMode(void)
{
	U32 i = 0;
	
	while(1)
	{
		if(SXGAWriteRegister[i][0]==0xff && SXGAWriteRegister[i][1]==0xff) break;
		
		hyhwCmosSensor_WriteRegister_OV(SXGAWriteRegister[i][0],SXGAWriteRegister[i][1]);
		
		i++;
	}	
	return;
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwCmosSensor_SetQXGAMode()
* 功能:	将cmos sensor的输出设置成QXGA输出
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
const U16  QXGAWriteRegister[][2] = 
{
/**/
	0x3302, 0xcf,
	0x335f, 0x68,
	0x3360, 0x18,
	0x3361, 0x0c,
	0x3362, 0x68,
	0x3363, 0x08,
	0x3364, 0x04,
	0x3403, 0x42,
	0x3088, 0x08,
	0x3089, 0x00,
	0x308a, 0x06,
	0x308b, 0x00,

	0xff, 0xff
};
void hyhwCmosSensor_SetQXGAMode(void)
{
	U32 i = 0;
	
	while(1)
	{
		if(VGAWriteRegister[i][0]==0xff && VGAWriteRegister[i][1]==0xff) break;
		
		hyhwCmosSensor_WriteRegister_OV(QXGAWriteRegister[i][0],QXGAWriteRegister[i][1]);
		
		i++;
	}
	
	return;
}
/*-----------------------------------------------------------------------------
* 函数:	caculate_expo
* 功能:	preview as QVGA 12.5fps,capture at SXGA 6.25fps  no auto nightmode 
* 参数:	
* 返回:	none (always success)
*----------------------------------------------------------------------------*/
void caculate_expo(short pre_expo,short pre_gain ,short *cap_expo,short *cap_gain,U16 Mode)
{
//	return HY_ERROR;
/*	int Capture_Exposure_Gain;
	short Capture_Exposure =0;
	short Capture_gain;


	//pre_expo =pre_expo*21/20*10/26; 	//pre_expo*21/40; 21/20*10/28
	
	pre_expo =pre_expo/Mode;//6
	
	Capture_Exposure_Gain = pre_gain * pre_expo;
	if (Capture_Exposure_Gain < 1048*16)
	{
		Capture_Exposure = Capture_Exposure_Gain/16;
		
		if (Capture_Exposure > 21) 	//32		
		{			
			Capture_Exposure = Capture_Exposure/22*22;
					
		}	
		//Capture_gain = 16;
			
		if(Capture_Exposure!=0)
		{
			Capture_gain = Capture_Exposure_Gain/Capture_Exposure;//test jemy				
		}		
		
	}
	else
	{//if image is dark,//and use the correct exposure time to eliminate 50/60 hz line
		Capture_Exposure = 1048;
		Capture_gain = Capture_Exposure_Gain / 1048;
	}


	*cap_expo =Capture_Exposure;
	*cap_gain=Capture_gain;
*/
}

#endif