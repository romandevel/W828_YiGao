#include "Test_MainList.h"

////////////////////////////////   待完善...
////test
extern U8   gu8AutoScan;


const U8 * const TestFunStr[]=
{
	"蜂鸣器",	"震动",	
};


U8	gu8TestStep;
U32 gu32TestTime;

tGRAPBUTTON *gpTestAutoPhotoBtn=NULL;
char *gpTeatAutoWorkBuf, *gpTestAutoPreviewBuf;//拍照时用
tGrapButtonBuffPic  gtTestAutoRgbbuf;
/*=============================================拍照测试界面===================================================*/
const tGrapViewAttr TeatAuto_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//拍照
	
	{BUTTON_ID_3,	   90,  100 ,80, 60,   0,0,0,0,   0,0,0,0,1,0,0,0},//图片预览
	
	{EDIT_ID_1,		29,60,186,24,  29,100,215,164,   0,0,0xffff,0,1,1,1,0},
	
	{LABEL_ID_1, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
};

const tGrapButtonPriv TeatAuto_BtnPriv[]=
{
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"进入拍照",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//拍照
	{NULL,		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//图片预览
};

/*edt 属性*/
const tGrapEditPriv	  TeatAuto_EdtAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,6,23,  1,30,0,0,1,0,0,1,   50,50,0,  0,0,1},
};

const tGrapLabelPriv TeatAuto_LabPriv[]=
{
	{NULL,0,0,0,0,0,0,0},
};

int TeatAuto_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	U32 i,size;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://拍照
		if(gu8TestStep != 2) break;
		
		gpTeatAutoWorkBuf = (char *)hsaSdram_UpgradeBuf();
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gpTeatAutoWorkBuf = DC_Desk(pDesk,gpTeatAutoWorkBuf,&size);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		if (size > 0)
		{
			//Test_WriteLog(gpTeatAutoWorkBuf, size);			
			memset(&gtTestAutoRgbbuf, 0, sizeof(tGrapButtonBuffPic));
			if(0 == Photo_Decode(gpTeatAutoWorkBuf,size,60,80, gpTeatAutoWorkBuf+size,gpTestAutoPreviewBuf,TRUE))
			{				
				gtTestAutoRgbbuf.rgb = gpTestAutoPreviewBuf;
		    	gtTestAutoRgbbuf.off_x1 = 0;
		    	gtTestAutoRgbbuf.off_y1 = 0;
		    	gtTestAutoRgbbuf.width = 80;
		    	gtTestAutoRgbbuf.height = 60;
		    	gpTestAutoPhotoBtn->btn_privattr.pBufPic = (tGrapButtonBuffPic *)&gtTestAutoRgbbuf;
		    }
		    else
		    {
		    	Com_SpcDlgDeskbox("预览图解码失败!",6,pDesk,2,NULL,NULL,150);
		    }
		}
		else
		{
			Com_SpcDlgDeskbox("拍照失败!",6,pDesk,2,NULL,NULL,150);
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

S32 TeatAuto_BtnDestroy(void *pView)
{
	tGRAPBUTTON *pButton;
	
	pButton = (tGRAPBUTTON *)pView;
	free((void *)pButton);
	
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: TeatAuto_LabDraw
*功能: 空函数不进行刷新   
*参数: none
*返回: none
*---------------------------------------------------------*/
static S32 TeatAuto_LabDraw(void *pView, U32 xy, U32 wh)
{
	 return SUCCESS;
}

/*---------------------------------------------------------
*函数: TeatAuto_LabTimer
*功能: 定时器函数，各个应用可以放入改函数下面   
*参数: none
*返回: none
*---------------------------------------------------------*/
static void TeatAuto_LabTimer(void *pView)
{
    tGRAPDESKBOX	*pDesk;
    tGRAPBUTTON 	*pBtn;
    tGRAPLABEL		*pLable;
    U32				now;
    
    pLable = (tGRAPLABEL *)pView;
    pDesk = ((tGRAPLABEL*)pView)->pParent;
	
	now = rawtime(NULL);
	if(ABS(now-gu32TestTime) > 3000)
	{
		gu8TestStep++;
		if(gu8TestStep > 2)
		{
			pLable->label_timer.enable = 0;
			return ;
		}
		
		//测试项
		switch(gu8TestStep)
		{
		case 1://扫描
			gu8AutoScan = 1;
			break;
		case 2://DC
			gu8AutoScan = 0;
			pBtn = (tGRAPEDIT *)Grap_GetViewByID(pDesk, BUTTON_ID_1);
			pBtn->pressEnter(pBtn,STATE_NORMAL);
			break;
		default:
			
			break;
		}
		gu32TestTime = rawtime(NULL);
	}
}

/*-------------------------------------------------------------
* 函数:TestFun_Auto
* 功能:自动测试
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_Auto(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT 		*pEdt;
	tGRAPLABEL		*pLable;
	int i, idx = 0;
	
	gu8AutoScan = 1;
	
	gu8TestStep = 0;
	gu32TestTime = 0;
	
	gpTeatAutoWorkBuf = (char *)hsaSdram_UpgradeBuf();
	gpTestAutoPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
	
	pDesk = Grap_CreateDeskbox(&TeatAuto_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	pDesk->scanEnable = 1;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TeatAuto_ViewAttr[idx++], &TeatAuto_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		if(i < 2)
		{
			pBtn->pressEnter = TeatAuto_BtnEnter;
		}
		else if(i == 2)
		{
			gpTestAutoPhotoBtn = pBtn;
			pBtn->destroy = TeatAuto_BtnDestroy;
		}
	}
	
	pEdt= Grap_InsertEdit(pDesk, &TeatAuto_ViewAttr[idx++], &TeatAuto_EdtAttr[0]);
	if (NULL == pEdt)
	{
		return -1;
	}
	
	pLable = Grap_InsertLabel(pDesk, &TeatAuto_ViewAttr[0], &TeatAuto_LabPriv[0]);
	if(NULL == pLable) return;
	pLable->draw = TeatAuto_LabDraw;
	pLable->label_timer.enable = 0;
	pLable->label_timer.TimerLft = 100;
	pLable->label_timer.TimerTot = 100;
	pLable->labelTimerISR = TeatAuto_LabTimer;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}


