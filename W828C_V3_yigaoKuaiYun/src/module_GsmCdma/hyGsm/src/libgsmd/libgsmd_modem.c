/* libgsmd modem functions
 *
 * (C) 2006-2007 by OpenMoko, Inc.
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



int lgsm_modem_power(struct lgsm_handle *lh, int power)
{
	int rc;
	int subtype;
	struct gsmd_msg_hdr *gmh;


	if (0 == power || 1 == power)
		subtype = GSMD_MODEM_POWEROFF;
	else
		subtype = GSMD_MODEM_POWERON;

	gmh = lgsm_gmh_fill(GSMD_MSG_MODEM,	subtype, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *)gmh->data = power;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_open_mux(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_MUX_MODE);
}
int lgsm_open_dcl0(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_DLC0_MODE);
}
int lgsm_dlc1_pn(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_DLC1_PN);
}

int lgsm_open_dcl1(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_DLC1_MODE);
}
int lgsm_dlc2_pn(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_DLC2_PN);
}
int lgsm_open_dcl2(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_MODEM, GSMD_OPEN_DLC2_MODE);
}

