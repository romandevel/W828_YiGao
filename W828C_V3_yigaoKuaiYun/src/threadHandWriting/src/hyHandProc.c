
#include "hyTypes.h"
#include "ospriority.h"
#include "std.h"
#include "sys.h"
#include "hyHandProc.h"
#include "appControlProcess.h"



/*-----------------------------------------------------------------------------
* 函数:	hyHWRecog_ThreadMethod(void);
* 功能:	手写识别驱动进程
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
int hyHWRecog_ThreadMethod(void)
{
	HandWriting_InitMemory();
	
	if(-1 == hyHWRecog_Init())
	{
		//hyUsbPrintf("init fail\r\n");
		while(1);
	}
	else
	{
		//hyUsbPrintf("init success\r\n");
	}
	
	hyHWRecog_RecognizeMain();
	
	return 0;
}

/*-----------------------------------------------------------------------------
* 函数:	hyHWRecog_CreatThread(void);
* 功能:	启动手写识别进程
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
S32  hyHWRecog_CreatThread(void)
{
	
    S32 pid, ret = -1;
    
    pid = ntopid(HANDWRITING_NAME);
    
    if (pid > 0)
    {
        if (PSPROC(pid)->state == P_INACT)
        {
            ret = 0;
        }
        else
        {
            return -1;
        }
    }
    if (ret != 0)
    {
        pid = createp((int (*)())hyHWRecog_ThreadMethod, 
                                HANDWRITING_NAME, 
                                HANDWRITING_SIZE, 
                                HANDWRITING_PRIOPITY, 
                                NULL, NULL, NULL);
    }
    pstart(pid, (void *)hyHWRecog_ThreadMethod);
    
    return ret;
}