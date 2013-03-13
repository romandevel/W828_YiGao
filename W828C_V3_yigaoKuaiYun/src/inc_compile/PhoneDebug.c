#include "PhoneDebug.h"
#include "hyhwGpio.h"
#include "AbstractFileSystem.h"

extern U16  gLcd_Total_Row;
void PhoneTrace1(int level, const char *fmt,...)
{
#if 0	
	int len,i;
	char *str,buffer[128] = {0};
	va_list va;
	
	U16 row = 20;	//5
	U32 color = COLOR_RED;
	
	/*
		LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
	*/
	switch(level)
	{
		case LOG_CRITICAL:
			color = COLOR_RED;
			break;
		case LOG_ERR:
			color = COLOR_YELLOW;
			break;
		case LOG_NOTICE:
			color = COLOR_BLUE;
			break;
		case LOG_WARNING:
			color = COLOR_GREEN;
			break;
		case LOG_INFO:
			color = COLOR_WHITE;
			break;
		case LOG_DETAIL:
			color = COLOR_WHITE;
			break;
		case LOG_DEBUG:
			color = COLOR_WHITE;
		default:
			color = COLOR_WHITE;
			break;
	}
	
	
	drvLcd_SetColor(color,0x0000);
	
	
	va_start(va,fmt);	
	vsprintf(buffer,fmt,va);
	va_end(va);	
	
	len = strlen(buffer);
	
	for(i=0;len>0;len-=30,i+=30)
	{
		drvLcd_Background2DisplayMemory(16,240,row,0);
		//drvLcd_ClearBox(row,0,16,320);
		drvLcd_DisplayString(row,0,0,(unsigned char *)(buffer+i), NULL, 0);
		row += 16;
		if(row >=300) row = 0;
	}
	
	DisplayData2Screen();
#endif
}
void PhoneTrace(int level, const char *fmt,...)
{
	
#if PHONE_DEBUG > 0	
	int len,i;
	char *str,buffer[128] = {0};
	va_list va;
	
	static U16 row = 5;	//5
	U32 color = COLOR_RED;
	
 
	
	/*
		LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
	*/
	switch(level)
	{
		case LOG_CRITICAL:
			color = COLOR_RED;
			break;
		case LOG_ERR:
			color = COLOR_YELLOW;
			break;
		case LOG_NOTICE:
			color = COLOR_BLUE;
			break;
		case LOG_WARNING:
			color = COLOR_GREEN;
			break;
		case LOG_INFO:
			color = COLOR_WHITE;
			break;
		case LOG_DETAIL:
			color = COLOR_WHITE;
			break;
		case LOG_DEBUG:
			color = COLOR_WHITE;
		default:
			color = COLOR_WHITE;
			break;
	}
	
	
	drvLcd_SetColor(color,0x0000);
	
	va_start(va,fmt);	
	vsprintf(buffer,fmt,va);
	va_end(va);	
	
	len = strlen(buffer);
	
	for(i=0;len>0;len-=30,i+=30)
	{
		drvLcd_Background2DisplayMemory(16,240,row,0);
		//drvLcd_ClearBox(row,0,16,320);
		drvLcd_DisplayString(row,0,0,(unsigned char *)(buffer+i), NULL, 0);
		row += 16;
		if(row >= gLcd_Total_Row) row = 0;
	}
	
	DisplayData2Screen();
#endif
}
void PhoneTrace2(int level, const char *fmt,...)
{
#if 0	
	
	int len,i;
	char *str,buffer[128] = {0};
	va_list va;
	
	static U16 row = 5;	//5
	U32 color = COLOR_RED;
	

	
	/*
		LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
	*/
	switch(level)
	{
		case LOG_CRITICAL:
			color = COLOR_RED;
			break;
		case LOG_ERR:
			color = COLOR_YELLOW;
			break;
		case LOG_NOTICE:
			color = COLOR_BLUE;
			break;
		case LOG_WARNING:
			color = COLOR_GREEN;
			break;
		case LOG_INFO:
			color = COLOR_WHITE;
			break;
		case LOG_DETAIL:
			color = COLOR_WHITE;
			break;
		case LOG_DEBUG:
			color = COLOR_WHITE;
		default:
			color = COLOR_WHITE;
			break;
	}
	
	
	drvLcd_SetColor(color,0x0000);
	
	va_start(va,fmt);	
	vsprintf(buffer,fmt,va);
	va_end(va);	
	
	len = strlen(buffer);
	
	for(i=0;len>0;len-=30,i+=30)
	{
		drvLcd_Background2DisplayMemory(16,240,200,0);
		//drvLcd_ClearBox(row,0,16,320);
		drvLcd_DisplayString(200,0,0,(unsigned char *)(buffer+i), NULL, 0);
		//row += 16;
		//if(row >= gLcd_Total_Row) row = 0;
	}
	
	DisplayData2Screen();
#endif
}
void PhoneTrace3(int level, const char *fmt,...)
{
#if 0
	
	int len,i;
	char *str,buffer[128] = {0};
	va_list va;
	
	static U16 row = 5;	//5
	U32 color = COLOR_RED;
	

	
	/*
		LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
	*/
	switch(level)
	{
		case LOG_CRITICAL:
			color = COLOR_RED;
			break;
		case LOG_ERR:
			color = COLOR_YELLOW;
			break;
		case LOG_NOTICE:
			color = COLOR_BLUE;
			break;
		case LOG_WARNING:
			color = COLOR_GREEN;
			break;
		case LOG_INFO:
			color = COLOR_WHITE;
			break;
		case LOG_DETAIL:
			color = COLOR_WHITE;
			break;
		case LOG_DEBUG:
			color = COLOR_WHITE;
		default:
			color = COLOR_WHITE;
			break;
	}
	
	
	drvLcd_SetColor(color,0x0000);
	
	va_start(va,fmt);	
	vsprintf(buffer,fmt,va);
	va_end(va);	
	
	len = strlen(buffer);
	
	for(i=0;len>0;len-=30,i+=30)
	{
		drvLcd_Background2DisplayMemory(16,240,row,0);
		//drvLcd_ClearBox(row,0,16,320);
		drvLcd_DisplayString(row,0,0,(unsigned char *)(buffer+i), NULL, 0);
		row += 16;
		if(row >= gLcd_Total_Row) row = 0;
	}
	
	DisplayData2Screen();
#endif
}
void PhoneTrace4(int level, const char *fmt,...)
{
#if 0
	
	int len,i;
	char *str,buffer[128] = {0};
	va_list va;
	
	static U16 row = 20;	//5
	U32 color = COLOR_RED;
	

	
	/*
		LOG_CRITICAL = 0,
	LOG_ERR = 1,
	LOG_NOTICE = 2,
	LOG_WARNING = 3,
	LOG_INFO = 5,
	LOG_DETAIL = 6,
	LOG_DEBUG = 7
	*/
	switch(level)
	{
		case LOG_CRITICAL:
			color = COLOR_RED;
			break;
		case LOG_ERR:
			color = COLOR_YELLOW;
			break;
		case LOG_NOTICE:
			color = COLOR_BLUE;
			break;
		case LOG_WARNING:
			color = COLOR_GREEN;
			break;
		case LOG_INFO:
			color = COLOR_WHITE;
			break;
		case LOG_DETAIL:
			color = COLOR_WHITE;
			break;
		case LOG_DEBUG:
			color = COLOR_WHITE;
		default:
			color = COLOR_WHITE;
			break;
	}
	
	
	drvLcd_SetColor(color,0x0000);
	
	va_start(va,fmt);	
	vsprintf(buffer,fmt,va);
	va_end(va);	
	
	len = strlen(buffer);
	
	for(i=0;len>0;len-=30,i+=30)
	{
		drvLcd_Background2DisplayMemory(16,240,row,0);
		//drvLcd_ClearBox(row,0,16,320);
		drvLcd_DisplayString(row,0,0,(unsigned char *)(buffer+i), NULL, 0);
		row += 16;
		if(row >= 200) row = 20;
	}
	
	DisplayData2Screen();
#endif
}
void debug_string2screen(const char *str)
{
    drvLcd_Background2DisplayMemory(16,240,5,0); 	
	drvLcd_DisplayString(5,0,0,str, NULL, 0);	
	DisplayData2Screen();    
}
void test_add_file(U8 *name,U8 *print,int size)
{
	#if 0
	{
		int fp;		
		
		fp = AbstractFileSystem_Open(name,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(name,AFS_SINGLE_DELETE_FDM);
		}
		fp = AbstractFileSystem_Create(name);
		if(fp >= 0)
		{	
			AbstractFileSystem_Write(fp,print,size);			
			AbstractFileSystem_Close(fp);
		}
	}
	#endif
}
void test_write_log(U8 *name,U8 *print,int size)
{
	#if 0
	{
		int fp,sizefile;		
		
		fp = AbstractFileSystem_Open(name,AFS_RDWR);
		if(fp >= 0)
		{
			sizefile = AbstractFileSystem_FileSize(fp);
			AbstractFileSystem_Seek(fp,sizefile,0);
			AbstractFileSystem_Write(fp,"\r\n",strlen("\r\n"));
			AbstractFileSystem_Write(fp,print,size);			
			AbstractFileSystem_Close(fp);
		}
		else
		{
			fp = AbstractFileSystem_Create(name);
			if(fp >= 0)
			{	
				AbstractFileSystem_Write(fp,print,size);			
				AbstractFileSystem_Close(fp);
			}
		}
	}
	#endif
}

void test_write_log1(U8 *name,U8 *print,int size)
{
	#if 0
	{
		int fp,sizefile;		
		
		fp = AbstractFileSystem_Open(name,AFS_RDWR);
		if(fp >= 0)
		{
			sizefile = AbstractFileSystem_FileSize(fp);
			AbstractFileSystem_Seek(fp,sizefile,0);
			AbstractFileSystem_Write(fp," ",strlen(" "));
			AbstractFileSystem_Write(fp,print,size);			
			AbstractFileSystem_Close(fp);
		}
		else
		{
			fp = AbstractFileSystem_Create(name);
			if(fp >= 0)
			{	
				AbstractFileSystem_Write(fp,print,size);			
				AbstractFileSystem_Close(fp);
			}
		}
	}
	#endif
}