#ifndef _APP_CONTROL_CONSUME_H_
#define _APP_CONTROL_CONSUME_H_

/*
*include files
*/
#include "hyTypes.h"

//系统运行的最高频率
#define CSM_MAX_CLK     (240)


//临时改变频率超时的时间
//#define APP_CSM_ELAPSE_TIME   100
#define APP_CSM_ELAPSE_TIME   150

/*
*define types
*/
//只有特殊的进程运行时才需要这个进程系数，否则
//每个进程定义一个功耗系数，当达到某个系数时对应实施相应的策略
typedef struct _tAPP_CTR_CSM
{
   eAPPLICATION_NO pid; //进程的pid
   U32             coef;//进程运行的系数  
}tAPP_CTR_CSM;

/*---------------------------------------------------------------------------
 函数：ConSume_CoefInitial
 功能：初始化变量
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_CoefInitial(void);

/*---------------------------------------------------------------------------
 函数：ConSume_ChangeCoefTemp
 功能：临时改变一下频率，主要是由于在进程切换和刷整屏的时候有延时的感觉,如果当前频率比要调整的高则不改变频率
 参数：AppId:进程编号枚举  coef:需要改变的系数
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTemp(U32 coef, int type);

/*---------------------------------------------------------------------------
 函数：ConSume_ChangeCoefTempElapseTime
 功能：临时改变一下频率的时间是否超时
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTempElapseTime(void);

/*---------------------------------------------------------------------------
 函数：ConSume_UpdataAppCoef
 功能：更新应用进程的系数,这个系数是动态改变的
 参数：AppId:进程编号枚举  coef:需要改变的系数
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataAppCoef(eAPPLICATION_NO AppId, U32 coef);

/*---------------------------------------------------------------------------
 函数：ConSume_UpdataSpeedDirect
 功能：直接更改频率，不需要控制台去修改频率
 参数：AppId:进程编号枚举  coef:需要改变的系数
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataSpeedDirect(eAPPLICATION_NO AppId, U32 coef);

/*---------------------------------------------------------------------------
 函数：ConSume_Get_PidSpeed
 功能：得到当前pid的频率
 参数：AppId:进程编号枚举  coef:需要改变的系数
 返回：none
* -----------------------------------------------------------------------------*/
U16 ConSume_Get_PidSpeed(eAPPLICATION_NO AppId);

#if 0
/*---------------------------------------------------------------------------
 函数：ConSume_WifiChangeSpeedTimeOut
 功能：该函数只提供该wifi进程使用其他勿使用，修改频率有超时功能
 参数：coef:需要增加的系数
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_WifiChangeSpeedTimeOut(U32 coef);

/*---------------------------------------------------------------------------
 函数：ConSume_AppChangeSpeedTimeOut
 功能：直接增加频率，不需要控制台去修改频率
 参数：coef:需要增加的系数
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_WifiCheckTimeOut(void);
#endif

/*---------------------------------------------------------------------------
 函数：ConSume_DetectCurCoef
 功能：后台调整频率
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
void ConSume_DetectCurCoef(void);

#endif