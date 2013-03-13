#ifndef _OP_STRUCT_H
#define _OP_STRUCT_H


#include "hyTypes.h"


//定义一些信息的长度
#define COMPANY_NAME_LEN		64	//网点名称
#define COMPANY_ID_LEN			12	//网点编号
#define	COMPANY_RIGHT_LEN		4	//公司权限

#define EMPLOYEE_ID_LEN			16	//员工编号
#define EMPLOYEE_NAME_LEN		40	//员工名称
#define EMPLOYEE_PW_LEN			16	//员工密码
#define EMPLOYEE_ROLE_LEN		16	//员工角色

#define PROBLEM_ID_LEN			8	//问题类型编码
#define PROBLEM_CODE_LEN		8	//问题类型代码
#define PROBLEM_NAME_LEN		32	//问题类型名称
#define PROBLEM_TYPE_LEN		4	//问题类型

#define LEAVE_CODE_LEN		8	//问题类型代码
#define LEAVE_NAME_LEN		32	//问题类型名称

#define CLASS_ID_LEN			8	//运输方式编码
#define CLASS_NAME_LEN			16	//运输方式名称

#define FLIGHT_ID_LEN			16	//航空信息编码
#define FLIGHT_NAME_LEN			64	//航空信息名称

#define CAR_ID_LEN				16	//车辆信息编码
#define CAR_NAME_LEN			64	//车辆信息名称

#define CAR_INDEX_LEN			16	//班次
#define	BARCODE_LEN				24	//条码
#define WEIGHT_LEN				8	//重量
#define FEE_LEN					8	//运费

#define	REASON_ID_LEN			4	//问题类型
#define REASON_DISC_LEN			32	//问题描述
#define CUSTOMER_ID_LEN			20	//客户编号
#define CUSTOMER_NAME_LEN		10	//客户名称

#define PHONE_NUM_LEN			16	//电话号码长度
#define REMARK_LEN				80	//备注

#define BCHID_LEN				30	//批次长度
#define GOODS_NAME_LEN			20	//货物名称

#define COMMON_LESSTHAN8		8	//小于等于8的都可以使用
#define COMMON_LESSTHAN4		4	//小于等于4的都可以使用


/*++++++++++++++++++++++++++++++++++业务结构+++++++++++++++++++++++++++++++++++++++*/
//装  封  解  卸  
//收  发  到  派  
//扫  留  称  签  


//扫描业务上传数据结构   416
typedef struct _SaoMiao_
{
	U8      bchId[BCHID_LEN]  ;    //批次编号  默认为时间+巴枪编号
	
	U8		othercode[BARCODE_LEN];	//转单号/或其他单号
	U8		packetcode[BARCODE_LEN];	//大包号
	U8      obj_ids[COMMON_LESSTHAN4]   ;  //货样编码 	 20:大货 21:文件
	U8		cheliang[BARCODE_LEN];	//车辆码    如：京G56842  编码为：010G56842）   9或10位	
	U8		chexian[BARCODE_LEN];	//车线码    10位阿拉伯数字编码
	U8      wei_tpyes[COMMON_LESSTHAN4] ;  //重量类型   1:大 2:中 3:小
	U8      obj_wei[COMMON_LESSTHAN8]   ;  //物品重量
	U8      obj_len[COMMON_LESSTHAN8]   ;  //物品长
	U8      obj_wid[COMMON_LESSTHAN8]   ;  //物品宽
	U8      obj_hei[COMMON_LESSTHAN8]   ;  //物品高
	U8      vol_tpyes[COMMON_LESSTHAN4] ;  //体积类型   1:大 2:中 3:小
	U8      obj_vol[COMMON_LESSTHAN8]   ;  //物品体积
	U8      tran_types[COMMON_LESSTHAN4];  //运输类型
	U8      cmt_ids[COMMON_LESSTHAN4]   ;  //备注编码
	U8      cmt_inf[REMARK_LEN] ;  //备注信息
	U8      origstat[COMPANY_ID_LEN];    //始发站点
	U8      prevstat[COMPANY_ID_LEN]  ;  //上级站点
	U8      nextstat[COMPANY_ID_LEN]  ;  //下级站点
	U8      nextprov[COMMON_LESSTHAN8];   //下级省份
	U8      deststat[COMPANY_ID_LEN];    //目的站点
	U8      dest_zone[COMPANY_ID_LEN];  //目的区划
	U8      customer[EMPLOYEE_NAME_LEN] ;  //客户名称
	U8      deliverer[EMPLOYEE_ID_LEN];  //派送人员
	U8		daishou_fee[FEE_LEN];//代收款
	U8		daofu_fee[FEE_LEN];//到付款
	U8		worktype;	//工单类型  91 卸车分工  92 装车分工
	U8		signType;			//签收类别
	U8      picflag;  			//是否拍照

}tSAOMIAO;


//代收付款
typedef struct _daishou_t
{
	U8		collect_id[EMPLOYEE_ID_LEN];	//收件员
	U8		pay_type[REASON_ID_LEN];		//支付类型id
	U8		pay_name[REASON_ID_LEN];		//支付类型名称
	
	U8		cost_trans[COMMON_LESSTHAN8];	//代收付款运费
	U8		money[COMMON_LESSTHAN8];		//代收金额
	U8		cost_protect[COMMON_LESSTHAN8];//保价金额
	
	U8		customer_id[CUSTOMER_ID_LEN];	//客户编号
	
}tDAISHOU;


//GPS数据上传结构    经度，维度，高度，采集时间   156
typedef struct _tGpsData_
{
	U8      bchId[BCHID_LEN]  ;    //批次编号  默认为时间+巴枪编号
	
	U8		Long[14];		//经度
	U8		Lat[14];		//纬度
	U8		high[8];		//高度
	U8      addr[40];       //地址
	
	U8		date[20];		//日期时间
	U8		machinecode[18];//巴抢编号
	U8		simcard[12];	//SIM卡号

}tGPSDATA;


//录单   461
typedef  struct  _ludan_t
{
	U8      bchId[BCHID_LEN];    //批次编号  默认为时间+巴枪编号
	
	U8      sub_code[BCHID_LEN];
	U8      deststat[COMPANY_ID_LEN];//目的地(站点)
	U8      obj_wei[WEIGHT_LEN];//重量
	U8		num[COMMON_LESSTHAN8];//件数
	U8		payType;//支付方式
	U8		daishouCode[BARCODE_LEN];//代收签单
	U8		feeDaishou[FEE_LEN];//代收货款
	U8		feeTotal[FEE_LEN];//费用
	U8		customer_id[CUSTOMER_ID_LEN];//客户
	U8		tranType;//运输方式
	U8		goodsType;//物品类别
	U8		goodsName[GOODS_NAME_LEN];//物品名称
	U8		moneyType;//币种
	U8		ifNotice;//短信通知
	U8		noticeNum[PHONE_NUM_LEN];//通知号码
	U8		recvName[CUSTOMER_NAME_LEN];//收件人姓名
	U8		recvPhone[PHONE_NUM_LEN];//收件人电话
	U8		recvAddr[REMARK_LEN];//收件人地址
	U8		sendName[CUSTOMER_NAME_LEN];//寄件人姓名
	U8		sendPhone[PHONE_NUM_LEN];//寄件人电话
	U8		sendAddr[REMARK_LEN];//寄件人地址
	
}tLUDAN;


//测试业务
typedef struct _test_op_t
{
	U8 Station[COMPANY_ID_LEN];//下一站
	
}tTESTOP;




#endif//_OP_STRUCT_H