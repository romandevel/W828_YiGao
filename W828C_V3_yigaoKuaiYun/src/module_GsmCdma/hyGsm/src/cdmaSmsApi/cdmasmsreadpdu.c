//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
#include "hyTypes.h"
#include "cdmasmsapi.h"
#include "cdmasmsutil.h"
#include "usock.h"

/*****************************************************************************
    FUNCTION NAME:  CdmaSmsBcdCodeToChar
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static char CdmaSmsBcdCodeToChar(uint8 BcdCode)
{
    char   BcdChar;

    switch (BcdCode)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        BcdChar = '0' + BcdCode;
        break;

    case 0:
    case 10:
        BcdChar = '0';
        break;

    case 11:
        BcdChar = '*';
        break;

    case 12:
        BcdChar = '#';
        break;

    default:
        BcdChar = '\0';
        break;
    }
        
    return (BcdChar);
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsBcdToAscii
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsBcdToAscii(char* AsciiStrP, const uint8* BcdStrP , uint8 DigitCount)
{
    uint8    BcdCode;
    char     BcdChar;
    uint8    i;

    if( (AsciiStrP == NULL) || (BcdStrP == NULL) || (BcdStrP == (uint8 *)AsciiStrP) )
    {
        return CDMA_FALSE;
    }                
       
    for( i = 0; i < (DigitCount + 1) / 2; i++ )
    {
        BcdCode = BcdStrP[i] >> 4;
        BcdChar = CdmaSmsBcdCodeToChar(BcdCode);
        if( BcdChar == '\0' )
        {
            return CDMA_FALSE;
        }             

        AsciiStrP[i * 2] = BcdChar;
       
        if ( (i * 2) != (DigitCount - 1) )
        {
            BcdCode = BcdStrP[i] & 0x0F;
            BcdChar = CdmaSmsBcdCodeToChar(BcdCode);
            if( BcdChar == '\0' )
            {
                return CDMA_FALSE;
            }             
            
            AsciiStrP[i * 2 + 1] = BcdChar;
        }
    }

    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTextFrom8BitStream
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsTextFrom8BitStream(uint8  *DestDataP, const uint8 *SrcDataP, int NumFields)
{
    int i;

    if ( (DestDataP == NULL) || (SrcDataP == NULL) ||(DestDataP == SrcDataP) )
    {
        return CDMA_FALSE;
    }

    if( NumFields > CDMA_SMS_MAX_USER_DATA_LEN )
    {
        return CDMA_FALSE;
    }

    for( i = 0; i < NumFields; i++ )
    {
        DestDataP[i] = SrcDataP[i];
    }
    
    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsBitUnpackB
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 CdmaSmsBitUnpackB(uint8 *BufP, uint16 Pos, uint16 Len)
{
    uint8   Result = 0;
    int     RShift = 0;
    uint16  BitPosToUnpack;
    uint16  BitsLeftInByte;

    BufP += (Pos / 8);

    BitPosToUnpack  = Pos % 8;
    BitsLeftInByte  = 8 - BitPosToUnpack;
    
    RShift = MAX((BitsLeftInByte - Len), 0);

    if ( BitsLeftInByte > Len )
    {
        Result = MASK_AND_SHIFT(Len, BitPosToUnpack, RShift, *BufP);
    }
    else
    {
        Result = MASK(BitsLeftInByte, BitPosToUnpack, *BufP);
        
        BufP++;
        Len -= BitsLeftInByte;

        if( Len > 0 )
        {            
            Result = ( Result << Len ) | (*BufP >> (8 - Len));  /* if any bits left */
        }
    }

    return Result;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTextFrom7BitStream
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsTextFrom7BitStream(uint8* DestDataP, const uint8* SrcDataP, uint16 NumFields)
{
    uint32      i;
    uint32      BitPos = 0;    /* Bit position for unpacking characters. */

    if ( (DestDataP == NULL) || (SrcDataP == NULL) || (DestDataP == SrcDataP) )
    {
        return CDMA_FALSE;
    }

    if( NumFields > CDMA_SMS_MAX_USER_DATA_LEN )
    {
        return CDMA_FALSE;
    }

    for( i = 0; i < NumFields; i++ )
    {
        DestDataP[i] = CdmaSmsBitUnpackB( (uint8 *)SrcDataP, BitPos, 7 );

        BitPos += 7;
    }

    DestDataP[i] = '\0';

    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTextFromUnicode2
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsTextFromUnicode2(uint8* DestDataP, uint8* SrcDataP, uint32 PduLen)
{
    uint16  i;

    if( PduLen > CDMA_SMS_MAX_SMS_PDU_LEN )
    {
        return CDMA_FALSE;
    }

    for( i = 0; i < PduLen; i += 2 )
    {
        DestDataP[i + 1] = SrcDataP[i];
        DestDataP[i] = SrcDataP[i + 1];
    }        

    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTextFromUnicode1
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsTextFromUnicode1(uint8* DestDataP, uint8* SrcDataP, uint32 PduLen)
{
    uint16  i;

    if ( PduLen > CDMA_SMS_MAX_SMS_PDU_LEN )
    {
        return CDMA_FALSE;
    }

    for ( i = 0; i < PduLen; i++ )
    {
        DestDataP[i] = SrcDataP[i];       
    }
    
    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTextFromUnicode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsTextFromUnicode(uint8 *DestDataP, const uint8 *SrcDataP, uint16 NumFields)
{
#define UNICODE_BOM                     (0xFEFF)        /* byte order mark, normal      */
#define UNICODE_BOM_SWAP                (0xFFFE)        /* byte order mark, reversed    */
    uint16  Code;

    Code = ((uint16)(SrcDataP[0]) << 8) | SrcDataP[1];
        
    switch(Code)
    {
    case UNICODE_BOM_SWAP:
        SrcDataP += 2;     /* got it already */
        return CdmaSmsTextFromUnicode2(DestDataP, (uint8 *)SrcDataP, NumFields);
        break;

    case UNICODE_BOM:
        SrcDataP += 2;     /* got it already */
        return CdmaSmsTextFromUnicode1(DestDataP, (uint8 *)SrcDataP, NumFields);
        break;

    default:
        return CdmaSmsTextFromUnicode1(DestDataP, (uint8 *)SrcDataP, NumFields);
        break;
    }

    return CDMA_TRUE;
}

/*****************************************************************************
    FUNCTION NAME:  CdmaSmsUserDataDecode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static bool CdmaSmsUserDataDecode(uint8 *DestDataP, uint8 *SrcDataP, uint16 NumFields, uint8 Encode)
{
    if( (DestDataP == NULL) || (SrcDataP == NULL) || (DestDataP == SrcDataP) )
    {
        return CDMA_FALSE;
    }        
    
    switch( Encode )
    {
    case CDMA_SMS_MSG_ENCODE_OCTET:
    case CDMA_SMS_MSG_ENCODE_LATIN:
    case CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG:
    case CDMA_SMS_MSG_ENCODE_LATIN_HEBREW:
        CdmaSmsTextFrom8BitStream( DestDataP, SrcDataP, NumFields);
        break;

    case CDMA_SMS_MSG_ENCODE_7BIT_ASCII:
    case CDMA_SMS_MSG_ENCODE_IA5:
        if ( !CdmaSmsTextFrom7BitStream(DestDataP, SrcDataP, NumFields) )
        {        
            return CDMA_FALSE;
        }
        break;

    case CDMA_SMS_MSG_ENCODE_UNICODE:        
        if ( !CdmaSmsTextFromUnicode(DestDataP, SrcDataP, NumFields * 2) )
        {         
            return CDMA_FALSE;
        }
        break;

    default:
        return CDMA_FALSE;
    }

    return CDMA_TRUE;
}

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

  //static char PduHexString1[] ="0000021002040702C56885E9A2A8060100082A00032CCCC001062012716AB7E803060802021444000501A70801000A01400D01010E07058AD10BD34550"
//static char PduHexString1[] = "0000021002020702c6995606a84408a30003100000018e22300140019001780190014a9e532fb28a2a81e7f80b0543b772926b3178018b30fbf53ba94311bff86330fbf53c6c54d6c7f86278c3070338fa8c2a8a2a81e41dbc6b0278c3070272fc6c52c8d7f8645f345f100188018001800188018001680188016801880168018ff8636f8ab99c02a480d00140039803d0018801800180018801700318037803680149801003060803181700500801000a0100";
/*****************************************************************************
    FUNCTION NAME:  HexCharToBcd
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 HexCharToBcd(char HexChar)
{
    if( (HexChar >= '0') && (HexChar <= '9') )
    {
        return (uint8)(HexChar - '0');
    }

    if( (HexChar >= 'A') && (HexChar <= 'F') )
    {
        return 10 + (HexChar - 'A');
    }

    if( (HexChar >= 'a') && (HexChar <= 'f') )
    {
        return 10 + (HexChar - 'a');
    }

    return 0xFF;
}
/*****************************************************************************
    FUNCTION NAME:  HexStringToPdu
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
bool HexStringToPdu(uint8* PduMsgP, const char* SrcDataP, uint16 PduLen)
{
    char    HexChar;
    uint8   HexCode;
    uint16  i;
       
    if(PduLen > CDMA_SMS_MAX_SMS_PDU_LEN ) 
    {
        return CDMA_FALSE;
    }
    
    for( i = 0; i < PduLen; i++ )
    {
        HexChar = SrcDataP[2 * i];

        HexCode = HexCharToBcd(HexChar);

        if( HexCode == 0xFF)
        {
            return CDMA_FALSE;
        }

        PduMsgP[i] = HexCode << 4;

        HexChar = SrcDataP[2 * i + 1];
        HexCode = HexCharToBcd(HexChar);

        if( HexCode == 0xFF )
        {
            return CDMA_FALSE;
        }

        PduMsgP[i] |= HexCode;        
    }

    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsReadPduMsg
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
bool CdmaSmsReadPduMsg(uint8 *PduMsgP, uint8 PduLen,struct gsmd_sms_list *dst)
{
    CdmaSmsMessageT     SmsTextMsg;
    CdmaSmsStateT       SmsState;
    CdmaSmsTeleSrvIdT   TeleSrvId;
    uint16              NumMsgs;        /* just for voice mail */
    uint16              Year;
    uint8               Month, Day, Hour, Minute, Second;
    
    uint16              ValidYear;
    uint8               ValidMonth, ValidDay, ValidHour, ValidMinute, ValidSecond;
    uint8               RelValPeriod;
            
    char      	Address[CDMA_SMS_MAX_ADDRESS_LEN + 1];
    char     	CallBackNum[CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS + 1];
   // uint8      	SmsUserData[CDMA_SMS_MAX_USER_DATA_LEN + 1];
   // char 		UserData[CDMA_SMS_MAX_USER_DATA_LEN * 2 + 1];
	
	      
    memset( (void *)&SmsTextMsg, 0, sizeof(CdmaSmsMessageT) );
    
    CdmaSmsPdu2Txt(PduMsgP, PduLen, &SmsTextMsg);
    
    memset( Address, 0, sizeof(Address) );

    /* sms state */
    SmsState = SmsTextMsg.SMSState;

	
	          
    /* address */            
    if( SmsTextMsg.FieldMask & CDMA_SMS_ADDRESS_PRESENT )
    {
        if( SmsTextMsg.Address.NumFields > CDMA_SMS_MAX_ADDRESS_LEN )   /* judge the length */
        {
        	
        	
            return CDMA_FALSE;       /* address error */
        }
        
        if ( SmsTextMsg.Address.DigitMode == CDMA_SMS_DIGIT_MODE_4_BIT )    /* 4 bit digit */
        {
            /* Bcd Address To Ascii Address */
            if( !CdmaSmsBcdToAscii(Address, SmsTextMsg.Address.Address , SmsTextMsg.Address.NumFields) )
            {              	
                return CDMA_FALSE;
            }                                        
        }
        else /*8 bit digit */
        {

            if(SmsTextMsg.Address.NumberType.NumType == CDMA_SMS_NUMBER_TYPE_INTERNATIONAL)
            {
                if( SmsTextMsg.Address.NumFields > (CDMA_SMS_MAX_ADDRESS_LEN - 1) )   /* the address is too long for international plan */
                {
                    return CDMA_FALSE;
                }

                memcpy( &Address[1], SmsTextMsg.Address.Address, SmsTextMsg.Address.NumFields);
                Address[0] = '+';
            }
            else
            {
                memcpy( Address, SmsTextMsg.Address.Address, SmsTextMsg.Address.NumFields);                          
            }
        }    
        strcpy(dst->addr.number,Address);    
    }        
	
	

	{
		uint8 DebugStr[100];
				
		memset(DebugStr, 0, sizeof(DebugStr) );
		memcpy( DebugStr, Address, sizeof(Address) );
		
		
	}		
    
    /* service id */
    TeleSrvId = SmsTextMsg.TeleSrvId;    
    
	

 
    /* num of msgs for voice mail */
    if( SmsTextMsg.FieldMask & CDMA_SMS_NUM_MESSAGES_PRESENT )
    {
       NumMsgs = (SmsTextMsg.NumMsgs >> 4) * 10 + (SmsTextMsg.NumMsgs & 0x0F );
    }
    else
    {
       NumMsgs = 0;
    }

	

    /* call back num */
    memset( CallBackNum, 0, sizeof(CallBackNum) );    
    if( SmsTextMsg.FieldMask & CDMA_SMS_CALLBACK_NUM_PRESENT )
    {        
        if( SmsTextMsg.CallBackNum.NumFields > CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS )
        {
            return CDMA_FALSE;
        }
               
        if (SmsTextMsg.CallBackNum.DigitMode == CDMA_SMS_DIGIT_MODE_4_BIT )  /* 4 bit digit */
        {
            if( !CdmaSmsBcdToAscii( CallBackNum, (uint8 *)SmsTextMsg.CallBackNum.Digits, SmsTextMsg.CallBackNum.NumFields ) )
            {
                return CDMA_FALSE;
            }
        }
        else /*8 bit digit */
        {
            memcpy( CallBackNum, (void *)SmsTextMsg.CallBackNum.Digits, SmsTextMsg.CallBackNum.NumFields);
        }
    }

    Year    = 0;
    Month   = 0;        
    Day     = 0;
    
    Hour    = 0;
    Minute  = 0;
    Second  = 0;
    
    if( SmsTextMsg.FieldMask & CDMA_SMS_MC_TIME_STAMP_PRESENT )
    {   
        uint8   LowBcd, HighBcd;//msg.time_stamp
        
        LowBcd  = (SmsTextMsg.TimeStamp.Hours & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Hours >> 4) & 0x0F;        
        Hour    =  HighBcd * 10 + LowBcd;
        
        LowBcd  = (SmsTextMsg.TimeStamp.Minutes & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Minutes >> 4) & 0x0F;        
        Minute  =  HighBcd * 10 + LowBcd;
            
        LowBcd  = (SmsTextMsg.TimeStamp.Seconds & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Seconds >> 4) & 0x0F;        
        Second  =  HighBcd * 10 + LowBcd;
        
        LowBcd  = (SmsTextMsg.TimeStamp.Day & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Day >> 4) & 0x0F;        
        Day     =  HighBcd * 10 + LowBcd;
        
        LowBcd  = (SmsTextMsg.TimeStamp.Month & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Month >> 4) & 0x0F;        
        Month     =  HighBcd * 10 + LowBcd;        
                
        LowBcd  = (SmsTextMsg.TimeStamp.Year & 0x0F);
        HighBcd = (SmsTextMsg.TimeStamp.Year >> 4) & 0x0F;        
        Year    =  HighBcd * 10 + LowBcd + 2000; 
         
        
        dst->time_stamp[0] = ((SmsTextMsg.TimeStamp.Year & 0x0F)<< 4)|(SmsTextMsg.TimeStamp.Year >> 4) & 0x0F;
        dst->time_stamp[1] = ((SmsTextMsg.TimeStamp.Month & 0x0F)<< 4)|(SmsTextMsg.TimeStamp.Month >> 4) & 0x0F;
        dst->time_stamp[2] = ((SmsTextMsg.TimeStamp.Day & 0x0F)<< 4)|(SmsTextMsg.TimeStamp.Day >> 4) & 0x0F;
        dst->time_stamp[3] = ((SmsTextMsg.TimeStamp.Hours & 0x0F)<< 4)|(SmsTextMsg.TimeStamp.Hours >> 4) & 0x0F;
        dst->time_stamp[4] = ((SmsTextMsg.TimeStamp.Minutes & 0x0F)<< 4)|(SmsTextMsg.TimeStamp.Minutes >> 4) & 0x0F;
    }
    
    
    ValidYear   = 0;
    ValidMonth  = 0;
    ValidDay    = 0;
    ValidHour   = 0;
    ValidMinute = 0;
    ValidSecond = 0;
        
    if( (SmsState == CDMA_SMS_STATE_READ) || (SmsState == CDMA_SMS_STATE_UNREAD) 
            || (SmsState == CDMA_SMS_STATE_DRAFT_READ) || (SmsState == CDMA_SMS_STATE_DRAFT_READ)
            || (SmsState == CDMA_SMS_STATE_DUPLICATE_READ) || (SmsState == CDMA_SMS_STATE_DUPLICATE_UNREAD) )
    {            
        /* no valid period, it is just for send message */   
    }        
    else if( (SmsState == CDMA_SMS_STATE_SENT) || (SmsState == CDMA_SMS_STATE_UNSENT)
                    || (SmsState == CDMA_SMS_STATE_DRAFT_SENT) || (SmsState == CDMA_SMS_STATE_DRAFT_UNSENT) )
    {                            
        if( SmsTextMsg.FieldMask & CDMA_SMS_ABS_VAL_PERIOD_PRESENT )
        {
            uint8   LowBcd, HighBcd;
        
            LowBcd      = (SmsTextMsg.ValTimeAbs.Hours & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Hours >> 4) & 0x0F;        
            ValidHour   =  HighBcd * 10 + LowBcd;
        
            LowBcd      = (SmsTextMsg.ValTimeAbs.Minutes & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Minutes >> 4) & 0x0F;        
            ValidMinute =  HighBcd * 10 + LowBcd;
            
            LowBcd      = (SmsTextMsg.ValTimeAbs.Seconds & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Seconds >> 4) & 0x0F;        
            ValidSecond =  HighBcd * 10 + LowBcd;
        
            LowBcd      = (SmsTextMsg.ValTimeAbs.Day & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Day >> 4) & 0x0F;        
            ValidDay    =  HighBcd * 10 + LowBcd;
 
            LowBcd      = (SmsTextMsg.ValTimeAbs.Month & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Month >> 4) & 0x0F;        
            ValidMonth  =  HighBcd * 10 + LowBcd;
                
            LowBcd      = (SmsTextMsg.ValTimeAbs.Year & 0x0F);
            HighBcd     = (SmsTextMsg.ValTimeAbs.Year >> 4) & 0x0F;        
            ValidYear   =  HighBcd * 10 + LowBcd + 2000;       
            
			

        }
        else if( SmsTextMsg.FieldMask & CDMA_SMS_REL_VAL_PERIOD_PRESENT )
        {
            RelValPeriod = SmsTextMsg.RelValPeriod;
            
			
        }
        else
		{
			
		}        	
    }

    /* user data */
   // memset( SmsUserData, 0, sizeof(SmsUserData) );
    memset(dst->payload.data,0,165);
    //if( !CdmaSmsUserDataDecode( SmsUserData, (uint8 *)SmsTextMsg.UserData[0].Data, SmsTextMsg.UserData[0].NumFields, SmsTextMsg.UserData[0].MessageEncoding))
    if( !CdmaSmsUserDataDecode( dst->payload.data, (uint8 *)SmsTextMsg.UserData[0].Data, SmsTextMsg.UserData[0].NumFields, SmsTextMsg.UserData[0].MessageEncoding))
    {
        return CDMA_FALSE;
    }

	dst->payload.length = (SmsTextMsg.UserData[0].NumFields*2);
	//memset( UserData, 0, sizeof(UserData) );

	dst->payload.has_header = SmsTextMsg.HeaderInd;
	switch(SmsTextMsg.UserData[0].MessageEncoding)
	{
		case CDMA_SMS_MSG_ENCODE_UNICODE:
		    dst->payload.coding_scheme = ALPHABET_UCS2;
		    break;
		case CDMA_SMS_MSG_ENCODE_7BIT_ASCII:
			dst->payload.coding_scheme = ALPHABET_8BIT;//ALPHABET_DEFAULT;
		    break;
		case CDMA_SMS_MSG_ENCODE_OCTET:
			dst->payload.coding_scheme = ALPHABET_8BIT;
		    break;
		default:
			 dst->payload.coding_scheme = ALPHABET_RESERVED;
		    break;
	}
	
    return CDMA_TRUE;
}
/*****************************************************************************
    FUNCTION NAME:  PduReadTest
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/ 
#if 0 
void PduReadTest(void)
{
    static uint8 PduMsg[255];
    uint8 	PduSize;
    
    PduSize = strlen(PduHexString1) / 2;
    
    HexStringToPdu(PduMsg, PduHexString1, PduSize);
       
    CdmaSmsReadPduMsg(PduMsg, PduSize);
}
#endif