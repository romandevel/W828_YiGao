/* gsmd unix domain socket handling
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
//#include "ctype.h"
//#include "socket.h"


#include "gsmd.h"
#include "usock.h"
#include "gsmd_select.h"
#include "atcmd.h"
#include "extrsp.h"
#include "ts0707.h"
#include "gsmdsms.h"


extern struct gsmd_timer * discard_timer2(struct gsmd *g);



struct gsmd_ucmd *ucmd_alloc(int extra_size)
{
	return (struct gsmd_ucmd*)MALLOC(sizeof(struct gsmd_ucmd) + extra_size);
}

void usock_cmd_enqueue(struct gsmd_ucmd *ucmd, struct gsmd_user *gu)
{
	//DEBUGP("enqueueing usock cmd %p for user %p\n", ucmd, gu);

	/* add to per-user list of finished cmds */
	llist_add_tail(&ucmd->list, &gu->finished_ucmds);

	/* mark socket of user as we-want-to-write */
	gu->gfd.when |= GSMD_FD_WRITE;
}



static int null_cmd_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//gsmd_log(GSMD_DEBUG, "null cmd cb\n");
	return 0;
}


/* callback for completed passthrough gsmd_atcmd's */
static int usock_cmd_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int rlen = strlen(resp)+1;
	struct gsmd_ucmd *ucmd = ucmd_alloc(rlen);

	//DEBUGP("entering(cmd=%p, gu=%p)\n", cmd, gu);

	if (!ucmd)
		return -ENOMEM;
	
	/* FIXME: pass error values back somehow */
	ucmd->hdr.version = GSMD_PROTO_VERSION;
	ucmd->hdr.msg_type = GSMD_MSG_PASSTHROUGH;
	ucmd->hdr.msg_subtype = GSMD_PASSTHROUGH_RESP;
	ucmd->hdr.len = rlen;
	ucmd->hdr.id = cmd->id;
	memcpy(ucmd->buf, resp, ucmd->hdr.len);

	usock_cmd_enqueue(ucmd, gu);

	return 0;
}

/* handle gprs data received */
int usock_gdata_enqueue(struct gsmd *g, const void *data, int rlen)
{
	struct gsmd_user *gu;
	struct gsmd_ucmd *ucmd = ucmd_alloc(rlen);

	//DEBUGP("entering(cmd=%p, gu=%p)\n", cmd, gu);

	if (!ucmd)
		return -ENOMEM;

	llist_for_each_entry(gu, struct gsmd_user, &g->users, list) {
		if (gu->gfd.fd  == USER_ID_GPRS) {

			ucmd->hdr.version = GSMD_PROTO_VERSION;
			ucmd->hdr.msg_type = GSMD_MSG_DATACALL;
			ucmd->hdr.msg_subtype = GSMD_DATACALL_TRANSFER;
			ucmd->hdr.len = rlen;
			ucmd->hdr.id = 0;
			memcpy(ucmd->buf, data, rlen);
			usock_cmd_enqueue(ucmd, gu);
			
			break;
		}
	}
	return 0;
}


typedef int usock_msg_handler(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len);

static int usock_rcv_passthrough(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	struct gsmd_atcmd *cmd;
	cmd = atcmd_fill((char *)gph+sizeof(*gph), gph->len, &usock_cmd_cb, gu, gph->id, NULL);
	if (!cmd)
		return -ENOMEM;

	//DEBUGP("submitting cmd=%p, gu=%p\n", cmd, gu);

	return atcmd_submit(gu->gsmd, cmd);
}

static int usock_rcv_event(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	u_int32_t *evtmask = (u_int32_t *) ((char *)gph + sizeof(*gph));

	if (len < sizeof(*gph) + sizeof(u_int32_t))
		return -EINVAL;

	if (gph->msg_subtype != GSMD_EVT_SUBSCRIPTIONS)
		return -EINVAL;

	gu->subscriptions = *evtmask;
	return 0;
}



static int voicecall_volume_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_VOL_SET,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_audiochl_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_AUDIOCHL_SET,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_micgain_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_MICGAIN_SET,
			cmd->id, sizeof(ret), &ret);
}

static int voicecall_mute_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_MUTE_SET,
			cmd->id, sizeof(ret), &ret);
}



static int voicecall_get_stat_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp) 
{
	struct gsmd_user *gu = ctx;
	struct gsmd_call_status gcs;
	struct gsm_extrsp *er;

	//DEBUGP("resp: %s\n", resp);


	gcs.is_last = (cmd->ret == 0)? 1:0;

	if(!strncmp(resp, "+CLCC: ", 7))
	{
		er = extrsp_parse(resp);
		
		if ( !er )
			return -ENOMEM;
	
		if ( er->num_tokens == 7 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[3].type == GSMD_ECMD_RTT_NUMERIC && 
			er->tokens[4].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[5].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[6].type == GSMD_ECMD_RTT_NUMERIC ) {
			/*
			 * [+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,
			 * <number>,<type>[,<alpha>]]
			 * [<CR><LF>+CLCC: <id2>,<dir>,<stat>,<mode>,<mpty>[,
			 * <number>,<type>[,<alpha>]]
			 * [...]]]
			 */

			gcs.idx = er->tokens[0].u.numeric;
			gcs.dir = er->tokens[1].u.numeric;
			gcs.stat = er->tokens[2].u.numeric;
			gcs.mode = er->tokens[3].u.numeric;
			gcs.mpty = er->tokens[4].u.numeric;
			strlcpy(gcs.number, er->tokens[5].u.string, GSMD_ADDR_MAXLEN+1);
			gcs.type = er->tokens[6].u.numeric;
		}
		else if ( er->num_tokens == 8 &&
				er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[3].type == GSMD_ECMD_RTT_NUMERIC && 
				er->tokens[4].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[5].type == GSMD_ECMD_RTT_STRING &&
				er->tokens[6].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[7].type == GSMD_ECMD_RTT_STRING ) {

			/*
			 * [+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,
			 * <number>,<type>[,<alpha>]]
			 * [<CR><LF>+CLCC: <id2>,<dir>,<stat>,<mode>,<mpty>[,
			 * <number>,<type>[,<alpha>]]
			 * [...]]]
			 */

			gcs.idx = er->tokens[0].u.numeric;
			gcs.dir = er->tokens[1].u.numeric;
			gcs.stat = er->tokens[2].u.numeric;
			gcs.mode = er->tokens[3].u.numeric;
			gcs.mpty = er->tokens[4].u.numeric;
			strlcpy(gcs.number, er->tokens[5].u.string, GSMD_ADDR_MAXLEN+1);
			gcs.type = er->tokens[6].u.numeric;
			strlcpy(gcs.alpha, er->tokens[7].u.string, GSMD_ALPHA_MAXLEN+1);
		}
		else {
			//DEBUGP("Invalid Input : Parse error\n");
			gcs.idx = -1;
		}
		
		FREE(er);
	}
	else if (!strncmp(resp, "OK", 2) || !strncmp(resp, "Timeout", 7) || !strncmp(resp, "ERROR", 5)) {
		/* No existing call */
		gcs.idx = 0;
		gcs.stat = GSMD_CALL_STAT_NONE;
		
		gcs.is_last = 1;
	}
	else if ( !strncmp(resp, "+CME", 4) ) {
		/* +CME ERROR: <err> */
		//DEBUGP("+CME error\n");
		
		//char *strpbrk(const char *s1, const char *s2),strpbrk得到s1中首次出现的也是s2中字符的位置指针
		gcs.idx = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == gcs.idx)
			gcs.idx = -100;
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		gcs.idx = -1;
		//return -EINVAL;
	}
	
	return gsmd_ucmd_submit(gu, GSMD_MSG_VOICECALL, GSMD_VOICECALL_GET_STAT,
			cmd->id, sizeof(gcs), &gcs);
}

static int voicecall_ctrl_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp) 
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;
	
	//DEBUGP("resp: %s\n", resp);
	
	if ( !strncmp(resp, "+CME", 4) ) {
		/* +CME ERROR: <err> */
		//DEBUGP("+CME error\n");
		ret = atoi(strpbrk(resp, "0123456789"));
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_VOICECALL, GSMD_VOICECALL_CTRL,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_fwd_dis_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp) 
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_FWD_DIS,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_fwd_en_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp) 
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_FWD_EN,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_fwd_stat_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsm_extrsp *er;
	struct gsmd_call_fwd_stat gcfs;
	
	//DEBUGP("resp: %s\n", resp);

	gcfs.is_last = (cmd->ret == 0)? 1:0;

	if(!strncmp(resp, "+CCFC: ", 7))
	{
		er = extrsp_parse(resp);
		if ( !er )
			return -ENOMEM;

		if ( er->num_tokens == 2 &&
				er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC ) {

			/*
			 * +CCFC: <status>,<class1>[,<number>,<type>
			 * [,<subaddr>,<satype>[,<time>]]][
			 * <CR><LF>+CCFC: <status>,<class2>[,<number>,<type>
			 * [,<subaddr>,<satype>[,<time>]]]
			 * [...]]
			 */

			gcfs.status = er->tokens[0].u.numeric;
			gcfs.classx = er->tokens[1].u.numeric;
			gcfs.addr.number[0] = '\0';
		}
		else if ( er->num_tokens == 4 &&
				er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[2].type == GSMD_ECMD_RTT_STRING &&
				er->tokens[3].type == GSMD_ECMD_RTT_NUMERIC ) {
			
			gcfs.status = er->tokens[0].u.numeric;
			gcfs.classx = er->tokens[1].u.numeric;
			strlcpy(gcfs.addr.number, er->tokens[2].u.string, GSMD_ADDR_MAXLEN+1);
			gcfs.addr.type = er->tokens[3].u.numeric;
		}
		else if ( er->num_tokens == 7 &&
				er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[2].type == GSMD_ECMD_RTT_STRING  &&
				er->tokens[3].type == GSMD_ECMD_RTT_NUMERIC && 
				er->tokens[4].type == GSMD_ECMD_RTT_STRING  &&
				er->tokens[5].type == GSMD_ECMD_RTT_EMPTY   &&
				er->tokens[6].type == GSMD_ECMD_RTT_NUMERIC ) {
			
			gcfs.status = er->tokens[0].u.numeric;
			gcfs.classx = er->tokens[1].u.numeric;
			strlcpy(gcfs.addr.number, er->tokens[2].u.string, GSMD_ADDR_MAXLEN+1);
			gcfs.addr.type = er->tokens[3].u.numeric;
			gcfs.time = er->tokens[6].u.numeric;
		}
		else
		{
			gcfs.is_last = -1;
		}
		
		FREE(er);
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		gcfs.is_last = -1;
		//return -EINVAL;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_VOICECALL, GSMD_VOICECALL_FWD_STAT,
			cmd->id, sizeof(gcfs), &gcfs);
}


static int voicecall_fwd_reg_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_FWD_REG,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_fwd_eras_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_FWD_ERAS,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_colp_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_COLP_SET,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_clip_set_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_CLIP_SET,
			cmd->id, sizeof(ret), &ret);
}


static int voicecall_dial_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	struct gsmd_user *gu = ctx;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT, GSMD_EVT_CALL_STAT,
					     sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;
		
	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	if(0 == ret)
	{
		aux->u.csurc.stat = GSMD_CALL_STAT_DIALING;
	}
	else
	{
		aux->u.csurc.stat = GSMD_CALL_STAT_NONE;
		//gsmd_simplecmd(gu->gsmd, "ATH");
	}


	usock_evt_send(gu->gsmd, ucmd, GSMD_EVT_CALL_STAT);

	//resp:OK,NO CARRIER
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_DIAL,
			cmd->id, sizeof(ret), &ret);
}


#if 1
static int voicecall_hangup_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	struct gsmd_user *gu = ctx;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT, GSMD_EVT_CALL_STAT,
					     sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;
	if(0 == ret)
	{
		aux->u.csurc.stat = GSMD_CALL_STAT_NONE;
		usock_evt_send(gu->gsmd, ucmd, GSMD_EVT_CALL_STAT);
	}
	else
	{
		FREE(ucmd);
	}
	
	//resp:OK
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_HANGUP,
			cmd->id, sizeof(ret), &ret);
}

#else

static int voicecall_hangup_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	struct gsmd_user *gu = ctx;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT, GSMD_EVT_CALL_STAT,
					     sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;

	if(0 == ret)
		aux->u.csurc.stat = GSMD_CALL_STAT_NONE;
	else if(!strncmp(resp, "+CME ERROR", 10) || 
			!strncmp(resp, "+CMS ERROR", 10) || 
			!strncmp(resp, "ERROR", 5))
		aux->u.csurc.stat = GSMD_CALL_STAT_ERROR;
	else
		aux->u.csurc.stat = GSMD_CALL_STAT_UNKNOWN;

	usock_evt_send(gu->gsmd, ucmd, GSMD_EVT_CALL_STAT);

	//resp:OK/+CME ERROR/+CMS ERROR/ERROR/Timeout
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_HANGUP,
			cmd->id, sizeof(ret), &ret);
}

#endif



#if 1
static int voicecall_answer_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	struct gsmd_user *gu = ctx;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT, GSMD_EVT_CALL_STAT,
					     sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;

	if(0 == ret)
		aux->u.csurc.stat = GSMD_CALL_STAT_ACTIVE;
	else if((!strncmp(resp, "NO CARRIER", 10)) /*|| (ret == 3)*/)
		aux->u.csurc.stat = GSMD_CALL_STAT_NONE;
	else
		aux->u.csurc.stat = GSMD_CALL_STAT_ERROR;
		
	usock_evt_send(gu->gsmd, ucmd, GSMD_EVT_CALL_STAT);

	//resp:OK,NO CARRIER,+CME ERROR: 3
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_ANSWER,
			cmd->id, sizeof(ret), &ret);
}

#else

static int voicecall_answer_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	struct gsmd_user *gu = ctx;
	struct gsmd_evt_auxdata *aux;
	struct gsmd_ucmd *ucmd = usock_build_event(GSMD_MSG_EVENT, GSMD_EVT_CALL_STAT,
					     sizeof(struct gsmd_evt_auxdata));

	if (!ucmd)
		return -ENOMEM;

	aux = (struct gsmd_evt_auxdata *) ucmd->buf;

	if(0 == ret)
		aux->u.csurc.stat = GSMD_CALL_STAT_ACTIVE;
	else if(!strncmp(resp, "NO CARRIER", 10))
		aux->u.csurc.stat = GSMD_CALL_STAT_NONE;
	else if(!strncmp(resp, "+CME ERROR", 10) || 
			!strncmp(resp, "+CMS ERROR", 10) || 
			!strncmp(resp, "ERROR", 5))
		aux->u.csurc.stat = GSMD_CALL_STAT_ERROR;
	else
		aux->u.csurc.stat = GSMD_CALL_STAT_UNKNOWN;

	usock_evt_send(gu->gsmd, ucmd, GSMD_EVT_CALL_STAT);

	//resp:OK/NO CARRIER/+CME ERROR/+CMS ERROR/ERROR/Timeout
	return gsmd_ucmd_submit(ctx, GSMD_MSG_VOICECALL, GSMD_VOICECALL_ANSWER,
			cmd->id, sizeof(ret), &ret);
}

#endif


static int usock_rcv_voicecall(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	struct gsmd_atcmd *cmd = NULL;
	struct gsmd_addr *ga;
	struct gsmd_dtmf *gd;
	struct gsmd_microphone_gain *gmg;
	struct gsmd_call_ctrl *gcc; 
	struct gsmd_call_fwd_reg *gcfr;
	char buf[64];
	int atcmd_len;
	int *value;

	switch (gph->msg_subtype) {
	case GSMD_VOICECALL_DIAL:
		if (len < sizeof(*gph) + sizeof(*ga))
			return -EINVAL;
		ga = (struct gsmd_addr *) ((char *)gph + sizeof(*gph));
		ga->number[GSMD_ADDR_MAXLEN] = '\0';
		
		atcmd_len = sprintf(buf, "ATD%s;", ga->number);
		cmd = atcmd_fill(buf, atcmd_len+1, &voicecall_dial_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		/* FIXME: number type! */
		break;
	case GSMD_VOICECALL_HANGUP:
		/* ATH0 is not supported by QC, we hope ATH is supported by everone */
		cmd = atcmd_fill("ATH", 4, voicecall_hangup_cb, gu, gph->id,NULL);
		if (!cmd)
			return -ENOMEM;
		return atcmd_submit_ext(gu->gsmd, cmd, 1);

        /* This command is special because it needs to be sent to
        * the MS even if a command is currently executing.  */
        //if (cmd) {
        //		return cancel_atcmd(gu->gsmd, cmd);
        //}
		break;
	case GSMD_VOICECALL_ANSWER:
        cmd = atcmd_fill("ATA", 4, &voicecall_answer_cb, gu, gph->id,NULL);
		if (!cmd)
			return -ENOMEM;
		return atcmd_submit_ext(gu->gsmd, cmd, 1);
		break;

	case GSMD_VOICECALL_DTMF:
		if (len < sizeof(*gph) + sizeof(*gd))
			return -EINVAL;

		gd = (struct gsmd_dtmf *) ((char *)gph + sizeof(*gph));
		if (len < sizeof(*gph) + sizeof(*gd) + gd->len)
			return -EINVAL;

		/* FIXME: we don't yet support DTMF of multiple digits */
		if (gd->len != 1)
			return -EINVAL;

		atcmd_len = sprintf(buf, "AT+VTS=%c", gd->dtmf[0]);
		cmd = atcmd_fill(buf, atcmd_len + 1, &usock_cmd_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		return atcmd_submit_ext(gu->gsmd, cmd, 1);
		break;

	case GSMD_VOICECALL_VOL_SET:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

		atcmd_len = sprintf(buf, "AT+CLVL=%i", *value);
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_volume_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;

	case GSMD_VOICECALL_AUDIOCHL_SET:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

#ifdef GSM_MODULE_SIM300
		/*
		Swap The Audio Channels
		AT+CHFA=[<n>]
		<n>:0 Normal audio channel(default)
			1 Aux audio channel
		*/
		atcmd_len = sprintf(buf, "AT+CHFA=%i", *value);

#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)
		/*
		Set audio mode. We have three audio mode , normal, loud speaker and handset.
		AT+ ESAM = <mode>
		<mode>:0 normal
			   1 handset
			   2 loudspeaker
		*/
		if(1 == *value)*value=2;
		atcmd_len = sprintf(buf, "AT+ESAM=%i", *value);
#endif
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_audiochl_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;

	case GSMD_VOICECALL_MICGAIN_SET:
		if(len < sizeof(*gph) + sizeof(*gmg))
			return -EINVAL;
		gmg = (struct gsmd_microphone_gain *) ((char *)gph + sizeof(*gmg));

#ifdef GSM_MODULE_SIM300
		/*
		Change The Microphone Gain Level.
		AT+CMIC= <channel>,<gainlevel>
		<channel>:0 Main Microphone
				  1 Aux Microphone
		<gainlevel> int: 0-15
		*/
		atcmd_len = sprintf(buf, "AT+CMIC=%i,%i", gmg->channel, gmg->gainlevel);
		
#elif defined(GSM_MODULE_MD231)
		/*
		Set the microphone volume.
		AT+CMIC=<level>
		<level> the microphone volume level, range from 0~6.
		*/
		atcmd_len = sprintf(buf, "AT+CMIC=%i", gmg->gainlevel);
#endif
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_micgain_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;
		
	case GSMD_VOICECALL_MUTE_SET:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

		atcmd_len = sprintf(buf, "AT+CMUT=%i", *value);
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_mute_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;

	case GSMD_VOICECALL_GET_STAT:
		cmd = atcmd_fill("AT+CLCC", 7+1, &voicecall_get_stat_cb, 
				 gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;
	case GSMD_VOICECALL_CTRL:
		if (len < sizeof(*gph) + sizeof(*gcc))
			return -EINVAL;

		gcc = (struct gsmd_call_ctrl *) ((char *)gph + sizeof(*gph));

		atcmd_len = 1 + strlen("AT+CHLD=") + 2;
		cmd = atcmd_fill("AT+CHLD=", atcmd_len, &voicecall_ctrl_cb,
				 gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;

		switch (gcc->proc) {
			case GSMD_CALL_CTRL_R_HLDS:
			case GSMD_CALL_CTRL_UDUB:
				sprintf(cmd->buf, "AT+CHLD=%d", 0);
				break;
			case GSMD_CALL_CTRL_R_ACTS_A_HLD_WAIT:
				sprintf(cmd->buf, "AT+CHLD=%d", 1);
				break;
			case GSMD_CALL_CTRL_R_ACT_X:
				sprintf(cmd->buf, "AT+CHLD=%d%d", 1, gcc->idx);
				break;
			case GSMD_CALL_CTRL_H_ACTS_A_HLD_WAIT:
				sprintf(cmd->buf, "AT+CHLD=%d", 2);
				break;
			case GSMD_CALL_CTRL_H_ACTS_EXCEPT_X:
				sprintf(cmd->buf, "AT+CHLD=%d%d", 2, gcc->idx);
				break;
			case GSMD_CALL_CTRL_M_HELD:
				sprintf(cmd->buf, "AT+CHLD=%d", 3);
				break;
			default:
				return -EINVAL;
		}

		break;
	case GSMD_VOICECALL_FWD_DIS:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		
		value = (int *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,0", *value);

		atcmd_len = 1 + strlen("AT+CCFC=") + strlen(buf);
		cmd = atcmd_fill("AT+CCFC=", atcmd_len,
				 &voicecall_fwd_dis_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CCFC=%s", buf);
		break;
	case GSMD_VOICECALL_FWD_EN:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		
		value = (int *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,1", *value);

		atcmd_len = 1 + strlen("AT+CCFC=") + strlen(buf);
		cmd = atcmd_fill("AT+CCFC=", atcmd_len,
				 &voicecall_fwd_en_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CCFC=%s", buf);
		break;
	case GSMD_VOICECALL_FWD_STAT:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;

		value = (int *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,2", *value);

		atcmd_len = 1 + strlen("AT+CCFC=") + strlen(buf);
		cmd = atcmd_fill("AT+CCFC=", atcmd_len,
				 &voicecall_fwd_stat_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CCFC=%s", buf);
		break;
	case GSMD_VOICECALL_FWD_REG:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		
		gcfr = (struct gsmd_call_fwd_reg *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,3,\"%s\"", gcfr->reason, gcfr->addr.number);

		atcmd_len = 1 + strlen("AT+CCFC=") + strlen(buf);
		cmd = atcmd_fill("AT+CCFC=", atcmd_len,
				 &voicecall_fwd_reg_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CCFC=%s", buf);
		break;
	case GSMD_VOICECALL_FWD_ERAS:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		
		value = (int *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,4", *value);

		atcmd_len = 1 + strlen("AT+CCFC=") + strlen(buf);
		cmd = atcmd_fill("AT+CCFC=", atcmd_len,
				 &voicecall_fwd_eras_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CCFC=%s", buf);
		break;

	case GSMD_VOICECALL_COLP_SET:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

		atcmd_len = sprintf(buf, "AT+COLP=%i", *value);
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_colp_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;

	case GSMD_VOICECALL_CLIP_SET:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		value = (int *) ((char *)gph + sizeof(*gph));

		atcmd_len = sprintf(buf, "AT+CLIP=%i", *value);
		cmd = atcmd_fill(buf, atcmd_len + 1, &voicecall_clip_set_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;

	default:
		return -EINVAL;
	}

	if (cmd)
		return atcmd_submit(gu->gsmd, cmd);
	else
		return -ENOMEM;
}


static int datacall_dial_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	if(0 == ret) //GPRS CONNECT
	{
		struct gsmd_user *gu = ctx;
		
		gprs_evt_send(gu->gsmd, 1);
	}
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_DATACALL, GSMD_DATACALL_DIAL,
			cmd->id, sizeof(ret), &ret);
}


static int datacall_cgdcont_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_DATACALL, GSMD_DATACALL_CGDCONT,
			cmd->id, sizeof(ret), &ret);
}




static int usock_rcv_datacall(struct gsmd_user *gu, struct gsmd_msg_hdr *gph,
				int len)
{
	struct gsmd_atcmd *cmd = NULL;
	struct gsmd_gframe *gf;
	struct gsmd_data_connect_set *gc;
	char buf[64], *payload, *pbuf;
	int atcmd_len;
	int rc;

	switch (gph->msg_subtype) {
	case GSMD_DATACALL_DIAL:
		if (len < sizeof(*gph) + sizeof(*gc))
			return -EINVAL;
		gc = (struct gsmd_data_connect_set *) ((char *)gph + sizeof(*gph));
		//gc->addr[GSMD_ADDR_MAXLEN] = '\0';
		//gc->l2p[sizeof(gc->l2p)-1] = '\0';
		
		atcmd_len = sprintf(buf, "ATD*99#");
		cmd = atcmd_fill(buf, atcmd_len + 1, &datacall_dial_cb, gu, 2, NULL);
		if (!cmd)
			return -ENOMEM;
		rc = atcmd_submit(gu->gsmd, cmd);
		break;

	case GSMD_DATACALL_CGDCONT:
		if (len < sizeof(*gph) + 2)
			return -EINVAL;

		cmd = atcmd_fill("ATH", 3+1, &null_cmd_cb, gu, 2, NULL);
		if (!cmd)
			return -ENOMEM;
		atcmd_submit(gu->gsmd, cmd);

		cmd = atcmd_fill("ATE0", 4+1, &null_cmd_cb, gu, 2, NULL);
		if (!cmd)
			return -ENOMEM;
		atcmd_submit(gu->gsmd, cmd);

		payload = (char *)((char *)gph + sizeof(*gph));
		pbuf = buf;
		atcmd_len = strlen(payload);
		if(atcmd_len > 40)
		{
			pbuf = (char *)MALLOC(atcmd_len);
			if(!pbuf)
				return -ENOMEM;
		}
		atcmd_len = sprintf(pbuf, "AT+CGDCONT=1,\"IP\",\"%s\"", payload);
		cmd = atcmd_fill(pbuf, atcmd_len + 1, &datacall_cgdcont_cb, gu, 2, NULL);

		if(pbuf != buf)
			FREE(pbuf);

		if (!cmd)
			return -ENOMEM;
		rc = atcmd_submit(gu->gsmd, cmd);
	    break;
		
	case GSMD_DATACALL_TRANSFER:
		if (len < sizeof(*gph) + 3)	//~x~
			return -EINVAL;
			
		gf = gframe_fill(gph->data, gph->len, gu);
		if (!gf)
			return -ENOMEM;

		rc = gframe_submit(gu->gsmd, gf);
		break;
		
	default:
		return -EINVAL;
	}
	return rc;
}



const char *pin_type_names[__NUM_GSMD_PIN] = {
	/*[GSMD_PIN_READY]			= */"READY",
	/*[GSMD_PIN_SIM_PIN]		= */"SIM PIN",
	/*[GSMD_PIN_SIM_PUK]		= */"SIM PUK",
	/*[GSMD_PIN_PH_SIM_PIN]		= */"PH-SIM PIN",
	/*[GSMD_PIN_PH_FSIM_PIN]	= */"PH-FSIM PIN",
	/*[GSMD_PIN_PH_FSIM_PUK]	= */"PH-FSIM PUK",
	/*[GSMD_PIN_SIM_PIN2]		= */"SIM PIN2",
	/*[GSMD_PIN_SIM_PUK2]		= */"SIM PUK2",
	/*[GSMD_PIN_PH_NET_PIN]		= */"PH-NET PIN",
	/*[GSMD_PIN_PH_NET_PUK]		= */"PH-NET PUK",
	/*[GSMD_PIN_PH_NETSUB_PIN]	= */"PH-NETSUB PIN",
	/*[GSMD_PIN_PH_NETSUB_PUK]	= */"PH-NETSUB PUK",
	/*[GSMD_PIN_PH_SP_PIN]		= */"PH-SP PIN",
	/*[GSMD_PIN_PH_SP_PUK]		= */"PH-SP PUK",
	/*[GSMD_PIN_PH_CORP_PIN]	= */"PH-CORP PIN",
	/*[GSMD_PIN_PH_CORP_PUK]	= */"PH-CORP PUK",
	/*[GSMD_PIN_PHONE_LOCK]		= *///"PHONE LOCK",
};


const char *facility_type_names[__NUM_GSMD_PIN] = {
	/*[GSMD_FAC_SC]		= */"SC",
	/*[GSMD_FAC_AO]		= */"AO",
	/*[GSMD_FAC_OI]		= */"OI",
	/*[GSMD_FAC_OX]		= */"OX",
	/*[GSMD_FAC_AI]		= */"AI",
	/*[GSMD_FAC_IR]		= */"IR",
	/*[GSMD_FAC_AB]		= */"AB",
	/*[GSMD_FAC_AG]		= */"AG",
	/*[GSMD_FAC_AC]		= */"AC",
	/*[GSMD_FAC_PN]		= */"PN",
	/*[GSMD_FAC_PU]		= */"PU",
	/*[GSMD_FAC_PP]		= */"PP",
	/*[GSMD_FAC_PC]		= */"PC",
	/*[GSMD_FAC_P2]		= */"P2",
};



static int security_pin_status_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//enum gsmd_pin_type type = __NUM_GSMD_PIN;
	int type = (int)__NUM_GSMD_PIN;


	if (!strncmp(resp, "+CPIN: ", 7))
	{
		resp += 7;
		for (type=0;type<__NUM_GSMD_PIN;type++)
		{
			if(!strcmp(resp,pin_type_names[type]))
			{
				break;
			}
		}
	}
	else if (!strncmp(resp, "+CME", 4) )
	{
		type = 0 - atoi(strpbrk(resp, "0123456789"));

		/*
		+CME ERROR Codes:
			0 phone failure
			5 PH-SIM PIN required
			6 PH-FSIM PIN required
			7 PH-FSIM PUK required
			10 SIM not inserted
			11 SIM PIN required
			12 SIM PUK required
			17 SIM PIN2 required
			18 SIM PUK2 required
		*/
		switch(type)
		{
		case 0:	//GSMD_PIN_READY = 0
			type = -1000;
		case -5:
			type = (int)GSMD_PIN_PH_SIM_PIN;
			break;
		case -6:
			type = (int)GSMD_PIN_PH_FSIM_PIN;
			break;
		case -7:
			type = (int)GSMD_PIN_PH_FSIM_PUK;
			break;
		case -11:
			type = (int)GSMD_PIN_SIM_PIN;
			break;
		case -12:
			type = (int)GSMD_PIN_SIM_PUK;
			break;
		case -17:
			type = (int)GSMD_PIN_SIM_PIN2;
			break;
		case -18:
			type = (int)GSMD_PIN_SIM_PUK2;
			break;
		}
	}
	else
	{
		//DEBUGP("Invalid Input : Parse error\n");
		type = - 1001;
	}
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_GET_STATUS,
			cmd->id, sizeof(type), &type);
}


/* PIN command callback. Gets called for response to AT+CPIN cmcd */
static int security_pin_input_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;

	/* Pass a GSM07.07 CME code directly, don't issue a new PIN
	 * request because the client waits for a response to her
	 * PIN submission rather than an event.  */

	//+CME ERROR: 12	SIM PUK required
	//+CME ERROR: 16	Incorrect password
	//+CME ERROR: 18	SIM PUK2 required
	if (!strncmp(resp, "+CME", 4) )
	{
		ret = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == ret)	//+CME ERROR: 0   phone failure
			ret = -1000;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_INPUT,
			cmd->id, sizeof(ret), &ret);
}


static int security_pin_input_remain_times_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_pin_remain_times times;

#ifdef GSM_MODULE_ULC2
		if (sscanf(resp, "+XPINCNT: %i,%i,%i,%i", &times.pin, &times.pin2, &times.puk, &times.puk2) < 4)
#else
	if (sscanf(resp, "+EPINC: %i, %i, %i, %i", &times.pin, &times.pin2, &times.puk, &times.puk2) < 4)
#endif
	{
		/* TODO: Send a response */
		times.pin = -1;
	}

	return gsmd_ucmd_submit(ctx, GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES,
			cmd->id, sizeof(times), &times);
}


static int security_lock_en_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;

	//+CME ERROR: 12	SIM PUK required
	//+CME ERROR: 16	Incorrect password
	if (!strncmp(resp, "+CME", 4) )
	{
		ret = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == ret)	//+CME ERROR: 0   phone failure
			ret = -1000;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_SECURITY, GSMD_SECURITY_LOCK_EN,
			cmd->id, sizeof(ret), &ret);
}


static int security_lock_dis_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;

	//+CME ERROR: 12	SIM PUK required
	//+CME ERROR: 16	Incorrect password
	if (!strncmp(resp, "+CME", 4) )
	{
		ret = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == ret)	//+CME ERROR: 0   phone failure
			ret = -1000;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_SECURITY, GSMD_SECURITY_LOCK_DIS,
			cmd->id, sizeof(ret), &ret);
}


static int security_lock_status_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;
	

	if (!strncmp(resp, "+CME", 4) )
	{
		//+CME ERROR: 11	SIM PIN required
		//+CME ERROR: 12	SIM PUK required
		ret = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == ret)	//+CME ERROR: 0   phone failure
			ret = -1000;
	}
	else if(!strncmp(resp, "+CLCK:", 6))
	{
		//+CLCK: 1
		resp += 7;
		ret = atoi(resp);
	}
	else
	{
		ret = -1001;
	}
	
	return gsmd_ucmd_submit(gu, GSMD_MSG_SECURITY, GSMD_SECURITY_LOCK_GET_STATUS,
			cmd->id, sizeof(ret), &ret);
}


static int security_change_pwd_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;
	

	if (!strncmp(resp, "+CME", 4) )
	{
		//+CME ERROR: 3		operation not allowed
		//+CME ERROR: 12	SIM PUK required
		//+CME ERROR: 16	Incorrect password
		//+CME ERROR: 18	SIM PUK2 required
		ret = 0 - atoi(strpbrk(resp, "0123456789"));
		if(0 == ret)	//+CME ERROR: 0   phone failure
			ret = -1000;
	}
	
	return gsmd_ucmd_submit(gu, GSMD_MSG_SECURITY, GSMD_SECURITY_CHANGE_PWD,
			cmd->id, sizeof(ret), &ret);
}


static int usock_rcv_security(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, 
			 int len)
{
	struct gsmd_pin *gp = (struct gsmd_pin *) ((char *)gph + sizeof(*gph));
	struct gsmd_facility *gf;
	struct gsmd_atcmd *cmd;
	char buf[40];
	int atcmd_len;
	
	switch (gph->msg_subtype) {
	case GSMD_SECURITY_PIN_GET_STATUS:
		//AT+CPIN?
		//AT+EPIN2?
		cmd = atcmd_fill("AT+CPIN?", 8 + 1, &security_pin_status_cb, gu, gph->id, NULL);
		break;
	case GSMD_SECURITY_PIN_INPUT:
		if (len < sizeof(*gp)+sizeof(*gph))
			return -EINVAL;

		gp = (struct gsmd_pin *) ((char *)gph + sizeof(*gph));

		//AT+CPIN="1234"
		//AT+CPIN="34118706","1234"
		//at+epin2="1234"
		//at+epin2="12345678","1234"
		if(GSMD_PIN_SIM_PIN == gp->type || GSMD_PIN_SIM_PUK == gp->type)
		{
			if(GSMD_PIN_SIM_PIN == gp->type)
				atcmd_len = sprintf(buf, "AT+CPIN=\"%s\"", gp->pin);
			else
				atcmd_len = sprintf(buf, "AT+CPIN=\"%s\",\"%s\"", gp->pin, gp->newpin);
		}
		else //if(GSMD_PIN_SIM_PIN2==gp->type || GSMD_PIN_SIM_PUK2==gp->type)
		{
#ifdef GSM_MODULE_ULC2
			if(GSMD_PIN_SIM_PIN2 == gp->type)
				atcmd_len = sprintf(buf, "AT+CPIN=\"%s\"", gp->pin);
			else
				atcmd_len = sprintf(buf, "AT+CPIN=\"%s\",\"%s\"", gp->pin, gp->newpin);
#else
			if(GSMD_PIN_SIM_PIN2 == gp->type)
				atcmd_len = sprintf(buf, "AT+EPIN2=\"%s\"", gp->pin);
			else
				atcmd_len = sprintf(buf, "AT+EPIN2=\"%s\",\"%s\"", gp->pin, gp->newpin);
#endif
		}

		cmd = atcmd_fill(buf, atcmd_len+1,	&security_pin_input_cb, gu, gph->id, NULL);
		break;
	case GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES:
#ifdef GSM_MODULE_ULC2
			cmd = atcmd_fill("AT+XPINCNT", 10 + 1, &security_pin_input_remain_times_cb, gu, gph->id, NULL);
#else
		cmd = atcmd_fill("AT+EPINC", 8 + 1, &security_pin_input_remain_times_cb, gu, gph->id, NULL);
#endif
		break;

	case GSMD_SECURITY_LOCK_EN:
		if(len < sizeof(*gph) + sizeof(*gf))
			return -EINVAL;

		gf = (struct gsmd_facility *)((char *)gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CLCK=\"%s\",1,\"%s\"", 
			facility_type_names[gf->type], gf->pwd);
		cmd = atcmd_fill(buf, atcmd_len + 1, &security_lock_en_cb, gu, gph->id, NULL);
		break;

	case GSMD_SECURITY_LOCK_DIS:
		if(len < sizeof(*gph) + sizeof(*gf))
			return -EINVAL;

		gf = (struct gsmd_facility *)((char *)gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CLCK=\"%s\",0,\"%s\"", 
			facility_type_names[gf->type], gf->pwd);
		cmd = atcmd_fill(buf, atcmd_len + 1, &security_lock_dis_cb, gu, gph->id, NULL);
		break;

	case GSMD_SECURITY_LOCK_GET_STATUS:
		if(len < sizeof(*gph) + sizeof(*gf))
			return -EINVAL;

		gf = (struct gsmd_facility *)((char *)gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CLCK=\"%s\",2", facility_type_names[gf->type]);
		cmd = atcmd_fill(buf, atcmd_len + 1, &security_lock_status_cb, gu, gph->id, NULL);
		break;
	case GSMD_SECURITY_CHANGE_PWD:
		if(len < sizeof(*gph) + sizeof(*gf))
			return -EINVAL;

		//AT+CPWD=<fac>,<oldpwd>,<newpwd>
		gf = (struct gsmd_facility *)((char *)gph + sizeof(*gph));
		atcmd_len = sprintf(buf, "AT+CPWD=\"%s\",\"%s\",\"%s\"", 
			facility_type_names[gf->type], gf->pwd, gf->newpwd);
		cmd = atcmd_fill(buf, atcmd_len + 1, &security_change_pwd_cb, gu, gph->id, NULL);
		break;

	default:
		//gsmd_log(GSMD_ERROR, "unknown pin type %u\n", gph->msg_subtype);
		return -EINVAL;
	}
	if (!cmd)
		return -ENOMEM;
		
	return atcmd_submit(gu->gsmd, cmd);
}


static int phone_power_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	if (!strncmp(resp, "+CME", 4) )
	{
		if(0 == ret)		//+CME ERROR: 0   phone failure
			ret = -1000;
		else				//+CME ERROR: 10	SIM not inserted
			ret = 0 - ret;
	}

	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONE, GSMD_PHONE_POWER,
			cmd->id, sizeof(ret), &ret);
}


static int phone_power_status_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	char result = 0;

#if defined(GSM_MODULE_SIM300)||defined(GSM_MODULE_IW328)||defined(GSM_MODULE_ULC2)
	
	if (!strncmp(resp, "+CFUN: ", 7))
	{
		result = resp[7];
		if('4' == result)
			result = '0';
	}
#elif defined(GSM_MODULE_MD231)

	if (!strncmp(resp, "+EFUN:", 6))
	{
		result = resp[6];
	}
#endif

	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONE, GSMD_PHONE_POWER_STATUS,
			cmd->id, sizeof(result), &result);
}


static int phone_get_imsi_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;

	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_IMSI,
			cmd->id, strlen(resp) + 1, resp);
}



static int phone_get_manuf_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);
	if (!strncmp(resp, "+CGMI: ", 7))
		resp += 7;
	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_MANUF,
			cmd->id, strlen(resp) + 1, resp);
}

static int phone_get_model_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);
	if (!strncmp(resp, "+CGMM: ", 7))
		resp += 7;
	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_MODEL,
			cmd->id, strlen(resp) + 1, resp);
}

static int phone_get_revision_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);
	if (!strncmp(resp, "+CGMR: ", 7))
		resp += 7;
	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_REVISION,
			cmd->id, strlen(resp) + 1, resp);
}


static int phone_get_serial_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;

#if	0
	if (!strncmp(resp, "+CGSN: ", 7))
		resp += 7;
#endif

	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_SERIAL,
			cmd->id, strlen(resp) + 1, resp);
}

static int phone_get_battery_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_battery_charge gbs;
	struct gsm_extrsp *er;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);
	er = extrsp_parse(resp);
	if(!er)
		return -ENOMEM;
	/* +CBC: 0,0 */
	if((er->num_tokens == 2) &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC ) {
				gbs.bcs = er->tokens[0].u.numeric;
				gbs.bcl = er->tokens[1].u.numeric;
	}
	FREE(er);
	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_BATTERY,
		cmd -> id, sizeof(gbs), &gbs);
}

static int phone_vibrator_enable_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	int ret = cmd->ret;

	switch(ret) {
	case 0:
		//gsmd_log(GSMD_DEBUG, "Vibrator enabled\n");
		gu->gsmd->dev_state.vibrator = 1;
		break;
	default:
		//gsmd_log(GSMD_DEBUG, "AT+CVIB=1 operation failed\n");
		break;
	}
	
	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_VIB_ENABLE,
				cmd->id, sizeof(ret), &ret);
}

static int phone_vibrator_disable_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONE, GSMD_PHONE_VIB_DISABLE,
				cmd->id, sizeof(ret), &ret);
}

static int phone_detect_simcard_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	char result = 0;

#ifdef GSM_MODULE_ULC2
	//AT+CNMI?	有卡+CNMI: 1,0,0,0,0 OK，没卡+CMS ERROR: 310，拔出卡之后+CMS ERROR: 313

	if(0 == ret)
		result = '1';
	else if(310 == ret || 313 == ret)
		result = '0';

#elif defined GSM_MODULE_MD231
	if (!strncmp(resp, "+ESIMS: ", 8))
		result = resp[8];

#endif
	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONE, GSMD_PHONE_DETECT_SIMCARD,
				cmd->id, sizeof(result), &result);
}


static int usock_rcv_phone(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, 
			   int len)
{
	int *data, atcmd_len;
	char buf[64];
	struct gsmd_atcmd *cmd;


	switch (gph->msg_subtype) {
	case GSMD_PHONE_POWER:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		data = (int *)gph->data;

		#if defined(GSM_MODULE_SIM300)||defined(GSM_MODULE_IW328)||defined(GSM_MODULE_ULC2)

		atcmd_len = sprintf(buf, "AT+CFUN=%i", *data);
		#elif defined(GSM_MODULE_MD231)
		
		if(4 == *data)
			*data = 0;
		atcmd_len = sprintf(buf, "AT+EFUN=%i", *data);
		#endif
		
		cmd = atcmd_fill(buf, atcmd_len+1,
				&phone_power_cb, gu, gph->id, discard_timer2);
		break;
		
	case GSMD_PHONE_POWER_STATUS:
		#if defined(GSM_MODULE_SIM300)||defined(GSM_MODULE_IW328)||defined(GSM_MODULE_ULC2)
		
		cmd = atcmd_fill("AT+CFUN?", 8+1,
				&phone_power_status_cb, gu, gph->id, NULL);
		#elif defined(GSM_MODULE_MD231)
		
		cmd = atcmd_fill("AT+EFUN?", 8+1,
				&phone_power_status_cb, gu, gph->id, NULL);
		#endif
		break;

	case GSMD_PHONE_GET_IMSI:
		//return gsmd_ucmd_submit(gu, GSMD_MSG_PHONE, GSMD_PHONE_GET_IMSI,
		//	0, strlen(gu->gsmd->imsi) + 1, gu->gsmd->imsi);
		cmd = atcmd_fill("AT+CIMI", 7+1,
				&phone_get_imsi_cb, gu, gph->id, NULL);
		break;

	case GSMD_PHONE_GET_MANUF:
		cmd = atcmd_fill("AT+CGMI", 7+1,
				&phone_get_manuf_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_GET_MODEL:
		cmd = atcmd_fill("AT+CGMM", 7+1,
				&phone_get_model_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_GET_REVISION:
		cmd = atcmd_fill("AT+CGMR", 7+1,
				&phone_get_revision_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_GET_SERIAL:
		cmd = atcmd_fill("AT+CGSN", 7+1,
				&phone_get_serial_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_GET_BATTERY:
		cmd = atcmd_fill("AT+CBC", 6+1, &phone_get_battery_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_VIB_ENABLE:
		cmd = atcmd_fill("AT+CVIB=1", 9+1, &phone_vibrator_enable_cb, gu, gph->id, NULL);
		break;
	case GSMD_PHONE_VIB_DISABLE:
		cmd = atcmd_fill("AT+CVIB=0", 9+1, &phone_vibrator_disable_cb, gu, gph->id, NULL);
		gu->gsmd->dev_state.vibrator = 0;
		break;
	case GSMD_PHONE_DETECT_SIMCARD:
#ifdef GSM_MODULE_ULC2
			cmd = atcmd_fill("AT+CNMI?", 8+1, &phone_detect_simcard_cb, gu, gph->id, NULL);
#elif defined GSM_MODULE_MD231
		cmd = atcmd_fill("AT+ESIMS?", 9+1, &phone_detect_simcard_cb, gu, gph->id, NULL);
#endif
		break;

	default:
		return -EINVAL;
	}

	if (!cmd)
		return -ENOMEM;

	return atcmd_submit(gu->gsmd, cmd);
}


/*
#define GSMD_MODEM_POWEROFF_TIMEOUT	3

static void modem_poweroff_timeout(struct gsmd_timer *tmr, void *data) 
{
	//DEBUGP("power off timeout\n");
	//exit(0);
}

static struct gsmd_timer *modem_poweroff_timer(struct gsmd *g)
{
	struct timeval tv;
	
	tv.tv_sec = GSMD_MODEM_POWEROFF_TIMEOUT;
	tv.tv_usec = 0;
	//DEBUGP("Create power off timer\n");

	return gsmd_timer_create(&tv, &modem_poweroff_timeout, g);
}
*/


static int modem_poweroff_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_MODEM_POWEROFF,
			cmd->id, sizeof(ret), &ret);
}



static int mux_open_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_MUX_MODE,
			cmd->id, sizeof(ret), &ret);
}

static int datacall_dcl0_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
    char tempbuf[6]= {0xf9,0x03,0x73,0x01,0xd7,0xf9};//F9 03 73 01 D7 F9 
	int ret;
	
	ret = strncmp(resp,tempbuf,6);
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_DLC0_MODE,
			cmd->id, sizeof(ret), &ret);
}
static int datacall_dcl1_pn_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//F9 03 EF 15 81 11 01 00 00 00 E5 05 03 00 EE F9
    char tempbuf[16] = {0xf9,0x03,0xef,0x15,0x81,0x11,0x01,0x00,0x00,0x00,0xe5,0x05,0x03,0x00,0xee,0xf9};
	int ret;	
	ret = strncmp(resp,tempbuf,16);	   
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_DLC1_PN,
			cmd->id, sizeof(ret), &ret);
}
static int datacall_dcl2_pn_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//F9 03 EF 15 81 11 02 00 00 00 E5 05 03 00 EE F9
    char tempbuf[16]= {0xf9,0x03,0xef,0x15,0x81,0x11,0x02,0x00,0x00,0x00,0xe5,0x05,0x03,0x00,0xee,0xf9};
	int ret;	
	ret = strncmp(resp,tempbuf,16);	   
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_DLC2_PN,
			cmd->id, sizeof(ret), &ret);
}
static int datacall_dcl1_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
    char tempbuf[6]= {0xf9,0x07,0x73,0x01,0x15,0xf9};//F9 07 73 01 15 F9
	int ret;	
	ret = strncmp(resp,tempbuf,6);	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_DLC1_MODE,
			cmd->id, sizeof(ret), &ret);	
}


static int datacall_dcl2_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
    char tempbuf[6]= {0xf9,0x0b,0x73,0x01,0x92,0xf9};//F9 0B 73 01 92 F9
	int ret;
	
	ret = strncmp(resp,tempbuf,6);
  
	return gsmd_ucmd_submit(ctx, GSMD_MSG_MODEM, GSMD_OPEN_DLC2_MODE,
			cmd->id, sizeof(ret), &ret);
}




static int usock_rcv_modem(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, int len)
{
	int *power;
	struct gsmd_atcmd *cmd = NULL;

	switch (gph->msg_subtype) {
		case GSMD_MODEM_POWERON:
			break;

		case GSMD_MODEM_POWEROFF:
			if(len < sizeof(*gph) + sizeof(int))
				return -EINVAL;
			power = (int*)((char*)gph + sizeof(*gph));

#ifdef GSM_MODULE_MD231
			/*
			AT+SHUTDOWN
			OK
			SHUTDOWN SUCCESS
			*/
			cmd = atcmd_fill("AT+SHUTDOWN", 11 + 1, &modem_poweroff_cb, gu, gph->id, NULL);

#elif defined(GSM_MODULE_SIM300)

			cmd = atcmd_fill("AT+CPOWD=", 10 + 1, &modem_poweroff_cb, gu, gph->id, NULL);
			sprintf(cmd->buf, "AT+CPOWD=%d", *power);

#elif defined(GSM_MODULE_ULC2)
				cmd = atcmd_fill("AT+CPWROFF", 10 + 1, &modem_poweroff_cb, gu, gph->id, NULL);

#endif
			break;

		case GSMD_OPEN_MUX_MODE:
			cmd = atcmd_fill("AT+CMUX=0", 9 + 1, &mux_open_cb, gu, gph->id, NULL);
			break;
		case GSMD_OPEN_DLC0_MODE:
			{// F9 03 3F 01 1C F9
		   		char buf[] = {0xf9,0x03,0x3f,0x01,0x1c,0xf9,0x00};
		   		cmd = atcmd_fill(buf, 6 + 1, &datacall_dcl0_cb, gu, gph->id, discard_timer2);

/*if(1 == test_log)
{
#ifdef _GSMD_DEBUG_LOG_1
	gsmd_debug_log("\r\ntest_log:\r\n", 12, 0, NULL);
	gsmd_debug_log(cmd->buf,cmd->buflen, 0, NULL);
	gsmd_debug_log("\r\ntest_log:\r\n", 12, 0, NULL);
	gsmd_debug_log(cmd->cur,cmd->buflen, 0, NULL);
#endif

}*/

			}
		 	break;
		case GSMD_OPEN_DLC1_PN:
			{// F9 03 EF 15 83 11 01 00 07 64 FF FF 03 02 EE F9
		   		char buf[] = {0xf9,0x03,0xef,0x15,0x83,0x11,0x01,0x00,0x07,0x64,0xff,0xff,0x03,0x02,0xee,0xf9,0x00};
		   		cmd = atcmd_fill(buf, 16 + 1, &datacall_dcl1_pn_cb, gu, gph->id, discard_timer2);
			}
			break;
		case GSMD_OPEN_DLC1_MODE:
			{// F9 07 3F 01 DE F9
			    char buf[] = {0xf9,0x07,0x3f,0x01,0xde,0xf9,0x00};
			    cmd = atcmd_fill(buf, 6 + 1, &datacall_dcl1_cb, gu, gph->id, discard_timer2);
			}
		    break;
		case GSMD_OPEN_DLC2_PN:
			{// F9 03 EF 15 83 11 02 00 07 64 FF FF 03 02 EE F9
		   		char buf[] = {0xf9,0x03,0xef,0x15,0x83,0x11,0x02,0x00,0x07,0x64,0xff,0xff,0x03,0x02,0xee,0xf9,0x00};
		   		cmd = atcmd_fill(buf, 16 + 1, &datacall_dcl2_pn_cb, gu, gph->id, discard_timer2);
			}
			break;
		case GSMD_OPEN_DLC2_MODE:
			{// F9 0B 3F 01 59 F9
			    char buf[] = {0xf9,0x0b,0x3f,0x01,0x59,0xf9,0x00};
			    cmd = atcmd_fill(buf, 6 + 1, &datacall_dcl2_cb, gu, gph->id, discard_timer2);
			}
			break;

		default:
			return -EINVAL;
	}

	if (cmd)
		return atcmd_submit(gu->gsmd, cmd);
	else
		return -ENOMEM;
}


extern int mdm_reg_stat; 
static int network_register_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	if(ret == 0)
		mdm_reg_stat = 1;
	if (!strncmp(resp, "+CME", 4) )
	{
		if(0 == ret)		//+CME ERROR: 0   phone failure
			ret = -1000;
		else				//+CME ERROR: 10	SIM not inserted
			ret = 0 - ret;	//+CME ERROR: 15	SIM wrong
	}

	return gsmd_ucmd_submit(ctx, GSMD_MSG_NETWORK, GSMD_NETWORK_REGISTER,
			cmd->id, sizeof(ret), &ret);
}
static int network_re_register_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;
	if(ret == 0)
		mdm_reg_stat = 1;
	if (!strncmp(resp, "+CME", 4) )
	{
		if(0 == ret)		//+CME ERROR: 0   phone failure
			ret = -1000;
		else				//+CME ERROR: 10	SIM not inserted
			ret = 0 - ret;	//+CME ERROR: 15	SIM wrong
	}

	return gsmd_ucmd_submit(ctx, GSMD_MSG_NETWORK, GSMD_NETWORK_REGISTER,
			cmd->id, sizeof(ret), &ret);
}

static int network_deregister_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	int ret = cmd->ret;

	if (!strncmp(resp, "+CME", 4) )
	{
		if(0 == ret)		//+CME ERROR: 0   phone failure
			ret = -1000;
		else				//+CME ERROR: 10	SIM not inserted
			ret = 0 - ret;
	}

	return gsmd_ucmd_submit(ctx, GSMD_MSG_NETWORK, GSMD_NETWORK_DEREGISTER,
			cmd->id, sizeof(ret), &ret);
}


static int network_query_reg_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsm_extrsp *er;
	enum gsmd_netreg_state state = GSMD_NETREG_UNKNOWN;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);

	if (!strncmp(resp, "+CREG: ", 7))
	{
		er = extrsp_parse(resp);
		if(!er)
			return -ENOMEM;
		//extrsp_dump(er);
		/* +CREG: <n>,<stat>[,<lac>,<ci>] */
		if((er->num_tokens == 4 || er->num_tokens == 2 ) &&
				er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
				er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC ) {
					state = er->tokens[1].u.numeric;
		}

		FREE(er);
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_QUERY_REG,
		cmd->id, sizeof(state), &state);
}

static int network_vmail_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_voicemail vmail;
	struct gsm_extrsp *er;
	int rc;
	int ret = cmd->ret;

	//DEBUGP("cmd = '%s', resp: '%s'\n", cmd->buf, resp);

	if (cmd->buf[7] == '=') {
		/* response to set command */
		rc = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, 
				GSMD_NETWORK_VMAIL_SET,cmd->id, sizeof(ret), &ret);
	} else {
		/* response to get command */
		if (strncmp(resp, "+CSVM: ", 7))
			return -EINVAL;
		resp += 7;
		er = extrsp_parse(resp);
		if(!er)
			return -ENOMEM;
		if(er->num_tokens == 3 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC) {
				vmail.enable = er->tokens[0].u.numeric;
				strlcpy(vmail.addr.number, er->tokens[1].u.string, GSMD_ADDR_MAXLEN+1);
				vmail.addr.type = er->tokens[2].u.numeric;
		}
		rc = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_VMAIL_GET,
			cmd->id, sizeof(vmail), &vmail);
		FREE(er);
	}
	return rc;
}

int gsmd_ucmd_submit(struct gsmd_user *gu, u_int8_t msg_type,
		u_int8_t msg_subtype, u_int16_t id, int len, const void *data)
{
	struct gsmd_ucmd *ucmd = ucmd_alloc(len);

	if (!ucmd)
		return -ENOMEM;

	ucmd->hdr.version = GSMD_PROTO_VERSION;
	ucmd->hdr.msg_type = msg_type;
	ucmd->hdr.msg_subtype = msg_subtype;
	ucmd->hdr.len = len;
	ucmd->hdr.id = id;
	memcpy(ucmd->buf, data, len);

	usock_cmd_enqueue(ucmd, gu);
	return 0;
}

static int network_sigq_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_signal_quality gsq;
	char *comma;

	gsq.rssi = 99;
	gsq.ber = 99;
	
	if(!strncmp(resp, "+CSQ: ", 6))
	{
		gsq.rssi = atoi(resp + 6);
		comma = strchr(resp, ',');
		if (!comma ++)
		{
			//return -EIO;
		}
		else
		{
			gsq.ber = atoi(comma);
		}
	}
	
	return gsmd_ucmd_submit(ctx, GSMD_MSG_NETWORK, GSMD_NETWORK_SIGQ_GET,
			cmd->id, sizeof(gsq), &gsq);
}



char *strndup(const char *p, size_t n)
{
	size_t len;
	char *ret;

	for (len=0; len<n && p[len]; len++) ;

	ret = (char *)MALLOC(len + 1);
	if (!ret)
		return NULL;
	memcpy(ret, p, len);
	ret[len] = 0;
	return ret;
}




static int network_oper_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	const char *end, *op_name;
	int format, s, ret;
	char *buf;

	/* Format: <mode>[,<format>,<oper>] */
	/* In case we're not registered, return an empty string.  */
	if (sscanf(resp, "+COPS: %*i,%i,\"%n", &format, &s) <= 0)
		end = op_name = resp;
	else {
		/* If the phone returned the opname in a short or numeric
		 * format, then it probably doesn't know the operator's full
		 * name or doesn't support it.  Return any information we
		 * have in this case.  */
		#if 0
		if (format != 0)
			gsmd_log(GSMD_NOTICE, "+COPS response in a format "
					" different than long alphanumeric - "
					" returning as is!\n");
		#endif
		
		op_name = resp + s;
		end = strchr(op_name, '"');
		if (!end)
			end = op_name = resp;
			//return -EINVAL;
	}

	buf = strndup(op_name, end - op_name);
	ret = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_GET,
			cmd->id, end - op_name + 1, buf);
	FREE(buf);
	return ret;
}



static int network_oper_n_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	char buf[16+1] = {'\0'};
	struct gsm_extrsp *er;

	er = extrsp_parse(resp);

	if ( !er )
		return -ENOMEM;

	//extrsp_dump(er);	

	/* Format: <mode>[,<format>,<oper>] */
	if ( er->num_tokens == 1 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC ) {
		
		/* In case we're not registered, return an empty string */
		buf[0] = '\0';
	}
	else if ( er->num_tokens >= 3 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[2].type == GSMD_ECMD_RTT_STRING ) {

		strlcpy(buf, er->tokens[2].u.string, sizeof(buf));
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		return -EINVAL;
	}
	
	FREE(er);

	return gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_N_GET,
			cmd->id, sizeof(buf), buf);
}

static int network_opers_parse(const char *str, struct gsmd_msg_oper **out)
{
	int len = 0;
	struct gsm_extrsp *er;
	char buf[64];
	char *head, *tail, *ptr;
	struct gsmd_msg_oper *out2;

	if (strncmp(str, "+COPS: ", 7))
		return -EINVAL;
	/*
	 * string ",," means the begginig of extended parameters and we
	 * don't want to scan them for operators.
	 */
	ptr = strstr(str, ",,");
	if(ptr)
		ptr[0] = '\0';

	ptr = (char *) str;
	while (*str) {
		if ( *str == '(' && isdigit(*(str+1)) ) {
			len++;	
			str+=2;
		}
		else
			str++;
	}

	*out = (struct gsmd_msg_oper *)MALLOC(sizeof(struct gsmd_msg_oper) * (len + 1));

	if (!out)
		return -ENOMEM;

	out2 = *out;
	str = ptr;

	while (*str) {
		if ( *str == '(' )
			head = (char *) str;
		else if ( *str == ')' ) {
			tail = (char *) str;
			
			memset(buf, '\0', sizeof(buf));
			strncpy(buf, head+1, (tail-head-1));

			DEBUGP("buf: %s\n", buf);

			er = extrsp_parse(buf);

			if ( !er )
				return -ENOMEM;

			//extrsp_dump(er);	
				
			if ( er->num_tokens >= 4 &&
					er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
					er->tokens[1].type == GSMD_ECMD_RTT_STRING &&
					er->tokens[2].type == GSMD_ECMD_RTT_STRING &&
					er->tokens[3].type == GSMD_ECMD_RTT_STRING ) {
				
				/*
				 * +COPS=? +COPS: [list of supported (<stat>,long alphanumeric <oper>
				 *       ,short alphanumeric <oper>,numeric <oper>)s]
				 */
				
				out2->stat = er->tokens[0].u.numeric;
				strlcpy(out2->opname_longalpha, er->tokens[1].u.string,
					sizeof(out2->opname_longalpha));
				strlcpy(out2->opname_shortalpha, er->tokens[2].u.string,
					sizeof(out2->opname_shortalpha));
				strlcpy(out2->opname_num, er->tokens[3].u.string,
					sizeof(out2->opname_num));
			}
			else {
				DEBUGP("Invalid Input : Parse error\n");
				FREE(*out);
				return -EINVAL;
			}

			FREE(er);
			out2->is_last = 0;
			out2 ++;
		}

		str ++;
	}

	out2->is_last = 1;
	return len;
}

static int network_opers_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_msg_oper *buf = NULL;
	int len, ret;

	len = network_opers_parse(resp, &buf);
	if(len < 0)
		return len;	/* error we got from network_opers_parse */

	ret = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_LIST,
			cmd->id, sizeof(*buf) * (len + 1), buf);
	FREE(buf);
	return ret;
}

static int network_pref_opers_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = (struct gsmd_user *) ctx;
	struct gsmd_msg_prefoper entry;
	int index;
	char opname[17];

	if (cmd->ret && cmd->ret != -255)
		return 0;	/* TODO: Send a response */

	if (sscanf(resp, "+CPOL: %i,0,\"%16[^\"]\"", &index, opname) < 2)
		return -EINVAL;	/* TODO: Send a response */

	entry.index = index;
	entry.is_last = (cmd->ret == 0);
	memcpy(entry.opname_longalpha, opname, sizeof(entry.opname_longalpha));

	return gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_LIST,
			cmd->id, sizeof(entry), &entry);
}

static int network_pref_num_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = (struct gsmd_user *) ctx;
	int min_index, max_index, size;

	if (cmd->ret)
		return 0;	/* TODO: Send a response */

	/* This is not a full general case, theoretically the range string
	 * can include commas and more dashes, but we have no full parser for
	 * ranges yet.  */
	if (sscanf(resp, "+CPOL: (%i-%i)", &min_index, &max_index) < 2)
		return -EINVAL;	/* TODO: Send a response */
	size = max_index - min_index + 1;

	return gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_SPACE,
			cmd->id, sizeof(size), &size);
}


static int network_get_ownnumbers_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = (struct gsmd_user *) ctx;
	struct gsmd_own_number *num;
	struct gsm_extrsp *er;
	int len, ret, type;
	char dummy;


	//+CNUM:[<alpha1>],<number1>,<type1>[,<speed>,<service>[,<itc>]]
	//+CNUM: "00310035","",,,
	//+CNUM: "","8963331186",129,,4

	len = sizeof(*num) + (GSMD_PB_TEXT_MAXLEN<<1) +2; // 2*GSMD_PB_TEXT_MAXLEN +2
	num = (struct gsmd_own_number *)MALLOC(len);
	memset(num, 0, len);
	if ( !strncmp(resp, "+CNUM", 4) )
	{
		er = extrsp_parse(resp);
		if ( !er )
		{
			FREE(num);
			return -ENOMEM;
		}
		
		if (er->num_tokens > 0)
		{
			if(er->tokens[0].type == GSMD_ECMD_RTT_STRING)	//name
			{
				int i,n;
				char *str,*text;

				//UCS2字符串转换成UCS2编码
				str = er->tokens[0].u.string;
				text = num->name;
				for (i=0,n=0; str[i] >= '0' && str[i + 1] >= '0' &&
						n < (GSMD_PB_TEXT_MAXLEN<<1/* 2*GSMD_PB_TEXT_MAXLEN */); n ++) {
					if (sscanf(str+i, "%2hhX", &text[n]) < 1) {
						return -EINVAL;	/* TODO: Send a response */
					}
					i += 2;
				}
				text[n++] = 0;
				text[n++] = 0;
			}

			if(er->tokens[1].type == GSMD_ECMD_RTT_STRING)	//num
			{
				strlcpy(num->addr.number, er->tokens[1].u.string, GSMD_ADDR_MAXLEN+1);
			}
			
			if(er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC)	//type
			{
				num->addr.type = er->tokens[2].u.numeric;
			}

			if(er->tokens[4].type == GSMD_ECMD_RTT_NUMERIC)	//service
			{
				num->service = er->tokens[2].u.numeric;
			}
		}
		FREE(er);
		
		num->is_last = (cmd->ret == 0);
	}
	else if ( !strncmp(resp, "OK", 2) ) 
	{
		/* The record is empty or could not read yet */
		num->is_last = -1;
	}
	else
	{
		//DEBUGP("+CME error\n");
		num->is_last = -2;
	}

	ret = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_GET_NUMBER,
			cmd->id, len, num);

	FREE(num);
	
	return ret;
}


#if 0
static int network_get_ownnumbers_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = (struct gsmd_user *) ctx;
	struct gsmd_own_number *num;
	int len, ret, type;
	char dummy;

	if (cmd->ret && cmd->ret != -255)
		return 0;	/* TODO: Send a response */


	//+CNUM:[<alpha1>],<number1>,<type1>[,<speed>,<service>[,<itc>]]
	if (sscanf(resp, "+CNUM: \"%*[^\"]\"%c%n", &dummy, &len) > 0)
		len -= strlen("+CNUM: \"\",");
	else
		len = 0;

	num = (struct gsmd_own_number *)MALLOC(sizeof(*num) + len + 2);
	if (len)
	{
		ret = sscanf(resp, "+CNUM: \"%[^\"]\",\"%32[^\"]\",%i,%*i,%i,",
				num->name, num->addr.number,
				&type, &num->service) - 1;
	}
	else
	{
		ret = sscanf(resp, "+CNUM: ,\"%32[^\"]\",%i,%*i,%i,",
				num->addr.number,
				&type, &num->service);
	}
	
	if (ret < 2)
	{
		if(!strncmp(resp, "OK", 2))
			num->is_last = -1;
		else
			num->is_last = -2;

		//FREE(num);
		//return -EINVAL;	/* TODO: Send a response */
	}
	else
	{
		if (ret < 3)
			num->service = GSMD_SERVICE_UNKNOWN;
		num->name[len] = 0;
		num->name[len+1] = 0;

		if(len)
		{
			int i,n;
			char *str,text[30];

			//UCS2字符串转换成UCS2编码
			str = num->name;
			for (i=0,n=0; str[i] >= '0' && str[i + 1] >= '0' &&
					n < (GSMD_PB_TEXT_MAXLEN<<1/* 2*GSMD_PB_TEXT_MAXLEN */); n ++) {
				if (sscanf(str+i, "%2hhX", &text[n]) < 1) {
					return -EINVAL;	/* TODO: Send a response */
				}
				i += 2;
			}
			text[n++] = 0;
			text[n++] = 0;
			
			memcpy(str, text, n);
		}
		
		num->addr.type = type;
		num->is_last = (cmd->ret == 0);
	}

	ret = gsmd_ucmd_submit(gu, GSMD_MSG_NETWORK, GSMD_NETWORK_GET_NUMBER,
			cmd->id, sizeof(*num) + len + 1, num);

	FREE(num);
	return ret;
}
#endif

static int network_set_ownnumbers_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//DEBUGP("resp: %s\n", resp);

	return gsmd_ucmd_submit(ctx, GSMD_MSG_NETWORK, GSMD_NETWORK_SET_NUMBER,
			cmd->id, strlen(resp) + 1, resp);
}


static int network_timestamp_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	return 0;
}



static int usock_rcv_network(struct gsmd_user *gu, struct gsmd_msg_hdr *gph, 
			     int len)
{
	struct gsmd_atcmd *cmd;
	struct gsmd_voicemail *vmail = (struct gsmd_voicemail *) gph->data;
	gsmd_oper_numeric *oper = (gsmd_oper_numeric *) gph->data;
	char buffer[15 + sizeof(gsmd_oper_numeric)];
	int atcmd_len;

	struct gsmd_phonebook *gp;


	switch (gph->msg_subtype) {
	case GSMD_NETWORK_REGISTER:
		if ((*oper)[0])
			atcmd_len = sprintf(buffer, "AT+COPS=1,2,\"%.*s\"",
					sizeof(gsmd_oper_numeric), (char *)oper);
		else
			atcmd_len = sprintf(buffer, "AT+COPS=0");
		cmd = atcmd_fill(buffer, atcmd_len + 1, &network_register_cb, gu, gph->id, discard_timer2);
		break;
	case GSMD_NETWORK_RE_REGISTER:
		if ((*oper)[0])
			atcmd_len = sprintf(buffer, "AT+COPS=1,2,\"%.*s\"",
					sizeof(gsmd_oper_numeric), (char *)oper);
		else
			atcmd_len = sprintf(buffer, "AT+COPS=0");
		cmd = atcmd_fill(buffer, atcmd_len + 1, &network_re_register_cb, gu, gph->id, discard_timer2);
		break;
	case GSMD_NETWORK_DEREGISTER:
		cmd = atcmd_fill("AT+COPS=2", 9+1, &network_deregister_cb, gu, gph->id, discard_timer2);
		break;
	case GSMD_NETWORK_QUERY_REG:
		cmd = atcmd_fill("AT+CREG?", 8+1, &network_query_reg_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_VMAIL_GET:
		cmd = atcmd_fill("AT+CSVM?", 8+1, &network_vmail_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_VMAIL_SET:
		atcmd_len = sprintf(buffer, "AT+CSVM=1,\"%s\",%d",
			vmail->addr.number, vmail->addr.type);
		cmd = atcmd_fill(buffer, atcmd_len + 1, &network_vmail_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_SIGQ_GET:		
		cmd = atcmd_fill("AT+CSQ", 6+1, &network_sigq_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_OPER_GET:
		/* Set long alphanumeric format */
		cmd = atcmd_fill("AT+COPS=3,0", 11+1, &null_cmd_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		atcmd_submit(gu->gsmd, cmd);

		cmd = atcmd_fill("AT+COPS?", 8+1, &network_oper_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_OPER_N_GET:
		/* Set numeric format */
		atcmd_submit(gu->gsmd, atcmd_fill("AT+COPS=3,2", 11+1,
					&null_cmd_cb, gu, gph->id, NULL));
		cmd = atcmd_fill("AT+COPS?", 8+1, &network_oper_n_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_OPER_LIST:
		cmd = atcmd_fill("AT+COPS=?", 9+1, &network_opers_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_PREF_LIST:
		/* Set long alphanumeric format */
		atcmd_submit(gu->gsmd, atcmd_fill("AT+CPOL=,0", 10 + 1,
					&null_cmd_cb, gu, gph->id, NULL));
		cmd = atcmd_fill("AT+CPOL?", 8 + 1,
				&network_pref_opers_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_PREF_DEL:
		atcmd_len = sprintf(buffer, "AT+CPOL=%i", *(int *) gph->data);
		cmd = atcmd_fill(buffer, atcmd_len + 1, &null_cmd_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_PREF_ADD:
		atcmd_len = sprintf(buffer, "AT+CPOL=,2,\"%.*s\"",
				sizeof(gsmd_oper_numeric), (char *)oper);
		cmd = atcmd_fill(buffer, atcmd_len + 1, &null_cmd_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_PREF_SPACE:
		cmd = atcmd_fill("AT+CPOL=?", 9 + 1,
				&network_pref_num_cb, gu, gph->id, NULL);
		break;
	case GSMD_NETWORK_GET_NUMBER:
		cmd = atcmd_fill("AT+CNUM", 7 + 1,
				&network_get_ownnumbers_cb, gu, gph->id, NULL);
		break;

	case GSMD_NETWORK_SET_NUMBER:
		if(len < sizeof(*gph) + sizeof(*gp))
			return -EINVAL;
		gp = (struct gsmd_phonebook *) ((char *)gph + sizeof(*gph));

#ifdef GSM_MODULE_ULC2
		{
			char buf[100];

			//切换存储区域
			cmd = atcmd_fill("AT+CPBS=\"ON\"", 12+1, &null_cmd_cb, gu, gph->id, NULL);
			if (!cmd)
				return -ENOMEM;
			atcmd_submit(gu->gsmd, cmd);


			//写入本机号码
			{
				//UCS2编码转换成UCS2字符串
				//计算UCS2字节个数，必须小于等于12(GSMD_PB_TEXT_MAXLEN-2)字节
				int i;
				char *text, *pUcs2buf, ucs2buf[25];
				
				text = gp->text;
		        i=0;
		        while ((text[i]!=0 || text[i+1]!=0) && i<=GSMD_PB_TEXT_MAXLEN-2)
		        {
		            i+=2;
		        }

				atcmd_len = i;
				pUcs2buf = ucs2buf;
				for (i = 0; i < atcmd_len; i ++)
				{
					sprintf(pUcs2buf, "%02X", text[i]);
					pUcs2buf += 2;
				}
				*pUcs2buf = '\0';
				
				atcmd_len = sprintf(buf, "AT+CPBW=%d,\"%s\",%d,\"%s\"",
						gp->index, gp->numb, gp->type, ucs2buf);
			}

			//atcmd_len = sprintf(buf, "AT+CPBW=%d,\"%s\",%d,\"%s\"",
			//		gp->index, gp->numb, gp->type, gp->text);
			cmd = atcmd_fill(buf, atcmd_len+1,
					 &null_cmd_cb, gu, gph->id, NULL);
			if (!cmd)
				return -ENOMEM;
			atcmd_submit(gu->gsmd, cmd);

			//切换存储区域
			cmd = atcmd_fill("AT+CPBS=\"SM\"", 12+1, &network_set_ownnumbers_cb, gu, gph->id, NULL);
		}

#elif defined(GSM_MODULE_MD231)

		{
			char buf[100];
			
			/*
			AT+SNUM=[<index>][,<number>[,<type>[,<text>]]]，该命令参数如下：
			index(0-1), 最多支持两条
			type(0-1)，0:ASCII code, 1: UCS2 code
			*/
			if(0 == gp->type)	//ASCII
			{
				atcmd_len = sprintf(buf, "AT+SNUM=%d,\"%s\",%d,\"%s\"",
						gp->index, gp->numb, gp->type, gp->text);
			}
			else if(1 == gp->type)	//UCS2
			{
				//UCS2编码转换成UCS2字符串
				//计算UCS2字节个数，必须小于等于12(GSMD_PB_TEXT_MAXLEN-2)字节
				int i;
				char *text, *pUcs2buf, ucs2buf[25];
				
				text = gp->text;
		        i=0;
		        while ((text[i]!=0 || text[i+1]!=0) && i<=GSMD_PB_TEXT_MAXLEN-2)
		        {
		            i+=2;
		        }

				atcmd_len = i;
				pUcs2buf = ucs2buf;
				for (i = 0; i < atcmd_len; i ++)
				{
					sprintf(pUcs2buf, "%02X", text[i]);
					pUcs2buf += 2;
				}
				*pUcs2buf = '\0';
				
				atcmd_len = sprintf(buf, "AT+SNUM=%d,\"%s\",%d,\"%s\"",
						gp->index, gp->numb, gp->type, ucs2buf);
			}

			cmd = atcmd_fill(buf, atcmd_len+1,
					 &network_set_ownnumbers_cb, gu, gph->id, NULL);
		}
#endif
		break;
		
		
	case GSMD_NETWORK_GET_TIMESTAMP:
		cmd = atcmd_fill("AT+CLTS", 7 + 1,
				&network_timestamp_cb, gu, gph->id, NULL);
		break;
		
	default:
		return -EINVAL;
	}
	if (!cmd)
		return -ENOMEM;

	return atcmd_submit(gu->gsmd, cmd);
}


static int phonebook_find_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebooks gps;
	struct gsm_extrsp *er;

	//DEBUGP("resp: %s\n", resp);

	er = extrsp_parse(resp);

	if ( !er )
		return -ENOMEM;

	gps.is_last = (cmd->ret == 0)? 1:0;

	if ( !strncmp(resp, "OK", 2) ) {
		/* The record is empty or could not read yet */
		gps.pb.index = 0;
	}
	else if ( !strncmp(resp, "+CME", 4) ) {
		//DEBUGP("== +CME error\n");
		/* +CME ERROR: 21 */
		gps.pb.index = 0 - atoi(strpbrk(resp, "0123456789"));
	}
	else if ( er->num_tokens == 4 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[3].type == GSMD_ECMD_RTT_STRING ) {

		int i,n;
		char *str, *text;
		
		/*
		 * [+CPBR: <index1>,<number>,<type>,<text>[[...]
		 * <CR><LF>+CPBR: <index2>,<unmber>,<type>,<text>]]
		 */

		gps.pb.index = er->tokens[0].u.numeric;
		strlcpy(gps.pb.numb, er->tokens[1].u.string, GSMD_PB_NUMB_MAXLEN+1);
		gps.pb.type = er->tokens[2].u.numeric;
		
		//UCS2字符串转换成UCS2编码
		str = er->tokens[3].u.string;
		text = gps.pb.text;
		for (i=0,n=0; str[i] >= '0' && str[i + 1] >= '0' &&
				n < GSMD_PB_TEXT_MAXLEN-2; n ++) {
			if (sscanf(str+i, "%2hhX", &text[n]) < 1) {
				return -EINVAL;	/* TODO: Send a response */
			}
			i += 2;
		}
		text[n++] = 0;
		text[n++] = 0;
		
		//strlcpy(gps.pb.text, er->tokens[3].u.string, GSMD_PB_TEXT_MAXLEN+1);
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		gps.pb.index = 0;
		//return -EINVAL;
	}
	
	FREE(er);

	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_FIND,
			cmd->id, sizeof(gps), &gps);
}

static int phonebook_read_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebook gp;
	struct gsm_extrsp *er;

	//DEBUGP("resp: %s\n", resp);
	if(!strncmp(resp, "+CPBR: ", 7))
	{
		er = extrsp_parse(resp);
		if ( !er )
			return -ENOMEM;

		if ( er->num_tokens == 4 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[3].type == GSMD_ECMD_RTT_STRING ) {

			int i,n;
			char *str, *text;


			/*
			 * [+CPBR: <index1>,<number>,<type>,<text>[[...]
			 * <CR><LF>+CPBR: <index2>,<unmber>,<type>,<text>]]
			 */

			gp.index = er->tokens[0].u.numeric;
			strlcpy(gp.numb, er->tokens[1].u.string, GSMD_PB_NUMB_MAXLEN+1);
			gp.type = er->tokens[2].u.numeric;
			
			//UCS2字符串转换成UCS2编码
			str = er->tokens[3].u.string;
			text = gp.text;
			for (i=0,n=0; str[i] >= '0' && str[i + 1] >= '0' &&
					n < GSMD_PB_TEXT_MAXLEN-2; n ++) {
				if (sscanf(str+i, "%2hhX", &text[n]) < 1) {
					return -EINVAL;	/* TODO: Send a response */
				}
				i += 2;
			}
			text[n++] = 0;
			text[n++] = 0;

			//strlcpy(gp.text, er->tokens[3].u.string, GSMD_PB_TEXT_MAXLEN+1);
		}
		else
		{
			gp.index = 0;
		}
	
		FREE(er);
	}
	else if ( !strncmp(resp, "OK", 2) ) {
		/* The record is empty or could not read yet */
		gp.index = 0;
	}
	else if ( !strncmp(resp, "+CME", 4) ) {
		//DEBUGP("+CME error\n");
		/* +CME ERROR: 21 */
		gp.index = 0 - atoi(strpbrk(resp, "0123456789"));
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		gp.index = 0;
		//return -EINVAL;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_READ,
			cmd->id, sizeof(gp), &gp);
}

static int phonebook_readrg_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebooks gps;
	struct gsm_extrsp *er;

	//DEBUGP("resp: %s\n", resp);
	gps.is_last = (cmd->ret == 0)? 1:0;
	
	if(!strncmp(resp, "+CPBR: ", 7))
	{
		er = extrsp_parse(resp);

		if ( !er )
			return -ENOMEM;

		if ( er->num_tokens == 4 &&
			er->tokens[0].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[1].type == GSMD_ECMD_RTT_STRING &&
			er->tokens[2].type == GSMD_ECMD_RTT_NUMERIC &&
			er->tokens[3].type == GSMD_ECMD_RTT_STRING ) {

			int i,n;
			char *str, *text;


			/*
			 * [+CPBR: <index1>,<number>,<type>,<text>[[...]
			 * <CR><LF>+CPBR: <index2>,<unmber>,<type>,<text>]]
			 */

			gps.pb.index = er->tokens[0].u.numeric;
			strlcpy(gps.pb.numb, er->tokens[1].u.string, GSMD_PB_NUMB_MAXLEN+1);
			gps.pb.type = er->tokens[2].u.numeric;


			//UCS2字符串转换成UCS2编码
			str = er->tokens[3].u.string;
			text = gps.pb.text;
			for (i=0,n=0; str[i] >= '0' && str[i + 1] >= '0' &&
					n < GSMD_PB_TEXT_MAXLEN-2; n ++) {
				if (sscanf(str+i, "%2hhX", &text[n]) < 1) {
					return -EINVAL;	/* TODO: Send a response */
				}
				i += 2;
			}
			text[n++] = 0;
			text[n++] = 0;
			
			//strlcpy(gps.pb.text, er->tokens[3].u.string, GSMD_PB_TEXT_MAXLEN+1);
		}
		else
		{
			gps.pb.index = 0;
		}
		
		FREE(er);
	}
	else if ( !strncmp(resp, "OK", 2) ) {
		/* The record is empty or could not read yet */
		gps.pb.index = 0;
	}
	else if ( !strncmp(resp, "+CME", 4) ) {
		//DEBUGP("+CME error\n");
		/* +CME ERROR: 21 */
		gps.pb.index = 0 - atoi(strpbrk(resp, "0123456789"));
	}
	else {
		//DEBUGP("Invalid Input : Parse error\n");
		gps.pb.index = 0;
		//return -EINVAL;
	}

	return gsmd_ucmd_submit(gu, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_READRG,
			cmd->id, sizeof(gps), &gps);
}

static int phonebook_write_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//DEBUGP("resp: %s\n", resp);

	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_WRITE,
			cmd->id, strlen(resp) + 1, resp);
}

static int phonebook_delete_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//DEBUGP("resp: %s\n", resp);

	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_DELETE,
			cmd->id, strlen(resp) + 1, resp);
}

static int phonebook_get_support_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	/* TODO: Need to handle command error */
	/* +CPBR: (1-100),44,16 */
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebook_support gps;
	char *fcomma, *lcomma;
	char *dash;

	//DEBUGP("resp: %s\n", resp);
	
	gps.index = 0;
	
	if (!strncmp(resp, "+CPBR", 5)) 
	{
		dash = strchr(resp, '-');
		if (!dash)
			return -EIO;	/* TODO: Send a response */
		gps.index = atoi(dash + 1);

		fcomma = strchr(resp, ',');
		if (!fcomma)
			return -EIO;	/* TODO: Send a response */
		gps.nlength = atoi(fcomma+1);

		lcomma = strrchr(resp, ',');
		if (!lcomma)
			return -EIO;	/* TODO: Send a response */
		gps.tlength = atoi(lcomma+1);
	}
	return gsmd_ucmd_submit(gu,
			GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_GET_SUPPORT,
			cmd->id, sizeof(gps), &gps);
}

static int phonebook_list_storage_cb(struct gsmd_atcmd *cmd,
		void *ctx, char *resp)
{
	/* TODO; using link list ; need to handle command error */
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebook_storage gps;
	char *ptr;

	//DEBUGP("resp: %s\n", resp);

	/*
	 * +CPBS: (<storage>s)
	 */
	gps.num = 0;

	if (!strncmp(resp, "+CPBS", 5)) {
		char* delim = "(,";
		ptr = strpbrk(resp, delim);
		while (ptr) {
			if(!isupper(*(ptr+2)))
				ptr++;
			strncpy(gps.mem[gps.num].type, ptr + 2, 2);
			gps.mem[gps.num].type[2] = '\0';
			ptr = strpbrk(ptr + 2, delim);
			gps.num++;
		}
	}

	return gsmd_ucmd_submit(gu,
			GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_LIST_STORAGE,
			cmd->id, sizeof(gps), &gps);
}



static int phonebook_get_storage_cb(struct gsmd_atcmd *cmd,
		void *ctx, char *resp)
{
	/* TODO; using link list ; need to handle command error */
	struct gsmd_user *gu = ctx;
	struct gsmd_phonebook_mem gpm;
	char *ptr;

	//DEBUGP("resp: %s\n", resp);

	/*
	 * +CPBS: <storage>[,<used>,<total>]
	 */
	//+CPBS: "ME", 5, 100
	//+CPBS: "ME",5,100
	
	gpm.total = 0;
	
	if (!strncmp(resp, "+CPBS", 5)) {
		ptr = strchr(resp,'"');
		strncpy(gpm.type, ptr + 1, 2);
		gpm.type[2] = '\0';

		ptr = strchr(ptr+2,',');
		gpm.used = atoi(ptr+1);

		ptr = strchr(ptr+2,',');
		gpm.total = atoi(ptr+1);
	}

	return gsmd_ucmd_submit(gu,
			GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_GET_STORAGE,
			cmd->id, sizeof(gpm), &gpm);
}



static int phonebook_set_storage_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//DEBUGP("resp: %s\n", resp);

	return gsmd_ucmd_submit(ctx, GSMD_MSG_PHONEBOOK, GSMD_PHONEBOOK_SET_STORAGE,
			cmd->id, strlen(resp) + 1, resp);
}




static int usock_rcv_phonebook(struct gsmd_user *gu,
		struct gsmd_msg_hdr *gph,int len)
{	
	struct gsmd_atcmd *cmd = NULL;
	struct gsmd_phonebook_readrg *gpr;
	struct gsmd_phonebook *gp;
	struct gsmd_phonebook_find *gpf;
	int *index;
	int atcmd_len;
	char *storage;
	char buf[100];

	switch (gph->msg_subtype) {
	case GSMD_PHONEBOOK_LIST_STORAGE:
		cmd = atcmd_fill("AT+CPBS=?", 9 + 1,
				&phonebook_list_storage_cb,
				gu, gph->id, NULL);
		break;
	case GSMD_PHONEBOOK_GET_STORAGE:
		cmd = atcmd_fill("AT+CPBS?", 8 + 1,
				&phonebook_get_storage_cb,
				gu, gph->id, NULL);
		break;
	case GSMD_PHONEBOOK_SET_STORAGE:
		if (len < sizeof(*gph) + 3)
			return -EINVAL;

		storage = (char*) ((char *)gph + sizeof(*gph));

		/* ex. AT+CPBS="ME" */
		atcmd_len = 1 + strlen("AT+CPBS=\"") + 2 + strlen("\"");
		cmd = atcmd_fill("AT+CPBS=\"", atcmd_len,
				&phonebook_set_storage_cb, gu, gph->id, NULL);

		if (!cmd)
			return -ENOMEM;

		sprintf(cmd->buf, "AT+CPBS=\"%s\"", storage);
		break;
	case GSMD_PHONEBOOK_FIND:
		if(len < sizeof(*gph) + sizeof(*gpf))
			return -EINVAL;
		gpf = (struct gsmd_phonebook_find *) ((char *)gph + sizeof(*gph));

		atcmd_len = 1 + strlen("AT+CPBF=\"") +
			strlen(gpf->findtext) + strlen("\"");
		cmd = atcmd_fill("AT+CPBF=\"", atcmd_len,
				 &phonebook_find_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CPBF=\"%s\"", gpf->findtext);
		break;
	case GSMD_PHONEBOOK_READ:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		
		index = (int *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d", *index);

		/* ex, AT+CPBR=23 */
		atcmd_len = 1 + strlen("AT+CPBR=") + strlen(buf);
		cmd = atcmd_fill("AT+CPBR=", atcmd_len,
				 &phonebook_read_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CPBR=%d", *index);
		break;
	case GSMD_PHONEBOOK_READRG:
		if(len < sizeof(*gph) + sizeof(*gpr))
			return -EINVAL;
		gpr = (struct gsmd_phonebook_readrg *) ((char *)gph + sizeof(*gph));

		sprintf(buf, "%d,%d", gpr->index1, gpr->index2);

		/* ex, AT+CPBR=1,100 */
		atcmd_len = 1 + strlen("AT+CPBR=") + strlen(buf);
		cmd = atcmd_fill("AT+CPBR=", atcmd_len,
				 &phonebook_readrg_cb, gu, gph->id, discard_timer2);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CPBR=%s", buf);
		break;
	case GSMD_PHONEBOOK_WRITE:
		if(len < sizeof(*gph) + sizeof(*gp))
			return -EINVAL;
		gp = (struct gsmd_phonebook *) ((char *)gph + sizeof(*gph));

		{
			//UCS2编码转换成UCS2字符串
			//计算UCS2字节个数，必须小于等于12(GSMD_PB_TEXT_MAXLEN-2)字节
			int i;
			char *text, *pUcs2buf, ucs2buf[25];
			
			text = gp->text;
	        i=0;
	        while ((text[i]!=0 || text[i+1]!=0) && i<=GSMD_PB_TEXT_MAXLEN-2)
	        {
	            i+=2;
	        }

			atcmd_len = i;
			pUcs2buf = ucs2buf;
			for (i = 0; i < atcmd_len; i ++)
			{
				sprintf(pUcs2buf, "%02X", text[i]);
				pUcs2buf += 2;
			}
			*pUcs2buf = '\0';
			
			atcmd_len = sprintf(buf, "AT+CPBW=%d,\"%s\",%d,\"%s\"",
					gp->index, gp->numb, gp->type, ucs2buf);
		}
		
		//atcmd_len = sprintf(buf, "AT+CPBW=%d,\"%s\",%d,\"%s\"",
		//		gp->index, gp->numb, gp->type, gp->text);
		cmd = atcmd_fill(buf, atcmd_len+1,
				 &phonebook_write_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		break;
	case GSMD_PHONEBOOK_DELETE:
		if(len < sizeof(*gph) + sizeof(int))
			return -EINVAL;
		index = (int *) ((char *)gph + sizeof(*gph));
	    	
		sprintf(buf, "%d", *index);
		
		/* ex, AT+CPBW=3*/
		atcmd_len = 1 + strlen("AT+CPBW=") + strlen(buf);
		cmd = atcmd_fill("AT+CPBW=", atcmd_len,
				 &phonebook_delete_cb, gu, gph->id, NULL);
		if (!cmd)
			return -ENOMEM;
		sprintf(cmd->buf, "AT+CPBW=%s", buf);
		break;
	case GSMD_PHONEBOOK_GET_SUPPORT:
		cmd = atcmd_fill("AT+CPBR=?", 9+1,
				 &phonebook_get_support_cb, gu, gph->id, NULL);
		break;
	default:
		return -EINVAL;
	}

	if (cmd)
		return atcmd_submit(gu->gsmd, cmd);
	else
		return 0;
}


static usock_msg_handler *pcmd_type_handlers[__NUM_GSMD_MSGS] = {
								NULL,
	/*[GSMD_MSG_EVENT]		= */&usock_rcv_event,
	/*[GSMD_MSG_PASSTHROUGH]= */&usock_rcv_passthrough,
	/*[GSMD_MSG_VOICECALL]	= */&usock_rcv_voicecall,
	/*[GSMD_MSG_DATACALL]	= */&usock_rcv_datacall,
	/*[GSMD_MSG_PHONEBOOK]	= */&usock_rcv_phonebook,
	/*[GSMD_MSG_NETWORK]	= */&usock_rcv_network,
	/*[GSMD_MSG_PHONE]		= */&usock_rcv_phone,
	/*[GSMD_MSG_SECURITY]	= */&usock_rcv_security,
	/*[GSMD_MSG_SMS]		= */&usock_rcv_sms,
	/*[GSMD_MSG_CB]			= */&usock_rcv_cb,
	/*[GSMD_MSG_MODEM]		= */&usock_rcv_modem,
};


static int usock_rcv_pcmd(struct gsmd_user *gu, char *buf, int len)
{
	struct gsmd_msg_hdr *gph = (struct gsmd_msg_hdr *)buf;
	usock_msg_handler *umh;

	if (gph->version != GSMD_PROTO_VERSION)
		return -EINVAL;

	if (gph->msg_type >= __NUM_GSMD_MSGS)
		return -EINVAL;

	umh = pcmd_type_handlers[gph->msg_type];
	if (!umh)
		return -EINVAL;

	return umh(gu, gph, len);
}



/* callback for read/write on client (libgsmd) socket */
static int gsmd_usock_user_cb(int fd, unsigned int what, void *data)
{
	struct gsmd_user *gu = data;

	/* FIXME: check some kind of backlog and limit it */

	if (what & GSMD_FD_READ) {
		char buf[2048/*512 1024*/];
		int rcvlen;
		
		/* read data from socket, determine what he wants */
		rcvlen = u_recv(fd, buf, sizeof(buf), 1);

	#if 1
		if(0 < rcvlen)
			usock_rcv_pcmd(gu, buf, rcvlen);
	#else
		if (rcvlen <= 0) 
		{
			return rcvlen;
		}
		else
			return usock_rcv_pcmd(gu, buf, rcvlen);
	#endif
	}

	if (what & GSMD_FD_WRITE) {
		/* write data from pending replies to socket */
		struct gsmd_ucmd *ucmd, *uctmp;
		llist_for_each_entry_safe(ucmd, uctmp, struct gsmd_ucmd, &gu->finished_ucmds, list) {
			int rc;

			rc = u_send(fd, &ucmd->hdr, sizeof(ucmd->hdr) + ucmd->hdr.len, 1);
			if (rc < 0) {
				//DEBUGP("write return %d\n", rc);
				return rc;
			}
			if (rc == 0) {
				//DEBUGP("write returns zero!!\n");
				break;
			}
			if (rc != sizeof(ucmd->hdr) + ucmd->hdr.len) {
				//DEBUGP("short write\n");
				break;
			}

			//DEBUGP("successfully sent cmd %p to user %p, freeing\n", ucmd, gu);
			llist_del(&ucmd->list);
			FREE(ucmd);
		}
		if (llist_empty(&gu->finished_ucmds))
			gu->gfd.when &= ~GSMD_FD_WRITE;
	}

	return 0;
}




const char *user_msg_str[][USER_NUM]=
{
	{
		CONNECT_USER0,
		CONNECT_USER1,
		CONNECT_USER2,
		CONNECT_USER3,
		CONNECT_USER4
	},
	
	{
		DISCONNECT_USER0,
		DISCONNECT_USER1,
		DISCONNECT_USER2,
		DISCONNECT_USER3,
		DISCONNECT_USER4
	}
};


tMSG_QUEUE *guser_reqmsg_que=NULL;
gsmdmailbox gmbox[USER_NUM];	//有5个通信通道可用
SEMAPHORE  *psema_guser[USER_NUM][RESULT_NUM]={NULL};



/* callback for read on master-listen-socket */
static int gsmd_usock_cb(int fd, unsigned int what, void *data)
{
	struct gsmd *g = data;
	struct gsmd_user *gu;


	/* FIXME: implement this */
	if (what & GSMD_FD_READ) 
	{
		/* new incoming connection */
		int  ret,len;
		char buf[15];
		
		ret=msg_queue_tryget(g->user_reqmsg_que, buf, &len);
		
		if(0>ret || 0>=len)
		{
			//DEBUGP("error accepting incoming conn: `%s'\n",	strerror(errno));
			return -1;
		}
		
		//check the request
		if(0==strncmp(buf,"con",3))
		{
			int id,bufSize;

			for(id=0; id<USER_NUM; id++)
			{
				if(0==strncmp(buf,user_msg_str[CONNECT][id],len))
					break;
			}
			if(USER_NUM <= id)
				return -1;

			if(USER_1 == id)
			{
				//voicecall
				gmbox[id].rcv = 1;
				gmbox[id].snd = 0;
			}
			else
			{
				gmbox[id].rcv = 1;
				gmbox[id].snd = 1;
			}


			//build a gsmdmailbox structure
			//receive box
			if(0 != gmbox[id].rcv)
			{
				gmbox[id].rcvbuf = (mailboxbuf *)MALLOC(sizeof(mailboxbuf));
				if(NULL == gmbox[id].rcvbuf)
					return -ENOMEM;
				memset((char *)gmbox[id].rcvbuf,0,sizeof(mailboxbuf));

				if(USER_3 == id)	//GPRS
					bufSize = MAILBOX_BUFSIZE_2;
				else
					bufSize = MAILBOX_BUFSIZE_1;

				gmbox[id].rcvbuf->size = bufSize;
				gmbox[id].rcvbuf->buf = (char *)MALLOC(bufSize);
				if(NULL == gmbox[id].rcvbuf->buf)
				{
					FREE(gmbox[id].rcvbuf);//kong 2011-05-12  
					return -ENOMEM;
				}
				memset((char *)gmbox[id].rcvbuf->buf,0,bufSize);
			}
			else
			{
				gmbox[id].rcvbuf = NULL;
			}
			
			
			//send box
			if(0 != gmbox[id].snd)
			{
				gmbox[id].sndbuf = (mailboxbuf *)MALLOC(sizeof(mailboxbuf));
				if(NULL == gmbox[id].sndbuf)
				{
					FREE(gmbox[id].rcvbuf->buf);
					FREE(gmbox[id].rcvbuf);
					return -ENOMEM;
				}
				memset((char *)gmbox[id].sndbuf,0,sizeof(mailboxbuf));
				
				
				if(USER_3 == id)	//GPRS
					bufSize = MAILBOX_BUFSIZE_2;
				else
					bufSize = MAILBOX_BUFSIZE_1;
				
				
				gmbox[id].sndbuf->size = bufSize;
				gmbox[id].sndbuf->buf = (char *)MALLOC(bufSize);
				if(NULL == gmbox[id].sndbuf->buf)
				{
					FREE(gmbox[id].rcvbuf->buf);
					FREE(gmbox[id].rcvbuf);
					FREE(gmbox[id].sndbuf);//kong 2011-05-12  
					return -ENOMEM;
				}
				memset((char *)gmbox[id].sndbuf->buf,0,bufSize);
			}
			else
			{
				gmbox[id].sndbuf = NULL;
			}
			
			//build a gsmd_user structure
			gu = (struct gsmd_user *)MALLOC(sizeof(struct gsmd_user));
			if (!gu)
				return -ENOMEM;

			gu->gfd.fd = id;
			if (gu->gfd.fd < 0) 
			{
				//DEBUGP("error accepting incoming conn: `%s'\n",	strerror(errno));
				FREE(gu);
				return -1;
			}
			gu->gfd.when = GSMD_FD_READ;
			gu->gfd.data = gu;
			gu->gfd.cb = &gsmd_usock_user_cb;
			gu->gsmd = g;

			if(USER_0 == id)		//client
				gu->subscriptions = 0xffffffff;
			else if(USER_3 == id)	//GPRS
				gu->subscriptions = (u_int32_t)(1 << GSMD_EVT_CIPHER);
			else
				gu->subscriptions = 0;
				
			INIT_LLIST_HEAD(&gu->finished_ucmds);

			llist_add(&gu->list, &g->users);
			gsmd_register_fd(&gu->gfd);
			
			//inform user that the connection is ok
			semaphore_post(psema_guser[id][CONNECT]);
		}
		else //if(0==strcmp(buf,"discon",6))
		{
			int id;
			
			for(id=0; id<USER_NUM; id++)
			{
				if(0==strncmp(buf,user_msg_str[DISCONNECT][id],len))
					break;
			}
			if(USER_NUM <= id)
				return -1;

			if(0 != gmbox[id].rcv)
			{
				gmbox[id].rcv = 0;
				
				FREE(gmbox[id].rcvbuf->buf);
				FREE(gmbox[id].rcvbuf);
				gmbox[id].rcvbuf=NULL;
			}
			if(0 != gmbox[id].snd)
			{
				gmbox[id].snd = 0;
			
				FREE(gmbox[id].sndbuf->buf);
				FREE(gmbox[id].sndbuf);
				gmbox[id].sndbuf=NULL;
			}
			
			//destory the user
			llist_for_each_entry(gu, struct gsmd_user, &g->users, list) 
			{
				if (gu->gfd.fd  == id)
				{
					struct gsmd_fd *ufd;
					struct llist_head *p_gsmd_fds;
					struct gsmd_ucmd *ucmd, *uctmp;
					extern struct gsmd_fd *ufd2;
					
					p_gsmd_fds = get_gsmd_fds();
					llist_for_each_entry(ufd, struct gsmd_fd, p_gsmd_fds, list)
						if(ufd->fd==fd && (struct gsmd_fd*)ufd->list.next==&gu->gfd)
						{
							ufd2 = llist_entry(gu->gfd.list.next, struct gsmd_fd, list);
							break;
						}

					gsmd_unregister_fd(&gu->gfd);

					/* finish pending atcmd's from this client thus
					 * destroying references to the user structure.  */
					atcmd_terminate_matching(g, gu);
					gprsframe_terminate_matching(g, gu);

					//destroy gu->finished_ucmds
					llist_for_each_entry_safe(ucmd, uctmp, struct gsmd_ucmd, 
													&gu->finished_ucmds, list)
					{
						//DEBUGP("discard finished_ucmds %p(to user %p)\n", ucmd, gu);
						llist_del(&ucmd->list);
						FREE(ucmd);
					}
					
					/* destroy whole user structure */
					llist_del(&gu->list);
					FREE(gu);
					break;
				}
			}

			//inform user that the disconnection is completed
			semaphore_post(psema_guser[id][DISCONNECT]);
			
			if(USER_3 == id)	//GPRS
			{
				/*
				如果在断开gprs连接时没有收到NO CARRIER消息，当PPP协议层等待RIL层的gprs断开事件超时后，
				PPP协议层会调用lgsm_exit()自动退出，RIL层收到PPP协议层的lgsm_exit()请求，此时可以认为
				gprs连接无响应或已断开，并发送gprs disconnect事件
				*/
				if (1==g->net_gprs)
				{
				#ifdef _GSMD_DEBUG_LOG_1
					gsmd_debug_log("\r\nsend gprs disconnect\r\n",24, 0, NULL);
				#endif
					gprs_evt_send(g, 0);
				}
			}
		}
	}

	return 0;
}




/* handling of socket with incoming client connections */
int usock_init(struct gsmd *g)
{
	int i;
	SEMAPHORE  *psema;
	
	
	//must do this work before build the guser_reqmsg_que
	for(i=0; i<USER_NUM; i++)
	{
		psema = semaphore_init(0);
		if(NULL == psema)
			return -ENOMEM;
		psema_guser[i][CONNECT] = psema;

		psema = semaphore_init(0);
		if(NULL == psema)
			return -ENOMEM;
		psema_guser[i][DISCONNECT] = psema;
	}
	
	//build the guser_reqmsg_que
	guser_reqmsg_que = msg_queue_init(8, 15);
	if(NULL == guser_reqmsg_que)
		return -ENOMEM;

	g->user_reqmsg_que = guser_reqmsg_que;
	g->gfd_sock.fd = USER_NUM;
	g->gfd_sock.when = GSMD_FD_READ | GSMD_FD_EXCEPT;
	g->gfd_sock.data = g;
	g->gfd_sock.cb = &gsmd_usock_cb;

	return gsmd_register_fd(&g->gfd_sock);
}


void usock_uninit(struct gsmd *g)
{
	int i,rc;
	SEMAPHORE  *psema;
	
	
	for(i=0; i<USER_NUM; i++)
	{
		psema = psema_guser[i][CONNECT];
		if(psema)
			rc = semaphore_destroy(psema,OS_DEL_NO_PEND);
		
		psema = psema_guser[i][DISCONNECT];
		if(psema)
			rc = semaphore_destroy(psema,OS_DEL_NO_PEND);
	}
	memset(psema_guser, 0, sizeof(psema_guser));
	
	//destroy the guser_reqmsg_que
	rc = msg_queue_destroy(guser_reqmsg_que);
	if(!rc)
		guser_reqmsg_que = NULL;

	gsmd_unregister_fd(&g->gfd_sock);
}





/*-----------------------------------------------------------------------------
* 函数:	u_send()
* 功能:	
* 参数:	
* 返回:	the length of data sended:SUCCESS or <0:FAIL
*----------------------------------------------------------------------------*/
int u_send(int fd, char *buf, int len, int flag)
{
	u_int32_t  r,w,size;
	mailboxbuf	*mboxbuf;


	if(0==flag)
	{
		//user send
		mboxbuf = gmbox[fd].rcvbuf;
	}
	else
	{
		//gsmd send
		mboxbuf = gmbox[fd].sndbuf;
	}
	
	if(NULL == mboxbuf)
		return -1;
	
	size = mboxbuf->size;
	if(len > size)
		return -1;

	flag = 0;
//hyUsbPrintf("---------------send = %s\r\n",buf);
tryagain:

	Mutex_Protect();
	r = mboxbuf->r;
	w = mboxbuf->w;
	Mutex_Unprotect();

	if(w < r)
	{
		if(len < r-w) //full: (w+1)%size == r
		{
			memcpy((char *)(mboxbuf->buf+w), buf, len);
			w += len;
			
			flag = 0;
		}
		else
		{
			flag++;
		}
	}
	else //if(w >= r)
	{
		if(len < size-(w-r)) //full: (w+1)%size == r
		{
			if(len <= size-w)
			{
				memcpy((char *)(mboxbuf->buf+w), buf, len);
				w += len;
				if(w >= size)
					w = 0;
			}
			else //if(len > size-w)
			{
				int n;
				
				n = size-w;
				memcpy((char *)(mboxbuf->buf+w), buf, n);
				memcpy(mboxbuf->buf, (char *)(buf+n), len-n);
				w = len-n;
			}
			flag = 0;
		}
		else
		{
			flag++;
		}
	}

	if(0==flag)
	{
		Mutex_Protect();
		mboxbuf->w = w;	//succeed
		Mutex_Unprotect();
	}
	else //if(0<flag)
	{
		flag = 0;	//try until succeed
		sleep(1);
		goto tryagain;
	}

	return len;
}



/*-----------------------------------------------------------------------------
* 函数:	u_recv()
* 功能:	
* 参数:	
* 返回:	the length of data received:SUCCESS or <0:FAIL
*----------------------------------------------------------------------------*/
int u_recv(int fd, char *buf, int len, int flag)
{
	u_int16_t  datalen;
	u_int32_t  r,w,size,size_of_hdr;
	mailboxbuf	*mboxbuf;
	struct gsmd_msg_hdr *phdr;
	

	if(0==flag)
	{
		//user recv
		mboxbuf = gmbox[fd].sndbuf;
	}
	else
	{
		//gsmd recv
		mboxbuf = gmbox[fd].rcvbuf;
	}
	
	if(NULL == mboxbuf)
		return -1;

	Mutex_Protect();
	r = mboxbuf->r;
	w = mboxbuf->w;
	Mutex_Unprotect();

	if(r==w)
	{
		return -1; //empty:r==w
	}
	
	
	size = mboxbuf->size;
	size_of_hdr = sizeof(struct gsmd_msg_hdr);
	
	if(r < w)
	{
		u_int8_t *pdatalen; 
		
		phdr = (struct gsmd_msg_hdr *)(mboxbuf->buf+r);
		//datalen = (u_int16_t)phdr->len;
		pdatalen = (u_int8_t *)(mboxbuf->buf+r +offsetof(struct gsmd_msg_hdr,len));
		memcpy(&datalen, pdatalen, 2);

		if(len >= size_of_hdr + datalen)
			len = size_of_hdr + datalen;
		else 
			return -1;
		memcpy(buf, phdr, len);
		r += len;
	}
	else //if(r > w)
	{
		u_int8_t *pdatalen; 
		
		if(size-r < size_of_hdr)
		{
			u_int32_t datapos;

			//first copy gsmd_msg_hdr ,then check len
			memcpy(buf, (char *)(mboxbuf->buf+r), size-r);
			datapos = size_of_hdr - (size-r);
			memcpy((char *)(buf+size-r), mboxbuf->buf, datapos);

			phdr = (struct gsmd_msg_hdr *)buf;
			//datalen = (u_int16_t)phdr->len;
			pdatalen = (u_int8_t *)(buf +offsetof(struct gsmd_msg_hdr,len));
			memcpy(&datalen, pdatalen, 2);
			
			if(len >= size_of_hdr + datalen)
				len = size_of_hdr + datalen;
			else
				return -1;

			//copy data
			memcpy(phdr->data, (char *)(mboxbuf->buf + datapos), datalen);
			r = datapos + datalen;
		}
		else
		{
			phdr = (struct gsmd_msg_hdr *)(mboxbuf->buf+r);
			//datalen = (u_int16_t)phdr->len;
			pdatalen = (u_int8_t *)(mboxbuf->buf+r +offsetof(struct gsmd_msg_hdr,len));
			memcpy(&datalen, pdatalen, 2);
			
			if(len >= size_of_hdr + datalen)
				len = size_of_hdr + datalen;
			else 
				return -1;
				
			if(size-r < len)
			{
				memcpy(buf, phdr, size-r);
				memcpy((char *)(buf+size-r), mboxbuf->buf, len-(size-r));
				r = len-(size-r);
			}
			else
			{
				memcpy(buf, phdr, len);
				r += len;
				if(r >= size)
					r = 0;
			}
		}
	}

	Mutex_Protect();
	mboxbuf->r = r;
	Mutex_Unprotect();

	return len;
}




