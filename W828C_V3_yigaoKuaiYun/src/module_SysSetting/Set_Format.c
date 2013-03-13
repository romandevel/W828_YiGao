#include "SysSetting.h"


/*---------------------------------------------
*函数:Set_Format
*功能:格式化
*参数:
*返回:
*-----------------------------------------------*/
void Set_Format(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("设备中文件将被清空,是否继续?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		AbstractFileSystem_Format("C");
		
		Com_SpcDlgDeskbox("格式化完成!",0,pDesk,1,NULL,NULL,100);
	}
}
