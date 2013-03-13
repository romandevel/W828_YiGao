#include "cdmasmsapi.h"
#include "cdmasmsutil.h"

/*****************************************************************************
    FUNCTION NAME:  ExtractAddress
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/
static uint8 ExtractAddress(uint8 * data, CdmaSmsAddressT* Address)
{
    uint8   i, size, byte, totalLength;
    uint16  startBit = 0, putBit = 0;

    Address->DigitMode = (bool)CdmaSmsGetUint8(data, startBit, 1); 
    startBit++;
    
    Address->NumberMode = (bool)CdmaSmsGetUint8(data, startBit, 1); 
    startBit++;

    /*
     *  Calculate bit/byte offsets for the beginning of the
     *  CHARi field.
     */
    if(Address->DigitMode)
    {
        /* numberType is present */
        if(Address->NumberMode)
        {
            Address->NumberType.DataNumType = (CdmaSmsDataNumberT)CdmaSmsGetUint8(data, startBit, 3); 
            startBit += 3;
        }
        else
        {
            Address->NumberType.NumType = (CdmaSmsNumberT)CdmaSmsGetUint8(data, startBit, 3); 
            startBit += 3;
            
            /* numberPlan is here too */
            Address->NumberPlan = (CdmaSmsNumPlanT)CdmaSmsGetUint8(data, startBit, 4); 
            startBit += 4;
        }

        size = 8;  /* all address characters will be 8 bits */        
    }
    else
    {
        size = 4;  /* all address characters will be 4 bits */
    }
    
    /* Get numFields from message then increment count */
    Address->NumFields = CdmaSmsGetUint8(data, startBit, 8); 
    startBit += 8;
    
    for(i = 0; i < Address->NumFields; i++ )
    {
        byte = CdmaSmsGetUint8(data, startBit, size);
        CdmaSmsPutUint8(Address->Address, putBit, size, byte);
        startBit += size;
        putBit += size;
    }
    
    totalLength = CDMA_SMS_NUM_BYTES(startBit);
    
    return( totalLength );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractSubaddress
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/
static uint8 ExtractSubaddress(uint8 *data, CdmaSmsSubaddressT *addr)
{
    uint8       i, totalLength;
    uint16      startBit = 0;
 
    addr->SubaddrType   = CdmaSmsGetUint8(data, startBit, 3); 
    startBit += 3;
    
    addr->Odd           = (bool)CdmaSmsGetUint8(data, startBit, 1); 
    startBit++;
    
    addr->NumFields     = CdmaSmsGetUint8(data, startBit, 8); 
    startBit += 8;

    for( i = 0; i < addr->NumFields; i++ )
    {
        addr->Address[i] = CdmaSmsGetUint8(data, startBit, 8); 
        startBit += 8;
    }
    
    totalLength = CDMA_SMS_NUM_BYTES(startBit);
    
    return(totalLength);
}
/*****************************************************************************
    FUNCTION NAME:  ExtractBearerReplyOption
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/
uint8 ExtractBearerReplyOption( uint8 *data, uint8* replySeq )
{
     *replySeq = CdmaSmsGetUint8(data, 0, 6);
     
     return( 1 );
}

/*****************************************************************************
    FUNCTION NAME:  ExtractMessageId
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/
static uint8 ExtractMessageId(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->TeleMsgType    = (CdmaSmsTeleMsgT)CdmaSmsGetUint8(data, 0, 4); 
    msg->MsgId          = CdmaSmsGetUint16(data, 4, 16);
    msg->HeaderInd      = (bool)CdmaSmsGetUint8(data, 20, 1); 
    
    return (3);
}
/*****************************************************************************
    FUNCTION NAME:  ExtractUsrData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/ 
static uint8 ExtractUsrData(uint8 *data, CdmaSmsMessageT *msg)
{
    uint8   i, dataSize = 8, byte;
    uint16  startBit = 0, word, putBit;

    msg->UserData[0].MessageEncoding = CdmaSmsGetUint8(data, startBit, 5); 
    startBit += 5;
    
    if( msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG )
    {
        dataSize = 6;  /* IS-91 character messages */
        msg->UserData[0].MessageType = CdmaSmsGetUint8(data, startBit, 8);
        startBit += 8;
    }
    
    if( (msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_7BIT_ASCII) 
            || (msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_IA5) )
    {
        dataSize = 7;  /* 7 bit ascii or latin/hebrew */
        msg->UserData[0].MessageType = 0;
    }
    
    if( msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_UNICODE )
    {
        dataSize = 16;  /* 16 bit unicode */
        msg->UserData[0].MessageType = 0;
    }
    
    if( msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_LATIN )
    {
        dataSize = 8;  
        msg->UserData[0].MessageType = 0;
    }
    
    msg->UserData[0].NumFields = CdmaSmsGetUint8(data, startBit, 8); 
    startBit += 8;
    putBit = 0;
    
    if( (msg->UserData[0].MessageEncoding == CDMA_SMS_MSG_ENCODE_UNICODE) 
            && (msg->UserData[0].NumFields > 70) )
    {
        msg->UserData[0].NumFields = msg->UserData[0].NumFields / 2;
    }
    
    /* Check to make sure the message will fit in our array */
    if( ( (msg->UserData[0].NumFields * dataSize) / 8) > CDMA_SMS_MAX_USER_DATA_LEN )
    {       
        return 0;   /* error */
    }   

    for( i = 0; i < msg->UserData[0].NumFields; i++ )
    {
        if( dataSize <= 8 )
        {
            byte = CdmaSmsGetUint8(data, startBit, dataSize);
            CdmaSmsPutUint8((uint8 *)(msg->UserData[0].Data), putBit, dataSize, byte);
        }
        else
        {
            word = CdmaSmsGetUint16(data, startBit, dataSize);
            CdmaSmsPutUint16((uint8 *)msg->UserData[0].Data, putBit, dataSize, word);
        }
        
        startBit += dataSize;
        putBit += dataSize;
    }
    
    msg->NumUserData = 1;
    
    return( CDMA_SMS_NUM_BYTES(startBit) );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractRespCode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractRespCode(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->UserRspCode = CdmaSmsGetUint8(data, 0, 8);
    
    return( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractAbsTime
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractAbsTime(uint8 *data, CdmaSmsAbsTimeT *msg)
{
    msg->Year       = CdmaSmsGetUint8(data, 0, 8);
    msg->Month      = CdmaSmsGetUint8(data, 8, 8);
    msg->Day        = CdmaSmsGetUint8(data, 16, 8);
    msg->Hours      = CdmaSmsGetUint8(data, 24, 8);
    msg->Minutes    = CdmaSmsGetUint8(data, 32, 8);
    msg->Seconds    = CdmaSmsGetUint8(data, 40, 8);
    
    return( 6 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractRelTime
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractRelTime(uint8 *data, uint8 *msg)
{
    *msg = CdmaSmsGetUint8(data, 0, 8);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractPrivacy
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractPrivacy(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->Privacy = (CdmaSmsPrivacyT)CdmaSmsGetUint8(data, 0, 2);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractPriority
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractPriority(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->Priority = (CdmaSmsPriorityT)CdmaSmsGetUint8(data, 0, 2);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractUserAck
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractUserAck(uint8 *data, CdmaSmsRplOptionT *msg)
{
    /* Ignore DAK it is NEVER RECEIVED by the mobile only SENT */
    msg->UserAckReq     = (bool)CdmaSmsGetUint8(data, 0, 1);
    msg->DakReq         = (bool)CdmaSmsGetUint8(data, 1, 1);
    msg->ReadAckReq     = (bool)CdmaSmsGetUint8(data, 2, 1);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractNumMsg
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractNumMsg(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->NumMsgs = CdmaSmsGetUint8(data, 0, 8);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractAlert
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractAlert(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->Alert = (CdmaSmsAlertT)CdmaSmsGetUint8(data, 0, 2);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractLang
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractLang(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->Lang = (CdmaSmsLanguageT)CdmaSmsGetUint8(data, 0, 8);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractCbNum
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractCbNum(uint8 *data, CdmaSmsMessageT *msg)
{
    uint8   i, byte, size;
    uint16  startBit = 0, putBit = 0;

    msg->CallBackNum.DigitMode = (CdmaSmsDigitModeT) CdmaSmsGetUint8(data, startBit, 1);
    startBit++;
    
    if( msg->CallBackNum.DigitMode )
    {
        /* NUMBER_TYPE is present */
        msg->CallBackNum.NumberType = CdmaSmsGetUint8(data, startBit, 3);
        startBit += 3;
        
        /* NUMBER_PLAN is here too */
        msg->CallBackNum.NumberPlan = CdmaSmsGetUint8(data, startBit, 4);
        startBit += 4;
        
        size = 8;
    }
    else
    {
        size = 4;
    }
    
    /* Get NUM_FIELDs from message then increment count */
    msg->CallBackNum.NumFields = CdmaSmsGetUint8(data, startBit, 8);
    startBit += 8;
    
    for( i = 0; i < msg->CallBackNum.NumFields; i++ )
    {
        byte = CdmaSmsGetUint8(data, startBit, size);
        CdmaSmsPutUint8((uint8 *)(msg->CallBackNum.Digits), putBit, size, byte);
        startBit += size;
        putBit += size;
    }
    
    return( CDMA_SMS_NUM_BYTES(startBit) );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractDispMode
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractDispMode(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->DispMode.SmsDispMode   = (CdmaSmsDisplayModeT)CdmaSmsGetUint8(data, 0, 2);
    msg->DispMode.Reserved      = CdmaSmsGetUint8(data, 2, 6);
    
    return ( 1 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractMEUserData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractMEUserData(uint8 *data, uint8 parmLength, CdmaSmsMessageT *msg)
{
    uint16  word, startBit = 0, putBit = 0;
    uint8   i, j, dataSize, byte, msgCount;
 
    msgCount = 0; 
    
    for(i = 0; i < parmLength; )
    {
        msg->UserData[msgCount].MessageEncoding = CdmaSmsGetUint8(data, startBit, 5);
        startBit += 5;
        dataSize = 8;
        
        if( msg->UserData[msgCount].MessageEncoding == CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG )
        {
            dataSize = 6;
        }
        
        if( (msg->UserData[msgCount].MessageEncoding == CDMA_SMS_MSG_ENCODE_7BIT_ASCII) 
            || (msg->UserData[msgCount].MessageEncoding == CDMA_SMS_MSG_ENCODE_IA5) )
        {
            dataSize = 7;  /* 7 bit ascii or latin/hebrew */
        }
        
        if( msg->UserData[msgCount].MessageEncoding == CDMA_SMS_MSG_ENCODE_UNICODE )
        {
            dataSize = 16;  /* 16 bit unicode */
        }
        
        if( msg->UserData[msgCount].MessageEncoding == CDMA_SMS_MSG_ENCODE_LATIN )
        {
            dataSize = 8;  /* 8 bit unicode */
        }
        
        msg->UserData[msgCount].NumFields = CdmaSmsGetUint8(data, startBit, 8);
        startBit += 8;
        putBit = 0;   
        
        /* Check to make sure the message will fit in our array */
        if( ((msg->UserData[msgCount].NumFields * dataSize)/8) > CDMA_SMS_MAX_USER_DATA_LEN )
        {          
            return 0;
        }
           
        for( j = 0; j < msg->UserData[msgCount].NumFields; j++ ) 
        {
            if( dataSize <= 8 )
            {
                byte = CdmaSmsGetUint8(data, startBit, dataSize);
                CdmaSmsPutUint8((uint8 *)(msg->UserData[msgCount].Data), putBit, dataSize, byte);
            }
            else
            {
                word = CdmaSmsGetUint16(data, startBit, dataSize);
                CdmaSmsPutUint16((uint8 *) (msg->UserData[msgCount].Data), putBit, dataSize, word);
            }
            
            startBit += dataSize;
            putBit += dataSize;
        }
        
        msgCount++;
        
        i = CDMA_SMS_NUM_BYTES(startBit);
    }
    
    msg->NumUserData = msgCount;
    
    return ( CDMA_SMS_NUM_BYTES(startBit) );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractMsgDepIndexData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractMsgDepIndexData(uint8 *data, CdmaSmsMessageT *msg)
{
    msg->MsgDepIndex = CdmaSmsGetUint16(data, 0, 16);
    
    return ( 2 );
}
/*****************************************************************************
    FUNCTION NAME:  ExtractMsgStatus
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractMsgStatus(uint8 *data, CdmaSmsMsgStatusT *msgStatus)
{
    msgStatus->ErrorClass       = CdmaSmsGetUint8(data, 0, 2);
    msgStatus->MsgStatusCode    = CdmaSmsGetUint8(data, 2, 6);
    
    return (1);
}
/*****************************************************************************
    FUNCTION NAME:  ExtractSrvCatResult
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractSrvCatResult(uint8 *data, CdmaSmsSrvCatProgRsltT *msg, uint8 paramLen)
{
    uint8 i;
    
    msg->NumCategories = paramLen/3;
    
    for(i = 0; i < msg->NumCategories; i++ )
    {
        msg->Cat[i].Category        = CdmaSmsGetUint16(data, 24*i, 16);
        msg->Cat[i].CategoryResult  = CdmaSmsGetUint8(data, 24*i+16, 4);
    }
    
    return paramLen;
}
/*****************************************************************************
    FUNCTION NAME:  ExtractSrvCatProgData
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static uint8 ExtractSrvCatProgData(uint8 *data, uint8 parmLength, CdmaSmsMessageT *msg)
{
    uint8   i = 0, dataSize = 8, byte;
    uint16  startBit = 0, word, putBit;

    msg->SrvCatData.MsgEncoding = CdmaSmsGetUint8(data, 0, 5);
    
    if( msg->SrvCatData.MsgEncoding == 1 )
    {
        dataSize = 6;  /* IS-91 character messages */
    }
    
    if( (msg->SrvCatData.MsgEncoding == 2) 
            || (msg->SrvCatData.MsgEncoding == 3) )
    {
        dataSize = 7;  /* 7 bit ascii or latin/hebrew */
    }
    
    if( msg->SrvCatData.MsgEncoding == 4 )
    {
        dataSize = 16;  /* 16 bit unicode */
    }
    
    if (msg->SrvCatData.MsgEncoding == CDMA_SMS_MSG_ENCODE_LATIN)
    {
        dataSize = 8;  /* 8 bit unicode */
    }
    
    msg->SrvCatData.NumCat = 0;
    
    while( CDMA_SMS_NUM_BYTES(startBit) < parmLength )
    {
        msg->SrvCatData.Cat[i].OperationCode = CdmaSmsGetUint8(data, startBit, 4);
        startBit += 4;
        
        msg->SrvCatData.Cat[i].SrvCat = CdmaSmsGetUint16(data, startBit, 16);
        startBit += 16;
        
        msg->SrvCatData.Cat[i].Lang = CdmaSmsGetUint8(data, startBit, 8);
        startBit += 8;
        
        msg->SrvCatData.Cat[i].MaxMsg = CdmaSmsGetUint8(data, startBit, 8);
        startBit += 8;
        
        msg->SrvCatData.Cat[i].Alert = CdmaSmsGetUint8(data, startBit, 4);
        startBit += 4;
        
        msg->SrvCatData.Cat[i].NumChar = CdmaSmsGetUint8(data, startBit, 8);
        startBit += 8;
        
        putBit = 0;
        
        /* Check to make sure the message will fit in our array */
        if( ((msg->SrvCatData.Cat[i].NumChar * dataSize) / 8) > CDMA_SMS_MAX_CHAR_IN_CATEGORY )
        {
            return 0;
        }   
        
        for( i = 0; i < msg->SrvCatData.Cat[i].NumChar; i++ )
        {
            if( dataSize <= 8 )
            {
                byte = CdmaSmsGetUint8(data, startBit, dataSize);
                CdmaSmsPutUint8((uint8 *)(msg->SrvCatData.Cat[i].Chari), putBit, dataSize, byte);
            }
            else
            {
                word = CdmaSmsGetUint16(data, startBit, dataSize);
                CdmaSmsPutUint16((uint8 *)(msg->SrvCatData.Cat[i].Chari), putBit, dataSize, word);
            }
            
            startBit += dataSize;
            putBit += dataSize;
        }
        
        i++;
        msg->SrvCatData.NumCat++;
        
        /* Check to make sure the message will fit in our array */
        if( msg->SrvCatData.NumCat > CDMA_SMS_MAX_CATEGORIES )
        {
            return 0;
        }   
    }
    
    return ( CDMA_SMS_NUM_BYTES(startBit) );
}    

/*****************************************************************************
    FUNCTION NAME:  ParseTeleLayerPdu
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
static void ParseTeleLayerPdu(uint8 *data, uint8 length, CdmaSmsMessageT *msg)
{
    uint8 paramId, paramLength, byteCount = 0, messageCount;
     
    while( byteCount < length )
    {
        /* get subparameter id */
        paramId = CdmaSmsGetUint8(data+byteCount, 0, 8);
        byteCount++;
        
        paramLength = CdmaSmsGetUint8(data+byteCount, 0, 8);
        byteCount++;
        
        switch (paramId)
        {
        case CDMA_SMS_ASE_MESSAGE_ID:    /*0*/
            messageCount = ExtractMessageId(data + byteCount, msg);
            break;
            
        case CDMA_SMS_ASE_USER_DATA:   /*1*/
            messageCount = ExtractUsrData(data + byteCount, msg);
            msg->FieldMask |= CDMA_SMS_USER_DATA_PRESENT;
            break;
            
        case CDMA_SMS_ASE_USER_RESP_CODE:     /*2*/
            messageCount = ExtractRespCode(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_USER_RESP_CODE_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_TIMESTAMP:        /*3*/
            messageCount = ExtractAbsTime(data + byteCount, &(msg->TimeStamp));
            if (messageCount == paramLength)
            {
                msg->FieldMask|= CDMA_SMS_MC_TIME_STAMP_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_VPA:            /*4*/
            messageCount = ExtractAbsTime(data + byteCount, &(msg->ValTimeAbs));
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_ABS_VAL_PERIOD_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_VPR:             /*5*/
            messageCount = ExtractRelTime(data + byteCount, &(msg->RelValPeriod));
            if (messageCount == paramLength)
            {
                msg->FieldMask|= CDMA_SMS_REL_VAL_PERIOD_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_DDTA:           /*6*/
            messageCount = ExtractAbsTime(data + byteCount, &(msg->AbsDelTime));
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_ABS_DEF_DEL_TIME_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_DDTR:            /*7*/
            messageCount = ExtractRelTime(data + byteCount, &(msg->RelDelPeriod));
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_REL_DEF_DEL_TIME_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_PRIORITY:         /*8*/
            messageCount = ExtractPriority(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_PRIORITY_IND_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_PRIVACY:          /*9*/
            messageCount = ExtractPrivacy(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_PRIVACY_IND_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_RPLY_OPT:          /*10*/
            messageCount = ExtractUserAck(data + byteCount, &(msg->RplOp));
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_REPLY_OPTION_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_MSGNUM:           /*11*/
            messageCount = ExtractNumMsg(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_NUM_MESSAGES_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_ALERT:             /*12*/
            messageCount = ExtractAlert(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_ALERT_MSG_DEL_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_LANG:             /*13*/
            messageCount = ExtractLang(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_LANGUAGE_IND_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_CALLBACK:          /*14*/
            messageCount = ExtractCbNum(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_CALLBACK_NUM_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_DISPMODE:           /*15*/
            messageCount = ExtractDispMode(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_MSG_DISP_MODE_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_MENC_DATA:          /*16*/
            messageCount = ExtractMEUserData(data + byteCount, paramLength, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_MULT_ENCODE_USER_DATA_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_MSG_DEP_INDEX:      /*17*/
            messageCount = ExtractMsgDepIndexData(data + byteCount, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_MESSAGE_DEPOSIT_INDEX_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_SRV_CAT_DATA:      /*18*/
            messageCount = ExtractSrvCatProgData(data + byteCount, paramLength, msg);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_SER_CAT_PROGRAM_DATA_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_SRV_CAT_RESULT:    /*19*/
            messageCount = ExtractSrvCatResult(data + byteCount, &(msg->SrvCatProgResult), paramLength);
            if (messageCount == paramLength)
            {
                msg->FieldMask |= CDMA_SMS_SER_CAT_PROGRAM_RESULT_PRESENT;
            }
            break;
            
        case CDMA_SMS_ASE_MSG_STATUS:         /*20*/
            messageCount = ExtractMsgStatus(data + byteCount, &(msg->MsgStatus));
            if(messageCount == paramLength)
            {
               msg->FieldMask |= CDMA_SMS_MESSAGE_STATUS_PRESENT;
            }
            break;
            
        default:
            /* Bad Message Parameter */
            return;
            break;
        }
        
        byteCount += paramLength;
    }
    
    return ;
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsPdu2Txt 
    DESCRIPTION:
    PARAMETERS: 
    RETURNS:
*****************************************************************************/  
void CdmaSmsPdu2Txt (uint8 *SmsPduP, uint8 Length, CdmaSmsMessageT *SmsTxtMsgP)
{
    uint8 index=0, paramId, paramLen, messageCount;
    
    memset( (void *)SmsTxtMsgP,0,sizeof(CdmaSmsMessageT));
    
    /* reset Text message masks */
    SmsTxtMsgP->FieldMask   = 0;
    
    /* init the fields irrelevant to the pdu */
    SmsTxtMsgP->SmsMsgRecId = 0; //init it as 0, because can't know where it is stored

    if( Length == 0 )
    {
        SmsTxtMsgP->SMSState = CDMA_SMS_STATE_FREE;
        return;
    }
    
    SmsTxtMsgP->TransMsgType = SmsPduP[index]; 
    index++;
        
    while (index < Length)
    {
        /* get subparameter id */        
        paramId = SmsPduP[index];
        index++;
        
        paramLen = SmsPduP[index];
        index++;
        
        switch (paramId)
        {
	    case CDMA_SMS_TL_TELESERVICE_ID:
            if(paramLen != 2)
            {
            }
            else
            {
                SmsTxtMsgP->TeleSrvId = ((uint16)(SmsPduP[index])) << 8;
                SmsTxtMsgP->TeleSrvId |= (uint16)(SmsPduP[index + 1]);                   
            }
	        break;
	        
	    case CDMA_SMS_TL_SERVICE_CAT:
	        if(paramLen!=2)
            {
            }
            else
            {
                SmsTxtMsgP->FieldMask |= CDMA_SMS_SERVICE_CAT_PRESENT;
                SmsTxtMsgP->SrvCat = ((uint16)(SmsPduP[index]))<<8;
                SmsTxtMsgP->SrvCat |= (uint16)(SmsPduP[index + 1]);                  
            }
	        break;
	        
	    case CDMA_SMS_TL_ORIG_ADDR:
	    case CDMA_SMS_TL_DEST_ADDR:
            messageCount = ExtractAddress(SmsPduP + index, &(SmsTxtMsgP->Address));
 	        if (messageCount == paramLen)
 	        {
                SmsTxtMsgP->FieldMask |= CDMA_SMS_ADDRESS_PRESENT;
 	        }
	        break;
	        
	    case CDMA_SMS_TL_ORIG_SUBADDR:
	    case CDMA_SMS_TL_DEST_SUBADDR:
	        messageCount = ExtractSubaddress(SmsPduP + index, &(SmsTxtMsgP->Subaddress));
	        if (messageCount == paramLen)
	        {
	            SmsTxtMsgP->FieldMask |= CDMA_SMS_SUBADDRESS_PRESENT;
	        }
	        break;
	        
	    case CDMA_SMS_TL_BEARER_RPLY_OPT:
            messageCount = ExtractBearerReplyOption( SmsPduP + index, (uint8*)&(SmsTxtMsgP->BearerReplyOption.replySeq) );            
 	        if (messageCount == paramLen)
 	        {
 	            SmsTxtMsgP->FieldMask |= CDMA_SMS_BEARER_REPLY_OPTION_PRESENT;
 	        }
	        break;

	        break;
	        
	    case CDMA_SMS_TL_CAUSE_CODE:
	        break;
	        
	    case CDMA_SMS_TL_BEARER_DATA:
	        if( paramLen > CDMA_SMS_TL_MAX_MSG_LENGTH )
	        {
	        }
	        else
	        {
	            ParseTeleLayerPdu(SmsPduP + index, paramLen, SmsTxtMsgP);
	            
	        }
	        break;
	    default:
            break;

        }
        
        index += paramLen;
    }
   
    return;
}





