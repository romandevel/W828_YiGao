#ifndef _TEMUX_H_
#define _TEMUX_H_

//#include "sysdefs.h"
#include "cdmasmstype.h"
#define OPERATION_BASIC              0
#define OPERATION_ADDVANCE           1
#define FRAME_UIH                    0
#define FRAME_UI                     1
#define FRAME_I                      2

#define  IOP_MUX_SLEEP_FLAG  MON_DEEP_SLEEP_IOP_1_FLAG
#define  MUX_INVALID_POS     0xff
#define  MUX_MAX_DLC_NUM 64

#define  MUX_RX_BUFF_NUM  8
#define  MUX_MAX_FRAME_SIZE   8192/*32768*/
#define  MUX_VALID_CHECKSUM   0xcf

#define  MUX_ARM_LOW(value)    ((value) & 0xff)
#define  MUX_ARM_HIGH(value)  (((value) & 0xff00) >> 8)

#define  MUX_FIELD_SIZE(field)     ((field & 0xf0) >> 4)
#define  MUX_FIELD_START(field)  ((field) & 0x0f)
#define  MUX_FIELD_MASK(field)    ((1 << MUX_FIELD_SIZE(field)) - 1)
#define  MUX_STD_MASK(field)       (MUX_FIELD_MASK(field) << MUX_FIELD_START(field))
#define  MUX_GET_FIELD(value, field) (((value) >> MUX_FIELD_START(field)) & MUX_FIELD_MASK(field))
#define  MUX_MAKE_FIELD(value, field) (((value) & MUX_FIELD_MASK(field)) << MUX_FIELD_START(field))

#define  MUX_ADDR_MAKE_FIELD(p,value, field) (*p = ((((value) & MUX_FIELD_MASK(field)) << MUX_FIELD_START(field)) |\
	                                                                          ((*p) & ~ MUX_STD_MASK(field))))	                                                                         

#define  MUX_PRINT   MonPrintf
#define  MUX_FAULT()    do                           \
                                      {                              \
                                              MonFault(MON_IOP_FAULT_UNIT, IOP_MUX_ERROR, __LINE__, MON_CONTINUE);         \
                                      }while(0)

#define  MUX_CTS_RTS_DEASSERT  1
#define  MUX_CTS_RTS_ASSERT      0
#define  MUX_RTS_GPIO            HWD_GPIO_GI_5
#define  MUX_CTS_GPIO            HWD_GPIO_GI_4

#define  MUX_PBIT_FIELD   0x14
#define  MUX_CRBIT_FIELD 0x11
#define  MUX_EABIT_FIELD 0x10
#define  MUX_FC_FIELD       0x11
#define  MUX_RTC_FIELD     0x12
#define  MUX_RFR_FIELD     0X13
#define  MUX_DV_FIELD       0x17
#define  MUX_DLC_FIELD     0x62 
#define  MUX_MSG_TYPE_FIELD 0x62
#define  MUX_MSG_LEN_FIELD   0x71
#define  MUX_DATA_SERVICE_FIELD 0x11
#define  MUX_VOICE_SERVICE_FIELD 0x12

#define  MUX_DEFAULT_ACK_TIME  100  /*ms*/

#define  MUX_CTRL_CHNNL_BASIC_N1   128
#define  MUX_CTRL_CHNNL_ADDV_N1    512
#define  MUX_DATA_CHNNL_N1   32768

#define  MUX_INVALID_SIGNAL 0xff

#define  MUX_CTRL_MSG_BUFF             24

#define  MUX_ASSERT(TorF)  do \
                                              {   \
                                                  if(!TorF) \
						            MUX_PRINT("ASSERT FAIL, %d", __LINE__); \
                                              }while(0);
                                                              
#define  MUX_HDLR_STATE_NUM           2
#define  MUX_TE_WAKEUP_FLAG           0x0f
#define  MUX_UE_WAKEUP_FLAG           0xf0

#define  IOP_MUX_POOL_SIZE             1024 *20
#define  IOP_MUX_POOL_MINI_ALLOC        12

#define  MUX_BASIC_MODE_FLAG            0xF9
#define  MUX_ADDV_MODE_FLAG             0x7E
#define  MUX_CONTROL_FLAG               0x7D    

typedef enum
{
    MUX_SABM = 0x2f,
    MUX_UA = 0x63,
    MUX_DM = 0x0f,
    MUX_DISC = 0x43,
    MUX_UIH = 0xef,
    MUX_UI = 0x03,
    MUX_I = 0x0,
    MUX_RR = 0x01,
    MUX_RNR = 0x05,
    MUX_REJ = 0x09
}MuxFrameT;

typedef enum
{
    MUX_CTRL_MSG_CLD = 0xC0,
    MUX_CTRL_MSG_PN = 0x80,
    MUX_CTRL_MSG_PSC = 0x40,
    MUX_CTRL_MSG_TEST = 0x20,
    MUX_CTRL_MSG_FCON = 0xA0,
    MUX_CTRL_MSG_FCOFF = 0x60,
    MUX_CTRL_MSG_MSC = 0xE0,
    MUX_CTRL_MSG_NSC = 0x10,
    MUX_CTRL_MSG_RPN = 0x90,
    MUX_CTRL_MSG_RLS = 0x50,
    MUX_CTRL_MSG_SNC = 0xD0
}MuxCtrlMsgT;

typedef enum
{
    MUX_TRANSP_NONE,
    MUX_TRANSP_SUPPORTED,
    MUX_TRANSP_MAX_NUM
}MuxTranspT;

typedef enum
{
    MUX_MODE_BASIC,
    MUX_MODE_ADDV_WITHOUT_ER,
    MUX_MODE_ERM,
    MUX_MODE_MAX_NUM
}MuxModeT;


typedef enum
{
    /*can NOT modify the sequence*/
    MUX_STATE_IDLE,
    MUX_CMD_SEND,
    MUX_RES_SEND,
    MUX_CMD_RECV,
    MUX_RES_RECV 
}MuxCtrlHdlrStateT;

typedef enum
{
    MUX_CMD_NONE,
    MUX_SABM_CMD,
    MUX_DIS_CMD,
    MUX_CTRL_PN_CMD,
    MUX_CTRL_PSC_CMD,
    MUX_CTRL_TEST_CMD,
    MUX_CTRL_FC_ON_CMD,
    MUX_CTRL_FC_OFF_CMD,
    MUX_CTRL_MSC_CMD,
    MUX_CTRL_RPN_CMD,
    MUX_CTRL_RLS_CMD,
    MUX_CTRL_SNC_CMD,
    MUX_CMD_INVALID,
    MUX_CMD_MAX_NUM
}MuxCmdT;

typedef enum
{
    MUX_RW_STATE_START,     /* start flag*/
    MUX_RW_STATE_ADDR,
    MUX_RW_STATE_CONTROL,
    MUX_RW_STATE_LENGTH_LOW,
    MUX_RW_STATE_LENGTH_HIGH,
    MUX_RW_STATE_INFO,
    MUX_RW_STATE_SIGNAL,
    MUX_RW_STATE_FCS,
    MUX_RW_STATE_END,         /*END flag*/
    MUX_RW_STATE_TE_WAKEUP_UE,   /*this state is only for TE wake up UE*/
    MUX_RW_STATE_UE_WAKEUP_TE,
    MUX_RW_STATE_UE_WAKEUP_TE_END
}MuxRWStateT;

typedef enum
{
    MUX_BAUDRATE_9600 = 0x01,
    MUX_BAUDRATE_19200,
    MUX_BAUDRATE_38400,
    MUX_BAUDRATE_57600,
    MUX_BAUDRATE_115200,
    MUX_BAUDRATE_230400,
    MUX_BAUDRATE_MAX_NUM
}MuxBaudarateT;

typedef enum
{
    MUX_DATA_BIT_5 = 0x0,
    MUX_DATA_BIT_6,
    MUX_DATA_BIT_7,
    MUX_DATA_BIT_8,
    MUX_DATA_BIT_MAX_NUM
}MuxDataBitT;

typedef enum
{
    MUX_STOP_BIT_1 = 0x0,
    MUX_STOP_BIT_15,
    MUX_STOP_BIT_MAX_NUM
}MuxStopBitT;

typedef enum
{
    MUX_PARITY_ODD = 0x0,
    MUX_PARITY_EVEN,
    MUX_PARITY_MARK,
    MUX_PARITY_SPACE,
    MUX_PARITY_MAX_NUM
}MuxParityT;

typedef enum
{
    TIMER_STATE_NOT_START = 0x0,
    TIMER_STATE_SLEEP,
    TIMER_STATE_ACTIVE
}MuxTimerStateT;

typedef enum
{
    CTRL_MSG_START,
    CTRL_MSG_PN = CTRL_MSG_START,
    CTRL_MSG_PSC,
    CTRL_MSG_CLD,
    CTRL_MSG_TEST,
    CTRL_MSG_FCON,
    CTRL_MSG_FCOFF,
    CTRL_MSG_MSC,
    CTRL_MSG_NSC,
    CTRL_MSG_RPN,
    CTRL_MSG_RLS,
    CTRL_MSG_SNC,
    CTRL_MSG_END,
    CTRL_MSG_NUM = CTRL_MSG_END
}CtrlMsgIdT;

typedef enum
{
    MUX_BUFF_STATE_IDLE,
    MUX_BUFF_STATE_FOR_READ,
    MUX_BUFF_STATE_PENDING,
    MUX_BUFF_STATE_WAIT_FOR_ACK  /*BUFFER is shared between MUX with app */
}MuxBuffStateT;

typedef enum
{
    MUX_TE_STATE_WAKEUP,
    MUX_TE_STATE_WAKEUPING,
    MUX_TE_STATE_SLEEP
}MuxTeStateT;

typedef struct 
{
    uint8 TaskId;
    uint8 MsgId;
    uint8 MailboxId;
}ExeRspMsgT;

typedef struct 
{
    uint8  *Data;
    uint16 DataLen;
    uint8  DLC;
    ExeRspMsgT RspInfo;
}IopAppRxMsgT;

typedef struct
{
    MuxFrameT                frameType;
//    IopMuxConvergeLayerT   convergeLayerType; /*Only for data channel*/
    uint8                         priority; 
    uint8                         ackTime;                         /*Acknowledgement timer, if dlc = 0, T2, else T1*/
    uint16                       N1;                              /*Max frame size*/
    uint8                         N2;                              /*Max times of retrnsmissions*/
    uint8                         K;                                /*Window size*/
    uint8                         T3;                              /*Wake up response timer, only for information*/
}MuxSysParaT;

typedef struct ListNode
{
    struct ListNode  *pNext;
    struct ListNode  *pPre;
    uint8                  priority;
}ListNodeT;

struct MuxDataDlcObj;
typedef struct
{
    MuxTimerStateT    timerState;        /*if dlc = 0 it is T2, else it is T1*/
//    ExeTimerT            timer;
    uint32                  timerOrigTime;
    uint32                  timerRestTime;  /*used when sleep, for timer freezed*/
    uint8                    count;
    void                     (*pTimerFunc)(uint32);
}TimerObjT;

typedef struct
{
    MuxBaudarateT   baudrate;
    MuxDataBitT        dataBit;
    MuxStopBitT        stopBit;
    bool                    parityEnable;
    MuxParityT          parityType;
    uint8                   FCEnableBit;    /*Bit1 XON/XOFF on input. BIT is counted base on 1
                                                          Bit2 XON/XOFF on output
                                                          Bit3 RTR on input
                                                          Bit4 RTR on output
                                                          Bit5 RTC on input
                                                          Bit6 RTC on output*/
    uint8                   xOnChar;          /*default, DC1*/
    uint8                   xOffChar;          /*default DC3*/
}MuxDLCPortParaT;

typedef struct
{
    uint8                     *buffer;
    uint16                    totalLen;
    uint16                    pos;
}MuxBuffInfoT;

typedef struct
{
    uint8                      flagStart;
    uint8                      addr;
    uint8                      control;
    uint8                      length[2];
    bool                       isSignalOcetExist;
    uint8                      signal;
    MuxBuffInfoT          writeInfo;
    uint8                      fcs;
    uint8                      flagEnd;
}MuxFrameStructT;

typedef struct
{
    bool                       isValid;
    MuxRWStateT         state;
    MuxFrameStructT    frame;
    MuxTranspT           curMode;	
    uint8                      priority;

    /*for transparency transmit*/
    bool                       isEscape;
    uint8                      escapeByte;

    /*for data rx ack*/
    ExeRspMsgT           resInfo;
    bool                       isFollowed;
}MuxWriteInfoT;

typedef struct
{
    ListNodeT               node;
    MuxWriteInfoT        *pWriteInfo;
}MuxWriteInfoListT;

typedef struct
{
    ListNodeT             node;
    uint8                    posNum;
}MuxRxDataListT;

typedef struct MuxDataDlcObj
{
    uint8                    DLCNum;
    bool                     established;

    bool                     pBitSend;

    MuxModeT            muxMode;

    MuxSysParaT        systemPara;      /*Negotiated or supported system parameter*/
    MuxDLCPortParaT portPara;           /*Negotiated or supported system parameter*/

    bool                      fcs1;                  /*bit2(flow control) of MSC or convergence layer2 control ocet*/
    bool                      fcs3;                  /*inner buffer is full*/
    bool                      rtc;
    bool                      rtr;
	
    bool                      localFcs1;
    bool                      localFcs3;         /*True - can NOT accept data*/

    bool                      localRtc;
    bool                      localDv;

    TimerObjT             timer;

//    IopMuxDLCServiceT	 serType;

    /*Following member is only for error recovery mode*/
    uint8                     NR;                     /*Receive state variable*/
    uint8                     NS;                     /*Receive state variable*/

    /*if TE temporaly can NOT accept frame, the frame will be hang up in this list */
    MuxWriteInfoListT  *pFrameHead;   /*pending list*/

    MuxRxDataListT     *pWaitForAckList;
    MuxRxDataListT     *pWaitForSendList;

    /*following is for IPC between iop to app*/
    ExeRspMsgT          resInfo;	
}MuxDataDlcObjT;

typedef struct
{
    MuxDataDlcObjT	          dlc0;
    TimerObjT                   wakeupTimer;
}MuxCtrlDlcObjT;

typedef struct
{
    IopAppRxMsgT                 data;
    bool                         followed;
}MuxAppRxMsgT;

typedef struct
{
    ListNodeT                         node;
 //   bool                                alloc[IOP_MUX_MAX_REQUEST_DLC_NUM];
//    IopMuxRegParaT              regData;
}MuxRegParaT;


typedef struct
{
    bool                        muxActive;                              /*whether mux is enable*/

    bool                        aggreFcs;                                 /*aggregate flow control*/
    bool                        localAggreFcs;

    bool                        isUeWakeupTe;
    MuxTeStateT           isTeSleep;                                /*whether te is in sleep status*/
   
    MuxTranspT             transpType;                             /*basic or addvance.*/

    MuxDataDlcObjT      *pDlcCtrl[MUX_MAX_DLC_NUM];
    uint8                       nextDlc[MUX_MAX_DLC_NUM];   /*priority list*/

    MuxRegParaT            *pPendingReqH;
	
    bool                   pBitSend;                                 /*only for cmd send*/
    MuxCmdT                pBitCmdSend;

}MuxControlStructT;

int MuxFrameConstruct(int fd,uint8 dlc, MuxCtrlHdlrStateT state, MuxFrameT framType, uint8 *pData, uint16 len, bool pbit);
bool MuxFrameDestruct(uint8 *pData, uint16 len, MuxWriteInfoT *pFrameInfo);
#endif
