
/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "glbStatusDef.h"
#include "glbCmdDef.h"
#include "string.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "CrcVerify.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "LcdAPIFunc.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define CRC_OK		0
#define CRC_ERR		1


/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* Variable ,constant or function
--------------------------------------------------------------------------*/

//显示屏
U16  gLcd_Total_Column = cLCD_TOTAL_COLUMN;
U16  gLcd_Total_Row = cLCD_TOTAL_ROW;

// 为了在某些产品上正确地显示，如在各个模式下，显示的界限不一样
U16 gu16Lcd_ColumnLimit = cLCD_TOTAL_COLUMN;//从1到cLCD_TOTAL_COLUMN

U16 gFrontColor = COLOR_WHITE;
U16 gBackColor = COLOR_BLACK;

//U8 gScreen_Display_Mode = 0;//0:右手方向  1:左手方向

U8 gu8EnableRefreshSrc = 1;

BOOL gRefreshNeedQue = 1;

BOOL bUSB_Status = FALSE; //usb连接状态
int  gs32VolumeFd = -1;   //C盘的handle

U16 gu16ScreenPosX = 0,gu16ScreenPosY = 0;

//resource.bin load offset
S32  gs32BmpOffset = cSYSTEM_BMP_OFFSET;

RtcTime_st gtRtc_solar;

//#pragma arm section
/*-----------------------------------------------------------------------------
* 函数:	Hyc_crc32
* 功能:	计算32bit CRC校验结果
* 参数:	array----需要计算校验的buffer
*		size----buffer大小
* 返回:	32位CRC校验结果
*----------------------------------------------------------------------------*/
U32 Hyc_crc32(void *array,U32 size)
{
	unsigned long crc;
	CRC32_Generate(&crc,(unsigned char *)array,size);
	return crc;
} 


/*-----------------------------------------------------------------------------
* 函数:	glbVariable_CalculateCRC
* 功能:	计算CRC校验结果
* 参数:	pBuf----需要计算校验的buffer
*		size----buffer大小
* 返回:	16位CRC校验结果
*----------------------------------------------------------------------------*/
U16 glbVariable_CalculateCRC(U8 *pBuf,U32 size)
{
	U16 crc;
	
	for( crc = 0xffff; size-- ; )
	{
		crc = (U16)((crc >> 8) | (crc << 8));
		crc = (U16)(crc^(*pBuf++));
		crc = (U16)(crc^(U8)(crc & 0xff) >> 4);
		crc = (U16)(crc^(crc << 12));
		crc = (U16)(crc^((crc & 0xff) << 5));
	}
	return crc;
}

/*-----------------------------------------------------------------------------
* 函数:	glbVariable_CheckCRC
* 功能:	CRC校验
* 参数:	kAddress----待校验buffer
*		size--------buffer大小
* 返回:	16位CRC校验结果
*----------------------------------------------------------------------------*/
U8 glbVariable_CheckCRC(U8 * kAddress,U32 size)
{
	U16 *paddress,crc;
	U32 tempSize;
	
	tempSize = size -2;
	paddress  = ( U16 *)(kAddress  + tempSize );
	
	crc = glbVariable_CalculateCRC((U8 *)kAddress,tempSize);
	
	if ( (*paddress) == crc)
	{
		return CRC_OK;
	}
	else
	{
		return  CRC_ERR;
	}
}

/*-----------------------------------------------------------------------------
* 函数:	glb_SetPersistentParameter
* 功能:	保存的系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表到Sdram Parameter Area中
* 参数: source		--	指向调用者给出的缓存，即需要保存的数据
*		offset		--	相对persistent area的偏移量，以字节计
*		len			--	长度，以字节计
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameter( char* source, int offset, int len )
{
	memcpy((char*)((char *)hsaPersistentParameters_GetBuffer()+offset),source,len);	
	return cSSA_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	glb_GetPersistentParameter
* 功能:	从Sdram Parameter Area中获取系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表
* 参数: destination	--	指向调用者给出的缓存，获取保存的数据将存入此缓存
*		offset		--	相对persistent area的偏移量，以字节计
*		len			--	长度，以字节计
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameter( char* destination, int offset, int len )
{
	memcpy(destination,(char*)((char *)hsaPersistentParameters_GetBuffer()+offset),len);
	
	return cSSA_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	glb_GetPersistentParameterFromNand
* 功能:	从Nand PersistentArea中一次性获取系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表到Sdram Parameter Area
* 参数: none
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameterFromNand(void)
{
	int rc;
	
	Mutex_Protect();
	memset((char*)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	rc = HSA_GetPersistentParameter(0,(char*)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	Mutex_Unprotect();
	
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	glb_SetPersistentParameterToNand
* 功能:	将Sdram Parameter Area中系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表一次性写入到Nand PersistentArea
* 参数: none
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameterToNand(void)
{
	int rc = HY_OK;
	glb_HycParameter_t curPara;
		
	HSA_GetPersistentParameter(0,(char*)&curPara, 0, sizeof(glb_HycParameter_t));
	
	if(memcmp((U8 *)&curPara,(char *)(hsaPersistentParameters_GetBuffer()),sizeof(glb_HycParameter_t)) != 0)
	{
		rc = HSA_SetPersistentParameter(0,(char *)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	}

	return rc;
}

int LcdModule_Get_ShowType(void)
{
    return  gScreen_Display_Mode;
}

/*-----------------------------------------------------------------------------
* 函数:	GetResouceBmpOffset
* 功能:	获得资源在nand reserve区中的OffSet
* 参数:	none
* 返回:	>0----资源在nand reserve区中的OffSet
*		-1----nand reserve区中无资源
*----------------------------------------------------------------------------*/
S32 GetResouceBmpOffset(void)
{
	return gs32BmpOffset;
}

/*-----------------------------------------------------------------------------
* 函数:	SetResouceBmpOffset
* 功能:	设置资源在nand reserve区中的OffSet
* 参数:	offset
* 返回:	none
*----------------------------------------------------------------------------*/
void SetResouceBmpOffset(S32 offset)
{
	gs32BmpOffset = offset;
}

/*-----------------------------------------------------------------------------
* 函数:	lcdRefreshQue_get
* 功能:	获取队列刷屏使能/禁止状态
* 参数:	none
* 返回:	0:禁止队列刷屏，1:使能
*----------------------------------------------------------------------------*/
BOOL lcdRefreshQue_get(void)
{
	return gRefreshNeedQue;
}

/*-----------------------------------------------------------------------------
* 函数:	lcdRefreshQue_set
* 功能:	设置使能/禁止队列刷屏
* 参数:	enable-----0:禁止队列刷屏，1:使能
* 返回:	none
*----------------------------------------------------------------------------*/
void lcdRefreshQue_set(BOOL enable)
{
	gRefreshNeedQue = enable;
}

/*-----------------------------------------------------------------------------
* 函数:	rtcSolar_get
* 功能:	获得当前时间(阳历)
* 参数:	pTime
* 返回:	ds
*----------------------------------------------------------------------------*/
void rtcSolar_get(RtcTime_st *psolarTime)
{
	if (psolarTime == NULL) return;
	memcpy(psolarTime, &gtRtc_solar, sizeof(RtcTime_st));
}

