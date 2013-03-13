/* libgsmd pin support
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

//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>

#include "types.h"

#include "gsmdevent.h"

#include "libgsmd.h"
#include "lgsm_internals.h"


extern const char *pin_type_names;



const char *lgsm_pin_name(enum gsmd_pin_type ptype)
{
	if (ptype >= __NUM_GSMD_PIN)
		return "unknown";

	return pin_type_names[ptype];
}


int lgsm_security_pin_status(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_GET_STATUS);
}


int lgsm_security_pin_input(struct lgsm_handle *lh, struct gsmd_pin *gp)
{
	int rc;
	
	struct {
		struct gsmd_msg_hdr gmh;
		struct gsmd_pin gp;
	}*gm;

	
	if (strlen(gp->pin) > GSMD_PIN_MAXLEN || 
	    (gp->newpin && strlen(gp->newpin) > GSMD_PIN_MAXLEN) ||
	    gp->type >= __NUM_GSMD_PIN)
		return -EINVAL;

	gm = (void *) lgsm_gmh_fill(GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_INPUT,
				    sizeof(struct gsmd_pin));
	if (!gm)
		return -ENOMEM;

	gm->gp.type = gp->type;
	strncpy(gm->gp.pin, gp->pin, sizeof(gm->gp.pin));

	switch (gp->type) {
	case GSMD_PIN_SIM_PUK:
	case GSMD_PIN_SIM_PUK2:
		/* GSM 07.07 explicitly states that only those two PUK types
		 * require a new pin to be specified! Don't know if this is a
		 * bug or a feature. */
		if (!gp->newpin) {
			FREE(gm);
			return -EINVAL;
		}
		strncpy(gm->gp.newpin, gp->newpin, sizeof(gm->gp.newpin));
		break;
	default:
		break;
	}
	//DEBUGP("sending pin='%s', newpin='%s'\n", gm->gp.pin, gm->gp.newpin);
	rc = lgsm_send(lh, &gm->gmh);
	FREE(gm);

	return rc;
}


int lgsm_security_pin_remain_times(struct lgsm_handle *lh)
	{
		return lgsm_send_simple(lh, GSMD_MSG_SECURITY, GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES);
	}


int lgsm_security_lock_en(struct lgsm_handle *lh, struct gsmd_facility *gf)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_facility *gf2;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SECURITY,
			GSMD_SECURITY_LOCK_EN, sizeof(struct gsmd_facility));
	if (!gmh)
		return -ENOMEM;

	gf2 = (struct gsmd_facility *)gmh->data;
	gf2->type = gf->type;
	strncpy(gf2->pwd, gf->pwd, sizeof(gf2->pwd));

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_security_lock_dis(struct lgsm_handle *lh, struct gsmd_facility *gf)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_facility *gf2;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SECURITY,
			GSMD_SECURITY_LOCK_DIS, sizeof(struct gsmd_facility));
	if (!gmh)
		return -ENOMEM;

	gf2 = (struct gsmd_facility *)gmh->data;
	gf2->type = gf->type;
	strncpy(gf2->pwd, gf->pwd, sizeof(gf2->pwd));

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_security_lock_status(struct lgsm_handle *lh, struct gsmd_facility *gf)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_facility *gf2;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SECURITY,
			GSMD_SECURITY_LOCK_GET_STATUS, sizeof(struct gsmd_facility));
	if (!gmh)
		return -ENOMEM;

	gf2 = (struct gsmd_facility *)gmh->data;
	gf2->type = gf->type;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_security_change_pwd(struct lgsm_handle *lh, struct gsmd_facility *gf)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_facility *gf2;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SECURITY,
			GSMD_SECURITY_CHANGE_PWD, sizeof(struct gsmd_facility));
	if (!gmh)
		return -ENOMEM;

	gf2 = (struct gsmd_facility *)gmh->data;
	gf2->type = gf->type;
	strncpy(gf2->pwd, gf->pwd, sizeof(gf2->pwd));
	strncpy(gf2->newpwd, gf->newpwd, sizeof(gf2->newpwd));
	
	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

		
		

