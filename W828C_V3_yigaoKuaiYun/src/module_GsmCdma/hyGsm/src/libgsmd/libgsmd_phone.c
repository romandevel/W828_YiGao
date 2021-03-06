/* libgsmd phone related functions
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
#include "usock.h"
#include "gsmdevent.h"
#include "lgsm_internals.h"




/*  funMode:
	(0 - minimum functionality)
	1 - full functionality (Default)
	4 - disable phone both transmit and receive RF circuits) */
int lgsm_phone_power(struct lgsm_handle *lh, int funMode)
{
	int rc,type;
	struct gsmd_msg_hdr *gmh;
	
	gmh = lgsm_gmh_fill(GSMD_MSG_PHONE,
			    GSMD_PHONE_POWER, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *)gmh->data = funMode;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}
	
	lgsm_gmh_free(gmh);
	
	return 0;
}

int lgsm_get_power_status(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_POWER_STATUS);
}

int lgsm_get_imsi(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_IMSI);
}

int lgsm_get_manufacturer(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_MANUF);
}

int lgsm_get_model(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_MODEL);
}

int lgsm_get_revision(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_REVISION);
}

int lgsm_get_serial(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_SERIAL);
}

int lgsm_get_battery(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_GET_BATTERY);
}

int lgsm_phone_vibrator(struct lgsm_handle *lh, int enable)
{
	int type;
	if(enable)
		type = GSMD_PHONE_VIB_ENABLE;
	else
		type = GSMD_PHONE_VIB_DISABLE;
	
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, type);
}


int lgsm_detect_simcard(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_PHONE, GSMD_PHONE_DETECT_SIMCARD);
}



