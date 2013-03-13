


#ifndef HY_HW_NANDFLASH_H_
#define HY_HW_NANDFLASH_H_


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/

#define NAND_SIZE_UNIT_BIT		(20)//M Bytes单位的bit数

/* NandFlash type 
*/
typedef enum
{
    NAND_16K_512B = 0,
    NAND_128K_2K,
    NAND_32K_512B,
    NAND_512K_2K,
    NAND_256K_2K_MLC,	/* 其实是128K_2K, 由于奇偶页在不同的block上，所以block size等于大了一倍 */
    NAND_256K_2K,
    NAND_512K_2K_MLC,	/* 其实是256K_2K, 由于奇偶页在不同的block上，所以block size等于大了一倍 */
    NAND_512K_2K_JOINT,	/* 其实是256K_2K, 相邻的2个256k block组合为一个512k的block */
    NAND_512K_4K,
    NAND_1M_4K_JOINT,
    NAND_UNKNOWN
} eNAND_TYPE;

/* NandFlash class 
*/
typedef enum
{
    SLC ,
    MLC
} eNAND_CLASS;

typedef enum
{
    DISABLE ,
    ENABLE
} eNAND_SELECT;

typedef enum
{
    CS0 ,
    CS1 ,
    CS2 ,
	CS3 ,
	CS4 ,
    CS5 ,
    CS6 ,
	CS7 
} eNAND_CS;

typedef enum
{
    NandFlashProcess_success,
    NandFlashProcess_fall
} eNAND_PROCESS;



/* NandFlash bank 设置 */
typedef struct
{
    int          Gpio_nCE;		/* Gpio mask bit pattern , corresponding bit equals 1*/
    int          Gpio_nWP;		/* Gpio mask bit pattern , corresponding bit equals 1*/
    unsigned int SMCBank_DATA;
    unsigned int SMCBank_CLE;
    unsigned int SMCBank_ALE;    
    int          bankNo;

} NandFlashBank_st;

/* nandflash的存储设置 */
typedef struct
{
    NandFlashBank_st bank0;
    NandFlashBank_st bank1;

    unsigned int	reservedSize;

} InternalStorageMedia_st;





typedef volatile struct 
{
	U16	setUp;
	U16	pulseWidth;
	U16	hold;
	U32	regValue;
} tNAND_IO_CYCLE;

typedef volatile struct 
{
	U8	makerCode;
	U8	deviceCode;
	U8	extendedIDCode;
	U8	extendedIDCode_e1;
	U8	extendedIDCode_e2;
	U32	nandFlashSize;
	eNAND_TYPE nandFlashType;
} tNAND_ID;

/* nandflash 的nandPageSize 和 nandPagesPerBlocksBits 
   现在由于假定两块nand的 type是一样的，所以不需要使用数组
   nandPagesPerBlockBits 是使用2的指数来表示的每block 的page 数 
	type				pagesPerBlock	nandPagesPerBlockBits 
	NAND_16K_512B			32				5				
	NAND_32K_512B			64				6
	NAND_128K_2KB			64				6
	NAND_256K_2K			128				7		这种型号是虚拟的
	NAND_512K_2KB			256				8	*/     

typedef volatile struct 
{
	
	
	/* 下面是一些常数，在初始化时确定，为了计算方便，*/
	U32		blockSize;
	U16		pageSize;
	
	U16		blockSizeBit;
	U16		pageSizeBit;	/*	pageSizeBit 引入是为了避免除法，以指数表示的pageSize	
										pageSize 现在仅有2种， 512B和 2KB，对应pageSizeBit 为 9 和 11*/
	U16		pagesPerBlockBit;
	
	U32		size;			/* 将两个bank看作是一体时的总的容量，以M为单位 */
	eNAND_TYPE	type;
	
	U16		readWaitTime;	/* 大部分nandflash 读命令发完之后的延时为24ms，有一种是50ms
										所以需要这个数据进行调整 */
	/*	从最底层再保护一次写有程序的那些block对应的page，防止当usb信号有干扰时，误读了逻辑层的mapping table
		从而引起对保护的block的写入，必须使用unsigned int类型的数据来表示page index 
		逻辑为  protectPageStart <= page < protectPageEnd 
		注意这个page是真正的物理page  */
	unsigned int		protectPageStart;	
	unsigned int		protectPageEnd;	

	/*	注意对于对于NAND_256K_2K_MLC，仍是逻辑page，除以2之后，才是物理page
		而其他类型的flash，都是物理page */
	unsigned int		reservePageStart;	
	unsigned int		reservePageEnd;	

} tNAND_INFO;


typedef volatile struct 
{

/*程序区域、保护区域和资源区域*/	

	U32		protectPageStart;
	U32		protectPageEnd;
	U32		protectblockStart;
	U32		protectblockEnd;	
	
	U32		programAreapageStart;
	U32		programAreapageEnd;
	U32		programAreablockStart;
	U32		programAreablockEnd;
	
	Bool	needReadBack;
} tNAND_PROTECT_RESERV;

typedef volatile struct
{
	U32		bankNo;
	U32 	smcBankDATA;
	U32 	smcBankALE;
	U32 	smcBankCLE;
	S32		gpioNWP;
	U32		Gpio_nWP;
} tNAND_BANK;



typedef volatile struct
{
	tNAND_BANK 	bank0;
	tNAND_BANK 	bank1;
	tNAND_BANK 	bank2;
	tNAND_BANK 	bank3;
	U32			reservedSize;
} tINTERNAL_STORAGE_MEDIA;


/*---------------------------------------------------------------------------
   Global variables:
   -------------------------------------------------------------------------*/
extern  tNAND_PROTECT_RESERV			protectAndReserv;
extern	tNAND_INFO						gtnandInfo;
extern  const InternalStorageMedia_st	internalStorageMedia;

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_init
* 功能:	初始化NandFlash，获得nandflash的信息
*		系统中，仅被nandLogicDrv_Init在初始化中调用一次，所以，可以overlay在 sram
*		中，以节省sram的使用
* 参数:	none
* 返回:	none， nandflash的信息在全局变量gtnandInfo中
*----------------------------------------------------------------------------*/
eNAND_PROCESS hyhwNandFlash_Init( void );

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
U32 hyhwNandFlash_DataRead(Int8 * pdata, U32 page, U32 offset, U32 size);

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_erase
* 功能:	将指定的block擦除
* 参数:	block----需要擦除的block号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_Erase( U32 block );

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_pageWrite
* 功能:	向指定的page中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据
*		page----指定的page号
*		size----需要写入数据的宽度（byte）
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite( Int8* pdata, U32 page, U32 size );

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_pageWrite
* 功能:	向指定的page中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据
*		page----指定的page号
*		size----需要写入数据的宽度（byte）
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite_slc( Int8* pdata, U32 page, U32 offset, U32 size );

/*-----------------------------------------------------------------------------
* 函数:	hyNandflash_InformUsbStatus
* 功能:	在 usb 状态下，nandflash hardware 驱动会保护reserve区，这个函数让usb 准备函数
*		通知nandflash 硬件drv，进入了usb状态
* 参数:	usbStatus	--	1, 表示要进入usb 状态了，0 表示退出出usb 状态
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandflash_InformUsbStatus(U32 usbStatus);
/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_EnableWrite
* 功能:	将nandflash的WP置高，使写操作可以进行，即去掉写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

void hyhwNandFlash_EnableWrite(void); ///现在还没有写保护功能引脚
/*-----------------------------------------------------------------------------
* 函数:	hyNandFlash_DisableWrite
* 功能:	将nandflash的WP置低，使写操作不可以进行，即进入写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_DisableWrite(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_blockWrite
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWrite( Int8* pdata, U32 block, U32 pagenum);


/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_informUsbStatus
* 功能:	在 usb 状态下，nandflash hardware 驱动会保护reserve区，这个函数让usb 准备函数
*		通知nandflash 硬件drv，进入了usb状态
* 参数:	usbStatus	--	1, 表示要进入usb 状态了，0 表示退出出usb 状态
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandflash_InformUsbStatus(U32 usbStatus);

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_enableWrite
* 功能:	将nandflash的WP置高，使写操作可以进行，即去掉写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_EnableWrite(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_disableWrite
* 功能:	将nandflash的WP置低，使写操作不可以进行，即进入写保护状态
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_DisableWrite(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_BlockWriteNormal
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteNormal( Int8 * pdata, U32 block, U32 pagenum);
/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_BlockWriteTwoPlane
* 功能:	向指定的block中写入指定大小的数据
* 参数:	data----指针参数，需要写入数据,该数据保存在sdram中，注意在写的过程中将数据搬移到sram中
*		block----指定的block号
*		pagenum----需要写入的page数
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteTwoPlane( Int8* pdata, U32 block, U32 pagenum);
/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_twoPlanePageWrite
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
U32 hyhwNandFlash_TwoPlanePageWrite( Int8 * pdata1,Int8 * pdata2, U32 page1, U32 page2);

/*-----------------------------------------------------------------------------
* 函数:	hyhwNand_twoPageModeBlockErase
* 功能:	将指定的block擦除
* 参数:	block----需要擦除的block号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_TwoPageModeBlockErase( U32 block );


#endif /* HY_HW_NANDFLASH_H_ */