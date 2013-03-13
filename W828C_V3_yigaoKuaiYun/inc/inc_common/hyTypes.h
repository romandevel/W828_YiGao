/*************************************************************
%
% Filename     : hyTypes.h
% Project name : basic type definition for ARM platform
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040106  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the basic data type definition  
%
****************************************************************/

#ifndef _HY_TYPE_DEFINITION_H
#define _HY_TYPE_DEFINITION_H


/*	第一部分：基础定义
	最一般形式的返回值，当没有定义错误的具体值时，可使用下面的定义 */
#define HY_OK       0         	/* Global success return status */
#define HY_ERROR    0xFFFFFFFF	/* Global error return status */

#define YES			1
#define NO			0


/*	第二部分：类型定义 */
/*	用于PC simulation时, vc 的环境会定义 _WIN32 除此之外，仅支持ARM平台 */
/* Boolean values */
/* #ifndef TRUE switch introduced to avoid compilation problems in the Visual studio enviroment*/
#ifndef TRUE
	#define TRUE	(1==1)
#endif

/*#ifndef FALSE switch introduced to avoid compilation problems in the Visual studio enviroment*/
#ifndef FALSE
	#define FALSE	(1==0)
#endif

/* #ifndef TRUE switch introduced to avoid compilation problems in the Visual studio enviroment*/
#ifndef True
#define True	TRUE
#endif

/*#ifndef FALSE switch introduced to avoid compilation problems in the Visual studio enviroment*/
#ifndef False
#define False   FALSE
#endif

#ifndef NULL
	#define NULL	((void *) 0)
#endif

#ifndef CONST	/* Const */
#define CONST  const
#endif 

#ifndef ASSERT
#define ASSERT(a,b)
#endif




/* 1.1：基本类型 */
#define TEXT	char
#define VOID	void
#define UCOUNT	unsigned short
#define ULONG	unsigned long
#define TINY	char

#define INT		int
#define ARGINT	int
#define COUNT	short
#define SHORT	short
#define BITS	short
#define BYTES	unsigned
#define LONG	long
#define DOUBLE	double
#define FILE	short
#define SFAST	register
#define FAST	register
#define GLOBAL
#define IMPORT	extern
#define INTERN	static
#define LOCAL	static


typedef signed char			S8,		*pS8,    Int8,   *pInt8;     /*  8 bit   signed integer */
typedef unsigned char		U8,		*pU8,    UInt8,  *pUInt8;    /*  8 bit unsigned integer */
typedef short				S16,	*pS16,   Int16,  *pInt16;    /* 16 bit   signed integer */
typedef int					S32,	*pS32,   Int32,  *pInt32;    /* 32 bit   signed integer */
typedef unsigned short		U16,	*pU16,   UInt16, *pUInt16;   /* 16 bit unsigned integer */
typedef unsigned int		U32,	*pU32,   UInt32, *pUInt32;   /* 32 bit unsigned integer */
typedef void            	Void,   *pVoid;     /* Void (typeless) */
typedef float           	Float,  *pFloat;    /* 32 bit floating point */
typedef double          	Double, *pDouble;   /* 32/64 bit floating point */
typedef char            	Char,   *pChar;     /* character, character array ptr */
typedef char           		*String, **pString; /* Null terminated 8 bit char str, */
typedef char const     		*ConstString;		/* Null term 8 bit char str ptr */
typedef void 		  		(* PFN)(void);      /* PFN is pointer to function */

typedef void *HANDLE;

#if	defined(_WIN32)
	typedef int BOOL;
	typedef unsigned char  BOOLEAN; 
#else
	#ifdef BOOL
	#undef BOOL
	#endif
	
	typedef  unsigned char	BOOL;
	
	#ifndef Bool
	typedef  int  	Bool,   *pBool;     /* Boolean (True/False) */
	#endif
#endif

#if	defined(_WIN32)
	typedef unsigned long int	U64,	*pU64;
#else
	typedef unsigned long long	U64,	*pU64, UInt64, *pUInt64;
#endif



/* 2.2 宏定义*/
#define isalpha(c)	(islower(c) || isupper(c))
#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define isupper(c)	('A' <= (c) && (c) <= 'Z')
#define iswhite(c)	((c) <= ' ' || 0177 <= (c))
#define tolower(c)	(isupper(c) ? ((c) + ('a' - 'A')) : (c))
#define toupper(c)	(islower(c) ? ((c) - ('a' - 'A')) : (c))
#define isspace(c)	((c) == ' ' || (c) == '\t' || (c) == '\n' )

#ifndef MAX		/* The larger of x and y */
	#define MAX(x, y)	((x) > (y) ? (x) : (y))
#endif
#ifndef max	
	#define max(x, y)	(((x) < (y)) ? (y) : (x))
#endif

#ifndef MIN		/* The smaller of x and y */
	#define MIN(x, y)	((x) < (y) ? (x) : (y))
#endif
#ifndef min
	#define min(x, y)	(((x) < (y)) ? (x) : (y))
#endif

#ifndef ABS		/* The absolute value of x */
	#define ABS(x)		(((x) > 0) ? (x) : -(x))
#endif
#ifndef abs
	#define abs(x)		((x) < 0 ? -(x) : (x))
#endif

#ifndef LIMIT	/* The value x or the bound y or z if outside them */
	#define LIMIT(x, y, z)	MAX(MIN((x), MAX((y),(z))), MIN((y),(z)))
#endif

#ifndef LOW_BYTE
	#define LOW_BYTE(word)      (   word & 0x00ff )
#endif

#ifndef HIGH_BYTE
	#define HIGH_BYTE(word)     ( ( word & 0xff00 ) >> 8 )
#endif


/* 2.3，和硬件有关的一些预定义*/
#ifndef BIT0
#define BIT0  (0x00000001)            
#define BIT1  (0x00000002)            
#define BIT2  (0x00000004)            
#define BIT3  (0x00000008)      
#define BIT4  (0x00000010)        
#define BIT5  (0x00000020)        
#define BIT6  (0x00000040)        
#define BIT7  (0x00000080)            
#define BIT8  (0x00000100)            
#define BIT9  (0x00000200)            
#define BIT10 (0x00000400)            
#define BIT11 (0x00000800)            
#define BIT12 (0x00001000)            
#define BIT13 (0x00002000)            
#define BIT14 (0x00004000)            
#define BIT15 (0x00008000)            
#define BIT16 (0x00010000)           
#define BIT17 (0x00020000)           
#define BIT18 (0x00040000)           
#define BIT19 (0x00080000)           
#define BIT20 (0x00100000)           
#define BIT21 (0x00200000)       
#define BIT22 (0x00400000)            
#define BIT23 (0x00800000)            
#define BIT24 (0x01000000)            
#define BIT25 (0x02000000)            
#define BIT26 (0x04000000)           
#define BIT27 (0x08000000)           
#define BIT28 (0x10000000)           
#define BIT29 (0x20000000)           
#define BIT30 (0x40000000)           
#define BIT31 (0x80000000)       
	
	/* 定义整数中的bit数 */
	#define BITS_INT 32
#endif /*#ifndef BIT0 */

/* 全局的使能/禁止枚举类型 enable / disable  */
typedef enum Enable_en
{
	EnableOff,
	EnableOn
} Enable_en;

/* 全局的通道选择枚举类型 */
typedef enum Channel_en
{
	leftChannel,
	rightChannel,
	bothChannel
} Channel_en;


#endif /* ARMTYPE_DEFINITION_H */
