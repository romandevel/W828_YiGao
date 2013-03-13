#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwTimer.h"
#include "hyhwGpio.h"

#include "beeperCtrl.h"

/*
高八度
音符		频率	(Hz)	频率差值		周期	(us)
1		1581.8		0			632.2
2		1744.3		162.5		573.3
3		1906.8		162.5		524.4
4		2014.8		108			496.3
5		2213.8		217			451.7
6		2448.5		216.7		408.4
7		2719.3		270.8		367.7
i		2881.8		162.5		347

正常		+250Hz
音符		频率	(Hz)	频率差值		周期	(us)
1		1831.8		0			545.9
2		1994.3		162.5		501.4
3		2156.8		162.5		463.6
4		2264.8		108			441.5
5		2463.8		217			405.9
6		2698.5		216.7		370.6
7		2969.3		270.8		336.8
i		3131.8		162.5		319.3

低八度	-250Hz
音符		频率	(Hz)	频率差值		周期	(us)
1		1331.8		0			750.9
2		1494.3		162.5		669.2
3		1656.8		162.5		603.6
4		1764.8		108			566.6
5		1963.8		217			509.2
6		2198.5		216.7		454.9
7		2469.3		270.8		405
i		2631.8		162.5		380
*/

#if 0
//2.7k
const U16 beepTone[] =
{
	0, 751, 669, 604, 567, 509, 455, 405, 380, 0,	//L
	0, 546, 501, 464, 442, 406, 371, 337, 319, 0,	//N
//	0, 632, 573, 524, 496, 452, 408, 368, 347, 0,	//H
	0, 573, 524, 496, 452, 408, 368, 347, 320, 0,	//H
};
#endif
#if 0
// 2k
const U16 beepTone[] =
{
	0, 751, 669, 604, 567, 509, 455, 405, 380, 0,	//L
	0, 546, 501, 464, 442, 406, 371, 337, 319, 0,	//N
	0, 726, 677, 649, 594, 550, 510, 488, 460, 0,	//H
};
#endif
#if 0
// 2200
const U16 beepTone[] =
{
	0, 751, 669, 604, 567, 509, 455, 405, 380, 0,	//L
	0, 546, 501, 464, 442, 406, 371, 337, 319, 0,	//N
	0, 580, 546, 520, 504, 480, 465, 454, 440, 0,	//H
};
#endif
#if 1
// 1500
const U16 beepTone[] =
{
	0, 751, 669, 604, 567, 509, 455, 405, 380, 0,	//L
	0, 546, 501, 464, 442, 406, 371, 337, 319, 0,	//N
	0, 900, 850, 790, 740, 704, 685, 667, 650, 0,	//H
};
#endif

//各状态的鸣叫提示音
//扫描成功
const BEEP_MUSIC_DATA_st scanOk[]=
{
	27, 10,
	0, 0
};

//扫描成功
const BEEP_MUSIC_DATA_st keySound[]=
{
	18, 5,
	0, 0
};

//检测到扫描枪，并初始化成功
const BEEP_MUSIC_DATA_st scannerPowerOnOk[]=
{
	26, 10,
	0xFF, 10,
	27, 10,
	0xFF, 10,
	28, 10,
	0, 0
};
//有数据等待发送
const BEEP_MUSIC_DATA_st waitTransData[]=
{
	28, 10,
	0xFF, 10,
	27, 10,
	0, 0
};
//低电压报警
const BEEP_MUSIC_DATA_st lowPower[]=
{
	27, 15,
	0xFF, 5,
	27, 15,
	0xFF, 5,
	27, 15,
	0, 0
};
//低电压关机报警
const BEEP_MUSIC_DATA_st lowPowerDown[]=
{
	28, 15,
	0xFF, 5,
	27, 15,
	0xFF, 5,
	26, 15,
	0xFF, 5,
	25, 15,
	0xFF, 5,
	24, 15,
	0xFF, 5,
	23, 15,
	0, 0
};
//设置成功
const BEEP_MUSIC_DATA_st settingOk[]=
{
	26, 10,
	0xFF, 10,
	27, 10,
	0xFF, 10,
	28, 10,
	0, 0
};
//设置失败
const BEEP_MUSIC_DATA_st settingErr[]=
{
	27, 10,
	0xFF, 5,
	27, 10,
	0xFF, 5,
	27, 10,
	0xFF, 5,
	27, 10,
	0xFF, 5,
	27, 10,
	0xFF, 5,
	27, 10,
	0, 0
};

/*------------------------------------------------------*/
//保留1
const BEEP_MUSIC_DATA_st reserved1[]=
{
	27, 10,
	0xFF, 10,
	27, 10,
	0xFF, 10,
	0, 0
};
//保留2
const BEEP_MUSIC_DATA_st reserved2[]=
{
	26, 10,
	0xFF, 10,
	27, 10,
	0xFF, 10,
	0, 0
};
//保留3
const BEEP_MUSIC_DATA_st reserved3[]=
{
	27, 10,
	0xFF, 10,
	26, 10,
	0xFF, 10,
	0, 0
};
/*
//保留4
const BEEP_MUSIC_DATA_st reserved4[]=
{
	24, 50,
	0xFF, 10,
	25, 15,
	0xFF, 10,
	26, 15,
	0xFF, 10,
	27, 15,
	0xFF, 10,
	28, 15,
	0, 0
};*/

const BEEP_MUSIC_DATA_st* const beepData[] =
{
	&scanOk[0],
	&scannerPowerOnOk[0],
	&waitTransData[0],
	&lowPower[0],
	&lowPowerDown[0],
	&settingOk[0],
	&settingErr[0],
	&keySound[0],
	&reserved1[0],
	&reserved2[0],
	&reserved3[0],
/*	&reserved4[0]
    */
};

