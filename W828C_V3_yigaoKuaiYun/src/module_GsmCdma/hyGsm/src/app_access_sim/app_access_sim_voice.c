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

#include "voicecall.h"


extern u_int32_t appLockAccessSIM;



/*-----------------------------------------------------------------------------
* 函数:	voice_access_SIM
* 功能:	
* 参数:	
* 返回:	0:OK
*----------------------------------------------------------------------------*/
int voice_access_SIM(accessVoice_u *u, u_int8_t subtype)
{
	int		brk,fd,len,intresult,rc = -1;
	char 	*payload;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	struct  gsmd_call_fwd_stat *gcfs;


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
	case GSMD_VOICECALL_FWD_DIS:
		lgsm_voice_fwd_disable(lh, (enum lgsmd_voicecall_fwd_reason)u->call_fwd.reason);
		break;

	case GSMD_VOICECALL_FWD_EN:
		lgsm_voice_fwd_enable(lh, (enum lgsmd_voicecall_fwd_reason)u->call_fwd.reason);
		break;

	case GSMD_VOICECALL_FWD_STAT:
		lgsm_voice_fwd_stat(lh, (enum lgsmd_voicecall_fwd_reason)u->call_fwd.reason);
		break;
		
	case GSMD_VOICECALL_FWD_REG:
		{
		struct lgsm_voicecall_fwd_reg fwd_reg;
		
		fwd_reg.reason = (enum lgsmd_voicecall_fwd_reason)u->call_fwd.reason;
		if('+' == u->call_fwd.number[0])
			fwd_reg.number.type = LGSM_ATYPE_ISDN_INTL;
		else
			fwd_reg.number.type = LGSM_ATYPE_ISDN_OTHE;
		strlcpy(fwd_reg.number.addr, u->call_fwd.number, GSMD_ADDR_MAXLEN+1);
		lgsm_voice_fwd_reg(lh, &fwd_reg);
		}
		break;

	case GSMD_VOICECALL_FWD_ERAS:
		lgsm_voice_fwd_erase(lh, (enum lgsmd_voicecall_fwd_reason)u->call_fwd.reason);
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

			if (GSMD_MSG_VOICECALL != gmh->msg_type)
				break;

			switch (gmh->msg_subtype)
			{
			case GSMD_VOICECALL_FWD_DIS:
				intresult = *(int *)((char *)gmh + sizeof(*gmh));
				if(0 == intresult)
					rc = 0;
				brk = 1;
				break;

			case GSMD_VOICECALL_FWD_EN:
				intresult = *(int *)((char *)gmh + sizeof(*gmh));
				if(0 == intresult)
					rc = 0;
				brk = 1;
				break;

			case GSMD_VOICECALL_FWD_STAT:
				gcfs = (struct  gsmd_call_fwd_stat*) ((char *)gmh + sizeof(*gmh));
				
				if(1 == gcfs->is_last)
				{
					//目前只考虑查询以下条件转移的状态:UNCOND、BUSY、NO REPLY、NOT REACHABLE
					u->call_fwd.status = (u_int8_t)gcfs->status;
					u->call_fwd.classx = (u_int8_t)gcfs->classx;
					strlcpy(u->call_fwd.number, gcfs->addr.number, GSMD_ADDR_MAXLEN+1);
					u->call_fwd.time = gcfs->time;
					
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_VOICECALL_FWD_REG:
				intresult = *(int *)((char *)gmh + sizeof(*gmh));
				if(0 == intresult)
					rc = 0;
				brk = 1;
				break;

			case GSMD_VOICECALL_FWD_ERAS:
				intresult = *(int *)((char *)gmh + sizeof(*gmh));
				if(0 == intresult)
					rc = 0;
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
* 函数:	voice_call_fwd_dis
* 功能:	
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_dis(accessVoice_u *v)
{
	return voice_access_SIM(v, GSMD_VOICECALL_FWD_DIS);
}


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_en
* 功能:	
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_en(accessVoice_u *v)
{
	return voice_access_SIM(v, GSMD_VOICECALL_FWD_EN);
}


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_stat
* 功能:	
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_stat(accessVoice_u *v)
{
	return voice_access_SIM(v, GSMD_VOICECALL_FWD_STAT);
}


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_reg
* 功能:	
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_reg(accessVoice_u *v)
{
	return voice_access_SIM(v, GSMD_VOICECALL_FWD_REG);
}


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_eras
* 功能:	
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_eras(accessVoice_u *v)
{
	return voice_access_SIM(v, GSMD_VOICECALL_FWD_ERAS);
}


