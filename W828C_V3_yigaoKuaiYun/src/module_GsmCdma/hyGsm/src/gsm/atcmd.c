/* gsmd AT command interpreter / parser / constructor
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
//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"

#include "types.h"
#include "llist.h"

#include "gsmd.h"
#include "ts0705.h"
#include "ts0707.h"
#include "atcmd.h"
#include "unsolicited.h"
#include "temux.h"

extern int *EventGsmCdmaClient;
static int remove_timer(struct gsmd_atcmd * cmd);

int mux_flag = 0;
unsigned  testtime1,testtime2;
char *parse_buf = NULL;
char *ready_buf = NULL;
char *tran_buf = NULL;
char *save_buf = NULL;
static int mux_flow_flag = 0;
extern U8 recvBuf1[2000];
extern U32 recvBufIndex1;

enum final_result_codes {
	GSMD_RESULT_OK = 0,
	GSMD_RESULT_ERR = 1,
	NUM_FINAL_RESULTS,
};

#if 0
static const char *final_results[] = {
	"OK",
	"ERROR",
	"+CME ERROR:",
	"+CMS ERROR:",
};
#endif

/* we basically implement a parse that can deal with
 * - receiving and queueing commands from higher level of libgmsd
 * - optionally combining them into one larger command (; appending)
 * - sending those commands to the TA, receiving and parsing responses
 * - calling back application on completion, or waiting synchronously
 *   for a response
 * - dealing with intermediate and unsolicited resultcodes by calling
 *   back into the application / higher levels
 */

static inline int llparse_append(struct llparser *llp, char byte)
{
	if (llp->cur < llp->buf + llp->len) {
		*(llp->cur++) = byte;
		return 0;
	} else {
		//DEBUGP("llp->cur too big!!!\n");
		return -EFBIG;
	}
}

static inline void llparse_endline(struct llparser *llp)
{
	/* re-set cursor to start of buffer */
	llp->cur = llp->buf;
	llp->state = LLPARSE_STATE_IDLE;
	memset(llp->buf, 0, LLPARSE_BUF_SIZE);
}

static int llparse_byte(struct llparser *llp, char byte)
{
	int ret = 0;

	switch (llp->state) {
	case LLPARSE_STATE_IDLE:
	case LLPARSE_STATE_PROMPT_SPC:
		if (llp->flags & LGSM_ATCMD_F_EXTENDED) 
		{
			if (byte == '\r')
				llp->state = LLPARSE_STATE_IDLE_CR;
			else if (byte == '>')
				llp->state = LLPARSE_STATE_PROMPT;
			else if(byte == '~') {
				llp->state = LLPARSE_STATE_GPRS_DATA_H;	//GPRS Data
				ret = llparse_append(llp, byte);
			} 
			else 
			{
#ifdef STRICT
				llp->state = LLPARSE_STATE_ERROR;
#else
				//hyUsbPrintf("last state = %d   %c\r\n",llp->state,byte);
				llp->state = LLPARSE_STATE_RESULT;
				ret = llparse_append(llp, byte);
#endif
			}
		} else 
		{
			llp->state = LLPARSE_STATE_RESULT;
			ret = llparse_append(llp, byte);
		}
		break;
	case LLPARSE_STATE_IDLE_CR:
		if (byte == '\n')
			llp->state = LLPARSE_STATE_IDLE_LF;
		else if (byte != '\r')
			llp->state = LLPARSE_STATE_ERROR;
		break;
	case LLPARSE_STATE_IDLE_LF:
		/* can we really go directly into result_cr ? */
		if (byte == '\r')
			llp->state = LLPARSE_STATE_RESULT_CR;
		else if (byte == '>')
			llp->state = LLPARSE_STATE_PROMPT;
		else {
			llp->state = LLPARSE_STATE_RESULT;
			ret = llparse_append(llp, byte);
		}
		break;
	case LLPARSE_STATE_RESULT:
		if (byte == '\r')
			llp->state = LLPARSE_STATE_RESULT_CR;
		else if ((llp->flags & LGSM_ATCMD_F_LFCR) && byte == '\n')
			llp->state = LLPARSE_STATE_RESULT_LF;
		else
			ret = llparse_append(llp, byte);
		break;
	case LLPARSE_STATE_RESULT_CR:
		if (byte == '\n')
			llparse_endline(llp);
		if (byte == '\r')     //20090808
		{
			llparse_endline(llp);
			llp->state = LLPARSE_STATE_IDLE_CR;
		}
		break;
	case LLPARSE_STATE_RESULT_LF:
		if (byte == '\r')
			llparse_endline(llp);
		break;
	case LLPARSE_STATE_PROMPT:
		if (byte == ' ')
			llp->state = LLPARSE_STATE_PROMPT_SPC;
		else {
			/* this was not a real "> " prompt */
			llparse_append(llp, '>');
			ret = llparse_append(llp, byte);
			llp->state = LLPARSE_STATE_RESULT;
		}
		break;
		
	case LLPARSE_STATE_GPRS_DATA_H:
		/* can we really go directly into result_cr ? */
		if (byte == '~') {
			llp->state = LLPARSE_STATE_GPRS_DATA_E;
			ret = llparse_append(llp, byte);
		}
		else {
			llp->state = LLPARSE_STATE_GPRS_DATA;
			ret = llparse_append(llp, byte);
		}
		break;
	case LLPARSE_STATE_GPRS_DATA:
		if (byte == '~') {
			llp->state = LLPARSE_STATE_GPRS_DATA_E;
			ret = llparse_append(llp, byte);
		}
		else
			ret = llparse_append(llp, byte);
		break;
	case LLPARSE_STATE_GPRS_DATA_E:
		if (byte == '\r')
		{
			llparse_endline(llp);
			llp->state = LLPARSE_STATE_IDLE_CR;
		}
		else if(byte == '~')	//GPRS Data
		{
			llparse_endline(llp);
			llp->state = LLPARSE_STATE_GPRS_DATA_H;
			ret = llparse_append(llp, byte);
		}
		else
			llp->state = LLPARSE_STATE_ERROR;
		break;

	case LLPARSE_STATE_ERROR:
		if (byte == '\r')
		{
			llparse_endline(llp);
			llp->state = LLPARSE_STATE_IDLE_CR;
		}
		else if(byte == '~')	//GPRS Data
		{
			llparse_endline(llp);
			llp->state = LLPARSE_STATE_GPRS_DATA_H;
			ret = llparse_append(llp, byte);
		}
		break;
	}

	return ret;
}

static int llparse_string(struct llparser *llp, char *pdata, unsigned int len)
{
	struct gsmd_atcmd *cmd = NULL;
	struct gsmd *g = llp->ctx;
	char *buf = pdata;

	if( (buf[0] != 0xf9)&&(mux_flag != 1)||(buf[0] == 0x0d) )//
	{
		while (len--)
		{
			int rc = llparse_byte(llp, *(buf++));
			if (rc < 0)
				return rc;


			/* if _after_ parsing the current byte we have finished,
			 * let the caller know that there is something to handle */
				if (llp->state == LLPARSE_STATE_RESULT_CR || 
					llp->state == LLPARSE_STATE_GPRS_DATA_E)
			{
				/* FIXME: what to do with return value ? */
				llp->cb(llp->buf, llp->cur - llp->buf, llp->ctx);
			}

			/* if a full SMS-style prompt was received, poke the select */
			if (llp->state == LLPARSE_STATE_PROMPT_SPC)
				llp->prompt_cb(llp->ctx);
		}
	}
	else
	{
		
		//memcpy(llp->buf,buf,len);
		//llp->len = len;
		if (!llist_empty(&g->busy_atcmds))
			cmd = llist_entry(g->busy_atcmds.next, struct gsmd_atcmd, list);		
		if(cmd != NULL)
		{
			if (cmd && !strncmp(buf, cmd->buf,len-1)) 
			{
			//DEBUGP("ignoring echo\n");
				return 0;
			}
			cmd->resp = (char *)buf;
			cmd->buflen = len;
			//cmd->resp[len] = 0;
			cmd->cb(cmd, cmd->ctx, cmd->resp);
			remove_timer(cmd);
	    	llist_del(&cmd->list);
	    	g->mlunsolicited = 0;
	    	g->mlbuf_len = 0;
	    	FREE(cmd);


	        if (llist_empty(&g->busy_atcmds)) {
	        
	            if (!llist_empty(&g->pending_atcmds))
	                atcmd_wake_pending_queue(g);
	            else if(0==g->net_gprs)
	            	modem_sleep_enable();
	        }
      	}
	}

	return 0;
}

static int llparse_init(struct llparser *llp)
{
	llp->state = LLPARSE_STATE_IDLE;
	return 0;
}

#if 0
/* mid-level parser */

static int parse_final_result(const char *res)
{
	int i;
	for (i = 0; i < NUM_FINAL_RESULTS; i++) {
		if (!strcmp(res, final_results[i]))
			return i;
	}
	
	return -1;
}
#endif

void atcmd_wake_pending_queue (struct gsmd *g) 
{
        g->gfd_uart.when |= GSMD_FD_WRITE;
}

void atcmd_wait_pending_queue (struct gsmd *g) 
{
        g->gfd_uart.when &= ~GSMD_FD_WRITE;
}


void gframe_wake_pending_queue (struct gsmd *g) 
{
        g->gfd_uart.when |= GSMD_FD_WRITE_GPRS;
}


void gframe_wait_pending_queue (struct gsmd *g) 
{
        g->gfd_uart.when &= ~GSMD_FD_WRITE_GPRS;
}



static int atcmd_done(struct gsmd *g, struct gsmd_atcmd *cmd, const char *buf)
{
        int rc = 0;
        
	#ifdef _GSMD_DEBUG_LOG_2
        struct timeval exp, *exp1;
        
        exp1 = (struct timeval *)(&(cmd->timeout->expires));
        exp.tv_sec = exp1->tv_sec;
        exp.tv_usec = exp1->tv_usec;
	#endif
        
		/* remove timer if get respond before timeout */
		remove_timer(cmd);
        if (!cmd->cb) {
                //gsmd_log(GSMD_NOTICE, "command without cb!!!\n");
        } else {
                //DEBUGP("Calling final cmd->cb()\n");
                /* send final result code if there is no information
                * response in mlbuf */
                if (g->mlbuf_len) {
                        cmd->resp = (char *) g->mlbuf;
                        g->mlbuf[g->mlbuf_len] = 0;
                } else {
                        cmd->resp = (char *) buf;
                }

			#ifdef _GSMD_DEBUG_LOG_2
				gsmd_debug_log("\n",1, 0, NULL);
				gsmd_debug_log(cmd->buf,strlen(cmd->buf), 0, NULL);
				gsmd_debug_log("\n",1, 0, NULL);
				gsmd_debug_log(cmd->resp,strlen(cmd->resp), 1, &exp);
			#endif

                rc = cmd->cb(cmd, cmd->ctx, cmd->resp);
                //DEBUGP("Clearing mlbuf\n");
                memset(g->mlbuf, 0, MLPARSE_BUF_SIZE);
                g->mlbuf_len = 0;
        }
        
        /* remove from list of currently executing cmds */
        llist_del(&cmd->list);
        FREE(cmd);
        
        /* if we're finished with current commands, but still have pending
        * commands: we want to WRITE again */


        if (llist_empty(&g->busy_atcmds)) {
        
            if (!llist_empty(&g->pending_atcmds))
                atcmd_wake_pending_queue(g);
            else if(0==g->net_gprs)
            	modem_sleep_enable();
        }

        return rc;
}



static int ml_parse(const char *buf, int len, void *ctx)
{
	struct gsmd *g = ctx;
	struct gsmd_atcmd *cmd = NULL;
	int rc = 0;
	int cme_error = 0L;
	int cms_error = 0;


	//DEBUGP("buf=`%s'(%d)\n", buf, len);

	/* FIXME: This needs to be part of the vendor plugin. If we receive
	 * an empty string or that 'ready' string, we need to init the modem */
	if (strlen(buf) == 0) {
		return 0;
	}

	/* 如果固定波特率，模块正常启动先上报RDY，+CPIN和+CFUN信息，然后上报Call Ready
	 * 如果没有固定波特率，模块正常启动，收到串口数据后自适应波特率，然后上报Call Ready  */


#ifdef GSM_MODULE_SIM300
	if (0==strcmp(buf, "RDY") || 0 == strcmp(buf, "Call Ready"))
#elif defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)
	if (0==strcmp(buf, "+EIND: 128"))
#elif defined(GSM_MODULE_ULC2)
	if (strncmp(buf, "+XDRVI", 6) == 0)
#endif
	{
		//PhoneTrace(1,"GSM rx ok");
		//if (PhoneTrace_base != NULL)
		//	PhoneTrace_base(0,"GSM rx ok");
		if(0 == g->interpreter_ready)
		{
			//模块开机
			gsmd_initsettings2(g, 0);
			ewakeup(&EventGsmCdmaClient);
		}
		else
		{
		//if (PhoneTrace_base != NULL)
		//	PhoneTrace_base(0,"GSM reset!!!!");
		//hyUsbPrintf("GSM reset!!!!\r\n");
			//模块在使用过程中复位
			gsmd_initsettings2(g, 1);
					
			unsolicited_parse(g, buf, len, NULL);
			//如果模块在gprs connect状态时出现自动复位，发送gprs disconnect事件
			if (1==g->net_gprs)
			{
				gprs_evt_send(g, 0);
			}
			init_variable();
		}
		g->interpreter_ready = 1;
		g->net_gprs = 0;
		return 0;
	}


#ifdef GSM_MODULE_ULC2
	if( !strncmp(buf, "AT+CFUN?", 8))
	{
		gsmd_initsettings2(g, 1);
		
	}
#endif



	//GPRS data
	if (buf[0] == '~' ) {
		rc = usock_gdata_enqueue(g, buf, len);
		return rc;
	}


	/* responses come in order, so first response has to be for first
	 * command we sent, i.e. first entry in list */
	if (!llist_empty(&g->busy_atcmds))
		cmd = llist_entry(g->busy_atcmds.next, struct gsmd_atcmd, list);

	if (cmd && !strcmp(buf, cmd->buf)) {
		//DEBUGP("ignoring echo\n");
		return 0;
	}

	/* we have to differentiate between the following cases:
	 *
	 * A) an information response ("+whatever: ...")
	 *    we just pass it up the callback
	 * B) an unsolicited message ("+whateverelse: ... ")
	 *    we call the unsolicited.c handlers
	 * C) a final response ("OK", "+CME ERROR", ...)
	 *    in this case, we need to check whether we already sent some
	 *    previous data to the callback (information response).  If yes,
	 *    we do nothing.  If no, we need to call the callback.
	 * D) an intermediate response ("CONNECTED", "BUSY", "NO DIALTONE")
	 *    TBD
	 */

	if (buf[0] == '+' /*|| strchr(g->vendorpl->ext_chars, buf[0])*/) {
		/* an extended response */
		const char *colon = strchr(buf, ':');

		if (!colon) {
			//gsmd_log(GSMD_ERROR, "no colon in extd response `%s'\n", buf);
#ifdef GSM_MODULE_ULC2
			if (!strncmp("+PBREADY", buf, 8))
				return unsolicited_parse(g, buf, len, NULL);
			else
#endif
				return -EINVAL;
		}
		if (!strncmp(buf+1, "CME ERROR", 9)) {
			/* Part of Case 'C' */
			unsigned long err_nr;
			err_nr = strtoul(colon+1, NULL, 10);
			//DEBUGP("error number %lu\n", err_nr);
			if (cmd)
				cmd->ret = err_nr;
			cme_error = 1;
			goto final_cb;
		}
		if (!strncmp(buf+1, "CMS ERROR", 9)) {
			/* Part of Case 'C' */
			unsigned long err_nr;
			
			err_nr = strtoul(colon+1, NULL, 10);
			//DEBUGP("error number %lu\n", err_nr);
			if (cmd)
				cmd->ret = err_nr;
			cms_error = 1;
			goto final_cb;
		}

		if (!cmd || strncmp(buf, &cmd->buf[2], colon-buf)) {
			/* Assuming Case 'B' */
			//DEBUGP("extd reply `%s' to cmd `%s', must be "
			//       "unsolicited\n", buf, cmd ? &cmd->buf[2] : "NONE");
			colon++;
			if (colon > buf+len)
				colon = NULL;
			rc = unsolicited_parse(g, buf, len, colon);
			if (rc == -EAGAIN) {
				/* The parser wants one more line of
				 * input.  Wait for the next line, concatenate
				 * and resumbit to unsolicited_parse().  */
				//DEBUGP("Multiline unsolicited code\n");
				g->mlbuf_len = len;
				memcpy(g->mlbuf, buf, len);
				g->mlunsolicited = 1;
				return 0;
			}
			/* if unsolicited parser didn't handle this 'reply', then we 
			 * need to continue and try harder and see what it is */
			if (!cmd || rc != -ENOENT) {
				/* Case 'B' finished */
				return rc;
			}

			/* contine, not 'B' */
		}

		if (cmd) {
			if (cmd->buf[2] != '+' /*&& strchr(g->vendorpl->ext_chars, cmd->buf[2]) == NULL*/) {
				//gsmd_log(GSMD_ERROR, "extd reply to non-extd command?\n");
				return -EINVAL;
			}

			/* if we survive till here, it's a valid extd response
			 * to an extended command and thus Case 'A' */

			/* it might be a multiline response, so if there's a previous
			   response, send out mlbuf and start afresh with an empty buffer */
			if (g->mlbuf_len) {

				if (!cmd->cb) {
					//gsmd_log(GSMD_NOTICE, "command without cb!!!\n");
				} else {
					//DEBUGP("Calling cmd->cb()\n");
					cmd->resp = (char *) g->mlbuf;

				#ifdef _GSMD_DEBUG_LOG_2
				{
					struct timeval *exp;
				
					gsmd_debug_log("\n",1, 0, NULL);
					gsmd_debug_log(cmd->buf,strlen(cmd->buf), 0, NULL);
					gsmd_debug_log("\n",1, 0, NULL);
					
					exp = (struct timeval *)(&(cmd->timeout->expires));
					gsmd_debug_log(cmd->resp,strlen(cmd->resp), 1, exp);
				}
				#endif
					
					//有多行回复的命令每接收到一次回复(除最终回复)，timeout值自动延长
					if(cmd->timeout)
					{
						struct timeval *expires;
						struct timeval tv;

						gettimeofday(&tv, NULL);
						
						expires = (struct timeval *)(&(cmd->timeout->expires));
						expires->tv_sec = tv.tv_sec + GSMD_ATCMD_TIMEOUT_2;
						expires->tv_usec = tv.tv_usec;
					}


					rc = cmd->cb(cmd, cmd->ctx, cmd->resp);
					//DEBUGP("Clearing mlbuf\n");
					memset(g->mlbuf, 0, MLPARSE_BUF_SIZE);
				}
				g->mlbuf_len = 0;
			}

			/* the current buf will be appended to mlbuf below */
		}
	} else {
		if (!strcmp(buf, "RING") ||
		    ((g->flags & GSMD_FLAG_V0) && buf[0] == '2')) {
			/* this is the only non-extended unsolicited return
			 * code, part of Case 'B' */
			return unsolicited_parse(g, buf, len, NULL);
		}

		if (!strcmp(buf, "ERROR") ||
		    ((g->flags & GSMD_FLAG_V0) && buf[0] == '4')) {
			/* Part of Case 'C' */
			//DEBUGP("unspecified error\n");
			if (cmd)
				cmd->ret = 4;
			goto final_cb;
		}

#ifdef GSM_MODULE_ULC2
			if (!strcmp(buf, "ABORTED") ||
			    ((g->flags & GSMD_FLAG_V0) && buf[0] == '4')) {
				/* Part of Case 'C' */
				if (cmd)
					cmd->ret = 5;
				goto final_cb;
			}
#endif

		if (!strncmp(buf, "OK", 2) ||
		    ((g->flags & GSMD_FLAG_V0) && buf[0] == '0')) {
			/* Part of Case 'C' */
			if (cmd)
				cmd->ret = 0;
			goto final_cb;
		}


		/* FIXME: handling of those special commands in response to
		 * ATD / ATA */
		if (!strncmp(buf, "NO CARRIER", 10) || ((g->flags & GSMD_FLAG_V0) && buf[0] == '3')) 
		{
			/* Part of Case 'D' */
			if (!cmd || (cmd && !strncmp(cmd->buf, "AT+", 3)))
			{
				/* this is an unsolicited return code, part of Case 'B' */
				if(1 == g->muxchl_recv_chl)
				{
					return unsolicited_parse(g, buf, len, NULL);
				}
				else if(2 == g->muxchl_recv_chl)
				{
					return gprs_evt_send(g, 0);
				}
				else //no mux
				{
					if (0 == g->net_gprs)
					{
						return unsolicited_parse(g, buf, len, NULL);
					}
					else
					{
						//GPRS disconnected
					    return gprs_evt_send(g, 0);
					}
				}
			}
			else
			{
				//if (cmd)
					cmd->ret = 3;
				goto final_cb;
			}
		}


		if (!strncmp(buf, "CONNECT", 7)) {

			if (!cmd || (cmd && !strncmp(cmd->buf, "AT+", 3)))
			{
				/* this is an unsolicited return code, part of Case 'B' */
				return unsolicited_parse(g, buf, len, NULL);
			}
			else
			{
				//if (cmd)
					cmd->ret = 0;
				goto final_cb;
			}
		}

#ifdef GSM_MODULE_MD231
		if (!strncmp(buf, "RINGBACK", 8))
		{
			return unsolicited_parse(g, buf, len, NULL);
		}
		
		if (!strncmp(buf, "VOICE", 5))
		{
			return unsolicited_parse(g, buf, len, NULL);
		}
#endif

		if (!strncmp(buf, "BUSY", 4) ||
		    ((g->flags & GSMD_FLAG_V0) && buf[0] == '7')) {
			/* Part of Case 'D' */
			return unsolicited_parse(g, buf, len, NULL);
		}
		
		if (!strncmp(buf, "NO DIALTONE", 11)) {
			/* Part of Case 'D' */
			return unsolicited_parse(g, buf, len, NULL);
		}

#ifdef GSM_MODULE_SIM300
		if (!strncmp(buf, "NORMAL POWER DOWN", 17)) {
			/* Part of Case 'D' */
			if (cmd)
				cmd->ret = 0;
			goto final_cb;
		}
#endif

#ifdef GSM_MODULE_MD231
		/*
		AT+SHUTDOWN
		OK
		SHUTDOWN SUCCESS
		*/
		if (!strncmp(buf, "SHUTDOWN SUCCESS", 17)) {
			return 0;
		}
#endif
	}

	/* we reach here, if we are at an information response that needs to be
	 * passed on */
	 if(len+g->mlbuf_len >= MLPARSE_BUF_SIZE)
	 {
	 	return rc;
	 }

	if (g->mlbuf_len)
		g->mlbuf[g->mlbuf_len ++] = '\n';
	//DEBUGP("Appending buf to mlbuf\n");
	if (len > MLPARSE_BUF_SIZE - g->mlbuf_len)
		len = MLPARSE_BUF_SIZE - g->mlbuf_len;
	memcpy(g->mlbuf + g->mlbuf_len, buf, len);
	g->mlbuf_len += len;

	if (g->mlunsolicited) {
		rc = unsolicited_parse(g, (const char*) g->mlbuf, (int) g->mlbuf_len,
				strchr((const char*)g->mlbuf, ':') + 1);
		if (rc == -EAGAIN) {
			/* The parser wants one more line of
			 * input.  Wait for the next line, concatenate
			 * and resumbit to unsolicited_parse().  */
			//DEBUGP("Multiline unsolicited code\n");
			return 0;
		}
		g->mlunsolicited = 0;
		g->mlbuf_len = 0;
	}
	return rc;

final_cb:
	/* if reach here, the final result code of a command has been reached */

	if (!cmd)
		return rc;

	if (cmd && cme_error)
		generate_event_from_cme(g, cmd->ret);

	if (cmd && cms_error)
		generate_event_from_cms(g, cmd->ret);

	return atcmd_done(g, cmd, buf);
}

/* called when the modem asked for a new line of a multiline atcmd */
static int atcmd_prompt(void *data)
{
	struct gsmd *g = data;

    atcmd_wake_pending_queue(g);
	return 0;
}

#if 1
static int mux_llparse_byte(unsigned char *buf, char byte,int *state)
{
	static int ret = 0;	
	int parse_cnt = 0;
	static int this_cnt = 0;
	static int flag = 0;
	switch (*state) 
	{
		case 2:				
			memset(buf, 0, 512);//新的解析
			*state = 0;		
			ret = 0;
		case 0:
			if( (byte == (char)0xf9)&&(ret == 0) )
			{
				*state = 1;
				*buf = byte;	
				ret += 1;			
			}
			break;
		case 1:
			if(ret > 0)
			{
				ret += 1;
				if( (byte == (char)0xf9)&&((ret - 6) >= this_cnt)) 
				{
					*buf = byte;				
					*state = 2;//解析MUX数据
					parse_cnt = ret;	
					ret = 0;	
					this_cnt = 0;		
				}
				else
				{				
					*buf = byte;				
				}
				if(ret == 4)
				{
					if( (byte&0x01) == 0 )//长度是两个字节
					{
						flag = 1;						
					}
					this_cnt = (byte >> 1) & 0x7f;
				//hyUsbPrintf("444----------byte == 0x%x-----------this_cnt == %d\r\n",byte,this_cnt);
				}
				if(ret == 5)				
				{
					if(flag == 1)
					{
						//this_cnt |=(((uint32)(byte) << 8) & 0xff00) >> 1;
  						this_cnt |=((uint32)(byte) << 7);
						flag = 0;
					//hyUsbPrintf("555----------byte == 0x%x-----------this_cnt == %d\r\n",byte,this_cnt);
					}	
				}
				if (this_cnt > 2048)
				{
					//当前帧的大小超过2048，认为是错误的数据帧，丢弃，
					//并恢复state几个static值
					*state = 0;
					this_cnt = 0;
					flag = 0;
					ret = 0;
					return -1;
				}
			}
			break;			
	}
	return parse_cnt;
}
#else
	int llparse_byte_ret = 0;	
	int llparse_byte_this_cnt = 0;
	int llparse_byte_flag = 0;
static int mux_llparse_byte(unsigned char *buf, char byte,int *state)
{
	int parse_cnt = 0;
	switch (*state) 
	{
		case 2:				
			memset(buf, 0, 512);//新的解析
			*state = 0;		
			llparse_byte_ret = 0;
		case 0:
			if( (byte == (char)0xf9)&&(llparse_byte_ret == 0) )
			{
				*state = 1;
				*buf = byte;	
				llparse_byte_ret += 1;			
			}
			break;
		case 1:
			if(llparse_byte_ret > 0)
			{
				llparse_byte_ret += 1;
				if( (byte == (char)0xf9)&&((llparse_byte_ret - 6) >= llparse_byte_this_cnt))
				{
					*buf = byte;				
					*state = 2;//解析MUX数据
					parse_cnt = llparse_byte_ret;	
					llparse_byte_ret = 0;	
					llparse_byte_this_cnt = 0;		
				}
				else
				{				
					*buf = byte;				
				}
				if(llparse_byte_ret == 4)
				{
					if( (byte&0x01) == 0 )//长度是两个字节
					{
						llparse_byte_flag = 1;						
					}
					llparse_byte_this_cnt = (byte >> 1) & 0x7f;
				//hyUsbPrintf("444----------byte == 0x%x-----------this_cnt == %d\r\n",byte,this_cnt);
				}
				if(llparse_byte_ret == 5)
				{
					if(llparse_byte_flag == 1)
					{
						//this_cnt |=(((uint32)(byte) << 8) & 0xff00) >> 1;
  						llparse_byte_this_cnt |=((uint32)(byte) << 7);
						llparse_byte_flag = 0;
					//hyUsbPrintf("555----------byte == 0x%x-----------this_cnt == %d\r\n",byte,this_cnt);
					}	
				}
			}
			break;			
	}
	return parse_cnt;
}

#endif


//返回发送的长度
int mux_uart_write(int fd, char *txbuf, int len, u_int8_t dlc)
{
	int rc = 0;
	
	/*
	unsigned char dlc;

	if( (flag &0x04) != 0 )
	{
		dlc = 1;
	}
	else
	{
		dlc = 2;
	}
	if(!strncmp(txbuf,"ATD*99#",7) || !strncmp(txbuf, "AT+CGDCONT=", 11) )//||(!strncmp(txbuf,"MSC,RT",6)))
	{
		dlc = 2;		
	}
	*/
	
	
	if(2 != dlc)
		dlc = 1;
	
	rc = MuxFrameConstruct(fd,dlc,MUX_CMD_SEND,MUX_UIH,txbuf,len,0);
	return rc;
}


#define CHL0	BIT0
#define CHL1	BIT1
#define CHL2	BIT2


/* callback to be called if [virtual] UART has some data for us */
static int atcmd_select_cb(int fd, unsigned int what, void *data)
{
	int len,len1, rc,temp,i,cnt,mux_cur;	
	struct gsmd *g = data;
	//MuxWriteInfoT *pFrameInfo = NULL;
	MuxWriteInfoT tFrameInfo;//kong
	char *cr = NULL,*pmux_temp1 = NULL,*pmux_temp2 = NULL,*p_start = NULL;
	static unsigned int offset = 0;
	static int parse_state = 0;		
	static int buf_state = 0;
	static int parse_len = 0;
	static int zclose = 0;	
	static unsigned int zs = 0;
	static int len_start;
	len_start = 0;
	i = 0;
	if (what & GSMD_FD_READ) 
	{
		char rxbuf[2048/*1024*/];

		//testGsmd_select_list();
		
		while ((len = gsmcdma_uart_read(fd, rxbuf, sizeof(rxbuf)))) 
		{
			//testGsmd_select_list();
			//if (len < 0) {
			if (len <= 0) 
			{//kong
				//if (errno == EAGAIN)
				//	return 0;
				//gsmd_log(GSMD_NOTICE, "ERROR reading from fd %u: %d (%s)\n", fd, len, strerror(errno));
				return len;
			}

		#ifdef _GSMD_DEBUG_LOG_1
			gsmd_debug_log("\r\nread:\r\n",9, 0, NULL);
			gsmd_debug_log(rxbuf,len, 0, NULL);
		#endif
			
			p_start = strstr(rxbuf,"+XDRVI");
		    if( p_start)                      //module is restart	
		    {		        
		    	mux_flag = 0;
		    	//PhoneTrace(0,"restart!");		    	
		    	len_start = p_start-2 - rxbuf;
		    	offset = 0;		    	
		    }	

		    //hyUsbPrintf("GSM recv len == %d\r\n",len);
			//hyUsbMessageByLen(rxbuf,len);
			//hyUsbPrintf("\r\nkong\r\n");
		    if(mux_flag == 2)
		    {
		    	//解析
		    	i = 0;
		    	mux_cur = 0;
		    	pmux_temp2 = ready_buf+parse_len;
				pmux_temp1 = parse_buf;
		    	while (len--)
		    	{
		    		if(offset >= 2048 || (offset >= 1536 && parse_state == 2))
		    		{//kong
		    		hyUsbPrintf("+++++++++++++++++++parse_buf offset err == %d\r\n",offset);
		    			offset = 0;
		    			parse_state = 0;
		    			break;
		    		}
		    		
		    		cnt = mux_llparse_byte(&pmux_temp1[offset],*(rxbuf+i),&parse_state);
		    		//testGsmd_select_list();
		    		i++;
		    		if (cnt < 0)
		    		{
		    			//错误的mux包，丢弃
		    			hyUsbPrintf("cmux parse_byte err!!!!!!!\r\n");
		    			offset = 0;
		    			continue;
		    		}
		    		offset++;
		    		
		    		if(parse_state == 2)
				    {
				    	//F9 01 EF 09 E1 05 0B 09 9A F9 
						char buf[]={0xF9,0x01,0xFF,0x09,0xE1,0x05,0x0B,0x09,0x9A,0xF9,0x00};
												
						if(!strncmp(pmux_temp1,buf,10))
						{
							buf_state = 3;
						}
						else
						{
							u_int8_t dlc,fc;
							bool rc;
							//testGsmd_select_list();
					    	rc = MuxFrameDestruct(pmux_temp1,offset,&tFrameInfo);	//解帧
					    	if (rc == FALSE)
					    	{
					    		
					    		char *tembuf = pmux_temp1+4;
					    		if(0x0d == tembuf[0] && 0x0a == tembuf[1])
								{
					    			hyUsbPrintf("cmux AT Destruct err!!!!!!!  len=%d\r\n", offset);
					    		}
					    		else
					    		{
					    			hyUsbPrintf("cmux PPP Destruct err!!!!!!!  len=%d\r\n", offset);
					    		}
					    		//错误的mux包，丢弃
					    		parse_state = 0;
					    		offset = 0;
					    		buf_state = 0;
					    		continue;
					    	}
							//testGsmd_select_list();
					    	g->muxchl_recv_chl = (u_int32_t)((tFrameInfo.frame.addr)>>2);


					    	//判断是否msc帧
					    	if(1 == mux_msc_get(fd, &tFrameInfo, &dlc, &fc))
					    	{
					    		if(fc)
					    			g->muxchl_fctl_flag |= (u_int32_t)(1<<dlc);
					    		else
					    			g->muxchl_fctl_flag &= (u_int32_t)(~(1<<dlc));								

					    		offset -= 11;
					    		
					    		continue;
					    	}
					    }

					    offset = 0;

				 		 //0d 0a AT 0d 0a 
				    	switch(buf_state)//组合正常帧
				    	{
				    		case 0:
				    			if((len >= 0)&&(tFrameInfo.frame.writeInfo.totalLen <= 2))
				    			{
									buf_state += 1;			
				    		    }
				    		    else
				     		    {
				    		       	buf_state += 3;
				    		    }
				    		    memcpy(pmux_temp2,tFrameInfo.frame.writeInfo.buffer,tFrameInfo.frame.writeInfo.totalLen);
								pmux_temp2 += tFrameInfo.frame.writeInfo.totalLen;	
								parse_len += tFrameInfo.frame.writeInfo.totalLen;
				    			break;
				    		case 1:
				    			buf_state += 1;				    			
				    			memcpy(pmux_temp2,tFrameInfo.frame.writeInfo.buffer,tFrameInfo.frame.writeInfo.totalLen);
				    		   	pmux_temp2 += tFrameInfo.frame.writeInfo.totalLen;	
								parse_len += tFrameInfo.frame.writeInfo.totalLen;
				    			break;
				    		case 2:	
				    			buf_state += 1;    			
				    		    memcpy(pmux_temp2,tFrameInfo.frame.writeInfo.buffer,tFrameInfo.frame.writeInfo.totalLen);
								pmux_temp2 += tFrameInfo.frame.writeInfo.totalLen;	
								parse_len += tFrameInfo.frame.writeInfo.totalLen;
				    			break;
				    		case 3:
				    			memcpy(pmux_temp2,pmux_temp1,10);
								pmux_temp2 += 10;	
								parse_len += 10;	  
				    			break;
				    		default:
				    		    break;
				    	}	
				    }

				    if(buf_state == 3)
				    {
				   
				    	g->llp.flags |= LGSM_ATCMD_F_EXTENDED;
				    	llparse_string(&g->llp, ready_buf+mux_cur, parse_len);	//处理帧
						mux_cur += parse_len;
				    	parse_len = 0;
				    	buf_state = 0;
				    }
		    	}//	WHILE(1)

		    	if(parse_len >0)
				{
					memcpy(tran_buf,pmux_temp2-parse_len,parse_len);
					memcpy(ready_buf,tran_buf,parse_len);
				}
		    }
   		    else
   		    {
		    	if((rxbuf[0] != 0xf9)&&(offset == 0))//(parse_len == 0))
		    	{

					
					#ifdef _GSMD_DEBUG_LOG_2
						gsmd_debug_log("\r\ntest1_log:\r\n", 12, 0, NULL);
						gsmd_debug_log(rxbuf+len_start,len, 0, NULL);						
					#endif
					
		    		rc = llparse_string(&g->llp, rxbuf+len_start, len);
		    		len_start = 0;
		    	}
		    	else
		    	{
		    		char resp_dlc = 0;
					
		    		pmux_temp2 = ready_buf;
		    		while (len--)
		    		{
		    			if(offset >= 2048 || (offset >= 1536 && parse_state == 2))
			    		{//kong
			    		hyUsbPrintf("--------------------parse_buf offset err == %d\r\n",offset);
			    			offset = 0;
			    			parse_state = 0;
			    			break;
			    		}
			    		
			    		cnt = mux_llparse_byte(&pmux_temp2[offset],*(rxbuf+i+len_start),&parse_state);
			    		i++;
			    		offset++;
			    		if(parse_state == 2)
					    {
					    	offset = 0;

					    	#ifdef _GSMD_DEBUG_LOG_2
								gsmd_debug_log("\r\ntest2_log:\r\n", 12, 0, NULL);
								gsmd_debug_log(rxbuf+len_start,len, 0, NULL);		
							#endif
							len_start = 0;
					    	if(ready_buf[4] == 0xE3)
					    	{
					    		if(ready_buf[6] == 0x07 || ready_buf[6] == 0x0B)
					    		{
					    			resp_dlc = ready_buf[6];
					    		}
					    		//parse_len = 0;
				    			buf_state = 0;	
					    		continue;
					    	}
					    	g->llp.flags |= LGSM_ATCMD_F_EXTENDED;	
				    		llparse_string(&g->llp, ready_buf, cnt);
					    }
					}

					if (resp_dlc)
					{
						char msc_dlc1[] = {0xF9,0x01,0xFF,0x0B,0xE1,0x07,0x07,0x0D,0x01,0x6C,0xF9};	// F9 01 FF 0B E1 07 07 0D 01 6C F9
						msc_dlc1[6] = resp_dlc;
					    gsmcdma_uart_write(fd, msc_dlc1, 11);
					}

					//parse_len = offset;
					i = 0;
		    	}
		    }
		    
			if (rc < 0)
			{
				return rc;
			}
		}
	}


	/* write pending commands to UART */
	if (1==g->interpreter_ready && modem_rcv_check()) 
	{


		//testGsmd_select_list();
		if((what&GSMD_FD_WRITE) && 0==(g->muxchl_fctl_flag&CHL1)) 
		{

			//ATCMD mode
			struct gsmd_atcmd *pos, *pos2;
			char *sndbuf;
			//testGsmd_select_list();
			llist_for_each_entry_safe(pos, pos2, struct gsmd_atcmd, &g->pending_atcmds, list) 
			{


				cr = strchr(pos->cur, '\n');
				if (cr)
					len = cr - pos->cur;
				else
					len = pos->buflen - 1;  /* assuming zero-terminated strings */

				/*if(1 == test_log)
				{
				#ifdef _GSMD_DEBUG_LOG_1
					gsmd_debug_log("\r\nbuf_log:\r\n", 12, 0, NULL);
					gsmd_debug_log(pos->buf,pos->buflen, 0, NULL);
					gsmd_debug_log("\r\ncur_log:\r\n", 12, 0, NULL);
					gsmd_debug_log(pos->cur,len+1, 0, NULL);
				#endif

				}*/


				#ifdef GSM_MODULE_MD231
					rc = gsmcdma_uart_write(fd, pos->cur, len);
					if (rc == 0) {
						break;
					} else if (rc < 0) {
						return rc;
					}

					if (!cr || rc == len)
						rc ++;	/* Skip the \n or \0 */
					pos->buflen -= rc;
					pos->cur += rc;
					gsmcdma_uart_write(fd, "\r", 1);

				#elif defined(GSM_MODULE_ULC2)

						sndbuf = (char *)MALLOC(len+1);
						if(!sndbuf)
							return -ENOMEM;

						memcpy(sndbuf, pos->cur, len);
					if(mux_flag != 2)
					{	
						if(sndbuf[0] == 0xf9)
						{
							rc = gsmcdma_uart_write(fd,sndbuf,len);
						}
						else
						{
							sndbuf[len] = '\r';
							rc = gsmcdma_uart_write(fd, sndbuf, len+1);
						}
					}
					else
					{
						if(sndbuf[0] == 0xf9)
						{
							rc = gsmcdma_uart_write(fd,sndbuf,len);
						}
						else
						{
							sndbuf[len] = '\r';
							rc = mux_uart_write(fd, sndbuf, len+1, pos->id);
						}
					}
						FREE(sndbuf);

					if (rc == 0) {
						break;
					} else if (rc < 0) {
						return rc;
					}

			#ifdef _GSMD_DEBUG_LOG_1
				gsmd_debug_log("\r\nsend at:\r\n", 12, 0, NULL);
				gsmd_debug_log(pos->cur,len+1, 0, NULL);
			#endif
				
				pos->buflen -= (len + 1); /* Skip the \n or \0 */
				pos->cur += (len + 1);

				#endif

				if (!pos->buflen) {
					/* success: create atcommand timeout timer */
					pos->timeout = pos->create_timer_func(g);

					/* success: remove from global list of
					 * to-be-sent atcmds */
					llist_del(&pos->list);
					/* append to global list of executing atcmds */
					llist_add_tail(&pos->list, &g->busy_atcmds);

					g->write_multi_atcmd = 0;

					/* we only send one cmd at the moment */
					break;
				} else {

					if (rc < len) {
						/* The write was short */
						return 0;
					} else {
						/* The atcmd has more lines to send after a "> " */
						
						g->write_multi_atcmd = 1;
						break;
					}
				}
			}

			// ATCMD: Either pending_atcmds is empty or a command has to wait 
	        atcmd_wait_pending_queue(g);
		}


		if((what&GSMD_FD_WRITE_GPRS) && 0==(g->muxchl_fctl_flag&CHL2)) {

			//GPRS DATA mode
			struct gsmd_gframe *pos, *pos2;
			
			llist_for_each_entry_safe(pos, pos2, struct gsmd_gframe, &g->pending_gframes, list) {
				if(mux_flag < 2)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
				{
					rc = gsmcdma_uart_write(fd, pos->buf, pos->buflen);
					if (rc == 0) 
					{
						break;
					} 
					else if (rc < 0) 
					{
						return rc;
					}
					pos->buflen -= rc;
					if (!pos->buflen) {
						/* success: remove from global list of
						 * to-be-sent gprs frames */
						llist_del(&pos->list);
					} else {
						/* write again */
						rc = gsmcdma_uart_write(fd, (char *)(pos->buf+rc), pos->buflen);
					}
				}
				else
				{
					rc =  mux_uart_write(fd, pos->buf, pos->buflen,2);

					if (rc == 0)
					{
						break;
					}
					else if(rc < 0)
					{
						return rc;
					}


				#ifdef _GSMD_DEBUG_LOG_1
					gsmd_debug_log("\r\nsend data:\r\n", 14, 0, NULL);
					gsmd_debug_log(pos->buf, pos->buflen, 0, NULL);
				#endif

					pos->buflen = 0;
					if (!pos->buflen) 
					{
						llist_del(&pos->list);
					} 
				}
				
				FREE(pos);
			}

			// GPRS DATA: pending_gframes is empty
			gframe_wait_pending_queue(g);
		}
	}
	return 0;
}

static void discard_timeout(struct gsmd_timer *tmr, void *data) 
{
    struct gsmd *g=data;
    struct gsmd_atcmd *cmd=NULL;

#ifdef _GSMD_DEBUG_LOG_2
    struct timeval exp, *exp1;

#endif

//hyUsbPrintf("discard_timeout \r\n");
    //DEBUGP("discard time out!!\n");
    if (!llist_empty(&g->busy_atcmds)) {
            cmd = llist_entry(g->busy_atcmds.next,struct gsmd_atcmd, list);
    }
    if (!cmd) { 
            //DEBUGP("ERROR!! busy_atcmds is NULL\n");
            return;
    }
    if (cmd->timeout != tmr) {
            //DEBUGP("ERROR!! cmd->timeout != tmr\n");
            return;
    }

#ifdef _GSMD_DEBUG_LOG_2
    
    exp1 = (struct timeval *)(&(cmd->timeout->expires));
    exp.tv_sec = exp1->tv_sec;
    exp.tv_usec = exp1->tv_usec;
#endif

    gsmd_timer_free(cmd->timeout);
    cmd->timeout = NULL;


	if (cmd->cb) {
		cmd->resp = "Timeout";

	#ifdef _GSMD_DEBUG_LOG_2
		gsmd_debug_log("\n",1, 0, NULL);
		gsmd_debug_log(cmd->buf,strlen(cmd->buf), 0, NULL);
		gsmd_debug_log("\n",1, 0, NULL);
		gsmd_debug_log(cmd->resp,strlen(cmd->resp), 1, &exp);
	#endif
        cmd->cb(cmd, cmd->ctx, cmd->resp);
	}
	
	// discard the timeout at command
	llist_del(&cmd->list);
	FREE(cmd);
	
	// pass the next pending at command
    if (llist_empty(&g->busy_atcmds)) {
    
        if (!llist_empty(&g->pending_atcmds))
            atcmd_wake_pending_queue(g);
        else if(0==g->net_gprs)
        	modem_sleep_enable();
    }


	//added by song, 090512 
    if (g->mlbuf_len) {
        //DEBUGP("Discarding mlbuf: %.*s\n", g->mlbuf_len, g->mlbuf);
        g->mlbuf_len = 0;
    }
	gsmd_modem_alive_test(g);
}



static struct gsmd_timer * discard_timer1(struct gsmd *g)
{
	struct timeval tv;
	
	tv.tv_sec = GSMD_ATCMD_TIMEOUT_1;
	tv.tv_usec = 0;

	//DEBUGP("Create discard timer\n");
//hyUsbPrintf("discard_timer1 \r\n");
	return gsmd_timer_create(&tv, &discard_timeout, g);
}


struct gsmd_timer * discard_timer2(struct gsmd *g)
{
	struct timeval tv;
	
	tv.tv_sec = GSMD_ATCMD_TIMEOUT_2;
	tv.tv_usec = 0;

	//DEBUGP("Create discard timer\n");

	return gsmd_timer_create(&tv, &discard_timeout, g);
}


struct gsmd_atcmd *atcmd_fill(const char *cmd, int rlen,
			      atcmd_cb_t cb, void *ctx, u_int16_t id,
			      create_timer_t ct)
{
	int buflen = strlen(cmd);
	struct gsmd_atcmd *atcmd;

	if (rlen > buflen)
		buflen = rlen;

	atcmd = (struct gsmd_atcmd*)MALLOC(sizeof(*atcmd)+ buflen);
	if (!atcmd)
		return NULL;

	atcmd->ctx = ctx;
	atcmd->id = id;
	atcmd->flags = 0;
	atcmd->ret = -255;
	atcmd->buflen = buflen;
	atcmd->buf[buflen-1] = '\0';
	atcmd->cur = atcmd->buf;
	atcmd->cb = cb;
	atcmd->resp = NULL;
	atcmd->timeout = NULL;
	//strncpy(atcmd->buf, cmd, buflen-1);

	if(mux_flag >= 1)
	{
		//atcmd->flags |= 0x04;
	memcpy(atcmd->buf, cmd, buflen-1);
	}
	else
	{
		strncpy(atcmd->buf, cmd, buflen-1);
	}
	if (!ct)
		atcmd->create_timer_func = discard_timer1;
	else
		atcmd->create_timer_func = ct;

	return atcmd;
}


struct gsmd_gframe *gframe_fill(const u_int8_t *buf, int buflen, void *ctx)
{
	struct gsmd_gframe *frame;

	frame = (struct gsmd_gframe*)MALLOC(sizeof(*frame)+ buflen);
	if (!frame)
		return NULL;

	frame->ctx = ctx;
	frame->buflen = buflen;
	memcpy(frame->buf, buf, buflen);

	return frame;
}



static int remove_timer(struct gsmd_atcmd * cmd)
{
	if (cmd->timeout) {
		//DEBUGP("Get respond before timeout, remove timer!\n");
		gsmd_timer_unregister(cmd->timeout);
		gsmd_timer_free(cmd->timeout);
		cmd->timeout = NULL;
	} else {
		//DEBUGP("ERROR!! The %s response comes too late!!\n", cmd->buf);
	}

	return 0;
}


/* submit an atcmd in the global queue of pending atcmds */
int atcmd_submit(struct gsmd *g, struct gsmd_atcmd *cmd)
{
	//DEBUGP("submitting command `%s'\n", cmd->buf);

	llist_add_tail(&cmd->list, &g->pending_atcmds);
	if (llist_empty(&g->busy_atcmds) &&
		0==g->write_multi_atcmd	&&
		!llist_empty(&g->pending_atcmds)) {
		
		atcmd_wake_pending_queue(g);
		
		if (0==g->net_gprs)
			modem_sleep_disable();
	}

	return 0;
}


/* submit an atcmd in the global queue of pending atcmds */
int atcmd_submit_ext(struct gsmd *g, struct gsmd_atcmd *cmd, int head)
{
	//DEBUGP("submitting command `%s'\n", cmd->buf);

	if(head)
		llist_add(&cmd->list, &g->pending_atcmds);
	else
		llist_add_tail(&cmd->list, &g->pending_atcmds);

	if (llist_empty(&g->busy_atcmds) &&
		0==g->write_multi_atcmd	&&
		!llist_empty(&g->pending_atcmds)) {
		
		atcmd_wake_pending_queue(g);
		
		if (0==g->net_gprs)
			modem_sleep_disable();
	}

	return 0;
}


/* submit an atcmd in the global queue of pending atcmds */
int gframe_submit(struct gsmd *g, struct gsmd_gframe *gf)
{
	llist_add_tail(&gf->list, &g->pending_gframes);
	if (!llist_empty(&g->pending_gframes)) {
		gframe_wake_pending_queue(g);
	}

	return 0;
}



/* cancel a currently executing atcmd by issuing the command given as
 * parameter, usually AT ot ATH.  */
int cancel_atcmd(struct gsmd *g, struct gsmd_atcmd *cmd)
{
    struct gsmd_atcmd *cur;
    
    if (llist_empty(&g->busy_atcmds)) {
            return atcmd_submit(g, cmd);
    }
    cur = llist_entry(g->busy_atcmds.next, struct gsmd_atcmd, list);
    //DEBUGP("cancelling command `%s' with an `%s'\n", cur->buf, cmd->buf);
    
    if (g->mlbuf_len) {
            //DEBUGP("Discarding mlbuf: %.*s\n", g->mlbuf_len, g->mlbuf);
            g->mlbuf_len = 0;
    }
    
    llist_add(&cmd->list, &g->pending_atcmds);
    return atcmd_done(g, cur, "ERROR");
}



/* init atcmd parser */
int atcmd_init(struct gsmd *g, int fd)
{
	g->gfd_uart.fd = fd;
	g->gfd_uart.when = GSMD_FD_READ;
	g->gfd_uart.data = g;
	g->gfd_uart.cb = &atcmd_select_cb;

	INIT_LLIST_HEAD(&g->pending_atcmds);
	INIT_LLIST_HEAD(&g->busy_atcmds);

	INIT_LLIST_HEAD(&g->pending_gframes);
	
	llparse_init (&g->llp);

	g->mlbuf_len = 0;
	g->mlunsolicited = 0;
	g->alive_responded = 0;

	g->llp.cur = g->llp.buf;
	g->llp.len = sizeof(g->llp.buf);
	memset(g->llp.cur,0,LLPARSE_BUF_SIZE);//zd
	g->llp.cb = &ml_parse;
	g->llp.prompt_cb = &atcmd_prompt;
	g->llp.ctx = g;
	g->llp.flags = LGSM_ATCMD_F_EXTENDED;

	parse_buf = (char *)malloc(2048);
	ready_buf = (char *)malloc(2048);
    tran_buf  = (char *)malloc(2048);
    save_buf  = (char *)malloc(1024);
    memset(save_buf,0,1024);

	return gsmd_register_fd(&g->gfd_uart);
}


void atcmd_uninit(struct gsmd *g)
{

	//INIT_LLIST_HEAD(&g->pending_atcmds);
	//INIT_LLIST_HEAD(&g->busy_atcmds);
	FREE(parse_buf);
	FREE(ready_buf);
	FREE(tran_buf);
	FREE(save_buf);
	
	gsmd_unregister_fd(&g->gfd_uart);
}





/* remove from the queues any command whose .ctx matches given */
int atcmd_terminate_matching(struct gsmd *g, void *ctx)
{
	int num = 0;
	struct gsmd_atcmd *cmd, *pos;

	//busy_atcmds在atcmd_done()中destroy
	llist_for_each_entry_safe(cmd, pos, struct gsmd_atcmd, &g->busy_atcmds, list)
		if (cmd->ctx == ctx) {
			cmd->ret = -ESHUTDOWN;
			cmd->cb(cmd, cmd->ctx, "ERROR");
			cmd->cb = NULL;		//right
			cmd->ctx = NULL;	//right
			num ++;
		}

	llist_for_each_entry_safe(cmd, pos, struct gsmd_atcmd, &g->pending_atcmds, list)
		if (cmd->ctx == ctx) {
			llist_del(&cmd->list);
			cmd->ret = -ESHUTDOWN;
			cmd->cb(cmd, cmd->ctx, "ERROR");
			FREE(cmd);
			num ++;
		}

	return num;
}


/* remove from the queues any gprs frame whose .ctx matches given */
int gprsframe_terminate_matching(struct gsmd *g, void *ctx)
{
	int num = 0;
	struct gsmd_gframe *frame, *pos;
	
	llist_for_each_entry_safe(frame, pos, struct gsmd_gframe, &g->pending_gframes, list)
		if (frame->ctx == ctx) {
			llist_del(&frame->list);
			FREE(frame);
			num ++;
		}


	return num;
}


