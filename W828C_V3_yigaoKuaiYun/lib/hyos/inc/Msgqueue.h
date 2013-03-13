#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "hyErrors.h"
#include "semaphore.h"

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/


typedef struct msg_qentry_t tMSG_QENTRY;

/* 消息队列头的结构体 */
typedef struct msg_queue_t
{
	SEMAPHORE	*getsem;			/* 记数信号量，记录消息队列中的get的消息数 */
	SEMAPHORE	*putsem;			/* 记数信号量，记录消息队列中的put的消息数 */
    tMSG_QENTRY	*q;					/* queue entries in use */
    tMSG_QENTRY	*freelist;			/* queue entries not in use */
    void		*queuespace;		/* placeholder for the dynamically allocated area */
    unsigned int queuespacesize;
    unsigned int itemmaxsize;       /* 消息队列中每条消息内容的最大长度 */

} tMSG_QUEUE;

/* 一个消息的结构体 */
typedef struct msg_qentry_t
{
	struct msg_qentry_t	*next;
	unsigned int	priority;			/* 该消息的优先级 */
	unsigned int	buflen;				/* 消息内容的长度 */
	unsigned int	busy;				/* 该消息槽是否忙，用于分段来处理数据，保证临界区内处理数据量尽量少 */
										/* 事实上，该busy标记是用来保证put/get的协调的（而不是put/put协调或get/get协调） */
	char			*buf;				/* 消息内容的buff */
	
} tMSG_QENTRY;

#define MSG_QUEUE_OK                HY_OK
/* 消息函数操作失败的返回值定义 */
#define MSG_QUEUE_ERR_POINT_NULL	(-1)		/* 操作指针为空 */
#define MSG_QUEUE_ERR_TIMEOUT		(-2)		/* 超时返回 */
#define MSG_QUEUE_ERR_FULL			(-3)		/* 队列满 */
#define MSG_QUEUE_ERR_EMPTY			(-4)		/* 队列空 */
#define MSG_QUEUE_ERR_SEM			(-5)		/* 记数信号量操作错误 */
#define MSG_QUEUE_ERR_PARAM			(-6)		/* 参数错误 */
#define MSG_QUEUE_ERR_FAIL			(-7)		/* 操作失败 */

/*----------------------------------------------------------------------
* 函数: msg_queue_init
* 说明: 创建消息队列对象
* 参数: maxmsgs: 这个消息队列的消息个数
*		maxmsgsize：每个消息内容的最大占用空间(多少byte)
* 返回: 如果创建成功，返回指向该消息队列的指针；否则返回空指针
*-----------------------------------------------------------------------*/
tMSG_QUEUE *msg_queue_init( int maxmsgs, int maxmsgsize );


/*----------------------------------------------------------------------
* 函数: msg_queue_destroy
* 说明: 销毁消息队列
* 参数: pqueue： 指向消息队列的指针
* 返回: 0:SUCCESS or -1:FAIL
*		如果有进程仍在使用该消息队列(在等待列表中)，不允许销毁，返回失败。
* 注意：如果判断到返回值为SUCCESS时，说明销毁成功，请置pqueue为NULL，
*		防止下次或其他进程的再引用。
*-----------------------------------------------------------------------*/
int msg_queue_destroy( tMSG_QUEUE *pqueue );


/*----------------------------------------------------------------------
* 函数: msg_queue_put
* 说明: 消息队列中放入一个消息(等待方式)
* 参数: pqueue: 消息队列对象指针
*		buf: 要放的消息的内容指针，不能为NULL
*		len: 要放的消息的内容的长度
*		prio:该消息的优先级
* 返回: 0: success; < 0: fail
* 说明：如果消息池已经满，则等待直至可放入该消息为止
*-----------------------------------------------------------------------*/
int msg_queue_put( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio);


/*----------------------------------------------------------------------
* 函数: msg_queue_timed_put
* 说明: 消息队列中放入一个消息(时间限制方式)
* 参数: pqueue: 消息队列对象指针
*		buf: 要放的消息的内容指针，不能为NULL
*		len: 要放的消息的内容的长度
*		prio:该消息的优先级
*		ticks:限制等待的时间
* 返回: 0: success; < 0: fail （-2：超时返回）
* 说明：如果消息池已经满，则等待到可放入该消息或超时返回
*-----------------------------------------------------------------------*/
int msg_queue_timed_put( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio, unsigned int ticks);


/*----------------------------------------------------------------------
* 函数: msg_queue_tryput
* 说明: 消息队列中放入一个消息(不等待的方式)
* 参数: pqueue: 消息队列对象指针
*		buf: 要放的消息的内容指针，不能为NULL
*		len: 要放的消息的内容的长度
*		prio:该消息的优先级
* 返回: 0: success; < 0: fail
* 说明：如果消息池已经满，则不等待直接返回
*-----------------------------------------------------------------------*/
int msg_queue_tryput( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio);


/*----------------------------------------------------------------------
* 函数: msg_queue_get
* 说明: 从消息队列中取出一个消息(等待的方式)
* 参数: pqueue: 消息队列对象指针
*		buf:	消息内容要写入的地址（这个地址必须在调用该函数前分配好，
*				长度应该大于/等于init时的maxmsgsize。）
*		len:	返回得到的消息内容的长度
* 返回: 0: success; < 0: fail
* 说明：如果消息池已经空，则等待直至有消息放入为止
*-----------------------------------------------------------------------*/
int msg_queue_get( tMSG_QUEUE *pqueue, char *buf, int *len);


/*----------------------------------------------------------------------
* 函数: msg_queue_timed_get
* 说明: 从消息队列中取出一个消息(时间限制的方式)
* 参数: pqueue: 消息队列对象指针
*		buf:	消息内容要写入的地址（这个地址必须在调用该函数前分配好，
*				长度应该大于/等于init时的maxmsgsize。）
*		len:	返回得到的消息内容的长度
* 返回: 0: success; < 0: fail
* 说明：如果消息池已经空，则等待到有消息放入或者超时返回
*-----------------------------------------------------------------------*/
int msg_queue_timed_get( tMSG_QUEUE *pqueue, char *buf, int *len, unsigned int ticks);


/*----------------------------------------------------------------------
* 函数: msg_queue_tryget
* 说明: 从消息队列中取出一个消息(不等待的方式)
* 参数: pqueue: 消息队列对象指针
*		buf:	消息内容要写入的地址（这个地址必须在调用该函数前分配好，
*				长度应该大于/等于init时的maxmsgsize。）
*		len:	返回得到的消息内容的长度
* 返回: 0: success; < 0: fail
* 说明：如果消息池已经满，则不等待直接返回
*-----------------------------------------------------------------------*/
int msg_queue_tryget( tMSG_QUEUE *pqueue, char *buf, int *len);




#endif /* MESSAGE_QUEUE_H */


