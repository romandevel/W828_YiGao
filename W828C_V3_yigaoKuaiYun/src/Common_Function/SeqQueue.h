#ifndef _HYHW_AT_BUFFER_H_
#define _HYHW_AT_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "hyTypes.h"



typedef struct _tSeqQueue
{
   U32  front;
   U32  rear;
   U32	size;
   char	*element;  //队列的元素空间
}tSeqQueue;




#ifdef __cplusplus
}
#endif

#endif 