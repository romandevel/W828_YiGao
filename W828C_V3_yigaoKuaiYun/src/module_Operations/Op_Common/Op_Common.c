#include "Op_Common.h"
#include "Rec_Info.h"
#include "Op_Types.h"


extern const U8 * const Net_ErrorCode[];



tGRAPBUTTON *gpPhotoBtn=NULL;
U8 	 gau8PhotoCode[BARCODE_LEN+2];
char *gpDcWorkBuf, *gpPreviewBuf;//拍照时用
U32	 gu32PhotoSize;
tGrapButtonBuffPic  rgb_buf;

tOPINFO_GLB	gtOpInfoGlb;

tGRAPEDIT	*gpEdit_Stat1;	//上一站/始发站
tGRAPEDIT	*gpEdit_Stat2;	//下一站/目的站
tGRAPEDIT	*gpEdit_Employee;//人员
tGRAPEDIT	*gpEdit_Packet;	//大包号
tGRAPEDIT	*gpEdit_CarCode;//车辆码
tGRAPEDIT	*gpEdit_CarLine;//车线码
tGRAPEDIT	*gpEdit_Code;	//条码

eERRCODE	geLastErrCode;

const U8 *const ComErrStr[]=
{
	"成功",	"网络连接失败",	"网络连接超时",	"网络通讯失败",	"已是最新版本",
	"数据错误", "更新失败"
};

const tTYPE_STR gaScanType2Str[]=
{
	/*{HYCO_OP_SJ,	"收件"},
	{HYCO_OP_FJ,	"发件"},
	{HYCO_OP_DJ,	"到件"},
	{HYCO_OP_PJ,	"派件"},
	
	{HYCO_OP_WENTI,	"问题件"},
	{HYCO_OP_LIUCANG,"留仓件"},
	{HYCO_OP_QS,	"快件签收"},
	{HYCO_OP_DSHK,	"代收付款"},
	
	{HYCO_OP_SJWT,	"称重收件"},
	{HYCO_OP_FJWT,	"称重发件"},
	{HYCO_OP_DJWT,	"称重到件"},*/

	{0xff,	""},
};

////////////////////////////////////////////////////////////////
/*--------------------------------------
*函数：hy_memcmp
*功能：不区分大小写比较字串
*参数：
*返回：
*--------------------------------------*/
int hy_memcmp(char *pStr1, char *pStr2, int len)
{
	int i, cmp=0;
	char ch1,ch2;
	
	for(i = 0; i < len; i++)
	{
		if(pStr1[i] >= 'a' && pStr1[i] <= 'z')//小写转为大写
		{
			ch1 = pStr1[i] - 0x20;
		}
		else
		{
			ch1 = pStr1[i];
		}
		if(pStr2[i] >= 'a' && pStr2[i] <= 'z')//小写转为大写
		{
			ch2 = pStr2[i] - 0x20;
		}
		else
		{
			ch2 = pStr2[i];
		}
		if(ch1 > ch2)
		{
			cmp = 1;
			break;
		}
		else if(ch1 < ch2)
		{
			cmp = -1;
			break;
		}
	}
	
	return cmp;
}

/*--------------------------------------
*函数：hy_strstr
*功能：不区分大小写查找字串
*参数：
*返回：
*--------------------------------------*/
char *hy_strstr(const char *str, const char *subStr)
{
    int len = strlen(subStr);
    
    if(len == 0)
    {
        return NULL;
    } 

    while(*str)
    {
        if(hy_memcmp(str, subStr, len) == 0)
        {
            return str;
        }
        
        str++;
    }
    
    return NULL;
}

/*------------------------------------------
* 函数:Op_CommVar_Init
* 功能:业务操作模块信息初始化
* 参数:none
* 返回:none
-------------------------------------------*/
void Op_CommVar_Init()
{	
	gpEdit_Stat1	= NULL;	//上一站/始发站
	gpEdit_Stat2	= NULL;	//下一站/目的站
	gpEdit_Employee	= NULL;//人员
	gpEdit_Packet	= NULL;	//大包号
	gpEdit_CarCode	= NULL;//车辆码
	gpEdit_CarLine	= NULL;//车线码
	gpEdit_Code		= NULL;	//条码
	
	//业务操作中可能用到的一些信息
	memset(&gtOpInfoGlb, 0, sizeof(tOPINFO_GLB));
	
	geLastErrCode = ERR_NULL;
}

U8 *OpCom_OpType2Name(eOPTYPE type)
{
	int i;
	U8  *pStr = NULL;
	
	for(i = 0; ;i++)
	{
		if(gaScanType2Str[i].type == type || gaScanType2Str[i].type == 0xff)
		{
			pStr = gaScanType2Str[i].pStr;
			break;
		}
	}
	
	return pStr;
}

//设置本次错误类型
void OpCom_SetLastErrCode(eERRCODE errCode)
{
	geLastErrCode = errCode;
}

//获取最后一次错误类型
eERRCODE OpCom_GetLastErrCode()
{
	return geLastErrCode;
}


//通用错误提醒
//1:函数内部已经提醒  0:未提醒
int OpCom_ErrorRemind(tGRAPDESKBOX *pDesk)
{
	int			ret = 0;
	eERRCODE	errCode;
	U8			*pErrStr;
	
	pErrStr = UpDateTime_GetFailReason();
	if(strlen(pErrStr) > 0)
	{
		Com_SpcDlgDeskbox(pErrStr,6,pDesk,1,NULL,NULL,200);
	}
	else
	{
		Com_SpcDlgDeskbox("操作失败!",6,pDesk,1,NULL,NULL,200);
	}
	geLastErrCode = ERR_NULL;
	
	return ret;
}

//检查返回信息请求信息是否一致
//1:一致   0:不一致
int OpCom_CheckReqType(U8 *pData, U8 *pReq)
{
	U8  *pS,*pE;
	U8	len,temp[6];
	
	pS = strstr(pData, "<optype>");
	pE = strstr(pData, "</optype>");
	if(pS == NULL || pE == NULL)
	{
		//错误数据
		return 0;
	}
	pS += strlen("<optype>");
	len = pE-pS;
	if(len >= 6) len = 5;
	memcpy(temp, pS, len);
	temp[len] = 0;
	if(strcmp(pReq, temp) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*---------------------------------------------------------
*函数: OpCom_GetCurOpType
*功能: 获取当前操作类型
*参数: 
*返回: opType
*---------------------------------------------------------*/
U8 OpCom_GetCurOpType()
{
	return gtOpInfoGlb.curOptype;
}

/*---------------------------------------------------------
*函数: OpCom_SetCurOpType
*功能: 设置本次业务操作类型  如果本次操作类型和上次不一样,则清除之前保存的信息
*参数: opType
*返回: 无
*---------------------------------------------------------*/
void OpCom_SetCurOpType(U8 opType)
{
	gpEdit_Stat1	= NULL;	//上一站/始发站
	gpEdit_Stat2	= NULL;	//下一站/目的站
	gpEdit_Employee	= NULL;//人员
	gpEdit_Packet	= NULL;	//大包号
	gpEdit_CarCode	= NULL;//车辆码
	gpEdit_CarLine	= NULL;//车线码
	gpEdit_Code		= NULL;	//条码
	
	if(opType != gtOpInfoGlb.curOptype)//类型不一致
	{
		memset(&gtOpInfoGlb, 0, sizeof(tOPINFO_GLB));
		gtOpInfoGlb.curOptype = opType;//记录本次操作类型
		Rec_ClearExist();
	}
	
}

const U8 OpCom_Month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

/*---------------------------------------------------------
*函数: OpCom_GetBeforeDay
*功能: 以当今日期计算cnt 天前的日期
*参数: cnt ----- 需要减的天数
*返回: 无
*---------------------------------------------------------*/
void OpCom_GetBeforeDay(U16 cnt, U8 *date)
{
	U16  year, hour;
	S16  month,day ;
	S16  y,m,d ;
	U16  i, leap=0; 
	
	year  = gtRtc_solar.year;
	month = gtRtc_solar.month;
	day   = gtRtc_solar.day;
	
	hour  = gtRtc_solar.hour;
	
	if( (month == 2) && ((year%400) == 0 || ((year%4) == 0 && (year%100) != 0)) )
	{//是2月  且是闰年
		leap = 1;
	}
	
	if(gtHyc.workDay != 0 && hour >= gtHyc.workDay)//特殊处理日期  以此为新的一天开始
	{
		day++;
	}
	
	if(day > OpCom_Month[month-1]+leap)
	{
		day = 1;
		month++;
		
		if(month > 12)
		{
			month = 1;
			year++;
		}
	}
	
	day -= cnt;
	
	while(day <= 0)
	{
		month -= 1;
		if(month <= 0)
		{
			year -= 1;
			month = 12;
		}

		day += OpCom_Month[month-1];
		if(month == 2)//如果是2月  需要判断是否闰年
		{
			if( (year%400) == 0 || ((year%4) == 0 && (year%100) != 0) )
			{
				day++;
			}
		}
	}
	
	sprintf(date, "%02d%02d%02d", year%100, month, day);
}

/*---------------------------------------------------------
*函数: OpCom_GetBeforeMonth
*功能: 以当今月计算cnt 月前的月
*参数: cnt ----- 需要减的月数
*返回: 无
*---------------------------------------------------------*/
void OpCom_GetBeforeMonth(U16 cnt, U8 *date)
{
	U16 year ;
	U8  month,day ;
	S16 y,m;
	
	U16  i ; 
	
	year  = gtRtc_solar.year;
	month = gtRtc_solar.month;
	day   = gtRtc_solar.day;
	
	/* 一个一个的减 */
	for(i = 0 ; i < cnt ; i++)
	{
		m = month - 1 ;
		
		if (0 == m)
		{
			y = year - 1 ;
			if (y <= 0)
			{
				year = 0 ;
			}
			else
			{
				year = y ;
			}
			month = 12 ;
		}
		else
		{
			month = m ;
		}
	}

	sprintf(date, "%02d%02d%02d", year%100, month, day);
}

/*--------------------------------------
*函数：OpCom_GetCurDay
*功能：得到今天日期的字符串  年2位 月2位 日2位   例：2010年12月20   101220
*参数：pStr:接收的buf   len：buf的长度
*返回：none
*--------------------------------------*/
void OpCom_GetCurDay(U8 *pStr, int len)
{
	RtcTime_st	kRtc;
	U16 leap = 0;
	
	memset(pStr,0,len);
	
	hyhwRtc_GetTime(&kRtc);
	
	if( (kRtc.month == 2) && ((kRtc.year%400) == 0 || ((kRtc.year%4) == 0 && (kRtc.year%100) != 0)) )
	{//是2月  且是闰年
		leap = 1;
	}
	
	if(gtHyc.workDay != 0 && kRtc.hour >= gtHyc.workDay)// 特殊处理日期  以此为新的一天开始
	{
		kRtc.day++;
	}
	
	if(kRtc.day > OpCom_Month[kRtc.month-1]+leap)
	{
		kRtc.day = 1;
		kRtc.month++;
		
		if(kRtc.month > 12)
		{
			kRtc.month = 1;
			kRtc.year++;
		}
	}
	
	sprintf(pStr,"%02d%02d%02d",(kRtc.year%100),kRtc.month,kRtc.day);
	
	return ;
}

/*--------------------------------------
*函数：OpCom_GetTimeStr
*功能：得到时间字符串  格式:yyyymmddhhmmss
*参数：pStr:接收的buf
*返回：none
*--------------------------------------*/
void OpCom_GetTimeStr(char *pStr)
{
	RtcTime_st	kRtc;
	
	hyhwRtc_GetTime(&kRtc);
	//sprintf(pStr,"%04d-%02d-%02d %02d:%02d:%02d",
	sprintf(pStr,"%04d%02d%02d%02d%02d%02d",
		kRtc.year,kRtc.month,kRtc.day,kRtc.hour,kRtc.minute,kRtc.second);
	
	return ;
}

/*--------------------------------------
*函数：OpCom_GetTimeStr
*功能：得到时间字符串  格式:yyyy-mm-dd hh:mm:ss
*参数：pStr:接收的buf
*返回：none
*--------------------------------------*/
void OpCom_GetTimeStr2(char *pStr)
{
	RtcTime_st	kRtc;
	
	hyhwRtc_GetTime(&kRtc);
	sprintf(pStr,"%04d-%02d-%02d %02d:%02d:%02d",
		kRtc.year,kRtc.month,kRtc.day,kRtc.hour,kRtc.minute,kRtc.second);
	
	return ;
}

/*---------------------------------------------------------
*函数: OpCom_FillCommInfo
*功能: 填充记录的公共部分
*参数: state:期望该记录的初始状态    一票多件时需注意
*返回: 无
*---------------------------------------------------------*/
void OpCom_FillCommInfo(tDATA_INFO *pRecData, U8 *code, U8 *subCode, U8 state)
{
	memset(pRecData, 0, sizeof(tDATA_INFO));
	//公共部分
	pRecData->tComm.opType = OpCom_GetCurOpType();
	pRecData->tComm.right = Login_GetRight();
	pRecData->tComm.state = state;
	pRecData->tComm.groupId = Login_GetGroupId();
	strcpy(pRecData->tComm.userId, Login_GetUserId());
	strcpy(pRecData->tComm.scanStat, glbVar_GetCompanyId());
	
	if(code != NULL)
		strcpy(pRecData->tComm.code, code);
	
	if(subCode != NULL)
		strcpy(pRecData->tComm.subCode, subCode);
	
	OpCom_GetTimeStr(pRecData->tComm.scanTime);
}

/*
 字符串中是否都是数字
 0：真   -1：假
*/
int OpCom_IfDigit(U8 *pStr)
{
	int len,i;
	int ret = 0;
	
	len = strlen(pStr);
	for(i = 0; i < len; i++)
	{
		if(*pStr < '0' || *pStr > '9')
		{
			ret = -1;
			break;
		}
		pStr++;
	}
	
	return ret;
}

/*
是否字母和数字组合
 0：真   -1：假
*/
int OpCom_IfChar_Digit(U8 *pStr)
{
	int len,i;
	int ret = 0;
	U8  ch,flag=0;
	
	len = strlen(pStr);
	for(i = 0; i < len; i++)
	{
		ch = *pStr;
		if(ch >= '0' && ch <= '9')
		{
			pStr++;
		}
		else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			pStr++;
			flag = 1;
		}
		else
		{
			ret = -1;
			flag = 0;
			break;
		}
	}

	if(flag == 1)
	{
		ret = 0;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}


S32 OpCom_EditDrawInputCnt(tGRAPEDIT *pEdit)
{
	tStrDispInfo disp_info;
	
	if(pEdit->view_attr.curFocus)
	{//显示个数
		U8 buf[8];
		
		Grap_ClearScreen(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H);
		
		memset(buf,0,8);
		sprintf(buf,"%d/%d",strlen(pEdit->edit_privattr.digt),pEdit->edit_privattr.containLen);
		
		disp_info.string = buf;
		disp_info.color  = COLOR_RED;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H, &disp_info);
	}
	
    return SUCCESS;
}

void OpComm_PhotoBufInit()
{
	gu32PhotoSize = 0;
	memset(gau8PhotoCode, 0, BARCODE_LEN+2);
	gpDcWorkBuf = (char *)hsaSdram_UpgradeBuf();
	gpDcWorkBuf = (char *)((unsigned)(gpDcWorkBuf + 3) & 0xFFFFFFFC);
	gpPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
}

//flag 0:拍照  1:预览大图   2:加载缩略图
void OpComm_Photo(tGRAPDESKBOX *pDesk, U8 flag, U8 *code)
{
	int size;
	
	if(flag == 0)
	{
		gpDcWorkBuf  = (char *)hsaSdram_UpgradeBuf();
		gpDcWorkBuf = (char *)((unsigned)(gpDcWorkBuf + 3) & 0xFFFFFFFC);
		gpPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gpDcWorkBuf = DC_Desk(pDesk,gpDcWorkBuf,&gu32PhotoSize);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);

		if (gu32PhotoSize > 0)
		{
			memset(&rgb_buf, 0, sizeof(tGrapButtonBuffPic));
			if(0 == Photo_Decode(gpDcWorkBuf,gu32PhotoSize,60,80, gpDcWorkBuf+gu32PhotoSize,gpPreviewBuf,TRUE))
			{
				//保存当前拍照的单号
				if(code != NULL)
				{
					strcpy(gau8PhotoCode, code);
				}
				
				rgb_buf.rgb = gpPreviewBuf;
		    	rgb_buf.off_x1 = 0;
		    	rgb_buf.off_y1 = 0;
		    	rgb_buf.width  = 80;
		    	rgb_buf.height = 60;
		    	gpPhotoBtn->btn_privattr.pBufPic = (tGrapButtonBuffPic *)&rgb_buf;
		    }
		    else
		    {
		    	gu32PhotoSize = 0;
		    	Com_SpcDlgDeskbox("预览图解码失败!",6,pDesk,2,NULL,NULL,100);
		    }
		}
		else
		{
			gu32PhotoSize = 0;
			gpPhotoBtn->btn_privattr.pBufPic = NULL;
		}
	}
	else if(flag == 1)//预览大图
	{
		if(gpPhotoBtn->btn_privattr.pBufPic == NULL || (gu32PhotoSize == 0 && code == NULL))
		{
			Com_SpcDlgDeskbox("没有预览图片!",6,pDesk,2,NULL,NULL,100);
		}
		else
		{
			gpDcWorkBuf  = (char *)hsaSdram_UpgradeBuf();
			gpDcWorkBuf  = (char *)((unsigned)(gpDcWorkBuf + 3) & 0xFFFFFFFC);
			gpPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
			
			if(gu32PhotoSize == 0)
			{
				size = PicStore_Read(code,gpDcWorkBuf,0x10000);
			}
			else
			{
				size = gu32PhotoSize;
			}
			
			if(0 == Photo_Decode(gpDcWorkBuf,size,240,320,gpDcWorkBuf+size,gpPreviewBuf, TRUE))
			{
				ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
				Photo_DisplayDesk(pDesk,gpPreviewBuf);
				ComWidget_Link2Desk(pDesk,pgtComFirstChd);
				
				if(0 == Photo_Decode(gpDcWorkBuf,size,60,80, gpDcWorkBuf+size,gpPreviewBuf,TRUE))
				{
					rgb_buf.rgb = gpPreviewBuf;
			    	rgb_buf.off_x1 = 0;
			    	rgb_buf.off_y1 = 0;
			    	rgb_buf.width  = 80;
			    	rgb_buf.height = 60;
			    	gpPhotoBtn->btn_privattr.pBufPic = (tGrapButtonBuffPic *)&rgb_buf;
			    }
			    else
			    {
			    	gu32PhotoSize = 0;
			    	Com_SpcDlgDeskbox("预览图解码失败!",6,pDesk,2,NULL,NULL,100);
			    }
			}
		}
	}
	else if(flag == 2)//加载缩略图
	{
		gpDcWorkBuf  = (char *)hsaSdram_UpgradeBuf();
		gpDcWorkBuf = (char *)((unsigned)(gpDcWorkBuf + 3) & 0xFFFFFFFC);
		gpPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
		
		size = PicStore_Read(code,gpDcWorkBuf,0x10000);
		if(size > 0)
		{
			if(0 == Photo_Decode(gpDcWorkBuf,size,60,80, gpDcWorkBuf+size,gpPreviewBuf,TRUE))
			{
				if(code != NULL)
				{
					strcpy(gau8PhotoCode, code);
				}
				
				rgb_buf.rgb = gpPreviewBuf;
		    	rgb_buf.off_x1 = 0;
		    	rgb_buf.off_y1 = 0;
		    	rgb_buf.width  = 80;
		    	rgb_buf.height = 60;
		    	gpPhotoBtn->btn_privattr.pBufPic = (tGrapButtonBuffPic *)&rgb_buf;
		    }
		    else
		    {
		    	Com_SpcDlgDeskbox("预览图解码失败!",6,pDesk,2,NULL,NULL,100);
		    }
		}
		else
		{
			Com_SpcDlgDeskbox("没有预览图片!",6,pDesk,2,NULL,NULL,100);
		}
	}
}


/*================================重载edit2 可上下键滚动(仅限只可显示的用)=====================================*/

S32 OpCom_LineInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos)
{
	tCHARPOS *pSub;
	
	*firstCharPos = 0;
	pSub = pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			*firstCharPos = pSub->firstCharPos;
			break;
		}
		pSub = pSub->pNext;
	}
	
	return 0;
}

S32 OpComm_BtnBTDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	if(hyco_getConnectState() == TRUE)
	{
		Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
	}
	else
	{
		Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
	}
	
	if (pbtn_privattr->pName)//显示button上的字符
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1+20, viewY1+2, viewX2,viewY2, &disp_info);
	}
	
	return SUCCESS;
}

//检查重量  1:OK   
int OpComm_CheckWeight(U8 *pWeight)
{
	int ret = 0,val;
	U8  *pSub,buf[20];
	
	strcpy(buf, pWeight);
	pSub = strstr(buf, ".");
	if(pSub != NULL)
	{
		memmove(pSub, pSub+1, 2);
		pSub[2] = 0;
		val = atoi(buf);
		if(val >= 20 && val <= 15000)
		{
			ret = 1;
		}
	}
	
	return ret;
}

S32 OpComm_BTEditDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT *pEdit;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	tDROPLIST	  *pDropList;	
	Character_Type_e kLycStr_Type;
	U8 tempdata[50]={0};
	U16 x,y,w,h,i,j;
	U8 temp[30], *pStr;
			
	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pDropList  = &(pEdit->dropList);
	
	kLycStr_Type = pedit_privattr->font_type;
	
	//clear edit screen
	if (pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
	}
	else if(pedit_privattr->coverPicId)
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	else if(pview_attr->color != pedit_privattr->color)
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}

	if(strlen(pedit_privattr->digt) == 0)
	{
		pedit_privattr->firstCharPos = 0 ;
		pedit_privattr->Pos          = 0 ;
	}
	else if(pedit_privattr->Pos > strlen(pedit_privattr->digt))
	{
		pedit_privattr->Pos = strlen(pedit_privattr->digt);
	}
	
	
	//检查是否处在半个汉字位置
	for(i = 0; i < pedit_privattr->firstCharPos; i++)
	{
		if(pedit_privattr->digt[i] > 0x80)//汉字
		{
			if(i+1 >= pedit_privattr->firstCharPos)
			{
				pedit_privattr->firstCharPos++;
				break;
			}
			else
			{
				i++;
			}
		}
	}

	// write string
	if (pedit_privattr->digt[pedit_privattr->firstCharPos])
	{
		U16 len;

		x = pview_attr->viewX1+pedit_privattr->xoffset;
		y = pview_attr->viewY1+pedit_privattr->yoffset;
		
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->firstCharPos);

		if(len <= pedit_privattr->len)
		{
			j = 0;
			pStr = pedit_privattr->digt+pedit_privattr->firstCharPos;
			if(kLycStr_Type == CHARACTER_LOCALCODE)
			{
				for(i=0; i<pedit_privattr->len;i++)
				{
					if(pStr[i]>0x80)
					{
						j+=2;i++;
						if(j > pedit_privattr->len)
						{j-=2;break;}
					}
					else
					{
						j++;
						if(j > pedit_privattr->len)
						{j--;break;}
					}
				}
				//hyUsbPrintf("edit j = %d \r\n", j);
			}
			else if(kLycStr_Type == CHARACTER_UNICODE)
			{
				j = pedit_privattr->len;
				if(j%2!=0)	j--;
			}
			memcpy(temp, (pedit_privattr->digt+pedit_privattr->firstCharPos), j);
			temp[j] = 0;
			temp[j+1] = 0;
			disp_info.string = temp;
			disp_info.color = pedit_privattr->color;
			disp_info.font_size = pedit_privattr->font_size;
			disp_info.font_type = pedit_privattr->font_type;
			disp_info.flag = pedit_privattr->position;
			disp_info.keyStr = NULL;
			
			if(OpComm_CheckWeight(pedit_privattr->digt) != 1)
			{
				disp_info.color = COLOR_RED;
			}
			Grap_WriteString(x, y, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
		}
		else
		{
			j = 0;
			pStr = pedit_privattr->digt+pedit_privattr->firstCharPos;
			if(kLycStr_Type == CHARACTER_LOCALCODE)
			{
				for(i=0; i<pedit_privattr->len;i++)
				{
					if(pStr[i]>0x80)
					{
						j+=2;i++;
						if(j > pedit_privattr->len)
						{j-=2;break;}
					}
					else
					{
						j++;
						if(j > pedit_privattr->len)
						{j--;break;}
					}
				}
				//hyUsbPrintf("edit j = %d \r\n", j);
			}
			else if(kLycStr_Type == CHARACTER_UNICODE)
			{
				j = pedit_privattr->len;
				if(j%2!=0)	j--;
			}
			memcpy(temp, (pedit_privattr->digt+pedit_privattr->firstCharPos), j);
			temp[j] = 0;
			temp[j+1] = 0;
			disp_info.string = temp;		
			disp_info.color = pedit_privattr->color;
			disp_info.font_size = pedit_privattr->font_size;
			disp_info.font_type = pedit_privattr->font_type;
			disp_info.flag = pedit_privattr->position;
			disp_info.keyStr = NULL;
			
			if(OpComm_CheckWeight(pedit_privattr->digt) != 1)
			{
				disp_info.color = COLOR_RED;
			}
			Grap_WriteString(x,y,(U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
		}
	}
	
	pEdit->DrawInputCnt(pEdit);
	
	return SUCCESS;
}

//称重业务连接蓝牙 0:未连接   1:已连接
int OpComm_ConnBT(tGRAPDESKBOX *pDesk, tGRAPEDIT *pEditWT)
{
	int ret;
	
	if (hyco_getConnectState() == TRUE)
	{
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否断开连接?",6, pDesk, 3, NULL, NULL, 3000))
		{
			bt_disconnect(pDesk);
			if((gtHyc.ParamCfg&0x01) == 0)
			{
				//修改重量控件的属性
				pEditWT->view_attr.reserve = 2;
				pEditWT->view_attr.FocusEnable = 1;
				pEditWT->view_attr.color=0xffff;
				pEditWT->edit_privattr.TimerTot = 50;
			}
		}
		return 0;
	}
	
	if(strlen(gtHyc.bt_addr2) == 0 || gtHyc.bt_devenable == 0)//跳入BT设置界面
	{
		if(gtHyc.bt_devenable == 0)//未选择电子秤
		{
			Com_SpcDlgDeskbox("请先选择电子秤类型!",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		}
		
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Set_BT_UseNow(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		if(ret == 1)//连接成功
		{
			//修改重量控件的属性
			pEditWT->view_attr.reserve = 1;
			pEditWT->view_attr.FocusEnable = 0;
			pEditWT->view_attr.color=0xCE59;
			pEditWT->edit_privattr.TimerTot = 4;
		}
	}
	else//直接根据输入的MAC和密码配对
	{
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否重新选择电子秤?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Set_BT_UseNow(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			if(ret == 1)//连接成功
			{
				//修改重量控件的属性
				pEditWT->view_attr.reserve = 1;
				pEditWT->view_attr.FocusEnable = 0;
				pEditWT->view_attr.color=0xCE59;
				pEditWT->edit_privattr.TimerTot = 4;
			}
			return ret;
		}
		
		if(1 == bt_dev_connect(pDesk))
		{
			//修改重量控件的属性
			pEditWT->view_attr.reserve = 1;
			pEditWT->view_attr.FocusEnable = 0;
			pEditWT->view_attr.color=0xCE59;
			pEditWT->edit_privattr.TimerTot = 4;
			
			ret = 1;
		}
		else
		{
			ret = 0;
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否搜索蓝牙设备?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
				ret = Set_BT_UseNow(pDesk);
				ComWidget_Link2Desk(pDesk,pgtComFirstChd);
				if(ret == 1)//连接成功
				{					
					//修改重量控件的属性
					pEditWT->view_attr.reserve = 1;
					pEditWT->view_attr.FocusEnable = 0;
					pEditWT->view_attr.color=0xCE59;
					pEditWT->edit_privattr.TimerTot = 4;
				}
			}
		}
	}
	
	
	return ret;
}



int OpComm_EditHandle_Price(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	U16 len,tempLen;
	tGRAPEDIT *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	Character_Type_e kLycStr_Type;
	S32 ret = SUCCESS;

	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	kLycStr_Type = pedit_privattr->font_type;
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	if(message == VIEW_PRESS && pview_attr->curFocus && pEdit->editKeyInput != NULL)
	{
		U32 inputret;
		
		inputret = pEdit->editKeyInput(pEdit,&lParam);
		//hyUsbPrintf("inputret = %d  lParam = %d  \r\n", inputret, lParam);
		if(PARSED == inputret)
		{
			//pEdit->draw(pEdit,0,0);
			ReDraw(pEdit,0,0);
			return PARSED;
		}
		else if(inputret == VIEW_DELETE)
		{
			message = VIEW_DELETE;
			ret = PARSED;
		}
		else if(inputret == FAIL)
		{
		}
		else if(lParam != 0xff)
		{
			message = VIEW_INPUT;
			ret = PARSED;
		}
	}
	
	switch(message)
	{
	case VIEW_INIT:
		pedit_privattr->Pos = strlen(pedit_privattr->digt);
		break;
	case VIEW_ENABLE:
		pEdit->view_attr.enable = 1;
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_DISABLE:
		pEdit->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_TIMER:
		if(pedit_privattr->TimerLft == 0)
		{
			pedit_privattr->TimerLft = pedit_privattr->TimerTot;
			pedit_privattr->showState = (1-pedit_privattr->showState);
			pEdit->editTimer(pEdit);
		}
		else
		{ 
			S32 timeleft;
			timeleft =  pedit_privattr->TimerLft-lParam;
			
			if (timeleft < 0)  pedit_privattr->TimerLft = 0;
			else pedit_privattr->TimerLft = timeleft;			
		}
		break;
	case VIEW_INPUT:
	case VIEW_DIGIT:
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		if ((U8)(lParam&0xff) != '.' && len < pedit_privattr->containLen)
		{	
			int i;
			U8 temp[MAX_DIGITEDIT_LEN];
			U8 hCode = 0, lCode = 0;
			
			pedit_privattr->Pos = len;
			
			memset(temp,0,MAX_DIGITEDIT_LEN);
			lCode = (U8)(lParam&0xff) ;
			hCode = (U8)((lParam>>8)&0xff) ;
		//hyUsbPrintf("edit input lcode  = %x  hcode = %x \r\n",lCode, hCode);
			if((kLycStr_Type == CHARACTER_UNICODE||hCode > 0) && len+2 > pedit_privattr->containLen)
				break;
			if(hCode > 0 && pedit_privattr->containFlag)
				break;
			if( (pedit_privattr->containFlag&CONTAIN_NUM) &&( (lCode>0x39 || lCode<0x30) || hCode > 0))
				break;
			if( (pedit_privattr->containFlag&CONTAIN_NUM1) &&( (lCode != '.' &&(lCode>0x39 || lCode<0x30)) || hCode > 0))
				break;
			
			tempLen = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->Pos);
			memcpy(temp, pedit_privattr->digt+pedit_privattr->Pos,tempLen+2);//光标后面的字符

			if(hCode || kLycStr_Type == CHARACTER_UNICODE)
				pedit_privattr->digt[pedit_privattr->Pos++] = hCode;

			pedit_privattr->digt[pedit_privattr->Pos++] = lCode;
			memcpy(pedit_privattr->digt+pedit_privattr->Pos,temp, tempLen+2);//光标后面的字符

			while((pedit_privattr->Pos-pedit_privattr->firstCharPos) > pedit_privattr->len)
			{
				if(pedit_privattr->digt[pedit_privattr->firstCharPos] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
					pedit_privattr->firstCharPos+=2;
				else
					pedit_privattr->firstCharPos++;
			}
			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;	
			
			len = strlen(pedit_privattr->digt);
			if(len == 1)
			{//刚输入第一个数字,需要转成金额格式0.01
				temp[0] = pedit_privattr->digt[0];
				strcpy(pedit_privattr->digt,"0.00");
				pedit_privattr->digt[3] = temp[0];
			}
			else
			{
				pedit_privattr->digt[len-3-1] = pedit_privattr->digt[len-2-1];
				pedit_privattr->digt[len-2-1] = '.';
				if(pedit_privattr->digt[0] == '0' && pedit_privattr->digt[1] != '.')
				{
					memmove(&pedit_privattr->digt[0],&pedit_privattr->digt[1],len-1);
					pedit_privattr->digt[len-1] = 0;
				}
			}
			pedit_privattr->Pos = strlen(pedit_privattr->digt);
			
			pEdit->draw(pEdit, 0, 0);
			pEdit->editKeyEnd(pEdit, 0);
		}
		else
		{
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_LEFT:
		break;
	case VIEW_RIGHT:
		break;
	case VIEW_DELETE:
		break;
	case VIEW_DELETEALL:
		len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
		if (len > 0 )
		{
			memset(pedit_privattr->digt,0,pedit_privattr->containLen+2);
			pedit_privattr->Pos = 0;
			pedit_privattr->firstCharPos = 0;			
			pedit_privattr->TimerLft = 1;
			pedit_privattr->showState = 0;
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_QUIT:
		break;
	case VIEW_KEY_MSG:
	case VIEW_PRESS:
		//hyUsbPrintf("focus = %d  lparam  = %d   wparam  = %d  \r\n", pEdit->view_attr.curFocus,lParam,wParam);
		if(pEdit->view_attr.curFocus == 1 || pEvent->reserve == 1)//焦点时响应OK键
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				if(cKEYB_EVENT_RELEASE == wParam)
				{
					ret = pEdit->editenter(pEdit);
					if ( ret== RETURN_REDRAW)
					{
						ReDraw(pEdit, 0, 0);
					}
					else if(ret == RETURN_QUIT)
					{
						*((tGRAPDESKBOX *)(pEdit->pParent))->pQuitCnt = 1;
						pEdit->draw(pEdit, 0, 0);
					}
								
					return ret;
				}
				break;
			case KEYEVENT_LEFT:
				break;
			case KEYEVENT_RIGHT:
				break;
			case KEYEVENT_DELETE:
				len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt);
				if (len > 0 && pedit_privattr->Pos>0)
				{
					int i,j;
					for(i=0;i<pedit_privattr->Pos;i++)
					{
						if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
							{i++;j=2;}
						else	j = 1;
					}
					tempLen = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->Pos);
					memcpy(pedit_privattr->digt+pedit_privattr->Pos-j,pedit_privattr->digt+pedit_privattr->Pos,tempLen+2);
					
					if(pedit_privattr->firstCharPos == pedit_privattr->Pos)
					{
						pedit_privattr->Pos-=j;
						pedit_privattr->firstCharPos-=j;
						for(i=0;i<pedit_privattr->Pos;i++)
						{
							if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
								{i++;j=2;}
							else	j = 1;
						}
						if(pedit_privattr->firstCharPos >= j)
							pedit_privattr->firstCharPos-=j;
					}
					else
					{
						pedit_privattr->Pos-=j;
						j = 0;
						while(pedit_privattr->firstCharPos > 0)
						{
							len = (U16)Get_StringLength(kLycStr_Type,pedit_privattr->digt+pedit_privattr->firstCharPos);
							if(len < pedit_privattr->len)
							{
								for(i=0;i<pedit_privattr->firstCharPos;i++)
								{
									if(pedit_privattr->digt[i]>0x80 || kLycStr_Type == CHARACTER_UNICODE)
										{i++;j=2;}
									else	j = 1;
								}
							}
							if(len + j < pedit_privattr->len)
							{
								pedit_privattr->firstCharPos-=j;
							}
							else
							{
								break;
							}
						}
					}
					
					pedit_privattr->TimerLft = 1;
					pedit_privattr->showState = 0;
					len = strlen(pedit_privattr->digt);
					
					pedit_privattr->digt[len-2] = pedit_privattr->digt[len-3];
					pedit_privattr->digt[len-3] = '.';
					if(pedit_privattr->digt[0] == '.' && pedit_privattr->digt[1] == '0' && pedit_privattr->digt[2] == '0')
					{//.00
						memset(pedit_privattr->digt,0,pedit_privattr->containLen+2);
						pedit_privattr->Pos = 0;
						pedit_privattr->firstCharPos = 0;
					}
					else if(pedit_privattr->digt[0] == '.')
					{//.01 .11
						memmove(&pedit_privattr->digt[1],&pedit_privattr->digt[0],len);
						pedit_privattr->digt[0] = '0';
						len++;
						pedit_privattr->digt[len] = 0;
					}
					pedit_privattr->Pos = strlen(pedit_privattr->digt);
					pEdit->draw(pEdit, 0, 0);
				}
				
				return PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_REV_SYSMSG:
		pEdit->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pEdit->recvPullMsg(pEdit,pEvent);
	    break;	
	}
	return ret;
}

S32 OpComm_EditTimer(void *pView)
{
	tGRAPEDIT *pEdit;
	tGRAPVIEW *pFocus;
	tGRAPCOMBOBOX *pCom;
	tGrapViewAttr  *pview_attr;
	tGrapEditPriv *pedit_privattr;
	int len;
	
	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	len = strlen(pedit_privattr->digt);
	if(len <= pedit_privattr->len)
	{
		if(pedit_privattr->firstCharPos > 0)
		{
			pedit_privattr->firstCharPos = 0;
			pEdit->draw(pEdit, 0, 0);
		}
		return FAIL;
	}
	
	pFocus = Grap_GetCurFocus(pEdit, NULL);
	if(pFocus->view_attr.id > COMBOBOX_ID_BASE && pFocus->view_attr.id < PICTURE_ID_BASE)
	{
		pCom = (tGRAPCOMBOBOX *)pFocus;
		if(pCom->list_opened == 1)
		{
			return FAIL;
		}
	}

	if(pedit_privattr->digt[pedit_privattr->firstCharPos]>=0x80)	pedit_privattr->firstCharPos+=2;
	else	pedit_privattr->firstCharPos++;
	
	if(pedit_privattr->firstCharPos >= len-2)
	{
		pedit_privattr->firstCharPos = 0;
	}
	
	pEdit->draw(pEdit, 0, 0);
	
	return SUCCESS;
}