/******************** (C) COPYRIGHT 2010 hyco ********************
* File Name          : receive_barcode.c
* Author             : W302 Application Team
* Version            : V1.0
* Date               : 06/18/2010
* Description        : receive  binary barcode  data from scanner and decode
*******************************************************************************/

/*
	使用两个外部中断，一个timer用于计时，暂定使用1us计数
		1. frame中断：标志一帧开始，启动data中断，整理管理结构及buf；
					下一次中断时，记录当前当前帧的数据
		2. data中断：标志数据到达，读取data电平，记录timer计数(即电平持续时间)；
					注意：每次data中断记录的是上一次电平及电平持续时间，
							所以电平必须取反
	
	关于data电平的畸变，约靠近frame信号边缘，data畸变值越大，
		修正畸变时，必须考虑data电平位置与frame信号的距离
		扫描头与条码间的距离也会影响畸变值
		
*/


/* Includes ------------------------------------------------------------------*/
#include "hyTypes.h"

#include "hyhwGPIO.h"
#include "hyhwTimer.h"
#include "hyhwIntCtrl.h"

#include "barcode_api.h"
#include "scanner.h"
#include "hardWare_Resource.h"

//#define TEST_DECODE
#define BAR_DECODE_MULTI_1

//#define HONYWELL_MODULE			/* honywell的扫描光头模块 */

//#define TEST_WRITE

#include "AbstractFileSystem.h"
#ifdef TEST_WRITE
#define BAR_FILE	"C:/bar.txt"
int gbarfp;

#endif


/* 预定义 */
//#define TEST_GPIO
#ifdef TEST_GPIO
	#define TESTSCAN_PORT	PORT_E//PORT_F//
	#define TESTSCAN_GPIO	GPIO_12//GPIO_31//
#endif
/* IO及控制定义 */

#define BARAPP_READ_FRAME			hyhwGpio_Read(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE)
#define BARAPP_READ_RAW_DATA		hyhwGpio_Read(SCANNER_DATA_PORT, SCANNER_DATA_LINE)

#define	BARAPP_LASER_ENABLE			hyhwGpio_setHigh(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE)
#define	BARAPP_LASER_DISABLE		hyhwGpio_setLow(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE)

#define	BARAPP_PENDULUM_ENABLE		hyhwGpio_setHigh(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE)
#define	BARAPP_PENDULUM_DISABLE		hyhwGpio_setLow(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE)



/* 中断控制预定义 */
#define BARAPP_FRAME_EXTINT_LINE		INT_BIT_EI0		//FRAME使用的是 GPIO_C30
#define BARAPP_FRAME_EXTINT_ENABLE		hyhwInt_enable(BARAPP_FRAME_EXTINT_LINE)
#define BARAPP_FRAME_EXTINT_DISABLE		hyhwInt_disable(BARAPP_FRAME_EXTINT_LINE)
#define BARAPP_FRAME_EXTINT_CLEAR		hyhwInt_clear(BARAPP_FRAME_EXTINT_LINE)

#define BARAPP_DATA_EXTINT_LINE			INT_BIT_EI1		//DATA使用的是GPIO_A2
#define BARAPP_DATA_EXTINT_ENABLE		hyhwInt_enable(BARAPP_DATA_EXTINT_LINE)
#define BARAPP_DATA_EXTINT_DISABLE		hyhwInt_disable(BARAPP_DATA_EXTINT_LINE)
#define BARAPP_DATA_EXTINT_CLEAR		hyhwInt_clear(BARAPP_DATA_EXTINT_LINE)

#define BARAPP_TIMER_BASE				5
#define BARAPP_TIMER_ENABLE				hyhwTimer_countEnable(BARAPP_TIMER_BASE)
#define BARAPP_TIMER_DISABLE			hyhwTimer_countDisable(BARAPP_TIMER_BASE)
#define BARAPP_TIMER_COUNTER_CLEAR		hyhwTimer_setConter(BARAPP_TIMER_BASE, 0)
#define BARAPP_TIMER_COUNTER_READ(pCount)		hyhwTimer_readConter(BARAPP_TIMER_BASE, pCount)
#define BARAPP_TIMER_LOADVALUE			(30000)		/*	30ms TIMER计数范围，
														现假定一个frame不会超过30ms
													*/

#define BARAPP_INT_ALL_ENABLE		
#define BARAPP_INT_ALL_DISABLE		


#define BARAPP_FARME_STABLE_IDX		1//4//  /* 认为可以稳定的帧，需要实测 */

#define BARAPP_MIN_SUCCESS_NUM		1//2//
#define BARAPP_LASER_LIGHT_TIME		300	/* 激光点亮最长时间，3秒，10ms为单位 */

#define BAR_CODE_DATA_MAX_LEN		(512)		/* 接收data的buf最大长度(S16) */
#define BARDATA_SIZE				(2)			/* 一个数据的字节数 */
#define BARDATA_BUF_NUM				(2)//(5)//			/* 接收条码数据buf的个数 */

#define BARDATA_VALID_NUM_MIN		20	/* 可解码的有效数据最低阀值 */
#define BARDATA_VALID_NUM_MAX		(BAR_CODE_DATA_MAX_LEN-50)	/* 可解码的有效数据最高阀值 */

#define BARDATA_MAX_LEN				(48)//(100)

#define SCAN_DIRECTION_POSITIVE		1


/* 接收buf状态的枚举 */
typedef enum bardata_buf_state_en
{
	BARDATA_BUF_IDLE = 0,				/* 空闲状态，buf内数据无用 */
	BARDATA_BUF_RECEIVING,				/* 接收状态，buf内数据不能用于解码 */
	BARDATA_BUF_RECEIVE_OK,				/* 接收完成状态，buf内数据可用于解码 */
	BARDATA_BUF_DECODING				/* 解码状态，buf内数据正在用于解码
											此时，不可用于接收新的数据
											解码完成后，处于IDLE状态
										*/
}BARDATA_BUF_STATE_en;

/* bar数据结构 */
typedef struct bardata_buf_st
{
	S16	data[BAR_CODE_DATA_MAX_LEN];	/* 扫描数据buf */
	U8	scanDirect;						/* 扫描方向：0---正向，1---反向 */
	BARDATA_BUF_STATE_en satate;		/* 数据buf的状态，解码完成后处于IDLE状态 */
	U16	validNum;						/* 数据buf中的有效数据个数，buf处于IDLE时，该变量需要清零 */
	
}BARDATA_BUF_st;

/*	此结构是用来管理解码应用的，包括解码库需要的变量和采集数据需要的变量  */
typedef struct barcode_app_st
{
	S8	decodeBuf_idx;					/* 解码数据的buffer index */
	S8	recvBuf_idx;					/* 接收数据的buffer index */
	
	U16	frameIdx;						/* frame 帧信号的索引，开始的一些帧，需要丢弃 */
	
	U8	minSuccessNum;					/* 解码成功的次数要求，需要解码成功几次，才能算成功 */
	
	BARCODE_PIC tPic;					/* 解码库参数 */

}BARCODE_APP_st;

BARDATA_BUF_st	gtbarDataBuf[BARDATA_BUF_NUM];
BARCODE_APP_st	gtbarApp;
void *gbarAppDec_handle;

U8	barDecode[BARDATA_MAX_LEN];	/* 当需要的解码成功的次数大于1时，需要这两个参数 */
#ifndef BAR_DECODE_MULTI_1
U8	tempbarDecode[BARDATA_MAX_LEN];
U8	tempBarlength;
U8	tempScanDirect;
#endif
U8	barDecodeSuccNum;
U8	barDecodeFailNum;

//U8	barDecode_bak[BARDATA_MAX_LEN];
//U8	barLen_bak;
//U32	barDecodeOkTick;
//BARCODE_CODE_TYPE codeType_bak;

U8	gScanner_state = 0;			/* 扫描模块状态：0---关闭，1---开启 */
//U8	gScanKey_lastState;		/* 扫描键的上一次状态：0---释放；1---按下 */
U32	gLaserStartTime;		/* 激光超时关闭的起始时间(ticks) */
U32	gLaserTimeOut = BARAPP_LASER_LIGHT_TIME;			/* 激光超时时间(ticks) */

#ifdef BAR_DECODE_MULTI_1
typedef struct barcode_bak_st
{
	U8 barCode[BARDATA_MAX_LEN];
	U8 barCodeLen;
	U8 okCnt;
}BARCODE_BAK_st;
#define BARCODE_OK_NUM		(2)//(4)////识别到同一条码数目达到时，认为识别成功
#define BARCODE_BAK_NUM		(3)//(10)//(6)//
BARCODE_BAK_st tbarCodeBakBuff[BARCODE_BAK_NUM];
//U8 barCodeBakIndex;
#endif//BAR_DECODE_MULTI_1

#ifdef TEST_DECODE
BARCODE_CODE tTempcode;
#endif

#if 0
//for test
char *pTestBuf;
int  testSize;
#endif

//U32 scanKey_ok = 0;
//U32 barDecode_ok = 0;

void (* barDecode_WakeupDecoder)(void);


void barApp_dataExtInt_ISR(void);
static void barApp_bufRestore(void);

int barApp_getPowerStatus(void);

#ifdef TEST_DECODE
int testBarCode(char *pbarcode, int *pBarLen, BARCODE_CODE* tcode, BARDATA_BUF_st *ptbar);
#endif

#if 0
HANDLE Barcode_Open()
{
	return 0;
}
int Barcode_SetOpt(HANDLE hBc,BARCODE_OPT_TYPE eType,void *pParam)
{
	return BARCODE_ERROR;
}
int Barcode_Decode(HANDLE hBc,BARCODE_PIC *pPic,BARCODE_CODE *pCode)
{
	return BARCODE_ERROR;
}
void Barcode_Close(HANDLE hBc)
{
}
#endif

/*-------------------------------------------------------------------------
* 函数:	barScannerHW_init
* 说明:	扫描模块控制IO初始化
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barScannerHW_init(void)
{
	
	/* 激光控制口 laser Ctrl */
	hyhwGpio_setAsGpio(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	hyhwGpio_setOut(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	//hyhwGpio_setHigh(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	hyhwGpio_setLow(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	/* 摆片控制口 PENDULUM Ctrl */
	hyhwGpio_setAsGpio(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	hyhwGpio_setOut(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	//hyhwGpio_setHigh(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	hyhwGpio_setLow(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);

	/* 光头帧信号 */
	hyhwGpio_setAsGpio(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	hyhwGpio_setIn(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	hyhwGpio_setPullUp(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	/* 光头数据信号 */
	hyhwGpio_setAsGpio(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	hyhwGpio_setIn(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	hyhwGpio_pullDisable(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	
	return;
}

void barApp_powerOnOff(int isEnable)
{
	hyhwScanner_powerCtrl(isEnable);
	//PhoneTrace(0, "scan power %d", isEnable);
}

#define POWER_ON			(BIT0)
#define POWER_OFF			(BIT1)

#define POWER_STATUS		(BIT4)

U8 scannerPowerFlag = 0;
/*-------------------------------------------------------------------------
* 函数:	barApp_powerCtrl
* 说明:	扫描头电源控制，由于其电源是由I2C设置PMU控制，
*		不能在中断中调用，该函数只能在应用中调用
* 参数:	none
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_powerCtrl(void)
{
	if ((scannerPowerFlag&POWER_ON) != 0)
	{
		if ((scannerPowerFlag&POWER_STATUS) == 0)
		{
			barApp_powerOnOff(1);
		}
		scannerPowerFlag = POWER_STATUS;//扫描头电源已开启
		//hyUsbPrintf("power_%x\r\n", scannerPowerFlag);
	}
	else if ((scannerPowerFlag&POWER_OFF) != 0)
	{
		if ((scannerPowerFlag&POWER_STATUS) != 0)
		{
			barApp_powerOnOff(0);
		}
		scannerPowerFlag = 0;
		//hyUsbPrintf("power_%x\r\n", scannerPowerFlag);
	}
}
/*-------------------------------------------------------------------------
* 函数:	barApp_setPowerCtrlFlag
* 说明:	设置扫描头电源控制flag，可在中断中调用
* 参数:	isEnable
*			0---关闭；否则---开启
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_setPowerCtrlFlag(int isEnable)
{
	if (isEnable)
	{
		scannerPowerFlag |= POWER_ON;
	}
	else
	{
		scannerPowerFlag |= POWER_OFF;
	}
}

/*-------------------------------------------------------------------------
* 函数:	barApp_getPowerStatus
* 说明:	获取扫描头电源开启状态
* 参数:	none
* 返回:	无
*			0---关闭；否则---开启
* -------------------------------------------------------------------------*/
int barApp_getPowerStatus(void)
{
	return (int)(scannerPowerFlag&POWER_STATUS);
}

/*-------------------------------------------------------------------------
* 函数:	barApp_laserCtrl
* 说明:	光头激光控制
* 参数:	isEnable
*			0---关闭；否则---开启
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_laserCtrl(int isEnable)
{
	if (isEnable)
	{
		BARAPP_LASER_ENABLE;
		gLaserStartTime = rawtime(NULL);//Fsk_Get_CurTime();
		
	#ifdef BAR_DECODE_MULTI_1
		//barCodeBakIndex = 0;
		memset(&tbarCodeBakBuff[0], 0, sizeof(BARCODE_BAK_st)*BARCODE_BAK_NUM);
	#endif
		
		BARAPP_INT_ALL_DISABLE;
		/* 清除frame和data中断标志，防止误中断 */
		BARAPP_FRAME_EXTINT_CLEAR;
		BARAPP_DATA_EXTINT_CLEAR;
		/* 允许frame和data口中断 */
		BARAPP_FRAME_EXTINT_ENABLE;
		BARAPP_DATA_EXTINT_ENABLE;
		BARAPP_INT_ALL_ENABLE;
		
		/* timer开始计数 */
		BARAPP_TIMER_ENABLE;
	}
	else
	{
		BARAPP_LASER_DISABLE;
		gLaserStartTime = 0;
		
		barApp_bufRestore();
		
		BARAPP_INT_ALL_DISABLE;
		/* 禁止frame和data中断 */
		BARAPP_FRAME_EXTINT_DISABLE;
		BARAPP_DATA_EXTINT_DISABLE;
		/* 清除frame和data中断标志，防止误中断 */
		BARAPP_FRAME_EXTINT_CLEAR;
		BARAPP_DATA_EXTINT_CLEAR;
		BARAPP_INT_ALL_ENABLE;
		
		/* timer停止计数 */
		BARAPP_TIMER_DISABLE;
		
		barDecodeSuccNum = 0;
		barDecodeFailNum = 0;
	}
}

#if 1
/*-------------------------------------------------------------------------
* 函数:	barApp_setLaserOutTime
* 说明:	设置光头激光超时时间，且转换为系统ticks
* 参数:	outTimeSecond---超时时间(以秒为单位)
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_setLaserOutTime(int outTimeSecond)
{
	gLaserTimeOut = outTimeSecond*100;//转换为系统ticks
}
#if 0
/*-------------------------------------------------------------------------
* 函数:	barApp_laserCloseTimeout
* 说明:	光头激光超时关闭，超时时间为 BARAPP_LASER_LIGHT_TIME(3秒)
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_laserCloseTimeout(void)
{
	U32 now;
	
	if (gLaserStartTime == 0) return;/* 已经处于关闭状态 */
	now = rawtime(NULL);
	if ((now - gLaserStartTime) >= gLaserTimeOut)//BARAPP_LASER_LIGHT_TIME
	{
		/* 关闭扫描模块激光 */
		barApp_laserCtrl(0);
		//停摆光
		BARAPP_PENDULUM_DISABLE;
		//关扫描头电源
		//barApp_powerOnOff(0);
		barApp_setPowerCtrlFlag(0);
		if (barDecode_WakeupDecoder != NULL)
		{
			barDecode_WakeupDecoder();
		}
		//hyUsbMessage("power Off_timeOut\r\n");
		
//		if (g_mcu_ctl.saveMsg.continueScan != 0)
//		{
//			gScanKey_lastState = 0;
//		}
		//test_scanTimeRestore();
	}
}
#endif
#endif


/*-------------------------------------------------------------------------
* 函数:	barApp_getLaserState
* 说明:	获取光头激光状态
* 参数:	无
* 返回:	0----关闭状态；1----开启状态
* -------------------------------------------------------------------------*/
int barApp_getLaserState(void)
{
	if (gLaserStartTime == 0)
	{
		//处于关闭状态
		return 0;
	}
	//处于开启状态
	return 1;
}

#if 1
/*-------------------------------------------------------------------------
* 函数:	barScanKey_laserCtrl
* 说明:	扫描键检测和激光控制
* 参数:	scanKeyStatus --  低层扫描驱动的按键状态，0 为按键释放，
*						  1-6000的值，表示按下状态，且间隔为10ms，值增加1
* 返回:	0----扫描键处于释放状态
*		1----扫描键处于按下状态
* -------------------------------------------------------------------------*/
int barScanKey_laserCtrl(U32 scanKeyStatus)
{
	int rc;
	
	if (scanKeyStatus)	/* 按键按下 */ 
	{
		if (scanKeyStatus <= gLaserTimeOut)	/* 按键时间小于超时控制 */ 
		{
			/* 判断是否超时那一时刻点，这一时刻需要关扫描 */
			if (scanKeyStatus == gLaserTimeOut)
			{
				/* 关闭扫描模块激光 */
				barApp_laserCtrl(0);
				/*停摆光*/
				BARAPP_PENDULUM_DISABLE;
				/*关扫描头电源*/
				
				//barApp_powerOnOff(0);
				
				barApp_setPowerCtrlFlag(0);
				if (barDecode_WakeupDecoder != NULL)
				{
					barDecode_WakeupDecoder();
					//_kqputlo(barDecode_WakeupDecoder, NULL);
				}
			}
			else if(scanKeyStatus == 1)
			{
				/*按下键第1个检测点，开扫描头电源和摆光*/
				
				//barApp_powerOnOff(1);
				//scanKey_ok++;
				barApp_setPowerCtrlFlag(1);
				if (barDecode_WakeupDecoder != NULL)
				{
					barDecode_WakeupDecoder();
					//_kqputlo(barDecode_WakeupDecoder, NULL);
				}
				
				/*开摆光*/
				BARAPP_PENDULUM_ENABLE;
				//hyUsbMessage("power On_Key\r\n");
			}
			else if(scanKeyStatus == 2)
			{
				/* 按下键第2个检测点，开启扫描模块激光 */
				barApp_laserCtrl(1);
			}
		}
		
		rc = 1;
	}
	else
	{
		/* 扫描键释放 */
		//if (barApp_getLaserState() != 0)
		if (barApp_getPowerStatus() != 0)
		{
			/* 关闭扫描模块激光 */
			barApp_laserCtrl(0);
			//停摆光
			BARAPP_PENDULUM_DISABLE;
			//关扫描头电源
			//barApp_powerOnOff(0);
			barApp_setPowerCtrlFlag(0);
			if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}
			//hyUsbMessage("power Off_noKey\r\n");
		}

		rc = 0;
	}
	
	return rc;
}
#endif

void barDecode_Wakeup_Config(void(*pHandle)(void))
{
	barDecode_WakeupDecoder = pHandle;
}
/*-------------------------------------------------------------------------
* 函数:	barApp_frameExtInt_ISR
* 说明:	光头帧信号外部中断的处理函数(沿中断)，在这个函数中，交替使用两个buffer，
*		一个buf用于接收采集的数据，另一个用于解码
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_frameExtInt_ISR(void)
{
	int i;
	BARDATA_BUF_st	*ptbarDataBuf;
	S8 recvBuf_idx;
	U16	frameIdx, validNum;
	U8 scanDirect;
	
	recvBuf_idx = gtbarApp.recvBuf_idx;
	
	gtbarApp.frameIdx++;
	frameIdx = gtbarApp.frameIdx;
	
	/* 考虑到激光的稳定，前面几帧放弃 */
	/* frame的中断既是前一帧的结束，也是当前帧的开始 */
	if(frameIdx > BARAPP_FARME_STABLE_IDX)
	{
		/* 读取frame口，以记录扫描方向 */
		scanDirect = BARAPP_READ_FRAME;
		/* 接收完一帧 */
		ptbarDataBuf = &gtbarDataBuf[recvBuf_idx];
		if (ptbarDataBuf->satate == BARDATA_BUF_RECEIVING)
		{
			/* 记录最后一个电平的持续时间 */
			barApp_dataExtInt_ISR();
			//修正最后一个电平的记录
			validNum = ptbarDataBuf->validNum;
			if (validNum>0 && validNum<=BARDATA_VALID_NUM_MAX)
			{
				S16 barData;
				validNum -= 1;
				barData = 0-ptbarDataBuf->data[validNum];
				ptbarDataBuf->data[validNum] = barData;
			}
			ptbarDataBuf->satate = BARDATA_BUF_RECEIVE_OK;
			/*if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}*/
		}
		
		/* 新的一帧，重新开始计数 */
		BARAPP_TIMER_COUNTER_CLEAR;
		
		/* 查找可用于接收数据的buf */
		recvBuf_idx++;
		if (recvBuf_idx >= BARDATA_BUF_NUM)
		{
			recvBuf_idx = 0;
		}
		for (i=0; i<BARDATA_BUF_NUM; i++)
		{
			if (gtbarDataBuf[recvBuf_idx].satate == BARDATA_BUF_IDLE)
			{
				break;
			}
			recvBuf_idx++;
			if (recvBuf_idx >= BARDATA_BUF_NUM)
			{
				recvBuf_idx = 0;
			}
		}
		if (i == BARDATA_BUF_NUM)
		{
			/* 没有可用于接收数据的buf，下次中断再找 */
		#if 0
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
			BARAPP_FRAME_EXTINT_DISABLE;
			BARAPP_DATA_EXTINT_DISABLE;
		#endif
			return;
		}
		
		/* 找到一帧可接收数据，记录其index */
		gtbarApp.recvBuf_idx = recvBuf_idx;
		
		ptbarDataBuf = &gtbarDataBuf[recvBuf_idx];
		ptbarDataBuf->satate = BARDATA_BUF_RECEIVING;
		ptbarDataBuf->validNum = 0;
		if (scanDirect == SCAN_DIRECTION_POSITIVE)
		{
			scanDirect = 1;
		}
		else
		{
			scanDirect = 0;
		}
		ptbarDataBuf->scanDirect = scanDirect;
		
		BARAPP_DATA_EXTINT_ENABLE;
	}
	else if(frameIdx == BARAPP_FARME_STABLE_IDX)
	{
		/* 第一次采集数据的开始 */
		/* 允许data外部中断，开始采集数据 */
		//BARAPP_DATA_EXTINT_ENABLE;
	}
}

/*-------------------------------------------------------------------------
* 函数:	barApp_dataExtInt_ISR
* 说明:	光头的数据信号外部中断的处理函数(沿中断)，
*		在这个函数中，记录该中断之前的电平及电平持续时间
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_dataExtInt_ISR(void)
{
	U8	data, flag;
	U32 count;
	S16	barData, tempData=0;
	U16 validNum;
	BARDATA_BUF_st	*ptbarDataBuf;
	
	/* 得到上次电平持续时间及当前电平(上次电平与当前电平相反) */
	BARAPP_TIMER_COUNTER_READ(&count);
	BARAPP_TIMER_COUNTER_CLEAR;
	data = BARAPP_READ_RAW_DATA;
	
	ptbarDataBuf = &gtbarDataBuf[gtbarApp.recvBuf_idx];
	validNum = ptbarDataBuf->validNum;
	
	if (ptbarDataBuf->satate == BARDATA_BUF_RECEIVING)
	{
		/* 数据写入buf */
		barData = 0;
		/* 上次电平与当前电平相反，负值表示低电平，绝对值表示持续时间 */
		if (data)
		{
			barData = (S16)count;
		}
		else
		{
			barData = (S16)(0 - count);
		}
		#if 1
		if (validNum >= 1)
		{
			flag = 0;
			if (barData>0 && ptbarDataBuf->data[validNum-1]>0)
			{
				flag = 1;
				tempData = -600;
			}
			else if (barData<0 && ptbarDataBuf->data[validNum-1]<0)
			{
				flag = 1;
				tempData = 600;
			}
			if (flag == 1)
			{
				if (validNum < BAR_CODE_DATA_MAX_LEN)
				{
					ptbarDataBuf->data[validNum] = tempData;
					ptbarDataBuf->validNum++;
					validNum++;
				}
			}
		}
		#endif
		
		if (validNum < BAR_CODE_DATA_MAX_LEN)
		{
			ptbarDataBuf->data[validNum] = barData;
			ptbarDataBuf->validNum++;
		}
	}
	
	return;
}

#if 0
/*-------------------------------------------------------------------------
* 函数:	barApp_timerInt_ISR
* 说明:	
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_timerInt_ISR(void)
{
	if (barScanKey_laserCtrl() != 0)
	{
		/* 扫描键按下时，不关机 */
		//Sys_ClearShutDownTimer();
	}
}
#endif

#if 0
void barApp_softInt_ISR(void)
{
	int	len, flag;
	U32 now;
	BARCODE_CODE_TYPE codeType;
	U8 continueScan;
	
	len = 0;
	flag = 0;
	codeType = CODE_TYPE_MAX;
	//continueScan = g_mcu_ctl.saveMsg.continueScan;
	continueScan = 0;
	now = rawtime(NULL);//Fsk_Get_CurTime();
	
	/* 解码条码扫描数据 */
	barApp_getBarcode(&barDecode[0], &len, &codeType);
	if (len > 0)
	{
		if (continueScan != 0)
		{
			/*	连续扫描模式下，需要对比连续识别条码是否一致，
				不同则输出，相同不输出
			*/
			if (memcmp(&barDecode_bak[0], &barDecode[0], len) == 0 &&	//条码相同
				len == barLen_bak &&									//长度相同
				codeType == codeType_bak)								//类型相同
			{
				//条码相同，不再输出
				flag = 0;
			}
			else
			{
				//条码不同，输出
				memcpy(&barDecode_bak[0], &barDecode[0], len);
				barLen_bak = len;
				codeType_bak = codeType;
				flag = 1;
			}
		}
		else
		{
			barLen_bak = 0;
			flag = 1;
		}
		
		if (flag != 0)
		{
			//if (barDecode[0] != CODE_CTRL_FNC3)
			{
			//	beeper_start(SCAN_OK, g_mcu_ctl.saveMsg.beeperEn, 1);
			}
			barBuf_writeBarCode(&barDecode[0], len);
		}
		barDecodeOkTick = now;
		//test_scanTimeRestore();
	}
	else
	{
		if (continueScan != 0)
		{
			if ((now-barDecodeOkTick) > 50)
			{
				barDecodeOkTick = now;
				barLen_bak = 0;
			}
		}
	}
}
#else
int barApp_decodeBarcode(void)
{
	int	len;
	BARCODE_CODE_TYPE codeType;
	
	barApp_getBarcode(&barDecode[0], &len, &codeType);
	if (len > 0)
	{
		barBuf_writeBarCode(&barDecode[0], len);
	}
	return len;
}
#endif

#if 0
void barApp_fullBakInfo(void)
{
	barLen_bak = 8;
	codeType_bak = CODE_TYPE_CODE128;
	barDecode_bak[0] = CODE_CTRL_FNC3;
	memcpy(&barDecode_bak[1], "010501", 7);
}
#endif
/*-------------------------------------------------------------------------
* 函数:	barData_FrameInt_init
* 说明:	初始化frame口的外部中断，沿中断
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barData_FrameInt_init(void)
{
	//hyhwGpio_IntenGpioc30(EINT0SEL);			/* 设置指定 GPIO 为中断功能 GPIO_C30 ExternInt0 */
//	hyhwGpio_IntenHpcsn_l(EINT0SEL);			/* 设置指定 GPIO 为中断功能 GPIO_F24 ExternInt0 */
	hyhwGpio_IntenHpxd3(EINT0SEL);				/* 设置指定 GPIO 为中断功能 GPIO_F3 ExternInt0 */
	//hyhwExtern5_setTriggerFallingEdge();		/* ExternInt0 的中断出发条件 */
 	INT_TRIGGER_MODE_REGISTER &= (~INT_BIT_EI0);/* 沿触发 */
 	INT_MODEA_REGISTER |= INT_BIT_EI0;			/* 上升沿&下降沿触发 */
 	INT_INTMASK_REGISTER |= INT_BIT_EI0;
 	
 	hyhwInt_setFIQ(INT_BIT_EI0);				/* 中断为FIQ类型 */
	//hyhwInt_enable(INT_BIT_EI0);				/* 使能 */
	
	hyhwInt_ConfigScannerFrame(barApp_frameExtInt_ISR);
	BARAPP_FRAME_EXTINT_DISABLE;
	BARAPP_FRAME_EXTINT_CLEAR;
}

/*-------------------------------------------------------------------------
* 函数:	barData_DataInt_init
* 说明:	初始化data口的外部中断，沿中断
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barData_DataInt_init(void)
{
	//hyhwGpio_IntenGpioa2(EINT1SEL);			/* 设置指定 GPIO 为中断功能 GPIO_A2 ExternInt1 */
//	hyhwGpio_IntenHpxd3(EINT1SEL);				/* 设置指定 GPIO 为中断功能 GPIO_F3 ExternInt1 */
	hyhwGpio_IntenHpcsn_l(EINT1SEL);			/* 设置指定 GPIO 为中断功能 GPIO_F24 ExternInt1 */
	//hyhwExtern5_setTriggerFallingEdge();		/* ExternInt1 的中断出发条件 */
 	INT_TRIGGER_MODE_REGISTER &= (~INT_BIT_EI1);/* 沿触发 */
 	INT_MODEA_REGISTER |= INT_BIT_EI1;			/* 上升沿&下降沿触发 */
 	INT_INTMASK_REGISTER |= INT_BIT_EI1;
 	
 	hyhwInt_setFIQ(INT_BIT_EI1);				/* 中断为FIQ类型 */
	//hyhwInt_enable(INT_BIT_EI1);				/* 使能 */
	
	hyhwInt_ConfigScannerData(barApp_dataExtInt_ISR);
	BARAPP_DATA_EXTINT_DISABLE;
	BARAPP_DATA_EXTINT_CLEAR;
}

/*-------------------------------------------------------------------------
* 函数:	barData_Timer_init
* 说明:	初始化timer计数
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barData_Timer_init(void)
{
	tTIMER_REGISTER	*ptTimerReg;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + BARAPP_TIMER_BASE;
	
	ptTimerReg->REFERENCE	= BARAPP_TIMER_LOADVALUE;
	ptTimerReg->CONFIG		= 0;			//以1/6us计数
	
	hyhwTimer_countDisable(BARAPP_TIMER_BASE);
	hyhwTimer_intDisable(BARAPP_TIMER_BASE);
}

#define BARAPP_TEST_TIMER		(0)
#define BARAPP_TEST_LOADVALUE	(50000)
void barData_Timer_init_test(void)
{
	tTIMER_REGISTER	*ptTimerReg;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + BARAPP_TEST_TIMER;
	
	ptTimerReg->REFERENCE	= BARAPP_TEST_LOADVALUE;
	ptTimerReg->CONFIG		= 0;			//以1/6us计数
	
	//hyhwTimer_countDisable(BARAPP_TEST_TIMER);
	hyhwTimer_countEnable(BARAPP_TEST_TIMER);
	hyhwTimer_intDisable(BARAPP_TEST_TIMER);
}

/*-------------------------------------------------------------------------
* 函数:	barData_Init
* 说明:	初始化frame/data口的外部中断、和timer的配置
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barData_Init(void)
{
	barScannerHW_init();
	barData_FrameInt_init();
	barData_DataInt_init();
	barData_Timer_init();

	/* 初始化数据结构 */
	memset(&gtbarDataBuf[0], 0x00, sizeof(BARDATA_BUF_st)*BARDATA_BUF_NUM);
	
}





static const BARCODE_OPT_DEC gtDecOption[2] = 
{
  	/* 0 -- BARCODE_CODE_TYPE_EAN128 */
	{
		BARCODE_CODE_TYPE_EAN128,
		BARCODE_DEC_MODE_COMBINED_COMBINED,
		BARCODE_DEC_MODE_COMBINED_COMBINED,
		BARCODE_DEC_RELIABLE_LOW,
		//BARCODE_DEC_RELIABLE_MIDDLE,
		1,
		1,
		2,
		50,
		BARCODE_OPT_DEC_FLAG_ENABLE
	},
	/* 2 -- BARCODE_CODE_TYPE_CODE39 */
	{
		BARCODE_CODE_TYPE_CODE39,
		BARCODE_DEC_MODE_BINARY_SINGLE,
		BARCODE_DEC_MODE_BINARY_SINGLE,
		BARCODE_DEC_RELIABLE_LOW,
		//BARCODE_DEC_RELIABLE_HIGH,
		1,
		1,
		2,
		50,
		BARCODE_OPT_DEC_FLAG_ENABLE|BARCODE_OPT_DEC_FLAG_FULL_ASCII|BARCODE_OPT_DEC_FLAG_OUTPUT_ECC
	},
};

/*-------------------------------------------------------------------------
* 函数:	barcode_setOption
* 说明:	设置条码识别的属性
* 参数:	barDec_handle--解码handle
*		pbarcodeOpt----获取属性的结构指针
* 返回:	0----设置成功，-1----设置无效
* -------------------------------------------------------------------------*/
int barcode_setOption(HANDLE barDec_handle)
{
	int ret;
	int i;
        
	BARCODE_OPT bopt;
	BARCODE_OPT_SCAN *pbscan;

	bopt.eType = BARCODE_OPT_TYPE_SCAN;
	pbscan = &bopt.tParam.tOptScan;
	
	//解码双向扫描
	pbscan->uFlag = BARCODE_OPT_SCAN_FLAG_BIDIRECT;
	
	//按顺序解码CODE39、CODE128这2种条码（其他类型不检测）
	pbscan->uScanSeqNum = 2;
	pbscan->eScanSeq[0] = BARCODE_SCAN_TYPE_CODE39;
	pbscan->eScanSeq[1] = BARCODE_SCAN_TYPE_CODE128;
	
	//调用设置接口，注意pbscan结构体中所有成员都要正确赋值
	if(Barcode_SetOpt(barDec_handle,&bopt) != BARCODE_SUCCESS)
	{
		return -1;
	}
	
	/* 实际设置 */
	bopt.eType = BARCODE_OPT_TYPE_DEC;
	for(i=0; i<2; i++)
	{
		/* 组合库使用的结构, 即填充设置参数的结构*/
		memcpy((char *)&bopt.tParam.tOptDec, (char *)&gtDecOption[i], sizeof(BARCODE_OPT_DEC));
		/*调用设置接口，注意pbdec结构体中所有成员都要正确赋值 */
		ret = Barcode_SetOpt(barDec_handle, &bopt);
		if (ret != BARCODE_SUCCESS)
		{
			return -1;
		}
	}
	return 0;
}



/*-------------------------------------------------------------------------
* 函数:	barApp_init
* 说明:	提供给调用者的接口，在这个函数中，进行必要的初始化
* 参数:	无
* 返回:	0	-- 初始化成功
*		-1	-- 初始化失败
* -------------------------------------------------------------------------*/
int barApp_init(void)
{
	int rc = 0;
	
	/* 初始化解码顺序需要的参数*/
	BARCODE_PIC			*ptPic;
	
	#if 0
	//for test
	pTestBuf = hsaSdram_UpgradeBuf();
	testSize = 0;
	#endif
	
	/* 中断资源初始化*/
	barData_Init();
	
    /* 全局变量初始化 */
    memset((U8*)&gtbarApp, 0, sizeof(BARCODE_APP_st));
	gbarAppDec_handle = NULL;

	gtbarApp.minSuccessNum	= BARAPP_MIN_SUCCESS_NUM;
	gtbarApp.frameIdx = 0;
	
	ptPic = &gtbarApp.tPic;
	ptPic->eType					= BARCODE_PIC_TYPE_STRIPE;		/* 一维Y分量格式  */
	ptPic->tData.tBImage.uWidth		= BAR_CODE_DATA_MAX_LEN;		/* 图像宽度 */
	ptPic->tData.tBImage.uHeight	= 1;							/* 图像高度，应为1 */
	ptPic->tData.tBImage.uStartX	= 0;							/* 起始点x坐标 */
	ptPic->tData.tBImage.uStartY	= 0;							/* 起始点y坐标，应为0 */
	ptPic->tData.tBImage.uEndX		= BAR_CODE_DATA_MAX_LEN-1;		/* 终止点x坐标 */
	ptPic->tData.tBImage.uEndY		= 0;							/* 终止点y坐标，应为0 */
	ptPic->tData.tBImage.pData		= &gtbarDataBuf[0].data[0];		/* 图像数据 */

	/* 解码库初始化 */
	gbarAppDec_handle = Barcode_OpenDec(NULL);
	//hyUsbPrintf("handle === %x\r\n",gbarAppDec_handle);
	if(gbarAppDec_handle)
	{
		rc = barcode_setOption(gbarAppDec_handle);
		if (rc != 0)
		{
		   Barcode_Close(gbarAppDec_handle);
		   rc =  -1;
		}
	}
	else
	{
		rc = -1;
	}
	
#ifdef TEST_WRITE
	gbarfp = AbstractFileSystem_Open(BAR_FILE,AFS_RDWR);
	if(gbarfp >= 0)
	{
		AbstractFileSystem_Seek(gbarfp,0,AFS_SEEK_OFFSET_FROM_END);
	}
	else
	{
		gbarfp = AbstractFileSystem_Create(BAR_FILE);
	}
	if (gbarfp >= 0)
	{
		#define TIME_M	"time;"##__DATE__##" "##__TIME__
		char str[50];
		char *pTime = (char *)TIME_M;
		strcpy(str, "测试扫描,");
		strcat(str, pTime);
		strcat(str, "\r\n");
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
	}
#endif
	
	return rc;
}

/*-------------------------------------------------------------------------
* 函数:	barApp_bufRestore
* 说明:	接收buf恢复初始化值
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
static void barApp_bufRestore(void)
{
	int i;
	
	for (i=0; i<BARDATA_BUF_NUM; i++)
	{
		gtbarDataBuf[i].satate = BARDATA_BUF_IDLE;
		gtbarDataBuf[i].validNum = 0;
	}
	gtbarApp.decodeBuf_idx = 0;
	gtbarApp.recvBuf_idx = 0;
	gtbarApp.frameIdx = 0;
}

/*-------------------------------------------------------------------------
* 函数:	barApp_close
* 说明:	提供给调用者的接口，关闭解码库
* 参数:	无
* 返回:	无
* -------------------------------------------------------------------------*/
void barApp_close(void)
{
	Barcode_Close(gbarAppDec_handle);
}


/*-------------------------------------------------------------------------
* 函数:	barScanner_Init
* 说明:	初始化扫描头：初始化变量、解码库、开启扫描头电源
* 参数:	none
* 返回:	0	- 初始化成功
*		-1  - 初始化失败，此时没有开启扫描头电源
* -------------------------------------------------------------------------*/
int barScanner_init(void)
{
	int rc;
	
	rc = barApp_init();
	if (rc == 0)
	{
		gScanner_state = 1;		//扫描模块开启
		//barApp_powerOnOff(1);
	}
	barApp_laserCtrl(0);
	//gScanKey_lastState = 0;
	barDecode_WakeupDecoder = NULL;
	
	barBuf_init();
	
	//barData_Timer_init_test();
	//beeper_start(SCANNER_POWERON_OK, 1, 1);
	//while(beeper_getState() == 1);
	
	//test_scanInitOk();
	#ifdef TEST_GPIO
	hyhwGpio_setAsGpio(TESTSCAN_PORT, TESTSCAN_GPIO);
	hyhwGpio_setOut(TESTSCAN_PORT, TESTSCAN_GPIO);
	hyhwGpio_setLow(TESTSCAN_PORT, TESTSCAN_GPIO);
	#endif
	
	return rc;
}

/*-------------------------------------------------------------------------
* 函数:	barScanner_Init
* 说明:	初始化扫描头：初始化变量、解码库、开启扫描头电源
* 参数:	none
* 返回:	0	- 初始化成功
*		-1  - 初始化失败，此时没有开启扫描头电源
* -------------------------------------------------------------------------*/
int barScanner_close(void)
{
	Barcode_Close(gbarAppDec_handle);
	BARAPP_LASER_DISABLE;
	BARAPP_PENDULUM_DISABLE;	//禁止摆光片摆动
	//gScanKey_lastState = 0;
	gScanner_state = 0;
	barApp_powerOnOff(0);
#ifdef TEST_WRITE
	if(gbarfp >= 0)
	{
		extern int okCnt;
		extern int errCnt;
		char str[50];
		sprintf(str, "扫描识别正确总数:%d\r\n", okCnt);
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
		sprintf(str, "扫描识别错误总数:%d\r\n", errCnt);
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
		AbstractFileSystem_Close(gbarfp);
	}
#endif

}

/*-------------------------------------------------------------------------
* 函数:	barScanner_checkValidFrame
* 说明:	检测是否有有效的帧数据，如果有有效帧，则直接唤醒
*		该函数在IRQ[user_irq_handler()]中断中调用
* 参数:	none
* 返回:	none
* -------------------------------------------------------------------------*/
void barScanner_checkValidFrame(void)
{
	int i, flag = 0;
	
	if (scanner_getType() != SCANNER_TYPE_OPTICON)
	{
		/* 不是欧光扫描头，退出 */
		return;
	}
	
	/* 欧光扫描头激光是否处于开启状态 */
	if (barApp_getLaserState() == 0)
	{
		/* 激光已关闭 */
		return;
	}
	for (i=0; i<BARDATA_BUF_NUM; i++)
	{
		if (gtbarDataBuf[i].satate == BARDATA_BUF_RECEIVE_OK)
		{
			if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}
			break;
		}
	}
	
	return;
}

/*-------------------------------------------------------------------------
* 函数:	barScanner_getState
* 说明:	获得扫描模块状态
* 参数:	none
* 返回:	0	- 关闭状态
*		1	- 开启状态
* -------------------------------------------------------------------------*/
int barScanner_getState(void)
{
	return gScanner_state;
}

void barTestWrite(U8 *pBar, U16 *pDat, int len)
{
#ifdef TEST_WRITE
	int i,j;
	U16  ch;
	U8  *pWrite;
	
	pWrite = (U8 *)hsaSdram_UpgradeBuf();
	
	j = 0;
	
	if(gbarfp >= 0)
	{
		//写条码
		AbstractFileSystem_Write(gbarfp,pBar,strlen(pBar));
		AbstractFileSystem_Write(gbarfp,"\r\n",strlen("\r\n"));
		
		//hyUsbPrintf("len == %d\r\n",len);
		//写数据
		for(i = 0; i < len; i++)
		{
			ch = pDat[i];
			sprintf(&pWrite[j],"%04x ",ch);
			j+=5;
		}
		pWrite[j] = 0;
		AbstractFileSystem_Write(gbarfp,pWrite,j);
		AbstractFileSystem_Write(gbarfp,"\r\n",strlen("\r\n"));
	}
#else
	
	int i,j,fp;
	U16  ch;
	char path[50];
	U8  *pWrite;
	
	pWrite = (U8 *)hsaSdram_UpgradeBuf();
	
	j = 0;
	
	sprintf(path,"C:/%d.txt",rawtime(NULL));
	fp = AbstractFileSystem_Create(path);
	
	if(fp >= 0)
	{
		//写条码
		AbstractFileSystem_Write(fp,pBar,strlen(pBar));
		AbstractFileSystem_Write(fp,"\r\n",strlen("\r\n"));
		
		//hyUsbPrintf("len == %d\r\n",len);
		//写数据
		for(i = 0; i < len; i++)
		{
			ch = pDat[i];
			sprintf(&pWrite[j],"%04x ",ch);
			j+=5;
		}
		pWrite[j] = 0;
		AbstractFileSystem_Write(fp,pWrite,j);
		AbstractFileSystem_Write(fp,"\r\n",strlen("\r\n"));
		
		AbstractFileSystem_Close(fp);
	}
	
#endif
}

/*-------------------------------------------------------------------------
* 函数:	barApp_getBarcode
* 说明:	获得条码
* 参数:	pbarcode	-- 输入，调用者给出的存放条码的buffer
*		pbarLength	-- 输出，返回条码的长度
* 返回:	0	- 得到一个有效的条码
*		-1  - 失败
* -------------------------------------------------------------------------*/
int barApp_getBarcode(U8 *pbarcode, int *pbarLength, BARCODE_CODE_TYPE *pCodeType)
{
	int	i, rc = -1;
	int decodeRet;
	int	successNum, barlength;//, tempBarlength;
	S8	decodeBuf_idx;
//	U8	tempBar[BARDATA_MAX_LEN];	/* 当需要的解码成功的次数大于1时，需要这两个参数 */
	BARCODE_CODE	tcode;		/* 解码库带回的条码参数 */
	BARDATA_BUF_st	*ptbarDataBuf;
#ifdef BAR_DECODE_MULTI_1
	BARCODE_BAK_st	*ptbarBak;
#endif
#ifdef TEST_WRITE
	U16 valNum;
#endif
	
	successNum = 0;
	while(1)
	{
		/* 扫描头激光是否处于开启状态 */
		if (barApp_getLaserState() == 0)
		{
			/* 激光已关闭，返回解码错误 */
			*pbarLength = 0;
			return -1;
		}
		
		/* 判断是否有可解码的数据帧 */
		decodeBuf_idx = gtbarApp.decodeBuf_idx;
		decodeBuf_idx++;
		if (decodeBuf_idx >= BARDATA_BUF_NUM)
		{
			decodeBuf_idx = 0;
		}
		for (i=0; i<BARDATA_BUF_NUM; i++)
		{
			if (gtbarDataBuf[decodeBuf_idx].satate == BARDATA_BUF_RECEIVE_OK)
			{
				break;
			}
			decodeBuf_idx++;
			if (decodeBuf_idx >= BARDATA_BUF_NUM)
			{
				decodeBuf_idx = 0;
			}
		}
		if (i == BARDATA_BUF_NUM)
		{
			/* 没有可解码数据，返回解码错误 */
			*pbarLength = 0;
			return -1;
		}
		
		/* 找到一帧可解码数据，记录其index */
		gtbarApp.decodeBuf_idx = decodeBuf_idx;
		ptbarDataBuf = &gtbarDataBuf[decodeBuf_idx];
		if (ptbarDataBuf->validNum < BARDATA_VALID_NUM_MIN ||
			ptbarDataBuf->validNum > BARDATA_VALID_NUM_MAX)
		{
			/* 有效数据太少或太多，直接返回错误 */
			ptbarDataBuf->satate = BARDATA_BUF_IDLE;
			//*pbarLength = 0;
			//return -1;
			continue;
		}
		/* 这帧数据正在解码 */
		ptbarDataBuf->satate = BARDATA_BUF_DECODING;

		gtbarApp.tPic.tData.tBBuf.pData		= (void *)&ptbarDataBuf->data[0];
		gtbarApp.tPic.tData.tBBuf.uLen		= ptbarDataBuf->validNum*BARDATA_SIZE;
		
	#ifdef TEST_WRITE
		valNum = ptbarDataBuf->validNum;
	#endif
		if (/*ptbarDataBuf->scanDirect && */gtbarApp.frameIdx > 50)
		{
			//ptbarDataBuf->scanDirect = 1;
		}
		memset(&tcode, 0, sizeof(BARCODE_CODE));
		#ifdef TEST_GPIO
		hyhwGpio_setHigh(TESTSCAN_PORT, TESTSCAN_GPIO);
		#endif
		decodeRet = Barcode_Decode(gbarAppDec_handle, &gtbarApp.tPic, &tcode);
		//if (gtbarApp.frameIdx >= 100 && gtbarApp.frameIdx < 120)
		//{
		//	barTestWrite("TT", gtbarApp.tPic.tData.tBBuf.pData, ptbarDataBuf->validNum);
		//}
		//decodeRet = BARCODE_ERROR;
		#ifdef TEST_GPIO
		hyhwGpio_setLow(TESTSCAN_PORT, TESTSCAN_GPIO);
		#endif
		
		barDecodeSuccNum = 0;
		if (decodeRet == BARCODE_SUCCESS)
		{
			#if 1
			for (i=0; i<BARCODE_BAK_NUM; i++)
			{
				//长度和内容相同，增加正确数
				if (tcode.uLen == tbarCodeBakBuff[i].barCodeLen &&
					(memcmp(tbarCodeBakBuff[i].barCode, 
							tcode.pBuf, 
							tcode.uLen) == 0))
				{
					tbarCodeBakBuff[i].okCnt++;
					if (tbarCodeBakBuff[i].okCnt >= BARCODE_OK_NUM)
					{
						//正确数目达到BARCODE_OK_NUM，认为成功识别
						barlength	= tbarCodeBakBuff[i].barCodeLen;
						memcpy(pbarcode, tbarCodeBakBuff[i].barCode, barlength);
						barDecodeSuccNum = BARAPP_MIN_SUCCESS_NUM;
					}
					break;
				}
			}
			if (i == BARCODE_BAK_NUM)
			{
				//没有匹配的，就按新的条码存起来
				for (i=0; i<BARCODE_BAK_NUM; i++)
				{
					if (tbarCodeBakBuff[i].barCodeLen == 0)
					{
						memcpy(tbarCodeBakBuff[i].barCode, tcode.pBuf, tcode.uLen);
						tbarCodeBakBuff[i].barCodeLen = tcode.uLen;
						tbarCodeBakBuff[i].okCnt++;
						break;
					}
				}
			}
			#endif
		}
		/*else
		{
			barTestWrite("TT",gtbarApp.tPic.tData.tBBuf.pData,ptbarDataBuf->validNum);
		}*/
		/* 这帧数据使用完毕 */
		ptbarDataBuf->satate = BARDATA_BUF_IDLE;
		/* 成功解码次数达到要求，返回成功 */
		if (barDecodeSuccNum >= gtbarApp.minSuccessNum)
		{
			U8 intLev;
			intLev = InterruptDisable();
			barApp_laserCtrl(0);
			//停摆光
			BARAPP_PENDULUM_DISABLE;
			InterruptRestore(intLev);
			
			//关扫描头电源
			//barApp_powerOnOff(0);
			barApp_setPowerCtrlFlag(0);
			
			pbarcode[barlength++] = BARCODE_END_CHAR;
			*pbarLength = barlength;
			*pCodeType = tcode.eType;
		
			rc = 0;
			break;
		}
	}
	
	return rc;
}


#ifdef TEST_DECODE
int testBarCode(char *pbarcode, int *pBarLen, BARCODE_CODE* tcode, BARDATA_BUF_st *ptbar)
{
	BARCODE_CODE_TYPE type;
#if 1
	if (memcmp(&pbarcode[0], "977100707700515", *pBarLen) != 0 &&
		memcmp(&pbarcode[0], "9771007077005", *pBarLen) != 0 &&
		tcode->eType != CODE_TYPE_CODE128)
#endif
#if 0
	type = tcode->eType;
	if (type == CODE_TYPE_MATRIX25 ||
		type == CODE_TYPE_IATA25)
#endif
	{
		#if 1
		//uart 输出错误条码的输入
		{
			U16	validNum, strLen, i, j;
			S16	*pData;
			U8	str[30];
			
			validNum = ptbar->validNum;
			pData = &ptbar->data[0];
			for (i=0; i<validNum; i++)
			{
				sprintf(str, "0x%08X, ", (S16)*pData);
				pData++;
				if ((i>0 && ((i+1)%8)==0) ||
					(i == (validNum-1)))
				{
					strcat(str, "\r\n");
				}
				strLen = strlen(str);
				j=0;
				while(strLen--)
				{
					USART_SendData(USART1, str[j++]);
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}
			}
			//显示识别类型
			sprintf(str, "--barcode type: %d\r\n", tTempcode.eType);
			strLen = strlen(str);
			j=0;
			while(strLen--)
			{
				USART_SendData(USART1, str[j++]);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			}
		}
		#endif
		
		//strcat(&pbarcode[0], "_err");
		//*pBarLen += 4;
		
		return -1;
	}
	return 0;
}
#endif