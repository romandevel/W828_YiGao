
/******************** Jerry xiang /2012-8*************************************
	

 ******************************************************************************/


#ifndef	__MMA8452_H__
#define	__MMA8452_H__

#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"

#include "hyhwIIC.h"

#define	MMA8452_ACC_DEV_NAME	"mma8452"

#define MMA8452_IIC_CHANNEL		I2C_CH0
/* SAO pad is connected to GND, set LSB of SAD '0' */
#define MMA8452_ACC_I2C_ADDR     0x38
#define MMA8452_ACC_I2C_NAME     MMA8452_ACC_DEV_NAME

#define	MMA8452_ACC_IOCTL_BASE 77
/** The following define the IOCTL command values via the ioctl macros */
#define	MMA8452_ACC_IOCTL_SET_DELAY		_IOW(MMA8452_ACC_IOCTL_BASE, 0, int)
#define	MMA8452_ACC_IOCTL_GET_DELAY		_IOR(MMA8452_ACC_IOCTL_BASE, 1, int)
#define	MMA8452_ACC_IOCTL_SET_ENABLE		_IOW(MMA8452_ACC_IOCTL_BASE, 2, int)
#define	MMA8452_ACC_IOCTL_GET_ENABLE		_IOR(MMA8452_ACC_IOCTL_BASE, 3, int)
#define	MMA8452_ACC_IOCTL_SET_FULLSCALE		_IOW(MMA8452_ACC_IOCTL_BASE, 4, int)
#define	MMA8452_ACC_IOCTL_SET_G_RANGE		MMA8452_ACC_IOCTL_SET_FULLSCALE

#define	MMA8452_ACC_IOCTL_SET_CTRL_REG3		_IOW(MMA8452_ACC_IOCTL_BASE, 6, int)
#define	MMA8452_ACC_IOCTL_SET_CTRL_REG6		_IOW(MMA8452_ACC_IOCTL_BASE, 7, int)
#define	MMA8452_ACC_IOCTL_SET_DURATION1		_IOW(MMA8452_ACC_IOCTL_BASE, 8, int)
#define	MMA8452_ACC_IOCTL_SET_THRESHOLD1		_IOW(MMA8452_ACC_IOCTL_BASE, 9, int)
#define	MMA8452_ACC_IOCTL_SET_CONFIG1		_IOW(MMA8452_ACC_IOCTL_BASE, 10, int)

#define	MMA8452_ACC_IOCTL_SET_DURATION2		_IOW(MMA8452_ACC_IOCTL_BASE, 11, int)
#define	MMA8452_ACC_IOCTL_SET_THRESHOLD2		_IOW(MMA8452_ACC_IOCTL_BASE, 12, int)
#define	MMA8452_ACC_IOCTL_SET_CONFIG2		_IOW(MMA8452_ACC_IOCTL_BASE, 13, int)

#define	MMA8452_ACC_IOCTL_GET_SOURCE1		_IOW(MMA8452_ACC_IOCTL_BASE, 14, int)
#define	MMA8452_ACC_IOCTL_GET_SOURCE2		_IOW(MMA8452_ACC_IOCTL_BASE, 15, int)

#define	MMA8452_ACC_IOCTL_GET_TAP_SOURCE		_IOW(MMA8452_ACC_IOCTL_BASE, 16, int)

#define	MMA8452_ACC_IOCTL_SET_TAP_TW		_IOW(MMA8452_ACC_IOCTL_BASE, 17, int)
#define	MMA8452_ACC_IOCTL_SET_TAP_CFG		_IOW(MMA8452_ACC_IOCTL_BASE, 18, int)
#define	MMA8452_ACC_IOCTL_SET_TAP_TLIM		_IOW(MMA8452_ACC_IOCTL_BASE, 19, int)
#define	MMA8452_ACC_IOCTL_SET_TAP_THS		_IOW(MMA8452_ACC_IOCTL_BASE, 20, int)
#define	MMA8452_ACC_IOCTL_SET_TAP_TLAT		_IOW(MMA8452_ACC_IOCTL_BASE, 21, int)

#define MMA8452_ACC_IOCTL_GET_COOR_XYZ           _IOW(MMA8452_ACC_IOCTL_BASE, 22, int)


/************************************************/
/* 	Accelerometer defines section	 	*/
/************************************************/

/* Accelerometer Sensor Full Scale */
#define	MMA8452_ACC_FS_MASK		0x30
#define MMA8452_ACC_G_2G 		0x00
#define MMA8452_ACC_G_4G 		0x10
#define MMA8452_ACC_G_8G 		0x20
#define MMA8452_ACC_G_16G		0x30


/* Accelerometer Sensor Operating Mode */
#define MMA8452_ACC_ENABLE		0x01
#define MMA8452_ACC_DISABLE		0x00

//#if defined(CONFIG_MACH_SP6810A)
#define I2C_BUS_NUM_STATIC_ALLOC
#define I2C_STATIC_BUS_NUM        (0)
//#endif

#endif	/* __MMA8452_H__ */



