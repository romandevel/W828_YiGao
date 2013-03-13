
#ifndef	_HYHW_ARM926EJS_H_
#define	_HYHW_ARM926EJS_H_




/*	cp15的控制寄存器 c1 的位定义 */
#define ARM926EJS_CP15C1_BIT_RESERVE		0x00050078		/*BIT[18:16],BIT[6:3] reserve 的bit 的取值的组合*/
#define ARM926EJS_CP15C1_BIT_ROUNDROBIN		BIT14			/* Replacement strategy for ICache and DCache:
																0 = Random replacement
																1 = Round-robin replacement. round-robin replacement */
#define ARM926EJS_CP15C1_BIT_ICACHE_ENABLE	BIT12
#define ARM926EJS_CP15C1_BIT_ROM_PROTECT	BIT9
#define ARM926EJS_CP15C1_BIT_SYS_PROTECT	BIT8
#define ARM926EJS_CP15C1_BIT_DCACHE_ENABLE	BIT2
#define ARM926EJS_CP15C1_BIT_MMU_ENABLE		BIT0


/*	data Tightly coupled memory size 的定义*/
#define ARM926EJS_DTCM_SIZE_0K				0x00000000
#define ARM926EJS_DTCM_SIZE_4K				0x0000000C
#define ARM926EJS_DTCM_SIZE_8K				0x00000010
#define ARM926EJS_DTCM_SIZE_16K				0x00000014
#define ARM926EJS_DTCM_SIZE_32K				0x00000018
#define ARM926EJS_DTCM_SIZE_64K				0x0000001C
#define ARM926EJS_DTCM_SIZE_128K			0x00000020
#define ARM926EJS_DTCM_SIZE_256K			0x00000024
#define ARM926EJS_DTCM_SIZE_512K			0x00000028
#define ARM926EJS_DTCM_SIZE_1M				0x0000002C

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_readFaultStatus
* 功能:	通过读取协处理器的Fault state register c5, 得到出错的状态
* 参数:	none
* 返回:	出错的状态
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultStatus(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_readFaultAddress
* 功能:	通过读取协处理器的Fault Address register c6, 得到出错的地址
* 参数:	none
* 返回:	出错的地址
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultAddress(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_drainWriteBuffer
* 功能:	将write buffer的内容强制回写到物理空间 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_drainWriteBuffer(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_updateControl
* 功能:	更新cp15的控制寄存器 c1的值
* 参数:	val		-- 要写入到控制寄存器 c1的值
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_updateControl(U32 val);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_enableDcache
* 功能:	使能 data cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableDcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_disableDcache
* 功能:	禁止 data cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableDcache(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_enableIcache
* 功能:	使能 Instruct cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableIcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_disableIcache
* 功能:	禁止 Instruct cache 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableIcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidateDcache
* 功能:	使 Data cache 无效 
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateDcache(void);
/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidateIcache
* 功能:	使 Instruct cache 的指令无效
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateIcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_invalidIcacheDcache
* 功能:	使 Instruct cache 和 Data cache 无效
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidIcacheDcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926e_writeBackDcache
* 功能:	回写Data Cache数据
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926e_writeBackDcache(void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuEnable
* 功能:	使能MMU
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuEnable (void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuEnable
* 功能:	禁止MMU
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuDisable (void);

/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_mmuConfig
* 功能:	配置arm926ej的mmu，通过协处理器cp15完成
* 参数:	sectionTable	-- 指向mmu 的section table 的地址
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuConfig(int *sectionTable);


/*-----------------------------------------------------------------------------
* 函数:	hyhwArm926ejs_dtcmConfig
* 功能:	TCM (Tightly Coupled Memory) 的控制
* 参数:	baseAddr	-- data TCM memory 的基地址
*		sizebit		-- 表示TCM大小的定义，注意使bit位的表示
*		enable		-- 是否enable 0-diable， 1- enable
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_dtcmConfig(U32 baseAddr, U32 sizebit, U32 enable);




#endif  /* _HYHW_MMU_H_  */
