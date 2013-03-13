#include "glbVariable.h"


/*---------------------------------------------------------
*函数: Remind_Fun
*功能: 提醒方式    
*参数: type:0:提醒   1:按键   2:扫描
*返回: 
*---------------------------------------------------------*/
void Remind_Fun(U8 soundType, U8 type)
{
	switch(type)
	{
	case 0:
		if(gtHyc.RemindType)
		{
			beeper_Start(soundType,gtHyc.RemindType,0);
		}
		break;
	case 1:
		if(gtHyc.KeySound)
		{
			beeper_Start(soundType,gtHyc.KeySound,0);
		}
		break;
	case 2:
		beeper_Start(soundType,gtHyc.ScanSound,1);
		break;
	default:
		break;
	}
	
	
	
	//beeper  led
	/*if((gtHyc.RemindType&0x01) || (gtHyc.RemindType&0x04))
	{
		beeper_Start(soundType,(gtHyc.RemindType&0x01),(gtHyc.RemindType&0x04));
	}
	//震动
	if(gtHyc.RemindType&0x02)
	{
		VibratorCtrl_Start(160);
	}*/
}
