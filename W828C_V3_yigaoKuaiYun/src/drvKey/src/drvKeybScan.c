/*************************************************************
%
% Filename     : DrvKeybScan.c
% Project name : part of the lower key driver of GUI part of hyco, manage the input key scanning 
%
% Copyright 2010 Hyco,
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    	Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040128  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the key process part of the GUI   
% 
****************************************************************/
#include "glbStatusDef.h"
#include "hyhwGpio.h"
#include "drvKeyb.h"
#include "hyhwAdc.h"
#include "LedCtl.h"
#include "HycDeviceConfig.h"

#define CLOSE_AUTOSCAN_MAXTIME		(2*6000)//2分钟

U8   gu8AutoScan;//1:开启自动扫描   0:关闭
U32	 gu32LastAutoScanTime;//自动扫描时最后一次扫描到条码时间
static int gsScanKeyPushTime=0;

static U32 gbadScanKey ; //记录按坏的扫描键,仅限扫描键 在开机过程检测
static U8  gCheckScanKeyF; //检测扫描键

static S8 sS8ScanKeyEnable ;

/*-----------------------------------------
*注意：*注意：*注意：*注意：*注意：*注意：
*注意：                             注意：
*注意：JTAG 调试的时候不能使用按键  注意：
*注意：JTAG 调试的时候不能使用按键  注意：
*注意：								注意：
*注意：*注意：*注意：*注意：*注意：*注意：
-------------------------------------------*/

//#define JTAG_DEBUG

typedef struct _tKEYIOINPUT
{
	U8   port;  //端口号
	U32  iobit; //bit
	U8   iook;  //有效方式 0或1，具体参考硬件参数
}tKEYIOINPUT;

typedef struct _tKEYIOOUTPUT
{
	U8   port;  //端口号
	U32  iobit; //bit
}tKEYIOOUTPUT;


#define KEY_IO_INPUT_NUM		4   
#define KEY_IO_OUTPUT_NUM		5
#define KEY_IO_INPUT_SINGLE_NUM 4  /* 非矩阵按键 */
/* 
 * 说明: 共20个按键 4*5

 * 矩阵键盘定义 
W828
						     KEY_IN1	KEY_IN2		KEY_IN3		KEY_IN4
						     GPIO_A2	GPIO_E29	GPIO_E30 	GPIO_D10
KEY_OUT1		GPIO_A10		S2-0		S3-*		S4-7		S5-#
KEY_OUT2		GPIO_F15		S7-5		S8-DEL		S9-ESC		S10-6
KEY_OUT3		GPIO_A12		S11-DOWN	S12-R		S13-OK		S14-TAB
KEY_OUT4		GPIO_D6			S15-8		S16-4		S17-1		S18-9
KEY_OUT5		GPIO_D7			S19-2		S20-L		S21-UP		S22-3

 * 行设置为上拉输入，列设置为推挽输出
 */
//矩阵输出端口
const tKEYIOOUTPUT gtIOOutput[KEY_IO_OUTPUT_NUM]=
{
	{PORT_A, GPIO_10},
	{PORT_F, GPIO_15},
	{PORT_A, GPIO_12},
	{PORT_D, GPIO_6},
	{PORT_D, GPIO_7},
};

const tKEYIOINPUT  gtIOInput[KEY_IO_INPUT_NUM]=
{	
	//{PORT_F, GPIO_14,	0},
	{PORT_A, GPIO_2,	0},
	{PORT_E, GPIO_29,	0},
	{PORT_E, GPIO_30,	0},
	{PORT_D, GPIO_10,	0}
};

//非矩阵键盘
const tKEYIOINPUT  gtIOInputSingle[KEY_IO_INPUT_SINGLE_NUM]=
{
	{PORT_A, GPIO_11,	1},	/* 开机键 */
	{PORT_D, GPIO_5,	0},	/* 扫描键左 */
	{PORT_D, GPIO_12,	0},	/* 扫描键右 */
	{PORT_F, GPIO_31,	0},	/* 功能键F1 */
};


/* 每个bit对应的按键 */
const U8 gdrvKeyb_u8KeycodeTable[] = 
{
	0,
	Keyb_Code_Power,  /* bit 0 */
	Keyb_Code_ScanL,  /* bit 1 */
	Keyb_Code_ScanR,
	Keyb_Code_F1,
	
	Keyb_Code_0,
	Keyb_Code_Xing,
	Keyb_Code_7,
	Keyb_Code_Jing,
	Keyb_Code_5,
	Keyb_Code_Del,
	Keyb_Code_Esc,
	Keyb_Code_6,
	Keyb_Code_Down,
	Keyb_Code_Right,
	Keyb_Code_OK,
	Keyb_Code_Tab,
	Keyb_Code_8,
	Keyb_Code_4,
	Keyb_Code_1,
	Keyb_Code_9,
	Keyb_Code_2,
	Keyb_Code_Left,
	Keyb_Code_Up,
	Keyb_Code_3,
	
};

//检测B6是否为低
const U32 cWakeUpSysKeyInfo[25] = 
{
	//power key
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_A,		/* in  put port */
	GPIO_11,	/* in  put port */
	1,			/* 有效值 */
	
	//扫描键左
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_D,		/* in  put port */
	GPIO_5,		/* in  put port */
	0,			/* 有效值 */

	//扫描键右
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_D,		/* in  put port */
	GPIO_12,	/* in  put port */
	0,			/* 有效值 */
	
	//fun1 key
	0,		/* out put port */
	0,		/* out put gpio */
	PORT_F,		/* in  put port */
	GPIO_31,	/* in  put port */
	0,			/* 有效值 */

#if 1
	//ok key
	PORT_A,		/* out put port */
	GPIO_12,	/* out put gpio */
	PORT_E,		/* in  put port */
	GPIO_30,	/* in  put port */
	0,			/* 有效值 */
#endif
};


void drvKey_ClearScanTime()
{
	if(gu8AutoScan == 1)//自动扫描
	{
		hyhwScanner_TrigCtrl(0);
		gsScanKeyPushTime = 50;
		gu32LastAutoScanTime = rawtime(NULL);
	}
	else if(gblVar_GetScanType() == 1)
	{
		hyhwScanner_TrigCtrl(0);
		gsScanKeyPushTime = -1;
	}
}

/*-------------------------------------------------------
* 函数: drvKeyb_ScanInitialize
* 功能: Initialize the key matrix scan module
* 返回: cSSA_OK, initialization is successful 
* 参数:      
---------------------------------------------------------*/
U32 drvKeyb_ScanInitialize( void )
{
	U32 i;
	U32 port, iobit;
	
	//power_key 设为浮空
	hyhwGpio_setAsGpio(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	hyhwGpio_pullDisable(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	hyhwGpio_setIn(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	
	//其他IO键设为输入上拉
	for(i=1; i < KEY_IO_INPUT_SINGLE_NUM; i++)
	{
		port = gtIOInputSingle[i].port;
		iobit = gtIOInputSingle[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setPullUp(port, iobit);
		hyhwGpio_setIn(port, iobit);
	}

#ifndef JTAG_DEBUG
	//输入口初始化
	for(i=0; i<KEY_IO_INPUT_NUM; i++)
	{
		port = gtIOInput[i].port;
		iobit = gtIOInput[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setPullUp(port, iobit);
		hyhwGpio_setIn(port, iobit);
	}
	
	//输出口初始化
	for(i=0; i<KEY_IO_OUTPUT_NUM; i++)
	{
		port = gtIOOutput[i].port;
		iobit = gtIOOutput[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setOut(port, iobit);
		hyhwGpio_setHigh(port, iobit);
	}
#endif
	
	gu8AutoScan = 0;
	return cSSA_OK;
}


/*------------------------------------------------------------------------
* 函数: drvKeyb_IsLocked
* 功能: To determine whether the local keypad should be locked.
* 返回: TRUE, the local keys are locked & no events will be generated.
        FALSE, the local keys are operated normally.
* 参数: 
--------------------------------------------------------------------------*/
U32 drvKeyb_IsLocked( void )
{
	return 0;
}


/*------------------------------------------------------------------------
* 函数: drvKeybPowerOnKeyPressed
* 功能: 检测开机键是否按下
* 参数: none
* 返回: 1------键按下
*		0------键没有按下
-------------------------------------------------------------------------*/
U32 drvKeybIOinputSingleKeyPressed(U8 index)
{
	U32 rc = 0;

	if (hyhwGpio_Read(gtIOInputSingle[index].port, gtIOInputSingle[index].iobit) == gtIOInputSingle[index].iook)
	{
	 	rc = 1;
	}

	return rc;
}
/*------------------------------------------------------------------------
* 函数: drvKeybPowerOnKeyPressed
* 功能: 检测开机键是否按下
* 参数: none
* 返回: 1------开机键按下
*		0------开机键没有按下
-------------------------------------------------------------------------*/
U32 drvKeybPowerOnKeyPressed(void)
{
	U32 rc = 0;
	U32 powerKeyIndex;

	powerKeyIndex = 0;
	if (hyhwGpio_Read(gtIOInputSingle[powerKeyIndex].port, gtIOInputSingle[powerKeyIndex].iobit) == gtIOInputSingle[powerKeyIndex].iook)
	{
	 	rc = 1;
	}

	return rc;
}

/*------------------------------------------------------------------------
* 函数: drvKeyb_Scan()
* 功能: 扫描按键状态
* 返回: 得到按下的键 用bit来表示某个按键
* 参数: none
--------------------------------------------------------------------------*/
S32 drvKeyb_Scan( void )
{
	unsigned int status = 0;
	unsigned int bit = 1;
	unsigned int i, j;
	int scanKey=0;
	static int laser = 0;

	/* I/O键检测，power键 */
	for(i = 0 ; i < KEY_IO_INPUT_SINGLE_NUM; i++)
	{
		if(0 != drvKeybIOinputSingleKeyPressed(i))
		{
			status |= bit;
		}
		
		bit <<= 1;
	}
	
#ifndef JTAG_DEBUG
	/* 矩阵键盘扫描 */
	for(i=0; i<KEY_IO_OUTPUT_NUM; i++)
	{
		hyhwGpio_setLow(gtIOOutput[i].port, gtIOOutput[i].iobit);

		/* 读取KEY_INPUT_PORT所有I/O值 */
		for (j=0; j<KEY_IO_INPUT_NUM; j++)
		{
			if(hyhwGpio_Read(gtIOInput[j].port, gtIOInput[j].iobit) == gtIOInput[j].iook)
			{
				status |= bit;
			}
			bit <<= 1;
		}
		
		hyhwGpio_setHigh(gtIOOutput[i].port, gtIOOutput[i].iobit);
	}
#endif
	
	if((status&BIT3)&&(status&BIT5))//中间键+*
	{
		if(gu8AutoScan == 0)//自动扫描关
		{
			laser = 0;
			gu8AutoScan = 2;
			gsScanKeyPushTime = 0;
			//Led_ctrl(LED_RED);
			hyhwScanner_TrigCtrl(1);
		}
		else if(gu8AutoScan == 1)
		{
			//hyhwScanner_TrigCtrl(0);
			//laser = 0;
			gu8AutoScan = 3;
			//gsScanKeyPushTime = 0;
			//Led_ctrl(LED_GREEN);
		}
		status &= ~BIT5;
	}
	if(gu8AutoScan == 1)
	{
		if(ABS(rawtime(NULL)-gu32LastAutoScanTime) >= CLOSE_AUTOSCAN_MAXTIME) //关自动扫描
		{
			//hyhwScanner_TrigCtrl(0);
			//laser = 0;
			gu8AutoScan = 3;
			//gsScanKeyPushTime = 0;
			//Led_ctrl(LED_CLOSE);
		}
	}
	
	//是否允许扫描
	if(Grap_GetCurDesk_ScanEnable() > 0)
	{
		//扫描trig IO控制
		scanKey = (status&BIT1||status&BIT2||status&BIT3);
		if(gu8AutoScan == 0)//自动扫描关
		{
			if(laser == 0 && scanKey != 0)
			{//opticon扫描头驱动需要不断调用激光触发函数(用于计数和超时用)
				hyhwScanner_TrigCtrl(1);
				if(gsScanKeyPushTime == 0) gsScanKeyPushTime = 300;
				else gsScanKeyPushTime--;
				laser = 1;
				//Led_ctrl(LED_RED);
			}
			else if(laser == 1 && scanKey == 0 && (gblVar_GetScanType() == 0 || gsScanKeyPushTime == -1))
			{
				hyhwScanner_TrigCtrl(0);
				laser = 0;
				gsScanKeyPushTime = 0;
				//Led_ctrl(LED_GREEN);
			}
			else
			{
				if(gsScanKeyPushTime > 0) gsScanKeyPushTime--;
				if(laser == 1 && gsScanKeyPushTime == 0)
				{
					hyhwScanner_TrigCtrl(0);
					laser = 0;
					//Led_ctrl(LED_BLUE);
				}
			}
		}
		else if(gu8AutoScan == 2 && scanKey == 0)
		{
			gu8AutoScan = 1;
			gu32LastAutoScanTime = rawtime(NULL);
			//Led_ctrl(LED_BLUE);
		}
		else if(gu8AutoScan == 3 && scanKey == 0)
		{
			gu8AutoScan = 0;
			hyhwScanner_TrigCtrl(0);
			laser = 0;
			gsScanKeyPushTime = 0;
			//Led_ctrl(LED_YELLOW);
		}
		else if(gu8AutoScan == 1)//自动扫描
		{
			if(gsScanKeyPushTime == 0)
			{
				gsScanKeyPushTime = 300;
				laser = 0;
			}
			else
			{
				//模拟扫描按键按下
				if(gsScanKeyPushTime > 0) gsScanKeyPushTime--;
				if (laser == 0 && gsScanKeyPushTime >= 50)
				{
					hyhwScanner_TrigCtrl(1);
					laser = 1;
				}
				else if(laser == 1 && gsScanKeyPushTime < 50)
				{
					hyhwScanner_TrigCtrl(0);
					laser = 0;
				}
			}
			
			/*if(ABS(rawtime(NULL)-gu32LastAutoScanTime) >= CLOSE_AUTOSCAN_MAXTIME || ((status&BIT3)&&(status&BIT5))) //关自动扫描
			{
				hyhwScanner_TrigCtrl(0);
				laser = 0;
				gu8AutoScan = 3;
				gsScanKeyPushTime = 0;
				Led_ctrl(LED_CLOSE);
			}*/
		}
		
		//status &= ~(BIT1|BIT2|BIT3);
	}
	/*else if(gu8AutoScan == 1)
	{
		hyhwScanner_TrigCtrl(0);
		laser = 0;
		gu8AutoScan = 0;
		gsScanKeyPushTime = 0;
		Led_ctrl(LED_CLOSE);
	}
	else
	{
		if(Grap_GetCurDeskType() != 2 && (status&BIT3))//不是对话框
		{
			//焦点为不能扫描的控件时,把中间键转成确定键
			status &= (~BIT3);
			status |= BIT14;
		}
	}*/
	
    return status;
} 

//0:停止检测 1:开始检测
void drvBadScanKey_check(U8 enable)
{
	gCheckScanKeyF = enable ;
}

