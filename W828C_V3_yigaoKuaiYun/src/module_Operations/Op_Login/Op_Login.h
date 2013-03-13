#ifndef _LOGIN_H_
#define _LOGIN_H_

#include "hyTypes.h"
#include "Op_Common.h"
#include "Op_Config.h"




//管理员账号
#define	ADMIN_ID		"00000000"
#define ADMIN_PW		"34290323"

//硬件测试账号
#define HARDWARE_ID		"0000"
#define HARDWARE_PW		"4926"

//业务测试账号
#define OPTEST_ID		"000000"
#define OPTEST_PW		"021"


//分组信息
typedef struct _tGroupInfo
{
	U8  groupId[12];	//分组编号
	U8  groupName[52];	//分组名

}tGROUPINFO;


//登录信息
typedef struct _logoinfo_t
{
	U8	userID[20];		//员工编号
	U8	userPw[20];		//员工密码
	U8	userName[32];	//员工姓名
	U8	userRight;		//员工权限
	U8	userType;		//员工类型
	U8  groupTotal;		//分组总数
	U8  groupDef;		//默认分组
	U8  groupSelect;	//员工所在分组索引
	
	struct _tGroupInfo	*pGroup;	//员工所在分组名称
	
	U32 landTime;		//上次验证时间
	
}tLOGOINFO;




















void Contact_Desk(void *pDeskFather);



#endif//_LOGIN_H_