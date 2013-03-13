#include "Rec_Queue.h"
#include "Rec_Info.h"

/*===============================================================*/

tDATA_QUEUE	gtData_Que;	//数据队列

tMULTI_CODE	*gptMultiCode;

static U32 gu32QueueLock;

U8	gau8MainCode[32];


/*----------------------------------------------------------------*/

/*----------------------------------------------------------
* 函数: Queue_Init
* 功能: 队列初始化操作
* 参数: pQueBuf:外部提供buf   
        bufsize:pQueBuf的大小
        pMultiBuf:提供给一票多件加载数据使用   
        multiBufSize:pMultiBuf 大小   >=250K
* 返回: none 
------------------------------------------------------------*/
void Queue_Init(U8 *pQueBuf, int bufsize, U8 *pMultiBuf, int multiBufSize)
{
	int nodesize;
	
	nodesize = sizeof(tQUE_NODE);
	
	gtData_Que.rd 		= 0;
	gtData_Que.wr 		= 0;
	gtData_Que.delCnt	= 0;
	gtData_Que.size		= bufsize/nodesize;
	gtData_Que.pQueBuf	= (tQUE_NODE *)pQueBuf;
	
	gu32QueueLock = 0;
	
	gptMultiCode = pMultiBuf;
	gptMultiCode->ifOver   = 0;
	gptMultiCode->sendType = 0;
	gptMultiCode->total    = 0;
	gptMultiCode->maxNum   = (multiBufSize-600)/BARCODE_LEN;
	
	memset(gau8MainCode, 0, 32);
}

/*----------------------------------------------------------
* 函数: Queue_GetMainCode
* 功能: 获取一票多件的主单号
* 参数: 
* 返回: 
------------------------------------------------------------*/
U8 *Queue_GetMainCode()
{
	return gau8MainCode;
}

/*----------------------------------------------------------
* 函数: Queue_AddNode_Multi
* 功能: 添加一个子单
* 参数: sendType:发送方式   0:后台发送    1:前台发送
* 返回: 0:成功   -1:失败 
------------------------------------------------------------*/
int Queue_AddNode_Multi(U8	*pMainCode, U8 *pSubCode, tDATA_INFO *pData, U8 sendType)
{
	
	//总数已经超过最大数目
	if(gptMultiCode->total >= MAX_SUBCODE_NUM)
	{
		return -1;
	}
	
	//之前已经有存储,本次的主单号和之前不一样,则不存储该条
	if(gu32MultiInNand >= 1)
	{
		//出现这种情况,说明存在不止一个一票多件
		if(gptMultiCode->total > 0 && strcmp(gptMultiCode->mainCode, pMainCode) != 0)
		{
			gu32MultiInNand++;//这里直接使票数加1,后续如果gu32MultiInNand>1  就不会再走这里,所以gu32MultiInNand并不是实际存在的票数,
		  	                  //只是方便知道后面还有需要加载的票数
		}
		
		return 0;
	}
	
	//如果是第一次添加,则需要把主单号和详细信息添加进来
	if(gptMultiCode->total == 0)
	{
		//memcpy(&(gptMultiCode->tData), pData, sizeof(tDATA_INFO));//2012-10-30 kong  最后一票时加载这些信息比较合理
		//strcpy(gptMultiCode->mainCode, pMainCode);
		//gptMultiCode->sendType = sendType;//发送方式
	}
	
	
	if(strcmp(pMainCode, pSubCode) != 0)
	{
		strcpy(gptMultiCode->subCode[gptMultiCode->total], pSubCode);
		gptMultiCode->total++;
	}
	else
	{
		//如果本次主单号和子单号一样,则说明这是本票的最后一个结束信息,不用添加进来
		memcpy(&(gptMultiCode->tData), pData, sizeof(tDATA_INFO));
		strcpy(gau8MainCode, pMainCode);
		strcpy(gptMultiCode->mainCode, pMainCode);
		gptMultiCode->sendType = sendType;//发送方式
		gptMultiCode->ifOver = 1;
		gu32MultiInNand++;//新增一票
	}
	
	hyUsbPrintf("add multi total = %d \r\n", gptMultiCode->total);
	
	return 0;
}

/*----------------------------------------------------------
* 函数: Queue_ClearMulti
* 功能: 队列清空
* 参数: none
* 返回: none 
------------------------------------------------------------*/
void Queue_ClearMulti()
{
	gptMultiCode->ifOver= 0;
	gptMultiCode->sendType = 0;
	gptMultiCode->total = 0;
	memset(gau8MainCode, 0, 32);
}


/*----------------------------------------------------------
* 函数: Queue_GetMultiCodeCnt
* 功能: 获取数据个数
* 参数: none
* 返回: 总数 
------------------------------------------------------------*/
U16 Queue_GetMultiCodeCnt()
{
	return gptMultiCode->total;
}

/*--------------------------------------
*函数：Rec_GetMultiSendType
*功能：获取本次一票多件的发送方式
*参数：none
*返回：
*--------------------------------------*/
U8 Rec_GetMultiSendType()
{
	return gptMultiCode->sendType;
}


/*----------------------------------------------------------
* 函数: Queue_GetMultiBuf
* 功能: 获取指针
* 参数: none
* 返回:  
------------------------------------------------------------*/
tMULTI_CODE *Queue_GetMultiBuf()
{
	return gptMultiCode;
}

/*----------------------------------------------------------
* 函数: Queue_Clear
* 功能: 队列清空
* 参数: none
* 返回: none 
------------------------------------------------------------*/
void Queue_Clear()
{
	gtData_Que.rd 		= 0;
	gtData_Que.wr 		= 0;
	gtData_Que.delCnt	= 0;
}

/*----------------------------------------------------------
* 函数: Queue_DelNodes
* 功能: 删除结点  从rd处开始,  不支持随机位置删除
* 参数: changeFlag:是否同时改变记录在nand上的状态 0:不  1:改
        cnt:个数
* 返回: 0:成功   -1:失败 
------------------------------------------------------------*/
int Queue_DelNodes(U8 changeFlag, int cnt)
{
	int		ret;
	U32		i,fileid = 0xFFFFFFFF, *recIdx;
	U16		opType,ifimage=0;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.rd == gtData_Que.wr)//队列空
	{
	hyUsbPrintf("队列空  \r\n");
		unlock(&gu32QueueLock);
		return -1;
	}
//hyUsbPrintf("1111 del rd = %d  wr = %d \r\n", gtData_Que.rd, gtData_Que.wr);	
	recIdx = (U32 *)hsaSdram_RecSendBuf();
	
	for(i = 0; i < cnt;)
	{
		if(2 == gtData_Que.pQueBuf[gtData_Que.rd].delFlag)//已经删除的记录
		{
			if(gtData_Que.delCnt > 0) gtData_Que.delCnt--;
			gtData_Que.rd++;
			if(gtData_Que.rd >= gtData_Que.size)
			{
				gtData_Que.rd = 0;
			}
			continue;
		}
		
		//还没有定位文件和操作类型
		if(fileid == 0xffffffff)
		{
			fileid = gtData_Que.pQueBuf[gtData_Que.rd].fileId;
			opType = gtData_Que.pQueBuf[gtData_Que.rd].opType;
			ifimage= gtData_Que.pQueBuf[gtData_Que.rd].tData.tComm.ifimage;
		}
		//记录当前记录索引
		recIdx[i] = gtData_Que.pQueBuf[gtData_Que.rd].recIdx;
		
		gtData_Que.rd++ ;
		if(gtData_Que.rd >= gtData_Que.size)
		{
			gtData_Que.rd = 0;
		}
		i++;
	}
	
//hyUsbPrintf("del rd = %d  wr = %d delflag = %d \r\n", gtData_Que.rd, gtData_Que.wr,gtData_Que.pQueBuf[gtData_Que.rd].delFlag);	
	//是否需要同时修改记录在nand上的状态
	if(changeFlag == 1)
	{
		//改变记录的状态
		ret = RecStore_repedRecordMulti(fileid,recIdx,cnt);
	//hyUsbPrintf("ret %d gu32RecUnSendTotal = %d \r\n",ret,gu32RecUnSendTotal);	
		//根据实际改变的个数来调整信息
		
		if(ifimage == 1)//图片
		{
			gu32RecUnSendImage = (gu32RecUnSendImage >= ret)? (gu32RecUnSendImage-ret):0;
		}
		else
		{
			gu32RecUnSendTotal = (gu32RecUnSendTotal >= ret)? (gu32RecUnSendTotal-ret):0;
		}
		
		/*for(i = 0; i < ret; i++)
		{
			if(gu32RecUnSendTotal <= Rec_GetExistMax())
			{
				//Rec_DelExistCnt();
			}
			
			if(gu32RecUnSendTotal > 0) gu32RecUnSendTotal--;
			//hyUsbPrintf("gu32RecUnSendTotal  = %d \r\n",gu32RecUnSendTotal);
		}*/
	}
	
	unlock(&gu32QueueLock);
	
	return 0;
}


/*----------------------------------------------------------
* 函数: Queue_AddNode
* 功能: 向队列中增加一个记录
* 参数: pNode:待增加的记录信息
* 返回: 0:成功   -1:失败 
------------------------------------------------------------*/
int Queue_AddNode(tQUE_NODE *pNode)
{
	U32		left;
	
	if(gu32RecInNandTotal > 0)//之前有记录没有加载进来，后面的都不再往队列加载
	{
//hyUsbPrintf("gu32RecUnSendTotal = %d   gu32RecInNandTotal = %d  \r\n",gu32RecUnSendTotal, gu32RecInNandTotal);	
		gu32RecUnSendTotal++;//无论是否加载到队列中,全局计数都加一
		gu32RecInNandTotal++;
		return -1;
	}
	
	wlock(&gu32QueueLock);
	if(gtData_Que.rd > gtData_Que.wr)
	{
		left = gtData_Que.rd - gtData_Que.wr;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.rd - gtData_Que.wr;
	}
	
	if(left > 1)//保留一个空结点  放置出现和初始状态一样的情况
	{
		memcpy(&gtData_Que.pQueBuf[gtData_Que.wr++], pNode, sizeof(tQUE_NODE));
		
		if(gtData_Que.wr >= gtData_Que.size)
		{
			gtData_Que.wr = 0;
		}
		
		if(pNode->tData.tComm.ifimage == 1)//图片
		{
		hyUsbPrintf("add image ok \r\n");
			gu32RecUnSendImage++;
		}
		else
		{
			gu32RecUnSendTotal++;
		}
		
		unlock(&gu32QueueLock);
		return 0;
	}
	else
	{
	hyUsbPrintf("add que full ===================== \r\n");
		if(pNode->tData.tComm.ifimage == 1)//图片
		{
			gu32RecUnSendImage++;
		}
		else
		{
			gu32RecUnSendTotal++;//无论是否加载到队列中,全局计数都加一
			gu32RecInNandTotal++;
		}
		unlock(&gu32QueueLock);
		return -1;
	}
}

/*----------------------------------------------------------
* 函数: Queue_GetNodeCnt
* 功能: 获取当前队列中记录的总数
* 参数: none
* 返回: 队列中记录条数
------------------------------------------------------------*/
U32 Queue_GetNodeCnt()
{
	U32	left;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.wr >= gtData_Que.rd)
	{
		left = gtData_Que.wr - gtData_Que.rd;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.wr - gtData_Que.rd;
	}
	
	left = (left >= gtData_Que.delCnt)?(left-gtData_Que.delCnt):0;
	
	unlock(&gu32QueueLock);
	
	return left;
}

/*----------------------------------------------------------
* 函数: Queue_DelNode
* 功能: 从队列中删除一个结点
* 参数: fileid:记录所在文件索引
        recIdx:记录索引
* 返回: 0:删除成功  -2:正在发送不能删除  -1: 没找到该node
------------------------------------------------------------*/
int Queue_DelNode(U32 fileid, U32 recIdx)
{
	U32		i,left;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.wr >= gtData_Que.rd)
	{
		left = gtData_Que.wr - gtData_Que.rd;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.wr - gtData_Que.rd;
	}
	
	left = (left >= gtData_Que.delCnt)?(left-gtData_Que.delCnt):0;
//hyUsbPrintf("left = %d  rd = %d  wr = %d \r\n", left, gtData_Que.rd, gtData_Que.wr);	
	if(left == 0)
	{
		unlock(&gu32QueueLock);
		return -1;
	}
	
	i = gtData_Que.rd;
	while(i != gtData_Que.wr)
	{
		if(fileid == gtData_Que.pQueBuf[i].fileId && recIdx == gtData_Que.pQueBuf[i].recIdx)
		{
			break;
		}
		i++;
		if(i >= gtData_Que.size)
		{
			i = 0;
		}
	}
	if(i == gtData_Que.wr)//没有找到
	{
		unlock(&gu32QueueLock);
		return -1;
	}
	else
	{
		gtData_Que.pQueBuf[i].delFlag = 2;//标记为删除
		gtData_Que.delCnt++;
		
		if(gtData_Que.pQueBuf[i].tData.tComm.ifimage == 1)//图片
		{
			if(0 == PicStore_Delete(gtData_Que.pQueBuf[i].tData.tComm.code))
			{
				if(gu32RecUnSendImage > 0) gu32RecUnSendImage--;
			}
		}
		else
		{
			if(gu32RecUnSendTotal > 0) gu32RecUnSendTotal--;
		}
	hyUsbPrintf("gu32RecUnSendTotal  = %d \r\n",gu32RecUnSendTotal);	
		unlock(&gu32QueueLock);
		return 0;
	}
}

/*----------------------------------------------------------
* 函数: Queue_GetNodesDate
* 功能: 获取队列中结点的信息
* 参数: tQUE_NODE:存放获取到的结点信息
        cnt:希望获取的条数
* 返回: 实际获取到的结点数
------------------------------------------------------------*/
U32 Queue_GetNodesDate(tQUE_NODE *pQue, int cnt)
{
	U32 i,j,find;
	U16 opType=0xFFFF;
	U32 fileid = 0xFFFFFFFF;
	
	wlock(&gu32QueueLock);
	
	i = gtData_Que.rd;
	if(i + cnt < gtData_Que.size)
	{
		memcpy(&pQue[0], &gtData_Que.pQueBuf[i], cnt*sizeof(tQUE_NODE));
	}
	else
	{
		j = gtData_Que.size-i;
		memcpy(&pQue[0], &gtData_Que.pQueBuf[i], j*sizeof(tQUE_NODE));
		memcpy(&pQue[j], &gtData_Que.pQueBuf[0], (cnt-j)*sizeof(tQUE_NODE));
	}
	
	find = 0;
	for(i = 0; i < cnt;)
	{
		//先判断是否删除
		if(pQue[i].delFlag == 2)
		{
			if(find == 0)//之前还没有找到有效记录
			{
				if(gtData_Que.delCnt > 0) gtData_Que.delCnt--;
				gtData_Que.rd++;
				if(gtData_Que.rd >= gtData_Que.size)
				{
					gtData_Que.rd = 0;
				}
			}
			cnt--;
			if(i != cnt)
			{
				memcpy(&pQue[i], &pQue[i+1], (cnt-i)*sizeof(tQUE_NODE));
			}
			continue;
		}
		else if(opType == 0xFFFF)
		{
			find = 1;
			fileid	= pQue[i].fileId;
			opType	= pQue[i].opType;
			if(pQue[i].tData.tComm.nopacket == 1)//如果当前记录是不允许打包的,则直接跳出
			{
				break;
			}
		}
		else if(opType != pQue[i].opType || fileid != pQue[i].fileId || pQue[i].tData.tComm.nopacket == 1)//类型不一致或不在同一个文件时后面的调过
		{
		hyUsbPrintf("opType = (%d  %d) fileid  = (%d  %d)\r\n",opType, pQue[i].opType, fileid, pQue[i].fileId);
			break;
		}
		else
		{
			find++;
		}
		i++;
	}
	
	unlock(&gu32QueueLock);
	
	return find;
}

/*----------------------------------------------------------
* 函数: Queue_Add2Que
* 功能: 向队列中增加一条记录
* 参数: 
* 返回: 0:成功   -1:失败
------------------------------------------------------------*/
int Queue_Add2Que(U32 entry, U32 idx, U16 type, tDATA_INFO *pdata)
{
	tQUE_NODE tQue;
	
	tQue.fileId = entry;
	tQue.recIdx = idx;
	tQue.delFlag= 0;
	tQue.opType = type;
	
	memcpy(&tQue.tData, pdata,sizeof(tDATA_INFO));
	
	return Queue_AddNode(&tQue);
}