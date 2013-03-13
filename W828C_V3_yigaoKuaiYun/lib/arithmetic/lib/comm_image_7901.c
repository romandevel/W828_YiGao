/*由于tcc8300_comm_debug.o中的2个函数直接使用导致图像异常,怀疑是库中函数没更新,
还是使用的8300的硬件寄存器
现增加2个函数
*/
#include "hyTypes.h"


#define WRITE_REG32(pAddr,Value) (*((volatile U32 *)(pAddr))=(U32)(Value))
#define READ_REG32(pAddr) (*((volatile U32 *)(pAddr)))
#define SET_REG(pAddr,Mask,Value) ( *((volatile U32 *)(pAddr)) = (*((volatile U32 *)(pAddr)) & ~(Mask)) | (Value) )

#define HWADDR(Addr) ( (U32)hsaConvertUnCacheMemory((void *)(Addr)) )

#define SCALER_BASE				0xF0070000 //Scaler Registers Base Address
#define SCALER_SRC_Y_BASE		(SCALER_BASE+0x000) //Scaler source image Y base address register, R/W
#define SCALER_SRC_U_BASE		(SCALER_BASE+0x004) //Scaler source image U base address register, R/W
#define SCALER_SRC_V_BASE		(SCALER_BASE+0x008) //Scaler source image V base address register, R/W
#define SCALER_SRC_SIZE			(SCALER_BASE+0x00c) //Source image size register, R/W

#define SCALER_SRC_OFFSET		(SCALER_BASE+0x010) //Source image line offset register, R/W
#define SCALER_SRC_CONFIG		(SCALER_BASE+0x014) //Source image configuration register, R/W

#define SCALER_DST_Y_BASE		(SCALER_BASE+0x020) //Scaler destination image Y base address register, R/W
#define SCALER_DST_U_BASE		(SCALER_BASE+0x024) //Scaler destination image U base address register, R/W
#define SCALER_DST_V_BASE		(SCALER_BASE+0x028) //Scaler destination image V base address register, R/W
#define SCALER_DST_SIZE			(SCALER_BASE+0x02c) //Destination image size register, R/W

#define SCALER_DST_OFFSET		(SCALER_BASE+0x030) //Destination image line offset register, R/W
#define SCALER_DST_CONFIG		(SCALER_BASE+0x034) //Destination image configuration register, R/W

#define SCALER_SCALE_RATIO		(SCALER_BASE+0x040) //Scale ratio register, R/W
#define SCALER_SCALE_CTRL		(SCALER_BASE+0x044) //Scaler control register, R/W
#define SCALER_STATUS			(SCALER_BASE+0x048) //Scaler status register, R

#define SCALER_DST_RM_CNT		(SCALER_BASE+0x50) //Destination rolling(middle) line counter, R/W
#define SCALER_C_ROLL_CNT		(SCALER_BASE+0x54) // Current Rolling line counter, R



#define GE_BASE					0xF6000000 //Graphic Engine Base Address

#define GE_FCH0_SADDR0			(GE_BASE+0x00) //Front-End Channel 0 Source Address 0, R/W
#define GE_FCH0_SADDR1			(GE_BASE+0x04) //Front-End Channel 0 Source Address 1, R/W
#define GE_FCH0_SADDR2			(GE_BASE+0x08) //Front-End Channel 0 Source Address 2, R/W
#define GE_FCH0_SFSIZE			(GE_BASE+0x0C) //Front-End Channel 0 Source Frame Pixel Size, R/W

#define GE_FCH0_SOFF			(GE_BASE+0x10) //Front-End Channel 0 Source Pixel Offset, R/W
#define GE_FCH0_SISIZE			(GE_BASE+0x14) //Front-End Channel 0 Source Image Pixel Size, R/W
#define GE_FCH0_WOFF			(GE_BASE+0x18) //Front-End Channel 0 Window Pixel Offset, R/W
#define GE_FCH0_SCTRL			(GE_BASE+0x1C) //Front-End Channel 0 Control, R/W

#define GE_FCH1_SADDR0			(GE_BASE+0x20) //Front-End Channel 1 Source Address 0, R/W
#define GE_FCH1_SADDR1			(GE_BASE+0x24) //Front-End Channel 1 Source Address 1, R/W
#define GE_FCH1_SADDR2			(GE_BASE+0x28) //Front-End Channel 1 Source Address 2, R/W
#define GE_FCH1_SFSIZE			(GE_BASE+0x2C) //Front-End Channel 1 Source Frame Pixel Size, R/W

#define GE_FCH1_SOFF			(GE_BASE+0x30) //Front-End Channel 1 Source Pixel Offset, R/W
#define GE_FCH1_SISIZE			(GE_BASE+0x34) //Front-End Channel 1 Source Image Pixel Size, R/W
#define GE_FCH1_WOFF			(GE_BASE+0x38) //Front-End Channel 1 Window Pixel Offset, R/W
#define GE_FCH1_SCTRL			(GE_BASE+0x3C) //Front-End Channel 1 Control, R/W

#define GE_S0_CHROMA			(GE_BASE+0x60) //Source 0 Chroma-Key Parameter, R/W
#define GE_S0_PAR				(GE_BASE+0x64) //Source 0 Arithmetic Parameter, R/W
#define GE_S1_CHROMA			(GE_BASE+0x68) //Source 1 Chroma-Key Parameter, R/W
#define GE_S1_PAR				(GE_BASE+0x6C) //Source 1 Arithmetic Parameter, R/W

#define GE_S_CTRL				(GE_BASE+0x78) //Source Control Register, R/W

#define GE_OP0_PAT				(GE_BASE+0x80) //Source Operator 0 Pattern, R/W
#define GE_OP_CTRL				(GE_BASE+0x88) //Source Operation Control Register, R/W

#define GE_BCH_DADDR0			(GE_BASE+0x90) //Back-End Channel Destination Address 0, R/W
#define GE_BCH_DADDR1			(GE_BASE+0x94) //Back -End Channel Destination Address 1, R/W
#define GE_BCH_DADDR2			(GE_BASE+0x98) //Back -End Channel Destination Address 2, R/W
#define GE_BCH_DFSIZE			(GE_BASE+0x9C) //Back -End Channel Destination Frame Pixel Size, R/W

#define GE_BCH_DOFF				(GE_BASE+0xA0) //Back -End Channel Destination Pixel Offset, R/W
#define GE_BCH_DCTRL			(GE_BASE+0xA4) //Back -End Channel Control, R/W

#define GE_CTRL					(GE_BASE+0xB0) //R/W Graphic Engine Control, R/W
#define GE_IREQ					(GE_BASE+0xB4) //R/W Graphic Engine Interrupt Request, R/W

/*--------------------------------------------------------------------------------------------*/

void Comm_HWScaler_YUV420_t(U32 *pSrcY,U32 *pSrcU,U32 *pSrcV,U32 *pDstY,U32 *pDstU,U32 *pDstV,
							U32 SrcFrameYX,U32 DstFrameYX)
{
	U32 ratio,SrcOffsetYX,DstOffsetYX;
	
	hyc_cache_write_back();
	
	SrcOffsetYX=SrcFrameYX & 0xffff;
	SrcOffsetYX=((SrcOffsetYX>>1)<<16) | SrcOffsetYX;
	DstOffsetYX=DstFrameYX & 0xffff;
	DstOffsetYX=((DstOffsetYX>>1)<<16) | DstOffsetYX;

	//src
	WRITE_REG32(SCALER_SRC_Y_BASE,HWADDR(pSrcY));
	WRITE_REG32(SCALER_SRC_U_BASE,HWADDR(pSrcU));
	WRITE_REG32(SCALER_SRC_V_BASE,HWADDR(pSrcV));
	
	WRITE_REG32(SCALER_SRC_SIZE,SrcFrameYX);
	WRITE_REG32(SCALER_SRC_OFFSET,SrcOffsetYX);
	WRITE_REG32(SCALER_SRC_CONFIG,0x00000003);//YUV420
	
	//dst
	WRITE_REG32(SCALER_DST_Y_BASE,HWADDR(pDstY));
	WRITE_REG32(SCALER_DST_U_BASE,HWADDR(pDstU));
	WRITE_REG32(SCALER_DST_V_BASE,HWADDR(pDstV));

	WRITE_REG32(SCALER_DST_SIZE,DstFrameYX);
	WRITE_REG32(SCALER_DST_OFFSET,DstOffsetYX);
	WRITE_REG32(SCALER_DST_CONFIG,0x00000003);//YUV420
	
	//config
	ratio=((SrcFrameYX>>16)<<8)/(DstFrameYX>>16); //Vertical scale ration ( = 256*SRC_VSIZE/DST_VSIZE)
	ratio<<=16;
	ratio|=((SrcFrameYX&0xffff)<<8)/(DstFrameYX&0xffff); //Horizontal scale ratio ( = 256*SRC_HSIZE/DST_HSIZE)
	
	WRITE_REG32(SCALER_SCALE_RATIO,ratio);
	WRITE_REG32(SCALER_SCALE_CTRL,0x00000001);

	while(!(READ_REG32(SCALER_STATUS) & 0x00000001));

	return;
}


void Color_YUV420toRGB565_Buf_t(U32 *pY,U32 *pU,U32 *pV,U32 *pRGB,U32 SrcYX,U32 SrcOffYX,U32 ImageYX,U32 DstYX,U32 DstOffYX)
{
	//check input parameter
	
	hyc_cache_write_back();
		
	//wait if Graphic Engine is busy
	
	WRITE_REG32(GE_CTRL,0);//disable Graphic Engine
	
	//clear flag
	WRITE_REG32(GE_IREQ,0x00010000);
	
	//source address
	WRITE_REG32(GE_FCH0_SADDR0,HWADDR(pY));
	WRITE_REG32(GE_FCH0_SADDR1,HWADDR(pU));
	WRITE_REG32(GE_FCH0_SADDR2,HWADDR(pV));
	
	//source frame size
	WRITE_REG32(GE_FCH0_SFSIZE,SrcYX);

	//source frame offset size
	WRITE_REG32(GE_FCH0_SOFF,SrcOffYX);

	//source image size
	WRITE_REG32(GE_FCH0_SISIZE,ImageYX);

	//source control
	WRITE_REG32(GE_FCH0_SCTRL,0x00000003);//YUV420, copy data

	WRITE_REG32(GE_S_CTRL,0x00010001);//source0=front-end0, enable YUV convert
		
	//source operation control
	WRITE_REG32(GE_OP_CTRL,0x00060006);//source copy
	
	//destination address
	WRITE_REG32(GE_BCH_DADDR0,HWADDR(pRGB));
	
	//destination frame size
	WRITE_REG32(GE_BCH_DFSIZE,DstYX);

	//destination frame offset size
	WRITE_REG32(GE_BCH_DOFF,DstOffYX);

	//destination control
	WRITE_REG32(GE_BCH_DCTRL,0x0000000e);//RGB565, copy data
	
	//general control
	WRITE_REG32(GE_CTRL,0x00000001);//Graphic Engine Enable With Front-End Channel 0

	while(!(READ_REG32(GE_IREQ) & 0x00010000));

	//clear flag
	WRITE_REG32(GE_IREQ,0x00010000);
	
	return;
}

