/* libgsmd core
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
//#include <stdio.h>
//#include <string.h>

#include "types.h"
#include "socket_api.h"

#include "usock.h"
#include "libgsmd.h"
#include "libgsmdevent.h"
#include "lgsm_internals.h"



static int lgsm_open_backend(struct lgsm_handle *lh, const char *user)
{
	int rc;
	int len;
	int id;
	char *usermsg;


	//user request to connect
	for(id=0; id<USER_NUM; id++)
	{
		if(strstr(user_msg_str[CONNECT][id], user))
		{
			usermsg = user_msg_str[CONNECT][id];
			break;
		}
	}
	if(USER_NUM <= id)
		return -1;

	len = strlen(usermsg);
	if(0>=len)
		return -1;
		
	while(NULL == guser_reqmsg_que)
	{
		sleep(1);	//wait until gsmd is ready
	}
	rc = msg_queue_put(guser_reqmsg_que, usermsg, len, 10);
	if(0 > rc)
		return -1;
#if 1
	while(1)
	{
		rc = semaphore_timed_wait(psema_guser[id][CONNECT],500);
		if(rc >=0)
		{
			break;
		}
		else if(Net_GsmReset() == 1)
		{
			return -1;
		}
	}
#else
	rc = semaphore_wait(psema_guser[id][CONNECT]);
	if(rc < 0)
		return -1;
#endif
	lh->fd = id;

	return 0;
}


/* handle a packet that was received on the gsmd socket */
int lgsm_handle_packet(struct lgsm_handle *lh, char *buf, int len)
{
	struct gsmd_msg_hdr *gmh;
	lgsm_msg_handler *handler; 
	int rc = 0;

	//while (len) 
	//{
		if (len < sizeof(*gmh))
			return -EINVAL;
		gmh = (struct gsmd_msg_hdr *) buf;

		if (len - sizeof(*gmh) < gmh->len)
			return -EINVAL;
			
		/*
		len -= sizeof(*gmh) + gmh->len;
		buf += sizeof(*gmh) + gmh->len;
		*/
	
		if (gmh->msg_type >= __NUM_GSMD_MSGS)
			return -EINVAL;

		handler = lh->handler[gmh->msg_type];

		if (handler)
			rc |= handler(lh, gmh);
		//else
		//	gsmd_log(stderr, "unable to handle packet type=%u\n", gmh->msg_type);
	//}
	return rc;
}

int lgsm_register_handler(struct lgsm_handle *lh, int type, lgsm_msg_handler *handler)
{
	if (type >= __NUM_GSMD_MSGS)
		return -EINVAL;

	lh->handler[type] = handler;

	return 0;
}

void lgsm_unregister_handler(struct lgsm_handle *lh, int type)
{
	if (type < __NUM_GSMD_MSGS)
		lh->handler[type] = NULL;
}


int lgsm_fd(struct lgsm_handle *lh)
{
	return lh->fd;
}

struct lgsm_handle *lgsm_init(const char *user)
{
	struct lgsm_handle *lh = malloc(sizeof(*lh));

	memset(lh, 0, sizeof(*lh));
	lh->fd = -1;

	if (lgsm_open_backend(lh, user) < 0) 
	{
		free(lh);
		return NULL;
	}

	//lgsm_evt_init(lh);

	return lh;
}


int lgsm_exit(struct lgsm_handle *lh)
{
	int rc,len;
	char *usermsg;
	
	//user request to disconnect
	usermsg = user_msg_str[DISCONNECT][lh->fd];
	
	len = strlen(usermsg);
	if(0>=len)
		return -1;
	rc = msg_queue_put(guser_reqmsg_que, usermsg, len, 10);
	if(0 > rc)
		return -1;
#if 1	
	while(1)
	{
		rc = semaphore_timed_wait(psema_guser[lh->fd][DISCONNECT],500);
		if(rc >= 0)
		{
			break;
		}
		else if(Net_GsmReset() == 1)
		{
			free(lh);
			return -1;
		}
	}
#else
	rc = semaphore_wait(psema_guser[lh->fd][DISCONNECT]);
#endif
	free(lh);

	return 0;
}


// static u_int16_t next_msg_id;

int lgsm_send(struct lgsm_handle *lh, struct gsmd_msg_hdr *gmh)
{
	//Mutex_Protect();
	//if(0 == next_msg_id)	//gmh->id == 0的数据包为GSMD_MSG_EVENT,见usock_build_event()
	//	next_msg_id = 1;
	//gmh->id = next_msg_id++;
	//Mutex_Unprotect();
	
	gmh->id = 0;
	return u_send(lh->fd, (char *) gmh, sizeof(*gmh) + gmh->len, 0);
}



struct gsmd_msg_hdr *lgsm_gmh_fill(int type, int subtype, int payload_len)
{
	struct gsmd_msg_hdr *gmh = malloc(sizeof(*gmh)+payload_len);
	if (!gmh)
		return NULL;

	memset(gmh, 0, sizeof(*gmh)+payload_len);

	gmh->version = GSMD_PROTO_VERSION;
	gmh->msg_type = type;
	gmh->msg_subtype = subtype;
	gmh->len = payload_len;

	return gmh;
}

int lgsm_send_simple(struct lgsm_handle *lh, int type, int sub_type)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(type, sub_type, 0);
	if (!gmh)
		return -ENOMEM;
	rc = lgsm_send(lh, gmh);

	lgsm_gmh_free(gmh);
	return rc;
}


int lgsm_subscriptions(struct lgsm_handle *lh, u_int32_t subscriptions)
{
	struct gsmd_msg_hdr *gmh;
	int rc;

	gmh = lgsm_gmh_fill(GSMD_MSG_EVENT, GSMD_EVT_SUBSCRIPTIONS, sizeof(u_int32_t));
	if (!gmh)
		return -ENOMEM;
	*(u_int32_t *)gmh->data = subscriptions;

	rc = lgsm_send(lh, gmh);
	if (rc < gmh->len + sizeof(*gmh)) {
		lgsm_gmh_free(gmh);
		return -EIO;
	}
	lgsm_gmh_free(gmh);

	return 0;
}

