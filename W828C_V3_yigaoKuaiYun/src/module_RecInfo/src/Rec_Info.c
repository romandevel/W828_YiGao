#include "hyhwRtc.h"
#include "Rec_Info.h"

/*===============================================================*/
U32		gu32RecUnSendTotal;//未上传记录总数
U32		gu32RecUnSendImage;//未上传图片总数
U32		gu32RecInNandTotal;//未上传记录在nand上未被加载的条数
U32		gu32LastScanTime;//最后一次扫描时间
U32		gu32MultiInNand;	//一票多件在nand上存储的数量(还未加载到队列中)

tREC_CONFIG gtRecConfig;

tREC_EXIST	gtRecExist;
static 	U32 gu32LockExist;

/*--------------------------------------
*函数：Rec_InfoInit
*功能：信息初始化
*参数：pStoreCfg:配置信息
*返回：0:成功  其他:失败
*--------------------------------------*/
int Rec_InfoInit(tSTORE_CFG *pStoreCfg)
{
	int ret = -1;
	
	gu32RecUnSendTotal = 0;
	gu32RecUnSendImage = 0;
	gu32RecInNandTotal = 0;
	gu32LastScanTime   = 0xFFFFFFFF;
	gu32MultiInNand    = 0;
	
	gu32LockExist = 0;
		
	memset((char *)&gtRecExist,0,sizeof(tREC_EXIST));
	gtRecExist.exist  = (tEXIST_CHECK *)(pStoreCfg->pExistBuf);
	gtRecExist.maxcnt = pStoreCfg->existBufSize/sizeof(tEXIST_CHECK);
	
	gtRecConfig.groupMax = pStoreCfg->groupMax;
	gtRecConfig.existMax = gtRecExist.maxcnt;
	gtRecConfig.delaySend= pStoreCfg->delaySecond*100;

	Queue_Init(pStoreCfg->pQueBuf, pStoreCfg->queBufSize, pStoreCfg->pMultiCodeBuf, pStoreCfg->multiBufSize);
	
	ret = RecStore_Init(pStoreCfg->pSmallRecBuf, pStoreCfg->smallBufSize);
	if(ret== 0)
	{
		ret= PicStore_Init(pStoreCfg->pLargeRecBuf, pStoreCfg->largeBufSize);
	}
	
	return ret;
}

void Rec_SetScanTime(U32 tim)
{
	gu32LastScanTime = tim;
}

/*--------------------------------------
*函数：Rec_CheckRecMaxSize
*功能：检查记录数据结构的大小
*参数：
*返回：当前大小
*--------------------------------------*/
int Rec_CheckRecMaxSize()
{
	return sizeof(tDATA_INFO);
}

/*--------------------------------------
*函数：Rec_GetExistMax
*功能：获取重复扫描判断的最大数
*参数：none
*返回：重复扫描判断的最大数
*--------------------------------------*/
U16 Rec_GetExistMax()
{
	return gtRecConfig.existMax;
}

/*--------------------------------------
*函数：Rec_GetExistMax
*功能：获取最大打包上传数
*参数：none
*返回：最大打包上传数
*--------------------------------------*/
U16 Rec_GetGroupMax()
{
	return gtRecConfig.groupMax;
}

/*--------------------------------------
*函数：Rec_GetDelaySend
*功能：获取延迟发送的最大时间
*参数：none
*返回：延迟发送的最大时间
*--------------------------------------*/
U16 Rec_GetDelaySend()
{
	return gtRecConfig.delaySend;
}

/*--------------------------------------
*函数：Rec_GetUnSendCnt
*功能：获取有多少条记录未上传   包括一票多件的
*参数：none
*返回：未上传的记录条数
*--------------------------------------*/
U32 Rec_GetUnSendCnt()
{
	return gu32RecUnSendTotal+gu32MultiInNand;
}

/*--------------------------------------
*函数：Rec_GetUnSendImageCnt
*功能：获取有多少条图片记录未上传
*参数：none
*返回：未上传的图片记录条数
*--------------------------------------*/
U32 Rec_GetUnSendImageCnt()
{
	return gu32RecUnSendImage;
}

/*--------------------------------------
*函数：Rec_GetMultiCnt
*功能：获取有多少票一票多件的单子   !!!!并不一定是准确数据
*参数：none
*返回：
*--------------------------------------*/
U32 Rec_GetMultiCnt()
{
	return gu32MultiInNand;
}

/*--------------------------------------
*函数：Rec_DelMultiCnt
*功能：减少一个一票多件的单子
*参数：none
*返回：
*--------------------------------------*/
void Rec_DelMultiCnt()
{
	if(gu32MultiInNand > 0)
	{
		gu32MultiInNand--;
	}
}

/*--------------------------------------
*函数：Rec_GetInNandCnt
*功能：获取有多少条记录在nand上未被加载
*参数：none
*返回：未被加载的记录条数
*--------------------------------------*/
U32 Rec_GetInNandCnt()
{
	return gu32RecInNandTotal;
}

/*-----------------------------------------------------------
*函数：Rec_GetExistCnt
*功能：获取已经存在条码的个数
*参数：
*返回：已经操作的个数
*------------------------------------------------------------*/
int Rec_GetExistCnt()
{
	return gtRecExist.curcnt;
}

/*-----------------------------------------------------------
*函数：Rec_CheckExist
*功能：检查是否存在相同的条码
*参数：pCode:扫描码
*返回：-1 条码不存在    ==0 条码已存在
*------------------------------------------------------------*/
int Rec_CheckExist(U8 *pCode)
{
	int i,t,ret = -1;
	
	if(NULL == pCode)
		return -1;
	
	/*if(ABS(rawtime(NULL) - gu32LastScanTime) > Rec_GetDelaySend())
	{
		return -1;
	}*/
	wlock(&gu32LockExist);
	
	hyUsbPrintf("have exist cnt = %d \r\n",gtRecExist.curcnt);
	for(i = 0,t = gtRecExist.rd; i < gtRecExist.curcnt; i++,t++)
	{
		if(t >= gtRecExist.maxcnt)
		{
			t -= gtRecExist.maxcnt;
		}
		hyUsbPrintf("bar = %s   cur = %s \r\n",pCode, gtRecExist.exist[t]);
		if(strcmp(gtRecExist.exist[t].code,pCode) == 0)//已经存在
		{
			ret = 0;
			//gu32LastScanTime = rawtime(NULL);
			break;
		}
	}
	
	unlock(&gu32LockExist);
	
	return ret;
}

/*-----------------------------------------------------------
*函数：Rec_AddExist
*功能：添加一个条码
*参数：pCode:扫描码
*返回：-1 失败    ==0 成功
*------------------------------------------------------------*/
int Rec_AddExist(U8 *pCode, U8 *pOther)
{
	int ret = 0,len;
	
	if(NULL == pCode)
		return -1;
	
	wlock(&gu32LockExist);
	
	memset(&gtRecExist.exist[gtRecExist.wr], 0, sizeof(tEXIST_CHECK));
	strcpy(gtRecExist.exist[gtRecExist.wr].code, pCode);
	if(pOther != NULL)
	{
		len = strlen(pOther);
		if(len >= 32)
		{
			len = 30;
		}
		memcpy(gtRecExist.exist[gtRecExist.wr].other, pOther, len);
		gtRecExist.exist[gtRecExist.wr].other[len] = 0;
	}
	
	//hyUsbPrintf("111 AddExist rd = %d wr = %d  cnt = %d max = %d   code = %s\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt,gtRecExist.exist[gtRecExist.wr].code);
	
	gtRecExist.wr++;
	if(gtRecExist.curcnt >= gtRecExist.maxcnt)
	{
		gtRecExist.rd++;
	}
	else
	{
		gtRecExist.curcnt++;
	}
	
	if(gtRecExist.wr >= gtRecExist.maxcnt)
	{
		gtRecExist.wr -= gtRecExist.maxcnt;
	}
	
	if(gtRecExist.rd >= gtRecExist.maxcnt)
	{
		gtRecExist.rd -= gtRecExist.maxcnt;
	}
	//hyUsbPrintf("222 AddExist rd = %d wr = %d  cnt = %d max = %d\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt);
	unlock(&gu32LockExist);
	
	return ret;
}


/*-----------------------------------------------------------
*函数：Rec_GetExistCodeByIdx
*功能：通过索引找到条码   反序查找   及最后存储的一条算第0个
*参数：idx索引  从0开始   
*返回：
*------------------------------------------------------------*/
tEXIST_CHECK *Rec_GetExistCodeByIdx(int idx)
{
	int i;
//hyUsbPrintf("idx = %d  curcnt = %d \r\n", idx, gtRecExist.curcnt);	
	if(idx >= gtRecExist.curcnt)
	{
		return NULL;
	}
	
	i = gtRecExist.wr-1-idx;
	if(i < 0) i = gtRecExist.maxcnt+i;
//hyUsbPrintf("exist i = %d \r\n", i);	
	return &gtRecExist.exist[i];
}


/*-----------------------------------------------------------
*函数：Rec_DelExist
*功能：删除一个已存在条码
*参数：pCode:扫描码
*返回：-1 失败    ==0 成功
*------------------------------------------------------------*/
int Rec_DelExist(U8 *pCode)
{
	int i,t,ret = -1;
	
	if(NULL == pCode)
		return ret;
	
	wlock(&gu32LockExist);
	
	if(gtRecExist.curcnt > 0)
	{
		for(i = 0,t = gtRecExist.rd; i < gtRecExist.curcnt; i++,t++)
		{
			if(t >= gtRecExist.maxcnt)
			{
				t -= gtRecExist.maxcnt;
			}
			//hyUsbPrintf("DelExist bar = %s   cur = %s \r\n",pCode, gtRecExist.exist[t]);
			if(strcmp(gtRecExist.exist[t].code,pCode) == 0)//已经存在
			{
				//hyUsbPrintf("del cnt = %d   %s\r\n",t,pCode);
				ret = 0;
				break;
			}
		}
		//hyUsbPrintf("DelExist rd = %d wr = %d  cnt = %d max = %d\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt);
		if(ret == 0)//找到
		{
			if(gtRecExist.curcnt == 1)
			{
				gtRecExist.rd = 0;
				gtRecExist.wr = 0;
				gtRecExist.curcnt = 0;
				//hyUsbPrintf("clear all  end \r\n");
			}
			else if(gtRecExist.curcnt > 1)
			{
				if(t == gtRecExist.rd)
				{
					gtRecExist.rd++;
				}
				else if(t > gtRecExist.rd)
				{
					memmove(&gtRecExist.exist[gtRecExist.rd+1],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK)*(t-gtRecExist.rd));
					gtRecExist.rd++;
				}
				else//t < gtRecExist.rd
				{
					memmove(&gtRecExist.exist[1],&gtRecExist.exist[0],sizeof(tEXIST_CHECK)*t);
					if(gtRecExist.rd == gtRecExist.maxcnt-1)
					{
						memcpy(&gtRecExist.exist[0],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK));
					}
					else
					{
						memcpy(&gtRecExist.exist[0],&gtRecExist.exist[gtRecExist.maxcnt-1],sizeof(tEXIST_CHECK));
						memmove(&gtRecExist.exist[gtRecExist.rd+1],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK)*(gtRecExist.maxcnt-1-gtRecExist.rd));
					}
					gtRecExist.rd++;
				}
				
				if(gtRecExist.rd >= gtRecExist.maxcnt)
				{
					gtRecExist.rd -= gtRecExist.maxcnt;
				}
				gtRecExist.curcnt--;
			}
		}
	}
	
	unlock(&gu32LockExist);
	
	return ret;
}

/*-----------------------------------------------------------
*函数：Rec_DelExistCnt
*功能：减少计数
*参数：
*返回：none
*------------------------------------------------------------*/
void Rec_DelExistCnt()
{
	wlock(&gu32LockExist);
	
	if(gtRecExist.curcnt > 0)
	{
		gtRecExist.curcnt--;
		gtRecExist.rd++;
		if(gtRecExist.rd >= gtRecExist.maxcnt)
		{
			gtRecExist.rd -= gtRecExist.maxcnt;
		}
	}
	
	unlock(&gu32LockExist);
}

/*-----------------------------------------------------------
*函数：Rec_ClearExist
*功能：清空重复条码判断计数
*参数：
*返回：none
*------------------------------------------------------------*/
void Rec_ClearExist()
{
	wlock(&gu32LockExist);
	
	gtRecExist.rd = 0;
	gtRecExist.wr = 0;
	gtRecExist.curcnt = 0;
	
	unlock(&gu32LockExist);
}

/*--------------------------------------
*函数：Rec_WriteNewRecode
*功能：写入新记录
*参数：pData：要写入的数据
*返回：成功：0   失败：-1
*--------------------------------------*/
int Rec_WriteNewRecode(tDATA_INFO *pdata, U8 *pOther)
{
	U32         idx;
	U8			buf[8], code[32];
	int         fileIdx ;
	int         ret ,i;
	U16	        opType;
	
	code[0] = 0;
	if(strlen(pdata->tComm.subCode) == 0)//没有子单号,是一票一件的
	{
		strcpy(code, pdata->tComm.code);
	}
	else
	{
		strcpy(code, pdata->tComm.subCode);
	}
	opType = pdata->tComm.opType;
	
	OpCom_GetCurDay(buf,8);
	fileIdx = RecStore_WriteRecord(buf, pdata, &idx);
	if (fileIdx != HY_ERROR)
	{
		if(pdata->tComm.ifimage == 1)
		{
			//图片数量+1
			gu32RecUnSendImage++;
		}
		else
		{
			if(pdata->tComm.state == 0)//可以直接添加到队列上传的
			{
				Queue_Add2Que(fileIdx,idx,opType,pdata);
			}
			if(code[0] != 0)
			{
				Rec_AddExist(code, pOther);
			}
		}
		ret = 0;
	}
	else
	{
		if(pdata->tComm.ifimage == 1)
		{
			//图片数量+1
			gu32RecUnSendImage++;
		}
		else
		{
			if(pdata->tComm.state == 0)//可以直接添加到队列上传的
			{
				Queue_Add2Que(100,0,opType,pdata);
			}
			if(code[0] != 0)
			{
				Rec_AddExist(code, pOther);
			}
		}
		ret = -1;
	}
//hyUsbPrintf("wr gu32RecInNandTotal = %d \r\n",gu32RecInNandTotal);	
	//if(gu32RecInNandTotal == 0)
		gu32LastScanTime = rawtime(NULL);

	return ret;
}
