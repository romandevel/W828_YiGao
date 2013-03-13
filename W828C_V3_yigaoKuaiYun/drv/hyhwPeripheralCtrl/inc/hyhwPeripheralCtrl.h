
#ifndef PERIPHERAL_CTRL_H
#define PERIPHERAL_CTRL_H

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"

/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwCkc.h"
#include "hyhwPeripheralCtrl.h"


/*-----------------------------------------------------------------------------
* 函数:	hyhwPeripheralCtrl_Init
* 功能:	根据应用需求，关闭不常用的外围设备，以节省功耗。
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwPeripheralCtrl_Init(void);


#endif