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

#ifdef HYC_DEV_LCD_BYD

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
void drvLcd_InitLcdModuleInstructs_0x9325( void)
{
	Delay1(18);                           

    drvLcd_WriteInstruct(0x00e5);drvLcd_WriteData(0x78f0);

    drvLcd_WriteInstruct(0x0001);drvLcd_WriteData(0x0100);
    drvLcd_WriteInstruct(0x0002);drvLcd_WriteData(0x0700);
    drvLcd_WriteInstruct(0x0003);drvLcd_WriteData(0x1030);
    drvLcd_WriteInstruct(0x0004);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0008);drvLcd_WriteData(0x0808);
    drvLcd_WriteInstruct(0x0009);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x000a);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x000c);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x000d);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x000f);drvLcd_WriteData(0x0003);
    

	//*************Power On sequence ****************//
    drvLcd_WriteInstruct(0x0010);drvLcd_WriteData(0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    drvLcd_WriteInstruct(0x0011);drvLcd_WriteData(0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    drvLcd_WriteInstruct(0x0012);drvLcd_WriteData(0x0000); // VREG1OUT voltage
    drvLcd_WriteInstruct(0x0013);drvLcd_WriteData(0x0000); // VDV[4:0] for VCOM amplitude  0000
    drvLcd_WriteInstruct(0x0007);drvLcd_WriteData(0x0001);
	Delay1(50);                           
    
    drvLcd_WriteInstruct(0x0010);drvLcd_WriteData(0x1590); //1590
    drvLcd_WriteInstruct(0x0011);drvLcd_WriteData(0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]0227
	Delay1(50);                           

    drvLcd_WriteInstruct(0x0012);drvLcd_WriteData(0x0088); // External reference voltage= Vci;
	Delay1(50);                           

    drvLcd_WriteInstruct(0x0013);drvLcd_WriteData(0x1c00); // VDV[4:0] for VCOM amplitude 1900
    drvLcd_WriteInstruct(0x0029);drvLcd_WriteData(0x002f); // VCM[5:0] for VCOMH 0x23  
    drvLcd_WriteInstruct(0x002B);drvLcd_WriteData(0x000b); // Set Frame Rate 0x0d  0x0b
	Delay1(50);                           

    drvLcd_WriteInstruct(0x0020);drvLcd_WriteData(0x0000);//vcom2
    drvLcd_WriteInstruct(0x0021);drvLcd_WriteData(0x0000);//vcom1

	//------------------ Gamma---------------------//
    drvLcd_WriteInstruct(0x0030);drvLcd_WriteData(0x0000);  //0007  0001
    drvLcd_WriteInstruct(0x0031);drvLcd_WriteData(0x0505);	//0303  0605
    drvLcd_WriteInstruct(0x0032);drvLcd_WriteData(0x0006);	//0003  0106
    drvLcd_WriteInstruct(0x0035);drvLcd_WriteData(0x0300);	//0206  0402
    drvLcd_WriteInstruct(0x0036);drvLcd_WriteData(0x0004);	//0008  0807
    drvLcd_WriteInstruct(0x0037);drvLcd_WriteData(0x0107);	//0406  0106
    drvLcd_WriteInstruct(0x0038);drvLcd_WriteData(0x0202);	//0304  0101
    drvLcd_WriteInstruct(0x0039);drvLcd_WriteData(0x0707);	//0007  0507
    drvLcd_WriteInstruct(0x003C);drvLcd_WriteData(0x0003);	//0602  0204
    drvLcd_WriteInstruct(0x003D);drvLcd_WriteData(0x0004);	//0008  0608

	//------------------ Set GRAM area ---------------//
    drvLcd_WriteInstruct(0x0050);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0051);drvLcd_WriteData(0x00ef);
    drvLcd_WriteInstruct(0x0052);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0053);drvLcd_WriteData(0x013f);
    drvLcd_WriteInstruct(0x0060);drvLcd_WriteData(0xa700);
    drvLcd_WriteInstruct(0x0061);drvLcd_WriteData(0x0001);
    drvLcd_WriteInstruct(0x006A);drvLcd_WriteData(0x0000);

	//-------------- Partial Display Control ---------//
    drvLcd_WriteInstruct(0x0080);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0081);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0082);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0083);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0084);drvLcd_WriteData(0x0000);
    drvLcd_WriteInstruct(0x0085);drvLcd_WriteData(0x0000);

	//-------------- Panel Control -------------------//
    drvLcd_WriteInstruct(0x0090);drvLcd_WriteData(0x0010);
    drvLcd_WriteInstruct(0x0092);drvLcd_WriteData(0x0600);

    drvLcd_WriteInstruct(0X0007);drvLcd_WriteData(0x0133);
	Delay1(200);                           

    drvLcd_WriteInstruct(0x0022);

	Delay1(8);                           

   
}

#ifdef DISPLAYMODE_TWO
void drvLcd_176220DisplayMode_0x9325(void)
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

void drvLcd_220176DisplayMode_0x9325(void)
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
void drvLcd_DisplayOn_0x9325(void)
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
void drvLcd_DisplayOff_0x9325(void)
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

void drvLcd_ContrastAdjust_0x9325(U8 u8Contrast)
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
void drvLcd_SetRowColumn_Lcd_0x9325(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
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
void test_lcdcpu_0x9325(void)
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