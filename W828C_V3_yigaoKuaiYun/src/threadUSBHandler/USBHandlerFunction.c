/*************************************************************************
 * INCLUDES
 *************************************************************************/

 /*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "vhChip.h"

//#include "hyhwPmu.h"
//#include "hyhwClockSet.h"
//#include "hyhwDma.h"
#include "hyhwNandflash.h"
#include "hyhwIntCtrl.h"

//#define HYHW_MEMORY_SD_MMC
#ifdef HYHW_MEMORY_SD_MMC
	#include "hyhwSdmmc.h"
	#include "hyswSdmmc.h"
#endif

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "ioctl.h"

#include "HSA_API.h"
#include "AbstractFileSystem.h"
#include "BoardSupportPackage.h"

#include "hyhwGpio.h"


#define USB_MAPPING_BLOCK0_TO_SDRAM	//USB 时BLOCK0被映射到Sdram
#define USB_FILE_SYS

#define USB_USE_MAX_BUF

/*************************************************************************
 * DEFINES
 *************************************************************************/

#define USB_VOLUME              ("C")

#define USB_RUN_HIGH_SPEED      192, 96, 48, 48
#define USB_RUN_LOW_SPEED        48, 48, 24, 24

#define SUSPEND_COUNT           (40)//40*50 = 2秒，连续2秒检测到suspend时，就退出USB
#define ADAPTER_COUNT           (200)//200*50 = 10秒，10秒还未进入USB，就退出USB

#define USB_LOWSPEED_IDLE_CNT   (12000)	//12000*50 = 10分钟后进入低速
#define SD_DETECT_COUNT         (10)//10*50 = 0.5秒，连续检测到SD卡插入，则认为已插入SD卡

#define FLUSHING_PERIOD         (10)//10*50 = 0.5秒，USB连续空闲0.5秒后，回写文件系统信息

#define USB_NAND_BLOOK_POOL_MANAGER_SIZE		(2*1024*1024)

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
 
/*************************************************************************
 * GLOBALS
 *************************************************************************/
UInt8  gScsiApplnScsiMaxLunFlag;

#pragma arm section zidata="usb_zidata", rwdata="usb_zidata"
/*************************************************************************
 * STATICS
 *************************************************************************/
static BOOL   sbUSB_AdapterFlag;
static BOOL   sbUSBspeedDetectFlag;
static UInt8  su8USB_AdapterCnt;
static Int8	  susbTransferStatus;
static UInt16 su16USB_IdleCnt;

#ifdef HYHW_MEMORY_SD_MMC
	static Bool   sbSdInit;
	static UInt8  su8SdDetectCnt;
	static UInt8  su8SdSdLastStatus;
#endif

#pragma arm section //zidata="usb_zidata", rwdata="usb_zidata"

static void USBHandler_InitialiseHardware(void);
static void USBHandler_DeInitialiseHardware(void);
static void USBHandler_DetectSDMMC(void);

void USBHandler_USB_DeInit(int volumeHandle);

void (* usb_UpdataSpeedDirect)(int clk);

/*************************************************************************
 * EXTERNALS
 *************************************************************************/
extern Int8 gtmUsbTransferInProgressRx;
extern Int8 gtmUsbTransferInProgressTx;
extern BOOL gbSuspendDetected;

extern void fnUSB_DevInit(void);
extern int  hyudsDcd_Connected(void);

extern void mappingTable_ReadTabletoBuffer(int value);
//extern void nandLogicDrv_SetBlockPool(void* buffer, int size);
extern void nandLogicDrv_InvalidBlockPool(void);

extern void *hsaUsbTransport_GetBuffer(void);
extern int  hsaUsbTransport_GetBufferSize(void);

void USBHandler_ClearAdapterCnt(void);

extern hyudsDcd_UsbDispatchReadStatus(void);

void usb_UpdataSpeed_Config(void(*pHandle)(int clk))
{
	usb_UpdataSpeedDirect = pHandle;
}

void USBHandler_StartDetectUSBStatus(void)
{
#if 1
	//hyhwInt_ConfigUsbSuspend_ISR(hyudsDcd_UsbDispatchReadStatus);
	/* 初始化定时器 */
	hyhwTimer_countDisable(1);
	hyhwTimer_intDisable(1);
	hyhwInt_ConfigTimer1_ISR(hyudsDcd_UsbDispatchReadStatus);
	hyhwTimer_SetFrequency(1, 500);
	hyhwTimer_intEnable(1);
	hyhwTimer_countEnable(1);
#endif
}
void USBHandler_StopDetectUSBStatus(void)
{
#if 1
	//hyhwInt_ConfigUsbSuspend_ISR(NULL);
	hyhwTimer_countDisable(1);
	hyhwTimer_intDisable(1);
	hyhwInt_ConfigTimer1_ISR(NULL);
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_USBInit
* 功能:	进入USB状态时，对USB进行初始化(软硬件)
* 参数:	none
* 返回:	初始化成功-----返回文件系统handle("C"盘符的handle)
*       初始化失败-----HY_ERROR
*----------------------------------------------------------------------------*/
int USBHandler_USBInit(void)
{
    int volumeHandle = -1;
    
    USB_InitMemory_buf();
    sbUSB_AdapterFlag   = True;
    su16USB_IdleCnt     = 0;
    sbUSBspeedDetectFlag  = True;
    su8USB_AdapterCnt   = 0;
    gScsiApplnScsiMaxLunFlag = 0;
    
    /* Initialise Hardware */
    USBHandler_InitialiseHardware();

	/* 关闭nand 抽象层，回写 */
	hyswNandFlash_Close();
	
	/* 更新FAT2 */
	volumeHandle = open("C", AFS_WRITE);
	ioctl(volumeHandle, UPDATE_FAT2);
	close(volumeHandle);
	volumeHandle = -1;

	/* 关闭nand 抽象层，回写 */
	hyswNandFlash_Close();
	
	/* 将mapping table 读入到ram中， 加快仅仅只有上传时的速度 */
	mappingTable_ReadTabletoBuffer(1);
	#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
	{
		char * usbTransBuf;int usbTransBufSize;
		//usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
		//usbTransBufSize = hsaUsbTransport_GetBufferSize();
		usbTransBuf = (char *)hsaSdramReusableMemoryPool();
		//usbTransBufSize = hsaSdramReusableMemoryPool_Size();
		usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
		#ifdef USB_USE_MAX_BUF
		nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
									(char *)usbTransBuf,
									usbTransBufSize);
		#else
		nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
									(char *)hsaUsbTransport_GetBuffer(),
									hsaUsbTransport_GetBufferSize());
		#endif
	}
	#endif

#ifdef USB_FILE_SYS
    /* Open volume */
    volumeHandle = AbstractFileSystem_Open(USB_VOLUME, AFS_RDWR);
    if (volumeHandle < 0)
    {//文件系统错误，直接返回错误
        return ((int)HY_ERROR);
    }
#else
	hyswNandFlash_Open();
	volumeHandle = 0;
#endif    
    
#ifdef HYHW_MEMORY_SD_MMC
 	hyhwDma_Init();
 	
	/* Scsi drivers initialisation - SD card*/
    //hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Dma, EnableOn);
    //hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Ssi, EnableOn);
	
	su8SdDetectCnt = 0;
	su8SdSdLastStatus = 0;
	sbSdInit= False;
	/* 在这个函数中会检测是否有卡 */
  	if( hyswSdMmc_Init() == HY_OK)
  	{
	  	sbSdInit = True;
	  	su8SdSdLastStatus = 1;
  	}
#endif

    /* Initialise USB  */
    tmllpeScsiAppln_Initialise(volumeHandle);

    /* initialise Usb Stack */
  	fnUSB_DevInit();
  	/*	检测是否为USB线插入
  		判断条件为USB的速度检测，不十分准确
  		在USBHandler_Application()中还要以gScsiApplnScsiMaxLunFlag判断
  		当gScsiApplnScsiMaxLunFlag为1时，才真正进入USB状态
  	*/
  	if (hyudsDcd_GetSpeedDetectFlag() == FALSE)
  	{
  		//插入的是适配器
  		USBHandler_USB_DeInit(volumeHandle);
  		volumeHandle = -1;
  	}
	hyudsDcd_StopSpeedDetect();
	su8USB_AdapterCnt = 0;
	hyudsDcd_ConfigCallback(USBHandler_ClearAdapterCnt);
	susbTransferStatus = 0;

    return (volumeHandle);
}


/*-----------------------------------------------------------------------------
* 函数:	USBHandler_USB_DeInit
* 功能:	退出USB状态时，对USB进行反初始化(软硬件)
* 参数:	volumeHandle------文件系统handle("C"盘符的handle)
* 返回:	none
*----------------------------------------------------------------------------*/
void USBHandler_USB_DeInit(int volumeHandle)
{
#ifdef USB_FILE_SYS
    /* Close volume */
    AbstractFileSystem_Close( volumeHandle );
#else
	hyswNandFlash_Close();
#endif
    
	/* 关闭nand 抽象层，回写 */
	hyswNandFlash_Close();
	#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
	nandLogicDrv_InvalidBlockPool();
	mappingTable_ClearTableInBuffer();
	
	mappingTable_ReadTabletoBuffer(1);
	nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
								(char *)hsaUsbTransport_GetBuffer(),
								hsaUsbTransport_GetBufferSize());
	#endif

    /* Restore hard ware initial settings */
    USBHandler_DeInitialiseHardware();
}

#pragma O0
#pragma arm section rodata="usb_function", code="usb_function"
/*-----------------------------------------------------------------------------
* 函数:	USBHandler_Application
* 功能:	usb 状态下的调用的功能函数
* 参数:	volumeHandle------文件系统handle("C"盘符的handle)
* 返回:	none
*----------------------------------------------------------------------------*/
int USBHandler_Application(int *pvolumeHandle)
{
    HSA_USBCommunicationStatus_en usbStatus;
    Bool connect_status = True;
    
    //扫描SD卡的热拔插
    USBHandler_DetectSDMMC();
    
    /* 周期性地调用，如果返回True，
       表示已经连续地空闲了16次，可以调用flash close 以回写 */
    if ( gScsiApplnScsiMaxLunFlag == 1 )
    {
        UInt32 USB_DeviceStatus;
        
    #if 1
		//检查PC端是否断开USB连接(软断开)
       if (gbSuspendDetected == TRUE)
        {
			//退出USB
			connect_status = False;
        }
        else
    #endif
        {
            /* Process transfer transitions */
            if ( gtmUsbTransferInProgressRx )
            {
                usbStatus = HSA_USB_RECEIVING_UCS;
            }
            else if ( gtmUsbTransferInProgressTx )
            {
                usbStatus = HSA_USB_TRANSMITTING_UCS;
            }
            else
            {
                usbStatus = HSA_USB_IDLE_UCS;
            }
            if (susbTransferStatus == 1)
            {
            	if (usbStatus == HSA_USB_IDLE_UCS)
            	{
            		//进入空闲状态，启动检测USB设备状态
            		USBHandler_StartDetectUSBStatus();
            	}
            }
            else
            {
            	if (usbStatus != HSA_USB_IDLE_UCS)
            	{
            		//进入数据传输状态，停止检测USB设备状态
            		USBHandler_StopDetectUSBStatus();
            	}
            }
            
            /* Reset Transfer in progress flag */
            gtmUsbTransferInProgressRx = False;
            gtmUsbTransferInProgressTx = False;
            
            /* Reset counter if we are transmitting */
            if( usbStatus != HSA_USB_IDLE_UCS )
            {
                susbTransferStatus = 1;
                if (su16USB_IdleCnt >= USB_LOWSPEED_IDLE_CNT)
                {//如果已经进入低速状态，那么就提高速度
    				//hyhwSysClk_SetFree(USB_RUN_HIGH_SPEED,PLL1);
    			  	//hyhwClk_SetSsiClk(SSICLKMAX);
    			  	if (usb_UpdataSpeedDirect != NULL)
    			  	{
    			  		usb_UpdataSpeedDirect(192);
    			  	}
                }
                su16USB_IdleCnt = 0;
            }
    		else //HSA_USB_IDLE_UCS
    		{
                susbTransferStatus = 0;
                if (su16USB_IdleCnt < USB_LOWSPEED_IDLE_CNT)
                {
    	            su16USB_IdleCnt++;
    	            switch(su16USB_IdleCnt)
    	            {
    	                case FLUSHING_PERIOD:
                            /* Flush cached contents */
                        #ifdef USB_FILE_SYS
                            AbstractFileSystem_Close(*pvolumeHandle);
                            hyswNandFlash_Close();
                          /* 将usb mapping table 重新指向 */
                            mappingTable_ReadTabletoBuffer(0);
							#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
							{
								char * usbTransBuf;int usbTransBufSize;
								//usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
								//usbTransBufSize = hsaUsbTransport_GetBufferSize();
								usbTransBuf = (char *)hsaSdramReusableMemoryPool();
								//usbTransBufSize = hsaSdramReusableMemoryPool_Size();
								usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
								#ifdef USB_USE_MAX_BUF
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)usbTransBuf,
															usbTransBufSize);
								#else
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)hsaUsbTransport_GetBuffer(),
															hsaUsbTransport_GetBufferSize());
								#endif
							}
							#endif
                            *pvolumeHandle = AbstractFileSystem_Open(USB_VOLUME, AFS_RDWR);
                        #else
                            hyswNandFlash_Close();
                            hyswNandFlash_Open();
                        #endif
#if 0
                            /* 将usb mapping table 重新指向 */
                            mappingTable_ReadTabletoBuffer(0);
							#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
							{
								char * usbTransBuf;int usbTransBufSize;
								usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
								//usbTransBufSize = hsaUsbTransport_GetBufferSize();
								usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
								#ifdef USB_USE_MAX_BUF
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)usbTransBuf,
															usbTransBufSize);
								#else
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)hsaUsbTransport_GetBuffer(),
															hsaUsbTransport_GetBufferSize());
								#endif
							}
							#endif
#endif
    	                    break;
    	                case USB_LOWSPEED_IDLE_CNT:
        					//idle计时到后，降低速度
        					//hyhwSysClk_SetFree(USB_RUN_LOW_SPEED,PLL1);
        				 	//hyhwClk_SetSsiClk(SSICLKMAX);        				 
		    			  	if (usb_UpdataSpeedDirect != NULL)
		    			  	{
		    			  		usb_UpdataSpeedDirect(0);
		    			  	}
    	                    break;
    	                default:
    	                    break;
    	            }
    	        }
            }//if( usbStatus != HSA_USB_IDLE_UCS )
        }//if ((USB_DeviceStatus & 0x80) != 0)
    }
    else //if ( gScsiApplnScsiMaxLunFlag == 0)
    {
        /* 如果插入的是适配器，则gScsiApplnScsiMaxLunFlag不会被置True
           等待一段时间后，如果还是等不到True，则认为是适配器，推出USB状态 
        */
        if (sbUSB_AdapterFlag == True)
        {
            su8USB_AdapterCnt++;
            if (su8USB_AdapterCnt >= ADAPTER_COUNT)
            {//退出USB
                gScsiApplnScsiMaxLunFlag = 2;
                connect_status = False;
            }
        }
    }//if ( gScsiApplnScsiMaxLunFlag == 1 )
    
    /* Check if USB still connected ，Automatic USB connection */
    if ( !(hyudsDcd_Connected() & 1))
    {
        connect_status = False; 
    	#if 0
    	{
    	    //延时，保证回写文件系统信息
        	int i;
        	i=0x007FFFFF;
        	while(i--);
        	//syssleep(100);
    	}
    	#endif
    }
    
    return (int)(connect_status);
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_DetectSDMMC
* 功能:	usb 状态下扫描SD卡的热拔插
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void USBHandler_DetectSDMMC(void)
{
#ifdef HYHW_MEMORY_SD_MMC
    int SdDetected, flag;
	
    //扫描SD卡的热拔插
    flag = 0;
    SdDetected = hyhwCardDetect();
    if (SdDetected == su8SdSdLastStatus)
    {
        su8SdDetectCnt++;
        if (su8SdDetectCnt >= SD_DETECT_COUNT)
        {//滤波
            if (SdDetected)
            {
                flag = 1;
            }
            
            su8SdDetectCnt = SD_DETECT_COUNT;
        }
    }
    else
    {
        su8SdDetectCnt = 0;
        su8SdSdLastStatus = SdDetected;
    }

    if (flag)
    {
        if(sbSdInit == False)
        {
            if( hyswSdMmc_Init() == 0)
            {//初始化成功
                tmllpeScsiAppln_SDInitialise(1);
                sbSdInit = True;
            }
        }
    }
    else
    {
        sbSdInit = False;
    }
#endif
}
#pragma arm section	/* rodata="usb_function", code="usb_function" */


extern void USB_InitExeRam_Instruction(void);
extern void USB_InitExeRam_Data(void);
/*-----------------------------------------------------------------------------
* 函数:	USBHandler_InitialiseHardware
* 功能:	初始化USB设置
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void USBHandler_InitialiseHardware( void )
{
    /* USB状态下关闭Codec，降低功耗 */
//    hyhwCodec_Configuration(hwCodec_Off);

    /* 初始化 usb 运行时的RAM空间，由scatter定义分配，overlay，所以这里必须初始化。
       RO 区域和RW区域的性质一样且在一起，都是从一个load区copy到image区，所以，如果scatter中
       包含了RO和RW区域，则copy时会和RW区域一起拷贝。
       overlay manager 是在overlay.s 中 */ 
//    USB_InitExeRam_Instruction();
//    USB_InitExeRam_Data();

    /* dma 这里是重复调用了，初始化应打开了 
       hyhwDma_Init 中会打开电源 */
    hyhwDma_Init();

    // 进入USB时，NandFlash允许写
    hyhwNandFlash_EnableWrite();

    /* 通知nandflash硬件驱动进入usb状态，在usb状态下， nandflash硬件驱动会使用DMA */
//    hyhwNandflash_InformUsbStatus(1);

    /* 将系统时钟设置在 USB 的运行状态下，usb2.0速度不能太慢  */
//    hyhwSysClk_SetFree(USB_RUN_HIGH_SPEED,PLL1);
//    hyhwClk_SetSsiClk(SSICLKMAX);
  	if (usb_UpdataSpeedDirect != NULL)
  	{
  		usb_UpdataSpeedDirect(192);
  	}

    /* Enable USB clocks */
//    hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Udc, EnableOn);
}


/*-----------------------------------------------------------------------------
* 函数:	USBHandler_DeInitialiseHardware
* 功能:	反初始化USB设置
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
static void USBHandler_DeInitialiseHardware( void )
{
    /* 为省电，Disable USB clocks */
//    hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Udc, EnableOff);

//	hyhwSysClk_SetFree(USB_RUN_LOW_SPEED,PLL1);
//	hyhwClk_SetSsiClk(SSICLKMAX);
	
	hyudsDcd_HwDeinit(); //080827
	
  	if (usb_UpdataSpeedDirect != NULL)
  	{
  		usb_UpdataSpeedDirect(0);
  	}
	
	/* 通知nandflash硬件驱动退出usb状态，在usb状态下， nandflash硬件驱动会使用DMA */
//	hyhwNandflash_InformUsbStatus(0);
	
	/* 退出USB时，NandFlash禁止写 */
	hyhwNandFlash_DisableWrite();
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_TransferStatus
* 功能:	获得USB传输状态
* 参数:	none
* 返回:	USB传输状态  1----正在传输  0----空闲状态
*----------------------------------------------------------------------------*/
int USBHandler_TransferStatus(void)
{
	return (int)susbTransferStatus;
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_SetAdapterFlag
* 功能:	设置USB时，是否进入适配器判断
* 参数:	adapterFlag-----1:允许，0:禁止进入适配器判断
* 返回:	none
*----------------------------------------------------------------------------*/
void USBHandler_SetAdapterFlag(Bool adapterFlag)
{
	sbUSB_AdapterFlag = (Bool)adapterFlag;
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_ClearAdapterCnt
* 功能:	清除适配器超时判断计数，该函数在收到PC的reset命令时调用
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void USBHandler_ClearAdapterCnt(void)
{
	su8USB_AdapterCnt = 0;
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_USBConnect
* 功能:	检测USB Vbus是否有效
* 参数:	none
* 返回:	1-----USB Vbus有效，即usb线插入
*       0-----USB Vbus无效，即usb线未插入
*----------------------------------------------------------------------------*/
int USBHandler_USBConnect(void)
{
    return hyudsDcd_Connected();
//	return 1;
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_GetScsiFlag
* 功能:	判断设备连接状态
* 参数:	none
* 返回:	1-----USB Vbus有效，即usb线插入
*       0-----USB Vbus无效，即usb线未插入
*----------------------------------------------------------------------------*/
int USBHandler_GetScsiFlag(void)
{
    return gScsiApplnScsiMaxLunFlag;
}

/*-----------------------------------------------------------------------------
* 函数:	USBHandler_SetScsiFlag
* 功能:	设置设备器状态
* 参数:	none
* 返回:	1-----USB Vbus有效，即usb线插入
*       0-----USB Vbus无效，即usb线未插入
*----------------------------------------------------------------------------*/
void USBHandler_SetScsiFlag(int value)
{
    gScsiApplnScsiMaxLunFlag = value;
}

/*-----------------------------------------------------------------------------
* 函数:	USBSlave_Enable
* 功能:	使slave enable
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void USBSlave_Enable(int value)
{

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUsbHost_PowerCtrl
* 功能:	使slave enable
* 参数:	value 1 对外供电 0 不对外供电
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwUsbHost_PowerCtrl(int value)
{
}
