#include "Rec_Store.h"
#include "Rec_Queue.h"
#include "glbVariable_base.h"
#include "Common_Widget.h"
#include "base.h"
#include "grap_api.h"
#include "Op_Types.h"
#include "Op_Config.h"
#include "Op_Common.h"


tREC_SEARCH	*gptRecSearch;
U32	gu32RecSearchTotal;
U16 *gpu16RecSearchInfo;

U8 gu8SearchType;//查询业务的类型

/*=====================================================================*/
/*+++++++++++++++++++++++++++++++++++++++++++++记录查询接口+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*---------------------------------------------------------
*函数: Rec_SearchRecInfo
*功能: 根据日期,权限,业务类型得到该权限下所有扫描类型的总数和已发数 并做现实排版
*参数: dateStr  ------- 日期
*      right    ------- 权限
*      type     ------- 操作类型 0:统计所有   0xff:统计报表(只统计 签收/到达/分发)
*      outBuf   ------- 输出
*返回: none
*---------------------------------------------------------*/
void Rec_SearchRecInfo(U8 *dateStr, U8 right, U8 type, U8 *pOutBuf)
{
	U16		cntInfo[3];//总数/已传/未传
	U16		baobiaoType[3]={HYCO_OP_QS,HYCO_OP_DJ,HYCO_OP_PJ};
	U8		namebuf[20];
	U16		i,j,len=0,namelen,opCnt=0;
	//U32     total,rep,unrep ;
	int		index,typeCnt;
	tREC_INFO	recInfo;
	
	if(type > HYCO_TYPE_MAX)
	{
		return ;
	}

//hyUsbPrintf("dateStr = %s  right = %d  type  = %d  \r\n ", dateStr, right, type);
	
	if(dateStr[4] == 0)//按月查询
	{
		if(type == 0)//统计  需要查所有操作
		{
			opCnt = Op_Config_GetCntByRight(0)-OP_CONFIG_OFFSET;//统计   不算在具体操作类型内
			for(i = 0; i < opCnt; i++)
			{
				cntInfo[0] = 0;
				cntInfo[1] = 0;
				cntInfo[2] = 0;
				
				if(i == 0)//只有第一条需要获取信息
				{
					recInfo.max = 50;
					recInfo.cnt = 0;
				}
				else
				{
					recInfo.max = 0;
					for(typeCnt = 0; typeCnt < recInfo.cnt; typeCnt++)
					{
						if(gaOp_Config[i+OP_CONFIG_OFFSET].opType == recInfo.recType[typeCnt])
						{
							break;
						}
					}
					/*if(typeCnt >= recInfo.cnt)
					{
						continue;
					}*/
				}
				if(recInfo.max != 0 || typeCnt < recInfo.cnt)
				{
					for(j = 1; j < 32; j++)
					{
						sprintf(&dateStr[4], "%02d", j);
						//RecStore_GetRecordStateNum(dateStr,right,gaOp_Config[i+OP_CONFIG_OFFSET].opType,&total,&rep,&unrep);
						RecStore_GetRecordStateNum(dateStr,right,gaOp_Config[i+OP_CONFIG_OFFSET].opType,&recInfo);
					//hyUsbPrintf("按月统计 type = %d  tot = %d  rep = %d  un = %d \r\n",gaOp_Config[i+OP_CONFIG_OFFSET].opType,rep+unrep,rep,unrep);
						//cntInfo[0] += rep+unrep;
						//cntInfo[1] += rep;
						//cntInfo[2] += unrep;
						cntInfo[0] += recInfo.reported+recInfo.unreported;
						cntInfo[1] += recInfo.reported;
						cntInfo[2] += recInfo.unreported;
					}
				}
				
				memset(namebuf, 0, 20);
				if(strlen(gaOp_Config[i+OP_CONFIG_OFFSET].pName) > 8)
				{
					memcpy(namebuf, gaOp_Config[i+OP_CONFIG_OFFSET].pName, 8);
					//strcat(namebuf, "查询");
				}
				else
				{
					//sprintf(namebuf, "%s查询",gaOp_Config[i+OP_CONFIG_OFFSET].pName);
					strcpy(namebuf, gaOp_Config[i+OP_CONFIG_OFFSET].pName);
				}
				sprintf(pOutBuf+len, "%-9s%-6d%-6d%-6d%",namebuf,cntInfo[0],cntInfo[1],cntInfo[2]);
				len += strlen(pOutBuf+len);
			}
		}
		else if(type = 0xff)//统计报表(只统计 签收/到达/分发)
		{
			for(i = 0; i < 3; i++)
			{
				cntInfo[0] = 0;
				cntInfo[1] = 0;
				cntInfo[2] = 0;
				
				if(i == 0)//只有第一条需要获取信息
				{
					recInfo.max = 50;
					recInfo.cnt = 0;
				}
				else
				{
					recInfo.max = 0;
					for(typeCnt = 0; typeCnt < recInfo.cnt; typeCnt++)
					{
						if(baobiaoType[i] == recInfo.recType[typeCnt])
						{
							break;
						}
					}
					/*if(typeCnt >= recInfo.cnt)
					{
						continue;
					}*/
				}
				if(recInfo.max != 0 || typeCnt < recInfo.cnt)
				{
					for(j = 1; j < 32; j++)
					{
						sprintf(&dateStr[4], "%02d", j);
						
						RecStore_GetRecordStateNum(dateStr,right,baobiaoType[i],&recInfo);
						cntInfo[0] += recInfo.reported+recInfo.unreported;
						cntInfo[1] += recInfo.reported;
						cntInfo[2] += recInfo.unreported;
						//RecStore_GetRecordStateNum(dateStr,right,baobiaoType[i],&total,&rep,&unrep);
					//hyUsbPrintf("按月报表 type = %d  tot = %d  rep = %d  un = %d \r\n",baobiaoType[i],rep+unrep,rep,unrep);
						//cntInfo[0] += rep+unrep;
						//cntInfo[1] += rep;
						//cntInfo[2] += unrep;
					}
				}
				
				index = Op_Config_GetIndexByType(baobiaoType[i]);//找到该类型在数组中对应的位置
				memset(namebuf, 0, 20);
				if(strlen(gaOp_Config[index].pName) > 8)
				{
					memcpy(namebuf, gaOp_Config[index].pName, 8);
					//strcat(namebuf, "查询");
				}
				else
				{
					//sprintf(namebuf, "%s查询",gaOp_Config[index].pName);
					strcpy(namebuf, gaOp_Config[index].pName);
				}
				sprintf(pOutBuf+len, "%-9s%-6d%-6d%-6d%",namebuf ,cntInfo[0],cntInfo[1],cntInfo[2]);
				len += strlen(pOutBuf+len);
			}
		}
		else//查询指定类型
		{
			cntInfo[0] = 0;
			cntInfo[1] = 0;
			cntInfo[2] = 0;
			
			opCnt = 1;
			for(j = 1; j < 32; j++)
			{
				sprintf(&dateStr[4], "%02d", j);
				recInfo.max = 0;
				recInfo.cnt = 0;
				RecStore_GetRecordStateNum(dateStr,right,type,&recInfo);
				cntInfo[0] += recInfo.reported+recInfo.unreported;
				cntInfo[1] += recInfo.reported;
				cntInfo[2] += recInfo.unreported;
				//RecStore_GetRecordStateNum(dateStr,right,type,&total,&rep,&unrep);
				//cntInfo[0] += rep+unrep;
				//cntInfo[1] += rep;
				//cntInfo[2] += unrep;
			}
			index = Op_Config_GetIndexByType(type);//找到该类型在数组中对应的位置
			memset(namebuf, 0, 20);
			if(strlen(gaOp_Config[index].pName) > 8)
			{
				memcpy(namebuf, gaOp_Config[index].pName, 8);
				//strcat(namebuf, "查询");
			}
			else
			{
				//sprintf(namebuf, "%s查询",gaOp_Config[index].pName);
				strcpy(namebuf, gaOp_Config[index].pName);
			}
			sprintf(pOutBuf, "%-9s%-6d%-6d%-6d%",namebuf,cntInfo[0],cntInfo[1],cntInfo[2]);
		}
	}
	else//按天
	{
		if(type == 0)//统计  需要查所有操作
		{
			opCnt = Op_Config_GetCntByRight(0)-OP_CONFIG_OFFSET;//统计   不算在具体操作类型内
			for(i = 0; i < opCnt; i++)
			{
				cntInfo[0] = 0;
				cntInfo[1] = 0;
				cntInfo[2] = 0;
				
				if(i == 0)//只有第一条需要获取信息
				{
					recInfo.max = 50;
					recInfo.cnt = 0;
				}
				else
				{
					recInfo.max = 0;
					for(typeCnt = 0; typeCnt < recInfo.cnt; typeCnt++)
					{
						if(gaOp_Config[i+OP_CONFIG_OFFSET].opType == recInfo.recType[typeCnt])
						{
							break;
						}
					}
					/*if(typeCnt >= recInfo.cnt)
					{
						continue;
					}*/
				}
				
				if(recInfo.max != 0 || typeCnt < recInfo.cnt)
				{
					RecStore_GetRecordStateNum(dateStr,right,gaOp_Config[i+OP_CONFIG_OFFSET].opType,&recInfo);
					cntInfo[0] = recInfo.reported+recInfo.unreported;
					cntInfo[1] = recInfo.reported;
					cntInfo[2] = recInfo.unreported;
					//RecStore_GetRecordStateNum(dateStr,right,gaOp_Config[i+OP_CONFIG_OFFSET].opType,&total,&rep,&unrep);
					//cntInfo[0] = rep+unrep;
					//cntInfo[1] = rep;
					//cntInfo[2] = unrep;
				}
				
				memset(namebuf, 0, 20);
				if(strlen(gaOp_Config[i+OP_CONFIG_OFFSET].pName) > 8)
				{
					memcpy(namebuf, gaOp_Config[i+OP_CONFIG_OFFSET].pName, 8);
					//strcat(namebuf, "查询");
				}
				else
				{
					//sprintf(namebuf, "%s查询",gaOp_Config[i+OP_CONFIG_OFFSET].pName);
					strcpy(namebuf, gaOp_Config[i+OP_CONFIG_OFFSET].pName);
				}
				sprintf(pOutBuf+len, "%-9s%-6d%-6d%-6d%",namebuf,cntInfo[0],cntInfo[1],cntInfo[2]);
				len += strlen(pOutBuf+len);
			}
		}
		else if(type = 0xff)//统计报表(只统计 签收/到达/分发)
		{
			for(i = 0; i < 3; i++)
			{
				cntInfo[0] = 0;
				cntInfo[1] = 0;
				cntInfo[2] = 0;
				
				if(i == 0)//只有第一条需要获取信息
				{
					recInfo.max = 50;
					recInfo.cnt = 0;
				}
				else
				{
					recInfo.max = 0;
					for(typeCnt = 0; typeCnt < recInfo.cnt; typeCnt++)
					{
						if(baobiaoType[i] == recInfo.recType[typeCnt])
						{
							break;
						}
					}
					/*if(typeCnt >= recInfo.cnt)
					{
						continue;
					}*/
				}
				if(recInfo.max != 0 || typeCnt < recInfo.cnt)
				{
					RecStore_GetRecordStateNum(dateStr,right,baobiaoType[i],&recInfo);
					cntInfo[0] = recInfo.reported+recInfo.unreported;
					cntInfo[1] = recInfo.reported;
					cntInfo[2] = recInfo.unreported;
					//RecStore_GetRecordStateNum(dateStr,right,baobiaoType[i],&total,&rep,&unrep);
				//hyUsbPrintf("按天报表 type = %d  tot = %d  rep = %d  un = %d \r\n",baobiaoType[i],rep+unrep,rep,unrep);
					//cntInfo[0] = rep+unrep;
					//cntInfo[1] = rep;
					//cntInfo[2] = unrep;
				}
				index = Op_Config_GetIndexByType(baobiaoType[i]);//找到该类型在数组中对应的位置
				memset(namebuf, 0, 20);
				if(strlen(gaOp_Config[index].pName) > 8)
				{
					memcpy(namebuf, gaOp_Config[index].pName, 8);
					//strcat(namebuf, "查询");
				}
				else
				{
					//sprintf(namebuf, "%s查询",gaOp_Config[index].pName);
					strcpy(namebuf, gaOp_Config[index].pName);
				}
				sprintf(pOutBuf+len, "%-9s%-6d%-6d%-6d%",namebuf ,cntInfo[0],cntInfo[1],cntInfo[2]);
				len += strlen(pOutBuf+len);
			}
		}
		else//查询指定类型
		{
			opCnt = 1;
			recInfo.max = 0;
			recInfo.cnt = 0;
			RecStore_GetRecordStateNum(dateStr,right,type,&recInfo);
			cntInfo[0] = recInfo.reported+recInfo.unreported;
			cntInfo[1] = recInfo.reported;
			cntInfo[2] = recInfo.unreported;
			//RecStore_GetRecordStateNum(dateStr,right,type,&total,&rep,&unrep);
			
			index = Op_Config_GetIndexByType(type);//找到该类型在数组中对应的位置
			memset(namebuf, 0, 20);
			if(strlen(gaOp_Config[index].pName) > 8)
			{
				memcpy(namebuf, gaOp_Config[index].pName, 8);
				//strcat(namebuf, "查询");
			}
			else
			{
				//sprintf(namebuf, "%s查询",gaOp_Config[index].pName);
				strcpy(namebuf, gaOp_Config[index].pName);
			}
			sprintf(pOutBuf, "%-9s%-6d%-6d%-6d%",namebuf,cntInfo[0],cntInfo[1],cntInfo[2]);
		}
	}
}


/*=============================================按日期查询界面======================================================*/
const tGrapViewAttr Rec_SearchByCodeViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif	
	{BUTTON_ID_3,  10,50,64,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//查询单号
	
	{EDIT_ID_2,   10, 90,210, 186,      10,  90,  220,  276,   0,	0,	0x0,0,1,0,1,0},
	
	{EDIT_ID_1,	  90, 45,133,24,  90, 45,223,70, 0,0,0xffff,0,1,1,1,0},
};

const tGrapButtonPriv  Rec_SearchByCodeBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"确定",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//确定
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
#else
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
	{(U8 *)"确定",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//确定
#endif	
	
	{(U8 *)"查询单号",0xffff,0xffff,0,0,1,0,TY_UNITY_BG1_EN, NULL},//查询单号
};

//edit
const tGrapEditPriv Rec_SearchByCodeEditPri[] = 
{
	{NULL,0x0,0,0,2,4,16,  1,BARCODE_LEN-1,0,0,2,0,0,1,  50,50,0,  0,1,1,1},
};

//edit2
const tGrapEdit2Priv  Rec_SearchByCodeEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,1024,2,2,0,0},//显示查询信息
};


int Rec_SearchByCode_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT2		*pEdit2;
	tGRAPEDIT		*pEdit;
	tGRAPDESKBOX	*pDesk;
	tOP_CALLBACK	tOpCb;
	int total,cnt,i;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)(pBtn->pParent);	
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确定
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			if(strlen(pEdit->edit_privattr.digt) > 0)
			{
				Com_SpcDlgDeskbox("正在搜索,请稍候...",0,pDesk,1,NULL,NULL,10);
				
				pEdit2   = (tGRAPEDIT2 *)Grap_GetViewByID(pDesk,EDIT_ID_2);
				
				//查询的具体记录操作信息
				total = RecStore_SearchByCode(Login_GetRight(), pDesk->view_attr.reserve, NULL, pEdit->edit_privattr.digt, 
							(U8 *)hsaSdram_UpgradeBuf(), 2048);
			hyUsbPrintf("search total = %d \r\n", total);				
				if(total > 0)
				{
					OpCom_SetCurOpType((U8)pDesk->view_attr.reserve);
					//调用对应的解析显示接口
					memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
					tOpCb.right = 0;
					tOpCb.funType = 3;
					tOpCb.opType = (U8)pDesk->view_attr.reserve;
					tOpCb.uOpcfg.tDisp.pData = (U8 *)hsaSdram_UpgradeBuf();
					tOpCb.uOpcfg.tDisp.total = total;
					tOpCb.uOpcfg.tDisp.pDispBuf = pEdit2->edit_privattr.digt;
					tOpCb.uOpcfg.tDisp.dispBufLen = pEdit2->edit_privattr.containLen;
					Op_Config_CallBack(&tOpCb);
				}
				else
				{
					memset(pEdit2->edit_privattr.digt, 0, pEdit2->edit_privattr.containLen);
					Com_SpcDlgDeskbox("无记录.",0,pDesk,1,NULL,NULL,100);
				}
				pEdit2->get_default_str(pEdit2);
			}
			break;
		case BUTTON_ID_2://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

/*---------------------------------------------
*函数:Rec_SearchByCodeDesk
*功能:按指定单号查询界面
*参数:type:业务类型
*返回:
*-----------------------------------------------*/
void Rec_SearchByCodeDesk(void *pDeskFather, U16 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
	
	pDesk = Grap_CreateDeskbox(&Rec_SearchByCodeViewAttr[idx++], "记录查询");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->view_attr.reserve = type;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	//按钮
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Rec_SearchByCodeViewAttr[idx++], &Rec_SearchByCodeBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    if(i < 2)
	    {
	    	pBtn->pressEnter = Rec_SearchByCode_BtnEnter;
	    }
    }

	//显示查询信息
	pEdit2 = Grap_InsertEdit2(pDesk, &Rec_SearchByCodeViewAttr[idx++], &Rec_SearchByCodeEdit2Attr[0],0);
	if(NULL == pEdit2 ) return;
	
	pEdit = Grap_InsertEdit(pDesk, &Rec_SearchByCodeViewAttr[idx++], &Rec_SearchByCodeEditPri[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(NULL == pEdit ) return;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return;
}

/*==============================================查询指定日期/权限的所有记录=================================================================*/
int Rec_SearchRecCode(void *p)
{
	tCARTOONPARA *pUp;
	U8		*pBuf, date[10];
	int		bufLen,i,total,offset,cnt;
	
	pUp    = (tCARTOONPARA *)p;
	pBuf   = pUp->pdata;
	bufLen = pUp->datalen;
	
	memset(date, 0, 10);
	strcpy(date, pBuf);
sprintf("search date = %s \r\n", date);	
	total = 0;
	//查询
	if(date[4] == 0)//按月查询
	{
		for(i = 1; i < 32; i++)
		{
			offset = total*sizeof(tREC_SEARCH);
			if(offset >= bufLen)//buf超出范围
			{
				break;
			}
			sprintf(&date[4], "%02d", i);
			
			cnt = RecStore_GetRecordCode(date, Login_GetRight(), gu8SearchType, pBuf+offset, bufLen-offset);
			if(cnt > 0)
			{
				total += cnt;
			}
		hyUsbPrintf("date = %s  total = %d  offset = %d \r\n", date, total, offset);
		}
	}
	else//查询指定一天
	{
		total = RecStore_GetRecordCode(date, Login_GetRight(), gu8SearchType, pBuf, bufLen);
		hyUsbPrintf("search total = %d \r\n", total);
	}
	
	gu32RecSearchTotal = (total > 0)? total: 0;
	
	return 1;
}

/*=============================================显示详细信息============================================================*/
const tGrapViewAttr Rec_ShowInfoViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//背景图片1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回	
	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,1,0},
};

const tGrapButtonPriv  Rec_ShowInfoBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapEdit2Priv  Rec_ShowInfoEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,2048,1,1,0,0},//显示
};

int Rec_ShowInfo_BtnPress(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------
*函数:Rec_ShowInfoDesk
*功能:显示记录详细信息
*参数:
*返回:
*-----------------------------------------------*/
void Rec_ShowInfoDesk(void *pDeskFather, tDATA_INFO *node)
{
	tOP_CALLBACK	tOpCb;
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	idx=0,type;
	
	type = node->tComm.opType;
	
	pDesk = Grap_CreateDeskbox(&Rec_ShowInfoViewAttr[idx++], "详细信息");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&Rec_ShowInfoViewAttr[idx++],&Rec_ShowInfoBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = Rec_ShowInfo_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &Rec_ShowInfoViewAttr[idx++], &Rec_ShowInfoEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	
	OpCom_SetCurOpType(type);
	//调用对应的解析显示接口
	memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
	tOpCb.right = 0;
	tOpCb.funType = 3;
	tOpCb.opType = (U8)type;
	tOpCb.uOpcfg.tDisp.pData = (U8 *)node;
	tOpCb.uOpcfg.tDisp.total = 1;
	tOpCb.uOpcfg.tDisp.pDispBuf   = pEdit2->edit_privattr.digt;
	tOpCb.uOpcfg.tDisp.dispBufLen = pEdit2->edit_privattr.containLen;
	Op_Config_CallBack(&tOpCb);
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}

/*=============================================显示查询到的所有单号======================================================*/
const tGrapViewAttr Rec_SearchList_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//详情
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//删除
	{BUTTON_ID_3,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_3,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//删除
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//详情
#endif	
	
	{BUTTON_ID_10,	   5,  38,72,16,    0,0,0,0,   0,0,0,0,1,0,0,0},//查询单号
	{EDIT_ID_1,	   	   78, 35,150,22,    78,35,230,57, 0,0,0xffff,0,1,0,1,0},
	
	{LISTER_ID_1,	   10,64,224,212,	10,64,234,276,	0,0,0,0,1,1,1,0},
};

/*const tGrapListerPriv Rec_SearchList_ListPriv[]=
{
	{24,8,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};*/

const tSupListPriv Rec_SearchList_SupListPriv[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 8, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  20,4,26,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

const tGrapButtonPriv Rec_SearchList_BtnPriv[]=
{
#ifdef W818C
	{"详情",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
	{"删除",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"删除",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//删除
	{"详情",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//确定
#endif	

	{"查询单号:",0xffff,0x0000,0,0,1,0,0,0},	
};

const tGrapEditPriv Rec_SearchList_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,15,  1, BARCODE_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,0},//查询单号
};

S32 Rec_SearchList_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tSUPPERLIST		*pSupList;
	tGRAPEDIT 		*pEdit;
	int		focus, fileId, recId,ret;
	U8		recState;
	
	
	if (state != STATE_NORMAL) return SUCCESS;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://查询
		pSupList = (tSUPPERLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		pSupList->SupListEnter(pSupList, pSupList->SupList_priv.focusItemNo);
		break;
	case BUTTON_ID_2://删除
		pSupList = (tSUPPERLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		focus = pSupList->SupList_priv.focusItemNo;
		if(gpu16RecSearchInfo[0] == 0xffff)//没有进行搜索
		{
			recState = gptRecSearch[focus].recState;
			fileId= gptRecSearch[focus].fileId;
			recId = gptRecSearch[focus].recIdx;
		}
		else
		{
			focus = gpu16RecSearchInfo[focus];
			recState = gptRecSearch[focus].recState;
			fileId= gptRecSearch[focus].fileId;
			recId = gptRecSearch[focus].recIdx;
		}
		
		if(recState == 1)//已传
		{
			Com_SpcDlgDeskbox("记录已上传,不能删除",0,pDesk,1,NULL,NULL,150);
		}
		else if(recState == 2)//已删
		{
			Com_SpcDlgDeskbox("记录已删除!",0,pDesk,1,NULL,NULL,150);
		}
		else
		{
			//从上传队列中删除
			ret = Queue_DelNode(fileId, recId);
			
			if(-1 == RecStore_DeleteRecord(fileId, recId))
			{
				Com_SpcDlgDeskbox("删除失败!",6,pDesk,1,NULL,NULL,150);
			}
			else
			{
				//如果是图片记录, 还需要同时删除图片
				if(gptRecSearch[focus].ifimage == 1)
				{
					if(0 == PicStore_Delete(gptRecSearch[focus].barCode))
					{
						if(gu32RecUnSendImage > 0) gu32RecUnSendImage--;
					}
				}
				else if(ret == -1)//从队列删除时没有找打该记录,要在这里减一
				{
					if(gu32RecUnSendTotal > 0) gu32RecUnSendTotal--;
				}
				
				//修改查询buf中的状态
				gptRecSearch[focus].recState = 2;
				Com_SpcDlgDeskbox("删除成功!",0,pDesk,1,NULL,NULL,150);
			}
		}
		
		break;
	case BUTTON_ID_3://返回
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

U32 Rec_SearchList_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tSUPPERLIST		*pSupList;
	U8	*pSub;
	int i,cmp = 0,total = 0;
	
	pEdit = (tGRAPEDIT *)pView;

	if(pEdit->view_attr.id == EDIT_ID_1)//
	{
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pSupList = (tSUPPERLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);//ID号一定确认好
		
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			if(type == 0 && gpu16RecSearchInfo[0] != 0xffff)//输入
			{
				for(i = 0; i < pSupList->SupList_priv.totalItem; i++)
				{
					cmp = 0;
					pSub = hy_strstr(gptRecSearch[gpu16RecSearchInfo[i]].barCode, pEdit->edit_privattr.digt);
					if(pSub != NULL)//有匹配字符串
					{
						cmp = 1;
					}
					
					if(cmp == 1)//匹配
					{
						gpu16RecSearchInfo[total++] = gpu16RecSearchInfo[i];
					}
				}
			}
			else
			{
				for(i = 0; i < gu32RecSearchTotal; i++)
				{
					cmp = 0;
					pSub = hy_strstr(gptRecSearch[i].barCode, pEdit->edit_privattr.digt);
					if(pSub != NULL)//有匹配字符串
					{
						cmp = 1;
					}
					
					if(cmp == 1)//匹配
					{
						gpu16RecSearchInfo[total++] = i;
					}
				}
			}
			pSupList->SupList_priv.topItemNo   = 0;
			pSupList->SupList_priv.focusItemNo = 0;
			pSupList->SupList_priv.totalItem = total;
		}
		else
		{
			pSupList->SupList_priv.topItemNo   = 0;
			pSupList->SupList_priv.focusItemNo = 0;
			pSupList->SupList_priv.totalItem = gu32RecSearchTotal;
			gpu16RecSearchInfo[0] = 0xffff;
		}
		
		if(pSupList)
		{
			pSupList->SupListUpDateByTotal(pSupList);
			pSupList->draw(pSupList, 0, 0);
		}
	}
	
	return SUCCESS ;
}

S32 Rec_SearchList_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	tDATA_INFO		node;
	int				ret,idx,size;
	U8				*pWork,*pPreview;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(pSupListPriv->totalItem == 0 || i >= pSupListPriv->totalItem)
	{
		return SUCCESS;
	}
	
	pDesk = (tGRAPDESKBOX *)(pSupList->pParent);
	
	if(gpu16RecSearchInfo[0] != 0xffff)
	{
		idx = gpu16RecSearchInfo[i];
	}
	else
	{
		idx = i;
	}
	
	if(gptRecSearch[idx].ifimage == 1)
	{
		pWork = (U8 *)hsaSdramReusableMemoryPool();
		pWork  = (char *)((unsigned)(pWork + 3) & 0xFFFFFFFC);
		pPreview = pWork + 307200;//300K
		size = PicStore_GetPic(gptRecSearch[idx].barCode,pWork,0x10000);
		if(size <= 0)
		{
			Com_SpcDlgDeskbox("查询图片失败!",0,pDesk,1,NULL,NULL,200);
		}
		else
		{
			if(0 == Photo_Decode(pWork,size,240,320,pWork+size,pPreview, TRUE))
			{
				ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
				Photo_DisplayDesk(pDesk,pPreview);
				ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			}
		}
	}
	else
	{
		if(1 == RecStore_SearchByIndex(gptRecSearch[idx].fileId, gptRecSearch[idx].recIdx, (U8 *)&node))
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			Rec_ShowInfoDesk(pDesk, &node);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		}
		else
		{
			Com_SpcDlgDeskbox("查询详细信息失败!",0,pDesk,1,NULL,NULL,200);
		}
	}
	
	return RETURN_REDRAW;
}

S32 Rec_SearchList_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	U8 *state[]={"未传", "已传", "已删", "未传", "未传"};
	U8 buf[6];
	
	memset(buf, 0, 6);
	if(gpu16RecSearchInfo[0] == 0xffff)//没有进行搜索
	{
		if(gptRecSearch[itemNo].ifimage == 1)
		{
			strcpy(buf, "图片");
		}
		sprintf(str, "%s  %s %s", state[gptRecSearch[itemNo].recState], gptRecSearch[itemNo].barCode, buf);
	}
	else
	{
		if(gptRecSearch[itemNo].ifimage == 1)
		{
			strcpy(buf, "图片");
		}
		sprintf(str, "%s  %s %s", state[gptRecSearch[gpu16RecSearchInfo[itemNo]].recState], gptRecSearch[gpu16RecSearchInfo[itemNo]].barCode, buf);
	}
	
	return SUCCESS;
}

/*---------------------------------------------------------
*函数: Rec_SearchList_Desk
*功能: 显示查询单号列表
*参数: 
*返回: 
*---------------------------------------------------------*/
int Rec_SearchList_Desk(void *pDeskFather, U8 *pDate, U16 type)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tSUPPERLIST		*pSupList;
	tGRAPEDIT 		*pEdit;
	tCARTOONPARA	tUp;
	U8              index = 0;
	int 			i,sel;
	
	gpu16RecSearchInfo = (U16 *)hsaSdram_FrontBuf1();
	gpu16RecSearchInfo[0] = 0xffff;
	
	gptRecSearch = (U8 *)hsaSdram_UpgradeBuf();
	gu32RecSearchTotal = 0;
	
	tUp.pdata   = (U8 *)gptRecSearch;
	tUp.datalen = hsaSdram_UpgradeSize();
	strcpy(tUp.pdata, pDate);
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 192);
	
	cartoon_app_entry(Rec_SearchRecCode,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	Com_CtnDlgDeskbox(pDeskFather,2,"正在查询,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
		
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	pDesk = Grap_CreateDeskbox(&Rec_SearchList_ViewAttr[index++], "查询列表");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->view_attr.reserve = type;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for(i = 0; i < 4; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&Rec_SearchList_ViewAttr[index++],&Rec_SearchList_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 3)
		{
			pBttn->pressEnter = Rec_SearchList_BtnEnter;
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &Rec_SearchList_ViewAttr[index++], &Rec_SearchList_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	pEdit->editKeyEnd = Rec_SearchList_EditKeyEnd;
	
	pSupList = Grap_InsertSupList(pDesk, &Rec_SearchList_ViewAttr[index++], &Rec_SearchList_SupListPriv[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = Rec_SearchList_SupListEnter;
	pSupList->SupListGetStr= Rec_SearchList_SupListGetStr;
	pSupList->SupList_priv.totalItem = gu32RecSearchTotal;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*=============================================显示统计信息界面======================================================*/
const tGrapViewAttr Rec_StatisticsShowViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//退出
	
	{BUTTON_ID_2,  10,30,64,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//显示查询的日期
	{BUTTON_ID_3,   0,50,240,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//
	
	{EDIT_ID_2,   0, 70,220, 206,      0,  70,  220,  276,   0,	0,	0x0,0,1,1,1,0},
	
};

const tGrapButtonPriv  Rec_StatisticsShowBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
	{NULL,COLOR_RED,COLOR_RED,0,0,1,0,TY_UNITY_BG1_EN, NULL},//显示查询的日期
	{(U8 *)"业务类型 总数  已传  未传",COLOR_YELLOW,COLOR_YELLOW,0,0,1,0,TY_UNITY_BG1_EN, NULL},//
};

//edit2
const tGrapEdit2Priv  Rec_StatisticsShowEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,1024,2,2,0,0},//显示查询信息
};


int Rec_StatisticsShow_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}


int Rec_StatisticsPrc(void *p)
{
	tCARTOONPARA	*pUp;
	U16				type;
	U8				date[10];
	
	pUp = (tCARTOONPARA *)p;
	memcpy(&type, pUp->pdata, 0);
	strcpy(date, pUp->pdata+2);
	
	Rec_SearchRecInfo(date, Login_GetRight(), type, pUp->pdata);
	
	return 1;
}


/*---------------------------------------------
*函数:Rec_StatisticsShowDesk
*功能:显示统计信息界面
*参数:pDate:查询日期 (如果pData的日期2位为0,则表示按月查询)
      type:查询类型
*返回:
*-----------------------------------------------*/
void Rec_StatisticsShowDesk(void *pDeskFather, U8 *pDate, U16 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	tCARTOONPARA	tUp;
	U8				dateStr[20];
	int				i,idx=0;
	
	pDesk = Grap_CreateDeskbox(&Rec_StatisticsShowViewAttr[idx++], "业绩统计");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	sprintf(dateStr, "20%s", pDate);
	//按钮
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Rec_StatisticsShowViewAttr[idx++], &Rec_StatisticsShowBtnAttr[i]);
		if(NULL == pBtn ) return;
		if(i == 0)
		{
			pBtn->pressEnter = Rec_StatisticsShow_BtnEnter;
		}
		else if(i == 1)
	    {
	    	pBtn->btn_privattr.pName = dateStr;
	    }
	}

	//显示查询信息
	pEdit2 = Grap_InsertEdit2(pDesk, &Rec_StatisticsShowViewAttr[idx++], &Rec_StatisticsShowEdit2Attr[0],0);
	if(NULL == pEdit2 ) return;
	
	//如果是业绩统计,需要进行查询
	
	tUp.pdata   = pEdit2->edit_privattr.digt;
	tUp.datalen = pEdit2->edit_privattr.containLen;
	memcpy(tUp.pdata, &type, 2);
	strcpy(tUp.pdata+2, pDate);
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 192);
	
	cartoon_app_entry(Rec_StatisticsPrc,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	Com_CtnDlgDeskbox(pDesk,2,"正在查询,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	//Rec_SearchRecInfo(pDate, Login_GetRight(), type, pEdit2->edit_privattr.digt);
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return;
}

/*=============================================任意一天选择界面======================================================*/
const tGrapViewAttr Rec_SearchAnyDayAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else
	{BUTTON_ID_2,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//确定
#endif
	{BUTTON_ID_3,	   70, 70 , 20, 16,   0,0,0,0,   0,0, 0,0,1,0,0,0},//年
	{BUTTON_ID_4,	   70, 100 ,20, 16,   0,0,0,0,   0,0, 0,0,1,0,0,0},//月
	{BUTTON_ID_5,	   70, 130 ,20, 16,   0,0,0,0,   0,0, 0,0,1,0,0,0},//日
	
	{EDIT_ID_1,	  95,  68, 40,24,  95, 68, 216,92,  0,0,0xffff,0,1,0,1,0},//年
	{EDIT_ID_2,	  95,  98, 40,24,  95, 98, 216,122, 0,0,0xffff,0,1,0,1,0},//月
	{EDIT_ID_3,	  95,  128,40,24,  95, 128,216,152, 0,0,0xffff,0,1,1,1,0},//日
};

const tGrapButtonPriv Rec_SearchAnyDayBtnAttr[]=
{
#ifdef W818C
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
	{"确定",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
#endif	
	{"年",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"月",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"日",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv Rec_SearchAnyDayEditPri[] = 
{
	{NULL,0x0,0,0,2,6,4,  1,4,0,0,2,0,0,1,  50,50,0,  0,0,1,0},//年
	{NULL,0x0,0,0,2,6,2,  1,2,0,0,2,0,0,1,  50,50,0,  0,0,1,0},//月
	{NULL,0x0,0,0,2,6,2,  1,2,0,0,2,0,0,1,  50,50,0,  0,0,1,0},//日
};

int Rec_SearchAnyDay_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT		*pEditYear, *pEditMonth, *pEditDay;
	int				year,month,day,daytotal;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)(pBtn->pParent);	
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://确认
			pEditYear = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			pEditMonth= (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
			pEditDay  = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
			
			if(strlen(pEditYear->edit_privattr.digt) && strlen(pEditMonth->edit_privattr.digt) && strlen(pEditDay->edit_privattr.digt))
			{
				year = atoi(pEditYear->edit_privattr.digt);
				month= atoi(pEditMonth->edit_privattr.digt);
				day  = atoi(pEditDay->edit_privattr.digt);
				if(year < 2012 || year >= 2100)
				{
					Com_SpcDlgDeskbox("年份输入有误.",0,pDesk,1,NULL,NULL,100);
					break;
				}
				else if(month < 1 || month > 12)
				{
					Com_SpcDlgDeskbox("月份输入有误.",0,pDesk,1,NULL,NULL,100);
					break;
				}
				else
				{
					daytotal = OpCom_Month[month-1];
					if(month == 2)//如果是2月  需要判断是否闰年
					{
						if( (year%400) == 0 || ((year%4) == 0 && (year%100) != 0) )
						{
							daytotal++;
						}
					}
					if(day < 1 || day > daytotal)
					{
						Com_SpcDlgDeskbox("日期输入有误.",0,pDesk,1,NULL,NULL,100);
						break;
					}
					
					sprintf(pDesk->view_attr.reserve, "%02d%02d%02d", year%100,month,day);
					pDesk->view_attr.reserve = 1;
					return RETURN_QUIT;
				}
			}
			break;
		case BUTTON_ID_2://退出
			pDesk->view_attr.reserve = 0;
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------
*函数:Rec_SearchAnyDayDesk
*功能:任意一天输入界面
*参数:pDate:(out)输入的日期
*返回:0:返回   1:查询
*-----------------------------------------------*/
int Rec_SearchAnyDayDesk(void *pDeskFather, U8 *pDate)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				ret,i,idx=0;
	RtcTime_st 		tRtc;
	
	pDate[0] = 0;
	pDesk = Grap_CreateDeskbox(&Rec_SearchAnyDayAttr[idx++], "记录查询");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->view_attr.reserve = pDate;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_3;
	
	//按钮
	for(i = 0; i < 5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Rec_SearchAnyDayAttr[idx++], &Rec_SearchAnyDayBtnAttr[i]);
	    if(NULL == pBtn ) return;
	    if(i < 2)
	    {
	    	pBtn->pressEnter = Rec_SearchAnyDay_BtnEnter;
	    }
    }

	hyhwRtc_GetTime(&tRtc);
	for(i = 0; i < 3; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &Rec_SearchAnyDayAttr[idx++], &Rec_SearchAnyDayEditPri[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return;
		if(i == 0)//年
		{
			sprintf(pEdit->edit_privattr.digt, "%04d", tRtc.year);
		}
		else if(i == 1)//月
		{
			sprintf(pEdit->edit_privattr.digt, "%02d", tRtc.month);
		}
		else if(i == 2)//日
		{
			sprintf(pEdit->edit_privattr.digt, "%02d", tRtc.day);
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	
	ret = pDesk->view_attr.reserve;
	Grap_DeskboxDestroy(pDesk);
	
	return ret;
}

/*=============================================业绩统计界面======================================================*/
const tGrapViewAttr Rec_StatisticsViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  Rec_StatisticsBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

const tGrapListerPriv Rec_StatisticsListAttr[]=
{   
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};


int Rec_Statistics_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	switch(i)
	{
	case 0:
		strcpy(str, "1:当天");
		break;
	case 1:
		strcpy(str, "2:昨天");
		break;
	case 2:
		strcpy(str, "3:前天");
		break;
	case 3:
		strcpy(str, "4:任意一天");
		break;
	case 4:
		strcpy(str, "5:当月");
		break;
	case 5:
		strcpy(str, "6:按单号查询");
		break;
	}
	
	return SUCCESS;
}

int Rec_Statistics_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	int ret;
	U8	date[10];
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	switch(i)
	{
	case 0://当天
		OpCom_GetCurDay(date, 10);
		break;
	case 1://昨天
		OpCom_GetBeforeDay(1, date);
		break;
	case 2://前天
		OpCom_GetBeforeDay(2, date);
		break;
	case 3://任意一天
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Rec_SearchAnyDayDesk(pDesk, date);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		if(ret == 0)
		{
			return RETURN_REDRAW;
		}
		break;
	case 4://当月
		OpCom_GetCurDay(date, 10);
		date[4] = 0;
		date[5] = 0;//按月查询时把日期2位清0
		break;
	case 5://按单号查询
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		Rec_SearchByCodeDesk(pDesk, gu8SearchType);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		return RETURN_REDRAW;
		break;
	default:
		break;
	}
	
	hyUsbPrintf("date  = %s   type  = %d  \r\n", date, gu8SearchType);
	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	if(gu8SearchType == 0 || gu8SearchType == HYCO_TYPE_MAX)
	{
		Rec_StatisticsShowDesk(pDesk, date, gu8SearchType);
	}
	else
	{
		Rec_SearchList_Desk(pDesk, date, gu8SearchType);
	}
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	
	return RETURN_REDRAW;
}


int Rec_Statistics_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------
*函数:Rec_Statistics
*功能:业绩统计界面    (当天   昨天   前天    任意一天    当月)
*参数:
*返回:
*-----------------------------------------------*/
int Rec_Statistics(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST		*pList;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox(&Rec_StatisticsViewAttr[idx++], "记录查询");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, &Rec_StatisticsViewAttr[idx++], &Rec_StatisticsBtnAttr[0]);
    if(NULL == pBtn ) return;
    pBtn->pressEnter = Rec_Statistics_BtnEnter;

    pList = Grap_InsertLister(pDesk, &Rec_StatisticsViewAttr[idx++], &Rec_StatisticsListAttr[0]);
    if(pList == NULL) return ;
    if(gu8SearchType == 0 || gu8SearchType == HYCO_TYPE_MAX)
    {
    	pList->lst_privattr.totalItem = 5;
    }
    else
    {
    	pList->lst_privattr.totalItem = 6;
	}
	pList->enter = Rec_Statistics_ListEnter;
	pList->getListString = Rec_Statistics_GetListStr;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


/*++++++++++++++++++++++++++++++++++++++++++++记录查询主界面++++++++++++++++++++++++++++++++++++++++++++++*/
const tGrapViewAttr Rec_SearchViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//返回
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  Rec_SearchBtnAttr[]=
{
	{(U8 *)"返回",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//返回
};

/*const tGrapListerPriv Rec_SearchListAttr[]=
{   
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv Rec_SearchSupListPriv[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,26,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

/*int Rec_Search_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if(i < OP_CONFIG_OFFSET)//统计
	{
		sprintf(str, "%2d:%s", i+1, gaOp_Config[i].pName);
	}
	else
	{
		sprintf(str, "%2d:%s查询", i+1, gaOp_Config[i].pName);
	}
	return SUCCESS;
}

int Rec_Search_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tOP_CALLBACK	tOpCb;
	int ret;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	ret = Op_Config_GetOpTypeByRight(0, i);
hyUsbPrintf("list enter optype = %d  \r\n", gu8SearchType);
	if(ret != -1)
	{
		gu8SearchType = (U8)ret;
		
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		//调用对应的解析显示接口
		memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
		tOpCb.right = 0;
		tOpCb.funType = 2;
		tOpCb.opType = gu8SearchType;
		tOpCb.uOpcfg.p = pDesk;
		Op_Config_CallBack(&tOpCb);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}*/

int Rec_Search_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	if(itemNo < OP_CONFIG_OFFSET)//统计
	{
		sprintf(str, "%2d:%s", itemNo+1, gaOp_Config[itemNo].pName);
	}
	else
	{
		sprintf(str, "%2d:%s查询", itemNo+1, gaOp_Config[itemNo].pName);
	}
	return SUCCESS;
}

int Rec_Search_SupListEnter(void *pView, U16 i)
{
	tSUPPERLIST		*pSupList;
	tGRAPDESKBOX	*pDesk;
	tOP_CALLBACK	tOpCb;
	int ret;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk = (tGRAPDESKBOX *)(pSupList->pParent);
	ret = Op_Config_GetOpTypeByRight(0, i);
hyUsbPrintf("list enter optype = %d  \r\n", gu8SearchType);
	if(ret != -1)
	{
		gu8SearchType = (U8)ret;
		
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		//调用对应的解析显示接口
		memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
		tOpCb.right = 0;
		tOpCb.funType = 2;
		tOpCb.opType = gu8SearchType;
		tOpCb.uOpcfg.p = pDesk;
		Op_Config_CallBack(&tOpCb);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}

int Rec_Search_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://退出
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}


/*---------------------------------------------
*函数:Rec_SearchDesk
*功能:查询记录主界面
*参数:
*返回:
*-----------------------------------------------*/
int Rec_SearchDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
	
	gu8SearchType = 0;

	pDesk = Grap_CreateDeskbox(&Rec_SearchViewAttr[idx++], "记录查询");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//按钮
	pBtn = Grap_InsertButton(pDesk, &Rec_SearchViewAttr[idx++], &Rec_SearchBtnAttr[0]);
    if(NULL == pBtn ) return -1;
    pBtn->pressEnter = Rec_Search_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &Rec_SearchViewAttr[idx++], &Rec_SearchListAttr[0]);
    if(pList == NULL) return -1;
    pList->lst_privattr.totalItem = Op_Config_GetCntByRight(0);
	pList->enter = Rec_Search_ListEnter;
	pList->getListString = Rec_Search_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &Rec_SearchViewAttr[idx++], &Rec_SearchSupListPriv[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = Rec_Search_SupListEnter;
	pSupList->SupListGetStr= Rec_Search_SupListGetStr;
	pSupList->SupList_priv.totalItem = Op_Config_GetCntByRight(0);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}






