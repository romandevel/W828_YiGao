#include "string.h"

#include "gps_ptypes.h"
#include "GN_GPS_DataLogs.h"

#include "hyTypes.h"
#include "AbstractFileSystem.h"


//#define GPS_LOG


extern FS_HANDLE FS_Open(const WCHAR * FileName, UINT Flags);
extern int FS_Close (FS_HANDLE File);
extern int FS_Write(FS_HANDLE File, void * DataPtr, UINT Length, UINT * Written);

//*****************************************************************************
// Global data
static BL             g_Enable_Log[NUM_LOGS];   // Flag which logs are active
static FS_HANDLE	hFile_Log[NUM_LOGS];

//*****************************************************************************
// Open the GPS log files
void GN_Open_Logs(void)
{
   int  i;

   memset( g_Enable_Log, 0, sizeof(g_Enable_Log) );

 // Update 2007.7.4
   i = (int)NMEA_LOG;
#ifdef GPS_LOG
	hFile_Log[i] = FS_Open((const WCHAR *)"C:/gps_log/GN_NMEA_Log.TXT", AFS_RDWR);
	g_Enable_Log[i] = TRUE;
#else
	g_Enable_Log[i] = FALSE;
#endif

   i = (int)EVENT_LOG;
#ifdef GPS_LOG
	hFile_Log[i] = FS_Open((const WCHAR *)"C:/gps_log/GN_Event_Log.TXT", AFS_RDWR);
	g_Enable_Log[i] = TRUE;
#else
	g_Enable_Log[i] = FALSE;
#endif

   i = (int)ME_LOG;
#ifdef GPS_LOG
	hFile_Log[i] = FS_Open((const WCHAR *)"C:/gps_log/GN_ME_Log.TXT", AFS_RDWR);
	g_Enable_Log[i] = TRUE;
#else
	g_Enable_Log[i] = FALSE;
#endif	
   i = (int)NAV_LOG;
#ifdef GPS_LOG
	hFile_Log[i] = FS_Open((const WCHAR *)"C:/gps_log/GN_Nav_Log.TXT", AFS_RDWR);
	g_Enable_Log[i] = TRUE;
#else
	g_Enable_Log[i] = FALSE;
#endif
	
   return;
}

//*****************************************************************************
// Close the GPS log files
void GN_Close_Logs(void)
{
   int i;

   // Close each of the log files
   for ( i = 0 ; i < NUM_LOGS ; i++ )
   {
      if ( g_Enable_Log[i] == TRUE && hFile_Log[i] >= 0)
      {
			FS_Close(hFile_Log[i]);
			//PhoneTrace2(0,"gps log close %d ok",i);
      }
   }

   return;
}

//*****************************************************************************
// Write data to the specified log file
// Write data to the specified log file
void Write_Data_To_Log(
       e_Data_Log  log,          // Data log type
       U2          num_bytes,    // Number of bytes to Write
       CH          *p_data )     // Pointer to the data
{
	UINT	WriteCnt;
	int  len;
   if ( g_Enable_Log[log] == TRUE && hFile_Log[log] >= 0)
   {
		len = FS_Write(hFile_Log[log], (void *) p_data, (UINT) num_bytes, &WriteCnt);
   }

   return;
}
//*****************************************************************************


