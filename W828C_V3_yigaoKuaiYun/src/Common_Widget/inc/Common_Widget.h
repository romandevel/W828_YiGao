#ifndef _COMMON_WIDGET_H_
#define _COMMON_WIDGET_H_

/*
*include standard files
*/
#include "hyTypes.h"

/*
*include local files
*/
#include "grap_api.h"
#include "LcdAPIFunc.h"
#include "base.h"
#include "hyhwRtc.h"


#define COM_WIDGET_NUM   		7


#define COM_LOGO_ID			(EDIT_ID_BASE-1)//logo
#define COM_GPRSEVT_ID		(EDIT_ID_BASE-2)//gprs 事件
#define COM_DATE_ID			(EDIT_ID_BASE-3)//日期时间
#define COM_GSMSIG_ID		(EDIT_ID_BASE-4)//gsm信号
#define COM_WIFISIG_ID		(EDIT_ID_BASE-5)//wifi信号
#define COM_BAT_ID			(EDIT_ID_BASE-6)//电量
#define COM_INPUT_ID		(EDIT_ID_BASE-7)//输入法



#define LOGO_X		0
#define LOGO_Y		3
#define LOGO_W		68
#define LOGO_H		23

#define GPRS_X		70
#define GPRS_Y		0
#define GPRS_W		74
#define GPRS_H		16

#define DATE_X		145
#define DATE_Y		2
#define DATE_W		40
#define DATE_H		9

//显示焦点输入框已输入个数和可输入总个数,不单独创建该控件,由对应edit的draw完成
#define EDIT_X		145
#define EDIT_Y		12
#define EDIT_W		40
#define EDIT_H		16

#define	GSM_X		186
#define GSM_Y		7
#define GSM_W		31
#define GSM_H		13

#define	WIFI_X		186
#define WIFI_Y		7
#define WIFI_W		31
#define WIFI_H		13

#define BAT_X		219
#define BAT_Y		0
#define BAT_W		21
#define BAT_H		11

#define INPUT_X		219
#define INPUT_Y		14
#define INPUT_W		20
#define INPUT_H		10


#define	GSMWIFI_X1		186
#define GSMWIFI_Y1		0
#define GSMWIFI_X2		240
#define GSMWIFI_Y2		26

extern tGRAPLABEL *pgtComFirstChd;




/*---------------------------------------------------------
*函数: ComWidget_CreatAllObj
*功能: 创建公共的电池对象链表      
*参数: none
*返回: 第一个公共控件的指针
*---------------------------------------------------------*/
tGRAPLABEL *ComWidget_CreatAllObj(void);


/*---------------------------------------------------------
*函数: ComWidget_DeleteAllObj
*功能: 释放公共对象链表,只有在关机时才真正使用这个函数来释放所有公共对象      
*参数: none
*返回: 第一个公共控件的指针
*---------------------------------------------------------*/
void ComWidget_DeleteAllObj(void *pView);

/*---------------------------------------------------------
*函数: ComWidget_Link2Desk
*功能: 把公共空件连接到desk上      
*参数: pDesk：要连接的desk指针 pFirstCom:第一个控件的指针
*返回: 
*---------------------------------------------------------*/
void ComWidget_Link2Desk(void *pDesk,void *pFirstCom);

/*---------------------------------------------------------
*函数: ComWidget_Link2Desk
*功能: 把公共空件连接到desk上      
*参数: pDesk：要连接的desk指针 pFirstCom:第一个控件的指针 TotalChild:脱离窗体控件个数
*返回: 
*---------------------------------------------------------*/
void ComWidget_LeftFromDesk(void *pDesk,void *pFirstCom,U32 TotalChild);

/*---------------------------------------------------------
*函数: ComWidget_CheckObjAttar
*功能: 检测信号和电话等公共图标，并更新     
*参数: pDesk 窗体指针
*返回: 
*---------------------------------------------------------*/
void ComWidget_CheckObjAttar(void *pView);







#endif