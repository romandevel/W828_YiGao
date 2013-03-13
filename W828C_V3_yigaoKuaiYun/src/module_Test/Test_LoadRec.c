#include "Test_MainList.h"
#include "hyswNandFile.h"


/*=============================================导出记录===================================================*/
const tGrapViewAttr TestLoadRec_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//LB识别
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//导出
#endif	
	{EDIT_ID_1,   10, 40,210, 240,      10,  40,  220,  280,   0,	0,	0,0,1,0,0,0},//
};

const tGrapButtonPriv TestLoadRec_BtnPriv[]=
{
#ifdef W818C
	{"导出",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//导出
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"导出",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//导出
#endif
};

const tGrapEdit2Priv  TestLoadRec_Edit2Attr[]=
{
 	{NULL,	COLOR_YELLOW,0,0,0,TY_UNITY_BG1_EN,0,0,2048,1,1,0,0},//显示
};



void TestLoadRec_GetQueData()
{
	U32 kQueCnt,i;
	tQUE_NODE tQue;
	int fp;
#if 0	
//	YDnet_Init();
	//导出所有记录
//	YDstore_GetAllUnreported(0);
	
	kQueCnt = YDqueue_getQueSendcnt();
	if(kQueCnt > 0)
	{
	hyUsbPrintf("queue cnt = %d \r\n", kQueCnt);
		fp = AbstractFileSystem_Open("C:/7788/que.bin",AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile("C:/7788/que.bin",AFS_SINGLE_DELETE_FDM);
		}
		fp = AbstractFileSystem_Create("C:/7788/que.bin");
		
		for(i = 0; i < kQueCnt; i++)
		{
			YDqueue_getNodedata(&tQue, 1);
			
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp, (char *)&tQue, 512);
			}
			YDqueue_delHeadNode(0,&tQue,1);
			//YDnet_SendData(&tQue,1);
		}
		AbstractFileSystem_Close(fp);
	}
#endif
}

void TestLoadRec_Write2File(int fp, tQUE_NODE *pQue, int total)
{
	U8	 		*state[]={"未传","已传","已删"};
	U8		 	writeBuf[600];
	int			i,delay;
	U16   		datatype;
	tSAOMIAO	*saomiao;
		
	for(i = 0; i < total; i++)
	{
		datatype = pQue[i].opType;

		memset(writeBuf, 0, 600);
		memcpy(writeBuf, (char *)&pQue[i], sizeof(tQUE_NODE));
		AbstractFileSystem_Write(fp, writeBuf, 512);
		
		delay = 100000;
		while(delay--);
	}
}

int TestLoadRec_LoadRecode(U8 *pName)
{
	U8	 	  *state[]={"未传","已传","已删","待传","可传"};
	tQUE_NODE *pQue;
	U8		  writeBuf[600];
	int		  lastName=-1;
	int		  i,total,fileIdx;
	int		  fp,fpAll,delay;
	char	  temp[12],path[20];
	
	
	pQue = (tQUE_NODE *)hsaSdramReusableMemoryPool();
	
	if(pName != NULL)//读指定日期的
	{
		total = RecStore_LoadInfo(pName, (U8 *)pQue, 0x300000);
		
		if(total > 0)
		{
			sprintf(path,"C:/7788/%s.txt",pName);
		
			fp = AbstractFileSystem_Open(path,AFS_READ);
			if(fp >= 0)
			{
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
			}
			fp = AbstractFileSystem_Create(path);
			if(fp >= 0)
			{
				TestLoadRec_Write2File(fp, pQue, total);
				AbstractFileSystem_Close(fp);
			}
		}
	}
	else//读取全部
	{
		fpAll = AbstractFileSystem_Open("C:/7788/all.txt",AFS_READ);
		if(fpAll >= 0)
		{
			AbstractFileSystem_Close(fpAll);
			AbstractFileSystem_DeleteFile("C:/7788/all.txt",AFS_SINGLE_DELETE_FDM);
		}
		fpAll = AbstractFileSystem_Create("C:/7788/all.txt");
		for(fileIdx = 0; fileIdx < MAX_NAND_FILE; fileIdx++)
		{
		hyUsbPrintf("name = %d \r\n", gtFileEntry[fileIdx].name);
			if(gtFileEntry[fileIdx].name != -1 && gtFileEntry[fileIdx].name != lastName)
			{
				lastName = gtFileEntry[fileIdx].name;
				
				pQue = (tQUE_NODE *)hsaSdramReusableMemoryPool();
				memset(temp, 0, 12);
				sprintf(temp, "%d", gtFileEntry[fileIdx].name);
				total = RecStore_LoadInfo(temp, (U8 *)pQue, 0x300000);
				
				if(total > 0)
				{
					sprintf(path,"C:/7788/%s.txt",temp);
				hyUsbPrintf("---------------total = %d -- file = %s \r\n", total,path);
					
					memset(writeBuf, 0, 600);
					sprintf(writeBuf, "%s\r\n", path);
					if(fpAll >= 0)
					{
						AbstractFileSystem_Write(fpAll, writeBuf, strlen(writeBuf));
					}
					
					fp = AbstractFileSystem_Open(path,AFS_READ);
					if(fp >= 0)
					{
						AbstractFileSystem_Close(fp);
						AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
					}
					fp = AbstractFileSystem_Create(path);
					if(fp >= 0)
					{						
						for(i = 0; i < total; i++)
						{
							//hyUsbPrintf("state = %d   %s   type = %d   idx = %d \r\n",pQue[i].delFlag, state[pQue[i].delFlag], datatype, i);
							memset(writeBuf, 0, 600);
							sprintf(writeBuf, "state = %d   %s   type = %d   idx = %d\r\n", 
									pQue[i].delFlag, state[pQue[i].delFlag], pQue[i].opType, i);
							
							if(fpAll >= 0)
							{
								AbstractFileSystem_Write(fpAll, writeBuf, strlen(writeBuf));
							}
							
							memset(writeBuf, 0, 600);
							memcpy(writeBuf, (char *)&pQue[i], sizeof(tQUE_NODE));
							AbstractFileSystem_Write(fp, writeBuf, 528);
							
							delay = 100000;
							while(delay--);
						}
						
						AbstractFileSystem_Close(fp);
					}
				}
			}
		}
		AbstractFileSystem_Close(fpAll);
	}

	return 0;
}

int TestLoadRec_Proc(void *p)
{
	
	TestLoadRec_GetQueData();
	TestLoadRec_LoadRecode(NULL);
	
	return 1;
}

int TestLoadRec_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT2		*pEdit2;
	U32 len;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://导出
			if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("是否导出数据?",0, pDesk, 3, NULL, NULL, 3000))
			{
				return RETURN_REDRAW;
			}
			
			ConSume_UpdataSpeedDirect(SHELL_AN, 192);
			
			cartoon_app_entry(TestLoadRec_Proc,NULL);
			AppCtrl_CreatPro(KERNAL_APP_AN, 0);
			Com_CtnDlgDeskbox(pDesk,2,"正在导出记录,请稍候...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
			AppCtrl_DeletePro(KERNAL_APP_AN);
			
			ConSume_UpdataSpeedDirect(SHELL_AN, 0);
			
			Com_SpcDlgDeskbox("导出完成!",0,pDesk,1,NULL,NULL,100);
			return RETURN_QUIT;
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

/*-------------------------------------------------------------
* 函数:TestFun_LoadRec
* 功能:导出记录
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_LoadRec(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int i, idx = 0;
	
	pDesk = Grap_CreateDeskbox(&TestLoadRec_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 2; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestLoadRec_ViewAttr[idx++], &TestLoadRec_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		pBtn->pressEnter = TestLoadRec_BtnEnter;
	}
	
	pEdit2 = Grap_InsertEdit2(pDesk, &TestLoadRec_ViewAttr[idx++], &TestLoadRec_Edit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"说明:把巴枪中存储的所有记录保存成文件.");
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}
