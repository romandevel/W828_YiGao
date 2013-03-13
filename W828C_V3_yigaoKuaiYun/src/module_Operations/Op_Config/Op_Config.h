#ifndef _OP_CONFIG_H
#define _OP_CONFIG_H

#include "hyTypes.h"

typedef int (*OpConfigFun)(void *p);


//业务功能选择开关

//由于每个公司数据接口格式都不一样,这里做区分
#define	VER_CONFIG_HYCO		//浩创
//#define	VER_CONFIG_YUNDA	//韵达
//#define	VER_CONFIG_ZTO		//中通
//#define	VER_CONFIG_YIDA		//义达
//#define	VER_CONFIG_GZND		//港中能达





//实际业务在gaOp_Config中的位置,有些客户需要2项统计,有些客户只需一项
#define OP_CONFIG_OFFSET	1


//业务信息配置 操作选择显示/查询/删除错扫/发送/查询显示
typedef struct _op_config_t
{
	U16		right;			//权限(可能不同的权限看到的业务不一样)
	U16		opType;			//业务类型
	U8		*pName;			//业务名称
	
	OpConfigFun		opFun;		//业务操作
	OpConfigFun		sendFun;	//网络发送
	OpConfigFun		searchFun;	//查询
	OpConfigFun		dispFun;	//显示查询结果
	OpConfigFun		delFun;		//删除错扫
	OpConfigFun		sendFunMore;//一票多件发送
	
}tOP_CONFIG;


//网络发送时使用的结构体
typedef struct _op_send_info
{
	U8		*pSend;		//发送时使用的buf
	int		bufLen;		//发送buf的长度
	U8		*pNode;		//一票一件是tQUE_NODE类型,一票多件是tMULTI_CODE类型,使用时需转换. 需要发送的数据
	int		cnt;		//需要发送数据的个数   一票多件时用不到
	U8		*pUrl;		//地址
	
}tOP_SENDINFO;

//记录查询结果显示时使用的结构体
typedef struct _op_disp_info
{
	U8		*pData;	//查询到的数据 tDATA_INFO 类型,使用时转换
	int		total;	//查询到的总数
	U8		*pDispBuf;	//提供显示的buf
	int		dispBufLen;	//显示buf的长度
	
}tOP_DISPINFO;


typedef union _op_cfg_u
{
	void	*p;
	tOP_SENDINFO	tSend;
	tOP_DISPINFO	tDisp;
	
}uOP_CFG;


typedef struct _op_callback_t
{
	U8	right;	//用户权限
	U8	funType;//功能类型  0:操作接口    1:发送接口   2:查询接口   3:查询显示接口    4:删除接口	5:一票多件发送
	U16	opType; //业务类型
	
	uOP_CFG	uOpcfg;
	
}tOP_CALLBACK;


//////////////////////////////////extern//////////////////////////
extern const tOP_CONFIG gaOp_Config[];


#endif //_OP_CONFIG_H