#ifndef _GSMD_EVENT_H
#define _GSMD_EVENT_H

enum gsmd_events {
	GSMD_EVT_NONE		= 0,
	GSMD_EVT_IN_CALL	= 1,	/* Incoming call */
	GSMD_EVT_IN_SMS		= 2,	/* Incoming SMS */
	GSMD_EVT_IN_GPRS	= 3,	/* Network initiated GPRS */
	GSMD_EVT_IN_CLIP	= 4,	/* Incoming CLIP */
	GSMD_EVT_NETREG		= 5,	/* Network (un)registration event */
	GSMD_EVT_SIGNAL		= 6,	/* Signal quality event */
	GSMD_EVT_PIN		= 7, 	/* Modem is waiting for some PIN/PUK */
	GSMD_EVT_OUT_STATUS	= 8,	/* Outgoing call status */
	GSMD_EVT_OUT_COLP	= 9,	/* Outgoing COLP */
	GSMD_EVT_CALL_WAIT	= 10,	/* Call Waiting */
	GSMD_EVT_TIMEZONE	= 11,	/* Timezone change */
	GSMD_EVT_SUBSCRIPTIONS	= 12,	/* To which events are we subscribed to */
	GSMD_EVT_CIPHER		= 13,	/* Ciphering Information */
	GSMD_EVT_IN_CBM		= 14,	/* Incoming Cell Broadcast message */
	GSMD_EVT_IN_DS		= 15,	/* SMS Status Report */
	GSMD_EVT_CALL_STAT	= 16,	/* CLCC: Call Status */
	GSMD_EVT_SIM_INIT	= 17,	/* ZeHan: +CSIM: PBK Ready,+CSIM: SMS Ready */
	GSMD_EVT_MODEM_RESET= 18,	/* Modem is reseted */
	GSMD_EVT_IN_ERROR	= 19,	/* CME/CMS error */
	__NUM_GSMD_EVT
};

/* Chapter 8.3 */
enum gsmd_pin_type {			/* waiting for ... */
	GSMD_PIN_READY		= 0,	/* not pending for any password */
	GSMD_PIN_SIM_PIN	= 1,	/* SIM PIN */
	GSMD_PIN_SIM_PUK	= 2,	/* SIM PUK */
	GSMD_PIN_PH_SIM_PIN	= 3,	/* phone-to-SIM passowrd */
	GSMD_PIN_PH_FSIM_PIN	= 4,	/* phone-to-very-first SIM password */
	GSMD_PIN_PH_FSIM_PUK	= 5,	/* phone-to-very-first SIM PUK password */
	GSMD_PIN_SIM_PIN2	= 6,	/* SIM PIN2 */
	GSMD_PIN_SIM_PUK2	= 7,	/* SIM PUK2 */
	GSMD_PIN_PH_NET_PIN	= 8,	/* netwokr personalisation password */
	GSMD_PIN_PH_NET_PUK	= 9,	/* network personalisation PUK */
	GSMD_PIN_PH_NETSUB_PIN	= 10, 	/* network subset personalisation PIN */
	GSMD_PIN_PH_NETSUB_PUK	= 11,	/* network subset personalisation PUK */
	GSMD_PIN_PH_SP_PIN	= 12,	/* service provider personalisation PIN */
	GSMD_PIN_PH_SP_PUK	= 13,	/* service provider personalisation PUK */
	GSMD_PIN_PH_CORP_PIN	= 14,	/* corporate personalisation PIN */
	GSMD_PIN_PH_CORP_PUK	= 15,	/* corporate personalisation PUK */
	__NUM_GSMD_PIN
};


enum gsmd_facility_type {
	GSMD_FAC_SC	= 0,	/* SIM (lock SIM card) */
	GSMD_FAC_AO	= 1,	/* 禁止所有出局呼叫 */
	GSMD_FAC_OI	= 2,	/* 禁止所有国际出局呼叫 */
	GSMD_FAC_OX	= 3,	/* 禁止所有国际出局呼叫，归属国除外 */
	GSMD_FAC_AI	= 4,	/* 禁止所有入局呼叫 */
	GSMD_FAC_IR	= 5,	/* 归属国以外漫游时，禁止所有入局呼叫 */
	GSMD_FAC_AB	= 6,	/* All Barring services (refer GSM02.30[19]) */
	GSMD_FAC_AG	= 7,	/* All out Going barring services (refer GSM02.30[19]) */
	GSMD_FAC_AC	= 8,	/* All in Coming barring services (refer GSM02.30[19]) */
	GSMD_FAC_PN	= 9,	/* 网络个性化（请参考GSM 02.22 [33]） */
	GSMD_FAC_PU	= 10, 	/* 网络子集个性化（请参考GSM 02.22 [33]） */
	GSMD_FAC_PP	= 11,	/* 服务供应商个性化（请参考GSM 02.22 [33]） */
	GSMD_FAC_PC	= 12,	/* 公司个性化（请参考GSM 02.22 [33]） */
	GSMD_FAC_P2	= 13,	/* SIM PIN2 */
	__NUM_GSMD_FAC
};




enum gsmd_call_type {
	GSMD_CALL_NONE		= 0,
	GSMD_CALL_UNSPEC	= 1,
	GSMD_CALL_VOICE		= 2,
	GSMD_CALL_FAX		= 4,
	GSMD_CALL_DATA_SYNC	= 5,
	GSMD_CALL_DATA_REL_ASYNC= 6,
	GSMD_CALL_DATA_REL_SYNC	= 7,
    GSMD_CALL_TIMEOUT       = 8,
	__NUM_GSMD_CALL
};

/* Chapter 7.2 */
enum gsmd_netreg_state {
	GSMD_NETREG_UNREG	= 0,
	GSMD_NETREG_REG_HOME,
	GSMD_NETREG_UNREG_BUSY,
	GSMD_NETREG_DENIED,
	GSMD_NETREG_UNKNOWN,
	GSMD_NETREG_REG_ROAMING,
	__NUM_GSMD_NETREG
};

enum gsmd_call_progress {
	GSMD_CALLPROG_SETUP		= 0,
	GSMD_CALLPROG_DISCONNECT	= 1,
	GSMD_CALLPROG_ALERT		= 2,
	GSMD_CALLPROG_CALL_PROCEED	= 3,
	GSMD_CALLPROG_SYNC		= 4,
	GSMD_CALLPROG_PROGRESS		= 5,
	GSMD_CALLPROG_CONNECTED		= 6,
	GSMD_CALLPROG_RELEASE		= 7,
	GSMD_CALLPROG_REJECT		= 8,
	GSMD_CALLPROG_UNKNOWN		= 9,
	__NUM_GSMD_CALLPROG
};



enum gsmd_call_direction {
	GSMD_CALL_DIR_MO		= 0,	/* Mobile Originated (Outgoing) */
	GSMD_CALL_DIR_MT		= 1,	/* Mobile Terminated (Incoming) */
	GSMD_CALL_DIR_CCBS		= 2,	/* network initiated MO */
	GSMD_CALL_DIR_MO_REDIAL	= 3	/* Mobile Originated Redial */
};


/*
0 Active call terminated, at least one held call remaining
1 Attempt to make an Mobile Originated call
2 Mobile Originated Call has failed for some reason
3 Mobile Originated call is ringing
4 Mobile Terminated call is queued (Call waiting)
5 Mobile Originated Call now connected
6 Mobile Originated or Mobile Terminated call has disconnected
7 Mobile Originated or Mobile Terminated call hung up
8 Mobile Originated call to non-emergency number in emergency mode
9 Mobile Originated call no answer
10 Mobile Originated call remote number busy
*/

/*  SIMCOM Module: call state transitions */
enum gsmd_call_stattran {
	GSMD_CALLTRAN_TERMINATED		= 0,
	GSMD_CALLTRAN_MO_GOING			= 1,
	GSMD_CALLTRAN_MO_FAILED			= 2,
	GSMD_CALLTRAN_MO_RING			= 3,
	GSMD_CALLTRAN_MT_QUEUED			= 4,
	GSMD_CALLTRAN_MOMT_CONNECTED	= 5,
	GSMD_CALLTRAN_MOMT_DISCONNECTED	= 6,
	GSMD_CALLTRAN_MOMT_HANGUP		= 7,
	GSMD_CALLTRAN_MO_EMERGENCY		= 8,
	GSMD_CALLTRAN_MO_NOANSWER		= 9,
	GSMD_CALLTRAN_MO_BUSY			= 10,
	__NUM_GSMD_CALLTRAN
};


#endif
