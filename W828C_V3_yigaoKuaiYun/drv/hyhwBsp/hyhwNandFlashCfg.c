/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwNandflash.h"
//#include "hyswNandFile.h"

//#ifdef NAND_USE_SLC
#define NAND_RESERV_SIZE			(430)//(1536)//1.5G//(254)//(200)	/* 单位为M字节*/
//#else
//#define NAND_RESERV_SIZE			(1536)
//#endif

#define NAND_WRITE_PROTECT			0		/* GPIO_5 GPIO口已经被占用 Nand写保护置为NULL */

/* 注：shark只有一组GPIO，不用分unit。由于片选也可以输出低电平（类似于IO），所以不用接在IO上。*/
const InternalStorageMedia_st internalStorageMedia =
{
    {
        0,			/* Don't care( Gpio_nCE NAND Flash in bank 0) */

        NAND_WRITE_PROTECT,		/* GPIO mask :MD_DFU(Gpio_nWP NAND Flash in bank 0) */

        0x03000000, /* NAND Flash bank 2 data addres */
        0x03040000, /* NAND Flash bank 2 command line enable address */
        0x03080000, /* NAND Flash bank 2 address line enable address */
        2  			/* MPMC bank number for NAND Flash bank 0        */
    },

    {
        0,			/* Don't care( Gpio_nCE NAND Flash in bank 1) */

        NAND_WRITE_PROTECT,		/* GPIO mask :MD_DFU(Gpio_nWP NAND Flash in bank 0) */

        0x02000000,	/* NAND Flash bank 1 data addres */
        0x02040000, /* NAND Flash bank 1 command line enable address */
        0x02080000,	/* NAND Flash bank 1 address line enable address */
        1 			/* MPMC bank number for NAND Flash bank 1        */
    },

    NAND_RESERV_SIZE		/* reserved空间，单位为M字节 */
};
