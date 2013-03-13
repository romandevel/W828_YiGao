#include "hyTypes.h"
#include "hyhwGpio.h"
#include "touchScreenMain.h"
#include "glbVariable_base.h"

#define TOUCH_MAX_ADCX   (4095)
#define TOUCH_MAX_ADCY   (4095)

static tTOUCH_CTR tTouchCtr; 

tTOUCH_PARAMETER gtTouchParam;

//这个变量用于校正显示屏时使用
static U16 gu16CurAdcX,gu16CurAdcY;

void ts_parameter_set(tTOUCH_PARAMETER *pTouchParam)
{
	if (pTouchParam != NULL)
	{
		memcpy(&gtTouchParam, pTouchParam, sizeof(tTOUCH_PARAMETER));
	}
}

void ts_parameter_get(tTOUCH_PARAMETER *pTouchParam)
{
	if (pTouchParam != NULL)
	{
		memcpy(pTouchParam, &gtTouchParam, sizeof(tTOUCH_PARAMETER));
	}
}

/*-----------------------------------------------------------------------------
* 函数:	ts_exchange_pos
* 功能:	当横竖屏转换的时候重新计算触摸的位置
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
static void ts_exchange_pos(U16 *pX, U16 *pY)
{
   U16 TempEx=0;
   if(1 == LcdModule_Get_ShowType())
   {
#ifdef DEBUG
   	 TempEx = *pY;
   	 *pY = *col;
   	 *pX = 319-TempEx;
#else
     
     TempEx=239-*pX;
     *pX=*pY;
     *pY=TempEx; 
     
#endif     
   }
} 

/*---------------------------------------------------------------------------
 函数：ts_is_valid_point
 功能：判断是否为有效的触摸点，防止有过多的点产生
 参数：x,y----点的位置
 返回：none
* -----------------------------------------------------------------------------*/
static int ts_is_valid_point(int x, int y)
{
	 int rv = 0;
	 U32 curTick;
	 U8 fsm;
	 
	 if (PEN_WR_IDEL == tTouchCtr.wrBufState)
	 {
	     return 1;
	 }
	 
	 curTick = rawtime(NULL);
	 
	 if ( ABS(tTouchCtr.lastPoint.x - x) > tTouchCtr.pointStep
        ||ABS(tTouchCtr.lastPoint.y - y) > tTouchCtr.pointStep)
     {
         rv = 1;
         tTouchCtr.longPressFsm = PRESS_LONG_IDEL;
     }
     else
     {//处于长按状态，需要改变处理方式
           
         fsm = tTouchCtr.longPressFsm;
          
         switch(fsm)
         {
         case PRESS_LONG_IDEL:
              tTouchCtr.longPressFsm = PRESS_LONG_START;
              tTouchCtr.longPressTick = curTick;
              tTouchCtr.longPressCnt = 0;
              break;
         case PRESS_LONG_START:
              if (ABS(curTick - tTouchCtr.longPressTick) > 3)
              {
                  tTouchCtr.longPressCnt = 0;
                  tTouchCtr.longPressFsm = PRESS_LONG_WORKING;
                  tTouchCtr.longPressMax = PRESS_LONG_MAX_CNT;
              }
              break;
         case PRESS_LONG_WORKING:
              if (++tTouchCtr.longPressCnt > tTouchCtr.longPressMax)
              {
                  tTouchCtr.longPressCnt = 0;
                  
                  if (tTouchCtr.longPressMax > 0)
                  {
                      tTouchCtr.longPressMax--;
                  }
                  
                  rv = 1;
              }
              break;
         }
     }    
     
     return rv;
}

/*---------------------------------------------------------------------------
 函数：ts_write_point
 功能：写入一个拖拽点
 参数：x,y----点的位置
 返回：none
* -----------------------------------------------------------------------------*/
static void ts_write_point(U16 x, U16 y, U32 type)
{
    tPOINT *pRead,*pWrite;
    
    
    //如果没有背光则关闭响应触摸 090305
    if (0 == BackLight_HaveLight())
    {    
        tTouchCtr.pRead  =  &tTouchCtr.pointSaveBuf[0];
        tTouchCtr.pWrite =  &tTouchCtr.pointSaveBuf[0];
        tTouchCtr.num = 0;
    }
    
    if (tTouchCtr.num >= PULL_RING_BUFFER_LEN)  return;
    
    switch (type)
    {
    //需要过滤掉一些触摸点，并且在手写时需要这些点
    case  PULL_WRITE_LONG:            
    case  PULL_WRITE_NORMAL:   	
     	ts_exchange_pos(&x,&y);
     	
     	if (1 == ts_is_valid_point(x,y))
     	{  
         	tTouchCtr.lastPoint.x = x;
         	tTouchCtr.lastPoint.y = y;
         	
         	tTouchCtr.pWrite->x = x;
         	tTouchCtr.pWrite->y = y;
         	tTouchCtr.pWrite++;
         	if (tTouchCtr.pWrite >= &tTouchCtr.pointSaveBuf[PULL_RING_BUFFER_LEN])
         	{
         	    tTouchCtr.pWrite =  &tTouchCtr.pointSaveBuf[0];
         	}
         	tTouchCtr.num++;
         	
         	tTouchCtr.wrBufState = PEN_WR_WORKING;
     	}
    	break;
    	
   case  PULL_WRITE_END:
   
        tTouchCtr.wrBufState = PEN_WR_IDEL;
        tTouchCtr.longPressFsm = PRESS_LONG_IDEL;
        
   	    tTouchCtr.pWrite->x = tTouchCtr.lastPoint.x;
        tTouchCtr.pWrite->y = tTouchCtr.lastPoint.y;
     	tTouchCtr.pWrite++;
     	if (tTouchCtr.pWrite >= &tTouchCtr.pointSaveBuf[PULL_RING_BUFFER_LEN])
     	{
     	    tTouchCtr.pWrite =  &tTouchCtr.pointSaveBuf[0];
     	}
     	tTouchCtr.num++;
     	
     	if (tTouchCtr.num >= PULL_RING_BUFFER_LEN)  return;
     	
     	tTouchCtr.pWrite->x = x;
        tTouchCtr.pWrite->y = y;
     	tTouchCtr.pWrite++;
     	if (tTouchCtr.pWrite >= &tTouchCtr.pointSaveBuf[PULL_RING_BUFFER_LEN])
     	{
     	    tTouchCtr.pWrite =  &tTouchCtr.pointSaveBuf[0];
     	}
     	tTouchCtr.num++;
       break;
        	
    default:
        break;    
    }
}

/*-----------------------------------------------------------------------------
* 函数:	ts_calculate_position
* 功能:	把得到的adc值转换成坐标位置
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
static void ts_calculate_position(U16 adcX, U16 adcY)
{
     int posX,posY;
     int posTem;

#if	defined HYC_DEV_LCD_HX8347     
     //Y = aX+b
     // a = 0.068454  b = -21.083856
     // a = 0.109409  b = -26.367615 
     
	gu16CurAdcX = adcX;
	gu16CurAdcY = adcY;

	posX = gtTouchParam.TouchCoffX*adcX+gtTouchParam.TouchOffsetX;

	posX = posX <0 ? 0:(posX >= cLCD_TOTAL_COLUMN ? (cLCD_TOTAL_COLUMN-1): posX);

	posY = gtTouchParam.TouchCoffY*adcY+gtTouchParam.TouchOffsetY;

	posY = posY <0 ? 0:posY;
#elif defined HYC_DEV_LCD_8877N_BYD
	gu16CurAdcX = adcX;
	gu16CurAdcY = adcY;

	gtTouchParam.TouchOffsetX = TOUCH_DEFAULT_X_OFF;
	gtTouchParam.TouchOffsetY = TOUCH_DEFAULT_Y_OFF;
	gtTouchParam.TouchCoffX   = TOUCH_DEFAULT_X_COEF;
	gtTouchParam.TouchCoffY   = TOUCH_DEFAULT_Y_COEF;
	
	posX = gtTouchParam.TouchCoffX*adcX+gtTouchParam.TouchOffsetX;

	posX = posX <0 ? 0:(posX >= cLCD_TOTAL_COLUMN ? (cLCD_TOTAL_COLUMN-1): posX);

	posY = gtTouchParam.TouchCoffY*adcY+gtTouchParam.TouchOffsetY;

	posY = posY <0 ? 0:posY;
    
#elif defined HYC_DEV_LCD_N2748TLY

    gu16CurAdcX = adcX;
	gu16CurAdcY = adcY;

	posX = gtTouchParam.TouchCoffX*adcX+gtTouchParam.TouchOffsetX;

	posX = posX <0 ? 0:(posX >= cLCD_TOTAL_COLUMN ? (cLCD_TOTAL_COLUMN-1): posX);

	posY = gtTouchParam.TouchCoffY*adcY+gtTouchParam.TouchOffsetY;

	posY = posY <0 ? 0:posY;
	
	//hyUsbPrintf("xy:%d:%d,xy:%d:%d\r\n",posX,posY,adcX,adcY);
     
#elif defined HYC_DEV_LCD_TIANMA

#elif defined HYC_DEV_LCD_R61505V

	gu16CurAdcX = adcX;
	gu16CurAdcY = adcY;

	posX = gtTouchParam.TouchCoffX*adcX+gtTouchParam.TouchOffsetX;

	posX = posX <0 ? 0:(posX >= cLCD_TOTAL_COLUMN ? (cLCD_TOTAL_COLUMN-1): posX);

	posY = gtTouchParam.TouchCoffY*adcY+gtTouchParam.TouchOffsetY;

	posY = posY <0 ? 0:posY;
#elif defined HYC_DEV_LCD_HXRGB

	gu16CurAdcX = adcX;
	gu16CurAdcY = adcY;

//PhoneTrace(0,"adcx = %d adcY = %d ",adcX,adcY);

	posX = gtTouchParam.TouchCoffX*adcX+gtTouchParam.TouchOffsetX;

	//posX = posX <0 ? 0:(posX >= cLCD_TOTAL_COLUMN ? (cLCD_TOTAL_COLUMN-1): posX);
	posX = posX <0 ? 0:(posX >= cLCD_TOTAL_ROW ? (cLCD_TOTAL_ROW-1): posX);

	posY = gtTouchParam.TouchCoffY*adcY+gtTouchParam.TouchOffsetY;

	posY = posY <0 ? 0:posY;
	
	////////////////////////////
	posTem = posX ;
	posX = 240 - posY ;	
	posY = posTem ;


#endif   
     
     ts_write_point(posX,posY,PULL_WRITE_NORMAL);
}

/*-----------------------------------------------------------------------------
* 函数:	ts_is_valid_offset_coef
* 功能:	判断各个系数是否正确
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
int ts_is_valid_offset_coef(tTOUCH_PARAMETER *pTouchParam)
{
    int rv = HY_OK;
    float coefX;
    float coefY;
    
    coefX = pTouchParam->TouchCoffX;
    coefY = pTouchParam->TouchCoffY;


    if (coefX > 0.1 ||
    	coefX < 0.03 ||
    	coefY > 0.15 ||
    	coefY < (float)0.08 ||
    	ABS(pTouchParam->TouchOffsetX) > 70 ||
    	ABS(pTouchParam->TouchOffsetY) > 70)
    {
        //PhoneTrace(0,"%d,%d,%f,%f",offX,offY,coefX,coefY);
        
        return HY_ERROR;
    }
    
    return rv; 
}

/*-----------------------------------------------------------------------------
* 函数:	ts_calculate_offset_coef
* 功能:	判断各个系数是否正确 此处需要解一个 y = ax+b的线性方程组
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
int ts_calculate_offset_coef(tPOINT *pPointBuf, tPOINT *pOriPointBuf, S16 *pOffX, S16 *pOffY, float *pCoefX, float *pCoefY)
{
#if 1
	float a1,a2,a3,a4;
	int   b1,b2,b3,b4;
	
	//算4组,求平均
	//X  1,2
	a1 = ((float)(pOriPointBuf[1].x-pOriPointBuf[0].x)/(pPointBuf[1].x-pPointBuf[0].x));
	b1 = pOriPointBuf[0].x-a1*pPointBuf[0].x;
	
	//X  3,4
	a2 = ((float)(pOriPointBuf[3].x-pOriPointBuf[2].x)/(pPointBuf[3].x-pPointBuf[2].x));
	b2 = pOriPointBuf[2].x-a2*pPointBuf[2].x;
	
	//X  1,4
	a3 = ((float)(pOriPointBuf[3].x-pOriPointBuf[0].x)/(pPointBuf[3].x-pPointBuf[0].x));
	b3 = pOriPointBuf[0].x-a3*pPointBuf[0].x;
	
	//X  2,3
	a4 = ((float)(pOriPointBuf[2].x-pOriPointBuf[1].x)/(pPointBuf[2].x-pPointBuf[1].x));
	b4 = pOriPointBuf[1].x-a4*pPointBuf[1].x;
	
	*pCoefX = (a1+a2+a3+a4)/4;
	*pOffX  = (b1+b2+b3+b4)/4;
	
	
	//Y  1,3
	a1 = ((float)(pOriPointBuf[2].y-pOriPointBuf[0].y)/(pPointBuf[2].y-pPointBuf[0].y));
	b1 = pOriPointBuf[0].y-a1*pPointBuf[0].y;
	
	//Y  2,4
	a2 = ((float)(pOriPointBuf[3].y-pOriPointBuf[1].y)/(pPointBuf[3].y-pPointBuf[1].y));
	b2 = pOriPointBuf[1].y-a2*pPointBuf[1].y;
	
	//Y  1,4
	a3 = ((float)(pOriPointBuf[3].y-pOriPointBuf[0].y)/(pPointBuf[3].y-pPointBuf[0].y));
	b3 = pOriPointBuf[0].y-a3*pPointBuf[0].y;
	
	//Y  2,3
	a4 = ((float)(pOriPointBuf[2].y-pOriPointBuf[1].y)/(pPointBuf[2].y-pPointBuf[1].y));
	b4 = pOriPointBuf[1].y-a4*pPointBuf[1].y;
	
	*pCoefY = (a1+a2+a3+a4)/4;
	*pOffY  = (b1+b2+b3+b4)/4;
    
    
    //PhoneTrace(0,"xcoef = %f  off = %d   ycoef = %f  off = %d \r\n",*pCoefX,*pOffX,*pCoefY,*pOffY);
    //syssleep(1000);
    
	return 0;
#else
     float tempCoef;
     int totalX1,totalX2,oriTotalX1,oriTotalX2;
     int rc = HY_OK;
     
	 int realX0 ,realX1 ,realX2 ,realX3;
	 int realY0 ,realY1 ,realY2 ,realY3;
	
	 realX0 = pOriPointBuf[0].x ;
	 realX1 = pOriPointBuf[1].x ;
	 realX2 = pOriPointBuf[2].x ;
	 realX3 = pOriPointBuf[3].x ;
	 
	 realY0 = pOriPointBuf[0].y ;
	 realY1 = pOriPointBuf[1].y ;
	 realY2 = pOriPointBuf[2].y ;
	 realY3 = pOriPointBuf[3].y ;
	 
	 //逻辑坐标
     oriTotalX1 = (realX0 + realX2)>>1;  //最小值取平均
     oriTotalX2 = (realX1 + realX3)>>1;  //最大值取平均
     
     //物理坐标
     totalX1 = (pPointBuf[0].x + pPointBuf[2].x)>>1;     
     totalX2 = (pPointBuf[1].x + pPointBuf[3].x)>>1;
     
     tempCoef = ((float)(oriTotalX2 - oriTotalX1))/(totalX2-totalX1);
     
     *pOffX = realX0 - tempCoef*pPointBuf[0].x; 
     
     *pCoefX = tempCoef;  
     
     oriTotalX1 = (realY0 + realY1)>>1;
     oriTotalX2 = (realY2 + realY3)>>1;
     
     totalX1 = (pPointBuf[0].y + pPointBuf[1].y)>>1;
     totalX2 = (pPointBuf[2].y + pPointBuf[3].y)>>1;
     
     tempCoef = ((float)(oriTotalX2 - oriTotalX1))/(totalX2-totalX1);
     
     *pOffY = realY0 - tempCoef*pPointBuf[0].y;
          
     *pCoefY = tempCoef; 
     
     return rc;
#endif
}	
	

/*-----------------------------------------------------------------------------
* 函数:	ts_get_cur_position
* 功能:	主要用于得到触目点
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
int ts_get_cur_position(int *pX, int *pY)
{
	 int rv = HY_ERROR;
	 
	 //if (0 == hyhwAk4183_PenDetect() && 0 != tTouchCtr.num)
	 if (PEN_WORKING == tTouchCtr.penFSM && 0 == hyhwAk4183_PenDetect())
	 {
	 	  *pX =  tTouchCtr.lastPoint.x;
	 	  *pY =  tTouchCtr.lastPoint.y;
	 	  
	 	  rv = HY_OK;
	 }
	 else
	 {
	     *pX = -1;
	     *pY = -1;
	 }
	 
	 return rv;
}

/*-----------------------------------------------------------------------------
* 函数:	ts_get_last_position
* 功能:	得到最后一次坐标点
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_get_cur_adc(int *pX, int *pY)
{
     *pX = gu16CurAdcX;
     *pY = gu16CurAdcY;
}

/*-----------------------------------------------------------------------------
* 函数:	ts_set_valid_point_step
* 功能:	设置有效步长
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_set_valid_point_step(int step)
{
     tTouchCtr.pointStep = step;
}

/*-----------------------------------------------------------------------------
* 函数:	ts_main(void);
* 功能:	adc主循环
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_main(void)
{
     int adcX,adcY;
     
     memset(&tTouchCtr,0,sizeof(tTOUCH_CTR));
     
     tTouchCtr.pRead  =  &tTouchCtr.pointSaveBuf[0];
     tTouchCtr.pWrite =  &tTouchCtr.pointSaveBuf[0];
     tTouchCtr.penFSM =  PEN_IDEL;
     tTouchCtr.status =  VIEW_PULL_END;
     
     tTouchCtr.pointStep = PULL_POIONT_SETP;
     tTouchCtr.longPressFsm = PRESS_LONG_IDEL;
     
     while(1)
     {
          
          ewait(&tTouchCtr.pTouchEvent); 
          
          if (0 == hyhwAk4183_PenDetect())
          { 
              adcX = hyhwAk4138_Get_12BitOut(0);
              adcY = hyhwAk4138_Get_12BitOut(1); 
              
              //PhoneTrace2(0,"adcx = %d adcY = %d ",adcX,adcY);
              
              if (adcX != 4095 && adcY != 4095)
              {
                  //PhoneTrace(0,"%d,%d",adcX,adcY);
                  
                  ts_calculate_position(adcX,adcY);
              }
          }
          
          syssleep(1); /* 持续有点事件,让出tick给GSM 通讯使用 */
     }
}

/*-----------------------------------------------------------------------------
* 函数:	ts_wake_collect_position
* 功能:	定时去采集坐标位置
* 参数:	none
* 返回: none
*----------------------------------------------------------------------------*/
void ts_wake_collect_position(void)
{
    U32 curTick;
    U8 fsm;
    
    curTick = rawtime(NULL);
    
    fsm = tTouchCtr.penFSM;
    
    //当连续30ms检测不到有触摸事件事，就默认为是抬起点
    if (0 == hyhwAk4183_PenDetect())
    //if (1)
    {
         switch(fsm)
         {
         case  PEN_IDEL:
             tTouchCtr.lastTick = curTick;
             tTouchCtr.penFSM = PEN_START;
             break;
             
         case  PEN_START:
             if (ABS(curTick-tTouchCtr.lastTick) > 2)
             {
                 tTouchCtr.penFSM = PEN_WORKING;
             }                 
             break;
             
         case  PEN_WORKING:
             if (ABS(curTick-tTouchCtr.lastTick) >= 2)
             {
                ewakeup(&tTouchCtr.pTouchEvent);
                tTouchCtr.lastTick = curTick;
             }
             break;
         }
    }
    else
    {
        
        switch(fsm)
        {
        case  PEN_WORKING://需要人工添加结束点
             if (ABS(curTick-tTouchCtr.lastTick) > 5)
             {
                  ts_write_point(PULL_END_POINT_X, PULL_END_POINT_Y,PULL_WRITE_END);
                  tTouchCtr.penFSM = PEN_IDEL;
             }
             break;
             
        case  PEN_START:
         	 tTouchCtr.penFSM = PEN_IDEL;
             break;      
        }        
        
        return;
    }
}

/*---------------------------------------------------------------------------
 函数：ts_get_touch_pull_position
 功能：提供给控制台获取拖拽点的接口
 参数：pKeyPullPiont: 获取指针
 返回：HY_OK  OR  HY_ERROR
* -----------------------------------------------------------------------------*/
int ts_get_touch_pull_position(tPOINT_KEY_PULL *pKeyPullPiont)
{
    if (0 == tTouchCtr.num) 
    {
        return HY_ERROR;
    }
    
    if (tTouchCtr.status == VIEW_PULL_END)
	{
	    tTouchCtr.status = VIEW_PULL_START;    
	}
	else if (tTouchCtr.status == VIEW_PULL_START)
	{
	    tTouchCtr.status = VIEW_PULL_ING;    
	}
	else if (tTouchCtr.status == VIEW_PULL_ING)
	{
	}
	else
	{
	     tTouchCtr.status =  VIEW_PULL_END;
	} 
	
	pKeyPullPiont->endFlag = tTouchCtr.status;
	pKeyPullPiont->y = tTouchCtr.pRead->y;
	pKeyPullPiont->x = tTouchCtr.pRead->x;
	tTouchCtr.pRead++;
	tTouchCtr.num--;
	
	if (tTouchCtr.pRead >= &tTouchCtr.pointSaveBuf[PULL_RING_BUFFER_LEN])
	{
		tTouchCtr.pRead =  &tTouchCtr.pointSaveBuf[0];
	}
	
	if (tTouchCtr.num >0 && tTouchCtr.pRead->x == PULL_END_POINT_X &&
		tTouchCtr.pRead->y == PULL_END_POINT_Y)
	{
		//抬起点
		pKeyPullPiont->endFlag = VIEW_PULL_END;
		tTouchCtr.status = VIEW_PULL_END;
		
		tTouchCtr.pRead++;
	    tTouchCtr.num--;
	    if (tTouchCtr.pRead >= &tTouchCtr.pointSaveBuf[PULL_RING_BUFFER_LEN])
    	{
    		tTouchCtr.pRead =  &tTouchCtr.pointSaveBuf[0];
    	}
	}
	
	return HY_OK;
}



/*---------------------------------------------------------------------------
 函数：ts_get_point_num
 功能：还有多少个触摸点
 参数：none
 返回：none
* -----------------------------------------------------------------------------*/
U8 ts_get_point_num(void)
{
    return tTouchCtr.num; 
}



