
#ifndef _APP_CLIENT_INCLUDES_H
#define _APP_CLIENT_INCLUDES_H



#include "lgsm_internals.h"
#include "libgsmdsms.h"
#include "libgsmdevent.h"


extern struct lgsm_handle *lh_sms_other;
extern struct lgsm_handle *lgsmh_voicecall;
extern struct lgsm_handle *lgsmh_gprs;

extern tMSG_QUEUE *to_appctrl_msg_que;
extern tMSG_QUEUE *gsmcdma_to_voicecall_msg_que;


#endif