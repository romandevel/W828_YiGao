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


#ifndef TM_LLPE_SCSI_APPLN_H_
#define TM_LLPE_SCSI_APPLN_H_

/**
 * \defgroup tmllpeUsbScsiAppln tmllpeUsbScsiAppln
 * @brief The peripheral driver layer for the Scsi application.
 *
 * This Scsi application driver handles the Scsi processing related part in a Usb - 
 * Mass Storage, Bulk Only Transport class - Scsi subclass - device.  
 * At one side, it interacts with Usb Scsi subclass driver. 
 * At the other side, it interacts with a device application.
 * The Scsi application driver is responsible for the actual processing of Scsi commands.
 * It is notified of the arrival of Scsi commands in the device through a set of callback functions
 * being called from the Usb Scsi subclass. The Scsi application driver has to take appropriate action on Scsi level
 * when receiving a Scsi command callback. If a Scsi commands requires data to be sent back to the host, it 
 * is the responsibility of the Scsi application driver to provide the Usb Scsi subclass driver with this data.
 * In case bulk data has to be transfered from the host to the device or in the other direction, the Scsi application
 * driver gets data transfer callbacks from the Usb Scsi subclass driver. The Scsi application driver doesn't have to
 * take care of data management, which is done in the Usb Scsi subclass driver. When it receives such a data transfer
 * callback, the Scsi application driver has to write/read a block of data, equal to it's storage medium block size,
 * to/from the device's storage medium. 
 * On reception of Usb Mass Storage commands, the Scsi application driver is also notified of it by callbacks from the 
 * Usb Scsi subclass driver. The Scsi application driver is supposed to take appropriate actions on application level. 
 *
 * Assumptions :
 * - The Usb Device Stack (Device Controller Driver + Usb Device Function + MsBot Class Driver) 
 *   must be initialised properly and be up and running correctly.
 * - A Usb Scsi subclass driver must be present in order to interact properly with the Scsi application driver.
 * - In order to interpret the Scsi commands and corresponding parameters properly, a knowledge of the following 
 *   Scsi specs is required : SAM2 / SPC2 / RBC2 
 *
 * Functional limitations :
 * - The following Usb Mass Storage commands are supported : Bulk-Only Mass Storage Reset / Get Max LUN
 * - The following Scsi commands are supported : TEST UNIT READY / FORMAT UNIT / INQUIRY / START STOP UNIT /
 *   PREVENT ALLOW REMOVAL / READ CAPACITY / READ 10 / WRITE 10 / VERIFY 10 / SYNC CACHE / WRITE BUFFER / 
 *   REQUEST SENSE / MODE SELECT / MODE SENSE / READ FORMAT CAPACITIES
 * - Only 1 application, supporting 1 LUN, can make use of this Scsi application driver.
 * - The parameters provided with Scsi command callbacks is limited. The supported parameters should be sufficient 
 *   to implement a basic Nand Flash memory based application.
 * - 1024 should be a integer multiple of the block size of the device's storage medium.                               
 * @{
 */

/** 
 * @file
 * @brief The peripheral driver layer for the Scsi application.
 *
 * This is the peripheral driver layer for the Scsi application.
 * 
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"  
#include "hyTypes.h"
#include "hyErrors.h"


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   Global variables:
   --------------------------------------------------------------------------*/

/**
 * @brief Scsi application driver format initialised variable.
 *
 * This global variable is set when a the host computer wants to format the medium.
 */
extern Int32 gtmStandardFormatInitiated;


/**
 * @brief Scsi application Usb receive transfer status.
 *
 * This global variable is set when data is written to the device.
 */
extern Int32 gtmUsbTransferInProgressRx;


/**
 * @brief Scsi application Usb transmit transfer status.
 *
 * This global variable is set when data is read from the device.
 */
extern Int32 gtmUsbTransferInProgressTx;



/*---------------------------------------------------------------------------
   Exported Function protoypes:
   --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------- */
/**
 * @brief Scsi application driver initialisation public function.
 *
 * This public function initialises the Scsi applicatoion driver :
 *  - the device's storage medium block size is reported to the Usb Scsi subclass driver
 *  - the physical bulk out/in endpoint numbers are reported to the Usb Scsi subclass driver
 *  - the appropriate functions are registered to the Scsi class driver callbacks
 *  - the Scsi application driver's internals are initialised
 * This function must being called from whithin the device application.
 *
 * @param       none                
 * @returns     none
 * @pre         Scsi application driver is not initialized.
 * @post        Scsi application driver is initialized.
 */
/* ----------------------------------------------------------------------------- */
void tmllpeScsiAppln_Initialise(Int32 volumeHandle);


/*@}*/

#endif /* TM_LLPE_SCSI_APPLN_H_ */