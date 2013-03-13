#ifndef _JPEGDOCEINC_
#define _JPEGDOCEINC_

/*
*standard include files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"


typedef enum  
{
	JpegDecodeReturn_Success  =	0,
	JpegDecodeReturn_error,
	JpegDecodeReturn_Nofile,
	JpegDecodeReturn_ReadfileError,
	JpegDecodeReturn_NotJpegfile,	
	JpegDecodeReturn_UnSupportJpegfile,
	JpegDecodeReturn_BadJpegfile,
	JpegDecodeReturn_BadQuantilizationID,
	JpegDecodeReturn_BadDataPrecision,
	JpegDecodeReturn_BadImageWidthorHeight,
	JpegDecodeReturn_BadComponentInfo,
	JpegDecodeReturn_BadHuffmanTable,
	JpegDecodeReturn_InsufficientMemory	,
	JpegDecodeReturn_ComponentNot3,
	JpegDecodeReturn_UserBreak,
	JpegDecodeReturn_NoThumbnail
} JpegDecodeReturn_en;

extern U32 mathdiv(U32 a,U32 b);
extern U32 mathunsignmulti(U32 a,U32 b);
extern int mathsignmulti(int a,int b);

typedef char *LPCSTR;


/*************************************************************************
 * 函数:	Jpeg_DecodeFile            
 *
 * 说明:	Jpeg解码借口函数，有缩放功能
 *
 * 参数:	FileName 	文件名
 *			pTemp		数据缓冲区首地址
 *			TempLen		缓冲区长度（建议大于12k）
 *			LcdW		缩放后图形宽度
 *			LcdH		缩放后图像高度
 *			pOutBuf		图像输出缓冲区首地址
 *
 * 返回:	JpegDecodeReturn_en
 **************************************************************************/
JpegDecodeReturn_en Jpeg_DecodeFile(char *FileName,U8 *pTemp,U32 TempLen,U16 LcdW,U16 LcdH,U8 *pOutBuf);

// idct.c 中实现
void JPEGDec_idctInt (int* coef_block,U8 *output_buf); 

// JpegDisplay.c  中实现                             
void displayImageInformation(void);

#endif

