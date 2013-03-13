/* select() FD handling for GSM Daemon.
 * (C) 2000-2006 by Harald Welte <laforge@gnumonks.org>
 *
 * Based on code originally written by myself for ulogd.
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


#include "gsmd_select.h"



static int maxfd = 0;
static struct llist_head gsmd_fds =  { &(gsmd_fds), &(gsmd_fds) };



void gsmd_fd_init(void)
{
	INIT_LLIST_HEAD(&gsmd_fds);
}


struct llist_head *get_gsmd_fds(void)
{
	return &gsmd_fds;
}



int gsmd_register_fd(struct gsmd_fd *fd)
{
	/* Register FD */
	if (fd->fd > maxfd)
		maxfd = fd->fd;

	llist_add_tail(&fd->list, &gsmd_fds);

	return 0;
}



void gsmd_unregister_fd(struct gsmd_fd *fd)
{
	llist_del(&fd->list);
	fd->cb = NULL;
	
	// Unregister FD  ?
	if (fd->fd == maxfd && 0<maxfd)
		maxfd--;
}



struct gsmd_fd *ufd2;

int gsmd_select_main(struct gsmd *g)
{
	struct llist_head *head = &gsmd_fds;
	struct gsmd_fd *ufd;
	int flags = 0;


	// prepare read and write, call registered callback functions
	//llist_for_each_entry_safe(ufd, ufd2, struct gsmd_fd, &gsmd_fds, list) 
	for ( ufd  =  (( struct gsmd_fd  *)((&gsmd_fds)->next)); 
	 	&(ufd->list)  !=  head ; ) 
	{
		ufd2  =  (struct gsmd_fd  *)(ufd->list.next);
	
		flags = 0;

		if (ufd->when & GSMD_FD_READ)
			flags |= GSMD_FD_READ;

		if (ufd->when & GSMD_FD_WRITE)
			flags |= GSMD_FD_WRITE;

		if (ufd->when & GSMD_FD_WRITE_GPRS)
			flags |= GSMD_FD_WRITE_GPRS;

		if (ufd->when & GSMD_FD_EXCEPT)
			flags |= GSMD_FD_EXCEPT;
			
		if (flags)
			ufd->cb(ufd->fd, flags, ufd->data);
		
		ufd  =  ufd2 ;
	}

	return 0;
}




	