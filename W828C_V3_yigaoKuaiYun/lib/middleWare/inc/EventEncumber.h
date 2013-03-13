#ifndef _EVENTENCUMBER_H_
#define _EVENTENCUMBER_H_

/*
使用事件等待函数时注意：
1	必需先创建
2	Event_Wait和Event_Wakeup是成对使用的
3	必需先Event_Wait有效，Event_Wakeup才有效，否则Event_Wait无效
4 	这里的事件是二值的
5	使用Event_Wait前必需先关中断，后恢复中断
*/

typedef int hyc_event_t;

/*
函数：Event_Create
功能：创建等待事件
参数：创建Event, count 为0,暂时定义
返回：0 ==ok,-1==出错 
*/
int Event_Create(hyc_event_t **Event,int count);

/*
函数：Event_Wait
功能：进程无事件时进程阻塞状态
参数：Event
返回：无
*/
void Event_Wait(hyc_event_t *Event);

/*
函数：Event_WaitTimeout
功能：进程带时间益出的阻塞,如果Timeout为0，则功能等同于Event_Wait
参数：Event
返回：无
*/
void Event_WaitTimeout(hyc_event_t *Event,int Timeout);
/*
函数：Event_Wakeup
功能：进程唤醒进入阻塞的进程
参数：Event
返回：无
*/
void Event_Wakeup(hyc_event_t *Event);

/*
函数：Event_Destroy
功能：销毁Event
参数：Event
返回：无
*/
void Event_Destroy(hyc_event_t **Event);

#endif


