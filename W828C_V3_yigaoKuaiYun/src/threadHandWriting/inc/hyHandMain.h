#ifndef __HYHAND_MAIN_H__
#define __HYHAND_MAIN_H__

#include "ISHWR_Engine.h"


//记录轨迹的buf (存放触摸点)
#define HY_REC_BUF_SIZE  (1024)

//输出buf的长度 (接收备选字)
#define HY_OUT_BUF_SIZE   (11)//(100)//

//识别的标志状态
#define HY_REG_STASTUS_IDEL  (0)
#define HY_REG_STASTUS_START (1)

//当前轨迹的状态标志
#define HY_TRACE_IDEL    (0)
#define HY_TRACE_START   (1)
#define HY_TRACE_PARSE   (2)

//定义手写断笔时间
#define HY_TRACE_TIME_PAUSE (8)
#define HY_TRACE_TIME_STOP  (60)//80
//#define HY_TRACE_TIME_PAUSE (5)
//#define HY_TRACE_TIME_STOP  (40)//80

typedef struct _tHY_RECG
{
     void *EventHyRcg;						//进程wait标志
     
     unsigned int lastTick;					//上次触摸的tick数用于判断当前触摸点的状态
     
     unsigned char enable;					//识别是否开启标志
     
     unsigned char traceStatus;				//当前轨迹的状态

     unsigned int index;					//有效点的个数
     ISHWR_POINT_T  recBuf[HY_REC_BUF_SIZE];//存储点
     
     unsigned char outFlag;					//是否有输出的标志
     short  outBuf[HY_OUT_BUF_SIZE];		//接收识别备选字
     char   outBakBuf[HY_OUT_BUF_SIZE*2];	//用于备份字符用
      
}tHY_RECG;

/*---------------------------------------------------
 * 函数 : hyHWRecog_RecogTrace
 * 功能 : 记录手写点,及时没有点时也需要定时传入点
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_RecogTrace(int x, int y);

/*---------------------------------------------------
 * 函数 : hyHWRecog_GetLetter
 * 功能 : 得到识别字符
 * 参数 : pbuf:传入的buf
 * 返回 : 得到的字符数 
---------------------------------------------------*/
int hyHWRecog_GetLetter(char *pbuf);

/*---------------------------------------------------
 * 函数 : hyHWRecog_RecognizeMain
 * 功能 : 失能/使能手写识别
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_EnableFlag(int type);

/*---------------------------------------------------
 * 函数 : hyHWRecog_RecognizeMain
 * 功能 : 手写识别主循环函数
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_RecognizeMain(void);

#endif