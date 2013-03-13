/* gsmd logging functions 
 *
 * (C) 2006-2007 by OpenMoko, Inc.
 * Written by Harald Welte <laforge@openmoko.org>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */ 

#include "std.h"
//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"
#include "stdarg.h"
//#include "time.h"

#include "types.h"
#include "gsmd.h"


#if 0
static FILE *logfile;
static FILE syslog_dummy;
static int loglevel;


static int gsmd2syslog[] = {
	[GSMD_DEBUG]	= LOG_DEBUG,
	[GSMD_INFO]	= LOG_INFO,
	[GSMD_NOTICE]	= LOG_NOTICE,
	[GSMD_ERROR]	= LOG_ERR,
	[GSMD_FATAL]	= LOG_CRIT,
};
#endif


static inline int gsmd2syslog_level(int level)
{
#if 0
	if (level >= SIZE_OF_ARRAY(gsmd2syslog))
		return LOG_ERR;

	return gsmd2syslog[level];
#endif
	return 0;
}

void __gsmd_log(int level, const char *file, int line, const char *function,
		const char *format, ...)
{
	char *timestr;
	va_list ap;
	
#if 0
	time_t tm;
	FILE *outfd;

	if (level < loglevel)
		return;
	
	if (logfile == &syslog_dummy) {
		va_start(ap, format);
		vsyslog(gsmd2syslog_level(level), format, ap);
		va_end(ap);
	} else {
		if (logfile)
			outfd = logfile;
		else
			outfd = stderr;

		tm = time(NULL);
		timestr = ctime(&tm);
		timestr[strlen(timestr)-1] = '\0';
		fprintf(outfd, "%s <%1.1d> %s:%d:%s() ", timestr, level, file, 
			line, function);

		va_start(ap, format);
		vfprintf(outfd, format, ap);
		va_end(ap);

		fflush(outfd);
	}
#endif
}

int gsmdlog_init(const char *path)
{
#if 0
	if (!strcmp(path, "syslog")) {
		logfile = &syslog_dummy;
		openlog("gsmd", 0, LOG_DAEMON);
	} else {
		logfile = fopen(path, "a+");
	}

	if (logfile == NULL)
		return -1;
	
	gsmd_log(LOG_INFO, "logfile successfully opened\n");
#endif

	return 0;
}




#ifdef _GSMD_DEBUG_LOG
#define FILE_LOG


#define GSMD_DEBUG_FILE "C:/gsmd.txt"


int gsmd_debug_fp = -1;

void gsmd_debug_init(void)
{
	int fp=0;
	int fsize;
	char *startstr = "\n\n\n-------------  log - start  ---------------\n";
	

#ifdef FILE_LOG

	fp = AbstractFileSystem_Open(GSMD_DEBUG_FILE,1);
	if(0>fp)
	{
		fp = AbstractFileSystem_Create(GSMD_DEBUG_FILE);
		if (0>fp)
		{
		}
		else
		{
			AbstractFileSystem_Seek(fp,0,0); //AFS_SEEK_OFFSET_FROM_START
			AbstractFileSystem_Write(fp, startstr, strlen(startstr));
		}
	}
	else
	{
		AbstractFileSystem_Seek(fp,0,2); //AFS_SEEK_OFFSET_FROM_END
		AbstractFileSystem_Write(fp, startstr, strlen(startstr));
	}
#endif

	gsmd_debug_fp = fp;
}


int gsmd_debug_log(const char *buf, int len, int log_tm, struct timeval *exp)
{
	int wlen = 0;
	
	if (gsmd_debug_fp >= 0)
	{
		//hyUsbMessageByLen(buf, len);
//		PhoneTrace(0,buf);
#ifdef FILE_LOG
		wlen = AbstractFileSystem_Write(gsmd_debug_fp, buf, len);
#endif

		if(log_tm)
		{
			char curtime[50];
			struct timeval tv;
			
			gettimeofday(&tv, NULL);
			len = sprintf(curtime, " - %d:%d %d:%d", tv.tv_sec, tv.tv_usec, exp->tv_sec,  exp->tv_usec);
			
			//hyUsbMessageByLen(curtime, len);
			
#ifdef FILE_LOG
			AbstractFileSystem_Write(gsmd_debug_fp, curtime, len);
#endif
		}
	}
	return wlen;
}



void gsmd_debug_stop(void)
{
#ifdef FILE_LOG
	if(gsmd_debug_fp>=0)
		AbstractFileSystem_Close(gsmd_debug_fp);
#endif
}

#endif


