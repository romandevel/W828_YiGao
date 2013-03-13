#include "std.h"
#include "AbstractFileSystem.h"
#include "stdlib.h"
#include "hal.h"
#include "InputRange_api.h"


//#define USE_IMGFILE



int CooTekReadStart = COOTEK_START;
int CooTekReadStart_User;
void *pImpTem=NULL;
U32	gu_CootekStart;
U32 gu_CootekEnd;


const char UserFileBuff[] = {0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
							 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
							 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
							 

IMAGE_HANDLE ext_fopen(const char* filename, const char* mode)
{
	//hyUsbPrintf1("ex_fopen\r\n");
#ifdef USE_IMGFILE
	return AbstractFileSystem_Open(filename, AFS_RDWR);//////////
#else
	CooTekReadStart = gu_CootekStart;
	return 1;
#endif
}

size_t ext_fread(void* buffer, size_t size, size_t count, IMAGE_HANDLE fHandle)
{
#ifdef USE_IMGFILE
	size_t ret;
	//hyUsbPrintf1("read file len=%d\r\n", (size*count));
	ret = AbstractFileSystem_Read(fHandle, buffer, (size*count));
	//hyUsbPrintf1("read len=%d\r\n", ret);
	return ret;
#else
	int tempSize, temp;;
	
	tempSize = size*count;
	if (tempSize < 0)
	{
		return -3;
	}
	
	//hyUsbPrintf1("##readfile## fd=%d   read file len=%x\r\n", fHandle, (size*count));
	
	if (fHandle == 0)
	{
		if (CooTekReadStart >= gu_CootekEnd)
		{
			return -3;
		}
		if ((CooTekReadStart+tempSize) > gu_CootekEnd)
		{
			tempSize = gu_CootekEnd - CooTekReadStart;
		}
		
		HSA_AccessReservedArea(buffer, CooTekReadStart, tempSize);
		CooTekReadStart += tempSize;
		
		return tempSize;
	}
	else if (fHandle == 1)
	{
		if (CooTekReadStart_User >= COOTEK_END_USER)
		{
			return -3;
		}
		
		if (tempSize >= COOTEK_END_USER)
		{
			temp = CooTekReadStart_User;
			memcpy(buffer, &UserFileBuff[CooTekReadStart_User], COOTEK_END_USER-CooTekReadStart_User);
			CooTekReadStart_User = COOTEK_END_USER;
			return COOTEK_END_USER-temp;
		}
		else 
		{
			temp = CooTekReadStart_User;
			if (CooTekReadStart_User+tempSize >= COOTEK_END_USER)
			{
				memcpy(buffer, &UserFileBuff[CooTekReadStart_User], COOTEK_END_USER-CooTekReadStart_User);
				CooTekReadStart_User = COOTEK_END_USER;
				return COOTEK_END_USER-temp;
			}
			else
			{
				memcpy(buffer, &UserFileBuff[CooTekReadStart_User], tempSize);
				CooTekReadStart_User+=tempSize;
				return tempSize;
			}
		}
	}
#endif
}

int ext_fclose(IMAGE_HANDLE fHandle)
{
	//hyUsbPrintf1("ext_fclose\r\n");
#ifdef USE_IMGFILE
	return AbstractFileSystem_Close(fHandle);
#else
	if (fHandle == 0)
		CooTekReadStart = gu_CootekStart;
	else if (fHandle == 1)
		CooTekReadStart_User = COOTEK_START_USER;
	return 0;
#endif
}


size_t ext_fwrite(void* buffer, size_t size, size_t count, IMAGE_HANDLE fHandle)// keep empty
{
	//hyUsbPrintf1("ext_fwrite=%d\r\n", (size * count));
#ifdef USE_IMGFILE
	return AbstractFileSystem_Write(fHandle, buffer, size * count);
#else
	return (size * count);
#endif
}


int ext_fseek(IMAGE_HANDLE stream, long offset, int origin)// keep empty
{
#ifdef USE_IMGFILE
	int tempOffset;
	tempOffset = AbstractFileSystem_Seek(stream, (int)offset, origin);
	//hyUsbPrintf1("ext_fseek:%d\r\n", tempOffset);
	return tempOffset;
#else
	int tempOffset;
	
	//hyUsbPrintf1("ext_fseek fd=%d\r\n", stream);
	
	if (stream == 0)
	{
		tempOffset = CooTekReadStart;
		if (origin == AFS_SEEK_OFFSET_FROM_START)
		{
			CooTekReadStart = gu_CootekStart+offset;
		}
		else if (origin == AFS_SEEK_OFFSET_FROM_CURRENT)
		{
			CooTekReadStart += offset;
		}
		else if (origin == AFS_SEEK_OFFSET_FROM_END)
		{
			CooTekReadStart = gu_CootekEnd+offset;
		}
	
		if (CooTekReadStart > gu_CootekEnd)
			CooTekReadStart = gu_CootekEnd;
		if (CooTekReadStart < gu_CootekStart)
			CooTekReadStart = gu_CootekStart;
			
			//hyUsbPrintf1("0---ext_fseek:%x\r\n", (CooTekReadStart-gu_CootekStart));
	
	return (CooTekReadStart-gu_CootekStart);
	}
	else if (stream == 1)
	{
		tempOffset = CooTekReadStart_User;
		if (origin == AFS_SEEK_OFFSET_FROM_START)
		{
			if (offset > COOTEK_END_USER)
			{
				CooTekReadStart_User = COOTEK_END_USER;
			}
			else
			{
				CooTekReadStart_User = COOTEK_START_USER+offset;
			}
		}
		else if (origin == AFS_SEEK_OFFSET_FROM_CURRENT)
		{
			if (CooTekReadStart_User + offset > COOTEK_END_USER)
			{
				CooTekReadStart_User = COOTEK_END_USER;
			}
			else
			{
				CooTekReadStart_User += offset;
			}
		}
		else if (origin == AFS_SEEK_OFFSET_FROM_END)
		{
			CooTekReadStart_User += offset;
		}
		
		if (CooTekReadStart_User > COOTEK_END_USER)
			CooTekReadStart_User = COOTEK_END_USER;
		if (CooTekReadStart_User < COOTEK_START_USER)
			CooTekReadStart_User = COOTEK_START_USER;
		
		//hyUsbPrintf1("1---ext_fseek:%d\r\n", (CooTekReadStart_User-COOTEK_START_USER));
		
		return (CooTekReadStart_User-COOTEK_START_USER);
	}
#endif
}


int ext_fflush(IMAGE_HANDLE stream)// keep empty
{
	return 0;
}


char* ext_fgets(char* string, int n, IMAGE_HANDLE stream)// keep empty
{
	return 0;
}


int ext_ftell(IMAGE_HANDLE fd)
{
	int ret;
#ifdef USE_IMGFILE
	ret = AbstractFileSystem_Seek(fd, 0, AFS_SEEK_OFFSET_FROM_CURRENT);
#else
	//hyUsbPrintf1("ext_ftell=%d\r\n", fd);
	if (fd == 0)
		ret = (CooTekReadStart-gu_CootekStart);
	else if (fd == 1)
		ret = CooTekReadStart_User - COOTEK_START_USER;
#endif
	//hyUsbPrintf1("ext_ftell=%d\r\n", ret);
	return ret;
}



/*
打印发现该函数调用时size情况
0x4c22c
0x4aa89e
0x40000
0x7ffc
0x28030
0x8

最大的一个要占用0x4aa89e
*/
void* ext_malloc(size_t size)
{
	char *p=NULL;
	U32 len;

	if (size >0x100000 )
	{
		p = (char *)hsaSdram_InputBuf();
		len = hsaSdram_InputSize();
		pImpTem = (void *)p;
		
		return (void *)p;
	}
	p = (void *)malloc(size);
		
	return p;
	
	return malloc(size);
}
void ext_free(void* memblock)
{
	if(NULL == memblock)return ;
	
	if (pImpTem && memblock && memblock == pImpTem)
	{
		pImpTem =NULL;
		//hyUsbPrintf1("not need free\r\n");
	}
	else
	{
		free(memblock);
	}
}

void* ext_memset(void* dest, int c, size_t count)
{
	return memset(dest, c, count);
}

void* ext_memcpy(void* dest, const void* src, size_t count)
{
	return memcpy(dest, src, count);
}


