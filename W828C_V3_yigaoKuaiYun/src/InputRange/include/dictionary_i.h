/*
 * ==================================================
 *   Copyright 2009, CooTek
 *   All rights reserved. http://www.cootek.com
 * ==================================================
 */

#ifndef _DICTIONARY_I_H_
#define _DICTIONARY_I_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "predefined.h"
#include "hal.h"

    typedef enum _DictionaryEngineType
    {
        Standard,
        ChinesePinYin,
        ChineseBiHua,
        ChineseZhuyin,
        Japanese,
    } DictionaryEngineType;

    // DictionaryHandleInternal is an internal-used structure
    // the only reason to read this structure is for CandidateItem.source_dic_handle:
    //      if ( DictionaryHandleInternal.internalDictionaryHandle == CandidateItem.source_dic_handle)
    //      { means this word comes from this dictionary instance. }
    struct DictionaryHandleInternal
    {
        DictionaryEngineType engineType;
        void* internalDictionaryHandle;
    };

    struct ImageFileDescriptor
    {
        IMAGE_HANDLE fHandle;
    };

    struct ImageArrayDescriptor
    {
        char *image;
        int imageSize;
    };

	/*
	使用文件句柄初始化字典，返回值为字典句柄，languageTag可以在多语言混输时告诉调用者候选词是从哪个字典里检索出来的
	*/
    DICTIONARY_HANDLE InitializeDictionary(DictionaryEngineType engineType, struct ImageFileDescriptor fImages[], int languageTag);


	/*
	使用内存指针初始化字典。使用的文件同上。这里需要注意的是需要为一些文件分配更多的内存：
	1. usr文件，至少需要容纳用户添加的用户词
	2. 如果pi，pl文件不为0，则至少需要为pi，pl多分配16k(16*1024B)的内存
	*/
    DICTIONARY_HANDLE InitializeDictionaryByArray(DictionaryEngineType engineType, struct ImageArrayDescriptor arrayImages[], int languageTag);


	/*
	释放使用文件句柄初始化的字典，必须配套使用。
	*/
    void  DeinitializeDictionary(DICTIONARY_HANDLE handle);


	/*
	
	*/
    void  DeinitializeDictionaryByArray(DICTIONARY_HANDLE handle);


	/*
	根据input的参数搜索字典，结果返回在result里，result由引擎外分配，释放内存
	*/
    void  SearchDictionary(DICTIONARY_HANDLE handle, struct InputContext* input, struct SearchResult* result);

    void  RetrieveNextWordCandidates(DICTIONARY_HANDLE handle, struct InputContext *input, struct SearchResult* result);

    void  AdjustWordPriority(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], char manualAdjustFlag, int targetPriority);

    int RetrieveWordPriority(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  IsWordExists(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    void  AddWordToDictionary(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], int targetPriority);

    void  RemoveWordFromDictionary(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  IsUserDefinedWord(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  HasLinkedWords(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    void  GetLinkedWords(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], struct SearchResult* result);

    void  GetLinkedWordsByIndex(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], int directionIndex, struct SearchResult* result);

    // mixed language input interfaces
    void AttachSecondaryDictionary(DICTIONARY_HANDLE handle, DICTIONARY_HANDLE secondaryDictionaryHandle);

    void DetachSecondaryDictionary(DICTIONARY_HANDLE handle, DICTIONARY_HANDLE secondaryDictionaryHandle);

    void SearchDictionaryForward(DICTIONARY_HANDLE handle, struct InputContext* input, struct SearchResult* result);

    // translate words to code
    void QueryDictionaryCode(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR mapping[], struct SearchResult* result);

    BOOL IsValidDictionaryCode(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR code[], WCHAR mapping[]);

    void QueryAbbreviations(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR mapping[], struct SearchResult* result);

    BOOL IsAbbreviationMatchingCode(WCHAR word[], WCHAR code[], WCHAR mapping[]);

    void AfterWordSelected(DICTIONARY_HANDLE handle, struct InputContext *input);

    int GetDictionaryVersion(DICTIONARY_HANDLE handle);

    void SetDictionaryVersion(DICTIONARY_HANDLE handle, int version);
#ifdef __cplusplus
}
#endif

#endif
