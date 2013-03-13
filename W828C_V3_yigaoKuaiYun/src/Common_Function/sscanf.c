/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/


/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/
#include "stdarg.h"

/*------------------------------------------------------------------------------
   Types and defines:
------------------------------------------------------------------------------*/
#define SSCANF_EOF -1
#define SSCANF_TAB 9
#define SSCANF_NEW_LINE 10
#define SSCANF_END 0
#define NULL 0


/*-----------------------------------------------------------------------------
* 函数:	int_sscanf
* 功能:	从输入字符串缓存中按格式读入一个十进制或八进制的整型数据,将该整型数据存
*		储到一个整型或短整型变量当中
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pIntArg			整型或短整型数据存放地址
*		m				标明进制	10:十进制	8:八进制
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个十进制或八进制整型数据				 
*----------------------------------------------------------------------------*/
static int int_sscanf(char **pBuffer, int *pIntArg, int m, int FieldWidth, int ShortFlag, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		int int_a = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
			{
				int_a = int_a * m + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			else 
				break;
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			if (ShortFlag == 1) 
			{ 
				short sh_a;
				sh_a = int_a; 
				*(short *)pIntArg = sh_a; 
			}
			else if (ShortFlag == 2)
			{
				char ch_a;
				ch_a = int_a;
				*(char *)pIntArg = ch_a;
			}
			else 
				*pIntArg = int_a;
			return 1;
		}
	} 
	else 
	{
		int int_a = 0;
		
		while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
		{
			int_a = int_a * m + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 

		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			if (ShortFlag == 1) 
			{ 
				short sh_a;
				sh_a = int_a; 
				*(short *)pIntArg = sh_a; 
			}
			else if (ShortFlag == 2)
			{
				char ch_a;
				ch_a = int_a;
				*(char *)pIntArg = ch_a;
			}
			else 
				*pIntArg = int_a;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	int64_sscanf
* 功能:	从输入字符串缓存中按格式读入一个十进制或八进制的整型数据,将该整型数据存储到一个
*		64位long long型变量当中
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pI64Arg			64位long long型数据存放地址
*		m				标明进制	10:十进制	8:八进制
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个十进制或八进制整型数据				 
*----------------------------------------------------------------------------*/
static int int64_sscanf(char **pBuffer, __int64 *pI64Arg, int m, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		__int64 int_a = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
			{
				int_a = int_a * m + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			else 
				break;
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pI64Arg = int_a;
			return 1;
		}
	} 
	else 
	{
		__int64 int_a = 0;
		
		while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
		{
			int_a = int_a * m + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 

		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pI64Arg = int_a;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	xint_sscanf
* 功能:	从输入字符串缓存中按格式读入一个十六进制整型数据,将该整型数据存储到一个
*		整型或短整型变量当中
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pIntArg			整型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个十六进制整型数据				 
*----------------------------------------------------------------------------*/
static int xint_sscanf(char **pBuffer, int *pIntArg, int FieldWidth, int ShortFlag, int DisFlag)
{
	char *p = *pBuffer;
	int j, n, int_a = 0;

	if (FieldWidth) 
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0; 
	}
	
	while (j)
	{
	
		if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
		{
			int_a = int_a * 16 + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 
		else if ((**pBuffer >= 'a')&&(**pBuffer <= 'f'))
		{
			int_a = int_a * 16 + (**pBuffer - 'a' + 10); 
			(*pBuffer)++;
		}
		else if ((**pBuffer >= 'A')&&(**pBuffer <= 'F')) 
		{
			int_a = int_a * 16 + (**pBuffer - 'A' + 10); 
			(*pBuffer)++;
		} 
		else break;
		
		j -= n;
	}
	
	if (*pBuffer == p) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		if (ShortFlag == 1) 
		{ 
			short sh_a;
			sh_a = int_a; 
			*(short *)pIntArg = sh_a; 
		}
		else if (ShortFlag == 2)
		{
			char ch_a;
			ch_a = int_a;
			*(char *)pIntArg = ch_a;
		}
		else 
			*pIntArg = int_a;
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	xint64_sscanf
* 功能:	从输入字符串缓存中按格式读入一个十六进制整型数据,将该整型数据存储到一个
*		64位long long型变量当中
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pI64Arg			64位long long型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个十六进制整型数据				 
*----------------------------------------------------------------------------*/
static int xint64_sscanf(char **pBuffer, __int64 *pI64Arg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	int j,n;
	__int64 int_a = 0;
	
	if (FieldWidth) 
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0; 
	}
	
	while (j)
	{
		if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
		{
			int_a = int_a * 16 + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 
		else if ((**pBuffer >= 'a')&&(**pBuffer <= 'f'))
		{
			int_a = int_a * 16 + (**pBuffer - 'a' + 10); 
			(*pBuffer)++;
		} 
		else if ((**pBuffer >= 'A')&&(**pBuffer <= 'F')) 
		{
			int_a = int_a * 16 + (**pBuffer - 'A' + 10); 
			(*pBuffer)++;
		} 
		else break;
		
		j -= n;
	}
	
	if (*pBuffer == p) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		*pI64Arg = int_a;
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	float_sscanf
* 功能:	从输入字符串缓存中按格式读入一个浮点型数据
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pFloatArg		浮点型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个浮点型数据				 
*----------------------------------------------------------------------------*/
static int float_sscanf(char **pBuffer, float *pFloatArg, int FieldWidth, int DisFlag)
{
	char *pp = *pBuffer;
	
	if (FieldWidth) 
	{
	
		int j, e_flag;
		float p = 0, fa = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			}
			else break; 
			
		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa; 
			return 1; 
		}
		
	} 
	else 
	{
		int e_flag = 0;
		float p = 0, fa = 0;
		
		while ( **pBuffer != ' ' )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			} 
			else break;

		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
			 
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa;
			return 1; 
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	LFloat_sscanf
* 功能:	从输入字符串缓存中按格式读入一个double型数据
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pFloatArg		double型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个浮点型数据				 
*----------------------------------------------------------------------------*/
static int LFloat_sscanf(char **pBuffer, double *pFloatArg, int FieldWidth, int DisFlag)
{
	char *pp = *pBuffer;
	
	if (FieldWidth) 
	{
	
		int j, e_flag;
		float p = 0;
		double fa = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			}
			else 
				break; 
			
		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa; 
			return 1; 
		}
		
	} 
	else 
	{
		int e_flag;
		float p = 0;
		double fa = 0;
		
		while ( **pBuffer != ' ' )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			} 
			else 
				break;

		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}	
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa;
			return 1; 
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	string_sscanf
* 功能:	从输入字符串缓存中按格式读入一个字符串
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pChArg			字符型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个字符串				 
*----------------------------------------------------------------------------*/
static int string_sscanf(char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth)
	{
		int j;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer != ' ')&&(**pBuffer != SSCANF_TAB)&&(**pBuffer != SSCANF_NEW_LINE)&&(**pBuffer != SSCANF_END)) 
			{
				if (DisFlag) 
				{
					*pChArg = **pBuffer; 
					pChArg++;
				}
				(*pBuffer)++;
			} 
			else break;
			
		if (*pBuffer == p)
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pChArg = 0;
			return 1;
		}
	} 
	else 
	{
		
		while ((**pBuffer != ' ')&&(**pBuffer != SSCANF_TAB)&&(**pBuffer != SSCANF_NEW_LINE)&&(**pBuffer != SSCANF_END)) 
		{
			if (DisFlag) 
			{
				*pChArg = **pBuffer;
				pChArg++;
			}
			(*pBuffer)++;
		}
		
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pChArg = SSCANF_END;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	char_sscanf
* 功能:	从输入字符串缓存中按格式读入一个字符或列宽个字符
*
* 参数:	pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pChArg			字符型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个字符				 
*----------------------------------------------------------------------------*/
static int char_sscanf(char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		
		for ( j=0; j<FieldWidth; j++ ) 
			if (**pBuffer != SSCANF_END) 
			{
				if (DisFlag) 
				{
					*pChArg = **pBuffer;
					pChArg++;
				}
				(*pBuffer)++;
			} 
			else break;
			
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
			return 1;
	}
	else 
	{
		if (**pBuffer != SSCANF_END)
		{
			if (DisFlag) 
			{
				*pChArg = **pBuffer; 
				(*pBuffer)++;
				pChArg++;
				return 1;
			}
			else 
				(*pBuffer)++;
		} 
		else 
			return SSCANF_EOF;
	}
	
	return 0;
}


/*-----------------------------------------------------------------------------
* 函数:	other_sscanf
* 功能:	当格式为"%[..]"时,使用该函数读入一个字符串,并存储到对应的参数中
*
* 参数:	pFormat			一个指向字符指针的指针,该字符指针指向当前格式字符串
*		pBuffer			一个指向字符指针的指针,该字符指针指向当前输入字符串缓存
*		pChArg			字符型数据存放地址
*		FieldWidth		输入格式指定宽度
*		DisFlag			格式是否标记'*',即是否跳过. 0:跳过 1:存储
*
* 返回:	SSCANF_EOF		输入出错
*		0				跳过,未存储数据
*		1				成功读入一个字符串				 
*----------------------------------------------------------------------------*/
static int other_sscanf(char **pFormat, char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer, *pp = *pFormat;
	char a[256];
	int j, n, jj = 1;
	
	memset( a, 0, 256);

/* find if the format is store "until" or "till" the character is encountered.
 * if "until", a[i] = 1(i=0,1,2,...,255), and the index of the character refered 
 * in the format will be 0. */
	if (**pFormat == '^') 
	{
		(*pFormat)++;
		jj = 0;
		memset( a, 1, 256);
	}

/* read the character in the format while the format end. */
	while (**pFormat != ']') 
	{
		if (**pFormat != '-') 
		{
			a[**pFormat] = jj;
			(*pFormat)++;
		} 
		else 
		{
			int i;
			char min, max, c;
			min = *(*pFormat - 1);
			max = *(*pFormat + 1);
			*pFormat += 2;
			if (max == ']') 
				return SSCANF_EOF;
			if (min > max) 
			{
				c = min;
				min = max;
				max = min;
			}
			for ( i=min; i<=max; i++ ) 
				a[i] = jj;
		}	// if (**pFormat != '-')
	}	// while (**pFormat != ']')

/* if the format refered nothing, it means all are included. */
	if (pp == *pFormat) 
		memset( a, 1, 256 );
	
	(*pFormat)++;

/* This is set for FieldWidth. */
	if (FieldWidth)
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0;
	}

	while ((a[**pBuffer])&&(**pBuffer != SSCANF_END)&&(j))
	{
		if (DisFlag) 
		{
			*pChArg = **pBuffer;
			pChArg++;
		}
		(*pBuffer)++;
		j -= n;
	}
	
	if (p == *pBuffer) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		*pChArg = SSCANF_END;
		return 1;
	}
	return 0;
}



/*-----------------------------------------------------------------------------
* 函数:	Format_Sscanf
* 功能:	配合hyco的sscanf函数使用,将输入字符串缓存中的数据按格式写到可变参数中
* 参数:	buffer   输入字符串
*		format	 格式控制字符串
*		list	 可变参数列表
* 返回:	输入到可变参数中的数据个数  
*----------------------------------------------------------------------------*/
static int Format_Sscanf(char *buffer, char *format, va_list ap)
{
	int j, i = 0;
	char *pFormat = format, *pBuffer = buffer;

	while (*pFormat != 0)
	{
		char type, *pChArg;
		int *pIntArg;
		__int64 *pI64Arg;
		float *pFloatArg;
		double *pDoubleArg;
		
		j = 0;

		/* read the information in the format */
		if ((*pFormat == '%')&&(*(++pFormat) != '%')) 
		{
			int FieldWidth = 0, DisFlag = 1, WidthFlag = 0;
			
			if (*pFormat == '*') 
			{ 
				DisFlag = 0; // if (DisFlag == 0) discard the input field
				pFormat++; 
			}	
			
			while ((*pFormat >= '0')&&(*pFormat <= '9'))	// read the FieldWidth if it's specified
			{
				FieldWidth = FieldWidth * 10 + (*pFormat - '0');
				WidthFlag = 1;
				pFormat++;
			}
			if ((WidthFlag)&&(!FieldWidth))
				return i;

		/* if the format is like this "%[..]", use other_sscanf */
			if (*pFormat == '[') 
			{
				pFormat++;
				if (DisFlag) 
					pChArg = va_arg(ap, char*); 
				else 
					pChArg = NULL;
				j = other_sscanf(&pFormat, &pBuffer, pChArg, FieldWidth, DisFlag);
			} 
			else 
		/* if the format is not like "%[..]", continue to read the rest information. */
			{
				int NegFlag = 0, ShortFlag = 0, LongFlag = 0, LLFlag = 0, jFlag = 0, zFlag = 0, tFlag = 0;

				switch (*pFormat)
				{
				case 'h':
					if (*(pFormat+1) == 'h')
					{
						ShortFlag = 2;	// C99--hh: if (ShortFlag == 2) the type is char
						pFormat += 2; 
					}
					else
					{
						ShortFlag = 1;	// if (ShortFlag == 1) the type is short int
						pFormat++;
					}	
					break;

				case 'l':
					if (*(pFormat+1) == 'l')
					{
						LLFlag = 1;		// C99--ll: if (LLFlag == 1) the type is long long int
						pFormat += 2; 
					}
					else
					{
						LongFlag = 1;	// if (LongFlag == 1) the type is long int
						pFormat++;
					}
					break;

				case 'I':
					if ((*(pFormat+1) == '6')&&(*(pFormat+2) == '4'))
					{
						LLFlag = 1;	// if (LLFlag == 1) the type is long long
						pFormat += 3; 
					}
					break;

				case 'L':
					LongFlag = 1;
					pFormat++;
					break;

				case 'j':		// C99 intmax_t 由于无法了解和定义该类型,暂时只做格式解析
					jFlag = 1;
					pFormat++;
					break;

				case 'z':		// C99 size_t 由于无法了解和定义该类型,暂时只做格式解析
					zFlag = 1;
					pFormat++;
					break;

				case 't':		// C99 ptrdiff_t 由于无法了解和定义该类型,暂时只做格式解析
					tFlag = 1;
					pFormat++;
					break;
				}
				
				type = *pFormat++;
				
				if (type == 'p')
				{
					NegFlag = 0, ShortFlag = 0, LongFlag = 0, LLFlag = 0, jFlag = 0, zFlag = 0, tFlag = 0;
				}
				
				if ((type != 'c')&&(type != 'n'))
					while ((*pBuffer == ' ')||(*pBuffer == SSCANF_TAB)||(*pBuffer == SSCANF_NEW_LINE)) 
						pBuffer++;
				
				if ((type != 's')&&(type != 'c')) 
					if ((*pBuffer == '-')||(*pBuffer == '+')) 
					{ 
						if (*pBuffer == '-') NegFlag = 1; 
						pBuffer++; 
						if (FieldWidth) 
						{
							if (FieldWidth - 1) 
							{
								FieldWidth--;
							} 
							else 
								return i;
						}
					} 

		/* converting the value according to the type */
				switch (type) 
				{
				case 'd':
				case 'i':
				case 'u':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = int64_sscanf(&pBuffer, pI64Arg, 10, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = int_sscanf(&pBuffer, pIntArg, 10, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'x':
				case 'X':
				case 'p':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = xint64_sscanf(&pBuffer, pI64Arg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = xint_sscanf(&pBuffer, pIntArg, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'o':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = int64_sscanf(&pBuffer, pI64Arg, 8, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = int_sscanf(&pBuffer, pIntArg, 8, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'f':
				case 'F':
				case 'a':	// C99
				case 'A':	// C99 
				case 'e':
				case 'E':
				case 'g':
				case 'G':
					if (LongFlag)
					{
						if (DisFlag) 
							pDoubleArg = va_arg(ap, double*); 
						else 
							pFloatArg = NULL;
						j = LFloat_sscanf(&pBuffer, pDoubleArg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pDoubleArg = -(*pDoubleArg);	
					}
					else
					{
						if (DisFlag) 
							pFloatArg = va_arg(ap, float*); 
						else 
							pFloatArg = NULL;
						j = float_sscanf(&pBuffer, pFloatArg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pFloatArg = -(*pFloatArg);	
					}
					break;

				case 's':
					if (DisFlag) 
						pChArg = va_arg(ap, char*); 
					else 
						pChArg = NULL;
					j = string_sscanf(&pBuffer, pChArg, FieldWidth, DisFlag);
					break;
					
				case 'c':
					if (DisFlag) 
						pChArg = va_arg(ap, char*); 
					else
						pChArg = NULL;
					j = char_sscanf(&pBuffer, pChArg, FieldWidth, DisFlag);					
					break;
					
				case 'n':
					if (DisFlag) 
					{
						pIntArg = va_arg(ap, int*);
						if (*pBuffer != SSCANF_END) 
							*pIntArg = pBuffer - buffer; 
						else 
							j = SSCANF_EOF;
					}
					else 
						pIntArg = NULL;
					break;
				
				default:
					j = SSCANF_EOF;
				}

			}		// if (*pFormat == '[') 
			
			if (j == SSCANF_EOF) 
			{
				if (pBuffer == buffer) 
					return SSCANF_EOF; 
				else 
					return i;
			} 
			else 
				i += j;
			
		} 
		else
	/* if the format is not a conversion specification(%...), find if it's conflicted with the buffer. */
		{
			if ((*pFormat != ' ')&&(*pFormat != SSCANF_TAB)&&(*pFormat != SSCANF_NEW_LINE))
			{
				if (*pFormat != *pBuffer) 
					return i; 
				else 
				{ 
					pFormat++; 
					pBuffer++; 
				}
			}
			else 
			{
				pFormat++;
				while ((*pBuffer == ' ')||(*pBuffer == SSCANF_TAB)||(*pBuffer == SSCANF_NEW_LINE)) 
					pBuffer++;
			}
		}			// *pFormat == '%'
		
	}	//while(*pFormat != 0)		
	
	if (pBuffer == buffer) 
		return SSCANF_EOF; 
	else 
		return i;
}


/*-----------------------------------------------------------------------------
* 函数:	sscanf
* 功能:	hyco的sscanf函数
* 参数:	buffer   输入字符串缓存
		format	 格式控制字符串
*		...		 可变参数列表
* 返回:	输入的数据个数  
*----------------------------------------------------------------------------*/
int sscanf(char *buffer, char *format,...)
{
	va_list list;
	int n;
	
	va_start(list,format);
	
	n = Format_Sscanf( buffer, format, list );
	
	va_end(list);
	
	return (n);
}
