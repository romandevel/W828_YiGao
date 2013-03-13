#ifndef _REC_QUEUE_H_
#define _REC_QUEUE_H_

/*===== include files =======*/
#include "hyTypes.h"
#include "Rec_Info.h"

/*---------------------------------------------------*/
/*存放记录信息的队列,上传时使用*/

//队列每个结点的信息
typedef struct _queNode_t
{
	U32		fileId;		//文件id
	U32		recIdx;		//记录索引 从0开始
	U16		delFlag;	//删除标志 2:表示已删除
	U16		opType;		//业务操作类型
	
	//数据部分
	tDATA_INFO	tData;
	
}tQUE_NODE;

//队列结构
typedef struct _data_queue_t
{
	U32		rd;	//读位置
	U32		wr;	//写位置
	U32		delCnt;	//队列中删除的格式
	U32		size;	//整个队列的大小
	
	tQUE_NODE	*pQueBuf;
		
}tDATA_QUEUE;


#define MAX_SUBCODE_NUM		8000	//这里只是暂定的最大值,实际可加载子单数与初始化时传进来的buf大小有关

//加载一票多件时使用的队列(256 Kbytes)    暂时不考虑8000票还加载不完的情况(默认子单数不会超过8000票)
typedef struct _multi_code_t
{
	tDATA_INFO	tData;
//	U8	ifOver;		//是否加载完成 0:本次还未加载完  1:已加载完
//	U8	fileIdx;	//上次加载的文件索引
//	U16	recIdx;		//上次加载的记录索引
	U8	ifOver;		//是否加载完成 0:本次还未加载完  1:已加载完
	U8	sendType;	//发送方式  0:后台发送   1:前台发送
	U16	total;		//总的子单数  (如果未加载完则表示已经加载的条数)
	U16 maxNum;		//最大可加载子单数
	
	U8	mainCode[BARCODE_LEN];	//主单号
	U8	subCode[MAX_SUBCODE_NUM][BARCODE_LEN];	//子单号
	
}tMULTI_CODE;


extern tDATA_QUEUE	gtData_Que;	//数据队列

extern tMULTI_CODE	*gptMultiCode;
#endif //_REC_QUEUE_H_