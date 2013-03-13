#include "hyhwRtc.h"
#include "string.h" 
#include "time.h"

/* format as "Sun Sep 16 01:03:52 1973\n\0" */
/* static const char format[] = "%.3s %.3s %2d %.2d:%.2d:%.2d %d\n"; */
static char tm_asc_rst[	         3+1+ 3+1+ 2+1+ 2+1+ 2+1+ 2+1+4+1 + 1 + 6];	/* 存放asctime结果。*/

static struct tm gm_use_tm;	/* 公用全局结构体，用来存储临时信息。gmtime/localtime/mktime/tm_getSystemTime公用。*/

static const char tm_week_name[7][10] =
{
	"Sunday",	/* Sunday */
	"monday",	/* monday */
	"tuesday",	/* tuesday */
	"Wednesday",/* Wednesday */
	"Thursday",	/* Thursday */
	"Friday",	/* Friday */
	"Saturday",	/* Saturday */
};
static const char tm_month_name[12][10] =
{
	"January",	/* January */
	"February",	/* February */
	"March",	/* March */
	"April",	/* April */
	"May",		/* May */
	"June",		/* June */
	"July",		/* July */
	"August",	/* August */
	"September",/* September */
	"October",	/* October */
	"November",	/* November */
	"December",	/* December */
};


// 声明
static int tm_writeSpcStr(char **dst, const char **fmt, int rstlen, int *rlen, const struct tm *timeptr);
extern struct tm *tm_tz_convert (const time_t *timer, int use_localtime, struct tm *tp);
extern time_t tm_zt_convert (struct tm *tp, int use_localtime);
static char *tm_itos_len(char *dst, int value, int len);

/*-----------------------------------------------------------------------
* 函数: clock
* 说明: 确定处理器使用的时间.
* 输入: none
* 返回: 估计程序启动以来，使用的处理器时间. 返回的时间是以毫秒为单位
* 注意: 我们只支持到10毫秒，故而这里的返回值均为10的倍数
*-----------------------------------------------------------------------*/
clock_t clock(void)
{
	return rawtime(NULL)*10;
}

/*-----------------------------------------------------------------------
* 函数: difftime
* 说明: 计算两个时间的差异: time1 - time0
* 输入: time1: 时间，被减数
*		time0: 时间，减数
* 返回: 用double类型表示的时间差，以秒为单位
*-----------------------------------------------------------------------*/
double difftime(time_t time1, time_t time0)
{
	/* 这里time_t已经是unsigned int型，不再考虑类型判断。 */
	
	return (double)time1 - (double)time0;
}

/*-----------------------------------------------------------------------
* 函数: mktime
* 说明: 将 tm 格式的时间转换为 time_t 格式的代码
* 		结构中的tm_wday和tm_yday分量被忽略,
* 输入: 
*		ptTime: 指向 struct tm 的结构的指针
* 返回: time_t 类型的时间。即1970.1.1 00:00:00到 tm 时间所间隔的秒数
*-----------------------------------------------------------------------*/
time_t mktime(struct tm * ptTime)
{
	return tm_zt_convert(ptTime, 0);
}

/*-----------------------------------------------------------------------
* 函数: tm_getSystemTime
* 说明: 得到系统原始时间
* 输入: 
*		ptTime: 指向 struct tm 的结构的指针
* 返回: struct tm* 指针
* 注意: 如果参数p_tm为NULL，则该函数和gmtime等共用结构体；
*		否则使用传进来的参数结构体
*-----------------------------------------------------------------------*/
static struct tm *tm_getSystemTime(struct tm *p_tm)
{
	RtcTime_st rtcTime;
	struct tm *timer = &gm_use_tm;
	
	if (p_tm)
	{
		timer = p_tm;
	}
	
	hyhwRtc_GetTime(&rtcTime);
	timer->tm_sec = rtcTime.second;
	timer->tm_min = rtcTime.minute;
	timer->tm_hour = rtcTime.hour;
	//tmTime.tm_sec = rtcTime.week;
	timer->tm_mday = rtcTime.day;
	timer->tm_mon = rtcTime.month - 1;	/* for [1-12] => [0-11] */
	timer->tm_year = rtcTime.year - 1900;
	
	return timer;
}

/*-----------------------------------------------------------------------
* 函数: time_time
* 说明: 得到系统时间。即从1970年1月1日00:00:00到现在的秒数。
* 参数: 
*		timer: 用来存放得到的时间差。
* 返回: 得到系统时间。其实和参数性质一样，是该值的两种返回形式。
* 用法：1). long time_s = 0; time_s = time( NULL ); time_s为目标值；
*		2). long time_s = 0; time(&time_s); time_s为目标值；
*		3). long time_s_p = 0, time_s_r = 0; time_s_r = time( &time_s_p ); 
*				time_s_r和time_s_p均为目标值且相同
*-----------------------------------------------------------------------*/
time_t time_time(time_t *timer)
{
#if 0
	if (!timer)
		*timer = rawtime(NULL) / 100;
	return rawtime(NULL) / 100;
#else

	time_t time_n;
	struct tm tmTime;
	
	time_n = mktime( tm_getSystemTime(&tmTime) );
	
	if (timer)
	{
		*timer = time_n;
	}
	return time_n;
	
#endif
}

/*-----------------------------------------------------------------------
* 函数: asctime
* 说明: 将tm结构的时间转换为字符串类型的时间。
* 		该字符串格式为 "Sun Sep 16 01:03:52 1973\n\0"，为24小时制。
* 输入: 
*		ptTime: 指向 struct tm 的结构的指针 
* 返回: 指向转换后字串的指针
*		字串格式"Sun Sep 16 01:03:52 1973\n\0"
*-----------------------------------------------------------------------*/
char *asctime(const struct tm * tp)
{
	char tmpStr[36];
	int pos = 0;
	
	
	if (!tp)
		return NULL;
	//if ( ptTime->isdst <= 0 )		//no judge it
	//	return NULL;
	
	/* 这里对tm结构体内的数据做合法性判断 */
	if (tp->tm_wday < 0 || tp->tm_wday >= 7)	//week
	{
		strcpy(tm_asc_rst, "???");	/* same as unix. */
	}
	else
	{
		strncpy(tm_asc_rst, tm_week_name[tp->tm_wday], 3);
	}
	pos += 3;
	
	/* blank */
	tm_asc_rst[pos++] = ' ';
	
	if (tp->tm_mon < 0 || tp->tm_mon >= 12)		/* month */
	{
		strncpy(tm_asc_rst + pos, "???", 3);
	}
	else
	{
		strncpy(tm_asc_rst + pos, tm_month_name[tp->tm_mon], 3);
	}
	pos += 3;
	
	/* blank */
	tm_asc_rst[pos++] = ' ';
	tm_asc_rst[pos++] = 0;	/* for bellow using 'strcat' */
	
	/* day */
	strcat(tm_asc_rst, _itoa(tp->tm_mday, tmpStr, 10));
	
	strcat(tm_asc_rst, " ");	/* blank */
	
	//hour
	if (tp->tm_hour > 23)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_hour, 2));
	}
	
	strcat(tm_asc_rst, ":");	/* ":" */
	
	/* minute */
	if (tp->tm_min > 59)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_min, 2));
	}
	
	strcat(tm_asc_rst, ":");	/* ":" */
	
	/* second */
	if (tp->tm_sec > 59)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_sec, 2));
	}
	
	strcat(tm_asc_rst, " ");	/* blank */
	
	/* year */
	if (tp->tm_year + 1900 > 2350 || tp->tm_year < 70)
	{
		strcat(tm_asc_rst, "????");
	}
	else
	{
		strcat(tm_asc_rst, _itoa(tp->tm_year + 1900, tmpStr, 10));
	}
	
	strcat(tm_asc_rst, "\n");
	
	return tm_asc_rst;
}


/*-----------------------------------------------------------------------
* 函数: _strdate
* 说明: 得到系统日期并转换为字串，放入datestr中，并返回该字串首指针
* 输入: 
*		datestr: 目标buffer
* 返回: 目标buffer指针
* 注意: datestr buffer长度至少为9
* 注意：输出格式为 "12/05/99"
*-----------------------------------------------------------------------*/
char *_strdate( char *datestr )
{
	struct tm tmTime;
	char tmpStr2[4];
	
	tm_getSystemTime(&tmTime);
	strcpy(datestr, tm_itos_len(tmpStr2, tmTime.tm_mon + 1, 2));
	strcat(datestr, "/");
	strcat(datestr, tm_itos_len(tmpStr2, tmTime.tm_mday, 2));
	strcat(datestr, "/");
	strcat(datestr, tm_itos_len(tmpStr2, tmTime.tm_year % 100, 2));
	
	return datestr;
}

/*-----------------------------------------------------------------------
* 函数: _strtime
* 说明: 得到系统时间并转换为字串，放入datestr中，并返回该字串首指针
* 输入: 
*		datestr: 目标buffer
* 返回: 目标buffer指针
* 注意: datestr buffer长度至少为9
* 注意：输出格式为 "13:40:40"
*-----------------------------------------------------------------------*/
char *_strtime( char *timestr )
{
	struct tm tmTime;
	char tmpStr2[4];
	
	tm_getSystemTime(&tmTime);
	strcpy(timestr, tm_itos_len(tmpStr2, tmTime.tm_hour, 2));
	strcat(timestr, ":");
	strcat(timestr, tm_itos_len(tmpStr2, tmTime.tm_min, 2));
	strcat(timestr, ":");
	strcat(timestr, tm_itos_len(tmpStr2, tmTime.tm_sec, 2));
	
	return timestr;
}

/*-----------------------------------------------------------------------
* 函数: ctime
* 说明: 把time_t型的time转换为tm结构体的形式并按字串格式输出
* 输入: 
*		ptTime: 存放 time_t 型数据的指针
* 返回: 指向转换后字串的指针
*		字串格式"Sun Sep 16 01:03:52 1973\n\0"
* 注意：如果时间为早于1970.1.1 00:00:00,则返回NULL。
*-----------------------------------------------------------------------*/
char *ctime(const time_t * timer)
{
	struct tm * ptTime;
	
	ptTime = gmtime(timer);
	
	if (ptTime->tm_year < (1970 - 1900))
	{
		return NULL;
	}
	
	return asctime( ptTime );
}

/*-----------------------------------------------------------------------
* 函数: gmtime
* 说明: 把 time_t 格式的时间转换为结构体 tm 类型的时间。
* 输入: 
*		timer: 存放要转换的time数据的变量的指针
* 返回: tm 类型的时间
* 注意：gmtime, mktime,和localtime 共用一个结构体"gm_use_tm"来装载他们的数据
*-----------------------------------------------------------------------*/
struct tm *gmtime(const time_t * timer)
{
	return tm_tz_convert(timer, 0, &gm_use_tm);
}

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
struct tm *localtime(const time_t * timer)
{
	return gmtime(timer);
}


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
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr)
{
	int len, overflow, rlen;
	char *dst;
	
	len = 0;
	dst = s;
	
	while ( *format )
	{
		if (*format != '%')	/* normal */
		{
			*dst++ = *format++;
			len++;
			if (len >= maxsize) overflow = 1;
		}
		else	/* special string */
		{
			overflow = tm_writeSpcStr(&dst, &format, maxsize-len, &rlen, timeptr);
			len + rlen;
		}
		
		if (overflow == 1)
			break;
	}
	
	if (overflow == 1)	/* memo-out */
	{
		return 0;
	}
	else
	{
		*dst = 0;	/* 以NULL结尾 */
		return len + 1;	/* 返回带NULL的长度 */
	}

}

/*-----------------------------------------------------------------------
* 函数: tm_itos_len
* 说明: 把int型数字转换为字串，并输出len长度，如果长度小于len，则前边补字符'0'。
* 输入: 
*		dst: 输出结果存放的地址
*		value: 要转的数字
*		len: 需要的长度
* 返回: 输出结果存放的地址
* 注意：只接受正数，只转换为10进制的转换
*-----------------------------------------------------------------------*/
static char *tm_itos_len(char *dst, int value, int len)
{
	int res, i;
	
	dst[len] = 0;
	for (i = len - 1; i >= 0; i--)
	{
		res = value % 10;
		value /= 10;
		dst[i] = res + '0';
	}
	
	return dst;
}

/*-----------------------------------------------------------------------
* 函数: tm_writeSpcStr
* 说明: 输出转义字串。
* 输入: 
*		dst: 输出结果存放的地方
*		fmt: 要转义的字串
*		rstlen: 目前buffer还剩余的长度，用于判断buffer越界
*		rlen: 转换后真实使用的长度
*		timeptr: time结构体
* 返回: 是否越界。1: 越界；0: 没有越界。
* 注意：不返回错误，如果有转义字串不支持，则忽略该字串。
*-----------------------------------------------------------------------*/
static int tm_writeSpcStr(char **dst, const char **fmt, int rstlen, int *rlen, const struct tm *timeptr)
{
	int len, overflow = 0, meaning = 0, tmpData;
	char tmpStr[50], tmpStr2[6], *str;
	const char *sourFmt;
	
	sourFmt = *fmt;
	sourFmt++;	/* skip '%'. */
	
	if (*sourFmt == '#')
	{
		sourFmt++;
		if (*sourFmt == 'a' || *sourFmt == 'A' || *sourFmt == 'b' || *sourFmt == 'B' || 
			*sourFmt == 'p' || *sourFmt == 'X' || *sourFmt == 'z' || *sourFmt == 'Z' || *sourFmt == '%')
		{
			/* do nothing. ignore '#' */
		}
		else
		{
			meaning = 1;
		}
	}
	
	switch(*sourFmt)
	{
	case 'a':	/* Abbreviated weekday name */
		len = 3;
		str = (char *)tm_week_name[timeptr->tm_wday];
		break;
	case 'A':	/* Full weekday name */
		str = (char *)tm_week_name[timeptr->tm_wday];
		len = strlen(str);
		break;
	case 'b':	/* Abbreviated month name */
		len = 3;
		str = (char *)tm_month_name[timeptr->tm_mon];
		break;
	case 'B':	/* Full month name */
		str = (char *)tm_month_name[timeptr->tm_mon];
		len = strlen(str);
		break;
	case 'c':	/* Date and time representation appropriate for locale */
		if (meaning)
		{
			/* format: "Thursday, July 03, 2008 15:02:34" (month/date/year) */
			strcpy(tmpStr, tm_week_name[timeptr->tm_wday]);
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_month_name[timeptr->tm_mon]);
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year + 1900, 4));
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		}
		else
		{	/* format: "07/03/08 15:02:34" (month/date/year) */
			strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mon + 1, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year % 100, 2));
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		}
		str = tmpStr;
		len = strlen(str);
		break;
	case 'd':	/* Day of month as decimal number (01 - 31) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_mday, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_mday, 2);
		}
 		break;
	case 'H':	/* Hour in 24-hour format (00 - 23) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_hour, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_hour, 2);
		}
		break;
	case 'I':	/* Hour in 12-hour format (01 - 12) */
		tmpData = (timeptr->tm_hour>12)?(timeptr->tm_hour-12):(timeptr->tm_hour);
		if (meaning)
		{
			str = _itoa(tmpData, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, tmpData, 2);
		}
		break;
	case 'j':	/* Day of year as decimal number (001 - 366) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_yday + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 3;
			str = tm_itos_len(tmpStr, timeptr->tm_yday + 1, 3);
		}
		break;
	case 'm':	/* Month as decimal number (01 - 12) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_mon + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_mon + 1, 2);
		}
		break;
	case 'M':	/* Minute as decimal number (00 - 59) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_min, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_min, 2);
		}
		break;
	case 'p':	/* Current locale’s A.M./P.M. indicator for 12-hour clock */
		len = 2;
		if (timeptr->tm_hour > 12) strcpy(tmpStr, "PM");
		else strcpy(tmpStr, "AM");
		str = tmpStr;
		break;
	case 'S':	/* Second as decimal number (00 - 59) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_sec, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_sec, 2);
		}
		break;
	case 'U':	/* Week of year as decimal number, with Sunday as first day of week (00 - 53) */
		if (meaning)
		{
			if (timeptr->tm_yday % 7 < timeptr->tm_wday)
				str = _itoa(timeptr->tm_yday / 7, tmpStr, 10);
			else
				str = _itoa(timeptr->tm_yday / 7 + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			if (timeptr->tm_yday % 7 < timeptr->tm_wday)
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7, 2);
			else
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7 + 1,2);
			len = strlen(str);
		}
		break;
	case 'w':	/* Weekday as decimal number (0 - 6; Sunday is 0) */
		/*  ignore 'meaning' value */
		len = 1;
		str = tm_itos_len(tmpStr, timeptr->tm_wday, 1);
		break;
	case 'W':	/* Week of year as decimal number, with Monday as first day of week (00 - 53) */
		if (meaning)
		{	/* for sunday belongs to pre-week */
			if (timeptr->tm_yday % 7 < ((timeptr->tm_wday == 0)?(timeptr->tm_wday + 7):(timeptr->tm_wday)))
				str = _itoa(timeptr->tm_yday / 7, tmpStr, 10);
			else
				str = _itoa(timeptr->tm_yday / 7 + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			if (timeptr->tm_yday % 7 < ((timeptr->tm_wday == 0)?(timeptr->tm_wday + 7):(timeptr->tm_wday)))
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7, 2);
			else
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7 + 1, 2);
			len = strlen(str);
		}
		break;
	case 'x':	/* Date representation for current locale */
		if (meaning)
		{
			/* format: "Thursday, July 03, 2008" */
			strcpy(tmpStr, tm_week_name[timeptr->tm_wday]);
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_month_name[timeptr->tm_mon]);
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year + 1900, 4));
		}
		else
		{
			/* format: "07/03/08" */
			strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mon + 1, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year % 100, 2));
		}
		str = tmpStr;
		len = strlen(str);
		break;
	case 'X':	/* Time representation for current locale */
				/* format: "15:02:34" */
		strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
		strcat(tmpStr, ":");
		strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
		strcat(tmpStr, ":");
		strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		str = tmpStr;
		len = strlen(str);
		break;
	case 'y':	/* Year without century, as decimal number (00 - 99) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_year % 100, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_year % 100, 2);
		}
		break;
	case 'Y':	/* Year with century, as decimal number */
		if (meaning)
		{
			str = _itoa(timeptr->tm_year+1900, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 4;
			str = tm_itos_len(tmpStr, timeptr->tm_year+1900, 4);
		}
		break;
	case 'z':	/* Time-zone name or abbreviation; no characters if time zone is unknown */
	case 'Z':
				/* should put out "中国标准时间", we ignore now */
		len = 0;
		str = tmpStr;
		break;
	case '%':	/* Percent sign */
		len = 1;
		tmpStr[0] = '%';
		str = tmpStr;
		break;
	default:
		len = 0;
		str = tmpStr;
		break;
	}
	
	if (len >= rstlen)
	{
		overflow = 1;
	}
	else
	{
		strncpy(*dst, str, len);
		*dst += len;
		**dst = 0;	/* Note: not *dst++ */
	}
	
	sourFmt++;	/* skip the checked character */
	*fmt = sourFmt;
	*rlen = len;
	
	return overflow;
	
}


