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

/**
 * \addtogroup tmllpeUsbScsiAppln
 * @{
 */

/** 
 * @file
 * @brief The peripheral driver layer for the Scsi application implementation.
 *
 * This is the peripheral driver layer for the Scsi application implementation and private interface.
 * 
 */

/*----------------------------------------------------------------------------
   Standard include files:
   These should always be included !
   --------------------------------------------------------------------------*/
//#include "hyOsCpuDef.h"  
#include "hyTypes.h"
#include "hyErrors.h"


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
#include "hyudsUsbScsi.h"


//#define HYHW_MEMORY_SD_MMC

#ifdef	TEST_U_DISK_NANDFLASH_ONLY
	#include "hyswNandflash.h"
#endif

#ifdef	HYHW_MEMORY_SD_MMC
	#include "hyhwSdMmc.h"
	#include "hyswSdMmc.h"
#endif

#include "BoardSupportPackage.h"

//#define SDMMC_UDISK_DEBUG

//#pragma O0
//#pragma arm section rodata="usb_function", code="usb_function", zidata="usb_zidata", rwdata="usb_zidata"
//#pragma O0
#pragma arm section zidata="usb_zidata", rwdata="usb_zidata"

extern UInt8 sUsbScsiTransmitLun;

/*---------------------------------------------------------------------------
   Local Types and defines:
   --------------------------------------------------------------------------*/

/**

 * @brief Scsi application driver request sense data structure.
 *
 * This structure defines the most essential fields of the Scsi Sense data format.
 * This data is filled in on reception of a Scsi command and is used to create the Sense data format
 * that is sent as an answer to a Scsi Request Sense command.
 * - senseKey : sense key field in Scsi Sense data format
 * - additionalSenseCode : additional sense code field in Scsi Sense data format
 * - additionalSenseCodeQualifier : additional sense code qualifier field in Scsi Sense data format
 *
 * @see tmllpeScsiAppln_TestUnitReadyScsiCommand, tmllpeScsiAppln_FormatUnitScsiCommand,
 * @see tmllpeScsiAppln_InquiryScsiCommand, tmllpeScsiAppln_StartStopUnitScsiCommand,                                  
 * @see tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand, tmllpeScsiAppln_ReadCapacityScsiCommand,                                  
 * @see tmllpeScsiAppln_Write10ScsiCommand, tmllpeScsiAppln_Read10ScsiCommand,                                  
 * @see tmllpeScsiAppln_Verify10ScsiCommand, tmllpeScsiAppln_SyncCacheScsiCommand,                                  
 * @see tmllpeScsiAppln_WriteBufferScsiCommand, tmllpeScsiAppln_RequestSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ModeSelectScsiCommand, tmllpeScsiAppln_ModeSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand                                  
 */
//该结构修改于20070827晚，具体含义不知道，所以直接使用reserve表示。
//在scsi中并没有关于这部分的描述，我是参考普通读卡器做的。
typedef struct tmllpeScsiApplnSenseData_st_t
{
    UInt8 senseKey;
    UInt8 additionalSenseCode;
    //UInt8 additionalSenseCodeQualifier;
    UInt8 Reserve1;
    UInt8 Reserve11;
    UInt8 Reserve14;
    
} tmllpeScsiApplnSenseData_st_t, * ptmllpeScsiApplnSenseData_st_t;


/*	支持的最多的盘符个数 
	在程序中，会检测SD卡，如果没有SD卡，则这个数字要减1 再返回 */
#define TMLLPE_SCSIAPPLN_MAX_LUN_VALUE    2     /* limited to 1 LUN */       
#define SD_LUN		(1)


/**
 * @brief Scsi application driver sense key/additional sense code/additional sense code qualifier defines.
 *
 * These defines define the needed sense key/additional sense code/additional sense code qualifier values 
 * for this type of scsi device.    
 *
 * @see tmllpeScsiAppln_TestUnitReadyScsiCommand, tmllpeScsiAppln_FormatUnitScsiCommand,
 * @see tmllpeScsiAppln_InquiryScsiCommand, tmllpeScsiAppln_StartStopUnitScsiCommand,                                  
 * @see tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand, tmllpeScsiAppln_ReadCapacityScsiCommand,                                  
 * @see tmllpeScsiAppln_Write10ScsiCommand, tmllpeScsiAppln_Read10ScsiCommand,                                  
 * @see tmllpeScsiAppln_Verify10ScsiCommand, tmllpeScsiAppln_SyncCacheScsiCommand,                                  
 * @see tmllpeScsiAppln_WriteBufferScsiCommand, tmllpeScsiAppln_RequestSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ModeSelectScsiCommand, tmllpeScsiAppln_ModeSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand                                  
 */
#define TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY             0x00
#define TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY            0x02
#define TMLLPE_SCSIAPPLN_MEDIUM_ERROR_SENSE_KEY         0x03
#define TMLLPE_SCSIAPPLN_ILLEGAL_REQUEST_SENSE_KEY      0x05
#define TMLLPE_SCSIAPPLN_UNIT_ATTENTION_SENSE_KEY       0x06
#define TMLLPE_SCSIAPPLN_DATA_PROTECT_SENSE_KEY         0x07
#define TMLLPE_SCSIAPPLN_NO_SENSE_ASC                   0x00
#define TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC         0x3A
#define TMLLPE_SCSIAPPLN_ILLEGAL_REQUEST_SENSE_KEY_ASC  0x24
#define TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ                  0x00
#define TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASCQ        0x00
#define TMLLPE_SCSIAPPLN_MEDIUM_MEDIA_CHANGED_SENSE_KEY 0x06
#define TMLLPE_SCSIAPPLN_MEDIUM_MEDIA_CHANGED_ASC       0x28


/**
 * @brief Scsi application driver eject state defines.
 *
 * These defines define the eject states, being set by the Scsi Start Stop Unit command.    
 *
 * @see tmllpeScsiAppln_TestUnitReadyScsiCommand, tmllpeScsiAppln_FormatUnitScsiCommand,
 * @see tmllpeScsiAppln_InquiryScsiCommand, tmllpeScsiAppln_StartStopUnitScsiCommand,                                  
 * @see tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand, tmllpeScsiAppln_ReadCapacityScsiCommand,                                  
 * @see tmllpeScsiAppln_Write10ScsiCommand, tmllpeScsiAppln_Read10ScsiCommand,                                  
 * @see tmllpeScsiAppln_Verify10ScsiCommand, tmllpeScsiAppln_SyncCacheScsiCommand,                                  
 * @see tmllpeScsiAppln_WriteBufferScsiCommand, tmllpeScsiAppln_RequestSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ModeSelectScsiCommand, tmllpeScsiAppln_ModeSenseScsiCommand,                                  
 * @see tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand                                  
 */
#define TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY       0x00
#define TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REQUEST      0x01
#define TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED     0x02
#define TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_NOT_READY   0x03

/**
 * @Brief Scsi application driver Vital Product Data (VPD) page codes defines.
 *
 * These defines define the Vital Product Data (VPD) page codes for the inquiry command.
 */
#define TMLLPE_SCSIAPPLN_VPD_PAGE_SUPPORTED_VPD_PAGES    0x00
#define TMLLPE_SCSIAPPLN_VPD_PAGE_UNIT_SERIAL_NUMBER     0x80
#define TMLLPE_SCSIAPPLN_VPD_PAGE_DEVICE_IDENTIFICATION  0x83   


/**
 * @Brief Scsi application driver Inquiry command data type defines.
 *
 * These defines define the to be returned Inquiry data (byte 1 of Inquiry command).
 *
 */
#define TMLLPE_SCSIAPPLN_INQUIRY_STANDARD_INQUIRY_DATA  0x00
#define TMLLPE_SCSIAPPLN_INQUIRY_VITAL_PRODUCT_DATA     0x01
#define TMLLPE_SCSIAPPLN_INQUIRY_COMMAND_SUPPORT_DATA   0x02
  
#define SD_RESERVE_BLOCKS		(1000)//保留block的数目
 
/*---------------------------------------------------------------------------
   Global variables:
   --------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   Static Variables:
   --------------------------------------------------------------------------*/
/**
 * @brief Scsi application driver Scsi block size.
 *
 * This static variable represents the Scsi block size.
 */
static UInt32 sScsiApplnScsiBlockSize[2];

/**
 * @brief Scsi application driver request sense data.
 *
 * This static array represents the request sense data.
 */
//该结构修改于20070827晚，具体含义不知道，所以直接使用reserve表示。
//在scsi中并没有关于这部分的描述，我是参考普通读卡器做的。
static UInt8 sScsiApplnReqSense[] =
{
    0xf0, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0a,
    0x00, 0xaa, 0x55, 0x40, 0x3a, 0x00, 0x00, 0x00,
    0x00, 0x00
};


/**
 * @brief Scsi application driver request sense data.
 *
 * This static array represents the request sense data.
 */
static const UInt8 sScsiApplnModeSense[] =
{
    0x03, 0x00, 0x00, 0x00
};


/**
 * @brief Scsi application driver read capacity data.
 *
 * This static array represents the read capacity data.
 * It assumes a 512 bytes block size and 8MB total disk size.
 */
static UInt8 sScsiApplnReadCapacity[2][8] = 
{	
	0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x02, 0x00   
};


/**
 * @brief Scsi application driver Scsi Sense data information.
 *
 * This static structure holds Scsi Sense data information.
 */
static tmllpeScsiApplnSenseData_st_t sScsiApplnSenseData;


#if 0
/**
 * @brief Scsi application driver inquiry data.
 *
 * This static array represents the inquiry data.
 *  only following bytes are configurable:
 *      - byte 7..15  : vendor identification
 *      - byte 16..31 : product identification
 *      - byte 32..35 : product revision level
 *  size should be equal to 36 
 */
static UInt8 gScsiApplInquiryStandardData[36] =
{
    0x00, 0x80, 0x00, 0x01, 0x1f, 0x00, 0x00, 0x00,
    'S',  'h',  'a',  'r',  'k',  '0',  '1',  ' ',  /* vendor id   : "shark" */
    'M',  'a',  's',  's',  'S',  't',  'o',  'r',  /* product id  : "Mass Storage Disc" */
    'a',  'g',  'e',  ' ',  'D',  'i',  's',  'c',
    ' ',  ' ',  ' ',  ' '
};
#endif

/* SCSI UNIT SERIAL NUMBER PAGE                         */
/*      byte 0     : peripheral qualifier + device type */
/*      byte 1     : page code (80h)                    */
/*      byte 2     : reserved                           */
/*      byte 3     : serial number length               */
/*      byte 4..35 : serial number                      */
const UInt8 gScsiApplUnitSerialNumber[SCSI_INQUIRY_DATA_LENGTH_UNIT_SERIAL_NUMBER] =
{
    0x00, 0x80, 0x00, 0x10, 
    0x35, 0x30, 0x39, 0x35, 0x4d, 0x33, 0x54, 0x31, /* ARBITRARY unit serial number  */
    0x30, 0x30, 0x35, 0x30, 0x30, 0x39, 0x35, 0x54  /* ARBITRARY unit serial number  */
};

/* pointer to start of Memory Disc */
static UInt8 *sScsiApplnVirtualMemory = (UInt8 *)0xa00;


/**
 * @brief Scsi application driver eject state.
 *
 * This static structure holds the eject state.
 */
static UInt8 sScsiApplnEjectState[2];

/*	原来的bug, windows不能完成格式化， 原因是由于windows格式化的内容和下位机的不一样
解决方案，让windows完成格式化后，下位机再做一次格式化
tmllpeScsiAppln.c  （20050930 -> 20051110）
原全局变量uint32 gtmStandardFormatInitiated 仅在usbhandler_p中使用， 增加1个，改为以下两个
uint32 gtmStandardFormatInitiated  在usbhandler_p 中用法同前
static uint32 gtmWindowsFormatStarted  在tmllpeScsiAppln.c 中表示windows开始格式化了，当完成后，才开始下位机的格式化
*/

#if 0  // 20061011， 让windows 格式化 
Int32	gtmStandardFormatInitiated = False;
static	Int32 gtmWindowsFormatStarted  = False;
#endif


/**
 * @brief Scsi application Usb receive transfer status.
 *
 * This global variable is set when data is written to the device.
 */
Int8 gtmUsbTransferInProgressRx = False;

/**
 * @brief Scsi application Usb transmit transfer status.
 *
 * This global variable is set when data is read from the device.
 */

Int8 gtmUsbTransferInProgressTx = False;

//用于解决windows2000下sd无法热插拔问题
static U8 gtmSdTestUnitReady;

Int8 gtmUsbMaxLunValue;

/*---------------------------------------------------------------------------
   Static Function protoypes:
   --------------------------------------------------------------------------*/
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_TestUnitReadyScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_FormatUnitScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_InquiryScsiCommand(ptmllpeScsiInquiry_st_t pScsiInquiry, ptmllpeScsiResponseData_st_t pScsiResponseData);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_StartStopUnitScsiCommand(ptmllpeScsiStartStopUnit_st_t pScsiStartStopUnit);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadCapacityScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Write10ScsiCommand(ptmllpeScsiWriteRead_st_t pScsiWriteRead);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Read10ScsiCommand(ptmllpeScsiWriteRead_st_t pScsiWriteRead);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Verify10ScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_SyncCacheScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_WriteBufferScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_RequestSenseScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ModeSelectScsiCommand(void);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ModeSenseScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData);
static void tmllpeScsiAppln_MassStorageResetUsbRequest(void);
static void tmllpeScsiAppln_GetMaxLunUsbRequest(ptmllpeScsiGetMaxLun_st_t pScsiGetMaxLun);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_WriteBlockDataTransfer(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess);
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadBlockDataTransfer(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess);




static Int32 sScsiApplnOpenVolumeHandle;

/* 磁盘的信息，开机读出 */
static UInt8 sScsiApplnBootSector[60];

/**
 * @Brief Scsi application driver filesystem type defines.
 *
 * These defines define the filesystem types.
 *
 * @see tmllpeScsiAppln_Initialise
 */
#define TMLLPE_SCSIAPPLN_FAT12                    0x01        /* FAT12 File System */
#define TMLLPE_SCSIAPPLN_FAT16                    0x02        /* FAT16 File System */



/*---------------------------------------------------------------------------
   Exported functions:
   --------------------------------------------------------------------------*/
   
/*---------------------------------------------------------------------------
   tmllpeScsiAppln_Initialise
   --------------------------------------------------------------------------*/
/*	usb的传输数据的buffer 有8k，在初始化时临时可复用一下 */
extern UInt8 sUsbScsiDataBuffer[];  

UInt32 totalSectors[2] ;
static Int32 sectorsPerFat[2], clusterSize[2], fileSystemType[2], sectorsPerCluster[2],sectorSize[2];
 
static Bool SdCardEject; //用于记录SD卡是否被弹出

static U8 SdBuf[512];

void tmllpeScsiAppln_SDInitialise(Int32 volumeHandle)
{
#if defined(HYHW_MEMORY_SD_MMC)
    tmllpeScsiRegister_st_t     scsiRegister;
    U8 i,j;

    
    gtmSdTestUnitReady = FALSE;
     /* Go to initial position */
	for(i=1;i<2;i++)
    {
    	
    	if(i == 0)
    	{
    		  
    		
    	}
    	else
    	{
    		U32 errorStatus;
    		
    		//RDA_ReadWriteSeekParameters_st SdMmcOper;
    		
    		
			totalSectors[1]		= 0;
			sectorSize[1]		= 0; 
    		
			sScsiApplnScsiBlockSize[1] = 512;//default value
			errorStatus = hyswSdMmc_Init();
			if(errorStatus != HY_OK)
			{
				break;
			}
			else
			{
				errorStatus = hyswSdMmc_Read_Usb(0, (U8*)SdBuf, 1);
			}
			if(errorStatus != HY_OK)
			{
				gtmUsbMaxLunValue = 1;
				break;
			}
			 
    	}
    	
	    /* sector size */
	    sectorSize[i] = ((UInt32) SdBuf[0x0B]) + (((UInt32) SdBuf[0x0C]) << 8);
	  
		if( ( SdBuf[0x13] ) || ( SdBuf[0x14] ) )
	    {
	        totalSectors[i] = ((UInt32) SdBuf[0x13]) + (((UInt32) SdBuf[0x14]) << 8);
	    }
	    else
	    {
	        totalSectors[i] = ((UInt32) SdBuf[0x20]) + (((UInt32) SdBuf[0x21]) << 8) +
	            (((UInt32) SdBuf[0x22]) << 16) + (((UInt32) SdBuf[0x23]) << 24);
	    }
	   
	    /* cluster size */
	    sectorsPerCluster[i] = (UInt32) SdBuf[0x0D];
	    clusterSize[i] = sectorsPerCluster[i] * sectorSize[i];
	    
#if 0 	    
	    
	    {
	    	int divBits;
	    	
	    	/* 避免除法，sectorsPerCluster肯定是2的幂，求出指数, arm926ej dsp 指令 */
	    	__asm
	    	{
	    		CLZ  divBits, sectorsPerCluster[i]
	    	}	
	    	
	    	divBits = 31 - divBits;
	    
	    
		    /* FAT type */
		    if( sScsiApplnBootSector[0x3A] == '2' )
		    {
		        fileSystemType[i]	= TMLLPE_SCSIAPPLN_FAT12;
		        //sectorsPerFat	= (totalSectors / sectorsPerCluster);
		        sectorsPerFat[i]	= (totalSectors[i] >>divBits);
		        sectorsPerFat[i]	= ((3 * (sectorsPerFat[i] >> 1)) - 1);
		        
		    }
		    else
		    {
		        fileSystemType[i] = TMLLPE_SCSIAPPLN_FAT16;
		        //sectorsPerFat	= (totalSectors / sectorsPerCluster);
		        sectorsPerFat[i]	= (totalSectors[i] >>divBits);
		        sectorsPerFat[i]	= ((2 * sectorsPerFat[i]) - 1);
		    }
		    
		    
	    	/* 避免除法，sectorSize肯定是2的幂，求出指数, arm926ej dsp 指令 */
	    	__asm
	    	{
	    		CLZ  divBits, sectorSize[i]
	    	}	
	    	
	    	divBits = 31 - divBits;
		    
	 	    //sectorsPerFat	= (sectorsPerFat / sectorSize) + 1;
	 	    sectorsPerFat[i]	= (sectorsPerFat[i] >> divBits) + 1;
	   
	    }
	    
#else
	    /* FAT type */
	    if( SdBuf[0x3A] == '2' )
	    {
	        fileSystemType[i]	= TMLLPE_SCSIAPPLN_FAT12;
	    }
	    else
	    {
	        fileSystemType[i]	= TMLLPE_SCSIAPPLN_FAT16;
	    }
	    sectorsPerFat[i] = SdBuf[22] + (SdBuf[23] <<8);

#endif	    
	    
	    /* Scsi block size equals filesystem sectorsize,  得到每个sectorSize的大小*/
    	sScsiApplnScsiBlockSize[i] = sectorSize[i];
	}	
    
    
	{//gao 2007-08-28 当SD卡容量没有保留的大，则返回为0
		int tempData;
		tempData = (int)(middlewareMCI_GetSDCapacity()/512 - SD_RESERVE_BLOCKS);
		if(tempData>0)
			totalSectors[1] = (U32)tempData; 
		else
			totalSectors[1] = 0;
	}
	sectorSize[1] = 512;
	sScsiApplnScsiBlockSize[1] = 512;
   
    sScsiApplnReadCapacity[1][0x00] = ((totalSectors[1] & 0xFF000000)>>24);
    sScsiApplnReadCapacity[1][0x01] = ((totalSectors[1] & 0x00FF0000)>>16);
    sScsiApplnReadCapacity[1][0x02] = ((totalSectors[1] & 0x0000FF00)>>8);
    sScsiApplnReadCapacity[1][0x03] = ((totalSectors[1] & 0x000000FF));
    sScsiApplnReadCapacity[1][0x04] = ((sectorSize[1] & 0xFF000000)>>24);
    sScsiApplnReadCapacity[1][0x05] = ((sectorSize[1] & 0x00FF0000)>>16);
    sScsiApplnReadCapacity[1][0x06] = ((sectorSize[1] & 0x0000FF00)>>8);
    sScsiApplnReadCapacity[1][0x07] = ((sectorSize[1] & 0x000000FF));
	
#endif	//#if defined(HYHW_MEMORY_SD_MMC)
	
    return;
}



void tmllpeScsiAppln_Initialise(Int32 volumeHandle)
{
    tmllpeScsiRegister_st_t     scsiRegister;
    U8 i,j;
	
	SdCardEject = FALSE;
	gtmSdTestUnitReady = FALSE;
    /* Initialise some variables */
     sScsiApplnEjectState[0] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;

#if defined(HYHW_MEMORY_SD_MMC)
	/* 需要每次检测是否卡还在 */
#ifdef SDMMC_UDISK_DEBUG
	sScsiApplnEjectState[1] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;
#else
	sScsiApplnEjectState[1] = hyhwCardDetect()== True? TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY: TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_NOT_READY;
#endif
#endif     
    
    
#if 0  
    gtmStandardFormatInitiated = False;
    gtmWindowsFormatStarted  = False;
#endif
    
    gtmUsbTransferInProgressRx = False;
    gtmUsbTransferInProgressTx = False;
    
    USBHandler_SetScsiFlag(False);
    
     /* Go to initial position */
#if defined(HYHW_MEMORY_SD_MMC)
	for(i=0;i<2;i++)
#else
	for(i=0;i<1;i++)
#endif	
    {
    	
    	if(i == 0)
    	{
    		/* 暂时使用一下usb的接受buffer  有8k */
    		//hyswNandFlash_DirectRead1K( (Int8 *) sUsbScsiDataBuffer , 0);
    		hyswNandFlash_Read( (Int8 *) sUsbScsiDataBuffer , 0, 0, 1024);
			for(j=0;j<60;j++)
			{
				sScsiApplnBootSector[j] = sUsbScsiDataBuffer[j];
			}   
    		gtmUsbMaxLunValue = 1;
    	}
#ifdef	HYHW_MEMORY_SD_MMC
    	else
    	{
    		U32 errorStatus;
    		
    		//RDA_ReadWriteSeekParameters_st SdMmcOper;
    		
    		
			totalSectors[1]		= 0;
			sectorSize[1]		= 0; 
    		
    		sScsiApplnScsiBlockSize[1] = 512;//default value
			errorStatus = hyswSdMmc_Init();
			if(errorStatus != HY_OK)
			{
				break;
			}
			else
			{
				errorStatus = hyswSdMmc_Read_Usb(0, (U8*)(&sUsbScsiDataBuffer[0] + 2048), 1);
			}
			if(errorStatus != HY_OK)
			{
				break;
			}
			gtmUsbMaxLunValue = 2;
			
			for(j=0;j<60;j++)
			{
				sScsiApplnBootSector[j] = sUsbScsiDataBuffer[2048 + j];
			}   
			
    	}
 #endif   	
		
	    /* sector size */
	    sectorSize[i] = ((UInt32) sScsiApplnBootSector[0x0B]) + (((UInt32) sScsiApplnBootSector[0x0C]) << 8);
	  
		if( ( sScsiApplnBootSector[0x13] ) || ( sScsiApplnBootSector[0x14] ) )
	    {
	        totalSectors[i] = ((UInt32) sScsiApplnBootSector[0x13]) + (((UInt32) sScsiApplnBootSector[0x14]) << 8);
	    }
	    else
	    {
	        totalSectors[i] = ((UInt32) sScsiApplnBootSector[0x20]) + (((UInt32) sScsiApplnBootSector[0x21]) << 8) +
	            (((UInt32) sScsiApplnBootSector[0x22]) << 16) + (((UInt32) sScsiApplnBootSector[0x23]) << 24);
	    }
	   
	    /* cluster size */
	    sectorsPerCluster[i] = (UInt32) sScsiApplnBootSector[0x0D];
	    clusterSize[i] = sectorsPerCluster[i] * sectorSize[i];
	    
#if 0 	    
	    
#else
	    /* FAT type */
	    if( sScsiApplnBootSector[0x3A] == '2' )
	    {
	        fileSystemType[i]	= TMLLPE_SCSIAPPLN_FAT12;
	    }
	    else
	    {
	        fileSystemType[i]	= TMLLPE_SCSIAPPLN_FAT16;
	    }
	    sectorsPerFat[i] = sScsiApplnBootSector[22] + (sScsiApplnBootSector[23] <<8);

#endif	    
	    
	    /* Scsi block size equals filesystem sectorsize,  得到每个sectorSize的大小*/
    	sScsiApplnScsiBlockSize[i] = sectorSize[i];
	}	
    
    /* intialise the read capacity data *///--tian --初始化SCSI协议,记录存储器容量
    //windows 要给logic volume后端写数据，大概要占用一个sector，
    //所以，在scsi中，给windows返回的容量应减去一个sector。//2007-5-11 09:39上午 周洁
    if (totalSectors[0] > 0) totalSectors[0] = totalSectors[0] - 1;
    
    sScsiApplnReadCapacity[0][0x00] = ((totalSectors[0] & 0xFF000000)>>24);
    sScsiApplnReadCapacity[0][0x01] = ((totalSectors[0] & 0x00FF0000)>>16);
    sScsiApplnReadCapacity[0][0x02] = ((totalSectors[0] & 0x0000FF00)>>8);
    sScsiApplnReadCapacity[0][0x03] = ((totalSectors[0] & 0x000000FF));
    sScsiApplnReadCapacity[0][0x04] = ((sectorSize[0] & 0xFF000000)>>24);
    sScsiApplnReadCapacity[0][0x05] = ((sectorSize[0] & 0x00FF0000)>>16);
    sScsiApplnReadCapacity[0][0x06] = ((sectorSize[0] & 0x0000FF00)>>8);
    sScsiApplnReadCapacity[0][0x07] = ((sectorSize[0] & 0x000000FF));

#ifdef	HYHW_MEMORY_SD_MMC
	if (hyhwCardDetect() == True)
	{
		{//gao 2007-08-28 当SD卡容量没有保留的大，则返回为0
			int tempData;
			tempData = (int)(middlewareMCI_GetSDCapacity()/512 - SD_RESERVE_BLOCKS);
			if(tempData>0)
				totalSectors[1] = (U32)tempData; 
			else
				totalSectors[1] = 0;
		}
		sectorSize[1] = 512;
		sScsiApplnScsiBlockSize[1] = 512;
		sScsiApplnReadCapacity[1][0x00] = ((totalSectors[1] & 0xFF000000)>>24);
	    sScsiApplnReadCapacity[1][0x01] = ((totalSectors[1] & 0x00FF0000)>>16);
	    sScsiApplnReadCapacity[1][0x02] = ((totalSectors[1] & 0x0000FF00)>>8);
	    sScsiApplnReadCapacity[1][0x03] = ((totalSectors[1] & 0x000000FF));
	    sScsiApplnReadCapacity[1][0x04] = ((sectorSize[1] & 0xFF000000)>>24);
	    sScsiApplnReadCapacity[1][0x05] = ((sectorSize[1] & 0x00FF0000)>>16);
	    sScsiApplnReadCapacity[1][0x06] = ((sectorSize[1] & 0x0000FF00)>>8);
	    sScsiApplnReadCapacity[1][0x07] = ((sectorSize[1] & 0x000000FF));
    }
#endif
	
    /* register storage medium's block size and callback functions */
    /* to Scsi application driver */
    scsiRegister.mediumBlockLength[0] = sScsiApplnScsiBlockSize[0];
#ifdef	HYHW_MEMORY_SD_MMC    
    scsiRegister.mediumBlockLength[1] = sScsiApplnScsiBlockSize[1];
#endif 
   
    //此处endpoint 应该是逻辑端口号，对tmudsMsBot_ClassInit有很影响。
    scsiRegister.bulkOutEndpoint = 2;
    scsiRegister.bulkInEndpoint = 3;
    
    
    scsiRegister.callback.pScsiTestUnitReadyCallback	= &tmllpeScsiAppln_TestUnitReadyScsiCommand; 
    scsiRegister.callback.pScsiFormatUnitCallback		= &tmllpeScsiAppln_FormatUnitScsiCommand;    
    scsiRegister.callback.pScsiInquiryCallback			= &tmllpeScsiAppln_InquiryScsiCommand;
    scsiRegister.callback.pScsiStartStopUnitCallback	= &tmllpeScsiAppln_StartStopUnitScsiCommand;    
    scsiRegister.callback.pScsiPreventAllowMediumRemovalCallback = &tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand;    
    scsiRegister.callback.pScsiReadCapacityCallback		= &tmllpeScsiAppln_ReadCapacityScsiCommand; 
    scsiRegister.callback.pScsiWrite10Callback			= &tmllpeScsiAppln_Write10ScsiCommand;    
    scsiRegister.callback.pScsiRead10Callback			= &tmllpeScsiAppln_Read10ScsiCommand; 
    scsiRegister.callback.pScsiVerify10Callback			= &tmllpeScsiAppln_Verify10ScsiCommand;
    scsiRegister.callback.pScsiSyncCacheCallback		= &tmllpeScsiAppln_SyncCacheScsiCommand; 
    scsiRegister.callback.pScsiWriteBufferCallback		= &tmllpeScsiAppln_WriteBufferScsiCommand;    
    scsiRegister.callback.pScsiRequestSenseCallback		= &tmllpeScsiAppln_RequestSenseScsiCommand;  
    scsiRegister.callback.pScsiModeSeletCallback		= &tmllpeScsiAppln_ModeSelectScsiCommand;
    scsiRegister.callback.pScsiModeSenseCallback		= &tmllpeScsiAppln_ModeSenseScsiCommand; 
    scsiRegister.callback.pScsiReadFormatCapacitiesCallback = &tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand;    
    scsiRegister.callback.pScsiMassStorageResetCallback	= &tmllpeScsiAppln_MassStorageResetUsbRequest; 
    scsiRegister.callback.pScsiGetMaxLunCallback		= &tmllpeScsiAppln_GetMaxLunUsbRequest;    
    scsiRegister.callback.pScsiWriteBlockCallback		= &tmllpeScsiAppln_WriteBlockDataTransfer; 
    scsiRegister.callback.pScsiReadBlockCallback		= &tmllpeScsiAppln_ReadBlockDataTransfer;    
	
	//给MsBotRegister的回调函数指明确切的函数。
    tmllpeUsbScsi_Initialise(&scsiRegister);
    
    /* 为了使sScsiApplnBootSector保存flash，而不是sd的信息, 前2k 是flash的信息 */
	for(j=0;j<60;j++)
	{
		sScsiApplnBootSector[j] = sUsbScsiDataBuffer[j];
	}   
	return;
}
/*---------------------------------------------------------------------------
   Static Functions
   --------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Test Unit Ready scsi command callback static function.  
 *
 * This static function represents the Test Unit Ready scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Test Unit Ready
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_TestUnitReadyScsiCommand(void)
{
	
#if 0
    // zhoujie add 20051110, until windows finish format , we start format
    if( gtmWindowsFormatStarted  == True)
 	{
 		gtmWindowsFormatStarted = False;
 		gtmStandardFormatInitiated = True;
  
    }
#endif    
    
#if defined(HYHW_MEMORY_SD_MMC)
	if(sUsbScsiTransmitLun == SD_LUN)
	{
		/*	当sUsbScsiTransmitLun ==0 时，指nandflash的u disk 
			当sUsbScsiTransmitLun ==1 时，指sd 的 u disk */
#ifdef SDMMC_UDISK_DEBUG
		sScsiApplnEjectState[1] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;
#else
		Bool CardDetected;
		CardDetected = hyhwCardDetect();
		
		if(CardDetected)
		{
			if(sScsiApplnEjectState[1] == TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY)
			{
			
			}
			else if((sScsiApplnEjectState[1] ==TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_NOT_READY)&&((SdCardEject == TRUE)))
			{
				sScsiApplnEjectState[1] =TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED;
				SdCardEject = FALSE;
			}
			else if(sScsiApplnEjectState[1] ==TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED)
			{
				sScsiApplnEjectState[1] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;
			}
			if(gtmSdTestUnitReady == FALSE)
			{
				gtmSdTestUnitReady = TRUE;
				sScsiApplnEjectState[1] =TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED;
				SdCardEject = FALSE;
			}
		}
		else
		{
			//if(sScsiApplnEjectState[1] !=TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED)
			{
				sScsiApplnEjectState[1] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_NOT_READY;
				SdCardEject = TRUE;
			}
			
			sScsiApplnReadCapacity[1][0x00] = 0;
		    sScsiApplnReadCapacity[1][0x01] = 0;
		    sScsiApplnReadCapacity[1][0x02] = 0;
		    sScsiApplnReadCapacity[1][0x03] = 0;
		    sScsiApplnReadCapacity[1][0x04] = 0;
		    sScsiApplnReadCapacity[1][0x05] = 0;
		    sScsiApplnReadCapacity[1][0x06] = 0;
		    sScsiApplnReadCapacity[1][0x07] = 0;
		    
		   	gtmSdTestUnitReady = FALSE;
		}
#endif
	}
#endif	

	if ( sScsiApplnEjectState[sUsbScsiTransmitLun] == TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_NOT_READY )
	{
	    /* Device has been stopped earlier, so we report a NOT_READY */
		sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
		sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
		//sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASCQ;
		sScsiApplnSenseData.Reserve1 = 0x00;
		sScsiApplnSenseData.Reserve11 = 0x40;
		sScsiApplnSenseData.Reserve14 = 0x00;
		        
		/* Device has been stopped earlier, so we report a NOT_READY */
		        
		return (TMLLPE_SCSI_CALLBACK_FAILURE);
	}
	else if ( sScsiApplnEjectState[sUsbScsiTransmitLun] == TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED )
	{
		/* Device has been stopped earlier, so we report a NOT_READY */
		sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_UNIT_ATTENTION_SENSE_KEY;
		sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_MEDIA_CHANGED_ASC;
		//sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASCQ;
		sScsiApplnSenseData.Reserve1 = 0x01;
		sScsiApplnSenseData.Reserve11 = 0x41;
		sScsiApplnSenseData.Reserve14 = 0x01;        
		/* Device has been stopped earlier, so we report a NOT_READY */
		        
		return (TMLLPE_SCSI_CALLBACK_FAILURE);
	}
	else
	{
		/* update request sense data : no sense key / no additional sense code / */
		/* no additional sense code qualifier */
		sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
		sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
		//sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
		sScsiApplnSenseData.Reserve1 = 0x00;
		sScsiApplnSenseData.Reserve11 = 0x40;
		sScsiApplnSenseData.Reserve14 = 0x00;
		return (TMLLPE_SCSI_CALLBACK_SUCCESS);
	}
}

/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Format Unit scsi command callback static function.  
 *
 * This static function represents the Format Unit scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Format Unit
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_FormatUnitScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Inquiry scsi command callback static function.  
 *
 * This static function represents the Inquiry scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Inquiry
 * scsi commmand.
 * Actions to be taken on reception of this command : send back the requested Inquiry data
 *
 * @param       pScsiInquiry                    Usb Scsi subclass driver inquiry structure                     
 * @param       pScsiResponseData               Usb Scsi subclass driver data response structure                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */

static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_InquiryScsiCommand(ptmllpeScsiInquiry_st_t pScsiInquiry,
                                               ptmllpeScsiResponseData_st_t pScsiResponseData)
{
    UInt16 counter;
    
    switch (pScsiInquiry->requestedData)
    {
        case TMLLPE_SCSIAPPLN_INQUIRY_STANDARD_INQUIRY_DATA:
        {
            /* send back the standard inquiry data */
            /* the same Standard Inquiry Data is returned for all supported LUNs */
        #if 0
            for(counter = 0; counter < sizeof(gScsiApplInquiryStandardData); counter++)
            {
                *(pScsiResponseData->pResponseDataBuffer + counter) = gScsiApplInquiryStandardData[counter];
            }
            pScsiResponseData->responseDataLength = sizeof(gScsiApplInquiryStandardData);
        #else
            for(counter = 0; counter < sizeof(gScsiApplDeviceIdentification); counter++)
            {
                *(pScsiResponseData->pResponseDataBuffer + counter) = gScsiApplDeviceIdentification[counter];
            }
            pScsiResponseData->responseDataLength = sizeof(gScsiApplDeviceIdentification);
        #endif
            
            /* update request sense data : no sense key / no additional sense code / */
            /* no additional sense code qualifier */
            sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
		    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
		    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
		    sScsiApplnSenseData.Reserve1 = 0x01;
			sScsiApplnSenseData.Reserve11 = 0x41;
			sScsiApplnSenseData.Reserve14 = 0x01;
            return (TMLLPE_SCSI_CALLBACK_SUCCESS);
            
            break;
        }
        
        case TMLLPE_SCSIAPPLN_INQUIRY_VITAL_PRODUCT_DATA:
        {
            if ( pScsiInquiry->pageCode == TMLLPE_SCSIAPPLN_VPD_PAGE_UNIT_SERIAL_NUMBER )
            {
                /* send the Vital Product Data through as an answer to the Scsi command */
                for(counter = 0; counter < sizeof(gScsiApplUnitSerialNumber); counter++)
                {
                    *(pScsiResponseData->pResponseDataBuffer + counter) = gScsiApplUnitSerialNumber[counter];
                }
                pScsiResponseData->responseDataLength = sizeof(gScsiApplUnitSerialNumber);

                /* update request sense data : no sense key / no additional sense code / */
                /* no additional sense code qualifier */
                sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
			    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
			    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
			    sScsiApplnSenseData.Reserve1 = 0x01;
				sScsiApplnSenseData.Reserve11 = 0x41;
				sScsiApplnSenseData.Reserve14 = 0x01;
                return (TMLLPE_SCSI_CALLBACK_SUCCESS);
            }
            
            break;
        }
        
        case TMLLPE_SCSIAPPLN_INQUIRY_COMMAND_SUPPORT_DATA:
        {
            /* command support data is not supported */
            
            /* update request sense data : illegal request sense key / no additional sense code / */ 
            /* no additional sense code qualifier */
            sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
            sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
            //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
            sScsiApplnSenseData.Reserve1 = 0x00;
			sScsiApplnSenseData.Reserve11 = 0x40;
			sScsiApplnSenseData.Reserve14 = 0x00;
            return (TMLLPE_SCSI_CALLBACK_FAILURE);
            
            break;
        }   
        
        default:
        {
            /* both EVPD and CMDDT are set : illegal request */
            
            /* update request sense data : illegal request sense key / no additional sense code / */ 
            /* no additional sense code qualifier */
            sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
            sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
            //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
            sScsiApplnSenseData.Reserve1 = 0x00;
			sScsiApplnSenseData.Reserve11 = 0x40;
			sScsiApplnSenseData.Reserve14 = 0x00;
            return (TMLLPE_SCSI_CALLBACK_FAILURE);
            
            break;
        }
    }
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Start Stop Unit scsi command callback static function.  
 *
 * This static function represents the Start Stop Unit scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Start Stop Unit
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       pScsiStartStopUnit  Usb Scsi subclass driver start stop unit structure                       
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_StartStopUnitScsiCommand(ptmllpeScsiStartStopUnit_st_t pScsiStartStopUnit)
{

#if defined(HYHW_MEMORY_SD_MMC)
    if(sUsbScsiTransmitLun == SD_LUN)
	{
		/*	当sUsbScsiTransmitLun ==0 时，指nandflash的u disk 
			当sUsbScsiTransmitLun ==1 时，指sd 的 u disk */
#ifdef SDMMC_UDISK_DEBUG
		sScsiApplnEjectState[1] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;
#else
		//sScsiApplnEjectState[1] = hyhwCardDetect()== True? TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY: TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED;
#endif
	}
#endif	
    
    if ( sScsiApplnEjectState[sUsbScsiTransmitLun] == TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY )
    {
        if ( (pScsiStartStopUnit->loadEject == True) && (pScsiStartStopUnit->start == False) )
        {
            /* an eject was requested */
            sScsiApplnEjectState[sUsbScsiTransmitLun] = TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REQUEST;
        }
    
        /* update request sense data : no sense key / no additional sense code / */
        /* no additional sense code qualifier */
        sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
        sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
        //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
        sScsiApplnSenseData.Reserve1 = 0x01;
		sScsiApplnSenseData.Reserve11 = 0x41;
		sScsiApplnSenseData.Reserve14 = 0x01;
        
        return (TMLLPE_SCSI_CALLBACK_SUCCESS);
    }
    
    else
    {
        /* an eject was already issued, so we report a faillure */
         sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
        sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
        //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
        sScsiApplnSenseData.Reserve1 = 0x00;
		sScsiApplnSenseData.Reserve11 = 0x40;
		sScsiApplnSenseData.Reserve14 = 0x00;
        //解决右键弹出时，提示框问题
        //return (TMLLPE_SCSI_CALLBACK_FAILURE);
        return (TMLLPE_SCSI_CALLBACK_SUCCESS);
    } 
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Prevent Allow Medium Removal scsi command callback static function.  
 *
 * This static function represents the Prevent Allow Medium Removal scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Prevent Allow Medium Removal
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_PreventAllowMediumRemovalScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    //changed on 2007.09.19, for win 98
    return (TMLLPE_SCSI_CALLBACK_FAILURE);
    //return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Read Capacity scsi command callback static function.  
 *
 * This static function represents the Read Capacity scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Read Capacity
 * scsi commmand.
 * Actions to be taken on reception of this command : send back the requested Inquiry data
 *
 * @param       pScsiResponseData   Usb Scsi subclass driver data response structure                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadCapacityScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData)
{
    UInt16 counter;
    Bool ZeroCapacity;
    
    ZeroCapacity = True;
    /* actions to be taken on reception of this command : send back the requested Read Capacity data */
    for (counter = 0; counter < ((sizeof(sScsiApplnReadCapacity))/2); counter++)
    {
        *(pScsiResponseData->pResponseDataBuffer + counter) = sScsiApplnReadCapacity[sUsbScsiTransmitLun][counter];
        if( (sScsiApplnReadCapacity[sUsbScsiTransmitLun][counter] != 0))
        {
        	ZeroCapacity = False;
        }
    }
    pScsiResponseData->responseDataLength = (sizeof(sScsiApplnReadCapacity)/2);
    
    USBHandler_SetScsiFlag(True);
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
	if(ZeroCapacity)
	{
		sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
        sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
        //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
        sScsiApplnSenseData.Reserve1 = 0x00;
		sScsiApplnSenseData.Reserve11 = 0x40;
		sScsiApplnSenseData.Reserve14 = 0x00;
	    return (TMLLPE_SCSI_CALLBACK_FAILURE);
	}
	else
	{  
	    if((gtmSdTestUnitReady == FALSE)&&(sUsbScsiTransmitLun == SD_LUN))
	    //if(0)
	    {
	    	sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
            sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
            //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
            sScsiApplnSenseData.Reserve1 = 0x00;
			sScsiApplnSenseData.Reserve11 = 0x40;
			sScsiApplnSenseData.Reserve14 = 0x00;
		    return (TMLLPE_SCSI_CALLBACK_FAILURE);
	    }
	    else
	    {
		    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
            sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
            //sScsiApplnSenseData.additionalSenseCo1eQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
            sScsiApplnSenseData.Reserve1 = 0x01;
			sScsiApplnSenseData.Reserve11 = 0x41;
			sScsiApplnSenseData.Reserve14 = 0x01;
		    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
		}
	}
} 


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Write 10 scsi command callback static function.  
 *
 * This static function represents the Write 10 scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Write 10
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       pScsiWriteRead  Usb Scsi subclass driver read write structure                       
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Write10ScsiCommand(ptmllpeScsiWriteRead_st_t pScsiWriteRead)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Read 10 scsi command callback static function.  
 *
 * This static function represents the Read 10 scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Read 10
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       pScsiWriteRead  Usb Scsi subclass driver read write structure                       
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Read10ScsiCommand(ptmllpeScsiWriteRead_st_t pScsiWriteRead)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Verify 10 scsi command callback static function.  
 *
 * This static function represents the Verify 10 scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Verify 10
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_Verify10ScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Sync Cache scsi command callback static function.  
 *
 * This static function represents the Sync Cache scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Sync Cache
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_SyncCacheScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Write Buffer scsi command callback static function.  
 *
 * This static function represents the Write Buffer scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Write Buffer
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_WriteBufferScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
	sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Request Sense scsi command callback static function.  
 *
 * This static function represents the Request Sense scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Request Sense
 * scsi commmand.
 * Actions to be taken on reception of this command : send back the requested Request Sense data
 *
 * @param       pScsiResponseData   Usb Scsi subclass driver data response structure                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_RequestSenseScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData)
{
    UInt16 counter;
    
    
    /* create sense data to be returned */
    sScsiApplnReqSense[1] = sScsiApplnSenseData.Reserve1;
    sScsiApplnReqSense[2] = sScsiApplnSenseData.senseKey;
    sScsiApplnReqSense[11] = sScsiApplnSenseData.Reserve11;
    sScsiApplnReqSense[12] = sScsiApplnSenseData.additionalSenseCode ;
    sScsiApplnReqSense[14] = sScsiApplnSenseData.Reserve14;
    
    /* prepare to be returned data */
    for (counter = 0; counter < (sizeof(sScsiApplnReqSense)); counter++)
    {
        *(pScsiResponseData->pResponseDataBuffer + counter) = sScsiApplnReqSense[counter];
    }
    //length is added on usb lib, this is changed for win98
    //pScsiResponseData->responseDataLength = sizeof(sScsiApplnReqSense);
    
    /* not ready failure sense information was reported to the Scsihost */
    if ((sScsiApplnEjectState[sUsbScsiTransmitLun] != TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY) && (sScsiApplnSenseData.senseKey == TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY))
    {
       //sScsiApplnEjectState[sUsbScsiTransmitLun] = TMLLPE_SCSIAPPLN_EJECT_STATE_EJECT_REPORTED;
    }
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    
    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Mode Select scsi command callback static function.  
 *
 * This static function represents the Mode Select scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Mode Select
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ModeSelectScsiCommand(void)
{
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Mode Sense scsi command callback static function.  
 *
 * This static function represents the Mode Sense scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Mode Sense
 * scsi commmand.
 * Actions to be taken on reception of this command : send back the requested Mode Sense data
 *
 * @param       pScsiResponseData   Usb Scsi subclass driver data response structure                     
 * @returns     none 
 * @retval      TMLLPE_SCSI_CALLBACK_SUCCESS    successful 
 * @retval      TMLLPE_SCSI_CALLBACK_FAILURE    not successful               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */


static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ModeSenseScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData)
{
    UInt16 counter;
    Bool CardLocked;
    
#if 0    
    if( gtmStandardFormatInitiated ==1)
    {
    	gtmStandardFormatInitiated = 2;
    }
#endif    
    
    /* actions to be taken on reception of this command : send back the requested Mode Sense data */
    for (counter = 0; counter < (sizeof(sScsiApplnModeSense)); counter++)
    {
        *(pScsiResponseData->pResponseDataBuffer + counter) = sScsiApplnModeSense[counter];
    }
    pScsiResponseData->responseDataLength = sizeof(sScsiApplnModeSense);

#if defined(HYHW_MEMORY_SD_MMC)
    if(sUsbScsiTransmitLun == SD_LUN)
    {
#ifdef SDMMC_UDISK_DEBUG

#else
		if( hyhwGetCardProtectStatus())
    	//if(CardLocked)
    	{
    		*(pScsiResponseData->pResponseDataBuffer + 2) |= 0x80;	//写保护
    	}
#endif
    }
#endif
    
    /* update request sense data : no sense key / no additional sense code / */
    /* no additional sense code qualifier */
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;

    return (TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Read Format Capacities scsi command callback static function.  
 *
 * This static function represents the Read Format Capacities scsi command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a Read Format Capacities
 * scsi commmand.
 * Actions to be taken on reception of this command : none
 *
 * @param       pScsiResponseData   Usb Scsi subclass driver data response structure                     
 * @returns     none 
 * @retval      none               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */


static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadFormatCapcitiesScsiCommand(ptmllpeScsiResponseData_st_t pScsiResponseData)
{
    U8 counter;
    Bool ZeroCapacity;
    /* actions to be taken on reception of this command : none */
    
    /* update request sense data : illegal request key / no additional sense code / */
    /* no additional sense code qualifier */
   
    
    ZeroCapacity = True;
    for (counter = 0; counter < ((sizeof(sScsiApplnReadCapacity))/4); counter++)
    {
        if( sScsiApplnReadCapacity[sUsbScsiTransmitLun][counter] != 0)
        {
        	ZeroCapacity = False;
        	break;
        }
    }
    if(ZeroCapacity)
    {
    	/*
    	*(pScsiResponseData->pResponseDataBuffer + 0)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 1)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 2)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 3)= 0x08;
		*(pScsiResponseData->pResponseDataBuffer + 4)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 5)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 6)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 7)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 8)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 9)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 10)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 11)= 0x00;
		*/
		sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
	    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
	    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
	    sScsiApplnSenseData.Reserve1 = 0x00;
		sScsiApplnSenseData.Reserve11 = 0x40;
		sScsiApplnSenseData.Reserve14 = 0x00;
		return TMLLPE_SCSI_CALLBACK_FAILURE;
    }
    else
    {
    	if((gtmSdTestUnitReady == FALSE)&&(sUsbScsiTransmitLun == SD_LUN))
    	{
    		return TMLLPE_SCSI_CALLBACK_FAILURE;
    	}
    	
    	/*	byte 0-3, 返回的字节数 
    		byte 4-7, 总的sector的个数，大端
    		byte 8,   descriptor type 
    		byte 9-10 sector 的大小， 大端 */
	    *(pScsiResponseData->pResponseDataBuffer + 0)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 1)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 2)= 0x00;
		*(pScsiResponseData->pResponseDataBuffer + 3)= 0x08;

		*(pScsiResponseData->pResponseDataBuffer + 4)	= sScsiApplnReadCapacity[0][0x00];   
		*(pScsiResponseData->pResponseDataBuffer + 5)	= sScsiApplnReadCapacity[0][0x01];
		*(pScsiResponseData->pResponseDataBuffer + 6)	= sScsiApplnReadCapacity[0][0x02];
		*(pScsiResponseData->pResponseDataBuffer + 7)	= sScsiApplnReadCapacity[0][0x03];
		*(pScsiResponseData->pResponseDataBuffer + 8)	= 0x03;   /* descriptor  */
		*(pScsiResponseData->pResponseDataBuffer + 9)	= sScsiApplnReadCapacity[0][0x05];  
		*(pScsiResponseData->pResponseDataBuffer + 10)	= sScsiApplnReadCapacity[0][0x06];
		*(pScsiResponseData->pResponseDataBuffer + 11)	= sScsiApplnReadCapacity[0][0x07];
	}
    
	pScsiResponseData->responseDataLength = 12;
	//不知道为什么返回失败就会出错，u盘容量可以根据不同容量的
	//flash而变化，所以可能其它地方调用了读容量的函数。
    //return (TMLLPE_SCSI_CALLBACK_FAILURE);//changed by ice
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    return (TMLLPE_SCSI_CALLBACK_SUCCESS);

   
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Mass Storage Reset usb command callback static function.  
 *
 * This static function represents the Mass Storage Reset usb command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a 
 * Mass Storage Reset usb command.
 * Actions to be taken on reception of this request : none
 *
 * @param       none                     
 * @returns     none 
 * @retval      none               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */


static void tmllpeScsiAppln_MassStorageResetUsbRequest(void)
{
    /* actions to be taken on reception of this request : none */
    
    return;                                               
}

/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Get Max LUN usb command callback static function.  
 *
 * This static function represents the Get Max LUN usb command callback function.
 * This function is called by the Usb Scsi subclass driver on reception of a 
 * Get Max LUN usb command.
 * Actions to be taken on reception of this request : send back the max lun value
 *
 * @param       pScsiGetMaxLun  Usb Scsi subclass driver get max lun structure                     
 * @returns     none 
 * @retval      none               
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */


static void tmllpeScsiAppln_GetMaxLunUsbRequest(ptmllpeScsiGetMaxLun_st_t pScsiGetMaxLun)
{

    /* actions to be taken on reception of this request : return max lun value */
#if defined(HYHW_MEMORY_SD_MMC)
//	pScsiGetMaxLun->maxLunValue = hyhwCardDetect()==True? TMLLPE_SCSIAPPLN_MAX_LUN_VALUE: (TMLLPE_SCSIAPPLN_MAX_LUN_VALUE - 1); 
	//pScsiGetMaxLun->maxLunValue = 2;//gtmUsbMaxLunValue; 
	pScsiGetMaxLun->maxLunValue = gtmUsbMaxLunValue; 
#else
	/* 调试使用，仅使用nandflash作为u disk 时 */
	pScsiGetMaxLun->maxLunValue = 1;
#endif
   
    
    /* This variable should be initialised every time the USB cable is plugged in. */
    /* Since tmllpeScsiAppln_Initialise() is only called at startup and not at every USB cable plugin in the USB Sample Application, */
    /* this variable is initialised here. */
    
    sScsiApplnEjectState[0] = TMLLPE_SCSIAPPLN_EJECT_STATE_MEDIUM_READY;
    
    return;
}

/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Write Block data transfer callback static function.  
 *
 * This static function represents the Write Block data transfer callback function.
 * This function is called by the Usb Scsi subclass driver data <= medium block size
 * has to be written to the storage medium. The Usb Scsi subclass driver divides transfers
 * of bulk data into appropriate writes of data <= medium block size.
 * Actions to be taken on reception of this request : write data to storage medium
 *
 * @param       pScsiMediumAccess    Usb Scsi subclass driver data read write transfer structure                     
 * @returns     error status code 
 * @retval      TM_OK : write is successful
 * @retval      TM_ERR_WRITE : write is not successful             
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_WriteBlockDataTransfer(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess)
{

         
  
    Int32   writeStatus;
    
    
#if 0    
    if( (pScsiMediumAccess->mediumDataBlockAddress >= 1) && (pScsiMediumAccess->mediumDataBlockAddress < (1+sectorsPerFat[sUsbScsiTransmitLun])) )
    {
/*       if( uploadProtectionSubset != NULL )
        {
            pUInt8 cachedAccessedSector =
                (pUInt8)(&(fatBuffer[ (pScsiMediumAccess->mediumDataBlockAddress - 1) * sScsiApplnScsiBlockSize ]));

            // Copy new sector in buffer 
            memcpy( cachedAccessedSector, pScsiMediumAccess->pMediumDataBuffer, sScsiApplnScsiBlockSize );
        }
  */  } 
#endif
    
#if 1	
	/* We stop the format command (we fill format the device ourself) 
    
   		比较修改0 地址 如果是格式化信号gtmStandardFormatInitiated为True
		如果要写的是0 地址，就是格式化的信号，检查是否需要格式化 */  
	if( (pScsiMediumAccess->mediumDataBlockAddress == 0)&&(sUsbScsiTransmitLun != SD_LUN))
	{
		Int8  i;
	#if 0
        /* Check if basic parameters modified， sScsiApplnBootSector 在开机时读出
        	如果是正常的读写文件，windows 读出这些信息，再写回， 所以不会改变
           如果是widows 发出format信号， 这些信息将不一样，表明进入format
           但是我们又不能让windows格式化 ，所以记住它 */
        for( i=0x0B; i<0x20; i++ )
        {
            if( (UInt8)sScsiApplnBootSector[i] != (UInt8)pScsiMediumAccess->pMediumDataBuffer[i] )
            {
            	// zhoujie 20051110, windows need to format
               // gtmStandardFormatInitiated = True;
            	//gtmWindowsFormatStarted = True;
            	break;
            }
        }  
		gtmWindowsFormatStarted ++;
		if( count ==0)
		{
			count ++;
			mappingTable_CreateTLTinUsb();
		}	
		     
        /* 让 windows 先完成一遍， 不返回 */
       	//return ;
    	if ((UInt8)pScsiMediumAccess->pMediumDataBuffer[14] == 0x20)
    	{
    		return (TMLLPE_SCSI_CALLBACK_FAILURE);
    	}
    #endif
    }

#endif  
    
    /* actions to be taken on reception of this data transfer request : write data to storage medium */
    
   //**找到要写的位置
   if(sUsbScsiTransmitLun != SD_LUN)
    {
		U64 mediumDataBlockAddress;
		U32 mediumDataBlockAddress_l, mediumDataBlockAddress_h;
		
		mediumDataBlockAddress = (U64)(((U64)pScsiMediumAccess->mediumDataBlockAddress)*
										(U64)(sScsiApplnScsiBlockSize[sUsbScsiTransmitLun]));
		mediumDataBlockAddress_l = (U32)(mediumDataBlockAddress & 0xFFFFFFFF);
		mediumDataBlockAddress_h = (U32)((mediumDataBlockAddress>>32) & 0xFFFFFFFF);
    	hyswNandFlash_Write( (Int8*) pScsiMediumAccess->pMediumDataBuffer,
    		mediumDataBlockAddress_l, mediumDataBlockAddress_h,
    		pScsiMediumAccess->mediumDataLength);
    }
	else
   	{
#ifdef	HYHW_MEMORY_SD_MMC
		if (hyhwCardDetect() == True)
		{
			hyswSdMmc_Write((pScsiMediumAccess->mediumDataBlockAddress),
							 (U8*)pScsiMediumAccess->pMediumDataBuffer,
							 (pScsiMediumAccess->mediumDataLength>>9)) ;
		}
#endif						 
	}
    
    gtmUsbTransferInProgressRx = True; ///**tian **发消息给usb进程要延时的标志
  
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    
    return(TMLLPE_SCSI_CALLBACK_SUCCESS);
}


/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver Read Block data transfer callback static function.  
 *
 * This static function represents the Read Block data transfer callback function.
 * This function is called by the Usb Scsi subclass driver data <= medium block size
 * has to be read from the storage medium. The Usb Scsi subclass driver divides transfers
 * of bulk data into appropriate reads of data <= medium block size.
 * Actions to be taken on reception of this request : read data from storage medium
 *
 * @param       pScsiMediumAccess    Usb Scsi subclass driver data read write transfer structure                     
 * @returns     error status code 
 * @retval      TM_OK : write is successful
 * @retval      TM_ERR_READ : read is not successful             
 * @pre         none                    
 * @post        none                    
 */
/* ----------------------------------------------------------------------------- */
//extern gTest;
static tmllpeScsiCallbackReturn_en_t tmllpeScsiAppln_ReadBlockDataTransfer(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess)
{

	tmllpeScsiCallbackReturn_en_t status ;
	
	status = TMLLPE_SCSI_CALLBACK_SUCCESS;
	/*	找到要读的位置 */
	if(sUsbScsiTransmitLun != SD_LUN)
	{
		U64 mediumDataBlockAddress;
		U32 mediumDataBlockAddress_l, mediumDataBlockAddress_h;
		
		mediumDataBlockAddress = (U64)(((U64)pScsiMediumAccess->mediumDataBlockAddress)*
										(U64)(sScsiApplnScsiBlockSize[sUsbScsiTransmitLun]));
		mediumDataBlockAddress_l = (U32)(mediumDataBlockAddress & 0xFFFFFFFF);
		mediumDataBlockAddress_h = (U32)((mediumDataBlockAddress>>32) & 0xFFFFFFFF);
		#if 1
			hyswNandFlash_Read( (Int8 *) pScsiMediumAccess->pMediumDataBuffer,
				mediumDataBlockAddress_l, mediumDataBlockAddress_h,
	 			pScsiMediumAccess->mediumDataLength);
		#else
			/* 使用usb buffer 作为nandflash 读出的buffer，条件是address 必须为1k整数倍，且读取长度为1k */
	 		//if(pScsiMediumAccess->mediumDataBlockAddress == 7)
	 		//{
	 		//	status = 2;
	 		//}	
			hyswNandFlash_DirectRead1K( (Int8 *) pScsiMediumAccess->pMediumDataBuffer,
				mediumDataBlockAddress,
	 			pScsiMediumAccess->mediumDataLength);
	 		);
	 	#endif
	 }

#if defined(HYHW_MEMORY_SD_MMC)
	else
   	{
		if(hyhwCardDetect() == True)
		{
			U32 result;
			result = hyswSdMmc_Read_Usb((pScsiMediumAccess->mediumDataBlockAddress),
							(U8*)pScsiMediumAccess->pMediumDataBuffer,
							(pScsiMediumAccess->mediumDataLength>>9)); 
			if (result != HY_OK)
			{
				sScsiApplnReadCapacity[1][0x00] = 0;
				sScsiApplnReadCapacity[1][0x01] = 0;
				sScsiApplnReadCapacity[1][0x02] = 0;
				sScsiApplnReadCapacity[1][0x03] = 0;
				sScsiApplnReadCapacity[1][0x04] = 0;
				sScsiApplnReadCapacity[1][0x05] = 0;
				sScsiApplnReadCapacity[1][0x06] = 0;
				sScsiApplnReadCapacity[1][0x07] = 0;
				sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
			    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
			    sScsiApplnSenseData.Reserve1 = 0x00;
				sScsiApplnSenseData.Reserve11 = 0x40;
				sScsiApplnSenseData.Reserve14 = 0x00;
				status = TMLLPE_SCSI_CALLBACK_FAILURE;
				return(status);
			}
		}
		else
		{
			sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NOT_READY_SENSE_KEY;
		    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_MEDIUM_NOT_PRESENT_ASC;
		    sScsiApplnSenseData.Reserve1 = 0x00;
			sScsiApplnSenseData.Reserve11 = 0x40;
			sScsiApplnSenseData.Reserve14 = 0x00;
			status = TMLLPE_SCSI_CALLBACK_FAILURE;
			return(status);
		}
	}
#endif	


    gtmUsbTransferInProgressTx = True; //**tian **发消息给usb进程要延时的标志
   
    sScsiApplnSenseData.senseKey = TMLLPE_SCSIAPPLN_NO_SENSE_SENSE_KEY;
    sScsiApplnSenseData.additionalSenseCode = TMLLPE_SCSIAPPLN_NO_SENSE_ASC;
    //sScsiApplnSenseData.additionalSenseCodeQualifier = TMLLPE_SCSIAPPLN_NO_SENSE_ASCQ;
    sScsiApplnSenseData.Reserve1 = 0x01;
	sScsiApplnSenseData.Reserve11 = 0x41;
	sScsiApplnSenseData.Reserve14 = 0x01;
    
    return(status);
}


/*@}*/
#pragma arm section	//zidata="usb_zidata", rwdata="usb_zidata"



