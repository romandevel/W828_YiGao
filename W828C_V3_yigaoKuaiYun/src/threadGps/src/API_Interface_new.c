#include "gps_ptypes.h"
#include "hyTypes.h"
#include "AbstractFileSystem.h"

#define FS_NO_ERR 1


/**********************************
 ** Function: Open uart          **
 ** Param: port - ID number      **
 ** 	   owner - Open mode     **
 ** Return: TRUE - Success       **
 **         FALSE - Failed       **
 **********************************/
int UART_Open(UART_PORT port, module_type owner)
{
	return 1;
}

/**********************************
 ** Function: Close uart         **
 ** Param: port - ID number      **
 **********************************/
void UART_Close (int Pport)
{
// close port;
	return;
}

/*****************************************
 ** Function: Send data to GPS by uart  **
 ** Param: port - ID number             **
 **        Buffaddr - TX Point          **
 ** 	   Length - Size of send data   **
 ** Return: length of sent data         **
 *****************************************/
int UART_PutBytes(int Pport, U1 *Buffaddr, U2 Length)
{
	int i;

	for(i=0; i<Length; i++)
	{
		if(0 != hyhwUart_WriteFifo(Buffaddr[i],Pport))
		{
			//PhoneTrace2(1,"uart write %d",i);
			return i;
		}
	}

	return i;
}

/*****************************************
 ** Function : Receive data from GPS    **
 ** Param: port - ID number             **
 **        Bufferaddr - RX Point        **
 ** Return: Length of received data     **
 *****************************************/
int UART_GetBytes(int Pport, U1 *Buffaddr)
{
	int getLen=0;
	getLen = Gps_ReadData(Buffaddr);
	return getLen;
}

void UART_SetDCBConfig (int Pport, UARTDCBStruct *DCB)
{
	return;
}

/*****************************************
 ** Function: Open file                 **
 ** Param: FileName - Name & Path       **
 **        Flags - Open mode            **
 ** Return: Fp - File point             **
 *****************************************/
FS_HANDLE FS_Open(const WCHAR * FileName, UINT Flags)
{
	FS_HANDLE Fp;

	Fp = AbstractFileSystem_Open(FileName,Flags);
	if(Fp < 0)//没有文件
	{
		Fp = AbstractFileSystem_Create(FileName);
	}
	else
	{
		//AbstractFileSystem_Seek(Fp,0,AFS_SEEK_OFFSET_FROM_END);
	}

	//PhoneTrace2(0,"gps log Fp = %d",Fp);
	return Fp;
}

/*****************************************
 ** Function: Close file                **
 ** Param: File - File point            **
 ** Return: Status                      **
 *****************************************/
int FS_Close(FS_HANDLE File)
{
	int status=FS_NO_ERR;

	#if 1
	if (1 == File)
	{
		char testStr[50];
		extern int testGpsRecvFlag;

		sprintf(testStr, "\r\nuart Over=%d\r\n",testGpsRecvFlag);
		AbstractFileSystem_Write(File, testStr, strlen(testStr));
	}
	#endif
	AbstractFileSystem_Close(File);

	return 0;
}

/*****************************************
 ** Function: Read file                 **
 ** Param: File - File point            **
 *****************************************/
int FS_Read(FS_HANDLE File, void * DataPtr, UINT Length, UINT * Read)
{
	int readLen = 0;
	
	readLen = AbstractFileSystem_Read(File,DataPtr,Length);
	
	*Read = readLen;
	
	return readLen;
}

/*****************************************
 ** Function: Write file                **
 ** Param: File - File point            **
 *****************************************/
int FS_Write(FS_HANDLE File, void * DataPtr, UINT Length, UINT * Written)
{
	int writeLen=0;

	writeLen = AbstractFileSystem_Write(File,DataPtr,Length);
	
	if(Written != NULL) *Written = writeLen; //0804
	
	return writeLen;
}

/*****************************************
 ** Function: Get tick from host        **
 ** Return: Ticks                       **
 *****************************************/
U4 get_tick_count()
{
	return (10*rawtime(NULL));
}

