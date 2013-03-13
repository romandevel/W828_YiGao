
#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"
#include "app_access_sim.h"



/*-----------------------------------------------------------------------------
* 函数:	security_access_SIM
* 功能:	
* 参数:	
* 返回:	0:SUCCESS
*----------------------------------------------------------------------------*/
int security_access_SIM(accessSecurity_u *u, u_int8_t subtype)
{
	int		brk,fd,len,result,rc = -1;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	struct gsmd_pin_remain_times *gprt;


	//后面的代码非可重入，所以要lock
	(void)wlock((unsigned)(&appLockAccessSIM));
	
	lh = lgsm_init(USER_OTHER);
	if (!lh)
	{
		(void)unlock((unsigned)(&appLockAccessSIM));
		return -1;	//Can't connect to server thread
	}

	fd = lgsm_fd(lh);
	brk = 0;


	switch(subtype)
	{
	case GSMD_SECURITY_PIN_GET_STATUS:
		lgsm_security_pin_status(lh);
		break;
	case GSMD_SECURITY_PIN_INPUT:
		{
		struct gsmd_pin gp;
		
		gp.type = (enum gsmd_pin_type)u->pin.type;
		strncpy(gp.pin, u->pin.pin, sizeof(gp.pin));
		strncpy(gp.newpin, u->pin.newpin, sizeof(gp.newpin));
		lgsm_security_pin_input(lh, &gp);
		}
		break;
	case GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES:
		lgsm_security_pin_remain_times(lh);
		break;
	case GSMD_SECURITY_LOCK_EN:
	case GSMD_SECURITY_LOCK_DIS:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		strncpy(gfac.pwd, u->lock.pwd, sizeof(gfac.pwd));
		if(GSMD_SECURITY_LOCK_EN == subtype)
			lgsm_security_lock_en(lh, &gfac);
		else
			lgsm_security_lock_dis(lh, &gfac);
		}
		break;
	case GSMD_SECURITY_LOCK_GET_STATUS:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		lgsm_security_lock_status(lh, &gfac);
		}
		break;
	case GSMD_SECURITY_CHANGE_PWD:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		strncpy(gfac.pwd, u->lock.pwd, sizeof(gfac.pwd));
		strncpy(gfac.newpwd, u->lock.newpwd, sizeof(gfac.newpwd));
		lgsm_security_change_pwd(lh, &gfac);
		}
		break;
		
	}


	while (1)
	{
		//receive data on the gsmd socket
		len = u_recv(fd, buf, sizeof(buf), 0);
		if(0<len)
		{
			/* we've received something on the gsmd socket, pass it
			 * on to the library */
			if (len < sizeof(*gmh))
				break;

			gmh = (struct gsmd_msg_hdr *) buf;
			if (len - sizeof(*gmh) < gmh->len)
				break;
				
			if (GSMD_MSG_SECURITY != gmh->msg_type)
				break;
			
			switch (gmh->msg_subtype)
			{
			case GSMD_SECURITY_PIN_GET_STATUS:
				result = *(int *) gmh->data;
				if(result >= 0)
				{
					u->pin.type = (enum pin_type)result;
					rc = 0;
				}
				else
				{
					//+CME ERROR: 10	SIM not inserted
					rc = result;
				}
				
				brk = 1;
				break;
			case GSMD_SECURITY_PIN_INPUT:
				rc = *(int *) gmh->data;

				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				//+CME ERROR: 18	SIM PUK2 required
				brk = 1;
				break;
			case GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES:
				gprt = (struct gsmd_pin_remain_times *)gmh->data;
				if(gprt->pin >= 0)
				{
					u->pin_times.pin = gprt->pin;
					u->pin_times.pin2 = gprt->pin2;
					u->pin_times.puk = gprt->puk;
					u->pin_times.puk2 = gprt->puk2;
					
					rc = 0;
				}
				brk = 1;
				break;
				
			case GSMD_SECURITY_LOCK_EN:
			case GSMD_SECURITY_LOCK_DIS:
				rc = *(int *) gmh->data;

				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				brk = 1;
				break;
			case GSMD_SECURITY_LOCK_GET_STATUS:
				rc = *(int *) gmh->data;

				//+CME ERROR: 11	SIM PIN required
				//+CME ERROR: 12	SIM PUK required
				brk = 1;
				break;
			case GSMD_SECURITY_CHANGE_PWD:
				rc = *(int *) gmh->data;

				//+CME ERROR: 3		operation not allowed
				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				//+CME ERROR: 18	SIM PUK2 required
				brk = 1;
				break;

			default:
				brk = 1;
				break;
			}

			if(1 == brk)
				break;
		}
		else
		{
			sleep(1);
		}
	}
	lgsm_exit(lh);
	
	(void)unlock((unsigned)(&appLockAccessSIM));
	
	return rc;
}



/*---------------------------------------------------------------------------------
* 函数:	security_check_pin_status
* 功能:	查询SIM卡是否需要密码，函数返回值为0时v->pin.type将带回需要的密码类型，如果
		类型为PIN_READY，有两种情况：
		1)不再需要输入密码
		2)SIM卡PUK码已被锁死，通过查询PUK码可输入次数是否为0以确认SIM卡已被锁死
* 参数:	accessSecurity_u *v
* 返回:	0:						SUCCESS
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡   
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_check_pin_status(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_PIN_GET_STATUS);
}



/*---------------------------------------------------------------------------------
* 函数:	security_pin_input
* 功能:	输入SIM卡密码，函数返回值为0时密码正确
* 参数:	accessSecurity_u *v
* 返回:	0:						SUCCESS
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_SIM_FAILURE:		SIM操作失败，此时需要再查询PUK码可输入次数是否
								为0，如果为0说明SIM卡已被锁死
		ERR_INCORRECT_PASSWORD:	密码不正确
		ERR_SIM_PUK2_REQUIRED:	需要输入SIM卡PUK2码(输入PUK码时必须要同时输入新的PIN2码)
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_pin_input(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_PIN_INPUT);
}



/*---------------------------------------------------------------------------------
* 函数:	security_pin_remain_times
* 功能:	查询SIM卡密码可输入次数
* 参数:	accessSecurity_u *v
* 返回:	0:						查询成功
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_pin_remain_times(accessSecurity_u *v)
{
		return security_access_SIM(v,GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES);
}


/*---------------------------------------------------------------------------------
* 函数:	security_lock_en
* 功能:	启用设备锁定功能(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						启用锁定成功
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:	密码不正确
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_en(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_EN);
}


/*---------------------------------------------------------------------------------
* 函数:	security_lock_dis
* 功能:	解除设备锁定功能(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						解除锁锁定成功
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:	密码不正确
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_dis(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_DIS);
}


/*---------------------------------------------------------------------------------
* 函数:	security_lock_status
* 功能:	查询设备锁定状态(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						设备锁定未启用
		1:						设备锁定已启用
		ERR_SIM_PIN_REQUIRED:	需要输入SIM卡PIN码(开机时)
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_SIM_FAILURE:		SIM操作失败，此时需要再查询PUK码可输入次数是否
								为0，如果为0说明SIM卡已被锁死
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_status(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_GET_STATUS);
}


/*---------------------------------------------------------------------------------
* 函数:	security_change_pwd
* 功能:	修改密码(如PIN码等)
* 参数:	accessSecurity_u *v
* 返回:	0:							修改密码成功
		ERR_OPERATION_NOT_ALLOWED:	操作不允许(可能设备锁定未启用)
		ERR_SIM_PUK_REQUIRED:		需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:		密码不正确
		ERR_SIM_PUK2_REQUIRED:		需要输入SIM卡PUK2码(输入PUK码时必须要同时输入新的PIN2码)
		others:						未知错误
*---------------------------------------------------------------------------------*/
int security_change_pwd(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_CHANGE_PWD);
}


