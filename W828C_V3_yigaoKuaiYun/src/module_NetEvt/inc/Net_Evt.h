#ifndef _NET_EVT_H_
#define _NET_EVT_H_

/*===== include files =======*/
#include "hyTypes.h"


/*---------------------------------------------------*/
#define NET_FOREGROUND_EVT			0x80000000
#define NET_EVENT_MASK				0x7FFFFFFF

#define NET_LOGIN_EVT				0x00000001	//登录
#define NET_UPDATE_SOFT_EVT			0x00000002	//程序升级
#define NET_DOWNDATA_EVT			0x00000004	//下载资料
#define NET_UPDATE_TIME_EVT			0x00000008	//时间同步
#define NET_SENDREC_EVT				0x00000010	//上传记录
#define NET_UPDATE_DATA_EVT			0x00000020	//更新资料
#define NET_CLOSESOCKET_EVT			0x00000040	//关闭socket
#define NET_CLOSEPPP_EVT			0x00000080	//关闭ppp
#define NET_CLEARREC_EVT			0x00000100	//清空所有记录
#define NET_ADDREC					0x00000200	//从nand加载记录
#define NET_ADDMULTI_EVT			0x00000400	//从nand加载一票多件记录
#define NET_SENDMULTI_EVT			0x00000800	//发送一票多件记录
#define NET_SENDMULTI_F_EVT			0x00001000	//前台发送一票多件记录
//#define NET_APPLYKEY_EVT			0x00002000	//申请key
#define NET_CHECKSOFT_EVT			0x00002000	//检查软件版本

#define NET_STOP					0x40000000	//停止后台进程

extern U32 gu32NetEvt;


#endif //_NET_EVT_H_