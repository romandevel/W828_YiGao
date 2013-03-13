#ifndef FUNCTION_H
#define FUNCTION_H
#define FALSE     0
#define TRUE      1
#define BOOL			int

#ifndef NULL
#define NULL    0
#endif

#ifndef stderr
#define stderr    0
#endif

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define abs(i) ((i) < 0 ? -(i) : (i))
#define assert(a)
#define INT_MAX       2147483647    /* maximum (signed) int value */
#define UNKNOWN								0xE000
#define PICTURE								0xE001
#define SPACE								0x0020

#ifdef __cplusplus
extern "C" {
#endif

//stdlib.h stdio.h string.h
typedef unsigned int size_t;
void* epass_memcpy(void * _Dst, const void * _Src, size_t _Size);
void* epass_memset(void * _Dst, int _Val, size_t _Size);
char* epass_strcpy(char * _Dst, const char * _Src);
char* epass_strcat(char * _Dst, const char * _Src);
char* epass_strncat(char * _Dst, const char * _Src, size_t _Size);
char* epass_strstr(char * _Str, const char * _SubStr);
char* epass_strchr(char * _Str, int _Ch);
int epass_strcmp(const char * _Str1, const char * _Str2);
int epass_islower(int c);
int epass_isupper(int c);
int epass_tolower(int c);
int epass_toupper(int c);
int epass_atoi(const char *_Str);
size_t epass_strlen(const char * _Str);
void * epass_malloc(size_t size);
void epass_free(void* ptr);
int epass_fprintf(void * _File, const char * _Format, ...);
int epass_sprintf(char * _Dst, const char * _Format, ...);
void epass_qsort(void * _Base, size_t _NumOfElements, size_t _SizeOfElements, int (* _PtFuncCompare)(const void *, const void *));

//math.h
double epass_atan(double _X);
double epass_sin(double _X);
double epass_cos(double _X);


//custom
enum format {
	ISO8859_1, TeX, HTML, XML, SGML, UTF8, ASCII
};
typedef enum format FORMAT;
typedef unsigned short wchar;

#ifdef __cplusplus
}
#endif
#endif
