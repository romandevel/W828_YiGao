
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
#include "hyTypes.h"
#include "cdmasmsapi.h"
#include "cdmasmsutil.h"
#include "usock.h"

/*********************************************************************
    FUNCTION NAME:  HexBcdToChar
    DESCRIPTION:    
    DEPENDENCIES:
    ARGUMENT:
    RETURN VALUE:
*********************************************************************/
static char HexBcdToChar(uint8 HexBcd)
{
    if( HexBcd <= 9 )
    {
        return '0' + HexBcd;
    }

    if( (HexBcd >= 10) && (HexBcd <= 15) )
    {
        return 'A' + (HexBcd - 10);
    }

    return '\0';
}

/******************************************************************************
    FUNCTION NAME:  PduToHexString1
    DESCRIPTION:
    ARGUMENTS:
    RETURN VALUE:
*****************************************************************************/
static bool PduToHexString1(char* HexStringP, const uint8* PduMsgP, uint32 PduLen)
{
    uint8   HexBcd;
    uint32  i;

    if( PduLen > CDMA_SMS_MAX_SMS_PDU_LEN )
    {
        return CDMA_FALSE;
    }

    for( i = 0; i < PduLen; i++ )
    {
        HexBcd = (PduMsgP[i] >> 4) & 0x0F;
        HexStringP[i * 2 ] = HexBcdToChar(HexBcd);

        HexBcd = PduMsgP[i] & 0x0F;        
        HexStringP[i * 2 + 1] = HexBcdToChar(HexBcd);        
    }

    HexStringP[i * 2] = '\0';

    return CDMA_TRUE;
}
#if 0
/*****************************************************************************
    FUNCTION NAME:  GsmDebug
    DESCRIPTION:                    
    PARAMETERS:	    
    RETURNS:
*****************************************************************************/
static void Debug(char *fmt, ...)
{
    char 	Info[64];
    uint16	Len;
    va_list args;

	memset( Info, 0, sizeof(Info) );
	
#if 0	
    snprintf( Info, sizeof(Info), "\n%s:  Line%ld:  %s:  ", FileNameP, Line, FuncNameP);
#endif

    Len = strlen(Info);
    
    va_start(args, fmt);

    vsnprintf(Info + Len, sizeof(Info) - Len, fmt, args);

    va_end(args);

    Len = strlen(Info);

    if( (Len + 3) < sizeof(Info) )
    {
        strcat( Info, "\n" );
    }

    MonPrintf(Info);  
    
    return;
}
#endif
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsSetTimeStamp
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static void CdmaSmsSetTimeStamp (uint16 Year, uint8 Month, uint8 Day, uint8 Hour, uint8 Minute, uint8 Second, CdmaSmsAbsTimeT*  TimeStampP)
{
#define CDMA_SMS_YEAR_OFFSET    (2000)

    Year -= CDMA_SMS_YEAR_OFFSET;

    TimeStampP->Year    = (((Year / 10) & 0x0f) << 4) | ((Year % 10) & 0x0f);
    TimeStampP->Month   = (((Month / 10) & 0x0f) << 4) | ((Month % 10) & 0x0f);
    TimeStampP->Day     = (((Day / 10) & 0x0f) << 4) | ((Day % 10) & 0x0f);
    
    TimeStampP->Hours   = (((Hour / 10) & 0x0f) << 4) | ((Hour % 10) & 0x0f);
    TimeStampP->Minutes = (((Minute / 10) & 0x0f) << 4) | ((Minute % 10) & 0x0f);
    TimeStampP->Seconds = (((Second / 10) & 0x0f) << 4) | ((Second % 10) & 0x0f);
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsDialStrConvert
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static bool CdmaSmsDialStrConvert(char* DestDataP, const char* SrcDataP)
{
    uint8   i, DialLen;
    
    DialLen = strlen( (char*)SrcDataP );

    for( i = 0; i < DialLen; i++ )
    {
        if ( (i == 0) && (SrcDataP[i] == '+') )
        {
            *DestDataP = '+';
            DestDataP++;
            continue;
        }

        if( (SrcDataP[i] >= '0') && (SrcDataP[i] <= '9') )
        {
        }
        else if( (SrcDataP[i] == '#') && (SrcDataP[i] == '*') )
        {
        }
        else
        {
            return CDMA_FALSE;
        }            
                    
        *DestDataP++ = SrcDataP[i];
    }

    *DestDataP = '\0';

    return CDMA_TRUE;
}
/*********************************************************************
    FUNCTION NAME:  CdmaSmsAddrCharToBcd
    DESCRIPTION:    
    DEPENDENCIES:
    ARGUMENTS:
    RETURN VALUE:
********************************************************************/
static uint8 CdmaSmsAddrCharToBcd(char DigitChar)
{
 	uint8 BcdCode;

  	switch( DigitChar )
  	{
    case '0':
      	BcdCode = 10;
      	break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      	BcdCode = (char) (DigitChar-(int8)'0');
      	break;

    case '*':
      	BcdCode = 11;
      	break;

    case '#':
      	BcdCode = 12;
      	break;

    default:
      	/* invalid digit .*/
      	BcdCode = 0xFF;
      	break;
  	}

  	return (BcdCode);
}

/*****************************************************************************
    FUNCTION NAME:  CdmaSmsAddrAsciiToBcdCode
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static bool CdmaSmsAddrAsciiToBcdCode(uint8* DestDataP, const char *SrcDataP, uint8 DigitCount)
{
    uint8   i;
    uint8   BcdCode;

    if ( (DestDataP == NULL) || (SrcDataP == NULL) ||((uint8*)SrcDataP == DestDataP) )
    {
        return CDMA_FALSE;
    }

    if (DigitCount > CDMA_SMS_MAX_ADDRESS_LEN)
    {
        return CDMA_FALSE;
    }

    for( i = 0; i < DigitCount; )
    {
        BcdCode = CdmaSmsAddrCharToBcd(SrcDataP[i]);

        if(BcdCode == 0xFF)
        {
            return CDMA_FALSE;
        }

        DestDataP[i / 2] = ( BcdCode << 4 ) & 0xF0;

        if (i == (DigitCount - 1))
        {
            break;
        }
        
        BcdCode = CdmaSmsAddrCharToBcd( SrcDataP[i + 1] );

        if( BcdCode == 0xFF )
        {
            return CDMA_FALSE;
        }

        DestDataP[i / 2] = DestDataP[i / 2] | (BcdCode & 0x0F);

        i += 2;
    }

    return CDMA_TRUE; /* return number of valid digits */
}

/*****************************************************************************
    FUNCTION NAME:  CdmaSmsSetAddress
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static bool CdmaSmsSetAddress(char *AddressP, CdmaSmsAddressT *SmsAddressP)
{
    uint32  AddrLen;
    char    SmsDestAddr[CDMA_SMS_MAX_ADDRESS_LEN + 1];
    
    AddrLen = strlen(AddressP);
    
    /* check the address */
    if( AddrLen == 0 )
    {
        return CDMA_FALSE;
    }        
    
    if( (AddrLen > CDMA_SMS_MAX_ADDRESS_LEN) 
            || ( (AddressP[0] == '+') && (AddrLen > (CDMA_SMS_MAX_ADDRESS_LEN  - 1))) )
    {
        return CDMA_FALSE;
    }                        
    
    memset( SmsAddressP, 0, sizeof(CdmaSmsAddressT) );
    
    SmsAddressP->DigitMode    = CDMA_SMS_DIGIT_MODE_4_BIT;
    SmsAddressP->NumberPlan   = CDMA_SMS_NUMBER_PLAN_UNKNOWN;
    SmsAddressP->NumberMode   = CDMA_SMS_NUMBER_MODE_ANSI_T1_607;
    
    memset( SmsDestAddr, 0, sizeof(SmsDestAddr) );
    
    if ( !CdmaSmsDialStrConvert(SmsDestAddr, AddressP) )
    {        
        return CDMA_FALSE;
    }
    
    SmsAddressP->NumFields = strlen( (char *)SmsDestAddr );
    
    if ( SmsDestAddr[0] == '+' )
    {
        SmsAddressP->NumberType.NumType   = CDMA_SMS_NUMBER_TYPE_INTERNATIONAL;     //  值 为 1
        SmsAddressP->DigitMode            = CDMA_SMS_DIGIT_MODE_8_BIT;  //   1
        SmsAddressP->NumFields--;
    
        memcpy( (uint8 *)SmsAddressP->Address, &SmsDestAddr[1], SmsAddressP->NumFields );    
    }
    else
    {
        if ( !CdmaSmsAddrAsciiToBcdCode( (uint8 * )SmsAddressP->Address, SmsDestAddr, SmsAddressP->NumFields ) )
        {
            return CDMA_FALSE;
        }
    }
    
    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsSetAddress
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static bool CdmaSmsSetCallBackNum(char *CallBackNumP, CdmaSmsCbNumberT *SmsCallBackNumP)
{
    uint32  Len;
    char    SmsDestAddr[CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS + 1];
    
    Len = strlen(CallBackNumP);
    
    /* check the address */
    if( Len == 0 )
    {
        return CDMA_FALSE;
    }        
    
    if( (Len > CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS) 
            || ( (CallBackNumP[0] == '+') && (Len > (CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS - 1))) )
    {
        return CDMA_FALSE;
    }                        
    
    memset( (void *)SmsCallBackNumP, 0, sizeof(CdmaSmsCbNumberT) );
    
    SmsCallBackNumP->DigitMode      = CDMA_SMS_DIGIT_MODE_4_BIT;
    SmsCallBackNumP->NumberPlan     = CDMA_SMS_NUMBER_PLAN_UNKNOWN;
     SmsCallBackNumP->NumberType     = CDMA_SMS_NUMBER_TYPE_INTERNATIONAL;
    
    memset( SmsDestAddr, 0, sizeof(SmsDestAddr) );
    
    if ( !CdmaSmsDialStrConvert(SmsDestAddr, CallBackNumP) )
    {
        return CDMA_FALSE;
    }
    
    SmsCallBackNumP->NumFields = strlen( (char *)SmsDestAddr );
    
    if ( SmsDestAddr[0] == '+' )
    {
        SmsCallBackNumP->NumberType     = CDMA_SMS_NUMBER_TYPE_INTERNATIONAL;
        SmsCallBackNumP->DigitMode      = CDMA_SMS_DIGIT_MODE_8_BIT;
        SmsCallBackNumP->NumFields--;
    
        memcpy( (uint8 *)SmsCallBackNumP->Digits, &SmsDestAddr[1], SmsCallBackNumP->NumFields );
    
    }
    else
    {
        if ( !CdmaSmsAddrAsciiToBcdCode( (uint8 * )SmsCallBackNumP->Digits, SmsDestAddr, SmsCallBackNumP->NumFields ) )
        {
            return CDMA_FALSE;
        }
    }
    
    return CDMA_TRUE;
}
/*****************************************************************************************
    FUNCTION NAME:  CdmaSmsBitPackB
    DESCRIPTION:
    DEPENDENCIES:
    RETURN VALUE:
******************************************************************************************/
static void CdmamsBitPackB(uint8 Val, uint8 *BufP, uint16 Pos, uint16 Len)
{
    uint16  BitPosToPack = Pos % 8;
    uint16  BitLeftInByte   = 8 - BitPosToPack;
    

    BufP += (Pos + Len - 1) / 8;

    if ( Len > BitLeftInByte )
    {
        BufP--;

        *BufP &= (uint8) ~(BYTE_MASK(BitPosToPack, BitLeftInByte));
        *BufP |= (uint8) ( BYTE_MASK(BitPosToPack, BitLeftInByte)
                            & (Val >> (Len - BitLeftInByte)) );

        BufP++;

        *BufP &= (uint8) ~( BYTE_MASK(0, (Len - BitLeftInByte)) );
        *BufP |= (uint8) ( BYTE_MASK(0, (Len - BitLeftInByte))
                            & (Val << (8 - (Len - BitLeftInByte))) );
    }
    else /* ( bits_left_in_a_uint8 >= len ) */
    {
        *BufP &= (uint8) ~( BYTE_MASK(BitPosToPack, Len) );
        *BufP |= (uint8) ( BYTE_MASK(BitPosToPack, Len)
                            & (Val << (BitLeftInByte - Len)) );
    }
}
/**********************************************************************************
    FUNCTION NAME:  CdmaSmsTextTo7BitStream
    DESCRIPTION:
    ARGUMENTS:
    RETURN VALUE:
*************************************************************************************/
static bool CdmaSmsTextTo7BitStream(uint8* DestDataP, const uint8* SrcDataP, uint16 NumFields)
{
    uint8       Code;
    uint16      i;
    uint32      BitPos;

    if ( (DestDataP == NULL) || (SrcDataP == NULL) || (SrcDataP == DestDataP) )
    {
        return CDMA_FALSE;
    }

    BitPos = 0;
    
    for( i = 0; i < NumFields; i++ )
    {
        Code = SrcDataP[i] & 0x7F;
        
        CdmamsBitPackB(Code, DestDataP, BitPos, 7);
        BitPos += 7;
    }

    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsEncodeUserData
    DESCRIPTION:
    PARAMETERS:
    RETURN VALUE:
*****************************************************************************/
static bool CdmaSmsEncodeUserData(uint8* DestDataP, const uint8* SrcDataP, uint16 NumFields, uint8 Encode)
{
#define CDMA_SMS_MAX_8BIT_USER_DATA_LEN     (140)
#define CDMA_SMS_MAX_7BIT_USER_DATA_LEN     (160)
#define CDMA_SMS_MAX_16BIT_USER_DATA_LEN    (70)
    
    uint16  DataLen;
    
    if( (DestDataP == NULL) || (SrcDataP == NULL) || (SrcDataP == DestDataP) )
    {
        return CDMA_FALSE;
    }

    switch(Encode)
    {
    case CDMA_SMS_MSG_ENCODE_OCTET:
    case CDMA_SMS_MSG_ENCODE_LATIN:
    case CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG:
    case CDMA_SMS_MSG_ENCODE_LATIN_HEBREW:
        if( NumFields > CDMA_SMS_MAX_8BIT_USER_DATA_LEN)
        {
            return CDMA_FALSE;
        }            
        memcpy( DestDataP, SrcDataP, NumFields);
        break;

    case CDMA_SMS_MSG_ENCODE_7BIT_ASCII:
    case CDMA_SMS_MSG_ENCODE_IA5:
        if( NumFields > CDMA_SMS_MAX_7BIT_USER_DATA_LEN )
        {
            return CDMA_FALSE;
        }            
        DataLen = CdmaSmsTextTo7BitStream(DestDataP, SrcDataP, NumFields);
        break;

    case CDMA_SMS_MSG_ENCODE_UNICODE:
        if( NumFields > CDMA_SMS_MAX_16BIT_USER_DATA_LEN )
        {
            return CDMA_FALSE;
        }
        memcpy( DestDataP, SrcDataP, NumFields * 2);                               
        break;

    default:
        return CDMA_FALSE;
    }

    return CDMA_TRUE;
}
/*
函数：CdmaDataSendTypeGet
功能：得到发送短信数据的发送类型
参数：pSrcdata---------界面上传来的待发送数据
返回：
*/
CdmaSmsMsgEncodeT CdmaDataSendTypeGet(U8 *pSrcdata)
{
	unsigned short int i,nLen;
	nLen=strlen(pSrcdata);
	for(i=0;i<nLen;i++)
	{
		if(0x80 <= pSrcdata[i])
		{
			return CDMA_SMS_MSG_ENCODE_UNICODE;
		}
     
	}
	return CDMA_SMS_MSG_ENCODE_7BIT_ASCII;	
}
void cdma_sendcell_get(tCDMA_SEND_PEMER *pSendCell,U8 *phone_num,U8 *data,U8 coding_scheme,U16 data_length)
{
	strcpy(pSendCell->pPhoneNum,phone_num);
	memcpy(pSendCell->pData,data,data_length);
	pSendCell->dataLen = data_length;
	if(ALPHABET_DEFAULT==coding_scheme)
	{
		pSendCell->dataType = CDMA_SMS_MSG_ENCODE_7BIT_ASCII;
	}
	else if(ALPHABET_UCS2==coding_scheme)
	{
		pSendCell->dataType = coding_scheme = CDMA_SMS_MSG_ENCODE_UNICODE;
	}
	//pSendCell->timeStamp = gtRtc_solar;//ZDDDZ
	rtcSolar_get(&pSendCell->timeStamp);
}

/*****************************************************************************
    FUNCTION NAME:  CdmaSmsSedPduMsg
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/ 
bool CdmaSmsWritePdu(tCDMA_SEND_PEMER *pSendCell,U8 *send_data_str,U16 *data_length)
{//U8 *pPhoneNum,U8 *UserDataString
    uint8   SmsLength=-1;
    uint8   SmsPduBuf[CDMA_SMS_MAX_SMS_PDU_LEN];
    uint8   SmsPduBufString[CDMA_SMS_MAX_SMS_PDU_LEN * 2 + 1];
   // static char UserDataString[4];
	CdmaSmsMessageT CdmaSmsMsgP;
    
    CdmaSmsMsgP.FieldMask = 0;

    /* Time Stamp is the time to send, it can be empty */
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_MC_TIME_STAMP_PRESENT;  //0x20
    CdmaSmsSetTimeStamp( pSendCell->timeStamp.year, pSendCell->timeStamp.month,pSendCell->timeStamp.day
						, pSendCell->timeStamp.hour, pSendCell->timeStamp.minute, 0, &(CdmaSmsMsgP.TimeStamp) );

    /* Teleservice. */
    CdmaSmsMsgP.TeleSrvId = 4098;

    /* language */
    CdmaSmsMsgP.FieldMask    |= CDMA_SMS_LANGUAGE_IND_PRESENT;
    CdmaSmsMsgP.Lang         = CDMA_SMS_LANGUAGE_ENGLISH;

    /* Address */
    CdmaSmsMsgP.FieldMask             |= CDMA_SMS_ADDRESS_PRESENT;
    
    CdmaSmsMsgP.Address.NumberPlan    = CDMA_SMS_NUMBER_PLAN_UNKNOWN;
    CdmaSmsMsgP.Address.NumberMode    = CDMA_SMS_NUMBER_MODE_ANSI_T1_607;

    /***********************************************************************************************
        目前中国联通的CDMA手机发送到中国移动的GSM手机，+86或者0086开头的号码虽然能够表示发送成功，
        但是实际上GSM手机仍然无法接收到CDMA手机发送的短消息
        在呼叫过程中，+86是替换成0086，否则电话无法呼叫成功。
    ************************************************************************************************/

    if( !CdmaSmsSetAddress(pSendCell->pPhoneNum, &(CdmaSmsMsgP.Address)) )
    {
        return CDMA_FALSE;
    }        
    

    /* message state */
    CdmaSmsMsgP.SMSState= CDMA_SMS_STATE_UNSENT;
   
    /* transport type */
   CdmaSmsMsgP.TransMsgType = CDMA_SMS_SMS_MSG_TYPE_POINT_TO_POINT;
 
    /* Teleservice Msg Type. */
    CdmaSmsMsgP.TeleMsgType = CDMA_SMS_MSG_TYPE_ORIGINATION_SUBMIT;
    
    /* Barer Replay Option */
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_BEARER_REPLY_OPTION_PRESENT;
    CdmaSmsMsgP.BearerReplyOption.replySeq = 0;            /* just filled with 0 */
 
    /* User Data. */
    /**********************************************************************************
     * USER DATA的最大字节数为140个字节，超过140个字节，GSM手机无法接收。
     * NumFields 为字符的个数，对应140个字节，8-BIT的编码字符格式是140个，7－BIT编码字符
     * 个数160个，UNICODE编码字符个数是70个。
     * CdmaSmsMsgP->UserData[0].Data里面的数据是编码后的数据，最大长度是140个字节。
    **********************************************************************************/
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_USER_DATA_PRESENT;    
    CdmaSmsMsgP.NumUserData = 1;
	if(CDMA_SMS_MSG_ENCODE_UNICODE==pSendCell->dataType)
	{
		CdmaSmsMsgP.UserData[0].MessageEncoding   = CDMA_SMS_MSG_ENCODE_UNICODE;
		CdmaSmsMsgP.UserData[0].NumFields         = (pSendCell->dataLen)/2;
	}
	else if(CDMA_SMS_MSG_ENCODE_7BIT_ASCII == pSendCell->dataType)
	{
		CdmaSmsMsgP.UserData[0].MessageEncoding   = CDMA_SMS_MSG_ENCODE_7BIT_ASCII;
		CdmaSmsMsgP.UserData[0].NumFields         = pSendCell->dataLen;
	}

    if( !CdmaSmsEncodeUserData( (uint8 *)CdmaSmsMsgP.UserData[0].Data, (uint8 *)pSendCell->pData, 
                                CdmaSmsMsgP.UserData[0].NumFields,
                                CdmaSmsMsgP.UserData[0].MessageEncoding) )
    {
        return CDMA_FALSE;
    }


    /* Callback. */
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_CALLBACK_NUM_PRESENT;    
    if( !CdmaSmsSetCallBackNum(pSendCell->pPhoneNum, &(CdmaSmsMsgP.CallBackNum) ) )
    {
        return CDMA_FALSE;
    }
  
    /* Priority */
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_PRIORITY_IND_PRESENT;
    CdmaSmsMsgP.Priority = CDMA_SMS_PRIORITY_NORMAL;   
    
    /********************************************** 
     * Validity Delivery. 
     * Advice to use REL_VAL_PERIOD_PRESENT,
     * 参照VP的说明
     **********************************************/
    CdmaSmsMsgP.FieldMask    |= CDMA_SMS_REL_VAL_PERIOD_PRESENT;
    CdmaSmsMsgP.RelValPeriod = 167;

    /* delivery ack is set: true for delivery ack is required. */
    CdmaSmsMsgP.FieldMask |= CDMA_SMS_REPLY_OPTION_PRESENT;
    if(smsCnmi_ds_get() == 1)
    {    
    	CdmaSmsMsgP.RplOp.DakReq = CDMA_TRUE;
    }
    else
    {
    	CdmaSmsMsgP.RplOp.DakReq = CDMA_FALSE;
    }
	
    CdmaSmsTxt2Pdu(&CdmaSmsMsgP, SmsPduBuf, &SmsLength); 
    PduToHexString1((char *)send_data_str, SmsPduBuf, SmsLength); 
    *data_length = strlen(send_data_str);
	#if 0
	 strcpy(send_data,SmsPduBuf);
	//在这里可以用AT命令发送SmsPduBufString字符串
	#endif
    return CDMA_TRUE;
}

#if 0
void CdmaSmsTestSend(void)
{
	CdmaSmsMessageT	CdmaSmsMsg;
	
//	CdmaSmsWritePdu(&CdmaSmsMsg);		
}
#endif

