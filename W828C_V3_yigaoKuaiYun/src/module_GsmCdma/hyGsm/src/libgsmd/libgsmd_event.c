/* libgsmd event demultiplexer handler 
 *
 * (C) 2006-2007 by OpenMoko, Inc.
 * Written by Harald Welte <laforge@openmoko.org>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

#include "std.h"
//#include <stdlib.h>
//#include <string.h>

#include "voicecall.h"
#include "libgsmdsms.h"
#include "libgsmdevent.h"

#include "usock.h"
#include "gsmdevent.h"

#include "lgsm_internals.h"
//#include "sms.h"

#include "app_shell.h"


extern tMSG_QUEUE *to_appctrl_msg_que = NULL;
extern tMSG_QUEUE *gsmcdma_to_voicecall_msg_que=NULL;
extern tMSG_QUEUE *sms_stat_report_msg_que=NULL;

extern enum shell_init_step init_step;
extern int sms_index_max;


static const char *scope_name[] = {
	/*[GSMD_SCOPE_CELL_WIDE_OPER] = */	"immediate-display cell",
	/*[GSMD_SCOPE_PLMN_WIDE] = */		"PLMN",
	/*[GSMD_SCOPE_LOC_AREA_WIDE] = */	"Location Area",
	/*[GSMD_SCOPE_CELL_WIDE] = */		"cell",
};

static const char *lang_name[] = {
	/*[GSMD_LANG_GERMAN] = */		"German",
	/*[GSMD_LANG_ENGLISH] = */		"English",
	/*[GSMD_LANG_ITALIAN] = */		"Italian",
	/*[GSMD_LANG_FRENCH] = */		"French",
	/*[GSMD_LANG_SPANISH] = */		"Spanish",
	/*[GSMD_LANG_DUTCH] = */		"Dutch",
	/*[GSMD_LANG_SWEDISH] = */		"Swedish",
	/*[GSMD_LANG_DANISH] = */		"Danish",
	/*[GSMD_LANG_PORTUGUESE] = */	"Portuguese",
	/*[GSMD_LANG_FINNISH] = */		"Finnish",
	/*[GSMD_LANG_NORWEGIAN] = */	"Norwegian",
	/*[GSMD_LANG_GREEK] = */		"Greek",
	/*[GSMD_LANG_TURKISH] = */		"Turkish",
	/*[GSMD_LANG_HUNGARIAN] = */	"Hungarian",
	/*[GSMD_LANG_POLISH] = */		"Polish",
	/*[GSMD_LANG_UNSPECIFIED] = */	"an unspecified language",
};


#define PIN_SIZE 8

static const char *pin;
static char pinbuf[PIN_SIZE+1];
static char pinbuf2[PIN_SIZE+1];

u_int8_t newSmsFlag = 0;

int new_sms_inform_num=0;


extern u_int8_t unsol_sim_phb_ready;
extern u_int8_t unsol_sim_sms_ready;
extern u_int8_t unsol_modem_reset;

u_int8_t lgsm_evt_getNewSmsFlag(void)
{
	return newSmsFlag;
}

static int shell_demux_msghandler(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	int len,rc=0;
	struct to_appctrl_msg msg; 
	struct gsmcdma_to_app_msg msg_v;
	struct gsmd_evt_auxdata *aux = (struct gsmd_evt_auxdata *) gmh->data;
	

	if (gmh->len < sizeof(*aux))
		return -EIO;
	
	if (gmh->msg_type != GSMD_MSG_EVENT || gmh->msg_subtype >= __NUM_GSMD_EVT)
		return -EINVAL;

	len=sizeof(*gmh) + gmh->len;
	
	switch (gmh->msg_subtype) {
	case GSMD_EVT_IN_CALL:		//inform AppControl
		//DEBUGP("EVENT: Incoming call type = %u\n", aux->u.call.type);
		if(GSMD_CALL_VOICE == aux->u.call.type)
		{
			msg.chl = MSG_CHL_GSMCDMA;
			msg.u.gsmcdma_msg.msgtype = GSMD_EVT_IN_CALL;
			msg.u.gsmcdma_msg.u.call.type = GSMD_CALL_VOICE;
			rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);
		}
		// 20100920 modify by zhaozheng 删除通话功能 FOR KXW
		lgsm_voice_hangup(lh);

		break;
	
	case GSMD_EVT_IN_CLIP:
	#if 1
		//inform voicecall
		//DEBUGP("EVENT: Incoming call clip = %s\n", aux->u.clip.addr.number);
		msg_v.msgtype = GSMD_EVT_IN_CLIP;
		msg_v.u.clip.addr.type = aux->u.clip.addr.type;
		strcpy(msg_v.u.clip.addr.number, aux->u.clip.addr.number);
		rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg_v, sizeof(msg_v), 10);
	#endif
		break;

	case GSMD_EVT_IN_SMS:
		if (aux->u.sms.inlined)
		{
			struct gsmd_sms_list *sms;
			
			sms = (struct gsmd_sms_list *) aux->data;
			//if(sms->payload.is_voicemail)
			//	DEBUGP("EVENT: You have a voice mail \n");
			//if ( strlen(aux->u.sms.alpha) )
			//	DEBUGP("From %s\n", aux->u.sms.alpha);
			
			//todo:save
			if(strlen(sms->addr.number)>2 && isdigit(sms->addr.number[1]))
			{
				//mediaSendMsg2Dec(1);//PAUSE media
				sms->stat = GSMD_SMS_REC_UNREAD;
				// 20100920 modify by zhaozheng 删除短信功能 FOR KXW
//				Sms_smsStore(sms,PhoneMemSms);
				//mediaSendMsg2Dec(2);//RESUME media
				newSmsFlag=1;
			}
		}
		else
		{
			struct lgsm_sms_read sms_rd;

			sms_rd.index = aux->u.sms.index;
			sms_rd.mode = LGSM_SMS_STAT_NOTCHANGE;
			
			//DEBUGP("EVENT: Incoming SMS stored at location %i\n", aux->u.sms.index);
			//todo:read
			lgsm_sms_read(lh, &sms_rd);

			sms_index_max = sms_rd.index;
		}
		break;

	case GSMD_EVT_IN_CBM:
		if (aux->u.cbm.inlined) 
		{
			struct gsmd_cbm *msg;
			
			msg = (struct gsmd_cbm *) aux->data;
			
			#if 0
			DEBUGP("EVENT: Incoming %s-wide Cell Broadcast message in "
					"%s (page %i of %i)\n",
					scope_name[msg->serial.scope],
					lang_name[msg->language],
					msg->page, msg->pages);
			#endif
			//todo:save
		}
		else
		{
			#if 0
			DEBUGP("EVENT: Incoming Cell Broadcast message stored at "
					"location %i\n", aux->u.cbm.index);
			#endif
			//todo:save index
		}
		//inform AppControl
		break;

	case GSMD_EVT_IN_DS:
		if (aux->u.ds.inlined) 
		{
			struct gsmd_sms_list *sms;
			struct sms_stat_report sms_rp;

			sms = (struct gsmd_sms_list *) aux->data;
			//DEBUGP("EVENT: Incoming Status Report\n");
			//DEBUGP("message ref = %d, status = %d\n", sms->index,sms->payload.coding_scheme);
			
			sms_rp.result = sms->payload.coding_scheme;
			memcpy(sms_rp.number, sms->addr.number, sizeof(sms->addr.number));
			rc = msg_queue_put(sms_stat_report_msg_que, (char *)&sms_rp, sizeof(sms_rp), 10);
		}
		else
		{
			//DEBUGP("EVENT: Incoming Status Report stored at location %i\n",
			//		aux->u.ds.index);
		}
		//todo:send the Status Report to who?
		break;
	
	case GSMD_EVT_OUT_COLP:
	#if 0
		//inform voicecall
		//DEBUGP("EVENT: Outgoing call colp = %s\n", aux->u.colp.addr.number);
		msg.msgtype = GSMD_EVT_OUT_COLP;
		msg.u.colp.addr.type = aux->u.colp.addr.type;
		strcpy(msg.u.colp.addr.number, aux->u.colp.addr.number);
		rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg, sizeof(msg), 10);
	#endif
		break;
	
	case GSMD_EVT_NETREG:
		lh->netreg_state = aux->u.netreg.state;
		break;

	case GSMD_EVT_SIGNAL:
		//DEBUGP("EVENT: Signal Quality: %u\n", aux->u.signal.sigq.rssi);
		break;

	case GSMD_EVT_PIN:
		#if 0
		{
			int rc;
			int type = aux->u.pin.type;
			char *newpin = NULL;
			
			//printf("EVENT: PIN request (type='%s') ", lgsm_pin_name(type));
			DEBUGP("EVENT: PIN request (type='%s') ", lgsm_pin_name(type));


			/* FIXME: read pin from STDIN and send it back via lgsm_pin */
			if (type == 1 && pin) 
			{
				//printf("Auto-responding with pin `%s'\n", pin);
				DEBUGP("Auto-responding with pin `%s'\n", pin);
				pending_responses ++;
				return lgsm_pin(lh, type, pin, NULL);
			} 
			else 
			{
				do 
				{
					//printf("Please enter %s: ", lgsm_pin_name(type));
					//rc = fscanf(stdin, "%8s", pinbuf);
					DEBUGP("Please enter %s: ", lgsm_pin_name(type));
					strcpy(pinbuf, "12345678");
					rc = 0;
				} while (rc < 1);

				switch (type) 
				{
				case GSMD_PIN_SIM_PUK:
				case GSMD_PIN_SIM_PUK2:
					do {
						//printf("Please enter new PIN: ");
						//rc = fscanf(stdin, "%8s", pinbuf2);
						DEBUGP("Please enter new PIN: ");
						strcpy(pinbuf2, "12345678");
						rc = 0;
						newpin = pinbuf2;
					} while (rc < 1);
					break;
				default:
					break;
				}

				pending_responses ++;
				return lgsm_pin(lh, type, pinbuf, newpin);
			}
		}
		#endif
		break;


	case GSMD_EVT_OUT_STATUS:
		break;

	case GSMD_EVT_IN_ERROR:
		break;

	case GSMD_EVT_CALL_WAIT:
	#if 0
		//inform voicecall
		DEBUGP("EVENT: Call Waiting: %s,%d\n", aux->u.ccwa.addr.number, aux->u.ccwa.addr.type);
		msg.msgtype = GSMD_EVT_CALL_WAIT;
		msg.u.ccwa.addr.type = aux->u.ccwa.addr.type;
		strcpy(msg.u.ccwa.addr.number, aux->u.ccwa.addr.number);
		rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg, sizeof(msg), 10);
	#endif
		break;

	case GSMD_EVT_SIM_INIT:
		if(1 == aux->u.sim_init.phb)
			unsol_sim_phb_ready = 1;

		if(1 == aux->u.sim_init.sms)
			unsol_sim_sms_ready = 1;
		break;

	case GSMD_EVT_MODEM_RESET:
		if(1 == aux->u.modem.reset)
		{
			unsol_modem_reset = 1;

			//msg.chl = MSG_CHL_GSMCDMA;
			//msg.u.gsmcdma_msg.msgtype = GSMD_EVT_MODEM_RESET;
			//rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);
		}
		break;

	case GSMD_EVT_CALL_STAT:
		//通知控制台有电话状态改变，防止通话进程处于后台时对方挂机通话进程收不到消息
		msg.chl = MSG_CHL_GSMCDMA;
		msg.u.gsmcdma_msg.msgtype = GSMD_EVT_CALL_STAT;
		msg.u.gsmcdma_msg.u.clcc.stat = aux->u.csurc.stat;
		rc = msg_queue_put(to_appctrl_msg_que, (char *)&msg, sizeof(msg), 10);

		//inform voicecall
		msg_v.msgtype = GSMD_EVT_CALL_STAT;
		msg_v.u.clcc.stat = aux->u.csurc.stat;
		rc = msg_queue_put(gsmcdma_to_voicecall_msg_que, (char *)&msg_v, sizeof(msg_v), 10);
		break;

    default:
        //DEBUGP( "lgsm_demux_msghandler: %s %d", "unhandled event type =", gmh->msg_subtype);
		break;
	}

	return rc;
}



int lgsm_evt_init(struct lgsm_handle *lh)
{
	lh->netreg_state = LGSM_NETREG_ST_NOTREG;
	return lgsm_register_handler(lh, GSMD_MSG_EVENT, &shell_demux_msghandler);
}



void lgsm_evt_exit(struct lgsm_handle *lh)
{
	lgsm_unregister_handler(lh, GSMD_MSG_EVENT);
}
