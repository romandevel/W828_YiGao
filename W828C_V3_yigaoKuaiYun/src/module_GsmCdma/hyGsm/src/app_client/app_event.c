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

#include "llist.h"
#include "libgsmd.h"
#include "libgsmdevent.h"
#include "libgsmdsms.h"

#include "PhoneDebug.h"



#if 0
static int incall_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Incoming call type = %u\n", aux->u.call.type);
	//PhoneTrace(0, "Incall type %u", aux->u.call.type);

	return 0;
}

static int insms_handler(struct lgsm_handle *lh, int evt,
		struct gsmd_evt_auxdata *aux)
{
	struct gsmd_sms_list *sms;
	char payload[GSMD_SMS_DATA_MAXLEN];
	
	if (aux->u.sms.inlined) {
		sms = (struct gsmd_sms_list *) aux->data;
		if(sms->payload.is_voicemail)
			DEBUGP("EVENT: You have a voice mail \n");
		DEBUGP("EVENT: Incoming SMS from/to %s%s, at %i%i-%i%i-%i%i "
				"%i%i:%i%i:%i%i, GMT%c%i\n",
				((sms->addr.type & __GSMD_TOA_TON_MASK) ==
				 GSMD_TOA_TON_INTERNATIONAL) ? "+" : "",
				sms->addr.number,
				sms->time_stamp[0] & 0xf,
				sms->time_stamp[0] >> 4,
				sms->time_stamp[1] & 0xf,
				sms->time_stamp[1] >> 4,
				sms->time_stamp[2] & 0xf,
				sms->time_stamp[2] >> 4,
				sms->time_stamp[3] & 0xf,
				sms->time_stamp[3] >> 4,
				sms->time_stamp[4] & 0xf,
				sms->time_stamp[4] >> 4,
				sms->time_stamp[5] & 0xf,
				sms->time_stamp[5] >> 4,
				(sms->time_stamp[6] & 8) ? '-' : '+',
				(((sms->time_stamp[6] << 4) |
				  (sms->time_stamp[6] >> 4)) & 0x3f) >> 2);
		if ( strlen(aux->u.sms.alpha) )		  
			DEBUGP("From %s\n", aux->u.sms.alpha);
		if (sms->payload.coding_scheme == ALPHABET_DEFAULT) {
			unpacking_7bit_character(&sms->payload, payload);
			DEBUGP("\"%s\"\n", payload);
		} else if (sms->payload.coding_scheme == ALPHABET_8BIT)
			DEBUGP("8-bit encoded data\n");
		else if (sms->payload.coding_scheme == ALPHABET_UCS2)
			DEBUGP("Unicode-16 encoded text\n");
	} else	{
		DEBUGP("EVENT: Incoming SMS stored at location %i\n",
				aux->u.sms.index);
				
		//PhoneTrace(0, "sms from %s", aux->u.sms.alpha);
		lgsm_sms_read(lh, aux->u.sms.index);
	}
	return 0;
}


static int incbm_handler(struct lgsm_handle *lh, int evt,
		struct gsmd_evt_auxdata *aux)
{
	struct gsmd_cbm *msg;
	char payload[95];
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
	
	
	if (aux->u.cbm.inlined) {
		msg = (struct gsmd_cbm *) aux->data;
		DEBUGP("EVENT: Incoming %s-wide Cell Broadcast message in "
				"%s (page %i of %i)\n",
				scope_name[msg->serial.scope],
				lang_name[msg->language],
				msg->page, msg->pages);

		if (msg->coding_scheme == ALPHABET_DEFAULT) {
			cbm_unpacking_7bit_character((char *)msg->data, payload);
			DEBUGP("\"%s\"\n", payload);
		} else if (msg->coding_scheme == ALPHABET_8BIT)
			DEBUGP("8-bit encoded data\n");
		else if (msg->coding_scheme == ALPHABET_UCS2)
			DEBUGP("Unicode-16 encoded text\n");
	} else
		DEBUGP("EVENT: Incoming Cell Broadcast message stored at "
				"location %i\n", aux->u.cbm.index);
	return 0;
}

static int inds_handler(struct lgsm_handle *lh, int evt,
		struct gsmd_evt_auxdata *aux)
{
	if (aux->u.ds.inlined) {
		struct gsmd_sms_list *sms;
		sms = (struct gsmd_sms_list *) aux->data;
		DEBUGP("EVENT: Incoming Status Report\n");
		DEBUGP("message ref = %d, status = %d\n", sms->index,sms->payload.coding_scheme);
	}
	else
		DEBUGP("EVENT: Incoming Status Report stored at location %i\n",
				aux->u.ds.index);
	return 0;
}

static int clip_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Incoming call clip = %s\n", aux->u.clip.addr.number);

	//PhoneTrace(0, "Incall clip %s", aux->u.clip.addr.number);
	return 0;
}

static int colp_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Outgoing call colp = %s\n", aux->u.colp.addr.number);

	//PhoneTrace(0, "Outcall colp %s", aux->u.colp.addr.number);
	return 0;
}

static int netreg_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Netreg ");

	switch (aux->u.netreg.state) {
	case GSMD_NETREG_UNREG:
		DEBUGP("not searching for network ");
		break;
	case GSMD_NETREG_REG_HOME:
		DEBUGP("registered (home network) ");
		break;
	case GSMD_NETREG_UNREG_BUSY:
		DEBUGP("searching for network ");
		break;
	case GSMD_NETREG_DENIED:
		DEBUGP("registration denied ");
		break;
	case GSMD_NETREG_REG_ROAMING:
		DEBUGP("registered (roaming) ");
		break;
	default:
		break;
	}

	if (aux->u.netreg.lac)
		DEBUGP("LocationAreaCode = 0x%04X ", aux->u.netreg.lac);
	if (aux->u.netreg.ci)
		DEBUGP("CellID = 0x%04X ", aux->u.netreg.ci);
	
	DEBUGP("\n");

	return 0;
}

static int sigq_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Signal Quality: %u\n", aux->u.signal.sigq.rssi);
	return 0;
}

static int ccwa_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	DEBUGP("EVENT: Call Waiting: %s,%d\n", aux->u.ccwa.addr.number, aux->u.ccwa.addr.type);

	//PhoneTrace(0, "Call Wait %s,%d", aux->u.ccwa.addr.number, aux->u.ccwa.addr.type);
	return 0;
}


static const char *cprog_names[] = {
	/*[GSMD_CALLPROG_SETUP]		= */		"SETUP",
	/*[GSMD_CALLPROG_DISCONNECT]	= */	"DISCONNECT",
	/*[GSMD_CALLPROG_ALERT]		= */		"ALERT",
	/*[GSMD_CALLPROG_CALL_PROCEED]	= */	"PROCEED",
	/*[GSMD_CALLPROG_SYNC]		= */		"SYNC",
	/*[GSMD_CALLPROG_PROGRESS]	= */		"PROGRESS",
	/*[GSMD_CALLPROG_CONNECTED]	= */		"CONNECTED",
	/*[GSMD_CALLPROG_RELEASE]		= */	"RELEASE",
	/*[GSMD_CALLPROG_REJECT]		= */	"REJECT",
	/*[GSMD_CALLPROG_UNKNOWN]		= */	"UNKNOWN",
};


static const char *cdir_names[] = {
	/*[GSMD_CALL_DIR_MO]		= */"Outgoing",
	/*[GSMD_CALL_DIR_MT]		= */"Incoming",
	/*[GSMD_CALL_DIR_CCBS]		= */"CCBS",
	/*[GSMD_CALL_DIR_MO_REDIAL]	= */"Outgoing Redial",
};

static int cprog_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	const char *name, *dir;

	if (aux->u.call_status.prog >= SIZE_OF_ARRAY(cprog_names))
		name = "UNDEFINED";
	else
		name = cprog_names[aux->u.call_status.prog];

	if (aux->u.call_status.dir >= SIZE_OF_ARRAY(cdir_names))
		dir = "";
	else
		dir = cdir_names[aux->u.call_status.dir];

	DEBUGP("EVENT: %s Call Progress: %s\n", dir, name);

	return 0;
}

static int error_handler(struct lgsm_handle *lh, int evt, struct gsmd_evt_auxdata *aux)
{
	if(aux->u.cme_err.number)
		DEBUGP("cme error: %u\n", aux->u.cme_err.number);
	else if(aux->u.cms_err.number)
		DEBUGP("cms error: %u\n", aux->u.cms_err.number);
		
	return 0;
}


int event_init(struct lgsm_handle *lh)
{
	int rc;

	rc  = lgsm_evt_handler_register(lh, GSMD_EVT_IN_CALL, &incall_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_IN_CLIP, &clip_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_IN_SMS, &insms_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_IN_CBM, &incbm_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_IN_DS, &inds_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_OUT_COLP, &colp_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_NETREG, &netreg_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_SIGNAL, &sigq_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_OUT_STATUS, &cprog_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_IN_ERROR, &error_handler);
	rc |= lgsm_evt_handler_register(lh, GSMD_EVT_CALL_WAIT, &ccwa_handler);
	return rc;
}
#endif
