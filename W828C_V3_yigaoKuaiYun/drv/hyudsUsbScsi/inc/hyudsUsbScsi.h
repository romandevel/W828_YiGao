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


#ifndef TM_LLPE_USB_SCSI_H_
#define TM_LLPE_USB_SCSI_H_

/**
 * \defgroup tmllpeUsbScsi tmllpeUsbScsi
 * @brief The peripheral driver layer for the Usb Scsi subclass.
 *
 * This Usb Scsi subclass driver handles the Usb Scsi subclass related part in a Usb - 
 * Mass Storage, Bulk Only Transport class - Scsi subclass - device.  
 * At one side, it interacts with the Usb MsBot class driver of a Usb Device Stack. 
 * At the other side, it interacts with a Scsi peripheral device application.
 * The Usb Scsi subclass driver extracts Scsi related data out of the bulk data coming from the Usb MsBot
 * class driver, like Usb Mass Storage commands and Scsi commands. No processing of this Scsi data is done in the 
 * Usb Scsi subclass driver itself. Through a set of callback functions, registered by the Scsi application, 
 * the Usb Scsi subclass driver notifies the Scsi application about the arrival of the Usb Mass Storage commands and Scsi
 * commands. It is the responsibility of the Scsi application to process them. If the received Scsi commands
 * require data to be sent back to the host, the Scsi application has to provide the Usb Scsi subclass driver with this 
 * feedback data. The Usb Scsi subclass driver on it's turn will forward this data to the Usb MsBot class driver.
 * In case bulk data has to be transfered from the host to the device or in the other direction, the Usb Scsi subclass
 * driver takes care of the data management. It translates the receiving/transmiting of bulk data into appropriate writes/
 * reads to/from the device's storage medium. This interaction is also done through a callback function mechanism between 
 * Usb Scsi subclass driver and the Scsi application.
 *
 * Assumptions :
 * - The Usb Device Stack (Device Controller Driver + Usb Device Function + MsBot Class Driver)
 *   must be initialised properly and be up and running correctly.
 * - A Scsi application driver must be present in order to interact properly with the Usb Scsi subclass driver.
 * - In order to interpret the Scsi commands and corresponding parameters properly, a knowledge of the following 
 *   Scsi specs is required : SAM2 / SPC2 / RBC2  
 *
 * Functional limitations :
 * - The following Usb Mass Storage commands are supported : Bulk-Only Mass Storage Reset / Get Max LUN
 * - The following Scsi commands are supported : TEST UNIT READY / FORMAT UNIT / INQUIRY / START STOP UNIT /
 *   PREVENT ALLOW REMOVAL / READ CAPACITY / READ 10 / WRITE 10 / VERIFY 10 / SYNC CACHE / WRITE BUFFER / 
 *   REQUEST SENSE / MODE SELECT / MODE SENSE / READ FORMAT CAPACITIES
 * - Only 1 application, supporting 1 LUN, can make use of this Usb Scsi subclass driver.
 * - The parameters provided with Scsi command callbacks is limited. The supported parameters should be sufficient 
 *   to implement a basic Nand Flash memory based application.
 * - 1024 should be a integer multiple of the Scsi block size of the device's storage medium.                               
 * @{
 */

/** 
 * @file
 * @brief The peripheral driver layer for the Usb Scsi subclass.
 *
 * This is the peripheral driver layer for the Usb Scsi subclass.
 * 
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
//#include "hyOsCpuDef.h"  
#include "hyTypes.h"
#include "hyErrors.h"


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/
/**
 * @brief Usb Scsi subclass driver callback return values.
 *
 * This enumeration defines the possible return values to be returned by 
 * the Usb Scsis subclass driver callback functions.
 *
 * @see ptmllpeScsiTestUnitReadyCallbackFunction, ptmllpeScsiFormatUnitCallbackFunction
 * @see ptmllpeScsiInquiryCallbackFunction, ptmllpeScsiStartStopUnitCallbackFunction, ptmllpeScsiStartStopUnitCallbackFunction
 * @see ptmllpeScsiPreventAllowMediumRemovalCallbackFunction, ptmllpeScsiReadCapacityCallbackFunction, ptmllpeScsiWrite10CallbackFunction
 * @see ptmllpeScsiRead10CallbackFunction, ptmllpeScsiVerify10CallbackFunction, ptmllpeScsiSyncCacheCallbackFunction
 * @see ptmllpeScsiWriteBufferCallbackFunction, ptmllpeScsiRequestSenseCallbackFunction, ptmllpeScsiModeSelectCallbackFunction
 * @see ptmllpeScsiModeSenseCallbackFunction, ptmllpeScsiReadFormatCapacitiesCallbackFunction, ptmllpeScsiMassStorageResetCallbackFunction
 * @see ptmllpeScsiGetMaxLunCallbackFunction, ptmllpeScsiWriteBlockCallbackFunction, ptmllpeScsiReadBlockCallbackFunction
 */
typedef enum tmllpeScsiCallbackReturn_en_t
{
    TMLLPE_SCSI_CALLBACK_SUCCESS,
    TMLLPE_SCSI_CALLBACK_FAILURE

} tmllpeScsiCallbackReturn_en_t;

/**
 * @brief Usb Scsi subclass driver data response structure.
 *
 * This structure defines the data that needs to be sent back to the host when a Scsi command 
 * that requests for data feedback is being received.
 * - pResponseDataBuffer : a pointer to the buffer containing the data
 * - responseDataLength : the size (in bytes) of the relevant data in the buffer  
 *
 * @see ptmllpeScsiInquiryCallbackFunction, ptmllpeScsiReadCapacityCallbackFunction
 * @see ptmllpeScsiRequestSenseCallbackFunction, ptmllpeScsiModeSenseCallbackFunction
 * @see ptmllpeScsiReadFormatCapacitiesCallbackFunction  
 */
typedef struct tmllpeScsiResponseData_st_t
{
    pUInt8  pResponseDataBuffer;
    UInt32  responseDataLength;

} tmllpeScsiResponseData_st_t, * ptmllpeScsiResponseData_st_t;


/**
 * @brief Usb Scsi subclass driver data read write transfer structure.
 *
 * This structure defines the information needed in order to transfer data between host and application
 * and vice versa after reception of a WRITE 10 or READ 10 Scsi command.   
 * - mediumDataBlockAddress : Logical Block Address of the device's storage medium to write/read data to/from
 * - pMediumDataBuffer : pointer to the buffer holding data to be written/read
 * - mediumDataLength : number of bytes to be transfered
 *
 * @see ptmllpeScsiWriteBlockCallbackFunction, ptmllpeScsiReadBlockCallbackFunction  
 */
 
 //*********************************************************************
 
typedef struct tmllpeScsiMediumAccess_st_t
{
    UInt32  mediumDataBlockAddress;
    pUInt8  pMediumDataBuffer;
    UInt32  mediumDataLength;
    UInt8   logicalUnit;

} tmllpeScsiMediumAccess_st_t, * ptmllpeScsiMediumAccess_st_t;


 //*********************************************************************
/**
 * @brief Usb Scsi subclass driver start stop unit structure.
 *
 * This structure defines the information needed when a START STOP UNIT Scsi command is received.
 * - start : TRUE when the application shall make the block device ready for use, FALSE otherwise
 * - loadEject : TRUE when the application shall take action regarding loading or ejecting the medium, FALSE otherwise
 *
 * @see ptmllpeScsiStartStopUnitCallbackFunction  
 */
 
typedef struct tmllpeScsiStartStopUnit_st_t
{
    UInt8  start;
    UInt8  loadEject;

} tmllpeScsiStartStopUnit_st_t, * ptmllpeScsiStartStopUnit_st_t;


/**
 * @brief Usb Scsi subclass driver get max lun structure.
 *
 * This structure defines the maximum LUN value the application supports. This value will be feed back to the host on 
 * reception of a Usb Mass Storage Get Max LUN command.
 * - maxLunValue : maximum lun value
 *
 * @see ptmllpeScsiGetMaxLunCallbackFunction  
 */
typedef struct tmllpeScsiGetMaxLun_st_t
{   
    UInt8  maxLunValue;

} tmllpeScsiGetMaxLun_st_t, * ptmllpeScsiGetMaxLun_st_t;


/**
 * @brief Usb Scsi subclass driver read write structure.
 *
 * This structure defines the information received when a WRITE 10 or READ 10 Scsi command has arrived.
 * - blockAddress : start LBA of the device's storage medium to/from which data will be written/read
 * - blockCount : number of LBAs that will be written/read  
 *
 * @see ptmllpeScsiWrite10CallbackFunction, ptmllpeScsiRead10CallbackFunction  
 */
typedef struct tmllpeScsiWriteRead_st_t
{
    UInt32 blockAddress;
    UInt32 blockCount; 

} tmllpeScsiWriteRead_st_t, * ptmllpeScsiWriteRead_st_t;


/**
 * @brief Usb Scsi subclass driver inquiry structure.
 *
 * This structure defines the information needed when a INQUIRY Scsi command is received.
 * - requestedData : specifies byte 1 of the Scsi INQUIRY command (containing CMDDT and EVPD)
 * - pageCode : specifies which page of vital product data information shall be returned
 *
 * @see ptmllpeScsiInquiryCallbackFunction  
 */
typedef struct tmllpeScsiInquiry_st_t
{
    UInt8 requestedData;
    UInt8 pageCode;

} tmllpeScsiInquiry_st_t, * ptmllpeScsiInquiry_st_t;


/**
 * @brief Usb Scsi subclass driver Test Unit Ready callback function.
 *
 * This callback function is called on reception of a TEST UNIT READY Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiTestUnitReadyCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Format Unit callback function.
 *
 * This callback function is called on reception of a FORMAT UNIT Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiFormatUnitCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Inquiry callback function.
 *
 * This callback function is called on reception of a INQUIRY Scsi command.
 * The data that is inquired, is specified through pScsiInquiry.  
 * The Inquiry data is returned through pScsiResponseData.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiResponseData_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiInquiryCallbackFunction)(ptmllpeScsiInquiry_st_t pScsiInquiry,
                                                    ptmllpeScsiResponseData_st_t pScsiResponseData);


/**
 * @brief Usb Scsi subclass driver Start Stop Unit callback function.
 *
 * This callback function is called on reception of a START STOP UNIT Scsi command.  
 * The Start Stop Unit data is received through pScsiStartStopUnit.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiStartStopUnit_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiStartStopUnitCallbackFunction)(ptmllpeScsiStartStopUnit_st_t pScsiStartStopUnit);


/**
 * @brief Usb Scsi subclass driver Prevent Allow Medium Removal callback function.
 *
 * This callback function is called on reception of a PREVENT ALLOW MEDIUM REMOVAL Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiPreventAllowMediumRemovalCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Read Capacity callback function.
 *
 * This callback function is called on reception of a READ CAPACITY Scsi command.  
 * The Read Capacity data is returned through pScsiResponseData.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiResponseData_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiReadCapacityCallbackFunction)(ptmllpeScsiResponseData_st_t pScsiResponseData);


/**
 * @brief Usb Scsi subclass driver Write 10 callback function.
 *
 * This callback function is called on reception of a WRITE 10 Scsi command.  
 * The Write 10 data is received through pScsiWriteRead.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiWriteRead_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiWrite10CallbackFunction)(ptmllpeScsiWriteRead_st_t pScsiWriteRead);


/**
 * @brief Usb Scsi subclass driver Read 10 callback function.
 *
 * This callback function is called on reception of a READ 10 Scsi command.  
 * The Write 10 data is received through pScsiWriteRead.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiWriteRead_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiRead10CallbackFunction)(ptmllpeScsiWriteRead_st_t pScsiWriteRead);


/**
 * @brief Usb Scsi subclass driver Verify 10 callback function.
 *
 * This callback function is called on reception of a VERIFY 10 Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiVerify10CallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Sync Cache callback function.
 *
 * This callback function is called on reception of a SYNC CACHE Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiSyncCacheCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Write Buffer callback function.
 *
 * This callback function is called on reception of a WRITE BUFFER Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiWriteBufferCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Request Sense callback function.
 *
 * This callback function is called on reception of a REQUEST SENSE Scsi command.  
 * The Request Sense data is returned through pScsiResponseData.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiResponseData_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiRequestSenseCallbackFunction)(ptmllpeScsiResponseData_st_t pScsiResponseData);


/**
 * @brief Usb Scsi subclass driver Mode Select callback function.
 *
 * This callback function is called on reception of a MODE SELECT Scsi command.  
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiModeSelectCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Mode Sense callback function.
 *
 * This callback function is called on reception of a REQUEST SENSE Scsi command.  
 * The Mode Sense data is returned through pScsiResponseData.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiResponseData_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiModeSenseCallbackFunction)(ptmllpeScsiResponseData_st_t pScsiResponseData);


/**
 * @brief Usb Scsi subclass driver Read Format Capacities callback function.
 *
 * This callback function is called on reception of a READ FORMAT CAPACITIES Scsi command.  
 * The Read Format Capacities data is returned through pScsiResponseData.
 * If successful, it returns TMLLPE_SCSI_CALLBACK_SUCCESS. Otherwise, it returns TMLLPE_SCSI_CALLBACK_FAILURE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiResponseData_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiReadFormatCapacitiesCallbackFunction)(ptmllpeScsiResponseData_st_t pScsiResponseData);


/**
 * @brief Usb Scsi subclass driver Usb Mass Storage Reset callback function.
 *
 * This callback function is called on reception of a Usb Mass Storage Reset command.  
 *
 * @see tmllpeScsiCallback_st_t  
 */
typedef void (* ptmllpeScsiMassStorageResetCallbackFunction)(void);


/**
 * @brief Usb Scsi subclass driver Usb Get Max Lun callback function.
 *
 * This callback function is called on reception of a Usb Mass Storage Get Max Lun command.  
 * The Get Max Lun data is returned through pScsiGetMaxLun.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiGetMaxLun_st_t  
 */
typedef void (* ptmllpeScsiGetMaxLunCallbackFunction)(ptmllpeScsiGetMaxLun_st_t pScsiGetMaxLun);


/**
 * @brief Usb Scsi subclass driver Write Blok callback function.
 *
 * This callback function is called when a block of data has to be written to the device's storage medium,
 * as a result of a previously received WRITE 10 Scsi command.  
 * The to be transfered data information is received and returned through pScsiMediumAccess.
 * If successful, it returns TM_OK. Otherwise, it returns TM_ERR_WRITE.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiMediumAccess_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiWriteBlockCallbackFunction)(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess);


/**
 * @brief Usb Scsi subclass driver Read Blok callback function.
 *
 * This callback function is called when a block of data has to be read from the device's storage medium,
 * as a result of a previously received Read 10 Scsi command.  
 * The to be transfered data information is received and returned through pScsiMediumAccess.
 * If successful, it returns TM_OK. Otherwise, it returns TM_ERR_READ.
 *
 * @see tmllpeScsiCallback_st_t, tmllpeScsiMediumAccess_st_t  
 */
typedef tmllpeScsiCallbackReturn_en_t (* ptmllpeScsiReadBlockCallbackFunction)(ptmllpeScsiMediumAccess_st_t pScsiMediumAccess);
   

/**
 * @brief Usb Scsi subclass driver callback structure.
 *
 * This structure defines the function pointers to the callbacks that are being called 
 * when Usb Mass Storage commands/Scsi commands/data transfer events occur. 
 *
 * @see tmllpeScsiRegister_st_t, ptmllpeScsiTestUnitReadyCallbackFunction, ptmllpeScsiFormatUnitCallbackFunction
 * @see ptmllpeScsiInquiryCallbackFunction, ptmllpeScsiStartStopUnitCallbackFunction, ptmllpeScsiStartStopUnitCallbackFunction
 * @see ptmllpeScsiPreventAllowMediumRemovalCallbackFunction, ptmllpeScsiReadCapacityCallbackFunction, ptmllpeScsiWrite10CallbackFunction
 * @see ptmllpeScsiRead10CallbackFunction, ptmllpeScsiVerify10CallbackFunction, ptmllpeScsiSyncCacheCallbackFunction
 * @see ptmllpeScsiWriteBufferCallbackFunction, ptmllpeScsiRequestSenseCallbackFunction, ptmllpeScsiModeSelectCallbackFunction
 * @see ptmllpeScsiModeSenseCallbackFunction, ptmllpeScsiReadFormatCapacitiesCallbackFunction, ptmllpeScsiMassStorageResetCallbackFunction
 * @see ptmllpeScsiGetMaxLunCallbackFunction, ptmllpeScsiWriteBlockCallbackFunction, ptmllpeScsiReadBlockCallbackFunction
 */
typedef struct tmllpeScsiCallback_st_t
{
    ptmllpeScsiTestUnitReadyCallbackFunction                pScsiTestUnitReadyCallback;
    ptmllpeScsiFormatUnitCallbackFunction                   pScsiFormatUnitCallback;
    ptmllpeScsiInquiryCallbackFunction                      pScsiInquiryCallback;
    ptmllpeScsiStartStopUnitCallbackFunction                pScsiStartStopUnitCallback;
    ptmllpeScsiPreventAllowMediumRemovalCallbackFunction    pScsiPreventAllowMediumRemovalCallback;
    ptmllpeScsiReadCapacityCallbackFunction                 pScsiReadCapacityCallback;
    ptmllpeScsiWrite10CallbackFunction                      pScsiWrite10Callback;
    ptmllpeScsiRead10CallbackFunction                       pScsiRead10Callback;
    ptmllpeScsiVerify10CallbackFunction                     pScsiVerify10Callback;
    ptmllpeScsiSyncCacheCallbackFunction                    pScsiSyncCacheCallback;
    ptmllpeScsiWriteBufferCallbackFunction                  pScsiWriteBufferCallback;
    ptmllpeScsiRequestSenseCallbackFunction                 pScsiRequestSenseCallback;
    ptmllpeScsiModeSelectCallbackFunction                   pScsiModeSeletCallback;
    ptmllpeScsiModeSenseCallbackFunction                    pScsiModeSenseCallback;
    ptmllpeScsiReadFormatCapacitiesCallbackFunction         pScsiReadFormatCapacitiesCallback;
    ptmllpeScsiMassStorageResetCallbackFunction             pScsiMassStorageResetCallback;
    ptmllpeScsiGetMaxLunCallbackFunction                    pScsiGetMaxLunCallback;
    ptmllpeScsiWriteBlockCallbackFunction                   pScsiWriteBlockCallback;       
    ptmllpeScsiReadBlockCallbackFunction                    pScsiReadBlockCallback;       
        
} tmllpeScsiCallback_st_t, * ptmllpeScsiCallback_st_t;


/**
 * @brief Usb Scsi subclass driver registration structure.
 *          
 * This structure defines the data needed in order to initialise the driver properly.
 * - mediumBlockLength : size (in bytes) of the medium blocks, 1024 should an integer multiple of this size 
 * - bulkOutEndpoint : physical endpoint number of the bulk out endpoint used by the Usb MsBot class driver
 * - bulkInEndpoint : physical endpoint number of the bulk in endpoint used by the Usb MsBot class driver
 * - callback : a structure containing the function pointers to the callbacks   
 *
 * @see tmllpeScsiCallback_st_t, tmllpeUsbScsi_Initialise  
 */
typedef struct tmllpeScsiRegister_st_t
{
    UInt32                      mediumBlockLength[2];
    UInt8                       bulkOutEndpoint;
    UInt8                       bulkInEndpoint;
    tmllpeScsiCallback_st_t     callback;
    
} tmllpeScsiRegister_st_t, * ptmllpeScsiRegister_st_t;


/**
* @brief:usbscsi transmit or receive buffer using information
*
* BufUsed:  	TRUE OR FALSE,buf will be or has been used
* BufToWrite	:TRUR OR FALSE,Data is valid in buffer to write
* pBufStartPtr:	buffer start address
* BufLength		:Buffer length
* LogiBlockAddrToWrite: the start logical block address to write
*/
typedef struct tmllpeScsiBufInfo_st_t
{
	Bool						BufToWrite;
	UInt8						*pBufStartPtr;
	UInt32                      BufLength;
	UInt32						LogiBlockAddrToWrite;
	UInt32						ByteToWrite;
} tmllpeScsiBufInfo_st_t;


/*---------------------------------------------------------------------------
   Global variables:
   --------------------------------------------------------------------------*/
   
/*---------------------------------------------------------------------------
   Exported Function protoypes:
   --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------- */
/**
 * @brief Usb Scsi subclass driver initialisation public function.
 *
 * This public function initialises the Usb Scsi subclass driver :
 *  - the device's storage medium block size is retrieved from the Scsi application driver
 *  - the Scsi callbacks are connected to the corresponding Scsi application driver functions
 *  - the appropriate functions are registered to the MsBot class driver callbacks
 *  - the Usb Scsi subclass driver's internals are initialised
 * This function must being called from whithin the Scsi application driver.
 *
 * @param       pScsiRegister       pointer to registration structure 
 * @returns                         none
 * @pre                             Usb Scsi subclass driver is not initialised
 * @post                            Usb Scsi subclass driver is initialised
 */
/* ----------------------------------------------------------------------------- */
void tmllpeUsbScsi_Initialise(ptmllpeScsiRegister_st_t pScsiRegister);


/*@}*/

#endif /* TM_LLPE_USB_SCSI_H_ */
