
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
/*------------------------------------------------------------------------------
   Project include files:
   ---------------------------------------------------------------------------*/
#include "hyhwGpio.h"

#include "BoardSupportPackage.h"


/*------------------------------------------------------------------------------
   Defines : other configurations
   ---------------------------------------------------------------------------*/

#define USB_INFO_STRING			0x00, 0x80, 0x00, 0x01, 0x1f, 0x00, 0x00, 0x00,\
								'H',  'y',  'c',  'o',  ' ',  ' ',  ' ',  ' ', \
								'D',  'e',  'v',  'i',  'c',  'e',  ' ',  ' ', \
								' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ', \
								' ',  ' ',  ' ',  ' '

//NandFlash写保护GPIO
#define NAND_WRITE_PROTECT			0 //GPIO_5 GPIO口已经被占用 Nand写保护置为NULL

//SD/MMC卡检测GPIO，及有效电平
#define SD_MMC_CARD_DETECT_PORT		0
#define SD_MMC_CARD_DETECT_GPIO		0
#define SD_MMC_CARD_DETECT_LEVEL	0//1		//0 表示低电平有效

//SD/MMC卡写保护检测GPIO,及有效电平
#define SD_MMC_CARD_WRITE_PROTECT_PORT		0
#define SD_MMC_CARD_WRITE_PROTECT_GPIO		0
#define SD_MMC_CARD_WRITE_PROTECT_LEVEL		0  //0 表示低电平有效

/*------------------------------------------------------------------------------
   Local Types:
   ---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Global variables : storage media
   ---------------------------------------------------------------------------*/


//#ifdef HYHW_MEMORY_SD_MMC
const SdmmcStorageMedia_st SdmmcStorageMedia	=
{
	SD_MMC_CARD_DETECT_PORT,				/* detect port */
	SD_MMC_CARD_DETECT_GPIO,				/* detect gpio */
	SD_MMC_CARD_DETECT_LEVEL,				/* detect gpio 有效电平, 0 表示低电平有效 */			

	SD_MMC_CARD_WRITE_PROTECT_PORT,			/* protect port */
	SD_MMC_CARD_WRITE_PROTECT_GPIO,			/* protect gpio */
	SD_MMC_CARD_WRITE_PROTECT_LEVEL	/* protect gpio 有效电平*/
};
//#endif

/*------------------------------------------------------------------------------
   Global variables : USB configuration
   ---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Global variables : USB configuration      */
/* SCSI STANDARD INQUIRY DATA                */
/*  only following bytes are configurable :  */
/*      byte 7..15  : vendor identification  */
/*      byte 16..31 : product identification */
/*      byte 32..35 : product revision level */
/*  all other bytes should not be modified   */
/* Standard inquiry data */
const unsigned char gScsiApplDeviceIdentification[36] =
{
	USB_INFO_STRING
};

/*------------------------------------------------------------------------------
   Global variables : upload protection
   ---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Global variables : other variables
   ---------------------------------------------------------------------------*/

