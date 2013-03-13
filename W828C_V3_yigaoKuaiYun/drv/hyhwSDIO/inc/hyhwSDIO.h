#ifndef _HY_HW_SDIO_H_
#define _HY_HW_SDIO_H_

#include "hyhwChip.h"
#include "hyTypes.h"

/***********************************************************************
*	 SD/MMC Controller Register Define				(Base Addr = 0x8000e000)
************************************************************************/
#define SD_PORT_CTRL_REGSITER				TCC7901_SDPORTCTRL_REGISTER
	#define	SLOT0_PORT_SELECT(X)					(X)
	#define SLOT1_PORT_SELECT(X)					(X<<4)
#define SDMA_SYSTEM_ADDRESS_REGSITER		TCC7901_SDIO_SDMA_REGISTER
#define BLOCK_SIZE_REGISTER					TCC7901_SDIO_BSIZE_REGISTER
#define	BLOCK_COUNT_REGISTER				TCC7901_SDIO_BCNT_REGISTER
#define ARGUMENT_REGISTER					TCC7901_SDIO_ARG_REGISTER
#define TRANSFER_MODE_REGISTER			    TCC7901_SDIO_TMODE_REGISTER
                
#define COMMAND_REGISTER					TCC7901_SDIO_CMD_REGISTER
#define RESPONSE0_REGISTER					TCC7901_SDIO_RESP0_REGISTER
#define RESPONSE1_REGISTER					TCC7901_SDIO_RESP1_REGISTER
#define RESPONSE2_REGISTER					TCC7901_SDIO_RESP2_REGISTER
#define RESPONSE3_REGISTER					TCC7901_SDIO_RESP3_REGISTER
#define RESPONSE4_REGISTER					TCC7901_SDIO_RESP4_REGISTER
#define RESPONSE5_REGISTER					TCC7901_SDIO_RESP5_REGISTER
#define RESPONSE6_REGISTER					TCC7901_SDIO_RESP6_REGISTER
#define RESPONSE7_REGISTER					TCC7901_SDIO_RESP7_REGISTER
#define BUFFER_DATA_PORT_REGISTER		    TCC7901_SDIO_DATAL_REGISTER
#define BUFFER_DATA_PORT_REGISTER_REG8      TCC7901_SDIO_DATA_REGISTER_REG8
         

/*------------------- present state register -------------------------*/
#define	PRESENT_STATE_REGISTER			TCC7901_SDIO_STATEL_REGISTER
    #define STATE_BIT_BURRER_RD_ENABLE      BIT11
    #define STATE_BIT_BURRER_WR_ENABLE      BIT10
    #define STATE_BIT_DATALINE_ACT			BIT1	// can't issue data
    #define STATE_BIT_CMDLINE_INUSE			BIT0	// can't issue command

#define DAT_CMD_LINE_ACT  \
   ( PRESENT_STATE_REGISTER & (STATE_BIT_DATALINE_ACT|STATE_BIT_CMDLINE_INUSE) )
    
/*------------------- power control/host control register -------------------------*/
#define	POWER_HOST_CONTROL_REGISTER			TCC7901_SDIO_CONTL_REGISTER
    #define CONTROL_BIT_VOLTAGE_33                  (BIT11|BIT10|BIT9)
    #define CONTROL_BIT_VOLTAGE_30                  (BIT11|BIT10)
    #define CONTROL_BIT_VOLTAGE_18                  (BIT11|BIT9)
    #define CONTROL_BIT_VOLTAGE_MASK                (BIT11|BIT10|BIT9)
    #define CONTROL_BIT_POWER						BIT8  // SD bus power
    #define CONTROL_BIT_SD_8_BIT_MODE				BIT5  // SD 8-bit mode
    #define CONTROL_BIT_HIGH_SPEED_ENABLE			BIT2  // high speed enable
    #define CONTROL_BIT_DATA_TRANSFER_WIDTH_4		BIT1  // data transfer width

#define	WAKEUP_BLOCKGAP_CONTROL_REGISTER	TCC7901_SDIO_CONTH_REGISTER

/*------------------- clock control register -------------------------*/
#define CLOCK_CONTROL_REGISTER				TCC7901_SDIO_CLK_REGISTER
    #define	CLOCK_BIT_CLKSELECT_DIV_256			0x80
    #define	CLOCK_BIT_CLKSELECT_DIV_128			0x40
    #define	CLOCK_BIT_CLKSELECT_DIV_64			0x20
    #define	CLOCK_BIT_CLKSELECT_DIV_32			0x10
    #define	CLOCK_BIT_CLKSELECT_DIV_16			0x08
    #define	CLOCK_BIT_CLKSELECT_DIV_8			0x04
    #define	CLOCK_BIT_CLKSELECT_DIV_4			0x02
    #define	CLOCK_BIT_CLKSELECT_DIV_2			0x01
    #define	CLOCK_BIT_CLKSELECT_DIV_0			0x00
    #define CLOCK_BIT_SD_CLOCK_ENABLE			BIT2
    #define CLOCK_BIT_INTERNAL_CLOCK_STABLE		BIT1
    #define CLOCK_BIT_INTERNAL_CLOCK_ENABLE		BIT0



/*------------------- software reset/timeout register -------------------------*/
#define SOFTWARE_RESET_TIMEOUT_REGISTER		TCC7901_SDIO_TIME_REGISTER
    #define TIME_BIT_SOFTWARE_RESET_DAT		BIT10	//reset for DAT line
    #define TIME_BIT_SOFTWARE_RESET_CMD		BIT9	//reset for CMD line
    #define TIME_BIT_SOFTWARE_RESET_ALL		BIT8	//reset for All
    #define TIME_BIT_TIMEOUT_VALUE_MASK     (BIT3|BIT2|BIT1|BIT0)
#define TIMEOUT_VALUE(X)       ( X &= ( BIT3|BIT2|BIT1|BIT0 ) )

#define	NORMAL_INTERRUPT_STATUS_LOW_REGISTER			TCC7901_SDIO_STSL_REGISTER

/*------------------- normal interrupt status register -------------------------*/
#define	NORMAL_INTERRUPT_STATUS_REGISTER	TCC7901_SDIO_STSL_REGISTER
    #define NIS_BIT_DAT_ENDBIT_ERROR		BIT22	// data end bit error
    #define NIS_BIT_DAT_CRC_ERROR			BIT21	// data crc error
    #define NIS_BIT_DAT_TIMEOUT_ERROR		BIT20	// data timeout error
    #define NIS_BIT_CMD_INDEX_ERROR			BIT19	// command index error
    #define NIS_BIT_CMD_ENDBIT_ERROR		BIT18	// command end bit error
    #define NIS_BIT_CMD_CRC_ERROR			BIT17	// command crc error
    #define NIS_BIT_CMD_TIMEOUT_ERROR		BIT16	// command timeout error
    #define NIS_BIT_ERR_INTERRUPT			BIT15	// error interrupt
    #define NIS_BIT_CARD_INTERRUPT			BIT8	// card interrupt
    #define NIS_BIT_CARD_REMOVAL			BIT7	// card removal
    #define NIS_BIT_CARD_INSERTION			BIT6	// card insertion
    #define NIS_BIT_BUFFER_READ_READY		BIT5	// buffer read ready
    #define NIS_BIT_BUFFER_WRITE_READY		BIT4	// buffer write ready
    #define NIS_BIT_DMA_INTERRUPT		    BIT3	// DMA interrupt
    #define	NIS_BIT_BLOCK_GAP_EVENT			BIT2	// block gap event
    #define NIS_BIT_TRANSFER_COMPLETE		BIT1	// transfer complete
    #define NIS_BIT_COMMAND_COMPLETE		BIT0	// command complete
    #define NIS_ERROR_INTERRUPT_BIT_MASK    0x3ff0000
#define CHECK_COMMAND_TIMEOUT   \
    (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_BIT_CMD_TIMEOUT_ERROR)
#define CHECK_DATA_TIMEOUT      \
    (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_BIT_DAT_TIMEOUT_ERROR)
#define CHECK_COMMAND_COMPLETE  \
    (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_BIT_COMMAND_COMPLETE)
#define CHECK_TRANSFER_COMPLETE  \
    (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_BIT_TRANSFER_COMPLETE)
#define ALL_INTERRUPT_ENABLE    0xffffffff


/*------------------- normal interrupt status enable register ----------------*/
#define NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER		TCC7901_SDIO_STSENL_REGISTER
    #define STAEN_BIT_ADMA_ERR_ENABLE			    BIT25	
    #define STAEN_BIT_ACMD12_ERR_ENABLE		        BIT24	
    #define STAEN_BIT_CURRENT_LIMIT_ERR_ENABLE  	BIT23	
    #define STAEN_BIT_DAT_ENDBIT_ERR_ENABLE	        BIT22	
    #define STAEN_BIT_DAT_CRC_ERR_ENABLE		    BIT21	
    #define STAEN_BIT_DAT_TIMEOUT_ERR_ENABLE	    BIT20	
    #define STAEN_BIT_CMD_INDEX_ERR_ENABLE 	        BIT19	
    #define STAEN_BIT_CMD_ENDBIT_ERR_ENABLE	        BIT18	
    #define STAEN_BIT_CMD_CRC_ERR_ENABLE		    BIT17	
    #define STAEN_BIT_CMD_TIMEOUT_ERR_ENABLE	    BIT16	
    #define STAEN_BIT_CARD_INTERRUPT_ENABLE		    BIT8	
    #define STAEN_BIT_CARD_REMOVAL_ENABLE		    BIT7	
    #define STAEN_BIT_CARDD_INSERTION_ENABLE		BIT6	
    #define STAEN_BIT_BUFFER_READ_READY_ENABLE	    BIT5	
    #define STAEN_BIT_BUFFER_WRITE_READY_ENABLE     BIT4	
    #define STAEN_BIT_DMA_INTERRUPT_ENABLE		    BIT3	
    #define STAEN_BIT_BLOCK_GAP_EVENT_ENABLE		BIT2	
    #define STAEN_BIT_TRANSFER_COMPLETE_ENABLE	    BIT1	
    #define STAEN_BIT_CMD_COMPLETE_ENABLE		    BIT0	

/*------------------- normal interrupt signal enable register ----------------*/
#define NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER		TCC7901_SDIO_INTENL_REGISTER
    #define SIGEN_BIT_ADMA_ERR_ENABLE			    BIT25	
    #define SIGEN_BIT_ACMD12_ERR_ENABLE		        BIT24	
    #define SIGEN_BIT_CURRENT_LIMIT_ERR_ENABLE  	BIT23	
    #define SIGEN_BIT_DAT_ENDBIT_ERR_ENABLE	        BIT22	
    #define SIGEN_BIT_DAT_CRC_ERR_ENABLE		    BIT21	
    #define SIGEN_BIT_DAT_TIMEOUT_ERR_ENABLE	    BIT20	
    #define SIGEN_BIT_CMD_INDEX_ERR_ENABLE 	        BIT19	
    #define SIGEN_BIT_CMD_ENDBIT_ERR_ENABLE	        BIT18	
    #define SIGEN_BIT_CMD_CRC_ERR_ENABLE		    BIT17	
    #define SIGEN_BIT_CMD_TIMEOUT_ERR_ENABLE	    BIT16	
    #define SIGEN_BIT_CARD_INTERRUPT_ENABLE		    BIT8	
    #define SIGEN_BIT_CARD_REMOVAL_ENABLE		    BIT7	
    #define SIGEN_BIT_CARDD_INSERTION_ENABLE		BIT6	
    #define SIGEN_BIT_BUFFER_READ_READY_ENABLE	    BIT5	
    #define SIGEN_BIT_BUFFER_WRITE_READY_ENABLE     BIT4	
    #define SIGEN_BIT_DMA_INTERRUPT_ENABLE		    BIT3	
    #define SIGEN_BIT_BLOCK_GAP_EVENT_ENABLE		BIT2	
    #define SIGEN_BIT_TRANSFER_COMPLETE_ENABLE	    BIT1	
    #define SIGEN_BIT_CMD_COMPLETE_ENABLE		    BIT0	

#define BLKSIZE_BLOCK_SIZE_BIT(X)       ( (X & 0x8fff) )
#define BLKSIZE_SDMA_BUF_BIT(X)         ( (X & 0x7000) )

#define TMODE_MULT_BIT(X)               ( (X & BIT0)<<5  )
#define TMODE_RW_BIT(X)                 ( (X & BIT0)<<4  )
#define TMODE_ACMD12_EN_BIT(X)          ( (X & BIT0)<<2  )
#define TMODE_BLKCOUNT_EN_BIT(X)        ( (X & BIT0)<<1  )
#define TMODE_DMA_EN_BIT(X)             ( (X & BIT0) )

#define HC_BUS_WIDTH_BIT(X)             ( ( (X & BIT0)<<1 ) )

#define HC_DMA_SEL_BIT(X)   \
( ( (0 == X) ? (X & BIT0) : ( (1 == X) ? ((X & BIT0)<<3) : ((X & BIT0)<<4) ) ) )

#define CMD_CMD_NUM_BIT(X)              ( ( (X & 0x3f)<<8 ) )
#define CMD_RESP_TYPE_BIT(X)            ( ( X & (BIT1|BIT0) ) )
#define CMD_DATSEL_BIT(X)               ( ( (X & BIT0)<<5 ) )

#define DATA_PRESENT_SELECT BIT5



#define AUTO_CMD12_ERROR_REGISTER						TCC7901_SDIO_CMD12ERR_REGISTER

#define CAPABILITIES_LOW_REGISTER						TCC7901_SDIO_CAPL_REGISTER

#define CAPABILITIES_HIGH_REGISTER						TCC7901_SDIO_CAPH_REGISTER

#define	MAXIMUM_CURRENT_CAPABILITIES_LOW_REGISTER		TCC7901_SDIO_CURL_REGISTER

#define	MAXIMUM_CURRENT_CAPABILITIES_HIGH_REGISTER		TCC7901_SDIO_CURH_REGISTER

#define FORCE_EVENT_FOR_AUTOCMD12_ERROR_REGISTER		TCC7901_SDIO_FORCEL_REGISTER

#define	FORCE_EVENT_FOR_ERROR_INTERRUPT_STATUS_REGISTER	TCC7901_SDIO_FORCEH_REGISTER

#define ADMA_ERROR_STATUS_REGISTER						TCC7901_SDIO_ADMAERR_REGISTER

#define ADMA_ADDRESS_15_0_REGISTER						TCC7901_SDIO_ADDR0_REGISTER

#define ADMA_ADDRESS_31_16_REGISTER						TCC7901_SDIO_ADDR1_REGISTER

#define ADMA_ADDRESS_47_32_REGISTER						TCC7901_SDIO_ADDR2_REGISTER

#define ADMA_ADDRESS_63_48_REGISTER						TCC7901_SDIO_ADDR3_REGISTER

#define	SLOT_INTERRUPT_STATUS_REGISTER					TCC7901_SDIO_SLOT_REGISTER

#define	HOST_CONTROLLER_VERSION_REGISTER				TCC7901_SDIO_VERSION_REGISTER

#define CMDLINE_TIMEOUT		1000
#define DATALINE_TIMEOUT	1000
#define CMD_INDEX(x)    ( (x<<8)&( BIT13|BIT12|BIT11|BIT10|BIT9|BIT8 ) )



#define MMC_NO_ERROR		0
#define MMC_ERR_TIMEOUT		-1
#define MMC_ERR_CRCRSP		-2
#define MMC_ERR_NODETECT	-3
#define MMC_ERR_WRPROTECT	-4
#define MMC_ERR_NOTRAN		-5

#define MMC_STATUS_IDLE		0x00000000
#define MMC_STATUS_READY	0x00000200
#define MMC_STATUS_STANDBY	0x00000600
#define MMC_STATUS_TRAN		0x00000800
#define MMC_STATUS_DATA		0x00000A00

typedef struct SCR_Register
{
	unsigned int sd_bus_widths          : 4;
	unsigned int sd_security            : 3;
	unsigned int data_stat_after_erase  : 1;
	unsigned int sd_spec                : 4;
	unsigned int scr_structure          : 4;
} tSCR_Register;

typedef struct mmc_hidden
{
	unsigned int	start;		// In Sector Address
	unsigned int	size;		// Sector Count
} tMMC_hidden;


#define	CMD_NW		BIT10	// CMD with no-waiting
#define	CMD_WD		BIT8		// CMD with DATA
#define	CMD_HC		BIT9		// CMD for HC

#define	BLOCKLEN	512


typedef enum BaseClkDivisor
{
	Divisor_256 = 0x8000,   //base clock divided by 256
	Divisor_128 = 0x4000,   //base clock divided by 128
	Divisor_64 = 0x2000,    //base clock divided by 64
	Divisor_32 = 0x1000,    //base clock divided by 32
	Divisor_16 = 0x0800,    //base clock divided by 16
	Divisor_8 = 0x0400,     //base clock divided by 8
	Divisor_4 = 0x0200,     //base clock divided by 4
	Divisor_2 = 0x0100,     //base clock divided by 2
	Divisor_1 = 0  	//base clock
} tBaseClkDivisor; 

typedef enum TimeoutValue
{
	TmclkMult2Exp27 = 14,
	TmclkMult2Exp26 = 13,
	TmclkMult2Exp25 = 12,
	TmclkMult2Exp24 = 11,
	TmclkMult2Exp23 = 10,
	TmclkMult2Exp22 = 9,
	TmclkMult2Exp21 = 8,
	TmclkMult2Exp20 = 7,
	TmclkMult2Exp19 = 6,
	TmclkMult2Exp18 = 5,
	TmclkMult2Exp17 = 4,
	TmclkMult2Exp16 = 3,
	TmclkMult2Exp15 = 2,
	TmclkMult2Exp14 = 1,
	TmclkMult2Exp13 = 0
} eTimeoutValue;

typedef enum ResponseType
{
	Response_Nothing	= 0,	//no response
	Response_r2			= 9,    //r2 :       01001b, Lenth_136
	Response_r3_r4		= 2,	//r3,r4 :    00010b, Lenth_48_NoCheck
	Response_r1_r5		= 0x1a,	//r1,r5 :    11010b, Lenth_48_Check
	Response_r1b_r5b_abrt=0xdb, //r1,r5 : 11011011b,  
	Response_r1b_r5b	= 0x1b, // :         11011b,Lenth_48_CheckBusy
	                              
	Response_r6_r7		= 0x1a	// 与r1,r5在一起
} eResponseType;
//Response_r1_r5_abrt
typedef volatile struct scmdStruct
{
    unsigned reserved1  : 2 ;
    unsigned cmdIndex   : 6 ;
    unsigned cType      : 2 ; /* 11 -- cmd12 52 for writing 'io abort'
                                 10 -- cmd52 for writing "function select"
                                 01 -- cmd52 for writing "bus suspend"
                                 00 -- all other commands
                              */
    unsigned datSel     : 1 ;
    unsigned cmdIndexChk: 1 ; // cmdIndex check enable
    unsigned crcChk     : 1 ; // crc check enable
    unsigned reserved2  : 1 ;
    unsigned rType      : 2 ; /* 00 -- no response,
                                 01 -- response length 136( r2 )
                                 10 -- response length 48(r1,3,4,5,6,7)
                                 11 -- response length 48 check busy
                                       (r1b, r5b)
                              */ 
    
    unsigned reserved3  : 8 ;
    unsigned spi        : 1 ; // 1 -- spi mode, 0 -- sd mode
    unsigned ataCmd     : 1 ;
    unsigned msBlock    : 1 ; // 1 -- multi block              
    unsigned direction  : 1 ; // 1 -- read
    unsigned reserved4  : 1 ;
    unsigned autoCmd12  : 1 ; // 1 -- autoCmd12 enable
    unsigned blockCntEn : 1 ;
    unsigned dmaEn      : 1 ;   
}tCMD_STRUCT;

typedef enum Multselect
{
    Single,
    Multiple
} eMultselect;

typedef enum BlockcountEnable
{
    BlockCountDisable,
    BlockCountEnable
} eBlockcountEnable;


typedef enum Acmd12Enable
{
    acmd12Disable,
    acmd12Enable
} eAcmd12Enable;

typedef enum DataRW
{
    DataWrite,
    DataRead
} eDataRW;

typedef enum PortSelect
{
    port0,
    port1,
    port2,
    port3,
    port4,
    port5,
    port6,
    port7
} ePortSelect;

typedef enum dmaOperationSelect
{
    SDMA,
    ADMA1,
    ADMA2
} eDmaOperationSelect;

typedef enum dmaEnable
{
    DmaDisable,
    DmaEnable
} eDmaEnable;

typedef enum BusWidth
{
    Bus_1_BitWideh,
    Bus_4_BitWideh
} eBusWidth;


typedef struct DataTransferParameter
{
	U32 commandNum;
	U32 BlockLength;
	enum dmaOperationSelect DmaOperationSelect;
	U32 Dma_BufferSize;
	U16 BlockCount;
	U32 Argument;
	enum Multselect MultSelect;
	enum DataRW Read_Select;
	enum Acmd12Enable ACMD12_Enable;
	enum BlockcountEnable BlockCount_Enable;
	enum dmaEnable DMA_Enable;
	enum BusWidth Bus_Width;
	enum ResponseType eResponse_Type;
} tDataTransferParameter;

#define	SDMMC_AREA_BOOTROM			(1 << 0)
#define	SDMMC_AREA_HIDDEN			(1 << 1)
#define	SDMMC_AREA_HIDDEN_FS		(1 << 2)
#define	SDMMC_AREA_REINIT			(1 << 7)

#define	SDMMC_STAT_INSERT			(1 << 0)
#define	SDMMC_STAT_CLOCK_ON			(1 << 1)
#define	SDMMC_STAT_TRANS			(1 << 2)
#define	SDMMC_STAT_WRITE_PROTECT	(1 << 3)

#define	SDMMC_PROP_VER20			(1 << 0)
#define	SDMMC_PROP_INTERNAL			(1 << 1)
#define	SDMMC_PROP_MUTATION_CARD	(1 << 2)

#define	SDMMC_CTRL_CHKTRAN_MASK		(0xF)
#define	SDMMC_CTRL_AUTOSTOP			(1 << 4)
#define	SDMMC_CTRL_DMA				(1 << 5)
#define	SDMMC_CTRL_BY_MSC			(1 << 6)


/*------------------------- 宏定义得到反馈数据 ----------------------------*/
#define hyhwSDIO_mGetResponse()      RESPONSE0_REGISTER      

#define hyhwSDIO_mEnCardIntSignal() \
(NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER |= SIGEN_BIT_CARD_INTERRUPT_ENABLE)

#define hyhwSDIO_mDisCardIntSignal() \
(NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER &= ~SIGEN_BIT_CARD_INTERRUPT_ENABLE)


#define hyhwSDIO_mDisCardIntStatus() \
(NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER &= ~SIGEN_BIT_CARD_INTERRUPT_ENABLE)
/***************************************************************************
* 函数:			hyhwSDIO_SendCommand

* 功能:        	执行发送Command的动作

* 参数:			cmdID  -- 发送的命令号
                commandArg -- 写入 Argument 寄存器的内容
                eResponseType -- 发送命令的反馈类型
                dataSelect -- 是否占用 data0 数据线发送命令的标志
                
* 返回:        	HY_OK -- 发送命令成功
                HY_ERROR  -- 发送命令失败
****************************************************************************/
U32 hyhwSDIO_SendCommand(U16 cmdID, U32 commandArg, eResponseType respType,
                        Bool dataSelect);

/***************************************************************************
* 函数:			hyhwSDIO_GetResponse

* 功能:        	得到反馈数据

* 参数:			无
                
* 返回:        	反馈数据
****************************************************************************/
U32 hyhwSDIO_GetResponse(void);

/***************************************************************************
* 函数:			hyhwSDIO_SoftwareReset

* 功能:        	Software Reset SD Controller

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_SoftwareReset(void);

/***************************************************************************
* 函数:			hyhwSDIO_SetBusasSdFunction

* 功能:        	将总线设置成SD模式

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_SetBusasSdFunction(void);

/***************************************************************************
* 函数:			hyhwSDIO_Initialize

* 功能:        	初始化 host 端控制器

* 参数:			freq -- SD controller 时钟频率
                timeout -- 设置超时时间，设置范围为 0~14
                
                14 - TMCLK * 2^27
                ------------------
                ------------------
                1 - TMCLK * 2^14
                0 - TMCLK * 2^13
                
* 返回:        	无
****************************************************************************/  
void hyhwSDIO_Initialize(tBaseClkDivisor freq, eTimeoutValue timeout);

/***************************************************************************
* 函数:			hyhwSDIO_WorkingFrequenceSet

* 功能:        	设置SDIO卡工作频率

* 参数:			无
                               
* 返回:        	无
                
****************************************************************************/
void hyhwSDIO_WorkingFrequenceSet(tBaseClkDivisor freq);

/***************************************************************************
* 函数:			hyhwSDIO_ErrorInterruptDetect

* 功能:        	检测是否有错误中断产生

* 参数:			无
                               
* 返回:        	HY_OK -- 不产生错误中断
                HY_ERROR -- 产生错误中断
****************************************************************************/
U32 hyhwSDIO_ErrorInterruptDetect( void );

/***************************************************************************
* 函数:			hyhwSDIO_DatCmdLineFreeDetect

* 功能:        	检测command line 与 data line 是否都为free

* 参数:			无
                               
* 返回:        	HY_OK -- command line 与 data line都为free
                HY_ERROR -- command line 与 data line至少一根线不为free
****************************************************************************/
U32 hyhwSDIO_DatCmdLineFreeDetect( void );

/***************************************************************************
* 函数:			hyhwSDIO_CmdCompleteDetect

* 功能:        	检测command 是否传输完成

* 参数:			无
                               
* 返回:        	HY_OK -- 有command 传输完成，并且已经收到response
                HY_ERROR -- 没有command 传输完成。
****************************************************************************/  
U32 hyhwSDIO_CmdCompleteDetect( void );

/***************************************************************************
* 函数:			hyhwSDIO_DatCompeletDetect

* 功能:        	检测data 是否传输完成

* 参数:			无
                               
* 返回:        	HY_OK -- 有data 传输完成
                HY_ERROR -- 没有data 传输完成。
****************************************************************************/ 
U32 hyhwSDIO_DatCompeletDetect( void );

/***************************************************************************
* 函数:			hyhwSDIO_ErrorCheck

* 功能:        	检测在command传输过程中是否产生错误

* 参数:			无
                               
* 返回:        	HY_OK -- 在command传输过程中没有产生错误
                HY_ERROR -- 在command传输过程中有错误
****************************************************************************/
U32 hyhwSDIO_ErrorCheck( void );

/***************************************************************************
* 函数:			hyhwSDIO_DataRead

* 功能:        	从SDIO controller的 buffer中读数据

* 参数:			dataLength  -- 准备读数据的长度
                pReadBuffer -- 存放数据的指针

* 返回:        	无
****************************************************************************/
void hyhwSDIO_DataRead(U32 dataLength, U8 *pReadBuffer);

/***************************************************************************
* 函数:			hyhwSDIO_DataWrite

* 功能:        	向SDIO controller的 buffer中写数据

* 参数:			dataLength  -- 准备写数据的长度
                pReadBuffer -- 存放数据的指针

* 返回:        	无
****************************************************************************/
void hyhwSDIO_DataWrite(U32 dataLength, U8 *pWriteBuffer);

/***************************************************************************
* 函数:			hyhwSDIO_DatCmdLineReset

* 功能:        	reset data/cmd line

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_DatCmdLineReset( void );

/***************************************************************************
* 函数:			hyhwSDIO_HwIntDetect

* 功能:        	检测是否有SDIO中断信号产生

* 参数:			无
                
* 返回:        	0 -- 没有中断信号产生
                1 -- 有中断信号产生
****************************************************************************/
U8 hyhwSDIO_HwIntDetect( void );

#endif