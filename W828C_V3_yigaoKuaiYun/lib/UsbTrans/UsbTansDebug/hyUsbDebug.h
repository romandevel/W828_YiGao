#ifndef HY_DEBUG_H_
#define HY_DEBUG_H_

#define USB_DEBUG

#ifdef  USB_DEBUG 
     #define  USBTRACE(x)   hyUsbPrintf x
     #define  USBMSG(x)   hyUsbMessage(x)
#else
     #define USBTRACE(x)
     #define  USBMSG(x)
#endif

#ifdef NDEBUG
	#define hyDebug(x)  
#else
    #define hyDebug(x)   x
#endif    


/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_softInit
* 功能:	初始化Usb传输
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_softInit(void);

/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_Init
* 功能:	初始化Usb传输(硬件)
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_Init(void);

/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_Init
* 功能:	检测是否可以以USB通道发送数据
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyUsbTrans_DetectStart(void);

/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_isRunning
* 功能:	判断是否进入USB传输模式状态
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_isRunning(void);



/*-----------------------------------------------------------------------------
* 函数:	hyUsbPrintf
* 功能:	hyctron的printf函数
* 参数:	format	-- 格式控制字符串
*		...		-- 可变参数列表
* 返回:	print的字符个数  
*----------------------------------------------------------------------------*/
int hyUsbPrintf(const char *format, ...);

/*-----------------------------------------------------------------------------
* 函数:	hyUsbMessage
* 功能:	hyctron的Message函数, 不需要格式控制，仅仅是输出字符串
* 参数:	msg	-- 需要输出的字符串
* 返回:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessage(const char *msg);

/*-----------------------------------------------------------------------------
* 函数:	hyUsbData
* 功能:	hyctron的data发送函数, 
* 参数:	pdata	-- 数据指针
		len		-- 数据长度
* 返回:	none  
*----------------------------------------------------------------------------*/
void hyUsbData(char *pdata, int len);


#endif /* TM_HW_VH_DEBUG_H_*/
