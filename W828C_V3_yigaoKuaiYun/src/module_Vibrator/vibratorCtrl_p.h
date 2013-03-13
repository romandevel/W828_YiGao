#ifndef HYC_VIBRATOR_CTRL_MODE
#define HYC_VIBRATOR_CTRL_MODE

#include "hyTypes.h"

#include "hyhwGpio.h"

/*---------------------------------------------------------------------------
 函数: VibratorCtrl_Start
 功能: 启动振动器
 参数: millisecond----要求振动总时间，毫秒为单位，时间到时，主动退出该进程
 					 当该值为0时，不主动退出，直到调用VibratorCtrl_Stop()
 返回: HY_ERROR---该进程正在运行，当前设置无效
 	   HY_OK------启动成功
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Start(U32 millisecond);

/*---------------------------------------------------------------------------
 函数: VibratorCtrl_Stop
 功能: 停止振动器
 参数: 无
 返回: 无
* -----------------------------------------------------------------------------*/
void VibratorCtrl_Stop(void);


#endif //#ifndef HYC_VIBRATOR_CTRL_MODE