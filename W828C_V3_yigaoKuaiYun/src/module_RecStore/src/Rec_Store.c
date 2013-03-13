#include "Rec_Store.h"
#include "Rec_Info.h"
#include "Rec_Queue.h"


/*=======================================================================*/
/*	全局变量 */
U32	appLockNandCtr = 0;

U8	*gpu8RecBuf;
static U8  gu8CurId[30];//当前登录员工的ID
static U8  gu8CurRight; //当前登录员工的权限



/*-----------------------------------------------------------
*函数:	RecStore_SetId
*功能:	把当前登录的id设置进来
*参数： 
*返回:	none
*------------------------------------------------------------*/
void RecStore_SetId(U8 *pId)
{
	memset(gu8CurId,0,30);
	strcpy(gu8CurId,pId);
}

/*-----------------------------------------------------------
*函数:	RecStore_GetId
*功能:	获取当前登录的id
*参数： 
*返回:	ID
*------------------------------------------------------------*/
U8 *RecStore_GetId()
{
	return gu8CurId;
}

/*-----------------------------------------------------------
*函数:	RecStore_SetRight
*功能:	把当前登录的权限设置进来
*参数： 
*返回:	none
*------------------------------------------------------------*/
void RecStore_SetRight(U8 right)
{
	gu8CurRight = right;
}

/*-----------------------------------------------------------
*函数:	RecStore_GetRight
*功能:	获取当前登录的权限
*参数： 
*返回:	right
*------------------------------------------------------------*/
U8 RecStore_GetRight()
{
	return gu8CurRight;
}

/*-----------------------------------------------------------
*函数:	RecStore_Init
*功能:	模块初始化.
*参数：.pRecBuf:外部提供buf  
        bufSize:buf长度  (>=140K)
*返回:	成功：HY_OK   失败：HY_ERROR
*------------------------------------------------------------*/
int RecStore_Init(U8 *pRecBuf, U32 bufSize)
{
	int ret;
	
	if(bufSize < 140*1024)//外部提供的buf不够
	{
		return -1;
	}
	wlock((unsigned int)&appLockNandCtr);
	gpu8RecBuf = pRecBuf;
	ret = nandFile_Init(0);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*函数：RecStore_getFileHandle
*功能：通过文件名找到文件，返回file idx   
       注意:可能存在同名文件, 如果一天的记录太多,一个文件存放不完,就要使用2个文件,这2个文件名相同.
*参数：	name -- 文件名
*		pagebuf	-- 临时buf,一个page 大小
*返回：成功：返回找到的文件个数   失败：<=0
*------------------------------------------------------------*/
int RecStore_getFileHandle(U8 *name, int *pFileIdx)
{
	return nandFile_open(name,pFileIdx);
}

/*-----------------------------------------------------------
*函数:	RecStore_GetExistFileList
*功能:	得到目前已有的文件个数和名字列表.
*参数： 
*		ppArr: 存储名字列表的二维数组
*返回:	文件个数.
*------------------------------------------------------------*/
int RecStore_GetFileNameList(int *pNodeArr)
{
	int i, j, k, fileNum,fileIdx;
	tFILE_ENTRY	*ptFile,*ptFile2;
	
	for(i = 0, j = 0; i<MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name != -1) 
		{
			pNodeArr[j] = i;
			j++;
		}
	}
	
	fileNum = j;
	//对这些文件按日期排序  日期最小的在最前面
	for(i = 0; i < fileNum-1; i++)
	{
		k = 0;
		for(j = 0; j < fileNum-i-1; j++)
		{
			ptFile  = &gtFileEntry[pNodeArr[j]];
			ptFile2 = &gtFileEntry[pNodeArr[j+1]];
			if(ptFile->name > ptFile2->name)
			{
				fileIdx      = pNodeArr[j];
				pNodeArr[j]  = pNodeArr[j+1];
				pNodeArr[j+1]=fileIdx;
				k = 1;
			}
			else if(ptFile->name > ptFile2->name)//日期相同的文件比较索引
			{
				if(ptFile->dayIdx > ptFile2->dayIdx)
				{
					fileIdx      = pNodeArr[j];
					pNodeArr[j]  = pNodeArr[j+1];
					pNodeArr[j+1]=fileIdx;
					k = 1;
				}
			}
		}
		if(k == 0)
		{
			break;
		}
	}
	
	return fileNum;
}

/*-----------------------------------------------------------
*函数:	RecStore_deleteFile
*功能:	删除指定的文件
*参数： 
*返回:	文件个数.
*------------------------------------------------------------*/
int RecStore_deleteFile(U8 *name)
{
	int ret;
	if (name == NULL)
		return HY_ERROR;
	
	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_delete(atoi(name));
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}


/*-----------------------------------------------------------
*函数:	nandFile_changeRecordStatus
*功能:	在指定文件中写入一条记录.
*	  	如果该文件不存在,会自动创建文件.如果存在,直接写入.
*参数：
*	    pDate_name:文件名. 6字字符
*		pNode:要写的数据
*		recId:返回的ID.记录写入的位置.
*返回:	成功：文件idx   失败：HY_ERROR
*------------------------------------------------------------*/
int RecStore_WriteRecord(U8 *pDate_name, tDATA_INFO *pNode, U32 *recId)
{
	tFILE_ENTRY	*pFile;
	int	fileIdx[MAX_NAND_FILE], useIdx;
	int i,cnt,ret;
	
	if(pDate_name == NULL || pNode == NULL)
	{
	hyUsbPrintf("pDate_name or pNode is NULL !\r\n");
		return HY_ERROR;
	}
	
	wlock((unsigned int)&appLockNandCtr);
	
	useIdx = -1;
	// 直接打开. 如果该文件不存在,返回-1
	cnt = nandFile_open(pDate_name, fileIdx);
	if(cnt > 0)
	{
		//从找到的这些文件中找出一个没有存满的文件
		for(i = 0; i < cnt; i++)
		{
			pFile = &gtFileEntry[fileIdx[i]];
			hyUsbPrintf("file id = %d  total = %d   name = %d  idx = %d \r\n", 
						fileIdx[i],pFile->totalRecords,pFile->name, pFile->dayIdx);
			if(pFile->totalRecords < MAX_RECORDS_PER_FILE)
			{
				useIdx = fileIdx[i];
				break;
			}
		}
	}
	
	if(useIdx == -1)//没有找到 或 之前那个文件已经存满
	{
		useIdx = nandFile_create(pDate_name, cnt);
	}
	hyUsbPrintf("cur useidx = %d \r\n", useIdx);
	if(-1 != useIdx)
	{
		ret = nandFile_write(useIdx, (U8 *)pNode, sizeof(tDATA_INFO), pNode->tComm.state, recId);
		hyUsbPrintf("nandFile_write ret = %d \r\n", ret);
		if(ret == -1)
		{
			useIdx = -1;
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	return useIdx;
}

/*-----------------------------------------------------------
*函数:	RecStore_CmpId
*功能:	对比记录中的ID号和当前登录员工的ID号
*参数： pNode:数据
*返回:	成功：HY_OK   失败：HY_ERROR
*------------------------------------------------------------*/
int RecStore_CmpId(tDATA_INFO *pNode)
{
	int ret = HY_ERROR;
	
	if(strcmp(pNode->tComm.userId,gu8CurId) == 0)
	{
		ret = HY_OK;
	}
	
	return ret;
}

int default_cmp(tRECORD_STATUS *par, void *param)
{
	return 0;
}


/*-----------------------------------------------------------
*函数:	RecStoresub_SearchNextRecord
*功能:	在指定文件中按给定类型从指定位置向后查找一条记录.
*参数：
*	    fileIdx:文件在gtFileEntry中的idx
*		datacmpFunc: 回调函数,对比数据是否相等
*		typecmpFunc: 回调函数,对比状态是否相同
*		datacmpPara: datacmpFunc要传进去的参数
*		cmpfuncPara: typecmpFunc要传进去的参数
*		recId:返回的ID.记录找到记录的位置.
*返回:	成功：HY_OK   失败：HY_ERROR
*注意:	外层第一次调用时,如果是要从头查找,*recId参数必须为0.
*       如果没有状态对比函数,则直接进行内容相比; 如果有状态对比函数,则先对比状态,再对比内容.
*		模块子函数,非对外接口.
*------------------------------------------------------------*/
int RecStoresub_SearchNextRecord(int fileIdx, DATA_CMP datacmpFunc, TYPE_CMP typecmpFunc, void *datacmpPara, void *cmpfuncPara, U32 *recId)
{
	int	i, j, ret, rc;
	int pageOffset, tmpIdx;
	int absNum,blk,isEnd = 0;
	tRECORD_STATUS *pagebuf;
	tDATA_INFO node;
	tFILE_ENTRY	*ptFile;
	
	rc = HY_ERROR;
	if (fileIdx >= MAX_NAND_FILE || datacmpFunc == NULL)
		return HY_ERROR;
	if (typecmpFunc == NULL)
		typecmpFunc = default_cmp;
	
	ptFile	= &gtFileEntry[fileIdx];
	if (*recId >= ptFile->totalRecords)	//查找索引已经超出该文件最大存储条数, 返回.
	{
		return HY_ERROR;
	}
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	//只能删除未上传的记录
	if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
	{//没有未上传的记录
		return HY_ERROR;
	}
	
	absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
	if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
	{
		absNum++;
	}
	
	blk			= (*recId) / ABSTRACT_NUM_PER_PAGE;
	pageOffset  = (*recId) % ABSTRACT_NUM_PER_PAGE;
	
	absNum -= blk;
	
	//读取有效的摘要信息
	absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, blk, absNum);
	
	if(absNum == HY_ERROR)//读取失败
	{
		return HY_ERROR;
	}
	
	for(i = 0; i < absNum; i++)
	{
		for(j = 0; j < ABSTRACT_NUM_PER_PAGE; j++)//每组摘要的最大数量
		{
			tmpIdx = i*ABSTRACT_NUM_PER_PAGE+j;
			if(tmpIdx+blk*ABSTRACT_NUM_PER_PAGE >= ptFile->totalRecords)//查找索引已经超出该文件最大存储条数, 返回.
			{
				isEnd = 1;
				break;
			}
			
			if (typecmpFunc(&pagebuf[tmpIdx], cmpfuncPara) != 0)
			{
				continue;
			}
			
			rc = nandFile_read(fileIdx, tmpIdx, (U8 *)&node, sizeof(tDATA_INFO));
			if (rc == HY_ERROR)
			{
				continue;
			}
			
			rc = HY_ERROR;
			if (datacmpFunc(&node, datacmpPara) == 0)	//真正匹配
			{
				*recId = tmpIdx;
				if(gu8CurRight != 0)
				{
					rc = RecStore_CmpId(&node);
					if(rc == HY_OK)
					{
						isEnd = 1;
						break;
					}
				}
				else
				{
					isEnd = 1;
					rc = HY_OK;
					break;
				}
			}
		}
		
		if(isEnd)
		{
			break;
		}
	}
		
	return rc;
}


// 配对给RecStore_DeleteRecord的类型修改函数  改状态为 "已删"
int CB_modifyState_DeleteRecord(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 2;

	return pStatus->recState;
}
/*-----------------------------------------------------------
*函数：RecStore_DeleteRecord
*功能：删除 某一指定条.(指定文件和文件内索引)
*参数：fileIdx:文件索引   
	   recordIdx:文件内记录索引
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_DeleteRecord(int fileIdx, int recordIdx)
{
	int ret;
	
	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_changeRecordStatus(fileIdx, recordIdx, CB_modifyState_DeleteRecord);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

// 配对给RecStore_DeleteRecord的类型修改函数  改状态为 "已传"
int CB_modifyState_repedRecord(tRECORD_STATUS *pStatus)
{
	int ret = 1;
	
	if(pStatus->recState == 0)
	{
		pStatus->recState = 1;
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}


/*-----------------------------------------------------------
*函数：RecStore_repedRecord
*功能：修改某一指定条为上传状态.(指定文件和文件内索引)
*参数：fileIdx:文件索引   
	   recordIdx:文件内记录索引
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_repedRecord(int fileIdx, int recordIdx)
{
	int ret;

	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_changeRecordStatus(fileIdx, recordIdx, CB_modifyState_repedRecord);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*函数：RecStore_repedRecordMulti
*功能：修改多条记录状态为上传状态.(指定文件和文件内索引)
*参数：fileIdx:文件索引   
	   recordIdx:文件内记录索引
*返回：失败：=0    成功：>0  (成功修改记录的个数)
*------------------------------------------------------------*/
int RecStore_repedRecordMulti(int fileIdx, int *recordIdx, int num)
{
	int ret,i,j,total;
	int reccnt,blk;
	
	total  = 0;
	reccnt = 0;
	blk    = -1;
	
	for(i = 0, j = 0; i < num; i++)
	{
		if(blk == -1)
		{
			blk = (recordIdx[i]/ABSTRACT_NUM_PER_PAGE);
		}
		else if(blk != recordIdx[i]/ABSTRACT_NUM_PER_PAGE)
		{
			blk = (recordIdx[i]/ABSTRACT_NUM_PER_PAGE);
			
			j += reccnt;
			
			wlock((unsigned int)&appLockNandCtr);
			ret = nandFile_changeRecordStatusMulti(fileIdx, &recordIdx[j-reccnt], &reccnt, CB_modifyState_repedRecord);
			unlock((unsigned int)&appLockNandCtr);
			
			if(ret == HY_OK)
			{
				total += reccnt;
			}
			
			reccnt = 0;
		}
		
		reccnt++;
		
		if(i+1 == num)
		{
			j += reccnt;
			
			wlock((unsigned int)&appLockNandCtr);
			ret = nandFile_changeRecordStatusMulti(fileIdx, &recordIdx[j-reccnt], &reccnt, CB_modifyState_repedRecord);
			unlock((unsigned int)&appLockNandCtr);
			
			if(ret == HY_OK)
			{
				total += reccnt;
			}
			reccnt = 0;
		}
	}
	
	return total;
}


/*-----------------------------------------------------------
*函数：RecStore_GetAllUnreported
*功能：加载所有未上传的记录
*参数:  usertype : 权限
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_GetAllUnreported(U8 usertype)
{
	int i,j,k, fileNum, ret;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS *pagebuf,recStat;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO node;
	tQUE_NODE  tQue;
	int readCnt = 0,absNum,fileIdx;
	int quefull=0;
	U32 total,nand,image;
	
	wlock((unsigned int)&appLockNandCtr);
	
	total = gu32RecUnSendTotal;
	image = gu32RecUnSendImage;
	nand  = gu32RecInNandTotal;
hyUsbPrintf("当前未发送数:%d,  磁盘中未发送数:%d  图片数: %d \r\n", total, nand,gu32RecUnSendImage);	
	gu32RecUnSendTotal = 0;
	gu32RecUnSendImage = 0;
	gu32RecInNandTotal = 0;
	gu32MultiInNand    = 0;
		
	fileNum = RecStore_GetFileNameList(fileList);
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
//hyUsbPrintf("记录文件总数:%d \r\n", fileNum);
	for (i = 0; i < fileNum && quefull == 0; i++)//文件循环 start
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		hyUsbPrintf("文件序号:%d, 记录总数:%d,  删除数:%d,  已传数:%d\r\n",
				fileIdx, ptFile->totalRecords, ptFile->deletedRecords, ptFile->reportedRecords);
		/* kong 2011-05-24 先判断该文件中是否有未上传的记录*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//没有未上传的记录
			continue;
		}
		
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		//hyUsbPrintf("摘要总数:%d\r\n", absNum);
		if(absNum == HY_ERROR)//读取失败
		{
		hyUsbPrintf("absNum  = %d \r\n");
			continue;
		}

		for(j = 0; j < ptFile->totalRecords && quefull == 0; j++)//循环处理实际读取到的摘要组数
		{
			recStat = pagebuf[j];
		//hyUsbPrintf("当前记录索引:%d, 状态:%d\r\n", j, recStat.recState);	
			if(recStat.recState == 0)//0:未上传
			{
				//读出该记录的详细信息加载到上传队列中
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("读取当前记录结果(0:成功):%d\r\n", ret);
				if(ret == HY_OK)
				{
					if(node.tComm.ifimage == 1 && gu32RecUnSendTotal > 0)
					{
						gu32RecUnSendImage++;
					}
					else
					{
						tQue.fileId = fileIdx;
						tQue.recIdx = j;
						tQue.delFlag= 0;
						tQue.opType = node.tComm.opType;
						tQue.tData  = node;
						if(-1 == Queue_AddNode(&tQue))
						{
						hyUsbPrintf("加入队列失败!! \r\n");
							quefull = 1;
							break;
						}
							hyUsbPrintf("add to queue success nand = %d  gu32RecUnSendImage = %d \r\n",nand, gu32RecUnSendImage);
							if(nand > 0 && node.tComm.ifimage == 0) nand--;

						readCnt++;
						if(nand > 0 && readCnt >= 1000)//为了让出时间给前台写记录  每次只加载一部分
						{
							//syssleep(1);
							//readCnt = 0;
							quefull = 1;
							break;
						}
					}
				}
			}
			else if(recStat.recState == 4)//可传   这种状态的记录加载时需要做关联
			{
				//读取本条记录信息
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				
				if(ret == HY_OK)
				{
					//函数里面做判断
					Queue_AddNode_Multi(node.tComm.code, node.tComm.subCode, &node, 0);
				}
			}
		}
		
	}//文件循环 end
hyUsbPrintf("加载后磁盘剩余记录:%d, 加载后未发送记录数:%d\r\n", nand, total);
//hyUsbPrintf("\r\nGetAll nand = %d gu32RecInNandTotal = %d total = %d gu32RecUnSendTotal = %d\r\n",nand,gu32RecInNandTotal,total,gu32RecUnSendTotal);
	if(nand > gu32RecInNandTotal && gu32RecInNandTotal > 0) gu32RecInNandTotal = nand;
	if(total > gu32RecUnSendTotal)   gu32RecUnSendTotal = total;
	if(image > gu32RecUnSendImage)   gu32RecUnSendImage = image;

	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}


/*-----------------------------------------------------------
*函数：RecStore_GetUnreportedMulti
*功能：加载可传状态的一票多件记录
*参数: usertype : 权限
       sendType:  发送方式  0:后台发送    1:前台发送
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_GetUnreportedMulti(U8 usertype, U8 sendType)
{
	int i,j,k, fileNum, ret;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS *pagebuf,recStat;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO node;
	tQUE_NODE  tQue;
	int readCnt = 0,absNum,fileIdx;
	int multiFlag = 0;
	
	wlock((unsigned int)&appLockNandCtr);
	
	gu32MultiInNand    = 0;
	fileNum = RecStore_GetFileNameList(fileList);
	
	k = 0;
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
//hyUsbPrintf("fileNum  = %d \r\n", fileNum);
	for (i = 0; i < fileNum; i++)//文件循环 start
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		/* kong 2011-05-24 先判断该文件中是否有未上传的记录*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//没有未上传的记录
			continue;
		}
		
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			continue;
		}
	//hyUsbPrintf("fileIdx  = %d   total  = %d\r\n", i, ptFile->totalRecords);	
		for(j = 0; j < ptFile->totalRecords; j++)//循环处理实际读取到的摘要组数
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[j];
		hyUsbPrintf("load rec state = %d \r\n",recStat.recState);	
			if(recStat.recState == 4)//可传   这种状态的记录加载时需要做关联
			{
				//读取本条记录信息
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				
				if(ret == HY_OK)
				{
					//函数里面做判断
					if(-1 == Queue_AddNode_Multi(node.tComm.code, node.tComm.subCode, &node, sendType))
					{
						k = 1;//可以结束
					}
				}
			}
		}
		
		if(k == 1)
		{
			break;
		}
		
	}//文件循环 end
	
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*函数：RecStore_GetRecordStateNum
*功能：得到指定用户类型 日期 类型的记录数据的条数  (总数  已上传  已删除)
*参数：
	   usertype:用户类型
	   pDate_name:指定的日期   如果pEntry不为NULL 
	   type:指定类型
	   total: 总数
	   rep ：已上传
	   unrep：未上传
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
//int RecStore_GetRecordStateNum(U8 *pDate_name, U8 usertype, U8 type, U32 *total, U32 *rep, U32 *unrep)
int RecStore_GetRecordStateNum(U8 *pDate_name, U8 usertype, U8 type, tREC_INFO *pRecInfo)
{
	int ret, cnt,fileIdx[MAX_NAND_FILE], i, j;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat;
	tDATA_INFO tNode;
	int     rc,absNum,typeCnt=0;
	
hyUsbPrintf("date = %s usertype = %d type = %d\r\n",pDate_name,usertype,type);	
	if (pDate_name == NULL)
		return HY_ERROR;
	
	wlock((unsigned int)&appLockNandCtr);
	
	//*total = *rep = *unrep = 0;	//清0
	pRecInfo->total = 0;
	pRecInfo->reported = 0;
	pRecInfo->unreported = 0;
	cnt = nandFile_open(pDate_name, fileIdx);
	if(cnt <= 0)//没有找到
	{
		hyUsbPrintf("no file\r\n");
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
		
		pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	hyUsbPrintf("file idx = %d  total = %d  rep = %d \r\n",fileIdx[j], ptFile->totalRecords, ptFile->reportedRecords);
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		hyUsbPrintf("absNum = %d  totalRecords = %d\r\n",absNum,ptFile->totalRecords);
		for(i = 0; i < ptFile->totalRecords; i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
			
			if(pRecInfo->cnt < pRecInfo->max)
			{
				//记录下记录的类型
				for(typeCnt = 0; typeCnt < pRecInfo->cnt; typeCnt++)
				{
					if(pRecInfo->recType[typeCnt] == recStat.type)
					{
						break;
					}
				}
				if(typeCnt >= pRecInfo->cnt)
				{
					pRecInfo->recType[pRecInfo->cnt++] = recStat.type;
				}
			}
			
			//hyUsbPrintf("type = %d  userType = %d  recState = %d \r\n",recStat.type,recStat.userType,recStat.recState);
			/* 找到指定权限、扫描类型、状态的记录 */
			if( (type == recStat.type) && (usertype == 0 || recStat.userType == 0 || usertype == recStat.userType)) /* 高6位为权限  */
			{
				if(usertype != 0 && recStat.userType != 0)//不是管理员 需要匹配登录ID号
				{
					//hyUsbPrintf("fileidx = %d \r\n",fileIdx[j]);
					rc = nandFile_read(fileIdx[j], i, (U8 *)&tNode, sizeof(tDATA_INFO));
					//hyUsbPrintf("read rec = %d  %d\r\n",rc,i);
					if(rc == HY_OK)
					{
						rc = RecStore_CmpId(&tNode);
						//hyUsbPrintf("CmpId = %d\r\n",rc);
						if(rc == HY_OK)
						{
							//(*total)++;
							pRecInfo->total++;
							if(0 == recStat.recState || 3 == recStat.recState || 4 == recStat.recState)/* 未上传 */
							{
								//(*unrep)++;
								pRecInfo->unreported++;
							}
							else if(1 == recStat.recState)/* 已上传 */
							{
								//(*rep)++;
								pRecInfo->reported++;
							}
						}
					}
				}
				else//管理员
				{
					//(*total)++;
					pRecInfo->total++;	
					if(0 == recStat.recState || 3 == recStat.recState || 4 == recStat.recState)/* 未上传 */
					{
					//hyUsbPrintf("未发送===\r\n");
						//(*unrep)++;
						pRecInfo->unreported++;
					}
					else if(1 == recStat.recState)/* 已上传 */
					{
					//hyUsbPrintf("已发送===\r\n");
						//(*rep)++;
						pRecInfo->reported++;
					}
				}
			}
		}
	}
	unlock((unsigned int)&appLockNandCtr);
hyUsbPrintf("total = %d  unrep = %d rep = %d \r\n",pRecInfo->total,pRecInfo->unreported,pRecInfo->reported);	
	return HY_OK;
}

// 配对给nandFile_GetRecordInfo的匹配函数
int CB_typeCheck_RecordInfo(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* 找到指定权限、扫描类型、状态的记录 */
hyUsbPrintf("type = %d  %d    user type = %d   %d \r\n", state->type, pState->type, state->userType, pState->userType);
	if( state->type == pState->type  
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0)) /* 高6位为权限  */
	{
		return pState->recState;
	}
	else
	{
		return -1;
	}
}

/*-----------------------------------------------------------
*函数：RecStore_GetRecordInfo
*功能：得到指定用户类型 日期 类型的记录数据的详细信息   不包括已删除的记录
*参数：pEntry:如果知道文件的entry则传进来  不知道可以传NULL
	   usertype:用户类型
	   date:指定的日期   如果pEntry不为NULL  date可以传NULL
	   type:指定类型
	   pBuf：返回的buf  tQUE_NODE结构的
	   buflen：buf长度
*返回：失败：-1    成功：返回记录个数
*------------------------------------------------------------*/
int RecStore_GetRecordInfo(U8 *pDateName, U16 usertype, U8 type, U8 *pBuf, int buflen)
{
	int cnt,fileIdx[MAX_NAND_FILE], ret, total, len;
	int state,absNum;
	tDATA_INFO node;
	tQUE_NODE *pQue;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat,fidRec;
	int  readsize,i,j;
	
	wlock((unsigned int)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	fidRec.type = type;
	fidRec.userType = usertype;
	pQue = (tQUE_NODE *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
				
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
					
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("recInfo read = %d  %d\r\n",ret,i);
				if(ret == HY_OK)
				{
					if(usertype != 0)//不是管理员
					{
						//需要解析出记录中的员工工号和当前登录员工做对比
						ret = RecStore_CmpId(&node);
					}
				}
				
				if(ret == HY_OK)
				{
					if(strlen(node.tComm.subCode) == 0 || (strcmp(node.tComm.code, node.tComm.subCode) == 0))
					{
						node.tComm.state = state;
						pQue[total].fileId = fileIdx[j];
						pQue[total].recIdx = i;
						pQue[total].delFlag= node.tComm.state;
						memcpy(&(pQue[total].tData), &node, sizeof(tDATA_INFO));
						total++;
						len += sizeof(tQUE_NODE);
					}
				}
			}
		}
	}

	unlock((unsigned int)&appLockNandCtr);

	return total;
}

/*-----------------------------------------------------------
*函数：RecStore_GetRecordCode
*功能：得到指定用户类型 日期 类型的记录数据的条码信息   包括已删除的记录
*参数：pEntry:如果知道文件的entry则传进来  不知道可以传NULL
	   usertype:用户类型
	   date:指定的日期   如果pEntry不为NULL  date可以传NULL
	   type:指定类型
	   pBuf：返回的buf
	   buflen：buf长度
*返回：失败：-1    成功：返回记录个数
*------------------------------------------------------------*/
int RecStore_GetRecordCode(U8 *pDateName, U16 usertype, U8 type, U8 *pBuf, int buflen)
{
	int cnt,fileIdx[MAX_NAND_FILE], ret, total, len;
	int state,absNum;
	tDATA_INFO node;
	tREC_SEARCH	*pSearch;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat,fidRec;
	int  readsize,i,j;
	
	wlock((unsigned int)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
hyUsbPrintf("get file cnt = %d \r\n", cnt);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	fidRec.type = type;
	fidRec.userType = usertype;
	pSearch = (tREC_SEARCH *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
	hyUsbPrintf("file id = %d  total = %d \r\n", fileIdx[j], ptFile->totalRecords);			
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
					
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("recInfo read = %d  %d\r\n",ret,i);
				if(ret == HY_OK)
				{
					if(usertype != 0)//不是管理员
					{
						//需要解析出记录中的员工工号和当前登录员工做对比
						ret = RecStore_CmpId(&node);
					}
				}
				
				if(ret == HY_OK)//图片记录不加载
				{
					//如果是一票多件,则只加载主单,
					if(strlen(node.tComm.subCode) == 0 || (strcmp(node.tComm.code, node.tComm.subCode) == 0))
					{
						memset(&pSearch[total], 0, sizeof(tREC_SEARCH));
						pSearch[total].fileId = fileIdx[j];
						pSearch[total].recIdx = i;
						pSearch[total].recState = state;
						pSearch[total].ifimage = node.tComm.ifimage;
						strcpy(pSearch[total].barCode, node.tComm.code);
						
						total++;
						len += sizeof(tREC_SEARCH);
					}
				}
			}
		}
	}

	unlock((unsigned int)&appLockNandCtr);

	return total;
}

/*-----------------------------------------------------------
*函数：RecStore_delLog
*功能：删除指定日期以外的文件
*参数: dates ---- 指定的日期
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_delLog(void *names)
{
	int i, fileNum;
	int fileList[MAX_NAND_FILE];
	int filename;
	
	filename = atoi(names);
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);
	for (i = 0; i < fileNum; i++)
	{
		if (filename != gtFileEntry[fileList[i]].name)
		{
			nandFile_delete(gtFileEntry[fileList[i]].name);
		}
	}
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*函数：RecStore_Clear
*功能：清除所有记录信息
*参数: none
*返回：NULL
*------------------------------------------------------------*/
void RecStore_Clear()
{
	char *pagebuf;
	
	wlock((unsigned int)&appLockNandCtr);
	pagebuf = (char *)gpu8RecBuf;
	nandFile_format(0, pagebuf);
	unlock((unsigned int)&appLockNandCtr);
	
	return;
}

// 配对给nandFile_DelErrorScan的类型匹配函数
int CB_typeCheck_DelErrorScan(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* 找到指定权限、未上传的记录 */
	if( state->type == pState->type      /* 扫描类型 */
		&& 2 != pState->recState  /* 2:已删除 */
		&& 1 != pState->recState  /* 1 已上传*/
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* 高6位为权限  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

// 配对给nandFile_DelErrorScan的数据匹配函数
int CB_dataCheck_DelErrorScan(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}


/*-----------------------------------------------------------
*函数：RecStore_DelErrorScan
*功能：删除 某一记录 (根据权限,扫描类型,扫描码)
*参数：usertype:用户类型	   
	   type:指定类型
	   code:扫描码  (可能是一票一件, 可能是一票多件的主单号或子单号)
*返回：成功删除的个数
*------------------------------------------------------------*/
int RecStore_DelErrorScan(U16 usertype, U8 type, U8 *pcode)
{
	int i, j, fileNum,recNum,ret;
	int recId,delCnt=0,isQuit=0;
	int fileList[MAX_NAND_FILE];
	int readCnt = 0,absNum,fileIdx;
	tRECORD_STATUS *pagebuf,state;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO  node;
	
	if(NULL == pcode) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	recId = 0;
	
	wlock((unsigned int)&appLockNandCtr);
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	fileNum = RecStore_GetFileNameList(fileList);
	for (i = 0; i < fileNum && isQuit == 0; i++)
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		/* kong 2011-05-24 先判断该文件中是否有未上传的记录*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//没有未上传的记录
			continue;
		}
		
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			continue;
		}
		
		//循环检测所有读出来的摘要
		for(j = 0; j < absNum && isQuit == 0; j++)
		{
			for(recNum = 0; recNum < ABSTRACT_NUM_PER_PAGE && isQuit == 0; recNum++)//循环处理实际读取到的摘要组数
			{
				recId = j*ABSTRACT_NUM_PER_PAGE+recNum;
				if(recId >= ptFile->totalRecords)
				{
					break;
				}
				
				state = pagebuf[recId];
				
				if(state.recState == 1 || state.recState == 2)//已传或已删
				{
					continue;
				}
				
				if(state.type == type && (state.userType == usertype || usertype == 0))//类型相同  且 权限相同或是管理员
				{
					//比较条码  --需要读出记录
					if(HY_ERROR == nandFile_read(fileList[i], recId, (U8 *)&node, sizeof(tDATA_INFO)))
					{
						continue;
					}
					//条码相同
					if(strcmp(pcode, node.tComm.code) == 0)
					{
						if(strlen(node.tComm.subCode) == 0)//没有子单号,说明是一票一件
						{
							ret = Queue_DelNode(fileIdx, recId);
							hyUsbPrintf("Queue_DelNode  ret = %d \r\n", ret);
							if (ret == HY_OK || ret == -1 || 0 == usertype) /* 管理员可能不会加载未上传的到队列 */
							{
								pagebuf[recId].recState = 2;//改成已删
								ptFile->deletedRecords++;
								delCnt++;
							}
							isQuit = 1;
						}
						else//一票多件
						{
							if(Net_IfSendMulti() && strcmp(Queue_GetMainCode(), pcode) == 0)//已经加载到上传队列中
							{
								isQuit = 1;
							}
							else
							{
								pagebuf[recId].recState = 2;//改成已删
								ptFile->deletedRecords++;
								delCnt++;
								if(strcmp(Queue_GetMainCode(), pcode) == 0)
								{
									//清空本票信息
									Queue_ClearMulti();
								}
								//票数减1
								Rec_DelMultiCnt();
							}
						}
					}
					else if(strcmp(pcode, node.tComm.subCode) == 0)
					{
						//有子单号,是一票多件
						if(Net_IfSendMulti() && strcmp(Queue_GetMainCode(), node.tComm.code) != 0)//未加载到上传队列中
						{
							pagebuf[recId].recState = 2;//改成已删
							ptFile->deletedRecords++;
							delCnt++;
							if(strcmp(Queue_GetMainCode(), pcode) == 0)
							{
								//清空本票信息
								Queue_ClearMulti();
							}
							//票数减1
							Rec_DelMultiCnt();
						}
						isQuit = 1;
					}
				}
			}
			
			//该摘要block处理完,回写
			recId = j*ABSTRACT_NUM_PER_PAGE;
			//hyUsbPrintf("write back abs  fileid = %d   recid = %d \r\n", fileList[i], recIdx);
			nandFile_WriteBackAbs(fileList[i], recId, (char *)pagebuf+j*NAND_PAGE_SIZE);
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	return delCnt;
}


// 配对给RecStore_ChangeState的类型匹配函数
int CB_typeCheck_ChangeState(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* 找到指定权限、未上传的记录 */
	if( state->type == pState->type      /* 扫描类型 */
		&& 2 != pState->recState  /* 2:已删除 */
		&& 1 != pState->recState  /* 1 已上传*/
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* 高6位为权限  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

// 配对给RecStore_ChangeState的数据匹配函数
int CB_dataCheck_ChangeState(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}

// 配对给RecStore_ChangeState的类型修改函数  改状态为 "待传"
int CB_modifyState_ChangeState_Disable(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 3;

	return pStatus->recState;
}

// 配对给RecStore_ChangeState的类型修改函数  改状态为 "可传"
int CB_modifyState_ChangeState_Enable(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 4;

	return pStatus->recState;
}

/*-----------------------------------------------------------
*函数：RecStore_ChangeState
*功能：改变指定用户类型  业务类型  条码    的状态
*参数：usertype:用户类型	   
	   type:指定类型
	   code:扫描码
	   changeState:想要改变成的状态
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：0
*------------------------------------------------------------*/
int RecStore_ChangeState(U16 usertype, U8 type, U8 *pcode, U8 changeState)
{
	int i, fileNum, ret;
	int recId;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS state;
	MODIFY_STATUE_FUNC pFun;
	
	if(NULL == pcode || changeState == 0 || changeState > 4) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	recId = 0;
	state.type     = type;
	state.userType = usertype;
	
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);

	for (i = 0; i < fileNum; i++)
	{
		recId = 0;
		
		ret = RecStoresub_SearchNextRecord(fileList[i], CB_dataCheck_ChangeState, CB_typeCheck_ChangeState, pcode, &state, &recId);
		if (ret == HY_OK)	//匹配成功.
		{
			ret = Queue_DelNode(fileList[i], recId);
			
			if (ret == HY_OK || ret == -1 || 0 == usertype) /* 管理员可能不会加载未上传的到队列 */
			{
				switch(changeState)
				{
				case 1:
					pFun = CB_modifyState_repedRecord;
					break;
				case 2:
					pFun = CB_modifyState_DeleteRecord;
					break;
				case 3:
					pFun = CB_modifyState_ChangeState_Disable;
					break;
				case 4:
					pFun = CB_modifyState_ChangeState_Enable;
					break;
				default:
					pFun = NULL;
					break;
				}
				nandFile_changeRecordStatus(fileList[i], recId, pFun);
				if(ret == -1)
				{//nand上存在该记录 但未加载进上传队列
					ret = HY_OK;
				}
			}
			break;	//正常情况下,所有文件中只有一条这样的记录. 故删除完后可直接break.
		}
	}

	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}


/*-----------------------------------------------------------
*函数：RecStore_ChangeMultiCodeState
*功能：改变指定用户类型  业务类型  条码    的状态   (一票多件使用)    可能的切换:1:待传-->可传   2:待传-->删除  3:可传-->已传   4:可传-->删除
*参数：usertype:用户类型	   
	   type:指定类型
	   code:扫描码(主单号)
	   changeState:想要改变成的状态
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：0
*------------------------------------------------------------*/
int RecStore_ChangeMultiCodeState(U16 usertype, U8 type, U8 *pcode, U8 changeState)
{
	int i, j, fileNum, ret,rc;
	int recIdx,recNum,absNum;
	int fileList[MAX_NAND_FILE];
	tDATA_INFO  node;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf;
	tRECORD_STATUS state;
	MODIFY_STATUE_FUNC pFun;
	
	if(NULL == pcode || changeState == 0 || changeState > 4) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	state.type     = type;
	state.userType = usertype;
	
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);
hyUsbPrintf("filenum = %d  state = %d  \r\n", fileNum, changeState);	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for (i = 0; i < fileNum; i++)
	{
		ptFile	= &gtFileEntry[fileList[i]];
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//没有未上传的记录
			continue;
		}
		
		//该文件有几个摘要
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		
		//从头读取有效的摘要信息
		absNum = nandFile_readFileAbstract(fileList[i], (char *)pagebuf, 0, absNum);
		
		if(absNum == HY_ERROR)//读取失败
		{
			continue;
		}
	hyUsbPrintf("absNum = %d  \r\n", absNum);
		//循环检测所有读出来的摘要
		for(j = 0; j < absNum; j++)
		{
			for(recNum = 0; recNum < ABSTRACT_NUM_PER_PAGE; recNum++)
			{
				recIdx = j*ABSTRACT_NUM_PER_PAGE+recNum;
				if(recIdx >= ptFile->totalRecords)
				{
					break;
				}
		hyUsbPrintf("abs recState = %d  userType = %d   type = %d \r\n", pagebuf[recIdx].recState, pagebuf[recIdx].userType, pagebuf[recIdx].type);
				if(pagebuf[recIdx].recState != 3 && pagebuf[recIdx].recState != 4)//只能改变待传 和 可传状态的记录
				{
					continue;
				}
				
				//权限为管理员  或权限相同       扫描类型相同
				if( (usertype == 0 || usertype == pagebuf[recIdx].userType) && (pagebuf[recIdx].type == type))
				{
					//比较条码  --需要读出记录
					rc = nandFile_read(fileList[i], recIdx, (U8 *)&node, sizeof(tDATA_INFO));
					if(rc == HY_ERROR)
					{
						continue;
					}
					
					hyUsbPrintf("code = %s   %s   %s\r\n",pcode, node.tComm.code, node.tComm.subCode);
					//条码相同
					if(strcmp(pcode, node.tComm.code) == 0)
					{
						//改变状态
						pagebuf[recIdx].recState = changeState;
						if(changeState == 1)//已传
						{
							ptFile->reportedRecords++;
						}
						else if(changeState == 2)//已删
						{
							ptFile->deletedRecords++;
						}
					}
				}
			}
			
			//该摘要block处理完,回写
			recIdx = j*ABSTRACT_NUM_PER_PAGE;
			hyUsbPrintf("write back abs  fileid = %d   recid = %d \r\n", fileList[i], recIdx);
			nandFile_WriteBackAbs(fileList[i], recIdx, (char *)pagebuf+j*NAND_PAGE_SIZE);
		}
	}

	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*----------------------------------------------------------------
*函数：RecStore_DeleteOldRecord
*功能：清除过期的记录.(每天登陆时,检测是否有过期的记录,如果有,则删除之.)
*参数：pDateName: 这天之前都认为是旧的记录(包括当天).
*返回：0.
*-----------------------------------------------------------------*/
int RecStore_DeleteOldRecord(U8 *pDateName)
{
	int i, fileNum;
	int fileList[MAX_NAND_FILE];
	int filename;
	
	filename = atoi(pDateName);
	wlock((unsigned int)&appLockNandCtr);
	fileNum = RecStore_GetFileNameList(fileList);
	
	for (i = 0; i < fileNum; i++)
	{
		if (filename >= gtFileEntry[fileList[i]].name)
		{
			nandFile_delete(gtFileEntry[fileList[i]].name);
		}
	}
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}


// 获取类型.
int nandFile_CallBack_GetStatueValue(U8 *data, int len, int *param1, int *param2)
{
	tSEQUECE_PAGE	*pagebuf;
	tDATA_INFO      *ptRec;
	U8  type, usertype;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	memcpy((U8 *)pagebuf,data,len);	
	
	ptRec = (tDATA_INFO *)data ;  /* 未做值的正确性判断 */	
	type = ptRec->tComm.opType ;
	 
	usertype = ptRec->tComm.right ;
//hyUsbPrintf("GetStatueValue opType = %d  right  = %d \r\n ",type, usertype);	
	*param1 = type;
	*param2 = usertype;
	
	return 1;
}

/*-----------------------------------------------------------
*函数：RecStore_LoadAllSubcode
*功能：主要针对一票多件,加载所有子单
*参数：pBuf:加载到的内存
       pCode:主单号
*返回：失败：-1    成功：返回记录个数
*------------------------------------------------------------*/
int RecStore_LoadAllSubcode(U8 *pBuf, U8 *pCode)
{
	return -1;//暂未实现
}

int CB_dataCheck_SearchByCode(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}

int CB_typeCheck_SearchByCode(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* 找到指定权限、未上传的记录 */
	if( state->type == pState->type      /* 扫描类型 */
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* 高6位为权限  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*-----------------------------------------------------------
*函数：RecStore_SearchByCode
*功能：通过用户类型  操作类型  单号  查找信息
*参数：right:用户类型	   
	   type:指定类型
	   pDate:指定日期   为NULL时查询全部
	   code:扫描码 (如果是一票多件  则是子单号)
	   pBuf:返回查到的数据
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：>0
*------------------------------------------------------------*/
int RecStore_SearchByCode(U16 right, U8 type, U8 *pDate, U8 *pcode, U8 *pBuf, int buflen)
{
	int i, fileIdx, fileNum, ret, total, len;
	int recId,state,absNum;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS fidRec, recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO *pData;
	tDATA_INFO node;
	U8  searchCode[32];
	
	if(NULL == pcode) 
		return HY_ERROR;
	
	strcpy(searchCode, pcode);
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
//hyUsbPrintf("right  = %d  type = %d code = %s \r\n", right, type, searchCode);	
	total = len = 0;
	
	fidRec.type = type;
	fidRec.userType = right;
//hyUsbPrintf("buf addr = %x  \r\n",(U32)pBuf);
	pData = (tDATA_INFO *)pBuf;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	if(pDate == NULL)//查询全部
	{
		fileNum = RecStore_GetFileNameList(fileList);
	}
	else
	{
		fileNum = nandFile_open(pDate, fileList);
	}
	//hyUsbPrintf("filenum = %d \r\n", fileNum);
	for (fileIdx = 0; fileIdx < fileNum; fileIdx++)
	{
		ptFile	= &gtFileEntry[fileList[fileIdx]];
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileList[fileIdx], (char *)pagebuf, 0, absNum);
	//hyUsbPrintf("absNum = %d \r\n",absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
	//hyUsbPrintf("totalRecords = %d \r\n",ptFile->totalRecords);
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
		//hyUsbPrintf("rectype = %d   fidRectype  = %d \r\n",recStat.type,fidRec.type);			
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileList[fileIdx], i, (U8 *)&node, sizeof(tDATA_INFO));
			//hyUsbPrintf("recInfo read optype = %d   i = %d \r\n",node.tComm.opType,i);
				if(ret == HY_OK)
				{
					if(gu8CurRight != 0)//不是管理员
					{
						//需要解析出记录中的员工工号和当前登录员工做对比
						ret = RecStore_CmpId(&node);
					}
				}
			//hyUsbPrintf("ret = %d \r\n",ret);	
				if(ret == HY_OK)
				{
					//比较单号
					if(strcmp(searchCode, node.tComm.subCode) == 0)
					{
						if(strcmp(searchCode, node.tComm.code) == 0)
						{
							//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
						hyUsbPrintf("00opType = %d searchCode = %s \r\n", node.tComm.opType,node.tComm.code);
							
						//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
							memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
						
						//hyUsbPrintf("11opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
							pData[total].tComm.state = recStat.recState;
						//hyUsbPrintf("22opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
							total++;
							len += sizeof(tDATA_INFO);
						}
						else
						{
							strcpy(searchCode, node.tComm.code);
						}
					}
					else if(strcmp(searchCode, node.tComm.code) == 0 && node.tComm.subCode[0] == 0)
					{
						//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
					hyUsbPrintf("00opType = %d searchCode = %s \r\n", node.tComm.opType,node.tComm.code);
						
					//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
						memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
					
					//hyUsbPrintf("11opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						pData[total].tComm.state = recStat.recState;
					//hyUsbPrintf("22opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						total++;
						len += sizeof(tDATA_INFO);
					}
				}
			}
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	//hyUsbPrintf("search optype = %d  code = %s  \r\n", pData[0].tComm.opType,pData[0].tComm.code);
	
	return total;
}


/*-----------------------------------------------------------
*函数：RecStore_GetSubCodeByMainCode
*功能：通过用户类型  操作类型  主单号 获取该主单号中的子单号
*参数：right:用户类型	   
	   type:指定类型
	   pDate:指定日期   为NULL时查询全部
	   pMainCode:主单号
	   pBuf:返回查到的数据
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：0
*------------------------------------------------------------*/
int RecStore_GetSubCodeByMainCode(U16 right, U8 type, U8 *pDate, U8 *pMainCode, U8 *pBuf, int buflen)
{
	int i, fileIdx, fileNum, ret, total, len;
	int recId,state,absNum;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS fidRec, recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO *pData;
	tDATA_INFO node;
	
	if(NULL == pMainCode) 
		return HY_ERROR;
	
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
//hyUsbPrintf("right  = %d  type = %d code = %s \r\n", right, type, pcode);	
	total = len = 0;
	
	fidRec.type = type;
	fidRec.userType = right;
//hyUsbPrintf("buf addr = %x  \r\n",(U32)pBuf);
	pData = (tDATA_INFO *)pBuf;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	if(pDate == NULL)//查询全部
	{
		fileNum = RecStore_GetFileNameList(fileList);
	}
	else
	{
		fileNum = nandFile_open(pDate, fileList);
	}
	//hyUsbPrintf("filenum = %d \r\n", fileNum);
	for (fileIdx = 0; fileIdx < fileNum; fileIdx++)
	{
		ptFile	= &gtFileEntry[fileList[fileIdx]];
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileList[fileIdx], (char *)pagebuf, 0, absNum);
	//hyUsbPrintf("absNum = %d \r\n",absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
	hyUsbPrintf("totalRecords = %d \r\n",ptFile->totalRecords);
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
		hyUsbPrintf("rectype = %d   fidRectype  = %d \r\n",recStat.type,fidRec.type);			
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileList[fileIdx], i, (U8 *)&node, sizeof(tDATA_INFO));
			hyUsbPrintf("recInfo read optype = %d   i = %d \r\n",node.tComm.opType,i);
				if(ret == HY_OK)
				{
					if(gu8CurRight != 0)//不是管理员
					{
						//需要解析出记录中的员工工号和当前登录员工做对比
						ret = RecStore_CmpId(&node);
					}
				}
			hyUsbPrintf("ret = %d \r\n",ret);	
				if(ret == HY_OK)
				{
					//比较主单号
					if(strcmp(pMainCode, node.tComm.code) == 0)
					{
					//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
					hyUsbPrintf("00opType = %d pcode = %s \r\n", node.tComm.opType,node.tComm.code);
						
					//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
						memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
					
					hyUsbPrintf("11opType = %d pcode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						pData[total].tComm.state = recStat.recState;
					//hyUsbPrintf("22opType = %d pcode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						total++;
						len += sizeof(tDATA_INFO);
					}
				}
			}
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	//hyUsbPrintf("search optype = %d  code = %s  \r\n", pData[0].tComm.opType,pData[0].tComm.code);
	
	return total;
}

//通过文件索引和记录索引查找记录
int RecStore_SearchByIndex(int fileIdx, int recIdx, tDATA_INFO *pData)
{
	int i, ret;
	int state,absStart;
	tRECORD_STATUS recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
		
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);

	ptFile	= &gtFileEntry[fileIdx];

	absStart = recIdx/ABSTRACT_NUM_PER_PAGE;

	if(nandFile_readFileAbstract(fileIdx, (char *)pagebuf, absStart, 1) == HY_ERROR)//读取失败
	{
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}

	recStat = ((tRECORD_STATUS *)pagebuf)[recIdx%ABSTRACT_NUM_PER_PAGE];
	nandFile_read(fileIdx, recIdx, (U8 *)pData, sizeof(tDATA_INFO));
	pData->tComm.state = recStat.recState;
	
	unlock((unsigned int)&appLockNandCtr);
		
	return 1;
}

/*-----------------------------------------------------------
*函数：RecStore_LoadInfo
*功能：得到指定用户类型 日期 类型的记录数据的详细信息   不包括已删除的记录
*参数：pEntry:如果知道文件的entry则传进来  不知道可以传NULL
	   usertype:用户类型
	   date:指定的日期   如果pEntry不为NULL  date可以传NULL
	   type:指定类型
	   pBuf：返回的buf  tQUEnode结构的
	   buflen：buf长度
*返回：失败：-1    成功：返回记录个数
*------------------------------------------------------------*/
int RecStore_LoadInfo(U8 *pDateName, U8 *pBuf, int buflen)
{
	int fileIdx[MAX_NAND_FILE], ret, total, len;
	int absNum,cnt;
	tDATA_INFO node;
	tQUE_NODE  *pQue;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat;
	int  readsize,i,j;
	
	wlock((unsigned)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	pQue = (tQUE_NODE *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
		
		/* 先读取该文件的所有摘要信息 */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//读取失败
		{
			unlock((unsigned)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];

			ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
			
			if(ret == HY_OK)
			{
				node.tComm.state = recStat.recState;
				pQue[total].fileId = fileIdx[j];
				pQue[total].recIdx = i;
				pQue[total].delFlag= node.tComm.state;
				pQue[total].opType = node.tComm.opType;
				memcpy(&(pQue[total].tData), &node, sizeof(tDATA_INFO));
				total++;
				len += sizeof(tQUE_NODE);
			}
		}
	}

	unlock((unsigned)&appLockNandCtr);

	return total;
}