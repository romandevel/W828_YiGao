/* gsmd SMS functions
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
//#include "stdlib.h"
//#include "stdio.h"
//#include "string.h"


#include "gsmd.h"
#include "usock.h"
#include "gsmd_select.h"
#include "atcmd.h"
#include "unsolicited.h"
#include "gsmdsms.h"
#include "extrsp.h"



static const char *ts0705_memtype_name[] = {
	/*[GSM0705_MEMTYPE_NONE]		= */"NONE",
	/*[GSM0705_MEMTYPE_BROADCAST]	= */"BM",
	/*[GSM0705_MEMTYPE_ME_MESSAGE]	= */"ME",
	/*[GSM0705_MEMTYPE_MT]			= */"MT",
	/*[GSM0705_MEMTYPE_SIM]			= */"SM",
	/*[GSM0705_MEMTYPE_TA]			= */"TA",
	/*[GSM0705_MEMTYPE_SR]			= */"SR",
};



extern struct gsmd_timer * discard_timer2(struct gsmd *g);



static inline int parse_memtype(char *memtype)
{
	int i;

	for (i = 0; i < SIZE_OF_ARRAY(ts0705_memtype_name); i++) {
		if (!strcmp(ts0705_memtype_name[i], memtype))
			return i;
	}

	return GSM0705_MEMTYPE_NONE;
}

static int sms_list_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_sms_list msg;
	int i, idx, stat, len, cr;
	u_int8_t pdu[SMS_MAX_PDU_SIZE];


	if (cmd->ret && cmd->ret != -255)
		return 0;	/* TODO: Send a response */

	/* FIXME: TEXT mode */
	if (
			sscanf(resp, "+CMGL: %i,%i,,%i\n%n",
				&idx, &stat, &len, &cr) < 3 &&
			sscanf(resp, "+CMGL: %i,%i,\"%*[^\"]\",%i\n%n",
				&idx, &stat, &len, &cr) < 3)
		return -EINVAL;	/* TODO: Send a response */
	if (len > 164)
		return -EINVAL;	/* TODO: Send a response */

	msg.index = idx;
	msg.stat = stat;

	if(0 == cmd->ret)
		msg.is_last = 1;//(cmd->ret == 0);
	else
		msg.is_last = 0;

	for (i = 0; resp[cr] >= '0' && resp[cr + 1] >= '0' &&
			i < SMS_MAX_PDU_SIZE; i ++) {
		if (sscanf(resp + cr, "%2hhX", &pdu[i]) < 1) {
			//gsmd_log(GSMD_DEBUG, "malformed input (%i)\n", i);
			return -EINVAL;	/* TODO: Send a response */
		}
		cr += 2;
	}
	
	
	if(!(0x02&pdu[0]))//by kenny
	{
		if (sms_pdu_to_msg(&msg, pdu, len, i, 0)) {
			//gsmd_log(GSMD_DEBUG, "malformed PDU\n");
			return -EINVAL;	/* TODO: Send a response */
		}
	}

	
	return gsmd_ucmd_submit(gu, GSMD_MSG_SMS, GSMD_SMS_LIST,
			cmd->id, sizeof(msg), &msg);
}

static int sms_read_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_sms_list msg;
	int i, stat, len, cr;
	u_int8_t pdu[SMS_MAX_PDU_SIZE];
	const char *colon;

	if(!strncmp(resp, "+CMGR:", 6))
	{
		if (cmd->ret)
			return 0;	/* TODO: Send a response */

		/* FIXME: TEXT mode */
		if (
				sscanf(resp, "+CMGR: %i,,%i\n%n",
					&stat, &len, &cr) < 2 &&
	            sscanf(resp, "+CMGR: %i,\"%*[^\"]\",%i\n%n",
					&stat, &len, &cr) < 2)
			return -EINVAL;	/* TODO: Send a response */
		if (len > 164)
			return -EINVAL;	/* TODO: Send a response */

		memset(&msg, 0,sizeof(msg));
		
		msg.index = 0;
		colon = strchr(cmd->buf, '=');

	    /* get a correct message index value on reading a SMS */
		if (!strncmp(cmd->buf, "AT+CMGR", 7) && colon) 
			msg.index = atoi(colon+1);
		msg.stat = stat;
		msg.is_last = 1;
		for (i = 0; resp[cr] >= '0' && resp[cr + 1] >= '0' &&
				i < SMS_MAX_PDU_SIZE; i ++) {
			if (sscanf(resp + cr, "%2hhX", &pdu[i]) < 1) {
				gsmd_log(GSMD_DEBUG, "malformed input (%i)\n", i);
				return -EINVAL;	/* TODO: Send a response */
			}
			cr += 2;
		}
		if (sms_pdu_to_msg(&msg, pdu, len, i, 0)) {
			gsmd_log(GSMD_DEBUG, "malformed PDU\n");
			return -EINVAL;
		}
	}
	else
	{
		//+CMS ERROR: 321 --- invalid memory index
		msg.index = -1;
	}
	return gsmd_ucmd_submit(gu, GSMD_MSG_SMS, GSMD_SMS_READ,
			cmd->id, sizeof(msg), &msg);
}

static int sms_send_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int msgref;


	if (cmd->ret == 0 || cmd->ret == -255) {
		if (sscanf(resp, "+CMGS: %i", &msgref) < 1)
			msgref = -EINVAL;
	} else
		msgref = -cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_SEND,
			cmd->id, sizeof(msgref), &msgref);
}

static int sms_write_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result;

	if (cmd->ret == 0) {
		if (sscanf(resp, "+CMGW: %i", &result) < 1)
			result = -EINVAL;
	} else
		result = -cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_WRITE,
			cmd->id, sizeof(result), &result);
}

static int sms_delete_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_DELETE,
			cmd->id, sizeof(result), &result);
}

static int sms_delete_all_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_DELETE_ALL,
			cmd->id, sizeof(result), &result);
}


static int usock_cpms_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_ucmd *ucmd = ucmd_alloc(sizeof(struct gsmd_sms_storage));
	struct gsmd_sms_storage *gss = (struct gsmd_sms_storage *) ucmd->buf;
	char buf[3][3];

	//DEBUGP("entering(cmd=%p, gu=%p)\n", cmd, gu);

	if (!ucmd)
		return -ENOMEM;

	ucmd->hdr.version = GSMD_PROTO_VERSION;
	ucmd->hdr.msg_type = GSMD_MSG_SMS;
	ucmd->hdr.msg_subtype = GSMD_SMS_GET_MSG_STORAGE;
	ucmd->hdr.len = sizeof(struct gsmd_sms_storage);
	ucmd->hdr.id = cmd->id;

#if defined(GSM_MODULE_SIM300) || defined(GSM_MODULE_ULC2)
	if (sscanf(resp, "+CPMS: \"%2[A-Z]\",%i,%i,"
				"\"%2[A-Z]\",%i,%i,\"%2[A-Z]\",%i,%i",
				buf[0], &gss->mem[0].used, &gss->mem[0].total,
				buf[1], &gss->mem[1].used, &gss->mem[1].total,
				buf[2], &gss->mem[2].used, &gss->mem[2].total) < 9)

#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)
	if (sscanf(resp, "+CPMS: \"%2[A-Z]_P\", %i, %i,"
				" \"%2[A-Z]_P\", %i, %i, \"%2[A-Z]_P\", %i, %i",
				buf[0], &gss->mem[0].used, &gss->mem[0].total,
				buf[1], &gss->mem[1].used, &gss->mem[1].total,
				buf[2], &gss->mem[2].used, &gss->mem[2].total) < 9)
#endif
	{
		gss->mem[0].total = 0;
		
		//FREE(ucmd);
		//return -EINVAL;	/* TODO: Send a response */
	}
	else
	{
		gss->mem[0].memtype = parse_memtype(buf[0]);
		gss->mem[1].memtype = parse_memtype(buf[1]);
		gss->mem[2].memtype = parse_memtype(buf[2]);
	}

	usock_cmd_enqueue(ucmd, gu);

	return 0;
}


static int usock_get_smsc_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_addr ga;


		if (sscanf(resp, "+CSCA: \"%31[^\"]\",%hhi", ga.number, &ga.type) < 2)
		{
			ga.type = 0;
		//return -EINVAL;	/* TODO: Send a response */
	}

	return gsmd_ucmd_submit(ctx,
			GSMD_MSG_SMS, GSMD_SMS_GET_SERVICE_CENTRE,
			cmd->id, sizeof(ga), &ga);
}


static int usock_set_smsc_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_SET_SERVICE_CENTRE,
			cmd->id, sizeof(result), &result);
}


static int usock_get_cnmi_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_sms_cnmi gsc;

	//+CNMI: <mode>,<mt>,<bm>,<ds>,<bfr>
	if (sscanf(resp, "+CNMI: %hhi,%hhi,%hhi,%hhi,%hhi",
				&gsc.mode, &gsc.mt, &gsc.bm, &gsc.ds, &gsc.bfr) < 5)
		gsc.mode = 255;		//fail
		//return -EINVAL;	/* TODO: Send a response */

	return gsmd_ucmd_submit(ctx,
			GSMD_MSG_SMS, GSMD_SMS_GET_NEW_MSG_INDICATIONS,
			cmd->id, sizeof(gsc), &gsc);
}


static int usock_set_cnmi_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_SET_NEW_MSG_INDICATIONS,
			cmd->id, sizeof(result), &result);
}


static int sms_set_fmt_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int result = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SMS, GSMD_SMS_SET_FMT,
			cmd->id, sizeof(result), &result);
}



static const char *gsmd_cmgl_stat[] = {
	"REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL",
};

static const char *gsmd_cmgda_type[] = {
	"DEL READ", "DEL UNREAD", "DEL SENT", "DEL UNSENT", "DEL INBOX", "DEL ALL"
};


/* main unix socket Short Message receiver */
int usock_rcv_sms(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	/* FIXME: TEXT mode support!!  */
	struct gsmd_atcmd *cmd = NULL;
	struct _gsmd_sms_list *gsl;
	struct gsmd_sms_read *gsr;
	struct gsmd_sms_submit *gss;
	struct gsmd_sms_write *gsw;
	struct gsmd_addr *ga;
	enum ts0705_mem_type *storage;
	struct gsmd_sms_cnmi *gsc;
	int *type, *index, *value;
	int atcmd_len;
	char buf[1024];

	switch (gph->msg_subtype) {
	case GSMD_SMS_LIST:
		if(len < sizeof(*gph) + sizeof(*gsl))
			return -EINVAL;

		gsl = (struct _gsmd_sms_list *) ((char *)gph + sizeof(*gph));
		if (gsl->stat < 0 || gsl->stat > 4)
			return -EINVAL;

#ifdef GSM_MODULE_SIM300
		/* 
		AT+CMGL=<stat>[,<mode>]
		<mode>:0 normal
			   1 not change status of the specified SMS record
		*/
		if (gu->gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT)
			atcmd_len = sprintf(buf, "AT+CMGL=\"%s\",%d", gsmd_cmgl_stat[gsl->stat], gsl->mode);
		else
			atcmd_len = sprintf(buf, "AT+CMGL=%d,%d", gsl->stat, gsl->mode);

#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)||defined(GSM_MODULE_ULC2)

		//AT+CMGL[=<stat>]
		if (gu->gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT)
			atcmd_len = sprintf(buf, "AT+CMGL=\"%s\"", gsmd_cmgl_stat[gsl->stat]);
		else
			atcmd_len = sprintf(buf, "AT+CMGL=%d", gsl->stat);
#endif

		cmd = atcmd_fill(buf, atcmd_len + 1,
				&sms_list_cb, gu, gph->id, discard_timer2);
		break;

	case GSMD_SMS_READ:
		if(len < sizeof(*gph) + sizeof(*gsr))
			return -EINVAL;
		gsr = (struct gsmd_sms_read *) ((char *)gph + sizeof(*gph));

#ifdef GSM_MODULE_SIM300
		/*
		AT+CMGR=<index>[,<mode>]
		<mode>:0 normal
			   1 not change status of the specified SMS record
		*/
		atcmd_len = sprintf(buf, "AT+CMGR=%d,%d", gsr->index, gsr->mode);

#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)||defined(GSM_MODULE_ULC2)

		//AT+CMGR=<index>
		atcmd_len = sprintf(buf, "AT+CMGR=%d", gsr->index);
#endif

		cmd = atcmd_fill(buf, atcmd_len + 1,
				&sms_read_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_SEND:
		if (len < sizeof(*gph) + sizeof(*gss))
			return -EINVAL;
		gss = (struct gsmd_sms_submit *) ((char *) gph + sizeof(*gph));

		if (gu->gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT) {
			atcmd_len = sprintf(buf, "AT+CMGS=\"%s\"\n%.*s",
					gss->addr.number,
					gss->payload.length,
					gss->payload.data);	/* FIXME */
		} else {
			atcmd_len = sprintf(buf, "AT+CMGS=%i\n",
					sms_pdu_make_smssubmit(NULL, gss) - 1);
			atcmd_len += sms_pdu_make_smssubmit(buf + atcmd_len,
					gss) * 2;
		}
		buf[atcmd_len ++] = 26;	/* ^Z ends the message */
		buf[atcmd_len ++] = 0;

		cmd = atcmd_fill(buf, atcmd_len, &sms_send_cb, gu, gph->id, discard_timer2);
		break;

	case GSMD_SMS_WRITE:
		if (len < sizeof(*gph) + sizeof(*gsw))
			return -EINVAL;
		gsw = (struct gsmd_sms_write *) ((char *) gph + sizeof(*gph));
		if (gsw->stat > 4)
			return -EINVAL;

		if (gu->gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT) {
			atcmd_len = sprintf(buf, "AT+CMGW=\"%s\"\n%.*s",
					gsw->sms.addr.number,
					gsw->sms.payload.length,
					gsw->sms.payload.data);	/* FIXME */
		} else {
			atcmd_len = sprintf(buf, "AT+CMGW=%i,%i\n",
					sms_pdu_make_smssubmit(NULL,
						&gsw->sms) - 1, gsw->stat);
			atcmd_len += sms_pdu_make_smssubmit(buf + atcmd_len,
					&gsw->sms) * 2;
		}
		buf[atcmd_len ++] = 26;	/* ^Z ends the message */
		buf[atcmd_len ++] = 0;

		cmd = atcmd_fill(buf, atcmd_len, &sms_write_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_DELETE:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

		atcmd_len = sprintf(buf, "AT+CMGD=%i", *value);

		cmd = atcmd_fill(buf, atcmd_len + 1,
				&sms_delete_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_DELETE_ALL:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));
		if(*value<1 || *value>6)
			return -EINVAL;

#ifdef GSM_MODULE_SIM300

		/*
		AT+CMGDA=<type>
		<type>:
		1) If text mode:
			"DEL READ"		delete all read messages
			"DEL UNREAD"	delete all unread messages
			"DEL SENT"		delete all sent SMS
			"DEL UNSENT"	delete all unsent SMS
			"DEL INBOX"		delete all received SMS
			"DEL ALL"		delete all SMS
		2) if PDU mode :
			1 delete all read messages
			2 delete all unread messages
			3 delete all sent SMS
			4 delete all unsent SMS
			5 delete all received SMS
			6 delete all SMS
		*/
		if (gu->gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT)
			atcmd_len = sprintf(buf, "AT+CMGDA=\"%s\"",	gsmd_cmgda_type[*value-1]);
		else
			atcmd_len = sprintf(buf, "AT+CMGDA=%i", *value);

#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)||defined(GSM_MODULE_ULC2)
		/*
		AT+CMGD=<index>[,<delflag>]
		<delflag>: an integer indicating multiple message deletion request as follows:
		0 (or omitted) Delete the message specified in <index>
		1 Delete all read messages from preferred message storage,
		  leaving unread messages and stored mobile originated messages (whether sent or not) untouched
		2 Delete all read messages from preferred message storage and sent mobile originated messages,
		  leaving unread messages and unsent mobile originated messages untouched
		3 Delete all read messages from preferred message storage, sent and unsent mobile originated messages
		  leaving unread messages untouched.
		4 Delete all messages from preferred message storage including unread messages.
		*/
		atcmd_len = sprintf(buf, "AT+CMGD=1,4");
#endif

		cmd = atcmd_fill(buf, atcmd_len + 1,
				&sms_delete_all_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_GET_MSG_STORAGE:
		cmd = atcmd_fill("AT+CPMS?", 8 + 1, usock_cpms_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_SET_MSG_STORAGE:
		if (len < sizeof(*gph) + 3 * sizeof(enum ts0705_mem_type))
			return -EINVAL;
		storage = (enum ts0705_mem_type *)((char *) gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CPMS=\"%s\",\"%s\",\"%s\"",
				ts0705_memtype_name[storage[0]],
				ts0705_memtype_name[storage[1]],
				ts0705_memtype_name[storage[2]]);
		cmd = atcmd_fill(buf, atcmd_len + 1, NULL, gu, gph->id, NULL);
		break;

	case GSMD_SMS_GET_SERVICE_CENTRE:
		cmd = atcmd_fill("AT+CSCA?", 8 + 1, &usock_get_smsc_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_SET_SERVICE_CENTRE:
		if (len < sizeof(*gph) + sizeof(struct gsmd_addr))
			return -EINVAL;
		ga = (struct gsmd_addr *) ((char *) gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CSCA=\"%s\",%i",
						ga->number, ga->type);
		cmd = atcmd_fill(buf, atcmd_len + 1, &usock_set_smsc_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_GET_NEW_MSG_INDICATIONS:
		cmd = atcmd_fill("AT+CNMI?", 8 + 1, &usock_get_cnmi_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_SET_NEW_MSG_INDICATIONS:
		if (len < sizeof(*gph) + sizeof(struct gsmd_sms_cnmi))
			return -EINVAL;
		gsc = (struct gsmd_sms_cnmi *) ((char *) gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CNMI=%i,%i,%i,%i,%i",
				gsc->mode, gsc->mt, gsc->bm, gsc->ds, gsc->bfr);
		cmd = atcmd_fill(buf, atcmd_len + 1, &usock_set_cnmi_cb, gu, gph->id, NULL);
		break;

	case GSMD_SMS_SET_FMT:
		if (len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *) gph + sizeof(*gph));
		
		atcmd_len = sprintf(buf, "AT+CMGF=%i", *value);
		cmd = atcmd_fill(buf, atcmd_len + 1, &sms_set_fmt_cb, gu, gph->id, NULL);
		break;

	default:
		return -ENOSYS;
	}

	if (!cmd)
		return -ENOMEM;

	//gsmd_log(GSMD_DEBUG, "%s\n", cmd ? cmd->buf : 0);
	return atcmd_submit(gu->gsmd, cmd);
}

/* main unix socket Cell Broadcast receiver */
int usock_rcv_cb(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	struct gsmd_atcmd *cmd;

	switch (gph->msg_subtype) {
	case GSMD_CB_SUBSCRIBE:
		cmd = atcmd_fill("AT+CSCB=1", 9 + 1, NULL, gu->gsmd, gph->id, NULL);
		break;
	case GSMD_CB_UNSUBSCRIBE:
		cmd = atcmd_fill("AT+CSCB=0", 9 + 1, NULL, gu->gsmd, gph->id, NULL);
		break;
	default:
		return -ENOSYS;
	}

	if (!cmd)
		return -ENOMEM;

	return atcmd_submit(gu->gsmd, cmd);
}
extern int sms_readed;
/* Unsolicited messages related to SMS / CB */
static int cmti_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	char memstr[3];
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_SMS, sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

    if(sms_readed == 1)
    {
	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	if (sscanf(param, "\"%2[A-Z]\",%i", memstr, &aux->u.sms.index) < 2) {
		FREE(ucmd);
		return -EINVAL;
	}

	aux->u.sms.inlined = 0;
	aux->u.sms.memtype = parse_memtype(memstr);
		return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_SMS);
	}
	else
	{
		
		return 0;
		
	}
}

static int cmt_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	/* TODO: TEXT mode */
	u_int8_t pdu[SMS_MAX_PDU_SIZE];
	const char *cr = NULL;
	int i;
	char tmp[64];
	struct gsm_extrsp *er;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_sms_list *msg;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_SMS, sizeof(struct gsmd_evt_auxdata) +
			sizeof(struct gsmd_sms_list));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	msg = (struct gsmd_sms_list *) aux->data;

	cr = strchr(param, '\n');

	if (!cr) {
		FREE(ucmd);
		return -EAGAIN;
	}

	strncpy(tmp, param, (cr-param));
	tmp[(cr-param)] = '\0';

	er = extrsp_parse(tmp);

	if ( !er ) {
		FREE(ucmd);
		return -ENOMEM;
	}

	//extrsp_dump(er);	

	if ( er->num_tokens == 2 && 
			er->tokens[0].type == GSMD_ECMD_RTT_EMPTY &&
			er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC ) {

		aux->u.sms.alpha[0] = '\0';
		len = er->tokens[1].u.numeric; 
	}
	else if ( er->num_tokens == 2 && 
			er->tokens[0].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC ) {

		strcpy(aux->u.sms.alpha, er->tokens[0].u.string);
		len = er->tokens[1].u.numeric; 
	}
	else {
		FREE(ucmd);
		FREE(er);
		return -EINVAL;
	}

	cr ++;
	for (i = 0; cr[0] >= '0' && cr[1] >= '0' && i < SMS_MAX_PDU_SIZE;
			i ++) {
		if (sscanf(cr, "%2hhX", &pdu[i]) < 1) {
			gsmd_log(GSMD_DEBUG, "malformed input (%i)\n", i);
			FREE(ucmd);
			return -EINVAL;
		}
		cr += 2;
	}

	aux->u.sms.inlined = 1;
	if (sms_pdu_to_msg(msg, pdu, len, i, 0)) {
		gsmd_log(GSMD_DEBUG, "malformed PDU\n");
		FREE(ucmd);
		return -EINVAL;
	}

	return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_SMS);
}

static int cbmi_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	char memstr[3];
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_CBM, sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	if (sscanf(param, "\"%2[A-Z]\",%i", memstr, &aux->u.cbm.index) < 2) {
		FREE(ucmd);
		return -EINVAL;
	}

	aux->u.cbm.inlined = 0;
	aux->u.cbm.memtype = parse_memtype(memstr);

	return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_CBM);
}

static int cbm_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	/* TODO: TEXT mode */
	u_int8_t pdu[CBM_MAX_PDU_SIZE];
	char *cr;
	int i;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_cbm *msg;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_CBM, sizeof(struct gsmd_evt_auxdata) +
			sizeof(struct gsmd_cbm));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	msg = (struct gsmd_cbm *) aux->data;

	len = strtoul(param, &cr, 10);
	if (cr[0] != '\n') {
		FREE(ucmd);
		return -EAGAIN;
	}

	cr ++;
	for (i = 0; cr[0] >= '0' && cr[1] >= '0' && i < CBM_MAX_PDU_SIZE;
			i ++) {
		if (sscanf(cr, "%2hhX", &pdu[i]) < 1) {
			//gsmd_log(GSMD_DEBUG, "malformed input (%i)\n", i);
			FREE(ucmd);
			return -EINVAL;
		}
		cr += 2;
	}

	aux->u.cbm.inlined = 1;
	if (cbs_pdu_to_msg(msg, pdu, len, i)) {
		//gsmd_log(GSMD_DEBUG, "malformed PDU\n");
		FREE(ucmd);
		return -EINVAL;
	}

	return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_CBM);
}

static int cdsi_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	char memstr[3];
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_DS, sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	if (sscanf(param, "\"%2[A-Z]\",%i", memstr, &aux->u.ds.index) < 2) {
		FREE(ucmd);
		return -EINVAL;
	}

	aux->u.ds.inlined = 0;
	aux->u.ds.memtype = parse_memtype(memstr);

	return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_DS);
}

static int cds_parse(const char *buf, int len, const char *param,
		struct gsmd *gsmd)
{
	/* TODO: TEXT mode */
	u_int8_t pdu[SMS_MAX_PDU_SIZE];
	char *cr;
	int i;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_sms_list *msg;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT,
			GSMD_EVT_IN_DS, sizeof(struct gsmd_evt_auxdata) +
			sizeof(struct gsmd_sms_list));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	msg = (struct gsmd_sms_list *) aux->data;

	len = strtoul(param, &cr, 10);
	if (cr[0] != '\n') {
		FREE(ucmd);
		return -EAGAIN;
	}

	cr ++;
	for (i = 0; cr[0] >= '0' && cr[1] >= '0' && i < SMS_MAX_PDU_SIZE;
			i ++) {
		if (sscanf(cr, "%2hhX", &pdu[i]) < 1) {
			//gsmd_log(GSMD_DEBUG, "malformed input (%i)\n", i);
			FREE(ucmd);
			return -EINVAL;
		}
		cr += 2;
	}

	aux->u.ds.inlined = 1;
	if (sms_pdu_to_msg(msg, pdu, len, i, 1)) {
		//gsmd_log(GSMD_DEBUG, "malformed PDU\n");
		FREE(ucmd);
		return -EINVAL;
	}

	return usock_evt_send(gsmd, ucmd, GSMD_EVT_IN_DS);
}


static const struct gsmd_unsolicit gsm0705_unsolicit[] = {
	{ "+CMTI",	&cmti_parse },	/* SMS Deliver Index (stored in ME/TA)*/
	{ "+CMT",	&cmt_parse },	/* SMS Deliver to TE */
	{ "+CBMI",	&cbmi_parse },	/* Cell Broadcast Message Index */
	{ "+CBM",	&cbm_parse },	/* Cell Broadcast Message */
	{ "+CDSI",	&cdsi_parse },	/* SMS Status Report */
	{ "+CDS",	&cds_parse },	/* SMS Status Index (stored in ME/TA) */
};

int sms_cb_init(struct gsmd *gsmd, int reset)
{
	struct gsmd_atcmd *atcmd;
	char buffer[10];

	if(0 == reset)
		unsolicited_register_array(gsm0705_unsolicit, SIZE_OF_ARRAY(gsm0705_unsolicit));

	/* If text mode, set the encoding */
	if (gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT) {
		atcmd = atcmd_fill("AT+CSCS=\"IRA\"", 13 + 1, NULL, gsmd, 0, NULL);
		if (!atcmd)
			return -ENOMEM;
		atcmd_submit(gsmd, atcmd);
	}
	else
	{
		atcmd = atcmd_fill("AT+CSCS=\"UCS2\"", 14 + 1, NULL, gsmd, 0, NULL);
		if (!atcmd)
			return -ENOMEM;
		atcmd_submit(gsmd, atcmd);
	}
	
	/* Switch into desired mode (Section 3.2.3) */
	atcmd = atcmd_fill(buffer, snprintf(buffer, sizeof(buffer),
				"AT+CMGF=%i",
				(gsmd->flags & GSMD_FLAG_SMS_FMT_TEXT) ?
				GSMD_SMS_FMT_TEXT : GSMD_SMS_FMT_PDU) + 1,
			NULL, gsmd, 0, NULL);
	if (!atcmd)
		return -ENOMEM;

	return atcmd_submit(gsmd, atcmd);
}

/* Called everytime the phone registers to the network and we want to start
 * receiving messages.  */
int sms_cb_network_init(struct gsmd *gsmd)
{
	int ret = 0;

	ret |= gsmd_simplecmd(gsmd, "AT+CSMS=0");

	/*
	 * Set the New Message Indications properties to values that are
	 * likely supported.  We will get a:
	 * +CMTI on a new incoming SMS,
	 * +CBM on a new incoming CB,
	 * +CDS on an SMS status report.
	 *
	 * FIXME: ask for supported +CNMI values first.
	 */
	ret |= gsmd_simplecmd(gsmd, "AT+CNMI=2,1,2,1,0");
	//ret |= gsmd_simplecmd(gsmd, "AT+CNMI=2,2,2,1,0");

	return ret;
}
