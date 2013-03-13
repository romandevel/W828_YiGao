
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
/*------------------------------------------------------------------------------
   Project include files:
   ---------------------------------------------------------------------------*/
#include "BoardSupportPackage.h"


/*------------------------------------------------------------------------------
   Defines : other configurations
   ---------------------------------------------------------------------------*/

/* total persistent parameter storage size (bytes)
*/
#define PERSISTENT_STORAGE_TOTAL_SIZE   (2048)



/*------------------------------------------------------------------------------
   Local Types:
   ---------------------------------------------------------------------------*/

/* System Mode Configuration Table
**
** This table  defines the minimal read and write wait states for the NAND flash
** for a specified frequency.  Entries should be in order of ascending frequency
** for optimal results.  This table has been optimised for  the pnx0101 refernce
** design NAND Flash.
** Other settings could be necessary when using different [slower] types of
** NAND Flashes.
**
** REMARK : DON'T forget to change the size of `gkhsaBsp_smcNandConfigTable`
**          declaration line in `BoardSupportPackage.h` when you add extra
**          settings.
*/
const hsaBsp_SmcConfiguration_t gkhsaBsp_smcNandConfigTable[8] =
{
   { 12000000, 10, 10},        /* 12 Mhz                              */
   { 16000000, 10, 10},        /* 16 Mhz, untested for read and write */
   { 20000000, 10, 10},        /* 20 Mhz                              */
   { 24000000, 10, 10},        /* 24 Mhz, untested for read and write */
   { 30000000, 10, 10},        /* 30 Mhz                              */
   { 48000000, 10, 10},        /* 48 Mhz,                             */
   { 60000000, 10, 10},        /* 60 Mhz, untested for write          */
   {120000000, 10, 10},        /* 60 Mhz, Insane, safe value          */
};

/*
  Volume storage table
*/


/*------------------------------------------------------------------------------
   Global variables : USB configuration
   ---------------------------------------------------------------------------*/

// v0.19 
/* Portable Device Unit ID (PDUID) */
const unsigned char PDUID[PDUID_TOTALLENGTH] =
{
    0x35, 0x30, 0x39, 0x35, 0x4d, 0x33, 0x54, 0x31, /* ARBITRARY unit serial number          */
    0x30, 0x30, 0x35, 0x30, 0x30, 0x39, 0x35, 0x54  /* ARBITRARY unit serial number (cont'd) */
};

const unsigned char * PDUID_START_ADDRESS = PDUID;

/*------------------------------------------------------------------------------
   Global variables : upload protection
   ---------------------------------------------------------------------------*/

const UploadProtection_st uploadProtection =
{
    NULL,       /* Directory to upload protect (max 1 level): "C:/" for root, NULL for none */
    "WAVMP3",   /* File extensions to upload protect (UPPER CASE): NULL for all */
    4           /* Number of allowed clusters per upload protected file */
};

/*------------------------------------------------------------------------------
   Global variables : other variables
   ---------------------------------------------------------------------------*/

int persistentStorageTotalSize = PERSISTENT_STORAGE_TOTAL_SIZE;

//Restore the interrupts using the CPSR flags 

void InterruptRestore( UInt8 interrupt_level)
{
	__asm
	{
		MRS R1,CPSR
		AND R0, R0, #0x3
		MOV R0, R0, LSL #0x6
		BIC R1,R1,R0
		MSR CPSR_c,R1
	};
}

// Disable the interrupts using the CPSR flags 
UInt8 InterruptDisable( void )
{
	register UInt8 i;
	
	__asm
	{
		MRS R1,CPSR
		MVN i, R1, LSR #6
		AND i, i, #3
		ORR R1,R1,#0xC0
		MSR CPSR_c,R1
	};
	return i;
}
