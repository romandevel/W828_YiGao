#include "hyTypes.h"
#include "hyHandMain.h"


#define HYHAND_OFFSET	   (32602415)
#define HYHAND_SIZE		   (2144956)

#define IS_VALID_POS(X,Y)  ((X) != -1 || (Y) != -1)


static ISHWR_ENGINE_HANDLE	ghEngine;
static tHY_RECG				*ptHyRecg = NULL;
static U8					gsu8RecogNum;



/**************************
* 函数:hyHWRecog_Init
* 功能:初始化识别库
* 参数:none
* 返回:0:成功 -1:失败
****************************/
int hyHWRecog_Init()
{
	int		par;
	char    *pModelData;
	
	pModelData = (char *)hsaSdram_HandWriteBuf();
	
	HSA_AccessReservedArea(pModelData, HYHAND_OFFSET, HYHAND_SIZE);
	
	ghEngine = ISHWR_Initialize( pModelData, HYHAND_SIZE, NULL, ISHWR_ADAPT_SIZE );
	
	if(ghEngine == NULL)
	{
		return -1;
	}
	
    par = ISHWR_CHARSET_DEFAULT;
    ISHWR_SetParam( ghEngine, ISHWR_Param_CharSet, &par, sizeof(par) );
	
	gsu8RecogNum = 0;
	
	return 0;
}


/*---------------------------------------------------
 * 函数 : hyHWRecog_StartFlag
 * 功能 : 判断是否已经开启手写识别或者开启失败等等
 * 参数 : none
 * 返回 : 1:已经开启  0 ：没有开启 
---------------------------------------------------*/
static int hyHWRecog_StartFlag(void)
{
     if (ptHyRecg == NULL || 0 == ptHyRecg->enable) return 0;
     
     return 1;
}

/*---------------------------------------------------
 * 函数 : hyHWRecog_WakeParse
 * 功能 : 唤醒解析汉字进程
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
static void hyHWRecog_WakeParse(void)
{
      ptHyRecg->traceStatus = HY_TRACE_PARSE;
      ewakeup(&ptHyRecg->EventHyRcg);
}

/*---------------------------------------------------
 * 函数 : hyHWRecog_RecogTrace
 * 功能 : 记录手写点,及时没有点时也需要定时传入点
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_RecogTrace(int x, int y)
{
	U32 curTick;
	int id;
	int traceStatus;
	
	if(0 == hyHWRecog_StartFlag())
	{
		return; 
	}
	
	traceStatus = ptHyRecg->traceStatus;
	
	//在解析轨迹时 
	if(HY_TRACE_PARSE == traceStatus || (HY_TRACE_IDEL == traceStatus && !IS_VALID_POS(x,y)))
	{
		return ;
	}
	
	curTick  = rawtime(NULL);

	id = ptHyRecg->index;

	if (id + 5 > HY_REC_BUF_SIZE) 
	{
		//id = 2;
		//需要唤醒进程进行解析汉字
		hyHWRecog_WakeParse();
		
		return ;
	}
	
	//没有触摸点时，处理状态    
	if (!IS_VALID_POS(x,y))
	{
		int diffTick;

		if(id == 0)
		{
			return;
		}

		diffTick = ABS(curTick - ptHyRecg->lastTick);

		//要么输入(-1,-1) 要么不输入
		if(diffTick >= HY_TRACE_TIME_STOP)
		{
			if (ptHyRecg->recBuf[id - 1].x != -1)
			{
				ptHyRecg->recBuf[id].x   = -1;
				ptHyRecg->recBuf[id++].y = -1;
			}

			//需要唤醒进程进行解析汉字
			hyHWRecog_WakeParse();
		} 
		else if (diffTick >= HY_TRACE_TIME_PAUSE)
		{
			if (-1 != ptHyRecg->recBuf[id - 1].x)
			{
				ptHyRecg->recBuf[id].x   = -1;
				ptHyRecg->recBuf[id++].y = -1;
				ptHyRecg->index = id;
			} 
		}

		return;
	}

	//有触摸点时只管记录输入点
	if(traceStatus == HY_TRACE_IDEL)
	{
		ptHyRecg->traceStatus = HY_TRACE_START;
	}

	ptHyRecg->recBuf[id].x   = x;
	ptHyRecg->recBuf[id++].y = y;
	ptHyRecg->index = id;

	ptHyRecg->lastTick = curTick;
	
	return ;
}

/*---------------------------------------------------
 * 函数 : hyHWRecog_GetLetter
 * 功能 : 得到识别字符  
 * 参数 : pbuf:传入的buf
 * 返回 : 得到的字符数  <0 没有字符  0：需要刷新字符 
---------------------------------------------------*/
int hyHWRecog_GetLetter(char *pbuf)
{
	int len = 0;

	if (0 == hyHWRecog_StartFlag() || 0 == ptHyRecg->outFlag) return -1; 

	if (2 == ptHyRecg->outFlag)
	{
		ptHyRecg->outFlag = 0;
		return 0;
	}  

	memcpy(pbuf,ptHyRecg->outBakBuf,gsu8RecogNum<<1);
//hyUsbPrintf("Recog str = %s   num = %d \r\n", pbuf, gsu8RecogNum);
	len = gsu8RecogNum;
	
	ptHyRecg->outFlag = 0;
	gsu8RecogNum = 0;
	
	return len;
}

/*---------------------------------------------------
 * 函数 : hyHWRecog_EnableFlag
 * 功能 : 失能/使能手写识别
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_EnableFlag(int type)
{
    ptHyRecg->enable = type;
    
    //外部关闭的时候，清除buf内容
    if (type == 0)
    {
    	 ptHyRecg->outFlag = 0;
    	 ptHyRecg->traceStatus = HY_TRACE_IDEL;
    	 ptHyRecg->index = 0; 
    } 
}

/*---------------------------------------------------
 * 函数 : hyHWRecog_RecognizeMain
 * 功能 : 手写识别主循环函数
 * 参数 : none
 * 返回 : none 
---------------------------------------------------*/
void hyHWRecog_RecognizeMain(void)
{
	int id,i;
	char buf[3];
	U16 uniArray[2];

	ptHyRecg = (tHY_RECG *)malloc(sizeof(tHY_RECG));
	if (NULL == ptHyRecg)
	{
		while(1);
	}

	memset(ptHyRecg,0,sizeof(tHY_RECG));

	while(1)
	{
		ewait(&ptHyRecg->EventHyRcg);
		if(ptHyRecg->index > 5)
		{
			gsu8RecogNum = ISHWR_Recognize( ghEngine, ptHyRecg->recBuf, ptHyRecg->index, ptHyRecg->outBuf, 5 );
			if (gsu8RecogNum > 0)
			{
				for (i=0; i<gsu8RecogNum; i++)
				{
					uniArray[0] = ptHyRecg->outBuf[i];
					uniArray[1] = 0;
					
					Hyc_Unicode2Gbk( (char *)&uniArray, ptHyRecg->outBakBuf+(i<<1), 0);
				}

				ptHyRecg->outFlag = 1;
			}
		}
		else
		{
			ptHyRecg->outFlag = 2; 
		}

		ptHyRecg->index = 0;
		ptHyRecg->traceStatus = HY_TRACE_IDEL;
	}

	free(ptHyRecg);

	return ;
}