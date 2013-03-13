/*
 * ==================================================
 *   Copyright 2009, CooTek 
 *   All rights reserved. http://www.cootek.com
 * ==================================================
 */

#ifndef HAL_H_
#define HAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "string.h"

//#ifndef size_t
//#define size_t unsigned int
//#endif
    
#define IMAGE_HANDLE unsigned int

IMAGE_HANDLE ext_fopen(const char* filename, const char* mode);
size_t ext_fread(void* buffer, size_t size, size_t count, IMAGE_HANDLE fHandle);
int ext_fclose(IMAGE_HANDLE fHandle);
size_t ext_fwrite(void* buffer, size_t size, size_t count, IMAGE_HANDLE fHandle);       // keep empty
int ext_fseek(IMAGE_HANDLE stream, long offset, int origin);                            // keep empty
int ext_fflush(IMAGE_HANDLE stream);                                                    // keep empty
char* ext_fgets(char* string, int n, IMAGE_HANDLE stream);                              // keep empty
void* ext_malloc(size_t size);
void ext_free(void* memblock);
void* ext_memset(void* dest, int c, size_t count);
void* ext_memcpy(void* dest, const void* src, size_t count);
int ext_ftell(IMAGE_HANDLE fd);

#define CT_FOPEN(file_name, str_mode, int_mode) ext_fopen(file_name, str_mode)
#define CT_FREAD        ext_fread
#define CT_FCLOSE       ext_fclose
#define CT_FWRITE       ext_fwrite
#define CT_FSEEK        ext_fseek
#define CT_FFLUSH       ext_fflush
#define CT_FGETS        ext_fgets
#define CT_MALLOC       ext_malloc
#define CT_FREE         ext_free
#define CT_MEMSET       ext_memset
#define CT_MEMCPY       ext_memcpy
#define CT_FTELL        ext_ftell

#ifdef __cplusplus
}
#endif

#endif
