#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"

#include "usock.h"
//#include "ts0705.h"
#include "app_access_sim.h"
#include "phonebook.h"


extern int gsmcdma_net_query_reg;
extern int gsmcdma_net_query_sigq;
extern int gsmcdma_net_reg_stat;
extern u_int32_t gsmcdma_net_sigq;


/*-----------------------------------------------------------------------------
* 函数:	net_access_SIM
* 功能:	
* 参数:	
* 返回:	0:SUCCESS 	-1:FAIL
*----------------------------------------------------------------------------*/
int net_access_SIM(accessNet_u *u, u_int8_t subtype)
{
	int		brk,fd,len,readNum=0,rc = -1;
	char 	*payload;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	enum gsmd_netreg_state state;
	struct gsmd_own_number *num;
	struct gsmd_phonebook *gp;


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
	case GSMD_NETWORK_QUERY_REG:
		lgsm_netreg_query(lh);
		break;
		
	case GSMD_NETWORK_GET_NUMBER:
		lgsm_get_subscriber_num(lh);
		break;

	case GSMD_NETWORK_SET_NUMBER:
	
#ifdef GSM_MODULE_ULC2
		{
		int i,j;
		char name[GSMD_PB_TEXT_MAXLEN+1];
		struct lgsm_phonebook phb;
		
		phb.index = u->rwNode.index;
		memcpy(phb.numb, u->rwNode.number, sizeof(phb.numb));
		phb.numb[sizeof(phb.numb)-1] = '\0';
		if ('+' == phb.numb[0])
			phb.type = LGSM_PB_ATYPE_INTL;
		else
			phb.type = LGSM_PB_ATYPE_OTHE;
		//memcpy(phb.text, u->rwNode.name, sizeof(phb.text));
		//phb.text[sizeof(phb.text)-1] = '\0';
		
		//超过6个字截断
		memcpy(name, u->rwNode.name, sizeof(name));
		for(i=0,j=0;j<6 && name[i]!=0;j++)
		{
			if(name[i] >= 0x80)
				i+=2;
			else
				i++;
		}
		name[i] = 0;
		Hyc_Gbk2Unicode(name, phb.text, LITTLE_ENDIAN);

		lgsm_set_subscriber_num(lh,	&phb);
		}

#elif defined GSM_MODULE_MD231

		{
		int i,j;
		char *name;
		struct lgsm_phonebook phb;

		phb.index = u->rwNode.index - 1;
		memcpy(phb.numb, u->rwNode.number, sizeof(phb.numb));
		phb.numb[sizeof(phb.numb)-1] = '\0';
		
		i=0;
		name = u->rwNode.name;
		while(i<14)
		{
			if(name[i] >= 0x80)
				break;
			
			i++;
		}

		if(14 == i)
		{
			phb.type = 0;	//不带汉字的GBK编码
			
			memcpy(phb.text, u->rwNode.name, sizeof(phb.text));
			phb.text[sizeof(phb.text)-1] = '\0';
		}
		else
		{
			phb.type = 1;	//带有汉字的GBK编码
			
			//超过6个字截断
			name = u->rwNode.name;
			for(i=0,j=0;j<6 && name[i]!=0;j++)
			{
				if(name[i] >= 0x80)
					i+=2;
				else
					i++;
			}
			name[i] = 0;
			Hyc_Gbk2Unicode(u->rwNode.name, phb.text, LITTLE_ENDIAN);
		}
		lgsm_set_subscriber_num(lh,	&phb);
		}
#endif

		break;

	case GSMD_NETWORK_OPER_GET:
		lgsm_oper_get(lh);
		memset(u->operName, 0, sizeof(u->operName));
		break;

	case GSMD_NETWORK_REGISTER:
		{
		gsmd_oper_numeric oper;
		
		memcpy(oper, u->operName, sizeof(gsmd_oper_numeric));
		lgsm_netreg_register(lh, oper);
		}
		break;

	case GSMD_NETWORK_DEREGISTER:
		lgsm_netreg_deregister(lh);
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
				
			if (GSMD_MSG_NETWORK != gmh->msg_type)
				break;

			switch (gmh->msg_subtype)
			{
			case GSMD_NETWORK_QUERY_REG:
				state = *(enum gsmd_netreg_state *) gmh->data;
				rc = 0;
				
				switch (state) 
				{
				case GSMD_NETREG_UNREG:
				case GSMD_NETREG_DENIED:
					u->netreg_stat = -1;	//未注册
					break;
				case GSMD_NETREG_REG_HOME:
				case GSMD_NETREG_REG_ROAMING:
					u->netreg_stat = 1;		//已注册
					break;
				case GSMD_NETREG_UNREG_BUSY:
					u->netreg_stat = 0;		//正在注册
					break;

				default:
					rc = -1;
					break;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_GET_NUMBER:
				num = (struct gsmd_own_number *)gmh->data;
				if(num->is_last >= 0)
				{
					gp = (struct gsmd_phonebook *)(u->ownNumber.allNode+readNum);
					readNum++;
					
					strlcpy(gp->numb, num->addr.number, GSMD_PB_NUMB_MAXLEN+1);
					Hyc_Unicode2Gbk(num->name, gp->text, BIG_ENDIAN);
					gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';

					if(1 == num->is_last)
					{
						u->ownNumber.total = readNum;
					
						rc = 0;
						brk = 1;
					}
				}
				else
				{
					if(-1 == num->is_last)
					{
						//没有记录(total为0)
						u->ownNumber.total = 0;
					
						rc = 0;
					}
					brk = 1;
				}
				break;

			case GSMD_NETWORK_SET_NUMBER:
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && !strncmp(payload, "OK", 2))
				{
					//DEBUGP("%s\n", payload);
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_OPER_GET:
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && strlen(payload)>0)
				{
					//DEBUGP("%s\n", payload);
					strncpy(u->operName, payload, sizeof(u->operName)-1);
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_REGISTER:
				rc = *(int *) gmh->data;
				
				//+CME ERROR: 10	SIM not inserted
				//+CME ERROR: 15	SIM wrong
				brk = 1;
				break;

			case GSMD_NETWORK_DEREGISTER:
				rc = *(int *) gmh->data;

				//+CME ERROR: 10	SIM not inserted
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
* 函数:	net_query_reg
* 功能:	实时查询GSM/CDMA注册状态(注册状态由accessNet_u结构中的netreg_stat带回，
*		netreg_stat  ---  -1:未注册  0:正在注册   1:已注册)
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_query_reg(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_QUERY_REG);
}




/*-----------------------------------------------------------------------------------
* 函数:	net_query_reg_nrt
* 功能:	非实时查询GSM/CDMA注册状态(可在要求不等待且对所读的值不要求实时值的地方调用)
*		
* 参数:	
* 返回:	-1:未注册  0:正在注册   1:已注册
*-----------------------------------------------------------------------------------*/
int net_query_reg_nrt(void)
{
	gsmcdma_net_query_reg = 1;
	if(get_modem_init_stat() == 1)
	{
		return gsmcdma_net_reg_stat;
	}
	else
	{
		return 0;
	}
}
/*-----------------------------------------------------------------------------------
* 函数:	net_onlyRead_regStatus
* 功能:	只读取是否注册上网络的状态，不引起实际检测
* 参数:	
* 返回:	-1:未注册  0:正在注册   1:已注册
*-----------------------------------------------------------------------------------*/
int net_onlyRead_regStatus(void)
{
	if(get_modem_init_stat() == 1)
	{
		return gsmcdma_net_reg_stat;
	}
	else
	{
		return 0;
	}
}


/*-----------------------------------------------------------------------------------
* 函数:	net_query_sigq_set
* 功能:	非实时查询GSM/CDMA信号强度(可在要求不等待且对所读的值不要求实时值的地方调用)
*		
* 参数:	
* 返回:	信号强度 0-31,99
*-----------------------------------------------------------------------------------*/
u_int32_t net_query_sigq_nrt(void)
{
	gsmcdma_net_query_sigq = 1;
	return gsmcdma_net_sigq;
}



/*-----------------------------------------------------------------------------
* 函数:	net_get_subscriber_num
* 功能:	查询本机号码
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_get_subscriber_num(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_GET_NUMBER);
}



/*-----------------------------------------------------------------------------
* 函数:	net_set_subscriber_num
* 功能:	设置本机号码
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(设置成功)	-1:FAIL(设置失败)
*----------------------------------------------------------------------------*/
int net_set_subscriber_num(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_SET_NUMBER);
}



/*-----------------------------------------------------------------------------
* 函数:	net_get_operator
* 功能:	查询当前网络运营商
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_get_operator(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_OPER_GET);
}



/*-----------------------------------------------------------------------------
* 函数:	net_register_operator
* 功能:	向网络运营商注册
*		
* 参数:	accessNet_u *v
* 返回:	0:						SUCCESS(注册成功)
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡
		ERR_SIM_WRONG:			SIM卡错误，该SIM不能注册
		others:					未知错误
*----------------------------------------------------------------------------*/
int net_register_operator(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_REGISTER);
}



/*-----------------------------------------------------------------------------
* 函数:	net_deregister_operator
* 功能:	从网络上注销注册
*		
* 参数:	accessNet_u *v
* 返回:	0:						SUCCESS(注销成功)
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡
		others:					未知错误
*----------------------------------------------------------------------------*/
int net_deregister_operator(void)
{
	return net_access_SIM(NULL, GSMD_NETWORK_DEREGISTER);
}



