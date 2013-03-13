/*
 * ==================================================
 *   Copyright 2009, CooTek 
 *   All rights reserved. http://www.cootek.com
 * ==================================================
 */

#ifndef __predefine_h
#define __predefine_h

//typedef _int64 INT64;
//typedef unsigned _int64 UINT64;

typedef long int INT64;
typedef unsigned long int UINT64;

#define BUILD_FOR_FEDORA 1

#ifdef __cplusplus
extern "C"
{
#endif

//#include <stddef.h>

#ifndef BOOL
#define BOOL char
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef void* DICTIONARY_HANDLE;
#define ERROR_DICTIONARY_HANDLE 0
#define CAST_DICTIONARY_HANDLE(handle, type)	((type*)handle)


//TODO: enable unique
#define ENABLE_WESTERN_DICTIONARY_UNIQUE 0

#define MAX_WORD_ARRAY_SIZE 36
#define MAX_INPUT_CODE_EXPAND_LENGTH 30
#define MAX_CANDIDATE_ITEM_SIZE 12
#define MAX_FILTER_LIST_SIZE 50
#define PINYIN_INDEX_PINYIN_LETTER_SIZE 7
#define ZHUYIN_INDEX_PINYIN_LETTER_SIZE 7
#define MAX_HISTORY_SIZE 5

typedef unsigned short WCHAR;
#define INVALID_FILE -1

typedef unsigned long	DICTIONARY_OPTIONS;

#define MIXED_LANGUAGE_INPUT_OPTION			0x01
#define CHINESE_SENTENCE_PREDICT_OPTION		0x02
#define CHINESE_GB18030_OPTION				0x04
#define WESTERN_SUPER_FETCH					0x08
//if enable trunk, search "lookforward" cannot get "look"
//chinese dictionary may not use trunk
#define WESTERN_FORWARD_TRUNK               0x10
#define ENABLE_SPELL_CHECK                  0x20
#define JAPANESE_PRECISE_MATCH_OPTION       0x40
#define JAPANESE_PREDICT_MATCH_OPTION       0x80
#define CHINESE_MOHUYIN_OPTION				0x7FF00
#define CHINESE_Z_ZH_MOHUYIN_OPTION         0x100
#define CHINESE_C_CH_MOHUYIN_OPTION         0x200
#define CHINESE_S_SH_MOHUYIN_OPTION         0x400
#define CHINESE_F_H_MOHUYIN_OPTION          0x800
#define CHINESE_L_N_MOHUYIN_OPTION          0x1000
#define CHINESE_AN_ANG_MOHUYIN_OPTION       0x2000
#define CHINESE_EN_ENG_MOHUYIN_OPTION       0x4000
#define CHINESE_IN_ING_MOHUYIN_OPTION       0x8000
#define CHINESE_IAN_IANG_MOHUYIN_OPTION     0x10000
#define CHINESE_UAN_UANG_MOHUYIN_OPTION     0x20000
#define CHINESE_L_R_MOHUYIN_OPTION          0x40000

#define IdlePriority			0
#define LowPriority				1
#define UndefinedPriority		2
#define BelowPriority			3
#define NormalPriority			4
#define AbovePriority			5
#define HighPriority			6
#define TopPriority				7

#define LinkDirectionLeft 0x0
#define LinkDirectionRight 0x1
#define LinkDirectionTop 0x2
#define LinkDirectionOther 0x3


#define ENG_DIC_ID 1
#define CHS_PINYIN_DIC_ID 2
#define FRENCH_DIC_ID 3
#define SPANISH_DIC_ID 4
#define BIHUA_DIC_ID 5
#define CANGJIE_DIC_ID 5


#include "hal.h"

struct CandidateItem
{
    WCHAR word[MAX_WORD_ARRAY_SIZE];
    WCHAR index[MAX_WORD_ARRAY_SIZE];
    short priority;
    short evidence_count;
    DICTIONARY_HANDLE source_dic_handle;
    int languageTag;
};

typedef struct SearchResult
{
    struct CandidateItem *candidate_items;
    size_t candidate_items_size;
    WCHAR display_evidence[MAX_WORD_ARRAY_SIZE * 3];// DEFINE \0 is to split pinyins \0\0 is the signal of end, words on top
    size_t display_evidence_size;
    WCHAR filter_list[MAX_FILTER_LIST_SIZE][PINYIN_INDEX_PINYIN_LETTER_SIZE];//all kinds of Pinyin that is legal
    size_t filter_list_size;
    int milestone;
    
}tRESULT;

typedef struct InputContext
{
    int input_codes[MAX_WORD_ARRAY_SIZE];
    int input_codes_size;
    int (*GetInputCodeExpandResult)(int code, WCHAR result[], void* callbackToken);
	void* callbackToken;
    WCHAR filter[PINYIN_INDEX_PINYIN_LETTER_SIZE];
    WCHAR qwerty_precise_string[MAX_WORD_ARRAY_SIZE];
    WCHAR history[MAX_HISTORY_SIZE][MAX_WORD_ARRAY_SIZE];
    short history_size;
    short request_size;
    int milestone;
	DICTIONARY_OPTIONS options;
    int bihua_input_codes[MAX_WORD_ARRAY_SIZE];
    int bihua_input_codes_size;

}tINPUT;

#define MAX_BIHUA_WORD_COUNT 1000
#define BihuaSeparator 0x0001
#define BihuaHeng 0x0003
#define BihuaShu 0x0005
#define BihuaPie 0x0006
#define BihuaNa 0x0009
#define BihuaZhe 0x000A
#define BihuaWildcard 0x0000
#define PinyinSeparator 0x27
#define PinyinSeparatorCode 0xE001
#define WubiWildcard 'z'

struct BihuaParameters
{
    int wordLength;
    int itemCount;
    int searchingLength;
    int searchingPriority;
    int returnedCount;
};

#ifdef __cplusplus
}
#endif

#endif
 
