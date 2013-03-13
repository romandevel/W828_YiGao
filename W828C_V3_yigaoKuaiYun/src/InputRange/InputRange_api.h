#ifndef _IUPUTRANGE_API_H_
#define _IUPUTRANGE_API_H_

#include "predefined.h"
#include "hyTypes.h"


#define OLD_COOTEK_START	0x02130e07
#define OLD_COOTEK_END		(OLD_COOTEK_START+0x0019c1cb)
#define COOTEK_START		0x02130c07
#define COOTEK_END			(COOTEK_START+0x004aa83a)

#define COOTEK_START_USER	0
#define COOTEK_END_USER		30


#define GET_MAXNUM  10

typedef struct _return_word
{
	U8 HanZi[MAX_CANDIDATE_ITEM_SIZE*GET_MAXNUM][MAX_WORD_ARRAY_SIZE];
	U8 PinYing[MAX_FILTER_LIST_SIZE][PINYIN_INDEX_PINYIN_LETTER_SIZE];
	U8 ShowInput[MAX_WORD_ARRAY_SIZE];
	
	U32 HZ_Num;
	U32 PY_Num;
	U32 Show_Num;

}tReturnWord;



extern U32	gu_CootekStart;
extern U32  gu_CootekEnd;




/*************************************
* 函数: InputRange_ClearKeyArray
* 功能: 清楚按键记录
* 参数: none
* 返回: none
*************************************/
void InputRange_ClearKeyArray(void);


/*************************************
* 函数: InputRange_Init
* 功能: 初始化字典
* 参数: none
* 返回: none
*************************************/
int InputRange_Init(void);


/*************************************
* 函数: InputRange_CloseFd
* 功能: 关闭字典
* 参数: none
* 返回: none
*************************************/
void InputRange_CloseFd(void);


/*************************************
* 函数: InputRange_DeleteOneKey
* 功能: 删除一个键值
* 参数: returnValue：返回结果
* 返回: none
*************************************/
void InputRange_DeleteOneKey(tReturnWord *returnValue);


/*************************************
* 函数: InputRange_GetWord
* 功能: 输入键值，获得对应的汉字
* 参数: key：键值   returnValue：返回结果
* 返回: none
*************************************/
void InputRange_GetWord(int key, tReturnWord *returnValue);


/*************************************
* 函数: InputRange_GetNextWord
* 功能: 输入前一个字或词，返回联想字或词
* 参数: firstWord：前一个字   returnValue：返回结果
* 返回: 0:成功  -1：失败
*************************************/
int InputRange_GetNextWord(U8 *firstWord, tReturnWord *returnValue);


/*************************************
* 函数: InputRange_GetExactlyWord
* 功能: 根据拼音获取精确的汉字
* 参数: inputPY：选中的拼音   returnValue：返回结果
* 返回: none
*************************************/
void InputRange_GetExactlyWord(U8 *inputPY, tReturnWord *returnValue);


/*************************************
* 函数: InputRange_GetPriority
* 功能: 获得词语的优先级
* 参数: input_Word：目标词语
* 返回: 优先级值
*************************************/
int InputRange_GetPriority(U8 *input_Word);


/*************************************
* 函数: InputRange_ChangePriority
* 功能: 调整input_Word的优先级
* 参数: input_Word：要调整的词   
		manualAdjustFlag：0  自动调整优先级
                          1  将优先级调整为targetPriority
        targetPriority：要调整的优先级
* 返回: none
*************************************/
void InputRange_ChangePriority(U8 *input_Word, char manualAdjustFlag, int targetPriority);


/*************************************
* 函数: InputRange_IsWordIn
* 功能: 判断输入的字是否存在字典中
* 参数: input_Word：目标词语
* 返回: TRUE：存在   FALSE 不存在
*************************************/
BOOL InputRange_IsWordIn(U8 *input_Word);


/*************************************
* 函数: InputRange_IsWordInUser
* 功能: 判断输入的字是否存在用户字典中
* 参数: input_Word：目标词语
* 返回: TRUE：存在   FALSE 不存在
*************************************/
BOOL InputRange_IsWordInUser(U8 *input_Word);


/*************************************
* 函数: InputRange_AddToUser
* 功能: 将input_Word添加到字典中
* 参数: input_Word：要添加的词
* 返回: none
*************************************/
void InputRange_AddToUser(U8 *input_Word);


/*************************************
* 函数: InputRange_RemoveFromUser
* 功能: 将input_Word从用户字典中删除
* 参数: input_Word：要删除的词
* 返回: none
*************************************/
void InputRange_RemoveFromUser(U8 *input_Word);


/*************************************
* 函数: InputRange_NextOrBefore
* 功能: 向前向后查找
* 参数: direction:方向   1：向后   0：向前
		returnValue：返回结果
* 返回: none
*************************************/
void InputRange_NextOrBefore(int direction, tReturnWord *returnValue);

#endif