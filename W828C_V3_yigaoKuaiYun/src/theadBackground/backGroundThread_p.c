/*
 * 该进程用于后台程序的扫描，进程是不会释放的，故在进程优先级时，安排较低
 * 不然会占用系统时间
*/

/*
 * 暂时加入的扫描：
    1、adc检测
*/
#include "hyTypes.h"
#include "backGroundThread_p.h"

/*---------------------------------------------------------
*函数: bgrd_ThreadMethod
*功能: 后台扫描应用进程,进程永不释放
*参数: none 
*返回: none
*---------------------------------------------------------*/
int bgrd_ThreadMethod(void)
{
    bgrd_main(); 
}