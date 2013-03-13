/**********************************************************************
收派区域下载   非实时占用buf,只有在下载或查询时才使用,所以没有固定分配buf
**********************************************************************/
#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "AbstractFileSystem.h"
#include "Net_Evt.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"


typedef struct ProvinceSearchList
{
	U8 *Province;//省份名称
	U16 Count;//查询到的记录条数
}ProvinceSearchList;
U8 ProvinceFlag[34];
ProvinceSearchList Province[34] = 
{
	{(U8 *)"北京市",0},
	{(U8 *)"天津市",0},
	{(U8 *)"河北省",0},
	{(U8 *)"山西省",0},
	{(U8 *)"内蒙古",0},
	{(U8 *)"辽宁省",0},
	{(U8 *)"吉林省",0},
	{(U8 *)"黑龙江省",0},
	{(U8 *)"上海市",0},
	{(U8 *)"江苏省",0},
	{(U8 *)"浙江省",0},
	{(U8 *)"安徽省",0},
	{(U8 *)"福建省",0},
	{(U8 *)"江西省",0},
	{(U8 *)"山东省",0},
	{(U8 *)"河南省",0},
	{(U8 *)"湖北省",0},
	{(U8 *)"湖南省",0},
	{(U8 *)"广东省",0},
	{(U8 *)"广西",0},
	{(U8 *)"海南省",0},
	{(U8 *)"重庆市",0},
	{(U8 *)"四川省",0},
	{(U8 *)"贵州省",0},
	{(U8 *)"云南省",0},
	{(U8 *)"西藏",0},
	{(U8 *)"陕西省",0},
	{(U8 *)"甘肃省",0},
	{(U8 *)"青海省",0},
	{(U8 *)"宁夏",0},
	{(U8 *)"新疆",0},
	{(U8 *)"台湾省",0},
	{(U8 *)"香港",0},
	{(U8 *)"澳门",0}	
};
#define KYEWORDLEN 34
extern U8  gu8Url[];

U8  gu8AreaVer[AREA_VER_LEN], gu8AreaVerTemp[AREA_VER_LEN];
U8 Keyword[KYEWORDLEN];

/*---------------------------CSV文件规范-------------------------
附：
CSV文件，也叫逗号分隔值文件，英文名称COMMA SEPARATED VALUE。具体格式规则如下：
* 每条记录占一行
* 以逗号为分隔符
* 逗号前后的空格会被忽略
* 字段中包含有逗号，该字段必须用双引号括起来
* 字段中包含有换行符，该字段必须用双引号括起来
* 字段前后包含有空格，该字段必须用双引号括起来
* 字段中的双引号用两个双引号表示
* 字段中如果有双引号，该字段必须用双引号括起来
* 第一条记录，可以是字段名
----------------------------------------------------------------*/
U8		*gu8Allspace,*pgu8Strs,*pgu8Strcur,*gu8StrsShow;
tCSV	*ptAllinfo;
U16		*pgu16Searchs,*province_str,search_total=0;


/*---------------------------------------------------------------------
*函数：DownLoad_Area_Init
*功能：初始化收派区域信息
*参数：pBuf:外部提供buf
*返回：
----------------------------------------------------------------------*/
void DownLoad_Area_Init(U8 *pBuf)
{
	pBuf = (U8 *)(((U32)pBuf + 4) & 0xfffffffc); //4字节对齐
	
	gu8Allspace = pBuf;
	memset(gu8Allspace,0,SITE_MEM_MAX);
	ptAllinfo   = (tCSV *)gu8Allspace ;
	pgu8Strs    = gu8Allspace + sizeof(tCSV)*SITE_MAX_RECORD;
	pgu8Strcur  = pgu8Strs ;
	pgu16Searchs= (U16 *)(pBuf+SITE_MEM_MAX);
	memset((char *)pgu16Searchs,0,0x20000);
	pgu16Searchs = (U16 *)(((U32)pgu16Searchs + 4) & 0xfffffffc); //4字节对齐
	province_str = (U8 *)hsaSdram_GetChaXunBuf();
	memset(province_str,0,hsaSdram_GetChaXunBufSize());
}


void DownLoad_Area_WriteFile(U8 *ver)
{
	int fp,size;
	U16 crc;
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH,AFS_SINGLE_DELETE_FDM);
	}
	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		size = pgu8Strcur - gu8Allspace ;
		size = size > SITE_MEM_MAX ? SITE_MEM_MAX : size ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,size);
		
		AbstractFileSystem_Write(fp,ver,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,size);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strncpy(gu8AreaVer,ver,AREA_VER_LEN);
	}
}


int DownLoad_Area_ReadFile(void)
{
	U16 crc ;
	int fp , size;
	
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp < 0) return -1 ;
	
	size = AbstractFileSystem_FileSize(fp);
	if(size <= AREA_VER_LEN+4)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH,AFS_SINGLE_DELETE_FDM);
		memset(gu8AreaVer,0,AREA_VER_LEN);
		return -1;
	}
	size -= (AREA_VER_LEN+4);
	
	AbstractFileSystem_Read(fp,gu8AreaVer,AREA_VER_LEN);
	AbstractFileSystem_Read(fp,(char *)&search_total,sizeof(U16));
	AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
	AbstractFileSystem_Read(fp,gu8Allspace,size);
	AbstractFileSystem_Close(fp);
	if(crc != glbVariable_CalculateCRC((U8 *)gu8Allspace,size))
	{
		memset(gu8AreaVer,0,AREA_VER_LEN);
		return -2 ;
	}
	
	return 0;
}


int DownLoad_Area_InitVer(void)
{
	U16 crc ;
	int fp , size;
	
	memset(gu8AreaVer,0,AREA_VER_LEN);
	
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp < 0) return -2 ;
	
	size = AbstractFileSystem_FileSize(fp);
	size -= (AREA_VER_LEN+2);
	
	//节省时间就不对数据进行校验了
	AbstractFileSystem_Read(fp,gu8AreaVer,AREA_VER_LEN);
	//AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
	//AbstractFileSystem_Read(fp,pdatabuf,size);
	AbstractFileSystem_Close(fp);
	
	//if(crc != glbVariable_CalculateCRC((U8 *)pdatabuf,size))
	//{
	//	memset(gu8AreaVer,0,AREA_VER_LEN) ;
	//	return -2;
	//}
	
	return 0 ;
}



/*-------------------------------------------------------------------------
* 函数: DownLoad_Area_GetOneline
* 说明: 取一行字符数据,0d 0a 认为是一行
* 参数: 
* 返回: 
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_GetOneline(U8 *sin,U32 maxlen,U8 *sout,U32 curlen)
{
	U32 i,j,k ;
	U32 l ;
	U8  *pi,*po ;

	i  = j = k = l = 0 ;
	pi = sin ;
	po = sout; 
	
	while (1)
	{
		if (0 == *pi)
		{
			if (curlen+i < maxlen)
			{
				i++;
				pi++;
				continue ;
			}
			else
			{
				return -1 ;
			}
		}
		
		if (0x0d == *pi && 0x0a == *(pi+1))
		{
			break;
		}
		*po++ = *pi++ ;
		i++ ;
		
		if(i >= 0x20000)
		{
			return -2 ;
		}

	}
	i += 2 ; // 回车换行
	
	return  i ;
}
#else
int DownLoad_Area_GetOneline(U8 *sin,U32 maxlen,U8 *sout,U32 curlen)
{
	U32 i,j,k ;
	U32 l ;
	U8  *pi,*po ;

	i  = j = k = l = 0 ;
	pi = sin ;
	po = sout; 
	
	while (1)
	{
		
		if (0 == *pi)
		{
			if (curlen+i < maxlen)
			{
				i++;
				pi++;
				continue ;
			}
			else
			{
				return -1 ;
			}
		}
		if (curlen==0&&0x0d == *pi && 0x0a == *(pi+1))
		{
			break;
		}
		else if (0xa1 == *pi && 0xa3 == *(pi+1)&&0x0d == *(pi+2)&&0x0a == *(pi+3)&&(0x00 == *(pi+4)||(0x30 <= *(pi+4)&&0x39 >= *(pi+4))))//句号\r\n作为一行的结束
		{
			break;
		}
		*po++ = *pi++ ;
		i++ ;
		
		if(i >= 0x20000)
		{
			return -2 ;
		}

	}
	i += 2 ; // 句号
	i += 2 ; // 回车换行
	
	return  i ;
}
#endif

/*-------------------------------------------------------------------------
* 函数: DownLoad_Area_ParseOneline
* 说明: 处理一行字符串
* 参数: 
* 返回: 
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_ParseOneline(U8 *sin,U32 inlen,tCSV *pt)
{
	U8  i ;
	U8  *pi ,*ptem;
	U8  scnt ;               //记录连续双引号的个数 ，奇数个:元素开始或结束 ；偶数个:字符是引号
	U8  index ,elef;

	ptem = pgu8Strcur ;
	pi = sin ;
	scnt = 0 ;
	index= 0 ;
	elef = 0 ;
	pt->offset[index] = ptem - pgu8Strs ;
	
	while (1)
	{
		if (0 == *pi) break;
		
		// ele start 
		if ('"' == *pi)
		{	
			pi++   ;
			scnt++ ;
			continue ;
		}
		else
		{
			if (scnt&0x01 != 0)//奇数个 "
			{
				for (i = 0 ; i < (scnt-1)/2 ; i++)
				{
					*ptem++ = '"' ;
				}

				if (',' == *pi)// 一个元素结束
				{
					elef = 0 ;
					pi++ ;
					index++ ;
					ptem++  ; // 没个字符多留一个空间置 0
					pt->offset[index] = ptem - pgu8Strs ;
				}
				else
				{
					elef = 1 ; //进入一个元素
					*ptem++ = *pi++ ;
				}
			}
			else               //偶数个或0个
			{
				if (0 != scnt)//元素部分含有 "
				{
					for (i = 0 ; i < scnt/2 ; i++)
					{
						*ptem++ = '"' ;
					}
					*ptem++ = *pi++ ;
				}
				else
				{
					if (',' == *pi && 0 == elef)// 一个元素结束
					{
						pi++ ;
						index++ ;
						ptem++  ; // 没个字符多留一个空间置 0
						pt->offset[index] = ptem - pgu8Strs ;
					}
					else
					{
						*ptem++ = *pi++ ;
					}
				}
			}
			
			scnt = 0 ;
		}
	}
	ptem++;//保留一个0字符作为结束符
	pgu8Strcur = ptem ;

	return 0;
}
#else
int DownLoad_Area_ParseOneline(U8 *sin,U32 inlen,tCSV *pt)
{
	U8  i ;
	U8  *pi ,*ptem;
	U8  scnt ;               //记录连续双引号的个数 ，奇数个:元素开始或结束 ；偶数个:字符是引号
	U8  index ,elef;
	ptem = pgu8Strcur ;
	pi = sin ;
	scnt = 0 ;
	index= 0 ;
	elef = 0 ;
	pt->offset[index] = ptem - pgu8Strs ;
	
	while (1)
	{
		if (0 == *pi) break;
		
		// ele start 
		if ('"' == *pi)
		{	
			pi++   ;
			scnt++ ;
			continue ;
		}
		else
		{
			if (scnt&0x01 != 0)//奇数个 "
			{
				for (i = 0 ; i < (scnt-1)/2 ; i++)
				{
					*ptem++ = '"' ;
				}

				if ('	' == *pi)// 一个元素结束
				{
					elef = 0 ;
					*pi = 0;
					pi++ ;
					index++ ;
					ptem++  ; // 每个字符多留一个空间置 0
					pt->offset[index] = ptem - pgu8Strs ;
					hyUsbPrintf("以来\r\n");
				}
				else
				{
					elef = 1 ; //进入一个元素
					*ptem++ = *pi++ ;
				}
			}
			else               //偶数个或0个
			{
				if (0 != scnt)//元素部分含有 "
				{
					for (i = 0 ; i < scnt/2 ; i++)
					{
						*ptem++ = '"' ;
					}
					*ptem++ = *pi++ ;
				}
				else
				{
					if ('	' == *pi && 0 == elef)// 一个元素结束
					{
						*pi = 0;
						pi++ ;
						index++ ;
						ptem++  ; // 每个字符多留一个空间置 0
						pt->offset[index] = ptem - pgu8Strs ;
							
					}
					else
					{
						*ptem++ = *pi++ ;
					}
				}
			}
			
			scnt = 0 ;
		}
	}
	ptem++;//保留一个0字符作为结束符
	pgu8Strcur = ptem ;

	return 0;
}
#endif

/*-------------------------------------------------------------------------
* 函数: DownLoad_Area_DataProcess
* 说明: 处理获取的文件数据
* 参数:    注意：pStrsIn buf 参数在使用之前一定要做清空否则 DownLoad_Area_GetOneline 函数可能不会结束
* 返回: 0 :success ;
* -------------------------------------------------------------------------*/
void DownLoad_Area_DataProcess(U8 *pStrsIn,U32 totallen)
{
	U32  curIndex             ;
	U8   *outbuf,*inbuf       ;
	int  datalen,ret          ;
	U16  i                    ;
	
	curIndex = 0 ;
	inbuf  = pStrsIn ;
	outbuf = (U8 *)hsaSdram_DecodeBuf() ;
	memset(outbuf,0,hsaSdram_DecodeSize()) ;
hyUsbPrintf("str addr = 0x%x   len = %d  out addr = 0x%x \r\n", (U32)pStrsIn, totallen, (U32)outbuf);
	//文件的第一行是标题,读取并去掉
	datalen = DownLoad_Area_GetOneline(inbuf,totallen,outbuf,curIndex);
	if (datalen < 0)return;  //文件结束了
	inbuf += datalen ;
	curIndex += datalen ;
	
	for(i = 0 ; i < SITE_MAX_RECORD; i++)
	{
		memset(outbuf,0,hsaSdram_DecodeSize()) ;
		datalen = DownLoad_Area_GetOneline(inbuf,totallen,outbuf,curIndex);
		hyUsbPrintf("line %d:\r\n",i);
		if (datalen < 0)
		{
			search_total = i;
			break;  //文件结束了
		}
		inbuf += datalen ;
		curIndex += datalen ;
		
		memset(&ptAllinfo[i], 0, sizeof(tCSV));
		ret = DownLoad_Area_ParseOneline(outbuf,0x10000,&ptAllinfo[i]);
		//hyUsbPrintf("i = %d  ret = %d  addr = 0x%x \r\n", i, ret, (U32)&ptAllinfo[i]);
	}
	
	hyUsbPrintf("ret = %d \r\n", ret);
}


/*-------------------------------------------------------------------------
* 函数: DownLoad_Area_GetFileData
* 说明: 获取site.csv文件数据
* 参数: 
* 返回: 0 :success
* -------------------------------------------------------------------------*/
int DownLoad_Area_GetFileData(void)
{
	int  ret ,fp ;
	int  datalen ;
	U8     *pTem ;
	
	ret = 1 ;
	/* 分配空间暂存处理文件数据 */
	pTem = (U8 *)hsaSdramReusableMemoryPool();
	memset(pTem,0,SITE_FILE_MAX);
	
	/* 读取文件数据 */
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp >= 0)
	{
		datalen = AbstractFileSystem_Read(fp,(char *)pTem,SITE_FILE_MAX);
		if(datalen > 0)
		{
			//PhoneTrace4(0,"read : = %d",datalen);
			/* 对数据进行处理 */
 			DownLoad_Area_DataProcess(pTem,datalen);
		}
		else
		{
			ret = 3 ;
		}
		AbstractFileSystem_Close(fp);
	}
	else
	{
		ret = 2 ;
	}
	
	return ret ;
}

/*-------------------------------------------------------------------------
* 函数: DownLoad_Area_Search
* 说明: 
* 参数: 
* 返回: 0 :success ;
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_Search(U8 *strs, U8 searchf, tCSV *ptin, U16 *pout)
{
	tCSV *ptNext;
	int i,n;
	U8  index, *curStr;

	ptNext = ptin;
	index = searchf;
	n = 0;
	for (i = 0 ; i < search_total/*SITE_MAX_RECORD*/; i++)
	{
		//hyUsbPrintf("addr = 0x%x   off = %d \r\n", (U32)pgu8Strs, ptNext[i].offset[index]);
		curStr = (U8 *)((U32)pgu8Strs+ptNext[i].offset[index]);
		//hyUsbPrintf("i = %d  str = %s \r\n", i, curStr);
		//if(curStr == pgu8Strs && i > 0)break; //搜索完毕
		if (0 != strstr(curStr,strs,strlen(strs)))
		{
			pout[n] = i;
			n++;
		}	
	}

	return  n;
}
#else
int DownLoad_Area_Search(U8 *strs,U8 searchf, tCSV *ptin,U16 *pout)
{
	tCSV *ptNext ;
	int i=0 ,n=0,j=0;
	U8  index ,*curStr;
	
	ptNext = ptin ;
	
	index = searchf;
	for(i=0;i<34;i++)
	{
		Province[i].Count=0;
	}
	hyUsbPrintf("search_total = %d\r\n",search_total);
	for (i = 0 ; i < search_total ; i++)
	{
		curStr = (U8 *)((U32)pgu8Strs+ptNext[i].offset[index]);
		//if(curStr == pgu8Strs && i > 0)break; //搜索完毕
		if (0 != strstr(curStr,strs,strlen(strs)))
		{
			pout[n] = i ;
			n++;
			for(j=0;j<34;j++)
			{
				if (0 != strstr((U8 *)((U32)pgu8Strs+ptNext[i].offset[4]),Province[j].Province,strlen(Province[j].Province)))
				{
					Province[j].Count++;
					//hyUsbPrintf("%s/%s:%d/%d,n=%d\r\n",(U8 *)((U32)pgu8Strs+ptNext[i].offset[4]),Province[j].Province,i,pout[n-1],n-1);
					break;
				}
			}
		}	
	}

	return  n ;

}
#endif


//size:0:表示读   >0:表示写
int DownLoad_TempFile(U8 *pBuf, int size, U8 *pVer)
{
	int fp, len=0;
	int filesize;
	U8  ver[AREA_VER_LEN];
	
	if(size == 0)
	{
		fp = AbstractFileSystem_Open(AREA_TEMPFILE_PATH, AFS_READ);
		if(fp >= 0)
		{
			filesize = AbstractFileSystem_FileSize(fp);
			if(filesize <= AREA_VER_LEN)
			{
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
			}
			else
			{
				AbstractFileSystem_Seek(fp, 0, AFS_SEEK_OFFSET_FROM_START);
				AbstractFileSystem_Read(fp, ver, AREA_VER_LEN);
				if(strcmp(pVer, ver) == 0)//版本相同
				{
					len = filesize-AREA_VER_LEN;
					AbstractFileSystem_Read(fp, pBuf, len);
				}
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
			}
		}
	}
	else
	{
		fp = AbstractFileSystem_Open(AREA_TEMPFILE_PATH, AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
		}

		fp = AbstractFileSystem_Create(AREA_TEMPFILE_PATH);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp, pVer, AREA_VER_LEN);
			AbstractFileSystem_Write(fp, pBuf, size);
			AbstractFileSystem_Close(fp);
		}
		
		len = size+AREA_VER_LEN;
	}
		
	return len;
}


int DownLoad_DelAndSave(U8 *pBuf, int size)
{
	int fp, len;
	U16 crc;
	
	//把csv文件保存一份,其实没必要  只是为了后续查看方便
	fp = AbstractFileSystem_Open(AREA_CSVFILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_CSVFILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp >= 0)
	{
		AbstractFileSystem_Write(fp, pBuf, size);
		AbstractFileSystem_Close(fp);
	}
	
	//解析文件,保存解析后的文件
	DownLoad_Area_Init((U8 *)hsaSdram_UpgradeBuf());
	DownLoad_Area_DataProcess(pBuf, size);
	fp = AbstractFileSystem_Open(AREA_FILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		len = pgu8Strcur - gu8Allspace ;
		hyUsbPrintf("len = %d   0x%x  0x%x\r\n", len, (U32)pgu8Strcur, (U32)gu8Allspace);
		len = len > SITE_MEM_MAX ? SITE_MEM_MAX : len ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,len);
		
		AbstractFileSystem_Write(fp,gu8AreaVerTemp,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,len);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strcpy(gu8AreaVer,gu8AreaVerTemp);
	}
	
	return 0;
}
int Gznd_DownLoad_DelAndSave(U8 *pBuf, int size,U8 *ver)
{
	int fp, len;
	U16 crc;
	
	//把csv文件保存一份,其实没必要  只是为了后续查看方便
	fp = AbstractFileSystem_Open(AREA_CSVFILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_CSVFILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp >= 0)
	{
		AbstractFileSystem_Write(fp, pBuf, size);
		AbstractFileSystem_Close(fp);
	}
	
	//解析文件,保存解析后的文件
	DownLoad_Area_Init((U8 *)hsaSdram_UpgradeBuf());
	DownLoad_Area_DataProcess(pBuf, size);
	fp = AbstractFileSystem_Open(AREA_FILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		len = pgu8Strcur - gu8Allspace ;
		hyUsbPrintf("len = %d   0x%x  0x%x\r\n", len, (U32)pgu8Strcur, (U32)gu8Allspace);
		len = len > SITE_MEM_MAX ? SITE_MEM_MAX : len ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,len);
		
		AbstractFileSystem_Write(fp,ver,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&search_total,sizeof(U16));
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,len);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strcpy(gu8AreaVer,ver);
	}
	
	return 0;
}
//1:成功   3:网络连接失败   11:已是最新版本  12:服务器返回数据格式有误    >20:服务器返回的错误
int DownLoad_DownArea(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA	*pUp;
	int		filesize, bufLen, recvLen, start;
	U8		*pRecv, *pUnComp;
	
	pUp = (tCARTOONPARA *)p;
	pRecv = pUp->pdata;
	bufLen= pUp->datalen;
	
	filesize = http_getsize(gu8Url);
hyUsbPrintf("filesize = %d  bufLen = %d \r\n", filesize,bufLen);
	if(filesize <= 0)
	{
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//网络连接失败
		return 3;
	}
	else if(filesize >= 0x300000)
	{
		OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
		return 12;
	}
	
	//开始下载
	//先看看是否有上次下载失败保存的临时文件
	start = DownLoad_TempFile(pRecv, 0, gu8AreaVerTemp);
	
	Com_SetPercentTotal(filesize);
	Com_SetPercentRecv(start);
	recvLen = http_downfile(gu8Url, start, filesize, pRecv, bufLen);
hyUsbPrintf("recvLen ===== %d  filesize = %d \r\n",recvLen,filesize);
	if(recvLen <= 0)//没有下载到数据
	{
		OpCom_SetLastErrCode(ERR_UPGRADEFAIL);
		return 3;
	}
	else if(recvLen < filesize)//没有下载完
	{
		//保存已下载的文件
		DownLoad_TempFile(pRecv, recvLen, gu8AreaVerTemp);
		
		OpCom_SetLastErrCode(ERR_UPGRADEFAIL);
		return 3;
	}
	
	//下载完成, 解压
	pUnComp = (U8 *)hsaSdramReusableMemoryPool();
	bufLen  = 0x400000;
	if(0 <= Gzip_Uncompress(pRecv,recvLen,pUnComp,&bufLen,1))
	{
		if(0 != DownLoad_DelAndSave(pUnComp, bufLen))
		{
			OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
			return 12;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
		return 12;
	}
	
	OpCom_SetLastErrCode(ERR_SUCCESS);
	return 1;
}


//1:成功   3:网络连接失败   11:已是最新版本  12:服务器返回数据格式有误    >20:服务器返回的错误
int DownLoad_GetAreaInfo(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA	*pUp;
	int		ret = 1, bufLen, recvLen, dataLen;
	U8		*pSend, *pS, *pE, *pGbk;
		
	if(Net_ConnectNetWork() != 1)
	{
	hyUsbPrintf("sendRequest 连接失败\r\n");
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//网络连接失败
		return 3;
	}
	
	pUp = (tCARTOONPARA *)p;
	pSend = pUp->pdata;
	bufLen= pUp->datalen;
	
	//组织请求数据
	strcpy(pSend, "<req op=\"getsiteinfo\"><h><ver>1.0</ver><company>529951</company><user>1007</user><pass>202cb962ac59075b964b07152d234b70</pass><dev1>180000666</dev1><dev2>00000000000</dev2></h><data><o><area_ver>2012-11-01</area_ver><site_ver>2012-11-01</site_ver></o></data></req>");
	
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
	
	recvLen = http_send_request("http://opws.yundasys.com:9900/ws/ws.jsp", bufLen, &tRequest);
/*<dta st="ok" res="0" op="getsiteinfo">
<h><ver>1.0</ver><time>2012-11-01 19:38:28</time></h>
<data>
<o><d>2011-12-13</d><d>http://pbzz.yundasys.com/area_20111213.zip</d></o>
<o><d>2011-12-13</d><d>http://pbzz.yundasys.com/site_20111213.zip</d></o>
</data>
</dta>*/	
	if(recvLen > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		recvLen = Net_DecodeData(pSend,recvLen,pGbk);
		if(recvLen > 0)
		{
			memcpy(pSend,pGbk,recvLen);
			pSend[recvLen] = 0;
			pSend[recvLen+1] = 0;

			pS = strstr(pSend,"st=\"ok\"");
			if(pS == NULL)
			{//失败
				OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
				return 12;
			}
			
			pS = strstr(pSend,"<o><d>");
			if(pS == NULL)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
				return 12;
			}
			pSend = pS+strlen("<o><d>");
			pS = strstr(pSend,"<o><d>");
			if(pS == NULL)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
				return 12;
			}
			pS += strlen("<o><d>");
			pE = strstr(pS,"</d><d>");
			dataLen = pE - pS;
			if(dataLen >= AREA_VER_LEN)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
				return 12;
			}
			memcpy(gu8AreaVerTemp, pS, dataLen);
			gu8AreaVerTemp[dataLen] = 0;
			
			pS = pE+strlen("</d><d>");
			pE = strstr(pS,"</d></o>");
			dataLen = pE - pS;
			if(dataLen >= 256)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//数据错误
				return 12;
			}
			memcpy(gu8Url, pS, dataLen);
			gu8Url[dataLen] = 0;
			
			if(strcmp(gu8AreaVerTemp, gu8AreaVer) <= 0)
			{
				OpCom_SetLastErrCode(ERR_NEWEST);//数据错误
				return 11;
			}
			
			OpCom_SetLastErrCode(ERR_SUCCESS);
			return 1;
		}
		else
		{
			OpCom_SetLastErrCode(ERR_SENDFAIL);
			return 2;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_SENDFAIL);
		return 2;
	}
	
	return ret;
}

/*--------------------------------------------------
* 函数:DownLoad_Area_Action
* 功能:下载派送区域的界面执行函数
* 参数: flag:0:获取信息    1:下载
* 返回: 0:成功   其他失败,具体失败原因可以调用OpCom_GetLastErrCode得到
----------------------------------------------------*/
int DownLoad_Area_Action(tGRAPDESKBOX *pDesk, U8 flag)
{
	int				ret = -1;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	lastValue = BackLight_CloseScreenProtect();//关闭屏保
		
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	
	if(flag == 0)
	{
		//获取版本信息
		cartoon_app_entry(DownLoad_GetAreaInfo,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在获取信息,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)
			{
				ret = 0;
			}
		}
		else
		{
			OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
		}
	}
	else if(flag == 1)
	{
		Com_SetPercentTotal(0);
		Com_SetPercentRecv(0);
		cartoon_app_entry(DownLoad_DownArea,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新资料,请稍候...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
					
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)//成功
			{
				ret = 0;
			}
		}
		else
		{
			OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
		}
	}
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//打开屏保
	
	return ret;
}

/*---------------------------------------------------------
*函数: Op_DownLoad_Area_Desk
*功能: 
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Op_DownLoad_Area_Desk(void *pDesk, U8 ifSearch)
{
	U64 		totalspace,freespace = 0;
	int			ret;
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否更新派送区域?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
	{
		AbstractFileSystem_VolumeSpace( "C", &totalspace, &freespace);
		freespace >>= 20;
		if(freespace < 6)//M
		{
			Com_SpcDlgDeskbox("磁盘空间不足,需6M.",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			return -1;
		}
		
		//设置网络事件
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			return -1;
		}
		
		ret = DownLoad_Area_Action(pDesk, 0);
		if(0 == ret || OpCom_GetLastErrCode() == ERR_NEWEST)
		{
			if(OpCom_GetLastErrCode() == ERR_NEWEST)//已是最新版本
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("已是最新版本,是否更新?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
				{
					ret = 0;
				}
				else
				{
					ret = 11;
				}
			}
			if(ret == 0)
			{
				ret = DownLoad_Area_Action(pDesk, 1);
				if(ret == 0)
				{
					Com_SpcDlgDeskbox("更新完成!",0,pDesk,1,NULL,NULL,100);
				}
				else
				{
					OpCom_ErrorRemind(pDesk);
				}
			}
		}
		else
		{
			OpCom_ErrorRemind(pDesk);
		}
	}
	
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++显示详细信息++++++++++++++++++++++++++++++++++++++++++++*/
U16 gu16KeyColor;
U8  *gpu8KeyStr;
const tGrapViewAttr AreaInfoViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//背景图片1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{EDIT_ID_1,   5, 40,232, 240,      5,  40,  237,  280,   0,	0,	0,0,1,0,0,0},//每行29个字符
};

const tGrapButtonPriv  AreaInfoBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapEdit2Priv  AreaInfoEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,10240,1,1,1,0},//显示
};

int AreaInfo_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}

S32 AreaInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos)
{
	tCHARPOS *pSub;
	
	*firstCharPos = 0;
	pSub = pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			*firstCharPos = pSub->firstCharPos;
			break;
		}
		pSub = pSub->pNext;
	}
	
	return 0;
}

//画指定行
S32 AreaInfo_Edit2DrawOneLine(void *pView, U32 line_idx, U16 x_start, U16 y_start)
{
	tGRAPEDIT2 *pEdit;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tGrapEdit2Priv *pedit_privattr;
	tGrapViewAttr  *pview_attr;
	U32 totalCharLen, pos, tempPos;      //要显示行首字符在字符串中的位置
	tStrDispInfo disp_info;
	Character_Type_e kLycStr_Type;
	U32 curDrawLen;
	U8  *pTempStr;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	AreaInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx, &pos);
	
	if(line_idx == pedit_ctrl->totLine-1)//最后一行
	{
		kLycStr_Type = pedit_privattr->font_type;//字符类型
		totalCharLen = Get_StringLength(kLycStr_Type,(pedit_privattr->digt+pos));//字符个数(UNICODE半角的也按2计)
		curDrawLen = totalCharLen;
	}
	else
	{
		AreaInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx+1, &tempPos);
		curDrawLen = tempPos - pos;
	}
	pTempStr = (U8 *)malloc(curDrawLen+2);
	if(NULL == pTempStr) return -1;
	
	memcpy(pTempStr,(U8*)(&pedit_privattr->digt[pos]),curDrawLen);
	pTempStr[curDrawLen] = 0;
	pTempStr[curDrawLen+1] = 0;
	disp_info.string = pTempStr;
	disp_info.color = pedit_privattr->color;
	disp_info.font_size = pedit_privattr->font_size;
	disp_info.font_type = pedit_privattr->font_type;
	disp_info.flag = pedit_privattr->position;
	disp_info.keyStr = gpu8KeyStr;
	disp_info.keyStrColor = gu16KeyColor;

	Grap_WriteString(x_start+pedit_privattr->xoffset, y_start+pedit_privattr->yoffset, 
				(U16)(x_start+pview_attr->viewWidth),
				(U16)(y_start + pview_attr->viewHeight), &disp_info);
	free((void *)pTempStr);
	return SUCCESS;
}


S32 AreaInfo_Edit2Draw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U32 i = 0;
	U32 line;  //一屏显示的行数
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	//clear edit screen
	if (pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
	}
	else if(pedit_privattr->coverPicId)
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	else
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}

	//draw lines  from top line
	for ( i = pedit_ctrl->topLine_idx; i < pedit_ctrl->topLine_idx + pedit_ctrl->line_screen; i++ )
	{
		if ( i < pedit_ctrl->totLine )
		{
			AreaInfo_Edit2DrawOneLine(pView, i, pview_attr->viewX1, 
					pview_attr->viewY1 + (i-pedit_ctrl->topLine_idx)*pedit_ctrl->line_height);
		}
	}
	
	return SUCCESS;
}

//显示查询的详细信息
int AreaInfo_ShowDesk(void *pDeskFather, tCSV *dataStr, U8 *SearchStr)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	i,idx=0,len;
	U8				*pStrs;
	U8              *strs[13] = {"【公司编码】","【公司名称】","【公司类别】","【上级单位名称】",
								"【省份】","【负责人】","【手机】","【电话】","【传真】",
								"---------【派送区域】--------","--------【不派送区域】-------","【时效】","【备注】"}; 
								
								
	
	gpu8KeyStr  = SearchStr;
	gu16KeyColor= COLOR_YELLOW;
	gu8StrsShow = (U8 *)hsaSdram_FrontBuf3() ;	
	memset(gu8StrsShow,0,hsaSdram_Front3Size());
							
	pDesk = Grap_CreateDeskbox(&AreaInfoViewAttr[idx++], NULL);	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&AreaInfoViewAttr[idx++],&AreaInfoBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = AreaInfo_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &AreaInfoViewAttr[idx++], &AreaInfoEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	pEdit2->draw = AreaInfo_Edit2Draw;
	pEdit2->edit_privattr.digt = gu8StrsShow;
	for(i = 0 ; i < sizeof(strs)/sizeof(U8*) ; i++)
	{
		pStrs = pgu8Strs + dataStr->offset[i];
		strcat(pEdit2->edit_privattr.digt,strs[i]);
		len = strlen(strs[i]);
		if(0 != *pStrs)
		{
			strcat(pEdit2->edit_privattr.digt,pStrs);
			len += strlen(pStrs);
		}
		if(len % 29)
		{
			strcat(pEdit2->edit_privattr.digt,"\r\n");
		}
	}
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*++++++++++++++++++++++++++++++++++++++++++++把查询到的站点按省份分类++++++++++++++++++++++++++++++++++++++++++++++++*/

const tGrapViewAttr Province_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tGrapListerPriv Province_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,30,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv Province_BtnPriv[]=
{
	{(U8 *)"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 Province_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ESC
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Province_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	tGRAPDESKBOX 	*pDesk;
	U16 len1,len2,j=0 ;
	U8  *pcomCode ,*pcomName,*ptem;
	
	pcomCode = pgu8Strs + ptAllinfo[province_str[i]].offset[0] ;
	pcomName = pgu8Strs + ptAllinfo[province_str[i]].offset[1] ;
	ptem     = str ;
	
	len1 = strlen(pcomCode);
	len2 = strlen(pcomName);
	
	if(len1+len2 < maxLen-2)
	{
		memcpy(ptem,pcomCode,len1);
		ptem[len1] = '|';
		ptem += len1+1 ;
		memcpy(ptem,pcomName,len2);
		ptem[len2] = 0;
		ptem[len2+1] = 0;
	}
	else
	{
		if(len1 < maxLen-2)
		{
			memcpy(ptem,pcomCode,len1);
			ptem[len1] = '|';
			ptem += len1+1 ;
			memcpy(ptem,pcomName,maxLen-len1-1);
			ptem[maxLen-1] = 0;
			ptem[maxLen-2] = 0;
		}
		else
		{
			memcpy(ptem,pcomCode,maxLen);
			str[maxLen-1] = 0; 
			str[maxLen-2] = 0; 
		}
	}	
		
	return SUCCESS;
}

S32 Province_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	pDesk = (tGRAPDESKBOX *)pLister->pParent ;
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	AreaInfo_ShowDesk(pDesk,&ptAllinfo[province_str[i]],Keyword);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	return RETURN_REDRAW ;
}

/*---------------------------------------------------------
*函数: ProvinceList_Desk
*功能: 派送区域查按省分类
*参数: 
*返回: 无
*---------------------------------------------------------*/
int ProvinceList_Desk(void *pDeskFather,U8 type)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	U8              index, *pBuf;
	
	index = 0;
	pDesk = Grap_CreateDeskbox(&Province_ViewAttr[index++], Province[ProvinceFlag[type]].Province);
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &Province_ViewAttr[index++], &Province_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = Province_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk, &Province_ViewAttr[index++], &Province_ListPriv[0]);
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = Province[ProvinceFlag[type]].Count;
	pList->getListString = Province_ListGetStr;
	pList->enter         = Province_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/*++++++++++++++++++++++++++++++++++++++++++++根据不同信息查询++++++++++++++++++++++++++++++++++++++++++++*/
const tGrapViewAttr AreaSearch_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_10,	   5, 35,72,16,    0,0,0,0,   0,0,0,0,1,0,0,0},//
	{EDIT_ID_1,	   	   10, 60,215,22,    10,60,225,82, 0,0,0xffff,0,1,1,1,0},
	
	{LISTER_ID_1,	   10,90,224,190,	10,90,234,280,	0,0,0,0,1,0,1,0},
};

const tGrapListerPriv AreaSearch_ListPriv[]=
{
	{24,7,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv AreaSearch_BtnPriv[]=
{
#ifdef W818C
	{"确定",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#else
	{"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
	{"确定",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
#endif	
	{NULL,0xffff,0x0000,0,0,1,0,0,0},
	
};

const tGrapEditPriv AreaSearch_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,26,  1, KYEWORDLEN-2,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//
};


S32 AreaSearch_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	tGRAPEDIT 	 	*pEdit;
	int  cnt,i;
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://确定
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk, LISTER_ID_1);
		
		if(strlen(pEdit->edit_privattr.digt) == 0)
		{
			Com_SpcDlgDeskbox("请输入查询内容!",0,pDesk,1,NULL,NULL,100);
			break;
		}
		
		
		cnt = DownLoad_Area_Search(pEdit->edit_privattr.digt,pDesk->view_attr.reserve,ptAllinfo,pgu16Searchs);
		
		if(cnt > 0)
		{
			
			pList->view_attr.reserve = cnt;
			cnt= 0;
			memset(ProvinceFlag,0,34);
			for(i=0;i<34;i++)
			{
				if(Province[i].Count>0)
				{
					ProvinceFlag[cnt++] = i;
				}
			}
			memset(Keyword,0,KYEWORDLEN);
			strcpy(Keyword,pEdit->edit_privattr.digt);
			Grap_ChangeFocus(pEdit,1);
		}
		pList->lst_privattr.totalItem = cnt ;
		break;
	case BUTTON_ID_2://返回
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}
#if 0
S32 AreaSearch_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	tGRAPDESKBOX 	*pDesk;
	U16 len1,len2 ;
	U8  *pcomCode ,*pcomName,*ptem;
	
	pcomCode = pgu8Strs + ptAllinfo[pgu16Searchs[i]].offset[0];
	pcomName = pgu8Strs + ptAllinfo[pgu16Searchs[i]].offset[1];
	ptem     = str ;
	
	len1 = strlen(pcomCode);
	len2 = strlen(pcomName);
	
	if(len1+len2 < maxLen-2)
	{
		memcpy(ptem,pcomCode,len1);
		ptem[len1] = '|';
		ptem += len1+1 ;
		memcpy(ptem,pcomName,len2);
		ptem[len2] = 0;
		ptem[len2+1] = 0;
	}
	else
	{
		if(len1 < maxLen-2)
		{
			memcpy(ptem,pcomCode,len1);
			ptem[len1] = '|';
			ptem += len1+1 ;
			memcpy(ptem,pcomName,maxLen-len1-1);
			ptem[maxLen-1] = 0;
			ptem[maxLen-2] = 0;
		}
		else
		{
			memcpy(ptem,pcomCode,maxLen);
			str[maxLen-1] = 0; 
			str[maxLen-2] = 0; 
		}
	}	
	
	return SUCCESS;
}
#else
S32 AreaSearch_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str,"%s | %d",Province[ProvinceFlag[i]].Province,Province[ProvinceFlag[i]].Count) ;
	return SUCCESS;
}
#endif
#if 0
S32 AreaSearch_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	tGRAPEDIT 	 	*pEdit;
	
	if(i < pLister->lst_privattr.totalItem)
	{
		pDesk = (tGRAPDESKBOX *)pLister->pParent ;
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		AreaInfo_ShowDesk(pDesk,&ptAllinfo[pgu16Searchs[i]],pEdit->edit_privattr.digt);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW ;
	
}
#else
S32 AreaSearch_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	U16 j,n=0;
	pDesk = (tGRAPDESKBOX *)pLister->pParent ;
	for(j=0;j<pLister->view_attr.reserve;j++)
	{
		U8 *temp = NULL;
		temp = pgu8Strs + ptAllinfo[pgu16Searchs[j]].offset[4] ;
		if(0 != strstr(temp,Province[ProvinceFlag[i]].Province))
		{
			province_str[n++] = pgu16Searchs[j];
			
		}
	}
	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ProvinceList_Desk(pDesk,i);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	
	return RETURN_REDRAW ;
	
}
#endif
/*---------------------------------------------------------
*函数: AreaSearch_Desk
*功能: 根据信息查询
*参数: type: 类型
*返回: 
*---------------------------------------------------------*/
int AreaSearch_Desk(void *pDeskFather, U8 searchid, U8 type)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	tGRAPEDIT 		*pEdit;
	U8              index = 0;
	int 			i;
	U8   *topword[5] = {"请输入公司代码","请输入公司名称","请输入公司电话","请输入可派送地","请输入不派送地"};
	
	pDesk = Grap_CreateDeskbox(&AreaSearch_ViewAttr[index++], "区域查询");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	pDesk->view_attr.reserve = searchid;
	
	for(i = 0; i < 3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&AreaSearch_ViewAttr[index++],&AreaSearch_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = AreaSearch_BtnEnter;
		}
		else
		{
			pBttn->btn_privattr.pName = topword[type];
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &AreaSearch_ViewAttr[index++], &AreaSearch_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(type == 1 || type == 3 || type == 4)
	{
		pDesk->inputMode = 4;
		pEdit->edit_privattr.input = 4;
	}
	
	pList = Grap_InsertLister(pDesk,&AreaSearch_ViewAttr[index++],&AreaSearch_ListPriv[0]);
	if (pList == NULL) return;
	pList->getListString = AreaSearch_ListGetStr;
	pList->enter         = AreaSearch_ListEnter;
	pList->lst_privattr.totalItem = 0;
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


/*++++++++++++++++++++++++++++++++++++++++++++派送区域查询++++++++++++++++++++++++++++++++++++++++++++++++*/
const U8 * const Hyco_SearchStr[]=
{
	(const U8 *)"根据公司代码查询",(const U8 *)"根据公司名称查询",
	(const U8 *)"根据公司电话查询",(const U8 *)"根据可派送地查询",
	(const U8 *)"根据不派送地查询"
};

const tGrapViewAttr Delivery_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tGrapListerPriv Delivery_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,30,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv Delivery_BtnPriv[]=
{
	{(U8 *)"返回",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 Delivery_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ESC
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Delivery_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if(i < pLister->lst_privattr.totalItem)
	{
		sprintf(str, "%2d:%s", i+1, Hyco_SearchStr[i]);
	}
	else
	{
		str[0] = 0;
	}

	return SUCCESS;
}

S32 Delivery_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk;
	 U8 config[5] = { 0,1,7,9,10};
	
	pDesk = (tGRAPDESKBOX *)(pLister->pParent);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	AreaSearch_Desk(pDesk, config[i], i);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			
	return RETURN_REDRAW;
}

/*---------------------------------------------------------
*函数: Delivery_InquireArea
*功能: 派送区域查询主界面
*参数: 
*返回: 无
*---------------------------------------------------------*/
int Delivery_InquireArea(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	U8              index, *pBuf;
	
	pBuf = (U8 *)hsaSdram_UpgradeBuf();
	DownLoad_Area_Init(pBuf);
	if(DownLoad_Area_ReadFile() < 0)
	{
		Com_SpcDlgDeskbox("无派送区域信息,请先点击\"资料下载\"进行更新!",6,pDeskFather,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		return 0;
	}
	
	index = 0;
	pDesk = Grap_CreateDeskbox(&Delivery_ViewAttr[index++], "区域查询");
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &Delivery_ViewAttr[index++], &Delivery_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = Delivery_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk, &Delivery_ViewAttr[index++], &Delivery_ListPriv[0]);
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = sizeof(Hyco_SearchStr)/sizeof(U8 *);
	pList->getListString = Delivery_ListGetStr;
	pList->enter         = Delivery_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

int DownLoadWholeFile(void *p);
int Op_DownLoad_Area(void *ppDesk, U8 ifSearch)
{
	tGRAPDESKBOX 	*pDesk;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,lastValue;
	
	pDesk = (tGRAPDESKBOX *)ppDesk;
	
	if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("确定更新收派区域列表?",0,pDesk,3,NULL,NULL,800))
	{
		return 0;
	}
	
	if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
	{
		return 0;
	}
	lastValue = BackLight_CloseScreenProtect();//关闭屏保
	
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	*pSend = 8;
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);	
	cartoon_app_entry(DownLoadWholeFile,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"正在更新资料,请稍候...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//打开屏保
	
	gpu16SearchInfo[0] = 0xffff;
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err == 1)
		{
			Com_SpcDlgDeskbox("更新完成!",0,pDesk,1,NULL,NULL,100);
		}
		else if(err == 11)
		{
			Com_SpcDlgDeskbox("数据下载错误!",0,pDesk,1,NULL,NULL,100);
		}
		else if(err == 2)
		{
			Com_SpcDlgDeskbox("网络通讯错误!",6, pDesk, 2,NULL,NULL,100);
		}
		else if(err == 3)
		{
			Com_SpcDlgDeskbox("网络连接失败,请稍候再试!",6, pDesk, 2,NULL,NULL,100);
		}
		else
		{
			err -= 20;
			if(err < 1 || err > 6) err = 0;
			Com_SpcDlgDeskbox(Net_ErrorCode[err],6,pDesk,2,NULL,NULL,100);
		}
	}
	else
	{
		Com_SpcDlgDeskbox("网络通讯错误,请稍候再试!",6, pDesk, 2,NULL,NULL,100);
	}
	return 0;
}
int file_parse_area(flag,pdata,datalen,getver)
{
	int fp;
	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp>=0)
	{
		AbstractFileSystem_Write(fp,(char *)pdata,datalen);
		AbstractFileSystem_Close(fp);
	}
	else
	{
		return HY_ERROR;
	}
	return HY_OK;
}