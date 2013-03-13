#ifndef _SHELL_P_H_
#define _SHELL_P_H_


/*
*include files
*/
#include "grap_deskbox.h"

/*---------------------------------------------------------
*函数: Shell_ThreadMethod
*功能: 系统初始化后调用该进程，相当于以前的swichmode但是该进程
*      可以创建或或者唤醒其它UI进程
*参数: 
*返回: none
*---------------------------------------------------------*/
int Shell_ThreadMethod(void);

#endif