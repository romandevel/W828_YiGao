/* libgsmd voice call support
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
#include "lgsm_internals.h"


int lgsm_voice_out_init(struct lgsm_handle *lh,
			const struct lgsm_addr *number)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_addr *ga;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_DIAL, sizeof(*ga));
	if (!gmh)
		return -ENOMEM;
	ga = (struct gsmd_addr *) gmh->data;
	ga->type = number->type;	/* FIXME: is this correct? */
	memcpy(ga->number, number->addr, sizeof(ga->number));
	ga->number[sizeof(ga->number)-1] = '\0';

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_dtmf(struct lgsm_handle *lh, char dtmf_char)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_dtmf *gd;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_DTMF, sizeof(*gd)+1);
	if (!gmh)
		return -ENOMEM;
	gd = (struct gsmd_dtmf *) gmh->data;
	gd->len = 1;
	gd->dtmf[0] = dtmf_char;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_in_accept(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_VOICECALL, GSMD_VOICECALL_ANSWER);
}

int lgsm_voice_hangup(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_VOICECALL, GSMD_VOICECALL_HANGUP);
}

int lgsm_voice_volume_set(struct lgsm_handle *lh, int volume)
{
	struct gsmd_msg_hdr *gmh;
	int rc;
	
	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_VOL_SET, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *) gmh->data = volume;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;


#if 0
	/* FIXME: we need to pass along the parameter */
	return lgsm_send_simple(lh, GSMD_MSG_VOICECALL, GSMD_VOICECALL_VOL_SET);
#endif
}


/*
SIM300
Swap The Audio Channels
AT+CHFA=[<n>]
<n>:0 Normal audio channel(default)
	1 Aux audio channel

IW328
Set audio mode. We have three audio mode , normal, loud speaker and handset.
AT+ ESAM = <mode>
<mode>:0 normal
	   1 handset
	   2 loudspeaker
*/
int lgsm_voice_audiochl_set(struct lgsm_handle *lh, int chl)
{
	struct gsmd_msg_hdr *gmh;
	int rc;
	
	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_AUDIOCHL_SET, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *) gmh->data = chl;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_voice_micgain_set(struct lgsm_handle *lh, const struct lgsm_microphone_gain *micgain)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_microphone_gain *gmg;
	int rc;
	
	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_MICGAIN_SET, sizeof(*gmg));
	if (!gmh)
		return -ENOMEM;
	gmg = (struct gsmd_microphone_gain *) gmh->data;
	gmg->channel = micgain->channel;
	gmg->gainlevel = micgain->gainlevel;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


/*
Mute Control
Enable/Disable the uplink voice muting during a voice call.
AT+CMUT=<n>
<n>:
	0 mute off(default)
	1 mute on
*/
int lgsm_voice_mute_set(struct lgsm_handle *lh, u_int8_t onoff)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_MUTE_SET, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *) gmh->data = (int)onoff;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}



int lgsm_voice_get_status(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_VOICECALL, GSMD_VOICECALL_GET_STAT);
}

int lgsm_voice_ctrl(struct lgsm_handle *lh, const struct lgsm_voicecall_ctrl *ctrl)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_call_ctrl *gcc;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			    GSMD_VOICECALL_CTRL, sizeof(*gcc));
	if (!gmh)
		return -ENOMEM;
	gcc = (struct gsmd_call_ctrl *) gmh->data;
	gcc->proc = ctrl->proc;
	gcc->idx = ctrl->idx;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_fwd_disable(struct lgsm_handle *lh, 
		           enum lgsmd_voicecall_fwd_reason reason)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_FWD_DIS, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = reason;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_fwd_enable(struct lgsm_handle *lh, 
                          enum lgsmd_voicecall_fwd_reason reason)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_FWD_EN, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = reason;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_fwd_stat(struct lgsm_handle *lh, 
                        enum lgsmd_voicecall_fwd_reason reason)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_FWD_STAT, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = (int)reason;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_fwd_reg(struct lgsm_handle *lh, 
		       struct lgsm_voicecall_fwd_reg *fwd_reg)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_call_fwd_reg *gcfr;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_FWD_REG, sizeof(struct gsmd_call_fwd_reg));
	if (!gmh)
		return -ENOMEM;

	gcfr = (struct gsmd_call_fwd_reg *)gmh->data;
	gcfr->reason = fwd_reg->reason;
	strcpy(gcfr->addr.number, fwd_reg->number.addr);

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_voice_fwd_erase(struct lgsm_handle *lh, 
                         enum lgsmd_voicecall_fwd_reason reason)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_FWD_ERAS, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = reason;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_voice_colp_set(struct lgsm_handle *lh, int value)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_COLP_SET, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = value;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_voice_clip_set(struct lgsm_handle *lh, int value)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_VOICECALL,
			GSMD_VOICECALL_CLIP_SET, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = value;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

/*
 * 112 ≤¶∫≈≤‚ ‘
 */
void gsm_dail_112_test(void)
{
	struct lgsm_addr addr;
	
	if(1 == get_modem_init_stat())
	{	
		int rc ;
		extern struct lgsm_handle *lgsmh_voicecall;
		if(NULL == lgsmh_voicecall)
		lgsmh_voicecall = lgsm_init(USER_VOICE);
		memset(&addr, 0, sizeof(addr));
		addr.type = 129;
		strncpy(addr.addr, "112", sizeof(addr.addr)-1);
		addr.addr[sizeof(addr.addr)-1] = '\0';

		rc = lgsm_voice_out_init(lgsmh_voicecall, &addr);
		//PhoneTrace3(0,"112 dail rc = %d",rc);
	}
	else
	{
		//PhoneTrace3(0,"112 dail fail --gsm no init");
	}
}

/* 
 * GSM π“∂œµÁª∞≤‚ ‘ 
 */
void gsm_hangup_112_test(void)
{
	if(1 == get_modem_init_stat())
	{
		int rc ;
		extern struct lgsm_handle *lgsmh_voicecall;
		rc = lgsm_voice_hangup(lgsmh_voicecall);
		//PhoneTrace3(0,"112 hungup rc = %d",rc);
	}
	else
	{
		//PhoneTrace3(0,"112 hungup fail --gsm no init");
	}
}
/*
 * ≤‚ ‘
 */
void gsm_dail_test(U8 dailhung)
{
	if(0 == dailhung)
	{
		gsm_dail_112_test();
	}
	else
	{
		gsm_hangup_112_test();
	}
}