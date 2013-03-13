#ifndef _PHONE_DEBUG_H_
#define _PHONE_DEBUG_H_

#include "stdarg.h"
#include "hyTypes.h"
#include "LcdAPIFunc.h"

#define PHONE_DEBUG    0

typedef enum {
	LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
} LogCodes;




void PhoneTrace(int level,const char *str,...);

#if PHONE_DEBUG > 0
#define PHONEDEBUG(a)  PhoneTrace a

#define LWIP_PHONEDEBUG(x,y)  PhoneTrace(x,y)

#else
#define PHONEDEBUG(a)
#endif


#endif