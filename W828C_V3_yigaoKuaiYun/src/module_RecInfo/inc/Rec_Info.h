#ifndef _REC_INFO_H_
#define _REC_INFO_H_

/*===== include files =======*/
#include "hyTypes.h"
#include "Op_Struct.h"

/*---------------------------------------------------*/
//#define ADJUST_EXIST_MAX	10		//重复扫描判断  最大数


/************************操作数据结构**********************/


/////////////////////////////////记录状态定义///////////////////////////////////////
#define RECORD_UNSEND			0 //未发送
#define RECORD_SEND				1 //已发送
#define RECORD_DELETE			2 //已删除
#define RECORD_WAITSEND			3 //待发送
#define RECORD_ENABLESEND		4 //可发送


typedef struct _Rec_Info_t
{
	int total;
	int	reported;
	int unreported;
	U8	recType[50];
	U8	cnt;
	U8	max;//recType 个数
	
}tREC_INFO;

typedef struct _Rec_Search_t
{
	U16 fileId;
	U16 recIdx;
	U8	recState;
	U8  ifimage;
	U8	barCode[BARCODE_LEN];
	
}tREC_SEARCH;


//初始化时对记录存储库的配置信息
typedef struct _Rec_Config_t
{
	U16		groupMax;	//最大打包上传数
	U16		existMax;	//重复扫描判断的最大数
	U16		delaySend;	//延迟发送的最大时间 (10ms为单位)
	
}tREC_CONFIG;

typedef struct _exist_check_t
{
	U8 code[32];
	U8 other[32];
	
}tEXIST_CHECK;

//判断重复扫描
typedef struct _tExist_
{
	int	rd;
	int	wr;
	int	maxcnt;
	int	curcnt;
	
	tEXIST_CHECK	*exist;
	
}tREC_EXIST;


/*记录的公共部分,如果具体业务的结构中有和该公共部分一样的元素,业务结构中可以去掉,节省空间
	共96字节 */
typedef struct _data_comm_t
{
	U8		opType;		//业务操作类型
	U8		right:5;	//登录用户权限  及类别
	U8		state:3;	//记录的状态
	U8		groupId;	//用户分组id
	U8		nopacket:4;	//该记录不能打包发送
	U8		ifimage:4;	//是否图片
	U8		userId[EMPLOYEE_ID_LEN];	//登录用户工号
	U8		scanStat[COMPANY_ID_LEN];	//扫描站点
	U8		code[BARCODE_LEN];		//单号, 一票多件时,是票号
	U8		subCode[BARCODE_LEN];		//子单号, 一票一件时为空   一票多件时为当前子单号, 
										//一票多件最后结束时要填与code一样的信息,程序根据该信息判断本票的完成
	
	U8		scanTime[16];//扫描时间 20120721102353
	
}tDATA_COMM;

#define DATABUF_MAXLEN		(512-sizeof(tDATA_COMM))		//记录结构的最大值  不能大于该值

//业务数据  每条记录最大512字节  再去掉前面的一些信息, 就是本union的最大大小
typedef union _data_buf_u
{
	//把不同操作类型的数据结构放在这里,  ::注意大小
	
	tSAOMIAO	saomiao;
	tLUDAN	    ludan;
	tGPSDATA	gps;
	tDAISHOU	daishou;

	tTESTOP		testOp;		//测试业务
	
	//最大
	char	dataBuf[DATABUF_MAXLEN];
	
}uDATA_BUF;

//记录的数据格式
typedef struct _data_info_t
{
	//记录结构的公共部分
	tDATA_COMM	tComm;
	
	//以上共96byte  所以uDATA_BUF最大为512-96=416
	
	uDATA_BUF	uData;
	
}tDATA_INFO;


//外部初始化该库时需要填写的结构
typedef struct _store_cfg_t
{
	U8		groupMax;		//记录打包发送的最大打包数
	U8		delaySecond;	//延迟发送时间(秒 为单位)
	
	U8		*pSmallRecBuf;	//小记录存储需要使用的buf
	U32		smallBufSize;	//pSmallRec 大小   >=12K
	
	U8		*pLargeRecBuf;	//大记录存储需要使用的buf
	U32		largeBufSize;	//pLargeRec 大小   >=64K
	
	U8		*pQueBuf;		//发送队列buf
	U32		queBufSize;		//pQueBuf 大小
	
	U8		*pMultiCodeBuf;	//一票多件时使用的buf
	U32		multiBufSize;	//pMultiCodeBuf 大小    >=250K
	
	U8		*pExistBuf;		//重复扫描判断的buf
	U32		existBufSize;	//pExistBuf 大小
	
}tSTORE_CFG;

extern U32		gu32RecUnSendTotal;//未上传记录总数
extern U32		gu32RecUnSendImage;//未上传图片总数
extern U32		gu32RecInNandTotal;//未上传记录在nand上未被加载的条数
extern U32		gu32LastScanTime;//最后一次扫描时间
extern U32		gu32MultiInNand;

extern tREC_EXIST	gtRecExist;

#endif //_REC_INFO_H_