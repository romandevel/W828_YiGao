


/*
SeqQueue.c<循环队列>
*/

#include "hyTypes.h"
#include "SeqQueue.h"



/*-----------------------------------------------------------------------------
* 函数:	InitQueue()
* 功能:	初始化循环队列
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void InitQueue(tSeqQueue *Que, char *buffer, U32 size)
{
	//将*Q初始化为一个空的循环队列
	Que->front=Que->rear=0;
    Que->element = buffer;
    Que->size = size;

    memset(buffer,0,size);
}

#if 1

/*-----------------------------------------------------------------------------
* 函数:	EnterQueue()
* 功能:	入队
* 参数:	
* 返回:	rv	--	0：没有成功	1：成功
*----------------------------------------------------------------------------*/
U8 EnterQueue(tSeqQueue *Que, U8 *pData, U32 len)
{
	U8   rv=0;
	U32  rd,wr;
	U32  size,left;
	U8	 *pBuf;
	U8 	 IntValue;

	IntValue = InterruptDisable();
	rd = Que->front;
	wr = Que->rear;
	size = Que->size;
	pBuf = Que->element;
	InterruptRestore(IntValue);
	
	if(wr < rd)
	{
		left = rd-wr-1;
		if(left >= len)
		{
			memcpy(&pBuf[wr], pData, len);
			wr += len;
			rv = 1;
		}
	}
	else
	{
		left = size-wr+rd-1;
		if(left >= len)
		{
			left = size - wr;
			if(left >= len)
			{
				memcpy(&pBuf[wr], pData, len);
			}
			else
			{
				memcpy(&pBuf[wr], pData, left);
				memcpy(&pBuf[0], &pData[left], len-left);
			}
			wr += len;
			if(wr >= size) wr -= size;
			rv = 1;
		}
	}
	
	if(rv == 1)
	{
		IntValue = InterruptDisable();
		Que->rear = wr;
		InterruptRestore(IntValue);
	}
		
	return rv;
}

#else
/*-----------------------------------------------------------------------------
* 函数:	EnterQueue()
* 功能:	入队
* 参数:	
* 返回:	rv	--	0：没有成功	1：成功
*----------------------------------------------------------------------------*/
U8 EnterQueue(tSeqQueue *Que, char x)
{
	U8   rv;
	U32  rear=Que->rear;
	U32  size=Que->size;


	//将元素x入队
	//if((Que->rear+1)%(Que->size)==Que->front)  			//队列已经满了
	if((rear+1==Que->front && rear+1<size)
		||(rear+1>=size && 0==Que->front))
	{
	     rv=0;
	     //hyUsbPrintf("\r\nkong--\r\n");
	}
	else
	{
		Que->element[rear]=x;
		//Que->rear=(Que->rear+1)%(Que->size);  		//重新设置队尾指针
		if(rear+1>=size)
		{
			rear=0;
		}
		else
		{
			rear=rear+1;
		}
		Que->rear=rear;
		rv=1;
	}

	return rv;
}
#endif

/*-----------------------------------------------------------------------------
* 函数:	DeleteQueue()
* 功能:	出队
* 参数:	
* 返回:	rv	--	0：没有成功	1：成功
*----------------------------------------------------------------------------*/
U8 DeleteQueue(tSeqQueue *Que, char *x)
{
	U8	 rv;
	U32  front=Que->front;


	//删除队列的队头元素，用x返回其值*/
    if(front==Que->rear)  	//队列为空
	{
		rv=0;
	}
	else
	{
        *x=Que->element[front];
        //Que->front=(Que->front+1)%(Que->size);  //重新设置队头指针
		if(front+1>=Que->size)
		{
			front=0;
		}
		else
		{
			front=front+1;
		}
		Que->front=front;
		rv=1;
	}
	
	return rv;
}


/*-----------------------------------------------------------------------------
* 函数:	GetQueueFreeSpace()
* 功能:	得到队列的空元素个数
* 参数:	
* 返回:	len		--	空元素个数
*----------------------------------------------------------------------------*/
U32 GetQueueFreeSpace(tSeqQueue *Que)
{
	U32 len;

	if(Que->front<=Que->rear)
	{
		len=Que->size-(Que->rear-Que->front)-1;
	}
	else if(Que->front>Que->rear)
	{
		len=Que->front-Que->rear-1;
	}

	return len;
}


