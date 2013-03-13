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

#ifdef HYC_DEV_SENSOR_HM2055
/*----------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/
#define WRITE_ADDR_HM2055	0x48  // 0x48 For write operation, 0x49 for read. 
#define READ_ADDR_HM2055	0x49

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
0x0022, 0x00,	//; Reset
0x3600, 0x00,   //add by ben @20110603
0x0004, 0x10,	//;
0x0006, 0x01,//0x00,//; Flip/Mirror	20110509 Zeet.
0x000D, 0x11,
0x000E, 0x11,
0x000F, 0x10,	//18->10 by ben @20110603
0x0011, 0x02,	
0x0012, 0x0F,	
0x0020, 0x00,//0X20,//
0x0027, 0x10,//0x18,//0x10,//0x38,//	20110506	Zeet.
0x0040, 0x0F,	
0x0053, 0x0F,	
0x0025, 0x00,//0x80,//20110506	Zeet.	
0x0026, 0x67,//0x6B,//	
0x002A, 0x44,	
0x004A, 0x0A,	
0x004B, 0x72,	
0x0070, 0x2A,	
0x0071, 0x37,	
0x0072, 0x55,	 
0x0080, 0xC0,	
0x0082, 0xA2,	
0x0083, 0xF0,	
0x0086, 0x22,	
0x0087, 0x08,	
0x0088, 0x01,	
0x0089, 0x2A,	
0x008D, 0x20,	
0x0090, 0x01,	
0x0091, 0x02,	
0x0092, 0x03,	
0x0093, 0x04,	
0x0094, 0x14,	
0x0095, 0x09,	
0x0096, 0x0A,	
0x0097, 0x0B,	
0x0098, 0x0C,	
0x0099, 0x04,	
0x009A, 0x14,	
0x009B, 0x34,	
0x00A0, 0x00,	
0x00A1, 0x00,	
0x011F, 0x80,
0x0120, 0x36,	//; 36:50Hz, 37:60Hz
0x0121, 0x80,
0x0122, 0x7B,
0x0123, 0xC0,
0x0124, 0xDE,
0x0125, 0xFF,
0x0126, 0x71,	//70->71 by ben@20110603
0x0128, 0x1F,
0x0131, 0xAD,
0x0140, 0x14,	//; BPC
0x0141, 0x0A,
0x0142, 0x14,
0x0143, 0x0A,
0x0144, 0x0A,
0x0145, 0x00,
0x0146, 0x20,
0x0147, 0x0A,
0x0148, 0x10,
0x0149, 0x0C,
0x014A, 0x80,
0x014B, 0x80,
0x014C, 0x2E,
0x014D, 0x2E,
0x014E, 0x05,
0x014F, 0x05,
0x0150, 0x0D,
0x0155, 0x00,
0x0156, 0x10,
0x0157, 0x0A,
0x0158, 0x0A,
0x0159, 0x0A,
0x015A, 0x05,
0x015B, 0x05,
0x015C, 0x05,
0x015D, 0x05,
0x0160, 0x14,
0x0161, 0x14,
0x0162, 0x0A,
0x0163, 0x0A,
0x0168, 0x0F,
0x0169, 0x0F,
0x016A, 0xB0,
0x016B, 0xB0,
0x016C, 0x0F,
0x016D, 0x0F,
0x016E, 0x0F,
0x016F, 0x0F,
0x0171, 0xB0,
0x0173, 0xD0,
0x0175, 0xB0,
0x01B0, 0x55,	//; G1G2 Balance
0x01B1, 0x0C,
0x01B2, 0x0A,
0x01B3, 0x10,
0x01B4, 0x0E,
0x01BA, 0x10,	//; BD
0x01BB, 0x04,
0x01D8, 0x40,
0x01DE, 0x60,
0x01E4, 0x10,
0x01E5, 0x10,
0x01F2, 0x0C,
0x01F3, 0x14,
0x01F8, 0x04,
0x01F9, 0x0C,
0x01FE, 0x02,
0x01FF, 0x04,
0x0220, 0x00,	//; LSC
0x0221, 0xB0,
0x0222, 0x00,
0x0223, 0x88,
0x0224, 0x8E,
0x0225, 0x00,
0x0226, 0x8C,
0x0227, 0x8E,
0x0228, 0x00,
0x0229, 0x8C,
0x022A, 0x88,
0x022B, 0x00,
0x022C, 0x90,
0x022D, 0x13,
0x022E, 0x0B,
0x022F, 0x13,
0x0230, 0x0B,
0x0231, 0x13,
0x0232, 0x0B,
0x0233, 0x13,
0x0234, 0x0B,
0x0235, 0x20,
0x0236, 0x03,
0x0237, 0x20,
0x0238, 0x03,
0x0239, 0x20,
0x023A, 0x03,
0x023B, 0x20,
0x023C, 0x03,
0x023D, 0x58,
0x023E, 0x02,
0x023F, 0x58,
0x0240, 0x02,
0x0241, 0x58,
0x0242, 0x02,
0x0243, 0x58,
0x0244, 0x02,
0x0251, 0x0E,
0x0252, 0x00,
0x0280, 0x0A,    	//; Gamma
0x0282, 0x14,
0x0284, 0x2A,
0x0286, 0x50,
0x0288, 0x60,
0x028A, 0x6E,
0x028C, 0x7A,
0x028E, 0x83,
0x0290, 0x8B,
0x0292, 0x93,
0x0294, 0xA0,
0x0296, 0xAE,
0x0298, 0xC5,
0x029A, 0xD9,
0x029C, 0xEA,
0x029E, 0x1E,
0x02A0, 0x06,
0x02C0, 0x98,	//; CCM
0x02C1, 0x01,
0x02C2, 0x8F,
0x02C3, 0x04,
0x02C4, 0x09,
0x02C5, 0x04,
0x02C6, 0x42,
0x02C7, 0x04,
0x02C8, 0x42,
0x02C9, 0x01,
0x02CA, 0x00,
0x02CB, 0x00,
0x02CC, 0x06,
0x02CD, 0x00,
0x02CE, 0xC4,
0x02CF, 0x04,
0x02D0, 0xBE,
0x02D1, 0x01,
0x0302, 0x00,
0x0303, 0x00,
0x0304, 0x00,
0x02E0, 0x04,
0x02F0, 0x96,	//; CCM
0x02F1, 0x04,
0x02F2, 0xA8,
0x02F3, 0x00,
0x02F4, 0x12,
0x02F5, 0x04,
0x02F6, 0x14,
0x02F7, 0x04,
0x02F8, 0x0A,
0x02F9, 0x04,
0x02FA, 0x1E,
0x02FB, 0x00,
0x02FC, 0x70,
0x02FD, 0x04,
0x02FE, 0x44,
0x02FF, 0x04,
0x0300, 0xB3,
0x0301, 0x00,
0x0305, 0x14,
0x0306, 0x0E,
0x0307, 0x00,
0x032D, 0x5A,
0x032E, 0x01,
0x032F, 0x00,
0x0330, 0x01,
0x0331, 0x5A,
0x0332, 0x01,
0x0333, 0x04,	//; AWB channel offset
0x0334, 0x00,
0x0335, 0x84,
0x0336, 0x00, 	//; LED AWB gain
0x0337, 0x01,
0x0338, 0x00,
0x0339, 0x01,
0x033A, 0x00,
0x033B, 0x01,
0x033E, 0x04,
0x033F, 0x86,
0x0340, 0x30,	//; AWB
0x0341, 0x44,
0x0342, 0x4A,
0x0343, 0x40,	//; CT1
0x0344, 0x70,  	//;
0x0345, 0x4F,	//; CT2
0x0346, 0x66,  	//;
0x0347, 0x54,	//; CT3
0x0348, 0x5C,	//;
0x0349, 0x63,	//; CT4
0x034A, 0x4C,	//;
0x034B, 0x6E,	//; CT5
0x034C, 0x40,	//;
0x0350, 0x80,
0x0351, 0x80,
0x0352, 0x18,
0x0353, 0x18,
0x0354, 0x6E,
0x0355, 0x4A,
0x0356, 0x80,
0x0357, 0xC0,
0x0358, 0x06,
0x035A, 0x06,
0x035B, 0xA0,
0x035C, 0x6C,
0x035D, 0x78,
0x035E, 0xBC,
0x035F, 0xAC,
0x0360, 0x02,
0x0361, 0x18,
0x0362, 0x80,
0x0363, 0x6C,
0x0364, 0x00,
0x0365, 0xF0,
0x0366, 0x08,
0x0367, 0x00,
0x0369, 0x00,
0x036A, 0x10,
0x036B, 0x10,
0x036E, 0x20,
0x036F, 0x00,
0x0370, 0x10,
0x0371, 0x18,
0x0372, 0x0C,
0x0373, 0x38,
0x0374, 0x3A,
0x0375, 0x13,
0x0376, 0x22,
0x0380, 0xFF,
0x0381, 0x58,
0x0382, 0x40,
0x038A, 0x80,
0x038B, 0x10,
0x038C, 0xC1,
0x038E, 0x4C,
0x038F, 0x05,
0x0390, 0x00,
0x0391, 0x05,
0x0393, 0x80,
0x0395, 0x21,
0x0398, 0x01,	//; AE Frame Control
0x0399, 0xA8,
0x039A, 0x02,
0x039B, 0x12,
0x039C, 0x02,
0x039D, 0x7C,
0x039E, 0x03,
0x039F, 0x50,
0x03A0, 0x04,
0x03A1, 0x24,
0x03A2, 0x04,
0x03A3, 0xF8,
0x03A4, 0x06,
0x03A5, 0x50,
0x03A6, 0x0C,
0x03A7, 0x0C,
0x03A8, 0x0C,
0x03A9, 0x0C,
0x03AA, 0x10,
0x03AB, 0x18,
0x03AC, 0x12,
0x03AD, 0x0C,
0x03AE, 0x09,
0x03AF, 0x09,
0x03B0, 0x09,
0x03B1, 0x09,
0x03BB, 0xFF,	//; AE Windowing
0x03BC, 0xFF,
0x03BD, 0xFF,
0x03BE, 0x64,
0x03BF, 0xFD,
0x03C0, 0xFE,
0x03C1, 0xFD,
0x03C3, 0x0F,
0x03C4, 0xA8,
0x03C5, 0x64,
0x03C6, 0x00,
0x03C7, 0x01,
0x03D1, 0x18,	//; AE Adjust
0x03D2, 0xF2,
0x03D3, 0x0C,
0x03D4, 0x00,
0x03D5, 0x33,
0x03D6, 0x00,
0x03D7, 0x1E,
0x03D8, 0x00,
0x03D9, 0x0C,
0x03DA, 0x00,
0x0420, 0x85,	//; Digital Gain offset
0x0421, 0x00,
0x0422, 0x00,
0x0423, 0x84,
0x0430, 0x10,	//; ABLC
0x0431, 0x58,
0x0432, 0x28,
0x0433, 0x10,
0x0435, 0x14,
0x0450, 0xFF,
0x0451, 0xB0,
0x0452, 0x90,
0x0453, 0x70,
0x0454, 0x00,
0x0458, 0x70,
0x0459, 0x03,
0x045A, 0x00,
0x045B, 0x34,
0x045C, 0x00,
0x045D, 0xA0,
0x0466, 0x14,
0x047A, 0x00,	//; ELOFFNRB
0x047B, 0x00,	//; ELOFFNRY
0x0480, 0x56,
0x0481, 0x08,
0x0482, 0x0C,
0x04B0, 0x48,	//; Contrast
0x04B6, 0x30,
0x04B9, 0x10,
0x04B3, 0x30,
0x04B1, 0x00,
0x04B4, 0x20,
0x0520, 0x01,	//; Capture mode
0x0521, 0x00,
0x0522, 0x00,
0x0523, 0xE0,
0x0524, 0x09,
0x0525, 0x03,
0x0526, 0x7F,
0x0530, 0x01,
0x0531, 0x00,
0x0532, 0x00,
0x0533, 0x00,
0x0534, 0x02,
0x0535, 0x01,
0x0536, 0x0F,
0x0537, 0x4F,
0x0538, 0x02,
0x0540, 0x00,	//;
0x0541, 0x68,	//; 60Hz Flicker 9B->64 by ben @20110607  64->68 by mei @20110608
0x0542, 0x00,	//;
0x0543, 0x7D,	//;50Hz Flicker BA->78 by ben @20110607   78->7D by mei @20110608
0x0544, 0x00,	//;
0x0545, 0x68,	//; 60Hz SS_flicker      9B->68 by mei @20110608
0x0546, 0x00,	//;
0x0547, 0x7D,	//; 50Hz SS_flicker      BA->7D by mei @20110608
0x0580, 0x01,	//; Blur str sigma
0x0581, 0x0F,	//; Blur str sigma ALPHA
0x0582, 0x04,	//; Blur str sigma OD
0x0590, 0x02,	//; CK UVD
0x0591, 0x02,	//; UV denoise Alpha                
0x0594, 0x00,	//; UV Gray TH
0x0595, 0x04,	//; UV Gray TH Alpha
0x05A1, 0x20,
0x05A2, 0x31,
0x05A3, 0x03,
0x05A4, 0x20,
0x05A5, 0x80,
0x05A6, 0x00,
0x05A7, 0x0A,
0x05A8, 0x0A,
0x05A9, 0x01,
0x05AA, 0x40,
0x05AB, 0x80,
0x05AC, 0x0A,
0x05AD, 0x10,
0x05AE, 0x0C,
0x05AF, 0x0C,
0x05B0, 0x03,
0x05B1, 0x03,
0x05B2, 0x1C,
0x05B3, 0x02,
0x05B4, 0x00,
0x05B5, 0x0C,	//; BlurW
0x05B6, 0x00,
0x05B7, 0x00,
0x05B8, 0x80,
0x05B9, 0x32,
0x05BA, 0x00,
0x05BB, 0x80,
0x05BC, 0x03,
0x05BD, 0x00,
0x05BE, 0x04,
0x05BF, 0x05,
0x05C0, 0x10,	//; BlurW LowLight
0x05C1, 0x00,
0x05C2, 0x00,
0x05C3, 0x00,
0x05C4, 0x0C,	//; BlurW Outdoor
0x05C5, 0x20,
0x05C6, 0x32,
0x05C7, 0x01,
0x05C8, 0x14,
0x05C9, 0x54,
0x05CA, 0x14,
0x05CB, 0x54,
0x05CC, 0x20,
0x05CD, 0x00,
0x05CE, 0x10,
0x05CF, 0x60,
0x05D2, 0x03,
0x05D3, 0x03,
0x05D4, 0x00,
0x05D5, 0x05,
0x05D6, 0x05,
0x05D7, 0x00,
0x05D8, 0x08,
0x05DC, 0x0C,
0x05D9, 0x00,
0x05DA, 0x00,
0x05DB, 0x00,
0x05DD, 0x0F,
0x05DE, 0x00,
0x05DF, 0x0A,
0x05E0, 0xA0,	//; Scaler
0x05E1, 0x00,
0x05E2, 0xA0,
0x05E3, 0x00,
0x05E4, 0x04,	//; Windowing
0x05E5, 0x00,
0x05E6, 0x83,
0x05E7, 0x02,
0x05E8, 0x06,
0x05E9, 0x00,
0x05EA, 0xE5,
0x05EB, 0x01,
0x0660, 0x04,
0x0661, 0x15,
0x0662, 0x04,
0x0663, 0x36,
0x0664, 0x04,
0x0665, 0x3B,
0x0666, 0x04,
0x0667, 0x32,
0x0668, 0x04,
0x0669, 0x13,
0x066A, 0x04,
0x066B, 0x20,
0x066C, 0x00,
0x066D, 0x1C,
0x066E, 0x00,
0x066F, 0x8C,
0x0670, 0x00,
0x0671, 0x23,
0x0672, 0x04,
0x0673, 0x20,
0x0674, 0x04,
0x0675, 0x1A,
0x0676, 0x00,
0x0677, 0x89,
0x0678, 0x01,
0x0679, 0x6B,
0x067A, 0x00,
0x067B, 0x9D,
0x067C, 0x04,
0x067D, 0x1D,
0x067E, 0x04,
0x067F, 0x21,
0x0680, 0x00,
0x0681, 0x16,
0x0682, 0x00,
0x0683, 0x8B,
0x0684, 0x00,
0x0685, 0x25,
0x0686, 0x04,
0x0687, 0x2B,
0x0688, 0x04,
0x0689, 0x10,
0x068A, 0x04,
0x068B, 0x26,
0x068C, 0x04,
0x068D, 0x24,
0x068E, 0x04,
0x068F, 0x23,
0x0690, 0x04,
0x0691, 0x17,          
0x0698, 0x20,
0x0699, 0x20,
0x069A, 0x01,
0x069B, 0x01,
//0x069C, 0x22,
0x069C, 0x40,
0x069D, 0x10,
//0x069E, 0x10,
0x069E, 0x30,
0x069F, 0x0C,
0x0000, 0x01,
0x0100, 0x01,
0x0101, 0x01,
0x0005, 0x01,	//; Turn on rolling shutter

0xff,	0xff
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
#ifdef GPIO_AS_IIC0
	if(hyhwIIC_writeData_16bit(WRITE_ADDR_HM2055, Addr, &Value, 1) == HY_ERROR)
#else
	if(hyhwI2c_writeData_16bit(I2C_CH0,WRITE_ADDR_HM2055, Addr, &Value, 1 ) == HY_ERROR)
#endif
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
#ifdef GPIO_AS_IIC0
	if(hyhwIIC_readData_16bit(WRITE_ADDR_HM2055, Addr, Value, 1) == HY_ERROR)
#else
	if(hyhwI2c_readData_16bit(I2C_CH0,WRITE_ADDR_HM2055, Addr, Value, 1 ) == HY_ERROR)
#endif
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
#ifdef GPIO_AS_IIC0
	if(hyhwIIC_writeData_16bit(WRITE_ADDR_HM2055, Addr, &Value, 1) == HY_ERROR)
#else
	if(hyhwI2c_writeData_16bit(I2C_CH0,WRITE_ADDR_HM2055, Addr, &Value, 1 ) == HY_ERROR)
#endif
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
#ifdef GPIO_AS_IIC0
	if(hyhwIIC_readData_16bit(WRITE_ADDR_HM2055, Addr, &Value, 1) == HY_ERROR)
#else
	if(hyhwI2c_readData_16bit(I2C_CH0,READ_ADDR_HM2055, Addr, Value, 1 ) == HY_ERROR)
#endif
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
	int ret = 0;
	
	hyhwGpio_setCifFunction();  		/* set cif port */
 	hyhwCpm_setCifClk(PERI_CLK_24M);	/* set cif master clk is 24M*/
 	
	hyhwCmosSensor_Reset(EnableOn);

    i=400000;                   //200000
    while(i--);//Delayms(20);
    //syssleep(2);

	hyhwCmosSensor_Reset(EnableOff);
	hyhwCmosSensor_PowerDown(EnableOff);

	i=4000000;                   //200000
	while(i--);//Delayms(200);
    //syssleep(2);

/* 2009.7.28 Zeet 	added to Check the Version Num */
	hyhwCmosSensor_ReadRegister_OV(0x0001, &inta);
	hyhwCmosSensor_ReadRegister_OV(0x0002, &intb);
	
	if(0x20 != inta || 0x55 != intb)
	{		
		ret = 1 ;
		return ret;
	}
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
		
		if(Inition_Sensor[i][0] == 0x3600||Inition_Sensor[i+1][0] == 0x3600)
		{
			syssleep(10);
		}
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
#define PowerDown_Port			PORT_F
#define PowerDown_Bit			BIT26
#define Reset_Port				PORT_F
#define Reset_Bit				BIT2
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
* 函数:	hyhwCmosSensor_PowerDown()
* 功能:	power down control ,EnableOn :power down., 	//为了节约功耗，不使用sensor应进入powerdown
* 参数:	
* 返回:	2009.3.28 Zeet added
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_PowerDown(Enable_en EnalbePowerDown )
{
	hyhwGpio_setAsGpio(PowerDown_Port,PowerDown_Bit);
	hyhwGpio_setOut(PowerDown_Port,PowerDown_Bit);

	if(EnalbePowerDown == EnableOff )
	{
		hyhwGpio_setHigh(PowerDown_Port,PowerDown_Bit);
		
	}
	else
	{
		hyhwGpio_setLow(PowerDown_Port,PowerDown_Bit);
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
#if 0	
	U32 i;
	//这里是Sensor 的数据端口(GPIOE12---GPIOE23)
	for(i=0;i<=11;i++)
	{
		hyhwGpio_setAsGpio(Bus_Port,BIT12<<i);
		hyhwGpio_setOut(Bus_Port,BIT12<<i);
		hyhwGpio_setLow(Bus_Port,BIT12<<i);
	}
	//这里是Sensor 复位管脚和Power_Down 	
	hyhwGpio_setAsGpio(PowerDown_Port,PowerDown_Bit);
	hyhwGpio_setAsGpio(Reset_Port,Reset_Bit);
	
	hyhwGpio_setOut(PowerDown_Port,PowerDown_Bit);
	hyhwGpio_setOut(Reset_Port,Reset_Bit);
	
	hyhwGpio_setHigh(PowerDown_Port,PowerDown_Bit);
	hyhwGpio_setLow(Reset_Port,Reset_Bit);
#endif
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
	
	return HY_OK;
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
/*
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

0x3302, 0x11,*/

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
//0x0006,0x00,	
0x000D,0x11,	
0x000E,0x11,	
0x011F,0x80,	//  ; Subsample
0x0125,0xFF, //  ;Scaler
0x05E0,0xA0,	
0x05E1,0x00,	
0x05E2,0xA0,	
0x05E3,0x00,	
0x05E4,0x08,	//  ; Windowing
0x05E5,0x00,	
0x05E6,0x87,	
0x05E7,0x02,	
0x05E8,0x08,	
0x05E9,0x00,	
0x05EA,0xE7,	
0x05EB,0x01,	

//0x0028,0x84,//20110509 Colorbar test, Zeet.
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
//0x0006,0x00,	
0x000D,0x00,	
0x000E,0x00,	
0x011F,0x88,	//  ;FullFrame
0x0125,0xFF, //  ;Scaler
0x05E0,0x80,	
0x05E1,0x00,	
0x05E2,0x80,	
0x05E3,0x00,	
0x05E4,0x08,	//	;1280x960
0x05E5,0x00,	
0x05E6,0x07,	
0x05E7,0x05,	
0x05E8,0x08,	
0x05E9,0x00,	
0x05EA,0xC7,	
0x05EB,0x03,	

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
/*
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
*/
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