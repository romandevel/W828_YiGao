/* gsmd core
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
#include "types.h"
#include "gsmd.h"
#include "atcmd.h"
#include "gsmd_select.h"
#include "usock.h"
#include "gsmdsms.h"
#include "unsolicited.h"
#include "SeqQueue.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "app_shell.h"
#include "hardWare_Resource.h"

#define GSMD_ALIVECMD		"AT"
#define GSMD_ALIVE_INTERVAL	10	//5*60
#define GSMD_ALIVE_TIMEOUT	3	//30

#define	GSMCDMA_RECVBUF_SIZE	20480//2048

#define MDM_SLP_ENABLE_ELAPSE_T	50		//50*10ms=500ms
#define PowerBBReset_Port		PORT_B
#define PowerBBReset_Bit		BIT11

static struct gsmd g;

tSeqQueue 	gsmcdma_uart_recvque;							//gsmcdma_server接收来自gsm/cdma adapter的信息
char		gsmcdma_uart_recvbuf[GSMCDMA_RECVBUF_SIZE];

char		gsmmodever[50];
static u_int8_t  mdm_init_stat,mdm_sleep_enable;
static u_int32_t mdm_sleep_enable_stime;

char		gsm_mode_state;//GSM模块是否出现异常,例如AT不响应  1：出现异常  0：未出现异常

extern void	hyhwGsm_BBreset();

extern int gsmcdma_server_exit;
extern SEMAPHORE *gsmcdma_server_exit_sem;

void modem_sleep_disable(void);
void modem_sleep_enable(void);
int gsmcdma_uart_init(u_int8_t reset);
void gsmcdma_adapter_poweronoff(U8 onoff);



char gsm_getmodestate()
{
	return gsm_mode_state;
}


char *gsm_getmodever()
{
	return gsmmodever;
}

static void alive_test_timeout(struct gsmd_timer *tmr, void *data)
{
    struct gsmd *g=data;
    struct gsmd_atcmd *cmd=NULL;
    
#ifdef _GSMD_DEBUG_LOG_2
    struct timeval exp, *exp1;

#endif


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

#if 1
	//DEBUGP("gsmd_alive timer expired\n");
	if (g->alive_responded == 0) 
	{
		/*
		gsmd_log(GSMD_FATAL, "modem dead!\n");
		while(1);	//便于发现问题，需要修改成复位模块
		exit(3);
		*/
		
		struct gsmd_user *gu;


		//destory all cmds
		llist_for_each_entry(gu, struct gsmd_user, &g->users, list) 
		{
			/* finish pending atcmd's from this client thus
			 * destroying references to the user structure.  */
			atcmd_terminate_matching(g, gu);
			gprsframe_terminate_matching(g, gu);
		}

		//对模块的测试结果为模块无响应，复位模块
		//rePowerGsm();//kong 2011-02-09
		//gsmcdma_adapter_poweronoff(1);//zd
		if(gsm_mode_state == 0)
		{
			gsm_mode_state = 1;
			g->interpreter_ready = -1;
		}
	}
	else
	{
		//gsmd_log(GSMD_INFO, "modem alive!\n");
	}

#endif
	// pass the next pending at command
	if (llist_empty(&g->busy_atcmds)) {
	
		if(!llist_empty(&g->pending_atcmds))
			atcmd_wake_pending_queue(g);
		else if(0==g->net_gprs)
			modem_sleep_enable();
	}
}



static struct gsmd_timer * alive_test_timer(struct gsmd *g)
{
 	struct timeval tv;
 	
	tv.tv_sec = GSMD_ALIVE_TIMEOUT;
	tv.tv_usec = 0;

	return gsmd_timer_create(&tv, &alive_test_timeout, g);
}


/* alive checking
 * either OK or ERROR is allowed since, both mean the modem still responds
 */

static int alive_test_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd *g = ctx;

	if (!strcmp(resp, "OK") || !strcmp(resp, "ERROR") || !strcmp(resp, "ABORTED") ||
	    ((g->flags & GSMD_FLAG_V0) && resp[0] == '0'))
		g->alive_responded = 1;
	return 0;
}

extern enum shell_init_step init_step;
int gsmd_modem_alive_test(struct gsmd *g)
{
	struct gsmd_atcmd *cmd;
	static U8 timeout = 0;
	g->alive_responded = 0;
//hyUsbPrintf("gsmd_modem_alive_test \r\n");
	cmd = atcmd_fill("AT", 2+1, &alive_test_cb, g, 0, alive_test_timer);
	if (!cmd) 
		return -ENOMEM;

	llist_add(&cmd->list, &g->pending_atcmds);
	if (llist_empty(&g->busy_atcmds) &&
		!llist_empty(&g->pending_atcmds)) {

		atcmd_wake_pending_queue(g);

		if (0==g->net_gprs)
			modem_sleep_disable();
	}

	if(net_onlyRead_regStatus() == 1 && gsm_mode_state == 0)//GSM模块有AT未响应
	{
		if(init_step == STEP_NET_REGISTER)//注册时加长
		{
			timeout++;
			if(timeout >= 4)
			{
				timeout = 0;
				//hyUsbPrintf("gsm err!!!! reset\r\n");
				gsm_mode_state = 1;
			}
		}
		else
		{
			timeout = 0;
			//hyUsbPrintf("gsm err!!!! reset\r\n");
			gsm_mode_state = 1;
		}
	}
	
	//return atcmd_submit(g, cmd);
}



/* initial startup code */
static int gsmd_test_atcb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	//DEBUGP("`%s' returned `%s'\n", cmd->buf, resp);
	//hyUsbPrintf("+++++++++  %s\r\n", resp);
	return 0;
}

static int gsmd_get_imsi_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd *g = ctx;

	//DEBUGP("imsi : %s\n", resp);
	strlcpy(g->imsi, resp, sizeof(g->imsi));

	return 0;
}


static int gsmd_get_imei_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd *g = ctx;

	//+EGMR: "135790246811220"
	if(!strncmp(resp, "+EGMR:", 6))
	{
		char imei[20];
		
		resp+=8;
		sscanf(resp, "%[0-9]", imei);
//		PhoneTrace(0, "%s", imei);
	}

	return 0;
}


static int gsmd_get_revision_cb(struct gsmd_atcmd *cmd, void *ctx, char *resp)
{
	struct gsmd *g = ctx;

	//+CGMR: MAUI.07B.W08.12, 2009/02/27 16:09
	/*if(!strncmp(resp, "+CGMR:", 6))
	{
		resp+=7;
//		PhoneTrace(0, "%s", resp);
		while(1);
	}*/
	memset(gsmmodever,0,50);
	strcpy(gsmmodever,resp);

	return 0;
}


int gsmd_simplecmd(struct gsmd *gsmd, char *cmdtxt)
{
	struct gsmd_atcmd *cmd;
	cmd = atcmd_fill(cmdtxt, strlen(cmdtxt)+1, &gsmd_test_atcb, NULL, 0, NULL);
	if (!cmd)
		return -ENOMEM;

	return atcmd_submit(gsmd, cmd);
}




int gsmd_initsettings2(struct gsmd *gsmd, int reset)
{
	
	int rc = 0;

//	sleep(100);
	rc |= gsmd_simplecmd(gsmd, "AT");

	//rc |= gsmd_simplecmd(gsmd, "AT&F");

	/* AT+IPR Set TE-TA Fixed Local Rate */
	//rc |= gsmd_simplecmd(gsmd, "AT+IPR=115200");
	//rc |= gsmd_simplecmd(gsmd, "AT+IPR=0");
	/* echo on, verbose */
	rc |= gsmd_simplecmd(gsmd, "ATE0");

    
#ifndef GSM_MODULE_ULC2
	/* AT+GMR Request TA Revision Identification Of Software Release */
	rc |= gsmd_simplecmd(gsmd, "AT+GMR");
#endif

	/* AT+GSN Request TA Serial Number Identification (IMEI) */
	//rc |= gsmd_simplecmd(gsmd, "AT+GSN");

	/* AT+CNUM */
	//rc |= gsmd_simplecmd(gsmd, "AT+CNUM");

	/* use +CRING instead of RING */
	rc |= gsmd_simplecmd(gsmd, "AT+CRC=1");
	/* enable +CREG: unsolicited response if registration status changes */
	rc |= gsmd_simplecmd(gsmd, "AT+CREG=0");
	/* use +CME ERROR: instead of ERROR */
	rc |= gsmd_simplecmd(gsmd, "AT+CMEE=1");
	/* use +CLIP: to indicate CLIP */
	rc |= gsmd_simplecmd(gsmd, "AT+CLIP=1");
	/* use +COLP: to indicate COLP */
	/* set it 0 to disable subscriber info and avoid cme err 512 */
	rc |= gsmd_simplecmd(gsmd, "AT+COLP=0");
	/* use +CCWA: to indicate waiting call */
	//rc |= gsmd_simplecmd(gsmd, "AT+CCWA=1,1");

	/* automatic answering is disable */
	//rc |= gsmd_simplecmd(gsmd, "ATS0=0");

	/* configure message format as PDU mode*/
	/* FIXME: TEXT mode support!! */
	//rc |= gsmd_simplecmd(gsmd, "AT+CMGF=0");
	/* reueset imsi */
	//atcmd_submit(gsmd, atcmd_fill("AT+CIMI", 7+1,
	//				&gsmd_get_imsi_cb, gsmd, 0, NULL));

	#if 0
	atcmd_submit(gsmd, atcmd_fill("AT+EGMR=0,7", 11+1,
					&gsmd_get_imei_cb, gsmd, 0, NULL));
	
	atcmd_submit(gsmd, atcmd_fill("AT+CGMR", 7+1,
					&gsmd_get_revision_cb, gsmd, 0, NULL));
	#endif
	
	//kong
	atcmd_submit(gsmd, atcmd_fill("ATI", 3+1,
					&gsmd_get_revision_cb, gsmd, 0, NULL));
	

#ifdef GSM_MODULE_SIM300

	/* AT+CSCLK Configure Slow Clock */
	rc |= gsmd_simplecmd(gsmd, "AT+CSCLK=0");

	/* Change The Microphone Gain Level */
	rc |= gsmd_simplecmd(gsmd, "AT+CMIC=0,5");
	rc |= gsmd_simplecmd(gsmd, "AT+CMIC=1,5");
	
	/* Swap The Audio Channels, 
	0 - Normal audio channel(default),	1 - Aux audio channel */
	rc |= gsmd_simplecmd(gsmd, "AT+CHFA=0");
	/* Loud Speaker Volume Level */
	rc |= gsmd_simplecmd(gsmd, "AT+CLVL=90");

#elif defined(GSM_MODULE_IW328)

	/* Set Audio Mode:0-normal,1-handset, 2-loud speaker */
	rc |= gsmd_simplecmd(gsmd, "AT+ESAM=0");

	/* Set Audio Gain Value */
	rc |= gsmd_simplecmd(gsmd, "AT+ESLT=4,255");

#elif defined(GSM_MODULE_MD231)

	/* Set audio output mode: 0-normal mode, 1-PCM mode */
	rc |= gsmd_simplecmd(gsmd, "AT+EPCM=1");

#elif defined(GSM_MODULE_ULC2)

	rc |= gsmd_simplecmd(gsmd, "AT+XDRV=0,0,4,2");
	rc |= gsmd_simplecmd(gsmd, "AT+XDRV=0,1,60,1");
	//rc |= gsmd_simplecmd(gsmd, "AT+XDRV=0,6");
	//rc |= gsmd_simplecmd(gsmd, "AT+XDRV=0,18");
	rc |= gsmd_simplecmd(gsmd, "AT+CLVL=60");//select the volume of the internal loudspeaker of the ME

#endif

#if defined(GSM_MODULE_IW328)||defined(GSM_MODULE_MD231)

	//rc |= gsmd_simplecmd(gsmd, "AT+EIND?");

	/* sleep enable:1-enable, 0-disable */
	rc |= gsmd_simplecmd(gsmd, "AT+ESLP=1");
#endif

	//rc |= gsmd_simplecmd(gsmd, "AT+CSCS=\"UCS2\"");

	sms_cb_init(gsmd, reset);
	
	return rc;
}



static int gsmd_initialize(struct gsmd *g)
{
	INIT_LLIST_HEAD(&g->users);

	g->mlbuf = (u_int8_t*)MALLOC(MLPARSE_BUF_SIZE);
	if (!g->mlbuf)
		return -ENOMEM;

	return 0;
}


void gsmd_uninitialize(struct gsmd *g)
{
	FREE(g->mlbuf);
	g->mlbuf = NULL;
}



/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_uart_write()
* 功能:	将信息发送给GSM/CDMA模块
* 参数:	
* 返回:	i 发送的字节数
*----------------------------------------------------------------------------*/
int gsmcdma_uart_write(int fd, char *txbuf, int len)
{
	int i;
	U8 uartId = UART0;
//	hyUsbPrintf("\r\n ~~UartWrite:");
//	hyUsbMessageByLen(txbuf,len);
	
	for(i=0; i<len; i++)
	{
		hyhwUart_WriteFifo((u_int8_t)txbuf[i],uartId);
	}
	
	return i;
}



/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_uart_read()
* 功能:	将接收到的信息从buf中读出
* 参数:	
* 返回:	读出的字节数
*----------------------------------------------------------------------------*/
int gsmcdma_uart_read(int fd, char *rxbuf, int len)
{
	u_int8_t  IntValue;
	u_int32_t rp,wp;
	tSeqQueue *Que=&gsmcdma_uart_recvque;
	char *buf;
	int num,size;

	IntValue = InterruptDisable();	//禁止中断
	rp=Que->front;
	wp=Que->rear;
	size=Que->size;
	buf=Que->element;
	InterruptRestore(IntValue);

	if(rp==wp)
	{
		len=0;
	}
	else
	{
		if(rp<wp)
		{
			num=wp-rp;
			if(len>num)
				len=num;
			memcpy(rxbuf,(char*)(buf+rp),len);
			rp+=len;
		}
		else //if(rp>wp)
		{
			num=size-(rp-wp);
			if(len>num)
			{
				len=num;
				num=size-rp;
				memcpy(rxbuf,(char*)(buf+rp),num);
				memcpy((char*)(rxbuf+num),buf,wp);
				rp=wp;
			}
			else //if(len<=num)
			{
				if(len<size-rp)
				{
					memcpy(rxbuf,(char*)(buf+rp),len);
					rp+=len;
				}
				else //if(len>=size-rp)
				{
					num=size-rp;
					memcpy(rxbuf,(char*)(buf+rp),num);
					memcpy((char*)(rxbuf+num),buf,len-num);
					rp=len-num;
				}
			}
		}
		
		IntValue = InterruptDisable();	//禁止中断
		Que->front=rp;
		InterruptRestore(IntValue);
	}
	if(fd == 3)
	{
		//hyUsbPrintf("~~uart read =%s",rxbuf);	
	}
	return len;
}


/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_uart_isr
* 功能:	uart(与GSM/CDMA模块通信的uart)的中断服务函数
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void gsmcdma_uart_isr(void)
{
#if 1//kong 2011-08-22
	u_int8_t i,data[20],len;
	U8 uartId = UART0;
	
	len = 0;
	for (i=0;i<16; i++)
	{
		if(HY_OK==hyhwUart_CheckRecDataReady(uartId))
		{
			hyhwUart_ReadFifo(&data[len++],uartId);	//接收数据
		}
		else
		{
			break;
		}
	}
	
	if(len > 0)
	{
		EnterQueue(&gsmcdma_uart_recvque, data, len);
	}
#else
	u_int8_t i,data;
	U8 uartId = UART0;
	

	for (i=0;i<16; i++)
	{
		if(HY_OK==hyhwUart_CheckRecDataReady(uartId))
		{
			hyhwUart_ReadFifo(&data,uartId);	//接收数据

			EnterQueue(&gsmcdma_uart_recvque, (char)data);
		}
		else
		{
			break;
		}
	}
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	void hyhwGsm_powerOff(void)
* 功能:	关闭 GSM 模块电源
* 参数:	none
*----------------------------------------------------------------------------*/
void hyhwGsm_powerOff(void)
{
	hyhwGpio_setAsGpio(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
	hyhwGpio_setOut(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
	hyhwGpio_setLow(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);

}

/*----------------------------------------------------------------------------
* 函数:	hyhwGsm_powerDown
* 功能:	关闭电源和设置所有的IO为低，避免漏电流
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwGsm_powerDown(void)
{
	hyhwGpio_setAsGpio(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_setIn(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_pullDisable(GSM_DET_PORT, GSM_DET_GPIO);

	hyhwGpio_setAsGpio(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	hyhwGpio_setOut(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	//hyhwGpio_setLow(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	//sleep(250);
	hyhwGpio_setHigh(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	
	hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setOut(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setLow(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	//syssleep(20);
	
	hyhwGsm_powerOff();
	
	hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setIn(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	
//下面是在GSM被关闭后对所有和GSM 相关的IO设置为低。防止漏电流
	//uart_in
/*
	2009-06-11 G22 delete function	
	hyhwGpio_setAsGpio(UART1_DCD_Port,UART1_DCD_Bit);
	hyhwGpio_setIn(UART1_DCD_Port,UART1_DCD_Bit);
*/
	hyhwGpio_setAsGpio(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);
	hyhwGpio_setIn(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);

	hyhwGpio_setAsGpio(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
	hyhwGpio_setIn(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
	hyhwGpio_setIn(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
	hyhwGpio_setIn(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	hyhwGpio_setIn(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
	hyhwGpio_setIn(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
}

/*-----------------------------------------------------------------------------
* 函数:	void hyhwGsm_powerOn(void)
* 功能:	打开 GSM 模块电源
* 参数:	none 和电路有关系，模拟一个缓慢电容充电,GSM缓慢上电的一个过程
*----------------------------------------------------------------------------*/
void hyhwGsm_powerOn(void)
{
	int i, j, delay;
	volatile U32 *gsmPower_Port;
	
	gsmPower_Port = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (GSM_POWER_ON_PORT*sizeof(tGPIO_REGISTER)));
	hyhwGpio_setAsGpio(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
	hyhwGpio_setOut(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
	
	i = 80;
	j = 0;
	delay = 0;
	while(i--)
	{
		//hyhwGpio_setHigh(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
		(*gsmPower_Port) |= GSM_POWER_ON_GPIO;
		while(delay--);
		//hyhwGpio_setLow(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
		(*gsmPower_Port) &= ~GSM_POWER_ON_GPIO;
		//syssleep(1);
		hyhwNand_delayUs(300);
		j++;
		delay = j;
	}
	hyhwGpio_setHigh(GSM_POWER_ON_PORT,GSM_POWER_ON_GPIO);
	syssleep(10);	//驱动集成时打开
}

/*-----------------------------------------------------------------------------
* 函数:	void hyhwGsm_powerKey(void)
* 功能:	模拟 GSM 模块开关按键
* 参数:	none
*----------------------------------------------------------------------------*/
void hyhwGsm_powerKey(void)
{
	U32 i;
	
	hyhwGpio_setAsGpio(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	hyhwGpio_setOut(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	hyhwGpio_setLow(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	syssleep(250);	//驱动集成时打开		//PWRKEY>2000ms
	hyhwGpio_setHigh(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
}
/*-----------------------------------------------------------------------------
* 函数:	void hyhwGsm_powerKey(void)
* 功能:	模拟 GSM 模块开关按键
* 参数:	none
*----------------------------------------------------------------------------*/
void hyhwGsm_BBreset(void)
{

	hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setOut(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setLow(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	syssleep(20);	//驱动集成时打开
	//hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	//hyhwGpio_setOut(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setHigh(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	syssleep(2);
	hyhwGpio_setIn(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setPullUp(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
}
/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_PortPowerSave
* 功能:	初始化uart端口PowerSave
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_PortPowerSave(void)
{
//uart_in
/*
	2009-06-11 G22 delete function	
	hyhwGpio_setAsGpio(UART1_DCD_Port,UART1_DCD_Bit);
	hyhwGpio_setIn(UART1_DCD_Port,UART1_DCD_Bit);
*/
	hyhwGpio_setAsGpio(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);
	hyhwGpio_setIn(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);

	hyhwGpio_setAsGpio(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
	hyhwGpio_setIn(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);

	hyhwGpio_setAsGpio(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
	hyhwGpio_setIn(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
	
//uart_out
 	hyhwGpio_setAsGpio(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
	hyhwGpio_setOut(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
	hyhwGpio_setLow(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
	hyhwGpio_setOut(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
	hyhwGpio_setLow(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
	
	hyhwGpio_setAsGpio(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	hyhwGpio_setOut(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	hyhwGpio_setLow(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	
	hyhwGpio_setAsGpio(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_setIn(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_pullDisable(GSM_DET_PORT, GSM_DET_GPIO);
	
	hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setOut(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	hyhwGpio_setHigh(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
	
	hyhwGpio_setAsGpio(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	hyhwGpio_setOut(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	hyhwGpio_setLow(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwUart_InitPort
* 功能:	初始化uart端口
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_InitPort(void)
{
// 	hyhwGpio_setAsFunction(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
//	hyhwGpio_setAsFunction(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhw_GsmUartClose
* 功能:	GSM模块的UART关闭(仅供通信接口层调用)
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhw_GsmUartClose()
{
	U8 uartid=UART0;

	hyhwUart_rxIntDisable(uartid);
	//hyhwInt_disable(INT_BIT_UART);
	hyhwInt_ConfigUart0_ISR(NULL);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhw_GsmModemCtrlGpioInit
* 功能:	初始化GSM模块串口流控(仅供通信接口层调用)
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhw_GsmModemCtrlGpioInit(void)
{
	hyhwGpio_setAsGpio(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);
	hyhwGpio_setOut(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);
	//hyhwGpio_setLow(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);//禁止GSM模块进入睡眠
	hyhwGpio_setHigh(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);
	/*	串口CTS GSM模块流控制输入
		CTS保持高电平，通知GSM模块系统不接收数据。
		CTS保持低电平，通知GSM模块系统可以接收数据。*/
	// 8300端RTS
	hyhwGpio_setAsGpio(GSM_UART0_CTS_PORT, GSM_UART0_CTS_GPIO);
	hyhwGpio_setOut(GSM_UART0_CTS_PORT, GSM_UART0_CTS_GPIO);
	hyhwGpio_setLow(GSM_UART0_CTS_PORT, GSM_UART0_CTS_GPIO);
	
	/*	串口RTS GSM模块流控制输出
		RTS输出高电平，GSM模块不接收系统发来的数据。
		RTS输出低电平，GSM模块可以接收系统发来的数据。*/
	hyhwGpio_setAsGpio(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
	hyhwGpio_setIn(GSM_UART0_RTS_PORT, GSM_UART0_RTS_GPIO);

	/*	串口RI */
	hyhwGpio_setAsGpio(GSM_UART0_RI_PORT, GSM_UART0_RI_GPIO);
	hyhwGpio_setIn(GSM_UART0_RI_PORT, GSM_UART0_RI_GPIO);
}

/*-----------------------------------------------------------------------------
* 函数:	hyhw_GsmUartInit
* 功能:	GSM模块的UART初始化(仅供通信接口层调用)
* 参数:	UART中断函数，是否重启
* 返回:	通信接口层FD
*----------------------------------------------------------------------------*/
int hyhw_GsmUartInit( void(*isr)(void), U8 reset )
{
	U8 uartid=UART0;
	
	hyhwGpio_setUart0Function(0);

	hyhwUart_Init(Uart_Baudrate_115200,Uart_DataBits_8,Uart_StopBits_1,Uart_Parity_Disable, Uart_Fifo_Mode,uartid, 0,0,0);
 
 	if(0 == reset)
 	{
	 	hyhwInt_ConfigUart0_ISR(isr);
		hyhwUart_rxIntEnable(uartid);
	    hyhwInt_setIRQ(INT_BIT_UART);
	    hyhwInt_enable(INT_BIT_UART);

		//用timer0扫描uart1的fifo，以便获取uart1 fifo中剩余的数据

		hyhwInt_ConfigTimer0_ISR(uartid, isr);

	}
	
	/*	串口DTR 设置GSM模块进入或退出sleep模式
		DTR信号引脚为高电平，模块在没有其它事务处理回自动进入sleep模式
		DTR引脚拉成低电平，该信号把SIM300从SLEEP模式退出	*/
	hyhw_GsmModemCtrlGpioInit();

    return 5;
}

/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_uart_init(u_int8_t reset)
* 功能:	与GSM/CDMA模块通信的uart初始化
* 参数:	none
* 返回:	uartid
*----------------------------------------------------------------------------*/
int gsmcdma_uart_init(u_int8_t reset)  //UART初始化
{
	int rv;
	
	rv = hyhw_GsmUartInit(gsmcdma_uart_isr,reset);
	
	return rv;
}

/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_uart_close(void)
* 功能:	与GSM/CDMA模块通信的uart关闭
* 参数:	none
* 返回:	uartid
*----------------------------------------------------------------------------*/
void gsmcdma_uart_close(void)
{
	hyhw_GsmUartClose();
}

/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_adapter_poweronoff
* 功能:	开启/关闭GSM/CDMA模块
* 参数:	onoffs	--	0:关闭	1:开启
* 返回:	none
*----------------------------------------------------------------------------*/
void gsmcdma_adapter_poweronoff(U8 onoff)
{
	static U8 gsmstate = 0;

//hyUsbPrintf("onoff == %d   %d\r\n",gsmstate,onoff);	
	if(gsmstate == onoff)
	{
		return ;
	}
	gsmstate = onoff;
	
	hyhwGpio_setAsGpio(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_setIn(GSM_DET_PORT, GSM_DET_GPIO);
	hyhwGpio_pullDisable(GSM_DET_PORT, GSM_DET_GPIO);
	
	if(0==onoff)
	{
		hyhwGpio_setAsGpio(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		hyhwGpio_setOut(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		hyhwGpio_setLow(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		sleep(250);
		hyhwGpio_setHigh(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		
		hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
		hyhwGpio_setOut(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
		hyhwGpio_setLow(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
		syssleep(20);
		hyhwGpio_setAsGpio(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
		hyhwGpio_setIn(GSM_BB_RESET_PORT,GSM_BB_RESET_GPIO);
		
		hyhwGsm_powerOff();
		
		hyhwGpio_setAsGpio(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);
		hyhwGpio_setIn(GSM_UART0_DSR_PORT,GSM_UART0_DSR_GPIO);
		
		hyhwGpio_setAsGpio(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
		hyhwGpio_setIn(GSM_UART0_RI_PORT,GSM_UART0_RI_GPIO);
		
		hyhwGpio_setAsGpio(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
		hyhwGpio_setIn(GSM_UART0_RXD_PORT,GSM_UART0_RXD_GPIO);
		
		hyhwGpio_setAsGpio(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
		hyhwGpio_setIn(GSM_UART0_TXD_PORT,GSM_UART0_TXD_GPIO);
		
		hyhwGpio_setAsGpio(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
		hyhwGpio_setIn(GSM_UART0_RTS_PORT,GSM_UART0_RTS_GPIO);
		
		hyhwGpio_setAsGpio(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
		hyhwGpio_setIn(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
	}
	else
	{
		hyhwGsm_powerOn();
		
		hyhwGsm_BBreset();
		hyhwGpio_setAsGpio(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		hyhwGpio_setOut(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		hyhwGpio_setLow(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
		sleep(250);
		hyhwGpio_setHigh(GSM_SWITCH_ON_PORT,GSM_SWITCH_ON_GPIO);
	}

}


u_int8_t modem_rcv_check(void)
{
#ifdef GSMCDMA_MODULE

	/*	串口RTS GSM模块流控制输出
		RTS输出高电平，GSM模块不接收系统发来的数据。
		RTS输出低电平，GSM模块可以接收系统发来的数据。*/
	if(0 == hyhwGpio_Read(GSM_UART0_RTS_PORT, GSM_UART0_RTS_GPIO))
		return 1;
	else
#endif
		return 0;
}

//1---powerOn, 0---powerDown
int modem_get_ispowerOn(void)
{
	if(1 == hyhwGpio_Read(GSM_DET_PORT, GSM_DET_GPIO))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int modem_get_sleep_enable(void)
{
	if(0 == hyhwGpio_Read(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO))
	{
		//允许进入睡眠
		return 1;
	}
	else
	{
		//禁止进入睡眠
		return 0;
	}
}

void modem_sleep_disable(void)
{
	mdm_sleep_enable = 0;
	
#ifdef GSMCDMA_MODULE
	if(0 == hyhwGpio_Read(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO))
	{
		/* 唤醒基带模块 */
		hyhwGpio_setHigh(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);
		sleep(5);
	}
#endif
}



void modem_sleep_enable(void)
{
	if(1 != mdm_sleep_enable)
	{
		mdm_sleep_enable = 1;
		mdm_sleep_enable_stime = rawtime(NULL);
	}
}


/*-----------------------------------------------------------------------------
* 函数:	gsmd_modem_sleep_ctrl()
* 功能:	
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void modem_sleep_ctrl(void)
{
	u_int32_t cur_time = rawtime(NULL);
// zhaozheng
#if 1
	if(1==mdm_init_stat && 1==mdm_sleep_enable && ABS(cur_time-mdm_sleep_enable_stime)>MDM_SLP_ENABLE_ELAPSE_T)
	{
	#ifdef GSMCDMA_MODULE
		if(0 != hyhwGpio_Read(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO))
		{
			hyhwGpio_setLow(GSM_UART0_DSR_PORT, GSM_UART0_DSR_GPIO);
		}
	#endif
	}
#endif
}

extern int mdm_reg_stat;

/*-----------------------------------------------------------------------------
* 函数:	modem_init_ok()
* 功能:	
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void modem_init_ok(void)
{
	mdm_init_stat = 1;
	mdm_reg_stat = 1;//为了使开机一注册上就能够打电话
}
void modem_init_reset(void)
{
	mdm_init_stat = 0;
}

/*-----------------------------------------------------------------------------
* 函数:	get_modem_init_stat()
* 功能:	
* 参数:	
* 返回:	0:初始化未完成	1:初始化已完成
*----------------------------------------------------------------------------*/
u_int8_t get_modem_init_stat(void)
{
	return mdm_init_stat;
}
u_int8_t get_modem_reg_stat(void)
{
	return mdm_reg_stat;
}


/*-----------------------------------------------------------------------------
* 函数:	gsmcdma_server
* 功能:	gsmcdma server 主函数
* 参数:	
* 返回:	none
*----------------------------------------------------------------------------*/
void gsmcdma_server(void)
{
	int fd;
	U32 ticks=0;	

#ifdef _GSMD_DEBUG_LOG
	gsmd_debug_init();
#endif

	gsm_mode_state = 0;
	
	mdm_init_stat = 0;
	
	
	InitQueue(&gsmcdma_uart_recvque,gsmcdma_uart_recvbuf,sizeof(gsmcdma_uart_recvbuf));
	gsmcdma_adapter_poweronoff(1);
	fd=(int)gsmcdma_uart_init(0);
	
if(0)
{
	char a = 'a';
	while(a < 'z')
	{
		hyhwUart_WriteFifo(a,UART0);
	}
	
}
	memset(&g, 0, sizeof(struct gsmd));
	gsmd_fd_init();
	gsmd_timer_init();

	if (gsmd_initialize(&g) < 0) {
		//gsmd_log("gsmd_initialize error\n");
		return;
	}


	g.interpreter_ready = 0;

	if (atcmd_init(&g, fd) < 0) {
		//gsmd_log("atcmd_init error\n");
		return;
	}
    //gsmcdma_uart_write(fd,"\r",2);	//匹配baudrate吗？


	if (usock_init(&g) < 0) {
		//gsmd_log("can't open unix socket\n");
		return;
	}

 //   PhoneTrace(0,"begin");

	unsolicited_init(&g);
	gsmd_opname_init(&g);

	while (1) {

#ifdef _GSMD_DEBUG_LOG
{

	static int cnt_g1 = 0;
	
	cnt_g1++;
	if(200 <= cnt_g1)
	{
		//hyUsbMessage("g server run\r\n");
//				PhoneTrace(0,"g server run");

		//hyUsbPrintf("\r\nfctl_flag=%d\r\n", g.muxchl_fctl_flag);
		cnt_g1 = 0;
	}
}
#endif
		
		if(ABS(rawtime(NULL)-ticks) >= 1000)
		{
			ticks = rawtime(NULL);
			hyUsbMessage("gsm S run\r\n");
		}
		
		gsmd_select_main(&g);
		gsmd_timer_check_n_run();
		modem_sleep_ctrl();
		//gprs_detect_sms_call(&g);

		if(1 == gsmcdma_server_exit)
			break;

		sleep(2);
	}

#ifdef _GSMD_DEBUG_LOG
	gsmd_debug_stop();
#endif
	/*{
    char buf2[] = {0xf9,0x0B,0x53,0x01,0xB8,0xf9};
    gsmcdma_uart_write(fd, buf2, 6);
    syssleep(50);
    }
	{char buf1[] = {0xf9,0x07,0x53,0x01,0x3f,0xf9};
    gsmcdma_uart_write(fd, buf1, 6);
    syssleep(50);
    }
    {char buf0[] = {0xf9,0x03,0x53,0x01,0xfd,0xf9};
    gsmcdma_uart_write(fd, buf0, 6);
    syssleep(50);
    }*/
    //PhoneTrace(0,"close");
//hyUsbMessage("uart_close\r\n");
	gsmcdma_uart_close();
//hyUsbMessage("adapter_close\r\n");
	gsmcdma_adapter_poweronoff(0);
	//hyhwGsm_powerDown();
//hyUsbMessage("uninit 000\r\n");	
	gsmd_opname_uninit(&g);
//hyUsbMessage("uninit 111\r\n");
	usock_uninit(&g);
//hyUsbMessage("uninit 222\r\n");
	atcmd_uninit(&g);
//hyUsbMessage("uninit 333\r\n");
	gsmd_uninitialize(&g);
//hyUsbMessage("semaphore_post\r\n");
	semaphore_post(gsmcdma_server_exit_sem);
	_exit(1);
}
void lgsm_modem_reset(void)
{
	return;
}