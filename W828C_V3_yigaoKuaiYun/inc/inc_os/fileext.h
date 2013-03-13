//
// 下位机文件操作接口函数
//
// 该套函数的目的是实现下位机和PC的文件操作函数接口的统一（对下位机文件操作函数的封装）
// 包含该文件时请不要同时包含PC下的stdio.h。
//
//
// 注意：
// 目前不支持变参函数（如 fprintf, fscanf）;
// 目前不支持宽字节操作函数（如 fwputc/fwputs）
// 目前打开文件时只支持 "r, w, a [b][+]" 组合起来的12种方式，其他方式打开结果未知
// 目前该层封装参考PC，支持缓存（大小为_FILE_BUFF_LEN（4K）），当然也可以通过关闭宏FILE_SUPPORT_BUFFER来关闭该功能
// [缓存：也就是读写文件时开辟出一个大小为_FILE_BUFF_LEN的内存空间，一次性读出/写入，不去频繁进行实际文件的读写]
//

#ifndef FILE_H
#define FILE_H

#ifndef _WIN32	// PC MACRO

#include "AbstractFileSystem.h"


#ifndef EOF
#define EOF     (-1)
#endif
#ifndef NULL	/* Null */
#define NULL  (0)
#endif 
#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

//#define FILE_SUPPORT_BUFFER			/* 是否支持缓存的宏开关，打开宏则支持缓存 */

#define _FILE_BUFF_LEN	4096		/* 写文件时候的缓存大小 */

/* define the open Method */
enum _openmethod
{
	OPEN_R = 1,				/* "r" */
	OPEN_W = 2,				/* "w" */
	OPEN_A = 4,				/* "a" */
	OPEN_OPT_B = 8,			/* "rb","wb","ab" */
	OPEN_OPT_ADD = 16		/* "r+","w+","a+","rb+"... */
};


#ifndef _FILE_DEFINED
struct _iobuf {
		int	 _file;			/* the id of this opened file in system */
		int  _flag;			/* "r", "w", "a", "rb", "wb", "ab", "r+", "w+", "ab+", "rb+", "wb+", "ab+" */
		long _curPos;		/* 当前读/写到的位置（文件中的真实位置） */
		long _filelen;		/* 文件总长度 */

		/* 这里不再支持缓存 --- 低层函数已经支持 */
#ifdef FILE_SUPPORT_BUFFER
		char *_ptr;			/* 缓存中当前操作到的位置 */
		char *_base;		/* 缓存buffer的起始地址 */
		int  _cnt;			/* 当前缓存内还有多少空间可用 --- 读的时候该值表示有效数据还有多少/写的时候表示还有多少空间可写 */
		int  _bufsiz;		/* 缓存buffer的大小 */
		int  _error;		/* 发生错误时的记录 */
		int  _dirty;		/* seek后是否超出该缓存的区域。1. 下次读/写时需要重添buff，0：不需要 */
#endif
        };
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif


/* Seek method constants */
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0


// 支持：
int feof(FILE *);
long ftell(FILE *);

FILE * fopen(const char *, const char *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
int fclose(FILE *);

int fgetc(FILE *);
char * fgets(char *, int, FILE *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);

// 不支持：
//int ferror(FILE *);
//int fprintf( FILE *stream, const char *format [, argument ]...);
//int fscanf( FILE *stream, const char *format [, argument ]... );
//int fflush(FILE *);


#endif //_WIN32

#endif //FILE_H


