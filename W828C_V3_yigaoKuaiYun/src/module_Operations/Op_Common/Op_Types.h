#ifndef OP_TYPES_H
#define OP_TYPES_H


#include "Op_Config.h"

/*
港中能达
寄件录单录单
收件扫描
发件扫描
发件确认
中转发件
到件扫描
到件确认
派件扫描
签收
装袋
拆袋
问题
留仓
*/
//业务操作类型 具体业务值根据具体客户需求修改
typedef enum _op_type_e
{
	HYCO_OP_SJ=1,	//收件
	HYCO_OP_FJ=2,	//发件
	HYCO_OP_DJ=3,	//到件
	HYCO_OP_PJ=4,	//派件
	
	HYCO_OP_ZD=5,	//装袋
	HYCO_OP_CD=6,	//拆袋
	
	HYCO_OP_WENTI=7,	//问题件
	HYCO_OP_LIUCANG=8,	//留仓件
	
	HYCO_OP_QS=9,		//签收
	
	HYCO_OP_DSHK=10,	//代收付款
	
	HYCO_OP_FJQR=11,	//发件确认
	HYCO_OP_ZZFJ=12,	//中转发件
	HYCO_OP_DJQR=13,	//到件确认
	
	HYCO_OP_LD=14,		//签收录单
	
	//HYCO_OP_15=15,
	//HYCO_OP_16=16,
	//HYCO_OP_17=17,
	//HYCO_OP_18=18,
	//HYCO_OP_19=19,
	//HYCO_OP_20=20,
	
	HYCO_OP_SJWT=21,	//称重收件
	HYCO_OP_FJWT=22,	//称重发件
	HYCO_OP_DJWT=23,	//称重到件
	HYCO_OP_PJWT=24,	//称重派件

	HYCO_OP_TEST=254,	//测试业务
	HYCO_TYPE_MAX = 255,//做统计报表用
	
}eOPTYPE;


#endif //OP_TYPES_H