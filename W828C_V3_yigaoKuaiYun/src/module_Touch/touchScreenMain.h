#ifndef __TOUCH_SCREEN_MAIN_H__
#define __TOUCH_SCREEN_MAIN_H__

#include "hyTypes.h"
//#include "appControlProcess.h"

#include "HycDeviceConfig.h"

//触摸偏移非法值
#define INVALID_OFFSET			(1000)
//触摸扩展系数非法值
#define INVALID_TOUCH_COFF		(10)

// 1 是正常的G22屏 
// 2 Y值反的屏
//#define TOUCH_SCREEN_TYPE      (2)

//#if TOUCH_SCREEN_TYPE == 1
//触摸相关系数值
#if	defined HYC_DEV_LCD_HX8347
    #define TOUCH_DEFAULT_X_COEF    (0.067606)
    #define TOUCH_DEFAULT_X_OFF     (-18)

    #define TOUCH_DEFAULT_Y_COEF    (0.092352)
    #define TOUCH_DEFAULT_Y_OFF     (-19)
//#elif TOUCH_SCREEN_TYPE == 2
#elif defined HYC_DEV_LCD_N2748TLY

	#define TOUCH_DEFAULT_X_COEF    (0.063291)//(0.066852)
    #define TOUCH_DEFAULT_X_OFF     -11//(-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.086475)
    #define TOUCH_DEFAULT_Y_OFF     (-18)
#elif defined HYC_DEV_LCD_8877N_BYD

	#define TOUCH_DEFAULT_X_COEF    (-0.0659263)//(0.066666)
    #define TOUCH_DEFAULT_X_OFF     256//-15

    #define TOUCH_DEFAULT_Y_COEF    (0.0850271)//(0.086580)//(0.086475)
    #define TOUCH_DEFAULT_Y_OFF     -15//0//(-18)
#elif defined HYC_DEV_LCD_TIANMA

#elif defined HYC_DEV_LCD_R61505V
    #define TOUCH_DEFAULT_X_COEF    (0.066852)
    #define TOUCH_DEFAULT_X_OFF     (-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.091954)
    #define TOUCH_DEFAULT_Y_OFF     (-22)     
#elif defined HYC_DEV_LCD_HXRGB
	#define TOUCH_DEFAULT_X_COEF    0.0846561//(0.066852)
    #define TOUCH_DEFAULT_X_OFF     -12//(-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.0688468)
    #define TOUCH_DEFAULT_Y_OFF     (-23)
#endif

//拖拽功能相关
#define PULL_RING_BUFFER_LEN	50	//保存拖拽有效点的个数
#define PULL_POIONT_SETP        15   //x,y方向偏移最小值，大于该值，认为点有效

//拖拽点状态
#define VIEW_PULL_FAIL       30
#define VIEW_PULL_START      31
#define VIEW_PULL_ING        32
#define VIEW_PULL_END        33
#define VIEW_PULL_TIMEOUT    34


//无效点数值
#define PULL_INVALID_POINT_X    0xFFFF        
#define PULL_INVALID_POINT_Y    0xFFFF        

//拖拽点数值
#define PULL_END_POINT_X    0xFFFE        
#define PULL_END_POINT_Y    0xFFFE

//拖拽点写入buf的方式
#define PULL_WRITE_NORMAL   1  //正常写入拖拽点
#define PULL_WRITE_END      2  //写入抬起点
#define PULL_WRITE_LONG     3  //写入长按点


//触摸笔处于idel,开始触摸，触摸中
#define PEN_IDEL      (0)
#define PEN_START     (1)
#define PEN_WORKING   (2)

//BUF 写状态标志，用于判断是否为第一个点
#define PEN_WR_IDEL     (0)
#define PEN_WR_WORKING  (1)


//长按状态
#define PRESS_LONG_IDEL    (0)
#define PRESS_LONG_START   (1)
#define PRESS_LONG_WORKING (2)

//长按最长计数
#define PRESS_LONG_MAX_CNT  (30)

typedef struct _tPOINT
{
    unsigned short x;
    unsigned short y;
}tPOINT;

//拖拽点或点触点
typedef struct _tPOINT_KEY_PULL
{
    U16 x;
    U16 y;
    U32 endFlag;
}tPOINT_KEY_PULL;

typedef struct _tTOUCH_CTR
{
     void  *pTouchEvent;
     volatile unsigned int lastTick; //上次有触摸点的时间点
     unsigned char  penFSM; //触摸笔状态，用于触发抬起或拖拽
     unsigned char  status; //触摸拖拽的状态
     unsigned char  wrBufState;//记录点信息状态标志
     unsigned char  pointStep; //有效的触摸点距离
     unsigned char  longPressFsm; //长按状态标志
     unsigned int   longPressTick; //长按开始计数的tick数
     unsigned int   longPressCnt; //长按计数
     int   longPressMax; //长按最大计数
     
     volatile unsigned char  num; //触摸的点个数
     tPOINT  pointSaveBuf[PULL_RING_BUFFER_LEN];
     tPOINT  lastPoint;
     tPOINT  *pRead;
     tPOINT  *pWrite;
       
}tTOUCH_CTR;

typedef struct touchParameter_st
{
	S16 TouchOffsetX;//触摸屏固定偏移
	S16 TouchOffsetY;
	float TouchCoffX;//触摸屏扩展系数
	float TouchCoffY;
} tTOUCH_PARAMETER;

/*-----------------------------------------------------------------------------
* 函数:	ts_main(void);
* 功能:	adc主循环
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_main(void);

/*-----------------------------------------------------------------------------
* 函数:	ts_wake_collect_position
* 功能:	定时去采集坐标位置
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_wake_collect_position(void);

/*---------------------------------------------------------------------------
 函数：ts_get_touch_pull_position
 功能：提供给控制台获取拖拽点的接口
 参数：pKeyPullPiont: 获取指针
 返回：HY_OK  OR  HY_ERROR
* -----------------------------------------------------------------------------*/
int ts_get_touch_pull_position(tPOINT_KEY_PULL *pKeyPullPiont);

/*---------------------------------------------------------------------------
 函数：ts_get_point_num
 功能：还有多少个触摸点
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
U8 ts_get_point_num(void);

#endif