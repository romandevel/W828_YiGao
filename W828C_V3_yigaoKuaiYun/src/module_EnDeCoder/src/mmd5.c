#include "hyTypes.h"


//Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

//F, G, H and I are basic MD5 functions
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//ROTATE_LEFT rotates x left n bits
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
//Rotation is separate from addition to prevent recomputation
#define FF(a, b, c, d, x, s, ac) \
{\
	(a) += F((b), (c), (d)) + (x) + (U32)(ac);\
	(a) = ROTATE_LEFT((a), (s));\
	(a) += (b);\
}

#define GG(a, b, c, d, x, s, ac) \
{\
	(a) += G((b), (c), (d)) + (x) + (U32)(ac);\
	(a) = ROTATE_LEFT((a), (s));\
	(a) += (b);\
}

#define HH(a, b, c, d, x, s, ac) \
{\
	(a) += H((b), (c), (d)) + (x) + (U32)(ac);\
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b);\
}

#define II(a, b, c, d, x, s, ac) \
{\
	(a) += I((b), (c), (d)) + (x) + (U32)(ac);\
	(a) = ROTATE_LEFT((a), (s));\
	(a) += (b);\
}

#define MD5_MAGIC0 0x67452301
#define MD5_MAGIC1 0xefcdab89
#define MD5_MAGIC2 0x98badcfe
#define MD5_MAGIC3 0x10325476
  

static void MD5_Transform(U32 *pState,U32 *pBlock)
{
	U32 a,b,c,d;
	
	a=pState[0];
	b=pState[1];
	c=pState[2];
	d=pState[3];

	//Round 1
	FF(a, b, c, d, pBlock[ 0], S11, 0xd76aa478);//1
	FF(d, a, b, c, pBlock[ 1], S12, 0xe8c7b756);//2
	FF(c, d, a, b, pBlock[ 2], S13, 0x242070db);//3
	FF(b, c, d, a, pBlock[ 3], S14, 0xc1bdceee);//4
	FF(a, b, c, d, pBlock[ 4], S11, 0xf57c0faf);//5
	FF(d, a, b, c, pBlock[ 5], S12, 0x4787c62a);//6
	FF(c, d, a, b, pBlock[ 6], S13, 0xa8304613);//7
	FF(b, c, d, a, pBlock[ 7], S14, 0xfd469501);//8
	FF(a, b, c, d, pBlock[ 8], S11, 0x698098d8);//9
	FF(d, a, b, c, pBlock[ 9], S12, 0x8b44f7af);//10
	FF(c, d, a, b, pBlock[10], S13, 0xffff5bb1);//11
	FF(b, c, d, a, pBlock[11], S14, 0x895cd7be);//12
	FF(a, b, c, d, pBlock[12], S11, 0x6b901122);//13
	FF(d, a, b, c, pBlock[13], S12, 0xfd987193);//14
	FF(c, d, a, b, pBlock[14], S13, 0xa679438e);//15
	FF(b, c, d, a, pBlock[15], S14, 0x49b40821);//16

	//Round 2
	GG(a, b, c, d, pBlock[ 1], S21, 0xf61e2562);//17
	GG(d, a, b, c, pBlock[ 6], S22, 0xc040b340);//18
	GG(c, d, a, b, pBlock[11], S23, 0x265e5a51);//19
	GG(b, c, d, a, pBlock[ 0], S24, 0xe9b6c7aa);//20
	GG(a, b, c, d, pBlock[ 5], S21, 0xd62f105d);//21
	GG(d, a, b, c, pBlock[10], S22, 0x02441453);//22
	GG(c, d, a, b, pBlock[15], S23, 0xd8a1e681);//23
	GG(b, c, d, a, pBlock[ 4], S24, 0xe7d3fbc8);//24
	GG(a, b, c, d, pBlock[ 9], S21, 0x21e1cde6);//25
	GG(d, a, b, c, pBlock[14], S22, 0xc33707d6);//26
	GG(c, d, a, b, pBlock[ 3], S23, 0xf4d50d87);//27
	GG(b, c, d, a, pBlock[ 8], S24, 0x455a14ed);//28
	GG(a, b, c, d, pBlock[13], S21, 0xa9e3e905);//29
	GG(d, a, b, c, pBlock[ 2], S22, 0xfcefa3f8);//30
	GG(c, d, a, b, pBlock[ 7], S23, 0x676f02d9);//31
	GG(b, c, d, a, pBlock[12], S24, 0x8d2a4c8a);//32

	//Round 3
	HH(a, b, c, d, pBlock[ 5], S31, 0xfffa3942);//33
	HH(d, a, b, c, pBlock[ 8], S32, 0x8771f681);//34
	HH(c, d, a, b, pBlock[11], S33, 0x6d9d6122);//35
	HH(b, c, d, a, pBlock[14], S34, 0xfde5380c);//36
	HH(a, b, c, d, pBlock[ 1], S31, 0xa4beea44);//37
	HH(d, a, b, c, pBlock[ 4], S32, 0x4bdecfa9);//38
	HH(c, d, a, b, pBlock[ 7], S33, 0xf6bb4b60);//39
	HH(b, c, d, a, pBlock[10], S34, 0xbebfbc70);//40
	HH(a, b, c, d, pBlock[13], S31, 0x289b7ec6);//41
	HH(d, a, b, c, pBlock[ 0], S32, 0xeaa127fa);//42
	HH(c, d, a, b, pBlock[ 3], S33, 0xd4ef3085);//43
	HH(b, c, d, a, pBlock[ 6], S34, 0x04881d05);//44
	HH(a, b, c, d, pBlock[ 9], S31, 0xd9d4d039);//45
	HH(d, a, b, c, pBlock[12], S32, 0xe6db99e5);//46
	HH(c, d, a, b, pBlock[15], S33, 0x1fa27cf8);//47
	HH(b, c, d, a, pBlock[ 2], S34, 0xc4ac5665);//48

	//Round 4
	II(a, b, c, d, pBlock[ 0], S41, 0xf4292244);//49
	II(d, a, b, c, pBlock[ 7], S42, 0x432aff97);//50
	II(c, d, a, b, pBlock[14], S43, 0xab9423a7);//51
	II(b, c, d, a, pBlock[ 5], S44, 0xfc93a039);//52
	II(a, b, c, d, pBlock[12], S41, 0x655b59c3);//53
	II(d, a, b, c, pBlock[ 3], S42, 0x8f0ccc92);//54
	II(c, d, a, b, pBlock[10], S43, 0xffeff47d);//55
	II(b, c, d, a, pBlock[ 1], S44, 0x85845dd1);//56
	II(a, b, c, d, pBlock[ 8], S41, 0x6fa87e4f);//57
	II(d, a, b, c, pBlock[15], S42, 0xfe2ce6e0);//58
	II(c, d, a, b, pBlock[ 6], S43, 0xa3014314);//59
	II(b, c, d, a, pBlock[13], S44, 0x4e0811a1);//60
	II(a, b, c, d, pBlock[ 4], S41, 0xf7537e82);//61
	II(d, a, b, c, pBlock[11], S42, 0xbd3af235);//62
	II(c, d, a, b, pBlock[ 2], S43, 0x2ad7d2bb);//63
	II(b, c, d, a, pBlock[ 9], S44, 0xeb86d391);//64

	pState[0]+=a;
	pState[1]+=b;
	pState[2]+=c;
	pState[3]+=d;
	
	return;
}

/*
pData:待加密数据
Len:待加密数据长度
pCode:加密后的数据
*/
void mMD5_Encode(U8 *pData,U32 Len,U32 *pCode)
{
	int i;
	U32 code[4],buf[16];
	U32 bitlen1,bitlen0;
	
	//init
	code[0]=MD5_MAGIC0;
	code[1]=MD5_MAGIC1;
	code[2]=MD5_MAGIC2;
	code[3]=MD5_MAGIC3;
	
	bitlen1=Len >> 29;
	bitlen0=Len << 3;
	
	//encode
	if((U32)pData & 3)
	{
		while(Len>=64)
		{
			memcpy(buf,pData,64);
			MD5_Transform(code,buf);
			pData+=64;
			Len-=64;
		}	
	}
	else
	{
		while(Len>=64)
		{
			MD5_Transform(code,(U32 *)pData);
			pData+=64;
			Len-=64;
		}
	}
	
	memcpy(buf,pData,Len);
	if(Len<56)
	{
		((U8 *)buf)[Len]=0x80;
		for(i=Len+1;i<56;i++)
		{
			((U8 *)buf)[i]=0x00;
		}
		
		buf[14]=bitlen0;
		buf[15]=bitlen1;
		MD5_Transform(code,buf);
	}
	else
	{
		((U8 *)buf)[Len]=0x80;
		for(i=Len+1;i<64;i++)
		{
			((U8 *)buf)[i]=0x00;
		}
		MD5_Transform(code,buf);
		
		for(i=0;i<14;i++)
		{
			buf[i]=0x00;
		}
		buf[14]=bitlen0;
		buf[15]=bitlen1;
		MD5_Transform(code,buf);
	}	
	
	pCode[0]=code[0];
	pCode[1]=code[1];
	pCode[2]=code[2];
	pCode[3]=code[3];
	
	return;
}