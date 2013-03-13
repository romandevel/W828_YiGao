#include "hyTypes.h"
#include "hyswNandFile.h"
#include "reserveManager.h"

/*-----------------------------------------------------------------------------------------------------------------------*/

static  U32	gPagetoSaveBitmap;		/* 下一次回存bitmap的page index*/
static  U32	gPagetoSaveFileEntry;/* 下一次回存file Entry的page index*/

static  U8  gNeedSaveBitmap = 0; /*是否需要保存bitmap*/
/*	全局变量 */

/* nand 的bitmap */
static	U32		gnandBitmap[NAND_BITMAP_WORDS];

/*  文件信息表*/ 
tFILE_ENTRY	gtFileEntry[MAX_NAND_FILE];     

/*-----------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------
*函数：nandFile_startBlock
*功能：
*参数：
*返回：用来存储记录的nand开始的block index
*------------------------------------------------------------*/
int nandFile_startBlock()
{
	int blockNum, size;
	
	size = HSA_GetReserveManagerSize();
	
	blockNum = RESEVE_START_BLOCK_R_FILE+((size*NAND_SIZE_UNIT)>>gtnandInfo.blockSizeBit);
	
	return blockNum;
}

/*-----------------------------------------------------------
*函数：nandFile_readFileAbstract
*功能：得到文件中的摘要信息. 可以从摘要的全部page中任意取
*参数：fileIdx	-- 文件索引号
*	   pagebuf	-- 输入临时buf，输出摘要信息
*	   absStart -- 摘要信息开始读取位置,因为每个摘要block中只存一个最新page,所以该值其实是blk[]中摘要block位置
*	   absNum   -- 希望连续读取的摘要块数,即需要连续从几个摘要block中读取
*返回：成功：读取的page数   失败：-1
*------------------------------------------------------------*/
int nandFile_readFileAbstract(int fileIdx, char *pagebuf, int absStart, int absNum)
{
	int	page_start, page, abstractBlk;
	int i,j;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx < 0 || fileIdx >= MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
		
	ptFile	= &gtFileEntry[fileIdx];
	
	for(i = 0,j = 0; i < absNum; i++)
	{
		abstractBlk = ptFile->blk[absStart+i];//存放摘要的blk
		
		page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
		page		= page_start + ptFile->pageOffset[absStart+i];
		//hyUsbPrintf("read abs blk = %x  page = %d \r\n",abstractBlk,page);
		if(HY_OK != hyhwNandFlash_DataRead_Virt(pagebuf, page , 0, NAND_PAGE_SIZE))
		{
			//hyUsbPrintf("read abs err \r\n");
			return HY_ERROR;
		}
		pagebuf += NAND_PAGE_SIZE;
		j++;
	}

	return j;
}

/*************************************************************************
* 函数:	nandFile_getNewestSequecePage
* 说明: 查找循环使用的page的最新的page，
* 参数: page_start	--	起始物理page 号
*		page_end	--  结束物理page 号,即最后一个有效page
*		pagebuf		--  输入的pagebuf，临时使用，需要保证4字节对齐，返回最新的page信息
*						注意大小需要2个page
*		pSavePage	--  取得信息的page 号
* 返回: 找到，则返回HY_OK, 否则返回HY_ERROR
*************************************************************************/
U32 nandFile_getNewestSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	int j,page, crc;
	U32 rc,*pword;	
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextpage;
	
	/*	先假定初始化正常 */
	rc = HY_OK;
	
	/* 使用输入的buf*/
	ptseq_page		= (tSEQUECE_PAGE *)pagebuf;
	ptseq_nextpage	= (tSEQUECE_PAGE *)(pagebuf + NAND_PAGE_SIZE);

	
	/*	由于有些nand需要ecc，尽量读取整个page_start，读取整个page_start的代价和
		读几个字节的代价相差不大，因为总是要等待24us时间,
		而且，对于MLC，即使这里读的是几个字节，由于存在校验问题，底层driver仍需读一个page_start */
	page	= page_start;
	
	rc = hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, page_start, 0, NAND_PAGE_SIZE);
	if(HY_ERROR == rc)
	{
		//hyUsbPrintf("seq read err \r\n");
	}
	crc = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	while(page <= page_end)
	{	
		/*	如果关键字存在，表示nandflash bitmap 存在 */
		if(	ptseq_page->tag1 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->tag2 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->crc16== crc )
		{

			/* 映射表有，如果这是最后一个page，则这个page就是最新的映射表*/
			if( page == page_end )
			{
				/* 最后一个page是最新bitmap的情况*/
				break;
			}
			else
			{
				/* 不是最后一个page，看是否下一个page是没有写过的，以判断当前是否是最新的bitmap*/
				hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,(page+1), 0, NAND_PAGE_SIZE);
				crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);
				
				/* 判断 两种情况，一种数据有效，就比较轮数，一种是空白页，都表示找到了 */
				if(	ptseq_nextpage->tag1 == NAND_SEQUECE_PAGE_TAG &&
					ptseq_nextpage->tag2 == NAND_SEQUECE_PAGE_TAG &&
					ptseq_nextpage->crc16== crc             	  &&
					(( ptseq_nextpage->sequece < ptseq_page->sequece) || ( ptseq_nextpage->sequece == 0xFFFF && ptseq_page->sequece == 0))
				  )
				{
					break;
				}
				else
				{
					/* 下一个页是空白页的情况 */
					pword = (U32 *) ptseq_nextpage;
					for(j = 0;j<(NAND_PAGE_SIZE>>2); j++)
					{
						if(*pword++ != 0xFFFFFFFF)
						{
							break;
						}
					}
					
					if(j == (NAND_PAGE_SIZE>>2))  /* 找到了最新page 但该 block 可能使用了几个page 被标记为坏块的情况 */
					{
						U32  blk ;
						blk	 = (page >> PAGE_PER_BLK_BIT);						
						
						if(((blk+1)<<PAGE_PER_BLK_BIT) > page_end) break;  /* 是最后的block */
						
						/* 读下一个block 的第一个page */
						hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,((blk+1)<<PAGE_PER_BLK_BIT), 0, NAND_PAGE_SIZE);
						crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);						
						
						if(	ptseq_nextpage->tag1 == NAND_SEQUECE_PAGE_TAG &&
							ptseq_nextpage->tag2 == NAND_SEQUECE_PAGE_TAG &&
							ptseq_nextpage->crc16== crc                   &&
							ptseq_nextpage->sequece == ptseq_page->sequece )
						{
							page = (blk+1)<<PAGE_PER_BLK_BIT ;
							page -= 1 ; /* 因为下面有 page++ 了 */
						}
						else
						{
							/* crc 不正确就认为是空page 即找到了,有可能存在crc不对但不是空page的情况*/
							break;
						}		
					}		
				}
			}
			
		}
		else
		{
			/* 如果数据无效，还是一直找下去，不跳过这个块，防止漏过好的page？*/   /*-----最新的page的下一个不应该为0xff吗----12-24--要添加处理 否则------------*/
			
			/* 不是最后一个page，看是否下一个page是没有写过的，以判断当前是否是最新的bitmap*/
			rc = hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,(page+1), 0, NAND_PAGE_SIZE);
			if(HY_ERROR == rc)
			{
				//PhoneTrace(0,"read fail !");
			}
			crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);
		}
		
		memcpy((S8*)ptseq_page,(S8*)ptseq_nextpage, NAND_PAGE_SIZE);
		page ++;	
	}
	
	/* 存储最新信息的page 号回传给调用者*/
	*pSavePage	= page;
	
	if(page > page_end)
	{
		/* 没有找到有效项，从头开始存*/
		*pSavePage = page_start;
		rc = HY_ERROR;
	}
	
	return rc;
}


/*************************************************************************
* 函数:	nandFile_saveSequecePage
* 说明: 存储循环使用的page的到nand上， blk 是连续的，适合于事先已经分配了连续
*		blk的 bitmap 和 fileentry 的page的存储，不适合按照循环page存储的文件的
*		abstract blk的循环page
* 参数: page_start	--	存储空间的起始page 号
*       page_end	--  存储空间的最后的page 号
*		pagebuf		--  输入的pagebuf，需要保证4字节对齐, 一个page 大小
*		pSavePage	--  输入/输出现在save 的page 号，返回这次存的page 号
*						总之，该page 上总是最新的信息
* 返回: 成功存储，没有分配新块，则返回HY_OK == 0
*		否则返回HY_ERROR   = -1
*************************************************************************/
U32 nandFile_saveSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	U8		isBitmap = 0;//本次存储的是否bitmap
	U32		rc,*pBitmap,*pBadBlock;
	int		page;
	int		blk, pageOffset;
	int		i,sblk,eblk;
	int		maxPage,totalPage;
	tSEQUECE_PAGE	*ptseq_page;
	
	if(page_start == (BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT))
	{
		isBitmap = 1;
	}
	
	/*	先假定正常 */
	rc = HY_OK;
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	pBitmap   = (U32 *)(ptseq_page->data);
	
	/* 读出当下要存的位置 */
	page	= *pSavePage;
	
	/* 这次要写的位置加1*/
	page ++;
	maxPage 	= page_end - page_start +1;
	
	if(page > page_end)
	{
		/* 由于save page no 是简单加1的，这里需要判断 */
		page -= maxPage;
		
		/* 从page_start 的情况，轮号需要加1 了， 如果是 0xFFFF，加1即溢出，为0， 是我们要的结果*/
		if( page == page_start)
		{
			ptseq_page->sequece +=1;
		}
	}
	
	if(page == page_start)
	{//把后面blk全erase
		sblk = (page_start >> PAGE_PER_BLK_BIT)+1;
		eblk = (page_end >> PAGE_PER_BLK_BIT);
		for(i = sblk; i <= eblk; i++)
		{
			hyhwNandFlash_Erase_Virt(i);
		}
	}
	
	/* 总page 数， 使用总page 数控制循坏 */
	totalPage = 0;
	while( totalPage <= maxPage)
	{
		blk			= (page >> PAGE_PER_BLK_BIT);
		pageOffset	= page - (blk<<PAGE_PER_BLK_BIT);	
		
		/* 注意下面的语句的顺序不能变，也不能简化*/
		if((pageOffset == 0 && 
		    hyhwNandFlash_Erase_Virt(blk) == HY_OK &&
		    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
		    )
		{
			break;
		}
		else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			break;
		}
		else	 	
		{		
			/*坏块标记*/
			//SET_BLOCK_BAD(blk);
			SET_BLOCK_USED(blk);
			if(isBitmap == 1)
			{
				//此处需要同步更新本次需要写入的信息
				pBitmap[blk>>5] &= ~(1<<(blk &0x1F));
			}
			else
			{
				gNeedSaveBitmap = 1;//下次需要存bitmap
			}
			
			/* 跳过去了很多page */
			page		+= (1<<PAGE_PER_BLK_BIT) - pageOffset;
			totalPage	+= (1<<PAGE_PER_BLK_BIT) - pageOffset;
			
			if(page > page_end)
			{
				/* 由于save page no 是简单加1的，这里需要判断 */
				page -= maxPage;//(page_end - page_start + 1);
				
				/* 从page_start 的情况，轮号需要加1 了， 如果是 0xFFFF，加1即溢出，为0， 是我们要的结果*/
				if( page == page_start)
				{
					ptseq_page->sequece +=1;
				}	
			}
		}
	}
	
	if( totalPage > maxPage)
	{
		rc = HY_ERROR;
	}
	else
	{
		*pSavePage = page ;	
	}
	
	return rc;
}

/*************************************************************************
* 函数: nandFile_saveFileEntry
* 说明: 保存fileentry
* 参数: 
* 返回: 如果正常回存成功，则返回HY_OK, 否则返回HY_ERROR
 *************************************************************************/
U32 nandFile_saveFileEntry(char *pagebuf)
{
	int		page;
	U32     ret, sblk,eblk;
	tFILE_ENTRY     *pFile;
	tSEQUECE_PAGE	*ptseq_page;
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gPagetoSaveFileEntry, 0, NAND_PAGE_SIZE);
	
	pFile = &gtFileEntry[0];
	
	/* 更新信息*/
	memcpy( ptseq_page->data, (char *)pFile, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_FILE;
	eblk = BLOCK_END_FILE+1;
	
	/* 注意由于指针要用地址，必须这样转一下 */
	page= gPagetoSaveFileEntry;
	ret = nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								    (eblk<<PAGE_PER_BLK_BIT)-1,
							 	    (char *)(ptseq_page), 
							 	    &page);
	if(HY_ERROR == ret)
	{
		//hyUsbPrintf("nandFile_saveFileEntry  fail  \r\n");
	}
	
	gPagetoSaveFileEntry = page;
	
	return ret;			 	 
}


/*************************************************************************
* 函数: nandFile_saveBitmap
* 说明: 将gnandBitmap（映射表在内存的buffer） 写入到nandFlash中 
*       仅在close nandflash时调用一次                 							
* 参数: none 
* 返回: 如果正常回存成功，则返回HY_OK, 否则返回HY_ERROR
 *************************************************************************/
U32 nandFile_saveBitmap( char *pagebuf )
{
	int page;
	U32 ret, sblk,eblk;
	tSEQUECE_PAGE	*ptseq_page;
	
	if(gNeedSaveBitmap == 0)
	{
		return HY_OK;
	}
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;

	/* 回存 fileentry，回存 fileEntry 	*/
	/* 这个一定放最后，以防可能存nandfile entry的时候，改变bitmap*/
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gPagetoSaveBitmap, 0, NAND_PAGE_SIZE);
	
	memcpy( ptseq_page->data, (char *)gnandBitmap, NAND_BITMAP_WORDS *sizeof(U32));
	
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_BITMAP;
	eblk = BLOCK_END_BITMAP+1;
	
	/* 注意由于指针要用地址，必须这样转一下 */
	page	= gPagetoSaveBitmap;
	ret  = nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								     (eblk<<PAGE_PER_BLK_BIT)-1,
						 		     (char *)(ptseq_page), 
						 		     &page);
	if(HY_ERROR == ret)
	{
		//hyUsbPrintf("nandFile_saveBitmap fail  \r\n");
	}				 	
						 	
	gPagetoSaveBitmap = page ;
	
	gNeedSaveBitmap = 0;
	
	return ret;						 	 
}


/*************************************************************************
* 函数:	nandFile_format
* 说明: 创建物理block的bitmap， 最重要的是，把bitmap区和file entry 区的blk
*		要擦除干净
* 参数: nandReservedSize	--  nand上被应用程序reserve的空间，单位为M字节
* 返回: 如果正常初始化成功，则返回HY_OK, 否则返回HY_ERROR
*************************************************************************/
U32 nandFile_format(U32 nandReservedSize, char *pagebuf)
{
	S32	page, i, usedBlknum;
	U32 *pword, sblk,eblk;
	tSEQUECE_PAGE	*ptpageBitmap, *ptpageEntry;
	

	gPagetoSaveBitmap    = (BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT);
	gPagetoSaveFileEntry = (BLOCK_START_FILE<<PAGE_PER_BLK_BIT);
	
	/* 因为这是format,第一个page 也没用 ,所以把当前的page 定为上一个 */
	gPagetoSaveBitmap    -= 1;
	gPagetoSaveFileEntry -= 1;
	
	for(i = 0 ; i < BLOCK_NUM_BITMAP; i++)
	{
		hyhwNandFlash_Erase_Virt(BLOCK_START_BITMAP+i);
	}
	
	for(i = 0 ; i < BLOCK_NUM_FILEENTRY; i++)
	{
		hyhwNandFlash_Erase_Virt(BLOCK_START_FILE+i);
	}

	/*	坏块计数，初始化时，telechip程序使用的block数和为bitmap 以及reserve空间的block为"坏"块，
		因为它们不能用来作为存储数据的block了 */
	usedBlknum		= BLOCK_END_FILE;
	
	memset((char *)gnandBitmap,0xff,NAND_BITMAP_WORDS * sizeof(U32));
	
	pword = (U32*)gnandBitmap;
	for(i= 0; i<=usedBlknum; i++)
	{
		/* 标记该block已被使用 */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	//磁盘最后还有部分保留block
	pword = (U32*)gnandBitmap;
	for(i= BLOCK_DATA_END+1; i < (NAND_FLASH_SIZE<<3); i++)
	{
		/* 标记该block已被使用 */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	/*-------------------------------------------entry-----------------------*/
	memset((U8 *)gtFileEntry,0xff, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
	
	ptpageEntry		= (tSEQUECE_PAGE *)pagebuf;
	
	memset((char *)ptpageEntry,0xFF,NAND_PAGE_DATASIZE);	
	ptpageEntry -> sequece	= 0;
	ptpageEntry -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry -> tag2		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry ->crc16 = glbVariable_CalculateCRC((U8*)ptpageEntry,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_FILE;
	eblk = BLOCK_END_FILE+1;
	page = gPagetoSaveFileEntry;
	nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageEntry), 
							  &page);
	gPagetoSaveFileEntry = page ;	
	/*------------------------------------------bitmap------------------------*/
	/* 使用输入的buf*/
	ptpageBitmap		= (tSEQUECE_PAGE *)pagebuf;
	
	/* 先假定所有的block都是好的*/
	memset((char *)ptpageBitmap,0xFF,NAND_PAGE_DATASIZE);
	ptpageBitmap -> sequece	= 0;
	ptpageBitmap -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageBitmap -> tag2		= NAND_SEQUECE_PAGE_TAG;

	
	memcpy(ptpageBitmap->data,(char *)gnandBitmap, NAND_BITMAP_WORDS * sizeof(U32));

	ptpageBitmap ->crc16 = glbVariable_CalculateCRC((U8*)ptpageBitmap,NAND_PAGE_DATASIZE);	
	
	/* 最后存bitmap*/
	page 	= gPagetoSaveBitmap;
	
	sblk = BLOCK_START_BITMAP;
	eblk = BLOCK_END_BITMAP+1;
	nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageBitmap), 
							  &page);
						 	 
	gPagetoSaveBitmap = page;
	
	/*--------------------------------------------------------------------*/
						 	 
	return HY_OK;
}

/*----------------------------------
* 函数: nandFile_checkRecBitmap
* 功能: 检查bitmap管理
* 参数: 
* 返回: 0:正常   -1:异常
*-----------------------------------*/
int nandFile_checkRecBitmap()
{
	int 		i,j,idx,sblk,eblk;
	int			blk,used,ret=0;
	tFILE_ENTRY	*pFile;

	sblk = BLOCK_DATA_START;
	eblk = BLOCK_DATA_END;
	
	for(i = sblk; i <= eblk; i++)
	{
	//hyUsbPrintf("cur block id = %d \r\n", i);
		if(!IS_BLOCK_FREE(i))//被占用
		{
			used = 0;
			
			//检查是否确实被文件占用
			for(idx = 0; idx < MAX_NAND_FILE; idx++)
			{
				pFile = &gtFileEntry[idx];
				if(pFile->totalRecords == 0)
				{
					continue;
				}
				for(j = 0; j < MAX_BLOCKS_PER_FILE; j++)
				{
					blk = pFile->blk[j];
					if(blk == i)//被该文件占用
					{
						used = 1;//标记
						break;
					}
				}
			}
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("block used = %d \r\n",used);
			#endif
			if(used == 0)//没有被文件占用
			{
				SET_BLOCK_FREE(i);
				ret = -1;
			}
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("block stat = %d   %d \r\n", i, IS_BLOCK_FREE(i));
			#endif
		}
	}
	
	return ret;
}

/*************************************************************************
* 函数:	nandFile_Init
* 说明: 查找化物理block的bitmap 表，bit0表示对应的物理block 占用，1表示没有占用
*		如果nandflash bitmap的TAG 存在，则表示有bitmap，否则，就创建
*       查找
* 参数: pagebuf				--	buffer，大小由nandflash的page size决定，2个page 大小
*		nandReservedSize	--  nand上被应用程序reserve的空间，单位为M字节
* 返回: 如果正常初始化成功，则返回HY_OK, 否则返回HY_ERROR
*************************************************************************/
U32 nandFile_Init( U32 reserve_ziku)
{
	char *pagebuf ;
	U32 rc;	
	int	page;
	S32	page_start,page_end;
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextPage;
	
	/*	先假定初始化正常 */
	rc = HY_OK;
	
	gNeedSaveBitmap = 1;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	ptseq_page		= (tSEQUECE_PAGE *)pagebuf;
	ptseq_nextPage	= (tSEQUECE_PAGE *)(pagebuf + NAND_PAGE_SIZE);

	/* 第一部分：从NandFlash的 存储nand bitmap的block 处读取bitmap  ，
	如果有证明原来已经有nand bitmap了，就不再重新建立,就找最新的nand bitmap,*/
	
	/* blk 的首page index */
	page_start	= (BLOCK_START_BITMAP<< PAGE_PER_BLK_BIT);
	page_end	= ((BLOCK_END_BITMAP +1) << PAGE_PER_BLK_BIT)-1;
	
	if(nandFile_getNewestSequecePage(page_start, page_end, pagebuf, &page) == HY_ERROR)
	{
		/* 映射表无效，需要写入nand bitmap，所以就enable write */
		hyhwNandFlash_EnableWrite();
		rc = nandFile_format(reserve_ziku, pagebuf);
		hyhwNandFlash_DisableWrite();
	}
	else
	{
		gPagetoSaveBitmap = page;
		
		/* 找到bitmap，copy */
		memcpy((char *)gnandBitmap, pagebuf, sizeof(U32)*NAND_BITMAP_WORDS);
		
		/* bitmap 对了，找file entry 才有意义*/
		/* 第二部分：从NandFlash的 存储file entry 区读文件信息,*/
		
		//找fileentry
		page_start	= (BLOCK_START_FILE<< PAGE_PER_BLK_BIT);
		page_end	= ((BLOCK_END_FILE +1) << PAGE_PER_BLK_BIT)-1;
		if(nandFile_getNewestSequecePage(page_start, page_end, pagebuf, &page) == HY_ERROR)
		{
			/* 没有找到有效的file entry，没有文件的情况 */
			gPagetoSaveFileEntry = page_start - 1;
			memset((U8 *)gtFileEntry,0xff, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
			nandFile_saveFileEntry(pagebuf);
			//在saveFileEntry时可能会更改bitmap
			nandFile_saveBitmap(pagebuf);
		}
		else
		{
			//PhoneTrace(0,"entry init ok!");
			gPagetoSaveFileEntry = page;
			/* 找到fileEntry，copy */
			memcpy((char *)gtFileEntry, pagebuf, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
		}
	}
	gNeedSaveBitmap = 0;
	
	//检查bitmap
	if(-1 == nandFile_checkRecBitmap())
	{
		gNeedSaveBitmap = 1;
		nandFile_saveBitmap(pagebuf);
	}
	
	return rc;
}


/*************************************************************************
* 函数: nandFile_find1FreeBlock
* 说明:	从bitmap中查找出来一个空的block供使用. 返回查找到的block.
* 参数:	none
* 返回:	成功，blk num  失败  -1
*************************************************************************/
int nandFile_find1FreeBlock( void )
{
	int i,findBlock=HY_ERROR;
	
	for(i = BLOCK_DATA_START; i <= BLOCK_DATA_END; i++)
	{
		if(IS_BLOCK_FREE(i))
		{
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("find1FreeBlock i = %d \r\n", i);
			#endif
			if(hyhwNandFlash_Erase_Virt(i) == HY_OK)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("find1FreeBlock erase ok \r\n");
				#endif
				findBlock =i;
				SET_BLOCK_USED(i);
				gNeedSaveBitmap = 1;
				break;
			}
		}
	}
	
	return findBlock;
}

/*************************************************************************
* 函数: nandFile_create
* 说明:	创建一个文件, 并初始化当前文件的信息(bitmap更新,FileEntry增加,block[0]暂不管)	  
* 参数:	name	-- 要创建的文件名
        cnt:已经存在的同名文件数
* 返回:	fileIdx  失败:-1
*************************************************************************/
int nandFile_create( char *name, int cnt)
{
	int i, minidx=-1,abstractBlk, rc = HY_OK;
	char *pagebuf;
	int  minname,filename;
	tFILE_ENTRY	*ptFileEntry;
	tSEQUECE_PAGE	*ptseq_page;
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptseq_page = (tSEQUECE_PAGE	*)pagebuf;
	
	filename = atoi(name);
	minname = 0x7FFFFFFF;//用来存放日期最早的一个文件名
	ptFileEntry	= NULL;
	/* 找到一个空的fileEntry */
	for(i = 0; i < MAX_NAND_FILE; i++)
	{
		if( gtFileEntry[i].name == -1)
		{
			/* 找到一个空的项*/
			ptFileEntry = &gtFileEntry[i];
			rc = i ;
			break;
		}
		else
		{
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("minidx = %d minname = %d  file idx = %d  file name = %d \r\n",minidx,minname,i,gtFileEntry[i].name);
			#endif
			if(minname > gtFileEntry[i].name)
			{
				minname = gtFileEntry[i].name;
				minidx = i;
			}
		}
	}
	#ifdef NANDFLASH_DEBUG
		hyUsbPrintf("minname = %d    %d \r\n",minname,minidx);
	#endif
	if(i >= MAX_NAND_FILE && minidx != -1)//没有空闲文件,删除最早的一个
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("删除最早的一个文件 \r\n");
		#endif
		nandFile_deleteByIndex(minidx);
		ptFileEntry = &gtFileEntry[minidx];
		rc = minidx;
		i = minidx;
	}
	
	if(ptFileEntry)
	{
		memset((char *)ptFileEntry,0xFF,sizeof(tFILE_ENTRY));
		
		ptFileEntry->name = filename; 
		if(cnt < 0) cnt = 0;
		ptFileEntry->dayIdx = cnt;
		ptFileEntry->totalRecords	= 0;
		ptFileEntry->reportedRecords= 0;
		ptFileEntry->deletedRecords	= 0;
		memset((char *)ptFileEntry->pageOffset,0,ABS_BLOCKS_PER_FILE*sizeof(U16));
		
		/* 回存 fileEntry，回存 fileEntry 	*/
		nandFile_saveFileEntry(pagebuf);
		/* 这个一定放最后，因为可能存nandfile entry的时候，改变bitmap*/
		nandFile_saveBitmap(pagebuf);
	}
	else
	{
		/* 没有找到空的文件项，返回错误 */
		rc = -1;
	}

	return rc;
}	

/*-----------------------------------------------------------
*函数：nandFile_open
*功能：通过文件名找到文件，返回file idx   
       注意:可能存在同名文件, 如果一天的记录太多,一个文件存放不完,就要使用2个文件,这2个文件名相同.
*参数：	name -- 文件名
*		pagebuf	-- 临时buf,一个page 大小
*返回：成功：返回找到的文件个数   失败：<=0
*------------------------------------------------------------*/
int nandFile_open(char *name, int *pFileIdx)
{
	int i, fileCnt;
	int filename;
	
	filename = atoi(name);
	/* 先假定没有找到	*/
	fileCnt = 0;
	
	if(name[4] == 0)//按月查
	{
		/* 找到文件 */
		for(i = 0; i<MAX_NAND_FILE; i++)
		{
			if((gtFileEntry[i].name/100) == filename) 
			{
				/* 找到文件*/
				pFileIdx[fileCnt++] = i;
			}
		}
	}
	else
	{
		/* 找到文件 */
		for(i = 0; i<MAX_NAND_FILE; i++)
		{
			if(gtFileEntry[i].name == filename) 
			{
				/* 找到文件*/
				pFileIdx[fileCnt++] = i;
			}
		}
	}
	
	return fileCnt;
}

/*-----------------------------------------------------------
*函数：nandFile_write
*功能：写入记录
*参数：fileIdx	-- 文件号
*	   data  	-- 缓存，放要写入的数据，纯数据部分
*      len     -- 数据长
*      recState  --- 写记录时记录的初始状态
*      reIdx   -- 带回index
*返回：成功：0   失败：-1
*------------------------------------------------------------*/
int nandFile_write(int fileIdx, char *data,int len,U8 recState,U32 *reIdx)
{
	U32 pageOffset, idxOfPage, page_start, page_end;
	int needNewBlk, recordIdx, page;
	int i, blk, blknum, abstractBlk;
	U8  type, usertype;
	tFILE_ENTRY		*ptFile;
	U8  *pOldBuf = NULL, *pagebuf;
	int  oldNum = 0,oldPage;
	
	if(fileIdx < 0 || fileIdx > MAX_NAND_FILE || len > NAND_PAGE_DATASIZE)
	{
		return HY_ERROR;
	}
	
	ptFile		= &gtFileEntry[fileIdx];
	
	if(ptFile->totalRecords >= MAX_RECORDS_PER_FILE)//达到最大block数
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("totalRecords (%d) > MAX_RECORDS_PER_FILE (%d) \r\n", ptFile->totalRecords, MAX_RECORDS_PER_FILE);
		#endif
		//需新建文件  暂不考虑
		return  HY_ERROR;
	}
	
	/* 找到 blk Idx 和 pageOffset*/
	blknum		= ptFile->totalRecords/RECORDS_PER_BLOCK;
	pageOffset	= ptFile->totalRecords%RECORDS_PER_BLOCK;
	
	idxOfPage   = pageOffset%RECORDS_PER_PAGE;
	pageOffset  = pageOffset/RECORDS_PER_PAGE;
	
	/* 由于前面ABS_BLOCKS_PER_FILE 个	blk  特殊用途，所以blknum +ABS_BLOCKS_PER_FILE, 根据数组得到物理blk idx*/
	blknum		+= ABS_BLOCKS_PER_FILE;
	blk	= ptFile->blk[blknum];
	
	
	/* 判断是否需要新的blk */
	needNewBlk = 0;
	if(blk == 0xFFFF)
	{
		needNewBlk = 1;
	}
	else
	{
		page = (blk<<PAGE_PER_BLK_BIT) + pageOffset;
	}

	
	do
	{
		if(needNewBlk)
		{
			/* 分配*/
			blk = nandFile_find1FreeBlock();
			if(blk == -1)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("--- write record find block fail \r\n");
				#endif
				/* 分配不出来了，返回错误 */
				return HY_ERROR;
			}
			
			ptFile->blk[blknum] = blk;
			
			/* 从block 头开始写*/
			page = (blk <<PAGE_PER_BLK_BIT); 
			
			/* 表示已经分配好了*/
			needNewBlk = 0;
		}
		
		/*	kong 2011-05-16 
			上一次循环中，没有成功写入，之前的blk坏了  需要把记录copy到新blk中 
			否则 一:会导致记录丢失  二:会导致nand中记录数量和totalRecords 不一致  后续read时异常 */
		if(pOldBuf && hyhwNandFlash_PageWrite_Virt(pOldBuf, page, 0,oldNum*NAND_PAGE_SIZE+RECORD_PER_SIZE*idxOfPage) != HY_OK)
		{
			
			/* 前面的旧信息copy到新的block 中没有成功，继续分配新块 */
			needNewBlk = 1;
			SET_BLOCK_USED(blk);	/* 直接标记为坏块 */
			gNeedSaveBitmap = 1;//下次需要存bitmap
			continue;
		}
		else if(pOldBuf)//kong 2011-05-24 如果坏块block中的数据成功写入到新申请的block中,在写新数据前需要调整page
		{
			page += oldNum;
		}
		
		/*kong 2011-05-24 在这里先把该标记置为1,后面写成功后会重新置为0 */
		needNewBlk = 1;
		
		if(hyhwNandFlash_PageWrite_Virt(data, page, RECORD_PER_SIZE*idxOfPage,RECORD_PER_SIZE) == HY_OK)
		{
			//hyUsbPrintf("\r\nwrite totalRecords = %d date = %c  blk = %d  page = %d  idx = %d\r\n",
			//	ptFile->totalRecords,ptFile->name[5],blk,page,idxOfPage);
			
			/* 写成功的情况 */
			ptFile->totalRecords++;
			needNewBlk = 0;
		}
		else if(pOldBuf == NULL)
		{
			/* 不管之前有没有分配新块，没有写成功，则必须处理，而且原有的记录需要copy到新block中*/
			oldPage	= (blk <<PAGE_PER_BLK_BIT);
			oldNum	= pageOffset;
			
			if(oldNum > 0 || idxOfPage > 0)
			{
				/* 把有用的信息读出来 */
				pOldBuf = (U8 *)(gpu8RecBuf+12288); /*128k */
				hyhwNandFlash_DataRead_Virt((S8*)pOldBuf, oldPage, 0, oldNum*NAND_PAGE_SIZE+RECORD_PER_SIZE*idxOfPage);
			}

			SET_BLOCK_USED(blk);	/* 直接标记为坏块 */
			gNeedSaveBitmap = 1;//下次需要存bitmap
			//hyUsbPrintf("PageWrite fail   blk = %d   page = %d\r\n",blk,page);
		}
		else//kong 2011-05-24 把旧记录全部copy到新申请的block中成功啦，但是copy当前一天新记录时失败了，需要再次把新申请的block标记为坏快
		{
			SET_BLOCK_USED(blk);	/* 直接标记为坏块 */
			gNeedSaveBitmap = 1;//下次需要存bitmap
		}
		
	}while(needNewBlk);
		
	/* 下面开始改记录的摘要信息 */
	pagebuf = (U8 *)gpu8RecBuf;
	
	/* 得到当前记录的type 和 usertype */
	nandFile_CallBack_GetStatueValue(data, len, &type, &usertype);
	
	/* 更新record 的状态，在摘要blk 组中 */
	recordIdx	= ptFile->totalRecords -1;
	
	//找到该记录需要写入的block及page
	blk 		= recordIdx / ABSTRACT_NUM_PER_PAGE;//当前需要更新的记录所在的blk
	pageOffset  = recordIdx % ABSTRACT_NUM_PER_PAGE;//该记录在当前page中的偏移
		
	abstractBlk = ptFile->blk[blk];
	
	needNewBlk = 0;
	if(abstractBlk == 0xFFFF)
	{
		needNewBlk = 1;
	}
	else
	{
		page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
		page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
		page		= page_start + ptFile->pageOffset[blk];
	}
	#ifdef NANDFLASH_DEBUG
		hyUsbPrintf("write   type = %d  usertype  = %d  recState = %d  \r\n",type, usertype, recState);	
	#endif
	memset(pagebuf,0xFF,NAND_PAGE_SIZE);
	do
	{
		if(needNewBlk)
		{
			/* 分配*/
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("--- write abs find block fail \r\n");
				#endif
				/* 分配不出来了，返回错误 */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* 从block 头开始写*/
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
						
			/* 表示已经分配好了*/
			needNewBlk = 0;
			
			((tRECORD_STATUS *)pagebuf)[pageOffset].type     = type ;
			((tRECORD_STATUS *)pagebuf)[pageOffset].userType = usertype;
			((tRECORD_STATUS *)pagebuf)[pageOffset].recState = recState;
			//hyUsbPrintf("write rec type = %d, usertype = %d, state = %d \r\n",
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].type,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].userType,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].recState);
		}
		else
		{
			hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
			((tRECORD_STATUS *)pagebuf)[pageOffset].type     = type ;
			((tRECORD_STATUS *)pagebuf)[pageOffset].userType = usertype;
			((tRECORD_STATUS *)pagebuf)[pageOffset].recState = recState;
			//hyUsbPrintf("write rec type = %d, usertype = %d, state = %d \r\n",
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].type,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].userType,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].recState);
				
			page++;
			if(page > page_end)
			{
				/* 超界了，重新从前面写起 */
				page = page_start;
				if(hyhwNandFlash_Erase_Virt(abstractBlk) == HY_ERROR)
				{
					SET_BLOCK_USED(abstractBlk);
					gNeedSaveBitmap = 1;
					needNewBlk = 1;
					continue;
				}
			}
		}
		
		if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			needNewBlk = 0;
		}
		else
		{
			/*写失败后无需把之前所有page中存的摘要全copy出来,因为最新的一个page中数据已经读出来,再申请新block时只需把最新数据写入即可*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			needNewBlk = 1;
		}
		
	}while(needNewBlk);
	
	ptFile->pageOffset[blk] = page - page_start;

	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	*reIdx = ptFile->totalRecords -1 ;
	
	return HY_OK;
}

/*-----------------------------------------------------------
*函数：nandFile_read
*功能：读取指定信息的数据
*参数：fileIdx
*	   :入口  pBuf:存放读取的数据 data:指定的数据
*返回：成功：0   失败：-1
*------------------------------------------------------------*/
int nandFile_read(int fileIdx, int recordIdx, char *buff, int len)
{
	U32 blk, page, pageOffset,idxOfPage;
	U32 rc;
	U8 *pagebuf;
	tFILE_ENTRY		*ptFile;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	if (fileIdx < 0 || fileIdx >= MAX_NAND_FILE)
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("fileidx err !!! = %d \r\n",fileIdx);
		#endif
		return HY_ERROR;
	}
	
	ptFile		= &gtFileEntry[fileIdx];
	
	if(recordIdx >= ptFile->totalRecords || ptFile->totalRecords == 0xFFFF)
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("recordIdx or totalRecords  err !!! = %d  %d\r\n",recordIdx, ptFile->totalRecords);
		#endif
		/* record idx 超界 或 文件已被删除*/
		return  HY_ERROR;
	}
		
	/* 找到 blk Idx 和 pageOffset*/
	blk 		= recordIdx/RECORDS_PER_BLOCK;
	pageOffset	= recordIdx%RECORDS_PER_BLOCK;
	
	idxOfPage   = pageOffset%RECORDS_PER_PAGE;
	pageOffset  = pageOffset/RECORDS_PER_PAGE;
	

	/* 由于前面ABS_BLOCKS_PER_FILE个blk 特殊用途，所以blknum +ABS_BLOCKS_PER_FILE, 根据数组得到物理blk idx*/
	blk	+= ABS_BLOCKS_PER_FILE;
	blk	= ptFile->blk[blk];
	
	page= (blk<<PAGE_PER_BLK_BIT) + pageOffset; 
	
	rc = hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, idxOfPage*RECORD_PER_SIZE, len);
	//hyUsbPrintf("hyhwNandFlash_DataRead_Virt 读取结果(0:成功)  %d\r\n",rc);
	if(HY_OK == rc)
	{
		//hyUsbPrintf("read data = %d\r\n",*(U32*)pagebuf);
		//kong 2011-05-16   对读出的数据做判断
		if(*(U32*)pagebuf != 0xFFFFFFFF)
		{
			memcpy(buff, pagebuf, len);
		}
		else
		{
			rc = HY_ERROR;
		}	
	}
	
	return rc;
}

/*************************************************************************
* 函数: nandFile_delete
* 说明:	删除文件	  注意:可能存在同名文件
* 参数:	name	-- 要删除的文件名
* 返回:	none
*************************************************************************/
int nandFile_delete(char *name)
{
	int i,j,filename;
	int rc = HY_ERROR;
	char	*pagebuf;
	tFILE_ENTRY	*ptFile;
	
	filename = atoi(name);
	ptFile	= NULL;
	
	/* 找到文件 */
	for(i = 0; i<MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name == filename)
		{
			rc = HY_OK;
			/* 找到文件*/
			ptFile = &gtFileEntry[i];
			
			/* 释放blk*/
			for(j = 0; j< MAX_BLOCKS_PER_FILE; j++)
			{
				/* 正确擦除了才能释放*/
				if(ptFile->blk[j] !=0xFFFF)
				{
					SET_BLOCK_FREE(ptFile->blk[j]);
					gNeedSaveBitmap = 1;
				}
			}
			
			/* 释放目录项*/
			memset((char *)ptFile,0xFF,sizeof(tFILE_ENTRY));
		}
	}
	
	if(rc == HY_OK)
	{
		pagebuf = (char *)gpu8RecBuf;
		
		/* 回存 fileentry，回存 fileEntry 	*/
		/* 这个一定放最后，以内可能存nandfile entry的时候，改变bitmap*/
		nandFile_saveFileEntry(pagebuf);
		nandFile_saveBitmap(pagebuf);
	}
	
	return rc ;
}	

/*************************************************************************
* 函数: nandFile_deleteByIndex
* 说明:	删除文件	  
* 参数:	index	-- 要删除的文件索引值
* 返回:	none
*************************************************************************/
int nandFile_deleteByIndex(int index)
{
	int i,j,rc = HY_OK;
	int filename;
	char *pagebuf;
	tFILE_ENTRY	*ptFile;

	if (index < 0 || index >= MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	ptFile = &gtFileEntry[index];
	filename = ptFile->name;//由于有同名文件,需要从所有文件中查找一遍
	for(i = 0; i < MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name == filename) 
		{
			/* 找到文件*/
			ptFile = &gtFileEntry[i];
			/* 释放blk*/
			for(j = 0; j< MAX_BLOCKS_PER_FILE; j++)
			{
				/* 正确擦除了才能释放*/
				if(ptFile->blk[j] !=0xFFFF)
				{
					SET_BLOCK_FREE(ptFile->blk[j]);
					gNeedSaveBitmap = 1;
				}
			}
			
			
			/* 释放目录项*/
			memset((char *)ptFile,0xFF,sizeof(tFILE_ENTRY));
		}
	}	
	
	pagebuf = (U8 *)gpu8RecBuf;
	/* 回存 fileentry，回存 fileEntry 	*/
	/* 这个一定放最后，以内可能存nandfile entry的时候，改变bitmap*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return rc;
}

/*-----------------------------------------------------------
*函数：nandFile_WriteBackAbs
*功能：回写摘要block
*参数：fileIdx		-- 文件index号
*	   recordIdx	-- 记录index号
       pAbsData     -- 新的摘要信息
*返回：成功：HY_OK   
*	   失败：HY_ERROR
*------------------------------------------------------------*/
int nandFile_WriteBackAbs(int fileIdx, int recordIdx, U8 *pAbsData)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
	
	//hyUsbPrintf("change state idx = %d\r\n",recordIdx);
	if(recordIdx >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	//待写入的新摘要信息
	memcpy(pagebuf, pAbsData, NAND_PAGE_SIZE);
	
	blk			= recordIdx / ABSTRACT_NUM_PER_PAGE;
	pageOffset	= recordIdx % ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	/* 回存到下一个page 中 */
	page ++;
	
	if(page > page_end)
	{
		/* 超界了，重新从前面写起 */
		page = page_start;
	}
	/* 注意下面的语句的顺序不能变，也不能简化*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*坏块标记*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*重新分配 blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* 分配不出来了，返回错误 */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 由于这里是新申请的block,需要重新定位page */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	
	if(ptFile->deletedRecords == ptFile->totalRecords)	//如果全部删除,则删掉文件
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* 更新file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}


/*-----------------------------------------------------------
*函数：nandFile_changeRecordStatus
*功能：修改记录状态
*      为了减少擦除次数，将存储abstract page 的 blk 扩大
*	   如果一个文件的abstract page 组最多是ABSTRACT_PAGE_NUM个，
*	   则使用前ABSTRACT_PAGE_NUM个blk 做为abstract page 的blk
*	   每次更新时，每个blk 仅更新一个page
*参数：fileIdx		-- 文件index号
*	   recordIdx	-- 记录index号
*	   modify_func	-- 函数指针，执行真正的修改
*返回：成功：HY_OK   
*	   失败：HY_ERROR
*------------------------------------------------------------*/
int nandFile_changeRecordStatus(int fileIdx, int recordIdx, MODIFY_STATUE_FUNC modify_func)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
	
	//hyUsbPrintf("change state idx = %d\r\n",recordIdx);
	if(recordIdx >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	blk			= recordIdx / ABSTRACT_NUM_PER_PAGE;
	pageOffset	= recordIdx % ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	//读出之前的记录摘要,只读当前需要改变的记录
	hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
	
	ret = modify_func(&((tRECORD_STATUS *)pagebuf)[pageOffset]);	//修改,
	//返回修改后的状态
	if(ret == -1)
	{
		return HY_ERROR;
	}
	
	/* 回存到下一个page 中 */
	page ++;
	
	if(page > page_end)
	{
		/* 超界了，重新从前面写起 */
		page = page_start;
	}
	/* 注意下面的语句的顺序不能变，也不能简化*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*坏块标记*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*重新分配 blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* 分配不出来了，返回错误 */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 由于这里是新申请的block,需要重新定位page */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	if (ret == 1)//已传
	{
		ptFile->reportedRecords++;
	}	
	else if (ret == 2)//已删
	{
		ptFile->deletedRecords++; 
	}
	
	if(ptFile->deletedRecords == ptFile->totalRecords)	//如果全部删除,则删掉文件
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* 更新file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*函数：nandFile_changeRecordStatusMulti
*功能：修改记录状态
*      为了减少擦除次数，将存储abstract page 的 blk 扩大
*	   如果一个文件的abstract page 组最多是ABSTRACT_PAGE_NUM个，
*	   则使用前ABSTRACT_PAGE_NUM个blk 做为abstract page 的blk
*	   每次更新时，每个blk 仅更新一个page
*参数：fileIdx		-- 文件index号
*	   recordIdx	-- 记录index号
*	   modify_func	-- 函数指针，执行真正的修改
*返回：成功：HY_OK   
*	   失败：HY_ERROR
*------------------------------------------------------------*/
int nandFile_changeRecordStatusMulti(int fileIdx, int *recordIdx, int *num, MODIFY_STATUE_FUNC modify_func)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret,change;
	tFILE_ENTRY		*ptFile;
	
	//hyUsbPrintf("fileidx = %d num = %d \r\n",fileIdx,*num);
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
#ifdef NANDFLASH_DEBUG
	hyUsbPrintf("change state totalRecords = %d  reportedRecords = %d \r\n",ptFile->totalRecords, ptFile->reportedRecords);
#endif
	if(recordIdx[0] >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	blk			= recordIdx[0] / ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	//读出之前的记录摘要,只读当前需要改变的记录
	hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
	
	change = 0;
	for(i = 0; i < *num; i++)
	{
		pageOffset	= recordIdx[i] % ABSTRACT_NUM_PER_PAGE;
		
		ret = modify_func(&((tRECORD_STATUS *)pagebuf)[pageOffset]);	//修改,
		if(ret > 0)
		{
			change++;
		}
	}
	*num = change;
	
	if(change <= 0)
	{
		return HY_ERROR;
	}
	
	/* 回存到下一个page 中 */
	page ++;
	
	if(page > page_end)
	{
		/* 超界了，重新从前面写起 */
		page = page_start;
	}
	/* 注意下面的语句的顺序不能变，也不能简化*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*坏块标记*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*重新分配 blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* 分配不出来了，返回错误 */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 由于这里是新申请的block,需要重新定位page */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	if (ret == 1 || ret == -1)//已传
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("change = %d  total = %d  cur = %d \r\n", ptFile->reportedRecords, ptFile->totalRecords, change);
		#endif
		if(ptFile->reportedRecords+change > ptFile->totalRecords-ptFile->deletedRecords)
		{
			ptFile->reportedRecords = ptFile->totalRecords-ptFile->deletedRecords;
		}
		else
		{
			ptFile->reportedRecords+=change;
		}	
	}
	else if (ret == 2 || ret == -2)//已删
	{
		if(ptFile->deletedRecords+change > ptFile->totalRecords-ptFile->reportedRecords)
		{
			ptFile->deletedRecords = ptFile->totalRecords-ptFile->reportedRecords;
		}
		else
		{
			ptFile->deletedRecords+=change;
		}
	}
	
	if (ptFile->deletedRecords == ptFile->totalRecords)	//如果全部删除,则删掉文件
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* 更新file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}
