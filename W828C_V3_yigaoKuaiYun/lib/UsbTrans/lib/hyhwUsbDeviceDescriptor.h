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

/******************************************************************************
 *  file		   : hyhwUsbDeviceRdWrite.h (Hyctron Usb Device Stack - Device Control)
 *  Created_by     : Ice wang
 *  Creation date  : 2008 05 27 15: 30
 *  Comment        : 
 *****************************************************************************/


#ifndef HYTCC7901USB_DEVICE_DESCRIPTOR_H
#define HYTCC7901USB_DEVICE_DESCRIPTOR_H


/*************************************************************************
 * DEFINES  USB ID's
 *************************************************************************/
//telechips USB VID	0x140E
#if 1
	#define USB_VID_HIGH                    0x14    /* USB Vendor Id high byte */
	#define USB_VID_LOW                     0x0e    /* USB Vendor Id low byte */
	#define USB_PID_HIGH                    0x20    /* USB Product Id high byte */
	#define USB_PID_LOW                     0x01    /* USB Product Id low byte */
#else
	#define USB_VID_HIGH                    0x40    /* USB Vendor Id high byte */
	#define USB_VID_LOW                     0x95    /* USB Vendor Id low byte */
	#define USB_PID_HIGH                    0x10    /* USB Product Id high byte */
	#define USB_PID_LOW                     0x05    /* USB Product Id low byte */
#endif

#define USB_DID_HIGH                    0x00    /* USB Device  Id high byte */
#define USB_DID_LOW                     0x01    /* USB Device Id low byte */

#define	USB_POWER_MODE					0xa0	//Enter 0xC0 for self powered or 0x80 for bus powered
#define	USB_CURRENT						0xe1	//from the bus in units of 2mA. (I.E. 0xe1 = 450mA)

#define	MAXPACKETSIZE_CTRL_EP			64
#define	MAXPACKETSIZE_BULK_EP_USB20		512
#define	MAXPACKETSIZE_BULK_EP_USB11		64

//Global declarations: Chapter 9 and mass storage class spec. and related strings

//device descriptor of Mass Storage Class 
extern const unsigned char gUSB_DeviceDescriptors_USB20[] ;
extern const unsigned char gUSB_DeviceDescriptors_USB11[] ;

// configuration descriptor of Mass Storage Class
// interface descriptor
// bulk IN endpoint descriptor
// bulk OUT endpoint descriptor
extern const unsigned char gUSB_ConfigDescriptors_USB20[] ;
extern const unsigned char gUSB_ConfigDescriptors_USB11[] ;

//String descriptor Index 0:  Language ID descriptor
extern const unsigned char gUSB_Language_StrDescriptors[] ;

//String descriptor Index 1: Manufacturer descriptor
extern const unsigned char gUSB_Manufacturer_StrDescriptors[] ;

//String descriptor Index 2:  Product descriptor
extern const unsigned char gUSB_Product_StrDescriptors[] ;

//String descriptor Index 3:Interface Name descriptor
extern const unsigned char gUSB_InterfaceName_StrDescriptors[] ;

//String descriptor Index 4: Serial Number descriptor
extern const unsigned char gUSB_SerialNumber_StrDescriptors[];



#endif //HYTCC7901USB_DEVICE_DESCRIPTOR_H

 
