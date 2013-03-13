#ifndef QRCODE_API_H
#define QRCODE_API_H


#define QRCODE_SUCCESS 0
#define QRCODE_ERROR -1
#define QRCODE_UNKNOWN_CODE -2


typedef enum qrcode_pic_type
{
	QRCODE_PIC_TYPE_Y=0,
	QRCODE_PIC_TYPE_RGB888
}QRCODE_PIC_TYPE;


typedef enum qrcode_opt_type
{
	QRCODE_OPT_TYPE_ECC_TYPE = 0,
	QRCODE_OPT_TYPE_VERSION,
}QRCODE_OPT_TYPE;


typedef struct qrcode_param
{
	void		*(*pMalloc)(U32 uSize);
	void		(*pFree)(void *pBuf);
}QRCODE_PARAM;


typedef struct qrcode_pic
{
	QRCODE_PIC_TYPE eType;	

	U16			uWidth;
	U16			uHeight;

	void		*pData;
}QRCODE_PIC;


typedef struct qrcode_code
{
	U8			*pBuf;
	U32			uLen;
}QRCODE_CODE;


HANDLE QRCode_OpenDec(QRCODE_PARAM *pParam);
HANDLE QRCode_OpenEnc(QRCODE_PARAM *pParam);
int QRCode_Decode(HANDLE hQRC,QRCODE_PIC *pPic,QRCODE_CODE *pCode);
int QRCode_Encode(HANDLE hQRC,QRCODE_CODE *pCode,QRCODE_PIC *pPic);
int QRCode_SetOpt(HANDLE hQRC,QRCODE_OPT_TYPE eType,void *pParam);
void QRCode_Close(HANDLE hQRC);



#endif //QRCODE_API_H
