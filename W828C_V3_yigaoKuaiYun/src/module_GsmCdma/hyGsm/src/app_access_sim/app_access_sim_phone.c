#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"

#include "usock.h"
//#include "ts0705.h"
#include "app_access_sim.h"
#include "app_shell.h"


u_int32_t appLockAccessSIM = 0;

extern int gsmcdma_ph_detect_simcard;
extern int gsmcdma_ph_simcard;


/*-----------------------------------------------------------------------------
* 函数:	phone_access_SIM
* 功能:	
* 参数:	
* 返回:	0:OK
*----------------------------------------------------------------------------*/
int phone_access_SIM(accessPhone_u *u, u_int8_t subtype)
{
	int		brk,fd,len,rc = -1;
	char 	*payload;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;

	
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
	case GSMD_PHONE_GET_IMSI:
		lgsm_get_imsi(lh);
		break;

	case GSMD_PHONE_GET_SERIAL:
		lgsm_get_serial(lh);
		break;

	case GSMD_PHONE_POWER:
		lgsm_phone_power(lh, u->cfun.funMode);
		break;

	case GSMD_PHONE_DETECT_SIMCARD:
		lgsm_detect_simcard(lh);
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
				
			if (GSMD_MSG_PHONE != gmh->msg_type)
				break;
			
			switch (gmh->msg_subtype)
			{
			case GSMD_PHONE_GET_IMSI:	//IMSI
			case GSMD_PHONE_GET_SERIAL:	//IMEI
				payload = (char *)((char *)gmh + sizeof(*gmh));
				if(isdigit(payload[0]))
				{
					strlcpy(u->simSerial, payload, sizeof(u->simSerial));
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_PHONE_POWER:
				rc = *(int *) gmh->data;

				brk = 1;
				break;

			case GSMD_PHONE_DETECT_SIMCARD:
				payload = (char *)((char *)gmh + sizeof(*gmh));
				if('0'==payload[0] || '1'==payload[0])
				{
					u->simDetect = (int)(payload[0] - '0');
					rc = 0;
				}
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



/*-----------------------------------------------------------------------------
* 函数:	phone_detect_simcard
* 功能:	检测是否有SIM卡，AT+ESIMS – Query SIM Status
* 参数:	
* 返回:	1:有卡	0:无卡	-1:检测失败
*----------------------------------------------------------------------------*/
int phone_detect_simcard(void)
{
	int rc=-1;
	accessPhone_u v;

	if(0 == phone_access_SIM(&v, GSMD_PHONE_DETECT_SIMCARD))
	{
		rc = v.simDetect;
	}

	/*
	rc = pin_check_simcard();
	if(0 == rc)
		rc = 1;
	else if(-2 == rc)
		rc = 0;
	*/
	
	return rc;
}



/*-----------------------------------------------------------------------------
* 函数:	phone_detect_simcard_nrt
* 功能:	非实时检测是否有SIM卡(可在要求不等待且对所读的值不要求实时值的地方调用)
* 参数:	
* 返回:	1:有卡	0:无卡
*----------------------------------------------------------------------------*/
int phone_detect_simcard_nrt(void)
{
	gsmcdma_ph_detect_simcard = 1;
	return gsmcdma_ph_simcard;
}

/*-----------------------------------------------------------------------------
* 函数:	phone_onlyRead_simcardStatus
* 功能:	只读取sim卡是否存在的状态，不引起实际sim卡检测
* 参数:	
* 返回:	1:有卡	0:无卡
*----------------------------------------------------------------------------*/
int phone_onlyRead_simcardStatus(void)
{
	return gsmcdma_ph_simcard;
}


/*-----------------------------------------------------------------------------
* 函数:	phone_get_sim_imsi
* 功能:	获取SIM卡的IMSI(International Mobile Subscriber Identity),即国际移动用
*		户识别码
* 参数:	accessPhone_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_sim_imsi(accessPhone_u *v)
{
	return phone_access_SIM(v, GSMD_PHONE_GET_IMSI);
}



/*-----------------------------------------------------------------------------
* 函数:	phone_get_IMEI
* 功能:	获取模块的IMEI(international mobile equipment identifier),即国际移动设
*		备识别码
* 参数:	accessPhone_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_IMEI(accessPhone_u *v)
{
	return phone_access_SIM(v, GSMD_PHONE_GET_SERIAL);
}



/*-----------------------------------------------------------------------------
* 函数:	phone_set_fun_mode
* 功能:	设置GSM模块的功能模式
* 参数:	funMode  ---  1:正常模式  0:飞行模式
* 返回:	0:SUCCESS	other:FAIL
*----------------------------------------------------------------------------*/
int phone_set_fun_mode(int funMode)
{
	int rc;

#ifdef GSM_MODULE_ULC2

	if(1 == funMode)
	{
		accessNet_u	v;

		memset(v.operName, 0, sizeof(v.operName));
		rc = net_register_operator(&v);

		//ERR_SIM_WRONG				SIM卡错误，该SIM不能注册
		//ERR_SIM_NOT_INSERTED		没有检测到SIM卡
		if(ERR_SIM_WRONG==rc || ERR_SIM_NOT_INSERTED==rc)
			rc = 0;
	}
	else
	{
		rc = net_deregister_operator();

		//ERR_SIM_NOT_INSERTED		没有检测到SIM卡
		if(ERR_SIM_NOT_INSERTED == rc)
		{
			accessPhone_u v;
			
			//silent reset (reset MS without resetting the SIM).
			v.cfun.funMode = 15;
			rc = phone_access_SIM(&v, GSMD_PHONE_POWER);
		}
	}

#elif defined GSM_MODULE_MD231

	accessPhone_u v;

	if(1 == funMode)
		v.cfun.funMode = 1;
	else
		v.cfun.funMode = 4;

	rc = phone_access_SIM(&v, GSMD_PHONE_POWER);
#endif

	return rc;
}



/*-----------------------------------------------------------------------------
* 函数:	phone_get_fun_mode
* 功能:	获得GSM模块当前的功能模式
* 参数:	
* 返回:	1:正常模式  0:飞行模式
*----------------------------------------------------------------------------*/
/*
int phone_get_fun_mode(void)
{
	return gsmcdma_fun_mode;
}
*/
