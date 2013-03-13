/* time.h: ANSI 'C' (X3J11 Oct 88) library header */

#ifndef __time_h
#define __time_h

#include "string.h"

//#ifndef __size_t
//#define size_t unsigned int
//#endif

#ifndef NULL
#define NULL (void *)0
#endif

typedef unsigned int clock_t;    /* cpu time type */
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */

/* 保存日历时间的结构，称为broken-down time. 如果 tm_isdst为正，表示Daylight Savings Time 
 * 有效，为零表示Daylight Savings Time 无效, 如果该信息没有获取到， tm_isdst为负 */
struct tm {
  int tm_sec;   /* Seconds after minute (0 - 60) (1 leap second)
                   (0 - 60 allows for the occasional leap second) */
  int tm_min;   /* minutes after the hour (0 to 59) */
  int tm_hour;  /* Hours since midnight (0 - 23) */
  int tm_mday;  /* Day of month (1 - 31) */
  int tm_mon;   /* Month (0 - 11; January = 0) */
  int tm_year;  /* years since 1900 */
  int tm_wday;  /* Day of week (0 - 6; Sunday = 0) */
  int tm_yday;  /* Day of year (0 - 365; January 1 = 0) */
  int tm_isdst; /* DST   [-1/0/1] */
};


/*-----------------------------------------------------------------------
* 函数: clock
* 说明: 确定处理器使用的时间.
* 输入: none
* 返回: 估计程序启动以来，使用的处理器时间. 返回的时间是以毫秒为单位
* 注意: 我们只支持到10毫秒，故而这里的返回值均为10的倍数
*-----------------------------------------------------------------------*/
clock_t clock(void);


/*-----------------------------------------------------------------------
* 函数: difftime
* 说明: 计算两个时间的差异: time1 - time0
* 输入: time1: 时间，被减数
*		time0: 时间，减数
* 返回: 用double类型表示的时间差，以秒为单位
*-----------------------------------------------------------------------*/
double difftime(time_t time1, time_t time0);


/*-----------------------------------------------------------------------
* 函数: mktime
* 说明: 将 tm 格式的时间转换为 time_t 格式的代码
* 		结构中的tm_wday和tm_yday分量被忽略,
* 输入: 
*		ptTime: 指向 struct tm 的结构的指针
* 返回: time_t 类型的时间。即1970.1.1 00:00:00到 tm 时间所间隔的秒数
*-----------------------------------------------------------------------*/
time_t mktime(struct tm * ptTime);


/*-----------------------------------------------------------------------
* 函数: time
* 说明: 得到系统时间。即从1970年1月1日00:00:00到现在的秒数。
* 参数: 
*		timer: 用来存放得到的时间差。
* 返回: 得到系统时间。其实和参数性质一样，是该值的两种返回形式。
* 用法：1). long time_s = 0; time_s = time( NULL ); time_s为目标值；
*		2). long time_s = 0; time(&time_s); time_s为目标值；
*		3). long time_s_p = 0, time_s_r = 0; time_s_r = time( &time_s_p ); 
*				time_s_r和time_s_p均为目标值且相同
*-----------------------------------------------------------------------*/
time_t time(time_t *timer);


/*-----------------------------------------------------------------------
* 函数: asctime
* 说明: 将tm结构的时间转换为字符串类型的时间。
* 		该字符串格式为 "Sun Sep 16 01:03:52 1973\n\0"，为24小时制。
* 输入: 
*		ptTime: 指向 struct tm 的结构的指针 
* 返回: 指向转换后字串的指针
*		字串格式"Sun Sep 16 01:03:52 1973\n\0"
*-----------------------------------------------------------------------*/
char *asctime(const struct tm * tp);


/*-----------------------------------------------------------------------
* 函数: _strdate
* 说明: 得到系统日期并转换为字串，放入datestr中，并返回该字串首指针
* 输入: 
*		datestr: 目标buffer
* 返回: 目标buffer指针
* 注意: datestr buffer长度至少为9
* 注意：输出格式为 "12/05/99"
*-----------------------------------------------------------------------*/
char *_strdate( char *datestr );


/*-----------------------------------------------------------------------
* 函数: _strtime
* 说明: 得到系统时间并转换为字串，放入datestr中，并返回该字串首指针
* 输入: 
*		datestr: 目标buffer
* 返回: 目标buffer指针
* 注意: datestr buffer长度至少为9
* 注意：输出格式为 "13:40:40"
*-----------------------------------------------------------------------*/
char *_strtime( char *timestr );


/*-----------------------------------------------------------------------
* 函数: ctime
* 说明: 把time_t型的time转换为tm结构体的形式并按字串格式输出
* 输入: 
*		ptTime: 存放 time_t 型数据的指针
* 返回: 指向转换后字串的指针
*		字串格式"Sun Sep 16 01:03:52 1973\n\0"
* 注意：如果时间为早于1970.1.1 00:00:00,则返回NULL。
*-----------------------------------------------------------------------*/
char *ctime(const time_t * timer);



/*-----------------------------------------------------------------------
* 函数: gmtime
* 说明: 把 time_t 格式的时间转换为结构体 tm 类型的时间。
* 输入: 
*		timer: 存放要转换的time数据的变量的指针
* 返回: tm 类型的时间
* 注意：gmtime, mktime,和localtime 共用一个结构体"gm_use_tm"来装载他们的数据
*-----------------------------------------------------------------------*/
struct tm *gmtime(const time_t * timer);


/*-----------------------------------------------------------------------
* 函数: localtime
* 说明: 把 time_t 格式的时间转换为结构体 tm 类型的时间并根据设定的时区再校正。
* 输入: 
*		timer: 存放要转换的time数据的变量的指针
* 返回: tm 类型的时间
* 注意: localtime与gmtime的不同点在于：localtime还会做校对当地时区的动作。
*		但该功能我们暂不支持。
* 注意：gmtime, mktime,和localtime 共用一个结构体"gm_use_tm"来装载他们的数据
*-----------------------------------------------------------------------*/
struct tm *localtime(const time_t * timer);


/*-----------------------------------------------------------------------
* 函数: strftime
* 说明: 将时间按照规定的格式格式化为字串
* 输入: 
*		s: 目标buffer
*		maxsize: 目标buffer长度
*		format: 格式化的要求
*		timeptr: tm 指针
* 返回: 字串中填充的总长度，包括结尾NULL的一个字节。
*		如果该长度超过buffer总长度，则返回值为0，此时buffer不可用。
* 说明: 具体使用方法请参考WIN32-VC。 
*		目前支持的格式类型为VC上支持的全部类型: %a,%A,%b,%B,%c,%d,%H,%I,%j
*		%m,%M,%p,%S,%U,%w,%W,%x,%X,%y,%Y,%z,%Z,%%,
*		%#a, %#A, %#b, %#B, %#p, %#X, %#z, %#Z, %#%, %#c, %#x, %#d, %#H, 
*		%#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y
*		具体意义及使用请参考VC。
*		但目前z/Z不能输出有效字串，因为该字串为表示时区的中文字串。
*-----------------------------------------------------------------------*/
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);


/* 注意: 该文件内并不提供 _getsystime 和 _setsystime 函数。 */
/*		 如果需要，请直接使用 hyhwRtc.h 中的 hyhwRtc_GetTime hyhwRtc_SetTime 。 */


#endif	//__time_h

/* end of time.h */









