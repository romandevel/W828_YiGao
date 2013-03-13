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
*******************************************************************************/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwIIC.h"
#include "DS28CN01.h"



tDS28CN01_SERIAL_NUM  ds28cn01SerialNum;

const U8 dscrc_table[256] =
{
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
	116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

unsigned char dowcrc(U8 crc8, unsigned char x)
{
	return dscrc_table[crc8 ^ x];
}




/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_reset
* 功能:	reset the ds28cn01
* 参数:	none
* 返回:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_reset( void )
{
	U32 rc;
	/* 设为iic 模式*/
	U8  bus_mode;
	
	bus_mode = 0;
	rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_COMMUNICATE_MODE, &bus_mode, 1 );
	bus_mode &= 0xFE;
	rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_COMMUNICATE_MODE, &bus_mode, 1 );
	
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_getComuniateMode
* 功能:	得到 ds28cn01 的通讯模式， 默认为 SM bus  读出的寄存器值为 1, 0 为IIC mode
* 参数:	none
* 返回:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_getComuniateMode(U8 *pcommunicateMode)
{
	*pcommunicateMode = 0 ;
	
	return hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_COMMUNICATE_MODE,pcommunicateMode,1 );
}



/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_readSerialNumber
* 功能:	读28cn01 唯一的序列号
* 参数:	none	
* 返回:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSerialNumber( void )
{
	U32 rc, i;
	U8 addr = DS28CN01_MEM_SERIAL;
	U8 crc8;
	
//	rc	= hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_SERIAL, &(ds28cn01SerialNum.serialNum[0]) ,8);
	rc = HY_OK;
	for (i=0; i<8; i++)
	{
		rc	= hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &(ds28cn01SerialNum.serialNum[i]), 1);
	}
 
	/* CRC8 check if reading ROM ID is right */
	if(rc == HY_OK)
	{
		crc8 = 0;
		for (i=0; i<8; i++)
		{
	    	crc8 = dowcrc(crc8, ds28cn01SerialNum.serialNum[i]); 
	    }
	    
	    if (crc8 != 0) rc = HY_ERROR;
	}
	
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_computeSHA
* 功能:	 启动ds28cn01 内部compter sha， 等待一段时间后，可以读mac
* 参数:	 pChallenge - 8字节的随机数
*		 pageNum	- 用来计算的page 数字， page number 为 0-3，但是如果page 3 用来做额外验证，就不能使用做普通验证
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_computeSHA(U8 *pChallenge, U8 pageNum)
{
	U32 rc = HY_OK, i;
	U8 addr;
	U8 data[9];  /* 写入密钥命令1字节 + 密钥8字节*/

	addr = DS28CN01_MEM_COMMAND;
	data[0] = DS28CN01_COMMAND_COMPUTE_PAGE_MAC|pageNum ;
	memcpy(&data[1], pChallenge, 8);

#if 1
	rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_COMMAND, data, 9);
#else
	rc = HY_OK;
	for (i=0; i<9; i++)
    {
		rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &data[i], 1);
		if (rc != HY_OK) break;
	}
#endif
	
	return rc;
}
	

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_readMAC
* 功能:	 read the mac  from ds28cn01 to compare if the SHA resulte which being calculated match the 28cn01's calculation
*		 mac 值是20字节，调用者注意给出的buffer是20字节
* 参数:	 pbuffer - the mac calculation
*		 len     - the mac calculation length	
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readMAC( U8* pbuffer, int len)
{
	//return hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_MAC, pbuffer,len);
	U32 rc = HY_OK, i;
	U8 addr = DS28CN01_MEM_MAC;
	
	rc = HY_OK;
	for (i=0; i<len; i++)
    {
    	rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &pbuffer[i], 1);
    	if (rc != HY_OK) break;
    }
    
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_writeMAC
* 功能:	 write the mac  to ds28cn01 , 在对eeprom 进行写操作的时候，ds28cn01 要求这个动作
*		 mac 值是20字节，调用者注意给出的buffer是20字节
* 参数:	 pMac - the mac 
*		 len     - the mac length	
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeMAC( U8* pMac, int len)
{
#if 1
	return hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_MAC, pMac ,len);
#else
	U32 rc = HY_OK, i;
	U8 addr = DS28CN01_MEM_MAC;
	
	rc = HY_OK;
	for (i=0; i<len; i++)
    {
    	rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &pMac[i], 1);
    	if (rc != HY_OK) break;
    }
    
	return rc;
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_writeSecret
* 功能:	 写入密钥， 注意ds28cn01 的基本密钥只有一个64bit即8字节的密钥
*		 加密烧录程序才可能调用这个函数
* 参数:	 pSSecret - the s secret
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeSecret( U8* pSecret )
{
	U32 rc = HY_OK, i;
	U8 addr;
	U8 data[9];  /* 写入密钥命令1字节 + 密钥8字节*/
	
	addr = DS28CN01_MEM_COMMAND;
	data[0] = DS28CN01_COMMAND_LOAD_FIRST_SECRET ;
	memcpy(&data[1],pSecret,8);

#if 1
	return hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, DS28CN01_MEM_COMMAND,data,9);
#else
	rc = HY_OK;
	for (i=0; i<9; i++)
    {
    	rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &data[i], 1);
    	if (rc != HY_OK) break;
    }
    
	return rc;
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_e2promWriteStart
* 功能:	 E2PROM写启动函数。 注意每次写入8byte， 
*		 真正的写需要启动验证程序的。所以，主机写eeprom时，需要启动验证
* 参数:	 00H--7FH, E2PROM分为4page，每page有32BYTE。，共128BYTE， 是否可以不按page读？ 需要确认
*    	 addr 	-- 必须是8的倍数 
		 pbuffer-- data buffer
		 len    -- data length  ，目前必须为8		 
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promWriteStart( U8 addr, U8 *newData )
{
#if 1
	return hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr, (U8*)newData ,8);
#else
	U32 rc = HY_OK, i;
 
	rc = HY_OK;
	for (i=0; i<8; i++)
    {
    	rc = hyhwI2c_writeData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &newData[i], 1);
    	if (rc != HY_OK) break;
    }
    
	return rc;
#endif
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_e2promRead
* 功能:	 E2PROM读函数。
* 参数:	 00H--7FH, E2PROM分为4page，每page有32BYTE。，共128BYTE， 是否可以不按page读？ 需要确认
*    	 pageNum -- 0 ~ 3
		 pbuffer-- data buffer， 必须 32字节？ 
		 len    -- data length
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promRead(U8 pageNum, U8* pbuffer, int len)
{
	U32 rc = HY_OK, i;
	U8 addr = pageNum * 32;  /* 1个 page 32 字节*/
	
	//return hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr, pbuffer, len );
	rc = HY_OK;
	for (i=0; i<len; i++)
	{
		rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, addr++, &pbuffer[i], 1);
		if (rc != HY_OK) break;
	}
	
	return rc;
}



/*-----------------------------------------------------------------------------
* 函数:	 getSerialNumber
* 功能:	 获取加密芯片的id号。
* 参数:	 无
* 返回:	 指向id号的指针
*------------------------------------------------------------------------------*/
U8 *getSerialNumber(void)
{
	return ds28cn01SerialNum.serialNum;
}

