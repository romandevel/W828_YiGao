#include "hyTypes.h"
#include "assnApi.h"   
#include "hyOsCpuCfg.h"
#include "hyErrors.h"

#include "AbstractFileSystem.h"
#include "edit.h"

#if 0
#define EI_COMPRESS_CH_FILE_NAME		("C:/系统/EI/dictionary.bin")
#define EI_COMPRESS_CH_MAX_OUT_NUM		(35)
#define EI_COMPRESS_CH_READ_SIZE		(EI_COMPRESS_CH_MAX_OUT_NUM*2)//35个汉字
/*---------------------------------------------------------------------------
* 函数: EI_getNextWord
* 功能: 根据输入的GBK汉字，找到与该汉字能组成词语的汉字
* 		最大返回EI_COMPRESS_CH_MAX_OUT_NUM个汉字，EI_COMPRESS_CH_READ_SIZE字节
* 参数: InValue-------GBK汉字code
*		pOutValue-----读取到的相关汉字字符
* 返回：<=0------失败
*		>0------实际读取到的汉字字符数*2
* -----------------------------------------------------------------------------*/
static int EI_getNextWord(char *InStr, char *pOutStr, int times)
{
	char *pOut = pOutStr;
	int fileHandle, fileSize, readSize, ChNum;
	int offset, w_offset;
	int high, low, i, len, ret;
	__align(4) char pbuffer[EI_COMPRESS_CH_READ_SIZE+1];

	fileHandle = AbstractFileSystem_Open(EI_COMPRESS_CH_FILE_NAME, AFS_READ);
	if (fileHandle < 0) return -1;

	low = (InStr[1] & 0xFF) - 0x40;
	high = (InStr[0] & 0xFF) - 0x81;
	if ((low<0x00) || (low>0xBF) || (high<0x00) || (high>0x7E))
	{
		AbstractFileSystem_Close(fileHandle);
		return -1;
	}
	offset = (low + high * (0xFF - 0x40)) * 6;

	AbstractFileSystem_Seek(fileHandle, offset, AFS_SEEK_OFFSET_FROM_START);
	readSize = AbstractFileSystem_Read(fileHandle, pbuffer, 6);
	w_offset = *(int *)pbuffer;
	ChNum = *(short *)(pbuffer+4);

	if ((ret = ChNum - (times - 1) * EI_COMPRESS_CH_MAX_OUT_NUM) > 0)
	{
		readSize = MIN(ret * 2, EI_COMPRESS_CH_READ_SIZE);
		w_offset += (times - 1) * EI_COMPRESS_CH_READ_SIZE;	
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		readSize = AbstractFileSystem_Read(fileHandle, pOutStr, readSize);
		//memcpy(pOutStr, pbuffer, readSize);
		pOut += readSize;
		ret = readSize;
		readSize = EI_COMPRESS_CH_READ_SIZE - readSize;
		w_offset = 0;
	}
	else
	{
		readSize = EI_COMPRESS_CH_READ_SIZE;
		w_offset = -ret * 2;
		ret = 0;
	}
	
	if (readSize > 0)
	{
		AbstractFileSystem_Seek(fileHandle, 24066 * 6, AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fileHandle, pbuffer, 4);
		w_offset += *(int *)pbuffer;
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		memset(pOut, 0, readSize + 1);
		len = AbstractFileSystem_Read(fileHandle, pOut, readSize);
		ret += len;
	}
	
	AbstractFileSystem_Close(fileHandle);

	return ret;
}
#else
#define EI_COMPRESS_CH_FILE_NAME		("C:/系统/EI/dictionary.bin")
#define EI_COMPRESS_CH_MAX_OUT_NUM		(30)
#define EI_COMPRESS_CH_READ_SIZE		(EI_COMPRESS_CH_MAX_OUT_NUM*2)//35个汉字
/*---------------------------------------------------------------------------
* 函数: EI_getNextWord
* 功能: 根据输入的GBK汉字，找到与该汉字能组成词语的汉字
* 		最大返回EI_COMPRESS_CH_MAX_OUT_NUM个汉字，EI_COMPRESS_CH_READ_SIZE字节
* 参数: InValue-------GBK汉字code
*		pOutValue-----读取到的相关汉字字符
* 返回：<=0------失败
*		>0------实际读取到的汉字字符数*2
* -----------------------------------------------------------------------------*/
//重要修改　090506　第一次读取３５个字以后读取３０个字
static int EI_getNextWord(char *InStr, char *pOutStr, int times)
{
	char *pOut = pOutStr;
	int fileHandle, fileSize, readSize, ChNum;
	int offset, w_offset;
	int high, low, i, len, ret;
	int willReadSize;
	__align(4) char pbuffer[EI_COMPRESS_CH_READ_SIZE+11];

	fileHandle = AbstractFileSystem_Open(EI_COMPRESS_CH_FILE_NAME, AFS_READ);
	if (fileHandle < 0) return -1;

	low = (InStr[1] & 0xFF) - 0x40;
	high = (InStr[0] & 0xFF) - 0x81;
	if ((low<0x00) || (low>0xBF) || (high<0x00) || (high>0x7E))
	{
		AbstractFileSystem_Close(fileHandle);
		return -1;
	}
	offset = (low + high * (0xFF - 0x40)) * 6;

	AbstractFileSystem_Seek(fileHandle, offset, AFS_SEEK_OFFSET_FROM_START);
	readSize = AbstractFileSystem_Read(fileHandle, pbuffer, 6);
	w_offset = *(int *)pbuffer;
	ChNum = *(short *)(pbuffer+4);
	
	if (times == 1)
	{
	    ret = ChNum;
    }
    else
    {
        ret = ChNum - (EI_COMPRESS_CH_MAX_OUT_NUM+5)-(times - 2) * EI_COMPRESS_CH_MAX_OUT_NUM;

    }
    
	if (ret > 0)
	{
		readSize = MIN(ret * 2, EI_COMPRESS_CH_READ_SIZE);
		
		//w_offset += (times - 1) * EI_COMPRESS_CH_READ_SIZE;
		if (times != 1)
		{
		     w_offset = w_offset + (EI_COMPRESS_CH_READ_SIZE + 10) + (times - 2) * EI_COMPRESS_CH_READ_SIZE;	
		} 		
		
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		readSize = AbstractFileSystem_Read(fileHandle, pOutStr, readSize);
		pOut += readSize;
		ret = readSize;
		
		if (times == 1)
		{
		    readSize = EI_COMPRESS_CH_READ_SIZE+10 - readSize; 
		}
		else
		{
		    readSize = EI_COMPRESS_CH_READ_SIZE - readSize; 
		}		
		
		w_offset = 0;
	}
	else
	{
		if (times == 1)
		{
		    readSize = EI_COMPRESS_CH_READ_SIZE+10; 
		}
		else
		{
		    readSize = EI_COMPRESS_CH_READ_SIZE;
		}
		  		
		w_offset = -ret * 2;
		ret = 0;
	}
	
	if (readSize > 0)
	{
		AbstractFileSystem_Seek(fileHandle, 24066 * 6, AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fileHandle, pbuffer, 4);
		w_offset += *(int *)pbuffer;
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		memset(pOut, 0, readSize + 1);
		len = AbstractFileSystem_Read(fileHandle, pOut, readSize);
		ret += len;
	}
	
	AbstractFileSystem_Close(fileHandle);

	return ret;
}
#endif

/*---------------------------------------------------------------------------
* 函数: EI_getAssociateWord
* 功能: 读取联想字库
* 参数: pAssn-------读取次数，从1开始
* 返回：正常 HY_OK  其他 HY_ERROR
* -----------------------------------------------------------------------------*/
int EI_getAssociateWord(tASSN_CTR *pAssn)
{
    int rv = HY_OK;
    int len;

    len = EI_getNextWord(&pAssn->lastWord, pAssn->assnBuf,pAssn->times+1);
   
    if (len > 0)
    {
         pAssn->validTotal = len;
    }
    else
    {
         pAssn->validTotal = 0;
         rv = HY_ERROR;
    }
    
    return rv;
    
}
