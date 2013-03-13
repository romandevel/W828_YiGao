/*
	使用此驱动IC的显示屏有:TFT320240_KGM281B0
*/


/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "hyhwLcd.h"
#include "hyhwGpio.h"
#include "glbVariable_base.h"
#include "HycDeviceConfig.h"

#ifdef HYC_DEV_LCD_N2748TLY

#define DISPLAYMODE_TWO

/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
#ifdef DISPLAYMODE_TWO
extern U8 gScreen_Display_Mode;	//显示方式
#endif

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_InitLcdModuleInstructs
* 功能:	LCD模块初始化
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_InitLcdModuleInstructs_0xB505( void)
{
	Delay1(8);                           

    drvLcd_WriteInstruct(0x0000);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0000);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0X00A4);drvLcd_WriteData(0x0001);
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0060);drvLcd_WriteData(0x2700);//Driver Output Control NL
    drvLcd_WriteInstruct(0x0008);drvLcd_WriteData(0x0806);//Display Control 2 (R08h) FP BP
//γ Control
    drvLcd_WriteInstruct(0X0030);drvLcd_WriteData(0X000f);//R30~39h: γ Control
    drvLcd_WriteInstruct(0X0031);drvLcd_WriteData(0X7707);
    drvLcd_WriteInstruct(0X0032);drvLcd_WriteData(0X160b);
    drvLcd_WriteInstruct(0X0033);drvLcd_WriteData(0X0007);
    drvLcd_WriteInstruct(0X0034);drvLcd_WriteData(0X0001);
    drvLcd_WriteInstruct(0X0035);drvLcd_WriteData(0X0700);
    drvLcd_WriteInstruct(0X0036);drvLcd_WriteData(0X7b15);
    drvLcd_WriteInstruct(0X0037);drvLcd_WriteData(0X0707);
    drvLcd_WriteInstruct(0X0038);drvLcd_WriteData(0X0f00);
    drvLcd_WriteInstruct(0X0039);drvLcd_WriteData(0X0013);
    drvLcd_WriteInstruct(0x0090);drvLcd_WriteData(0x0019);//R90h: RTNI, DIVI
    drvLcd_WriteInstruct(0X000a);drvLcd_WriteData(0X0008);
	Delay1(3);                           

    drvLcd_WriteInstruct(0X0010);drvLcd_WriteData(0X0410);//BT, AP
	Delay1(6);                           

    drvLcd_WriteInstruct(0X0011);drvLcd_WriteData(0X0247);//DC,VC
	Delay1(6);                           

    drvLcd_WriteInstruct(0x000E);drvLcd_WriteData(0x0020);
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0013);drvLcd_WriteData(0x0b00);//vcom
	Delay1(3);                           

    drvLcd_WriteInstruct(0x002A);drvLcd_WriteData(0x002c);//vcom2
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0029);drvLcd_WriteData(0x002c);//vcom1
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0012);drvLcd_WriteData(0x019a);
	Delay1(6);                           

    drvLcd_WriteInstruct(0x0012);drvLcd_WriteData(0x01b8);
	Delay1(6);                           
    
    drvLcd_WriteInstruct(0x0003);drvLcd_WriteData(0x1030);

//Window address control instruction
    drvLcd_WriteInstruct(0x0050);drvLcd_WriteData(0x0000);//Window Horizontal RAM Address Start/End (R50h/ R51h)
    drvLcd_WriteInstruct(0x0051);drvLcd_WriteData(0x00EF);
    drvLcd_WriteInstruct(0x0052);drvLcd_WriteData(0x0000);//Window Vertical RAM Address Start/End (R52h/R53h)
    drvLcd_WriteInstruct(0x0053);drvLcd_WriteData(0x013F);
//Base image display control instruction
    drvLcd_WriteInstruct(0x0061);drvLcd_WriteData(0x0001);//Base Image Display Control (R61h)0x0000
    drvLcd_WriteInstruct(0x006A);drvLcd_WriteData(0x0000);//Vertical Scroll Control (R6Ah)
//Partial display control instruction
    drvLcd_WriteInstruct(0x0080);drvLcd_WriteData(0x0000);//Partial Image 1: Display Position 
    													  //(R80h), RAM Address (Start/End Line Address) (R81h/R82h)
    drvLcd_WriteInstruct(0x0081);drvLcd_WriteData(0x0000);//Partial Image 2: Display Position
														  //(R83h), RAM Address (Start/End Line Address) (R84h/R85h)
    drvLcd_WriteInstruct(0x0082);drvLcd_WriteData(0x0000);
//Panel interface control instruction
    drvLcd_WriteInstruct(0x0092);drvLcd_WriteData(0x0300);
    drvLcd_WriteInstruct(0x0093);drvLcd_WriteData(0x0005);
    drvLcd_WriteInstruct(0x0001);drvLcd_WriteData(0x0100);//Driver Output Control (R01h)
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0002);drvLcd_WriteData(0x0200);//LCD Driving Wave Control (R02h)
	Delay1(3);                           

    drvLcd_WriteInstruct(0x0003);drvLcd_WriteData(0x1030);//Entry Mode (R03h) 0x1030
	Delay1(3);                           

    drvLcd_WriteInstruct(0x000C);drvLcd_WriteData(0x0000);//External Display Interface Control 1 (R0Ch)0x0000
    drvLcd_WriteInstruct(0x000F);drvLcd_WriteData(0x0000);//External Display Interface Control 2 (R0Fh)
//RAM access instruction
    drvLcd_WriteInstruct(0x0020);drvLcd_WriteData(0x0000);//RAM Address Set (Horizontal Address) (R20h)
    drvLcd_WriteInstruct(0x0021);drvLcd_WriteData(0x0000);//RAM Address Set (Vertical Address) (R21h)
//display on
	Delay1(3);                           

    drvLcd_WriteInstruct(0X0007);drvLcd_WriteData(0X0100);
	Delay1(6);                           

    drvLcd_WriteInstruct(0x0022);

	Delay1(8);                           

   
}

#ifdef DISPLAYMODE_TWO
void drvLcd_176220DisplayMode_0xB505(void)
{
	if(1 == gScreen_Display_Mode)
	{
		while(lcdCpuDmaOK() != TRUE)
		{//等DMA送屏结束
			syssleep(1);
		}
		
		lcdCpuDmaStop();
		syssleep(10);	
		
		drvLcd_WriteInstruct(0x0003); 
		drvLcd_WriteData(0x1030); 		//Entry Mode (R03h)
		gLcd_Total_Column = cLCD_TOTAL_COLUMN;
		gLcd_Total_Row = cLCD_TOTAL_ROW;
		gScreen_Display_Mode=0;
		gu16Lcd_ColumnLimit = gLcd_Total_Column;
		
	}
	
} 

void drvLcd_220176DisplayMode_0xB505(void)
{
	if(0 == gScreen_Display_Mode)
	{
		drvLcd_WriteInstruct(0x0003); 
		drvLcd_WriteData(0x1028); 	//Entry Mode (R03h)
		gLcd_Total_Column = cLCD_TOTAL_ROW;
		gLcd_Total_Row = cLCD_TOTAL_COLUMN;
		gScreen_Display_Mode=1;
	    gu16Lcd_ColumnLimit = gLcd_Total_Column;
	}
	
}
#endif


/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayOn
* 功能:	打开显示
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOn_0xB505(void)
{
   drvLcd_WriteInstruct(0x0007);//display on
   drvLcd_WriteData(0x0173);
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayOff
* 功能:	关闭显示
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOff_0xB505(void)
{
	drvLcd_WriteInstruct(0x0007);	//display off
	drvLcd_WriteData(0x0061);		//0x0001
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_ContrastAdjust
* 功能:	调整对比度
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/

void drvLcd_ContrastAdjust_0xB505(U8 u8Contrast)
{
	//待实现
}
/*-----------------------------------------------------------------------------
* 函数:	drvLcd_SetRowColumn
* 功能:	设置彩色显示屏的显示起始地址，地址为行序号和列序号row和Column,以及高度
*		和宽度
* 参数:	kRow 			- 行序号 , 起始行：0
*		kColumn 		- 列序号 ，起始列：0
*		U16 kHeight 	- 高度
*		U16 kWidth 		- 宽度
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn_Lcd_0xB505(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
{
	U16 tempdata;
	
	if(0 == gScreen_Display_Mode)
	{
		tempdata =(0x00FF&kColumn);						//Specify the horizontal start positions
		drvLcd_WriteInstruct(0x0050);
		drvLcd_WriteData(tempdata); 
		
		tempdata =(kColumn+kWidth-1); 	//Specify the horizontal end positions
		drvLcd_WriteInstruct(0x0051);
		drvLcd_WriteData(tempdata);               
		
					 
		tempdata=(0x01FF&kRow); 
		drvLcd_WriteInstruct(0x0052);               //Specify the Vertical start positions
		drvLcd_WriteData(tempdata); 
					
		tempdata=(0x01FF&(kRow+kHeight-1));	
		drvLcd_WriteInstruct(0x0053);               //Specify the Vertical end positions
		drvLcd_WriteData(tempdata);	
			
		tempdata=(0x00FF&(kColumn)); 
		drvLcd_WriteInstruct(0x0020);               //Specify the y address in RAM
		drvLcd_WriteData(tempdata); 

		tempdata=(0x01FF&kRow);
	    drvLcd_WriteInstruct(0x0021);               //Specify the x address in RAM
		drvLcd_WriteData(tempdata); 
	}
	else if(1 == gScreen_Display_Mode)
	{
		tempdata=(0xff&(239-(kRow+kHeight-1))); 	//Specify the horizontal start positions
		drvLcd_WriteInstruct(0x0050);
		drvLcd_WriteData(tempdata); 
		
		
		tempdata =(239-kRow);		//Specify the horizontal end positions	
		drvLcd_WriteInstruct(0x0051);
		drvLcd_WriteData(tempdata);   
					 
		tempdata=(0x01FF&kColumn); 
		drvLcd_WriteInstruct(0x0052);               //Specify the Vertical start positions
		drvLcd_WriteData(tempdata); 
					
		tempdata=(0x01FF&(kColumn+kWidth-1));	
		drvLcd_WriteInstruct(0x0053);               //Specify the Vertical end positions
		drvLcd_WriteData(tempdata);	
		
		tempdata=(0x00FF&((239-kRow))); 
		drvLcd_WriteInstruct(0x0020);               //Specify the y address in RAM
		drvLcd_WriteData(tempdata); 

		tempdata=(0x01FF&kColumn);
	    drvLcd_WriteInstruct(0x0021);               //Specify the x address in RAM
		drvLcd_WriteData(tempdata); 
	}
	
	drvLcd_WriteInstruct(0x0022); 
}

#if 0
void test_lcdcpu_0xB505(void)
{
	int i =  48 * 48  ;
	drvLcd_176220DisplayMode();	
	drvLcd_SetRowColumn_Lcd(0,0,48,48);

	while(i--)
	{
		drvLcd_WriteData(0xf800);//RED 16bit
	}
	i = 0xfffff;
	while(i--);
	hyhwLcd_lightSet(40);
	i = 0xffff;
	while(i--);
	hyhwLcd_lightSet(50);
	i = 0xfffff;
	while(i--);
	hyhwLcd_lightSet(60);
	
	while(1);
	
}
#endif

#endif