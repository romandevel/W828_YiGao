#ifndef _GLOBAL_VARIABLE_BASE_H_
#define _GLOBAL_VARIABLE_BASE_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/

#include "glbStatusDef.h"
#include "glbCmdDef.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "hyhwRtc.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/


#define cSYSTEM_BMP_OFFSET				0x00360000


//在Sdram上开一个缓存，开机时一次性读取Nand persistent area中全部保存的信息数据到此缓存
//开机过程中，获取或写入保存的信息数据都在此缓存中进行
//关机时，一次性写入Nand persistent area
	
#define cPARAMETER_TOTAL_SIZE		30720

//全局结构
#define cPARAMETER_OFFSET			0
#define cPARAMETER_SIZE				(sizeof(glb_HycParameter_t))

//备份的全局结构
#define cPARAMETER_BAK_OFFSET		(cPARAMETER_OFFSET+cPARAMETER_SIZE)
#define cPARAMETER_BAK_SIZE			(sizeof(glb_HycParameter_Bak_t))

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define OFFSET_UNICODE_FONT16X16     	0
#define OFFSET_GBK_FONT16X16         	0x00200000
#define OFFSET_KOREAN_FONT16X16      	0x002BD000
#define OFFSET_JAPAN_FONT16X16       	0x00377860

#define OFFSET_UNICODE_FONT24X24       	0x00431860
#define OFFSET_GBK_FONT24X24           	0x008B1860
#define OFFSET_KOREAN_FONT24X24        	0x00A5AC60
#define OFFSET_JAPAN_FONT24X24         	0x00BFE738

#define OFFSET_UNICODE_FONT32X32       	0x00DA0F38
#define OFFSET_GBK_FONT32X32           	0x015A0F38
#define OFFSET_KOREAN_FONT32X32        	0x01894F38
#define OFFSET_JAPAN_FONT32X32         	0x01B7F0B8

// BIT 7: REVERSE DISPLAY
// BIT 6: ENGLISH/CHINESE_CHARACTER
#define REVERSE_DISPLAY_BIT		0x80	// 0: Normal display; 1: reverse display
#define CHARACTER_TYPEBIT		0x40	// 0: English char; 1: Chinese char

#define FONTSIZE_BIT			0x0F	// 0: 8x16; 1: 6x7; 2: 6x6; 3: 6x5 ; 4: 9x16; 5: 4x4

//显示字符的来源
#define CHARACTER_SRC_MASK			0x7F00	//字符来源掩码
#define CHARACTER_SRC_PROGRAM		0x0000	//程序或同步歌词文件中字符
#define CHARACTER_SRC_FILENAME		0x0100	//从PSA提取的文件名
#define CHARACTER_SRC_MP3ID3		0x0200	//从PSA提取的MP3ID3信息
#define CHARACTER_SRC_WMAID3		0x0300	//从PSA提取的WMAID3信息
#define CHARACTER_SRC_LRCLOCAL		0x0400	//从LRC文件中得到的本地码字符
#define CHARACTER_SRC_LRCUNICODE	0x0500	//从LRC文件中得到的UNICODE字符

#define CHARACTER_NOT_UTF8			0x8000	//表明当前字符串不是UTF8编码字符串

#define FONTSIZE8X16		0x0
#define FONTSIZE6X7			0x1
#define FONTSIZE6X6			0x2
#define FONTSIZE6X5			0x3
#define FONTSIZE9X16		0x4
#define FONTSIZE4X4			0x5
#define FONTSIZE5X16_DDASH	0x6
#define	FONTSIZE5X8			0x7
#define FONTSIZE7X11        0x8
#define FONTSIZE12X17       0x9
#define FONTSIZE6X9         0xA
#define FONTSIZE9x7			0xB	//从右边到左边
#define FONTSIZE9x7_LR		0xC	//从左边到右边
#define FONTSIZE5X7			0xD

#define FONTSIZE24X24		0xE
#define FONTSIZE32X32		0xF


#define cLCD_16BIT_COLOR
//#define cLCD_18BIT_COLOR


#ifdef cLCD_16BIT_COLOR
	// define color pattern  65k color 16 bits
	#define  COLOR_RED			0xF800
	#define  COLOR_GREEN		0x07E0
	#define  COLOR_BLUE			0x001F
	#define  COLOR_WHITE		0xFFFF
	#define  COLOR_BLACK		0x0000
	#define  COLOR_YELLOW		0xFFE0
	#define	 COLOR_CYAN			0x07FF
	#define  COLOR_BACKGND0X0005	0x0005
	#define  COLOR_BACKGND0X0138	0x0138
	#define  COLOR_200225250	0xCB9F
	#define  COLOR_255075000	0xF920
	#define  COLOR_BATRED		0xD100
	#define  COLOR_BATYELLOW	0xEF40
	#define  COLOR_BATGREEN		0x0660
	#define  COLOR_000000085	0x55
	#define  COLOR_085162252	0xAA9F
	#define  COLOR_126168255	0x7abf

	//后续添加	EVA
	#define  COLOR_ORANGE		0xFA80
	#define  COLOR_CORAL		0xF9EA
	#define  COLOR_BRASS		0xB280
	#define  COLOR_SILVER		0xC318
	#define  COLOR_MAGENTA		0xF81F

	#define PERSPECTIVE_DISPLAY_BIT		0x80//透明图标志
#endif //#ifdef cLCD_16BIT_COLOR


#ifdef cLCD_18BIT_COLOR
	#define  COLOR_RED   	        0x3f000
	
	#define  COLOR_GREEN   	        0x0fc0
	#define  COLOR_BLUE             0x003f
	#define  COLOR_BLACK            0x00000000
	#define  COLOR_WHITE            0x3ffff

    #define  COLOR_YELLOW		    0x3ffc0
    
#endif

#define c1POINT_OCCUPY_BYTES		2//每个颜色点占用字节数

#define cLCD_TOTAL_ROW				320
#define cLCD_TOTAL_COLUMN			240


//锁屏状态
#define CLS_STATUS_UNLOCK     	0  	//没有锁屏 
#define CLS_STATUS_INTIAL     	1
#define CLS_STATUS_LOCKING    	2
#define CLS_STATUS_LOCKED     	3  	//机械锁屏
#define CLS_STATUS_UNLOCKING	4	//解锁中


//屏保时间
#define cSCREENPROTECT_DEFAULT	30//10//10秒 MEI
#define cSCREENPROTECT_1MIN	    60//60//20//20s
#define cSCREENPROTECT_5MIN	    300//300//40//40s
#define cSCREENPROTECT_10MIN	600//600//60//60s
#define cSCREENPROTECT_CLOSE	0//0


/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
typedef enum //_Character_Type_e
{
	CHARACTER_LOCALCODE=0,
	CHARACTER_UNICODE,
	CHARACTER_UTF8
	
}Character_Type_e;



//USB连接状态 变量: gUsbConnectStatus
typedef enum _USB_ConnectStatus_e
{
	USB_Disconnected = 0,
	USB_Connect_Init,
	USB_Connecting,
	USB_Suspend_Init,
	USB_Charging,
	USB_PowerDown
	//USB_Disconnect_Init

} USB_ConnectStatus_e,* pUSB_ConnectStatus_e;


/*--------------------------------------------------------------------------
* external Variable ,constant or function
--------------------------------------------------------------------------*/
extern U16  gLcd_Total_Column;
extern U16  gLcd_Total_Row;
// 为了在某些产品上正确地显示，如在各个模式下，显示的界限不一样
extern U16 gu16Lcd_ColumnLimit;

extern U16 gFrontColor;
extern U16 gBackColor;

//resource.bin load offset
extern S32 gs32BmpOffset;
extern BOOL gRefreshNeedQue;

extern U8 gu8EnableRefreshSrc; //是否允许刷屏

extern BOOL bUSB_Status;
extern int  gs32VolumeFd;

extern U16 gu16ScreenPosX,gu16ScreenPosY;

extern U8 gScreen_Display_Mode;

extern RtcTime_st gtRtc_solar;
/*-----------------------------------------------------------------------------
* 函数:	Hyc_crc32
* 功能:	计算32bit CRC校验结果
* 参数:	array----需要计算校验的buffer
*		size----buffer大小
* 返回:	32位CRC校验结果
*----------------------------------------------------------------------------*/
U32 Hyc_crc32(void *array,U32 size);


/*-----------------------------------------------------------------------------
* 函数:	glb_SetPersistentParameter
* 功能:	保存的系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表到Sdram Parameter Area中
* 参数: source		--	指向调用者给出的缓存，即需要保存的数据
*		index		--	相对persistent area的偏移量，以字节计
*		len			--	长度，以字节计
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameter( char* source, int index, int len );

/*-----------------------------------------------------------------------------
* 函数:	glb_GetPersistentParameter
* 功能:	从Sdram Parameter Area中获取系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表
* 参数: destination	--	指向调用者给出的缓存，获取保存的数据将存入此缓存
*		index		--	相对persistent area的偏移量，以字节计
*		len			--	长度，以字节计
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameter( char* destination, int index, int len );

/*-----------------------------------------------------------------------------
* 函数:	glb_GetPersistentParameterFromNand
* 功能:	从Nand PersistentArea中一次性获取系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表到Sdram Parameter Area
* 参数: none
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameterFromNand(void);

/*-----------------------------------------------------------------------------
* 函数:	glb_SetPersistentParameterToNand
* 功能:	将Sdram Parameter Area中系统变量、目录信息，电子书信息，Favorite信息，录音文件序号信息，
*		音频视频文件的Map表一次性写入到Nand PersistentArea
* 参数: none
* 返回:	错误码
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameterToNand(void);


/*-----------------------------------------------------------------------------
* 函数:	GetResouceBmpOffset
* 功能:	获得资源在nand reserve区中的OffSet
* 参数:	none
* 返回:	>0----资源在nand reserve区中的OffSet
*		-1----nand reserve区中无资源
*----------------------------------------------------------------------------*/
S32 GetResouceBmpOffset(void);

/*-----------------------------------------------------------------------------
* 函数:	SetResouceBmpOffset
* 功能:	设置资源在nand reserve区中的OffSet
* 参数:	offset
* 返回:	none
*----------------------------------------------------------------------------*/
void SetResouceBmpOffset(S32 offset);


#ifdef __cplusplus
}
#endif

#endif //_GLOBAL_VARIABLE_BASE_H_
