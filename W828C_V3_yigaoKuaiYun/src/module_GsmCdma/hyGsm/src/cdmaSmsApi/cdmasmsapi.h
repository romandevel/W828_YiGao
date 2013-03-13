
#ifndef _CDMA_SMS_API_H_
#define _CDMA_SMS_API_H_

#include "cdmasmstype.h"


//#define PC_PROGRAME
#ifndef PC_PROGRAME
#include "hyhwRtc.h"
#endif
/*
定义数据的最大值
*/

#define MAX_DATA_LENGTH 255

/*
 *    Transport Layer Message Field Parameter Ids
 */
#define CDMA_SMS_TL_TELESERVICE_ID              (0)
#define CDMA_SMS_TL_SERVICE_CAT                 (1)
#define CDMA_SMS_TL_ORIG_ADDR                   (2)
#define CDMA_SMS_TL_ORIG_SUBADDR                (3)
#define CDMA_SMS_TL_DEST_ADDR                   (4)
#define CDMA_SMS_TL_DEST_SUBADDR                (5)
#define CDMA_SMS_TL_BEARER_RPLY_OPT             (6)
#define CDMA_SMS_TL_CAUSE_CODE                  (7)
#define CDMA_SMS_TL_BEARER_DATA                 (8)

/*
 *    Teleservice Layer Parameter Values
 */
#define CDMA_SMS_ASE_MESSAGE_ID                 (0)
#define CDMA_SMS_ASE_USER_DATA                  (1)
#define CDMA_SMS_ASE_USER_RESP_CODE             (2)
#define CDMA_SMS_ASE_TIMESTAMP                  (3) 
#define CDMA_SMS_ASE_VPA                        (4)
#define CDMA_SMS_ASE_VPR                        (5)
#define CDMA_SMS_ASE_DDTA                       (6)
#define CDMA_SMS_ASE_DDTR                       (7)
#define CDMA_SMS_ASE_PRIORITY                   (8)
#define CDMA_SMS_ASE_PRIVACY                    (9)
#define CDMA_SMS_ASE_RPLY_OPT                   (10)
#define CDMA_SMS_ASE_MSGNUM                     (11)
#define CDMA_SMS_ASE_ALERT                      (12)
#define CDMA_SMS_ASE_LANG                       (13)
#define CDMA_SMS_ASE_CALLBACK                   (14)
#define CDMA_SMS_ASE_DISPMODE                   (15)
#define CDMA_SMS_ASE_MENC_DATA                  (16)
#define CDMA_SMS_ASE_MSG_DEP_INDEX              (17)
#define CDMA_SMS_ASE_SRV_CAT_DATA               (18)
#define CDMA_SMS_ASE_SRV_CAT_RESULT             (19)
#define CDMA_SMS_ASE_MSG_STATUS                 (20)

#define CDMA_SMS_NUM_BYTES(bitCount)            ( ((bitCount) % 8) ? (((bitCount) / 8) + 1) : ((bitCount) / 8))
      
#define CDMA_SMS_MASK_TO_BOOL(mask, flag)       ((bool)(((mask) & (flag)) ? CDMA_TRUE : CDMA_FALSE))

#define CDMA_SMS_BOOL_TO_MASK(cond, flag)       (((cond) == CDMA_TRUE) ? (flag) : 0)

#define BIT_SIZE(type)  (sizeof(type) * 8)

#ifndef MIN
#define MIN(x,y)        (((x) < (y)) ? (x): (y))
#endif

#ifndef MAX
#define MAX(x,y)        (((x) > (y)) ? (x): (y))
#endif

#define MASK_TO_COPY(type)          ( (0xffffffff) >> (32 - BIT_SIZE(type)) )

#define BYTE_MASK(offset, len) \
    ( (0xff >> offset) & (0xff << (8 - (offset + len))) )

#define MASK(width, offset, data) \
    (((width) == BIT_SIZE(data)) ? (data) :   \
    ((((MASK_TO_COPY(data) << (BIT_SIZE(data) - ((width) % BIT_SIZE(data)))) & \
    MASK_TO_COPY(data)) >>  (offset)) & (data)))

#define MASK_AND_SHIFT(width, offset, shift, data)  \
        ((((signed) (shift)) < 0) ?       \
        MASK((width), (offset), (data)) << -(shift) :  \
        MASK((width), (offset), (data)) >>  (((unsigned) (shift))))

#define CDMA_SMS_MAX_ADDRESS_LEN                    (32)
#define CDMA_SMS_MAX_SUBADDR_LEN                    (30)
#define CDMA_SMS_MAX_USER_DATA_LEN                  (255)
#define CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS         (30)
#define CDMA_SMS_MAX_CATEGORIES                     (10)
#define CDMA_SMS_MAX_CHAR_IN_CATEGORY               (20)
#define CDMA_SMS_TL_MAX_MSG_LENGTH                  (255)
#define CDMA_SMS_MAX_NUM_DATA_MSGS                  (2)
                                                    
#define CDMA_SMS_MAX_REGISTERED_IDS                 (5)
#define CDMA_SMS_TELESRVID_COUNT                    (15)
                                                    
#define CDMA_SMS_MAX_SMS_PDU_LEN                    (253)

/*sms message type*/
typedef enum
{
    CDMA_SMS_SMS_MSG_TYPE_POINT_TO_POINT = 0,
    CDMA_SMS_SMS_MSG_TYPE_BROADCAST,
    CDMA_SMS_SMS_MSG_TYPE_ACKNOWLEDGE,
    CDMA_SMS_MAX_SMS_MSG_TYPE
}CdmaSmsTransMsgTypeT;


typedef enum
{
    CDMA_SMS_ERRCLASS_NO_ERROR,
    CDMA_SMS_ERRCLASS_TEMPORARY = 2,
    CDMA_SMS_ERRCLASS_PERMANENT = 3,
    CDMA_SMS_MAX_ERRCLASS
}CdmaSmsErrClassT;


typedef enum
{
    CDMA_SMS_NO_TERMINAL_PROBLEMS,
    CDMA_SMS_DESTINATION_RESOURSE_SHORTAGE,
    CDMA_SMS_DESTINATION_OUT_OF_SERVICE,
    CDMA_SMS_MAX_TERM_STATUS
}CdmaSmsTermStatusT;

typedef enum
{
     CDMA_SMS_CC_ADDRESS_VACANT                     = 0,
     CDMA_SMS_CC_ADDRESS_TRANSLATION_FAILURE,
     CDMA_SMS_CC_NETWORK_RESOURCE_SHORTAGE,
     CDMA_SMS_CC_NETWORK_FAILURE,
     CDMA_SMS_CC_INVALID_TELESERVICE_ID,
     CDMA_SMS_CC_OTHER_NETWORK_PROBLEM,
     CDMA_SMS_CC_NO_PAGE_RESPONSE_S                 = 32,
     CDMA_SMS_CC_DESTINATION_BUSY,
     CDMA_SMS_CC_NO_ACKNOWLEDGEMENT,
     CDMA_SMS_CC_DESTINATION_RESOURCE_SHORTAGE,
     CDMA_SMS_CC_SMS_DELIVERY_POSTPONED,
     CDMA_SMS_CC_DESTINATION_OUT_OF_SERVICE,
     CDMA_SMS_CC_DESTINATION_NO_LONGER_AT_THIS_ADDRESS,
     CDMA_SMS_CC_OTHER_TERMINAL_PROBLEM,
     CDMA_SMS_CC_RADIO_INTERFACE_RESOURCE_SHORTAGE  = 64,
     CDMA_SMS_CC_RADIO_INTERFACE_INCOMPATIBILITY,
     CDMA_SMS_CC_OTHER_RADIO_INTERFACE_PROBLEM,
     CDMA_SMS_CC_ENCODING_PROBLEM                   = 96,
     CDMA_SMS_CC_SMS_ORIGINATION_DENIED,
     CDMA_SMS_CC_SMS_TERMINATION_DENIED,
     CDMA_SMS_CC_SUPPLEMENTARY_SERVICE_NOT_SUPPORTED,
     CDMA_SMS_CC_SMS_NOT_SUPPORTED,
     CDMA_SMS_CC_MISSING_EXPECTED_PARAMETER         = 102,
     CDMA_SMS_CC_MISSING_MANDATORY_PARAMETER,
     CDMA_SMS_CC_UNRECOGNIZED_PARAMETER_VALUE,
     CDMA_SMS_CC_UNEXPECTED_PARAMETER_VALUE,
     CDMA_SMS_CC_USER_DATA_SIZE_ERROR,
     CDMA_SMS_CC_OTHER_GENERAL_PROBLEMS,
     CDMA_SMS_MAX_CC
} CdmaSmsCCT;

typedef enum
{
    CDMA_SMS_DIGIT_MODE_4_BIT           = 0,
    CDMA_SMS_DIGIT_MODE_8_BIT,
    CDMA_SMS_MAX_DIGIT_MODE
} CdmaSmsDigitModeT;

typedef enum
{
    CDMA_SMS_NUMBER_MODE_ANSI_T1_607        = 0,
    CDMA_SMS_NUMBER_MODE_DATA_NETWORK,
    CDMA_SMS_MAX_NUMBER_MODE
} CdmaSmsNumModeT;

typedef enum
{
    CDMA_SMS_NUMBER_TYPE_UNKNOWN            = 0,
    CDMA_SMS_NUMBER_TYPE_INTERNATIONAL,
    CDMA_SMS_NUMBER_TYPE_NATIONAL,
    CDMA_SMS_NUMBER_TYPE_NETWORK_SPECIFIC,
    CDMA_SMS_NUMBER_TYPE_SUBSCRIBER,
    CDMA_SMS_NUMBER_TYPE_RESERVED,
    CDMA_SMS_NUMBER_TYPE_ABBREVIATED,
    CDMA_SMS_MAX_NUMBER_TYPE
} CdmaSmsNumberT;


typedef enum
{
    CDMA_SMS_NUMBER_PLAN_UNKNOWN        = 0,
    CDMA_SMS_NUMBER_PLAN_TELEPHONY,
    CDMA_SMS_NUMBER_PLAN_DATA           = 3,
    CDMA_SMS_NUMBER_PLAN_TELEX,
    CDMA_SMS_NUMBER_PLAN_PRIVATE        = 9,
    CDMA_SMS_MAX_NUMBER_PLAN
} CdmaSmsNumPlanT;

typedef enum
{
    CDMA_SMS_DATA_NUM_TYPE_UNKNOWN = 0,
    CDMA_SMS_DATA_NUM_TYPE_RFC_791,       // Internet Protocol
    CDMA_SMS_DATA_NUM_TYPE_RFC_822,       // Internet Email Address
    CDMA_SMS_MAX_DATA_NUM_TYPE
} CdmaSmsDataNumberT;

typedef enum
{
    CDMA_SMS_SUBADDR_TYPE_NSAP    = 0,
    CDMA_SMS_SUBADDR_TYPE_USER,
    CDMA_SMS_MAX_SUBADDR_TYPE
} CdmaSmsSubAddrT;


typedef enum
{
    CDMA_SMS_SVR_CAT_UNKNOWN                    = 0,
    CDMA_SMS_SVR_CAT_EMERGENCIES,
    CDMA_SMS_SVR_CAT_ADMINISTRATIVE,
    CDMA_SMS_SVR_CAT_MAINTENANCE,
    CDMA_SMS_SVR_CAT_GEN_NEWS_LOCAL,
    CDMA_SMS_SVR_CAT_GEN_NEWS_REGIONAL,
    CDMA_SMS_SVR_CAT_GEN_NEWS_NATIONAL,
    CDMA_SMS_SVR_CAT_GEN_NEWS_INTERNATIONAL,
    CDMA_SMS_SVR_CAT_FINANCE_NEWS_LOCAL,
    CDMA_SMS_SVR_CAT_FINANCE_NEWS_REGIONAL,
    CDMA_SMS_SVR_CAT_FINANCE_NEWS_NATIONAL,
    CDMA_SMS_SVR_CAT_FINANCE_NEWS_INTERNATIONAL,
    CDMA_SMS_SVR_CAT_SPORTS_NEWS_LOCAL,
    CDMA_SMS_SVR_CAT_SPORTS_NEWS_REGIONAL,
    CDMA_SMS_SVR_CAT_SPORTS_NEWS_NATIONAL,
    CDMA_SMS_SVR_CAT_SPORTS_NEWS_INTERNATIONAL,
    CDMA_SMS_SVR_CAT_ENTERTAINMENT_NEWS_LOCAL,
    CDMA_SMS_SVR_CAT_ENTERTAINMENT_NEWS_REGIONAL,
    CDMA_SMS_SVR_CAT_ENTERTAINMENT_NEWS_NATIONAL,
    CDMA_SMS_SVR_CAT_ENTERTAINMENT_NEWS_INTERNATIONAL,
    CDMA_SMS_SVR_CAT_LOCAL_WEATHER,
    CDMA_SMS_SVR_CAT_TRAFFIC,
    CDMA_SMS_SVR_CAT_FLIGHT_SCHEDULE,
    CDMA_SMS_SVR_CAT_RESTAURANTS,
    CDMA_SMS_SVR_CAT_LODGINGS,
    CDMA_SMS_SVR_CAT_RETAIL_DIRECTORY,
    CDMA_SMS_SVR_CAT_ADVERTISEMENT,
    CDMA_SMS_SVR_CAT_STOCK_QUOTES,
    CDMA_SMS_SVR_CAT_EMPLOYMENT_OPPORTUNITIES,
    CDMA_SMS_SVR_CAT_MEDICAL_HEALTH,
    CDMA_SMS_SVR_CAT_TECH_NEWS,
    CDMA_SMS_SVR_CAT_MULTI_CATEGORY,
    CDMA_SMS_MAX_SVR_CAT
} CdmaSmsSrvCatT;

typedef enum
{
    /*----------------------------------------------------------------
       The following are not defined as a teleservice in IS-637,
       but they are used to indicate the teleservice type in Analog mode.
    -----------------------------------------------------------------*/
    CDMA_SMS_TS_ID_IS91_PAGE                = 0,
    CDMA_SMS_TS_ID_IS91_VOICE_MAIL          = 1,
    CDMA_SMS_TS_ID_IS91_SHORT_MESSAGE       = 2,

    /*----------------------------------------------------------------
       Voice mail notification through Message Waiting Indication in
       CDMA mode or Analog mode
    -----------------------------------------------------------------*/
    CDMA_SMS_TS_ID_VOICE_MAIL_MWI           = 3,

    /*----------------------------------------------------------------
     IS-637 Teleservices
    -----------------------------------------------------------------*/
    CDMA_SMS_TS_ID_IS91                      = 4096,
    CDMA_SMS_TS_ID_PAGE                      = 4097,
    CDMA_SMS_TS_ID_MESSAGE                   = 4098,
    CDMA_SMS_TS_ID_VOICE_MAIL                = 4099,
    CDMA_SMS_TS_ID_WAP,
    CDMA_SMS_TS_ID_WEMT ,        /*add for EMS*/
    CDMA_SMS_TS_ID_SCPT,         /*add for SCPT*/
    CDMA_SMS_TS_ID_CATPT,        /*add for UIM card application*/
    CDMA_SMS_BROAD_CAST_SRV,     /*add for the broadcast service*/
    CDMA_SMS_Reliance_BROAD_CAST_SRV         = 49172,
    CDMA_SMS_MAX_TS_ID
} CdmaSmsTeleSrvIdT;

typedef enum
{
    CDMA_SMS_MSG_TYPE_RESERVED                  = 0,
    CDMA_SMS_MSG_TYPE_TERMINATION_DELIVER,
    CDMA_SMS_MSG_TYPE_ORIGINATION_SUBMIT,
    CDMA_SMS_MSG_TYPE_ORIGINATION_CANCELLATION,
    CDMA_SMS_MSG_TYPE_TERMINATION_DELIVERY_ACK,
    CDMA_SMS_MSG_TYPE_USER_ACK,
    CDMA_SMS_MSG_TYPE_READ_ACK,
    CDMA_SMS_MAX_MSG_TYPE
} CdmaSmsTeleMsgT;


/*
  For Mobile Stations that support MT SMS in Indian languages, Octet Unspecified
  format shall be interpreted as UTF-8 encoded Unicode string. For Mobile
  Stations that do not support MT SMS in Indian languages and support only
  English, Octet Unspecified format shall be interpreted as 8-Bit ASCII format.
*/
typedef enum
{
    CDMA_SMS_MSG_ENCODE_OCTET                = 0,
    CDMA_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG,
    CDMA_SMS_MSG_ENCODE_7BIT_ASCII,
    CDMA_SMS_MSG_ENCODE_IA5,
    CDMA_SMS_MSG_ENCODE_UNICODE,
    CDMA_SMS_MSG_ENCODE_SHIFT_JIS,
    CDMA_SMS_MSG_ENCODE_KS_C_5601,
    CDMA_SMS_MSG_ENCODE_LATIN_HEBREW,
    CDMA_SMS_MSG_ENCODE_LATIN,
    CDMA_SMS_MAX_MSG_ENCODE
} CdmaSmsMsgEncodeT;

typedef enum
{
    CDMA_SMS_PRIORITY_NORMAL  = 0,
    CDMA_SMS_PRIORITY_INTERACTIVE,
    CDMA_SMS_PRIORITY_URGENT,
    CDMA_SMS_PRIORITY_EMERGENCY,
    CDMA_SMS_MAX_PRIORITY
} CdmaSmsPriorityT;


typedef enum
{
    CDMA_SMS_PRIVACY_NOT_RESTRICTED = 0,
    CDMA_SMS_PRIVACY_RESTRICTED,
    CDMA_SMS_PRIVACY_CONFIDENTIAL,
    CDMA_SMS_PRIVACY_SECRET,
    CDMA_SMS_MAX_PRIVACY
} CdmaSmsPrivacyT;


typedef enum
{
    CDMA_SMS_ALERT_MOBILE_DEFAULT = 0,
    CDMA_SMS_ALERT_LOW_PRIORITY,
    CDMA_SMS_ALERT_MEDIUM_PRIORITY,
    CDMA_SMS_ALERT_HIGH_PRIORITY,
    CDMA_SMS_MAX_ALERT
} CdmaSmsAlertT;


typedef enum
{
    CDMA_SMS_LANGUAGE_UNKNOWN = 0,
    CDMA_SMS_LANGUAGE_ENGLISH,
    CDMA_SMS_LANGUAGE_FRENCH,
    CDMA_SMS_LANGUAGE_SPANISH,
    CDMA_SMS_LANGUAGE_JAPANESE,
    CDMA_SMS_LANGUAGE_KOREAN,
    CDMA_SMS_LANGUAGE_CHINESE,
    CDMA_SMS_LANGUAGE_HEBREW,
    CDMA_SMS_MAX_LANGUAGE
} CdmaSmsLanguageT;

typedef enum
{
    CDMA_SMS_DISP_MODE_IMMEDIATE = 0,
    CDMA_SMS_DISP_MODE_DEFAULT,
    CDMA_SMS_DISP_MODE_USER_INVOKE,
    CDMA_SMS_DISP_MODE_RESERVED,
    CDMA_SMS_MAX_DISP_MODE
} CdmaSmsDisplayModeT;

typedef  struct PACKED1
{
    CdmaSmsDisplayModeT SmsDispMode;
    uint8 Reserved;
}CdmaSmsDispModeT;

typedef  struct PACKED2
{
    CdmaSmsDigitModeT   DigitMode;
    CdmaSmsNumberT      NumberType;
    CdmaSmsNumPlanT     NumberPlan;
    uint8               NumFields;
    uint8               Digits[CDMA_SMS_CALLBACK_NUMBER_MAX_DIGITS];
} CdmaSmsCbNumberT;


typedef union
{
      CdmaSmsNumberT      NumType;
      CdmaSmsDataNumberT  DataNumType;
}CdmaSmsNumberType;

typedef struct
{
    CdmaSmsDigitModeT       DigitMode;//几个BIT表示号码的一个位
    CdmaSmsNumModeT         NumberMode;//号码的模式
    CdmaSmsNumberType       NumberType;
    CdmaSmsNumPlanT         NumberPlan;
    uint8                   NumFields;
    uint8                   Address[CDMA_SMS_MAX_ADDRESS_LEN];
} CdmaSmsAddressT;


typedef  struct PACKED3
{
    CdmaSmsSubAddrT         SubaddrType;
    uint8                   Odd;
    uint8                   NumFields;
    uint8                   Address[CDMA_SMS_MAX_SUBADDR_LEN];
} CdmaSmsSubaddressT;


typedef  struct PACKED4
{
    CdmaSmsMsgEncodeT       MessageEncoding;
    uint8                   MessageType;
    uint8                   NumFields;
    uint8                   Data[CDMA_SMS_MAX_USER_DATA_LEN];
} CdmaSmsUserDataT;


typedef  struct PACKED5 /*all the fields here are BCD codes, be sure to use it!*/
{
    uint8   Year;
    uint8   Month;
    uint8   Day;
    uint8   Hours;
    uint8   Minutes;
    uint8   Seconds;
} CdmaSmsAbsTimeT;


typedef   struct PACKED6
{
    bool    UserAckReq;     /*true for user ack is required*/
    bool    DakReq;         /*true for delivery ack is required*/
    bool    ReadAckReq;     /*true for read ack is required*/
} CdmaSmsRplOptionT;

typedef enum
{
    CDMA_SMS_MSGSTATUS_ACCEPTED,
    CDMA_SMS_MSGSTATUS_DEPOSITED_TO_INTERNET,
    CDMA_SMS_MSGSTATUS_DELIVERED,
    CDMA_SMS_MSGSTATUS_CANCELLED,
    CDMA_SMS_MSGSTATUS_NETWORK_CONGESTION,
    CDMA_SMS_MSGSTATUS_NETWORK_ERROR,
    CDMA_SMS_MSGSTATUS_CANCEL_FAILED,
    CDMA_SMS_MSGSTATUS_BLOCKED_DESTINATION,
    CDMA_SMS_MSGSTATUS_TEXT_TOO_LONG,
    CDMA_SMS_MSGSTATUS_DUPLICATE_MESSAGE,
    CDMA_SMS_MSGSTATUS_INVALID_DESTINATION,
    CDMA_SMS_MSGSTATUS_EXPIRED,
    CDMA_SMS_MAX_MSGSTATUS
} CdmaSmsMsgStatusCodeT;


typedef   struct PACKED7
{
    CdmaSmsErrClassT        ErrorClass;
    CdmaSmsMsgStatusCodeT   MsgStatusCode;
} CdmaSmsMsgStatusT;

typedef enum
{
    CDMA_SMS_CATRESULT_SUCCESS,
    CDMA_SMS_CATRESULT_MEMORY_EXCEEDED,
    CDMA_SMS_CATRESULT_LIMIT_EXCEEDED,
    CDMA_SMS_CATRESULT_ALREADY_PROGRAMMED,
    CDMA_SMS_CATRESULT_NOT_PREVIOUSLY_PROGRAMMED,
    CDMA_SMS_CATRESULT_INVALID_MAX_MESSAGES,
    CDMA_SMS_CATRESULT_INVALID_ALERT_OPTION,
    CDMA_SMS_CATRESULT_INVALID_CATEGORY_NAME,
    CDMA_SMS_CATRESULT_UNSPECIFIED_FAILURE,
    CDMA_SMS_MAX_CATRESULT
} CdmaSmsCategoryResultCodeT;


typedef   struct PACKED8
{
    CdmaSmsSrvCatT Category;
    CdmaSmsCategoryResultCodeT  CategoryResult;
} CdmaSmsCategoryResultT;

typedef   struct PACKED9
{
    uint8 NumCategories;
    CdmaSmsCategoryResultT Cat[CDMA_SMS_MAX_CATEGORIES];
} CdmaSmsSrvCatProgRsltT;

typedef enum
{
    CDMA_SMS_OP_DELETE,
    CDMA_SMS_OP_ADD,
    CDMA_SMS_OP_CLEAR_ALL,
    CDMA_SMS_MAX_OP
} CdmaSmsOperationCodeT;

typedef enum
{
    VAL_NO_ALERT = 0,
    VAL_MOBILE_DEFAULT_ALERT,
    VAL_VIBRATE_ALERT_ONCE,
    VAL_VIBRATE_ALERT_REPEAT,
    VAL_VISUAL_ALERT_ONCE,
    VAL_VISUAL_ALERT_REPEAT,
    VAL_LOW_PRIORITY_ALERT_ONCE,
    VAL_LOW_PRIORITY_ALERT_REPEAT,
    VAL_MED_PRIORITY_ALERT_ONCE,
    VAL_MED_PRIORITY_ALERT_REPEAT,
    VAL_HIGH_PRIORITY_ALERT_ONCE,
    VAL_HIGH_PRIORITY_ALERT_REPEAT,
    CDMA_SMS_MAX_CAT_ALERT
} CdmaSmsCatAlertT;

typedef   struct PACKED10
{
     CdmaSmsOperationCodeT  OperationCode;
     CdmaSmsSrvCatT         SrvCat;
     CdmaSmsLanguageT       Lang;
     uint8                  MaxMsg; /*the maximum number of messages that may be stored in the mobile station for this Service Category*/
     CdmaSmsCatAlertT       Alert;
     uint8                  NumChar;
     uint8                  Chari[CDMA_SMS_MAX_CHAR_IN_CATEGORY];
} CdmaSmsCatDataT;

typedef   struct PACKED11
{
    CdmaSmsMsgEncodeT       MsgEncoding;
    uint8                   NumCat;
    CdmaSmsCatDataT         Cat[ CDMA_SMS_MAX_CATEGORIES ];
} CdmaSmsSrvCatProgDataT;

// field mask definitions
#define CDMA_SMS_SERVICE_CAT_PRESENT                (0x00000001)
#define CDMA_SMS_ADDRESS_PRESENT                    (0x00000002)
#define CDMA_SMS_SUBADDRESS_PRESENT                 (0x00000004)
#define CDMA_SMS_USER_DATA_PRESENT                  (0x00000008)
#define CDMA_SMS_USER_RESP_CODE_PRESENT             (0x00000010)
#define CDMA_SMS_MC_TIME_STAMP_PRESENT              (0x00000020)
#define CDMA_SMS_ABS_VAL_PERIOD_PRESENT             (0x00000040)
#define CDMA_SMS_REL_VAL_PERIOD_PRESENT             (0x00000080)
#define CDMA_SMS_ABS_DEF_DEL_TIME_PRESENT           (0x00000100)
#define CDMA_SMS_REL_DEF_DEL_TIME_PRESENT           (0X00000200)
#define CDMA_SMS_PRIORITY_IND_PRESENT               (0x00000400)
#define CDMA_SMS_PRIVACY_IND_PRESENT                (0x00000800)
#define CDMA_SMS_REPLY_OPTION_PRESENT               (0x00001000)
#define CDMA_SMS_NUM_MESSAGES_PRESENT               (0x00002000)
#define CDMA_SMS_ALERT_MSG_DEL_PRESENT              (0x00004000)
#define CDMA_SMS_LANGUAGE_IND_PRESENT               (0x00008000)
#define CDMA_SMS_CALLBACK_NUM_PRESENT               (0x00010000)
#define CDMA_SMS_MSG_DISP_MODE_PRESENT              (0x00020000)
#define CDMA_SMS_MULT_ENCODE_USER_DATA_PRESENT      (0x00040000)
#define CDMA_SMS_MESSAGE_DEPOSIT_INDEX_PRESENT      (0x00080000)
#define CDMA_SMS_MESSAGE_STATUS_PRESENT             (0x00400000)
#define CDMA_SMS_SER_CAT_PROGRAM_RESULT_PRESENT     (0x00800000)
#define CDMA_SMS_SER_CAT_PROGRAM_DATA_PRESENT       (0x01000000)
#define CDMA_SMS_BEARER_REPLY_OPTION_PRESENT        (0x02000000)


typedef uint16 CdmaSmsRecIdT;

typedef enum
{
    CDMA_SMS_STATE_FREE                 = 0,   //the block has not occupied by a sms message
    CDMA_SMS_STATE_READ                 = 1,
    CDMA_SMS_STATE_UNREAD               = 3,
    CDMA_SMS_STATE_SENT                 = 5,
    CDMA_SMS_STATE_UNSENT               = 7,
    CDMA_SMS_STATE_DRAFT_READ           = 9,
    CDMA_SMS_STATE_DRAFT_SENT           = 11,
    CDMA_SMS_STATE_DRAFT_UNSENT         = 13,
    CDMA_SMS_STATE_DUPLICATE_READ       = 15,
    CDMA_SMS_STATE_DUPLICATE_UNREAD     = 17,
    CDMA_SMS_STATE_MAX
}CdmaSmsStateT;

typedef   struct PACKED12
{
    uint8 replySeq;        
}CmdaSmsRlyOptionT;

typedef struct
{
    //required fields
    uint32                          FieldMask;          //the field mask
    CdmaSmsRecIdT                   SmsMsgRecId;        //短息存储的位置 the phisical position of sms record in the storage device, UI NEEDN't fill it
    CdmaSmsStateT                   SMSState;           //短息的状态 sms message status, UI may not fill it when sending, But if UI want to write or update a message, it must fill it.
    uint8                           NumUserData;        //数据长度number of user data
    CdmaSmsTransMsgTypeT            TransMsgType;       //点对点短信类型transport layer message type, UI may not fill it
    uint16                          MsgId;              //message id, required, UI NEEDN'T fill it
    bool                            HeaderInd;          // 是否有头标志message head indication, required. UI MUST give it a correct value. True for the user data field includes a User Data Header, else set false
    CdmaSmsTeleMsgT                 TeleMsgType;        //是发送，还是接受的SMS teleservice layer message type, required. UI MUST give it a correct value
    CdmaSmsTeleSrvIdT               TeleSrvId;          //teleservice id, required, UI MUST give it a correct value
    
    //following is the optional fields
    CdmaSmsSrvCatT                  SrvCat;             //for broadcasting message, required, else, optional
    CdmaSmsAddressT                 Address;            //required for submit and delivery message
    CdmaSmsSubaddressT              Subaddress;         //optional
    CmdaSmsRlyOptionT               BearerReplyOption;

    CdmaSmsUserDataT                UserData[CDMA_SMS_MAX_NUM_DATA_MSGS]; //user data or multi encoding user data//2
    uint8                           UserRspCode;
    CdmaSmsAbsTimeT                 TimeStamp;
    CdmaSmsAbsTimeT                 ValTimeAbs;
    uint8                           RelValPeriod;
    CdmaSmsAbsTimeT                 AbsDelTime;
    uint8                           RelDelPeriod;
    CdmaSmsPriorityT                Priority;
    CdmaSmsPrivacyT                 Privacy;
    CdmaSmsRplOptionT               RplOp;
    uint8                           NumMsgs;            //only for VMN message
    CdmaSmsAlertT                   Alert;
    CdmaSmsLanguageT                Lang;
    CdmaSmsCbNumberT                CallBackNum;
    CdmaSmsDispModeT                DispMode;
    uint16                          MsgDepIndex;
    CdmaSmsMsgStatusT               MsgStatus;          //used only for SMS delivery Acknowledgement message
    CdmaSmsSrvCatProgRsltT          SrvCatProgResult;   // only for submitting message
    CdmaSmsSrvCatProgDataT          SrvCatData;         //only for delivery message
} CdmaSmsMessageT;

#ifdef PC_PROGRAME
typedef unsigned char U8;
typedef unsigned short int U16;
typedef unsigned int U32;
typedef volatile struct RtcTime_st
{
	unsigned char	second;		// (0 ~ 59)
	unsigned char	minute;		// (0 ~ 59)
	unsigned char	hour;		// (0 ~ 23)
	unsigned char	week;		// Day of Week (SUN=0, MON, TUE, WED, THR, FRI, SAT=6)

	unsigned char	day;		// (1 ~ 28,29,30,31)
	unsigned char	month;		// (1 ~ 12)
	unsigned short int	year;
} RtcTime_st,*pRtcTime_st;
#endif

typedef struct _tCDMA_SEND_PEMER
{
	U8 pPhoneNum[CDMA_SMS_MAX_ADDRESS_LEN];
	U8 pData[MAX_DATA_LENGTH];
	U16 dataLen;
	CdmaSmsMsgEncodeT dataType;
	RtcTime_st timeStamp;
	
}tCDMA_SEND_PEMER;

extern void CdmaSmsPdu2Txt (uint8 *SmsPduP, uint8 Length, CdmaSmsMessageT *SmsTxtMsgP);
extern void CdmaSmsTxt2Pdu (CdmaSmsMessageT *SmsMsgP, uint8 *PduP, uint8 *LengthP);
extern bool CdmaSmsWritePdu(tCDMA_SEND_PEMER *pSendCell,U8 *send_data_str,U16 *data_length);
extern bool HexStringToPdu(uint8* PduMsgP, const char* SrcDataP, uint16 PduLen);

#endif

