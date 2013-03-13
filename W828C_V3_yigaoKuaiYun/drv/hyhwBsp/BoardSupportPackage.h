/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/


#ifndef HY_HSA_BOARDSUPPORTPACKAGE_H_
#define HY_HSA_BOARDSUPPORTPACKAGE_H_


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   Defines : power values
   --------------------------------------------------------------------------*/

#define POWERED_ON                 (1)
#define POWERED_OFF                (0)



/*------------------------------------------------------------------------------
   Defines : storage media
   ---------------------------------------------------------------------------*/

#define NUMBER_OF_STORAGE_VOLUMES  (4)
#define USB_VISIBLE_YES            (1)
#define USB_VISIBLE_NO             (0)




/* EFLASH USED ADDRESS
**
** REMARK : This should be obsolited by using the offset from the end of
**          the EFLASH
*/

#define FLASH_RESERVED_AREA_USED_ADDRESS        (0x0007F810)
#define NOR_FLASH_SIZE							(512*1024)  // 512k norFlash
#define NOR_FLASH_SECTORS						(16)        // the total sectors
#define NOR_FLASH_SMALL_SECTOR_SIZE				(8 *1024)	// 8k for small nor Flash size

/*---------------------------------------------------------------------------------------------------
This value is used for the ATA STANDBY COMMAND
Function tmllicAta_Standby() called in tmllicAta_Init()
The obtained standby time is drive dependent (check drive specs)
e.g. Cornice drive: time = DEFAULT_HDD_STANDBY_VALUE * 100ms except for 0 then time is 256 * 100ms
     Hitachi MD: time = according ATA spec except for 0 then time = 109min
*/
#define DEFAULT_HDD_STANDBY_VALUE       (30) 
#define DEFAULT_HDD_MAX_STANDBY_VALUE   (0) 

/* The DEFAULT_HDD_POWER_OFF_TIME_OUT or DEFAULT_HDD_MAX_POWER_OFF_TIME_OUT
 * defines the timeout in milliseconds to switch Hard disk power off, once no
 * medium access activity is done. The installed time-out will be restarted 
 * or reinstalled each time an access to the medium is done.
 */
#define DEFAULT_HDD_POWER_OFF_TIME_OUT		(3500)
#define DEFAULT_HDD_MAX_POWER_OFF_TIME_OUT	(10000)
/*---------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   Defines : USB configuration
   ---------------------------------------------------------------------------*/

#define USB_DEV_DESCR_LENGTH                    (18)
#define USB_CONFIG_DESCR_LENGTH                 (9)
#define USB_CONFIG_DESCR_OTHER_SPEED_LENGTH     (9)


#define USB_SET_VID_PID(vid_hi, vid_lo, pid_hi, pid_lo)                                 \
    const unsigned char stDscDeviceFS[] = { 0x12, 0x01, 0x00, 0x02, 0x00,               \
    0x00, 0x00, 0x08, vid_hi, vid_lo, pid_hi, pid_lo,  0x01, 0x00, 0x01, 0x02, 0x04,    \
    0x01 };                                                                             \
                                                                                        \
    const unsigned char stDscDeviceHS[] = { 0x12, 0x01, 0x00, 0x02, 0x00,               \
    0x00, 0x00, 0x40, vid_hi, vid_lo, pid_hi, pid_lo,  0x01, 0x00, 0x01, 0x03, 0x04,    \
    0x01 };                                                                             \
                                                                                        
    
#define USB_SET_CONFIG_ATTR_MAXPOWER(bmAttributes, maxPower)                            \
    unsigned char stDscConfiguration[] = { 0x09, 0x02, 0x20, 0x00, 0x01,          \
    0x01, 0x00, bmAttributes, maxPower };                                               \
                                                                                        \
    unsigned char stDscConfiguration_HS[] = { 0x09, 0x02, 0x20, 0x00, 0x01,       \
    0x01, 0x00, bmAttributes, maxPower };                                               \
                                                                                        \
    unsigned char stDscConfigurationOtherSpeed[] = { 0x09, 0x07, 0x20, 0x00, 0x01,    \
    0x01, 0x00, bmAttributes, maxPower };                                                   \
                                                                                            \
    unsigned char stDscConfiguration_HSOtherSpeed[] = { 0x09, 0x07, 0x20, 0x00, 0x01, \
    0x01, 0x00, bmAttributes, maxPower };                                                   \
       
                                                                                        

/*------------------------------------------------------------------------------
   Defines : SCSI configuration
   ---------------------------------------------------------------------------*/
 
#define SCSI_INQUIRY_DATA_LENGTH_STANDARD_DATA  (36)                                                                        

/*------------------------------------------------------------------------------
   Defines : Portable Device Unit ID
   ---------------------------------------------------------------------------*/

#define PDUID_TOTALLENGTH                       (16)

#define SCSI_INQUIRY_DATA_LENGTH_SUPPORTED_VPD_PAGES (7)
#define SCSI_INQUIRY_DATA_LENGTH_DEVICE_IDENTIFICATION (36)
#define SCSI_INQUIRY_DATA_LENGTH_UNIT_SERIAL_NUMBER (PDUID_TOTALLENGTH + 4)
 
                                                                                        

/*------------------------------------------------------------------------------
   Defines : GPIO 
   ---------------------------------------------------------------------------*/

#define NO_GPIO  (-1)
#define NO_UNIT  (-1)

#define GPIO_ACTIVE_LOW  (0)
#define GPIO_ACTIVE_HIGH (1)


#define BSP_EFLASH_SIZE		     		(0x00080000) /* 524.288 bytes   */
#define BSP_EFLASH_BASE_ADDRESS			(0x10400000)
#define BSP_EFLASH_LAST_SECTOR_OFFSET	(0x0007E000)

/*------------------------------------------------------------------------------
   Types : storage media
   ---------------------------------------------------------------------------*/



/* Logical Volume allocation 
*
* Remark: Do NOT add, remove or change order of this enumeratiom without
* updating the following structures:
*	volumeDriverDispatchTable <-- tmJMIDosfile/src/DevTable.c
*   volumeInterfaceDriverTable <-- tmNucleusVfat/src/devtable.c
*/

typedef struct 
{
    unsigned int    startAddress;
    unsigned int    sizeInBytes;
    
} RamDiskConfiguration_st;

#if 0
typedef struct 
{
    VolumeInterfaceType_en  interfaceType;
    BOOL                    usbEnumeration;
    
} StorageVolumeTableEntry_st;
#endif



/* Sdmmc protect and detect pin description*/
typedef struct
{
	unsigned int 	detect_Port;		/* 检测卡是否插入的port */
	unsigned int 	detect_Gpio;		/* 检测卡是否插入的gpio */
	int	 			detect_Level;		/* 检测卡是否插入的gpio的有效电平，可能是高或低电平表示插入 */ 
	
	unsigned int 	protect_Port;		/* 检测卡是否写保护的port */
	unsigned int 	protect_Gpio;		/* 检测卡是否写保护的gpio */
	int				protect_Level;		/* 检测卡是否写保护的gpio的有效电平，可能是高或低电平表示写保护 */ 
	
}SdmmcStorageMedia_st;


/* SMC configuration table entry
**
** Defines the smc configuration table entries. Should always contain the maximum frequency
** for which the settings are safe. Currently contains read and write wait states.
*/
typedef struct 
{
    unsigned int	frequency;
    unsigned char	readWait;
    unsigned char	writeWait;
} hsaBsp_SmcConfiguration_t;


/* pointer to smc configuration table entry
*/
typedef hsaBsp_SmcConfiguration_t *  phsaBsp_SmcConfiguration_t;



/*------------------------------------------------------------------------------
   Types : upload protection
   ---------------------------------------------------------------------------*/

/* Upload protection enumeration 
*/
typedef struct
{
    const char * directoryName;
    const char * fileExtension;
    const unsigned numberOfAllowedClusters;
} UploadProtection_st;



/*------------------------------------------------------------------------------
   Exported variables:
   ---------------------------------------------------------------------------*/

//extern const VolumeInterfaceType_en storageVolumeTable[NUMBER_OF_STORAGE_VOLUMES];
extern const char* volumeIdentifierTable[ NUMBER_OF_STORAGE_VOLUMES ];
extern const unsigned char stDscDeviceFS[USB_DEV_DESCR_LENGTH];
extern const unsigned char stDscDeviceHS[USB_DEV_DESCR_LENGTH];
extern const unsigned char stDscConfiguration[USB_CONFIG_DESCR_LENGTH];
extern const unsigned char stDscConfiguration_HS[USB_CONFIG_DESCR_LENGTH];
extern const unsigned char stDscConfigurationOtherSpeed[USB_CONFIG_DESCR_OTHER_SPEED_LENGTH];
extern const unsigned char stDscConfiguration_HSOtherSpeed[USB_CONFIG_DESCR_OTHER_SPEED_LENGTH];
extern const unsigned char StringDescManuf[];
extern const unsigned char StringDescProdFS[];
extern const unsigned char StringDescProdHS[];
extern const unsigned char StringDescSeri[];
extern const unsigned char gScsiApplUnitSerialNumber[SCSI_INQUIRY_DATA_LENGTH_UNIT_SERIAL_NUMBER];
extern const unsigned char gScsiApplStandardInquiryData[SCSI_INQUIRY_DATA_LENGTH_STANDARD_DATA];
extern const      UploadProtection_st uploadProtection;
extern int        persistentStorageTotalSize;
extern const hsaBsp_SmcConfiguration_t gkhsaBsp_smcNandConfigTable[8];
extern const unsigned char * PDUID_START_ADDRESS;
extern const unsigned char PDUID[PDUID_TOTALLENGTH];
extern const RamDiskConfiguration_st hsabsp_RamDiskConfiguration;

extern const char userDefinedBeep[];
extern const int  userDefinedBeepSize;

extern const SdmmcStorageMedia_st SdmmcStorageMedia;
extern const unsigned char gScsiApplDeviceIdentification[36];



#endif /* HSA_BOARDSUPPORTPACKAGE_H_ */


