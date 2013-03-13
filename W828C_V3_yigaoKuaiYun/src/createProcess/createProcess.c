#include "hyTypes.h"
#include "std.h"
#include "stdarg.h"
#include "hyhwGPIO.h"
#include "hyhwCkc.h"

#include "LedCtl.h"

#include "ospriority.h"
#include "hardWare_Resource.h"
#include "AbstractFileSystem.h"
#include "appControlProcess.h"
#include "HycDeviceConfig.h"

//U8 appImageFlag = 0;

extern int appControlDesk_process(void);	

void hyhwBatteryPowerOnOff(U32 flag);

const U32 NULL_GPIO[][2] = 
{
	{PORT_C,	GPIO_16},
	{PORT_C,	GPIO_17},
	{PORT_C,	GPIO_18},
	{PORT_C,	GPIO_19},
	{PORT_C,	GPIO_20},
	{PORT_C,	GPIO_21},
	{PORT_C,	GPIO_22},
	{PORT_C,	GPIO_23},
	{PORT_C,	GPIO_24},
	{PORT_C,	GPIO_31},
	
	{PORT_D,	GPIO_0},
	
	//{PORT_E,	GPIO_4},
	{PORT_E,	GPIO_28},
	
	{PORT_F,	GPIO_13},
	{PORT_F,	GPIO_14},
	{PORT_F,	GPIO_28},
	//{PORT_F,	GPIO_29},//BM0
	//{PORT_F,	GPIO_30},//BM1
	
	{0xFF,	0xFF}
};

//一些空置的IO初始化为输入
void hyhwGpioDisable()
{
#if 1
	int i = 0;
	
	while(1)
	{
		if(NULL_GPIO[i][0] == 0xFF)
		{
			break;
		}
		
		hyhwGpio_setAsGpio(NULL_GPIO[i][0], NULL_GPIO[i][1]);
		//hyhwGpio_setIn(NULL_GPIO[i][0], NULL_GPIO[i][1]);
		//hyhwGpio_pullDisable(NULL_GPIO[i][0], NULL_GPIO[i][1]);
		hyhwGpio_setOut(NULL_GPIO[i][0], NULL_GPIO[i][1]);
		hyhwGpio_setLow(NULL_GPIO[i][0], NULL_GPIO[i][1]);
		i++;
	}
#endif
}

void enable_IF(void)
{
	int tmp;
	__asm
	{
		MRS tmp, CPSR
		BIC tmp, tmp, #0xC0
		MSR CPSR_c, tmp
	}
}


//电源维持
void hyhwBatteryPowerOnOff(U32 flag)
{
	hyhwGpio_setAsGpio(PWR_HOLD_PORT, PWR_HOLD_GPIO);
	hyhwGpio_setOut(PWR_HOLD_PORT, PWR_HOLD_GPIO);

	if (flag == 1)
		hyhwGpio_setHigh(PWR_HOLD_PORT, PWR_HOLD_GPIO);
	else
		hyhwGpio_setLow(PWR_HOLD_PORT, PWR_HOLD_GPIO);
}


void hyhwInitialSysClk(void)
{
	hyhwSysClk_Set(ICLK192M_AHB96M, EXTAL_CLK);
}

void HardWare_Init(void)
{
	hyhwSystem_powerCtrl(EnableOn);
	hyhwBatteryPowerOnOff(1);//打开电源维持
	
	hyhwGpioDisable();//所有空置的IO设置为输入状态
	
	Led_init();
	Led_ctrl(LED_RED);
	
	hyhwCmosSensor_Reset(EnableOn);
	hyhwCmosSensor_powerCtrl(EnableOff);//进入powerdown

    hySdram_autoPowerSave();
    hyhwAdc_Init();

	hyhwRtc_Init();
	Battery_Init();

	LcdModulePowerOnOff(0);
	hyhwLcd_powerCtrl(EnableOn);
	//---------------------cup 屏
	drvLcd_InitCPU();
    drvLcd_InitLCDModule();
    hyhwDma_lcdCpuConfig();
  
	hyhwUart_PortPowerSave();
		
	hyhwScanner_TrigInit();
}


//软件上进行overlay并初始化变量
//文件系统初始化，磁盘检测
static void Sys_SoftIntial(void)
{	
	IE_InitToSdram();
	Gsm_InitToSdram();
	Wifi_InitToSdram();
	
}

int CreateProcessor(void)
{
	/*这个下面加创建进程的内容*/
	int pid;
	int startValue;
	
    wLock_NandFlashInit();
    wLock_I2cInit();
    
	Sys_SoftIntial();
	
	HardWare_Init();
	
    pid = createp((int (*)())appControlDesk_process,
                CTRL_APP_NAME,
                CTRL_APP_SIZE,
                CTRL_APP_PRIOPITY,
                NULL,NULL,NULL);
    startValue = pstart( pid, (void *)appControlDesk_process );

	_exit(1);
	return (1);
}

