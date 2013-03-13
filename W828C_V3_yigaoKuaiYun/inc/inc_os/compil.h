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
#ifndef _COMPIL_H_
#define _COMPIL_H_

#define _RELEASE "ARM ADS 1.2"

/* configure const for this compiler, when _CONST is 1, the CONST pseudo
 * type will effect the const type. Otherwise CONST has no effect.
 */

#define _CONST 1


/* 和硬件平台有关的两个定义 */
/* definition for number of bits in integer */
#define BITS_INT 32

/* definition for size of kernel queue item */
typedef void *KQ_ITEM;


#endif //_COMPIL_H_
