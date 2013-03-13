/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/
/******************************************************************************
* 文件: hyNandflash.c (hyco Nandflash hardware Layer)
* 创建: ZhouJie 
* 创建时间: 2005 12 05 15:52
* 说明: the hardware Layer of Nandflash device 
*		由于硬件的限制关系，在对某些型号的nandflash发出CE命令时，必须
*		不能对SDRAM进行读写，这样，这些函数就必须放在sram中运行
*		在不同的时候，load到sram中不同的函数
*		1。 nand_init段： init 函数，overlay
*		2。 nand_sramAlways段：read 相关函数，常驻内存
*		3。 nand_write 段：write 相关函数，动态load进sram 中
*		4。 text 段（其他）：	和硬件、执行时间无关的函数
*
*		由于是复杂的overlay的机制，为了避免veneer产生错误，这个程序中要保证不再
*		调用别的函数，处理如下
*		1。 对 sdram standby和wakeup的函数调用，变为宏
*		2。 对 gpio的设置变为宏
*		3。 对 interrupt 的resume 和pause 的调用，变为宏
*
*		另外，在sdram standy期间，注意，不能对存储在sdram的数据进行操作，注意将
*		nandflash的硬件setting的结构的数据，例如bank_no, ale，cle,smc 等要先读出
*		到临时变量中（sram中）。	
*
*		20060918 这个代码中不能使用dma传输数据，由于dma要求使用物理地址0x1000.0000，而系统
*		给出的地址是0x0000.0000开头,两段内存性质不同，一起使用会有冲突，将dma部分代码去掉	
*****************************************************************************/

/*****************************************************************************
  修改记录:Yang Jian 2009.08.24
  增加了K9LBG08的操作
  增加了K9GAG08的操作
*******************************************************************************/


/*************************************************************************
 * INCLUDES    
 *************************************************************************/



#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"
#include "hyhwEcc.h"
#include "hyhwNandFlash.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"

//#define MC_NAND
/*************************************************************************
 * DEFINES
 *************************************************************************/
#define FLASH_BLOCK_SIZE_128		(128*1024)
#define FLASH_SECTOR_SIZE_128		(2048)
#define FLASH_SECTORS_PER_BLOCK_128	(FLASH_BLOCK_SIZE_128/FLASH_SECTOR_SIZE_128)
#define FLASH_SPAREARER_SIZE_128	(64)
#define FLASH_TOTAL_PAGE_SIZE_128	(FLASH_SECTOR_SIZE_128+FLASH_SPAREARER_SIZE_128)

#define FLASH_BLOCK_SIZE_256		(256*1024)
#define FLASH_SECTOR_SIZE_256		(2048)
#define FLASH_SECTORS_PER_BLOCK_256	(FLASH_BLOCK_SIZE_256/FLASH_SECTOR_SIZE_256)
#define FLASH_SPAREARER_SIZE_256	(64)
#define FLASH_TOTAL_PAGE_SIZE_256	(FLASH_SECTOR_SIZE_256+FLASH_SPAREARER_SIZE_256)


#define FLASH_BLOCK_SIZE_32			(32*1024)
#define FLASH_SECTOR_SIZE_32    	(512)
#define FLASH_SECTORS_PER_BLOCK_32	(FLASH_BLOCK_SIZE_32/FLASH_SECTOR_SIZE_32)

#define FLASH_BLOCK_SIZE_16			(16*1024)
#define FLASH_SECTOR_SIZE_16		(512)
#define FLASH_SECTORS_PER_BLOCK_16  (FLASH_BLOCK_SIZE_16/FLASH_SECTOR_SIZE_16)
#define FLASH_SPAREARER_SIZE_16		(16)
#define FLASH_TOTAL_PAGE_SIZE_16	(FLASH_SECTOR_SIZE_16+FLASH_SPAREARER_SIZE_16)

#define FLASH_BLOCK_SIZE_512		(512*1024)
#define FLASH_SECTOR_SIZE_512    	(2048)
#define FLASH_SECTORS_PER_BLOCK_512	(FLASH_BLOCK_SIZE_512/FLASH_SECTOR_SIZE_512)
#define FLASH_SPAREARER_SIZE_512	(64)
#define FLASH_TOTAL_PAGE_SIZE_512	(FLASH_SECTOR_SIZE_512+FLASH_SPAREARER_SIZE_512)

#define FLASH_BLOCK_SIZE_256_MLC 	(256*1024)						//number of block per device 
#define FLASH_SECTOR_SIZE_256_MLC	(2048)							//number ofpage  per block 
#define FLASH_SECTORS_PER_BLOCK_256_MLC (FLASH_BLOCK_SIZE_256_MLC/FLASH_SECTOR_SIZE_256_MLC)

/*NFC*/
#define NFC_CMD_REGISTER				TCC7901_NFC_CMD_REGISTER
#define NFC_LINEAR_ADDR_REGISTER		TCC7901_NFC_LADDR_REGISTER
#define NFC_BLOCK_ADDR_REGISTER			TCC7901_NFC_BADDR_REGISTER
#define NFC_SINGLE_ADDR_CYCLE_REGISTER	TCC7901_NFC_SADDR_REGISTER
#define NFC_WORD_DATA_REGISTER			TCC7901_NFC_WDATA_REGISTER
#define NFC_LINEAR_DATA_REGISTER		TCC7901_NFC_LDATA_REGISTER
#define NFC_SINGLE_DATA_REGISTER		TCC7901_NFC_SDATA_REGISTER
#define NFC_MULTIPLE_DATA_REGISTER		TCC7901_NFC_MDATA_REGISTER
#define NFC_CTRL_REGISTER				TCC7901_NFC_CTRL_REGISTER
	#define ND_BIT_READY_INTERRUPT_ENABLE	BIT31	
	#define ND_BIT_PROG_INTERRUPT_ENABLE	BIT30	
	#define ND_BIT_READ_INTERRUPT_ENABLE	BIT29	
	#define ND_BIT_DMA_REQUEST				BIT28	
	#define ND_BIT_FIFO_STATUS				BIT27	
	#define ND_BIT_BUS_WIDTH_SELECT_16BIT	BIT26	
	#define ND_BIT_BUS_WIDTH_SELECT_8BIT	~BIT26	
	#define ND_BIT_CS3_SELECT_DISABLE		BIT25	
	#define ND_BIT_CS2_SELECT_DISABLE		BIT24
	#define ND_BIT_CS1_SELECT_DISABLE		BIT23
	#define ND_BIT_CS0_SELECT_DISABLE		BIT22
	#define ND_BIT_READY_FLAG				BIT21		/*read-only register,1--EXTERNAL NAND FLASH IS READY   0--EXTERNAL NAND FLASH IS BUSY*/
	#define ND_BIT_BURST_SIZE_OF_NAND_CTRL_MASK		~(BIT20|BIT19)   
	#define BURST_SIZE_OF_NAND_CTRL_SHIFT			19  
	#define ND_BIT_BURST_SIZE_OF_NAND_CTRL_1RW		0<<BURST_SIZE_OF_NAND_CTRL_SHIFT
	#define ND_BIT_BURST_SIZE_OF_NAND_CTRL_2RW		1<<BURST_SIZE_OF_NAND_CTRL_SHIFT
	#define ND_BIT_BURST_SIZE_OF_NAND_CTRL_4RW		2<<BURST_SIZE_OF_NAND_CTRL_SHIFT
	#define ND_BIT_BURST_SIZE_OF_NAND_CTRL_8RW		3<<BURST_SIZE_OF_NAND_CTRL_SHIFT
	#define ND_BIT_PAGE_SIZE_MASK					~(BIT18|BIT17|BIT16)
	#define PAGE_SIZE_SHIFT							16  
	#define ND_BIT_PAGE_SIZE_256_16					0<<PAGE_SIZE_SHIFT
	#define ND_BIT_PAGE_SIZE_512_8					1<<PAGE_SIZE_SHIFT
	#define ND_BIT_PAGE_SIZE_1024_16				2<<PAGE_SIZE_SHIFT
	#define ND_BIT_PAGE_SIZE_2048_8					3<<PAGE_SIZE_SHIFT
	#define ND_BIT_PAGE_SIZE_4096_8					4<<PAGE_SIZE_SHIFT
	#define ND_BIT_IO_MASK_ENABLE					BIT15			
	#define ND_BIT_ADDR_CYCLE_NUM_MASK				~(BIT14|BIT13|BIT12)	/* NAND FLASH ADDRESS SYCLE	(N+1) */
	#define ADDR_CYCLE_NUM_SHIFT					12  
	#define ND_BIT_ADDR_CYCLE_NUM(X)				((X-1)<<ADDR_CYCLE_NUM_SHIFT)
	#define ND_BIT_CYCLE_FOR_SETUP_TIME_MASK		~(BIT11|BIT10|BIT9|BIT8)	
	#define CYCLE_FOR_SETUP_TIME_SHIFT				8  
	#define ND_BIT_CYCLE_FOR_SETUP_TIME(X)			(X<<CYCLE_FOR_SETUP_TIME_SHIFT)
	#define ND_BIT_CYCLE_FOR_PULSE_WIDTH_MASK		~(BIT7|BIT6|BIT5|BIT4)	
	#define CYCLE_FOR_PULSE_WIDTH_SHIFT				4  
	#define ND_BIT_CYCLE_FOR_PULSE_WIDTH(X)			(X<<CYCLE_FOR_PULSE_WIDTH_SHIFT)
	#define ND_BIT_CYCLE_FOR_HOLD_TIME_MASK			~(BIT3|BIT2|BIT1|BIT0)	
	#define CYCLE_FOR_HOLD_TIME_SHIFT				0 
	#define ND_BIT_CYCLE_FOR_HOLD_TIME(X)			(X<<CYCLE_FOR_HOLD_TIME_SHIFT)
#define NFC_PROG_START_REGISTER			TCC7901_NFC_PSTART_REGISTER
#define NFC_READ_START_REGISTER			TCC7901_NFC_RSTART_REGISTER
#define NFC_DATA_SIZE_REGISTER			TCC7901_NFC_DSIZE_REGISTER
#define NFC_INT_REQ_REGISTER			TCC7901_NFC_IREQ_REGISTER
	#define ND_BIT_READY_OCCURE_FLAG			BIT6
	#define ND_BIT_PROGRAM_FINISH_FLAG			BIT5
	#define ND_BIT_READ_TRANSFER_FINISH_FLAG	BIT4
	#define ND_BIT_READY_INT_OCCURE_FLAG		BIT2
	#define ND_BIT_PROGRAM_INT_FINISH_FLAG		BIT1
	#define ND_BIT_READ_INT_REQUEST_FLAG		BIT0
	#define ND_BIT_INT_REQUEST_CLEAR			(BIT6|BIT5|BIT4|BIT2|BIT1|BIT0)
#define NFC_CTRL_RST_REGISTER			TCC7901_NFC_RST_REGISTER
#define NFC_CTRL_REGISTER_1				TCC7901_NFC_CTRL1_REGISTER
	#define ND_BIT_DMA_ACK_ENABLE				BIT31
	#define ND_BIT_IDLE_STATE_FLAG				BIT26
	#define ND_BIT_MDATA_ACCESS_NUM_MASK		~(BIT25|BIT24)	/*used for reading and writing multiple 8bit or 16bit data according to NAND
																	Flash Bus Size.*/
	#define MDATA_ACCESS_NUM_SHIFT				24
	#define ND_BIT_MDATA_ACCESS_NUM(X)			(X<<MDATA_ACCESS_NUM_SHIFT)
	#define ND_BIT_SETUP_TIME_OF_READ_CYCLE_MASK	~(BIT23|BIT22|BIT21|BIT20)	
	#define SETUP_TIME_OF_READ_CYCLE_SHIFT			20
	#define ND_BIT_SETUP_TIME_OF_READ_CYCLE(X)		(X<<SETUP_TIME_OF_READ_CYCLE_SHIFT)
	#define ND_BIT_PULSE_WIDTH_OF_READ_CYCLE_MASK	~(BIT19|BIT18|BIT17|BIT16)	
	#define PULSE_WIDTH_OF_READ_CYCLE_SHIFT			16
	#define ND_BIT_PULSE_WIDTH_OF_READ_CYCLE(X)		(X<<PULSE_WIDTH_OF_READ_CYCLE_SHIFT) 
	#define ND_BIT_HOLD_TIME_OF_READ_CYCLE_MASK		~(BIT15|BIT14|BIT13|BIT12)	
	#define HOLD_TIME_OF_READ_CYCLE_SHIFT			12
	#define ND_BIT_HOLD_TIME_OF_READ_CYCLE(X)		(X<<HOLD_TIME_OF_READ_CYCLE_SHIFT) 
	#define ND_BIT_SETUP_TIME_OF_WRITE_CYCLE_MASK	~(BIT11|BIT10|BIT9|BIT8)	
	#define SETUP_TIME_OF_WRITE_CYCLE_SHIFT			8
	#define ND_BIT_SETUP_TIME_OF_WRITE_CYCLE(X)		(X<<SETUP_TIME_OF_WRITE_CYCLE_SHIFT)
	#define ND_BIT_PULSE_WIDTH_OF_WRITE_CYCLE_MASK	~(BIT7|BIT6|BIT5|BIT4)	
	#define PULSE_WIDTH_OF_WRITE_CYCLE_SHIFT		4
	#define ND_BIT_PULSE_WIDTH_OF_WRITE_CYCLE(X)	(X<<PULSE_WIDTH_OF_WRITE_CYCLE_SHIFT) 
	#define ND_BIT_HOLD_TIME_OF_WRITE_CYCLE_MASK	~(BIT3|BIT2|BIT1|BIT0)	
	#define HOLD_TIME_OF_WRITE_CYCLE_SHIFT			0
	#define ND_BIT_HOLD_TIME_OF_WRITE_CYCLE(X)		(X<<HOLD_TIME_OF_WRITE_CYCLE_SHIFT) 
	


#define ND_CS0_ENABLE		NFC_CTRL_REGISTER &= (~ND_BIT_CS0_SELECT_DISABLE)		/*NAND 0 SET high*/
#define ND_CS0_DISABLE		NFC_CTRL_REGISTER |= (ND_BIT_CS0_SELECT_DISABLE)		/*NAND 0 SET high*/

#define MLC_ECC4_ERROR_NUMBER_REGISTER			TCC7901_ECC_ERRNUM_REGISTER

//////////////////////
#define ECC_CONTROL_REGISTER					TCC7901_ECC_CTRL_REGISTER
#define ECC_DISABLE								ECC_CONTROL_REGISTER	&=	~(BIT0|BIT1|BIT2)
#define MLC_ECC4_CODE_REGISTER_0 				TCC7901_ECC_MECC4_0_REGISTER
#define MLC_ECC4_CODE_REGISTER_1 				TCC7901_ECC_MECC4_1_REGISTER
#define MLC_ECC4_CODE_REGISTER_2 				TCC7901_ECC_MECC4_2_REGISTER
#define MLC_ECC4_ERROR_NUMBER_REGISTER			TCC7901_ECC_ERRNUM_REGISTER


#define NAND_CMD_PAGE_READ 		0x00
#define NAND_CMD_PAGE_PROGRAM 	0x80
#define NAND_CMD_BLOCK_ERASE 	0x60
#define NAND_CMD_STATUS_READ 	0x70

#define TRYTIME 	4000//250//

//CS
#define NAND_CS_NUM		(4)
#define ND_CS0_ENABLE							NFC_CTRL_REGISTER &= (~ND_BIT_CS0_SELECT_DISABLE)		/*NAND 0 SET low*/
#define ND_CS0_DISABLE							NFC_CTRL_REGISTER |= (ND_BIT_CS0_SELECT_DISABLE)		/*NAND 0 SET high*/
#define ND_CS1_ENABLE							NFC_CTRL_REGISTER &= (~ND_BIT_CS1_SELECT_DISABLE)
#define ND_CS1_DISABLE							NFC_CTRL_REGISTER |= (ND_BIT_CS1_SELECT_DISABLE)		/*NAND 1 SET high*/
#define ND_CS2_ENABLE							NFC_CTRL_REGISTER &= (~ND_BIT_CS2_SELECT_DISABLE)
#define ND_CS2_DISABLE							NFC_CTRL_REGISTER |= (ND_BIT_CS2_SELECT_DISABLE)		/*NAND 2 SET high*/
#define ND_CS3_ENABLE							NFC_CTRL_REGISTER &= (~ND_BIT_CS3_SELECT_DISABLE)
#define ND_CS3_DISABLE							NFC_CTRL_REGISTER |= (ND_BIT_CS3_SELECT_DISABLE)		/*NAND 3 SET high*/

//#define NAND_DMA
tNAND_INFO	gtnandInfo;


//每个bank具有的逻辑block数
U32 numberOfBlocksInBank0;
U32 numberOfBlocksInBank1; 
U32 numberOfPagesInBank0;
U32 numberOfPagesInBank1; 

U32 numberOfPagesInBank;
U32 numberOfBlocksInBank;

U16 operationInProgress[2];
/*	某些MLC型号的nandflash, 一定需要ecc校验 */

//tINTERNAL_STORAGE_MEDIA internalStorageMedia;
/***********************************/
/*  记录flash类型 ：SLC 和 MLC  */
eNAND_CLASS  sbFlashClass;
/***********************************/
tNAND_PROTECT_RESERV protectAndReserv;

/*  记录该flash是否支持TWO plane page program*/
U8 TwoPlanePageProgramSupport = FALSE;
/*	某些flash，等待的时间短 */

U8	sbUsbStatusControl = 0;

U32 gNandDataT[128];
/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_delayUs
* 功能:	delay function
* 参数:	none
* 返回:	none，
*----------------------------------------------------------------------------*/
extern tSYSCLKREGISTER_ST  SysClkRegister;
void hyhwNand_delayUs( U32 us )  // test on 399-133  
{
//	us <<= 3;
	
	U32 clk;
	clk = (SysClkRegister.mCpuClk)*us;
	clk /= 3;
	while( clk --);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_IO_Init
* 功能:	initialize the gpio 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNand_IOInit( void )
{
	U32  nfc_ctrl_reg;
	
	hyhwGpio_setNfc8Function();
	TwoPlanePageProgramSupport = False;

	hyhwCpm_busClkEnable( CKC_NAND );
	
	/* Make Reset */
    NFC_CTRL_RST_REGISTER = (U32)0;
    
    /* reset 后，ctrl 寄存器的值应如下，为程序健壮起见，这里再次赋值为默认值 */
	nfc_ctrl_reg	= 0x03E08000;		    

    /* Set Default NFC Configuration 0x8000d050 配置为DMA使能，片选高，1READ/1WRITE，SETUP TIME=0，HODE TIME=1，PULSWIDTH=3 */
	nfc_ctrl_reg	&=	((ND_BIT_CYCLE_FOR_SETUP_TIME_MASK)&(ND_BIT_CYCLE_FOR_PULSE_WIDTH_MASK)&(ND_BIT_CYCLE_FOR_HOLD_TIME_MASK));
    nfc_ctrl_reg	|= 	(			ND_BIT_DMA_REQUEST	|
 			      			ND_BIT_CYCLE_FOR_SETUP_TIME(1)	|		// setup = 0
 			      			ND_BIT_CYCLE_FOR_PULSE_WIDTH(4)	|		// pw = 3
			      			ND_BIT_CYCLE_FOR_HOLD_TIME(2));			// hold = 1
    /*默认为8BIT总线模式*/
	nfc_ctrl_reg &= ((ND_BIT_PAGE_SIZE_MASK)&(ND_BIT_ADDR_CYCLE_NUM_MASK));
	nfc_ctrl_reg |= (ND_BIT_PAGE_SIZE_2048_8)|(ND_BIT_ADDR_CYCLE_NUM(5));	/*1 PAGE = 2048BYTE,5 ADDR CYCLE*/
	
	/* ctrl 寄存器的值准备好了，赋给寄存器 */
	NFC_CTRL_REGISTER = nfc_ctrl_reg;
	
    
    NFC_INT_REQ_REGISTER = ND_BIT_INT_REQUEST_CLEAR;	
	
 	/* Setup Variable about ECC */
	hyhwCpm_busClkEnable( CKC_ECC );
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_setIoSpeed
* 功能:	set the nand speed 
* 参数:	speed -- 	0  	frequence smaller than 96/48
					1	frequence smaller than 192/96
					2 	frequence smaller than 399/133
* 返回:	none
*----------------------------------------------------------------------------*/

void hyhwNand_setIoSpeed( U32  hClk )
{
	if( hClk < 48 )
	{
		NFC_CTRL_REGISTER	&=	((ND_BIT_CYCLE_FOR_SETUP_TIME_MASK)&(ND_BIT_CYCLE_FOR_PULSE_WIDTH_MASK)&(ND_BIT_CYCLE_FOR_HOLD_TIME_MASK));
		NFC_CTRL_REGISTER	|= 	(			ND_BIT_DMA_REQUEST	|
		ND_BIT_CYCLE_FOR_SETUP_TIME(0)	|		// setup = 0
		ND_BIT_CYCLE_FOR_PULSE_WIDTH(2)	|		// pw = 2
		ND_BIT_CYCLE_FOR_HOLD_TIME(0));			// hold = 0
	}
	else if( hClk < 96 )
	{
		NFC_CTRL_REGISTER	&=	((ND_BIT_CYCLE_FOR_SETUP_TIME_MASK)&(ND_BIT_CYCLE_FOR_PULSE_WIDTH_MASK)&(ND_BIT_CYCLE_FOR_HOLD_TIME_MASK));
		NFC_CTRL_REGISTER	|= 	(			ND_BIT_DMA_REQUEST	|
		ND_BIT_CYCLE_FOR_SETUP_TIME(0)	|		// setup = 0
		ND_BIT_CYCLE_FOR_PULSE_WIDTH(3)	|		// pw = 2
		ND_BIT_CYCLE_FOR_HOLD_TIME(1));			// hold = 1
	}
	else
	{
		NFC_CTRL_REGISTER	&=	((ND_BIT_CYCLE_FOR_SETUP_TIME_MASK)&(ND_BIT_CYCLE_FOR_PULSE_WIDTH_MASK)&(ND_BIT_CYCLE_FOR_HOLD_TIME_MASK));
		NFC_CTRL_REGISTER	|= 	(			ND_BIT_DMA_REQUEST	|
		ND_BIT_CYCLE_FOR_SETUP_TIME(1)	|		// setup = 0
		ND_BIT_CYCLE_FOR_PULSE_WIDTH(4)	|		// pw = 3
		ND_BIT_CYCLE_FOR_HOLD_TIME(2));			// hold = 1
	}
}
 /* Write the program command */
/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_waitingForStatusReady
* 功能:	read the Nand status 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
eNAND_PROCESS hyhwNand_waitingForStatusReady( void )
{
	U32 waitCycle = TRYTIME;
	U32 nandStatus;
	
	done:
	/* Write the read status command */
	NFC_CMD_REGISTER = 0x70;
	
	/* Wait until the prog has completed */
	hyhwNand_delayUs(5);		

	nandStatus = NFC_SINGLE_DATA_REGISTER;
	if( (nandStatus & BIT6) )	//等待nand响应
	{
		//nand的状态ready后，bit0--bit5才有效
		if( !(nandStatus & BIT0) )	//判断操作是否成功
	    {
	        /* bit0==0成功，否则错误 */
	        return NandFlashProcess_success;
	    }
    }
	
	if(waitCycle--)
	{
		goto done;
	}
	return NandFlashProcess_fall;
}

	
void hyhwNand_selectCs( U32 eCs, eNAND_SELECT enable )
{
	if(enable)
	{
		switch( eCs )
		{
			case	CS0:
					{
						ND_CS0_ENABLE;
					}
					break;	
			case 	CS1:
					{
						ND_CS1_ENABLE;	
					}
					break;
			case	CS2:
					{
						ND_CS2_ENABLE;
					}
					break;	
			case 	CS3:
					{
						ND_CS3_ENABLE;					
					}
					break;	
			default:	break;
		}
	}
	else
	{
		
		switch( eCs )
		{
			case	CS0:
					{
						ND_CS0_DISABLE;
					}
					break;	
			case 	CS1:
					{
						ND_CS1_DISABLE;
					}
					break;
			case	CS2:
					{
						ND_CS2_DISABLE;
					}
					break;	
			case 	CS3:
					{
						ND_CS3_DISABLE;
					}
					break;
			default:	break;
		}
	}
}


	
/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_readId
* 功能:	read the nand's ID 
* 参数:	tNAND_ID 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNand_readId( tNAND_ID * ptNandId, eNAND_CS cs )
{
	
	hyhwNand_selectCs( cs, ENABLE );

		/* Write the ID read command */
    NFC_CMD_REGISTER = 0x90;
	/* Write the dummy address */
    NFC_SINGLE_ADDR_CYCLE_REGISTER = 0x00;
    /* 和ICLK配合使用，读的等待时间，大部分nandflash 读命令发完之后的延时为24ms，有一种是50ms
		所以需要这个数据进行调整 */
	hyhwNand_delayUs( 40 );

	ptNandId->makerCode = TCC7901_NFC_SDATA_REGISTER;
	ptNandId->deviceCode = TCC7901_NFC_SDATA_REGISTER;
	ptNandId->extendedIDCode = TCC7901_NFC_SDATA_REGISTER;
	ptNandId->extendedIDCode_e1 = TCC7901_NFC_SDATA_REGISTER;
	ptNandId->extendedIDCode_e2 = TCC7901_NFC_SDATA_REGISTER;

	hyhwNand_selectCs( cs, DISABLE );
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_getSizeAndType
* 功能:	get nands size & type 
* 参数:	tNAND_ID 
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNand_getSizeAndType( tNAND_ID * ptNandId )
{
	
        switch( ptNandId->makerCode )
        {
        case 0x00:
        case 0xEE:              /* no nand flash on this bank */
            ptNandId->nandFlashSize = 0;
            ptNandId->nandFlashType = NAND_16K_512B;
            break;
		case 0x01:				/* Spansion orNand, ice 20060529*/
			switch( ptNandId->deviceCode )
			{
				case 0xF1:
					ptNandId->nandFlashSize = 128;// * 1024 * 1024;
					ptNandId->nandFlashType = NAND_512K_2K;
					break;
				case 0xDA:
				case 0xCA:
					ptNandId->nandFlashSize = 256;// * 1024 * 1024;
					ptNandId->nandFlashType = NAND_512K_2K;
					break;
				case 0x26:			
					/*	for spansion quad b flash	*/
					ptNandId->nandFlashSize = 256;// * 1024 * 1024;
					ptNandId->nandFlashType = NAND_128K_2K;
					/*	read delaytime 85uS. */
					gtnandInfo.readWaitTime = 20;			//现在不用了，但是还放在这里
					break;
				default:
					break;	
			}
			break;
        case 0xEC:              /* samsung */
        	/* 只有samsung的flash 允许支持 short wait */
        	
        case 0x20:              /* STMicroelectronics */
        case 0x98:              /* toshiba (sandisk uses the same number) */
        case 0xAD:				/* Hynix */
        default:                
            switch( ptNandId->deviceCode )
            {
            case 0xE6:
                ptNandId->nandFlashSize = 8;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_16K_512B;
                break;
            case 0x73:
            case 0x33:      /* 1.8V variant */
                ptNandId->nandFlashSize = 16;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_16K_512B;
                break;
            case 0x75:
            case 0x35:      /* 1.8V variant */
                ptNandId->nandFlashSize = 32;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_16K_512B;
                break;
            case 0x76:
            case 0x36:      /* 1.8V variant */
                ptNandId->nandFlashSize = 64;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_16K_512B;
                break;
            case 0x79:
            	/* 旧的型号128M NAND_32K_512B的extendedIDCode 需要发命令91得到extendedIDCode==0x24
            		由于没有遇到过这种型号，省略这个逻辑 */
				ptNandId->nandFlashSize = 128;// * 1024 * 1024;
				ptNandId->nandFlashType = NAND_16K_512B;
				break;
            case 0x78:      /* 1.8V variant, according to samsung*/
            case 0x39:      /* 1.8V variant, according to STM*/
                ptNandId->nandFlashSize = 128;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_16K_512B;
                break;
            case 0xF1:
            case 0xA1:      /* 1.8V variant */
                ptNandId->nandFlashSize = 128;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_128K_2K;
                break;
            case 0xDA:
             	/* 旧的型号256M NAND_32K_512B的extendedIDCode 需要发命令91得到extendedIDCode==0x24
            		由于没有遇到过这种型号，省略这个逻辑 */
				ptNandId->nandFlashSize = 256;// * 1024 * 1024;
				ptNandId->nandFlashType = NAND_128K_2K;
				protectAndReserv.needReadBack = FALSE;
				break;
            case 0xAA:      /* 1.8V variant */
				ptNandId->nandFlashSize = 256;// * 1024 * 1024;
				ptNandId->nandFlashType = NAND_128K_2K;
				break;
            case 0xDC:
             	/* 旧的型号512M NAND_32K_512B的extendedIDCode 需要发命令91得到extendedIDCode==0x25
            		由于没有遇到过这种型号，省略这个逻辑 */
				if( ptNandId->extendedIDCode ==0x84 )
				{
					if( ptNandId->makerCode == 0xAD )
					{
						gtnandInfo.readWaitTime = 20;
					//	sbFlashClass			 = MLC;  //这个信息我会给的

						
						ptNandId->nandFlashSize = 512;// * 1024 * 1024;
						ptNandId->nandFlashType = NAND_256K_2K; 
					}
				}
				else if( ptNandId->extendedIDCode == 0x14 )
				{
					if( ptNandId->makerCode == 0xEC )
					{
						gtnandInfo.readWaitTime = 20;
				//		sbFlashClass			 = MLC;
						
						ptNandId->nandFlashSize = 512;// * 1024 * 1024;
						ptNandId->nandFlashType = NAND_256K_2K; 
					}
				}
				else
				{
					ptNandId->nandFlashSize = 512;// * 1024 * 1024;
					ptNandId->nandFlashType = NAND_128K_2K;
				}
				break;
            case 0xAC:      /* 1.8V variant */
                ptNandId->nandFlashSize = 512;// * 1024 * 1024;
                ptNandId->nandFlashType = NAND_128K_2K;
               break;
            case 0xD3:    
            	if( ptNandId->extendedIDCode == 0x55 &&
            		ptNandId->extendedIDCode_e1 == 0x25 && 
            		ptNandId->extendedIDCode_e2 == 0x58 )
	            {
	            		/* samsung K9L8G08 MLC 这个型号很慢 */
		            ptNandId->nandFlashSize = 1024;// * 1024 * 1024;
		            ptNandId->nandFlashType = NAND_256K_2K;

		            gtnandInfo.readWaitTime = 20;
		            
		    //        sbFlashClass			 = MLC;
		            if(ptNandId->makerCode == 0xec)
		            {
		      	     	TwoPlanePageProgramSupport = True;
		             	ptNandId->nandFlashType = NAND_512K_2K_JOINT;
		            }
                }
                else if(ptNandId->extendedIDCode == 0x51)/*K9WAG08U1A*/
                {
                	ptNandId->nandFlashSize = 1024;// * 1024 * 1024;
                	ptNandId->nandFlashType = NAND_128K_2K;
	                gtnandInfo.readWaitTime = 20;
	                
	          //      sbFlashClass			 = MLC;
	                if( ptNandId->makerCode == 0xec )
	                {
	                	TwoPlanePageProgramSupport = True;
	       	        	ptNandId->nandFlashType = NAND_256K_2K_MLC;
	                }
                }
                else if(ptNandId->extendedIDCode == 0x14)//k9g8g08u0m
                {
                	ptNandId->nandFlashSize = 1024;// * 1024 * 1024;
                	ptNandId->nandFlashType = NAND_256K_2K;
                	gtnandInfo.readWaitTime = 20;
	                
	          //      sbFlashClass			 = MLC;
	                if(ptNandId->makerCode == 0xec)
	                {
	                	TwoPlanePageProgramSupport = True;
	                	ptNandId->nandFlashType = NAND_512K_2K_JOINT;
	                }
                }
                
                else
                {
                	/* samsung K9K9GO8UOM   0x51, 0x95, 0x58 */
	                ptNandId->nandFlashSize = 1024;// * 1024 * 1024;
	                ptNandId->nandFlashType = NAND_128K_2K;
                }
                
                break;
            case 0xD5:      /* Samsung 2Gbyte flash */
            	//K9LAG08
                if( ptNandId->extendedIDCode == 0x55 )
                {
                	ptNandId->nandFlashSize = 2 * 1024;// * 1024 * 1024;
                	ptNandId->nandFlashType = NAND_256K_2K;
	                gtnandInfo.readWaitTime = 20;
	                
	          //      sbFlashClass			 = MLC;
	                if( ptNandId->makerCode == 0xec )
	                {
	                	TwoPlanePageProgramSupport = True;
	       	        	ptNandId->nandFlashType = NAND_512K_2K_JOINT;
	                }
	                
                }
                else  if( ptNandId->extendedIDCode == 0x14 )
                {
                	ptNandId->nandFlashSize = 2 * 1024;// * 1024 * 1024;
                	ptNandId->nandFlashType = NAND_512K_4K;
	                gtnandInfo.readWaitTime = 20;
	                
	          //      sbFlashClass			 = MLC;
	                if( ptNandId->makerCode == 0xec )
	                {
	                //	TwoPlanePageProgramSupport = True;
	       	        //	ptNandId->nandFlashType = NAND_1M_4K_JOINT;
	                }
	                
                }
                else if( ptNandId->extendedIDCode == 0x94 )
                {//K9GAG08U0D
                	ptNandId->nandFlashSize = 2 * 1024;
                	ptNandId->nandFlashType = NAND_512K_4K;
	                gtnandInfo.readWaitTime = 20;
	                TwoPlanePageProgramSupport = False;
	                //对于4K pagesize的需要调整8300中NFC的PSIZE设置
	                NFC_CTRL_REGISTER &= ((ND_BIT_PAGE_SIZE_MASK));
					NFC_CTRL_REGISTER |= (ND_BIT_PAGE_SIZE_4096_8);
	                
                }
                break;
            case 0xD7:      /* Samsung 4Gbyte flash */
            	//K9LBG08
               ptNandId->nandFlashSize = 4 * 1024;// * 1024 * 1024;
               ptNandId->nandFlashType = NAND_512K_4K;
	           gtnandInfo.readWaitTime = 20;
	                
	           if( ptNandId->makerCode == 0xec )
	           {//先用不是双片写的
	    //          	TwoPlanePageProgramSupport = True;
	    //   	       	ptNandId->nandFlashType = NAND_1M_4K_JOINT;
	           }
	           //对于4K pagesize的需要调整8300中NFC的PSIZE设置
	           	NFC_CTRL_REGISTER &= ((ND_BIT_PAGE_SIZE_MASK));
				NFC_CTRL_REGISTER |= (ND_BIT_PAGE_SIZE_4096_8);
				{
					numberOfPagesInBank  = 0x100000;
					numberOfBlocksInBank = 0x2000;
				}
				
				
	           break;
            default:
                ptNandId->nandFlashSize = 0;
                ptNandId->nandFlashType = NAND_128K_2K;
            }
            break;   
        }
   

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_init
* 功能:	初始化NandFlash，获得nandflash的信息
*		系统中，仅被nandLogicDrv_Init在初始化中调用一次，所以，可以overlay在 sram
*		中，以节省sram的使用
* 参数:	none
* 返回:	none， nandflash的信息在全局变量gtnandInfo中
*----------------------------------------------------------------------------*/
eNAND_PROCESS hyhwNandFlash_Init( void )
{
    U32 size,i;
    U32 init_count;		/*初始化的次数，防止一次初始化出错 */
    tNAND_ID tNandId[NAND_CS_NUM];
	
	
	for(init_count = 0; init_count < 3; init_count ++)
	{
	
		/*设置gpio,片选，建立时间，保持时间*/
		
		hyhwNand_IOInit();
	    size = sizeof( tNAND_ID );
	    memset( (U8 *)&gtnandInfo, 0, sizeof(tNAND_INFO) );
	    for(i = 0; i < NAND_CS_NUM; i ++ )
	    {
	    	memset(&tNandId[i] , 0 , size );
	    }

	         
	    protectAndReserv.needReadBack = TRUE;
	    
	    for(i = 0; i < NAND_CS_NUM; i ++ )
	    {
	    	hyhwNand_readId( &tNandId[i], i );
	    }

	  	if( (tNandId[1].makerCode != tNandId[0].makerCode))
		{
			sbFlashClass = SLC;
		}
		else 
		{
			sbFlashClass = MLC;
		}
	  
	  	for(i = 0; i < NAND_CS_NUM; i ++ )
	    {
	       	hyhwNand_getSizeAndType( &tNandId[i] );
	    }
	    
	 
		/* 如果两个nandflash type的型号不一样，取其中一个大的, 丢弃一个小的 */
	    if( tNandId[0].nandFlashType != tNandId[1].nandFlashType )      /* incompatible at this moment */
	    {
	        if( tNandId[0].nandFlashSize >= tNandId[1].nandFlashSize )  /* take the one with the biggest size, discard the other one */
	        {
	            tNandId[1].nandFlashSize = 0;
	            tNandId[1].nandFlashType = tNandId[0].nandFlashType;	/* 这句其实没有用，反正size已经为0了 */
	        }
	        else
	        {
	            tNandId[0].nandFlashSize = 0;
	            tNandId[0].nandFlashType = tNandId[1].nandFlashType;	/* 这句其实没有用，反正size已经为0了 */
	        }
	    }

	    /*	现在的情况是，如果2片flash，则型号必然是一样的，
	    	使用nandFlashType[0]进行判断， 设置一些参数 */
	    if(tNandId[0].nandFlashType == NAND_128K_2K)
	    {
	        
	        gtnandInfo.blockSize	=	128*1024;
		    gtnandInfo.pageSize		=	2048;
		    gtnandInfo.blockSizeBit	=	17;				/* 1<<17 = 128k */
		    gtnandInfo.pageSizeBit	=	11;				/* 1<<11 = 2048 */
		    gtnandInfo.pagesPerBlockBit	=	6;
	    	
	    }
	    else if(tNandId[0].nandFlashType == NAND_512K_2K ||tNandId[0].nandFlashType == NAND_512K_2K_MLC)
	    {
	        gtnandInfo.blockSize	=	512*1024;
	        gtnandInfo.pageSize		=	2048;
	        gtnandInfo.blockSizeBit	=	19;				/* 1<<19 = 512k */
	        gtnandInfo.pageSizeBit	=	11;				/* 1<<11 = 2048 */
	        gtnandInfo.pagesPerBlockBit	=	8;
	    
	    }
	    else if( tNandId[0].nandFlashType == NAND_16K_512B )
	    {
	        gtnandInfo.blockSize	=	16*1024;
	        gtnandInfo.pageSize		=	512;
	        gtnandInfo.blockSizeBit	=	14;				/* 1<<14 = 16k */
	        gtnandInfo.pageSizeBit	=	9;				/* 1<<9 = 512 */
	        gtnandInfo.pagesPerBlockBit	=	5;

	    }
	    else if( tNandId[0].nandFlashType == NAND_32K_512B )
	    {
	        gtnandInfo.blockSize	=	32*1024;
	        gtnandInfo.pageSize		=	512;
	        gtnandInfo.blockSizeBit	=	15;				/* 1<<15 = 32k */
	        gtnandInfo.pageSizeBit	=	9;				/* 1<<9 = 512 */
	        gtnandInfo.pagesPerBlockBit	=	6;
	        
	        
	    }
	    else if( tNandId[0].nandFlashType == NAND_256K_2K_MLC || tNandId[0].nandFlashType  == NAND_256K_2K)
	    {
	        
	        gtnandInfo.blockSize	=	256*1024;
	        gtnandInfo.pageSize		=	2048;
	        gtnandInfo.blockSizeBit	=	18;				/* 1<<18 = 256k */
	        gtnandInfo.pageSizeBit	=	11;				/* 1<<11 = 2048 */
	        gtnandInfo.pagesPerBlockBit	=	7;
		            
	    }
	    else if( tNandId[0].nandFlashType == NAND_512K_2K_JOINT)//two plane page program support
	    {
	    	gtnandInfo.blockSize	=	512*1024;
	        gtnandInfo.pageSize		=	2048;
	        gtnandInfo.blockSizeBit	=	19;				/* 1<<18 = 256k */
	        gtnandInfo.pageSizeBit	=	11;				/* 1<<11 = 2048 */
	        gtnandInfo.pagesPerBlockBit	=	8;	
	       
	    }
	    else if( tNandId[0].nandFlashType == NAND_512K_4K)//two plane page program support
	    {
	    	gtnandInfo.blockSize	=	512*1024;
	        gtnandInfo.pageSize		=	4096;
	        gtnandInfo.blockSizeBit	=	19;				/* 1<<18 = 256k */
	        gtnandInfo.pageSizeBit	=	12;				/* 1<<11 = 2048 */
	        gtnandInfo.pagesPerBlockBit	=	7;	
	       
	    }    
	    else if( tNandId[0].nandFlashType == NAND_1M_4K_JOINT)//two plane page program support
	    {
	    	gtnandInfo.blockSize	=	1024*1024;
	        gtnandInfo.pageSize		=	4096;
	        gtnandInfo.blockSizeBit	=	20;				/* 1<<18 = 256k */
	        gtnandInfo.pageSizeBit	=	12;				/* 1<<11 = 2048 */
	        gtnandInfo.pagesPerBlockBit	=	8;	
	       
	    }    
	    gtnandInfo.type		=	tNandId[0].nandFlashType;
	//	gtnandInfo.size		=   tNandId[0].nandFlashSize + tNandId[1].nandFlashSize;

		numberOfPagesInBank  = (tNandId[0].nandFlashSize<<12)/(gtnandInfo.pageSize>>8);
		numberOfBlocksInBank = (tNandId[0].nandFlashSize<<12)/(gtnandInfo.blockSize>>8);
		
		gtnandInfo.size = 0;	
		for( i = 0; i < NAND_CS_NUM; i ++ )
		{
			gtnandInfo.size		+=   tNandId[i].nandFlashSize;
		}	
		
	    //保留前1M空间
	    gtnandInfo.protectPageStart	= 0x0; 
		gtnandInfo.protectPageEnd	= 0x100000>>gtnandInfo.pageSizeBit;
	    //保留总的NAND后面至少60M空间
			//gtnandInfo.reservePageStart = gtnandInfo.size>>gtnandInfo.pageSizeBit;
			//gtnandInfo.reservePageEnd   = gtnandInfo.reservePageEnd - (0x3c00000>>gtnandInfo.pageSizeBit);
		//reserve空间已经改到nand前端，不需在后端保护, 2011-06-17
		gtnandInfo.reservePageStart	= 0;
		gtnandInfo.reservePageEnd	= 0;
		
		
		/**************** 2008.3.22 *******************/
		//保留前1M空间
		//修改，保留空间只有1个block，该block被telechips  DFU占用   高---2011-01-20
		protectAndReserv.protectPageStart	= 0x0;
		protectAndReserv.protectPageEnd		= gtnandInfo.blockSize >> gtnandInfo.pageSizeBit;
		protectAndReserv.protectblockStart	= protectAndReserv.protectPageStart >> gtnandInfo.pagesPerBlockBit;
		protectAndReserv.protectblockEnd	= protectAndReserv.protectPageEnd >> gtnandInfo.pagesPerBlockBit;
		
		if( sbFlashClass == SLC )
		{		
			//单片nand flash
			numberOfPagesInBank0  = gtnandInfo.size <<(NAND_SIZE_UNIT_BIT - gtnandInfo.pageSizeBit);//第一片nand的page数
			numberOfBlocksInBank0 = numberOfPagesInBank0 >> gtnandInfo.pagesPerBlockBit;//第一片nand的block数
			numberOfPagesInBank1  = 0;
			numberOfBlocksInBank1 = 0;
		}
		else
		{
			//多片nand flash
			numberOfPagesInBank0  = gtnandInfo.size << (NAND_SIZE_UNIT_BIT-( gtnandInfo.pageSizeBit+1 ));//因为size是以M为单位的
			numberOfBlocksInBank0 = numberOfPagesInBank0 >> gtnandInfo.pagesPerBlockBit;
			numberOfPagesInBank1  = numberOfPagesInBank0;
			numberOfBlocksInBank1 = numberOfPagesInBank0 >> gtnandInfo.pagesPerBlockBit;
		}
		//保留第一片NAND后面10M空间，这些空间被telechips  DFU占用
		//注：被保护的page号在 【 programAreapageStart <= PAGE < programAreapageEnd 】 之间
		//得到第一片nand的最大page号
		protectAndReserv.programAreapageEnd		= 
						tNandId[0].nandFlashSize<<(NAND_SIZE_UNIT_BIT-gtnandInfo.pageSizeBit);
		//得到第一片nand减去10M的page号，保护的起始
		protectAndReserv.programAreapageStart	= 
						((tNandId[0].nandFlashSize-0xA)<<(NAND_SIZE_UNIT_BIT-gtnandInfo.pageSizeBit));
		//注：被保护的block号在 【 programAreablockStart <= PAGE < programAreablockEnd 】 之间
		protectAndReserv.programAreablockEnd	= protectAndReserv.programAreapageEnd>>gtnandInfo.pagesPerBlockBit;
		protectAndReserv.programAreablockStart	= protectAndReserv.programAreapageStart>>gtnandInfo.pagesPerBlockBit;
		
		
		hyhwEcc_rwNandMlcEcc4Set( ((U32)&NFC_WORD_DATA_REGISTER), 512);/*在设置完NAND后，设置ECC,固定为512字节*/
		
		//第一片NAND后面10M空间，这些空间被telechips  DFU占用，总容量减10M
		//gtnandInfo.size -= 8;
		gtnandInfo.size -= 10;//DFU占用后10M
		
		if( gtnandInfo.size)
		{
			/*正常读出nand信息的情况*/
			return NandFlashProcess_success;
		}
	
	}
	
	/*读了几次，都没有正常读出nand信息的情况*/
	return NandFlashProcess_fall;

}
/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_pageRead
* 功能:	从指定page的指定偏移读取指定大小的数据， 如果偏移量为0，则从page开始的地方
*		读数据，和原来的pageRead的函数的功能一样
* 参数:	data----指针参数，读取数据的buffer
*		page----指定的page号
*		offset--在指定page中的偏移
*		size----需要写入数据的长度（byte）
* 返回:	HY_OK	//2006.12.23 高 修改
*		HY_ERROR//如果错误bit数超过4，则返回HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_DataRead(Int8 * pdata, U32 page, U32 offset, U32 size)
{
	if(size == 0)
	{
		return HY_OK;
	}
	
    wLock_NandFlash();
	if( ( size == gtnandInfo.pageSize ) && ( ( (U32)pdata & 0x03 ) == 0) )
	{
		
		
		U32 cycle,i,temp;
		U32 *pRsDataCorrect;
		U32 data,cs;

		pRsDataCorrect = (U32*)pdata;
		cs = 0;
		while(1)
		{
			if( page < numberOfPagesInBank )
			{
				break;
			}
			page -= numberOfPagesInBank;
			cs ++;
		}

	    hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
	    /* Write the read1 command, pointer set to region A */
	    NFC_CMD_REGISTER = 0x00;
	    /* Write the address */
		NFC_SINGLE_ADDR_CYCLE_REGISTER = (offset&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((offset>>8)&0xFF);
	    NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
		NFC_CMD_REGISTER = 0x30;

		hyhwNand_delayUs( 50 );
			
			
		
		//temp = gtnandInfo.pageSize >> 6 ;
		//任何ECC都按照512字节计算
		temp = 32;
		/*边读边ECC校验*/
		for( cycle = 0; cycle < (gtnandInfo.pageSize>>9); cycle ++ )
		{
			hyhwEcc_ecc4DecodeStart();
			__asm
			{	
				mov 	i,#0
	      
	       readCycleTime:	
	       		ldr r6,  [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
	            ldr r7,  [0xf00b0010]
	            ldr r8,  [0xf00b0010]
	            ldr r9,  [0xf00b0010]
	            stmia pRsDataCorrect!, {r6-r9} 
		    	add i,i,#1
				cmp	i,temp
				bcc	readCycleTime	
			}	
			
			
			if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
			{
				hyhwNand_selectCs( cs, DISABLE );
				unLock_NandFlash();
				return HY_ERROR;
			}

			/*do error check*/
			if( hyhwEcc_readMlcEcc4ErrorCorrect( (U8*)(pdata+512*cycle) ) == ecc_symbol_error_occurred_large_than_4 )
			{
				U32 j;
				U32 isErased;
				U8 *pEarseData;
				//用于判断是否是擦除后的情况
				pEarseData = (U8*)(pdata+512*cycle);
				isErased = 1;
				for(j=0;j<512;j++)
				{
					if((pEarseData[j]) != 0xff)
					{
						isErased = 0;
						break;
					}
				}
				if(isErased == 0)
				{
					
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;//如果是不能修改的错误就跳出循环，然后告之系统
				}
			
			}
			data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
		}
		hyhwNand_selectCs( cs, DISABLE );
		unLock_NandFlash();

	    return HY_OK;
    }
    else
    {    
	    U32 i,j,cs,temp;
		Int8 *pRsData;
		U8 data;
		U32 off;
		
		
		pRsData = pdata;
		
		cs = 0;
		while(1)
		{
			if( page < numberOfPagesInBank )
			{
				break;
			}
			page -= numberOfPagesInBank;
			cs ++;
		}
		hyhwNand_selectCs( cs, ENABLE );/*cs enable*/

		for(off = 0; (off + 512)< (offset); off += 528,offset += 16 );
		
	    /* Write the read1 command, pointer set to region A */
	    NFC_CMD_REGISTER = 0x00;
	    /* Write the address */
		NFC_SINGLE_ADDR_CYCLE_REGISTER = (off&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((off>>8)&0xFF);
	    NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
		NFC_CMD_REGISTER = 0x30;
		hyhwNand_delayUs( 50 );
		
		temp = size;
		while( temp )
		{
			hyhwEcc_ecc4DecodeStart();
			for( i = 0; i < 128; i ++ )
			{
				gNandDataT[i] = NFC_WORD_DATA_REGISTER;
			}
				
			if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
			{
				hyhwNand_selectCs( cs, DISABLE );
				unLock_NandFlash();
				return HY_ERROR;
			}

			/* do error check,if error num large than 4, return hy_error */
			
			if( hyhwEcc_readMlcEcc4ErrorCorrect( (U8*)gNandDataT ) == ecc_symbol_error_occurred_large_than_4 )
			{
				U32 j;
				U32 isErased;
				U8 *pEarseData;
				//用于判断是否是擦除后的情况
				pEarseData = (U8*)gNandDataT;
				isErased = 1;
				for(j=0;j<512;j++)
				{
					if((pEarseData[j]) != 0xff)
					{
						isErased = 0;
						break;
					}
				}
				if(isErased == 0)
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;//如果是不能修改的错误就跳出循环，然后告之系统
				}
			
			}
			
			data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
			if((temp > (512-(offset-off))))
			{
				memcpy(pdata, ((U8*)&gNandDataT[0])+((offset-off)&0x1ff), 512-(offset-off));
				temp -= 512-(offset-off);
				pdata += 512-(offset-off);
				off += 528;
				offset = off;
			}
			else
			{
				memcpy(pdata, &((U8*)gNandDataT)[offset-off], temp );
				hyhwNand_selectCs( cs, DISABLE );
				unLock_NandFlash();	
			    return HY_OK;
			}
			
		}
		
    }
}



				
		
				
			

//允许非page读取nand
void hyhwNandFlashNonPageRead_Enable(void)
{
#ifdef MLC_FLASH
	sNonPageRead_Enable = True ;
#endif
}
//禁止非page读取nand
void hyhwNandFlashNonPageRead_Disable(void)
{
#ifdef MLC_FLASH
	sNonPageRead_Enable = False ;
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_Erase
* 功能:	将指定的block擦除
* 参数:	block----需要擦除的block号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_Erase( U32 block )
{
	
	
	
	if(TwoPlanePageProgramSupport)
	{
		return hyhwNandFlash_TwoPageModeBlockErase(block);
	}
	else
	{
		U32 tryTime,i;
		U32 cs;

#ifndef NAND_DEBUG		
		//protect 放在nand前端（0～1M），所以不用考虑小于start的情况，但是对于双片的情况，program区都需要考虑
		if( ( block < protectAndReserv.protectblockEnd ) || (( block >= protectAndReserv.programAreablockStart )&&(block < protectAndReserv.programAreablockEnd)) )
		{
			return HY_ERROR;
		}
#endif		
		cs = 0;
		while(1)
		{
			if( block < numberOfBlocksInBank )
			{
				break;
			}
			block -= numberOfBlocksInBank;
			cs ++;
		}
		
		block = block << gtnandInfo.pagesPerBlockBit;
		wLock_NandFlash();
		hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
		
		/* Write the auto block erase setup command */
	    NFC_CMD_REGISTER = 0x60;
		/* Write the address */
		NFC_SINGLE_ADDR_CYCLE_REGISTER = (block&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((block>>8)&0xFF);
		NFC_SINGLE_ADDR_CYCLE_REGISTER = ((block>>16)&0xFF);
	    /* Write the erase command */
	    NFC_CMD_REGISTER = 0xD0;

		
		
		hyhwNand_delayUs(50);
	 	if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
	   	{
	   		hyhwNand_selectCs( cs, DISABLE );
	   		unLock_NandFlash();
			return HY_ERROR;
	   	}		

		hyhwNand_selectCs( cs, DISABLE );	
		unLock_NandFlash();
	    return HY_OK;
	} 
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_PageWrite
* 功能:	向指定的page中写入指定大小的数据,必须整页写
* 参数:	data----指针参数，需要写入数据
*		page----指定的page号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite( Int8* pdata, U32 page, U32 size )
{ 
   
   	U32 data,cs,temp,time;
	U32 i,cycle;
    U32 statusRead,dataCorrect;
 //   U32 *pdata;

 	 //protect 放在nand前端（0～1M），所以不用考虑小于start的情况，但是对于双片的情况，program区都需要考虑
#ifndef NAND_DEBUG
	if( ( (page >= protectAndReserv.programAreapageStart )&&(page < protectAndReserv.programAreapageEnd)) || ( (page < protectAndReserv.protectPageEnd )))
	{
		return HY_ERROR;
	}  	
#endif	
 	cs = 0;
	while(1)
	{
		if( page < numberOfPagesInBank )
		{
			break;
		}
		page -= numberOfPagesInBank;
		cs ++;
	}
	
/**/
	wLock_NandFlash();
    hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
#ifdef NAND_DEBUG		
		hyhwGpio_setLow( PORT_F,BIT9 );
#endif
   	/* Write the sequential data input command */
    NFC_CMD_REGISTER = 0x80;
    /* Write the address */
    NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);

   	if( ( size == gtnandInfo.pageSize ) && ( ( (U32)pdata & 0x03 ) == 0 ) )
   	{
		
		U32 * pRsData;
		U32 writeCycleTime;
		pRsData = (U32*)pdata;
			
		temp = 32;//对于512字节ecc时，分32次传输，每次16字节
		writeCycleTime = gtnandInfo.pageSize>>9;//总共需要分几次512字节进行写。
		__asm
	    {	
	    	mov		cycle,#0
	    	
	   	writeCycle: 
			mov 	r6,#0
			str 	r6,[0xf005b00c]	//ecc start
			ldr     r6,[0xf005b000]
			orr     r6,r6,#4
			str     r6,[0xf005b000]
       	
       		mov 	i,#0
       	upWord:	
       		cmp 	i,temp
       		bcs downWord
       		
       		ldmia pRsData!, {r6-r9} 
            
            str r6,  [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
            str r7,  [0xf00b0010]
            str r8,  [0xf00b0010]
            str r9,  [0xf00b0010]
	    	add      i,i,#1
			b		 upWord 	
	    downWord:
		/* ecc  get */
								//here may set a time delay
			ldr r6,	 [0xf005b094]	/*judge that is ecc encode complete*/
			tst r6,	 #0x80000
			beq      downWord		//不相等，则跳转
			
			mov r9, #0xf005b010	/* write ecc code */
			ldmia r9!, {r6-r8} 
			    	
	    	ldr r9,[0xf005b000]
	    	bic r9,r9,#7		//清除最后三位	
			str r9,[0xf005b000]

	    	
	        str r6, [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
	        str r7, [0xf00b0010]
	        str r8, [0xf00b0010]
			str r8, [0xf00b0010]
			
			add		cycle,cycle,#1
			cmp      cycle,writeCycleTime
			bcc      writeCycle		//如果小于，则跳转


		}

		
		 /* Write the program command */
		NFC_CMD_REGISTER = 0x10;
#ifdef NAND_DEBUG		
		hyhwGpio_setHigh( PORT_F,BIT9 );
#endif

		
		hyhwNand_delayUs(300); 
			
		if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
#ifdef NAND_DEBUG		
		hyhwGpio_setLow( PORT_F,BIT9 );
#endif
		if( protectAndReserv.needReadBack == TRUE )	/*if slc,that need not read data back*/
		{			
		    NFC_CMD_REGISTER = 0x00;
		    /* Write the address */
			NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
			NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	    	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
			NFC_CMD_REGISTER = 0x30;
			
			
			
			hyhwNand_delayUs( 50 ); //???????????
			
			temp = 32;//对于512字节ecc时，分32次传输，每次16字节
			for( cycle = 0; cycle <(gtnandInfo.pageSize>>9); cycle ++ )
			{
				hyhwEcc_ecc4DecodeStart();
		
				for( i = 0; i < 128; i ++ )
				{
					data = NFC_WORD_DATA_REGISTER;
			
				}
					
				if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}

				/* do error check,if error num large than 4, return hy_error */
				if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}
				data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
			}
		}	
		hyhwNand_selectCs( cs, DISABLE );
#ifdef NAND_DEBUG		
	hyhwGpio_setHigh( PORT_F,BIT9 );
#endif
		unLock_NandFlash();
		return HY_OK;
	}
	else
	{
		Int8 *pwriteData;
		U32 dataTemp;
		
		pwriteData = pdata;
		//ECC总是按照512字节进行的
		temp = size;
		while( size )
		{
			hyhwEcc_ecc4EncodeStart();
			
			memcpy( (Int8*)gNandDataT, pwriteData, 512 );
			
			for( i = 0; i < 128; i ++ ) 
			{
				NFC_WORD_DATA_REGISTER = gNandDataT[i];
			}
			
			if(hyhwEcc_waitEncodingCompleted()==ecc_uncompleted)
			{
				hyhwNand_selectCs( cs, DISABLE );
				unLock_NandFlash();
				return HY_ERROR;
			}
			if(size < 512)
			{
				size = 0;
				break;
			}
			size -= 512;
			pwriteData += 512;
		}
		
		 /* Write the program command */
		NFC_CMD_REGISTER = 0x10;
		hyhwNand_delayUs( 300 );//??????????????????
		
		
		if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
		
		size = temp;
		if( protectAndReserv.needReadBack == TRUE )	/*if slc,that need not read data back*/
		{			
		    NFC_CMD_REGISTER = 0x00;
		    /* Write the address */
			NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
			NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	    	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
			NFC_CMD_REGISTER = 0x30;
			
			
			
			hyhwNand_delayUs( 50 ); //???????????
			
			
			for( cycle = 0; cycle <(gtnandInfo.pageSize>>9); cycle ++ )
			{
				hyhwEcc_ecc4DecodeStart();
		
				for( i = 0; i < 128; i ++ )
				{
					data = NFC_WORD_DATA_REGISTER;
			
				}
					
				if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}

				/* do error check,if error num large than 4, return hy_error */
				if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}
				data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
				if(size> 512 )
				{
					size -=512;
				}
				else
				{
					break;
				}
			}
		}
		hyhwNand_selectCs( cs, DISABLE );
		unLock_NandFlash();
		return HY_OK;			
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_PageWrite_slc
* 功能:	向指定的page中写入指定大小的数据,必须整页写
* 参数:	data----指针参数，需要写入数据
*		page----指定的page号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite_slc( Int8* pdata, U32 page, U32 offset, U32 size )
{ 
   
   	U32 data,cs,temp,time;
	U32 i,cycle, off;
    U32 statusRead,dataCorrect;
 //   U32 *pdata;

 	 //protect 放在nand前端（0～1M），所以不用考虑小于start的情况，但是对于双片的情况，program区都需要考虑
#ifndef NAND_DEBUG
	if( ( (page >= protectAndReserv.programAreapageStart )&&(page < protectAndReserv.programAreapageEnd)) || ( (page < protectAndReserv.protectPageEnd )))
	{
		return HY_ERROR;
	}  	
#endif	
 	cs = 0;
	while(1)
	{
		if( page < numberOfPagesInBank )
		{
			break;
		}
		page -= numberOfPagesInBank;
		cs ++;
	}
	
/**/
	wLock_NandFlash();
    hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
#ifdef NAND_DEBUG		
		hyhwGpio_setLow( PORT_F,BIT9 );
#endif
   	
   	//取整offset为0，512+16，1024+32，1536+48
	off = offset/512;
	offset += off*16;
	
   	
   	/* Write the sequential data input command */
    NFC_CMD_REGISTER = 0x80;
    /* Write the address */
    NFC_SINGLE_ADDR_CYCLE_REGISTER = (offset&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((offset>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);

   	if( ( size == gtnandInfo.pageSize ) && ( ( (U32)pdata & 0x03 ) == 0 ) )
   	{
		
		U32 * pRsData;
		U32 writeCycleTime;
		pRsData = (U32*)pdata;
			
		temp = 32;//对于512字节ecc时，分32次传输，每次16字节
		writeCycleTime = gtnandInfo.pageSize>>9;//总共需要分几次512字节进行写。
		__asm
	    {	
	    	mov		cycle,#0
	    	
	   	writeCycle: 
			mov 	r6,#0
			str 	r6,[0xf005b00c]	//ecc start
			ldr     r6,[0xf005b000]
			orr     r6,r6,#4
			str     r6,[0xf005b000]
       	
       		mov 	i,#0
       	upWord:	
       		cmp 	i,temp
       		bcs downWord
       		
       		ldmia pRsData!, {r6-r9} 
            
            str r6,  [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
            str r7,  [0xf00b0010]
            str r8,  [0xf00b0010]
            str r9,  [0xf00b0010]
	    	add      i,i,#1
			b		 upWord 	
	    downWord:
		/* ecc  get */
								//here may set a time delay
			ldr r6,	 [0xf005b094]	/*judge that is ecc encode complete*/
			tst r6,	 #0x80000
			beq      downWord		//不相等，则跳转
			
			mov r9, #0xf005b010	/* write ecc code */
			ldmia r9!, {r6-r8} 
			    	
	    	ldr r9,[0xf005b000]
	    	bic r9,r9,#7		//清除最后三位	
			str r9,[0xf005b000]

	    	
	        str r6, [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
	        str r7, [0xf00b0010]
	        str r8, [0xf00b0010]
			str r8, [0xf00b0010]
			
			add		cycle,cycle,#1
			cmp      cycle,writeCycleTime
			bcc      writeCycle		//如果小于，则跳转


		}

		
		 /* Write the program command */
		NFC_CMD_REGISTER = 0x10;
#ifdef NAND_DEBUG		
		hyhwGpio_setHigh( PORT_F,BIT9 );
#endif

		
		hyhwNand_delayUs(300); 
			
		if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
#ifdef NAND_DEBUG		
		hyhwGpio_setLow( PORT_F,BIT9 );
#endif
		if( protectAndReserv.needReadBack == TRUE )	/*if slc,that need not read data back*/
		{
		    NFC_CMD_REGISTER = 0x00;
		    /* Write the address */
			NFC_SINGLE_ADDR_CYCLE_REGISTER = (offset&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((offset>>8)&0xFF);
	    	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
			NFC_CMD_REGISTER = 0x30;
			
			
			
			hyhwNand_delayUs( 50 ); //???????????
			
			temp = 32;//对于512字节ecc时，分32次传输，每次16字节
			for( cycle = 0; cycle <(gtnandInfo.pageSize>>9); cycle ++ )
			{
				hyhwEcc_ecc4DecodeStart();
		
				for( i = 0; i < 128; i ++ )
				{
					data = NFC_WORD_DATA_REGISTER;
			
				}
					
				if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}

				/* do error check,if error num large than 4, return hy_error */
				if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}
				data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
			}
		}	
		hyhwNand_selectCs( cs, DISABLE );
#ifdef NAND_DEBUG		
	hyhwGpio_setHigh( PORT_F,BIT9 );
#endif
		unLock_NandFlash();
		return HY_OK;
	}
	else
	{
		Int8 *pwriteData;
		U32 dataTemp;
		
		pwriteData = pdata;
		//ECC总是按照512字节进行的
		temp = size;
		while( size )
		{
			hyhwEcc_ecc4EncodeStart();
			
			memcpy( (Int8*)gNandDataT, pwriteData, 512 );
			
			for( i = 0; i < 128; i ++ ) 
			{
				NFC_WORD_DATA_REGISTER = gNandDataT[i];
			}
			
			if(hyhwEcc_waitEncodingCompleted()==ecc_uncompleted)
			{
				hyhwNand_selectCs( cs, DISABLE );
				unLock_NandFlash();
				return HY_ERROR;
			}
			if(size < 512)
			{
				size = 0;
				break;
			}
			size -= 512;
			pwriteData += 512;
		}
		
		 /* Write the program command */
		NFC_CMD_REGISTER = 0x10;
		hyhwNand_delayUs( 300 );//??????????????????
		
		
		if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
		
		size = temp;
		if( protectAndReserv.needReadBack == TRUE )	/*if slc,that need not read data back*/
		{
		    NFC_CMD_REGISTER = 0x00;
		    /* Write the address */
			NFC_SINGLE_ADDR_CYCLE_REGISTER = (offset&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((offset>>8)&0xFF);
	    	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>8)&0xFF);
			NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page>>16)&0xFF);
			NFC_CMD_REGISTER = 0x30;
			
			
			
			hyhwNand_delayUs( 50 ); //???????????
			
			
			for( cycle = 0; cycle <(gtnandInfo.pageSize>>9); cycle ++ )
			{
				hyhwEcc_ecc4DecodeStart();
		
				for( i = 0; i < 128; i ++ )
				{
					data = NFC_WORD_DATA_REGISTER;
			
				}
					
				if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}

				/* do error check,if error num large than 4, return hy_error */
				if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
				{
					hyhwNand_selectCs( cs, DISABLE );
					unLock_NandFlash();
					return HY_ERROR;
				}
				data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
				if(size> 512 )
				{
					size -=512;
				}
				else
				{
					break;
				}
			}
		}
		hyhwNand_selectCs( cs, DISABLE );
		unLock_NandFlash();
		return HY_OK;			
	}
	
}


/*-----------------------------------------------------------------------------
* 函数:	hyNandflash_InformUsbStatus
* 功能:	在 usb 状态下，nandflash hardware 驱动会保护reserve区，这个函数让usb 准备函数
*		通知nandflash 硬件drv，进入了usb状态
* 参数:	usbStatus	--	1, 表示要进入usb 状态了，0 表示退出出usb 状态
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandflash_InformUsbStatus(U32 usbStatus)
{
	sbUsbStatusControl 	= usbStatus;
}


/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_EnableWrite
* 功能:	将nandflash的WP置高，使写操作可以进行，即去掉写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

void hyhwNandFlash_EnableWrite(void) ///现在还没有写保护功能引脚
{
}

/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_DisableWrite
* 功能:	将nandflash的WP置低，使写操作不可以进行，即进入写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_DisableWrite(void)
{


}

int hyhwNandFlash_GetBlockWriteStatus(void)
{
	return 0;
}


/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_BlockWrite
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWrite( Int8* pdata, U32 block, U32 pagenum)
{

	if(TwoPlanePageProgramSupport)
	{
		return hyhwNandFlash_BlockWriteTwoPlane(pdata,block, pagenum);
	}
	else
	{
		return hyhwNandFlash_BlockWriteNormal(pdata,block, pagenum);
	}
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_BlockWriteNormal
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteNormal( Int8 * pdata, U32 block, U32 pagenum)
{
	U32 i;
	U32 flag;
	U32 temp = 0;
	
	block = (block <<(gtnandInfo.pagesPerBlockBit));
	
	for( i = 0; i < pagenum; i ++, pdata += gtnandInfo.pageSize )
	{
		flag = hyhwNandFlash_PageWrite( pdata, i+block, gtnandInfo.pageSize );
		
		if(flag == HY_ERROR)
		{
			return HY_ERROR; 
		}
	}
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_BlockWriteTwoPlane
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteTwoPlane( Int8* pdata, U32 block, U32 pageNum)
{
	U16 twoPlanePageNum;
	U32 i,status;
	twoPlanePageNum = pageNum>>1;
	for(i=0;i<twoPlanePageNum;i++)
	{
		status = hyhwNandFlash_TwoPlanePageWrite( (Int8*)(pdata+( gtnandInfo.pageSize * i )),(Int8*)(pdata+ (gtnandInfo.pageSize * i) +( gtnandInfo.blockSize >> 1 )) 
												, (block*(1<< gtnandInfo.pagesPerBlockBit)+i),(block* (1<< gtnandInfo.pagesPerBlockBit)+(1<<(gtnandInfo.pagesPerBlockBit-1))+i));
		if( status == HY_ERROR )
		{
			return status;
		}	
	}

	return status;
}
	

/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_TwoPlanePageWrite
* 功能:	向指定的page中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据
*		page1----指定的page号
*		page2----指定的page号
* 返回:	HY_OK
*		HY_ERROR
* 注意：该函数目前只能支持2K的pagesize，page1和page2在紧临的2个物理block中，page1
*       在前一个（偶数）block，page2在后一个(奇数)，两者在block中的偏移量是相同的，
*       也就是说，page2－page1 ＝ blocksize
*       该函数不支持双片叠的flash，因为这种类型的flash，之前就已经实现了轮流写的功能
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_TwoPlanePageWrite( Int8 * pdata1,Int8 * pdata2, U32 page1, U32 page2)
{
    U32 data;
    U32 i,cycle,cs,temp;
  	U32* pRsData;
    U32 writeCycleTime;
	//对于支持two-plane pageprogram的flash，会将2个block组合成一个logic block,以下计算的是logic block address
	if( (TwoPlanePageProgramSupport == FALSE)||((page2-page1) != (1<<(gtnandInfo.pagesPerBlockBit-1))))
	{
		return HY_ERROR;
	}
	
	
   	//protect 放在nand前端（0～1M），所以不用考虑小于start的情况，但是对于双片的情况，program区都需要考虑
	if(( ( page1 >= protectAndReserv.programAreapageStart )&&( page1 < protectAndReserv.programAreapageEnd)) || ( (page1 < protectAndReserv.protectPageEnd )) )
	{
		return HY_ERROR;
	}
	
	cs = 0;
	while(1)
	{
		if( page1 < numberOfPagesInBank )
		{
			break;
		}
		page1 -= numberOfPagesInBank;
		page2 -= numberOfPagesInBank;
		cs ++;
	}
	
    /**/
	wLock_NandFlash();
    hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
   		
   	/* Write the sequential data input command */
    NFC_CMD_REGISTER = 0x80;
    /* Write the address */
   
    NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page1&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page1>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page1>>16)&0xFF);
   	
	
	pRsData = (U32*)pdata1;

	temp = 32;//对于512字节ecc时，分32次传输，每次16字节
	writeCycleTime = gtnandInfo.pageSize>>9;//总共需要分几次512字节进行写。	
	__asm
	    {	
	    	mov		cycle,#0
	    	
	   	twoPlaneWriteCycle1: 
			mov 	r6,#0
			str 	r6,[0xf005b00c]	//ecc start
			ldr     r6,[0xf005b000]
			orr     r6,r6,#4
			str     r6,[0xf005b000]
       	
       		mov 	i,#0
       	twoPlaneUpWord1:	
       		cmp 	i,temp
       		bcs twoPlaneDownWord1
       		
       		ldmia pRsData!, {r6-r9} 
            
            str r6,  [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
            str r7,  [0xf00b0010]
            str r8,  [0xf00b0010]
            str r9,  [0xf00b0010]
	    	add      i,i,#1
			b		 twoPlaneUpWord1 	
	    twoPlaneDownWord1:
		/* ecc  get */
								//here may set a time delay
			ldr r6,	 [0xf005b094]	/*judge that is ecc encode complete*/
			tst r6,	 #0x80000
			beq      twoPlaneDownWord1		//不相等，则跳转
			
			mov r9, #0xf005b010	/* write ecc code */
			ldmia r9!, {r6-r8} 
			    	
	    	ldr r9,[0xf005b000]
	    	bic r9,r9,#7		//清除最后三位	
			str r9,[0xf005b000]

	    	
	        str r6, [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
	        str r7, [0xf00b0010]
	        str r8, [0xf00b0010]
			str r8, [0xf00b0010]
			
			add		cycle,cycle,#1
			cmp      cycle,writeCycleTime
			bcc      twoPlaneWriteCycle1		//如果小于，则跳转


		}
			

	 /* Write the program command */
	NFC_CMD_REGISTER = 0x11;
	hyhwNand_delayUs(50); /*  1us  delay */

/*step 2*/	
	/* Write the sequential data input command */
    NFC_CMD_REGISTER = 0x81;

    /* Write the address */
    NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page2&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page2>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page2>>16)&0xFF);
   	
	
		pRsData = (U32*)pdata2;

		__asm
	    {	
	    	mov		cycle,#0
	    	
	   	twoPlaneWriteCycle2: 
			mov 	r6,#0
			str 	r6,[0xf005b00c]	//ecc start
			ldr     r6,[0xf005b000]
			orr     r6,r6,#4
			str     r6,[0xf005b000]
       	
       		mov 	i,#0
       	twoPlaneUpWord2:	
       		cmp 	i,temp
       		bcs twoPlaneDownWord2
       		
       		ldmia pRsData!, {r6-r9} 
            
            str r6,  [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
            str r7,  [0xf00b0010]
            str r8,  [0xf00b0010]
            str r9,  [0xf00b0010]
	    	add      i,i,#1
			b		 twoPlaneUpWord2 	
	    twoPlaneDownWord2:
		/* ecc  get */
								//here may set a time delay
			ldr r6,	 [0xf005b094]	/*judge that is ecc encode complete*/
			tst r6,	 #0x80000
			beq      twoPlaneDownWord2		//不相等，则跳转
			
			mov r9, #0xf005b010	/* write ecc code */
			ldmia r9!, {r6-r8} 
			    	
	    	ldr r9,[0xf005b000]
	    	bic r9,r9,#7		//清除最后三位	
			str r9,[0xf005b000]

	    	
	        str r6, [0xf00b0010] /* 这句对硬件来说，相当于写了4个bytes */
	        str r7, [0xf00b0010]
	        str r8, [0xf00b0010]
			str r8, [0xf00b0010]
			
			add		cycle,cycle,#1
			cmp      cycle,writeCycleTime
			bcc      twoPlaneWriteCycle2		//如果小于，则跳转


		}
	
	
	
	
	 /* Write the program command */
	NFC_CMD_REGISTER = 0x10;
	/*it  must be a long wait, but I don't know how long I waiting  until program finished */
	hyhwNand_delayUs( 100 );	/* program time = 700 us */
	
	if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
	{
		hyhwNand_selectCs( cs, DISABLE );
		unLock_NandFlash();
		return HY_ERROR;
	}

	/////////////////////////////////////////////////
	///////将写入的数据读回判断是否正确//////////////
	/////////////////////////////////////////////////
 //step1	
 	/* Write the read1 command, pointer set to region A */
    NFC_CMD_REGISTER = 0x00;
    /* Write the address */
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page1&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page1>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page1>>16)&0xFF);
	
	NFC_CMD_REGISTER = 0x30;
	hyhwNand_delayUs( 50 );
	
	temp = 512/4;;
	/*边读边ECC校验*/
	for( cycle = 0; cycle <  (gtnandInfo.pageSize>>9); cycle ++ )
	{
		hyhwEcc_ecc4DecodeStart();
		for( i = 0; i < temp; i ++ )
		{
			data = NFC_WORD_DATA_REGISTER;
			
		}

		if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
		{
			hyhwNand_selectCs( cs, DISABLE );/*cs disable*/
			unLock_NandFlash();
			return HY_ERROR;
		}
	
		/* do error check,if error num large than 4, return hy_error */
		if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
		{
			hyhwNand_selectCs( cs, DISABLE );/*cs disable*/
			unLock_NandFlash();
			return HY_ERROR;
		}
		data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
	}
//step2	
 	/* Write the read1 command, pointer set to region A */
    NFC_CMD_REGISTER = 0x00;
    /* Write the address */
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = 0;
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (page2&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page2>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((page2>>16)&0xFF);
	NFC_CMD_REGISTER = 0x30;
	hyhwNand_delayUs( 50 );
	
	/*边读边ECC校验*/
	for( cycle = 0; cycle <  (gtnandInfo.pageSize>>9); cycle ++ )
	{
		hyhwEcc_ecc4DecodeStart();
		for( i = 0; i < temp; i ++ )
		{
			data = NFC_WORD_DATA_REGISTER;
			
		}
		if(hyhwEcc_waitDecodingCompleted()==ecc_uncompleted)
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
	
		/* do error check,if error num large than 4, return hy_error */
		if( MLC_ECC4_ERROR_NUMBER_REGISTER >4 )
		{
			hyhwNand_selectCs( cs, DISABLE );
			unLock_NandFlash();
			return HY_ERROR;
		}
		data = NFC_WORD_DATA_REGISTER;/*for corresponding with the data write*/
	}	
 	
 	hyhwNand_selectCs( cs, DISABLE );
 	
 	unLock_NandFlash();

	return HY_OK;
   
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_twoPageModeBlockErase
* 功能:	将指定的block擦除
* 参数:	block----需要擦除的block号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_TwoPageModeBlockErase( U32 block )
{
	U32 blockTemp,cs,tryTime,i;
	
	
	
	//protect 放在nand前端（0～1M），所以不用考虑小于start的情况，但是对于双片的情况，program区都需要考虑
	if( ( block < protectAndReserv.protectblockEnd ) || (( block >= protectAndReserv.programAreablockStart )&&(block < protectAndReserv.programAreablockEnd )) )
	{
		return HY_ERROR;
	}
	
	cs = 0;
	while(1)
	{
		if( block < numberOfBlocksInBank )
		{
			break;
		}
		block -= numberOfBlocksInBank;
		cs ++;
	}
  
	blockTemp = (block <<(gtnandInfo.pagesPerBlockBit));
	wLock_NandFlash();	
	hyhwNand_selectCs( cs, ENABLE );/*cs enable*/
	/* Write the auto block erase setup command */
    NFC_CMD_REGISTER = 0x60;
	/* Write the address */
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (blockTemp&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((blockTemp>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((blockTemp>>16)&0xFF);
    /* Write the erase command */
    hyhwNand_delayUs( 40 );
    blockTemp = ((( block << 1 )+ 1 ) << ( gtnandInfo.pagesPerBlockBit - 1 ));
    
    NFC_CMD_REGISTER = 0x60;
	/* Write the address */
	NFC_SINGLE_ADDR_CYCLE_REGISTER = (blockTemp&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((blockTemp>>8)&0xFF);
	NFC_SINGLE_ADDR_CYCLE_REGISTER = ((blockTemp>>16)&0xFF);

	NFC_CMD_REGISTER = 0xD0;

	hyhwNand_delayUs(200);
   		
 	if( hyhwNand_waitingForStatusReady() == NandFlashProcess_fall )
   	{
   		hyhwNand_selectCs( cs, DISABLE );/*cs disable*/
   		unLock_NandFlash();
		return HY_ERROR;
   	}		
	
	hyhwNand_selectCs( cs, DISABLE );/*cs disable*/
	unLock_NandFlash();
    return HY_OK; 
 
}

