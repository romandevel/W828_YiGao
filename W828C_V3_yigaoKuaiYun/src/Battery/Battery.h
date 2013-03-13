#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "hyhwAdc.h"

//4.2v == 0x2c9  4v == 0x2aa 	  0x1f
/*
*define variable
*/

#define ADCCHANNEL_BATTERY			ADC_CH0 

//#define ADC_MAX_DIFF       	0x226



//应GSM要求3.55V就开始关机，电量显示 <3.6V 0格

#if 1
////3.3参考电压
#define POWER_LEVEL_FORBID  (0x229)	//3.56V，允许开机电压
#define POWER_PRE           (0x1F5)//3.22V涓流充电电压

#define POWER_LEVEL_OFFV3   0x222 //3.50V,关机电压，小于等于此电压关机
#define POWER_LEVEL_0V3     0x229 //3.55V,显示一格时的电压 小于此电压提示电量低

#define POWER_LEVER_LOW     0x238  //3.65V 以上显示一格  
 
#define POWER_LEVER_1V3     0x241 //3.7V,以上显示二格时的电压
#define POWER_LEVER_2V3     0x24f //3.8V,以上显示三格时的电压

#define POWER_LEVER_3V3     0x269 //3.98V,以上显示四格时的电压
#define POWER_LEVER_FULL    0x274 //4.05V,大于等于此电压提示电量满

#define BATTERY_DETECT_CNT	(10)


//各次提示的电压值
#define POWER_TIP1_LEV  (0x22E) // 3.6V提示电量低信息
#define POWER_TIP2_LEV  (0x228)
#define POWER_TIP3_LEV  (0x222)

#define POWER_DOWN_LEV  (0x226) //3.55V 开始准备关机

//大功耗设备禁止运行的参考电压
#define POWER_PERMIT_RUN_LEV 0x222//(0x235) //3.6V禁止打印机GSM高功耗设备运行


#else
////3.0参考电压
#define POWER_LEVEL_FORBID  (0x259)//(0x211)	//3.56V，允许开机电压
#define POWER_PRE           (0x236)//3.22V涓流充电电压



#define POWER_LEVEL_OFFV3   0x251//0x222 //3.50V,关机电压，小于等于此电压关机
#define POWER_LEVEL_0V3     0x259//0x229 //3.55V,显示一格时的电压 小于此电压提示电量低

#define POWER_LEVER_LOW     0x26A//0x238  //3.65V 以上显示一格  
 
#define POWER_LEVER_1V3     0x272//0x241 //3.7V,以上显示二格时的电压
#define POWER_LEVER_2V3     0x283//0x24f //3.8V,以上显示三格时的电压

//#define POWER_LEVER_3V3     0x294 //3.9V,以上显示四格时的电压
#define POWER_LEVER_3V3     0x2A2//0x269 //3.98V,以上显示四格时的电压
#define POWER_LEVER_FULL    0x2AE//0x274 //4.05V,大于等于此电压提示电量满

#define BATTERY_DETECT_CNT	(10)


//各次提示的电压值
#define POWER_TIP1_LEV  (0x261)//(0x22E) // 3.6V提示电量低信息
#define POWER_TIP2_LEV  (0x25e)//(0x228)
#define POWER_TIP3_LEV  (0x25a)//(0x222)

#define POWER_DOWN_LEV  (0x259)//(0x226) //3.55V 开始准备关机

//大功耗设备禁止运行的参考电压
#define POWER_PERMIT_RUN_LEV (0x261)//(0x235) //3.6V禁止打印机GSM高功耗设备运行
#endif

//电池测试表值仅供参考
/*
3.51v			平均值：256
				当前值：250 251 252 253
				
3.53v			平均值：25B
				当前值：254 255 256 257
				
3.55v			平均值：25B
				当前值：257 258 259 25A
				
3.56v			平均值：25c
				当前值：25A 25B 25C 25D
				
3.58v			平均值：25F
				当前值：25E 25F 260 261	
							
3.60v			平均值：265
				当前值：261 262 263 264	
							
3.62v			平均值：266
				当前值：264 265 266 267	
											
3.64v			平均值：269
				当前值：268 269 26A
					
3.66v			平均值：26C
				当前值：26A 26B 26D 26E
				
3.68v			平均值：26F
				当前值：26E 26F 270
				
3.70v			平均值：274
				当前值：271 272 273
				
3.72v			平均值：278
				当前值：277 278 279
				
3.74v			平均值：27A
				当前值：278 279 27A	
							
3.76v			平均值：27E
				当前值：279 27A 27B 27D	
							
3.78v			平均值：284
				当前值：27E 280 281	
											
3.80v			平均值：287
				当前值：282 283 285 286	
				
3.83v			平均值：28B
				当前值：287 288 289 28A	
														
3.86v			平均值：28F
				当前值：28E 28F 290					
				
3.89v			平均值：295
				当前值：293 294 295 296					
				
3.92v			平均值：29B
				当前值：29A 29B 29C	
				
3.95v			平均值：29D
				当前值：29E 29F 29D				
				
3.98v			平均值：2A6
				当前值：2A1 2A2 2A3 2A4					
				
4.01v			平均值：2AB
				当前值：2A7 2A8 2A9	
				
4.04v			平均值：2AF
				当前值：2AB 2AC 2AD				
				
4.07v			平均值：2B1
				当前值：2B0 2B1 2B2 2B3					
				
4.10v			平均值：2BA
				当前值：2B7 2B8 2B9	
*/


/*-----------------------------------------------------------------------------
* 函数:	Battery_Detect()
* 功能:	检测电池电压
* 参数:	none
* 返回:	读到的实际电压值
*----------------------------------------------------------------------------*/
U32  Battery_Detect(void);
/*-----------------------------------------------------------------------------
* 函数:	Battery_Level()
* 功能:	检测电池等级
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
U32 Battery_Level(void);

/*-----------------------------------------------------------------------------
* 函数:	Battery_If_StartPowerOff()
* 功能:	开机的时候检测是否小于3.1V如果小于该值，则直接关电池
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void Battery_If_StartPowerOff(void);

#endif