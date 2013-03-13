
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2009 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename gps_ptypes.h
//
// $Header: S:/GN_GPS_Nav_MKS/Arch/LSmtron/GN_GPS/rcs/gps_ptypes.h 1.2 2010/05/07 16:48:00Z belaroussi Exp $
// $Locker: $
//****************************************************************************
//
// GPS platform primitive typedefs


#ifndef GPS_PTYPES_H
#define GPS_PTYPES_H

//****************************************************************************
// GPS 4500 Build-specific option definitions


//****************************************************************************
// Include the following if all available debug screens are available

#define CPU_PROFILE              // Enable GPS_Core CPU Profiling


//****************************************************************************
// If the Nav Debug data is expected to be viewed in real-time, for example
// in ProComm, the following ESC characters may best be defined.

#define _HOME_  ""
#define _CLEAR_ ""
#define _ERASE_ ""


//****************************************************************************
// In Generic_Lib\R4_math.h the R4abs macros is defined as being equal to
// the _fabsf macro under certain conditions. However, in at least one
// port the _fabsf macro seems not to be defined when it was expected
// to be. Hence we can disable the definition of R4abs to be _fabsf by
// defining the following option.

#define NO_FABSF


//****************************************************************************
// On this target va_arg() needs I4 type for CH arguments.

#define VA_ARG_NEEDS_I4_FOR_CH


//****************************************************************************
// If there is no sign of valid Comms data from the GPS ME, the High-Level
// software will repeatedly send a Wake-Up command. The following parameter
// defines the interval between these commands. If a value of 0 is defined,
// a wake-up command will be sent every time the GN_GPS_Update function is
// called by the host.

#define WAKEUP_TX_INT    0


//****************************************************************************

// Define the Default Debug Enabled flags
#define DEFAULT_NAV_DEBUG   0x0001        // Default basic debug level
#define DEFAULT_GNB_DEBUG   0x0001        // Default basic debug level
#define DEFAULT_EVENT_LOG   0x0001        // Default basic debug level


//****************************************************************************
// Primative Data Type definitions

// Unsigned integer types
typedef unsigned char   U1;         // 1 byte unsigned numerical value
typedef unsigned short  U2;         // 2 byte unsigned numerical value
typedef unsigned int    U4;         // 4 byte unsigned numerical value

// Signed integer types
typedef signed   char   I1;         // 1 byte signed numerical value
typedef signed   short  I2;         // 2 byte signed numerical value
typedef signed   int    I4;         // 4 byte signed numerical value

// Real floating point number, single and double precision
typedef float           R4;         // 4 byte floating point value
typedef double          R8;         // 8 byte floating point value

// Define ASCII character type
typedef char            CH;         // ASCII character

// Boolean / Logical type
typedef unsigned char   BL;         // Boolean logical (TRUE or FALSE only)
typedef unsigned char   L1;         // 1 byte logical  (TRUE or FALSE only)
typedef unsigned short  L2;         // 2 byte logical  (TRUE or FALSE only)
typedef unsigned int    L4;         // 4 byte logical  (TRUE or FALSE only)

// Ensure TRUE and FALSE are defined
#ifndef TRUE
   #define TRUE  1
#endif

#ifndef FALSE
   #define FALSE 0
#endif

// Ensure NULL is defined
#ifndef NULL
   #define NULL (void*)0
#endif

/********************************
 * add for test 2007.8.3		*
 ********************************/
typedef int FS_HANDLE;
typedef unsigned char WCHAR;
typedef unsigned int UINT;
//typedef	int			INT;
typedef	float	kal_uint32;
typedef	int		module_type;	// device ID
typedef	char	kal_bool;
typedef	int		UART_PORT;
typedef	int		UART_baudrate;
typedef	char	UART_bitsPerCharacter;
typedef	char	UART_parity;
typedef	char	UART_flowCtrlMode;
typedef	char	UART_stopBits;
typedef	unsigned char uint8;
typedef	unsigned int  kal_uint16;
typedef	unsigned char kal_uint8;

typedef struct
{
UART_baudrate baud;
UART_bitsPerCharacter dataBits;
UART_stopBits stopBits;
UART_parity parity;
UART_flowCtrlMode flowControl;
uint8 xonChar;
uint8 xoffChar;
char	DSRCheck;	// can not support 'bool' type
} UARTDCBStruct;


#define	FS_READ_WRITE	1
#define	FS_CREATE_ALWAYS 	7
#define	uart_port2	1
#define	KAL_TRUE	1
#define	KAL_FALSE	0
//-----------------------------------------------------------------------------

// Define TRUE and FALSE
#ifndef TRUE
   #define TRUE  1
#endif

#ifndef FALSE
   #define FALSE 0
#endif

// Ensure NULL is defined
#ifdef NULL
   #undef NULL
#endif
#define NULL 0

//-----------------------------------------------------------------------------

// Define the use of inline function types as being possible
#define INLINE __inline


//*****************************************************************************

#endif   // GPS_PTYPES_H

// end of file
