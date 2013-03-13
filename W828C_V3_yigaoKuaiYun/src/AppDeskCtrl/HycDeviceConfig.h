#ifndef __HYC_DEVICE_CONFIG_H__
#define __HYC_DEVICE_CONFIG_H__

//该头文件主要用于配置硬件设备驱动的，请不要加其他宏参数

//#define HYCO_VERSION			"W828  1.1"
//#define HYCO_VERSION_APP		HYCO_VERSION##".008"
//#define HYCO_VERSION_INFO		"Version "##HYCO_VERSION_APP##" Build on "##__DATE__##" "##__TIME__

/*
W828  XAL  x.xxx.xxx
W828为硬件机型，占用5个字符，不足者以空格补齐，如W828  , W560  ....
XAL为客户项目缩写，占用5个字符，不足者已空格补齐，XAL  代表旭安隆，LB   代表龙邦......
x.xxx.xxx纯数字，蓝色字体的三位数代表标准版的版本号，粉红色字体代表各个客户的版本号，从0.001依次递增
*/
#define HYCO_VERSION			"W828 HYCO 1.20"
#define HYCO_VERSION_APP		HYCO_VERSION##"1.009"
#define HYCO_VERSION_INFO		"Version "##HYCO_VERSION_APP##" Build on "##__DATE__##" "##__TIME__

extern const char appVersionInfo_all[];
extern const char appVersionInfo[];

//宏开关用于配置生成的程序
//#define RELEASE_HX
//#define RELEASE_TY
//#define RELEASE_TM
//#define RELEASE_R6//TG2
//#define RELEASE_HXRGB
//#define RELEASE_BYD
#define RELEASE_BYD_8877N
//#define RELEASE_BCT_HX8347
//#define DEBUG_HX
//#define DEBUG_TY
//#define DEBUG_TM
//#define DEBUG_R6//TG2
//#define DEBUG_HXRGB
//#define DEBUG_BYD

//触摸板
#define HYC_DEV_TP_JC	//键烁

//-DUSE_WIFI

//摄像头配置
//#define HYC_DEV_SENSOR_130
//#define HYC_DEV_SENSOR_300
//#define HYC_DEV_SENSOR_NANO_300
//#define HYC_DEV_SENSOR_OV7690 //30W
//#define HYC_DEV_SENSOR_OV9660 //
//#define HYC_DEV_SENSOR_OV3640 //300W

//#define HYC_DEV_SENSOR_OV2655
//#define HYC_DEV_SENSOR_HM2055
#define HYC_DEV_SENSOR_HM2057

#if 0
//Lcd 驱动配置
#if  defined (RELEASE_HX)|| defined(DEBUG_HX)
    #define HYC_DEV_LCD_HX8347
#elif defined(RELEASE_TY) || defined(DEBUG_TY)   
    #define HYC_DEV_LCD_N2748TLY
#elif defined(RELEASE_TM) || defined(DEBUG_TM) 
    #define HYC_DEV_LCD_TIANMA
#elif defined(RELEASE_R6) || defined(DEBUG_R6)     
    #define HYC_DEV_LCD_R61505V
#elif defined(RELEASE_HXRGB) || defined(DEBUG_HXRGB)    
    #define HYC_DEV_LCD_HXRGB
#elif defined(RELEASE_BYD) || defined(DEBUG_BYD)    
    #define HYC_DEV_LCD_BYD
#endif
#else//#if 0
#if defined(RELEASE_TY) || defined(DEBUG_TY)
	#define HYC_DEV_LCD_N2748TLY
#endif
#if defined(RELEASE_BYD) || defined(DEBUG_BYD)
	#define HYC_DEV_LCD_BYD
#endif
#if defined(RELEASE_BYD_8877N) || defined(DEBUG_BYD_8877N)
	#define HYC_DEV_LCD_8877N_BYD
#endif
#if defined(RELEASE_BCT_HX8347) || defined(DEBUG_BCT_HX8347)
	#define HYC_DEV_LCD_HX8347_BCT
#endif
#endif//#if 0

//选择背光驱动芯片
#define CP2142MM	//在一段时间内给出的脉冲数来决定背光亮度，用在并联背光上(G20/G20W/W818C)
//#define ACT6311		//依靠调节pwm的占空比来调整背光亮度，用在串联背光上(W900/W900B)

//WATCHDOG 配置
//发布版本需要使用watchdog
#define HYC_DEV_USE_WATCHDOG


//硬件低功耗配置
#define HYC_DEV_USE_LOW_PS


#endif