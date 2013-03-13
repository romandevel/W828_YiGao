/*---------------------------------------------------------------------------------
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
*  说明：本文件为 arm926ej 核的 cp15 协处理器相关操作的代码
*  		详细信息，请参考ARM公司的文档ADS_DeveloperGuide_D.pdf 和DDI0198C_926_trm.pdf
*		注意，对于ARM不同的内核，内核配置寄存器可能有所不同，需要查阅相关的技术文档

*  应用案例1：对于程序搬移（overlay）时，在icache和dcache打开的情况下，需要做如下操作，
*		主要是为了避免overlay时部分原有程序在cache中，导致运行时实际运行了cache中
*		的程序而没有运行新copy的程序。这个过程包括对icache和dcache的刷新，调用顺序为
*		step1. 回写data Cache  (调用函数hyhwArm926e_writeBackDcache)
*		step2. 将write buffer的内容强制回写到物理空间 (调用函数 hyhwArm926ejs_drainWriteBuffer)
*		step3. 使Instruct Cache 和Data Cache的内容无效(调用函数 hyhwArm926ejs_invalidIcacheDcache)
* ---------------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwArm926EJs.h"




/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_readFaultStatus
* 功能:	通过读取协处理器的Fault state register c5, 得到出错的状态
* 参数:	none
* 返回:	出错的状态
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultStatus(void)
{
	U32 val;
	__asm
	{
		MRC p15, 0, val, c5, c0, 0
	}
	return val;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_readFaultAddress
* 功能:	通过读取协处理器的Fault Address register c6, 得到出错的地址
* 参数:	none
* 返回:	出错的地址
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultAddress(void)
{
	U32 val;
	__asm
	{
		MRC p15, 0, val, c6, c0, 0
	}
  	return val;
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_drainWriteBuffer
* 功能:	将write buffer的内容强制回写到物理空间 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_drainWriteBuffer(void)
{
	U32 src=0;
	__asm 
	{
		MCR p15, 0, src, c7, c10, 4
	}
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_updateControl
* 功能:	更新cp15的控制寄存器 c1的值
* 参数:	val		-- 要写入到控制寄存器 c1的值
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_updateControl(U32 val)
{
	__asm
	{
		/* 写协处理器cp15的控制寄存器 c1*/
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_enableDcache
* 功能:	使能 data cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableDcache(void)
{
	U32 val;
	
	__asm
	{
		/*  读取协处理器cp15的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0

		/* 清除BIT2 ( C bit DCache enable/disable ) 0 = DCache disabled 1 = DCache enabled
		   然后回写 c1*/
		ORR val, val, ARM926EJS_CP15C1_BIT_DCACHE_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_disableDcache
* 功能:	禁止 data cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableDcache(void)
{
	UInt32 val;
	
	__asm
	{
		/*  读取协处理器cp15的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0

		/* 清除BIT2 ( C bit DCache enable/disable ) 0 = DCache disabled 1 = DCache enabled
		   然后回写 c1*/
		BIC val, val, ARM926EJS_CP15C1_BIT_DCACHE_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}


/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_enableIcache
* 功能:	使能 Instruct cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableIcache(void)
{
	U32 val;
	
	__asm
	{
		/*  读取协处理器cp15的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0

		/* 置位BIT12 ( I bit ICache enable/disable ) 0 = ICache disabled 1 = ICache enabled
		   然后回写 c1*/
		ORR val, val, ARM926EJS_CP15C1_BIT_ICACHE_ENABLE  
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_disableIcache
* 功能:	禁止 Instruct cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableIcache(void)
{
	U32 val;
	
	__asm
	{
		/* 读取协处理器cp15的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0

		/* 清除BIT12 ( I bit ICache enable/disable ) 0 = ICache disabled 1 = ICache enabled
		   然后回写 c1*/
		BIC val, val, ARM926EJS_CP15C1_BIT_ICACHE_ENABLE  
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidateDcache
* 功能:	使 Data cache 无效 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateDcache(void)
{
	unsigned int src = 0;
	__asm
	{
		MCR p15, 0, src, c7, c6, 0
		NOP
		NOP
	}
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidateIcache
* 功能:	使 Instruct cache 的指令无效
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateIcache(void)
{
	U32 src = 0;
	
	__asm
	{
		MCR p15, 0, src, c7, c5, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidIcacheDcache
* 功能:	使 Instruct cache 和 Data cache 无效
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidIcacheDcache(void)
{
	U32 src = 0;
	
	__asm
	{
		MCR p15, 0, src, c7, c7, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926e_writeBackDcache
* 功能:	回写Data Cache数据
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926e_writeBackDcache(void)
{
	__asm
	{
		testClean:	
			MRC p15, 0, r15, c7, c10, 3
			BNE testClean
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuEnable
* 功能:	使能MMU
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuEnable (void)
{
	int val;

	__asm
	{
		/* 读取协处理器的控制寄存器 c1 */ 
		MRC p15, 0, val, c1, c0, 0

		/* 置位 BIT0: M， 使能MMU，然后回写c1 */
		ORR	val, val, ARM926EJS_CP15C1_BIT_MMU_ENABLE 
    	MCR p15, 0, val, c1, c0, 0
		NOP 
		NOP 
	    NOP
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuEnable
* 功能:	禁止MMU
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuDisable (void)
{
	UInt32 val;
	
	__asm
	{
		/* 读取协处理器的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0

		/* 清除位BIT0: M， 禁止MMU，然后回写c1 */
		BIC val, val, ARM926EJS_CP15C1_BIT_MMU_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuConfig
* 功能:	配置arm926ej的mmu，通过协处理器cp15完成
* 参数:	sectionTable	-- 指向mmu 的section table 的地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuConfig(int *sectionTable)
{
	int val;
  
	/*  通过协处理器cp15配置MMU */
	__asm
	{
		/* 读取协处理器的控制寄存器 c1 */
		MRC p15, 0, val, c1, c0, 0
		
		/* 将控制寄存器 c1 的 R 位(BIT9) 和 S位 (BIT8) 置位，再回写入 c1  
			R 位 -- ROM protection 
			S 位 -- System protection
			当 某个domain的 access permitssion 为零时，对access特性有影响  */  
		ORR val, val, (ARM926EJS_CP15C1_BIT_ROM_PROTECT|ARM926EJS_CP15C1_BIT_SYS_PROTECT)
    	MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
		NOP
                
		/* 写入Translation table base register c2，安装page table  */
		MCR p15, 0, sectionTable, c2, c0, 0
        
        /*	使TLB无效，目的是确认使用新写入的TLB？*/
		MCR	p15, 0, r0, c8, c7, 0		// Invalidate TLB
		
		/* Domain Setting 
		   16个domain，每个domain依次占用2BIT，这里每个domain的2个BIT 都是 01b，
		   表示permission check决定于每个region的access permitssion的设置 
		   组合起来就是 0x55555555 */
		mov	r0, 0x55555555
		MCR	p15, 0, r0, c3, c0, 0	
	}
	
}



/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_dtcmConfig
* 功能:	TCM (Tightly Coupled Memory) 的控制
* 参数:	baseAddr	-- data TCM memory 的基地址
*		sizebit		-- 表示TCM大小的定义，注意使bit位的表示
*		enable		-- 是否enable 0-diable， 1- enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_dtcmConfig(U32 baseAddr, U32 sizebit, U32 enable)
{
	U32	val;
	
	val = (baseAddr|sizebit|enable);
	__asm
	{
		MCR	p15, 0, val, c9, c1, 0
	}	

}


/* -----------------------------------------------------------------------------------------
附录1: ARM926EJ-s core coprocessor registers


0. Cache type register 0  两种
   +--------------------------------------------------------------------------+
   | 31 30 29 | 28   25 | 24 | 23        12 | 11         					0 |
   | 0  0  0  | Ctype   | S  |    Dsize     |     Isize    					  |
   +--------------------------------------------------------------------------+ 
                                            +---------------------------------+
                                            | 11 10 | 9   6 | 5   3 | 2 | 1 0 |
                                            | 0  0  | Size  | Assoc | M | Len |
                                            +---------------------------------+   
1. Control register 1
   +-------------------------------------------------------------------------------------+
   | 31        19 | 18 17 16  | 15 | 14 | 13 | 12 | 11 10 | 9 | 8 | 7 | 6  3 | 2 | 1 | 0 |
   |    SBZ       | O  Z   O  | L4 | RR | V  | Z  |  SBZ  | R | S | B |  SBO | C | A | M |
   +-------------------------------------------------------------------------------------+  

2. Translation table base register 2
   +-----------------------------------------+
   | 31             14 | 13                0 |
   |      TTBR         |     UNP/SBZ         |
   +-----------------------------------------+  

3. Domain access control register 3

	There are 16 domains, each has 2 bit in this register, indicate the access permission
	2 BIT   meaning
	00  No access	Any access generates a domain fault
	01  Client		Accesses are checked against the access permission bits in the section or page descriptor
	10  Reserved	Reserved. Currently behaves like the no access mode
	11  Manager		Accesses are not checked against the access permission bits so a permission
					fault cannot be generated
   +-----------------------------------------------+
   | 31 30 | 29 28 | 27  26 | ............. | 1  0 |
   |  D15  |  D14  |  D13   | ............. |  D0  |  
   +-----------------------------------------------+  
 
5. Fault state register 5
   The FSR is updated for alignment faults, external abort that occur 
   while the MMU is disabled 
   +------------------------------------------------+
   | 31               9 | 8 | 7         4 | 3     0 |
   |     UNP/SBZ        | 0 |   Domain    |  Status |
   +------------------------------------------------+    

6. Fault address register 6
   The FAR is only updated for data aborts, not for prefetch abort. The FAR is updated 
   for alignment faults, external abort that occur while the MMU is disabled.
   
7. Cache operation register 7
   Reading from CP15 r7 is UNP, with the exception of two test and clean operation

8. Tlb operation register 8
   Write only register.    

9. Cache lockdown and TCM operation register 9                 
   +--------------------------------------------------------+
   | 31              16 | 15           4 | 3     		 0  |
   |     UNP/SBZ        |       SBO      |  L bits 			|
   +--------------------------------------------------------+   
   +--------------------------------------------------------+
   | 31              12 | 11           6 | 5     2 | 1 | 0  |
   |    Base addr(phy)  |     SBZ/UNP    |  Size   | 0 | en |
   +--------------------------------------------------------+

10. TLB lockdown register 10
   +-------------------------------------------------+
   | 31        29 | 28     26 | 25             1 | 0 |
   |     SBZ      |  Victim   |    UNP/SBZ       | P |
   +-------------------------------------------------+   

11. 12. Accessing these registers causes unpredicated behavior

13. Process ID register 13
   +-------------------------------------------------+
   | 31            25 | 24                        0  |
   |     FCSE PID     |            SBZ               |
   +-------------------------------------------------+    
   +-------------------------------------------------+
   | 31                                           0  |
   |            context identifier                   |
   +-------------------------------------------------+   

14. Accessing these register is reserved

* ------------------------------------------------------------------------------------------*/ 


/* ------------------------------------------------------------------------------------------- 
附录2: ARM926EJ-s 的 mmu table 的说明 
	ARM 的MMU 单元的地址映射分为两类：section-mapped access 和 page-mapped access. 对于对于
	section-mapped access，映射的内存大小只有一种为 1M，而对于page-mapped accesses 有3种 大小，
	分别为large pages  small pages tiny pages.

	mmu First-level descriptor, 4096 word, 每一个word 对应 1M 的地址空间，
	以1M为单位，虚拟地址为表的索引，而对应的表的内容即为映射的物理地址和标记的组合，也就是说，
	虚拟空间从0x0开始，以1M为单位，每项连续地对应每个虚拟空间的映射的物理空间。4096项管理了4G的
	ARM地址空间。注意ARM Core 产生的地址总是指虚拟地址。
	
	这是第一级的映射表，
	根据BIT[1:0]的情况，可能是section descriptor, 则地址转换结束，或是page descriptor，
	则还要进行第二级的转换, 具体情况如下
	
	31				20 19           12 11 10 9 8 7 6 5 4 3 2 1 0
   +------------------+---------------+-----+-+-------+---------+
   |                                                        |0|0| 错误
   +------------------------------------------------------------+
   |coarse page table base address          | |domain |1|   |0|1| coarse page table
   +------------------+---------------+-------------------------+
   |section Base addr |               | AP  | |domain |1|C|B|1|0| section 
   +------------------------------------------------------------+
   |fine page table base address      |       |domain |!|   |1|1| fine page table
   +------------------------------------------------------------+
	注意Bit[1:0]	表示了第一级descriptor 的性质
					11	-- 表明这个word是 fine page table descriptor，还要继续根据第二级的表转换
					10 	-- 表明这个word是 section descriptor,1M的大小 ，这种情况地址转换到此结束
						   前面的位的描述是针对section 的情况，而fine page 和coarse page有所不同
					01	-- 表明这个word是 coarse page table descriptor,还要继续根据第二级的表转换
					00	-- invalid
 
	
	如果BIT[1:0] = 10b , 则表示是一个Section descriptor,这种情况地址转换到此结束，一般情况下使用
	这种section descriptor较多，通常就把这个第一级的表叫做 section descriptor 表
	每个表项的意义为  refer to ARM926EJ-s core manual: DDI0198C_926_trm.pdf，其他fine page和
	coarse page的信息，也可参见这个文档
	Bit[31:20]  映射的物理地址的高12位, form the corresponding bits of the physical address for a section
	Bit[19:12]  应设为0
	Bit[11:10]  access permission for this section, the interpreting depending on R S bits of control 
				register c1
				BIT[11:10]	R	S	Previleged permissions		User permissions
				00			0	0	No Access					No Access
				00			0   1   Read-only					No Access		
				00			1   0   Read-only					Read-only		
				00			1   1   Unpredictable				Unpredictable
				01			x	x	Read/Write					No Access
				10 			x   x	Read/Write					Read-only
				11			x	x	Read/Write					Read/Write
	Bit[9]  	应设为0
	Bit[8:5]	domain number 0~15 ，cp15的domain access register 设置了每个domain的总access权限
				由于16个domain的存取权限不同，这样就意味着管理的各个空间有16种可能的存取权限选项
	Bit[4]		ARM core 规定必须设为1， 为保持向前兼容
	Bit[3:2]	BIT3= Cachable; BIT2 = bufferable. 
					11	-- write-back cachable, 
					10	-- write-through cachable, 
					01	-- noncached buffered, 
					00	-- noncached nonbuffered
	Bit[1:0]	应为10 	-- 表明这个word是 section descriptor,1M的大小 ，这种情况地址转换到此结束
* ----------------------------------------------------------------------------------------------------*/
   
