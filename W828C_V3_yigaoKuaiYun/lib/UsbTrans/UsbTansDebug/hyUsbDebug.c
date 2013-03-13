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

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "stdarg.h"
/*------------------------------------------------------------------------------
  Project include files:
------------------------------------------------------------------------------*/
#include "vhchip.h"
#include "hyUsbDebug.h"

/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/
#ifdef USB_DEBUG
#define USB_DEBUG_PRINT
#endif

#define FRMWRI_BUFSIZE		256			// 格式输出的缓存
#define SEND_BUFFER_MAX_SIZE	(10*1024)//(5*1024*1024)//send buf最大5M

int usbDebugEn = 0;
int usbDebugLockInit = 0;
char* pMsgBuf = NULL;

int usbSendBufSize = 0;
char *pUsbSendBuf = NULL;

// static function prototype
static void putOneCharInBuff (char c, char** ptr);
static int formatted_write_ (const char *format,
                  void put_one_char(char, char**),
                  char * str_pointer,
                  va_list ap);

static int hyUsbConnect_Detect(void);


/*-----------------------------------------------------------------------------
* 函数:	putOneCharInBuff
* 功能:	将一个字符写到字符串的缓存中
* 参数:	c		-- 字符
*		ptr		-- 函字符串缓存
* 返回:	none
*----------------------------------------------------------------------------*/
static void putOneCharInBuff (char c, char** ptr)
{
   *(*(char **)ptr)++ = c;
}

/*-----------------------------------------------------------------------------
* 函数:	formatted_write_
* 功能:	配合hyco的printf函数使用，将要输出的可变参数按照控制格式写入到str_pointer
*		字符串缓存中
* 参数:	format			-- 格式控制字符串，
*		put_one_char	-- 函数指针，指向实现输出一个字符的函数
*		str_pointer		-- 输出，可变参数输出到的字符串
*		ap				-- 可变参数列表
* 返回:	输入到所给定的字符串中的字符个数  
*----------------------------------------------------------------------------*/
static int formatted_write_ (const char *format,
                  void put_one_char(char, char**),
                  char * str_pointer,
                  va_list ap)
{
    char format_flag;
    int  precision;
    int  n;
    int  field_width, nr_of_chars;
    char plus_space_flag, left_adjust, l_L_modifier;
    char h_modifier, alternate_flag;
    long ulong;
    unsigned long div_factor;
    char *buf_pointer;
    char *ptr, *hex;
    char zeropad;
    char *buf;

    buf = (char *)malloc(FRMWRI_BUFSIZE);
    nr_of_chars = 0;
    for (;;)                         /* Until full format string read */
    {
       while ((format_flag = *format++) != '%')  /* Until '%' or '\0' */
       {
          if (!format_flag)
          {
            put_one_char ('\0', &str_pointer);
            free(buf);
            return (nr_of_chars);
          }
          put_one_char (format_flag, &str_pointer);
          nr_of_chars++;
       }
       if (*format == '%') /* %% prints as % */
       {
         format++;
         put_one_char('%',&str_pointer);
         nr_of_chars++;
         continue;
       }

       plus_space_flag = left_adjust = alternate_flag = zeropad = 0;
       ptr = buf_pointer = &buf[0];
       hex = "0123456789ABCDEF";
       
       /* 检查是否有前导的 '-', '+', ' ','#' or '0' 标记  */
       for (;;)
       {
          switch (*format)
          {
             case ' ':
               if (plus_space_flag)
                goto NEXT_FLAG;
             case '+':
               plus_space_flag = *format;
               goto NEXT_FLAG;
             case '-':
               left_adjust++;
               goto NEXT_FLAG;
             case '#':
               alternate_flag++;
               goto NEXT_FLAG;
             case '0':
               zeropad++;
               goto NEXT_FLAG;
          }
          break;
       NEXT_FLAG:
         format++;
      }

      /*检查是否有不输出的格式控制'*'， 例如%*s  */
      if (*format == '*')
      {
         field_width = va_arg(ap, int);
         if (field_width < 0)
         {
            field_width = -field_width;
            left_adjust++;
         }
         format++;
      }
      else
      {
         field_width = 0;
         while (*format >= '0' && *format <= '9')
           field_width = field_width * 10 + (*format++ - '0');
      }

      /*精度控制'.'， 或'*' */
      if (*format=='.')
      {
        if (*++format == '*')
        {
           precision = va_arg(ap, int);
           format++;
        }
        else
        {
          precision = 0;
          while (*format >= '0' && *format <= '9')
            precision = precision * 10 + (*format++ - '0');
        }
      }
      else
        precision = -1;

      /* 到这里，
      	left_adjust	不为零时，表示有一个符号；
      	zeropad		为1时表示有一个前导零；为0时表示没有前导零；
      	field_width	为数据的小数点前的数据宽度；
      	precision	为数据的小数点后的数据宽度；如果没有小数部分，precision为-1;
      	plus_space_flag 为0时表示不加标记，为加号或空格时表示包含加号或空格；*/

      l_L_modifier = h_modifier = 0;
      /*是否有可选的'l','L' r 'h' 修饰符? */
      switch (*format)
      {
        case 'l':
        case 'L':
          l_L_modifier++;
          format++;
          break;
        case 'h':
          h_modifier++;
          format++;
          break;
      }

      /*从下面的switch语句退出后，将发送从buf_pointer开始，到"ptr"-1结束的字符串 */
      switch (format_flag = *format++)
      {
        case 'n':
          if (sizeof(short) == sizeof(long))
          {
            *va_arg(ap,int *) = nr_of_chars;
          }
          else if (sizeof(short) != sizeof(int))
          {
            if (sizeof(int) != sizeof(long))
            {
              if (h_modifier)
                *va_arg(ap,short *) = nr_of_chars;
              else if (l_L_modifier)
                *va_arg(ap,long *) = nr_of_chars;
              else
               *va_arg(ap,int *) = nr_of_chars;
            }
            else
            {
              if (h_modifier)
                *va_arg(ap,short *) = nr_of_chars;
              else
                *va_arg(ap,int *) = nr_of_chars;
            }
         }
         else
         {
            if (l_L_modifier)
              *va_arg(ap,long *) = nr_of_chars;
            else
              *va_arg(ap,int *) = nr_of_chars;
         }
         continue;

       case 'c':
          buf[0] = va_arg(ap, int);
          ptr++;
          break;

       case 's':
         if ( !(buf_pointer = va_arg(ap,char *)) )
            buf_pointer = "(null pointer)";
         if (precision < 0)
           precision = 10000;
         for (n=0; *buf_pointer++ && n < precision; n++)
         ;
         ptr = --buf_pointer;
         buf_pointer -= n;
         break;

       case 'x':
         hex = "0123456789abcdef";
       case 'o':
       case 'u':
       case 'p':
       case 'X':
         if (format_flag == 'p')
           ulong = (long)va_arg(ap,char *);
         else if (sizeof(short) == sizeof(long))
           ulong = va_arg(ap,unsigned int);
         else if (sizeof(short) == sizeof(int))
           ulong = l_L_modifier ? va_arg(ap,long) : va_arg(ap,unsigned int);
         else
          ulong = h_modifier ? (unsigned short) va_arg(ap, int) : va_arg(ap, int);

         div_factor = (format_flag == 'o') ? 8 : (format_flag == 'u') ? 10 : 16;
         goto INTEGRAL_CONVERSION;

       case 'd':
       case 'i':
         if (sizeof(short) == sizeof(long))
         {
            if ( (long)(ulong = va_arg(ap,long)) < 0)
            {
              plus_space_flag = '-';
              ulong = -ulong;
            }
         }
         else if (sizeof(short) == sizeof(int))
         {
           if ( (long)(ulong = l_L_modifier ? va_arg(ap,long) : (long) va_arg(ap,int)) < 0)
           {
             plus_space_flag = '-';
             ulong = -ulong;
           }
         }
         else
         {
           if ( (long)(ulong = (long) (h_modifier ?
             (short) va_arg(ap, int) : va_arg(ap,int))) < 0)
           {
             plus_space_flag = '-';
             ulong = -ulong;
           }
         }
         div_factor = 10;

        /*转换为数字*/
      INTEGRAL_CONVERSION:
         ptr = buf_pointer = &buf[FRMWRI_BUFSIZE - 1];
         do
            *--buf_pointer = hex[ulong % div_factor];
         while (ulong /= div_factor);
         if (precision < 0)  /* "precision" takes precedence */
           if (zeropad)
             precision = field_width - (plus_space_flag != 0);
         while (precision > ptr - buf_pointer)
           *--buf_pointer = '0';
         if (alternate_flag && *buf_pointer != '0')
         {
           if (format_flag == 'x' || format_flag == 'X')
           {
             *--buf_pointer = format_flag;
             *--buf_pointer = '0';
           }
           else if (format_flag == 'o')
           {
             *--buf_pointer = '0';
           }
         }
         break;
        break;

       case '\0':            /* Really bad place to find NULL in */
         format--;

       default:
        /*没有定义的转换*/
         ptr = buf_pointer = "???";
         ptr += 3;
         break;

      }

      /*以下部分发送格式化好的字符串到put_one_char */
      if ( (precision = ptr - buf_pointer) > field_width)
      {
        n = 0;
      }
      else
      {
        n = field_width - precision - (plus_space_flag != 0);
      }

      /*发送前导的填充字符 */
      if (!left_adjust)
         while (--n >= 0)
         {
           put_one_char(' ', &str_pointer);
           nr_of_chars++;
         }

      /*发送标记  */
       if (plus_space_flag)
      {
        put_one_char(plus_space_flag, &str_pointer);
        nr_of_chars++;
      }

      /* 发送字符串本身 */
      while (--precision >= 0)
      {
        put_one_char(*buf_pointer++, &str_pointer);
        nr_of_chars++;
      }

      /* 发送 trailing 的空格 */
      if (left_adjust)
        while (--n >= 0)
        {
          put_one_char(' ', &str_pointer);
          nr_of_chars++;
        }
    }
    free(buf);
    return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbPrintf
* 功能:	hyco的printf函数
* 参数:	format	-- 格式控制字符串
*		...		-- 可变参数列表
* 返回:	print的字符个数
*----------------------------------------------------------------------------*/
int hyUsbPrintf(const char *format, ...)
{
	va_list list;
	int     n = 0;
	
	if(App_GetUsbDebugStat() == 0)
	{
		return 0;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	
	if (hyUsbConnect_Detect() == HY_OK)
	{
		int len;
		va_start (list, format);
		
		n = formatted_write_(format, putOneCharInBuff, pMsgBuf, list);
		
		len = strlen(pMsgBuf);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PC未准备好接收，就先写入buf中
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], pMsgBuf, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(pMsgBuf, len);
		}
		va_end (list);
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
	return (n);
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbMessage
* 功能:	hyco的Message函数, 不需要格式控制，仅仅是输出字符串
* 参数:	msg	-- 需要输出的字符串
* 返回:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessage(const char *msg)
{
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		int len = strlen(msg);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PC未准备好接收，就先写入buf中
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], msg, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(msg, len);
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbMessageByLen
* 功能:	hyco的Message函数, 不需要格式控制，仅仅是输出字符串
* 参数:	msg	-- 需要输出的字符串
* 返回:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessageByLen(const char *msg, int len)
{
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		//int len = strlen(msg);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PC未准备好接收，就先写入buf中
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], msg, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(msg, len);
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbData
* 功能:	hyco的data发送函数, 
* 参数:	pdata	-- 数据指针
		len		-- 数据长度
* 返回:	none  
*----------------------------------------------------------------------------*/
void hyUsbData(char *pdata, int len)
{
#ifdef USB_DEBUG_PRINT
	int sendCnt, remainCnt, i;
	
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
	sendCnt = len/512;
	remainCnt = len%512;
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PC未准备好接收，就先写入buf中
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], pdata, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			for (i=0; i<sendCnt; i++)
			{
				hyhwUsb_TransmitData(&pdata[i*512], 512);
			}
			if (remainCnt > 0)
			{
				hyhwUsb_TransmitData(&pdata[sendCnt*512], remainCnt);
			}
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_softInit
* 功能:	初始化Usb传输
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_softInit(void)
{
#ifdef USB_DEBUG_PRINT
	usbDebugEn = 0;
	usbDebugLockInit = 0;
	usbSendBufSize = 0;
	pUsbSendBuf = NULL;
	
	if (hyudsDcd_Connected() == TRUE)
	{
		pMsgBuf = (char *)malloc(FRMWRI_BUFSIZE);
		if (pMsgBuf == NULL)
		{
			return HY_ERROR;
		}
		usbDebugEn = 1;
		usbDebugLockInit = 1;
		pUsbSendBuf = (char *)malloc(SEND_BUFFER_MAX_SIZE);
		return HY_OK;
	}
	return HY_ERROR;
#else
	return HY_ERROR;
#endif
}
/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_Init
* 功能:	初始化Usb传输
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_Init(void)
{
#ifdef USB_DEBUG_PRINT
	if (hyudsDcd_Connected() == TRUE)
	{
		int usbFlag = 0;
		int sysTicks, usb_sysTicks;
		
		//确认是usb插入还是充电插入
		USB_InitMemory_buf();
		fnUSB_DevInit();
	  	if (hyudsDcd_GetSpeedDetectFlag() == FALSE)
	  	{
			usbDebugEn = 0;
			usbDebugLockInit = 0;
			usbSendBufSize = 0;
			if (pMsgBuf != NULL)
			{
				free(pMsgBuf);
				pMsgBuf = NULL;
			}
			if (pUsbSendBuf != NULL)
			{
				free(pUsbSendBuf);
				pUsbSendBuf = NULL;
			}
	  		hyudsDcd_HwDeinit();
	  		return HY_ERROR;
		}
		
		usbDebugEn = 2;
		
		return HY_OK;
	}
	else
	{
		usbDebugEn = 0;
		usbDebugLockInit = 0;
		usbSendBufSize = 0;
		if (pMsgBuf != NULL)
		{
			free(pMsgBuf);
			pMsgBuf = NULL;
		}
		if (pUsbSendBuf != NULL)
		{
			free(pUsbSendBuf);
			pUsbSendBuf = NULL;
		}
		return HY_ERROR;
	}
#else
	return HY_ERROR;
#endif
}


int hyUsbTrans_DeInit(void)
{
	int ret = HY_ERROR;
	if (usbDebugEn != 0)
	{
		if (usbDebugEn >= 2)
		{
			//USB硬件初始化成功后
			//if (hyudsDcd_Connected() == FALSE)
			{
				//USB已经断开
				usbDebugEn = 0;
				usbDebugLockInit = 0;
				usbSendBufSize = 0;
				if (pMsgBuf != NULL)
				{
					free(pMsgBuf);
					pMsgBuf = NULL;
				}
				if (pUsbSendBuf != NULL)
				{
					free(pUsbSendBuf);
					pUsbSendBuf = NULL;
				}
				hyudsDcd_HwDeinit();
				return ret;
			}
		}
		//if (usbDebugEn == 2)
		{
			ret = HY_OK;
		}
	}
	return ret;
}
/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_Init
* 功能:	检测是否可以以USB通道发送数据
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyUsbTrans_DetectStart(void)
{
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (usbDebugEn == 2)
	{
		U32 recvLen;
		char* pRecvBuf = NULL;
		//接收
		hyhwUsb_Dispatch();
		recvLen = hyhwUsb_CheckDataReceived();
		if(recvLen != 0)
		{
			pRecvBuf = (char *)malloc(FRMWRI_BUFSIZE);
			if (pRecvBuf == NULL)
			{
				return;
			}
			hyhwUsb_ReceiveData(pRecvBuf, recvLen);
			if (memcmp(pRecvBuf, "USB_START", strlen("USB_START")) == 0)
			{
				//如果buf中有未发数据，则全部发送
				if (usbSendBufSize>0)
				{
					int sendCnt, remainCnt, i;
					
					sendCnt = usbSendBufSize/512;
					remainCnt = usbSendBufSize%512;
					
					for (i=0; i<sendCnt; i++)
					{
						hyhwUsb_TransmitData(&pUsbSendBuf[i*512], 512);
					}
					if (remainCnt > 0)
					{
						hyhwUsb_TransmitData(&pUsbSendBuf[sendCnt*512], remainCnt);
					}
				}
				usbDebugEn = 3;
			}
			if (pRecvBuf != NULL)
			{
				free(pRecvBuf);
				pRecvBuf = NULL;
			}
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif	
}
/*-----------------------------------------------------------------------------
* 函数:	hyUsbTrans_isRunning
* 功能:	判断是否进入USB传输模式状态
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_isRunning(void)
{
#ifdef USB_DEBUG_PRINT
	if (usbDebugEn != 0)
	{
		return HY_OK;
	}
	else
	{
		return HY_ERROR;
	}
#else
	return HY_ERROR;
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	hyUsbConnect_Detect
* 功能:	检测USB是否断开，如果断开，则释放内存，重置状态
* 参数:	none
* 返回:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
static int hyUsbConnect_Detect(void)
{
	int ret = HY_ERROR;
	if (usbDebugEn != 0)
	{
		if (usbDebugEn >= 2)
		{
			//USB硬件初始化成功后
			if (hyudsDcd_Connected() == FALSE)
			{
				//USB已经断开
				usbDebugEn = 0;
				usbDebugLockInit = 0;
				usbSendBufSize = 0;
				if (pMsgBuf != NULL)
				{
					free(pMsgBuf);
					pMsgBuf = NULL;
				}
				if (pUsbSendBuf != NULL)
				{
					free(pUsbSendBuf);
					pUsbSendBuf = NULL;
				}
				hyudsDcd_HwDeinit();
				return ret;
			}
		}
		//if (usbDebugEn == 2)
		{
			ret = HY_OK;
		}
	}
	return ret;
}

void hyUsbDebugDeviceInit(void)
{
     hyUsbConnect_Detect();
}

void hyUsbConnect_WaitStart()
{
#ifdef USB_DEBUG_PRINT
	while(1)
	{
		syssleep(10);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			break;
		}
	}
#endif
}

//在同一个进程中死等PC识别USB设备
void hyUsbConnect_WaitStart1(void)
{
#ifdef USB_DEBUG_PRINT
	while(1)
	{
		hyUsbTrans_DetectStart();
		syssleep(1);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			break;
		}
	}
#endif
}

int hyUsbConnect_WaitStart2(void)
{
#ifdef USB_DEBUG_PRINT
	int cnt = 0;
	while(1)
	{
		hyUsbTrans_DetectStart();
		syssleep(1);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			return HY_OK;
			//break;
		}
		cnt++;
		if (cnt >= 5000) return -1;
	}
#endif
	return HY_ERROR;
}

void hyUsbDebug_init(void)
{
#ifdef USB_DEBUG_PRINT
	U8 cnt=0;
	hyudsDcd_connectInit();
	while(1)
	{
		if (hyUsbTrans_softInit() == HY_OK)
		{
			int ret;
			hyUsbMessage("hyUsbTrans_softInit OK!!\r\n");
			syssleep(10);
//again:
	    	hyUsbTrans_Init();
	    	ret = hyUsbConnect_WaitStart2();
	    	if (ret == HY_OK) break;
		}
		syssleep(1);
		cnt++;
		if (cnt == 50)
		{
			Led_ctrl(6);
		}
		else if (cnt == 100)
		{
			Led_ctrl(0);
			cnt = 0;
		}
	}
#endif
}