/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*  本程序为tcc8300 mmu 模块的驱动
*	tcc8300 提供了一个section table, 而且，在应用程序没有用MMU的时候，适用一组寄存器
*	可以设置8个Region的cache 和 buffer的存取特性
*******************************************************************************/

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
  
#include "hyhwChip.h"
#include "hyhwArm926EJs.h"
#include "hyhwMmu.h"



#ifdef __cplusplus
  /* Assume C declarations for C++ */
  extern "C" {
#endif

/*-----------------------------------------------------------------------------
* 函数:	hyhwMmu_initCache
* 功能:	初始化系统的Cache
*		REGION7 优先级最高，REGION0优先级最低，这就表示，如果两个REGION有重叠，
*		则以优先级高的为准。由于REGION0的优先级最低，可以把所有的4G空间都设为Region0
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwMmu_initCache(void)
{

	U32	controlValue;
	
	int *psectionTable;
	

	/* Region 0， 定义为基地址从 0x0000.0000  */  
	MMU_REGION0_REGISTER	= (	0x00000000 | MMU_REGION_SIZE_4GB | MMU_REGION_DOMAIN(0) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE |
					 		 	MMU_BIT_REGION_CACHEABLE | MMU_BIT_REGION_BUFFERABLE );


	/* Region 1， 定义为基地址从 0x2FF0.0000  */  
	MMU_REGION1_REGISTER	= (	0x2FF00000 | MMU_REGION_SIZE_1MB | MMU_REGION_DOMAIN(1) |
								MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE |
								MMU_BIT_REGION_CACHEABLE |  MMU_BIT_REGION_BUFFERABLE );

	/* Region 2， 定义为基地址从 0x3000.0000  internal SRAM*/  
	MMU_REGION2_REGISTER	= (	0x30000000 | MMU_REGION_SIZE_256MB | MMU_REGION_DOMAIN(2) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE |
					 		 	MMU_BIT_REGION_CACHEABLE |  MMU_BIT_REGION_BUFFERABLE );


	/* Region 3， 定义为基地址从 0x4000.0000  chip select 0 */  
	MMU_REGION3_REGISTER	= (	0x40000000 | MMU_REGION_SIZE_512MB | MMU_REGION_DOMAIN(3) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE );

	/* Region 4， 定义为基地址从 0x6000.0000  chip select 2 */  
	MMU_REGION4_REGISTER	= (	0x60000000 | MMU_REGION_SIZE_256MB | MMU_REGION_DOMAIN(4) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE );

	/* Region 5， 定义为基地址从 0x1000.0000  */  
	MMU_REGION5_REGISTER	= (	0x10000000 | MMU_REGION_SIZE_256MB | MMU_REGION_DOMAIN(5) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE |
					 		 	MMU_BIT_REGION_CACHEABLE |  MMU_BIT_REGION_BUFFERABLE );


	/* Region 6， 定义为基地址从 0x8000.0000   AHB peripherals*/  
	MMU_REGION6_REGISTER	= (	0x80000000 | MMU_REGION_SIZE_2GB | MMU_REGION_DOMAIN(6) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE );

	/* Region 7， 定义为基地址从 0x3000.F000  */  
	MMU_REGION7_REGISTER	= (	0x3000F000 | MMU_REGION_SIZE_1MB | MMU_REGION_DOMAIN(7) |
					 		 	MMU_REGION_AP_MANAGER | MMU_BIT_REGION_ENABLE |
					 		 	MMU_BIT_REGION_CACHEABLE |  MMU_BIT_REGION_BUFFERABLE );


	/*	调用协处理器cp15指令，将上面的设置生效 */
	/* 对 TCM (Tightly Coupled Memory) 的控制, DTCM : base =0xA0000000 , size = 8k  */
	hyhwArm926ejs_dtcmConfig( 0xA0000000, ARM926EJS_DTCM_SIZE_8K, EnableOn);
	
	psectionTable			= (int *)MMU_DESCRIPT_BASEADDR;
	hyhwArm926ejs_mmuConfig ( psectionTable);
	
	
	hyhwArm926ejs_invalidateIcache();	/*Flush	Icache*/
	hyhwArm926ejs_invalidateDcache();	/*Flush	Dcache*/


	controlValue	=( ARM926EJS_CP15C1_BIT_RESERVE |		/* reserve 的bit 的取值的组合 	*/
					   ARM926EJS_CP15C1_BIT_ROUNDROBIN |	/* round-robin replacement		*/
					   ARM926EJS_CP15C1_BIT_ICACHE_ENABLE |	/* I-Cache Enable				*/	
					   ARM926EJS_CP15C1_BIT_DCACHE_ENABLE |	/* D-Cache Enable				*/		
					   ARM926EJS_CP15C1_BIT_MMU_ENABLE );	/* MMU enable					*/

	hyhwArm926ejs_updateControl( controlValue);
	

}

/*-----------------------------------------------------------------------------
* 函数:	hyhwMmu_initRegion
* 功能:	初始化系统的Region
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwMmu_initRegion(void)
{

}

#ifdef __cplusplus
  }
#endif


/* Tcc8300 的 memory map 
---------------------------------------------------------------------------------------
0x00000000 ~ 0x0FFFFFFF		Dependeing on REMAP register value
							1. Internal BootROM
							2. Internal SRAM
							3. External SDRAM
							4. External SRAM (CS0)
							5. External NOR (CS3)
0x10000000 ~ 0x1FFFFFFF		Not Used
0x20000000 ~ 0x2FFFFFFF		External SDRAM
0x30000000 ~ 0x3000FFFF		Internal SRAM (64 KB)
0x40000000 ~ 0x4FFFFFFF		Assigned to chip select 0
							Initially the configuration register is set to SRAM
0x50000000 ~ 0x5FFFFFFF		Assigned to chip select 1
							Initially the configuration register is set to IDE type device
0x60000000 ~ 0x6FFFFFFF		Assigned to chip select 2
							Initially the configuration register is set to NAND flash
0x70000000 ~ 0x7FFFFFFF		Assigned to chip select 3
							Initially the configuration register is set to ROM
0x80000000 ~ 0x8FFFFFFF		AHB peripherals
0x90000000 ~ 0x9FFFFFFF		APB peripherals
0xA0000000 ~ 0xA0001FFF		Data TCM (8 KB)
0xB0000000 ~ 0xBFFFFFFF		Not Used
0xC0000000 ~ 0xCFFFFFFF		Not Used
0xD0000000 ~ 0xDFFFFFFF		Not Used
0xE0000000 ~ 0xEFFFFFFF		Internal boot ROM
0xF0000000 ~ 0xFFFFFFFF		External memory controller configuration register space

----------------------------------------------------------------------------------------*/
 
