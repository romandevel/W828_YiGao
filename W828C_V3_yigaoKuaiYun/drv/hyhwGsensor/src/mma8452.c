

/******************** Jerry xiang /2012-8*************************************
	

 ******************************************************************************/

#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"

#include "mma8452.h"
#include "hardWare_Resource.h"

#define MMA8452_DEBUG

#ifdef MMA8452_DEBUG
#define MMA8452_DEBUG_PRINTF	hyUsbPrintf
#else
#define MMA8452_DEBUG_PRINTF
#endif



#define	G_MAX		16000	/** Maximum polled-device-reported g value */



#define SENSITIVITY_2G		1	/**	mg/LSB	*/
#define SENSITIVITY_4G		2	/**	mg/LSB	*/
#define SENSITIVITY_8G		4	/**	mg/LSB	*/
#define SENSITIVITY_16G		12	/**	mg/LSB	*/


#define	HIGH_RESOLUTION		0x08

#define	AXISDATA_REG		0x28
#define WHOAMI_MMA8452_ACC	0x2A	/*	Expctd content for WAI	*/

/*	CONTROL REGISTERS	*/

#define MMA8452_REG_STATUS   	    	0x00 //RO
#define MMA8452_REG_X_OUT_MSB       	0x01 //RO
#define MMA8452_REG_X_OUT_LSB       	0x02 //RO
#define MMA8452_REG_Y_OUT_MSB       	0x03 //RO
#define MMA8452_REG_Y_OUT_LSB       	0x04 //RO
#define MMA8452_REG_Z_OUT_MSB       	0x05 //RO
#define MMA8452_REG_Z_OUT_LSB       	0x06 //RO
#define MMA8452_REG_F_SETUP		       	0x09 //RW

#define MMA8452_REG_SYSMOD				0x0B //RO
#define MMA8452_REG_INTSRC	    		0x0C //RO

#define MMA8452_REG_WHO_AM_I      		0x0D //RO
#define MMA8452_REG_XYZ_DATA_CFG		0x0E //RW

#define MMA8452_REG_HP_FILTER_CUTOFF	0x0F //RW
#define MMA8452_REG_PL_STATUS			0x10 //RO
#define MMA8452_REG_PL_CFG				0x11 //RW
#define MMA8452_REG_PL_COUNT			0x12 //RW
#define MMA8452_REG_PL_BF_ZCOMP			0x13 //RW
#define MMA8452_REG_P_L_THS_REG			0x14 //RW
#define MMA8452_REG_FF_MT_CFG			0x15 //RW
#define MMA8452_REG_FF_MT_SRC			0x16 //RO
#define MMA8452_REG_FF_MT_THS			0x17 //RW
#define MMA8452_REG_FF_MT_COUNT			0x18 //RW
#define MMA8452_REG_TRANSIENT_CFG		0x1D //RW
#define MMA8452_REG_TRANSIENT_SRC		0x1E //RO
#define MMA8452_REG_TRANSIENT_THS		0x1F //RW
#define MMA8452_REG_TRANSIENT_COUNT		0x20 //RW
#define MMA8452_REG_PULSE_CFG			0x21 //RW
#define MMA8452_REG_PULSE_SRC			0x22 //RO
#define MMA8452_REG_PULSE_THSX			0x23 //RW
#define MMA8452_REG_PULSE_THSY			0x24 //RW
#define MMA8452_REG_PULSE_THSZ			0x25 //RW
#define MMA8452_REG_PULSE_TMLT			0x26 //RW
#define MMA8452_REG_PULSE_LTCY			0x27 //RW
#define MMA8452_REG_PULSE_WIND			0x28 //RW
#define MMA8452_REG_ASLP_COUNT			0x29 //RW


#define MMA8452_REG_CTRL_REG1			0x2A //RW
#define MMA8452_REG_CTRL_REG2			0x2B //RW
#define MMA8452_REG_CTRL_REG3			0x2C //RW
#define MMA8452_REG_CTRL_REG4			0x2D //RW
#define MMA8452_REG_CTRL_REG5			0x2E //RW



#define MMA8452_REG_OFF_X				0x2F //RW
#define MMA8452_REG_OFF_Y				0x30 //RW
#define MMA8452_REG_OFF_Z				0x31 //RW

/*	end CONTROL REGISTRES	*/


#define ENABLE_HIGH_RESOLUTION	1

#define MMA8452_ACC_PM_OFF		0x00
#define MMA8452_ACC_ENABLE_ALL_AXES	0x07

#define PMODE_MASK			0x08
#define ODR_MASK			0XF0

#define ODR1		0x10  /* 1Hz output data rate */
#define ODR10		0x20  /* 10Hz output data rate */
#define ODR25		0x30  /* 25Hz output data rate */
#define ODR50		0x40  /* 50Hz output data rate */
#define ODR100		0x50  /* 100Hz output data rate */
#define ODR200		0x60  /* 200Hz output data rate */
#define ODR400		0x70  /* 400Hz output data rate */
#define ODR1250		0x90  /* 1250Hz output data rate */



#define	IA			0x40
#define	ZH			0x20
#define	ZL			0x10
#define	YH			0x08
#define	YL			0x04
#define	XH			0x02
#define	XL			0x01
/* */
/* CTRL REG BITS*/
#define	CTRL_REG3_I1_AOI1	0x40
#define	CTRL_REG6_I2_TAPEN	0x80
#define	CTRL_REG6_HLACTIVE	0x02
/* */

/* TAP_SOURCE_REG BIT */
#define	DTAP			0x20
#define	STAP			0x10
#define	SIGNTAP			0x08
#define	ZTAP			0x04
#define	YTAP			0x02
#define	XTAZ			0x01


#define	FUZZ			32
#define	FLAT			32
#define	I2C_RETRY_DELAY		5
#define	I2C_RETRIES		5
#define	I2C_AUTO_INCREMENT	0x80

/* RESUME STATE INDICES */
#define	RES_CTRL_REG1		0
#define	RES_CTRL_REG2		1
#define	RES_CTRL_REG3		2
#define	RES_CTRL_REG4		3
#define	RES_CTRL_REG5		4
#define	RES_CTRL_REG6		5

#define	RES_INT_CFG1		6
#define	RES_INT_THS1		7
#define	RES_INT_DUR1		8
#define	RES_INT_CFG2		9
#define	RES_INT_THS2		10
#define	RES_INT_DUR2		11

#define	RES_TT_CFG		12
#define	RES_TT_THS		13
#define	RES_TT_LIM		14
#define	RES_TT_TLAT		15
#define	RES_TT_TW		16

#define	RES_TEMP_CFG_REG	17
#define	RES_REFERENCE_REG	18
#define	RES_FIFO_CTRL_REG	19

#define	RESUME_ENTRIES		20
/* end RESUME STATE INDICES */


struct {
	unsigned int cutoff_ms;
	unsigned int mask;
} mma8452_acc_odr_table[] = {
			{ 1, ODR1250 },
			{ 3, ODR400 },
			{ 5, ODR200 },
			{ 10, ODR100 },
			{ 20, ODR50 },
			{ 40, ODR25 },
			{ 100, ODR10 },
			{ 1000, ODR1 },
};

//U8 mma8452_initFlag = 0;

static int mma8452_acc_i2c_read(U8* buf, int len)
{
	U8 reg = buf[0];
	//if (mma8452_initFlag != 0)
	{
		if (buf==NULL || len==0) return HY_ERROR;
		if (hyhwI2c_readData(MMA8452_IIC_CHANNEL, 		//IIC CHN
							MMA8452_ACC_I2C_ADDR, 		//SLAVE ADDR
							reg, 						//REG ADDR
							&buf[0], 					//DATA BUF
							len)						//READ LEN
							== HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	return HY_OK;
}

static int mma8452_acc_i2c_write(U8* buf, int len)
{
	//if (mma8452_initFlag != 0)
	{
		if (hyhwI2c_writeData(MMA8452_IIC_CHANNEL, 		//IIC CHN
							MMA8452_ACC_I2C_ADDR, 		//SLAVE ADDR
							buf[0], 					//REG ADDR
							&buf[1], 					//DATA BUF
							len)						//WR DATA LEN
							== HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	
	return HY_OK;
}

int mma8452_acc_hw_init(void)
{
	int err = HY_ERROR;
	U8 buf[7];

	hyhwGpio_setAsGpio(G_SENSOR_INT1_PORT, G_SENSOR_INT1_GPIO);
	hyhwGpio_setIn(G_SENSOR_INT1_PORT, G_SENSOR_INT1_GPIO);
	hyhwGpio_setAsGpio(G_SENSOR_INT2_PORT, G_SENSOR_INT2_GPIO);
	hyhwGpio_setIn(G_SENSOR_INT2_PORT, G_SENSOR_INT2_GPIO);
	
	MMA8452_DEBUG_PRINTF("%s: hw init start\r\n", MMA8452_ACC_DEV_NAME);

	buf[0] = MMA8452_REG_WHO_AM_I;
	err = mma8452_acc_i2c_read(buf, 1);
	if (err != HY_OK)
	{
		MMA8452_DEBUG_PRINTF("%s: hw init err\r\n", MMA8452_ACC_DEV_NAME);
		return HY_ERROR;
	}
	
	if (buf[0] != WHOAMI_MMA8452_ACC)
	{
		/* choose the right coded error */
		MMA8452_DEBUG_PRINTF("%s: hw device code = %d\r\n", MMA8452_ACC_DEV_NAME, buf[0]);
		return HY_ERROR;
	}

	buf[0] = (MMA8452_REG_CTRL_REG1);
	buf[1] = 0x01 ;
	err = mma8452_acc_i2c_write(buf, 1);
	if (err != HY_OK)
	{
		MMA8452_DEBUG_PRINTF("%s: hw init done\r\n", MMA8452_ACC_DEV_NAME);
		return HY_ERROR;
	}

	MMA8452_DEBUG_PRINTF("%s: hw init done\r\n", MMA8452_ACC_DEV_NAME);

	return 0;
}


#if 0

static void mma8452_acc_device_power_off(struct mma8452_acc_data *acc)
{
	int err;
	u8 buf[2] = { MMA8452_REG_CTRL_REG1, MMA8452_ACC_PM_OFF };

	err = mma8452_acc_i2c_write(acc, buf, 1);
	if (err != HY_OK)
		dev_err(&acc->client->dev, "soft power off failed: %d\r\n", err);
}

static int mma8452_acc_device_power_on(struct mma8452_acc_data *acc)
{
	int err = -1;

	if (acc->pdata->power_on) {
		err = acc->pdata->power_on();
		if (err < 0) {
			dev_err(&acc->client->dev,
					"power_on failed: %d\r\n", err);
			return err;
		}
//		enable_irq(acc->irq1);
//		enable_irq(acc->irq2);
	}

	if (!acc->hw_initialized) {
		err = mma8452_acc_hw_init(acc);
		if (acc->hw_working == 1 && err < 0) {
			mma8452_acc_device_power_off(acc);
			return err;
		}
	}

#if 0
	if (acc->hw_initialized) {
		enable_irq(acc->irq1);
		enable_irq(acc->irq2);
		MMA8452_DEBUG_PRINTF("%s: power on: irq enabled\r\n",
						MMA8452_ACC_DEV_NAME);
	}
#endif    
	return 0;
}

#if CONFIG_ARCH_8810
static irqreturn_t mma8452_acc_isr1(int irq, void *dev)
{
	struct mma8452_acc_data *acc = dev;

	disable_irq_nosync(irq);
	queue_work(acc->irq1_work_queue, &acc->irq1_work);
	MMA8452_DEBUG_PRINTF("%s: isr1 queued\r\n", MMA8452_ACC_DEV_NAME);

	return IRQ_HANDLED;
}

static irqreturn_t mma8452_acc_isr2(int irq, void *dev)
{
	struct mma8452_acc_data *acc = dev;

	disable_irq_nosync(irq);
	queue_work(acc->irq2_work_queue, &acc->irq2_work);
	MMA8452_DEBUG_PRINTF("%s: isr2 queued\r\n", MMA8452_ACC_DEV_NAME);

	return IRQ_HANDLED;
}


static void mma8452_acc_irq1_work_func(struct work_struct *work)
{

	struct mma8452_acc_data *acc =
	container_of(work, struct mma8452_acc_data, irq1_work);
	/* TODO  add interrupt service procedure.
		 ie:mma8452_acc_get_int1_source(acc); */
	;
	/*  */
	MMA8452_DEBUG_PRINTF("%s: IRQ1 triggered\r\n", MMA8452_ACC_DEV_NAME);
}

static void mma8452_acc_irq2_work_func(struct work_struct *work)
{

	struct mma8452_acc_data *acc =
	container_of(work, struct mma8452_acc_data, irq2_work);
	/* TODO  add interrupt service procedure.
		 ie:mma8452_acc_get_tap_source(acc); */
	;
	/*  */

	MMA8452_DEBUG_PRINTF("%s: IRQ2 triggered\r\n", MMA8452_ACC_DEV_NAME);
}




int mma8452_acc_update_g_range(struct mma8452_acc_data *acc, u8 new_g_range)
{
	int err;

	u8 sensitivity;
	u8 buf[2];
	u8 updated_val;
	u8 init_val;
	u8 new_val;
	u8 mask = MMA8452_ACC_FS_MASK | HIGH_RESOLUTION;
    
        MMA8452_DEBUG_PRINTF("%s\r\n", __func__);

	switch (new_g_range) {
	case MMA8452_ACC_G_2G:

		sensitivity = SENSITIVITY_2G;
		break;
	case MMA8452_ACC_G_4G:

		sensitivity = SENSITIVITY_4G;
		break;
	case MMA8452_ACC_G_8G:

		sensitivity = SENSITIVITY_8G;
		break;
	case MMA8452_ACC_G_16G:

		sensitivity = SENSITIVITY_16G;
		break;
	default:
		dev_err(&acc->client->dev, "invalid g range requested: %u\r\n",
				new_g_range);
		return -EINVAL;
	}

	if (atomic_read(&acc->enabled)) {
		/* Set configuration register 4, which contains g range setting
		 *  NOTE: this is a straight overwrite because this driver does
		 *  not use any of the other configuration bits in this
		 *  register.  Should this become untrue, we will have to read
		 *  out the value and only change the relevant bits --XX----
		 *  (marked by X) */
		buf[0] = CTRL_REG4;
		err = mma8452_acc_i2c_read(acc, buf, 1);
		if (err < 0)
			goto error;
		init_val = buf[0];
		acc->resume_state[RES_CTRL_REG4] = init_val;
		new_val = new_g_range | HIGH_RESOLUTION;
		updated_val = ((mask & new_val) | ((~mask) & init_val));
		buf[1] = updated_val;
		buf[0] = CTRL_REG4;
		err = mma8452_acc_i2c_write(acc, buf, 1);
		if (err < 0)
			goto error;
		acc->resume_state[RES_CTRL_REG4] = updated_val;
		acc->sensitivity = sensitivity;
                MMA8452_DEBUG_PRINTF("%s sensitivity %d g-range %d\r\n", __func__, sensitivity, new_g_range);
	}


	return 0;
error:
	dev_err(&acc->client->dev, "update g range failed 0x%x,0x%x: %d\r\n",
			buf[0], buf[1], err);

	return err;
}

#endif




int mma8452_acc_update_odr(struct mma8452_acc_data *acc, int poll_interval_ms)
{
	#if CONFIG_ARCH_8810


	int err = -1;
	int i;
	u8 config[2];

	/* Convert the poll interval into an output data rate configuration
	 *  that is as low as possible.  The ordering of these checks must be
	 *  maintained due to the cascading cut off values - poll intervals are
	 *  checked from shortest to longest.  At each check, if the next lower
	 *  ODR cannot support the current poll interval, we stop searching */
	for (i = ARRAY_SIZE(mma8452_acc_odr_table) - 1; i >= 0; i--) {
		if (mma8452_acc_odr_table[i].cutoff_ms <= poll_interval_ms)
			break;
	}
	config[1] = mma8452_acc_odr_table[i].mask;

	config[1] |= MMA8452_ACC_ENABLE_ALL_AXES;

	/* If device is currently enabled, we need to write new
	 *  configuration out to it */
	if (atomic_read(&acc->enabled)) {
		config[0] = CTRL_REG1;
		err = mma8452_acc_i2c_write(acc, config, 1);
		if (err < 0)
			goto error;
		acc->resume_state[RES_CTRL_REG1] = config[1];
	}

	return 0;

error:
	dev_err(&acc->client->dev, "update odr failed 0x%x,0x%x: %d\r\n",
			config[0], config[1], err);

	return err;

#endif

	
}

/* */

static int mma8452_acc_register_write(struct mma8452_acc_data *acc, u8 *buf,
		u8 reg_address, u8 new_value)
{
	int err = -1;

	if (atomic_read(&acc->enabled)) {
		/* Sets configuration register at reg_address
		 *  NOTE: this is a straight overwrite  */
		buf[0] = reg_address;
		buf[1] = new_value;
		err = mma8452_acc_i2c_write(acc, buf, 1);
		if (err < 0)
			return err;
	}
	return err;
}

static int mma8452_acc_register_read(struct mma8452_acc_data *acc, u8 *buf,
		u8 reg_address)
{

	int err = -1;
	buf[0] = (reg_address);
	err = mma8452_acc_i2c_read(acc, buf, 1);
	return err;
}

static int mma8452_acc_register_update(struct mma8452_acc_data *acc, u8 *buf,
		u8 reg_address, u8 mask, u8 new_bit_values)
{
	int err = -1;
	u8 init_val;
	u8 updated_val;
	err = mma8452_acc_register_read(acc, buf, reg_address);
	if (!(err < 0)) {
		init_val = buf[1];
		updated_val = ((mask & new_bit_values) | ((~mask) & init_val));
		err = mma8452_acc_register_write(acc, buf, reg_address,
				updated_val);
	}
	return err;
}

/* */

static int mma8452_acc_get_acceleration_data(struct mma8452_acc_data *acc,
		int *xyz)
{
	int err = -1;
	int i;
	/* Data bytes from hardware xL, xH, yL, yH, zL, zH */
	u8 acc_data[6];
	/* x,y,z hardware data */
	s16 hw_d[3] = { 0 };

	acc_data[0] = (0x01);
	err = mma8452_acc_i2c_read(acc, acc_data, 6);
#ifdef SIGNAL_CMD_TEST
        {
        u8  reg_cmd[2] = {0};
        
        reg_cmd[0] = 0x28;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[0] = reg_cmd[0];

        reg_cmd[0] = 0x29;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[1] = reg_cmd[0];

        reg_cmd[0] = 0x2A;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[2] = reg_cmd[0];

        reg_cmd[0] = 0x2B;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[3] = reg_cmd[0];

        reg_cmd[0] = 0x2C;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[4] = reg_cmd[0];

        reg_cmd[0] = 0x2D;
	err = mma8452_acc_i2c_read(acc, reg_cmd, 1);
        acc_data[5] = reg_cmd[0];
        }
#endif	
	
	if (err < 0)
        {
                MMA8452_DEBUG_PRINTF("%s I2C read error %d\r\n", MMA8452_ACC_I2C_NAME, err);
		return err;
        }
/*
	for (i=0;i<6;i++)
    {
       MMA8452_DEBUG_PRINTF( "++++++++++++++++acc_data[%d]=0x%x \r\n",i,acc_data[i]);

		}
*/
	hw_d[0] = (((s16) ((acc_data[0] << 8) | acc_data[1])) >> 4);
	hw_d[1] = (((s16) ((acc_data[2] << 8) | acc_data[3])) >> 4);
	hw_d[2] = (((s16) ((acc_data[4] << 8) | acc_data[5])) >> 4);


for (i=0;i<3;i++)
{
   MMA8452_DEBUG_PRINTF("*************************hw_d[%d]=0x%x \r\n",i,hw_d[i]);


	}

	


//        acc_data[0] = 0x28;
        #ifdef MMA8452_DEBUG
        MMA8452_DEBUG_PRINTF("///acc_data[1] = %d, acc_data[3]=%d, acc_data[5]=%d\r\n",
                acc_data[1], acc_data[3], acc_data[5]);       

        MMA8452_DEBUG_PRINTF("///acc_data[0] = %d, acc_data[2]=%d, acc_data[4]=%d\r\n",
                acc_data[0], acc_data[2], acc_data[4]);       
        #endif


	xyz[0] = ((acc->pdata->negate_x) ? (-hw_d[acc->pdata->axis_map_x])
		   : (hw_d[acc->pdata->axis_map_x]));
	xyz[1] = ((acc->pdata->negate_y) ? (-hw_d[acc->pdata->axis_map_y])
		   : (hw_d[acc->pdata->axis_map_y]));
	xyz[2] = ((acc->pdata->negate_z) ? (-hw_d[acc->pdata->axis_map_z])
		   : (hw_d[acc->pdata->axis_map_z]));

	#ifdef MMA8452_DEBUG
	
		MMA8452_DEBUG_PRINTF("%s read x=%d, y=%d, z=%d\r\n",
			MMA8452_ACC_DEV_NAME, xyz[0], xyz[1], xyz[2]);
		MMA8452_DEBUG_PRINTF("%s poll interval %d\r\n", MMA8452_ACC_DEV_NAME, acc->pdata->poll_interval);
	
	#endif
	return err;
}

static void mma8452_acc_report_values(struct mma8452_acc_data *acc, int *xyz)
{
	//MMA8452_DEBUG_PRINTF("++++++++++++++++++++x=%d,y=%d,z=%d\r\n",xyz[0],xyz[1],xyz[2]);
	input_report_abs(acc->input_dev, ABS_X, xyz[0]);
	input_report_abs(acc->input_dev, ABS_Y, xyz[1]);
	input_report_abs(acc->input_dev, ABS_Z, xyz[2]);
	input_sync(acc->input_dev);
}

static int mma8452_acc_enable(struct mma8452_acc_data *acc)
{
	int err;

	if (!atomic_cmpxchg(&acc->enabled, 0, 1)) {
		err = mma8452_acc_device_power_on(acc);
		if (err < 0) {
			atomic_set(&acc->enabled, 0);
			return err;
		}

	if (acc->hw_initialized) {
		if (acc->irq1 != 0) enable_irq(acc->irq1);
		if (acc->irq2 != 0) enable_irq(acc->irq2);
		MMA8452_DEBUG_PRINTF("%s: power on: irq enabled\r\n",
						MMA8452_ACC_DEV_NAME);
	}

		schedule_delayed_work(&acc->input_work, msecs_to_jiffies(
				acc->pdata->poll_interval));
	}

	return 0;
}

static int mma8452_acc_disable(struct mma8452_acc_data *acc)
{
	if (atomic_cmpxchg(&acc->enabled, 1, 0)) {
		cancel_delayed_work_sync(&acc->input_work);
		mma8452_acc_device_power_off(acc);
	}

	return 0;
}

static int mma8452_acc_misc_open(struct inode *inode, struct file *file)
{
	int err;
	err = nonseekable_open(inode, file);
	if (err < 0)
		return err;

	file->private_data = mma8452_acc_misc_data;

	return 0;
}

static int mma8452_acc_misc_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	u8 buf[4];
	u8 mask;
	u8 reg_address;
	u8 bit_values;
	int err;
	int interval;
        int xyz[3] = {0};
	struct mma8452_acc_data *acc = file->private_data;

//	MMA8452_DEBUG_PRINTF("%s: %s call with cmd 0x%x and arg 0x%x\r\n",
//			MMA8452_ACC_DEV_NAME, __func__, cmd, (unsigned int)arg);

	switch (cmd) {
	case MMA8452_ACC_IOCTL_GET_DELAY:
		interval = acc->pdata->poll_interval;
		if (copy_to_user(argp, &interval, sizeof(interval)))
			return -EFAULT;
		break;

	case MMA8452_ACC_IOCTL_SET_DELAY:
		
		if (copy_from_user(&interval, argp, sizeof(interval)))
			return -EFAULT;
		if (interval < 0 || interval > 1000)
			return -EINVAL;

		acc->pdata->poll_interval = max(interval,
				acc->pdata->min_interval);
		err = mma8452_acc_update_odr(acc, acc->pdata->poll_interval);
		/* TODO: if update fails poll is still set */
		if (err < 0)
			return err;
		break;

	case MMA8452_ACC_IOCTL_SET_ENABLE:
		if (copy_from_user(&interval, argp, sizeof(interval)))
			return -EFAULT;
		if (interval > 1)
			return -EINVAL;
		if (interval)
			err = mma8452_acc_enable(acc);
		else
			err = mma8452_acc_disable(acc);
		return err;
		break;

	case MMA8452_ACC_IOCTL_GET_ENABLE:
		interval = atomic_read(&acc->enabled);
		if (copy_to_user(argp, &interval, sizeof(interval)))
			return -EINVAL;
		break;



#ifdef INTERRUPT_MANAGEMENT
	case MMA8452_ACC_IOCTL_SET_CTRL_REG3:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = CTRL_REG3;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_CTRL_REG3] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_CTRL_REG3]));
		break;

	case MMA8452_ACC_IOCTL_SET_CTRL_REG6:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = CTRL_REG6;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_CTRL_REG6] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_CTRL_REG6]));
		break;

	case MMA8452_ACC_IOCTL_SET_DURATION1:
		if (copy_from_user(buf, argp, 1))
			return -EFAULT;
		reg_address = INT_DUR1;
		mask = 0x7F;
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_DUR1] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_DUR1]));
		break;

	case MMA8452_ACC_IOCTL_SET_THRESHOLD1:
		if (copy_from_user(buf, argp, 1))
			return -EFAULT;
		reg_address = INT_THS1;
		mask = 0x7F;
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_THS1] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_THS1]));
		break;

	case MMA8452_ACC_IOCTL_SET_CONFIG1:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = INT_CFG1;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_CFG1] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_CFG1]));
		break;

	case MMA8452_ACC_IOCTL_GET_SOURCE1:
		err = mma8452_acc_register_read(acc, buf, INT_SRC1);
		if (err < 0)
			return err;
#ifdef DEBUG
		MMA8452_DEBUG_PRINTF("INT1_SRC content: %d , 0x%x\r\n",
				buf[0], buf[0]);
#endif
		if (copy_to_user(argp, buf, 1))
			return -EINVAL;
		break;

	case MMA8452_ACC_IOCTL_SET_DURATION2:
		if (copy_from_user(buf, argp, 1))
			return -EFAULT;
		reg_address = INT_DUR2;
		mask = 0x7F;
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_DUR2] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_DUR2]));
		break;

	case MMA8452_ACC_IOCTL_SET_THRESHOLD2:
		if (copy_from_user(buf, argp, 1))
			return -EFAULT;
		reg_address = INT_THS2;
		mask = 0x7F;
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_THS2] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_THS2]));
		break;

	case MMA8452_ACC_IOCTL_SET_CONFIG2:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = INT_CFG2;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_INT_CFG2] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_INT_CFG2]));
		break;

	case MMA8452_ACC_IOCTL_GET_SOURCE2:
		err = mma8452_acc_register_read(acc, buf, INT_SRC2);
		if (err < 0)
			return err;
#ifdef DEBUG
		MMA8452_DEBUG_PRINTF("INT2_SRC content: %d , 0x%x\r\n",
				buf[0], buf[0]);
#endif
		if (copy_to_user(argp, buf, 1))
			return -EINVAL;
		break;

	case MMA8452_ACC_IOCTL_GET_TAP_SOURCE:
		err = mma8452_acc_register_read(acc, buf, TT_SRC);
		if (err < 0)
			return err;
#ifdef DEBUG
		MMA8452_DEBUG_PRINTF("TT_SRC content: %d , 0x%x\r\n",
				buf[0], buf[0]);
#endif
		if (copy_to_user(argp, buf, 1)) {
			MMA8452_DEBUG_PRINTF("%s: %s error in copy_to_user \r\n",
					MMA8452_ACC_DEV_NAME, __func__);
			return -EINVAL;
		}
		break;
        case MMA8452_ACC_IOCTL_GET_COOR_XYZ:           
	        err = mma8452_acc_get_acceleration_data(acc, xyz);
                if (err < 0)
                    return err;
#ifdef DEBUG
//                MMA8452_DEBUG_PRINTF("%s Get coordinate xyz:[%d, %d, %d]\r\n", 
//                        __func__, xyz[0], xyz[1], xyz[2]);
#endif
                if (copy_to_user(argp, xyz, sizeof(xyz))) {
			MMA8452_DEBUG_PRINTF(" %s %d error in copy_to_user \r\n", 
					 __func__, __LINE__);
			return -EINVAL;
                }
                break;
	case MMA8452_ACC_IOCTL_SET_TAP_CFG:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = TT_CFG;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_TT_CFG] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_TT_CFG]));
		break;

	case MMA8452_ACC_IOCTL_SET_TAP_TLIM:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = TT_LIM;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_TT_LIM] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_TT_LIM]));
		break;

	case MMA8452_ACC_IOCTL_SET_TAP_THS:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = TT_THS;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_TT_THS] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_TT_THS]));
		break;

	case MMA8452_ACC_IOCTL_SET_TAP_TLAT:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = TT_TLAT;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_TT_TLAT] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_TT_TLAT]));
		break;

	case MMA8452_ACC_IOCTL_SET_TAP_TW:
		if (copy_from_user(buf, argp, 2))
			return -EFAULT;
		reg_address = TT_TW;
		mask = buf[1];
		bit_values = buf[0];
		err = mma8452_acc_register_update(acc, (u8 *) arg, reg_address,
				mask, bit_values);
		if (err < 0)
			return err;
		acc->resume_state[RES_TT_TW] = ((mask & bit_values) |
				( ~mask & acc->resume_state[RES_TT_TW]));
		break;

#endif /* INTERRUPT_MANAGEMENT */

	default:
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations mma8452_acc_misc_fops = {
		.owner = THIS_MODULE,
		.open = mma8452_acc_misc_open,
		.ioctl = mma8452_acc_misc_ioctl,
};

static struct miscdevice mma8452_acc_misc_device = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = MMA8452_ACC_DEV_NAME,
		.fops = &mma8452_acc_misc_fops,
};

static void mma8452_acc_input_work_func(struct work_struct *work)
{
	struct mma8452_acc_data *acc;

	int xyz[3] = { 0 };
	int err;

	acc = container_of((struct delayed_work *)work,
			struct mma8452_acc_data,	input_work);

	mutex_lock(&acc->lock);
	err = mma8452_acc_get_acceleration_data(acc, xyz);
	if (err < 0)
		dev_err(&acc->client->dev, "get_acceleration_data failed\r\n");
	else
		mma8452_acc_report_values(acc, xyz);

	schedule_delayed_work(&acc->input_work, msecs_to_jiffies(
			acc->pdata->poll_interval));
	mutex_unlock(&acc->lock);
}

#ifdef MMA8452_OPEN_ENABLE
int mma8452_acc_input_open(struct input_dev *input)
{
	struct mma8452_acc_data *acc = input_get_drvdata(input);

	return mma8452_acc_enable(acc);
}

void mma8452_acc_input_close(struct input_dev *dev)
{
	struct mma8452_acc_data *acc = input_get_drvdata(dev);

	mma8452_acc_disable(acc);
}
#endif

static int mma8452_acc_validate_pdata(struct mma8452_acc_data *acc)
{
	acc->pdata->poll_interval = max(acc->pdata->poll_interval,
			acc->pdata->min_interval);

	if (acc->pdata->axis_map_x > 2 || acc->pdata->axis_map_y > 2
			|| acc->pdata->axis_map_z > 2) {
		dev_err(&acc->client->dev, "invalid axis_map value "
			"x:%u y:%u z%u\r\n", acc->pdata->axis_map_x,
				acc->pdata->axis_map_y, acc->pdata->axis_map_z);
		return -EINVAL;
	}

	/* Only allow 0 and 1 for negation boolean flag */
	if (acc->pdata->negate_x > 1 || acc->pdata->negate_y > 1
			|| acc->pdata->negate_z > 1) {
		dev_err(&acc->client->dev, "invalid negate value "
			"x:%u y:%u z:%u\r\n", acc->pdata->negate_x,
				acc->pdata->negate_y, acc->pdata->negate_z);
		return -EINVAL;
	}

	/* Enforce minimum polling interval */
	if (acc->pdata->poll_interval < acc->pdata->min_interval) {
		dev_err(&acc->client->dev, "minimum poll interval violated\r\n");
		return -EINVAL;
	}

	return 0;
}

static int mma8452_acc_input_init(struct mma8452_acc_data *acc)
{
	int err;
    // Polling rx data when the interrupt is not used.
    if (1/*acc->irq1 == 0 && acc->irq1 == 0*/) {
    	INIT_DELAYED_WORK(&acc->input_work, mma8452_acc_input_work_func);
    }

	acc->input_dev = input_allocate_device();
	if (!acc->input_dev) {
		err = -ENOMEM;
		dev_err(&acc->client->dev, "input device allocate failed\r\n");
		goto err0;
	}

#ifdef MMA8452_ACC_OPEN_ENABLE
	acc->input_dev->open = mma8452_acc_input_open;
	acc->input_dev->close = mma8452_acc_input_close;
#endif

	input_set_drvdata(acc->input_dev, acc);

	set_bit(EV_ABS, acc->input_dev->evbit);
	/*	next is used for interruptA sources data if the case */
	set_bit(ABS_MISC, acc->input_dev->absbit);
	/*	next is used for interruptB sources data if the case */
	set_bit(ABS_WHEEL, acc->input_dev->absbit);

	input_set_abs_params(acc->input_dev, ABS_X, -G_MAX, G_MAX, FUZZ, FLAT);
	input_set_abs_params(acc->input_dev, ABS_Y, -G_MAX, G_MAX, FUZZ, FLAT);
	input_set_abs_params(acc->input_dev, ABS_Z, -G_MAX, G_MAX, FUZZ, FLAT);
	/*	next is used for interruptA sources data if the case */
	input_set_abs_params(acc->input_dev, ABS_MISC, INT_MIN, INT_MAX, 0, 0);
	/*	next is used for interruptB sources data if the case */
	input_set_abs_params(acc->input_dev, ABS_WHEEL, INT_MIN, INT_MAX, 0, 0);

	acc->input_dev->name = "accelerometer";

	err = input_register_device(acc->input_dev);
	if (err) {
		dev_err(&acc->client->dev,
				"unable to register input polled device %s\r\n",
				acc->input_dev->name);
		goto err1;
	}

	return 0;

err1:
	input_free_device(acc->input_dev);
err0:
	return err;
}

static void mma8452_acc_input_cleanup(struct mma8452_acc_data *acc)
{
	input_unregister_device(acc->input_dev);
	input_free_device(acc->input_dev);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void mma8452_early_suspend (struct early_suspend* es);
static void mma8452_early_resume (struct early_suspend* es);
#endif


static int mma8452_acc_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{

	struct mma8452_acc_data *acc;

	int err = -1;
	int tempvalue;

	pr_info("%s: probe start.\r\n", MMA8452_ACC_DEV_NAME);

	if (client->dev.platform_data == NULL) {
		dev_err(&client->dev, "platform data is NULL. exiting.\r\n");
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "client not i2c capable\r\n");
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}

	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE |
					I2C_FUNC_SMBUS_BYTE_DATA |
					I2C_FUNC_SMBUS_WORD_DATA)) {
		dev_err(&client->dev, "client not smb-i2c capable:2\r\n");
		err = -EIO;
		goto exit_check_functionality_failed;
	}


	if (!i2c_check_functionality(client->adapter,
						I2C_FUNC_SMBUS_I2C_BLOCK)){
		dev_err(&client->dev, "client not smb-i2c capable:3\r\n");
		err = -EIO;
		goto exit_check_functionality_failed;
	}
	/*
	 * OK. From now, we presume we have a valid client. We now create the
	 * client structure, even though we cannot fill it completely yet.
	 */

	acc = kzalloc(sizeof(struct mma8452_acc_data), GFP_KERNEL);
	if (acc == NULL) {
		err = -ENOMEM;
		dev_err(&client->dev,
				"failed to allocate memory for module data: "
					"%d\r\n", err);
		goto exit_alloc_data_failed;
	}

	mutex_init(&acc->lock);
	mutex_lock(&acc->lock);

	acc->client = client;
    mma8452_i2c_client = client;
	i2c_set_clientdata(client, acc);

    #ifdef CONFIG_ARCH_8810
    acc->irq1 = sprd_alloc_eic_irq(MMA8452_INT1);
    acc->irq2 = sprd_alloc_eic_irq(MMA8452_INT2);
    #else
    acc->irq1 = 0;
    acc->irq2 = 0;
    #endif 

	MMA8452_DEBUG_PRINTF("%s: %s has set irq1 to irq: %d\r\n",
		MMA8452_ACC_DEV_NAME, __func__, acc->irq1);
	MMA8452_DEBUG_PRINTF("%s: %s has set irq2 to irq: %d\r\n",
		MMA8452_ACC_DEV_NAME, __func__, acc->irq2);
	
 #if  CONFIG_ARCH_8810        
    if (acc->irq1 != 0) {
        MMA8452_DEBUG_PRINTF("%s request irq1\r\n", __func__);
    	err = request_irq(acc->irq1, mma8452_acc_isr1, IRQF_TRIGGER_HIGH,
    			"mma8452_acc_irq1", acc);
    	if (err < 0) {
    		dev_err(&client->dev, "request irq1 failed: %d\r\n", err);
    		goto err_mutexunlockfreedata;
    	}
    	disable_irq_nosync(acc->irq1);
        
    	INIT_WORK(&acc->irq1_work, mma8452_acc_irq1_work_func);
    	acc->irq1_work_queue = create_singlethread_workqueue("mma8452_acc_wq1");
    	if (!acc->irq1_work_queue) {
    		err = -ENOMEM;
    		dev_err(&client->dev, "cannot create work queue1: %d\r\n", err);
    		goto err_free_irq1;
    	}
    }

    if (acc->irq2 != 0) {
    	err = request_irq(acc->irq2, mma8452_acc_isr2, IRQF_TRIGGER_HIGH,
    			"mma8452_acc_irq2", acc);
    	if (err < 0) {
    		dev_err(&client->dev, "request irq2 failed: %d\r\n", err);
    		goto err_destoyworkqueue1;
    	}
    	disable_irq_nosync(acc->irq2);

            // Create workqueue for IRQ.

    	INIT_WORK(&acc->irq2_work, mma8452_acc_irq2_work_func);
    	acc->irq2_work_queue = create_singlethread_workqueue("mma8452_acc_wq2");
    	if (!acc->irq2_work_queue) {
    		err = -ENOMEM;
    		dev_err(&client->dev, "cannot create work queue2: %d\r\n", err);
    		goto err_free_irq2;
    	}
    }
#endif 

#if 1 
	if (i2c_smbus_read_byte(client) < 0) {
		MMA8452_DEBUG_PRINTF("i2c_smbus_read_byte error!!\r\n");
		goto err_destoyworkqueue2;
	} else {
		MMA8452_DEBUG_PRINTF("%s Device detected!\r\n", MMA8452_ACC_DEV_NAME);
	}
#endif
	/* read chip id */
	tempvalue = i2c_smbus_read_word_data(client, MMA8452_REG_WHO_AM_I);

	if ((tempvalue & 0x00FF) == WHOAMI_MMA8452_ACC) {
		MMA8452_DEBUG_PRINTF("%s I2C driver registered!\r\n",
							MMA8452_ACC_DEV_NAME);
	} else {
		acc->client = NULL;
		MMA8452_DEBUG_PRINTF("I2C driver not registered!"
				" Device unknown 0x%x\r\n", tempvalue);
		goto err_destoyworkqueue2;
	}


	acc->pdata = kmalloc(sizeof(*acc->pdata), GFP_KERNEL);
	if (acc->pdata == NULL) {
		err = -ENOMEM;
		dev_err(&client->dev,
				"failed to allocate memory for pdata: %d\r\n",
				err);
		goto err_destoyworkqueue2;
	}

	memcpy(acc->pdata, client->dev.platform_data, sizeof(*acc->pdata));

	err = mma8452_acc_validate_pdata(acc);
	if (err < 0) {
		dev_err(&client->dev, "failed to validate platform data\r\n");
		goto exit_kfree_pdata;
	}
	
	i2c_set_clientdata(client, acc);


	if (acc->pdata->init) {
		err = acc->pdata->init();
		if (err < 0) {
			dev_err(&client->dev, "init failed: %d\r\n", err);
			goto err2;
		}
	}

	memset(acc->resume_state, 0, ARRAY_SIZE(acc->resume_state));


//	acc->resume_state[RES_TT_TW] = 0x00;

	err = mma8452_acc_device_power_on(acc);
	if (err < 0) {
		dev_err(&client->dev, "power on failed: %d\r\n", err);
		goto err2;
	}

	atomic_set(&acc->enabled, 1);

#if CONFIG_ARCH_8810	
	err = mma8452_acc_update_g_range(acc, acc->pdata->g_range);
	if (err < 0) {
		dev_err(&client->dev, "update_g_range failed\r\n");
		goto  err_power_off;
	}

	err = mma8452_acc_update_odr(acc, acc->pdata->poll_interval);
	if (err < 0) {
		dev_err(&client->dev, "update_odr failed\r\n");
		goto  err_power_off;
	}
#endif
	err = mma8452_acc_input_init(acc);
	if (err < 0) {
		dev_err(&client->dev, "input init failed\r\n");
		goto err_power_off;
	}
	mma8452_acc_misc_data = acc;

	err = misc_register(&mma8452_acc_misc_device);
	if (err < 0) {
		dev_err(&client->dev,
				"misc MMA8452_ACC_DEV_NAME register failed\r\n");
		goto err_input_cleanup;
	}

	mma8452_acc_device_power_off(acc);

	/* As default, do not report information */
	atomic_set(&acc->enabled, 0);

    acc->on_before_suspend = 0;
	
 #ifdef CONFIG_HAS_EARLYSUSPEND
    acc->early_suspend.suspend = mma8452_early_suspend;
    acc->early_suspend.resume  = mma8452_early_resume;
    acc->early_suspend.level   = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    register_early_suspend(&acc->early_suspend);
#endif

	mutex_unlock(&acc->lock);

	dev_info(&client->dev, "%s: probed\r\n", MMA8452_ACC_DEV_NAME);


	return 0;

err_input_cleanup:
	mma8452_acc_input_cleanup(acc);
err_power_off:
	mma8452_acc_device_power_off(acc);
err2:
	if (acc->pdata->exit) acc->pdata->exit();
exit_kfree_pdata:
	kfree(acc->pdata);
err_destoyworkqueue2:
 	destroy_workqueue(acc->irq2_work_queue);
err_free_irq2:
	if (acc->irq2) free_irq(acc->irq2, acc);
err_destoyworkqueue1:
	destroy_workqueue(acc->irq1_work_queue);
err_free_irq1:
	if (acc->irq1) free_irq(acc->irq1, acc);
err_mutexunlockfreedata:
    #ifdef CONFIG_ARCH_8810
    sprd_free_eic_irq(acc->irq1);
    sprd_free_eic_irq(acc->irq2);
    #endif 

	kfree(acc);
 	mutex_unlock(&acc->lock);
    i2c_set_clientdata(client, NULL);
    mma8452_acc_misc_data = NULL;
exit_alloc_data_failed:
exit_check_functionality_failed:
	MMA8452_DEBUG_PRINTF("%s: Driver Init failed\r\n", MMA8452_ACC_DEV_NAME);
	return err;
}

static int __devexit mma8452_acc_remove(struct i2c_client *client)
{
	/* TODO: revisit ordering here once _probe order is finalized */
	MMA8452_DEBUG_PRINTF("mma8452_acc_remove+++++++++++++++\r\n");
	struct mma8452_acc_data *acc = i2c_get_clientdata(client);
    if (acc != NULL) {
    	if (acc->irq1 != 0) free_irq(acc->irq1, acc);
    	if (acc->irq2 != 0) free_irq(acc->irq2, acc);
        #ifdef CONFIG_ARCH_8810
        if (acc->irq1 != 0) sprd_free_eic_irq(acc->irq1);
        if (acc->irq2 != 0) sprd_free_eic_irq(acc->irq2);
        #else
        if (acc->irq1 != 0) sprd_alloc_gpio_irq(acc->irq1);
        if (acc->irq2 != 0) sprd_alloc_gpio_irq(acc->irq2);
        #endif
    	destroy_workqueue(acc->irq1_work_queue);
    	destroy_workqueue(acc->irq2_work_queue);
    	misc_deregister(&mma8452_acc_misc_device);
    	mma8452_acc_input_cleanup(acc);
    	mma8452_acc_device_power_off(acc);
    	if (acc->pdata->exit)
    		acc->pdata->exit();
    	kfree(acc->pdata);
    	kfree(acc);
    }

	return 0;
}

static int mma8452_acc_resume(struct i2c_client *client)
{
	struct mma8452_acc_data *acc = i2c_get_clientdata(client);
#ifdef MMA8452_DEBUG
    MMA8452_DEBUG_PRINTF("%s.\r\n", __func__);
#endif    

	if (acc != NULL && acc->on_before_suspend) {
        acc->on_before_suspend = 0;
		return mma8452_acc_enable(acc);
    }
    
	return 0;
}

static int mma8452_acc_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct mma8452_acc_data *acc = i2c_get_clientdata(client);
#ifdef MMA8452_DEBUG
    MMA8452_DEBUG_PRINTF("%s.\r\n", __func__);
#endif    
    if (acc != NULL) {
        if (atomic_read(&acc->enabled)) {
            acc->on_before_suspend = 1;
            return mma8452_acc_disable(acc);
        }
    }
    return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND

static void mma8452_early_suspend (struct early_suspend* es)
{
#ifdef MMA8452_DEBUG
    MMA8452_DEBUG_PRINTF("%s.\r\n", __func__);
#endif    
    mma8452_acc_suspend(mma8452_i2c_client,
         (pm_message_t){.event=0});
}

static void mma8452_early_resume (struct early_suspend* es)
{
#ifdef MMA8452_DEBUG
    MMA8452_DEBUG_PRINTF("%s.\r\n", __func__);
#endif    
    mma8452_acc_resume(mma8452_i2c_client);
}

#endif /* CONFIG_HAS_EARLYSUSPEND */

static const struct i2c_device_id mma8452_acc_id[]
				= { { MMA8452_ACC_DEV_NAME, 0 }, { }, };

MODULE_DEVICE_TABLE(i2c, mma8452_acc_id);

static struct i2c_driver mma8452_acc_driver = {
	.driver = {
			.name = MMA8452_ACC_I2C_NAME,
		  },
	.probe = mma8452_acc_probe,
	.remove = __devexit_p(mma8452_acc_remove),
	.resume = mma8452_acc_resume,
	.suspend = mma8452_acc_suspend,
	.id_table = mma8452_acc_id,
};


#ifdef I2C_BUS_NUM_STATIC_ALLOC

int i2c_static_add_device(struct i2c_board_info *info)
{
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	int err;

	adapter = i2c_get_adapter(I2C_STATIC_BUS_NUM);
	if (!adapter) {
		pr_err("%s: can't get i2c adapter\r\n", __FUNCTION__);
		err = -ENODEV;
		goto i2c_err;
	}

	client = i2c_new_device(adapter, info);
	if (!client) {
		pr_err("%s:  can't add i2c device at 0x%x\r\n",
			__FUNCTION__, (unsigned int)info->addr);
		err = -ENODEV;
		goto i2c_err;
	}

	i2c_put_adapter(adapter);

	return 0;

i2c_err:
	return err;
}

#endif /*I2C_BUS_NUM_STATIC_ALLOC*/ 

static int __init mma8452_acc_init(void)
{
        int  ret = 0;        

    	MMA8452_DEBUG_PRINTF("%s accelerometer driver: init\r\n",
						MMA8452_ACC_I2C_NAME);
#ifdef I2C_BUS_NUM_STATIC_ALLOC
        ret = i2c_static_add_device(&mma8452_i2c_boardinfo);
        if (ret < 0) {
            pr_err("%s: add i2c device error %d\r\n", __FUNCTION__, ret);
            goto init_err;
        }
#endif 

        return i2c_add_driver(&mma8452_acc_driver);

init_err:
        return ret;
}

static void __exit mma8452_acc_exit(void)
{
	MMA8452_DEBUG_PRINTF("%s accelerometer driver exit\r\n", MMA8452_ACC_DEV_NAME);

	#ifdef I2C_BUS_NUM_STATIC_ALLOC
    i2c_unregister_device(mma8452_i2c_client);
	#endif

	i2c_del_driver(&mma8452_acc_driver);
	return;
}

module_init(mma8452_acc_init);
module_exit(mma8452_acc_exit);

MODULE_DESCRIPTION("mma8452 accelerometer misc driver");
MODULE_AUTHOR("STMicroelectronics");
MODULE_LICENSE("GPL");
#endif
