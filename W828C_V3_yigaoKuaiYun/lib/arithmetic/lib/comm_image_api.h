#ifndef COMM_IMAGE_API_H
#define COMM_IMAGE_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/***example**************************************************************************

	//DC效果使用说明
	U16 *pBuf,*pCmos,*pOut;
	int i;
	
	//使用前必须先初始化数据表，pBuf指向128k bytes缓冲区
	Comm_Image_ProcInit(pBuf,0,0,COMM_IMAGE_NORMAL);
	
	
	//pCmos为Cmos Sensor输出图像缓冲区
	//pOut为图像存放地址
	//Width为捕捉的图像宽度，Height为图像高度
	
	if(预览模式)
	{
		//预览图为640x240
		for(i=0;i<240;i++)
		{
			Comm_Image_640x1_to_320x1(pCmos+i*640,pOut+i*320);
		}
	}
	else(拍摄模式)
	{
		Comm_Image_Process(pCmos,Width,Height);
	}
	
	if(改变效果)
	{
		Comm_Image_ProcInit(pBuf,亮度调节,对比度调节,效果值);
	}
		
****end of example*******************************************************************/


/***macro definition*****************************************************************/

//Comm_Image_ProcInit.Type
//DC效果
#define COMM_IMAGE_NORMAL 0
#define COMM_IMAGE_OLD_PHOTO 1
#define COMM_IMAGE_GRAY 2
#define COMM_IMAGE_RED 3
#define COMM_IMAGE_GREEN 4
#define COMM_IMAGE_BLUE 5
#define COMM_IMAGE_NEGATIVE 6

/***end of macro definition**********************************************************/


/***variable definition**************************************************************/
typedef struct comm_image_scale
{
	U16        *inputbuf;   //输入图像缓冲区指针
	U16        inbufwidth;  //输入图像缓冲区宽度
	U16        inbufheight; //输入图像缓冲区高度
	U16        inwidth;     //输入图像宽度
	U16        inheight;    //输入图像高度
	U32        inbufaddr;   //输入图像在缓冲区的起始位置
	U16        *oputbuf;    //输出图像缓冲区指针
	U16        obufwidth;   //输出图像缓冲区宽度
	U16        obufheight;  //输出图像缓冲区高度
	U16        owidth;      //输出图像宽度
	U16        oheight;     //输出图像高度
	U32        obufaddr;    //输出图像在缓冲区的起始位置(与旋转标志有关)
	BOOL       out_rotate;  //输出图像旋转标志
}COMM_IMAGE_SCALE;
/***end of variable definition*******************************************************/


/***function definition**************************************************************/

/************************************************************************************
*function:		Comm_Image_1280x1024to1600x1280_YUV            
*
*description:	scale image from 1280x1024 to 1600x1280
*				Transfer 4 input lines from 1280x1 to 1600x1 firstly, and store temporary
*				data in SRAM. Then transfer 1600x4 data block to 1600x5 and save data
*				in output buffer. Loop until all input data are done.
*
*				Be Attention, input buffer address should larger than ouput buffer
*				address 1440K bytes at least, otherwise input image will be currupted !!!
*
*parameter:		pInput		1280x1024, YUYV442
*				pOutput		1600x1280, YUYV442
*				pTempBuf	5*1600*2 bytes, store temporary data 
*
*return value:	0			success
*				-1			error			
*************************************************************************************/
int Comm_Image_1280x1024to1600x1280_YUV(U16 *pInput,U16 *pOutput,U16 *pTempBuf);

/************************************************************************************
*function:		Comm_Image_1280x1024to1600x1280            
*
*description:	scale image from 1280x1024 to 1600x1280
*				Transfer 4 input lines from 1280x1 to 1600x1 firstly, and store temporary
*				data in SRAM. Then transfer 1600x4 data block to 1600x5 and save data
*				in output buffer. Loop until all input data are done.
*
*				Be Attention, input buffer address should larger than ouput buffer
*				address 1440K bytes at least, otherwise input image will be currupted !!!
*
*parameter:		pInput		1280x1024, RGB565
*				pOutput		1600x1280, RGB565
*				pTempBuf	5*1600*2 bytes, store temporary data 
*
*return value:	0			success
*				-1			error			
*************************************************************************************/
int Comm_Image_1280x1024to1600x1280(U16 *pInput,U16 *pOutput,U16 *pTempBuf);

/************************************************************************************
*function:		Comm_Image_ScaleSub            
*
*description:	scale image data block according to required size
*
*parameter:		pScaleInfo		reference the struct define of comm_image_scale
*				pTempBuf1		image width * sizeof(U16)	
*				pTempBuf2		image width * sizeof(U8)
*				
*return value:	none
*notice:       output image size >=input image size/2
*************************************************************************************/
void Comm_Image_ScaleSub(COMM_IMAGE_SCALE *pScaleInfo,U16 *pTempBuf1,U8 *pTempBuf2);

/************************************************************************************
*function:		Comm_Image_Zoom                                                    
*                                                                                  
*description:	Zoom in the input image according to the zoom ratio					
*																					
*parameter:		pInputBuf	input image data block									
*				pOutputBuf	output image data block									
*               pTempBuf	temp buffer for	Jpeg_ScaleImage_sub,the size is width*3									*
*				Ratio	    zoom ratio(2,3,4,5,6)								
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	0    Success
*               -1   Error(超出变焦范围,输出buffer中没有数据,输入buffer中的数据有效)																
*************************************************************************************/
int Comm_Image_Zoom(U16 *pInputBuf,U16 *pOutputBuf,U8 *pTempBuf,U8 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_ProcInit                                                   
*                                                                                   
*description:	compute the image process table							
*																					
*parameter:		pBuf        point to table buffer, 128k bytes
*				LRatio	    light adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				CRatio      contrast adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0								
*	            Type        reference the macro definition
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_ProcInit(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Type);

/************************************************************************************
*function:		Comm_Image_640x1_to_320x1                                                   
*                                                                                   
*description:	scale and process image
*																					
*parameter:		pInput      input image buffer
*				pOutput	    output image buffer
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_640x1_to_320x1(U16 *pInput,U16 *pOutput);

/************************************************************************************
*function:		Comm_Image_Process                                                   
*                                                                                   
*description:	process image
*																					
*parameter:		pBuf      point to image buffer
*				Width	  width of image
*				Height	  height of image
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_Process(U16 *pBuf,U32 Width,U32 Height);

/************************************************************************************
*function:		Comm_Image_LightAdjust                                                        
*                                                                                   
*description:	Adjust the input image's light										
*																					
*parameter:		pBuf		input(output) buffer
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_LightAdjust(U16 *pBuf,S16 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_ContrastAdjust                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_ContrastAdjust(U16 *pBuf,S16 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Emboss                                                    
*                                                                                   
*description:	Emboss the input image							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Emboss(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Oldphoto                                                    
*                                                                                   
*description:	Aging the input image to old 							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Oldphoto(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Grayimage                                                    
*                                                                                   
*description:	transform the RGB565 image into gray image							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Grayimage(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_normal                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_normal(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_old                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_old(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_gray                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_gray(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/***end of function definition*******************************************************/


#ifdef __cplusplus
}
#endif

#endif