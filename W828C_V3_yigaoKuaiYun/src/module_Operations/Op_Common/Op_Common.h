#ifndef _OP_COMM_H
#define _OP_COMM_H


#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "Net_Evt.h"
#include "bmpComData.h"
#include "Rec_Queue.h"
#include "Op_Config.h"
#include "drvKeyArray.h"
#include "Op_RequestInfo.h"


typedef struct _scantype_str_t
{
	int	type;
	U8  *pStr;
	
}tTYPE_STR;


typedef  struct areaNode
{
	U8  *code ;
	U8  *area ;	
	struct areaNode *next ;
	U8  nodenum;
	
}AREAnode;


//错误代码
typedef enum _error_code_e
{
	ERR_SUCCESS=0,	//成功
	ERR_CONNECTFAIL,//网络连接失败
	ERR_CONNECTTIMEOUT,//网络连接超时
	ERR_SENDFAIL,	//网络通讯失败
	
	ERR_NEWEST,	//已经是最新版本
	ERR_DATAERROR,	//数据错误
	
	ERR_UPGRADEFAIL,	//更新失败
	
	ERR_OTHER = 0x80,	//其他错误,可能是网络返回的错误, &0x7f得到的值是网络返回的错误码
	
	ERR_NULL
	
}eERRCODE;

//主要是前台请求类型
/*typedef enum _gprs_reqtype_e
{
	REQ_LOGIN=1,	//登录
	REQ_UPGRADE,	//程序升级
	REQ_COMPANY,	//下载网点信息
	REQ_PROBLEM,	//问题件
	REQ_EMPLOYEE,	//员工信息
	REQ_GETWEIVOL,	//重量体积
	REQ_REGION,		//收派区域
	REQ_CARINFO,	//车辆信息
	
	REQ_SENDMULTI,	//发送一票多件
	
	REQ_NULL
	
}eNET_REQTYPE;*/


//业务操作页面可能用到的一些信息
typedef struct _opinfo_glb_t
{
	U8	curOptype;		//操作业务类型
	U8	tempIndex;		//下拉框临时使用
	
	U8	stationId1[COMPANY_ID_LEN];//上一站/始发站 编号
	U8 	station1[COMPANY_NAME_LEN];	//上一站/始发站
	U8	stationId2[COMPANY_ID_LEN];//下一站/目的站 编号
	U8 	station2[COMPANY_NAME_LEN];	//下一站/目的站
	
	U8	employeeId[EMPLOYEE_ID_LEN];//人员ID
	U8 	employee[EMPLOYEE_NAME_LEN];	//人员名称
	
	U8 	packet[BARCODE_LEN];		//大包号
	U8 	carCode[BARCODE_LEN];		//车辆码
	U8 	carLine[BARCODE_LEN];		//车线码
	
	U8 	weight[WEIGHT_LEN];			//重量
	U8 	fee1[FEE_LEN];				//费用
	U8 	fee2[FEE_LEN];				//费用
	
}tOPINFO_GLB;



/////////////////////////////////extern/////////////////////////////////////////////
extern const U8 OpCom_Month[];

extern tGRAPBUTTON *gpPhotoBtn;
extern U8 	 gau8PhotoCode[];
extern char *gpDcWorkBuf, *gpPreviewBuf;//拍照时用
extern U32	 gu32PhotoSize;
extern tGrapButtonBuffPic  rgb_buf;

extern tGRAPEDIT	*gpEdit_Stat1;	//上一站/始发站
extern tGRAPEDIT	*gpEdit_Stat2;	//下一站/目的站
extern tGRAPEDIT	*gpEdit_Employee;//人员
extern tGRAPEDIT	*gpEdit_Packet;	//大包号
extern tGRAPEDIT	*gpEdit_CarCode;//车辆码
extern tGRAPEDIT	*gpEdit_CarLine;//车线码
extern tGRAPEDIT	*gpEdit_Code;	//条码

extern tOPINFO_GLB	gtOpInfoGlb;

extern eERRCODE	geLastErrCode;

extern const AREAnode province[];




extern S32 ComWidget_LabTimerISR(void *pView);
extern int Net_SendRequest_F(void *p);
extern int Rec_Statistics(void *pDeskFather);

extern int BT_WeightEditTimer(void *pView);

S32 OpCom_EditDrawInputCnt(tGRAPEDIT *pEdit);

S32 OpComm_BtnBTDraw(void *pView, U32 xy, U32 wh);
S32 OpComm_BTEditDraw(void *pView, U32 xy, U32 wh);
int OpComm_ConnBT(tGRAPDESKBOX *pDesk, tGRAPEDIT *pEditWT);

S32 OpCom_LineInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos);
int OpComm_EditHandle_Price(void *pView, GRAP_EVENT *pEvent);

S32 OpComm_EditTimer(void *pView);

#endif//_OP_COMM_H