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
#include "hyhwCkc.h"

/*----------------------------------------------
|  e2prom 地址 0 开始    2011.11.16            | 
|  生产序列号1  len = 16                       | 
|  生产序列号2  len = 16 					   | 
|  生产年月时   len = 8						   | 
|  生产分秒     len = 8						   | 
|  代工工厂     len = 8  Askey- 苏州		   | 
|	'A','S','K','E','Y','S','Z','-',  		   |
------------------------------------------------*/

#define  DS28CN01_PRODUCT_INFO_LEN  (56) 

extern void hyhw_pmu_restore(void);

/*-----------------------------------------------------------------------------
* 函数:	autDrv_ds28cn01_longDelay()
* 功能:	较长时间的延时， // test on 399-133  
* 参数:	delay	
* 返回:	none
*------------------------------------------------------------------------------*/
extern tSYSCLKREGISTER_ST  SysClkRegister;
static void autDrv_ds28cn01_longDelay(U32 us)
{
   
	U32 clk;
	clk = (SysClkRegister.mCpuClk)*us;
	
	clk /= 3;
	while( clk --);
   
}

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_e2promWrite
* 功能:	 E2PROM写函数。
* 参数:	  00H--7FH, E2PROM分为4page，每page有32BYTE。，共128BYTE ,
*         目前只可写3个page,第四个page 加密使用

*    	 offset -- 写入地址,必须是8的倍数
		 pbuffer-- data buffer  注意: 写的长度不是8的倍数会强制写足，注意buf给足8的倍数
		 len    -- data length   
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promWrite(U8 offset, U8* pbuffer, int len)
{	
	U32 addr ,rc = HY_ERROR;
	U8  i ,page,cnt,outputbuf[20],random[128];
	U8  *pIndex ;
	
	if(offset%8 != 0) return HY_ERROR ; /* 地址必须是8的倍数 */
	
	addr = offset + len ;
	page = addr/32 ;
	page = addr%32 == 0 ? page : page+1 ;
	
	if(page >= 3) return HY_ERROR ; /* 第4page 不允许使用 */
	
	cnt = len/8 ;
	cnt = len%8 == 0 ? cnt : cnt+1 ;
	
	
	for(i = 0 ; i < cnt ; i++)
	{
		/* 由于计算mac 要使用验证函数，而在验证函数中有读eeprom page，所以必须放在前面调用，在下面的iic write 之前*/	
		addr   = offset + (i<<3) ;
		pIndex = pbuffer + (i<<3) ;
		
		memset(outputbuf ,0 ,20);
		outputbuf[0] = 0x4 ;
		outputbuf[1] = addr;
		memcpy(&outputbuf[2],pIndex,8);
		
		rc = hyAuthen_verify(1, random, outputbuf);
		if(HY_ERROR == rc)break ;
		
		/* 通知ds28cn01， 启动写， 将引起芯片进行sha 算法，然后主机也要计算后，才能真正写成功*/
		rc = autDrv_ds28cn01_e2promWriteStart(addr,(char *)pIndex);
		if(HY_ERROR == rc)break ;
		/*  delay 3ms for DS28CN01 completing SHA-1 algorithm */
		autDrv_ds28cn01_longDelay(5000);
		
		/* 主机运算的mac 后要写入芯片中， 写成功后，本次写入eeprom 才能成功 */
		rc = autDrv_ds28cn01_writeMAC(outputbuf,20);
		if(HY_ERROR == rc)break ;
		autDrv_ds28cn01_longDelay(10000);
	}
	
//	hyAuthen_reg(NULL, NULL);
//	hyAuthen_verify(1, random, NULL);
	
 	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_e2promRead1
* 功能:	 E2PROM读函数。
* 参数:	 00H--7FH, E2PROM分为4page，每page有32BYTE。，共128BYTE， 是否可以不按page读？ 需要确认
*    	 addr -- 读取地址
		 pbuffer-- data buffer 
		 len    -- 读取长度
* 返回:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promRead1(U8 addr, U8* pbuffer, int len)
{
	U32 i, j,raddr,pageI, offset,dert,cnt,rc;
	U8  readbuf[128],rw;
	
	pageI = addr/32 ;/* 第几个page开始 */
	offset= addr%32 ;
	dert  = len - (32 - offset);
	cnt   = 1 + dert/32 ;
	cnt   = dert%32 == 0 ? cnt : cnt+1 ; /* 要读的page说 */
	rw    = 0 ;
	
	memset(readbuf,0,128);
	
	for(i = pageI ; (i < cnt && i < 4) ; i++)
	{
		raddr = i * 32 ;
		
		//rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, raddr, &readbuf[rw], 32);
		rc = HY_OK;
		for (j=0; j<32; j++)
	    {
	    	rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, raddr++, &readbuf[rw+j], 1);
	    	if (rc != HY_OK) break;
	    }
		if(HY_ERROR == rc)return HY_ERROR ;
		
		rw += 32 ;		
	}
	
	if(len >= rw)
	{
		memcpy(pbuffer,&readbuf[offset],rw) ;
	}
	else
	{
		memcpy(pbuffer,&readbuf[offset],len) ;
	}
	
 	return  HY_OK ;
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01Init
* 功能:	 初始化ds2460，并读取序列号
* 参数:	 none
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
int autDrv_ds28cn01_readProductSerialNumberInit(U8 *pbuffer)
{
	//autDrv_ds28cn01_e2promRead(0,pbuffer,16);
	U32 offset, rc = HY_OK;
	
	offset = 0;
	rc |= autDrv_ds28cn01_e2promRead(offset, (U8*)&pbuffer[0], 8);
	offset += 8;
	rc |= autDrv_ds28cn01_e2promRead(offset, (U8*)&pbuffer[8], 8);
	if (rc != HY_OK)
	{
		return HY_ERROR;
	}
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01Init
* 功能:	 初始化ds2460，并读取序列号
* 参数:	 none
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01Init(void)
{
	U32 rc;
	//U8  ds2460SerialNumber[8];
	U8  productSerialNumber[16];
	
	
	hyAuthen_setChipAddress(DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR);
	rc = autDrv_ds28cn01_reset();
	if (rc != HY_OK)
	{//初始化错误
		
		return rc;
	}
	
	autDrv_ds28cn01_readSerialNumber();
	autDrv_ds28cn01_readSN(&productSerialNumber[0]);
	hyAuthen_init(&ds28cn01SerialNum.serialNum[0], &productSerialNumber[0]);
	hyAuthen_regPostFunction(hyhw_pmu_restore);
	hyAuthen_verify(0 /*don't care*/, NULL, NULL);
	
	#if 0
	{
		char pRandomBuf[128];
		int ret ,i,m;
		Led_init();
		for(i = 0 ; i < 10 ; i++)
		{
			hyAuthen_reg(NULL, NULL);
			ret = hyosv_verify(1, &pRandomBuf[0], NULL);
			
			if(ret == HY_ERROR)
			{
				Led_init();Led_statusCtrl(1);for(m = 0 ; m < 0xFFFFFFF ; m++);
				
			}
			Led_statusCtrl(1);
			for(m = 0 ; m < 0xFFFFF ; m++);
			Led_statusCtrl(0);
		}		
	}
	#endif
		
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_readProductInfo
* 功能:	 读取产品所有生产信息 
* 参数:	 pSn1 -- 序列号1  至少16字节 (不含结束符)
*        pSn2 -- 序列号2  至少16字节 (不含结束符)
*        pTime-- 批次时间 至少16字节 (不含结束符)
*        pInfo-- 生产工厂 至少8字节  (不含结束符)
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readProductInfo(U8* pSn1,U8 *pSn2 ,U8 *pTime ,U8 *pInfo)
{
	U32 rc;
	U8  info[DS28CN01_PRODUCT_INFO_LEN] ;
	
	memset(info,0,DS28CN01_PRODUCT_INFO_LEN);
	
	rc = autDrv_ds28cn01_e2promRead1(0,info,DS28CN01_PRODUCT_INFO_LEN);
	
	if(HY_OK == rc)
	{	
		memcpy(pSn1,&info[0],9);
		memcpy(pSn2,&info[16],9);
		memcpy(pTime,&info[32],12);
		memcpy(pSn2,&info[48],8);
	}
	
	return rc;
}

/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_writeSN
* 功能:	 写入产品序列号
* 参数:	 pbuffer-- SN Str，最大16字节长度(含字符串结束符)
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeSN(U8* pSerialNum)
{
	U32 len, rc  ;
	U8  wrbuf[16];
	
	len = strlen(pSerialNum);
	
	len = len >=16 ? 16 : len ;
	
	memset(wrbuf,0,16);
	memcpy(wrbuf,pSerialNum,len);
	
	rc = autDrv_ds28cn01_e2promWrite(0,wrbuf,16);
	hyhw_pmu_restore();
	
	return rc;
}


/*-----------------------------------------------------------------------------
* 函数:	 autDrv_ds28cn01_readSN
* 功能:	 读取产品序列号
* 参数:	 pbuffer-- SN Str，至少16字节长度(含字符串结束符)
* 返回:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSN(U8* pSerialNum)
{
	U32 offset, rc = HY_OK;
	
	rc = autDrv_ds28cn01_e2promRead1(0,pSerialNum,16);
	
	return rc;
}

void testWriteRead(void)
{
	//U8 testStr[36] = "12345678901234567890123456789012";
	//U8 outbuf[36];
	//U32 rcc,rcc1;
	//memset(outbuf,0,36);
	
	//rcc = autDrv_ds28cn01_e2promWrite(0,testStr,32);
	//rcc1 = autDrv_ds28cn01_e2promRead1(0,outbuf,32);
}