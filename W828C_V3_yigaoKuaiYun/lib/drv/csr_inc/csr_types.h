#ifndef CSR_TYPES_H__
#define CSR_TYPES_H__
/****************************************************************************

               (c) Cambridge Silicon Radio Limited 2009

               All rights reserved and confidential information of CSR

REVISION:      $Revision: #1 $
****************************************************************************/
#undef  FALSE
#define FALSE (0)

#undef  TRUE
#define TRUE (1)

#ifndef NULL
#define NULL (0L)
#endif

#undef  CSRMAX
#define CSRMAX(a,b)    (((a) > (b)) ? (a) : (b))

#undef  CSRMIN
#define CSRMIN(a,b)    (((a) < (b)) ? (a) : (b))

/* To shut lint up. */
#undef  CSR_UNUSED
#define CSR_UNUSED(x)          (void)(x)
#define CSR_PARAM_UNUSED(x)    ((void)(x))

#define CSR_INVALID_PHANDLE     0xFFFF /* Invalid protocol handle setting */
#define CSR_INVALID_TIMERID     (0)

/* Data types */

typedef unsigned char		CsrUint8;
typedef unsigned short		CsrUint16;
typedef unsigned int		CsrUint32;

typedef char				CsrInt8;
typedef short				CsrInt16;
typedef int					CsrInt32;

/* Synergy types */
typedef int				delimiter_for_start_of_csr_types_t;

typedef unsigned char		CsrBool;

//typedef unsigned char		CsrCharString;
//typedef unsigned char		CsrUcs2String; /* UCS2 oriented byte-pairs ordered (MSB,LSB) strings */
//typedef CsrUint16			CsrUtf16String; /* 16-bit UTF16 strings */
typedef CsrUint32			CsrUint24;

#endif

