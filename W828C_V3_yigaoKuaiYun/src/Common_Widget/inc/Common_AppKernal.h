#ifndef _COMMON_APPKERNAL_H_
#define _COMMON_APPKERNAL_H_

typedef struct _tCARTOONPARA
{
	int		datalen;
	char	*pdata;

}tCARTOONPARA;

/*--------------------------------------------------------
 * 函数 ；kernal_app_complete
 * 功能 : 判断内核进程是否完成
 * 返回 : 其他：完成 0xff：未完成 
 *-------------------------------------------------------*/
int kernal_app_get_stat(void);

/*--------------------------------------------------------
 * 函数 ；kernal_app_config_entry
 * 功能 : 注册进程运行函数
 * 参数 : 函数指针
 * 返回 : none 
 *-------------------------------------------------------*/
void kernal_app_config_entry(void (*process)(void));

/*--------------------------------------------------------
 * 函数 ；Kernal_App_ThreadMethod
 * 功能 : 应用进程的内核进程
 * 返回 : 1 
 *-------------------------------------------------------*/
int Kernal_App_ThreadMethod(void);

#endif