#include "cdmasmsutil.h"

static const uint8 bitValueMask[] =
{
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

static const uint8 saveTopMask[] =
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe
};

/* BitMasks for uint16 values */
static const uint16 valueMask[] =
{
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsGetUint8
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
uint8 CdmaSmsGetUint8( uint8 *data, uint16 startBit, uint8 numBits )
{
    uint8 startByteIndex;
    uint8 startBitIndex;   /* Index within the start byte */
    uint8 value;
    int  bitsIn2ndByte;

    startByteIndex = (uint8)(startBit >> 3);
    startBitIndex  = (uint8)(startBit & 0x07);

    /* First get the bits out of the startByteIndex byte */
    value = data[ startByteIndex ] & bitValueMask[ startBitIndex ];

    bitsIn2ndByte = numBits + startBitIndex - 8;

    if( bitsIn2ndByte > 0 )
    {
        /* We need some bits out of the next byte */

        value = (value << bitsIn2ndByte) | (data[startByteIndex+1] >> (8 - bitsIn2ndByte));
    }
    else
    {
        /*  We need to shift down the bits we already have into the least
         *  significant bit positions.  If we have negative bits in the 2nd
         *  byte, this is the same as having that many TOO MANY in the 1st
         *  byte, so we shift those bits out
         */
        value = value >> -bitsIn2ndByte;
    }

    return( value );
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsGetUint16
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
uint16 CdmaSmsGetUint16( uint8 *data, uint16 startBit, uint8 numBits )
{
    uint16 value;

    value = ((uint16)(CdmaSmsGetUint8(data, startBit, numBits - 8) ) << 8) |
             (uint16)(CdmaSmsGetUint8(data, startBit + numBits - 8, 8) );

    return( value );
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsPutUint8
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
void CdmaSmsPutUint8( uint8 *data, uint16 startBit, uint8 numBits, uint8 value )
{
    uint8 startByteIndex;
    uint8 startBitIndex;   /* Index within the start byte */
    int  bitsIn2ndByte;
    int  lowBitsSave1stByte;
    uint8 temp;
    uint8 mask;
    
    startByteIndex = (uint8)(startBit >> 3);
    startBitIndex  = (uint8)(startBit & 0x7);

    /* Two cases - all the bits are in a single byte, or the value to be
     * written spans two bytes.
     */
    bitsIn2ndByte = startBitIndex + numBits - 8;

    if( bitsIn2ndByte > 0 )
    {
        /* value spans 2 bytes */
        /* First, zero out the top bits of the first byte */
        mask = saveTopMask[ startBitIndex ];
        data[ startByteIndex ] &= mask;
        
        /* Or in the most significant bits of the value */
        data[ startByteIndex ] |= (~mask & (value >> bitsIn2ndByte));
        
        /* Now zero out the high bits of the low byte */
        data[ startByteIndex + 1 ] &= ~saveTopMask[ (uint8) bitsIn2ndByte ];
        
        /* Or in the least significant bits of the value */
        data[ startByteIndex + 1 ] |= (value << (8 - bitsIn2ndByte));
    }
    else
    {
        /* set the mask bits */
        
        lowBitsSave1stByte = -bitsIn2ndByte;
        
        mask = valueMask[numBits] << lowBitsSave1stByte;
        
        /* Zero out the bits we want to replace, i.e. put the bits we
         * want to keep in Temp */
        
        temp  = data[ startByteIndex ] & ~mask;
        
        /* Now or in the new bits with the ones we're keeping */
        data[ startByteIndex ] = temp | ( mask & (value << lowBitsSave1stByte));
    }    
}
/*****************************************************************************
    FUNCTION NAME:  CdmaSmsPutUint16
    DESCRIPTION:
    PARAMETERS:
    RETURNS:
*****************************************************************************/
void CdmaSmsPutUint16( uint8 *data, uint16 startBit, uint8 numBits, uint16 value )
{
    /* Make assertions here */    
    CdmaSmsPutUint8(data, startBit, 8, (uint8)(value >> (numBits -= 8)) );
    
    CdmaSmsPutUint8(data, startBit + 8, numBits, (uint8)(value & valueMask[numBits]) );
}
