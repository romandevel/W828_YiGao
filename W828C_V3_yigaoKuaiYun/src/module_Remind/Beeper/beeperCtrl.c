/*-------------------------------------------------------------------------
* 
* 用进程来处理蜂鸣器  及播放蜂鸣器的接口
* 使用 方法 : 直接调用 beeper_Start() ,并填入音乐名及参数 ,便可播放,播放完会
*             会自动停止,也可强行停止 ,自己可以编辑音乐放入数组中,以增加音乐
*
*  注意     : 1.优先级高的进程调用会有问题 
*             2.现在最小节奏是syssleep(1) 即 10ms
* -------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
* inlcude
* -------------------------------------------------------------------------*/
#include "std.h"
#include "ospriority.h"
#include "sys.h"
#include "semaphore.h"
#include "LedCtl.h"
#include "beeperCtrl.h"
#include "appControlProcess.h"
#include "hardWare_Resource.h"

/*-------------------------------------------------------------------------
* macro
* -------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
* globe
* -------------------------------------------------------------------------*/
SEMAPHORE *beeperExit_sem = NULL;

BEEP_MUSIC_DATA_st *gpstBeepData = NULL;
S16 gBeepCnt;
U16 gRhythmCnt;

U8	gBeepEnable;
U8	gLedEnable;

/*-------------------------------------------------------------------------
* declear
* -------------------------------------------------------------------------*/
void beeper_Running(void) ;
void beeper_Stop(void);

/*-------------------------------------------------------------------------
* 函数: beeper_PwmInit
* 说明: 初始化并配置蜂鸣器的频率
* 参数: 无
* 返回: 无
* -------------------------------------------------------------------------*/
void beeper_PwmInit(int period)
{
	hyhwGpio_setAsGpio(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setAsPwmFunction(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwTimer_setPwm(BEEPER_PWM1_TIMER, 1000000/period, 50);
}
/*-------------------------------------------------------------------------
* 函数: beeper_PwmDeinit
* 说明: 不使能蜂鸣器
* 参数: 无
* 返回: 无
* -------------------------------------------------------------------------*/
void beeper_PwmDeinit(void)
{
	hyhwGpio_setAsGpio(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setOut(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setHigh(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwTimer_pwmDisable(BEEPER_PWM1_TIMER);
}
/*-------------------------------------------------------------------------
* 函数: beeper_Init
* 说明: 初始化蜂鸣器
* 参数: 无
* 返回: 无
* -------------------------------------------------------------------------*/
void beeper_Init(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable)
{	
	//gpstBeepData = pBeep_data;
	gpstBeepData = (BEEP_MUSIC_DATA_st *)beepData[beep_type];
	gBeepCnt = -1;
	/* 准备启动鸣叫，这里赋1，以便在终端中进行声调(PWM)配置 */
	gRhythmCnt = 1;		//
	
	gBeepEnable = beepEnable;
	gLedEnable = ledEnable;
}

/*-------------------------------------------------------------------------
* 函数: beeper_Start
* 说明: 启动蜂鸣器
* 参数: 
* 返回: 无
* -------------------------------------------------------------------------*/
int beeper_Start(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable)
{
	int pid, startValue, pidStatus = HY_ERROR;
	int ret;
	
	if (gpstBeepData != NULL || beep_type == 0xff) return HY_ERROR;
	
	/*pid = ntopid(BEEPER_PRO_NAME);
	if (pid>0 && pid<30)
	{
		//进程存在，判断状态
		if (PSPROC(pid)->state == P_INACT)
		{
			//deletep(pid);//删除后再创建
			pidStatus = HY_OK;//进程存在且处于exit状态，则直接启动
		}
		else
		{
			return HY_ERROR;//进程还在运行，直接退出，当前设置无效
		}
	}*/
	beeper_Stop();
	beeperExit_sem = semaphore_init(0);
	
	if (pidStatus != HY_OK)
	{		
		beeper_Init(beep_type,beepEnable,ledEnable);
		//进程不存在，需要创建
		pid = createp((int (*)())beeper_Running,
								BEEPER_PRO_NAME,
								BEEPER_SIZE,
								BEEPER_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)beeper_Running );

	return HY_OK;
	
}
/*-------------------------------------------------------------------------
* 函数: beeper_Running
* 说明: 蜂鸣器主处理进程
* 参数: 无
* 返回: 无
* -------------------------------------------------------------------------*/
void beeper_Running(void)
{
	int tone;
	int ret ,powerState ;
	U32 start = rawtime(NULL);
	
	while(1)
	{		
		ret = semaphore_trywait(beeperExit_sem);
		if (ret == 0) /* 手动强行退出 */
		{			
			break;
		}
		
		
		/* 有旋律需要鸣叫 */
		gRhythmCnt--;
		if (gRhythmCnt == 0)
		{
			/* 节拍时间到，配置下一个鸣叫声调 */
			gBeepCnt++;
			tone = gpstBeepData[gBeepCnt].tone;
			/* 提取声调 */
			if(tone == 0) /* 音乐播放完 自动退出 */
			{
				break;
			}
			else if (tone == 0xFF)/* 休止符，不鸣叫，停PWM */
			{			
				beeper_PwmDeinit();
				gRhythmCnt = gpstBeepData[gBeepCnt].rhythm;
				if (gLedEnable != 0)
				{
					Led_ctrl(0);
				}
			}
			else /* 乐音,就配置PWM频率 */
			{			
				if (gBeepEnable != 0)
				{
					beeper_PwmInit(beepTone[tone]);
				}
				gRhythmCnt = gpstBeepData[gBeepCnt].rhythm;
				if (gLedEnable != 0)
				{
					Led_ctrl(LED_GREEN);
				}
			}			
		}		
		syssleep(1) ;		
	}
	Led_ctrl(0);
	gpstBeepData = NULL;
	beeper_PwmDeinit();
	
	semaphore_destroy(beeperExit_sem, OS_DEL_NO_PEND);
	beeperExit_sem = NULL;
	
}
/*-------------------------------------------------------------------------
* 函数: beeper_Stop
* 说明: 停止蜂鸣器
* 参数: 无
* 返回: 无
* -------------------------------------------------------------------------*/
void beeper_Stop(void)
{
	int pid;
	
	pid = ntopid(BEEPER_PRO_NAME);
	if (pid<0 || pid>30) return;
	
	if (beeperExit_sem != NULL)
		semaphore_post(beeperExit_sem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*-------------------------------------------------------------------------
* 函数: beeper_State
* 说明: 蜂鸣器状态
* 参数: 无
* 返回: 1---正在运行
*       0---没运行
* -------------------------------------------------------------------------*/
int beeper_State(void)
{
	int pid;
	
	pid = ntopid(BEEPER_PRO_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}


void beeper_test(void)
{
	beeper_Start(SETTING_ERR,1,0);
}

