/* libgsmd network related functions
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

#include "libgsmd.h"
#include "misc.h"
#include "libgsmdsms.h"
#include "usock.h"
#include "gsmdevent.h"
#include "lgsm_internals.h"
#include "phonebook.h"


/* Get the current network registration status */
int lgsm_get_netreg_state(struct lgsm_handle *lh,
		enum lgsm_netreg_state *state)
{
	*state = lh->netreg_state;
	return 0;
}

int lgsm_oper_get(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_GET);
}

int lgsm_oper_n_get(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_N_GET);
}

int lgsm_opers_get(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_OPER_LIST);
}

int lgsm_netreg_register(struct lgsm_handle *lh, gsmd_oper_numeric oper)
{
	struct gsmd_msg_hdr *gmh;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK, GSMD_NETWORK_REGISTER,
			sizeof(gsmd_oper_numeric));

	if (!gmh)
		return -ENOMEM;

	memcpy(gmh->data, oper, sizeof(gsmd_oper_numeric));

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) 
	{
		lgsm_gmh_free(gmh);
		return -EIO;
	}
	lgsm_gmh_free(gmh);
	return 0;
}
int lgsm_netreg_re_register(struct lgsm_handle *lh, gsmd_oper_numeric oper)
{
	struct gsmd_msg_hdr *gmh;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK, GSMD_NETWORK_RE_REGISTER,
			sizeof(gsmd_oper_numeric));

	if (!gmh)
		return -ENOMEM;

	memcpy(gmh->data, oper, sizeof(gsmd_oper_numeric));

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO; 
	}

	lgsm_gmh_free(gmh);
	return 0;
}

int lgsm_netreg_deregister(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_DEREGISTER);
}

int lgsm_netreg_query(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_QUERY_REG);
}

int lgsm_signal_quality(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_SIGQ_GET);
}

int lgsm_prefoper_list(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_LIST);
}

int lgsm_prefoper_delete(struct lgsm_handle *lh, int index)
{
	struct gsmd_msg_hdr *gmh;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_DEL,
			sizeof(int));
	if (!gmh)
		return -ENOMEM;

	memcpy(gmh->data, &index, sizeof(int));

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);
	return 0;
}

int lgsm_prefoper_add(struct lgsm_handle *lh, gsmd_oper_numeric oper)
{
	struct gsmd_msg_hdr *gmh;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_ADD,
			sizeof(gsmd_oper_numeric));
	if (!gmh)
		return -ENOMEM;

	memcpy(gmh->data, oper, sizeof(gsmd_oper_numeric));

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);
	return 0;
}

int lgsm_prefoper_get_space(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_PREF_SPACE);
}

int lgsm_get_subscriber_num(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_GET_NUMBER);
}

/*
AT+SNUM=[<index>][,<number>[,<type>[,<text>]]]

已经修改，能支持UCS2编码存储，

index(0-1), 最多支持两条

type(0-1)，0:ASCII code, 1: UCS2 code
*/
int lgsm_set_subscriber_num(struct lgsm_handle *lh, const struct lgsm_phonebook *pb)
{
	/* FIXME: only support alphabet now */
	struct gsmd_msg_hdr *gmh;
	struct gsmd_phonebook *gp;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK,
			GSMD_NETWORK_SET_NUMBER, sizeof(*gp));
	if (!gmh)
		return -ENOMEM;
	gp = (struct gsmd_phonebook *) gmh->data;
	gp->index = pb->index;
	memcpy(gp->numb, pb->numb, sizeof(gp->numb));
	gp->numb[sizeof(gp->numb)-1] = '\0';
	gp->type = pb->type;
	memcpy(gp->text, pb->text, sizeof(gp->text));
	gp->text[sizeof(gp->text)-1] = '\0';

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}



int lgsm_get_local_timestamp(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_GET_TIMESTAMP);
}



int lgsm_voicemail_set(struct lgsm_handle *lh, const char *number)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_voicemail *vmail;

	gmh = lgsm_gmh_fill(GSMD_MSG_NETWORK,
			GSMD_NETWORK_VMAIL_SET, sizeof(*vmail));
	if (!gmh)
		return -ENOMEM;

	vmail = (struct gsmd_voicemail *) gmh->data;
	vmail->enable = 1;
	if (lgsm_number2addr(&vmail->addr, number, 0)){
		lgsm_gmh_free(gmh);
		return -EINVAL;
	}

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);
	return 0;
}

/* Get currently configured voice mail number */
int lgsm_voicemail_get(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_NETWORK, GSMD_NETWORK_VMAIL_GET);
}
