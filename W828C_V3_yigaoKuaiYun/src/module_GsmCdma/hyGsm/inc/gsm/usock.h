#ifndef _GSMD_USOCK_H
#define _GSMD_USOCK_H

#include "types.h"

#include "gsmdevent.h"
#include "ts0707.h"
#include "ts0705.h"


#define GSMD_IPPORT 			5000
#define GSMD_IPADDR 			"127.0.0.1"

#define GSMD_SOCKET_TYPE		SOCK_STREAM
#define GSMD_PROTO_VERSION		1
#define GSMD_MSGSIZE_MAX		4096


enum gsmd_msg_type {
	GSMD_MSG_NONE		= 0,
	GSMD_MSG_EVENT		= 1,
	GSMD_MSG_PASSTHROUGH= 2,
	GSMD_MSG_VOICECALL	= 3,
	GSMD_MSG_DATACALL	= 4,
	GSMD_MSG_PHONEBOOK	= 5,
	GSMD_MSG_NETWORK	= 6,
	GSMD_MSG_PHONE		= 7,
	GSMD_MSG_SECURITY	= 8,
	GSMD_MSG_SMS		= 9,
	GSMD_MSG_CB			= 10,
	GSMD_MSG_MODEM		= 11,
	__NUM_GSMD_MSGS
};

enum gsmd_passthrough_type {
	GSMD_PASSTHROUGH_NONE	= 0,
	GSMD_PASSTHROUGH_REQ	= 1,
	GSMD_PASSTHROUGH_RESP	= 2
};

enum gsmd_msg_voicecall_type {
	GSMD_VOICECALL_DIAL		= 1,
	GSMD_VOICECALL_HANGUP	= 2,
	GSMD_VOICECALL_ANSWER	= 3,
	GSMD_VOICECALL_DTMF		= 4,
	GSMD_VOICECALL_VOL_SET	= 5,
	GSMD_VOICECALL_VOL_GET	= 6,
	GSMD_VOICECALL_GET_STAT	= 7,
	GSMD_VOICECALL_CTRL		= 8,
	GSMD_VOICECALL_FWD_DIS	= 9,
	GSMD_VOICECALL_FWD_EN	= 10,
	GSMD_VOICECALL_FWD_STAT	= 11,
	GSMD_VOICECALL_FWD_REG	= 12,
	GSMD_VOICECALL_FWD_ERAS	= 13,
	
	// AT+CHFA Swap The Audio Channels
	GSMD_VOICECALL_AUDIOCHL_SET	= 14,
	// AT+CMIC Change The Microphone Gain Level
	GSMD_VOICECALL_MICGAIN_SET = 15,
	// AT+CMUT Enable/Disable the uplink voice muting during a voice call.
	GSMD_VOICECALL_MUTE_SET = 16,

	GSMD_VOICECALL_COLP_SET	= 17,
	GSMD_VOICECALL_CLIP_SET	= 18
};


enum gsmd_msg_datacall_type {

	GSMD_DATACALL_DIAL		= 1,
	GSMD_DATACALL_HANGUP	= 2,
	GSMD_DATACALL_TRANSFER	= 3,
	GSMD_DATACALL_CGDCONT   = 4
	
};



/*  call direction from 3GPP TS 07.07, Clause 7.17 */
enum gsmd_call_dire {
	GSMD_CALL_DIRE_MO	= 0,
	GSMD_CALL_DIRE_MT	= 1
};


/*
0 active
1 held
2 dialing (MO call)
3 alerting (MO call)
4 incoming (MT call)
5 waiting (MT call)
*/

/*  call state from 3GPP TS 07.07, Clause 7.17 */
enum gsmd_call_stat {
	GSMD_CALL_STAT_ACTIVE	= 0,
	GSMD_CALL_STAT_HELD		= 1,
	GSMD_CALL_STAT_DIALING	= 2,
	GSMD_CALL_STAT_ALERTING	= 3,
	GSMD_CALL_STAT_INCOMING	= 4,
	GSMD_CALL_STAT_WAITING	= 5,
	GSMD_CALL_STAT_NONE		= 6,
	GSMD_CALL_STAT_ERROR	= 7,
	GSMD_CALL_STAT_UNKNOWN	= 8
};

/*  call mode from 3GPP TS 07.07, Clause 7.17 */
enum gsmd_call_mode {
	GSMD_CALL_MODE_VOICE			= 0,
	GSMD_CALL_MODE_DATA				= 1,
	GSMD_CALL_MODE_FAX				= 2,
	GSMD_CALL_MODE_VOICE_DATA		= 3,
	GSMD_CALL_MODE_VOICE_DATA_ALT	= 4,
	GSMD_CALL_MODE_VOICE_FAX_ALT	= 5,	
	GSMD_CALL_MODE_DATA_VOICE		= 6,
	GSMD_CALL_MODE_DATA_VOICE_ALT	= 7,
	GSMD_CALL_MODE_FAX_VOICE_ALT	= 8,
	GSMD_CALL_MODE_UNKNOWN			= 9
};

/*  multiparty(conference) from 3GPP TS 07.07, Clause 7.17 */
enum gsmd_call_mpty {
	GSMD_CALL_MPTY_NO	= 0,
	GSMD_CALL_MPTY_YES	= 1
};

/* 
 * call related supplementary services from 3GPP TS 02.30 4.5.5.1 
 * R - Release
 * A - Accept
 * H - Hold
 * M - Multiparty
 */
enum gsmd_call_ctrl_proc {
	GSMD_CALL_CTRL_R_HLDS				= 0,	// 0
	GSMD_CALL_CTRL_UDUB					= 1,	// 0
	GSMD_CALL_CTRL_R_ACTS_A_HLD_WAIT	= 2,	// 1	
	GSMD_CALL_CTRL_R_ACT_X				= 3,	// 1x
	GSMD_CALL_CTRL_H_ACTS_A_HLD_WAIT	= 4,	// 2
	GSMD_CALL_CTRL_H_ACTS_EXCEPT_X		= 5,	// 2x
	GSMD_CALL_CTRL_M_HELD				= 6	// 3
};

/* call forward reason from 3GPP TS 07.07 subclause 07.10 */
enum gsmd_call_fwd_reason {
	GSMD_CALL_FWD_REASON_UNCOND				= 0,
	GSMD_CALL_FWD_REASON_BUSY				= 1, 
	GSMD_CALL_FWD_REASON_NO_REPLY			= 2,
	GSMD_CALL_FWD_REASON_NOT_REACHABLE		= 3,
	GSMD_CALL_FWD_REASON_ALL_FORWARD		= 4,
	GSMD_CALL_FWD_REASON_ALL_COND_FORWARD	= 5 
};

/* call forward status from 3GPP TS 07.07 subclause 07.10 */
enum gsmd_call_fwd_status {
	GSMD_CALL_FWD_STATUS_NOT_ACTIVE	= 0,
	GSMD_CALL_FWD_STATUS_ACTIVE		= 1
};

/* Handset / MT related commands */
enum gsmd_msg_phone_type {
	GSMD_PHONE_VOLUME	= 1,
	GSMD_PHONE_VIBRATOR	= 2
};

enum gsmd_msg_security {
	GSMD_SECURITY_PIN_GET_STATUS    = 1,
	GSMD_SECURITY_PIN_INPUT			= 2,
	GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES	= 3,
	GSMD_SECURITY_LOCK_EN			= 4,
	GSMD_SECURITY_LOCK_DIS 			= 5,
	GSMD_SECURITY_LOCK_GET_STATUS 	= 6,
	GSMD_SECURITY_CHANGE_PWD 		= 7
};

enum gsmd_msg_phone {
	GSMD_PHONE_POWER		= 1,
	GSMD_PHONE_GET_IMSI     = 2,
	GSMD_PHONE_GET_MANUF	= 3,
	GSMD_PHONE_GET_MODEL	= 4,
	GSMD_PHONE_GET_REVISION	= 5,
	GSMD_PHONE_GET_SERIAL	= 6,
	GSMD_PHONE_GET_BATTERY  = 7,
	GSMD_PHONE_VIB_ENABLE   = 8,
	GSMD_PHONE_VIB_DISABLE  = 9,
	GSMD_PHONE_POWER_STATUS = 10,
	GSMD_PHONE_DETECT_SIMCARD	= 11
};


enum gsmd_msg_modem {
	GSMD_MODEM_POWEROFF	= 1,
	GSMD_MODEM_POWERON		= 2,
	GSMD_OPEN_MUX_MODE		,
	GSMD_OPEN_DLC0_MODE		,
	GSMD_OPEN_DLC1_PN		,
	GSMD_OPEN_DLC1_MODE		,
	GSMD_OPEN_DLC2_PN		,
	GSMD_OPEN_DLC2_MODE		
};

enum gsmd_msg_cb {
	GSMD_CB_SUBSCRIBE	= 1,
	GSMD_CB_UNSUBSCRIBE	= 2
};

enum gsmd_msg_network {
	GSMD_NETWORK_REGISTER	= 1,
	GSMD_NETWORK_SIGQ_GET	= 2,
	GSMD_NETWORK_VMAIL_GET	= 3,
	GSMD_NETWORK_VMAIL_SET	= 4,
	GSMD_NETWORK_OPER_GET	= 5,
	GSMD_NETWORK_OPER_N_GET	= 6,
	GSMD_NETWORK_OPER_LIST	= 7,
	GSMD_NETWORK_CIND_GET	= 8,
	GSMD_NETWORK_DEREGISTER	= 9,
	GSMD_NETWORK_GET_NUMBER = 10,
	GSMD_NETWORK_PREF_LIST  = 11,
	GSMD_NETWORK_PREF_DEL   = 12,
	GSMD_NETWORK_PREF_ADD   = 13,
	GSMD_NETWORK_PREF_SPACE = 14,
	GSMD_NETWORK_QUERY_REG  = 15,
	GSMD_NETWORK_GET_TIMESTAMP = 16,
	GSMD_NETWORK_SET_NUMBER = 17,
	GSMD_NETWORK_RE_REGISTER	= 18
};

enum gsmd_msg_sms {
	GSMD_SMS_LIST						= 1,
	GSMD_SMS_READ						= 2,
	GSMD_SMS_SEND						= 3,
	GSMD_SMS_WRITE						= 4,
	GSMD_SMS_DELETE						= 5,
	GSMD_SMS_DELETE_ALL					= 6,
	GSMD_SMS_GET_MSG_STORAGE			= 7,
	GSMD_SMS_SET_MSG_STORAGE			= 8,
	GSMD_SMS_GET_SERVICE_CENTRE			= 9,
	GSMD_SMS_SET_SERVICE_CENTRE			= 10,
	GSMD_SMS_GET_NEW_MSG_INDICATIONS	= 11,	//CNMI
	GSMD_SMS_SET_NEW_MSG_INDICATIONS	= 12,
	GSMD_SMS_SET_FMT							//PDU/TEXT
};

/* SMS stat from 3GPP TS 07.05, Clause 3.1 */
enum gsmd_msg_sms_type {
	GSMD_SMS_REC_UNREAD	= 0,
	GSMD_SMS_REC_READ	= 1,
	GSMD_SMS_STO_UNSENT	= 2,
	GSMD_SMS_STO_SENT	= 3,
	GSMD_SMS_ALL		= 4
};

/* SMS format from 3GPP TS 07.05, Clause 3.2.3 */
enum gsmd_msg_sms_fmt {
	GSMD_SMS_FMT_PDU	= 0,
	GSMD_SMS_FMT_TEXT	= 1
};

/* Data Coding Scheme, refer to GSM 03.38 Clause 4 */
#define B5_COMPRESSED	(1<<5)
#define B4_CLASSMEANING	(1<<4)

enum {
	MESSAGE_CLASS_CLASS0		= 0x00,
	MESSAGE_CLASS_CLASS1		= 0x01,
	MESSAGE_CLASS_CLASS2		= 0x02,
	MESSAGE_CLASS_CLASS3		= 0x03
};

enum gsmd_sms_alphabet {
	ALPHABET_DEFAULT		= (0x00<<2),
	ALPHABET_8BIT			= (0x01<<2),
	ALPHABET_UCS2			= (0x02<<2),
	ALPHABET_RESERVED		= (0x03<<2)
};

/* bit 1 & bit0 */
enum gsmd_sms_msg_class {
	MSG_CLASS_CLASS0	= 0,
	MSG_CLASS_CLASS1	= 1,
	MSG_CLASS_CLASS2	= 2,
	MSG_CLASS_CLASS3	= 3,
	MSG_CLASS_NONE		= 4
};

/* bit 3 & bit 2*/
enum gsmd_sms_alphabet_type {
	SMS_ALPHABET_7_BIT_DEFAULT 	= 0,
	SMS_ALPHABET_8_BIT			= 1,
	SMS_ALPHABET_UCS2			= 2,
	SMS_ALPHABET_RESESRVED		= 3
};

enum gsmd_sms_msg_compressed {
	NOT_COMPRESSED	= 0,
	COMPRESSED		= 1
};

/* message waiting indication */
enum gsmd_sms_msg_waiting_group {
	MESSAGE_WAITING_NONE		= 0,
	MESSAGE_WAITING_DISCARD		= 1,
	MESSAGE_WAITING_STORE		= 2,
	MESSAGE_WAITING_NONE_1111	= 3
};

enum gsmd_sms_msg_waiting_type {
	MESSAGE_WAITING_VOICEMAIL	= 0,
	MESSAGE_WAITING_FAX 		= 1,
	MESSAGE_WAITING_EMAIL 		= 2,
	MESSAGE_WAITING_OTHER 		= 3
};

enum gsmd_sms_msg_waiting_active {
	NOT_ACTIVE	= 0,
	ACTIVE		= 1
};




struct gsmd_sms_datacodingscheme {
	enum gsmd_sms_msg_class		        msg_class;
	enum gsmd_sms_alphabet_type 		alphabet;
	enum gsmd_sms_msg_compressed        is_compressed;
	enum gsmd_sms_msg_waiting_group 	mwi_group;
	enum gsmd_sms_msg_waiting_active    mwi_active;
	enum gsmd_sms_msg_waiting_type    	mwi_kind;
	u_int8_t                            raw_dcs_data;
};



/* Refer to GSM 03.40 subclause 9.2.3.1 */
enum gsmd_sms_tp_mti {
	GSMD_SMS_TP_MTI_DELIVER		= 0,
	GSMD_SMS_TP_MTI_DELIVER_REPORT	= 0,
	GSMD_SMS_TP_MTI_STATUS_REPORT	= 2,
	GSMD_SMS_TP_MTI_COMMAND		= 2,
	GSMD_SMS_TP_MTI_SUBMIT		= 1,
	GSMD_SMS_TP_MTI_SUBMIT_REPORT	= 1,
	GSMD_SMS_TP_MTI_RESERVED	= 3
};

/* Refer to GSM 03.40 subclause 9.2.3.2, */
/* for SMS-DELIVER, SMS-STATUS-REPORT */
enum gsmd_sms_tp_mms {
	GSMD_SMS_TP_MMS_MORE		= (0<<2),
	GSMD_SMS_TP_MMS_NO_MORE		= (1<<2)
};

/* Refer to GSM 03.40 subclause 9.2.3.3, */
/* for SMS-SUBMIT */
enum gsmd_sms_tp_vpf {
	GSMD_SMS_TP_VPF_NOT_PRESENT	= (0<<3),
	GSMD_SMS_TP_VPF_RESERVED	= (1<<3),
	GSMD_SMS_TP_VPF_RELATIVE	= (2<<3),	
	GSMD_SMS_TP_VPF_ABSOLUTE	= (3<<3)
};

/* Refer to GSM 03.40 subclause 9.2.3.4, */
/* for SMS-DELIVER */
enum gsmd_sms_tp_sri {
	GSMD_SMS_TP_SRI_NOT_RETURN		= (0<<5),
	GSMD_SMS_TP_SRI_STATUS_RETURN	= (1<<5)
};

/* Refer to GSM 03.40 subclause 9.2.3.5, */
/* for SMS-SUBMIT, SMS-COMMAND */
/*状态报告要求:TP-SPR（TP-Status-Report-Request）*/
enum gsmd_sms_tp_srr {
	GSMD_SMS_TP_SRR_NOT_REQUEST		= (0<<5),
	GSMD_SMS_TP_SRR_STATUS_REQUEST	= (1<<5)
};

/* Refer to GSM 03.40 subclause 9.2.3.17, */
/* for SMS-SUBMIT, SMS-DELIVER */
enum gsmd_sms_tp_rp {
	GSMD_SMS_TP_RP_NOT_SET		= (0<<7),
	GSMD_SMS_TP_RP_SET			= (1<<7)
};

/* Refer to GSM 03.40 subclause 9.2.3.23 */
/* for SMS-SUBMIT, SMS-DELIVER */
enum gsmd_sms_tp_udhi {
	GSMD_SMS_TP_UDHI_NO_HEADER		= (0<<6),
	GSMD_SMS_TP_UDHI_WITH_HEADER	= (1<<6)
};

/* SMS delflg from 3GPP TS 07.05, Clause 3.5.4 */
enum gsmd_msg_sms_delflg {
	GSMD_SMS_DELFLG_INDEX			= 0,
	GSMD_SMS_DELFLG_READ			= 1,
	GSMD_SMS_DELFLG_READ_SENT		= 2,
	GSMD_SMS_DELFLG_LEAVING_UNREAD	= 3,
	GSMD_SMS_DELFLG_ALL				= 4
};

enum gsmd_msg_phonebook {
	GSMD_PHONEBOOK_FIND			= 1,
	GSMD_PHONEBOOK_READ			= 2,
	GSMD_PHONEBOOK_READRG		= 3,
	GSMD_PHONEBOOK_WRITE		= 4,
	GSMD_PHONEBOOK_DELETE		= 5,	
	GSMD_PHONEBOOK_GET_SUPPORT	= 6,
	GSMD_PHONEBOOK_LIST_STORAGE	= 7,
	GSMD_PHONEBOOK_SET_STORAGE	= 8,
	GSMD_PHONEBOOK_GET_STORAGE	= 9
};

/* Type-of-Address, Numbering-Plan-Identification field, GSM 03.40, 9.1.2.5 */
enum gsmd_toa_npi {
	GSMD_TOA_NPI_UNKNOWN		= 0x0,
	GSMD_TOA_NPI_ISDN			= 0x1,
	GSMD_TOA_NPI_DATA			= 0x3,
	GSMD_TOA_NPI_TELEX			= 0x4,
	GSMD_TOA_NPI_NATIONAL		= 0x8,
	GSMD_TOA_NPI_PRIVATE		= 0x9,
	GSMD_TOA_NPI_ERMES			= 0xa,
	GSMD_TOA_NPI_RESERVED		= 0xf
};

/* Type-of-Address, Type-of-Number field, GSM 03.40, Subclause 9.1.2.5 */
enum gsmd_toa_ton {
	GSMD_TOA_TON_UNKNOWN		= (0<<4),
	GSMD_TOA_TON_INTERNATIONAL	= (1<<4),
	GSMD_TOA_TON_NATIONAL		= (2<<4),
	GSMD_TOA_TON_NETWORK		= (3<<4),
	GSMD_TOA_TON_SUBSCRIBER		= (4<<4),
	GSMD_TOA_TON_ALPHANUMERIC	= (5<<4),
	GSMD_TOA_TON_ABBREVIATED	= (6<<4),
	__GSMD_TOA_TON_MASK			= (7<<4)
};

/* Type-of-Address, bit 7 always 1 */
enum gsmd_toa_reserved {
	GSMD_TOA_RESERVED		= (1<<7)
};

/* Length from 3GPP TS 04.08, Clause 10.5.4.7 */

/* battery connection status from 3GPP TS 07.07 subclause 8.4 */
enum gsmd_bat_conn_status {
	GSMD_BAT_NORMAL			= 0,
	GSMD_BAT_NOT_POWERED	= 1,
	GSMD_BAT_NO_BAT			= 2,
	GSMD_BAT_POWER_FAULT	= 3
};


#define GSMD_ADDR_MAXLEN	32


struct gsmd_addr {
	u_int8_t type;
	char number[GSMD_ADDR_MAXLEN+1];
};


//GPRS
struct gsmd_data_connect_set {
	char addr[GSMD_ADDR_MAXLEN+1];
	char l2p[10];
	u_int8_t cid;
};


struct gsmd_dtmf {
	u_int8_t len;
	char dtmf[0];
};

struct gsmd_microphone_gain {
	u_int8_t channel;
	u_int8_t gainlevel;
};

struct gsmd_signal_quality {
	u_int8_t rssi;
	u_int8_t ber;
};

struct gsmd_voicemail {
	u_int8_t enable;
	struct gsmd_addr addr;
};

struct gsmd_battery_charge {
	enum gsmd_bat_conn_status bcs;
	u_int8_t bcl;
};

#define GSMD_ALPHA_MAXLEN	20

/* call status from 3GPP TS 07.07 clause 07.17 */
struct gsmd_call_status {
	int8_t idx;
	u_int8_t dir;
	u_int8_t stat;
	u_int8_t mode;
	u_int8_t mpty;
	char number[GSMD_ADDR_MAXLEN+1];	
	u_int8_t type;
	char alpha[GSMD_ALPHA_MAXLEN+1];
	int is_last;
};

/* call status from 3GPP TS 07.07 clause 7.12 */
struct gsmd_call_ctrl {
	enum gsmd_call_ctrl_proc proc;	
	u_int8_t idx;
};

/* call forwarding register from 3GPP TS 07.07 clause 7.10 */
struct gsmd_call_fwd_reg {
	enum gsmd_call_fwd_reason reason;
	struct gsmd_addr addr;
};

/* status of call forwarding from 3GPP TS 07.07 clause 7.10 */
struct gsmd_call_fwd_stat {
	enum gsmd_call_fwd_status status; 
	u_int8_t classx;
	struct gsmd_addr addr;
	char subaddr[16+1];
	u_int8_t satype;
	u_int8_t time;
	int is_last;
};

#define GSMD_PIN_MAXLEN		8
struct gsmd_pin {
	enum gsmd_pin_type type;
	char pin[GSMD_PIN_MAXLEN+1];
	char newpin[GSMD_PIN_MAXLEN+1];
};


struct gsmd_pin_remain_times {
	int pin;
	int pin2;
	int puk;
	int puk2;
};


struct gsmd_facility {
	enum gsmd_facility_type type;
	char pwd[GSMD_PIN_MAXLEN+1];
	char newpwd[GSMD_PIN_MAXLEN+1];
	int	 status;
};


struct gsmd_evt_auxdata {
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
			char alpha[GSMD_ALPHA_MAXLEN+1];
			int inlined;
			u_int8_t memtype;
			int index;
		} sms;
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
			enum gsmd_netreg_state state;
			u_int16_t lac;
			u_int16_t ci;
		} netreg;
		struct {
			u_int8_t tz;
		} timezone;
		struct {
			struct gsmd_signal_quality sigq;
		} signal;
		struct {
			enum gsmd_call_progress prog;
			struct gsmd_addr addr;
			u_int8_t ibt:1,
				 tch:1,
				 dir:2;
		} call_status;
		struct {
			u_int16_t flags;
			u_int16_t net_state_gsm;
			u_int16_t net_state_gprs;
		} cipher;
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
		} csurc;	//call status urc

		struct {
			u_int8_t phb;
			u_int8_t sms;
		} sim_init;

		struct {
			u_int8_t reset;
		} modem;
	} u;
	u_int8_t data[0];
};




struct _gsmd_sms_list {
	u_int8_t stat;
	u_int8_t mode;
};


struct gsmd_sms_read {
	int32_t index;
	u_int8_t mode;	
};


/* Refer to GSM 03.40 subclause 9.2.2.2 and GSM 07.05 subclause 4.3 */
#define GSMD_SMS_DATA_MAXLEN	164 
struct gsmd_sms {
	u_int8_t length;
	u_int8_t coding_scheme;
	int has_header;
	int is_voicemail;
	struct gsmd_sms_datacodingscheme dcs;
	enum gsmd_sms_tp_mti tp_mti;
	u_int8_t data[GSMD_SMS_DATA_MAXLEN+1];
};



/* Refer to GSM 03.40 subclause 9.2.2.2 */
struct gsmd_sms_submit {
	struct gsmd_addr addr;
	struct gsmd_sms payload;
	int ask_ds;
};



/* Refer to GSM 07.05 subclause 4.4 */
struct gsmd_sms_write {
	u_int8_t stat;
	struct gsmd_sms_submit sms;
};



/* Refer to GSM 03.40 subclause 9.2.2.1 */
struct gsmd_sms_list {
	/* FIXME Defined as in range of location numbers supported by memory */
	int32_t index;
	enum gsmd_msg_sms_type stat;
	char time_stamp[7];	
	struct gsmd_addr addr;
	struct gsmd_sms payload;
	int is_last;
};

/* Refer to GSM 07.05 subclause 3.1 */
enum ts0705_mem_type {
	GSM0705_MEMTYPE_NONE=0,
	GSM0705_MEMTYPE_BROADCAST,
	GSM0705_MEMTYPE_ME_MESSAGE,
	GSM0705_MEMTYPE_MT,
	GSM0705_MEMTYPE_SIM,
	GSM0705_MEMTYPE_TA,
	GSM0705_MEMTYPE_SR,
};



/* Refer to GSM 07.05 subclause 3.2.2 */
struct __gsmd_sms_storage {
	u_int8_t memtype;
	u_int8_t pad[3];
	int32_t used;
	int32_t total;
};

struct gsmd_sms_storage {
	struct __gsmd_sms_storage mem[3];
};


//+CNMI: <mode>,<mt>,<bm>,<ds>,<bfr>
struct gsmd_sms_cnmi {
	u_int8_t mode;
	u_int8_t mt;
	u_int8_t bm;
	u_int8_t ds;
	u_int8_t bfr;
};


/* Refer to GSM 03.41 subclause 9.3.1 - note: this indicates display mode too */
enum gsmd_geographical_scope {
	GSMD_SCOPE_CELL_WIDE_OPER	= 0,
	GSMD_SCOPE_PLMN_WIDE,
	GSMD_SCOPE_LOC_AREA_WIDE,
	GSMD_SCOPE_CELL_WIDE,
};

enum gsmd_language {
	GSMD_LANG_GERMAN	= 0,
	GSMD_LANG_ENGLISH,
	GSMD_LANG_ITALIAN,
	GSMD_LANG_FRENCH,
	GSMD_LANG_SPANISH,
	GSMD_LANG_DUTCH,
	GSMD_LANG_SWEDISH,
	GSMD_LANG_DANISH,
	GSMD_LANG_PORTUGUESE,
	GSMD_LANG_FINNISH,
	GSMD_LANG_NORWEGIAN,
	GSMD_LANG_GREEK,
	GSMD_LANG_TURKISH,
	GSMD_LANG_HUNGARIAN,
	GSMD_LANG_POLISH,
	GSMD_LANG_UNSPECIFIED,
};

/* Refer to GSM 03.41 subclause 9.3 */
struct gsmd_cbm {
	struct {
		enum gsmd_geographical_scope scope;
		int msg_code;
		int update_num;
	} serial;
	u_int16_t msg_id;
	enum gsmd_language language;
	u_int8_t coding_scheme;
	int pages;
	int page;
	u_int8_t data[82];
};



/* Refer to GSM 07.07 subclause 8.12 */
struct gsmd_phonebook_readrg {
	int32_t index1;
	int32_t index2;
};

/* Refer to GSM 07.07 subclause 8.14 */
/* FIXME: the nlength and tlength depend on SIM, use +CPBR=? to get */ 
#define	GSMD_PB_NUMB_MAXLEN	44
#define GSMD_PB_TEXT_MAXLEN	14
struct gsmd_phonebook {
	int32_t index;
	char numb[GSMD_PB_NUMB_MAXLEN+1];
	u_int8_t type;
	char text[GSMD_PB_TEXT_MAXLEN+1];
};

struct gsmd_phonebooks {
	struct gsmd_phonebook pb;
	int8_t is_last;
};

/* Refer to GSM 07.07 subclause 8.13 */
/* FIXME: the tlength depends on SIM, use +CPBR=? to get */ 
struct gsmd_phonebook_find {	
	char findtext[GSMD_PB_TEXT_MAXLEN+1];
};

/* Refer to GSM 07.07 subclause 8.12 */
struct gsmd_phonebook_support {
	int32_t index;
	u_int8_t nlength;
	u_int8_t tlength;
};




/* Operator status from 3GPP TS 07.07, Clause 7.3 */
enum gsmd_oper_status {
	GSMD_OPER_UNKNOWN,
	GSMD_OPER_AVAILABLE,
	GSMD_OPER_CURRENT,
	GSMD_OPER_FORBIDDEN,
};

/* Theoretically numeric operator code is five digits long but some
 * operators apparently use six digit codes.  */
typedef char gsmd_oper_numeric[6];

struct gsmd_msg_oper {
	enum gsmd_oper_status stat;
	int is_last;
	char opname_longalpha[16];
	char opname_shortalpha[8];
	gsmd_oper_numeric opname_num;
};

struct gsmd_msg_prefoper {
	int index;
	int is_last;
	char opname_longalpha[16];
};



/* Refer to GSM 07.07 subclause 8.11 */
struct gsmd_phonebook_mem {
	u_int8_t type[3];
	u_int8_t pad;
	int32_t used;
	int32_t total;
};

struct gsmd_phonebook_storage {
	/* FIXME the amount of phonebook storage should be dynamic */
	u_int8_t num;
	struct gsmd_phonebook_mem mem[20];
};




/* Subscriber number information from 3GPP TS 07.07, Clause 7.1 */
enum gsmd_subscriber_service {
	GSMD_SERVICE_UNKNOWN		= -1,
	GSMD_SERVICE_ASYNC_MODEM	= 0,
	GSMD_SERVICE_SYNC_MODEM,
	GSMD_SERVICE_PAD_ACCESS,
	GSMD_SERVICE_PACKET_ACCESS,
	GSMD_SERVICE_VOICE,
	GSMD_SERVICE_FAX,
};

struct gsmd_own_number {
	int is_last;
	enum gsmd_subscriber_service service;
	struct gsmd_addr addr;
	char name[0];
};



struct gsmd_msg_hdr {
	u_int8_t version;
	u_int8_t msg_type;
	u_int8_t msg_subtype;
	u_int8_t _pad;
	u_int16_t id;
	u_int16_t len;
	u_int8_t data[];
};

struct gsmd_msg_auxdata {
    union {
       struct {
            struct gsmd_signal_quality sigq;
        } signal;
       struct {
            char name[16];
        } current_operator;
        /* add more here please */
    } u;
    u_int8_t data[0];
};




//#ifdef __GSMD__

//#include "usock.h"
#include "gsmd.h"
#include "llist.h"

struct gsmd_user;




struct gsmd_ucmd {
	struct llist_head list;
	struct gsmd_msg_hdr hdr;
	char buf[];
};






#define CONNECT_USER0 		"con-user0"
#define CONNECT_USER1 		"con-user1"
#define CONNECT_USER2 		"con-user2"
#define CONNECT_USER3 		"con-user3"
#define CONNECT_USER4 		"con-user4"

#define DISCONNECT_USER0 	"discon-user0"
#define DISCONNECT_USER1 	"discon-user1"
#define DISCONNECT_USER2 	"discon-user2"
#define DISCONNECT_USER3	"discon-user3"
#define DISCONNECT_USER4	"discon-user4"


#define USER_CLIENT	"user0"
#define USER_VOICE	"user1"
#define USER_SMS_OTHER	"user2"
#define USER_GPRS	"user3"
#define USER_OTHER	"user4"

#define MAILBOX_BUFSIZE_1	2048
#define MAILBOX_BUFSIZE_2	4096


//gmbox[USER_NUM]的用户
typedef enum _gsmdusertype
{
	USER_0=0,	//client
	USER_1,		//voicecall
	USER_2,		//sms/other
	USER_3,		//GPRS
	USER_4,		//other
	USER_NUM	
}gsmdusertype;


#define USER_ID_GPRS	USER_3



typedef enum _gsmduser_req_result
{
	CONNECT=0,
	DISCONNECT,
	RESULT_NUM
}gsmduser_req_result;


typedef struct _mailboxbuf
{
   u_int32_t  r;
   u_int32_t  w;
   u_int32_t  size;
   char		  *buf;	//buf[4096];  //队列的元素空间
}mailboxbuf;


typedef struct _gsmdmailbox{
	int			rcv;
	mailboxbuf	*rcvbuf;
	int			snd;
	mailboxbuf	*sndbuf;
}gsmdmailbox;



extern const char *user_msg_str[][USER_NUM];
extern tMSG_QUEUE *guser_reqmsg_que;
extern gsmdmailbox gmbox[USER_NUM];	//有USER_NUM个通信通道可用
extern SEMAPHORE  *psema_guser[USER_NUM][RESULT_NUM];

extern int u_send(int fd, char *buf, int len, int flag);
extern int u_recv(int fd, char *buf, int len, int flag);



extern struct gsmd_ucmd *ucmd_alloc(int extra_size);
extern int usock_init(struct gsmd *g);
extern void usock_cmd_enqueue(struct gsmd_ucmd *ucmd, struct gsmd_user *gu);
extern struct gsmd_ucmd *usock_build_event(u_int8_t type, u_int8_t subtype, u_int16_t len);
extern int usock_evt_send(struct gsmd *gsmd, struct gsmd_ucmd *ucmd, u_int32_t evt);
extern int gsmd_ucmd_submit(struct gsmd_user *gu, u_int8_t msg_type,
		u_int8_t msg_subtype, u_int16_t id, int len, const void *data);
extern int gsmd_opname_init(struct gsmd *g);
extern int gsmd_opname_add(struct gsmd *g, const char *numeric_bcd_string,
		const char *alnum_long);
		
//#endif /* __GSMD__ */

#endif

