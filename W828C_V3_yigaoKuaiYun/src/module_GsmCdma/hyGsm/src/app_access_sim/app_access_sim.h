#ifndef _APP_API_H
#define _APP_API_H


#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "usock.h"
#include "gbk2unicode.h"



#define ERR_OPERATION_NOT_ALLOWED	-3
#define ERR_SIM_NOT_INSERTED		-10
#define ERR_SIM_PIN_REQUIRED		-11
#define ERR_SIM_PUK_REQUIRED		-12
#define ERR_SIM_FAILURE				-13
#define ERR_SIM_WRONG				-15
#define ERR_INCORRECT_PASSWORD		-16
#define ERR_SIM_PUK2_REQUIRED		-18



typedef union _accessSIM_u
{
	//phb_SIM_get_storage_t
	struct 
	{
		int used;
		int total;
	}storage;

	//phb_SIM_read_node_t,phb_SIM_write_node_t
	struct 
	{
		int 		index;
		char 		*number; //[GSMD_PB_NUMB_MAXLEN+1];
		u_int8_t 	type;	 //请参考enum lgsm_pb_addr_type
		char 		*name;	 //[GSMD_PB_TEXT_MAXLEN+1];
	}rwNode;
	
	//phb_SIM_read_allNode_t
	struct
	{
		int 	total;
		struct 	gsmd_phonebook *allNode;
	}readAllNode;
	
	//phb_SIM_del_node_t,phb_SIM_del_allNode_t
	struct
	{
		int delNum;
		int *delNodeIndex;
	}delNode;
}accessSIM_u;



typedef union _accessPhone_u
{
	u_int8_t simSerial[16];
	
	struct
	{
		int funMode;
		int result;
	}cfun;

	int simDetect;

}accessPhone_u;


typedef union _accessNet_u
{
	int netreg_stat;
	
	//本机号码
	struct
	{
		int 		index;	 //index:0,1
		char 		*number; //[GSMD_PB_NUMB_MAXLEN+1];
		u_int8_t 	type;	 //请参考enum lgsm_pb_addr_type
		char 		*name;	 //[GSMD_PB_TEXT_MAXLEN+1];
	}rwNode;
	
	struct
	{
		int 	total;
		struct 	gsmd_phonebook *allNode;	//MALLOC(2*sizeof(struct gsmd_phonebook))
	}ownNumber;

	char operName[30+1];		//当前网络运营商,如中国移动为"CHINA MOBILE"/"CMCC"/"46000"

}accessNet_u;



enum call_fwd_reason {
	CALL_FWD_REASON_UNCOND				= 0,
	CALL_FWD_REASON_BUSY				= 1,
	CALL_FWD_REASON_NO_REPLY			= 2,
	CALL_FWD_REASON_NOT_REACHABLE		= 3,
	CALL_FWD_REASON_ALL_FORWARD			= 4, //用于关闭所有呼叫转移
	//CALL_FWD_REASON_ALL_COND_FORWARD	= 5, 
};


typedef union _accessVoice_u
{
	struct
	{
		enum call_fwd_reason reason;
		
		u_int8_t status;	//0-not active	1-active
		u_int8_t classx;
		char number[GSMD_ADDR_MAXLEN+1];
		u_int8_t time;
	}call_fwd;

}accessVoice_u;


enum pin_type {				/* waiting for ... */
	PIN_READY		= 0,	/* not pending for any password */
	PIN_SIM_PIN		= 1,	/* SIM PIN */
	PIN_SIM_PUK		= 2,	/* SIM PUK */
	PIN_PH_SIM_PIN	= 3,	/* phone-to-SIM passowrd */
	PIN_PH_FSIM_PIN	= 4,	/* phone-to-very-first SIM password */
	PIN_PH_FSIM_PUK	= 5,	/* phone-to-very-first SIM PUK password */
	PIN_SIM_PIN2	= 6,	/* SIM PIN2 */
	PIN_SIM_PUK2	= 7,	/* SIM PUK2 */
	PIN_PH_NET_PIN	= 8,	/* netwokr personalisation password */
	PIN_PH_NET_PUK	= 9,	/* network personalisation PUK */
	PIN_PH_NETSUB_PIN	= 10, 	/* network subset personalisation PIN */
	PIN_PH_NETSUB_PUK	= 11,	/* network subset personalisation PUK */
	PIN_PH_SP_PIN	= 12,	/* service provider personalisation PIN */
	PIN_PH_SP_PUK	= 13,	/* service provider personalisation PUK */
	PIN_PH_CORP_PIN	= 14,	/* corporate personalisation PIN */
	PIN_PH_CORP_PUK	= 15,	/* corporate personalisation PUK */
	__NUM_PIN
};


enum lock_type {
	LOCK_SC	= 0,	/* SIM (lock SIM card) */
	LOCK_AO	= 1,	/* 禁止所有出局呼叫 */
	LOCK_OI	= 2,	/* 禁止所有国际出局呼叫 */
	LOCK_OX	= 3,	/* 禁止所有国际出局呼叫，归属国除外 */
	LOCK_AI	= 4,	/* 禁止所有入局呼叫 */
	LOCK_IR	= 5,	/* 归属国以外漫游时，禁止所有入局呼叫 */
	LOCK_AB	= 6,	/* All Barring services (refer GSM02.30[19]) */
	LOCK_AG	= 7,	/* All out Going barring services (refer GSM02.30[19]) */
	LOCK_AC	= 8,	/* All in Coming barring services (refer GSM02.30[19]) */
	LOCK_PN	= 9,	/* 网络个性化（请参考GSM 02.22 [33]） */
	LOCK_PU	= 10, 	/* 网络子集个性化（请参考GSM 02.22 [33]） */
	LOCK_PP	= 11,	/* 服务供应商个性化（请参考GSM 02.22 [33]） */
	LOCK_PC	= 12,	/* 公司个性化（请参考GSM 02.22 [33]） */
	LOCK_P2	= 13,	/* SIM PIN2 */
	__NUM_LOCK
};



typedef union _accessSecurity_u
{
	struct
	{
		enum pin_type type;
		char pin[GSMD_PIN_MAXLEN+1];
		char newpin[GSMD_PIN_MAXLEN+1];
	}pin;

	struct
	{
		int pin;
		int pin2;
		int puk;
		int puk2;
	}pin_times;

	struct
	{
		enum lock_type type;	//type=FAC_SC用于PIN码锁定、解除、查询状态
		char pwd[GSMD_PIN_MAXLEN+1];
		char newpwd[GSMD_PIN_MAXLEN+1];
	}lock;

}accessSecurity_u;



extern u_int32_t appLockAccessSIM;


/*-----------------------------------------------------------------------------
* 函数:	net_query_reg
* 功能:	实时查询GSM/CDMA注册状态(注册状态由accessNet_u结构中的netreg_stat带回，
*		netreg_stat  ---  -1:未注册  0:正在注册   1:已注册)
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_query_reg(accessNet_u *v);


/*-----------------------------------------------------------------------------------
* 函数:	net_query_reg_nrt
* 功能:	非实时查询GSM/CDMA注册状态(可在要求不等待且对所读的值不要求实时值的地方调用)
*		
* 参数:	
* 返回:	-1:未注册  0:正在注册   1:已注册
*-----------------------------------------------------------------------------------*/
int net_query_reg_nrt(void);


/*-----------------------------------------------------------------------------------
* 函数:	net_query_sigq_set
* 功能:	非实时查询GSM/CDMA信号强度(可在要求不等待且对所读的值不要求实时值的地方调用)
*		
* 参数:	
* 返回:	信号强度 0-31,99
*-----------------------------------------------------------------------------------*/
u_int32_t net_query_sigq_nrt(void);


/*-----------------------------------------------------------------------------
* 函数:	net_get_subscriber_num
* 功能:	查询本机号码
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_get_subscriber_num(accessNet_u *v);



/*-----------------------------------------------------------------------------
* 函数:	net_set_subscriber_num
* 功能:	设置本机号码
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(设置成功)	-1:FAIL(设置失败)
*----------------------------------------------------------------------------*/
int net_set_subscriber_num(accessNet_u *v);


/*-----------------------------------------------------------------------------
* 函数:	net_get_operator
* 功能:	查询当前网络运营商
*		
* 参数:	accessNet_u *v
* 返回:	0:SUCCESS(查询成功)	-1:FAIL(查询失败)
*----------------------------------------------------------------------------*/
int net_get_operator(accessNet_u *v);


/*-----------------------------------------------------------------------------
* 函数:	net_register_operator
* 功能:	向网络运营商注册
*		
* 参数:	accessNet_u *v
* 返回:	0:						SUCCESS(注册成功)
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡
		ERR_SIM_WRONG:			SIM卡错误，该SIM不能注册
		others:					未知错误
*----------------------------------------------------------------------------*/
int net_register_operator(accessNet_u *v);


/*-----------------------------------------------------------------------------
* 函数:	net_deregister_operator
* 功能:	从网络上注销注册
*		
* 参数:	accessNet_u *v
* 返回:	0:						SUCCESS(注销成功)
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡
		others:					未知错误
*----------------------------------------------------------------------------*/
int net_deregister_operator(void);





/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_check_ready_t
* 功能:	检查SIM卡电话本是否已准备好
* 参数:	
* 返回:	0:Ready   -1:Check Fail   -2:SIM not inserted
*----------------------------------------------------------------------------*/
int phb_SIM_check_ready_t(void);


/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_get_storage_t
* 功能:	获取SIM卡电话本已用空间、总存储空间
* 参数:	accessSIM_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_get_storage_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_read_node_t
* 功能:	读取SIM卡电话本中指定index的记录
* 参数:	accessSIM_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_node_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_read_allNode_t
* 功能:	读取SIM卡电话本中所有记录
* 参数:	accessSIM_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_allNode_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_write_node_t
* 功能:	写入SIM卡电话本中指定index的记录
* 参数:	accessSIM_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_write_node_t(accessSIM_u *v);

/*-----------------------------------------------------------------------------
* 函数:	phb_SIM_write_node_t
* 功能:	写入SIM卡电话本中指定index的记录
* 参数:	accessSIM_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_del_node_t(accessSIM_u *v);





/*-----------------------------------------------------------------------------
* 函数:	phone_detect_simcard
* 功能:	检测是否有SIM卡，AT+ESIMS – Query SIM Status
* 参数:	
* 返回:	1:有卡	0:无卡	-1:检测失败
*----------------------------------------------------------------------------*/
int phone_detect_simcard(void);


/*-----------------------------------------------------------------------------
* 函数:	phone_detect_simcard_nrt
* 功能:	非实时检测是否有SIM卡(可在要求不等待且对所读的值不要求实时值的地方调用)
* 参数:	
* 返回:	1:有卡	0:无卡
*----------------------------------------------------------------------------*/
int phone_detect_simcard_nrt(void);


/*-----------------------------------------------------------------------------
* 函数:	phone_get_sim_imsi
* 功能:	获取SIM卡的IMSI(International Mobile Subscriber Identity),即国际移动用
*		户识别码
* 参数:	accessPhone_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_sim_imsi(accessPhone_u *v);


/*-----------------------------------------------------------------------------
* 函数:	phone_get_IMEI
* 功能:	获取模块的IMEI(international mobile equipment identifier),即国际移动设
*		备识别码
* 参数:	accessPhone_u *v
* 返回:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_IMEI(accessPhone_u *v);


/*-----------------------------------------------------------------------------
* 函数:	phone_set_fun_mode
* 功能:	设置GSM模块的功能模式
* 参数:	funMode  ---  1:正常模式  0:飞行模式
* 返回:	0:SUCCESS	other:FAIL
*----------------------------------------------------------------------------*/
int phone_set_fun_mode(int funMode);





/*---------------------------------------------------------------------------------
* 函数:	security_check_pin_status
* 功能:	查询SIM卡是否需要密码，函数返回值为0时v->pin.type将带回需要的密码类型，如果
		类型为PIN_READY，有两种情况：
		1)不再需要输入密码
		2)SIM卡PUK码已被锁死，通过查询PUK码可输入次数是否为0以确认SIM卡已被锁死
* 参数:	accessSecurity_u *v
* 返回:	0:						SUCCESS
		ERR_SIM_NOT_INSERTED:	没有检测到SIM卡   
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_check_pin_status(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_pin_input
* 功能:	输入SIM卡密码，函数返回值为0时密码正确
* 参数:	accessSecurity_u *v
* 返回:	0:						SUCCESS
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_SIM_FAILURE:		SIM操作失败，此时需要再查询PUK码可输入次数是否
								为0，如果为0说明SIM卡已被锁死
		ERR_INCORRECT_PASSWORD:	密码不正确
		ERR_SIM_PUK2_REQUIRED:	需要输入SIM卡PUK2码(输入PUK码时必须要同时输入新的PIN2码)
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_pin_input(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_pin_remain_times
* 功能:	查询SIM卡密码可输入次数
* 参数:	accessSecurity_u *v
* 返回:	0:						查询成功
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_pin_remain_times(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_lock_en
* 功能:	启用设备锁定功能(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						启用锁定成功
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:	密码不正确
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_en(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_lock_dis
* 功能:	解除设备锁定功能(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						解除锁锁定成功
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:	密码不正确
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_dis(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_lock_status
* 功能:	查询设备锁定状态(如PIN码锁定等)
* 参数:	accessSecurity_u *v
* 返回:	0:						设备锁定未启用
		1:						设备锁定已启用
		ERR_SIM_PIN_REQUIRED:	需要输入SIM卡PIN码(开机时)
		ERR_SIM_PUK_REQUIRED:	需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_SIM_FAILURE:		SIM操作失败，此时需要再查询PUK码可输入次数是否
								为0，如果为0说明SIM卡已被锁死
		others:					未知错误
*---------------------------------------------------------------------------------*/
int security_lock_status(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* 函数:	security_change_pwd
* 功能:	修改密码(如PIN码等)
* 参数:	accessSecurity_u *v
* 返回:	0:							修改密码成功
		ERR_OPERATION_NOT_ALLOWED:	操作不允许(可能设备锁定未启用)
		ERR_SIM_PUK_REQUIRED:		需要输入SIM卡PUK码(输入PUK码时必须要同时输入新的PIN码)
		ERR_INCORRECT_PASSWORD:		密码不正确
		ERR_SIM_PUK2_REQUIRED:		需要输入SIM卡PUK2码(输入PUK码时必须要同时输入新的PIN2码)
		others:						未知错误
*---------------------------------------------------------------------------------*/
int security_change_pwd(accessSecurity_u *v);





/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_dis
* 功能:	禁用指定条件(条件由accessVoice_u结构中的reason指定)的呼叫转移，必须先设
*		置或启用后才能禁用
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_dis(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_en
* 功能:	启用指定条件(条件由accessVoice_u结构中的reason指定)的呼叫转移
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_en(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_stat
* 功能:	查询指定条件(条件由accessVoice_u结构中的reason指定)的呼叫转移设置
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_stat(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_reg
* 功能:	设置指定条件(条件由accessVoice_u结构中的reason指定)的呼叫转移，设置后就
*		已经启用
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_reg(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* 函数:	voice_call_fwd_eras
* 功能:	删除指定条件(条件由accessVoice_u结构中的reason指定)的呼叫转移设置
* 参数:	
* 返回:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_eras(accessVoice_u *v);


#endif