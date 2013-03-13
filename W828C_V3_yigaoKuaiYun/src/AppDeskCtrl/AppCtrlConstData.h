#ifndef _APP_CONTROL_CONSTDATA_H_
#define _APP_CONTROL_CONSTDATA_H_


#define	APP_ATTR_SYSTEM_AA				0x01		/* 系统进程*/
#define	APP_ATTR_EXCLUSIVE_AA			0x02		/* 独占的应用进程*/
#define	APP_ATTR_NON_EXCLUSIVE_AA		0x04		/* 不要求独占的应用进程*/
#define	APP_ATTR_ONLY_FOREGROUND_AA		0x08		/* 只可前台运行的应用进程*/
#define	APP_ATTR_ALLOW_BACKGROUND_AA	0x10		/* 允许后台运行的应用进程*/


typedef struct process_config_st
{
	int (*entry)();		/* 进程运行的入口*/
	char *pname;		/* 进程名字 */
	U16  stackSize;		/* 进程stack size，字为单位*/
	U16	 priority;		/* 进程优先级 */
						/* 每个进程都有一个独立的专用来接收的消息队列*/
	U32	 attribute;		/* 进程的属性 */					
						
	U16	 msgQueDeep;	/* 接收的消息队列的深度*/
	U16	 msg_maxSize;	/* 接收的消息队列的最大消息的长度，单位为字节*/	

}tPROCESS_CONFIG;


//系统要求应用进程退出时，一些相关数据的记录
typedef struct _tSYS_REQ_QUIT_INF
{
	U8              ReqPidNum;      /*要求退出的应用进程的个数*/
	U8              RecPidNum;      /*已经收到退出应用的个数*/
	
	eAPPLICATION_NO StartPid;       /*需要启动的pid*/
	eAPPLICATION_NO MyPid;          /*要求启动的pid*/
	U32             Flag;           /*进程启动时的附加信息*/
//	U8              State;          /*可用作电话状态*/
	void (*AppCtrl_Action)(void);   /*进程启动动作*/
}tSYS_REQ_QUIT_INF;


/* 进程的初始配置表，顺序一定要和 枚举 eAPPLICATION_NO的一致 */
extern int USBHandler_ThreadMethod(void);
extern int PowerOn_ThreadMethod(void);
extern int PowerDown_ThreadMethod(void);
extern int Shell_ThreadMethod(void);
extern int TcpIp_ThreadMethod(void);
extern int Kernal_App_ThreadMethod(void);
extern int GsmCdmaServer_ThreadMethod(void);
extern int GsmCdmaClient_ThreadMethod(void);
extern int ppp_ThreadMethod(void);
extern int WiFiDriver_ThreadMethod(void);
extern int hycoBt_main(void);
extern int Net_ThreadMain(void);
extern int Gps_Process(void);


extern const tPROCESS_CONFIG gtproc_Config[TOTAL_APP_AN];
#endif