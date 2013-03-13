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
* ����:	hyUsbTrans_softInit
* ����:	��ʼ��Usb����
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_softInit(void);

/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_Init
* ����:	��ʼ��Usb����(Ӳ��)
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_Init(void);

/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_Init
* ����:	����Ƿ������USBͨ����������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyUsbTrans_DetectStart(void);

/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_isRunning
* ����:	�ж��Ƿ����USB����ģʽ״̬
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_isRunning(void);



/*-----------------------------------------------------------------------------
* ����:	hyUsbPrintf
* ����:	hyctron��printf����
* ����:	format	-- ��ʽ�����ַ���
*		...		-- �ɱ�����б�
* ����:	print���ַ�����  
*----------------------------------------------------------------------------*/
int hyUsbPrintf(const char *format, ...);

/*-----------------------------------------------------------------------------
* ����:	hyUsbMessage
* ����:	hyctron��Message����, ����Ҫ��ʽ���ƣ�����������ַ���
* ����:	msg	-- ��Ҫ������ַ���
* ����:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessage(const char *msg);

/*-----------------------------------------------------------------------------
* ����:	hyUsbData
* ����:	hyctron��data���ͺ���, 
* ����:	pdata	-- ����ָ��
		len		-- ���ݳ���
* ����:	none  
*----------------------------------------------------------------------------*/
void hyUsbData(char *pdata, int len);


#endif /* TM_HW_VH_DEBUG_H_*/