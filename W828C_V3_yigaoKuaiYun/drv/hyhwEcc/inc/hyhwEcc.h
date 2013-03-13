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
#ifndef ECC_DRV_H
#define ECC_DRV_H

typedef enum {
	ecc_no_error,		/* no error */
	ecc_1_symbol_error_occurred,
	ecc_2_symbol_error_occurred,
	ecc_3_symbol_error_occurred,
	ecc_4_symbol_error_occurred,
	ecc_symbol_error_occurred_large_than_4	/* uncorrectable error */
} eECC4_DIFF;

typedef enum 
{
    ecc_completed,
    ecc_uncompleted                      
} eECC_STATUS;

typedef volatile struct 
{
	U32 ecc4_0;		//0:15
	U32 ecc4_1;		//16:47
	U32 ecc4_2;		//48:79	

}tECC4_ARRAY;

/*-----------------------------------------------------------------------------
* 函数:	hyhwDelay
* 功能:	delay 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwDelay(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc4EnCode_Start
* 功能:	start the ecc4 encode 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4EncodeStart(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc4EnCode_Stop
* 功能:	stop the ecc4 encode 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4EncodeStop(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc_Get
* 功能:	get the ecc code from the ecc4 code register 
* 参数:	ecc4_array
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_getEcc4( tECC4_ARRAY * ptEcc4Arry );
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc4_Set
* 功能:	set the ecc code to the ecc4 code register 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_setEcc4( void );
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc4_Decode_Start
* 功能:	start he ecc4 decode 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4DecodeStart(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc4_Decode_Stop
* 功能:	stop he ecc4 decode 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4DecodeStop(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwSetEccBaseAddr
* 功能:	set the ecc4 encode or decode base address 
* 参数:	base,mask
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_setEccBaseAddr(U32 base, U32 mask );
/*-----------------------------------------------------------------------------
* 函数:	hyhwMlcEcc4DecodingInterruptEnable
* 功能:	set the ecc4 decode interrupt, & if the decode is complete,there is an interrupt be occure 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4DecodingInterruptEnable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwMlcEcc4EncodingInterruptEnable
* 功能:	set the ecc4 encode interrupt, & if the encode is complete,there is an interrupt be occure 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4EncodingInterruptEnable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccWaitEncodingCompleted
* 功能:	wait for encode complete 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_waitEncodingCompleted(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccDmaRequestEnable
* 功能:	do ecc when data transfer use DMA 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dmaRequestEnable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccDmaRequestDisable
* 功能:	disable ecc when data DMA transfer  complete 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dmaRequestDisable(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccDataSize
* 功能:	set the size of data which be decode or encode 
* 参数:	size
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dataSize(U32 size);
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccWaitDecodingCompleted
* 功能:	waiting for decode complete 
* 参数:	none
* 返回:	apECC_status
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_waitDecodingCompleted( void );
/*-----------------------------------------------------------------------------
* 函数:	hyhwEccDecodingInterruptReqOcc
* 功能:	to indicate that is there an decoding interrupt  occure 
* 参数:	none
* 返回:	apECC_status
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_decodingInterruptReqOcc( void );
/*-----------------------------------------------------------------------------
* 函数:	hyhwReadMlcEcc4ErrorCorrect
* 功能:	to indicate that is it need error correct 
* 参数:	* pdata
* 返回:	eccdiff_t
*----------------------------------------------------------------------------*/
eECC4_DIFF hyhwEcc_readMlcEcc4ErrorCorrect( U8 * pdata );
/*-----------------------------------------------------------------------------
* 函数:	hyhwEcc_rwNandMlcEcc4Set
* 功能:	set the Ecc register before nand data read 
* 参数:	nandRwPortAddr , eccDataSize
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_rwNandMlcEcc4Set( U32 nandRwPortAddr, U32 eccDataSize );

#endif //ECC_DRV_H