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


#ifndef HYTCC7901USB_DEVICE_RDWRITE_H
#define HYTCC7901USB_DEVICE_RDWRITE_H

/*************************************************************************************
 *          hyhwUsb_TransmitData
 *
 * Description  :将buffer中的数据上传给PC
 * Argument     :pBuffer: 所要传送数据的指针;BufferSize:所要传输的字节数 
 * Return       :
 *
 *************************************************************************************/
unsigned int hyhwUsb_TransmitData( U8* pBuffer, U32 BufferSize );

/*************************************************************************************
 *          hyhwUsb_CheckDataReceived
 *
 * Description  :查询是否接收到pc下传的数据
 * Argument     :
 * Return       :0：表示没有数据，其余数据表示接收到的数据个数
 * note:         
 *************************************************************************************/
 unsigned int hyhwUsb_CheckDataReceived(void);
 
 /*************************************************************************************
 *          hyhwUsb_Dispatch
 *
 * Description  :用查询方式检查是否有数据过来
 * Argument     :
 * Return       :
 * note			:
 *************************************************************************************/
void   hyhwUsb_Dispatch ( void ) ;

#endif//HYTCC7901USB_DEVICE_RDWRITE_H




