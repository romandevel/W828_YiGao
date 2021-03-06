#ifndef _REC_STORE_API_H
#define _REC_STORE_API_H


#include "hyswNandFile.h"
#include "Rec_Info.h"
#include "Rec_Store.h"


/*--------------------------------------
*函数：Rec_InfoInit
*功能：信息初始化
*参数：pStoreCfg:配置信息
*返回：0:成功  其他:失败
*--------------------------------------*/
int Rec_InfoInit(tSTORE_CFG *pStoreCfg);


/*--------------------------------------
*函数：Rec_GetExistMax
*功能：获取最大打包上传数
*参数：none
*返回：最大打包上传数
*--------------------------------------*/
U16 Rec_GetGroupMax();

/*--------------------------------------
*函数：Rec_GetDelaySend
*功能：获取延迟发送的最大时间
*参数：none
*返回：延迟发送的最大时间
*--------------------------------------*/
U16 Rec_GetDelaySend();

/*--------------------------------------
*函数：Rec_GetUnSendCnt
*功能：获取有多少条记录未上传   包括一票多件的
*参数：none
*返回：未上传的记录条数
*--------------------------------------*/
U32 Rec_GetUnSendCnt();

/*--------------------------------------
*函数：Rec_GetMultiCnt
*功能：获取有多少票一票多件的单子   !!!!并不一定是准确数据
*参数：none
*返回：
*--------------------------------------*/
U32 Rec_GetMultiCnt();

/*--------------------------------------
*函数：Rec_DelMultiCnt
*功能：减少一个一票多件的单子
*参数：none
*返回：
*--------------------------------------*/
void Rec_DelMultiCnt();

/*--------------------------------------
*函数：Rec_GetMultiSendType
*功能：获取本次一票多件的发送方式
*参数：none
*返回：
*--------------------------------------*/
U8 Rec_GetMultiSendType();

/*--------------------------------------
*函数：Rec_GetInNandCnt
*功能：获取有多少条记录在nand上未被加载
*参数：none
*返回：未被加载的记录条数
*--------------------------------------*/
U32 Rec_GetInNandCnt();

/*--------------------------------------
*函数：Rec_GetCurDay
*功能：得到今天日期的字符串  年2位 月2位 日2位   例：2010年12月20   101220
*参数：pStr:接收的buf   len：buf的长度
*返回：none
*--------------------------------------*/
void Rec_GetCurDay(U8 *pStr, int len);

/*--------------------------------------
*函数：Rec_GetTimeStr
*功能：得到时间字符串  格式:yyyymmddhhmmss
*参数：pStr:接收的buf
*返回：none
*--------------------------------------*/
void Rec_GetTimeStr(char *pStr);

/*-----------------------------------------------------------
*函数：Rec_CheckExist
*功能：检查是否存在相同的条码
*参数：pCode:扫描码
*返回：-1 条码不存在    ==0 条码已存在
*------------------------------------------------------------*/
int Rec_CheckExist(U8 *pCode);

/*-----------------------------------------------------------
*函数：Rec_AddExist
*功能：添加一个条码
*参数：pCode:扫描码
*返回：-1 失败    ==0 成功
*------------------------------------------------------------*/
int Rec_AddExist(U8 *pCode);

/*-----------------------------------------------------------
*函数：Rec_DelExist
*功能：删除一个已存在条码
*参数：pCode:扫描码
*返回：-1 失败    ==0 成功
*------------------------------------------------------------*/
int Rec_DelExist(U8 *pCode);

/*-----------------------------------------------------------
*函数：Rec_DelExistCnt
*功能：减少计数
*参数：
*返回：none
*------------------------------------------------------------*/
void Rec_DelExistCnt();

/*-----------------------------------------------------------
*函数：Rec_ClearExist
*功能：清空重复条码判断计数
*参数：
*返回：none
*------------------------------------------------------------*/
void Rec_ClearExist();


/*--------------------------------------
*函数：Rec_WriteNewRecode
*功能：写入新记录    pdata->tComm.state为0表示直接可以添加到上传队列中的,为3表示需要等待结束后才能统一打包上传的
*参数：pData：要写入的数据
*返回：成功：0   失败：-1
*--------------------------------------*/
int Rec_WriteNewRecode(tDATA_INFO *pdata);


/*-----------------------------------------------------------
*函数：RecStore_GetAllUnreported
*功能：加载所有未上传的记录   同时会加载一票一件和一票多件
*参数: usertype : 权限
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_GetAllUnreported(U8 usertype);

/*-----------------------------------------------------------
*函数：RecStore_GetUnreportedMulti
*功能：加载可传状态的一票多件记录
*参数: usertype : 权限
       sendType:  发送方式  0:后台发送    1:前台发送
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_GetUnreportedMulti(U8 usertype, U8 sendType);

/*-----------------------------------------------------------
*函数：RecStore_ChangeState
*功能：改变指定用户类型  业务类型  条码    的状态
*参数：usertype:用户类型	   
	   type:指定类型
	   code:扫描码
	   changeState:想要改变成的状态
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：0
*------------------------------------------------------------*/
int RecStore_ChangeState(U16 usertype, U8 type, U8 *pcode, U8 changeState);


/*-----------------------------------------------------------
*函数：RecStore_ChangeMultiCodeState
*功能：改变指定用户类型  业务类型  条码    的状态   (一票多件使用)    可能的切换:1:待传-->可传   2:待传-->删除  3:可传-->已传   4:可传-->删除
*参数：usertype:用户类型	   
	   type:操作类型
	   pcode:主单号
	   changeState:想要改变成的状态
*返回：失败：-1,  正在上传: -2   已上传: -3    成功：0
*------------------------------------------------------------*/
int RecStore_ChangeMultiCodeState(U16 usertype, U8 type, U8 *pcode, U8 changeState);


/*-----------------------------------------------------------
*函数：RecStore_DelErrorScan
*功能：删除 某一记录 (根据权限,扫描类型,扫描码)
*参数：usertype:用户类型	   
	   type:指定类型
	   code:扫描码  (可能是一票一件, 可能是一票多件的主单号或子单号)
*返回：成功删除的个数
*------------------------------------------------------------*/
int RecStore_DelErrorScan(U16 usertype, U8 type, U8 *pcode);

/*-----------------------------------------------------------
*函数：RecStore_GetRecordStateNum
*功能：得到指定用户类型 日期 类型的记录数据的条数  (总数  已上传  已删除)
*参数：usertype:用户类型
	   pDate_name:指定的日期   如果pEntry不为NULL 
	   type:指定类型
	   total: 总数
	   rep ：已上传
	   unrep：未上传
*返回：失败：-1    成功：0
*------------------------------------------------------------*/
int RecStore_GetRecordStateNum(U8 *pDate_name, U16 usertype, U8 type, U32 *total, U32 *rep, U32 *unrep);










#endif