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


int lgsm_gprs_connect_init(struct lgsm_handle *lh,
			const struct lgsm_data_connect_set *connect_set)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_data_connect_set *gc;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_DATACALL,
			    GSMD_DATACALL_DIAL, sizeof(*gc));
	if (!gmh)
		return -ENOMEM;
	gc = (struct gsmd_data_connect_set *) gmh->data;
	memcpy(gc->addr, connect_set->addr, sizeof(connect_set->addr));
	gc->addr[sizeof(gc->addr)-1] = '\0';

	memcpy(gc->l2p, connect_set->l2p, sizeof(connect_set->l2p));
	gc->l2p[sizeof(gc->l2p)-1] = '\0';

	gc->cid = connect_set->cid;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_gprs_set_APN(struct lgsm_handle *lh, char *APN)
{
	struct gsmd_msg_hdr *gmh;
	char *payload;
	int len,rc;

	len = strlen(APN)+1;
	gmh = lgsm_gmh_fill(GSMD_MSG_DATACALL,
			    GSMD_DATACALL_CGDCONT, len);
	if (!gmh)
		return -ENOMEM;

	payload = (char *)gmh->data;
	strncpy(payload, APN, len);

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}






int lgsm_gprs_send(struct lgsm_handle *lh, char *buf, int len)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_DATACALL, GSMD_DATACALL_TRANSFER, len);
	if (!gmh)
		return -ENOMEM;
	memcpy(gmh->data, buf, len);

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) 
	{
		//hyUsbPrintf("lgsm_gprs_send fail send len = %d \r\n",rc);
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return rc;
}

