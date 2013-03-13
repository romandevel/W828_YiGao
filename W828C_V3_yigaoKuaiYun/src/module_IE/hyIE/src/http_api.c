#include "hyTypes.h"
#include "http.h"
#include "lprotocol.h"

/*========================================================================================*/
#pragma arm section zidata = "IE_memory"
char gu8IE_memory[102400];
#pragma arm section

static U32 gu32LockIE;

int gs32HttpSocket = -1;

U8	gu8GzipEncode;


eHTTP_CHARSET_TYPE geHttpCharset;
//////////////////////////////////////////////////////////////////
int form_url_encoded(char *strSrc, char *pEncoded);
unsigned int Strlen(const char *pBuf);

eHTTP_CHARSET_TYPE hyIE_GetEnType()
{
	return geHttpCharset;
}

void hyIE_resetSocket()
{
	gs32HttpSocket = -1;
}

int hyIE_GetHttpSocket()
{
	return gs32HttpSocket;
}

int hyIE_init() 
{ 
#if 1
	protocol_setting_set("http_proxy", NULL);
	
	IE_InitToSdram_buf();
//hyUsbPrintf("IE_InitToSdram_buf\r\n");

	//initialize memory controller.
	hy_mem_create(HYIE_MEM_IDX);
	hy_mem_load_blocks( gu8IE_memory, sizeof(gu8IE_memory) ,HYIE_MEM_IDX);
	
	protocol_init();
#else

	//加密芯片验证
  	__align(4) U8 randomBuf[128]; /* 两个64字节，提供给验证函数存随机数*/
  
  	hyWebSure_reg(hyIE_initOK, hyIE_initERR);
  	if( HY_OK != hyWebSure_verify(1, randomBuf, NULL) )
  	{
   		return -1;
  	}
#endif

  	return 0;
}


void hyIE_destroy()
{
#if 0
	free_httpHeader();
	//Dns_cache_free();
    protocol_destroy();
	
	hy_mem_release(HYIE_MEM_IDX);
#endif
}



/**
 * Reverse characters of a string.
 *
 * @param source string
 *
 * @return the new memory save the paramter string
 */
char *StrRev(char *str)
{
    char *right = str;
    char *left = str;
    char ch;
    
    while (*right)    right++;
    right--;
    while (left < right)
    {
        ch = *left;
        *left++ = *right;
        *right-- = ch;
    }
    return(str);
}

/**
 * Calculates the floor of a value.
 *
 * @param source string
 *
 * @return the new memory save the paramter string
 */
double Floor(double x)
{
    double y=x;
    
    if( (*( ( (int *) &y)+1) & 0x80000000)  != 0) 
        return (int)x-1;
    else
        return (int)x;
}

/**
* 表格input中的字符需要按url编码格式进行处理
* 注意：需要转义的特殊字符主要包括：=,+,&,%以及多行文本中的回车符、
* 换行符和所有不能直接显示的高位ASCII符。在ASCII中所有的127(十六进
* 制的7f)以上和33(十六进制的21)以下的字符将被转义
*/
// HEX Values array
const char hex_vals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
// UNSAFE String
const char unsafe_str[] = "\"<>%\\^[]`+$,@:;/!#?=&";
//提交表单时,eg:name=value,name=不需转义,value需转义

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
char *url_encoded_decToHex(char num, int radix)
{	
	int temp=0;	
    char strTmp[4]={0};
	int num_char;
	num_char = (int) num;
	
	// ISO-8859-1 
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
		num_char = 256 + num_char;

	while (num_char >= radix)
    {
		temp = num_char % radix;
		num_char = (int)Floor(num_char / radix);
		strTmp[0] = hex_vals[temp];
    }
	
	strTmp[1] = hex_vals[num_char];

	if(STRLEN(strTmp) < 2)
	{
		strTmp[2] = '0';
	}

    return StrRev(strTmp);
}


// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
char *url_encoded_convert(char val) 
{
    static char ret[5];
 
    memset(ret, 0, 5);
    strcpy(ret, "%");
    strcat(ret, url_encoded_decToHex(val, 16));
    return  ret;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
int url_encoded_isUnsafe(char compareChar)
{
	int  bcharfound = FALSE;
	char tmpsafeChar;
	int  strLen = 0;
    int  ichar_pos;
    int char_ascii_value = 0;
	
	strLen = STRLEN(unsafe_str);
	for(ichar_pos = 0; ichar_pos < strLen ;ichar_pos++)
	{
		tmpsafeChar = unsafe_str[ichar_pos]; 
		if(tmpsafeChar == compareChar)
		{ 
			bcharfound = TRUE;
			break;
		} 
	}
	//char_ascii_value = __toascii(compareChar);
	char_ascii_value = (int) compareChar;

	if(bcharfound == FALSE &&  char_ascii_value > 32 && char_ascii_value < 123)
	{
		return FALSE;
	}
	// found no unsafe chars, return false		
	else
	{
		return TRUE;
	}
	return TRUE;
}

// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
//pEncoded:buf 最少需要是strSrc长度的3倍+1
int form_url_encoded(char *strSrc, char *pEncoded)
{	
	int srcPos, desPos;
	int len;
    char ch;

	len = STRLEN(strSrc);
	
    desPos = 0;
    memset(pEncoded, 0, len*3+1);

	for(srcPos = 0; srcPos < len; srcPos++)
	{
		ch = strSrc[srcPos];
		if (ch < ' ') 
		{
			ch = ch;
		}		
		if(!url_encoded_isUnsafe(ch))
		{
			// Safe Character
            pEncoded[desPos++] = ch;
		}
		else
		{
			// get Hex Value of the Character
            strcat(pEncoded+desPos, url_encoded_convert(ch));
            desPos = STRLEN(pEncoded);
		}
	}
	
	return desPos;
}

//< 被编码成 &lt;   > 被编码成 &gt;  解码时对应
int html_decode(char *pSrc, int srclen, char *pDst)
{
	char *p, *ps, *pNext;
	int  len, type = 0;
	
	p  = pDst;
	ps = pSrc;
		
	pNext = strstr(pSrc, "&lt;");
	while(pNext != NULL)
	{
		//hyUsbPrintf("%s\r\n",pSrc);
		//hyUsbPrintf("%d  ---   %d\r\n",(pNext-ps), srclen);
		if( (pNext-ps) >= srclen )
		{
			len = srclen - (int)(pSrc-ps);
			memcpy(pDst, pSrc, len);
			break;
		}
		len = (int)(pNext-pSrc);
		memcpy(pDst, pSrc, len);
		pDst += len;
		pSrc = pNext+4;
		
		if(type == 0)
		{
			type = 1;
			*pDst++ = '<';
			pNext = strstr(pSrc, "&gt;");
		}
		else
		{
			type = 0;
			*pDst++ = '>';
			pNext = strstr(pSrc, "&lt;");
		}
	//	hyUsbPrintf("pNext=%s\r\n",pNext);
		//hyUsbPrintf("pDst len ==%d\r\n",(pDst - p));
	}
	
	*pDst = 0;
	
	len = (pDst - p);
	
	if(len == 0)
	{
		strcpy(pDst, ps);
		len = srclen;
	}
	
	return len;
}

/**
 * Binary safe case-insensitive string comparison
 *
 * @param two strings
 *
 * @return the decimal value.
 *   0  - if the two strings are equal 
 *   <0 - if string1 is less than string2 
 *   >0 - if string1 is greater than string2 
 */
int strcasecmp(const char *s1, const char *s2) 
{ 
	for(;;) 
	{ 
		if( *s1 != *s2 ) 
		{ 
			if( TOLOWER(*s1) != TOLOWER(*s2) ) 
				return *s1 - *s2; 
		} 
		else if	( *s1 == '\0' ) 
			break; 
		s1++; s2++; 
	} 
	return 0; 
}

unsigned int Strlen(const char *pBuf)
{
	int i = 0;
	if (pBuf == NULL)
		return 0;
	while (pBuf[i])
		i++;
	return i;
}


/*--------------------------------------
*函数：hy_memicmp
*功能：不区分大小写比较字符串
*参数：pStr1,   pStr2
*返回：-1:pStr1 < pStr2    0:pStr1 == pStr2    1:pStr1 > pStr2
*--------------------------------------*/
int hy_memicmp(U8 *pStr1, U8 *pStr2, int len)
{
	int ret = 0;
	int i;
	U8  ch1,ch2;
	
	for(i = 0; i < len; i++)
	{
		ch1 = *pStr1++;
		ch2 = *pStr2++;
		if(ch1 >= 'A' && ch1 <= 'Z')//大写字母 转成小写字母再比较
		{
			ch1 += 0x20;
		}
		if(ch2 >= 'A' && ch2 <= 'Z')//大写字母 转成小写字母再比较
		{
			ch2 += 0x20;
		}
		if(ch1 < ch2)
		{
			ret = -1;
			break;
		}
		else if(ch1 > ch2)
		{
			ret = 1;
			break;
		}
	}
	
	return ret;
}


/**
 * Convert strings to integer 
 *
 * @param source string
 *
 * @return interger
 */
int AtoI(const char *str) 
{
	long int v = 0;
	int sign = 0;
	
	while ( *str == ' ')  str++; 
	if(*str == '-'||*str == '+')
		sign = *str++;
	while ((unsigned int)(*str - '0') < 10u)
	{
		v = v*10 + *str - '0'; 
		str++;
	}
	return sign == '-' ? -v:v;
}

/**
 * malloc a block memory, then copy the paramter string to memory
 *
 * @param source string
 *
 * @return the new memory save the paramter string
 */
char *Strdup(const char *s)   
{   
	char *t = NULL;   

    if (s && ((t = (char*)GETBUFFER(STRLEN(s) + 1,HYIE_MEM_IDX)) != NULL))   
        strcpy(t, s);   
    return   t;   
} 

/**
 * Encodes a piece of data into an base64 encoded string. The encoded
 * string is terminated with a zero.
 * 
 * @param data The data to be encoded.
 * @param length The length of the data in bytes.
 * @param encoded A pointer to an area to hold the encoded string. This
 * @param encoded must be allocated by the caller.
 *
 * @return non-zero value if an error occurred.
 */
/*static unsigned char base64_codes[] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(unsigned char *data, int length, unsigned char *encoded)
{
    int i;
    unsigned char d0, d1, d2;
    
    for(i = 0 ; i < length ; i += 3) 
    {
        d0 = data[i + 0];
        
        if(i + 1 < length)
            d1 = data[i + 1];
        else
            d1 = 0;
        
        if(i + 2 < length)
            d2 = data[i + 2];
        else
            d2 = 0;
        
        *encoded++ = base64_codes[d0 >> 2];
        *encoded++ = base64_codes[((d0 & 0x03) << 4) | d1 >> 4];
        if(i + 1 < length)
            *encoded++ = base64_codes[((d1 & 0x0f) << 2) | d2 >> 6];
        else
            *encoded++ = '=';
        if(i + 2 < length)
            *encoded++ = base64_codes[d2 & 0x3f];
        else
            *encoded++ = '=';
    }
    
    *encoded = '\0';
    
    return 0;
}*/


////////////////////////////

/*
 * Len : 要接受的长度(长度受authMsg空间的限制)
 */

int protocol_recv_t(int fd, fd_set fs, struct timeval to, char *buf, int len)
{
	int ret, errCnt=0;
	
	while(1)
	{
		if(Net_GsmReset() == 1)
		{
			return -1;
		}
		ret = select(fd, &fs, 0, 0, &to);
		if (ret > 0)
		{
			return recv(fd, buf, len, 0);
		}
		else if (ret == 0)
		{
			errCnt++;
			sleep(1);
			if (errCnt>=5)
		    {
		    	//hyUsbPrintf("recv timeout == %d\r\n",errCnt);
		        return -1;
		    }
		}
		else
		{
			return -1;	
		}
	}
}

//////////////////////////////////////////////////////////

/*
 当前服务器返回的数据包是否gzip编码
 1：是   0：否
*/
U8 http_GzipFlag()
{
	return gu8GzipEncode;
}


////////////////////////////////////////////////////////////
const U8 gzip_end[5] = {0x30, 0x0d, 0x0a, 0x0d, 0x0a};

int http_str2hex(U8 *pStr)
{
	int i,len,val = 0;
	
	len = strlen(pStr);
	for(i = 0; i < len; i++)
	{
		if(pStr[i] >= '0' && pStr[i] <= '9')
		{
			val = val*16+(pStr[i]-'0');
		}
		else if(pStr[i] >= 'a' && pStr[i] <= 'z')
		{
			val = val*16+(pStr[i]-'a'+10);
		}
		else if(pStr[i] >= 'A' && pStr[i] <= 'Z')
		{
			val = val*16+(pStr[i]-'A'+10);
		}
	}
	
	return val;
}

int http_chunked(unsigned char *pData, int len)
{
	unsigned char  headBuf[10], uc;
	unsigned char  *pSub, buf[4];
	int i,j,wantLen,needLen,headlen;
	int firstH=0,tempLen;

	needLen = 0;
	headlen = 0;
	wantLen = len;
	for(i = 0; i < len && wantLen > 0; )
	{
		if(needLen != -1)
		{
			headlen++;
			uc = pData[i];
			if(pData[i] == '\r' && pData[i+1] == '\n')
			{
			//hyUsbPrintf("needLen = %d \r\n", needLen);
				if(needLen > 0)
				{
					//计算长度值
					headBuf[needLen] = 0;
					wantLen = http_str2hex(headBuf);
					needLen = -1;
				//hyUsbPrintf("want len = %d \r\n", wantLen);
					if(wantLen == 0)
					{
						len = i-3;
						break;
					}
					i+=2;
					headlen++;
					if(firstH == 0)//第一包
					{
						firstH = 1;
						if(pData[i] != 0)
						{
							buf[0] = pData[i];
							buf[1] = pData[i+1];
							buf[2] = 0;
							tempLen = (buf[0]<<8)+buf[1];
						}
						else
						{
							buf[0] = pData[i+1];
							buf[1] = 0;
							tempLen = buf[0];
						}
						
						if(wantLen-2 == tempLen)
						{
							i+=2;
							headlen+=2;
						}
					}
					len -= headlen;
					memmove(&pData[i-headlen], &pData[i], len);
					i -= headlen;
				}
				else
				{
					i++;
				}
			}
			else
			{
				//保存长度值
				headBuf[needLen++] = uc;
				i++;
			}
		}
		else
		{
			pSub = &pData[i];
			for(j = 0; j < wantLen; j++)
			{
				if(*pSub != 0)
				{
					break;
				}
				pSub++;
			}
			if(j > 0)
			{
				len -= j;
				wantLen -= j;
				memmove(&pData[i], pSub,len);
			}
			i += wantLen;
			needLen = 0;
			headlen = 0;
		}
	}

	return len;
}

/*----------------------------------------------------
* 函数:http_send_request
* 功能:发送网络请求  post/get方式
* 参数:pUrl:请求完整地址
       bufLen:请求时使用的buf长度
       pRequest:请求参数
* 返回:>0成功
------------------------------------------------------*/
int http_send_request(U8 *pUrl, int bufLen, struct protocol_http_request *pRequest)
{
	struct protocol_http_headers *headers;
	int leftLen,revLen,total;
	int i,ret,alive;
    U8  *temp;
    int TransferEncoding;
    struct timeval to;
    fd_set fs;

	wlock(&gu32LockIE);
	if(-1 == hyIE_init())
	{
		unlock(&gu32LockIE);
		return -1;
	}
    
    headers = (struct protocol_http_headers *)GETBUFFER(sizeof(struct protocol_http_headers),HYIE_MEM_IDX);
	if(headers == NULL)
	{
		hyIE_destroy();
		unlock(&gu32LockIE);
		return -1;
	}
    
    headers->return_code		= 200;
	headers->return_message		= NULL;
	headers->content_type_major = NULL;
	headers->content_type_minor = NULL;
	headers->content_length		= -1;
	headers->server				= NULL;
	headers->location			= NULL;
	headers->real_url			= NULL;
	headers->charset			= NULL;
	headers->content_encoding	= NULL;
	headers->connection			= NULL;
	headers->cookie				= NULL;
	headers->TransferEncoding   = NULL;
	
	alive = pRequest->connection;
	pRequest->connection = 1;
	
    if(gs32HttpSocket == -1)
    {
    	hyUsbPrintf("start create socket == \r\n");
    	gs32HttpSocket = protocol_create(pUrl);
    	if(-1 == gs32HttpSocket)
    	{
    		hyIE_destroy();
    		unlock(&gu32LockIE);
			return -1;
    	}
    	hyUsbPrintf("---------------------create socket ok\r\n");
    }
	
    ret = protocol_send_request(gs32HttpSocket, pUrl, pRequest, headers);
	    
    if(-1 == ret)
    {
    	//hyUsbPrintf("send_request fail !  last err = %d  \r\n",IE_GetLastErr());
    	if(IE_GetLastErr() != 20000 || alive == 0 || strcasecmp(headers->connection, "keep-alive") != 0)
		{
			//hyUsbPrintf("send_request fail ! close socket \r\n");
			protocol_http_close(gs32HttpSocket);
			gs32HttpSocket = -1;
		}
		
    	hyIE_destroy();
		unlock(&gu32LockIE);
		return -1;
    }
    
   	if(headers->content_encoding != NULL)
   	{
   		gu8GzipEncode = 1;
   	}
   	else
   	{
   		gu8GzipEncode = 0;
   	}
   	
   	//字符编码方式
hyUsbPrintf("charset  = %s \r\n", headers->charset);
   	if(headers->charset == NULL || _stricmp("UTF-8", headers->charset) == 0)
   	{
   		geHttpCharset = CHARSET_UTF8;
   	}
   	else
   	{
   		geHttpCharset = CHARSET_GBK;
   	}
   	
   	//hyUsbPrintf("gzip = %s    === %d \r\n",headers->content_encoding,gu8GzipEncode);
   	
    temp      = pRequest->content_data;
    leftLen   = bufLen;
	revLen    = 0;
    total     = 0;
    
    to.tv_sec  = 3;
    to.tv_usec = 0;

    FD_ZERO(&fs);
    FD_SET(gs32HttpSocket, &fs);
    ret = -1;
    
    if(pRequest->method == HTTP_GET_METHOD && pRequest->range == NULL)
    {
    	if(alive == 0 || strcasecmp(headers->connection, "keep-alive") != 0)
		{
			protocol_http_close(gs32HttpSocket);
			gs32HttpSocket = -1;
		}

	    hyIE_destroy();
		
	 	unlock(&gu32LockIE);
	 	
		return headers->content_length;
    }
    
    if(headers->TransferEncoding != NULL)
    {
    	TransferEncoding = 1;
    }
    else
    {
    	TransferEncoding = 0;
    }
hyUsbPrintf("TransferEncoding = %s \r\n",headers->TransferEncoding);    
	while(1)	
	{
		if(Net_GsmReset() == 1)
		{
			break;
		}
		
		revLen = protocol_recv_t(gs32HttpSocket,fs,to,temp, leftLen);
		if (revLen < 0)
		{
			if(total > 0)
			{
				ret = total;
			}
			else
			{
				ret = -1;
			}
			hyUsbPrintf("revLen   < 0 \r\n");
			break;
		}
		temp[revLen] = 0;
		
		//hyUsbPrintf("ie recv len = %d \r\n", revLen);
		//hyUsbMessageByLen(temp, revLen);
		//hyUsbPrintf("\r\n");
		
		if (revLen == 0)
		{
			ret = total;
			
			hyUsbPrintf("revLen   ==  0 \r\n");
			break;
		}
		else if(total + revLen > bufLen)
		{
			ret = total;
			hyUsbPrintf("total + revLen > bufLen \r\n");
			break;
		}
		else
		{
			total += revLen;
		}
		
		temp    += revLen;
		leftLen -= revLen;
		
		if(leftLen <= 0)
		{
			hyUsbPrintf("leftLen <= 0 \r\n");
			break;
		}
		
		hyUsbPrintf("content_length = %d   recvtotal = %d \r\n", headers->content_length, total);
		if(headers->content_length > 0 && total >= headers->content_length)
		{
			ret = total;
			break;
		}
		
		if(gu8GzipEncode == 1 || TransferEncoding == 1)//gzip 检查最后一个chunk大小为0时结束 30 0d 0a 0d 0a
		{
			if(memcmp(&(pRequest->content_data[total-5]),gzip_end,5) == 0)
			{
				//hyUsbMessage("--------------chunk end ok!\r\n");
				ret = total;
				break;
			}
		}
	}

hyUsbPrintf("connection  = %s \r\n",headers->connection);
   	if(alive == 0 || strcasecmp(headers->connection, "keep-alive") != 0)
	{
		protocol_http_close(gs32HttpSocket);
		gs32HttpSocket = -1;
	}
	
hyUsbPrintf("hyIE_destroy   sss  ret = %d \r\n",ret); 
    hyIE_destroy();
	
	if(ret > 0)
 	{
 		pRequest->content_data[ret] = 0;
 		//整理数据
		if(TransferEncoding == 1)
		{
			ret = http_chunked(pRequest->content_data, ret);
			pRequest->content_data[ret] = 0;
		}
 	}
	
 	unlock(&gu32LockIE);
hyUsbPrintf("quit  \r\n");
 	
	return ret;
}

/*获取文件大小*/
int http_getsize(U8 *pUrl)
{
	struct protocol_http_request tRequest;
	int size;
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_GET_METHOD;
    tRequest.user_agent     = NULL;
    tRequest.content_length = 0;
    tRequest.content_data   = NULL;
    tRequest.referer        = NULL;
    tRequest.content_type   = NULL;
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	size = http_send_request(pUrl, 0, &tRequest);
	
	return size;
}

#define DOWNFILE_TRYCNT		10

/*下载文件 返回本次下载的长度*/
int http_downfile(U8 *pUrl, int start, int end, U8 *pBuf, int bufLen)
{
	struct protocol_http_request tRequest;
	struct protocol_http_headers *headers;
	struct timeval to;
    fd_set fs;
    int		ret=-1, quit=0,errcnt=0;
	U8		*range[20], *pRecv;
	int		s_pos, e_pos;
	int		recvLen, finished = start, curRecv;
	
	wlock(&gu32LockIE);
	if(-1 == hyIE_init())
	{
		unlock(&gu32LockIE);
		return -1;
	}
	
	headers = (struct protocol_http_headers *)GETBUFFER(sizeof(struct protocol_http_headers),HYIE_MEM_IDX);
	if(headers == NULL)
	{
		hyIE_destroy();
		unlock(&gu32LockIE);
		return -1;
	}
    
    headers->return_code		= 200;
	headers->return_message		= NULL;
	headers->content_type_major = NULL;
	headers->content_type_minor = NULL;
	headers->content_length		= -1;
	headers->server				= NULL;
	headers->location			= NULL;
	headers->real_url			= NULL;
	headers->charset			= NULL;
	headers->content_encoding	= NULL;
	headers->connection			= NULL;
	headers->cookie				= NULL;
	headers->TransferEncoding   = NULL;
	
	s_pos = start;
	e_pos = end-1;
	sprintf(range, "%d-%d",s_pos,e_pos);
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_GET_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = 0;
    tRequest.content_data   = NULL;
    tRequest.referer        = NULL;
    tRequest.content_type   = NULL;
    tRequest.range          = range;
    tRequest.connection		= 1;
	tRequest.gzip			= 0;
	
	to.tv_sec = 5;
    to.tv_usec = 0;
    
    while(!quit)
    {
    	//判断网络是否正常
    	if(Net_GsmReset() == 1)
		{
			break;
		}
		
		if(gs32HttpSocket == -1)
	    {
	    	hyUsbPrintf("start create socket == \r\n");
	    	gs32HttpSocket = protocol_create(pUrl);
	    	if(-1 == gs32HttpSocket)
	    	{
	    		errcnt++;
		    	if(errcnt >= DOWNFILE_TRYCNT)
		    	{
		    		quit = 1;
		    	}
		    	continue;
	    	}
	    	hyUsbPrintf("---------------------create socket ok\r\n");
	    	FD_ZERO(&fs);
	   		FD_SET(gs32HttpSocket, &fs);
	    }
		
		ret = protocol_send_request(gs32HttpSocket, pUrl, &tRequest, headers);
		if(-1 == ret)
	    {
	    	errcnt++;
	    	if(errcnt >= DOWNFILE_TRYCNT)
	    	{
	    		quit = 1;
	    	}
	    	continue;
	    }
		
	hyUsbPrintf("errcnt = %d \r\n", errcnt);	
		pRecv   = pBuf+finished;
		recvLen = bufLen-finished;
		curRecv = 0;
		while ((recvLen=protocol_recv_t(gs32HttpSocket,fs,to, pRecv, recvLen)) > 0)
        {
        hyUsbPrintf("recvlen = %d finished = %d \r\n",recvLen, finished);
        	curRecv  += recvLen;
            finished += recvLen;
            Com_SetPercentRecv(Com_GetPercentRecv() + recvLen);
            recvLen  = 10240;
           	pRecv    = pBuf+finished;
            if(finished >= (end-start))
                break;
        }
        
        if(strcasecmp(headers->connection, "keep-alive") != 0)
		{
			protocol_http_close(gs32HttpSocket);
			gs32HttpSocket = -1;
		}
        if (finished == (end-start))      //正确接收
        {
            break;
        }
        else if (finished < (end-start))  //断点续传
        {
        	if(curRecv == 0)//本次没有收到数据
        	{
	        	errcnt++;
	        	if(errcnt >= DOWNFILE_TRYCNT)
	        	{
	        		quit = 1;
	        		break;
	        	}
        	}
        	s_pos = start+finished;
			e_pos = end-1;
			sprintf(range, "%d-%d",s_pos,e_pos);
        }
        else//错误接收
        {
        	//hyUsbPrintf("recv error ! id = %d \r\n",threadId);
        	break;
        }
    }
    
    hyIE_destroy();
	unlock(&gu32LockIE);
	
	return finished;
}

//////////////////////示例//////////////////////////
/*
post请求数据
int test_putfile(void *p)
{
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	struct protocol_http_request tRequest;

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	
	//recvLen:文件大小
	
	return recvLen;
}
*/

/*
get文件大小
int test_putfile(void *p)
{
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	struct protocol_http_request tRequest;

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_GET_METHOD;
    tRequest.user_agent     = NULL;
    tRequest.content_length = 0;
    tRequest.content_data   = NULL;
    tRequest.referer        = NULL;
    tRequest.content_type   = NULL;
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	
	return ret;
}
*/

/*
get文件数据
int test_putfile(void *p, char *range)
{
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	struct protocol_http_request tRequest;

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_GET_METHOD;
    tRequest.user_agent     = NULL;
    tRequest.content_length = 0;
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = NULL;
    tRequest.range          = range;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	
	//recvLen:本次接收数据大小
	
	return ret;
}
*/

/*
//上传文件
int test_putfile(U8 *pData, char *pPath, int datalen, int bufLen)
{
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	char			disposition[200];
	struct protocol_http_request tRequest;
	
	//pPath要上传文件的路径   格式: C:\\123.gz
	sprintf(disposition, "form-data; name=\"file\"; filename=\"%s\"", pPath);
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = datalen;
    tRequest.content_data   = pData;
    tRequest.referer        = NULL;
    tRequest.content_type   = "multipart/form-data;";
    tRequest.Content_Disposition = disposition;
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	
	return ret;
}*/