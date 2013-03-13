#ifndef __DEBUG_GPIO_H__
#define __DEBUG_GPIO_H__

#if 0
#define DEBUG_IO_INTIAL(a)   debug_io_initial a
#define _,
#define DEBUG_IO_LOW(a)	  debug_io_set_low  a
#define DEBUG_IO_HIGH(a)	 debug_io_set_high a
#else
#define DEBUG_IO_INTIAL(a)
#define DEBUG_IO_LOW(a)
#define DEBUG_IO_HIGH(a)
#endif

#endif