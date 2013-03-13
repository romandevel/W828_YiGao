#ifndef _LIBGSMD_EVENT_H
#define _LIBGSMD_EVENT_H

#include "gsmdevent.h"


enum to_appctrl_msg_chl
{
	MSG_CHL_VOIP = 0,
	MSG_CHL_GSMCDMA
};


struct gsmcdma_to_app_msg{
	enum gsmd_events msgtype;
	
	union {
		struct {
			enum gsmd_call_type type;
		} call;
		struct {
			struct gsmd_addr addr;
		} clip;
		struct {
			struct gsmd_addr addr;
		} colp;
		struct {
			int inlined;
			u_int8_t memtype;
			int index;
		} cbm;
		struct {
			int inlined;
			u_int8_t memtype;
			int index;
		} ds;
		struct {
			enum gsmd_pin_type type;
		} pin;
		struct {
			enum gsm0707_cme_error number;
		} cme_err;
		struct {
			enum gsm0705_cms_error number;
		} cms_err;
		struct {
			struct gsmd_addr addr;
			u_int8_t classx;
			char	alpha[GSMD_ALPHA_MAXLEN+1];
			u_int8_t cli; 
		} ccwa;

		struct {
			enum gsmd_call_stat stat;
		} clcc;
	} u;
};

struct to_appctrl_msg
{
	enum to_appctrl_msg_chl chl;
	
	union
	{
		struct gsmcdma_to_app_msg	gsmcdma_msg;
	}u;
};

struct sms_stat_report
{
	u_int8_t result;
	char	 number[GSMD_ADDR_MAXLEN+1];
};




/* Prototype of libgsmd callback handler function */
typedef int lgsm_evt_handler(struct lgsm_handle *lh, int evt_type, struct gsmd_evt_auxdata *aux);


extern int lgsm_evt_init(struct lgsm_handle *lh);
extern void lgsm_evt_exit(struct lgsm_handle *lh);


#endif
