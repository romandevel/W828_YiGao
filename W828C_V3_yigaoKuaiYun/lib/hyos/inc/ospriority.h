#ifndef OS_PRIORITY_H
#define OS_PRIORITY_H


/*************************************************************************
 * INCLUDES
 *************************************************************************/
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"


typedef enum
{
    PRIORITY_0_OSPP = 0,
    PRIORITY_1_OSPP,
    PRIORITY_2_OSPP,
    PRIORITY_3_OSPP,
    PRIORITY_4_OSPP,
    PRIORITY_5_OSPP,
    PRIORITY_6_OSPP,
    PRIORITY_7_OSPP,
    PRIORITY_8_OSPP,
    PRIORITY_9_OSPP,
    PRIORITY_10_OSPP,

    PRIORITY_11_OSPP,	//zbl
    PRIORITY_12_OSPP,
    
    PRIORITY_13_OSPP,	//gaowz
    PRIORITY_14_OSPP,
    PRIORITY_15_OSPP,
    PRIORITY_16_OSPP,
    PRIORITY_17_OSPP,
    PRIORITY_18_OSPP,
    PRIORITY_19_OSPP,
    PRIORITY_20_OSPP,
    
    
    PRIORITY_NEXT_HIGHEST_OSPP,
    PRIORITY_HIGHEST_OSPP //highest //zbl
} eOSPRIORITY;

#endif  //OS_PRIORITY_H