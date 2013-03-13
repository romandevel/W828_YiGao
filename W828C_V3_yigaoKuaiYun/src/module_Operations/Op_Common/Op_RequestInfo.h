#ifndef _OP_REQUESTINFO_H
#define _OP_REQUESTINFO_H


#include "hyTypes.h"
#include "Op_Config.h"


//网络请求接口类型  
typedef enum
{
	REQ_COMPANY=0,	//0 查询公司
	REQ_EMPLOYEE,	//1 查询员工
	REQ_PDATIME,	//2 更新终端时间
	REQ_RIGHT,		//3 查询权限 登录
	REQ_LUDAN,		//4 录单
	REQ_GETLUNDAN,  //5 获取录单信息
	REQ_PACKET,		//6 查询包裹
	REQ_UPGRADE,	//7 查询升级
	REQ_LOCATION,	//8 上传位置信息
	REQ_GPS,		//9 更新位置信息
	
	REQ_DOWNSITE,   //10获取收派区域
	// new -----------------------------
	REQ_LIU,		//11 留仓
	REQ_SAO,		//12 扫描
	REQ_PHOTO,		//13 拍照
	REQ_CHENG,		//14 称重
	REQ_XIE,		//15 卸车
	REQ_ZHUANG,		//16 装车
	REQ_JIE,		//17 解车
	REQ_FENG,		//18 封车
	REQ_MSG ,       //19 通知
	REQ_WJP,		//20 网页计泡
	REQ_ZXGD,		//21 工单
	//-----------------------------------
	REQ_GETWEIVOL,  //22 重量体积获取	
		 
	REQ_GROUP,		//23 查询分组   NOUSE
	REQ_PROBLEM,	//24 查询问题件 NOUSE		
	
	REQ_YICHANGSTR,	//25 获取异常类型	
	
	REQ_SIMID,		//26 获取sim卡号
	
	REQ_ER_COMPANY,	//27 欠费公司
	
	REQ_ALL
	
}eNET_REQTYPE;


///////////////////////////extern////////////////////////
extern eNET_REQTYPE	geReqType;

extern const U8 *Req_ver[];
extern const U8 * const Req_name[];
extern const U8 * const Net_ErrorCode[];



#endif//_OP_REQUESTINFO_H