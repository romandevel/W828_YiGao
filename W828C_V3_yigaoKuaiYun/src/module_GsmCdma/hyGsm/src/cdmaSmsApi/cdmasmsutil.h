#ifndef _CDMA_SMS_UTIL_H_
#define _CMDA_SMS_UTIL_H_

#include "cdmasmstype.h"

#define BIT_SIZE(type)  (sizeof(type) * 8)

#ifndef MIN
#define MIN(x,y)        (((x) < (y)) ? (x): (y))
#endif

#ifndef MAX
#define MAX(x,y)        (((x) > (y)) ? (x): (y))
#endif

#define MASK_TO_COPY(type)          ( (0xffffffff) >> (32 - BIT_SIZE(type)) )

#define BYTE_MASK(offset, len) \
    ( (0xff >> offset) & (0xff << (8 - (offset + len))) )

#define MASK(width, offset, data) \
    (((width) == BIT_SIZE(data)) ? (data) :   \
    ((((MASK_TO_COPY(data) << (BIT_SIZE(data) - ((width) % BIT_SIZE(data)))) & \
    MASK_TO_COPY(data)) >>  (offset)) & (data)))

#define MASK_AND_SHIFT(width, offset, shift, data)  \
        ((((signed) (shift)) < 0) ?       \
        MASK((width), (offset), (data)) << -(shift) :  \
        MASK((width), (offset), (data)) >>  (((unsigned) (shift))))


extern uint8    CdmaSmsGetUint8( uint8 *data, uint16 startBit, uint8 numBits );
extern uint16   CdmaSmsGetUint16( uint8 *data, uint16 startBit, uint8 numBits );
extern void     CdmaSmsPutUint8( uint8 *data, uint16 startBit, uint8 numBits, uint8 value );
extern void     CdmaSmsPutUint16( uint8 *data, uint16 startBit, uint8 numBits, uint16 value );

#endif
