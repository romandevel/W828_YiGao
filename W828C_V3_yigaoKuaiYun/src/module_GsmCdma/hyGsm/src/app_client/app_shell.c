/* libgsmd tool
 *
 * (C) 2006-2007 by OpenMoko, Inc.
 * Written by Harald Welte <laforge@openmoko.org>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */ 

#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "voicecall.h"
#include "misc.h"
#include "phonebook.h"
#include "libgsmdsms.h"
#include "pin.h"
#include "usock.h"
#include "ts0705.h"

//#include "sms.h"
#include "app_shell.h"
#include "Mailbox.h"
#include "semaphore.h"


//#include "PhoneDebug.h"

#define   LITTLE_ENDIAN 0 //小端编码方式
#define   BIG_ENDIAN    1 //大端编码方式


int pending_responses = 0;

int gsmcdma_net_query_reg = 0;
int gsmcdma_net_reg_stat = 0;

int gsmcdma_net_query_sigq = 0;
u_int32_t gsmcdma_net_sigq = 0;

int gsmcdma_ph_detect_simcard = 0;
int gsmcdma_ph_simcard = 0;
int reset_pb_state = 0;
int reset_sms_state = 0;

static unsigned char gsm_PINCode[6];

unsigned char gsm_SimCard[20],gsm_phoneName[20],gu8nameFlag;

int phb_sim_init_stat = -1;
static struct phb_sim_info phb_sim;
int sms_readed = 0;
MAILBOX   *pmail_phb_read = NULL;
SEMAPHORE *psema_phb_read = NULL;

static int next_init_step;

enum shell_init_step init_step;

u_int8_t unsol_sim_phb_ready = 0;	//+CSIM: PBK ready
u_int8_t unsol_sim_sms_ready = 0;	//+CSIM: SMS ready
u_int8_t unsol_modem_reset = 0;		//+EIND: 128

struct lgsm_sms_cnmi glsc;
int sms_index_max,sms_index,sms_total;

static int  reset_step_flag;
static enum reset_step cur_reset_step;

extern int gsmcdma_client_exit;
extern int mux_flag;
static int net_reg_state = 0;
int mdm_reg_stat = 0;


unsigned char *gsm_getIMSI()
{
	return phb_sim.simSerial;
}



void gsm_SetPinCode(unsigned char *pinCode)
{
	memset(gsm_PINCode,0,6);
	strcpy(gsm_PINCode,pinCode);
}

unsigned char *gsm_ReadSimCard()
{
	return gsm_SimCard;
}

unsigned char *gsm_Readgsm_phoneName(void)
{	
	if(1 == gu8nameFlag)
	{
		gu8nameFlag = 0 ;
		return gsm_phoneName;
	}
	
	return NULL ;
}

void gsm_WriteSimCard(unsigned char *pSim)
{
	memset(gsm_SimCard,0,20);
	strcpy(gsm_SimCard,pSim);
}

void init_variable(void)
{
	//PhoneTrace(0,"init!");
	
	mux_flag = 0;
	
	unsol_sim_phb_ready = 0;	
	unsol_sim_sms_ready = 0;
	pending_responses = 0;
	gsmcdma_net_query_reg = 1;//0;//
	gsmcdma_net_reg_stat = 0;

	gsmcdma_net_query_sigq = 1;//0;//
	gsmcdma_net_sigq = 0;

	gsmcdma_ph_simcard = 0;   
	phb_sim_init_stat = -1;
	gsmcdma_client_exit = 0;
	init_step = 0;
	sms_readed = 0;
	mdm_reg_stat = 0;
	net_reg_state = 0;
	
	modem_init_reset();
	memset(gsm_SimCard,0,20);
	memset(gsm_phoneName,0,20);
	gu8nameFlag = 0 ;
}
/* this is the handler for receiving passthrough responses */
static int pt_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	//char *payload = (char *)gmh + sizeof(*gmh);
	return 0;
}
void glsc_getSmsCnmi(struct lgsm_sms_cnmi* pLsc)
{
	if (pLsc != NULL)
	{
		memcpy(pLsc, &glsc, sizeof(struct lgsm_sms_cnmi));
	}
}
void glsc_setSmsCnmi(struct lgsm_sms_cnmi* pLsc)
{
	memcpy(&glsc, pLsc, sizeof(struct lgsm_sms_cnmi));
}

/*-----------------------------------------------------------------------------
* 函数:	smsCnmi_ds_get
* 功能:	获得短信设置的 ds 项
* 参数:	none
* 返回:	ds
*----------------------------------------------------------------------------*/
int smsCnmi_ds_get(void)
{
	return glsc.ds;
}

int phb_used = 0;
int phb_repeat = 0;
/* this is the handler for receiving phonebook responses */
static int pb_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	struct gsmd_phonebook_mem *gpm;
	struct gsmd_phonebooks *gps;
	struct gsmd_phonebook *gp;

	if(-1 != phb_sim_init_stat)
		return 0;

	switch (gmh->msg_subtype) {
	case GSMD_PHONEBOOK_GET_STORAGE:
		gpm = (struct gsmd_phonebook_mem *)((char *)gmh + sizeof(*gmh));
		if(gpm->total>0 && gpm->used>=0 && gpm->used<=gpm->total)
		{
			//DEBUGP("%s, %d, %d\n", gpm->type, gpm->used, gpm->total);
			if(init_step < INIT_STEP_NUM)
			{
				int size;

				phb_sim.used = gpm->used;
				phb_sim.total = gpm->total;

				//前2个为本机号码空间
				size = (phb_sim.total+2) * sizeof(struct gsmd_phonebook);
				phb_sim.allNode = (struct gsmd_phonebook *)MALLOC(size);
				if(!phb_sim.allNode)
					return -ENOMEM;
				memset(phb_sim.allNode, 0, size);

				if(phb_sim.used>0)
				{
					init_step = (enum shell_init_step)(init_step+1);
				}
				else
				{
					//电话本记录为空，读本机号码
					init_step = STEP_GET_OWN_NUMBER;
				}
			}
		}
		else
		{
			sleep(2);
		}
		if(init_step < INIT_STEP_NUM)
			next_init_step = 1;
		break;

	case GSMD_PHONEBOOK_READRG:
		if(init_step == STEP_LIST_PHB)
		{
		gps = (struct gsmd_phonebooks *) ((char *)gmh + sizeof(*gmh));
		if (gps->pb.index > 0)
		{
			if(gps->pb.index <= phb_sim.total)
			{
				gp = (struct gsmd_phonebook *)(phb_sim.allNode+2+(gps->pb.index-1));

				//memcpy(gp, &(gps->pb), sizeof(struct gsmd_phonebook));
				gp->index = gps->pb.index;
				strlcpy(gp->numb, gps->pb.numb, GSMD_PB_NUMB_MAXLEN+1);
				gp->type = gps->pb.type;
				Hyc_Unicode2Gbk(gps->pb.text, gp->text, BIG_ENDIAN);
				gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';

					phb_used++;
				}
			}
			else if(!gps->is_last)
			{
				phb_repeat = 1;
			next_init_step = 1;
		}

			if (gps->is_last)
			{
		
				//所有电话本记录读取完成
				if(init_step < INIT_STEP_NUM)
				{
					init_step = (enum shell_init_step)(init_step+1);
					next_init_step = 1;
				}	
			}
		}
		break;
	}

	return 0;
}



static struct sms_num_list init_sms_nl;

/* this is the handler for receiving sms responses */
static int sms_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	int *intresult;
	int i;
	struct gsmd_addr *addr;
	struct gsmd_sms_list *sms;
	struct gsmd_sms_cnmi *gsc;
	struct gsmd_sms_storage *mem;
	static const char *msgtype[] = {
		"Unread", "Received", "Unsent", "Sent"
	};
	static const char *memtype[] = {
		"Unknown", "Broadcast", "Me message", "MT", "SIM", "TA", "SR"
	};


	switch (gmh->msg_subtype) {
	case GSMD_SMS_LIST:
	case GSMD_SMS_READ:
		sms = (struct gsmd_sms_list *) ((char *) gmh + sizeof(*gmh));
		//if(sms->payload.is_voicemail)
		//DEBUGP("it's a voicemail \n");

		if(GSMD_SMS_LIST == gmh->msg_subtype)
		{
			if(sms->index<=init_sms_nl.total && init_sms_nl.num<init_sms_nl.used)
			memcpy(&(init_sms_nl.sms[init_sms_nl.num++]), sms, sizeof(struct gsmd_sms_list));

			if(sms->is_last)
			{
				//todo:save

				for( i = 0; i < init_sms_nl.num; i++ )
				{
					//Sms_smsStore(&init_sms_nl.sms[i],0);	

					lgsm_sms_delete(lh,init_sms_nl.sms[i].index );
				}

				
				FREE(init_sms_nl.sms);
				init_sms_nl.sms = NULL;
				init_sms_nl.num = 0;
				reset_sms_state = 1;
				sms_readed = 1;
				if(init_step < INIT_STEP_NUM)
				{
					init_step = (enum shell_init_step)(init_step+1);
					next_init_step = 1;
				}
			}
		}
		else //if(GSMD_SMS_READ==gmh->msg_subtype)
		{
			if(sms->index > 0)
			{
				//todo:save
				//Sms_smsStore(sms,0);
				lgsm_sms_delete(lh,sms->index );
//				newSmsFlag=1;

				//收到+CMTI后读取短信
				sms_index++;
				if(sms_index >= sms_total)
				{
					//读取完成，SIM卡已满，需要清空SIM卡
					lgsm_sms_delete_all(lh, LGSM_SMS_DELALL_ALL);
				}
			}
		}
		break;

	case GSMD_SMS_DELETE_ALL:
		intresult = (int *) ((char *) gmh + sizeof(*gmh));
		if(0 == *intresult)
		{
			sms_index_max = 0;
			sms_index = 0;

			if(init_step < INIT_STEP_NUM)
				init_step = (enum shell_init_step)(init_step+1);

			if(2 == gsmcdma_client_exit)
			{
				gsmcdma_client_exit = 3;
			}
		}
		else
		{
			if(2 == gsmcdma_client_exit)
			{
				gsmcdma_client_exit = 3;//1;
			}
		}				
		next_init_step = 1;
		break;

	case GSMD_SMS_GET_MSG_STORAGE:
		mem = (struct gsmd_sms_storage *)((char *) gmh + sizeof(*gmh));
		if(mem->mem[0].total>0 && mem->mem[0].used>=0 && mem->mem[0].used<=mem->mem[0].total)
		{
			
			if(init_step < INIT_STEP_NUM)
			{
				init_sms_nl.used = mem->mem[0].used;
				init_sms_nl.total = mem->mem[0].total;

				sms_total = init_sms_nl.total;
				
				if(mem->mem[0].used > 0)
				{
					int size;
					
					init_sms_nl.num = 0;
					
					size = mem->mem[0].used * sizeof(struct gsmd_sms_list);
					init_sms_nl.sms = (struct gsmd_sms_list *)MALLOC(size);
					if(NULL == init_sms_nl.sms)
						return -ENOMEM;
					memset(init_sms_nl.sms, 0, size);
				
					init_step = (enum shell_init_step)(init_step+1);
				}
				else
				{
					sms_readed = 1;
					init_step = STEP_NET_RE_REGISTER;
					next_init_step = 1;
				}
			}
		}

		if(init_step < INIT_STEP_NUM)
			next_init_step = 1;
		break;


	case GSMD_SMS_SET_NEW_MSG_INDICATIONS:
		intresult = (int *) ((char *) gmh + sizeof(*gmh));
		if(init_step == STEP_SET_NEW_SMS_IND)
		{
			if(0 == *intresult)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
		
		if(cur_reset_step == RESET_SET_NEW_SMS_IND)
		{
			if(0 == *intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}
		break;

	case GSMD_SMS_SET_FMT:
		intresult = (int *) ((char *) gmh + sizeof(*gmh));
		if(init_step == STEP_SET_SMS_FMT)
		{
			if(0 == *intresult)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
		
		if(cur_reset_step == RESET_SET_SMS_FMT)
		{
			if(0 == *intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}
		break;

	default:
		return -EINVAL;
	}
	return 0;
}



/* this is the handler for responses to network/operator commands */
static int net_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	const struct gsmd_signal_quality *sq;
	enum gsmd_netreg_state state;
	struct gsmd_own_number *num;
	struct gsmd_phonebook *gp;
	int *intresult;
	static int zd_reg_cnt = 0; 

	u_int32_t sigq;
	static u_int16_t sigq_id = 0;


	switch (gmh->msg_subtype) {
	case GSMD_NETWORK_REGISTER:
		intresult = (int *) ((char *) gmh + sizeof(*gmh));
		if(init_step == STEP_NET_REGISTER)
		{
		//PhoneTrace(0,"reg ret = %d",*intresult);
		//if (PhoneTrace_base != NULL)
		//	PhoneTrace_base(0,"reg ret = %d",*intresult);
		
			if(0 == *intresult)
			{
				//hyUsbPrintf("SIM reg ok\r\n");
				//if (PhoneTrace_base != NULL)
				//{
					//PhoneTrace_base(1,"SIM reg ok");
				//}
				net_reg_state = 1;				
			}
			if( -14 == *intresult)
			{
				syssleep(10);
				init_step = (enum shell_init_step)(init_step+1);	
			}
			if(0==*intresult || -15==*intresult || -30==*intresult)
			{
				zd_reg_cnt += 1;
				if(zd_reg_cnt > 3)
				{
					init_step = (enum shell_init_step)(init_step+1);
					zd_reg_cnt = 0;
				}								
			}
			next_init_step = 1;
		}

		if(cur_reset_step == RESET_NET_REGISTER)
		{
			if( -14 == *intresult)
			{
				syssleep(10);
				init_step = (enum shell_init_step)(init_step+1);	
			}
			if(0==*intresult || -15==*intresult || -30==*intresult)
			{
				zd_reg_cnt += 1;
				if(zd_reg_cnt > 3)
				{
					cur_reset_step = (enum reset_step)(cur_reset_step+1);
					zd_reg_cnt = 0;
				}
			}				

			reset_step_flag = 1;
		}
		break;
	case GSMD_NETWORK_RE_REGISTER:
		intresult = (int *) ((char *) gmh + sizeof(*gmh));
		if(init_step == STEP_NET_RE_REGISTER)
		{
			if(0==*intresult || -15==*intresult)
			{
				
				init_step = (enum shell_init_step)(init_step+1);				
			}
			next_init_step = 1;
		}

		if(cur_reset_step == RESET_NET_REGISTER)
		{
			if(0==*intresult || -15==*intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);
			
			reset_step_flag = 1;
		}
		break;
	case GSMD_NETWORK_SIGQ_GET:
		sq = (struct gsmd_signal_quality *)((char *) gmh + sizeof(*gmh));
	
		if(sq->rssi > 99)
			sigq = 99;
		else
			sigq = sq->rssi;

		sigq_id++;
		sigq = (u_int32_t)((sigq_id<<16) | sigq);

		gsmcdma_net_sigq = sigq;
		break;

	case GSMD_NETWORK_QUERY_REG:
		state = *(enum gsmd_netreg_state *) gmh->data;
		switch (state)
		{
		case GSMD_NETREG_UNREG:
		case GSMD_NETREG_DENIED:
			gsmcdma_net_reg_stat = -1;	//未注册
			break;
		case GSMD_NETREG_REG_HOME:
		case GSMD_NETREG_REG_ROAMING:
			gsmcdma_net_reg_stat = 1;	//已注册
			break;
		case GSMD_NETREG_UNREG_BUSY:
			gsmcdma_net_reg_stat = 0;	//正在注册
			break;		
		default:
			break;
		}

		break;

	case GSMD_NETWORK_GET_NUMBER:
		if(STEP_GET_OWN_NUMBER == init_step)
		{
			num = (struct gsmd_own_number *)gmh->data;
			if(num->is_last >= 0)
			{
				if(phb_sim.ownNumTotal < 2)
				{
					if (phb_sim.allNode == NULL)
					{
						int size;
						size = (phb_sim.total+2) * sizeof(struct gsmd_phonebook);
						phb_sim.allNode = MALLOC(size);
						if(!phb_sim.allNode)
							return -ENOMEM;
					}
					gp = (struct gsmd_phonebook *)(phb_sim.allNode+phb_sim.ownNumTotal);
					phb_sim.ownNumTotal++;

					strlcpy(gp->numb, num->addr.number, GSMD_PB_NUMB_MAXLEN+1);
					Hyc_Unicode2Gbk(num->name, gp->text, BIG_ENDIAN);
					gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';
					
					memset(gsm_SimCard,0,20);
					strcpy(gsm_SimCard,gp->numb);
					strcpy(gsm_phoneName,gp->text);
					gu8nameFlag = 1 ;
				//if(PhoneTrace_base)
				//	PhoneTrace_base(0,"SIM__%s",gsm_SimCard);
				}
			}
			else if(-2 == num->is_last)
			{
				//错误,重新读取
				phb_sim.ownNumTotal = 0;
				next_init_step = 1;
			}
			
			if(-1==num->is_last || 1==num->is_last)
			{
				phb_sim.ready = 0;
				phb_sim_init_stat = 0;	//ready

			//	psema_phb_read = semaphore_init(0);
			//	if(!psema_phb_read)
			//		return -ENOMEM;
			//	pmail_phb_read = mailbox_init();
			//	if(!pmail_phb_read)
			//		return -ENOMEM;

			//	mailbox_put(pmail_phb_read, (struct phb_sim_info *)(&phb_sim));

				// TODO: 名片册读取SIM卡
			//	Phb_ReadSIM_thead();

			//	semaphore_wait(psema_phb_read);
			//	semaphore_destroy(psema_phb_read, OS_DEL_NO_PEND);
			//	psema_phb_read = NULL;
			//	mailbox_destroy(pmail_phb_read, OS_DEL_NO_PEND);
			//	pmail_phb_read = NULL;

				FREE(phb_sim.allNode);
				phb_sim.allNode = NULL;
				reset_pb_state = 1; 
				init_step = (enum shell_init_step)(init_step+1);
				next_init_step = 1;
			}
		}
		break;

	default:
		return -EINVAL;
	}
	return 0;
}


static int phone_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	char *payload  = (char *)gmh + sizeof(*gmh);
	int *intresult = (char *)gmh + sizeof(*gmh);

	switch (gmh->msg_subtype) {
	case GSMD_PHONE_POWER_STATUS:
		if(STEP_GET_FUN_MODE == init_step)
		{
			//如果ULC2模块的状态不为+CFUN: 1，将其设置成为+CFUN: 1
			if('1' == payload[0])
				init_step = STEP_DETECT_SIMCARD;
			else
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}

		if(RESET_GET_FUN_MODE == cur_reset_step)
		{
			//如果ULC2模块的状态不为+CFUN: 1，将其设置成为+CFUN: 1
			if('1' == payload[0])
				cur_reset_step = RESET_CHECK_PIN;
			else
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}

		break;
	case GSMD_PHONE_POWER:
		if(STEP_SET_FUN_MODE == init_step)
		{
			//Set Phone Functionality Okay
			if (0 == *intresult)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}

		if(RESET_SET_FUN_MODE == cur_reset_step)
		{
			//Set Phone Functionality Okay
			if (0 == *intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}
		break;
	case GSMD_PHONE_GET_IMSI:
		if(STEP_GET_SIMCARD_IMSI == init_step)
		{
			if(isdigit(payload[0]))
			{
				strlcpy(phb_sim.simSerial, payload, SIMCARD_IMSI_LEN);

				init_step = (enum shell_init_step)(init_step+1);
			}
			else
			{
				//ERROR/+CME ERROR: 3  ---  operation not allowed
				sleep(5);
			}
			
			next_init_step = 1;
		}
		break;
	case GSMD_PHONE_DETECT_SIMCARD:
		if(STEP_DETECT_SIMCARD == init_step)
		{
			if('0' == payload[0])
			{
			//hyUsbMessage("detect no SIM\r\n");
				//if (PhoneTrace_base != NULL)
				//{
				//	PhoneTrace_base(1,"detect no SIM");
				//}
				//没有SIM卡，不需要执行与SIM卡有关的初始化
				init_step = INIT_STEP_NUM;
				modem_init_ok();
				phb_sim.ready = -2;	//没有SIM卡
				phb_sim_init_stat = -2;
				
				//psema_phb_read = semaphore_init(0);
				//if(!psema_phb_read)
				//	return -ENOMEM;

				// TODO: 名片册读取SIM卡
			//	Phb_ReadSIM_thead();
				
				//semaphore_wait(psema_phb_read);
				//semaphore_destroy(psema_phb_read, OS_DEL_NO_PEND);
				//psema_phb_read = NULL;
			}
			else if('1' == payload[0])
			{
				//hyUsbMessage("detect SIM OK\r\n");
				//if (PhoneTrace_base != NULL)
				//{
				//	PhoneTrace_base(1,"detect SIM OK");
				//}
				init_step = (enum shell_init_step)(init_step+1);
			}
			else
			{
				//hyUsbMessage("detect some error\r\n");
				//if (PhoneTrace_base != NULL)
				//{
				//	PhoneTrace_base(1,"detect some error");
				//}
				// ADD BY ZHAOZHENG
				//some error happend
				//没有SIM卡，不需要执行与SIM卡有关的初始化
				init_step = INIT_STEP_NUM;
				modem_init_ok();
				phb_sim.ready = -2;	//没有SIM卡 
				phb_sim_init_stat = -2;
				
				//psema_phb_read = semaphore_init(0);
				//if(!psema_phb_read)
				//	return -ENOMEM;

				// TODO: 名片册读取SIM卡
			//	Phb_ReadSIM_thead();
				
				//semaphore_wait(psema_phb_read);
				//semaphore_destroy(psema_phb_read, OS_DEL_NO_PEND);
				//psema_phb_read = NULL;
			
			}
			next_init_step = 1;
		}
		else
		{
			if('0'==payload[0] || '1'==payload[0])
				gsmcdma_ph_simcard = (int)(payload[0] - '0');
		}
		break;

	default:
		return -EINVAL;
	}
	return 0;
}



static int security_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	int result = *(int *) gmh->data;
	
	switch (gmh->msg_subtype) {
		case GSMD_SECURITY_PIN_GET_STATUS:
			if(RESET_CHECK_PIN == cur_reset_step)
			{
				if(result >= 0)
				{
					if(GSMD_PIN_SIM_PIN == (enum gsmd_pin_type)result)
						cur_reset_step = (enum reset_step)(cur_reset_step+1);
						
					else if(GSMD_PIN_READY == (enum gsmd_pin_type)result)
						cur_reset_step = RESET_SET_NEW_SMS_IND;
				}
				else
				{
					//+CME ERROR: 10	SIM not inserted
					if(-10 == result)
						cur_reset_step = RESET_STEP_NUM;
				}
				reset_step_flag = 1;
			}
			break;
		case GSMD_SECURITY_PIN_INPUT:
			//+CME ERROR: 12	SIM PUK required
			//+CME ERROR: 16	Incorrect password
			//+CME ERROR: 18	SIM PUK2 required
			if(RESET_INPUT_PIN == cur_reset_step)
			{
				//PIN accepted!
				if(0 == result)
					cur_reset_step = (enum reset_step)(cur_reset_step+1);

				reset_step_flag = 1;
			}
			break;
		default:
			return -EINVAL;
	}

	return 0;
}



//int NoSIM_voice_ctrl = 0;
static int voicecall_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	struct gsmd_call_status *gcs;
	struct gsmd_call_fwd_stat *gcfs;
	int *intresult;

	switch (gmh->msg_subtype) {
	case GSMD_VOICECALL_GET_STAT:
		/*
		gcs = (struct  gsmd_call_status*) ((char *)gmh + sizeof(*gmh));
		
		//if (gcs->idx>=0 && 1==gcs->is_last && 0==gcs->mode)
		if (gcs->idx>=0 && 1==gcs->is_last)
		{
			int rc=0;
			struct to_appctrl_msg msg;
			struct gsmcdma_to_app_msg msg_v;
			static enum gsmd_call_stat last_stat=GSMD_CALL_STAT_INIT;
			
			#if 0
			
			0 active
			1 held
			2 dialing (MO call)
			3 alerting (MO call)
			4 incoming (MT call)
			5 waiting (MT call)
			#endif
	
			if(GSMD_CALL_STAT_INIT==last_stat && GSMD_CALL_STAT_NONE==gcs->stat)
			{
				gcs->idx = 1;
				sleep(3);		//刚拨号时有可能还没有状态，稍微等等再查状态
			}
			else if(gcs->stat!=last_stat || (GSMD_CALL_STAT_NONE==last_stat && GSMD_CALL_STAT_NONE==gcs->stat))
			{
				//通知控制台有电话挂断，防止通话进程处于后台时对方挂机通话进程收不到消息
				msg.chl = MSG_CHL_GSMCDMA;
				msg.u.gsmcdma_msg.msgtype = GSMD_EVT_CALL_STAT;
				msg.u.gsmcdma_msg.u.clcc.stat = gcs->stat;
				rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);

				//inform voicecall
				msg_v.msgtype = GSMD_EVT_CALL_STAT;
				msg_v.u.clcc.stat = gcs->stat;
				rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg_v, sizeof(msg_v), 10);
			}
			
			if(0==gcs->idx )
			{
				last_stat = GSMD_CALL_STAT_INIT;
			}
			else
			{
				last_stat = gcs->stat;
				lgsm_voice_get_status(lh);
				sleep(5);
			}
		}
		else if (gcs->idx < 0)
		{
			if(-10==gcs->idx)	//无SIM卡
			{
				int rc=0;
				
				if(0==NoSIM_voice_ctrl)
				{
					struct to_appctrl_msg msg;
					struct gsmcdma_to_app_msg msg_v;
					
					NoSIM_voice_ctrl = 1;
					
					//通知控制台有电话挂断，防止通话进程处于后台时对方挂机通话进程收不到消息
					msg.chl = MSG_CHL_GSMCDMA;
					msg.u.gsmcdma_msg.msgtype = GSMD_EVT_CALL_STAT;
					msg.u.gsmcdma_msg.u.clcc.stat = GSMD_CALL_STAT_DIALING;
					rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);

					//inform voicecall
					msg_v.msgtype = GSMD_EVT_CALL_STAT;
					msg_v.u.clcc.stat = GSMD_CALL_STAT_DIALING;
					rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg_v, sizeof(msg_v), 10);
				}
				else if(2 == NoSIM_voice_ctrl)
				{
					struct to_appctrl_msg msg;
					struct gsmcdma_to_app_msg msg_v;
					
					NoSIM_voice_ctrl = 3;
					
					//通知控制台有电话挂断，防止通话进程处于后台时对方挂机通话进程收不到消息
					msg.chl = MSG_CHL_GSMCDMA;
					msg.u.gsmcdma_msg.msgtype = GSMD_EVT_CALL_STAT;
					msg.u.gsmcdma_msg.u.clcc.stat = GSMD_CALL_STAT_NONE;
					rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);

					//inform voicecall
					msg_v.msgtype = GSMD_EVT_CALL_STAT;
					msg_v.u.clcc.stat = GSMD_CALL_STAT_NONE;
					rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg_v, sizeof(msg_v), 10);
					
					return 0;
				}
				else if(3 == NoSIM_voice_ctrl)
				{
					return 0;
				}
			}
			lgsm_voice_get_status(lh);
			sleep(5);
		
		}
		*/
		
		/*
		if (gcs->is_last)
		{
		}
		*/
		break;

	case GSMD_VOICECALL_COLP_SET:
		intresult = (int *) ((char *)gmh + sizeof(*gmh));
		if (STEP_SET_COLP == init_step)
		{
			if(0 == *intresult)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}

		if(RESET_SET_COLP == cur_reset_step)
		{
			if(0 == *intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}
		break;

	case GSMD_VOICECALL_CLIP_SET:
		intresult = (int *) ((char *)gmh + sizeof(*gmh));
		if (STEP_SET_CLIP == init_step)
		{
			if(0 == *intresult)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}

		if(RESET_SET_CLIP == cur_reset_step)
		{
			if(0 == *intresult)
				cur_reset_step = (enum reset_step)(cur_reset_step+1);

			reset_step_flag = 1;
		}
		break;



	default:
		return -EINVAL;
	}
	return 0;
}


static int datacall_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	int *ret;

	switch (gmh->msg_subtype) {
	case GSMD_DATACALL_DIAL:
		ret = (int*)((char *)gmh + sizeof(*gmh));
		if(*ret)
		{
			//DEBUGP("+CME ERROR %d\n", *ret);
		}
		else
		{
			//DEBUGP("OK\n");
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}


extern int mux_flag;
static int modem_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	int *result = (char *)gmh + sizeof(*gmh);

	switch (gmh->msg_subtype)
	{
	case GSMD_MODEM_POWERON:
		break;
	case GSMD_MODEM_POWEROFF:
		/*
		if (*result)
		{
			//DEBUGP("Modem power-off failed: %i\n", *result);
			if(4 == gsmcdma_client_exit)
			{
				lgsm_modem_power(lh, 1);	//try again
			}
		}
		else
		*/
		{
			//DEBUGP("Modem off\n");
			if(4 == gsmcdma_client_exit)
			{
				gsmcdma_client_exit = 5;
			}
		}
		break;

	case GSMD_OPEN_MUX_MODE:
		if(STEP_OPEN_MUX_MODE == init_step)
		{
			if (!*result)
			{
				mux_flag = 1;
				init_step = (enum shell_init_step)(init_step+1);
			}
			
			next_init_step = 1;
		}
		break;
	case GSMD_OPEN_DLC0_MODE:
		if(STEP_OPEN_DLC0_MODE == init_step)
		{
			if (!*result)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
	 	break;
	case GSMD_OPEN_DLC1_PN:
		if(STEP_OPEN_DLC1_PN == init_step)
		{
			if (!*result)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
		break;
	case GSMD_OPEN_DLC1_MODE:
		if(STEP_OPEN_DLC1_MODE == init_step)
		{
			if (!*result)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
	    break;
	case GSMD_OPEN_DLC2_PN:
		if(STEP_OPEN_DLC2_PN == init_step)
		{
			if (!*result)
				init_step = (enum shell_init_step)(init_step+1);

			next_init_step = 1;
		}
		break;
	case GSMD_OPEN_DLC2_MODE:
		if(STEP_OPEN_DLC2_MODE == init_step)
		{
			if (!*result)
			{
				mux_flag = 2;
				init_step = (enum shell_init_step)(init_step+1);
			}
			next_init_step = 1;
		}
		break;

	default:
		return -EINVAL;
	}
	return 0;
}



struct msghandler_s {
	int type;
	lgsm_msg_handler *fn;
};

static const struct msghandler_s msghandlers[] = {

	{ GSMD_MSG_PASSTHROUGH,	pt_msghandler 			},
	{ GSMD_MSG_PHONEBOOK,	pb_msghandler 			},
	{ GSMD_MSG_SMS,			sms_msghandler 			},
	{ GSMD_MSG_NETWORK,		net_msghandler 			},
	{ GSMD_MSG_PHONE,		phone_msghandler 		},
	{ GSMD_MSG_SECURITY,	security_msghandler 	},
	{ GSMD_MSG_VOICECALL,	voicecall_msghandler 	},
	{ GSMD_MSG_DATACALL,	datacall_msghandler		},
	{ GSMD_MSG_MODEM, 		modem_msghandler		},
	{ 0, 					0 						}
};




int shell_main(struct lgsm_handle *lh)
{
	int rc;
	U32  powertime;
	char buf[512+1/*1025*/];
	int gsm_fd = lgsm_fd(lh);
	const struct msghandler_s *hndl; 	
	U32 ticks=0;
	
	init_step = 0;
	next_init_step = 1;

	phb_used = 0;
	phb_repeat = 0;

	sms_index_max = 0;
	sms_index = 0;
	sms_total = 0;

	cur_reset_step = RESET_STEP_NUM;
	reset_step_flag = 0;

//	psema_phb_read = NULL;
//	pmail_phb_read = NULL;


	for (hndl = msghandlers; hndl->fn; hndl ++)
		lgsm_register_handler(lh, hndl->type, hndl->fn);

	//lgsm_get_revision(lh);

	while (1)
	{
		if(ABS(rawtime(NULL)-ticks) >= 1000)
		{
			ticks = rawtime(NULL);
			hyUsbMessage("gsm C run\r\n");
		}
		
		#ifdef _GSMD_DEBUG_LOG_1
		{

			static int cnt_g1 = 0;
	
			cnt_g1++;
			if(200 <= cnt_g1)
			{
//				hyUsbMessage("g client run\r\n");
//				PhoneTrace(0,"g client run");
				//hyUsbPrintf("\r\nfctl_flag=%d\r\n", g.muxchl_fctl_flag);
				cnt_g1 = 0;
			}
		}
		#endif
		//receive data on the gsmd socket
		rc = u_recv(gsm_fd, buf, sizeof(buf), 0);
		if(0<rc)
		{
			/* we've received something on the gsmd socket, pass it
			 * on to the library */
			rc = lgsm_handle_packet(lh, buf, rc);
			//if (rc < 0)
			//	DEBUGP("ERROR processing packet: %d(%s)\n", rc, strerror(-rc));
		}
		else
		{
			if(gsmcdma_client_exit)
			{
				//关机运行
				if(1==gsmcdma_client_exit)
				{
					// del by zhaozheng for 关机死机，目前不需要关机删除短信
					//if(STEP_DEL_SMS!=init_step && sms_index == sms_index_max)
					{
						//if(init_step > STEP_LIST_SMS && -2!=phb_sim_init_stat && SIM_get_pin_stat())
						if(init_step > STEP_LIST_SMS && SIM_get_pin_stat())
						{
//							gsmcdma_client_exit = 2;
							gsmcdma_client_exit = 3;
//							lgsm_sms_delete_all(lh, LGSM_SMS_DELALL_ALL);
						}
						else
						{
							
							gsmcdma_client_exit = 3;
						}
					}
				}

				if(3 ==gsmcdma_client_exit)
				{
					gsmcdma_client_exit = 4;
					//if (PhoneTrace_base != NULL)
					//{
					//	PhoneTrace_base(0,"client = 3");
					//}
					//hyUsbMessage("client = 3\r\n");
					lgsm_modem_power(lh, 1);
					//if (PhoneTrace_base != NULL)
					//{
					//	PhoneTrace_base(0,"client = 3 ok");
					//}
					//hyUsbMessage("client = 3 ok\r\n");
					powertime = rawtime(NULL);
				}
				
				//有时模块会出现异常,导致不响应AT,这里增加超时判断
				if((5 ==gsmcdma_client_exit) || (gsmcdma_client_exit == 4 && ABS(rawtime(NULL)-powertime)>=400))
				{
					if(NULL != phb_sim.allNode)
					{
						FREE(phb_sim.allNode);
						phb_sim.allNode = NULL;
					}
					
					if(NULL != init_sms_nl.sms)
					{
						FREE(init_sms_nl.sms);
						init_sms_nl.sms = NULL;
					}
					reset_pb_state = 0;
					reset_sms_state = 0;
					return 0;
				}
			}
			else if(SIM_get_pin_stat())
			{
				//正常运行
				//模块被RESET后
				if(1 == unsol_modem_reset)
				{
					unsol_modem_reset = 0;//将模块变量复位
					cur_reset_step = RESET_GET_FUN_MODE;
					reset_step_flag = 1;					
				}


				//模块被RESET后
				if(reset_step_flag)
				{
					reset_step_flag = 0;

					switch(cur_reset_step)
					{
					case RESET_GET_FUN_MODE:
						lgsm_get_power_status(lh);
						break;
					case RESET_SET_FUN_MODE:
						//将ULC2模块设置成为+CFUN: 1
						lgsm_phone_power(lh, 1);
						break;

					case RESET_CHECK_PIN:
						lgsm_security_pin_status(lh);
						break;
					case RESET_INPUT_PIN:
						{
						struct gsmd_pin gp;
						//u_int8_t *pincode;
						
						//pincode = getPINCode();
						
						gp.type = GSMD_PIN_SIM_PIN;
						strncpy(gp.pin, gsm_PINCode, sizeof(gp.pin));
						lgsm_security_pin_input(lh, &gp);
						}
						break;
					case RESET_SET_NEW_SMS_IND:
						{
						struct lgsm_sms_cnmi lsc;
						
						glsc_getSmsCnmi(&lsc);
						
						/*
						lsc.mode = temp[0];	//2
						lsc.mt	= temp[1];	//+CMTI:1,+CMT:2
						lsc.bm = temp[2];	//None:0,+CBM:2
						lsc.ds = temp[3];	//None:0,+CDS:1
						lsc.bfr = temp[4];	//0
						*/
						lgsm_sms_set_cnmi(lh,&lsc);
						}
						break;

					case RESET_SET_SMS_FMT:
						lgsm_sms_set_fmt(lh,0);
						break;

					case RESET_SET_COLP:
						lgsm_voice_colp_set(lh,0);
						break;

					case RESET_SET_CLIP:
						lgsm_voice_clip_set(lh,1);
						break;

					case RESET_NET_REGISTER:
						//if(4!=GetPhoneSceneOpt() && 6!=GetPhoneSceneOpt())
						{
							//不是飞行/低辐射模式
							gsmd_oper_numeric oper;
							
							memset(oper, 0, sizeof(oper));
							lgsm_netreg_register(lh, oper);
						}
						/*else
						{
							cur_reset_step = (enum shell_init_step)(init_step+1);
							reset_step_flag = 1;
						}*/
						break;

					case RESET_STEP_NUM:
						//if((init_step >= STEP_GET_PHB_STORAGE)&&(reset_pb_state == 0))
						{
							
							if(phb_sim.allNode != NULL)	
							{						
								FREE(phb_sim.allNode);	
								phb_sim.used  = 0;
								phb_sim.total = 0;
							}												
						}
						
						//if((init_step >= STEP_GET_SMS_STORAGE)&&(reset_sms_state == 0))
						{
							
							if(init_sms_nl.sms != NULL)	
							{
								FREE(init_sms_nl.sms);
								init_sms_nl.used  = 0;
								init_sms_nl.total = 0;
							}													
						}						
						init_step = 0;	
						next_init_step = 1;
						break;
					}
				}


				//开机要做的步骤
				
		
				if(next_init_step )
				{
					next_init_step = 0;									
					if(init_step != STEP_NET_REGISTER)
					{
						//hyUsbPrintf("\r\n11**init_step = %d\r\n",init_step);
						//if (PhoneTrace_base != NULL)
						//	PhoneTrace_base(0,"11**init_step = %d",init_step);
					}
					switch(init_step)
					{
					case STEP_OPEN_MUX_MODE:	
						lgsm_open_mux(lh);
						break;
					case STEP_OPEN_DLC0_MODE:							
						lgsm_open_dcl0(lh);
						break;
					case STEP_OPEN_DLC1_PN:
						lgsm_dlc1_pn(lh);
						break;
					case STEP_OPEN_DLC1_MODE:
						lgsm_open_dcl1(lh);
						break;
					case STEP_OPEN_DLC2_PN:
						lgsm_dlc2_pn(lh);
						break;
					case STEP_OPEN_DLC2_MODE:
						lgsm_open_dcl2(lh);												
						break;					
					case STEP_GET_FUN_MODE:
						lgsm_get_power_status(lh);
						break;
					case STEP_SET_FUN_MODE:
						//将ULC2模块设置成为+CFUN: 1
						lgsm_phone_power(lh, 1);
						break;

					case STEP_DETECT_SIMCARD:
						lgsm_detect_simcard(lh);
						break;

					case STEP_SET_NEW_SMS_IND:
						{
						struct lgsm_sms_cnmi lsc;
						
						glsc_getSmsCnmi(&lsc);
						
						/*
						lsc.mode = temp[0];	//2
						lsc.mt	= temp[1];	//+CMTI:1,+CMT:2
						lsc.bm = temp[2];	//None:0,+CBM:2
						lsc.ds = temp[3];	//None:0,+CDS:1
						lsc.bfr = temp[4];	//0
						*/
						lgsm_sms_set_cnmi(lh,&lsc);
						}
						break;

					case STEP_SET_SMS_FMT:
						lgsm_sms_set_fmt(lh,0);
						break;

					case STEP_SET_COLP:
						lgsm_voice_colp_set(lh,0);
						break;

					case STEP_SET_CLIP:
						lgsm_voice_clip_set(lh,1);
						break;

					case STEP_GET_SIMCARD_IMSI:
						lgsm_get_imsi(lh);
						break;

					case STEP_GET_PHB_STORAGE:
						if(reset_pb_state == 0)
						{
						if(1==unsol_sim_phb_ready && 1==unsol_sim_sms_ready)
							lgsm_pb_get_storage(lh);
						else
							next_init_step = 1;
						}
						else
						{
							phb_sim.ready = 0;
							phb_sim_init_stat = 0;
							init_step = (enum shell_init_step)(init_step+3);
							next_init_step = 1;
							reset_pb_state = 0;
						}
						break;
					case STEP_LIST_PHB:
						/*
						{
						struct lgsm_phonebook_readrg pb_readrg;
						
						pb_readrg.index1 = 1;
						pb_readrg.index2 = phb_sim.total;
						lgsm_pb_read_entries(lh,&pb_readrg);
						}
						*/
						{
							#define PHB_LIST_NUM 3

							int index;
							static int cnt = 0;

							if(phb_repeat && cnt)
								cnt--;

							index = cnt*PHB_LIST_NUM+1;
							if(index<=phb_sim.total && phb_used<phb_sim.used)
							{
								struct lgsm_phonebook_readrg pb_readrg;

								pb_readrg.index1 = index;
								pb_readrg.index2 = index + (PHB_LIST_NUM-1);
								if(pb_readrg.index2 > phb_sim.total)
									pb_readrg.index2 = phb_sim.total;
								
								lgsm_pb_read_entries(lh,&pb_readrg);

								cnt++;
								phb_repeat = 0;
							}
							else
							{
								init_step = (enum shell_init_step)(init_step+1);
								next_init_step = 1;

								cnt = 0;
								phb_used = 0;
							}
						}
						break;

					case STEP_GET_OWN_NUMBER:
						lgsm_get_subscriber_num(lh);
						break;

					case STEP_GET_SMS_STORAGE:
						if(reset_sms_state == 0)
						{
							if(1==unsol_sim_phb_ready && 1==unsol_sim_sms_ready)
							{
								lgsm_sms_get_storage(lh);
							}
							else
								next_init_step = 1;
						}
						else
						{
							init_step = (enum shell_init_step)(init_step+3);
							next_init_step = 1;
							reset_sms_state = 0;
						}
						break;
					case STEP_LIST_SMS:
						{
						struct lgsm_sms_list sms_lt;
						
						sms_lt.stat = LGSM_SMS_ALL;
						sms_lt.mode = LGSM_SMS_STAT_NOTCHANGE;
						lgsm_sms_list(lh, &sms_lt);
						}
						break;
					case STEP_DEL_SMS:
//						lgsm_sms_delete_all(lh, LGSM_SMS_DELALL_ALL);
						{
		//					hyUsbPrintf("\r\n del all SMS 444\r\n");
							//lgsm_sms_delete_all(lh, LGSM_SMS_DELALL_ALL);
							init_step += 1;
							next_init_step = 1;
						}
						break;
					case STEP_NET_REGISTER:
						//if(4!=GetPhoneSceneOpt() && 6!=GetPhoneSceneOpt())
						{
							//不是飞行/低辐射模式				
							gsmd_oper_numeric oper;
							memset(oper, 0, sizeof(oper));
							lgsm_netreg_register(lh, oper);
						}
						/*else
						{
							init_step = (enum shell_init_step)(init_step+1);
							next_init_step = 1;
						}*/
						break;					
					case STEP_NET_RE_REGISTER:
						if(net_reg_state == 0)
						{
							//if(4!=GetPhoneSceneOpt() && 6!=GetPhoneSceneOpt())
							{
								gsmd_oper_numeric oper;
								memset(oper, 0, sizeof(oper));
								lgsm_netreg_re_register(lh, oper);
							}
							/*else
							{
															
								init_step = (enum shell_init_step)(init_step+1);
								next_init_step = 1;
							}*/
						}
						else
						{
							net_reg_state = 0;							
							init_step = (enum shell_init_step)(init_step+1);
							next_init_step = 1;
						}
					
					case INIT_STEP_NUM:
					
						modem_init_ok();
						
						break;
					}
				}


				if(RESET_STEP_NUM == cur_reset_step)
				{
					if(init_step > STEP_NET_REGISTER)
					{
						/*if(4==GetPhoneSceneOpt() || 6==GetPhoneSceneOpt())
						{
							//飞行/低辐射模式
							gsmcdma_net_reg_stat = -1;
							gsmcdma_net_sigq = 0;
						}
						else*/
						{
							if(gsmcdma_net_query_reg)
							{
							//hyUsbPrintf("query_reg\r\n");
								//查询注册状态
								lgsm_netreg_query(lh);
								gsmcdma_net_query_reg = 0;

								//测试来电按接听键无法接通问题
								//lgsm_voice_get_status(lh);
							}
							
							//正常模式
							if(gsmcdma_net_query_sigq)
							{
//								PhoneTrace(0,"middle");
								//读信号
								lgsm_signal_quality(lh);
								gsmcdma_net_query_sigq = 0;
								//hyUsbPrintf("signal_quality\r\n");
//								hyUsbPrintf("\r\n phb_sim_init_stat = %d",phb_sim_init_stat);
							
							}
						}
					}

					if(init_step > STEP_DETECT_SIMCARD)
					{
						if(gsmcdma_ph_detect_simcard)
						{
						//hyUsbPrintf("DETECT_SIMCARD\r\n");
							//检测是否有卡
							lgsm_detect_simcard(lh);
							gsmcdma_ph_detect_simcard = 0;
						}
					}
				}
			}
			
			sleep(2);
		}
	}
}



/*-------------------------------------------------------------------------------------
* function:		app_shell_init()
* description:	
* parameter:
* return:
* Note:
*-------------------------------------------------------------------------------------*/
void app_shell_init(void)
{
	memset(&phb_sim, 0, sizeof(struct phb_sim_info));
	phb_sim.ready = -1;		//正在初始化
	phb_sim_init_stat = -1;
	
	memset(&init_sms_nl, 0, sizeof(struct sms_num_list));
}


/*-------------------------------------------------------------------------------------
* function:		phb_sim_stat()
* description:	
* parameter:	
* return:		
* Note:
*-------------------------------------------------------------------------------------*/
int phb_sim_stat(void)
{
	return phb_sim_init_stat;
}



/*-------------------------------------------------------------------------------------
* function:		sms_sim_total()
* description:	
* parameter:
* return:
* Note:
*-------------------------------------------------------------------------------------*/
u_int32_t sms_sim_total(void)
{
	return init_sms_nl.total;
}

