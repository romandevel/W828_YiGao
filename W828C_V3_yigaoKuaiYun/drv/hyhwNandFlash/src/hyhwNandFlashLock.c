/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"
#include "hyhwNandFlash.h"
/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define NAND_USE_WLOCK

/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* EXTERNAL Variable ,constant or function
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* STATICS  Variable ,constant or function
--------------------------------------------------------------------------*/
#ifdef NAND_USE_WLOCK
U32 lockNandFlash = 0;
U32 lockNandFlashInit = 0;
#endif

void wLock_NandFlashInit(void)
{
#ifdef NAND_USE_WLOCK
	lockNandFlashInit = 1;
#endif
}

/*-----------------------------------------------------------------------------
* 函数:	wLock_NandFlash
* 功能:	共享资源Spi申请并lock
* 		在使用Spi之前调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void wLock_NandFlash(void)
{
#ifdef NAND_USE_WLOCK
	U32 *pNandFlash = (U32*)&lockNandFlash;
	if (lockNandFlashInit == 1)
		(void)wlock((unsigned) pNandFlash);
#endif
}
/*-----------------------------------------------------------------------------
* 函数:	unLock_NandFlash
* 功能:	共享资源申请并lock
* 		在使用Spi之后调用该函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void unLock_NandFlash(void)
{
#ifdef NAND_USE_WLOCK
	U32 *pNandFlash = (U32*)&lockNandFlash;
	if (lockNandFlashInit == 1)
		(void)unlock((unsigned) pNandFlash);
#endif
}
