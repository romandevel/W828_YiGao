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


#ifndef HYTCC7901USB_DEVICE_ENUM_H
#define HYTCC7901USB_DEVICE_ENUM_H



extern U8   CurrentlyHighSpeed;    //hyudsCore_EdpHl.c;	

/******************************************
* Function            : void USBSetup_RequestHandler( U8* pkSetupCmd)
* Description         : Requests Porcessing Routines, process standard / Class request in Chapter 9
*	                       The device has single config and single interface, therefore, the routine is much simple
* input     : U8* pkpkSetupDataa - U8 type pointer to the request (8 bytes)
* output    : none
* return    : none
* ----------------------------------------------------------------------------													  
* NOTE1 : The Request Command and Return Data are all transmitted by endpoint0
* NOTE2 : format of Setup Data, the length always be fixed to 8 bytes
	unsigned char		bmRequestType;	// bit[7] Transfer Dir 0: OUT, 1: IN; 
									// bit[6:5] Type 0-Standard; 1-Class; 2-Vendor; 3-Reserved
									// bit[4:0] Recipient 0-Device; 1-Interface; 2-Endpoint; 3-Other; 
	unsigned char		bRequest;   // specific request, definition refer to above defines
	unsigned char		wValue[2];	// word-sized field that varies according to request
	unsigned char		wIndex[2];	// word-sized field that varies according to request, typically used to pass an index or offset
	unsigned char		wLength[2];	// Number of bytes to transfer if there is a data stage
*******************************************/
void USBSetup_RequestHandler(U8 *pData);

/*---------------------------------------------------------------
2004-7-22  Zhou Jie    Initial version

----------------------------------------------------------------*/

#endif//HYTCC7901USB_DEVICE_ENUM_H

