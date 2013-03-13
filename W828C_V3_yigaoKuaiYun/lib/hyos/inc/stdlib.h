/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

#ifndef _STDLIB
#define _STDLIB


/*  macro definitions */
#define EXIT_FAILURE	_EXFAIL
#define EXIT_SUCCESS	_EXSUCCESS

/*  type definitions */
//#ifndef _SIZET
//#define _SIZET
//typedef unsigned int size_t;
//#endif

/*	macro definitions */

#define abs(x)		((x) < 0 ? -(x) : (x))

/*  function prototypes definitions*/
double	atof(char *s);

char	*calloc(unsigned n, unsigned size);
//char	*free(char *p, char *link);
char	*getenv(const char *);
char	*malloc(unsigned size);


#endif
