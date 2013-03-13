#ifndef _CDMA_SMS_TYPE_H_
#define _CDMA_SMS_TYPE_H_

#include "hyTypes.h"
//#include <stdlib.h>
//#include <string.h>

typedef signed char    		int8;
typedef signed short   		int16;
typedef signed long    		int32;
 
typedef unsigned char  		uint8;
typedef unsigned short 		uint16;
typedef unsigned long  		uint32;


typedef enum _eBOOL
{
    CDMA_FALSE = 0,
    CDMA_TRUE  = 1,
}bool;



#define PACKED  __packed 

#endif 