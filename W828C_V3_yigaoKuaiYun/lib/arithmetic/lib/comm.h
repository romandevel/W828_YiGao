#ifndef COMM_H
#define COMM_H

#ifdef __cplusplus
extern "C"
{
#endif

/***module configuration*************************************************************/


#pragma O2 Otime

/***end of module configuration******************************************************/


/***marco definition*****************************************************************/

	#include "hyOsCpuCfg.h"
	#include "hyTypes.h"
	#include "hyErrors.h"
	#include "abstractfilesystem.h"

	typedef void* HANDLE;
	
//common
#define DWALIGN(Offset) {if(((Offset)&3)!=0) {(Offset)=(Offset)&(~3);(Offset)+=4;}}
//#define WALIGN(Offset) {if(((Offset)&1)!=0) {(Offset)=(Offset)&(~1);(Offset)+=2;}}

/***end of marco definition**********************************************************/


/***variable definition**************************************************************/

/***end of variable definition*******************************************************/


/***function definition**************************************************************/

/***end of function definition*******************************************************/


#define COMM_MALLOC(x) (void *)malloc(x)
#define COMM_FREE(x) free(x)


#ifdef __cplusplus
}
#endif

#endif