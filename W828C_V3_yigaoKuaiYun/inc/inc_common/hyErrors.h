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

#ifndef _HY_ERROR_H_
#define _HY_ERROR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*   HY_OK is the 32 bit global status value used by all Nexperia components   */
/*   to indicate successful function/operation status.                         */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
#define HY_OK                     		0         	/* Global success return status */
#define HY_ERROR                   		0xFFFFFFFF	/* Global error return status */

#define HY_ERR_COMPATIBILITY            0x001 /* SW Interface compatibility   */
#define HY_ERR_MAJOR_VERSION            0x002 /* SW Major Version error       */
#define HY_ERR_COMP_VERSION             0x003 /* SW component version error   */
#define HY_ERR_BAD_MODULE_ID            0x004 /* SW - HW module ID error      */
#define HY_ERR_BAD_UNIT_NUMBER          0x005 /* Invalid device unit number   */
#define HY_ERR_BAD_INSTANCE             0x006 /* Bad input instance value     */
#define HY_ERR_BAD_HANDLE               0x007 /* Bad input handle             */
#define HY_ERR_BAD_INDEX                0x008 /* Bad input index              */
#define HY_ERR_BAD_PARAMETER            0x009 /* Invalid input parameter      */
#define HY_ERR_NO_INSTANCES             0x00A /* No instances available       */
#define HY_ERR_NO_COMPONENT             0x00B /* Component is not present     */
#define HY_ERR_NO_RESOURCES             0x00C /* Resource is not available    */
#define HY_ERR_INSTANCE_IN_USE          0x00D /* Instance is already in use   */
#define HY_ERR_RESOURCE_OWNED           0x00E /* Resource is already in use   */
#define HY_ERR_RESOURCE_NOT_OWNED       0x00F /* Caller does not own resource */
#define HY_ERR_INCONSISTENT_PARAMS      0x010 /* Inconsistent input params    */
#define HY_ERR_NOT_INITIALIZED          0x011 /* Component is not initialized */
#define HY_ERR_NOT_ENABLED              0x012 /* Component is not enabled     */
#define HY_ERR_NOT_SUPPORTED            0x013 /* Function is not supported    */
#define HY_ERR_INIT_FAILED              0x014 /* Initialization failed        */
#define HY_ERR_BUSY                     0x015 /* Component is busy            */
#define HY_ERR_NOT_BUSY                 0x016 /* Component is not busy        */
#define HY_ERR_READ                     0x017 /* Read error                   */
#define HY_ERR_WRITE                    0x018 /* Write error                  */
#define HY_ERR_ERASE                    0x019 /* Erase error                  */
#define HY_ERR_LOCK                     0x01A /* Lock error                   */
#define HY_ERR_UNLOCK                   0x01B /* Unlock error                 */
#define HY_ERR_OUT_OF_MEMORY            0x01C /* Memory allocation failed     */
#define HY_ERR_BAD_VIRT_ADDRESS         0x01D /* Bad virtual address          */
#define HY_ERR_BAD_PHYS_ADDRESS         0x01E /* Bad physical address         */
#define HY_ERR_TIMEOUT                  0x01F /* Timeout error                */
#define HY_ERR_OVERFLOW                 0x020 /* Data overflow/overrun error  */
#define HY_ERR_FULL                     0x021 /* Queue (etc.) is full         */
#define HY_ERR_EMPTY                    0x022 /* Queue (etc.) is empty        */
#define HY_ERR_NOT_STARTED              0x023 /* Component stream not started */
#define HY_ERR_ALREADY_STARTED          0x024 /* Comp. stream already started */
#define HY_ERR_NOT_STOPPED              0x025 /* Component stream not stopped */
#define HY_ERR_ALREADY_STOPPED          0x026 /* Comp. stream already stopped */
#define HY_ERR_ALREADY_SETUP            0x027 /* Component already setup      */
#define HY_ERR_NULL_PARAMETER           0x028 /* Null input parameter         */
#define HY_ERR_NULL_DATAINFUNC          0x029 /* Null data input function     */
#define HY_ERR_NULL_DATAOUTFUNC         0x02A /* Null data output function    */
#define HY_ERR_NULL_CONTROLFUNC         0x02B /* Null control function        */
#define HY_ERR_NULL_COMPLETIONFUNC      0x02C /* Null completion function     */
#define HY_ERR_NULL_PROGRESSFUNC        0x02D /* Null progress function       */
#define HY_ERR_NULL_ERRORFUNC           0x02E /* Null error handler function  */
#define HY_ERR_NULL_MEMALLOCFUNC        0x02F /* Null memory alloc function   */
#define HY_ERR_NULL_MEMFREEFUNC         0x030 /* Null memory free  function   */
#define HY_ERR_NULL_CONFIGFUNC          0x031 /* Null configuration function  */
#define HY_ERR_NULL_PARENT              0x032 /* Null parent data             */
#define HY_ERR_NULL_IODESC              0x033 /* Null in/out descriptor       */
#define HY_ERR_NULL_CTRLDESC            0x034 /* Null control descriptor      */
#define HY_ERR_UNSUPPORTED_DATACLASS    0x035 /* Unsupported data class       */
#define HY_ERR_UNSUPPORTED_DATATYPE     0x036 /* Unsupported data type        */
#define HY_ERR_UNSUPPORTED_DATASUBTYPE  0x037 /* Unsupported data subtype     */
#define HY_ERR_FORMAT                   0x038 /* Invalid/unsupported format   */
#define HY_ERR_INPUT_DESC_FLAGS         0x039 /* Bad input  descriptor flags  */
#define HY_ERR_OUTPUT_DESC_FLAGS        0x03A /* Bad output descriptor flags  */
#define HY_ERR_CAP_REQUIRED             0x03B /* Capabilities required ???    */
#define HY_ERR_BAD_TMALFUNC_TABLE       0x03C /* Bad TMAL function table      */
#define HY_ERR_INVALID_CHANNEL_ID       0x03D /* Invalid channel identifier   */
#define HY_ERR_INVALID_COMMAND          0x03E /* Invalid command/request      */
#define HY_ERR_STREAM_MODE_CONFUSION    0x03F /* Stream mode config conflict  */
#define HY_ERR_UNDERRUN                 0x040 /* Data underflow/underrun      */
#define HY_ERR_EMPTY_PACKET_RECVD       0x041 /* Empty data packet received   */
#define HY_ERR_OTHER_DATAINOUT_ERR      0x042 /* Other data input/output err  */
#define HY_ERR_STOP_REQUESTED           0x043 /* Stop in progress             */
#define HY_ERR_PIN_NOT_STARTED          0x044 /* Pin not started              */
#define HY_ERR_PIN_ALREADY_STARTED      0x045 /* Pin already started          */
#define HY_ERR_PIN_NOT_STOPPED          0x046 /* Pin not stopped              */
#define HY_ERR_PIN_ALREADY_STOPPED      0x047 /* Pin already stopped          */
#define HY_ERR_PAUSE_PIN_REQUESTED      0x048 /* Pause of single pin in progrs*/
#define HY_ERR_ASSERTION                0x049 /* Assertion failure            */
#define HY_ERR_HIGHWAY_BANDWIDTH        0x04A /* Highway bandwidth bus error  */
#define HY_ERR_HW_RESET_FAILED          0x04B /* Hardware reset failed        */
#define HY_ERR_PIN_PAUSED               0x04C /* Pin paused                   */
#define HY_ERR_BAD_FLAGS                0x04D /* Bad flags                    */
#define HY_ERR_BAD_PRIORITY             0x04E /* Bad priority                 */
#define HY_ERR_BAD_REFERENCE_COUNT      0x04F /* Bad reference count          */
#define HY_ERR_BAD_SETUP                0x050 /* Bad setup                    */
#define HY_ERR_BAD_STACK_SIZE           0x051 /* Bad stack size               */
#define HY_ERR_BAD_TEE                  0x052 /* Bad tee                      */
#define HY_ERR_IN_PLACE                 0x053 /* In place                     */
#define HY_ERR_NOT_CACHE_ALIGNED        0x054 /* Not cache aligned            */
#define HY_ERR_NO_ROOT_TEE              0x055 /* No root tee                  */
#define HY_ERR_NO_TEE_ALLOWED           0x056 /* No tee allowed               */
#define HY_ERR_NO_TEE_EMPTY_PACKET      0x057 /* No tee empty packet          */
#define HY_ERR_NULL_PACKET              0x059 /* Null packet                  */
#define HY_ERR_FORMAT_FREED             0x05A /* Format freed                 */
#define HY_ERR_FORMAT_INTERNAL          0x05B /* Format internal              */
#define HY_ERR_BAD_FORMAT               0x05C /* Bad format                   */
#define HY_ERR_FORMAT_NEGOTIATE_SUBCLASS    0x05D /* Format negotiate subclass*/
#define HY_ERR_FORMAT_NEGOTIATE_DATASUBTYPE 0x05E /* Format negot. datasubtype*/
#define HY_ERR_FORMAT_NEGOTIATE_DATATYPE    0x05F /* Format negotiate datatype*/
#define HY_ERR_FORMAT_NEGOTIATE_DESCRIPTION 0x060 /* Format negot. description*/
#define HY_ERR_NULL_FORMAT              0x061 /* Null format                  */
#define HY_ERR_FORMAT_REFERENCE_COUNT   0x062 /* Format reference count       */
#define HY_ERR_FORMAT_NOT_UNIQUE        0x063 /* Format not unique            */
#define HY_ERR_NEW_FORMAT               0x064 /* New format                   */

#define HY_ERR_FAT32_FORMAT             0x065 /* FAT32 format                 */

/*	MCI error type */
#define HY_ERR_MCI_INTINUSE				0x100 /*Interrupt already in use*/
#define HY_ERR_MCI_NOTINIT				0x101 /*MCI not yet initialised*/
#define HY_ERR_MCI_NOPOWER				0x102 /*unable to power up*/
#define HY_ERR_MCI_INTFAIL				0x103 /*Fails to obtain interrupt*/
#define HY_ERR_MCI_CLKILLEGAL			0x104 /*Illegal clock divider*/
#define HY_ERR_MCI_NO_BUFFER			0x105 /*No buffer specified for data*/
#define HY_ERR_MCI_COMFAIL				0x106 /*Command failed*/
#define HY_ERR_MCI_COMBUSY				0x107 /*Command currently being processed*/
#define HY_ERR_MCI_INDEX_MISMATCH		0x108 /*Response index does not match command index*/
#define HY_ERR_MCI_NO_CARD				0x109 /*Card relative ID does not exist*/
#define HY_ERR_MCI_INVALID_BLOCK		0x10A /*Data Length not multiple of block size*/
#define HY_ERR_MCI_TOO_LONG				0x10B /*Data Length exceeds ??kB*/
#define HY_ERR_MCI_INVALID_COMMAND		0x10C /*Command not allowed (for this card)*/
#define HY_ERR_MCI_WP_VIOLATION			0x10D /*Write command attempted on write-protected card*/
#define HY_ERR_MCI_LAST					0x10E /*dummy marker*/
#define HY_ERR_MCI_CARD_NOT_DETECTED	0x10F /*card not dectected or plugged out*/


/*	Module Nandflash error type */
#define HY_ERR_NAND_NO_FLASH			0x200
#define HY_ERR_NAND_INIT_FAILED			0x201
#define HY_ERR_NAND_ERASE_FAILED		0x202


#ifdef __cplusplus
}
#endif

#endif /* _HY_ERROR_H_  */
