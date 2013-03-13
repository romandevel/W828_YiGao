#include "Op_RequestInfo.h"
#include "glbVariable_base.h"
#include "glbVariable.h"

#ifdef VER_CONFIG_HYCO

extern char	ga8CompanyVer[];


eNET_REQTYPE	geReqType;

//////////////////////////////////////////////////////////////////
//请求接口的版本,
const U8 *Req_ver[] = 
{
	ga8CompanyVer,//0 查询公司
	//"1.0",//查询公司  强制全部更新时才使用 否则使用文件中记录的版本
	
	"1.0", //1 查询工号
	"1.0", //2 更新终端时间
	"2.0", //3 查询权限
	"5.0", //4 录单
	"3.0", //5 获取录单信息
	"2.0", //6 查询包裹
	"2.0", //7 查询升级
	"1.0", //8 上传位置信息
	"2.0", //9 更新位置信息
	
	"1.0", //10 获取收派区域
	
	//new -------------------------------
	"1.0", //11 留仓
	"1.0", //12 扫描
	"1.0", //13 拍照
	"1.0", //14 称重
	"1.0", //15 卸车
	"1.0", //16 装车
	"1.0", //17 解车
	"1.0", //18 封车
	"1.0", //19 通知
	"1.0", //20 重量体积获取
	//-----------------------------------
			 
	"1.0",	//21 查询分组   NOUSE
	"1.0",	//22 查询问题件 NOUSE	
	
	"1.0",	//23 异常件类型
	"1.0",	//24 获取sim卡号
};
//请求类别  注意一一对应
const U8 * const Req_name[]=
{
	(const U8 *)"getcompanylist",	//0  查询公司
	(const U8 *)"getusers",		    //1  查询工号
	(const U8 *)"getlastcfg",		//2  更新终端信息
	(const U8 *)"getright",		    //3  查询权限
	(const U8 *)"postdandata",      //4  录单
	(const U8 *)"getdocdtl",		//5  录单内容获取
	(const U8 *)"getpacksts",		//6  查询包裹
	(const U8 *)"getgunupdate",		//7  查询升级
	(const U8 *)"postaddress",		//8  上传位置信息
	(const U8 *)"postgpsdata",		//9  更新位置信息	
	
	(const U8 *)"getsiteinfo",	    //10 获取收派区域
	// new -------------------------------------
	
	(const U8 *)"op01liu",		    //11 留仓
	(const U8 *)"op02sao",		    //12 扫描
	(const U8 *)"op03pai",          //13 拍照
	(const U8 *)"op04chz",		    //14 称重
	(const U8 *)"op05xcd",		    //15 卸车
	(const U8 *)"op06zcd",		    //16 装车
	(const U8 *)"op07jcd",		    //17 解车
	(const U8 *)"op08fcd",		    //18 封车
	(const U8 *)"op09msg",		    //19 通知
	(const U8 *)"getweivol",		//20 重量体积获取
	//------------------------------------------
		
	(const U8 *)"getgroup",		    //21 查询分组 NOUSE
	(const U8 *)"getproblem",		//22 问题件	  NOUSE
	
	
	(const U8 *)"getabsigninfo",		//23 获取异常类型
	(const U8 *)"getsimid",			//24 获取sim卡号
};

//返回错误代码
const U8 * const Net_ErrorCode[]=
{
	(const U8 *)"其他错误",
	
	(const U8 *)"账号或密码错误", //1：密码错误
	(const U8 *)"请求缺失",       //2：请求缺失
	(const U8 *)"请求错误",       //3：请求错误
	(const U8 *)"系统维护",       //4：系统错误
	(const U8 *)"完整校验失败(字段缺失)",  //5：完整校验失败（文件不全或字段缺失）
	(const U8 *)"数据库中断(无法验证权限)",//6：数据库中断（无法验证权限）
	(const U8 *)"记录添加失败"
};


///////////////////////////////////////////////////////////
void NetReq_SetFrontReqType(eNET_REQTYPE type)
{
	geReqType = type;
}

eNET_REQTYPE NetReq_GetFrontReqType()
{
	return geReqType;
}


/*-----------------------------------------
* 函数: NetReq_FillRequestHead
* 功能: 根据不同接口,填充请求头
* 参数: kReqType:请求类型   pHead:请求头buf  pId:员工编号
* 返回: 0
*-----------------------------------------*/
int NetReq_FillRequestHead(eNET_REQTYPE kReqType, U8 *pHead, U8 *pCode, U8 *pId)
{
	int ret = 0;
	
	if(kReqType < REQ_ALL)
	{
	hyUsbPrintf("FillRequestHead  id = %s \r\n", pId);
		if(kReqType > REQ_UPGRADE && kReqType < REQ_ALL)//使用上次登录信息请求数据(主要是退出登录后的上传)
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
			Req_ver[kReqType],glbVar_GetCompanyId(),gtHycBak.userID,gtHycBak.userPw,glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else if(strncmp(pId,"000000",6) == 0)//管理员登录使用默认密钥
		{//管理员
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user></user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
			Req_ver[kReqType],glbVar_GetCompanyId(),glbVar_GetSysKey(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else if(REQ_RIGHT == kReqType)// 登陆时候用
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
					Req_ver[kReqType],pCode,pId,Login_GetPw_MD5(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
					Req_ver[kReqType],glbVar_GetCompanyId(),pId,Login_GetPw_MD5(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		
		hyUsbPrintf("pHead  =  %s  \r\n", pHead);
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}


int NetReq_FillUpdateTimeHead(U8 *pHead)
{
	sprintf(pHead,"<req op=\"%s\"><h><ver>%s</ver><company>%s</company><user></user>\
<pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h></req>",
			Req_name[REQ_PDATIME],Req_ver[REQ_PDATIME],glbVar_GetCompanyId(),/*gtHyc.impower*/"JKrqwU3sSTfMh4bu",glbVar_GetMachineId(),glbVar_GetSimCode());
	
	return 0;
}


int NetReq_FillHead_ZTO(U8 *pHead, U8 *pType)
{
	int len;
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><h><ver>1.0</ver><optype>%s</optype><manufacturer>HYCO</manufacturer>\
<u_company>%s</u_company><u_code>%s</u_code><sys_sn>%s</sys_sn><key>%s</key></h>", 
			pType, glbVar_GetCompanyId(),Login_GetUserId(),glbVar_GetMachineId(),glbVar_GetSysKey());
	
	len = strlen(pHead);
	
	return len;
}


int NetReq_FillHead_yida(U8 *pHead, U8 *ver, U8 *pMethod)
{
	int len;
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"GBK\"?><senddata><accessMethod>%s</accessMethod><versionTime>%s</versionTime>\
<baseValue><dev>%s</dev><userName>%s</userName><password>%s</password><belongSite>%s</belongSite></baseValue>",
				pMethod, ver, glbVar_GetMachineId(),Login_GetUserId(),Login_GetUserPw(),glbVar_GetCompanyId());
	
	len = strlen(pHead);
	
	return len;
}

int NetReq_FillHead_DHL(U8 *pHead, U16 type, U8 *pName)
{
	char *str[]={"","SJ", "FJ", "DJ", "PJ", "ZD", "CD", "WT", "LC", "QS"};
	int len, name[100];
	RtcTime_st	kRtc;
hyUsbPrintf("type = %d   str = %s \r\n", type, str[type]);	
	if(pName == NULL)
	{
		hyhwRtc_GetTime(&kRtc);
		sprintf(name, "%s%04d%02d%02d%02d%02d%02d-%s-%s.TXT", str[type],kRtc.year,kRtc.month,kRtc.day,kRtc.hour,kRtc.minute,kRtc.second,
												glbVar_GetCompanyId(), glbVar_GetMachineId());
	}
	else
	{
		sprintf(name, "%s.jpg", pName);
	}
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"GBK\"?><senddata><userID>%s</userID><password>%s</password><belongSite>%s</belongSite><dev>%s</dev>\
<fileName>%s</fileName>",
				Login_GetUserId(),Login_GetUserPw(),glbVar_GetCompanyId(),glbVar_GetMachineId(),name);
	
	len = strlen(pHead);
	
	return len;
}

#endif //VER_CONFIG_HYCO