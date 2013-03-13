#include "Test_MainList.h"

U8 gau8GpsState[10];
U8 gau8GpsUart[10];
U8 gau8GpsLong[15];
U8 gau8GpsLat[15];
U8 gau8GpsStar[4];
/*=============================================GPS界面===================================================*/
void TestGps_SetLong(U8 *Long)
{
	strcpy(gau8GpsLong, Long);
}

void TestGps_SetLat(U8 *Lat)
{
	strcpy(gau8GpsLat, Lat);
}

void TestGps_SetStar(U8 *Star)
{
	strcpy(gau8GpsStar, Star);
}

const tGrapViewAttr TestGPS_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景

	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//启动
	
	{BUTTON_ID_3,	   10,40,64,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//状态
	{BUTTON_ID_4,	   80,40,160,16,  0,0,0,0,   0,0,0,0,1,0,0,0},
	
	{BUTTON_ID_5,	   10,60,64,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//串口数据
	{BUTTON_ID_6,	   80,60,160,16,  0,0,0,0,   0,0,0,0,1,0,0,0},
	
	{BUTTON_ID_7,	   10,80,64,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//经度
	{BUTTON_ID_8,	   80,80,160,16,  0,0,0,0,   0,0,0,0,1,0,0,0},
	
	{BUTTON_ID_9,	   10,100,64,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//纬度
	{BUTTON_ID_10,	   80,100,160,16,  0,0,0,0,   0,0,0,0,1,0,0,0},
	
	{BUTTON_ID_11,	   10,120,64,16,  0,0,0,0,   0,0,0,0,1,0,0,0},//星数
	{BUTTON_ID_12,	   80,120,160,16,  0,0,0,0,   0,0,0,0,1,0,0,0},
	
	{LABEL_ID_1, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
};

const tGrapButtonPriv TestGPS_BtnPriv[]=
{
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"启动",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//启动
	
	{"状态",	0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//状态
	{gau8GpsState,		COLOR_YELLOW,COLOR_YELLOW,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},
	
	{"串口数据",0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//串口数据
	{gau8GpsUart,		COLOR_YELLOW,COLOR_YELLOW,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},
	
	{"经度",	0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//经度
	{gau8GpsLong,		COLOR_YELLOW,COLOR_YELLOW,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},
	
	{"纬度",	0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//纬度
	{gau8GpsLat,		COLOR_YELLOW,COLOR_YELLOW,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},
	
	{"星数",	0xffff,0x0,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},//星数
	{gau8GpsStar,		COLOR_YELLOW,COLOR_YELLOW,FONTSIZE8X16,0,1,0,  TY_UNITY_BG1_EN,NULL},
};

const tGrapLabelPriv TestGPS_LabPri[]=
{
	{NULL,0,0,0,0,0,0,0},
};

int TestGPS_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT2		*pEdit2;
	U32 len;
	U8 *pCode, *pUnic;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://启动
			if(1 == Gps_isRunning())
			{
				Gps_Init();
			}
			break;
		case BUTTON_ID_2://返回
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}


S32 TestGps_lab_draw(void *pView, U32 xy, U32 wh)
{
	 return SUCCESS;
}

void TestGps_lab_timer_isr(void *pView)
{
    tGRAPDESKBOX	*pDesk;
    U32 uartLen;
    
    pDesk = ((tGRAPLABEL*)pView)->pParent;
    
	//状态
	if(0 == Gps_isRunning())
	{
		sprintf(gau8GpsState, "已启动");
		
		//串口数据
		uartLen = GpsUart_DataLen();
		sprintf(gau8GpsUart, "%d", uartLen);
	}
	else
	{
		sprintf(gau8GpsState, "未启动");
	}
	
	ReDraw(pDesk, 0, 0);
}

/*-------------------------------------------------------------
* 函数:TestFun_GPS
* 功能:GPS测试
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_GPS(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLABEL *pLable;
	int i, idx = 0;
	U16 	lastValue;
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 192);
	lastValue = BackLight_CloseScreenProtect();
hyUsbPrintf("gps lastval = %d \r\n", lastValue);	
	memset(gau8GpsState, 0, sizeof(gau8GpsState));
	memset(gau8GpsUart, 0, sizeof(gau8GpsUart));
	memset(gau8GpsLong, 0, sizeof(gau8GpsLong));
	memset(gau8GpsLat, 0, sizeof(gau8GpsLat));
	memset(gau8GpsStar, 0, sizeof(gau8GpsStar));
		
	pDesk = Grap_CreateDeskbox(&TestGPS_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 12; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestGPS_ViewAttr[idx++], &TestGPS_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		if(i < 2)
		{
			pBtn->pressEnter = TestGPS_BtnEnter;
		}
	}
	
	pLable = Grap_InsertLabel(pDesk, &TestGPS_ViewAttr[idx++], &TestGPS_LabPri[0]);
	if(NULL == pLable) return;
	pLable->draw = TestGps_lab_draw;
	pLable->label_timer.enable = 1;
	pLable->label_timer.TimerLft = 1;
	pLable->label_timer.TimerTot = 100;
	pLable->labelTimerISR = TestGps_lab_timer_isr;	
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	Gps_DeInit();
	
	BackLight_SetScreenProtect(lastValue);
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	return 0;
}
