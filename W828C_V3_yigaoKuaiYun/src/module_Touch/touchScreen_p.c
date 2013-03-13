/*
*include standard files
*/
#include "ospriority.h"
#include "appControlProcess.h"

int touchScreen_ThreadMethod(void)
{
     hyhwAk4183_Initial();
     
     ts_main();
}

/*-----------------------------------------------------------------------------
* 函数:	touchscreen_create_thread(void);
* 功能:	启动触摸驱动进程
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void touchscreen_create_thread(void)
{
    int pid;
    pid = createp( touchScreen_ThreadMethod, TOUCH_NAME, TOUCH_SIZE, TOUCH_PRIOPITY, NULL,NULL,NULL); // 进程优先级由 6 --> 12
	pstart( pid, (void *)touchScreen_ThreadMethod);
}
