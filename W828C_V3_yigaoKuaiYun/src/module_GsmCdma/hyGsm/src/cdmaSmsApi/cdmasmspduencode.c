#include "cdmasmsapi.h"
#include "cdmasmsutil.h"

/*****************************************************************************
    FUNCTION NAME:  MakeTeleSrvId
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
static uint8 MakeTeleSrvId(uint8 *data, CdmaSmsMessageT *msgP)
{
     CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_TL_TELESERVICE_ID);
     CdmaSmsPutUint8(data, 8, 8, 2);
     CdmaSmsPutUint16(data, 16, 16, msgP->TeleSrvId);
     
     return( 4 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeServiceCat
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
static uint8 MakeServiceCat(uint8 *data, CdmaSmsMessageT *msg)
{
     CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_TL_SERVICE_CAT);
     CdmaSmsPutUint8(data, 8, 8, 2);
     CdmaSmsPutUint16(data, 16, 16, msg->SrvCat);
     
     return( 4 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeAddress
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
static uint8 MakeAddress(uint8 *data, CdmaSmsAddressT *addr, bool MoSmsMsg)
{
    uint8   i, size, byte, totalLength, result;
    uint16  getBit=0, startBit=0;
 
    if(MoSmsMsg) 
    {    
        CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_TL_DEST_ADDR); 
    }        
    else
    {         
        CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_TL_ORIG_ADDR);
    }
            
    startBit += 16;      /* skip over parameter length for now */
    CdmaSmsPutUint8(data, startBit, 1, addr->DigitMode); 
    startBit++;

    CdmaSmsPutUint8(data, startBit, 1, addr->NumberMode); 
    startBit++;
    
    if(addr->DigitMode)
    {
        /* 8 bit unspecified address characters */
        size = 8;
        
        /* numberType is present */
        CdmaSmsPutUint8(data, startBit, 3, addr->NumberType.NumType); 
        startBit += 3;
    
        if( addr->NumberMode == CDMA_FALSE )
        {
            /* numberPlan is here too */
            CdmaSmsPutUint8(data, startBit, 4, addr->NumberPlan); 
            startBit += 4;
        }
    }
    else 
    {
        /* DTMF codes for address */
        size = 4;
    }
    
    /* Put NUM_FIELDs in message then increment count */
    CdmaSmsPutUint8(data, startBit, 8, addr->NumFields); 
    startBit += 8;
    
    for( i = 0; i < addr->NumFields;i++ )
    {
        byte = CdmaSmsGetUint8(addr->Address, getBit, size);
        CdmaSmsPutUint8(data, startBit, size, byte); 
        startBit += size;
        getBit += size;
    }
    
    totalLength = CDMA_SMS_NUM_BYTES(startBit);
    
    /*   pad the data with 0s  */
    result = (totalLength * 8) - startBit;
    if(result)
    {
        CdmaSmsPutUint8(data, startBit, result , 0);
    }
    
    /* set the parameter length field */
    CdmaSmsPutUint8(data, 8, 8, totalLength - 2);
    
    /* Return the number of octets used */
    return(totalLength);
 }

/*****************************************************************************
    FUNCTION NAME:  MakeSubaddress
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
static uint8 MakeSubaddress(uint8 *data, CdmaSmsSubaddressT *addr, bool MoSmsMsg)
{
    uint8 i;
    uint16 startBit = 0;
    
    if(MoSmsMsg)
    {
        CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_TL_DEST_SUBADDR); 
    }         
    else
    {
        CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_TL_ORIG_SUBADDR);
    }
            
    startBit += 8;
    
    CdmaSmsPutUint8(data, startBit, 8, addr->NumFields + 2); 
    startBit += 8;
    
    CdmaSmsPutUint8(data, startBit, 3, addr->SubaddrType); 
    startBit += 3;
    
    CdmaSmsPutUint8(data, startBit, 1, addr->Odd); 
    startBit++;
    
    CdmaSmsPutUint8(data, startBit, 8, addr->NumFields); 
    startBit += 8;
    
    for( i = 0; i < addr->NumFields; i++ )
    {
        CdmaSmsPutUint8(data, startBit, 8, addr->Address[i]); 
        startBit += 8;
    }
    
    /* pad to integer# of octets */
    CdmaSmsPutUint8(data, startBit, 4, 0); 
    startBit += 4;
    
    return(addr->NumFields + 4);
} 
/*****************************************************************************
    FUNCTION NAME:  MakeBearerReplyOption
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeBearerReplyOption(uint8 *data,uint8 replySeq)
{
    CdmaSmsPutUint8( data, 0, 8, CDMA_SMS_TL_BEARER_RPLY_OPT );
    CdmaSmsPutUint8( data, 8, 8, 1 );
    CdmaSmsPutUint8( data, 16, 6, replySeq );
    CdmaSmsPutUint8( data, 22, 2, 0 );
    
    return(3);
}

/*****************************************************************************
    FUNCTION NAME:  MakeUsrData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeUsrData(uint8 *data, CdmaSmsUserDataT *msg)
{    
    uint16  word, getBit, startBit=0, plengthBit;
    uint8   i, byte, dataSize=8;
        
    CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_ASE_USER_DATA); 
    plengthBit = startBit + 8;
    startBit += 16;
    
    /* skip parameter length field for now */    
    CdmaSmsPutUint8(data, startBit, 5, msg->MessageEncoding); 
    startBit += 5;

    if( msg->MessageEncoding == CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG )
    {
        dataSize = 6;
        CdmaSmsPutUint8(data, startBit, 8, msg->MessageType);
        startBit +=8;
    }   
    else if ((msg->MessageEncoding == CDMA_SMS_MSG_ENCODE_7BIT_ASCII) 
                ||(msg->MessageEncoding == CDMA_SMS_MSG_ENCODE_IA5) )
    {
        dataSize = 7;  /* 7 bit ascii or latin/hebrew */
    }
    else if (msg->MessageEncoding == CDMA_SMS_MSG_ENCODE_UNICODE)
    {
        dataSize = 16;  /* 16 bit unicode */

        if(msg->NumFields > 70)
        {
          msg->NumFields = msg->NumFields / 2;
        }
    }
    else if (msg->MessageEncoding == CDMA_SMS_MSG_ENCODE_LATIN)
    {
        dataSize = 8;  /* 8 bit unicode */
    }

    CdmaSmsPutUint8(data, startBit, 8, msg->NumFields);
    startBit += 8;

    getBit = 0;

    for( i = 0; i < msg->NumFields; i++ ) 
    {
        if (dataSize <= 8)
        {
            byte = CdmaSmsGetUint8((uint8 *)(msg->Data), getBit, dataSize);
            CdmaSmsPutUint8(data, startBit, dataSize, byte);
        }
        else
        {
            word = CdmaSmsGetUint16((uint8 *)(msg->Data), getBit, dataSize);
            CdmaSmsPutUint16(data, startBit, dataSize, word);
        }

        startBit += dataSize;
        getBit += dataSize;
    }

    if( startBit % 8 )
    {
        /* then pad to next octet with 0's */
        CdmaSmsPutUint8(data, startBit, (8 - startBit % 8), 0);
    }
    
    CdmaSmsPutUint8(data, plengthBit, 8, CDMA_SMS_NUM_BYTES(startBit) - 2);

    return( CDMA_SMS_NUM_BYTES(startBit) );
}
/*****************************************************************************
    FUNCTION NAME:  MakeRespCode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeRespCode(uint8 *data, CdmaSmsMessageT *msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_USER_RESP_CODE);   /* parameter id */
    CdmaSmsPutUint8(data, 8, 8, 1);                        /* parameter length */
    CdmaSmsPutUint8(data, 16, 8, msg->UserRspCode);        /* response code */
    
    return( 3 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeMsgId
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeMsgId(uint8 *data, CdmaSmsMessageT *msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_MESSAGE_ID);
    CdmaSmsPutUint8(data, 8, 8, 3);
    CdmaSmsPutUint8(data, 16, 4, msg->TeleMsgType);
    CdmaSmsPutUint16(data, 20, 16, (msg->MsgId));
    CdmaSmsPutUint8(data, 36, 1, msg->HeaderInd);
    CdmaSmsPutUint8(data, 37, 3, 0);

    return (5);
}
/*****************************************************************************
    FUNCTION NAME:  MakeAbsTime
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/     
static uint8 MakeAbsTime(uint8 *data, CdmaSmsAbsTimeT *msg, uint8 timeType)
{
    CdmaSmsPutUint8(data, 0, 8, timeType);
    CdmaSmsPutUint8(data, 8, 8, 6);
    CdmaSmsPutUint8(data, 16, 8, msg->Year);
    CdmaSmsPutUint8(data, 24, 8, msg->Month);
    CdmaSmsPutUint8(data, 32, 8, msg->Day);
    CdmaSmsPutUint8(data, 40, 8, msg->Hours);
    CdmaSmsPutUint8(data, 48, 8, msg->Minutes);
    CdmaSmsPutUint8(data, 56, 8, msg->Seconds);

    return( 8 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeRelTime
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeRelTime(uint8 *data, uint8 relTime, uint8 timeType)
{
    CdmaSmsPutUint8(data, 0, 8, timeType);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 8, relTime);

    return ( 3 );
}
/*****************************************************************************
    FUNCTION NAME:  MakePrivacy
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakePrivacy(uint8 *data, CdmaSmsPrivacyT msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_PRIVACY);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 2, msg);
    CdmaSmsPutUint8(data, 18, 6, 0);

    return ( 3 );
}

/*****************************************************************************
    FUNCTION NAME:  MakePriority
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakePriority(uint8 *data, CdmaSmsPriorityT msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_PRIORITY);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 2, msg);
    CdmaSmsPutUint8(data, 18, 6, 0);

    return ( 3 );
}

/*****************************************************************************
    FUNCTION NAME:  MakeRplyOpt
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeRplyOpt(uint8 *data, CdmaSmsRplOptionT *msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_RPLY_OPT);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 1, msg->UserAckReq);
    CdmaSmsPutUint8(data, 17, 1, msg->DakReq);
    CdmaSmsPutUint8(data, 18, 1, msg->ReadAckReq);
    CdmaSmsPutUint8(data, 19, 5, 0);

    return 3;
}
/*****************************************************************************
    FUNCTION NAME:  MakeNumMsg
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeNumMsg(uint8 *data, CdmaSmsMessageT *msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_MSGNUM);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 8, msg->NumMsgs);

    return ( 3 );
}

/*****************************************************************************
    FUNCTION NAME:  MakeAlert
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeAlert(uint8 *data, CdmaSmsAlertT msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_ALERT);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 2, msg);
    CdmaSmsPutUint8(data, 18, 6, 0);

    return ( 3 );
}

/*****************************************************************************
    FUNCTION NAME:  MakeLang
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeLang(uint8 *data, CdmaSmsLanguageT msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_LANG);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 8, msg);

    return ( 3 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeCbNum
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeCbNum(uint8 *data, CdmaSmsCbNumberT *msg)
{
    uint8 byte, i, size;
    uint16 startBit=0, putBit=0, plengthBit;
    
    CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_ASE_CALLBACK); 
    plengthBit = startBit + 8;

    startBit += 16;  /* skip over the length for now */

    CdmaSmsPutUint8(data, startBit, 1, msg->DigitMode); 
    startBit++;

    if( msg->DigitMode)
    {
        /* NUMBER_TYPE is present */
        CdmaSmsPutUint8(data, startBit, 3, msg->NumberType);
        startBit += 3;
        
        /* NUMBER_PLAN is here too */
        CdmaSmsPutUint8(data, startBit, 4, msg->NumberPlan);
        startBit += 4;
        size = 8;
    } 
    else 
    {
       size = 4;
    }

    /* Put NUM_FIELDs in message then increment count */
    CdmaSmsPutUint8(data, startBit, 8, msg->NumFields);
    startBit += 8;

    /* fill in the address characters */
    for(i=0; i<msg->NumFields;i++)
    {
        byte = CdmaSmsGetUint8((uint8 *)(msg->Digits), putBit, size); 
        CdmaSmsPutUint8(data, startBit, size, byte);

        startBit += size;
        putBit += size;
    }

    if ( startBit % 8 )
    {
        /* then pad to next octet with 0's */
        CdmaSmsPutUint8(data, startBit, (8 - startBit % 8) ,0);
    }
    
    /* fill in the parameter_length field for the packet */
    CdmaSmsPutUint8(data, plengthBit, 8, CDMA_SMS_NUM_BYTES(startBit)-2);
    
    return(CDMA_SMS_NUM_BYTES(startBit));
}
/*****************************************************************************
    FUNCTION NAME:  MakeDispMode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeDispMode(uint8 *data, CdmaSmsMessageT *msg)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_DISPMODE);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 2, msg->DispMode.SmsDispMode);
    
    if( msg->DispMode.SmsDispMode == CDMA_SMS_DISP_MODE_RESERVED )
    {
       CdmaSmsPutUint8(data, 18, 6, 0x10);
    }       
    else
    {
       CdmaSmsPutUint8(data, 18, 6, 0);
    }
    
    return ( 3 );
}

/*****************************************************************************
    FUNCTION NAME:  MakeMEUserData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeMEUserData(uint8 *data, CdmaSmsUserDataT *msg, uint8 NumUserMsg) 
{
    uint16  word, startBit = 0, plengthBit, getBit = 0;
    uint8   i, byte, dataSize, j;
        
    CdmaSmsPutUint8(data, startBit, 8, CDMA_SMS_ASE_MENC_DATA);
    plengthBit = startBit + 8;
    startBit += 16;   /* just skip param length for now */

    for( j = 0; j < NumUserMsg; j++ )
    {
        CdmaSmsPutUint8(data, startBit, 5, (msg + j)->MessageEncoding);
        dataSize = 8;
        startBit += 5;
        
        if( (msg + j)->MessageEncoding == CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG )
        {
            dataSize = 6;
        }
        
        if( ((msg + j)->MessageEncoding == CDMA_SMS_MSG_ENCODE_7BIT_ASCII) 
                || ((msg + j)->MessageEncoding == CDMA_SMS_MSG_ENCODE_IA5) )
        {
            dataSize = 7;  /* 7 bit ascii or latin/hebrew */
        }
        
        if( (msg + j)->MessageEncoding == CDMA_SMS_MSG_ENCODE_UNICODE )
        {
            dataSize = 16;  /* 16 bit unicode */
        }
        
        if( (msg + j)->MessageEncoding == CDMA_SMS_MSG_ENCODE_LATIN )
        {
            dataSize = 8;  /* 8 bit unicode */
        }
        
        CdmaSmsPutUint8(data, startBit, 8, (msg + j)->NumFields);
        startBit += 8;
        
        getBit = 0;
        for( i = 0; i < (msg + j)->NumFields; i++ )
        {
            if( dataSize <= 8 )
            {
                byte = CdmaSmsGetUint8((uint8 *)((msg + j)->Data), getBit, dataSize);
                CdmaSmsPutUint8(data, startBit, dataSize, byte);
            }
            else
            {
                word = CdmaSmsGetUint16((uint8 *)((msg + j)->Data), getBit, dataSize);
                CdmaSmsPutUint16(data, startBit, dataSize, word);
            }
            
            getBit += dataSize; 
            startBit += dataSize; 
        }
    }
    
    if( startBit % 8 )
    {
         /* then pad to next octet with 0's! */
        CdmaSmsPutUint8(data, startBit, (8 - startBit % 8) ,0);
    }
    
    /* now fill in the parameter length field */
    CdmaSmsPutUint8(data, plengthBit, 8, CDMA_SMS_NUM_BYTES(startBit) - 2);
    
    return ( CDMA_SMS_NUM_BYTES(startBit) );
}

/*****************************************************************************
    FUNCTION NAME:  MakeMsgDepIndex
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeMsgDepIndex(uint8 *data, uint16 DepIndex)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_MSG_DEP_INDEX);
    CdmaSmsPutUint8(data, 8, 8, 2);
    CdmaSmsPutUint16(data, 16, 16, DepIndex);

    return ( 4 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeMsgStatus
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeMsgStatus(uint8 *data, CdmaSmsMsgStatusT *msgStatus)
{
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_MSG_STATUS);
    CdmaSmsPutUint8(data, 8, 8, 1);
    CdmaSmsPutUint8(data, 16, 2, msgStatus->ErrorClass);
    CdmaSmsPutUint8(data, 18, 6, msgStatus->MsgStatusCode);

    return ( 3 );
}
/*****************************************************************************
    FUNCTION NAME:  MakeSrvCatProgResult
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeSrvCatProgResult(uint8 *data, CdmaSmsSrvCatProgRsltT *msg)
{
    uint8 i;
    
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_SRV_CAT_DATA);
    CdmaSmsPutUint8(data, 8, 8, msg->NumCategories* 3 );
    for( i=0; i<msg->NumCategories; i++ )
    {
        CdmaSmsPutUint16(data, 16 + (i * 24), 16, msg->Cat[i].Category);
        CdmaSmsPutUint8(data, 32 + (i * 24), 4, msg->Cat[i].CategoryResult);
        CdmaSmsPutUint8(data, 36 + (i * 24), 4, 0 );
    }

    return ( 2 + (msg->NumCategories * 3) );
}
/*****************************************************************************
    FUNCTION NAME:  MakeSrvCatProgData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 MakeSrvCatProgData(uint8 *data, CdmaSmsSrvCatProgDataT *SrvCatData)
{
    uint8 i, j, bitCount, dataSize = 8 ;
    if( SrvCatData->MsgEncoding == 1 )
    {
        dataSize = 6;  /* IS-91 character messages */
    }
    
    if( (SrvCatData->MsgEncoding == 2) 
        || (SrvCatData->MsgEncoding == 3) )
    {
        dataSize = 7;  /* 7 bit ascii or latin/hebrew */
    }
    
    if( SrvCatData->MsgEncoding == 4 )
    {
        dataSize = 16;  /* 16 bit unicode */
    }
    
    if( SrvCatData->MsgEncoding == CDMA_SMS_MSG_ENCODE_LATIN )
    {
        dataSize = 8;  /* 8 bit unicode */
    }
    
    CdmaSmsPutUint8(data, 0, 8, CDMA_SMS_ASE_SRV_CAT_DATA);
    
    /* length of this fields will be put in the end,here just skip it */
    CdmaSmsPutUint8(data, 16, 5, SrvCatData->MsgEncoding);
    bitCount = 16 + 5;
    
    for(i = 0; i < SrvCatData->NumCat; i++ )
    {
        CdmaSmsPutUint8(data, bitCount, 4, SrvCatData->Cat[i].OperationCode);
        bitCount += 4;

        CdmaSmsPutUint16(data, bitCount, 16, SrvCatData->Cat[i].SrvCat);
        bitCount += 16;

        CdmaSmsPutUint8(data, bitCount, 8, SrvCatData->Cat[i].Lang);
        bitCount += 8;

        CdmaSmsPutUint8(data, bitCount, 8, SrvCatData->Cat[i].MaxMsg);
        bitCount += 8;

        CdmaSmsPutUint8(data, bitCount, 4, SrvCatData->Cat[i].Alert);
        bitCount += 4;

        CdmaSmsPutUint8(data, bitCount, 8, SrvCatData->Cat[i].NumChar);
        bitCount += 8;

        for( j = 0; j < SrvCatData->Cat[i].NumChar; j++ )
        {
            uint8   byteValue;
            uint16  wordValue;
            
            if( dataSize > 8 )
            {
                wordValue = CdmaSmsGetUint16((uint8 *)SrvCatData->Cat[i].Chari, j * dataSize, dataSize);
                CdmaSmsPutUint16(data, bitCount+j*dataSize, dataSize, wordValue);
            }
            else
            {
                byteValue = CdmaSmsGetUint8((uint8 *)SrvCatData->Cat[i].Chari, j * dataSize, dataSize);
                CdmaSmsPutUint8(data, bitCount+j*dataSize, dataSize, byteValue);
            }
        }
        
        bitCount += ( dataSize * (SrvCatData->Cat[i].NumChar) );
    }
    
    CdmaSmsPutUint8(data, 8, 16, (bitCount % 8) ? bitCount / 8 : bitCount / 8 + 1);
    if( !(bitCount % 8) )
    {
        CdmaSmsPutUint8(data, bitCount, 8 - bitCount % 8, 0);
    }
            
    return (bitCount % 8) ? bitCount / 8 : bitCount / 8 + 1;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsTxt2Pdu
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
void CdmaSmsTxt2Pdu (CdmaSmsMessageT *SmsMsgP, uint8 *PduP, uint8 *LengthP)
{
    uint8   BearDataStart;
    bool    MoSmsMsg = CDMA_TRUE;
    
    if( (SmsMsgP->SMSState == CDMA_SMS_STATE_READ) || (SmsMsgP->SMSState == CDMA_SMS_STATE_UNREAD)
            ||(SmsMsgP->SMSState == CDMA_SMS_STATE_DRAFT_READ)||(SmsMsgP->SMSState == CDMA_SMS_STATE_DUPLICATE_UNREAD)
            ||(SmsMsgP->SMSState == CDMA_SMS_STATE_DUPLICATE_READ) )
    {            
        MoSmsMsg = CDMA_FALSE;
    }        
    else if( (SmsMsgP->SMSState == CDMA_SMS_STATE_SENT) || (SmsMsgP->SMSState == CDMA_SMS_STATE_UNSENT)
                ||(SmsMsgP->SMSState == CDMA_SMS_STATE_DRAFT_SENT)||(SmsMsgP->SMSState == CDMA_SMS_STATE_DRAFT_UNSENT) )
    {                
        MoSmsMsg = CDMA_TRUE;
    }        
    else
    {
        *LengthP = 0;
        return;
    }
    
    *LengthP = 0;
    if(SmsMsgP == NULL)
    {        
        return;  /* error form of message */
    }
            
    /* TL message Type */
    PduP[*LengthP] = SmsMsgP->TransMsgType; 
    (*LengthP)++;

    /* Teleservice id */
    *LengthP += MakeTeleSrvId( PduP + *LengthP, SmsMsgP);
        
    if(CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_SERVICE_CAT_PRESENT))
    {
        *LengthP += MakeServiceCat( PduP + *LengthP, SmsMsgP);
         
    }
    if(CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_ADDRESS_PRESENT))
    {
        *LengthP += MakeAddress( PduP + *LengthP, &(SmsMsgP->Address), MoSmsMsg);
        
    }
    if(CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_SUBADDRESS_PRESENT))
    {
        *LengthP += MakeSubaddress( PduP + *LengthP, &(SmsMsgP->Subaddress), MoSmsMsg);
        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL( SmsMsgP->FieldMask, CDMA_SMS_BEARER_REPLY_OPTION_PRESENT ) )
    {
        *LengthP += MakeBearerReplyOption( PduP + *LengthP, SmsMsgP->BearerReplyOption.replySeq );         
    }

    PduP[*LengthP] = CDMA_SMS_TL_BEARER_DATA;  /* bear data must exists */
    
	/* the length of bearer data is left blank here */
    *LengthP += 2;
    
	BearDataStart = *LengthP;
    
    *LengthP += MakeMsgId(PduP + *LengthP, SmsMsgP);
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_USER_DATA_PRESENT) )
    {
        *LengthP += MakeUsrData( PduP + *LengthP, &(SmsMsgP->UserData[0]));        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_USER_RESP_CODE_PRESENT) )
    {
        *LengthP += MakeRespCode( PduP + *LengthP, SmsMsgP);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_MC_TIME_STAMP_PRESENT) )
    {
        *LengthP += MakeAbsTime( PduP + *LengthP, &(SmsMsgP->TimeStamp), CDMA_SMS_ASE_TIMESTAMP);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_ABS_VAL_PERIOD_PRESENT) )
    {
        *LengthP += MakeAbsTime( PduP + *LengthP, &(SmsMsgP->ValTimeAbs), CDMA_SMS_ASE_VPA);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_REL_VAL_PERIOD_PRESENT) )
    {
        *LengthP += MakeRelTime( PduP + *LengthP, SmsMsgP->RelValPeriod, CDMA_SMS_ASE_VPR);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_ABS_DEF_DEL_TIME_PRESENT) )
    {
        *LengthP += MakeAbsTime( PduP + *LengthP, &(SmsMsgP->AbsDelTime), CDMA_SMS_ASE_DDTA);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_REL_DEF_DEL_TIME_PRESENT) )
    {
        *LengthP += MakeRelTime( PduP + *LengthP, SmsMsgP->RelDelPeriod, CDMA_SMS_ASE_DDTR);       
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_PRIORITY_IND_PRESENT) )
    {
        *LengthP += MakePriority( PduP + *LengthP, SmsMsgP->Priority);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_PRIVACY_IND_PRESENT) )
    {
        *LengthP += MakePrivacy( PduP + *LengthP, SmsMsgP->Privacy);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_REPLY_OPTION_PRESENT) )
    {
        *LengthP += MakeRplyOpt( PduP + *LengthP, &(SmsMsgP->RplOp));        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_NUM_MESSAGES_PRESENT) )
    {
        *LengthP += MakeNumMsg( PduP + *LengthP, SmsMsgP);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_ALERT_MSG_DEL_PRESENT) )
    {
        *LengthP += MakeAlert( PduP + *LengthP, SmsMsgP->Alert);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_LANGUAGE_IND_PRESENT) )
    {
        *LengthP += MakeLang( PduP + *LengthP, SmsMsgP->Lang);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_CALLBACK_NUM_PRESENT) )
    {
        *LengthP += MakeCbNum( PduP + *LengthP, &(SmsMsgP->CallBackNum));       
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_MSG_DISP_MODE_PRESENT) )
    {
        *LengthP += MakeDispMode( PduP + *LengthP, SmsMsgP);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_MULT_ENCODE_USER_DATA_PRESENT) )
    {
        *LengthP += MakeMEUserData(PduP + *LengthP, SmsMsgP->UserData, SmsMsgP->NumUserData);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_MESSAGE_DEPOSIT_INDEX_PRESENT) )
    {
        *LengthP += MakeMsgDepIndex(PduP + *LengthP, SmsMsgP->MsgDepIndex);        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_MESSAGE_STATUS_PRESENT) )
    {
        *LengthP += MakeMsgStatus(PduP + *LengthP, &(SmsMsgP->MsgStatus));        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_SER_CAT_PROGRAM_RESULT_PRESENT) )
    {
        *LengthP += MakeSrvCatProgResult(PduP + *LengthP, &(SmsMsgP->SrvCatProgResult));        
    }
    
    if( CDMA_SMS_MASK_TO_BOOL(SmsMsgP->FieldMask, CDMA_SMS_SER_CAT_PROGRAM_DATA_PRESENT) )
    {
        *LengthP += MakeSrvCatProgData(PduP + *LengthP, &(SmsMsgP->SrvCatData));
    }

    PduP[BearDataStart - 1] = *LengthP - BearDataStart;  /* the length of bearer data */
    
    return;
}
