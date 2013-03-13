
#ifndef	_HYHW_MMU_H_
#define	_HYHW_MMU_H_


/* Virtual MMU Register Define  */
#define TCC8300_MMU_BASEADDR			0x8000C000

#define TCC8300_MMU_REGION0_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR))		/* R/W, Configuration Register for Region 0 */
#define TCC8300_MMU_REGION1_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x4))	/* R/W, Configuration Register for Region 1 */
#define TCC8300_MMU_REGION2_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x8))	/* R/W, Configuration Register for Region 2 */
#define TCC8300_MMU_REGION3_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0xC))	/* R/W, Configuration Register for Region 3 */
#define TCC8300_MMU_REGION4_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x10))	/* R/W, Configuration Register for Region 4 */
#define TCC8300_MMU_REGION5_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x14))	/* R/W, Configuration Register for Region 5 */
#define TCC8300_MMU_REGION6_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x18))	/* R/W, Configuration Register for Region 6 */
#define TCC8300_MMU_REGION7_REGISTER	*((volatile U32 *)(TCC8300_MMU_BASEADDR +0x1C))	/* R/W, Configuration Register for Region 7 */


#define	TCC8300_MMU_DESCRIPT_BASEADDR	0x8001C000											/* MMU Table Base Address  */
#define TCC8300_MMU_DESCRIPT_REGISTER	*((volatile U32 *)(TCC8300_MMU_DESCRIPT_BASEADDR)	/* R, MMU Table Base Register  */



/* MMU 部分的寄存器定义 */
/*	REGION7 优先级最高，	REGION0优先级最低，这就表示，如果两个REGION有重叠，则以优先级高的为准
	由于REGION0的优先级最低，可以把所有的4G空间都设为Region0*/
#define MMU_REGION0_REGISTER			TCC8300_MMU_REGION0_REGISTER	/* R/W, Configuration Register for Region 0 */
#define MMU_REGION1_REGISTER			TCC8300_MMU_REGION1_REGISTER	/* R/W, Configuration Register for Region 1 */
#define MMU_REGION2_REGISTER			TCC8300_MMU_REGION2_REGISTER	/* R/W, Configuration Register for Region 2 */
#define MMU_REGION3_REGISTER			TCC8300_MMU_REGION3_REGISTER	/* R/W, Configuration Register for Region 3 */
#define MMU_REGION4_REGISTER			TCC8300_MMU_REGION4_REGISTER	/* R/W, Configuration Register for Region 4 */
#define MMU_REGION5_REGISTER			TCC8300_MMU_REGION5_REGISTER	/* R/W, Configuration Register for Region 5 */
#define MMU_REGION6_REGISTER			TCC8300_MMU_REGION6_REGISTER	/* R/W, Configuration Register for Region 6 */
#define MMU_REGION7_REGISTER			TCC8300_MMU_REGION7_REGISTER	/* R/W, Configuration Register for Region 7 */
	#define MMU_BIT_REGION_BASEADDR				0xFFF00000						/* BIT[31:20] start Address */
	#define MMU_BIT_REGION_SIZE					0x000FF000						/* BIT[19:12] size*/
	#define MMU_BIT_REGION_ACCESS_PERMIT		(BIT11|BIT10)					/* BIT[11:10] access permission */
	#define	MMU_BIT_REGION_ENABLE				BIT9							/* Region Enable Register					*/
	#define MMU_BIT_REGION_DOMAIN				0x000001E0						/* BIT[8:5] domain */
	#define	MMU_BIT_REGION_CACHEABLE			BIT3							/* Cacheable Register						*/
	#define	MMU_BIT_REGION_BUFFERABLE			BIT2							/* Bufferable Register						*/

	/*	size 的具体定义  BIT[19:12] size*/
	#define	MMU_REGION_SIZE_4GB				0x0001F000
	#define	MMU_REGION_SIZE_2GB				0x0001E000
	#define	MMU_REGION_SIZE_1GB				0x0001D000
	#define	MMU_REGION_SIZE_512MB			0x0001C000
	#define	MMU_REGION_SIZE_256MB			0x0001B000
	#define	MMU_REGION_SIZE_128MB			0x0001A000
	#define	MMU_REGION_SIZE_64MB			0x00019000
	#define	MMU_REGION_SIZE_32MB			0x00018000
	#define	MMU_REGION_SIZE_16MB			0x00017000
	#define	MMU_REGION_SIZE_8MB				0x00016000
	#define	MMU_REGION_SIZE_4MB				0x00015000
	#define	MMU_REGION_SIZE_2MB				0x00014000
	#define	MMU_REGION_SIZE_1MB				0x00013000
	
	/*	access permission 的具体定义 */
	#define	MMU_REGION_AP_MANAGER			(BIT11|BIT10)
	#define	MMU_REGION_AP_CLIENT			BIT10
	#define MMU_REGION_AP_NOACCESS			0x0
	
	/*  domain 的具体定义 BIT[8:5] domain*/
	#define	MMU_REGION_DOMAIN(x)			(x<<5)	

	
/*	Tcc8300 提供了一个 section descriptor 表，4096个word，这个table的首地址为寄存器MMU_DESCRIPT_REGISTER的地址， 
	管理4G空间，管理的最小空间是1M  1M * 4096 = 4G  */
#define	MMU_DESCRIPT_BASEADDR			TCC8300_MMU_DESCRIPT_BASEADDR	
#define MMU_DESCRIPT_REGISTER			TCC8300_MMU_DESCRIPT_REGISTER	




#endif  /* _HYHW_MMU_H_  */
