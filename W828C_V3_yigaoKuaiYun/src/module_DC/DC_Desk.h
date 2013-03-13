#ifndef _DC_DESK_H_
#define _DC_DESK_H_

#include "hyTypes.h"
#include "grap_api.h"


/* jpeg 压缩输出图像大小，最大 256 */
#define	DC_IMAGE_ENCODED_BUFSIZE		(256 *1024)
/* jpeg 编码库需要24K工作缓冲区 */
#define DC_IMAGE_JPEG_INSTANCE			(24 *1024 )
/* jpeg 编码库需要的临时缓冲区*/
#define DC_IMAGE_JPEG_TEMPBUF			(512 *1024)//(256 *1024)



/*用作预览的buf*/
#define DC_PREVIEW_BUF_NUM					4

#ifdef W818C
#define DC_DISPLAY_START_ROW				30
#define DC_DISPLAY_START_COL				30
#else
#define DC_DISPLAY_START_ROW				50
#define DC_DISPLAY_START_COL				0
#endif

#define DC_PREVIEW_DISPLAY_WIDTH			240
#define DC_PREVIEW_DISPLAY_HEIGHT			180
#define DC_PREVIEW_DISPLAY_BUFSIZE			(DC_PREVIEW_DISPLAY_WIDTH * DC_PREVIEW_DISPLAY_HEIGHT * 2)
#define DC_PREVIEW_DISPLAY_PIXELS			(DC_PREVIEW_DISPLAY_WIDTH * DC_PREVIEW_DISPLAY_HEIGHT)

#define DC_PREVIEW_320_240_WIDTH			320
#define DC_PREVIEW_320_240_HEIGHT			240
#define DC_PREVIEW_320_240_PIXELS			(DC_PREVIEW_320_240_WIDTH * DC_PREVIEW_320_240_HEIGHT)

#define DC_PREVIEW_640_480_WIDTH			640
#define DC_PREVIEW_640_480_HEIGHT			480
#define DC_PREVIEW_640_480_PIXELS			(DC_PREVIEW_640_480_WIDTH * DC_PREVIEW_640_480_HEIGHT)



//#define DC_QVGA_PREVIEW
#define DC_VGA_PREVIEW

#ifdef DC_VGA_PREVIEW
	#define DC_PREVIEW_H_SIZE			DC_PREVIEW_640_480_HEIGHT
	#define DC_PREVIEW_W_SIZE			DC_PREVIEW_640_480_WIDTH

	#define DC_PREVIEW_Y_SIZE			DC_PREVIEW_640_480_PIXELS
	#define DC_PREVIEW_U_SIZE			(DC_PREVIEW_640_480_PIXELS >> 2)
	#define DC_PREVIEW_V_SIZE			(DC_PREVIEW_640_480_PIXELS >> 2)
#elif defined DC_QVGA_PREVIEW
	#define DC_PREVIEW_H_SIZE			DC_PREVIEW_320_240_HEIGHT
	#define DC_PREVIEW_W_SIZE			DC_PREVIEW_320_240_WIDTH

	#define DC_PREVIEW_Y_SIZE			DC_PREVIEW_320_240_PIXELS
	#define DC_PREVIEW_U_SIZE			(DC_PREVIEW_320_240_PIXELS >> 2)
	#define DC_PREVIEW_V_SIZE			(DC_PREVIEW_320_240_PIXELS >> 2)
#endif

typedef enum
{
	DC_STATE_INIT=0,		//初始化状态
	DC_STATE_GETOK,		//成功
	DC_STATE_UNKNOW,	//不能支持的类型
	DC_STATE_START,		//开始识别
	DC_STATE_QUIT		//退出

}eDCSTATE;



/*-----------------------------------------------------
* 函数:DC_Desk
* 功能:拍照
* 参数:pWorkBuf:外部提供工作buf  最少3M
       picSize:(out)编码后图片的大小  0表示失败
* 返回:编码后存储图像数据的指针
------------------------------------------------------*/
U8 *DC_Desk(tGRAPDESKBOX *pFather, U8 *pWorkBuf, U32 *picSize);


#endif //_DC_DESK_H_