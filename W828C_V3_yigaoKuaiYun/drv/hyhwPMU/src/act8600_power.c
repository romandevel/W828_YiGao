/*
 * PMU driver for act8600 PMU
 *
 * Copyright 2010 Ingenic Semiconductor LTD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"

#include "hyhwIIC.h"
#include "hyhwGpio.h"
#include "act8600_power.h"

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT		//hyUsbPrintf
#else
#define DEBUG_PRINT
#endif

/*
2012-12-24:
由于ACT8600芯片本身问题: 
	IIC总线上有连续读写操作(即便slave addr是别的芯片)时，
	该芯片可能产生误动作(本身电压输出异常，干扰IIC总线)
	单独占用一个IIC通道，且不能支持多字节操作
在W828上，DS28CN01与ACT8600挂在同一IIC通道上，由于以上原因，
	CPU与DS28CN01不能正常通讯，并有可能导致ACT8600电压输出异常；
	所以，我们必须在DS28CN01通讯完成后，重新设置ACT8600
方法：
	开机初始化设置ACT8600后，读取所有寄存器的值，备份；
	DS28CN01操作完成后，依据这些备份，重设ACT8600
*/
const U8 act8600_reg_addr_list[]=
{
	ACT8600_SYS0,
	ACT8600_SYS1,
	
	ACT8600_REG1_VSET,
	ACT8600_REG1_VCON,
	
	ACT8600_REG2_VSET,
	ACT8600_REG2_VCON,
	
	ACT8600_REG3_VSET,
	ACT8600_REG3_VCON,
	
	ACT8600_REG4_VSET,
	ACT8600_REG4_VCON,
	
	ACT8600_REG5_VSET,
	ACT8600_REG5_VCON,
	
	ACT8600_REG6_VSET,
	ACT8600_REG6_VCON,
	
	ACT8600_REG7_VSET,
	ACT8600_REG7_VCON,
	
	ACT8600_REG8_VSET,
	ACT8600_REG8_VCON,
	
	ACT8600_REG9_10_VCON,
	
	ACT8600_APCH_INTR0,
	ACT8600_APCH_INTR1,
	ACT8600_APCH_INTR2,
	//ACT8600_APCH_STAT,
	ACT8600_OTG_CON,
	ACT8600_OTG_INTR,
	
};
U8 act8600Reg_bak[sizeof(act8600_reg_addr_list)];

U8 PMU_initFlag = 0;


//共10个输出，OUT9&OUT10常开，不可控
#define ACT8600_OUT_NUM		(8)
const struct act8600_outputs_t act8600_out_default[ACT8600_OUT_NUM]=
{
	{ACT8600_OUT1, 0x18, ACT8600_OUT_ON},		//DC-DC REG1 VDD_1D2 1.2v
	{ACT8600_OUT2, 0x39, ACT8600_OUT_ON},		//DC-DC REG2 VDD_3D3 3.3v
	{ACT8600_OUT3, 0x24, ACT8600_OUT_ON},		//DC-DC REG3 VDD_1D8 1.8v
	{ACT8600_OUT4, 0x54, ACT8600_OUT_OFF},		//			VDD_USB_HOST 5v
	{ACT8600_OUT5, 0x34, ACT8600_OUT_OFF},		//LDO REG5 VDD_CMOS_2D8 2.8v
	{ACT8600_OUT6, 0x39, ACT8600_OUT_OFF},		//LDO REG6 VDD_SCANNER 3.3v
	{ACT8600_OUT7, 0x39, ACT8600_OUT_OFF},		//LDO REG7 VDD_RFID 3.3v
	{ACT8600_OUT8, 0x1E, ACT8600_OUT_OFF}		//LDO REG8 VDD_CMOS_1D5 1.5v
};

//各电源组的默认值
const struct act8600_platform_pdata_t act8600_default=
{
	&act8600_out_default[0],
	ACT8600_OUT_NUM
};

void act8600_saveReg_value(U8 regAddrs, U8 value);


void hyhwPmu_varbInit(void)
{
	PMU_initFlag = 1;
	memset(act8600Reg_bak, 0x00, sizeof(act8600_reg_addr_list));
}
void hyhwPmu_hwIoInit(void)
{
	//nIRQ
	hyhwGpio_setAsGpio(CHG_IRQ_PORT, CHG_IRQ_GPIO);
 	hyhwGpio_setIn(CHG_IRQ_PORT, CHG_IRQ_GPIO);
 	hyhwGpio_pullDisable(CHG_IRQ_PORT, CHG_IRQ_GPIO);
	
	//nSTAT
	hyhwGpio_setAsGpio(CHG_IRQ_PORT, CHG_IRQ_GPIO);
 	hyhwGpio_setIn(CHG_IRQ_PORT, CHG_IRQ_GPIO);
	//hyhwGpio_pullDisable(CHG_IRQ_PORT, CHG_IRQ_GPIO);
  	hyhwGpio_setPullUp(CHG_IRQ_PORT, CHG_IRQ_GPIO);
}

int act8600_write_reg(char reg,char val)
{
	if (PMU_initFlag != 0)
	{
		if (hyhwI2c_writeData(PMU_I2C_CHANNEL, 		//IIC CHN
							ATC8600_WR_Addr, 		//SLAVE ADDR
							reg, 					//REG ADDR
							&val, 					//DATA BUF
							1)						//WR DATA LEN
							 == HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	act8600_read_reg
* 功能:	read reg
* 参数:	reg----寄存器地址
*		val----存放读取值
* 返回: HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int act8600_read_reg(char reg,char *val)
{
	if (PMU_initFlag != 0)
	{
		if (val == NULL) return HY_ERROR;
		if (hyhwI2c_readData(PMU_I2C_CHANNEL, 		//IIC CHN
							ATC8600_WR_Addr, 		//SLAVE ADDR
							reg, 					//REG ADDR
							val, 					//DATA BUF
							1)						//READ LEN
							 == HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	return HY_OK;
}

int act8600_set_q1(int enable)
{
	char otg_con;
	int rc = HY_OK;
	
	//when Q1 enabled,Q2 and Q3 must disabled
	if(enable)
	{
		otg_con = ACT8600_OTG_CON_Q1;
		rc = act8600_write_reg(ACT8600_OTG_CON,otg_con);
	}
	else
	{
		rc = act8600_read_reg(ACT8600_OTG_CON, &otg_con);
		otg_con = otg_con & ~(ACT8600_OTG_CON_Q1);
		rc = act8600_write_reg(ACT8600_OTG_CON, otg_con);
	}
	act8600_saveReg_value(ACT8600_OTG_CON, otg_con);
	
	return rc;
}

static int act8600_output_set(int outnum,int regvalue)
{
	char reg;
	int rc = HY_OK;
	
	switch(outnum)
	{
		case ACT8600_OUT1:	reg = ACT8600_REG1_VSET;break;
		case ACT8600_OUT2:	reg = ACT8600_REG2_VSET;break;
		case ACT8600_OUT3:	reg = ACT8600_REG3_VSET;break;
		case ACT8600_OUT4:	reg = ACT8600_REG4_VSET;break;
		case ACT8600_OUT5:	reg = ACT8600_REG5_VSET;break;
		case ACT8600_OUT6:	reg = ACT8600_REG6_VSET;break;
		case ACT8600_OUT7:	reg = ACT8600_REG7_VSET;break;
		case ACT8600_OUT8:	reg = ACT8600_REG8_VSET;break;
		default:return -1;
	}

	rc = act8600_write_reg(reg, regvalue);
	act8600_saveReg_value(reg, regvalue);
	
	return rc;
}

int act8600_output_setVoltage(int outnum, eACT8600_VOLTAGE regvalue)
{
	char reg;
	int rc = HY_OK;
	
	switch(outnum)
	{
		case ACT8600_OUT1:
			reg = ACT8600_REG1_VSET;
			break;
		case ACT8600_OUT2:
			reg = ACT8600_REG2_VSET;
			break;
		case ACT8600_OUT3:
			reg = ACT8600_REG3_VSET;
			break;
		case ACT8600_OUT4:
			reg = ACT8600_REG4_VSET;
			break;
		case ACT8600_OUT5:
			reg = ACT8600_REG5_VSET;
			break;
		case ACT8600_OUT6:
			reg = ACT8600_REG6_VSET;
			break;
		case ACT8600_OUT7:
			reg = ACT8600_REG7_VSET;
			break;
		case ACT8600_OUT8:
			reg = ACT8600_REG8_VSET;
			break;
		default:
			return -1;
	}
	//除OUT1，其他输出如果不等于默认值，则设置错误，
	//由于目前的外设复用电源，有些设备不能承受非指定电压，所以固定
	if (ACT8600_OUT1 != outnum)
	{
		if (regvalue != act8600_out_default[outnum-1].value)
		{
			return -1;
		}
	}

	rc = act8600_write_reg(reg, regvalue);
	act8600_saveReg_value(reg, regvalue);
	
	return rc;
}

int act8600_output_enable(int outnum,int enable)
{
	char reg,value;
	signed char timeout;
	int rc = HY_OK;
	int i;
	
	switch(outnum)
	{
		case ACT8600_OUT1:
			reg = ACT8600_REG1_VCON;
			break;
		case ACT8600_OUT2:
			reg = ACT8600_REG2_VCON;
			break;
		case ACT8600_OUT3:
			reg = ACT8600_REG3_VCON;
			break;
		case ACT8600_OUT4:
			reg = ACT8600_REG4_VCON;
			break;
		case ACT8600_OUT5:
			reg = ACT8600_REG5_VCON;
			break;
		case ACT8600_OUT6:
			reg = ACT8600_REG6_VCON;
			break;
		case ACT8600_OUT7:
			reg = ACT8600_REG7_VCON;
			break;
		case ACT8600_OUT8:
			reg = ACT8600_REG8_VCON;
			break;
		default:
			return -1;
	}

	rc = act8600_read_reg(reg,&value);
	if(enable)
	{
		value |= ACT8600_REG_VCON_ON;
		if(outnum >= ACT8600_OUT5 && outnum <= ACT8600_OUT8)
		{
			value &= ~ACT8600_REG_VCON_DIS;
		}
	}
	else
	{
		value &= ~ACT8600_REG_VCON_ON;
		if(outnum >= ACT8600_OUT5 && outnum <= ACT8600_OUT8)
		{
			value |= ACT8600_REG_VCON_DIS;
		}
	}
	rc = act8600_write_reg(reg, value);
	
	for(timeout=100; timeout<=0; --timeout)
	{
		rc = act8600_read_reg(reg, &value);
		if(!(value & ACT8600_REG_VCON_OK))
		{
			//syssleep(1);
			i = 100000;
			while(i--);
			DEBUG_PRINT("out%d is not stable,wait 10 msec.\n",outnum);
		}
		else
			break;
	}
	if(timeout == 0)
	{
		DEBUG_PRINT("out%d set failed , this may cause system halt.\n",outnum);
	}
	else
	{
		act8600_saveReg_value(reg, value);
	}
	return timeout;
}

void act8600_saveReg_value(U8 regAddrs, U8 value)
{
	int i;
	
	for (i=0; i<sizeof(act8600_reg_addr_list); i++)
	{
		if (act8600_reg_addr_list[i] == regAddrs)
		{
			act8600Reg_bak[i] = value;
			break;
		}
	}
}

void act8600_readAllRegister(void)
{
	int i, rc;
	U8 reg_addr;
	
	for (i=0; i<sizeof(act8600_reg_addr_list); i++)
	{
		reg_addr = act8600_reg_addr_list[i];
		rc = act8600_read_reg(reg_addr, &act8600Reg_bak[i]);
		if (rc != HY_OK)
		{
			//重试一次
			act8600_read_reg(reg_addr, &act8600Reg_bak[i]);
		}
	}
}

int act8600_restoreAllRegister(void)
{
	int i, timeout, rc = HY_OK;
	U8 reg_addr, value;
	
	for (i=0; i<sizeof(act8600_reg_addr_list); i++)
	{
		reg_addr = act8600_reg_addr_list[i];
		rc = act8600_write_reg(reg_addr, act8600Reg_bak[i]);
		if (rc != HY_OK)
		{
			//重试一次
			rc = act8600_write_reg(reg_addr, act8600Reg_bak[i]);
		}
		//如果是enable寄存器，则需要读出比较
		switch (reg_addr)
		{
			case ACT8600_REG1_VCON:
			case ACT8600_REG2_VCON:
			case ACT8600_REG3_VCON:
			case ACT8600_REG4_VCON:
			case ACT8600_REG5_VCON:
			case ACT8600_REG6_VCON:
			case ACT8600_REG7_VCON:
			case ACT8600_REG8_VCON:
			case ACT8600_REG9_10_VCON:
				for(timeout=100; timeout<=0; --timeout)
				{
					rc = act8600_read_reg(reg_addr, &value);
					if(!(value & ACT8600_REG_VCON_OK))
					{
						//syssleep(1);
						i = 100000;
						while(i--);
					}
					else
						break;
				}
				break;
			
			default:
				break;
		}
	}
	return rc;
}

int act8600_probe_init(void)
{
	int i;
	struct act8600_platform_pdata_t *pdata = &act8600_default;
	
	hyhwPmu_varbInit();
	
	hyhwPmu_hwIoInit();
	
	for (i = 0; i < pdata->nr_outputs; i++)
	{
		struct act8600_outputs_t *p = &pdata->outputs[i];
		act8600_output_set(p->no,p->value);
		act8600_output_enable(p->no,p->active_on);

		DEBUG_PRINT("%d\t\t%d\t\t%d\n",p->no,p->value,p->active_on);
	}
	act8600_write_reg(ACT8600_OTG_CON,0);
	
	act8600_readAllRegister();
	
#if 0	//for test
	//act8600_output_enable(ACT8600_OUT4,ACT8600_OUT_ON);
	//act8600_write_reg(ACT8600_OTG_CON,0);
	//act8600_set_q1(1);
	//act8600_set_q1(0);
	//act8600_output_enable(ACT8600_OUT4,ACT8600_OUT_OFF);
	
	act8600_output_enable(ACT8600_OUT5,ACT8600_OUT_ON);
	
	act8600_output_enable(ACT8600_OUT6,ACT8600_OUT_ON);
	
	act8600_output_enable(ACT8600_OUT7,ACT8600_OUT_ON);
	
	act8600_output_enable(ACT8600_OUT8,ACT8600_OUT_ON);
#endif
	
}

void hyhw_pmu_restore(void)
{
	act8600_restoreAllRegister();
}