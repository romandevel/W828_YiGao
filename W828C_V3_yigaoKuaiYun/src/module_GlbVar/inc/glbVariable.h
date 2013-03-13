#ifndef _GLOBAL_VARIABLE_H_
#define _GLOBAL_VARIABLE_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "touchScreenMain.h"

#define WIFI_SAVEPW_MAX			10//保存wifi信息

/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/

//AP信息
typedef struct _AP_info_t
{
	U8	apName[32];		//AP名字
	U8	mac[6];			//MAC地址
	U16 pwType;			//加密类型
	U8	pw[64];			//密码
	
	U32	lastTime;		//最后一次使用时间 以2012-01-01-00:00:00 为基数
	
}tAPINFO;

//离线登录时使用,保存上次登录信息
typedef struct _last_login_t
{
	U8	userId[20];
	U8	userPw[20];
	U8	userName[32];
	U8	userRight;
	U8	userType;
	U8	groupTotal;
	U8	groupDefault;
	U8	groupSelect;
	U8	groupId[12];
	U8	groupName[52];
	
}tLAST_LOGIN;

/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
typedef struct _glb_HycParameter_t
{
	U32 CRC;	//校验和
	
	U8 	ScanSound;			//扫描声音开关：0：关  1：开
	U8  KeySound;			//按键音开关    0：关  1：开
	U8	RemindType;			//提示方式      0：关  1: 开  
	U8  BackLight;			//背光等级
	
	U32	ParamCfg;			//每个bit标识1种(bit0:是否必须蓝牙称重   bit1:重量是否允许为零  bit2:签收是否必须拍照)
	
	U16 ScreenProtect;		//屏保时间
	U8	scanMode;			//扫描模式选择 0:按住出光松开关闭(最长3-4秒)  1:按一次持续出光3-4秒
	U8	sendType;			//发送方式 0:自动(条数/时间阀值满足其一即开始发送)   1:手动(条数/时间阀值无效,需手动按F1上传) 
	U8  autoSendNum;		//自动发送条数阀值      20-200
	U8	autoSendTime;		//自动发送时间间隔 		1--30分钟
	U8	printTimes;			//打印次数  1---10
	U8	gpsSwitch;			//gps开关 0:关  1:开
	U8	gpsGetInterval;		//gps采集间隔时间   1---60分钟
	U8	gpsSendInterval;	//gsp发送间隔时间   5---60分钟
	U8	workDay;			//工作日开始时间点 //默认为0点
	
	U8	NetSelected;		//网络选择  0:wifi   1:gsm   0xff:网络切换中
	U8 	GprsAccess;			//GPRS接入点：4项可选 0：cmnet   1：cmwap   2：uninet   3：uniwap
	U8 	MachineCode[18];	//终端编号  使用16位硬件编号
	U8	simCard[12];	//sim卡	   	
	U8  UrlSelected;        //Url地址选择  0:电信网络 1:联通网络
	
	U8	ipMode;			   //0:自动获取IP   1:手动填写
	U8	dnsMode;		   //0:自动获取dns  1:手动填写
	U8	ipAddr[16];		    //ip
	U8	mask[16];			//子网掩码
	U8	gateWay[16];		//网关
	U8	dns1[16];			//首选dns
	U8	dns2[16];			//备用dns
	
	tAPINFO	apInfo[WIFI_SAVEPW_MAX];		//保存5个ap信息
	
	U8  bt_addr1[16];      //本地地址
	U8  bt_addr2[16];      //所选设备地址
	U8  bt_pwd[18];        //所选设备密码
	U8  bt_devenable;      //蓝牙称使能,目前只有称,即蓝牙使能 0:不使能 1/2/3:三种称的编码规则
	
	tTOUCH_PARAMETER touchParam;
	  
	////////////////////////////
	tLAST_LOGIN	lastLogin;	//上次登录信息
	
	U8	key[36];		//系统分配的key
	
	U8	companyId[20];	//公司编码
	U8	companyName[80];//公司名称
	
	U8 	defaultId;			//默认服务器ID：1，2，3  由于IP和DNS是一一对应的，故只需记录默认使用的服务器编号即可。如果为1，则表示默认使用DNS1，如果域名解析失败，则直接使用IP1
	U8  serverIP[3][18];	//服务器IP  (备用，域名解析失败时使用)
	U8 	serverDNS[3][32];	//域名(位数控制在30位以内)
	U8 	serverPort1[6];		//服务器端口(数据收发专用)
	U8 	serverPort2[6];		//服务器端口(软件无线升级专用)
	U8 	inquiryIP[18];		//快件状态在线查询服务器IP
	U8 	inquiryPort[6];		//快件状态在线查询端口
	
	U8	url[256];			//服务器地址
	
} glb_HycParameter_t, *pglb_HycParameter_t;


//备份信息
typedef struct _glb_HycParameter_Bak_t
{
	U32 CRC;	//校验和
	
	U8	companyId[20];	//公司编码
	
	U8	userID[20];		//员工编号
	U8	userPw[40];		//员工密码 (可能是MD5)
	
	U8	key[36];		//申请的key
	U8	groupId;		//分组编号
	
	U8	userRight;		//权限
	U8	userType;		//员工类型
	
	U8	pad;
	
}glb_HycParameter_Bak_t;

/*--------------------------------------------------------------------------
* external Variable ,constant or function
--------------------------------------------------------------------------*/
//system parameter that need to save
extern glb_HycParameter_t    	gtHyc;
extern glb_HycParameter_Bak_t	gtHycBak;

//System parameter that needn't to save

/*-----------------------------------------------------------------------------
* 函数:	glbVariable_LoadDefaultValue
* 功能:	load系统变量默认值
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadDefaultValue();

/*-----------------------------------------------------------------------------
* 函数:	glbVariable_CheckValue
* 功能:	检测且修正系统变量值，
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void glbVariable_CheckValue(void);

/*-----------------------------------------------------------------------------
* 函数:	glbVariable_LoadFromPersistentArea
* 功能:	从PersistentArea中load保存的系统变量
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadFromPersistentArea(void);

/*-----------------------------------------------------------------------------
* 函数:	glbVariable_SaveToPersistentArea
* 功能:	保存的系统变量到PersistentArea中
* 参数:	none
* 返回:	none
*----------------------------------------------------------------------------*/
void glbVariable_SaveToPersistentArea(void);


#ifdef __cplusplus
}
#endif

#endif //_GLOBAL_VARIABLE_H_
