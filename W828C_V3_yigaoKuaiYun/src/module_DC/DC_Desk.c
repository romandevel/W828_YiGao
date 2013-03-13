#include "DC_Desk.h"
#include "hyhwCif.h"
#include "hyhwGPIO.h"
#include "hyhwCkc.h"
#include "hyhwDma.h" 
#include "grap_api.h"
#include "base.h"
#include "jpeg_api.h"
#include "JPEGDoceinc.h"
#include "AbstractFileSystem.h"
#include "hyhwIntCtrl.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "drvKeyArray.h"

eDCSTATE eDCState;


U8  gu8DCRead,gu8DCWrite;
U8  gu8DCPrevDelayFrame;//预览丢弃帧数
U32 gu32DCQuitTime;//无操作时超时退出

U8	*gpDCUseBuf;//外部提供    最少2.94M
U32 gu32DCPicSize;//编码后图片数据大小   0:表示没有成功编码

/*经过缩放后的存储位置*/
U8 *gpDCQvga_y,*gpDCQvga_u,*gpDCQvga_v;
/*应用进程显示使用*/
U8 *gpDCDisplayRgb;
/*用作预览的buf*/
U8 *gpDCPrev_y[DC_PREVIEW_BUF_NUM],*gpDCPrev_u[DC_PREVIEW_BUF_NUM],*gpDCPrev_v[DC_PREVIEW_BUF_NUM];

U8 *gpDCEncodeRgb,*gpDCPhotoOutBuf,*gpDCPhotoWorkBuf;

/*=========================================================================================*/
void Photo_TurnBuf(U8 *pdata,U8 *pworkbuf)
{
	int i,j,w,h;
	U16 *ps ,*pt;
	
	ps = (U16 *)pdata ;
	pt = (U16 *)pworkbuf;
	w  = 320;
	h  = 240;
	
	for(i = 0 ; i < w; i++)
	{
		pt += gLcd_Total_Column  ;
		for(j = 0 ; j < h ; j++)
		{
			pt[j] = ps[w * (h-1-j) + i] ;
		}
	}
	
	memcpy(pdata,pworkbuf,w*h<<1);
}

int Photo_Save(U8 *pInput,U8 *pOutput,U8 *pWork)
{
	int			rc=0;
	tJPEG_API	tJpegApi;
		
	/*使用前应当清零,这个是jpeg编码库使用的输入参数 */
	memset((U8 *)&tJpegApi,0,sizeof(tJPEG_API));
	
	tJpegApi.Func                = Jpeg_Encode;
	tJpegApi.ImageLevel          = Jpeg_Standard;//Jpeg_High;//			/* 图像质量，共5级 */
	tJpegApi.JpegFile            = TRUE;
	tJpegApi.EncodeFormat        = JPEG_FORMAT_RGB565;//JPEG_FORMAT_YUV420;//
	
	tJpegApi.ReadOffset          = 0;
	tJpegApi.pInputBuf           = pInput;
	
	tJpegApi.pTempBuf            = pWork;
	tJpegApi.TempBufLen          = DC_IMAGE_JPEG_INSTANCE;	
							
	tJpegApi.pSdramBuf           = pWork + DC_IMAGE_JPEG_INSTANCE;
	tJpegApi.SdramBufLen         = DC_IMAGE_JPEG_TEMPBUF;		

	tJpegApi.pOutputBuf          = pOutput;
	tJpegApi.OutputBufLen        = DC_IMAGE_ENCODED_BUFSIZE;
#if 0	
	tJpegApi.InputBufLen         = (DC_PREVIEW_640_480_PIXELS<<1);//DC_PREVIEW_640_480_BUFSIZE;//
	tJpegApi.ImageWidth          = DC_PREVIEW_640_480_WIDTH;
	tJpegApi.ImageHeight         = DC_PREVIEW_640_480_HEIGHT;
#else
	tJpegApi.InputBufLen         = (DC_PREVIEW_320_240_PIXELS<<1);//DC_PREVIEW_320_240_BUFSIZE;//
	tJpegApi.ImageWidth          = DC_PREVIEW_320_240_WIDTH;//240;//
	tJpegApi.ImageHeight         = DC_PREVIEW_320_240_HEIGHT;//320;//
#endif

	//rc = Jpeg_HWCodec(&tJpegApi, NULL, 0 );
	rc = Jpeg_Codec(&tJpegApi);
	if(rc==Jpeg_Success)
	{
		gu32DCPicSize = tJpegApi.OutputDataLen;
		//把编码后的数据copy到外部提供buf的最前端
		memcpy(gpDCUseBuf, pOutput, gu32DCPicSize);
		
		hyUsbPrintf("picsize = %d \r\n", gu32DCPicSize);
		
		rc = 0;//成功
	}
	else
	{/* JPG 编码失败 */
		gu32DCPicSize = 0;
		rc = -1;
	}
	
	return rc;
}

int Photo_Decode(U8 *pData,U32 dataLen, U16 dsth, U16 dstw, U8 *pWork, U8 *pOut, BOOL ScaleImage)
{
	tJPEG_API tJpgApi;
	U8 *pReusableMemory;
	
	pReusableMemory = pWork;
	
	memset((char *)&tJpgApi,0,sizeof(tJPEG_API));//使用前应当清零
	
	tJpgApi.Func         = Jpeg_Decode;//Jpeg_GetSize
	tJpgApi.JpegFile     = FALSE;
	tJpgApi.ReadFromFile = FALSE;
	tJpgApi.ScaleImage   = ScaleImage;//FALSE;		//是否缩放
	tJpgApi.FileHandle   = 0;
	tJpgApi.ReadOffset   = 0;
	tJpgApi.pOutputBuf   = pOut;
	tJpgApi.OutputBufLen = dsth*dstw*2;
	tJpgApi.pTempBuf     = (U8*)pReusableMemory;
	tJpgApi.TempBufLen   = 20480;
	tJpgApi.ScrWidth     = dstw;
	tJpgApi.ScrHeight    = dsth;
	tJpgApi.PreViewImage = FALSE;
	tJpgApi.pSdramBuf    = (U8 *)(tJpgApi.pTempBuf + tJpgApi.TempBufLen);
	tJpgApi.SdramBufLen  = 256*1024;
	tJpgApi.pInputBuf    = pData;
	tJpgApi.InputBufLen  = dataLen;
	
	if(Jpeg_Codec(&tJpgApi) != 0)
	{
	hyUsbPrintf("decode err  w = %d  dataLen = %d \r\n", dstw, dataLen);
		return -2;
	}
		
	return 0;
}

/*********************************************************************
* 函数名  : DC_SensorISR
* 功能    : camera中断处理函数
* 输入    : none 
* 输出    : none
* 返回    :
**********************************************************************/
void DC_SensorISR()
{
	U8 rd,wr,wr2;

	rd = gu8DCRead;
	wr = gu8DCWrite;

	/* clear frame flag*/
	hyhwmCif_clearFrameFlag();
	wr2 = wr + 1;
	if(wr2 >= DC_PREVIEW_BUF_NUM)
	{
		wr2 -= DC_PREVIEW_BUF_NUM;
	}
	if(wr2 != rd)
	{
		wr = wr2;
	}
	/*if interrption coming，store the corresponding data to preview_y/u/v buffers*/
	hyhwCif_frameStart(	(U32)gpDCPrev_y[wr],(U32)gpDCPrev_u[wr],(U32)gpDCPrev_v[wr] );
	gu8DCWrite = wr;

	return;
}


static void DC_StopInterrupt()
{
	hyhwmCif_disable();
	hyhwInt_disable(INT_BIT_CAMERA);
	hyhwInt_clear(INT_BIT_CAMERA);
	hyhwmCif_intDisable();
	hyhwmCif_clearFrameFlag();
}

static int DC_HwDeInit()
{
	hyhwLcd_flashDisable();//关闭闪光灯
	
	hyhwmCif_disable();
	hyhwmCif_intDisable();
	hyhwInt_disable(INT_BIT_CAMERA);
	hyhwInt_ConfigCamera_ISR(NULL);
	hyhwmCif_clearFrameFlag();
	hyhwInt_clear(INT_BIT_CAMERA);
	hyhwInt_ConfigCodecISR(NULL);

	hyhwCmosSensor_standby_enable();

	/*关闭硬件解码设备*/
	hyhwJpegG2d_Ctrl(0);
	hyhwCmosSensor_powerCtrl(EnableOff);
}

static int DC_DeInit()
{
	DC_HwDeInit();
	
	eDCState = DC_STATE_QUIT;
	
	return 0;
}

static int DC_ParaInit()
{
	eDCState	= DC_STATE_INIT;
	gu8DCRead	= 0;
	gu8DCWrite	= 0;
	gu32DCPicSize = 0;
	
	gu8DCPrevDelayFrame	= 5;
	
	gu32DCQuitTime = rawtime(NULL);
	
	return 0;
}

static int DC_Alloc()
{
	U8  *pBuf,i;
	
	/*四字节对齐*/
    pBuf = (char *)((unsigned)(gpDCUseBuf + 3) & 0xFFFFFFFC);
    gpDCUseBuf = pBuf;
    
    //缩放后的数据buf   115200
    gpDCQvga_y	= pBuf;
	pBuf	+= DC_PREVIEW_320_240_PIXELS;
	gpDCQvga_u	= pBuf;
	pBuf	+= (DC_PREVIEW_320_240_PIXELS>>2);
	gpDCQvga_v	= pBuf;
	pBuf	+= (DC_PREVIEW_320_240_PIXELS>>2);
	
	//用来存放显示到屏上的RGB数据  153600
	gpDCDisplayRgb	= pBuf;
	pBuf	+= (DC_PREVIEW_320_240_PIXELS<<1);
	
	
	/*用作预览的buf   1843200*/
	for(i = 0; i < DC_PREVIEW_BUF_NUM; i++)
	{
		gpDCPrev_y[i] = pBuf;
		pBuf   += DC_PREVIEW_Y_SIZE;
		gpDCPrev_u[i] = pBuf;
		pBuf   += DC_PREVIEW_U_SIZE;
		gpDCPrev_v[i] = pBuf;
		pBuf   += DC_PREVIEW_V_SIZE;
	}
	
	//图片编码   
	gpDCEncodeRgb    = pBuf;
	pBuf      += (DC_PREVIEW_320_240_PIXELS<<1);//153600
	gpDCPhotoWorkBuf = pBuf;
	pBuf      += (DC_IMAGE_JPEG_TEMPBUF+DC_IMAGE_JPEG_INSTANCE);//548864
	gpDCPhotoOutBuf  = pBuf; //262144
	
	//以上 3076608   (大概2.9M)
	
	return 0;
}

static int DC_HwInit()
{
	hyhwCmosSensor_powerCtrl(EnableOn);
	//打开硬件解码设备JPEG & 2D
	hyhwJpegG2d_Ctrl(1);
	//打开camera时钟
	hyhwCpm_busClkEnable(CKC_CAM);
	//设置camera的中断处理函数
	hyhwInt_ConfigCamera_ISR(DC_SensorISR);
	
	if(hyhwCmosSensor_Init() != 0)
	{
	hyUsbPrintf("hyhwCmosSensor_Init  fail \r\n");
		return -1 ;
	}
	hyhwCif_init();
	//sleep(200);
	hyhwmCif_clearIntFlag();

#ifdef DC_VGA_PREVIEW	
	//设置预览模式
	hyhwCif_setVGAMode();   //640 x 480
	//hyhwCif_setSXGAMode(1);
#elif defined DC_QVGA_PREVIEW
	hyhwCif_setQVGAMode();
#endif
	hyhwmCif_clearFrameFlag();
	
	//指定INT_BIT_CAMERA中断为IRQ中断,并使能
	hyhwInt_setIRQ(INT_BIT_CAMERA);
	hyhwInt_enable(INT_BIT_CAMERA);

	/*获取一帧数据*/
	hyhwCif_frameStart((U32)gpDCPrev_y[0],(U32)gpDCPrev_u[0],(U32)gpDCPrev_v[0]);

	hyhwLcd_flashDisable();//关闭闪光灯
	
	return 0;
}

static int DC_Init()
{

	DC_ParaInit();
	DC_Alloc();
	if(-1 == DC_HwInit())
	{
		return -1 ;
	}
	
	return 0;
}


/*=========================================================================================*/
const tGrapViewAttr DCViewAttr[]=
{
	{COMM_DESKBOX_ID, 0,0,240,320,0,0,0,0,TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	  9,282, 73,34,     9,282, 82,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//拍照
	{BUTTON_ID_2,	158,282, 73,34,   158,282,231,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	  9,282, 73,34,     9,282, 82,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	158,282, 73,34,   158,282,231,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//拍照
#endif
};

const tGrapButtonPriv DCBtnPrv[]=
{
#ifdef W818C
	{(U8 *)"拍照", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK, 0,  NULL},//
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,0,  NULL},//
#else
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,0,  NULL},//
	{(U8 *)"拍照", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK, 0,  NULL},//
#endif
};

//加载预览数据
void DC_LoadDcData(U16 kRow, U16 kCol, U16 kHigh, U16 kWidth, U8 *pBuf)
{
	U8  *pMemory,*pTemp,*pRgb;
	U16 *pD,*pS;
	U32 off,size;
	int i,j;
	
	pMemory = (U8 *)Get_Display_DisplayMemory_Offset();
	
#ifdef W818C
	off  = (kRow*gLcd_Total_Column+kCol)*2;//在显存中的偏移
	pD   = (U16 *)(pMemory+off);
	pS = (U16 *)pBuf ;
	
	for(i = 0 ; i < kWidth; i++)
	{
		for(j = 0 ; j < kHigh ; j++)
		{
			pD[j] = pS[kWidth * (kHigh-1-j) + i] ;
		}
		pD += gLcd_Total_Column;
	}
#else
	size = kHigh*kWidth;
	off  = (kRow*gLcd_Total_Column+kCol)*2;//在显存中的偏移
	pD   = (U16 *)(pMemory+off);
	memcpy((char *)pD,pBuf,size*2);
#endif	

	drvLcd_BMPFromDisplayMemory2Screen();
	return ;
}

int DC_Preview()
{
	U8	rd,wr;
	U8	intLev;
	U8	*pTemp;
	int tQRret;
	
	rd = gu8DCRead;
	wr = gu8DCWrite;
	
	if(rd != wr)
	{
		if(gu8DCPrevDelayFrame > 0)
		{
			gu8DCPrevDelayFrame--;
		}
		else// if(hyhwLcd_RGBDmaOK() == TRUE)
		{
		#ifdef DC_VGA_PREVIEW	
			/*预览图像缩放成可以显示的图像*/
			Comm_HWScaler_YUV420_t((U32*)gpDCPrev_y[rd],(U32*)gpDCPrev_u[rd],(U32*)gpDCPrev_v[rd],
						 (U32*)gpDCQvga_y,(U32*)gpDCQvga_u,(U32*)gpDCQvga_v,
						 (DC_PREVIEW_H_SIZE<<16)|DC_PREVIEW_W_SIZE,(DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH );
			
			/*YUV-->RGB, gpDCDisplayRgb 320 x 240 x 2*/
			Color_YUV420toRGB565_Buf_t((U32*)gpDCQvga_y,(U32*)gpDCQvga_u,(U32*)gpDCQvga_v,
    		                         (U32*)gpDCDisplayRgb,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,0,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,0);
		#elif defined DC_QVGA_PREVIEW
			/*YUV-->RGB, gpDCDisplayRgb 320 x 240 x 2*/
			Color_YUV420toRGB565_Buf((U32*)gpDCPrev_y[rd],(U32*)gpDCPrev_u[rd],(U32*)gpDCPrev_v[rd],
    		                         (U32*)gpDCDisplayRgb,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,0,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,
    		                         (DC_PREVIEW_DISPLAY_HEIGHT<<16)|DC_PREVIEW_DISPLAY_WIDTH,0);
		#endif

        	DC_LoadDcData(DC_DISPLAY_START_ROW,DC_DISPLAY_START_COL,DC_PREVIEW_DISPLAY_HEIGHT,DC_PREVIEW_DISPLAY_WIDTH,gpDCDisplayRgb);

    		if(eDCState == DC_STATE_START)
    		{
    			eDCState = DC_STATE_INIT;

				/*预览图像缩放成可以显示的图像*/
				Comm_HWScaler_YUV420_t((U32*)gpDCPrev_y[rd],(U32*)gpDCPrev_u[rd],(U32*)gpDCPrev_v[rd],
							 (U32*)gpDCQvga_y,(U32*)gpDCQvga_u,(U32*)gpDCQvga_v,
							 (DC_PREVIEW_H_SIZE<<16)|DC_PREVIEW_W_SIZE,(DC_PREVIEW_320_240_HEIGHT<<16)|DC_PREVIEW_320_240_WIDTH );
				
				/*YUV-->RGB, gpDCDisplayRgb 320 x 240 x 2*/
				Color_YUV420toRGB565_Buf_t((U32*)gpDCQvga_y,(U32*)gpDCQvga_u,(U32*)gpDCQvga_v,
	    		                         (U32*)gpDCEncodeRgb,
	    		                         (DC_PREVIEW_320_240_HEIGHT<<16)|DC_PREVIEW_320_240_WIDTH,0,
	    		                         (DC_PREVIEW_320_240_HEIGHT<<16)|DC_PREVIEW_320_240_WIDTH,
	    		                         (DC_PREVIEW_320_240_HEIGHT<<16)|DC_PREVIEW_320_240_WIDTH,0);
				
				
				if(0 == Photo_Save(gpDCEncodeRgb,gpDCPhotoOutBuf,gpDCPhotoWorkBuf))
    			{
    				eDCState = DC_STATE_GETOK;
    			}
        	}
        	
        	rd++;
			if(rd >= DC_PREVIEW_BUF_NUM)
			{
				rd -= DC_PREVIEW_BUF_NUM;
			}
			gu8DCRead = rd;
		}
	}
	
	if(gu8DCPrevDelayFrame > 0)
	{
		return 0;
	}
	
	/*带闪光灯，自动打开闪光灯*/
	if(eDCState == DC_STATE_INIT)
	{
		hyhwLcd_flashEnable();
	}
	
	return 0;
}


int DC_DeskboxHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPDESKBOX	*pDesk;
	U32				delay;
	U32				message, wParam, lParam;
	int				ret = -1;
	tGRAPVIEW		*pSubView;
	tGRAPZORDER		*pSubZorder ;
	
	pDesk	= (tGRAPDESKBOX *)pView;
	message = pEvent->message;
	wParam  = pEvent->wParam;
	lParam  = pEvent->lParam;
	
	switch(message)
	{
	case VIEW_DRAW:		  
		pDesk->draw(pDesk, 0, 0); 
		if( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
		{
			DisplayData2Screen();
		}
		return SUCCESS;
	    break;
	case VIEW_QUIT:
	case VIEW_ESC:
		*pDesk->pQuitCnt = 1;
		break;
	case VIEW_QUITALL:
		*pDesk->pQuitCnt = 0xff;
		break;
	case VIEW_PRESS:
		break;
	case VIEW_REV_PULLMSG:
	    if (pDesk->pPullChild == NULL)
        {//第一点    
                        
            if (VIEW_PULL_END == pEvent->extParam) return SUCCESS;
            
            pEvent->extParam = VIEW_PULL_START;       
            
            pSubZorder = pDesk->pZorderList;

			while (pSubZorder)
			{
				pSubView = pSubZorder->pMember;
				
				if (pSubView->view_attr.enable == 1 && VIEW_PULL_START == pSubView->handle(pSubView,pEvent))
                {
                    pDesk->pPullChild = pSubView;
                    pDesk->lastPoint = lParam;
                    pDesk->pullTimeOut = rawtime(NULL);
                    break;    
                }
                
				pSubZorder = pSubZorder->pNext;
			}
        }
        else
        {//后续点
            int rv;
            
            pEvent->wParam = pDesk->lastPoint;
            
            pSubView = pDesk->pPullChild;
            rv = pSubView->handle(pSubView,pEvent);
            	        
            if (VIEW_PULL_END == rv)
            {
                pDesk->pPullChild = NULL;	        
            }
            else if (VIEW_PULL_ING == rv)
            {
                pDesk->lastPoint = lParam;
            }
            
            //特殊返回值
            if(rv == PARSED )
    		{
    	        pSubZorder = pDesk->pZorderList ;
    	        while(pSubZorder)
				{
					if(pSubView == pSubZorder->pMember)
					{
						break;
					}
					pSubZorder = pSubZorder->pNext;
				}
    			Grap_UpdateZorder(((tGRAPVIEW *)pView)->pParent, pSubZorder);	
    			pDesk->pPullChild = NULL;			
    		}
    		else if(RETURN_FOCUSMSG == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_FOCUSMSG;
    		}
    		else if(RETURN_QUIT == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_QUIT;
    		}
        }	    
        pDesk->pullTimeOut = rawtime(NULL);
	    break;
	default:
		break;
	}
	
	if(message != VIEW_QUIT && message != VIEW_ESC && message != VIEW_REV_PULLMSG && message != VIEW_KEY_MSG)
	{
		tGRAPZORDER  *pSubZorder ;
		S32		ret = 0 ;
		pSubZorder = pDesk->pZorderList ;

		while (pSubZorder)	// deskbox负责各个子控件的handle的调用。
		{
			pSubView = pSubZorder->pMember ;

			if( pSubView->view_attr.enable || message == VIEW_INIT)
			{
				ret = pSubView->handle(pSubView, pEvent);
				if(ret == PARSED )
				{
					Grap_UpdateZorder(((tGRAPVIEW *)pView)->pParent, pSubZorder);
					break ;
				}
				else if(RETURN_FOCUSMSG == ret)
				{
					return RETURN_FOCUSMSG;
				}
				else if(RETURN_QUIT == ret)
				{
					return RETURN_QUIT;
				}
			}
			pSubZorder = pSubZorder->pNext;
		}
	}
	
	if(eDCState == DC_STATE_INIT || eDCState == DC_STATE_START)
	{
		delay = ABS(rawtime(NULL) - gu32DCQuitTime);
		if(delay >= 18000)//3 min
		{
			*pDesk->pQuitCnt = 1;
		}
		else
		{
			/*图像预览*/
			DC_Preview();
			if(eDCState == DC_STATE_GETOK)//拍照成功
			{
				DC_StopInterrupt();
				hyhwLcd_flashDisable();
				*pDesk->pQuitCnt = 1;
				Remind_Fun(0, 0);
			}
		}
	}
	
	
	return SUCCESS;
}


S32 DC_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
    tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
    
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//在有效范围内
	         ret = VIEW_PULL_START;
	     }
	     else
	     {
	        return FAIL;
	     }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		Grap_ChangeFocusEnable(pButton);

	    pButton->state = STATE_PRESS;
		pButton->draw(pButton, 0, 0);
	    DisplayData2Screen();
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//在有效范围内
	         pButton->state = STATE_PRESS;
		     pButton->draw(pButton, 0, 0);
		     
		     ret = pButton->pressEnter(pView, pButton->state) ;
             if ( ret== RETURN_REDRAW)
             {
            	 ReDraw(pButton, 0, 0);
             }
             else if(ret == RETURN_QUIT)
             {
            	 *((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
             }
             else if (ret == RETURN_FOCUSMSG)
             {
            	 return ret;
             }			
		     ret = VIEW_PULL_ING; 
	     }
	     else
	     {
	        pButton->state = STATE_NORMAL;
		    pButton->draw(pButton,0,0);
	     }

	    
	}
	else if (VIEW_PULL_END == extParam)
	{//up
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0);				
		DisplayData2Screen();

        if(!GRAP_INWIDGET_RANGE(x,y,pButton))
        {
            return VIEW_PULL_END;
        }    
        
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}
		else if (ret == RETURN_FOCUSMSG)
		{
			return ret;
		}			
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0); 
		
		ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;  
 
}

S32 DC_ButtonHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPBUTTON *pButton;
	U32 message, wParam, lParam ;
	int ret = SUCCESS;

	pButton = (tGRAPBUTTON *)pView;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	switch(message)
	{
	case VIEW_ENABLE:
		pButton->view_attr.enable = 1;
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_DISABLE:
		pButton->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_PRESS:
		if ((U16)lParam == pButton->btn_privattr.keycode)
		{
			tGRAPVIEW *pTempView;
			
			pButton->view_attr.clNormal = pButton->view_attr.clFocus;
			pButton->btn_privattr.cl_normal = pButton->btn_privattr.cl_focus;
			
			if(pButton->view_attr.FocusEnable)
			{
				if(!pButton->view_attr.curFocus)
				{
					pTempView = pButton->pParent;
					while(pTempView)
					{
						if(pTempView->view_attr.curFocus)
						{
							pTempView->view_attr.curFocus = 0;
							pTempView->draw(pTempView, 0, 0);
						}						
						pTempView = pTempView->pNext ;
					}						
					pButton->view_attr.curFocus = 1;
				}
			}
				
			pButton->state = STATE_PRESS;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			syssleep(15);
			pButton->state = STATE_NORMAL;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			ret = pButton->pressEnter(pView, pButton->state) ;
			if ( ret== RETURN_REDRAW)
			{
				ReDraw(pButton, 0, 0);
				return PARSED;
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
			}
			else if (ret == RETURN_FOCUSMSG)
			{
				return ret;
			}	
		}
		break;
	case VIEW_CLICK:
	case VIEW_BTNUP:
		break ;	          
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pButton->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pButton->recvPullMsg(pButton,pEvent);
	    break;	
	}
	return ret;
}

int DC_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;

	if (state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1:
			gu32DCQuitTime = rawtime(NULL);
			eDCState = DC_STATE_START;
			break;
		case BUTTON_ID_2:
			*pDesk->pQuitCnt = 1;
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

/*-----------------------------------------------------
* 函数:DC_Desk
* 功能:拍照
* 参数:pWorkBuf:外部提供工作buf  最少3M
       picSize:(out)编码后图片的大小  0表示失败
* 返回:编码后存储图像数据的指针
------------------------------------------------------*/
U8 *DC_Desk(tGRAPDESKBOX *pFather, U8 *pWorkBuf, U32 *picSize)
{
	int				i,ret = 1;
	tGRAPDESKBOX	*pDesk	= NULL;
	tGRAPBUTTON		*pBtn;
	U16 			lastValue,lastSpeed;
	U8				index=0;
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 192);
	
	gpDCUseBuf = pWorkBuf;
	*picSize = 0;
	
	pDesk = Grap_CreateDeskbox(&DCViewAttr[index++], "拍照");
	if (NULL == pDesk)
	{
		goto DC_BAD;
	}
	Grap_Inherit_Public(pFather, pDesk);
	pDesk->handle = DC_DeskboxHandle;
	//pDesk->pKeyCodeArr = keyComArray;
	
	Grap_ShowPicture(pDesk->view_attr.clNormal, 0, 0);
	drvLcd_SetColor(0xffff, 0);
	drvLcd_SetDisplayAddress(Get_Display_DisplayMemory_Offset());
	drvLcd_DisplayString(100,84,0,"请稍等...", NULL, 0);
	DisplayData2Screen();
	
	for (i=0; i<2; i++)
	{
		pBtn = Grap_InsertButton(pDesk,&DCViewAttr[index++],&DCBtnPrv[i]);
		if (NULL == pBtn)	goto DC_BAD;
		pBtn->pressEnter = DC_BtnPress;
		pBtn->recvPullMsg = DC_ButtonRecvPullMsg;
		pBtn->handle = DC_ButtonHandle;
	}
	
	if(0 != DC_Init())
	{
		Com_SpcDlgDeskbox("无摄像设备!",6,pDesk,2,NULL,NULL,100);
		Grap_DeskboxDestroy(pDesk);
		goto DC_BAD;
	}
	
	lastValue = BackLight_CloseScreenProtect();//记录屏保信息 退出时还原
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	BackLight_SetScreenProtect(lastValue);
	
	DC_DeInit();
	drvLcd_IsUseQueue(0);
	drvLcd_ClearQueue();
	
	*picSize = gu32DCPicSize;
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	return gpDCUseBuf;
	
DC_BAD:
hyUsbPrintf("dc_bad  ---- \r\n");
	*picSize = 0;
	DC_DeInit();
	drvLcd_IsUseQueue(0);
	drvLcd_ClearQueue();
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	return NULL;
}


/*================================================================================*/
const tGrapViewAttr DCReViewAttr[]=
{
	{COMM_DESKBOX_ID, 0,0,240,320,0,0,0,0,TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,	  0,300,240,20,  0,280,240,320,   0,0,COLOR_SLIDE_BLOCK,  0,1,0,0,0},//返回
};

const tGrapButtonPriv DCReBtn[]=
{
	{(U8 *)"返回", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,0,  NULL},//
};

S32 Photo_DisplayDeskDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pButton=NULL;
	U16 *pSrc, *pDst;
	int r,c,off;
	
	pDesk = (tGRAPDESKBOX *)pView;
	Grap_Set_Current_Desk(pDesk);
	
	drvLcd_IsUseQueue(0);
	
	pSrc = (U16 *)pDesk->view_attr.reserve;
	pDst = (U16 *)Get_Display_DisplayMemory_Offset();
	for(c = 0; c < 320; c++)
	{
		for(r = 240-1; r >= 0; r--)
		{
			off = r*320+c;
			*pDst++ = pSrc[off];
		}
	}

	pButton = (tGRAPDESKBOX *)Grap_GetViewByID(pDesk, BUTTON_ID_1);
	if (pButton != NULL)
	{
		pButton->draw(pButton, 0, 0);
	}

	return SUCCESS;
}

int Photo_DisplayBtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;

	if (state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1:
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

S32 Photo_DisplayBtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 

	if (pButton->view_attr.enable)
	{//
		Grap_BrushScreen(viewX1, viewY1, viewX2,viewY2, pview_attr->color);
	}
	
	if (pbtn_privattr->pName)//显示button上的字符
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	}
	
	return SUCCESS;
}

int Photo_DisplayDesk(void *pView, U8 *pDCbuf)
{
	int				ret = 1;
	tGRAPDESKBOX	*pDesk	= NULL;
	tGRAPBUTTON		*pBtn;
	U8				index1=0;
	
	pDesk = Grap_CreateDeskboxNoCom(&DCReViewAttr[index1++], NULL);
	if (NULL == pDesk)
	{
		return ;
	}
	Grap_Inherit_Public((tGRAPDESKBOX *)pView, pDesk);
	pDesk->view_attr.reserve = pDCbuf;
	pDesk->draw = Photo_DisplayDeskDraw;
	
	pBtn = Grap_InsertButton(pDesk,&DCReViewAttr[index1++],&DCReBtn[0]);
	if (NULL == pBtn)	return;
	pBtn->pressEnter = Photo_DisplayBtnPress;
	pBtn->draw       = Photo_DisplayBtnDraw;
		
	Grap_DeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
	
	return ret;
}