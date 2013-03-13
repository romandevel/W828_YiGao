#include "SysSetting.h"


/*-----------------------------------------------------------------
marco def
-----------------------------------------------------------------*/
#define POINT_VERIFY_MIN_OFFSET	 (150)
#define POINT_VERIFY_MAX_OFFSET  (3000)
/*-----------------------------------------------------------------
globe para :
-----------------------------------------------------------------*/
static U8     sAdjFlag = 0; //开机的时候是否需要矫正触摸屏
static U8     ADnumber = 0; //屏幕焦点切换项
tPOINT RectBuf[5]  ; //前5个为原始点，后5个为计算后的点

/*
*各点击的公共属性
*/
const U16 AdjScrPoint[][6]=
{
	{  0,  0, 0,  0,  240,  320},
	{216,  0, 0,  0,  240,  320},
	{  0,296, 0,  0,  240,  320},
	{216,296, 0,  0,  240,  320},
	{107,147, 0,  0,  240,  320}
};
const tPOINT srcPoint[5]=
{
	{12,  12},
	{227, 12},
	{12,  307},
	{227, 307},
	{118, 158}
};
/* 校验小图标 24X24 */
const U8 picBitmap[72] = 
{
	0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
	0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,
	0x00,0xFF,0xE7,0xFF,0xFF,0xE7,0xFF,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,
	0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
	0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00
};


/*==============================================================================*/
const tGrapViewAttr    gtLcdCheckVAttr[] = 
{	
	{COMM_DESKBOX_ID, 	0,  0, 240,320,   0,  0,  0,  0, 	0,0,0,0,1,0,0,0},/*背景   */
	{BUTTON_ID_1, 		0,184, 240, 50,   0,  0,  0,  0, 	0,0,0,0,1,0,0,0},/*提示   */
	{BUTTON_ID_2, 		0,  0,  27, 25,   0,  0,240,320,  	0,0,0,0,1,0,0,0},
};
const tGrapButtonPriv gtLcdCheckBPro[]=
{
	{NULL,COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,0,NULL},
	{NULL,COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,0,0,NULL}
};

S32 SetLcdVerify_ButnDraw0(void *pView, U32 xy, U32 wh)
{		
	return SUCCESS;
}

S32 SetLcdVerify_ButnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight;
	drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
	drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
	drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
	gu16Lcd_ColumnLimit=gLcd_Total_Column;
	drvLcd_DisplayString(184,24,0,(U8*)"触摸屏校正请点击图标中心", NULL, 0);
	drvLcd_DisplayString(202,24,0,(U8*)"     按返回键退出", NULL, 0);
	drvLcd_WriteBMPColor(picBitmap,24,24,viewY1,viewX1);
	//Grap_Clear_Screen(viewX1, viewY1, viewX2,viewY2);
		
	return SUCCESS;
}


/*---------------------------------------------------------
*函数: SetLcdVerify_ButnEnter
*功能: 屏幕校验算法 原理：屏是电阻屏，采集的是ADC值, ADC值随X方向,Y方向增大
*                         算出一个单位ADC值所对应的像素的个数作为系数
*参数: wu
*返回: 无
*---------------------------------------------------------*/
S32 SetLcdVerify_ButnEnter(void *pView, U16 state)
{
	tGRAPBUTTON    *pBtn ;
	int flag = 0;
	U32 dertx , derty ;
	U16 pointX, pointY;
	
	int rv = HY_OK;
	int px,py;

    pBtn=(tGRAPBUTTON*)pView;
    
    if (state == STATE_NORMAL && BUTTON_ID_1 == pBtn->view_attr.id)
	{
		return RETURN_QUIT ;
	}
    
    ts_get_cur_adc(&px,&py);
    
	RectBuf[ADnumber].x = px;
	RectBuf[ADnumber].y = py;
	switch(ADnumber)
	{
	case 1:
		dertx = ABS((S16)(RectBuf[1].x-RectBuf[0].x));
		derty = ABS((S16)(RectBuf[0].y-RectBuf[1].y));
		//第二点与第一点比较 y 值			
		if (dertx <= POINT_VERIFY_MIN_OFFSET && derty > POINT_VERIFY_MAX_OFFSET)
		{
			flag = 1;
		}
		break;
	case 2:
		dertx = ABS((S16)(RectBuf[2].x-RectBuf[0].x));
		derty = ABS((S16)(RectBuf[2].y-RectBuf[0].y));
		//第三点与第一点比较 x 值
		if (derty <= POINT_VERIFY_MIN_OFFSET && dertx > POINT_VERIFY_MAX_OFFSET)
		{
			flag = 1;
		}
		break;
	case 3:
		dertx = ABS((S16)(RectBuf[3].x-RectBuf[2].x)) ;
		derty = ABS((S16)(RectBuf[3].y-RectBuf[1].y)) ;
		//第四点与第二点比较 x 值，与第三点比较 y 值
		if (dertx <= POINT_VERIFY_MIN_OFFSET && derty <= POINT_VERIFY_MIN_OFFSET)
		{
			dertx = ABS((S16)(RectBuf[3].x-RectBuf[1].x)) ;
			derty = ABS((S16)(RectBuf[3].y-RectBuf[2].y)) ;
			if(dertx > POINT_VERIFY_MAX_OFFSET && derty > POINT_VERIFY_MAX_OFFSET)
			{
				flag = 1;
			}
		}
		break;
	case 4:
		//第五点的 x 值与第一点和第四点的 x 平均值比较
		//第五点的 y 值与第一点和第四点的 y 平均值比较
		pointX = (RectBuf[0].x+RectBuf[3].x)/2;
		pointY = (RectBuf[0].y+RectBuf[3].y)/2;
					
		if ((ABS((S16)(RectBuf[4].x-pointX)) <= POINT_VERIFY_MIN_OFFSET) &&
			(ABS((S16)(RectBuf[4].y-pointY)) <= POINT_VERIFY_MIN_OFFSET))
		{							    
			
			//第五点的 x 值与第二点和第三点的 x 平均值比较
			//第五点的 y 值与第二点和第三点的 y 平均值比较
			pointX = (RectBuf[1].x+RectBuf[2].x)/2;
			pointY = (RectBuf[1].y+RectBuf[2].y)/2;
			
			if ((ABS((S16)(RectBuf[4].x-pointX)) <= POINT_VERIFY_MIN_OFFSET) &&
				(ABS((S16)(RectBuf[4].y-pointY)) <= POINT_VERIFY_MIN_OFFSET))
			{
				
				//计算偏移
				S16 offset_x, offset_y;
				float coff_x, coff_y;
				int rc, i;
				int width, height;
				
				rc = ts_calculate_offset_coef(&RectBuf[0],&srcPoint[0], &offset_x, &offset_y, &coff_x, &coff_y);
				if (rc == HY_OK)
				{
					gtHyc.touchParam.TouchOffsetX = offset_x;
					gtHyc.touchParam.TouchOffsetY = offset_y;
					gtHyc.touchParam.TouchCoffX = coff_x;
					gtHyc.touchParam.TouchCoffY = coff_y;
					ts_parameter_set(&gtHyc.touchParam);
					
					ADnumber = 0;
					glbVariable_SaveParameter();
					
					Com_SpcDlgDeskbox("屏幕校验完成!",0,pBtn->pParent,1,NULL,NULL,100);
                    
                    return RETURN_QUIT;   //返回上一层
                }
            }
		}
		break;
	default:
		flag = 1;
		break;
	}
	
	if (flag == 1)
		ADnumber++;
	else
		ADnumber = 0;
	pBtn->view_attr.viewX1	= AdjScrPoint[ADnumber][0];
	pBtn->view_attr.viewY1	= AdjScrPoint[ADnumber][1];
	pBtn->view_attr.touchX1	= AdjScrPoint[ADnumber][2];
	pBtn->view_attr.touchY1	= AdjScrPoint[ADnumber][3];
	pBtn->view_attr.touchX2	= AdjScrPoint[ADnumber][4];
	pBtn->view_attr.touchY2	= AdjScrPoint[ADnumber][5];
  
  return RETURN_REDRAW;
}

S32 SetLcdVerify_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
    tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
    
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点
	     ret = VIEW_PULL_START;
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//在有效范围内
	        	
		     ret = VIEW_PULL_ING; 
	     }
	     else
	     {
	        pButton->state = STATE_NORMAL;
		    pButton->draw(pButton,0,0);
	     }
	}
	else if (VIEW_PULL_END == extParam)
	{//up
	    
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}
		else if (ret == RETURN_FOCUSMSG)
		{
			return ret;
		}			
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0); 
		
		ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;  
 
}

/*---------------------------------------------
*函数:Set_ScreenVerify
*功能:屏幕校正
*参数:
*返回:
*-----------------------------------------------*/
void Set_ScreenVerify(void *pDeskFather)
{
	tGRAPDESKBOX   *pDesk;
	tGRAPBUTTON    *pBtn;
    tGRAPLABEL     *pLabel;
    U8              i;
    
	drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
	drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
	drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
	gu16Lcd_ColumnLimit=gLcd_Total_Column;
	drvLcd_DisplayString(184,24,0,(U8*)"触摸屏校正请点击图标中心", NULL, 0);
	drvLcd_DisplayString(202,24,0,(U8*)"     按返回键退出", NULL, 0);
	DisplayData2Screen();
    
    pDesk  =  Grap_CreateDeskboxNoCom(&gtLcdCheckVAttr[0], 0); 
	if ( NULL == pDesk ) return;
	
	Grap_Inherit_Public(pDeskFather,pDesk);

	pBtn = Grap_InsertButton(pDesk, &gtLcdCheckVAttr[1],&gtLcdCheckBPro[0]);
	if(NULL == pBtn) return;
	pBtn->pressEnter = SetLcdVerify_ButnEnter;
	pBtn->draw = SetLcdVerify_ButnDraw0 ;
	
	pBtn = Grap_InsertButton(pDesk, &gtLcdCheckVAttr[2],&gtLcdCheckBPro[1]);
	if(NULL == pBtn) return;
	pBtn->pressEnter = SetLcdVerify_ButnEnter;
	pBtn->draw = SetLcdVerify_ButnDraw ;
	pBtn->recvPullMsg = SetLcdVerify_ButtonRecvPullMsg;

    Grap_DeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
