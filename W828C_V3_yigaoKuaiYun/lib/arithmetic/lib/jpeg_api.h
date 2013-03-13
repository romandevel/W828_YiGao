/************************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved	*              
*																					*
*  This source code and any compilation or derivative thereof is the sole      		*
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 	*
*  to a Software License Agreement.  This code is the proprietary information of    * 
*  Hyctron and is confidential in nature.  Its use and dissemination by    			*
*  any party other than Hyctron is strictly limited by the confidential information *
*  provisions of the Agreement referenced above. 									*
*************************************************************************************/

#ifndef JPEGAPI_H
#define JPEGAPI_H

/***example**************************************************************************

	tJPEG_API tJpeApi;
	
	if(解码)
	{
		memset(&tJpegApi,0,sizeof(tJPEG_API));//使用前应当清零结构体！！！
		
		tJpegApi.Func=Jpeg_Decode;
		tJpegApi.JpegFile=TRUE;
		tJpegApi.ReadFromFile=TRUE;
		tJpegApi.ScaleImage=是否缩放，TRUE/FALSE;
		tJpegApi.PreViewImage=是否优先解预览图，TRUE/FALSE;
		tJpegApi.pFileName=(U8 *)"文件名";
		tJpegApi.ReadOffset=0;
		tJpegApi.pOutputBuf=(U8 *)输出图像缓冲区地址;
		tJpegApi.OutputBufLen=输出图像缓冲区长度，必须能装下整个图像;
		tJpegApi.pTempBuf=(U8 *)SRAM工作缓冲区地址;
		tJpegApi.TempBufLen=SRAM工作缓冲区长度，16K Bytes;
		
		if(tJpegApi.ScaleImage==TRUE)
		{
			tJpegApi.pSdramBuf=(U8 *)Sdram工作缓冲区地址;
			tJpegApi.SdramBufLen=Sdram工作缓冲区长度，256K Bytes;		
			tJpegApi.ScrWidth=缩放后宽度;
			tJpegApi.ScrHeight=缩放后高度;
		}

		if(Jpeg_Codec(&tJpegApi)==Jpeg_Success)
		{
			//解码成功
			...
		}
		else
		{
			//错误处理
			...	
		}
	}
	else if(编码)
	{
		memset(&tJpegApi,0,sizeof(tJPEG_API));//使用前应当清零结构体！！！
		
		tJpegApi.Func=Jpeg_Encode;
		tJpegApi.ImageLevel=图像质量共5级，建议使用Jpeg_High;
		tJpegApi.JpegFile=TRUE;
		tJpegApi.EncodeFormat=输入数据格式选择，JPEG_FORMAT_RGB565/JPEG_FORMAT_YUV422;
		tJpegApi.ReadOffset=0;
		tJpegApi.pInputBuf=(U8 *)输入数据地址，待编码数据;
		tJpegApi.InputBufLen=输入数据长度;
		tJpegApi.pOutputBuf=(U8 *)输出缓冲区地址，编码后数据;
		tJpegApi.OutputBufLen=输出缓冲区长度;
		tJpegApi.pTempBuf=(U8 *)SRAM工作缓冲区地址;
		tJpegApi.TempBufLen=SRAM工作缓冲区长度，24K Bytes;
		tJpegApi.pSdramBuf=(U8 *)Sdram工作缓冲区地址;
		tJpegApi.SdramBufLen=Sdram工作缓冲区长度，256K Bytes;
		tJpegApi.ImageWidth=输入图像宽度，必须为16的倍数;
		tJpegApi.ImageHeight=输入图像高度，必须为16的倍数;

		if(Jpeg_Codec(&tJpegApi)==Jpeg_Success)
		{
			//解码成功
			//tJpegApi.OutputDataLen为输出缓冲区中有效数据的长度
			fwrite(tJpegApi.pOutputBuf,1,tJpegApi.OutputDataLen,pfile);//保存文件
			...
		}
		else
		{
			//错误处理
			...
		}
	}
		
****end of example*******************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/***variable definition**************************************************************/

//执行功能
//tJPEG_API.Func
#define Jpeg_Decode 0
#define Jpeg_Encode 1
#define Jpeg_GetSize 2

//编码质量
//tJPEG_API.ImageLevel
#define Jpeg_Low 0
#define Jpeg_Standard 1
#define Jpeg_High 2
#define Jpeg_Vivid 3
#define Jpeg_Original 4

//输入数据格式
//tJPEG_API.EncodeFormat
#define JPEG_FORMAT_RGB565 1
#define JPEG_FORMAT_YUV422 2
#define JPEG_FORMAT_YUV420 3
	
typedef enum  
{
	Jpeg_Success=0,
	Jpeg_error,
	Jpeg_Nofile,
	Jpeg_ReadfileError,
	Jpeg_NotJpegfile,	
	Jpeg_UnSupportJpegfile,
	Jpeg_BadJpegfile,
	Jpeg_BadQuantilizationID,
	Jpeg_BadDataPrecision,
	Jpeg_BadImageWidthorHeight,
	Jpeg_BadComponentInfo,
	Jpeg_BadHuffmanTable,
	Jpeg_InsufficientMemory	,
	Jpeg_ComponentNot3,
	Jpeg_UserBreak,
	Jpeg_NoThumbnail
}JpegReturn_en;


typedef struct tjpeg_api
{
	U8			Func;			//执行功能
	U8			ImageLevel;		//编码质量，共5级，相关功能:Encode
	U8			EncodeFormat;	//编码输入数据格式(RGB565/YUV422)
	BOOL		JpegFile;		//是否为jpeg文件，相关功能:Decode,Encode
	BOOL		ReadFromFile;	//是否从文件读取数据，相关功能:Decode,GetSize
	BOOL		ScaleImage;		//是否需要图形缩放，相关功能:Decode
	BOOL		PreViewImage;	//是否只解预览图，1为只解预览图，0为直接解大图

	U8			*pFileName;		//文件名，相关功能:JpegFile
#ifdef PLATFORM_PC
	FILE		*FileHandle;	//文件句柄，相关功能:!JpegFile && ReadFromFile
#else
	int			FileHandle;		//文件句柄，相关功能:!JpegFile && ReadFromFile
#endif

	U32			ReadOffset;		//读偏移地址，相关功能:Decode,Encode,GetSize

	U8			*pInputBuf;		//输入缓冲区，相关功能:Encode || !ReadFromFile
	U32			InputBufLen;	//输入缓冲区长度，相关功能:Encode || !ReadFromFile
	U8			*pOutputBuf;	//输出缓冲区，相关功能:Decode,Encode
	U32			OutputBufLen;	//输出缓冲区长度，相关功能:Decode,Encode
	U32			OutputDataLen;	//输出数据长度，相关功能:Encode
	U8			*pTempBuf;		//工作缓冲区，相关功能:Decode,Encode
	U32			TempBufLen;		//工作缓冲区长度，相关功能:Decode,Encode
	U8			*pSdramBuf;
	U32			SdramBufLen;

	U16			ImageWidth;		//图像宽度，相关功能:Encode || GetSize
	U16			ImageHeight;	//图像高度，相关功能:Encode || GetSize
	U16			ScrWidth;		//屏幕宽度，相关功能:ScaleImage
	U16			ScrHeight;		//屏幕高度，相关功能:ScaleImage
}tJPEG_API;

/***end of variable definition*******************************************************/


/***function definition**************************************************************/

/************************************************************************************
*function:		Jpeg_Codec            
*
*description:	decode & encode interface method
*
*parameter:		pJpegApi	parameter struction
*
*return value:	JpegReturn_en
*************************************************************************************/
JpegReturn_en Jpeg_Codec(struct tjpeg_api *pJpegApi);
JpegReturn_en Jpeg_HWCodec(struct tjpeg_api *pJpegApi,U8 *pBuf,U32 BufLen);

/***end of function definition*******************************************************/

#ifdef __cplusplus
}
#endif

#endif

