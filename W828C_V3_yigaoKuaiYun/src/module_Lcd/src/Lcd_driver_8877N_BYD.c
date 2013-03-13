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

#ifdef HYC_DEV_LCD_8877N_BYD

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

    drvLcd_WriteInstruct(0x00CF);
    drvLcd_WriteData(0x0000);
    drvLcd_WriteData(0x0099);
    drvLcd_WriteData(0x0030);
    
    drvLcd_WriteInstruct(0x00ED);
    drvLcd_WriteData(0x0067);
    drvLcd_WriteData(0x0003);
    drvLcd_WriteData(0x0012);
    drvLcd_WriteData(0x0081);
    
    drvLcd_WriteInstruct(0x00E8);
    drvLcd_WriteData(0x0085);
    drvLcd_WriteData(0x000A);
    drvLcd_WriteData(0x0078);
    
    drvLcd_WriteInstruct(0x00CB);
    drvLcd_WriteData(0x0039);
    drvLcd_WriteData(0x002C);
    drvLcd_WriteData(0x0000);
    drvLcd_WriteData(0x0034);
    drvLcd_WriteData(0x0002);
    
	Delay1(3);                     

    drvLcd_WriteInstruct(0x00F7);
    drvLcd_WriteData(0x0020);
    
    drvLcd_WriteInstruct(0x00EA);
    drvLcd_WriteData(0x0000);
    drvLcd_WriteData(0x0000);
    
    drvLcd_WriteInstruct(0x00C0);	//Power control
    drvLcd_WriteData(0x0026);		//VRH[5:0]
    
    drvLcd_WriteInstruct(0x00C1);	//Power control
    drvLcd_WriteData(0x0013);		//SAP[2:0];BT[3:0]
    
    drvLcd_WriteInstruct(0x00C5);	//VCM control
    drvLcd_WriteData(0x002B);
    drvLcd_WriteData(0x002C);
    
    drvLcd_WriteInstruct(0x00C7);	//VCM control2
    drvLcd_WriteData(0x00D4);
    
    drvLcd_WriteInstruct(0x00F6);
    drvLcd_WriteData(0x0001);
    drvLcd_WriteData(0x0030);
    drvLcd_WriteData(0x0000);
    
    drvLcd_WriteInstruct(0x00B6);
    drvLcd_WriteData(0x000A);
    drvLcd_WriteData(0x00A2);
    
    drvLcd_WriteInstruct(0x0036);	// Memory Access Control
    drvLcd_WriteData(0x0008);
    
    drvLcd_WriteInstruct(0x003A);
    drvLcd_WriteData(0x0055);
    
    drvLcd_WriteInstruct(0x00B1);
    drvLcd_WriteData(0x0000);
    drvLcd_WriteData(0x0019);

    drvLcd_WriteInstruct(0x00F2);	// 3Gamma Function Disable
    drvLcd_WriteData(0x0000);
    
    drvLcd_WriteInstruct(0x0026);	//Gamma curve selected
    drvLcd_WriteData(0x0001);
    
    drvLcd_WriteInstruct(0x00E0);	//Set Gamma
    drvLcd_WriteData(0x000F);
    drvLcd_WriteData(0x0025);
    drvLcd_WriteData(0x0023);
    drvLcd_WriteData(0x000A);
    drvLcd_WriteData(0x000E);
    drvLcd_WriteData(0x0009);
    drvLcd_WriteData(0x0050);
    drvLcd_WriteData(0x00A9);
    drvLcd_WriteData(0x0041);
    drvLcd_WriteData(0x0007);
    drvLcd_WriteData(0x0010);
    drvLcd_WriteData(0x0003);
    drvLcd_WriteData(0x0028);
    drvLcd_WriteData(0x002A);
    drvLcd_WriteData(0x0000);
   
    drvLcd_WriteInstruct(0x00E1);	//Set Gamma
    drvLcd_WriteData(0x0000);
    drvLcd_WriteData(0x001A);
    drvLcd_WriteData(0x001C);
    drvLcd_WriteData(0x0005);
    drvLcd_WriteData(0x0011);
    drvLcd_WriteData(0x0006);
    drvLcd_WriteData(0x002F);
    drvLcd_WriteData(0x0056);
    drvLcd_WriteData(0x003E);
    drvLcd_WriteData(0x0008);
    drvLcd_WriteData(0x000F);
    drvLcd_WriteData(0x000C);
    drvLcd_WriteData(0x0017);
    drvLcd_WriteData(0x0015);
    drvLcd_WriteData(0x000F);
    
	Delay1(5);                           

    drvLcd_WriteInstruct(0x0011);	//Exit Sleep
    
	Delay1(12);                           

    drvLcd_WriteInstruct(0x0029);	//Display on
	Delay1(6);                           

    drvLcd_WriteInstruct(0x002c);

   
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
		
		drvLcd_WriteInstruct(0x002A); 
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
		drvLcd_WriteInstruct(0x002A); 
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
	drvLcd_WriteInstruct(0x0011);// Sleep out
	
	Delay1(6);                           
  
	drvLcd_WriteInstruct(0x0029);// Display on
}

/*-----------------------------------------------------------------------------
* 函数:	drvLcd_DisplayOff
* 功能:	关闭显示
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOff_0xB505(void)
{
	drvLcd_WriteInstruct(0x0028);// Display off

	Delay1(2);                           

	drvLcd_WriteInstruct(0x0010);// Enter Sleep mode
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
		tempdata =(0x00FF&kColumn);					//Specify the horizontal start positions
		drvLcd_WriteInstruct(0x002A);
		drvLcd_WriteData(tempdata >> 8); 
		drvLcd_WriteData(tempdata); 
		
		tempdata =(kColumn+kWidth-1); 				//Specify the horizontal end positions
		drvLcd_WriteData(tempdata >> 8);               
		drvLcd_WriteData(tempdata);               
	
					 
		tempdata=(0x01FF&kRow); 
		drvLcd_WriteInstruct(0x002B);           	//Specify the Vertical start positions
		drvLcd_WriteData(tempdata >>8); 
		drvLcd_WriteData(tempdata);               
					
		tempdata=(0x01FF&(kRow+kHeight-1));	
		drvLcd_WriteData(tempdata >> 8);	
		drvLcd_WriteData(tempdata);               
/*			
		tempdata=(0x00FF&(kColumn)); 
		drvLcd_WriteInstruct(0x0020);               //Specify the y address in RAM
		drvLcd_WriteData(tempdata); 

		tempdata=(0x01FF&kRow);
	    drvLcd_WriteInstruct(0x0021);               //Specify the x address in RAM
		drvLcd_WriteData(tempdata); */
	}
	else if(1 == gScreen_Display_Mode)
	{
		tempdata=(0xff&(239-(kRow+kHeight-1))); 	//Specify the horizontal start positions
		drvLcd_WriteInstruct(0x002A);
		drvLcd_WriteData(tempdata); 
		
		
		tempdata =(239-kRow);						//Specify the horizontal end positions	
		drvLcd_WriteData(tempdata);   
					 
		tempdata=(0x01FF&kColumn); 
		drvLcd_WriteInstruct(0x002B);               //Specify the Vertical start positions
		drvLcd_WriteData(tempdata); 
					
		tempdata=(0x01FF&(kColumn+kWidth-1));	
		drvLcd_WriteData(tempdata);	
/*		
		tempdata=(0x00FF&((239-kRow))); 
		drvLcd_WriteInstruct(0x0020);               //Specify the y address in RAM
		drvLcd_WriteData(tempdata); 

		tempdata=(0x01FF&kColumn);
	    drvLcd_WriteInstruct(0x0021);               //Specify the x address in RAM
		drvLcd_WriteData(tempdata); */
	}
	
	drvLcd_WriteInstruct(0x002c); 
} 

#if 0
void test_lcdcpu_0xB505(void)
{
	int i =  48 * 48  ;

	hyhwLcd_lightSet(40);

	drvLcd_176220DisplayMode();	
	drvLcd_SetRowColumn_Lcd(0,0,48,48);

	Led_ctrl(2);
	
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