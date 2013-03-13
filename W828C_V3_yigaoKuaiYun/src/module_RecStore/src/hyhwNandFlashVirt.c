/*
  提供出一个虚拟nand driver.
  
  目前专为韵达所用.
  目前使用的nand有三种,2K*128的和4K*128,2K*256的. 为便于上层管理,这里将2K*128和2K*256的虚拟封装为4K*128.
  具体做法:
  1.对涉及page和block操作的函数进行改写;
  2.改写时,合并2个block为1个,两个page连续来使用.
  
*/


#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"
#include "hyhwEcc.h"
#include "hyhwNandFlash.h"
#include "hyswNandFile.h"

#define VIR_PAGE_SIZE	2048	//2K

#define MIN_WRITE_UNIT		512

U8 gNandWrite[MIN_WRITE_UNIT];

/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_DataRead_Virt
* 功能:	从指定page的指定偏移读取指定大小的数据， 如果偏移量为0，则从page开始的地方
*		读数据，和原来的pageRead的函数的功能一样
* 参数:	data----指针参数，读取数据的buffer
*		page----指定的page号
*		offset--在指定page中的偏移
*		size----需要写入数据的长度（byte）
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
int hyhwNandFlash_DataRead_Virt(U8 *pdata, U32 page, U32 offset, U32 size)
{
	int ret, readLen;
	
	if(offset != 0)
	{
		readLen = VIR_PAGE_SIZE - offset;//第一个page可以读取的数据
		readLen = (readLen >= size)? size:readLen;
		
		ret = hyhwNandFlash_DataRead(pdata, page, offset, readLen);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		page ++;
		pdata += readLen;
		//还剩余需要读取的数据
		size -= readLen;
	}
	
	while(size > 0)
	{
		readLen = (size <= VIR_PAGE_SIZE) ? size : VIR_PAGE_SIZE;
		
		ret = hyhwNandFlash_DataRead(pdata, page, 0, readLen);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		page ++;
		pdata += readLen;
		size -= readLen;
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_Erase_Virt
* 功能:	将指定的block擦除
* 参数:	block----需要擦除的block号
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_Erase_Virt( U32 block )
{

	return hyhwNandFlash_Erase(block);
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwNandFlash_PageWrite_Virt
* 功能:	向指定的page中写入指定大小的数据,必须整页写
* 参数:	data----指针参数，需要写入数据
*		page----指定的page号
*		offset---page中的偏移
* 返回:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite_Virt( U8 *pdata, U32 page, U32 offset, U32 size )
{
	int ret, writeLen, left, off;
	int i,Num;
	
	//最小写单元 512 + 16字节ECC

	if(offset != 0)
	{
		memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
		
		writeLen = VIR_PAGE_SIZE - offset;//第一个page可以写入的数据
		writeLen = (writeLen >= size)? size:writeLen;
		
		off = offset % MIN_WRITE_UNIT;
		offset -= off;
				
		if(writeLen+off <= MIN_WRITE_UNIT)
		{
			memcpy(&gNandWrite[off],pdata,writeLen);
			ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
			if (ret == HY_ERROR)
			{
				return HY_ERROR;
			}
			page ++;
			pdata += writeLen;
			//还剩余需要写入的数据
			size -= writeLen;
		}
		else
		{
			memcpy(&gNandWrite[off],pdata,MIN_WRITE_UNIT-off);
			ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
			if (ret == HY_ERROR)
			{
				return HY_ERROR;
			}
			pdata   += (MIN_WRITE_UNIT-off);
			size    -= (MIN_WRITE_UNIT-off);
			offset  += MIN_WRITE_UNIT;
			writeLen-= (MIN_WRITE_UNIT-off);
			while(writeLen > 0)
			{
				if(writeLen <= MIN_WRITE_UNIT)
				{
					memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
					memcpy(gNandWrite,pdata,writeLen);
					ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
					if (ret == HY_ERROR)
					{
						return HY_ERROR;
					}
					pdata += writeLen;
					size  -= writeLen;
					offset+= writeLen;
					break;
				}
				else
				{
					ret = hyhwNandFlash_PageWrite_slc(pdata, page, offset, MIN_WRITE_UNIT);
					if (ret == HY_ERROR)
					{
						return HY_ERROR;
					}
					pdata   += MIN_WRITE_UNIT;
					size    -= MIN_WRITE_UNIT;
					offset  += MIN_WRITE_UNIT;
					writeLen-= MIN_WRITE_UNIT;
				}
			}
			page ++;
		}
	}
	
	Num = size / VIR_PAGE_SIZE;
	left= size % VIR_PAGE_SIZE;
	
	for(i = 0; i < Num; i++)
	{
		ret = hyhwNandFlash_PageWrite_slc(pdata, page, 0, VIR_PAGE_SIZE);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		pdata += VIR_PAGE_SIZE;
		page ++;
	}
	
	Num = left / MIN_WRITE_UNIT;
	left= left % MIN_WRITE_UNIT;
	
	if(Num > 0)
	{
		ret = hyhwNandFlash_PageWrite_slc(pdata, page, 0, Num*MIN_WRITE_UNIT);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		pdata += Num*MIN_WRITE_UNIT;
		offset = Num*MIN_WRITE_UNIT;
	}
	
	if(left > 0)
	{
		memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
		memcpy(gNandWrite,pdata,left);
		ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	
	return HY_OK;
}
