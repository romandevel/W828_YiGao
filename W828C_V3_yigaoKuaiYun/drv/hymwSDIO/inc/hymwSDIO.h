#ifndef HY_MW_SDIO_H_
#define HY_MW_SDIO_H_

#include "hytypes.h"

/*-----------SDIO command index----------------------------------*/
#define IO_SEND_OP_COND_CMD0	            0
#define IO_SEND_OP_COND_CMD5_R4             5
#define SEND_RELATIVE_ADDR_CMD3_R6          3
#define SELECT_DESELECT_CARD_CMD7_R1_R1b	7
#define IO_RW_DIRECT_CMD52_R5               52
#define IO_RW_EXTENDED_CMD53_R5             53

#define SDIO_CMD3_RESPONSE_CHECK_BIT	(BIT13|BIT14|BIT15)
#define SDIO_CMD7_RESPONSE_CHECK_BIT	(BIT25|BIT23|BIT22|BIT21|BIT20|BIT19)
#define SDIO_CMD52_RESPONSE_CHECK_BIT   (0xcb00)//BIT 8,9,11,14,15为校验位
#define SDIO_CMD53_RESPONSE_CHECK_BIT   (0xcb00)



#define SDIO_READ				0
#define SDIO_WRITE				1






typedef enum sdioBusWidth
{
	CCCR_BusWidth_1bit=0,
	CCCR_BusWidth_4bit=BIT1
} eSDIOBusWidth;

#define FUNCTION_0				    0
#define FUNCTION_1				    1
#define FUNCTION_2				    2
#define FUNCTION_3				    3
#define FUNCTION_4				    4
#define FUNCTION_5				    5
#define FUNCTION_6				    6
#define FUNCTION_7				    7

/*-----------SDIO OCR register--------------------------------*/
#define SDIO_OCR_VOLTAGE	0x00fe0000		//2.7~3.6V

#define CCCR_SDIO_REVISION			0x0

#define CCCR_IO_EN					0x2
#define IO_ENABLE_FUNCTION_1	    BIT1
#define IO_DISABLE_FUNCTION_1	    0

#define CCCR_IO_READY				0x3
#define IO_FUNCTION_1_READY 		BIT1

#define CCCR_INTERRUPT_ENABLE		0x4
#define Func_1_INT_ENABLE  		    BIT1
#define INT_ENABLE_MASTER		    BIT0

#define CCCR_INTERRUPT_PENDING		0x5

#define CCCR_IO_ABORT				0x6
#define IO_ABORT_RESET              BIT3
#define IO_ABORT_FN(X)              (X)

#define CCCR_BUS_INTERFACE_CTRL		0x7
#define CD_DISABLE					BIT7
#define BUS_WIDTH_4_BIT				BIT1
#define BUS_WIDTH_1_BIT				0

#define CCCR_CARD_CAPABILITY		0x8
#define ENABLE_4_BIT_DAT_INT    	BIT5
#define SUPPORT_4_BIT_DAT_INT	    BIT4

#define CCCR_POWER_CTRL				0x12
#define EnableMasterPowerCtrl		BIT1
#define SupportMasterPowerCtrl		BIT0

#define CCCR_HIGH_SPEED				0x13
#define ENABLE_HIGH_SPEED			BIT1
#define Support_HIGH_SPEED			BIT0


/*-----------SDIO OCR register--------------------------------*/
#define FBR_BLOCK_SIZE_0            0x110
#define FBR_BLOCK_SIZE_1            0x111



/* Host Control Registers */
#define IO_PORT_0_REG			0x00
#define IO_PORT_1_REG			0x01
#define IO_PORT_2_REG			0x02
#define CONFIGURATION_REG		0x03
#define HOST_WO_CMD53_FINISH_HOST	(0x1U << 2)
#define HOST_POWER_UP			(0x1U << 1)
#define HOST_POWER_DOWN			(0x1U << 0)
#define HOST_INT_MASK_REG		0x04
#define UP_LD_HOST_INT_MASK		(0x1U)
#define DN_LD_HOST_INT_MASK		(0x2U)
#define HOST_INTSTATUS_REG		0x05
#define UP_LD_HOST_INT_STATUS		(0x1U)
#define DN_LD_HOST_INT_STATUS		(0x2U)
#define HOST_INT_RSR_REG		0x06
#define UP_LD_HOST_INT_RSR		(0x1U)
#define HOST_INT_STATUS_REG		0x07
#define UP_LD_CRC_ERR			(0x1U << 2)
#define UP_LD_RESTART              	(0x1U << 1)
#define DN_LD_RESTART              	(0x1U << 0)

/* Card Control Registers */
#define SQ_READ_BASE_ADDRESS_A0_REG  	0x10
#define SQ_READ_BASE_ADDRESS_A1_REG  	0x11
#define SQ_READ_BASE_ADDRESS_A2_REG  	0x12
#define SQ_READ_BASE_ADDRESS_A3_REG  	0x13
#define SQ_READ_BASE_ADDRESS_B0_REG  	0x14
#define SQ_READ_BASE_ADDRESS_B1_REG  	0x15
#define SQ_READ_BASE_ADDRESS_B2_REG  	0x16
#define SQ_READ_BASE_ADDRESS_B3_REG  	0x17
#define CARD_STATUS_REG              	0x20
//#define IO_READY                     	(0x1U << 3)
#define CIS_CARD_RDY                 	(0x1U << 2)
#define UP_LD_CARD_RDY               	(0x1U << 1)
#define DN_LD_CARD_RDY               	(0x1U << 0)
#define HOST_INTERRUPT_MASK_REG      	0x24
#define HOST_POWER_INT_MASK          	(0x1U << 3)
#define ABORT_CARD_INT_MASK          	(0x1U << 2)
#define UP_LD_CARD_INT_MASK          	(0x1U << 1)
#define DN_LD_CARD_INT_MASK          	(0x1U << 0)
#define CARD_INTERRUPT_STATUS_REG    	0x28
#define POWER_UP_INT                 	(0x1U << 4)
#define POWER_DOWN_INT               	(0x1U << 3)
#define CARD_INTERRUPT_RSR_REG       	0x2c
#define POWER_UP_RSR                 	(0x1U << 4)
#define POWER_DOWN_RSR               	(0x1U << 3)
#define DEBUG_0_REG                  	0x30
#define SD_TESTBUS0                  	(0x1U)
#define DEBUG_1_REG                  	0x31
#define SD_TESTBUS1                  	(0x1U)
#define DEBUG_2_REG                  	0x32
#define SD_TESTBUS2                  	(0x1U)
#define DEBUG_3_REG                  	0x33
#define SD_TESTBUS3                  	(0x1U)
#define CARD_OCR_0_REG               	0x34
#define CARD_OCR_1_REG               	0x35
#define CARD_OCR_3_REG               	0x36
#define CARD_CONFIG_REG              	0x38
#define CARD_REVISION_REG            	0x3c
#define CMD53_FINISH_GBUS            	(0x1U << 1)
#define SD_NEG_EDGE                  	(0x1U << 0)

#define BUS_INTERFACE_CONTROL_REG 	0x07
#define BUS_WIDTH(reg)			((reg) & 0x3)

#define CCR_CARD_STATUS_REGISTER        0x20

#define LENGTH_SCRATCH_REGISTER1                        0x34
#define LENGTH_SCRATCH_REGISTER2                        0x35
#define PACKET_TYPE_MAC_EVENT_SCRATCH_REGISTER          0x36
/* Special registers in function 0 of the SDxx card */
#define	SCRATCH_0_REG			0x34//0x80fe
#define	SCRATCH_1_REG			0x35//0x80ff

#define HOST_F1_RD_BASE_0		0x0010
#define HOST_F1_RD_BASE_1		0x0011
#define HOST_F1_CARD_RDY		0x0020
#define SDIO_IO_PORT                0x10000

#define	HIM_DISABLE			0xff
#define HIM_ENABLE			0x03

#define FIRMWARE_READY			0xfedc
#ifndef DEV_NAME_LEN
#define DEV_NAME_LEN			32
#endif
#define MAXKEYLEN			13

/* The number of times to try when polling for status bits */
#define MAX_POLL_TRIES			1000

/* The number of times to try when waiting for downloaded firmware to 
     become active. (polling the scratch register). */

#define MAX_FIRMWARE_POLL_TRIES		100

#define FIRMWARE_TRANSFER_NBLOCK	2
#define SBI_EVENT_CAUSE_SHIFT		3
#define SD_BLOCK_SIZE_512	512
#define SD_BLOCK_SIZE_320	320
#define SD_BLOCK_SIZE_32	32
#define SDIO_HEADER_LEN		4
#define SD_BLOCK_SIZE		32
#define IO_READY                BIT31
/** INT Status Bit Definition*/
#define HIS_RxUpLdRdy			BIT0
#define HIS_TxDnLdRdy			BIT1
#define HIS_CmdDnLdRdy			BIT2
#define HIS_CardEvent			BIT3
#define HIS_CmdUpLdRdy			BIT4
#define HIS_WrFifoOvrflow		BIT5
#define HIS_RdFifoUndrflow		BIT6
#define HIS_WlanReady			BIT7

#define HCR_HOST_CONFIGURATION_REGISTER     0x3

/* This is for firmware specific length */
#define EXTRA_LEN	36 
#define MRVDRV_MAXIMUM_ETH_PACKET_SIZE	1514  

#define MRVDRV_ETH_TX_PACKET_BUFFER_SIZE \
	(MRVDRV_MAXIMUM_ETH_PACKET_SIZE + sizeof(TxPD) + EXTRA_LEN)

#define MRVDRV_ETH_RX_PACKET_BUFFER_SIZE \
	(MRVDRV_MAXIMUM_ETH_PACKET_SIZE + sizeof(RxPD) \
	 + MRVDRV_SNAP_HEADER_LEN + EXTRA_LEN)


#define ARGUMENT_RW_BIT(X)      ( (X & BIT0)<<31 )      
#define ARGUMENT_FN_BIT(X)      ( ( X & (BIT0|BIT1|BIT2) )<<28 )
#define ARGUMENT_DMODE_BIT(X)   ( (X & BIT0)<<27 )
#define ARGUMENT_AMODE_BIT(X)   ( (X & BIT0)<<26 )
#define ARGUNENT_ADDR_BIT(X)    ( (X & 0x1ffff)<<9 )//address共有17位
#define ARGUMENT_DATA_BIT(X)    (  X & 0x1ff )



/* SDIO 读/写 选择 */
typedef enum DIRECTION
{    
    CMD53_READ,
    CMD53_WRITE
}eDIRECTION;


typedef enum _mv_sd_type{
	MVSD_DAT = 0,
	MVSD_CMD = 1,
	MVSD_TXDONE = 2,
	MVSD_EVENT
} mv_sd_type;
/* SDIO 同步模式/异步模式 选择 */
typedef enum EXECMODE
{
    CMD53_SYNCHRONOUS,
    CMD53_ASYNCHRONOUS
}eEXECMODE;

/*  读写时，byte读写/block读写 选择 */
typedef enum DATAMODE
{
    CMD53_BYTE_BASIS,
    CMD53_BLOCK_BASIS
}eDATAMODE;

typedef enum ADDRMODE
{
    CMD53_FIXED_ADDRESS,
    CMD53_INCREMENTAL_ADDRESS
}eADDRMODE;

typedef struct CMD53Request
{
    eDIRECTION  direction; //HIF_READ/HIF_WRITE
    eDATAMODE   dmode;     //	HIF_BYTE_BASIS/HIF_BLOCK_BASIS
    eADDRMODE   amode;     //	HIF_FIXED_ADDRESS/HIF_INCREMENTAL_ADDRESS
}tCMD53Request;

typedef struct CMD53argumet
{
	eDIRECTION  RWFlag;
	U32         FunctionNum;
	eDATAMODE   blockmode;
	eADDRMODE   opcode;
	U32         RegAddr;
	U32         ByteBlockCount;
}tCmd53Parameter;

#define hymwSDIO_mCMD53_FrameRequest(request,a,b,c) \
		(request)->direction = a;          \
		(request)->dmode = b;              \
		(request)->amode = c
		
/***************************************************************************
* 函数:        	hymwSDIO_SendCommand_C52_R5

* 功能:        	发布CMD52命令

* 参数:        	RwDirection    -- 1: read  0:write
                functionNumber -- SDIO 有 0~7 共 8 个function 模块
                RegAddress     -- SDIO卡上待操作的寄存器地址，共 17 bit
                WriteData      -- 要写入的 unsigned char 型数据。
                                  若为读，则该值为 0

* 返回:        	出错标志
****************************************************************************/
U32 hymwSDIO_SendCommand_C52_R5(U32 RwDirection, U32 functionNumber, 
                               U32 RegAddress, U8 *pDataTemp);                               
/***************************************************************************
* 函数:        hymwSDIO_Initalize

* 功能:        SDIO卡初始化

* 参数:        无

* 返回:        错误标志
****************************************************************************/
U32 hymwSDIO_Initalize( void );

/***************************************************************************
* 函数:        	hymwSDIO_CMD53ReadWrite

* 功能:        	4-bit mode 数据读写

* 参数:        	Address -- target address 
                pBuffer -- buffer 指针
                Length -- operating data length
                Request -- 其他操作信息 
                           1,  read/write
                           2,  byte unit/block unit
                           3,  fixed address/incremental address                  

* 返回:        	错误标志
****************************************************************************/
U32 hymwSDIO_CMD53ReadWrite(U32 Address, U8 *pBuffer, U32 Length, 
                            tCMD53Request *Request);

int mv_sdio_read_scratch(  U16 *dat );
#endif