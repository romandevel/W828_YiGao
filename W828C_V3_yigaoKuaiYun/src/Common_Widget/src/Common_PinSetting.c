#include "grap_api.h"
#include "Common_Dlg.h"
#include "glbVariable.h"
#include "app_access_sim.h"

/////////////////////////////////////////////////////////////
#define NOTHING_TO_DO			0
#define CHECK_IN				1
#define BEYOUND_COUNT			2
#define CHECK_WRONG				3


static U8 PINCode[] = "1234";
static U8 PIN2Code[] = "0000";
static U8 checkPINState = 0;	//0:关	1:修改

U8 *getPINCode()
{
	return PINCode;
}


/*---------------------------------------------------------
*函数: Common_InputPin
*功能: 输入PIN码
*参数: 
*返回: 
*---------------------------------------------------------*/
/*U32 Common_InputPin(void *pDeskFather, U8 enter_mode)
{
	tGRAPDESKBOX	*pDesk;	
	tGRAPBUTTON		*pButton;
	tGRAPEDIT		*pEdit;
	int				i,index1 = 0;
	
	pDesk = Grap_CreateDeskbox(&gtSettingSIMViewAttr[index1++], 0);
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	for (i = 0; i < 4; i++)
	{
		pButton = Grap_InsertButton(pDesk,&gtSettingSIMViewAttr[index1++],&SettingSIM_ButnPriv[i],NULL);
		if (pButton == NULL)  return ;
		if(i < 2)
			pButton->pressEnter = SettingSIM_BtnEnter;
	}
	
	
}*/


/*---------------------------------------------------------
*函数: Common_CheckPin
*功能: 开机时调用, 进行PIN码判定及解锁.   
*参数: 
*返回: 0: SUCCESS
*	   -1: SIM卡死锁
*---------------------------------------------------------*/
int Common_CheckPin(void *pDeskFather)
{
	accessSecurity_u v;
	int ret, i = 10;
	
	
	while(i--)
	{
		ret = security_check_pin_status(&v);
	//PhoneTrace(0,"ret == %d",ret);
		if (ret == ERR_SIM_NOT_INSERTED)
			return 0;
		else if (ret == 0)
		{
			switch(v.pin.type)
			{
			case PIN_READY:
				memset( &v, 0, sizeof(accessSecurity_u) );
			//PhoneTrace(0,"pin == get");
				ret = security_pin_remain_times(&v);
			//PhoneTrace(0,"pin ret == %d",ret);
				if (v.pin_times.pin <= 0 && v.pin_times.puk <= 0)
				{
					Com_SpcDlgDeskbox("SIM卡永久锁死,请换卡!",6,pDeskFather,2,NULL,NULL,300);
					return -1 ;
				}
				else
					return 0;
				break;
			case PIN_SIM_PIN:
				Com_SpcDlgDeskbox("该卡设有PIN码,需解锁才能使用!",6,pDeskFather,2,NULL,NULL,300);
				return -1;
				/*ret = Common_InputPin(pDeskFather,2);
				if (ret == CHECK_IN)
				{
					return 0;
				}
				else if (ret == BEYOUND_COUNT)
				{
					Com_DlgDeskbox(pDeskFather, "PIN码锁死,请使用PUK码!", "确定", NULL, 3, 500);
					return -1;
					ret = PUK_check_desk(pDeskFather);
					if (ret == CHECK_IN)
					{
						New_PINCode_desk(pDeskFather);
						setPINCodeState(0);
						return 0;
					}
					else if (ret == BEYOUND_COUNT)
					{
						//Com_DlgDeskbox(pDeskFather, 2, cDIALOG_PINDIED_SET, NULL, NULL, 200);
					}
				}*/
				break;
			case PIN_SIM_PUK:
				Com_SpcDlgDeskbox("该卡设有PIN码,需解锁才能使用!",6,pDeskFather,2,NULL,NULL,300);
				return -1;
			/*	ret = PUK_check_desk(pDeskFather);
				if (ret == CHECK_IN)
				{
					New_PINCode_desk(pDeskFather);
					setPINCodeState(0);
					return 0;
				}
				else if (ret == BEYOUND_COUNT)
				{
					//Com_DlgDeskbox(pDeskFather, 2, cDIALOG_PINDIED_SET, NULL, NULL, 200);
				}
				break;*/
			}
			break;
		}
		else
		{
			sleep(20);
		}
	}
	
	//Com_DlgDeskbox(pDeskFather, 1, cDIALOG_PINCHECK_FAILED, NULL, NULL, DLG_ALLWAYS_SHOW);

	return -1;
}

//设置本机号码和名称
//0：成功  -1：失败
int Comm_SetMachineInfo(char *pNum, char *pName)
{
	int ret;
	accessNet_u v;
	
	v.rwNode.index = 1;
	v.rwNode.number= pNum;
	v.rwNode.name  = pName;
	
	ret = net_set_subscriber_num(&v);
	
	return ret;
}

//获取本机号码及名字
//0:成功  -1：失败
int CommGetMachineInfo(char *pNum, char *pName)
{
	int ret;
	
	accessNet_u v;
	
	v.rwNode.index = 1;
	v.rwNode.number= pNum;
	v.rwNode.name  = pName;
	ret = net_get_subscriber_num(&v);
	
	return ret;
}