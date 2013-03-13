/* libgsmd sms support
 *
 * (C) 2007 by OpenMoko, Inc.
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
//#include <stdio.h>

#include "libgsmd.h"
#include "misc.h"
#include "libgsmdsms.h"

#include "usock.h"
#include "gsmdevent.h"

#include "lgsm_internals.h"


int lgsm_sms_list(struct lgsm_handle *lh, const struct lgsm_sms_list *sms_lt)
{
	/* FIXME: only support PDU mode now */
	struct gsmd_msg_hdr *gmh;
	struct _gsmd_sms_list *gsl;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_LIST, sizeof(*gsl));
	if (!gmh)
		return -ENOMEM;
	gsl = (struct _gsmd_sms_list *) gmh->data;
	gsl->stat = sms_lt->stat;
	gsl->mode = sms_lt->mode;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}



int lgsm_sms_read(struct lgsm_handle *lh, const struct lgsm_sms_read *sms_rd)
{
	struct gsmd_msg_hdr *gmh;
	struct gsmd_sms_read *gsr;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_READ, sizeof(*gsr));
	if (!gmh)
		return -ENOMEM;
	gsr = (struct gsmd_sms_read *) gmh->data;
	gsr->index = sms_rd->index;
	gsr->mode = sms_rd->mode;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}



int lgsm_sms_delete(struct lgsm_handle *lh,	int index)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_DELETE, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *)gmh->data = index;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_sms_delete_all(struct lgsm_handle *lh,	enum lgsm_msg_sms_delall_type type)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_DELETE_ALL, sizeof(int));
	if (!gmh)
		return -ENOMEM;
	*(int *)gmh->data = type;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}


int lgsm_number2addr(struct gsmd_addr *dst, const char *src, int skipplus)
{
	char *ch;

	if (strlen(src) + 1 > sizeof(dst->number))
		return 1;
	if (src[0] == '+') {
		dst->type =
			GSMD_TOA_NPI_ISDN |
			GSMD_TOA_TON_INTERNATIONAL |
			GSMD_TOA_RESERVED;
		strcpy(dst->number, src + skipplus);
	} else {
		dst->type =
			GSMD_TOA_NPI_ISDN |
			GSMD_TOA_TON_UNKNOWN |
			GSMD_TOA_RESERVED;
		strcpy(dst->number, src);
	}

	for (ch = dst->number; *ch; ch ++)
		if (*ch < '0' || *ch > '9')
			return 1;
	return 0;
}

int lgsm_sms_send(struct lgsm_handle *lh,
		const struct lgsm_sms *sms)
{
	/* FIXME: only support PDU mode */
	struct gsmd_msg_hdr *gmh;
	struct gsmd_sms_submit *gss;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_SEND, sizeof(*gss));
	if (!gmh)
		return -ENOMEM;
	gss = (struct gsmd_sms_submit *) gmh->data;

	if (lgsm_number2addr(&gss->addr, sms->addr, 1))
		return -EINVAL;

	gss->ask_ds = sms->ask_ds;
	gss->payload.has_header = sms->has_header;
	gss->payload.length = sms->length;
	gss->payload.coding_scheme = sms->alpha;
	memcpy(gss->payload.data, sms->data, LGSM_SMS_DATA_MAXLEN);

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_sms_write(struct lgsm_handle *lh,
		const struct lgsm_sms_write *sms_write)
{
	/* FIXME: only support PDU mode */
	struct gsmd_msg_hdr *gmh;
	struct gsmd_sms_write *gsw; 
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS,
			GSMD_SMS_WRITE, sizeof(*gsw));
	if (!gmh)
		return -ENOMEM;
	gsw = (struct gsmd_sms_write *) gmh->data;

	gsw->stat = sms_write->stat;

	if (lgsm_number2addr(&gsw->sms.addr, sms_write->sms.addr, 1))
		return -EINVAL;

	gsw->sms.ask_ds = sms_write->sms.ask_ds;
	//gsw->sms.payload.has_header = 0;
	gsw->sms.payload.has_header = sms_write->sms.has_header;
	gsw->sms.payload.length = sms_write->sms.length;
	gsw->sms.payload.coding_scheme = sms_write->sms.alpha;
	memcpy(gsw->sms.payload.data, sms_write->sms.data,
			LGSM_SMS_DATA_MAXLEN);

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;
}

int lgsm_sms_get_storage(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_SMS, GSMD_SMS_GET_MSG_STORAGE);
}

int lgsm_sms_set_storage(struct lgsm_handle *lh, enum ts0705_mem_type mem1,
		enum ts0705_mem_type mem2, enum ts0705_mem_type mem3)
{
	struct gsmd_msg_hdr *gmh =
		lgsm_gmh_fill(GSMD_MSG_SMS, GSMD_SMS_SET_MSG_STORAGE,
				3 * sizeof(enum ts0705_mem_type));
	if (!gmh)
		return -ENOMEM;

	((enum ts0705_mem_type *) gmh->data)[0] = mem1;
	((enum ts0705_mem_type *) gmh->data)[1] = mem2;
	((enum ts0705_mem_type *) gmh->data)[2] = mem3;

	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);
	return 0;
}

int lgsm_sms_get_smsc(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_SMS, GSMD_SMS_GET_SERVICE_CENTRE);
}

int lgsm_sms_set_smsc(struct lgsm_handle *lh, const char *number)
{
	struct gsmd_msg_hdr *gmh =
		lgsm_gmh_fill(GSMD_MSG_SMS, GSMD_SMS_SET_SERVICE_CENTRE,
				sizeof(struct gsmd_addr));
	if (!gmh)
		return -ENOMEM;

	if (lgsm_number2addr((struct gsmd_addr *) gmh->data, number, 1)) {
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



//New SMS Message Indications
int lgsm_sms_get_cnmi(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_SMS, GSMD_SMS_GET_NEW_MSG_INDICATIONS);
}



int lgsm_sms_set_cnmi(struct lgsm_handle *lh, const struct lgsm_sms_cnmi *sms_cnmi)
{
	struct gsmd_sms_cnmi *gsc;
	struct gsmd_msg_hdr *gmh =
		lgsm_gmh_fill(GSMD_MSG_SMS, GSMD_SMS_SET_NEW_MSG_INDICATIONS,
				sizeof(struct gsmd_sms_cnmi));
				
	if (!gmh)
		return -ENOMEM;

	gsc = (struct gsmd_sms_cnmi *)gmh->data;
	gsc->mode = sms_cnmi ->mode;
	gsc->mt   = sms_cnmi ->mt;
	gsc->bm   = sms_cnmi ->bm;
	gsc->ds   = sms_cnmi ->ds;
	gsc->bfr  = sms_cnmi ->bfr;
	
	if (lgsm_send(lh, gmh) < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);
	return 0;
}



/*-----------------------------------------------------------------------------
* 函数:	lgsm_sms_set_fmt()
* 功能:	indicates to TA which input and output format of messages shall be used.
* 参数:	lh	---	
*		fmt	---	0:PDU	1:TEXT
* 返回:	0:OK
*----------------------------------------------------------------------------*/
int lgsm_sms_set_fmt(struct lgsm_handle *lh, int fmt)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_SMS, GSMD_SMS_SET_FMT, sizeof(int));
	if (!gmh)
		return -ENOMEM;

	*(int *) gmh->data = fmt;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}

	lgsm_gmh_free(gmh);

	return 0;

}



int packing_7bit_character(const char *src, struct lgsm_sms *dest)
{
	int len,i,j = 0;
	unsigned char ch1, ch2;
	int shift = 0;

	dest->alpha = ALPHABET_DEFAULT; //ALPHABET_UCS2 中文必须选择08，英文为00

	len = strlen(src);
	for ( i=0; i<len; i++ ) {
		
		ch1 = src[i] & 0x7F;
		ch1 = ch1 >> shift;
		ch2 = src[(i+1)] & 0x7F;
		ch2 = ch2 << (7-shift);

		ch1 = ch1 | ch2;

		if (j > sizeof(dest->data))
			break;
		dest->data[j++] = ch1;

		shift++;

		if ( 7 == shift ) {
			shift = 0;
			i++;
		}
	}

	dest->length = len;
	return j;
}



int packing_UCS2_character(const char *src, struct lgsm_sms *dest)
{
	int i,j = 0;
	unsigned char ch1, ch2;
	int shift = 0;

	dest->alpha = ALPHABET_UCS2; //ALPHABET_UCS2 中文必须选择08，英文为00
	dest->length = Hyc_Gbk2Unicode(src, dest->data, 0);

	return dest->length;
}




int packing_character(const char *src, struct lgsm_sms *dest)
{
	int i,len = 0;
	

	len = strlen(src);
	for( i=0; i<len; i++ ) {
		
		if(0x80 <= src[i])break; //GBK
	}

	if(i >= len)
	{
		i = packing_7bit_character(src, dest);
	}
	else //if(i < len)
	{
		i = packing_UCS2_character(src, dest);
	}

	return i;
}



int unpacking_7bit_character(const struct gsmd_sms *src, char *dest)
{
	int i = 0;
	int l = 0;

	if (src->has_header)
		l += ((src->data[0] << 3) + 14) / 7;
	i += l;
	for (; i < src->length; i ++)
		*(dest ++) =
			((src->data[(i * 7 + 7) >> 3] <<
			  (7 - ((i * 7 + 7) & 7))) |
			 (src->data[(i * 7) >> 3] >>
			  ((i * 7) & 7))) & 0x7f;
	*dest = '\0';

	return i - l;
}

int cbm_unpacking_7bit_character(const char *src, char *dest)
{
	int i;
	u_int8_t ch = 1;

	for (i = 0; i < 93 && ch; i ++)
		*(dest ++) = ch =
			((src[(i * 7 + 7) >> 3] << (7 - ((i * 7 + 7) & 7))) |
			 (src[(i * 7) >> 3] >> ((i * 7) & 7))) & 0x7f;
	*dest = '\0';

	return i;
}

/* Refer to 3GPP TS 11.11 Annex B */
int packing_UCS2_80(char *src, char *dest)
{
	return 0;
}

/* Refer to 3GPP TS 11.11 Annex B */
int unpacking_UCS2_80(char *src, char *dest)
{
	return 0;
}

/* Refer to 3GPP TS 11.11 Annex B */
int packing_UCS2_81(char *src, char *dest)
{
	return 0;
}

/* Refer to 3GPP TS 11.11 Annex B */
int unpacking_UCS2_81(char *src, char *dest)
{
	return 0;
}

/* Refer to 3GPP TS 11.11 Annex B */
int packing_UCS2_82(char *src, char *dest)
{
	return 0;
}

/* Refer to 3GPP TS 11.11 Annex B */
int unpacking_UCS2_82(char *src, char *dest)
{
	return 0;
}

int lgsm_cb_subscribe(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_CB, GSMD_CB_SUBSCRIBE);
}

int lgsm_cb_unsubscribe(struct lgsm_handle *lh)
{
	return lgsm_send_simple(lh, GSMD_MSG_CB, GSMD_CB_UNSUBSCRIBE);
}
