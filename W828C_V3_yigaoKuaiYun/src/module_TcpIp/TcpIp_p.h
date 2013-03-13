#ifndef _TCPIP_P_H_
#define _TCPIP_P_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

//tcpip协议栈运行的状态，当处于TCPIP_INI状态时，还没有分配ip address 不能进行相关网络操作
typedef enum {
TCPIP_INI = 0,
TCPIP_RUN,
TCPIP_FAIL //一般情况下是ip分配失败造成的
}eTCPIP_STATE;

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
//初始化TcpIp变量
//void TcpIp_Initial(void);

/*---------------------------------
*函数: TcpIp_GetRunState
*功能: 得到TcpIp运行状态
*参数: none
*返回: 0:处于wait 1:run
---------------------------------*/
//eTCPIP_STATE TcpIp_GetRunState(void);

int TcpIp_ThreadMethod(void);

#endif //_TCPIP_P_H_
