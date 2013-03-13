#ifndef _BT_API_H_
#define _BT_API_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "hyTypes.h"

typedef struct
{
	U32 lap;		/* Lower Address Part 00..23 */
	U8  uap;		/* upper Address Part 24..31 */
	U16 nap;		/* Non-significant    32..47 */
} tBdAddress;

#ifdef __cplusplus
}
#endif

#endif //_BT_API_H_