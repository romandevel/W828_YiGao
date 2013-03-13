#ifndef __ZLIB_FLASH_API_H__
#define __ZLIB_FLASH_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hyTypes.h"
#include "AbstractFileSystem.h"

//#ifndef NO_DUMMY_DECL
//struct internal_state      {int dummy;}; /* for buggy compilers */
//#endif

#define CHUNK 10*1024

typedef unsigned char uchar; 

#define PLATFORM_S2100

#ifndef PLATFORM_S2100

#define STREAM_READ "rb"
#define STREAM_WRITE "wb"
#define STREAM_RDWR
#define STREAM_SEEK_BEGIN SEEK_SET
#define STREAM_SEEK_CURENT SEEK_CUR
#define STREAM_SEEK_END SEEK_END

#define OPENFILE(FileName,Mode) fopen(FileName,Mode)
#define CLOSEFILE(pFile) fclose(pFile)
#define READFILE(pBuf,Size,Len,pFile) fread(pBuf,Size,Len,pFile)
#define WRITEFILE(pBuf,Size,Len,pFile) fwrite(pBuf,Size,Len,pFile)
#define SEEKFILE(pFile,Offset,Mode) fseek(pFile,Offset,Mode)
#define FILEPOS(pFile) ftell(pFile)

#else

//#ifndef STREAM_READ
#if 1

#define STREAM_READ AFS_READ	//HSA_READ_FOM
#define STREAM_WRITE AFS_WRITE	//HSA_WRITE_FOM
#define STREAM_RDWR AFS_RDWR	//HSA_RDWR_FOM
#define STREAM_SEEK_BEGIN AFS_SEEK_OFFSET_FROM_START
#define STREAM_SEEK_CURENT AFS_SEEK_OFFSET_FROM_CURRENT
#define STREAM_SEEK_END AFS_SEEK_OFFSET_FROM_END

#define CREATEFILE(FileName) AbstractFileSystem_Create((char *)(FileName))
#define OPENFILE(FileName,Mode) AbstractFileSystem_Open((char *)(FileName),Mode)
#define CLOSEFILE(pFile) AbstractFileSystem_Close(pFile)
#define READFILE(pBuf,Size,Len,pFile) AbstractFileSystem_Read(pFile,(char *)(pBuf),(Size)*(Len))
#define WRITEFILE(pBuf,Size,Len,pFile) AbstractFileSystem_Write(pFile,(char *)(pBuf),(Size)*(Len))
#define SEEKFILE(pFile,Offset,Mode) AbstractFileSystem_Seek(pFile,Offset,Mode)
#define FILEPOS(pFile) AbstractFileSystem_FileSize(pFile)
//#define CREATEFILE(FileName)  AbstractFileSystem_Create(FileName)

#endif

#endif

//以下为外层调用接口：

/***********************************************************************
 *
 * 函数名：		Zlib_Uncompress_Data
 *
 * 函数功能：	使用deflate算法解压一块数据块
 * 
 * @Param: 		'srcbuf'：指向一块要解压的数据块
 * @Param: 		'srcLen'：表示'srcbuf'的长度
 * @Param: 		'destbuf'：指向一块存放解压后数据的内存块
 * @Param: 		'destLen'：返回'destbuf'的长度
 * @Param: 		'bhead'：表示要解压的这块数据块是不是本文件的第一块数据
 *
 * 返回值：'0'：表示成功，小于0表示有错误
 ***********************************************************************/
int Zlib_Uncompress_Data1(const uchar *srcbuf, int srcLen, uchar **destbuf, int *destLen, int bhead);


/*
src:待解码数据
srcLen:待解码数据长度
dest:接收解码后数据的buf
destLen:解码后数据长度
bhead:是否含有压缩头的块  一般填1

返回:>=0:成功  
*/
int Gzip_Uncompress(const unsigned char *src, int srcLen, unsigned char *dest, int *destLen, int bhead);

/*
dest:压缩后文件存放buf
destLen(in):dest 长度    (out):压缩后数据的大小
source:待压缩文件
sourceLen:待压缩文件大小
pName:原始文件名,如果为NULL,表示不需要保存源文件名

返回:>=0:成功  
注:该函数目前没有提供保留原始文件最后更新时间的信息
*/
int Gzip_Compress(unsigned char *dest, unsigned int *destLen, unsigned char *source, int sourceLen, char *pName);


/***********************************************************************
 *
 * 函数名：		Gz_Uncompress_Data
 *
 * 函数功能：	使用gzip算法解压一块数据
 * 
 * @Param: 		'srcbuf'：指向一块要解压的数据块
 * @Param: 		'srcLen'：表示'srcbuf'的长度
 * @Param: 		'destbuf'：指向一块存放解压后数据的内存块
 * @Param: 		'destLen'：返回'destbuf'的长度
 * @Param: 		'bhead'：表示要解压的这块数据块是不是本文件的第一块数据
 *
 * 返回值：		'0'：表示成功，小于0表示有错误
 **********************************************************************/
int Gz_Uncompress_Data(const uchar *srcbuf, int srcLen, uchar **destbuf, int *destLen, int bhead);



/***********************************************************************
 * 
 * 函数名：		Gz_Free
 * 
 * 函数功能：	对应Zlib_Uncompress_Data1与Gz_Uncompress_Data，释放解压缩
 *              所占用的资源
 *
 * @Param:		'memblock'：指向要释放的内存块的地址
 *
 * 返回值：	
 **********************************************************************/
void Gz_Free(void *memblock);


/***********************************************************************
 * 
 * 函数名：		Zlib_Uncompress_Data
 * 
 * 函数功能：	解压一块swf数据块
 *
 * @Param:		'pData'：指向一块要解压的flash数据块
 * @Param:		'pTempBuf'：用于解压过程临时存放数据的内存块
 * @Param:		'TempLen'：临时数据块的长度
 * @Param:		'pBuf'：指向一块用于存放解压缩的数据内存块
 * @Param:		'BufLen'：解压缩数据块的长度
 * @Param:		'pureFlag'：表示swf选项是否打开
 *
 * 返回值：	    0: SUCCESS; -1: decode error; -2: open_file error.
 **********************************************************************/
int Zlib_Uncompress_Data(U8 *pData,U8 *pTempBuf, U32 TempLen,U8 *pBuf,U32 BufLen, U8 pureFlag);


/***********************************************************************
 * 
 * 函数名：		Zlib_Uncompress_File
 * 
 * 函数功能：	解压swf文件
 *
 * @Param:		'pSrcFileName'：源文件名
 * @Param:		'pDestFileName'：目标文件名
 * @Param:		'pBuf'：解压过程用使用的临时内存块
 * @Param:		'BufLen'：临时内存块长度
 *
 * 返回值：	
 **********************************************************************/
S32 Zlib_Uncompress_File(U8 *pSrcFileName,U8 *pDestFileName,U8 *pBuf,U32 BufLen);

#ifdef __cplusplus
}
#endif

#endif //__ZLIB_FLASH_API_H__


