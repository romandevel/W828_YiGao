#ifndef _COMMON_DLG_H_
#define _COMMON_DLG_H_

#include "grap_view.h"
#include "grap_api.h"

#define DLG_ALLWAYS_SHOW  	 (0xFFFFFFFF)
#define DLG_DEFAULT_TIME 	 500
#define DLG_DEFAULT_TIME_3S  300

#define DLG_WIDGET_NUM		9


//定义对话框返回状态
typedef enum _eDLG_STAT
{
    DLG_CONFIRM_EN, //对话框确定
    DLG_CANCEL_EN,   //对话框取消
    DLG_TIMEROUT_EN //处理事件超时退出
}eDLG_STAT;


/*===================================================================*/

int Com_GetPercentTotal();

void Com_SetPercentTotal(int total);

int Com_GetPercentRecv();

void Com_SetPercentRecv(int recvsize);

/*---------------------------------------------------------
*函数: Com_DlgUpdataStat
*功能: 更新对话框状态
*参数: stat ： 对话框状态
*返回: none
*---------------------------------------------------------*/
void Com_DlgUpdataStat(eDLG_STAT stat);

U8 Com_DlgGetStat();

/*---------------------------------------------------------
*函数: Com_DlgStartTime
*功能: 对话框开始的时间
*参数: none
*返回: 开始时间
*---------------------------------------------------------*/
U32 Com_DlgStartTime(void);

/*---------------------------------------------------------
*函数: Com_DlgTotalLiveTime
*功能: 对话框生存的时间
*参数: none
*返回: 生存时间
*---------------------------------------------------------*/
U32 Com_DlgTotalLiveTime(void);

/*---------------------------------------------------------
*函数: Com_DlgTimeOut
*功能: 对话框是否超时
*参数: none
*返回: 1: 超时 0：未超时
*---------------------------------------------------------*/
int Com_DlgTimeOut(void);

/*---------------------------------------------------------
*函数: ComWidget_LabTimerISR
*功能: 点击信号图标时产生的中断事件   
*参数: pView ： lable point
*返回: none 
*---------------------------------------------------------*/ 
S32 ComWidget_LabTimerISR(void *pView);

/*---------------------------------------------------------
*函数: ComWidget_UpdataTimerMsg
*功能: 把消息队列中的timer消息去空后    
*参数: pDeskFather ：父指针
*返回: none 
*---------------------------------------------------------*/  
void ComWidget_UpdataTimerMsg(void *pDeskFather);

S32 ComWidget_LabTimerTimeOutISR(void *pView);

/*
*功能：处理对话框信息
*参数: pDeskFather:父窗体指针 DlgType:对话框类型 1提示2警告3询问 id:显示的字符的id 
*      DlgPressEnter :确定按钮的函数指针 DlgCancelEnter：取消按钮的函数指针
*      DispTime : 对话框显示的时间		 
*/
eDLG_STAT Com_SpcDlgDeskbox(U8 *str,U8 soundType,void *pDeskFather,U32 DlgType,
						PressEnter DlgPressEnter,PressEnter DlgCancelEnter,U32 DispTime);

/*---------------------------------------------------------
*函数: Com_PercentLabTimerISR
*功能: 动画提示框中显示百分比  
*参数: pView ： lable point
*返回: none 
*---------------------------------------------------------*/ 
void Com_PercentLabTimerISR(void *pView);

/*
*功能：处理对话框信息
*参数: pDeskFather:父窗体指针 DlgType:对话框类型 1动画取消 id:显示的字符的id  
*      DlgPressEnter :确定按钮的函数指针 DlgCancelEnter：取消按钮的函数指针
*      DlgLabTimer: 传入的lableTimer判断函数指针 DispTime : lab中断的时间
*/
eDLG_STAT Com_CtnDlgDeskbox(void *pDeskFather,U32 DlgType,U8 *str,PressEnter DlgPressEnter,PressEnter DlgCancelEnter,
							Label_TimerISR DlgLabTimer,U32 DispTime , U32 TimeOut);


/*----------------------------------------------------------------------------------------------*/
/*---------------------------------------等待网络处理完毕---------------------------------------*/
/*----------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------
*函数: Com_ShutDownLabelTimerISR
*功能: 
*参数: none
*返回: 
*---------------------------------------------------------*/
void Com_Usb_ShutDownLabelTimerISR(void *pView);


/*------------------yu---------------------------------------
* 函数: Com_ShutDownDlgDeskbox
* 功能: 对话框，关闭后台进程用 主要是强制关机和强制进入USB时候调用
* 参数: pDeskFather --- 父窗体
* 返回: 
------------------------------------------------------------*/
U8 Com_ShutDownDlgDeskbox(void *pDeskFather,U8 *str,Label_TimerISR DlgLabTimer,U32 timeOut);

#endif