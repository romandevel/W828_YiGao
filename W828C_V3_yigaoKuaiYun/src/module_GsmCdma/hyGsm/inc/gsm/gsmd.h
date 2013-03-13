#ifndef _GSMD_H
#define _GSMD_H


#include "types.h"
#include "llist.h"

//#include "machineplugin.h"
//#include "vendorplugin.h"
#include "gsmd_select.h"
#include "state.h"
#include "strl.h"

#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwGpio.h"
#include "hyhwUart.h"
#include "hyhwIntCtrl.h"


#include "socket_api.h"

#include "Msgqueue.h"
#include "Mailbox.h"


//#ifndef __GSMD__
//#define __GSMD__


//#define _GSMD_DEBUG_LOG
#ifdef _GSMD_DEBUG_LOG
	#define _GSMD_DEBUG_LOG_1
	//#define _GSMD_DEBUG_LOG_2
#endif


#define MUX_3GPP_TS_0710




#define GSMCDMA_MODULE

//#define GSM_MODULE_SIM300
//#define GSM_MODULE_IW328
//#define GSM_MODULE_MD231
#define GSM_MODULE_ULC2


/* Refer to 3GPP TS 07.07 v 7.8.0, Chapter 4.1 */
#define LGSM_ATCMD_F_EXTENDED	0x01	/* as opposed to basic */
#define LGSM_ATCMD_F_PARAM		0x02	/* as opposed to action */
#define LGSM_ATCMD_F_LFCR		0x04	/* accept LFCR as a line terminator */


struct gsmd;


struct gsmd_timer {
	struct llist_head list;
	struct timeval expires;
	void (*cb)(struct gsmd_timer *tmr, void *data);
	void *data;
};


typedef struct gsmd_timer * (create_timer_t)(struct gsmd *data);



struct gsmd_atcmd {
	struct llist_head list;
	void *ctx;
	int (*cb)(struct gsmd_atcmd *cmd, void *ctx, char *resp);
	char *resp;
	int32_t ret;
	u_int32_t buflen;
	u_int16_t id;
	u_int8_t flags;
    struct gsmd_timer *timeout;
	create_timer_t * create_timer_func;
	char *cur;
	char buf[];
};


//GPRS data
struct gsmd_gframe {
	struct llist_head list;
	void *ctx;
	u_int32_t buflen;
	u_int8_t buf[];
};



enum llparse_state {
	LLPARSE_STATE_IDLE,			/* idle, not parsing a response */
	LLPARSE_STATE_IDLE_CR,		/* CR('\r',0x0D) before response (V1) */
	LLPARSE_STATE_IDLE_LF,		/* LF('\n',0x0A) before response (V1) */
	LLPARSE_STATE_RESULT,		/* within result payload */
	LLPARSE_STATE_RESULT_CR,	/* CR after result */
	LLPARSE_STATE_RESULT_LF,	/* LF after result */
	LLPARSE_STATE_PROMPT,		/* within a "> " prompt */
	LLPARSE_STATE_PROMPT_SPC,	/* a complete "> " prompt */
	
	LLPARSE_STATE_GPRS_DATA_H,	/* ~ before GPRS data */
	LLPARSE_STATE_GPRS_DATA,	/* within GPRS data */
	LLPARSE_STATE_GPRS_DATA_E,	/* ~ after GPRS data */

	LLPARSE_STATE_ERROR,		/* something went wrong */
								/* ... idle again */
};


/* we can't take any _single_ response bigger than this: */
#define LLPARSE_BUF_SIZE	1600	//1024

/* we can't pare a mutiline response biger than this: */
#define MLPARSE_BUF_SIZE	2048	//65535

struct llparser {
	enum llparse_state state;
	unsigned int len;
	unsigned int flags;
	void *ctx;
	int (*cb)(const char *buf, int len, void *ctx);
	int (*prompt_cb)(void *ctx);
	char *cur;
	char buf[LLPARSE_BUF_SIZE];
};

//struct gsmd;

#define GSMD_FLAG_V0			0x0001	/* V0 responses to be expected from TA */
#define GSMD_FLAG_SMS_FMT_TEXT	0x0002	/* TODO Use TEXT rather than PDU mode */
#define GSMD_ATCMD_TIMEOUT_1	10		/* If doesn get respond within 2 secs, discard */
#define GSMD_ATCMD_TIMEOUT_2	20		/* If doesn get respond within 20 secs, discard */



struct gsmd_fd{
	struct llist_head list;
	int fd;				/* file descriptor */
	unsigned int when;
	int (*cb)(int fd, unsigned int what, void *data);
	void *data;			/* void * to pass to callback */
};



struct gsmd{
	unsigned int flags;
	int interpreter_ready;				/* -1:reset   0:not ready   1:ready */
	int write_multi_atcmd;				/* 0:none	1:writing multiline atcmd */
	struct gsmd_fd gfd_uart;
	struct gsmd_fd gfd_sock;
	tMSG_QUEUE *user_reqmsg_que;
	struct llparser llp;
	struct llist_head users;
	struct llist_head pending_atcmds;	/* our busy gsmd_atcmds */
	struct llist_head busy_atcmds;		/* our busy gsmd_atcmds */
	
	struct llist_head pending_gframes;	/* our gprs frame to send */
	unsigned int muxchl_fctl_flag;		/* mux channel flow cntrol flag:bit0-chl0 bit1-chl1 bit2:chl2 */
	unsigned int muxchl_recv_chl;		/* the channel which we have received data from*/

	//struct gsmd_machine_plugin *machinepl;
	//struct gsmd_vendor_plugin *vendorpl;
	unsigned int net_gprs;					/* gprs net state:0-DISCONNECT  1-CONNECT */
	struct gsmd_device_state dev_state;

	struct llist_head operators;		/* cached list of operator names */
	unsigned char *mlbuf;				/* ml_parse buffer */
	unsigned int mlbuf_len;
	int mlunsolicited;
	int alive_responded;
	char imsi[16];						/* imsi mem space */
};




struct gsmd_user {
	struct llist_head list;				/* our entry in the global list */
	struct llist_head finished_ucmds;	/* our busy gsmd_ucmds */
	struct gsmd *gsmd;
	struct gsmd_fd gfd;					/* the socket */
	u_int32_t subscriptions;			/* bitmaks of subscribed event groups */
};




#define GSMD_DEBUG	1	/* debugging information */
#define GSMD_INFO	3
#define GSMD_NOTICE	5	/* abnormal/unexpected condition */
#define GSMD_ERROR	7	/* error condition, requires user action */
#define GSMD_FATAL	8	/* fatal, program aborted */



#if 1

#define gsmd_log(...)

#define DEBUGP(...)


#else

extern int gsmdlog_init(const char *path);
/* write a message to the daemons' logfile */
void __gsmd_log(int level, const char *file, int line, const char *function, const char *message, ...)
	__attribute__ ((__format__ (__printf__, 5, 6)));
/* macro for logging including filename and line number */
#define gsmd_log(level, format, args ...) \
	__gsmd_log(level, __FILE__, __LINE__, __FUNCTION__, format, ## args)

#define DEBUGP(x, args ...)	gsmd_log(GSMD_DEBUG, x, ## args)

#endif



extern int gsmd_simplecmd(struct gsmd *gsmd, char *cmdtxt);
extern int gsmd_initsettings(struct gsmd *gsmd);
extern int gsmd_alive_start(struct gsmd *gsmd);



/***********************************************************************
 * timer handling
 ***********************************************************************/
//int gsmd_timer_init(void);
void gsmd_timer_check_n_run(void);
struct gsmd_timer *gsmd_timer_alloc(void);
int gsmd_timer_register(struct gsmd_timer *timer);
void gsmd_timer_unregister(struct gsmd_timer *timer);

struct gsmd_timer *gsmd_timer_create(struct timeval *expires,
				     void (*cb)(struct gsmd_timer *tmr, void *data), void *data);
#define gsmd_timer_free(x) FREE(x)

//#endif /* __GSMD__ */

#endif /* _GSMD_H */

