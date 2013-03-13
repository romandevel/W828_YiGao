#include "hyErrors.h"

#include "hyhwTcc7901.h"
#include "hyhwSDIO.h"
#include "hyhwGpio.h"

#include "wmi.h"
#include "wifi_user.h"


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


/*-------- 宏定义向SD Controller 的 buffer 里面写 unsigned int 型数据 ------*/
#define mWriteDataToBuffer(X)       (BUFFER_DATA_PORT_REGISTER = X)

/*-------- 宏定义从SD Controller 的 buffer 里面读 unsigned int 型数据 ------*/
#define mReadDataFromBuffer(X)      (X = BUFFER_DATA_PORT_REGISTER)


/*-------- 宏定义向SD Controller 的 buffer 里面写 unsigned char 型数据 ------*/
#define mWriteReg8DataToBuffer(X)       (BUFFER_DATA_PORT_REGISTER_REG8 = X)

/*-------- 宏定义从SD Controller 的 buffer 里面读 unsigned char 型数据 ------*/
#define mReadReg8DataFromBuffer(X)      (X = BUFFER_DATA_PORT_REGISTER_REG8)



/*----------------- 宏定义等待 SD Controller 的 buffer 可写 ---------------*/
#define mBufferWrEnableWaiting(timeOut)  \
    while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_WR_ENABLE)  && (timeOut--))
    

/*----------------- 宏定义等待 SD Controller 的 buffer 可读 ---------------*/   
#define mBufferRdEnableWaiting(timeOut)  \
    while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_RD_ENABLE) && (timeOut--))


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
                        Bool dataSelect)
{
	S32 timeout ;
	U32 intStatusReg;
    
    timeout = 100000;
	while( (0 != (PRESENT_STATE_REGISTER & STATE_BIT_CMDLINE_INUSE) )&&(--timeout) );
	if(0 == timeout)
	{
	    return HY_ERROR;
	}
	if(dataSelect)
	{
	    timeout = 100000;
	    while( (0 != (PRESENT_STATE_REGISTER & STATE_BIT_DATALINE_ACT)) && (--timeout) );
		if(0 == timeout)
		{
		    return HY_ERROR;
		}
	}
	ARGUMENT_REGISTER = commandArg;
	COMMAND_REGISTER  = CMD_INDEX(cmdID)|respType;

	#if 1
	//while( (0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) );
	timeout = 100000;
	while( (0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) && (--timeout));
	#else
	while(1)
	{
		intStatusReg = NORMAL_INTERRUPT_STATUS_REGISTER;
		if ((intStatusReg&NIS_BIT_COMMAND_COMPLETE) != 0 ||
			(intStatusReg&NIS_BIT_CMD_TIMEOUT_ERROR) != 0)
		{
			break;
		}
	}
	#endif

	if( 0 != CHECK_COMMAND_COMPLETE )
	{
	    NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_COMMAND_COMPLETE;
	}
	else
	{
	    return HY_ERROR;
	}
    return HY_OK;
}

/***************************************************************************
* 函数:			hyhwSDIO_SoftwareReset

* 功能:        	Software Reset SD Controller

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_SoftwareReset(void)
{
    int i;
    SOFTWARE_RESET_TIMEOUT_REGISTER |= TIME_BIT_SOFTWARE_RESET_ALL;
    i = 700;    //肯定大于驱动里提出的 2.67*5 us
    while(i--);
    while(SOFTWARE_RESET_TIMEOUT_REGISTER & TIME_BIT_SOFTWARE_RESET_ALL);
    return;
}

/***************************************************************************
* 函数:			hyhwSDIO_SetBusasSdFunction

* 功能:        	将总线设置成SD模式

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_SetBusasSdFunction(void)
{
    //hyhwGpio_setSD2Function();
    hyhwGpio_setSD7Function();
}
/***************************************************************************
* 函数:			hyhwSDIO_sdPortSelect

* 功能:        	选择SLOT0、SLOT1对应的SDIO口

* 参数:			slot0、slot0
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_sdPortSelect( ePortSelect slot0, ePortSelect slot1 )
{
	SD_PORT_CTRL_REGSITER = (SLOT0_PORT_SELECT(slot0)|SLOT1_PORT_SELECT(slot1));

}
/***************************************************************************
* 函数:			ClockFrequencySelect

* 功能:        	SD时钟频率选择

* 参数:			枚举型分频的除数 基于MCLK分频
                               
* 返回:        	无
****************************************************************************/
static void ClockFrequencySelect(tBaseClkDivisor freq)
{
    int i;
    
    CLOCK_CONTROL_REGISTER &= ~CLOCK_BIT_SD_CLOCK_ENABLE;
    CLOCK_CONTROL_REGISTER |= (freq | CLOCK_BIT_INTERNAL_CLOCK_ENABLE);
    i = 10000;
    while(i--);
    while( 0 == (CLOCK_CONTROL_REGISTER & CLOCK_BIT_INTERNAL_CLOCK_STABLE) );
    CLOCK_CONTROL_REGISTER |= CLOCK_BIT_SD_CLOCK_ENABLE;
}

void hyhwSDIO_ClockDisable(void)
{
	CLOCK_CONTROL_REGISTER &= ~CLOCK_BIT_SD_CLOCK_ENABLE;
}

/***************************************************************************
* 函数:			WorkingVoltageSet

* 功能:        	工作电压设置 已经设置成 3.3V

* 参数:			无
                               
* 返回:        	无
****************************************************************************/
static void WorkingVoltageSet()
{
    POWER_HOST_CONTROL_REGISTER &= ~CONTROL_BIT_VOLTAGE_MASK;
    POWER_HOST_CONTROL_REGISTER |= CONTROL_BIT_VOLTAGE_30;
}

/***************************************************************************
* 函数:			 DataTimeoutValueSet

* 功能:        	 设置最大超时时间

* 参数:			 timeout -- 最大超时时间。范围为 0~14
				 
				 14 - TMCLK * 2^27
				 ------------------
				 ------------------
				 1 - TMCLK * 2^14
                 0 - TMCLK * 2^13
                                
* 返回:        	无
****************************************************************************/
static void DataTimeoutValueSet(eTimeoutValue timeout)
{
    SOFTWARE_RESET_TIMEOUT_REGISTER &= ~TIME_BIT_TIMEOUT_VALUE_MASK;
    SOFTWARE_RESET_TIMEOUT_REGISTER |= TIMEOUT_VALUE(timeout);
}

/***************************************************************************
* 函数:			InterruptStateEnable

* 功能:        	使能 normal interrupt 

* 参数:			无
                               
* 返回:        	无
****************************************************************************/
static void InterruptStateEnable()
{
    NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER = ALL_INTERRUPT_ENABLE;
    NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0;
}

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
void hyhwSDIO_Initialize(tBaseClkDivisor freq, eTimeoutValue timeout)
{
    hyhwSDIO_SoftwareReset();
    ClockFrequencySelect(freq);
    WorkingVoltageSet();
    DataTimeoutValueSet(timeout);
    InterruptStateEnable();
}

/***************************************************************************
* 函数:			hyhwSDIO_WorkingFrequenceSet

* 功能:        	设置SDIO卡工作频率

* 参数:			无
                               
* 返回:        	无
                
****************************************************************************/
void hyhwSDIO_WorkingFrequenceSet(tBaseClkDivisor freq)
{
    U16 temp;
    temp = CLOCK_CONTROL_REGISTER;
    CLOCK_CONTROL_REGISTER = (temp &0xff)|(freq);
}

/***************************************************************************
* 函数:			hyhwSDIO_ErrorInterruptDetect

* 功能:        	检测是否有错误中断产生

* 参数:			无
                               
* 返回:        	HY_OK -- 不产生错误中断
                HY_ERROR -- 产生错误中断
****************************************************************************/
U32 hyhwSDIO_ErrorInterruptDetect( void )
{
    U32 testBit;
    testBit = (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_ERROR_INTERRUPT_BIT_MASK);
    if( 0 != testBit)
    {
        return HY_ERROR;
    }
    else
    {
        return HY_OK;
    }
}

/***************************************************************************
* 函数:			hyhwSDIO_DatCmdLineFreeDetect

* 功能:        	检测command line 与 data line 是否都为free

* 参数:			无
                               
* 返回:        	HY_OK -- command line 与 data line都为free
                HY_ERROR -- command line 与 data line至少一根线不为free
****************************************************************************/
U32 hyhwSDIO_DatCmdLineFreeDetect()
{

	U32 i=1000;
	while(DAT_CMD_LINE_ACT)
	{
		i --;
		if(i == 0)
		{
			return HY_ERROR;
		}
	
	}
	return HY_OK;		
}

/***************************************************************************
* 函数:			hyhwSDIO_CmdCompleteDetect

* 功能:        	检测command 是否传输完成

* 参数:			无
                               
* 返回:        	HY_OK -- 有command 传输完成，并且已经收到response
                HY_ERROR -- 没有command 传输完成。
****************************************************************************/  
U32 hyhwSDIO_CmdCompleteDetect( void )
{
	//while((0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT));
	U32 timeOut = 100000;
	while((0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) && (timeOut--));
	
	if( 0 != CHECK_COMMAND_COMPLETE )
	{
	    NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_COMMAND_COMPLETE;
	}
	else
	{
	    return HY_ERROR;
	}
	return HY_OK;   
}

/***************************************************************************
* 函数:			hyhwSDIO_DatCompeletDetect

* 功能:        	检测data 是否传输完成

* 参数:			无
                               
* 返回:        	HY_OK -- 有data 传输完成
                HY_ERROR -- 没有data 传输完成。
****************************************************************************/ 
U32 hyhwSDIO_DatCompeletDetect( void )
{
    //while( ( 0 == CHECK_TRANSFER_COMPLETE )&& ( 0 == CHECK_DATA_TIMEOUT ) );
    U32 timeOut=100000;
    while(1)
    {
    	U32 status;
    	status = NORMAL_INTERRUPT_STATUS_REGISTER;
    	if (status&(NIS_BIT_TRANSFER_COMPLETE|NIS_BIT_DAT_TIMEOUT_ERROR))
    	{
    		break;
    	}
    	timeOut--;
    	if (timeOut == 0)
    	{
    		return HY_ERROR;
    	}
    }
   
    if( 0 != CHECK_TRANSFER_COMPLETE )
    {
        NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_TRANSFER_COMPLETE;
    }
    else
    {
        return HY_ERROR;
    }
    return HY_OK;
}

/***************************************************************************
* 函数:			hyhwSDIO_ErrorCheck

* 功能:        	检测在command传输过程中是否产生错误

* 参数:			无
                               
* 返回:        	HY_OK -- 在command传输过程中没有产生错误
                HY_ERROR -- 在command传输过程中有错误
****************************************************************************/
U32 hyhwSDIO_ErrorCheck( void )
{
    if( 0 != (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_ERROR_INTERRUPT_BIT_MASK) )
    {
        return HY_ERROR;
    }
    return HY_OK;
}


/***************************************************************************
* 函数:			DataTransferInit

* 功能:        	将传输命令的参数送给 host controller register

* 参数:			pb -- 传递参数存放的结构指针

* 返回:        	无
****************************************************************************/
void hyhwSDIO_DataTransferInit(tDataTransferParameter *pb)
{
	BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(pb->BlockLength) |
	                        BLKSIZE_SDMA_BUF_BIT(pb->Dma_BufferSize) );
	BLOCK_COUNT_REGISTER   = pb->BlockCount;
	ARGUMENT_REGISTER	   = pb->Argument;

	TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(pb->MultSelect) |
	                           TMODE_RW_BIT(pb->Read_Select) |
	                           TMODE_ACMD12_EN_BIT(pb->ACMD12_Enable) |
	                           TMODE_BLKCOUNT_EN_BIT(pb->BlockCount_Enable) |
	                           TMODE_DMA_EN_BIT(pb->DMA_Enable) );

	POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(pb->Bus_Width) |
	                                 HC_DMA_SEL_BIT(pb->DmaOperationSelect) );                          
	                              
    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(pb->commandNum) |
                               CMD_RESP_TYPE_BIT(pb->eResponse_Type) |
                               DATA_PRESENT_SELECT );
}

/***************************************************************************
* 函数:			hyhwSDIO_DataRead

* 功能:        	从SDIO controller的 buffer中读出数据

* 参数:			dataLength  -- 准备读数据的长度
                pReadBuffer -- 存放数据的指针

* 返回:        	无
****************************************************************************/
void hyhwSDIO_DataRead(U32 dataLength, U8 *pReadBuffer)
{
    U32 lengthPending, i;
    U32 *pRDBuf;
    if(1 == dataLength)
    {
        lengthPending = dataLength;
    }
    else
    {
        lengthPending = dataLength >> 2;
    }
    if(1 != dataLength)
    {
        pRDBuf = (U32 *)pReadBuffer;
    }
    
    for(i = 0; i < lengthPending; i++)
    {
        U32 j= 10;
        while(j--);
        j = 100000;
        mBufferRdEnableWaiting(j);
 
        if(1 == dataLength)
        {    
            mReadReg8DataFromBuffer( *pReadBuffer );
        }
        else
        {
            mReadDataFromBuffer( *pRDBuf++ );
        }
    }
}

/***************************************************************************
* 函数:			hyhwSDIO_DataWrite

* 功能:        	向SDIO controller的 buffer中写入数据

* 参数:			dataLength  -- 准备写数据的长度
                pReadBuffer -- 存放数据的指针

* 返回:        	无
****************************************************************************/
void hyhwSDIO_DataWrite(U32 dataLength, U8 *pWriteBuffer)
{
    U32 lengthPending, i;
    U32 *pWRBuf;
    if(1 == dataLength)
    {
        lengthPending = dataLength;
    }
    else
    {
        lengthPending = dataLength >> 2;
    }
    if(1 != dataLength)
    {
        pWRBuf = (U32 *)pWriteBuffer;
    }
    
    for(i = 0; i < lengthPending; i++)
    {
        U32 j= 10;
        while(j--);
        j = 100000;
        mBufferWrEnableWaiting(j);
        
        if(1 == dataLength)
        {    
            mWriteReg8DataToBuffer( *pWriteBuffer );
        }
        else
        {
            mWriteDataToBuffer( *pWRBuf++ );
        }
    }
}

/***************************************************************************
* 函数:			hyhwSDIO_DatCmdLineReset

* 功能:        	reset data/cmd line

* 参数:			无
                
* 返回:        	无
****************************************************************************/
void hyhwSDIO_DatCmdLineReset( void )
{
    TCC7901_SDIO_TIME_REGISTER |= TIME_BIT_SOFTWARE_RESET_CMD;
    TCC7901_SDIO_TIME_REGISTER |= TIME_BIT_SOFTWARE_RESET_DAT;
    while(TCC7901_SDIO_TIME_REGISTER&TIME_BIT_SOFTWARE_RESET_CMD);
    while(TCC7901_SDIO_TIME_REGISTER&TIME_BIT_SOFTWARE_RESET_DAT);
}

/***************************************************************************
* 函数:			hyhwSDIO_HwIntDetect

* 功能:        	检测是否有SDIO中断信号产生

* 参数:			无
                
* 返回:        	0 -- 没有中断信号产生
                1 -- 有中断信号产生
****************************************************************************/
U8 hyhwSDIO_HwIntDetect( void )
{
    if(NORMAL_INTERRUPT_STATUS_LOW_REGISTER&NIS_BIT_CARD_INTERRUPT)
    {
        NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER &= ~SIGEN_BIT_CARD_INTERRUPT_ENABLE;
        adapter.intReq = MV_INT_STATUS_INT_REQ;
        return 1;
    }
    else
    {
        return 0;
    }
}

void hyhwSDIO_mEnCardIntStatus( void ) 
{
	//if((adapter.intReq == MV_INT_STATUS_INT_REQ))//||(adapter.lastWorkType == workType_voip))
	{
    	NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER |= SIGEN_BIT_CARD_INTERRUPT_ENABLE;
    	//adapter.intReq = MV_INT_STATUS_NONE;
    }
}

