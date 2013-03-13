/*
============================================================================
Name        : ISHWR_Engine.h
Copyright   : IntSig Information Co., Ltd, 2010
Description : Interface definition of IntSig's handwriting recognition engine
Version     : SDC Chinese Combined 1.0.85.100325
Platform    : Android
Date        : 2010.03.25
============================================================================
*/


#ifndef __ISHWR_ENGINE_H__
#define __ISHWR_ENGINE_H__

//#define PLATFORM_ANDROID

#ifdef __cplusplus
#define ISHWR_ENGINE_API extern "C"
#else
#define ISHWR_ENGINE_API
#endif

#ifdef PLATFORM_ANDROID
#define ISHWRAPI 
#else
#define ISHWRAPI __stdcall
#endif

#define ISHWR_ADAPT_SIZE    ( 128 * 1024 )


typedef void (*ISHWR_ENGINE_HANDLE);
typedef unsigned long ISHWR_CHARSET_T;

typedef struct _ISHWR_POINT_T
{
    short x;
    short y;

} ISHWR_POINT_T;

#define MAX_PREDICT_WORD_LENGTH     16
typedef unsigned short ISHWR_PREDICT_WORD[ MAX_PREDICT_WORD_LENGTH ];

#define ISHWR_CHARSET_LOWERCASE                 ( 0x00000001 )
#define ISHWR_CHARSET_UPPERCASE                 ( 0x00000002 )
#define ISHWR_CHARSET_SYMBOL                    ( 0x00000004 )
#define ISHWR_CHARSET_NUMERALS                  ( 0x00000008 )
#define ISHWR_CHARSET_ENGLISH                   ( 0x00000010 )
#define ISHWR_CHARSET_CHINESE                   ( 0x10000100 )
#define ISHWR_CHARSET_CHINESE_SIMPLIFIED        ( 0x10000101 )
#define ISHWR_CHARSET_CHINESE_TRADITIONAL       ( 0x10000102 )

//#define ISHWR_CHARSET_DEFAULT  ( ISHWR_CHARSET_LOWERCASE | ISHWR_CHARSET_UPPERCASE | ISHWR_CHARSET_SYMBOL |\
//                                 ISHWR_CHARSET_NUMERALS | ISHWR_CHARSET_ENGLISH | ISHWR_CHARSET_CHINESE )
#define ISHWR_CHARSET_DEFAULT  (ISHWR_CHARSET_CHINESE )

#define ISHWR_ERROR_OK                          ( 0x00000000 )
#define ISHWR_ERROR_INVALID_PARAMETERS          ( 0x00000001 )
#define ISHWR_ERROR_BAD_FORMAT                  ( 0x00000002 )
#define ISHWR_ERROR_MODEL_NOT_LOADED            ( 0x00000003 )
#define ISHWR_ERROR_RE_INITIALIZE               ( 0x00000004 )
#define ISHWR_ERROR_BUFFER_FULL                 ( 0x00000005 )
#define ISHWR_ERROR_NOT_INITIALIZED             ( 0x00000006 )
#define ISHWR_ERROR_BUFFER_NOT_ENOUGH           ( 0x00000007 )


typedef enum
{
    ISHWR_Param_CharSet = 0,    /* Choose language */
    ISHWR_Param_CHT2CHS,        /* Convert CHT (traditional) to CHS (simplified) */
    ISHWR_Param_EngineSpeed

} ISHWR_PARAM_T;


//---------------------------------------------------------------------------------
// ISHWR_Initialize:
//      Initialize the recognition engine
//      pEngineData is the data buffer loaded from engine data file
//      pAdaptData is the adapt buffer loaded from adapt data file
// Return:
//      Handle of recognition engine which can be used to call HWR_Recognize, etc.
// 
ISHWR_ENGINE_API
ISHWR_ENGINE_HANDLE
ISHWR_Initialize(
                 char* pEngineData,
                 long sizeEngineData,
                 char* pAdaptData,
                 long sizeAdaptData
                 );


//---------------------------------------------------------------------------------
// ISHWR_Recognize:
//      Output recognized candidates according to input ink
//      pPoints is the stroke points, each stroke should be end up with (-1,-1),
//      e.g. two strokes should be as follows:
//      ..., (-1,-1), ..., (-1,-1)
//      pCandidates is an array of UNICODE candidates
//      numCandidates is the length of pCandidates
// Return:
//      number of recognized candidates
// 
ISHWR_ENGINE_API
long
ISHWR_Recognize(
                ISHWR_ENGINE_HANDLE hEngine,
                ISHWR_POINT_T* pPoints,
                long numPoints,
                unsigned short* pCandidates,
                long numCandidates
                );


//---------------------------------------------------------------------------------
// ISHWR_SetParam:
//      Set parameters of the recognition engine
//      ISHWR_PARAM_T is defined as above enumeration
//      pData point to the input parameter buffer
//      size is the byte count of the buffer which pData point to
// Return:
//      byte count accepted, -1 indicates an error
// 
ISHWR_ENGINE_API
long
ISHWR_SetParam(
               ISHWR_ENGINE_HANDLE hEngine,
               ISHWR_PARAM_T param,
               void* pData,
               unsigned long size
               );


//---------------------------------------------------------------------------------
// ISHWR_Predict:
//      Predict the next word
//      pCurrent point to the current input character
//      pPrev point to the previous input character, can be NULL if not used
//      pWords is an array of candidate words, in which each word ends up with zero
//      nMaxWords indicates the pWords buffer length
// Return:
//      number of predicted words
// 
ISHWR_ENGINE_API
long
ISHWR_Predict(
              ISHWR_ENGINE_HANDLE hEngine,
              unsigned short* pCurrent,
              unsigned short* pPrev,
              ISHWR_PREDICT_WORD *pWords,
              unsigned long nMaxWords
              );

//---------------------------------------------------------------------------------
// ISHWR_SaveWordAssociation:
//      Save two character association in user adaptation dictionary
// Return:
//      0 if failed, otherwise succeeded
// 
ISHWR_ENGINE_API
long
ISHWR_SaveWordAssociation(
                          ISHWR_ENGINE_HANDLE hEngine,
                          unsigned short current,
                          unsigned short next
                          );

//---------------------------------------------------------------------------------
// ISHWR_AdaptUserWriting:
//      Adapt user writing and save it to user adaptation dictionary 
// Return:
//      0 if failed, otherwise succeeded
// 
ISHWR_ENGINE_API
long
ISHWR_AdaptUserWriting(
                       ISHWR_ENGINE_HANDLE hEngine,
                       ISHWR_POINT_T* pPoints,
                       long numPoints,
                       unsigned short current
                       );


//---------------------------------------------------------------------------------
// ISHWR_SetLastError:
//      Set last error code
// Return:
//      Previous last error code
// 
ISHWR_ENGINE_API
unsigned long
ISHWR_SetLastError( unsigned long errCode );


//---------------------------------------------------------------------------------
// ISHWR_GetLastError:
//      Get last error code
// Return:
//      Last error code
// 
ISHWR_ENGINE_API
unsigned long
ISHWR_GetLastError();


//---------------------------------------------------------------------------------
// ISHWR_GetVersionString:
//      Get version string
// Return:
//      Pointer to the version string
// 
ISHWR_ENGINE_API
char*
ISHWR_GetVersionString();


//---------------------------------------------------------------------------------
// ISHWR_RecognizeScore:
//      Output recognized candidates according to input ink
//      pPoints is the stroke points, each stroke should be end up with (-1,-1),
//      e.g. two strokes should be as follows:
//      ..., (-1,-1), ..., (-1,-1)
//      pCandidates is an array of UNICODE candidates
//      numCandidates is the length of pCandidates
// Return:
//      number of recognized candidates
// 
ISHWR_ENGINE_API
long
ISHWR_RecognizeScore(
                ISHWR_ENGINE_HANDLE hEngine,
                ISHWR_POINT_T* pPoints,
                long numPoints,
                unsigned short* pCandidates,
                long numCandidates,
                long* pScore
                );


#endif

