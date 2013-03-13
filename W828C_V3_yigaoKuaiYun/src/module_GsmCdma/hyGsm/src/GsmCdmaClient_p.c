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
//#include <stdlib.h>
//#include <string.h>

#include "libgsmd.h"

#include "app_pin.h"
//#include "app_event.h"
#include "app_shell.h"
#include "app_atcmd.h"


#ifndef SIZE_OF_ARRAY
#define SIZE_OF_ARRAY(x) (sizeof(x) / sizeof((x)[0]))
#endif


struct lgsm_handle *lh_sms_other;
struct lgsm_handle *lgsmh_voicecall;
struct lgsm_handle *lgsmh_gprs = NULL;
static struct lgsm_handle *lgsmh;

int *EventGsmCdmaClient = NULL;

extern int gsmcdma_net_reg_stat;
extern u_int32_t gsmcdma_net_sigq;
extern int gsmcdma_ph_simcard;

extern int gsmcdma_client_exit;
extern SEMAPHORE *gsmcdma_client_exit_sem;

extern u_int8_t unsol_sim_phb_ready;
extern u_int8_t unsol_sim_sms_ready;
extern u_int8_t unsol_modem_reset;


/*---------------------------------------------------------
*函数: GsmCdmaClient_ThreadMethodInit
*功能: 初始化
*参数: 
*返回: none
*---------------------------------------------------------*/
static void GsmCdmaClient_ThreadMethodInit(void)
{
	gsmcdma_client_exit = 0;

	gsmcdma_net_reg_stat = 0;
	gsmcdma_net_sigq = 0;
	gsmcdma_ph_simcard = 0;
	
	unsol_sim_phb_ready = 0;
	unsol_sim_sms_ready = 0;
	unsol_modem_reset = 0;
	
	app_shell_init();
}



/*-----------------------------------------------------------------------------
* 函数:	GsmCdmaClient_ThreadMethod
* 功能:	
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
int GsmCdmaClient_ThreadMethod(void)
{
	//char *pin = NULL;
	
	GsmCdmaClient_ThreadMethodInit();

	ewait(&EventGsmCdmaClient);
	
	lgsmh = lgsm_init(USER_CLIENT);
	if (!lgsmh) 
	{
		//DEBUGP("Can't connect to gsmd\n");
		return -1;
	}
	lgsm_evt_init(lgsmh);
	//pin_init(lgsmh, pin);
	//event_init(lgsmh);

	shell_main(lgsmh);
	
	lgsm_evt_exit(lgsmh);
	lgsm_exit(lgsmh);
	
	semaphore_post(gsmcdma_client_exit_sem);
	_exit(1);
	
	return 0;
}

