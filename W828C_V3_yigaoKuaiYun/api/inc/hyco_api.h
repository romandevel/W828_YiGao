#ifndef _HYCO_API_H
#define _HYCO_API_H

#include "hyTypes.h"







/*++++++++++++++++++++++++++++++++++++++++ 硬件相关 API fun ++++++++++++++++++++++++++++++++++++++++++++*/
/*==============================================１．电源相关控制============================================*/
/*---------------------------------------
* 函数:Power_SetBattAutoOffTimer
* 功能:自动关机时间的控制。 如果 timeStr 不为NULL,则以该时间作为关键时间, 否则以delaySecond延迟做关机时间
* 参数:timeStr:时间字符串, 格式:yyyymmddhhmmss (20121202121000)
       delaySecond:从当前时间开始计算延迟秒数
* 返回:0:设置成功    -1:失败
*----------------------------------------*/
int Power_SetBattAutoOffTimer(char *timeStr, unsigned int delaySecond);


/*---------------------------------------
* 函数:Power_DisablePOff
* 功能:电源键禁止使用
* 参数:none
* 返回:none
*----------------------------------------*/
void Power_DisablePOff();


/*---------------------------------------
* 函数:Power_EnablePOff
* 功能:电源键允许使用
* 参数:none
* 返回:none
*----------------------------------------*/
void Power_EnablePOff();

/*---------------------------------------
* 函数:Power_GetVoltageLevel
* 功能:电量的取得
* 参数:none
* 返回:0~4:0格到4格   5:电量满
*----------------------------------------*/
U32 Power_GetVoltageLevel();

/*---------------------------------------
* 函数:Power_
* 功能:电池图标的控制
* 参数:none
* 返回:none
*----------------------------------------*/
//void Power_();

/*===========================================２．ＬＣＤ控制(很重要需要提供尽量详细的API)=========================================*/
/*---------------------------------------
* 函数:LCD_OpenBacklight
* 功能:打开显示屏背光
* 参数:none
* 返回:none
*----------------------------------------*/
void LCD_OpenBacklight();

/*---------------------------------------
* 函数:LCD_CloseBacklight
* 功能:关闭显示屏背光
* 参数:none
* 返回:none
*----------------------------------------*/
void LCD_CloseBacklight();

/*---------------------------------------
* 函数:LCD_SetBacklightGrade
* 功能:设置显示屏背光亮度等级
* 参数:grade:亮度等级  0~4级
* 返回:none
*----------------------------------------*/
void LCD_SetBacklightGrade(unsigned char grade);

//to do.......

/*===========================================3．状态栏的控制=========================================*/
/*---------------------------------------
* 函数:JT_SetIMEState
* 功能:设置输入法状态
* 参数:state:0:关闭  1:123  2:abc  3:ABC  4:手写  5:拼音
* 返回:none
*----------------------------------------*/
void JT_SetIMEState(unsigned char state);

/*===========================================4．操作的控制(很重要需要提供尽量详细的API)=========================================*/
/*---------------------------------------
* 函数:
* 功能:按键键值的取得和控制
* 参数:none
* 返回:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* 函数:
* 功能:按键输入相关
* 参数:none
* 返回:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* 函数:
* 功能:触摸控制相关
* 参数:none
* 返回:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* 函数:
* 功能:涉及到按键输入和按键相应相关的API
* 参数:none
* 返回:none
*----------------------------------------*/
//void ();

/*===========================================5．LED 控制=========================================*/
/*---------------------------------------
* 函数:LED_Control
* 功能:提示灯颜色
* 参数:color: 0:关闭  1:红  2:绿  4:蓝  3:黄  5:品红  6:青  7:白
       times:闪烁次数
* 返回:none
*----------------------------------------*/
void LED_Control(int color, int times);

/*===========================================6．蜂鸣控制=========================================*/
/*---------------------------------------
* 函数:Buzzer_On
* 功能:启动蜂鸣器
* 参数:beepType: 声音类型 0:滴  6:滴滴滴滴 
       ledType:LED类型 0:关闭  1:红  2:绿  4:蓝  3:黄  5:品红  6:青  7:白
* 返回:none
*----------------------------------------*/
void Buzzer_On(int beepType, int ledType);

/*---------------------------------------
* 函数:Buzzer_Off
* 功能:停止蜂鸣器
* 参数:none
* 返回:none
*----------------------------------------*/
void Buzzer_Off();
		
/*===========================================7．震动控制=========================================*/
/*---------------------------------------
* 函数:VIB_On
* 功能:启动
* 参数:millisecond:震动时间 单位 ms
* 返回:none
*----------------------------------------*/
void VIB_On(unsigned int millisecond);

/*---------------------------------------
* 函数:VIB_Off
* 功能:停止
* 参数:none
* 返回:none
*----------------------------------------*/
void VIB_Off();

/*===========================================8．扫描控制=========================================*/
/*---------------------------------------
* 函数:Scan_InitEx
* 功能:扫描设备的初始化
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_InitEx();

/*---------------------------------------
* 函数:Scan_DeinitEx
* 功能:扫描设备的关闭
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_DeinitEx();

/*---------------------------------------
* 函数:Scan_EnableOn
* 功能:开扫描 (打开激光)
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_EnableOn();

/*---------------------------------------
* 函数:Scan_EnableOff
* 功能:关扫描 (关闭激光)
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_EnableOff();

/*---------------------------------------
* 函数:Scan_SetScannerParam
* 功能:扫描相关参数的设置，按键动作、扫描方式、条码类型、checkdigt等等。
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_SetScannerParam();

/*---------------------------------------
* 函数:Scan_GetScannerParam
* 功能:扫描相关参数的取得。
* 参数:none
* 返回:none
*----------------------------------------*/
void Scan_GetScannerParam();

/*===========================================9．Bluetooth通讯控制=========================================*/
/*---------------------------------------
* 函数:BLT_PowerOn
* 功能:蓝牙设备的启动。 
* 参数:none
* 返回:none
*----------------------------------------*/
void BLT_PowerOn();	

/*---------------------------------------
* 函数:BLT_PowerOff
* 功能:蓝牙设备的关闭。  
* 参数:none
* 返回:none
*----------------------------------------*/
void BLT_PowerOff();	

//Master模式整个通信过程涉及到的函数，以及相关参数的设置和取得
//Slave模式整个通信过程涉及到的函数，以及相关参数的设置和取得
			
/*===========================================10．硬件相关信息的取得=========================================*/
//类似操作系统版本，产品序列号等等
			
/*===========================================11．WIFI和GPRS的通讯相关=========================================*/
//wifi连接的获取及选择	
//wifi与GPRS通讯模式的互相转换



/*++++++++++++++++++++++++++++++++++++++++ 软件相关 API fun ++++++++++++++++++++++++++++++++++++++++++++*/
/*===========================================１．TCP通信WinsockAPI=========================================*/


	/*１．TCP通信WinsockAPI		
	No. 	函数名称 	功能
	1	socket	The socket function creates a socket that is bound to a specific transport service provider.
	2	connect	The connect function establishes a connection to a specified socket.
	3	send	The send function sends data on a connected socket.
	4	recv	The recv function receives data from a connected or bound socket.
	5	shutdown	The shutdown function disables sends or receives on a socket.
	6	closesocket	The closesocket function closes an existing socket.
	7	WSAStartup	The WSAStartup function initiates use of the Winsock DLL by a process.
	8	WSACleanup	The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
	9	WSAGetLastError	The WSAGetLastError function returns the error status for the last Windows Sockets operation that failed.
	10	select	The select function determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
	11	htons	The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
	*/		
/*===========================================２．输入法相关API=========================================*/

/*	２．输入法相关API		
	No. 	函数名称 	功能
	1	ImmSetOpenStatus	Opens or closes the IME.
	2	ImmSetConversionStatus	Sets the current conversion status.
	3	ImmSetCompositionFont	Sets the logical font to use to display characters in the composition window.
	4	ImmReleaseContext	Releases the input context and unlocks the memory associated in the input context. An application must call this function for each call to the ImmGetContext function.
*/

/*===========================================3．画面显示相关API=========================================*/
			
/*	3．画面显示相关API		
	No. 	函数名称 	功能
	1	SelectObject	The SelectObject function selects an object into the specified device context (DC). The new object replaces the previous object of the same type. 
	2	GetStockObject	The GetStockObject function retrieves a handle to one of the stock pens, brushes, fonts, or palettes.
	3	SetRectEmpty	The SetRectEmpty function creates an empty rectangle in which all coordinates are set to zero.
	4	SetBkColor	The SetBkColor function sets the current background color to the specified color value, or to the nearest physical color if the device cannot represent the specified color value. 
	5	ExtTextOut	The ExtTextOut function draws text using the currently selected font, background color, and text color. You can optionally provide dimensions to be used for clipping, opaquing, or both. 
	6	GetCurrentObject	The GetCurrentObject function retrieves a handle to an object of the specified type that has been selected into the specified device context (DC). 
	7	SetTextColor	The SetTextColor function sets the text color for the specified device context to the specified color.
	8	DrawText	The DrawText function draws formatted text in the specified rectangle. It formats the text according to the specified method (expanding tabs, justifying characters, breaking lines, and so forth). 
	9	BitBlt	The BitBlt function performs a bit-block transfer of the color data corresponding to a rectangle of pixels from the specified source device context into a destination device context.
	10	DeleteObject	The DeleteObject function deletes a logical pen, brush, font, bitmap, region, or palette, freeing all system resources associated with the object. After the object is deleted, the specified handle is no longer valid. 
	11	GetDC	The GetDC function retrieves a handle to a display device context (DC) for the client area of a specified window or for the entire screen. You can use the returned handle in subsequent GDI functions to draw in the DC. 
	12	CreateCompatibleDC	The CreateCompatibleDC function creates a memory device context (DC) compatible with the specified device. 
	13	CreateCompatibleBitmap	The CreateCompatibleBitmap function creates a bitmap compatible with the device that is associated with the specified device context. 
	14	ReleaseDC	The ReleaseDC function releases a device context (DC), freeing it for use by other applications. The effect of the ReleaseDC function depends on the type of DC. It frees only common and window DCs. It has no effect on class or private DCs. 
	15	GetDeviceCaps	The GetDeviceCaps function retrieves device-specific information for the specified device. 
	16	CreateSolidBrush	The CreateSolidBrush function creates a logical brush that has the specified solid color. 
	17	FillRect	The FillRect function fills a rectangle by using the specified brush. This function includes the left and top borders, but excludes the right and bottom borders of the rectangle. 
	18	DrawIcon	The DrawIcon function draws an icon or cursor into the specified device context.
	19	SetBkMode	The SetBkMode function sets the background mix mode of the specified device context. The background mix mode is used with text, hatched brushes, and pen styles that are not solid lines. 
	20	GetClientRect	The GetClientRect function retrieves the coordinates of a window's client area. 
	21	InvalidateRect	The InvalidateRect function adds a rectangle to the specified window's update region. The update region represents the portion of the window's client area that must be redrawn. 
	22	BeginPaint	The BeginPaint function prepares the specified window for painting and fills a PAINTSTRUCT structure with information about the painting. 
	23	EndPaint	The EndPaint function marks the end of painting in the specified window. This function is required for each call to the BeginPaint function, but only after painting is complete. 
*/

/*===========================================4．字符串操作API=========================================*/
			
/*	4．字符串操作API		
	No. 	函数名称 	功能
	1	strlen	Get the length of a string.
	2	strcmp	Compare strings.
	3	atoi	Convert strings to integer.
	4	itoa	Converts an integer to a string.
	5	isdigit	Each routine returns nonzero if c is a particular representation of a decimal-digit character.
	6	islower	This routine returns nonzero if c is a particular representation of a lowercase character.
	7	isupper	This routine returns nonzero if c is a particular representation of an uppercase letter.
	8	MultiByteToWideChar	"This function maps a character string to a wide-character (Unicode) string. 
The character string mapped by this function is not necessarily from a multibyte character set. "
	9	WideCharToMultiByte	This function maps a wide-character string to a new character string. The new character string is not necessarily from a multibyte character set. 
	10	strcpy	Copy a string.
	11	strcat	Append a string.
	12	strstr	Find a substring.
	13	strncpy	Copy characters of one string to another.
	14	StringCchPrintf	"This function is a replacement for sprintf. It accepts a format string and a list of arguments and returns a formatted string. 
The size, in characters, of the destination buffer is provided to the function to ensure that StringCchPrintf does not write past the end of this buffer."
*/

/*===========================================5．文件操作相关API=========================================*/
			
	/*5．文件操作相关API		
	No. 	函数名称 	功能
	1	GetFileAttributes	Retrieves a set of FAT file system attributes for a specified file or directory.
	2	CreateDirectory	Creates a new directory. If the underlying file system supports security on files and directories, the function applies a specified security descriptor to the new directory.
	3	fopen	Open a file.
	4	fclose	Closes a stream or closes all open streams.
	5	fread	Reads data from a stream.
	6	fwrite	Writes data to a stream.
	*/
	
/*===========================================6．注册表操作相关API=========================================*/
	
	/*		
	6．注册表操作相关API		
	No. 	函数名称 	功能
	1	RegOpenKeyEx	This function opens the specified key.
	2	RegQueryValueEx	This function retrieves the type and data for a specified value name associated with an open registry key.
	3	RegCloseKey	This function releases the handle of the specified key.
	4	RegCreateKeyEx	This function creates the specified key. If the key already exists in the registry, the function opens it.
	5	RegSetValueEx	This function stores data in the value field of an open registry key. It can also set additional value and type information for the specified key.
	6	fwrite	Writes data to a stream.
	*/
	
/*===========================================7．物理地址取得相关API	=========================================*/
/*			
	7．物理地址取得相关API		
	No. 	函数名称 	功能
	1	gethostname	This function returns the standard host name for the local machine.
	2	gethostbyname	This function retrieves host information corresponding to a host name from a host database.
	3	GetAdaptersInfo	This function retrieves adapter information for the local computer.
*/

/*===========================================8．时间操作相关API	=========================================*/

			
	/*8．时间操作相关API		
	No. 	函数名称 	功能
	1	GetLocalTime	This function retrieves the current local date and time.
	2	GetTickCount	Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
	3	rand	Generates a pseudorandom number.
	4	srand	Sets a random starting point.
	5	SetTimer	The SetTimer function creates a timer with the specified time-out value.
	6	KillTimer	The KillTimer function destroys the specified timer. 
	7	Sleep	This function suspends the execution of the current thread for a specified interval. 
	*/

/*===========================================9．消息处理相关API	=========================================*/
	
	/*9．消息处理相关API		
	No. 	函数名称 	功能
	1	PeekMessage	This function checks a thread message queue for a message and places the message (if any) in the specified structure.
	2	PostMessage	The PostMessage function places (posts) a message in the message queue associated with the thread that created the specified window and returns without waiting for the thread to process the message.
	3	SendMessage	This function sends the specified message to a window or windows. SendMessage calls the window procedure for the specified window and does not return until the window procedure has processed the message. 
	*/
	
/*===========================================10．Windows操作相关API	=========================================*/
			
	/*10．Windows操作相关API		
	No. 	函数名称 	功能
	1	RegisterClass	This function registers a window class for subsequent use in calls to the CreateWindow or CreateWindowEx function.
	2	CreateWindowEx	The CreateWindowEx function creates an overlapped, pop-up, or child window with an extended window style; otherwise, this function is identical to the CreateWindow function. 
	3	IsWindow	The IsWindow function determines whether the specified window handle identifies an existing window. 
	4	SetWindowPos	This function changes the size, position, and z-order of a child, pop-up, or top-level window. Child, pop-up, and top-level windows are ordered according to their appearance on the screen. The topmost window receives the highest rank and is the first window in the z-order.
	5	ShowWindow	This function sets the specified window's show state.
	6	UpdateWindow	The UpdateWindow function updates the client area of the specified window by sending a WM_PAINT message to the window if the window's update region is not empty. The function sends a WM_PAINT message directly to the window procedure of the specified window, bypassing the application queue. If the update region is empty, no message is sent. 
	7	FindWindow	This function retrieves the handle to the top-level window whose class name and window name match the specified strings. This function does not search child windows. 
	8	DefWindowProc	This function calls the default window procedure to provide default processing for any window messages that an application does not process. This function ensures that every message is processed. DefWindowProc is called with the same parameters received by the window procedure.
	9	SetFocus	This function sets the keyboard focus to the specified window. All subsequent keyboard input is directed to this window. The window, if any, that previously had the keyboard focus loses it.
	10	GetWindowRect	This function retrieves the dimensions of the bounding rectangle of the specified window. The dimensions are given in screen coordinates that are relative to the upper-left corner of the screen. 
	11	MoveWindow	This function changes the position and dimensions of the specified window. For a top-level window, the position and dimensions are relative to the upper-left corner of the screen. For a child window, they are relative to the upper-left corner of the parent window's client area.
	12	IsWindowVisible	This function retrieves the visibility state of the specified window.
	*/		
			
/*===========================================11．其他操作相关API	=========================================*/
			
/*	11．其他操作相关API		
	No. 	函数名称 	功能
	1	malloc	Allocates memory blocks.
	2	free	Deallocates or frees a memory block.
	3	memset	Sets buffers to a specified character.
*/





#endif //_HYCO_API_H