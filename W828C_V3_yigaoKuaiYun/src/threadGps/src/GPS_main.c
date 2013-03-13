
//*************************************************************************
// Start of GloNav GPS code
//*************************************************************************

#include "gps_ptypes.h"
#include "gps.h"
#include "GN_GPS_api.h"
#include "GN_GPS_Example.h"
#include "GN_GPS_DataLogs.h"

//*****************************************************************************
// Function prototypes external to this file
//extern int fdr;
//extern int fdw;
//extern int len;

extern U1           patch_status;       // Status of GPS baseband patch transmission
extern U2           cur_mess[5];        // Current messages to send, of each 'stage'

//*****************************************************************************
// Global variables
U1           UARTrx_buffer[UART_RX_BUF_SIZE];   // UART data circular buffer (to be given to GPS)
s_CircBuff   UARTrx_cb;                         // UART Rx circular buffer
U1           UARTtx_buffer[UART_TX_BUF_SIZE];   // UART data circular buffer (to be given to bb)
s_CircBuff   UARTtx_cb;                         // UART Tx circular buffer
module_type Port_ID;
int DevCOMID;
/********************************************************************
 * The following is some function for UART Communication 			*
 * Update 2007.7.4													*
 ********************************************************************/
extern int UART_Open(UART_PORT port, module_type owner);
extern void UART_SetDCBConfig (int Pport, UARTDCBStruct *DCB);
extern void GPS_Configure(void);
extern U1 UART_Input_Task(void);
extern void GPS_Task(void);
extern void UART_Output_Task(void);
extern void GN_GPS_UploadPatch( void );
extern int GPS_PowerON();
extern int GPS_PowerDOWN();

extern void Read_UTC_File();
extern void Saving_UTC_File();

void GN_GPS_Thread(void);

U1 UART_Init()
{
	// Initialise the i/o circular buffers.
	// (NB. end_buf points to the next byte after the buffer)
	UARTrx_cb.start_buf = &UARTrx_buffer[0];
	UARTrx_cb.end_buf   = UARTrx_cb.start_buf + UART_RX_BUF_SIZE;
	UARTrx_cb.read      = UARTrx_cb.start_buf;
	UARTrx_cb.write     = UARTrx_cb.start_buf;
	
	UARTtx_cb.start_buf = &UARTtx_buffer[0];
	UARTtx_cb.end_buf   = UARTtx_cb.start_buf + UART_TX_BUF_SIZE;
	UARTtx_cb.read      = UARTtx_cb.start_buf;
	UARTtx_cb.write     = UARTtx_cb.start_buf;
	
#if 0
	UARTDCBStruct *DCB1;
	char COM_Info[20];
	
	DCB1 = (UARTDCBStruct *)COM_Info;
	

	
	// Open the UART
	DevCOMID = UART_Open(uart_port2, Port_ID);
	if (DevCOMID < 0)
	{
		return FALSE; // Open is failed.
	}
	
	DCB1->baud = 115200;
	DCB1->dataBits = 8;
	DCB1->stopBits = 1;
	DCB1->parity = 0;
	DCB1->flowControl = 0;
	
	UART_SetDCBConfig (DevCOMID, DCB1);
#endif	
	return TRUE;
}
//****************************************************************************
U1 GN_GPS_Init()
{
	UART_Init();
//	GPS_Init_ROM506();
//	 if ( == FALSE)
//	 	{
//	 		return FALSE;
//	 	}
   // Open the Data Log files
	GN_Open_Logs();
	Read_UTC_File();
   // Initialise the patch code upload parameters
   patch_status = 0;
   cur_mess[0]  = 0;
   cur_mess[1]  = 0;
   cur_mess[2]  = 0;
   cur_mess[3]  = 0;
   cur_mess[4]  = 0;
   
   // Initialise the GPS software. This API function MUST be called.
   GN_GPS_Initialise();
   // Setup the GPS configuration. This is not a GPS API function call.
   // This is optional.
	GPS_Configure();
	//GPS_PowerON();
	return TRUE;
}

void GN_GPS_Exit()
{
	//GPS_PowerDOWN();
	GN_GPS_Shutdown();
	
	Saving_UTC_File();
	// Close the Data Log files
   	GN_Close_Logs();
   	//UART_Close(GPS_UART_PORT);
}
int testGpsStatus = 0;
void GN_GPS_Thread(void)
{
	char Status_Task;
	s_GN_GPS_Nav_Data  Nav_Data;       // The latest Nav solution

	Status_Task = UART_Input_Task();
	//PhoneTrace2(0,"gps Task = %d",Status_Task);  
	if (Status_Task == TRUE)
	{
		GPS_Task();
   		#if 0
   		{
			U2 num;
			CH temp_buffer[64];
			num = sprintf( temp_buffer,
			            "GN_GPS_Thread Patch status = %d.\r\n", patch_status);
			GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );

			//GN_GPS_Write_NMEA(5,"123\r\n");
		}
		#endif
		if ( GN_GPS_Get_Nav_Data( &Nav_Data ) == TRUE )
		{
			// A new fix has just been generated, so send the control
			// data to the baseband
			UART_Output_Task();
			// Output information, just for visibility
			#if 1
				if ( Nav_Data.Valid_3D_Fix == TRUE )
				{
					//sprintf("3D fix\n");
					//PhoneTrace2(0,"3D fix");
					testGpsStatus = 3;
				}
				else if ( Nav_Data.Valid_2D_Fix == TRUE )
				{
					//sprintf("2D fix\n");
					//PhoneTrace2(0,"2D fix");
					testGpsStatus = 2;
				}
				//else
					//sprintf("Invalid fix\n");
			#endif 
		}
	}
	if ( (patch_status > 0) && (patch_status < 7) )
	{
		GN_GPS_UploadPatch();
	}
}

//*************************************************************************
// End of GloNav GPS code ...
//*************************************************************************
#if 0
__align(32) int main(int argc, char *argv[]) // just example
{

	GN_GPS_Init();

	while (1)
	{
		GN_GPS_Thread();
	}
		
	GN_GPS_Exit();
	return 0;
}
#endif

