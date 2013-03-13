#include "Op_Config.h"
#include "Op_Types.h"


extern int Rec_Statistics(void *p);
extern int Rec_DelErrScanDesk(void *p);

/*------------------------------------------------------------------------------*/
//寄件录单
extern int Operation_LuDan_Desk(void *p);

/*收件操作*/
extern int Operation_ShouJian_Desk(void *p);
extern int Operation_ShouJianWT_Desk(void *p);
extern int Operation_ShouJianWtFee_Desk(void *p);

/*发件操作*/
extern int Operation_FaJian_Desk(void *p);
extern int Operation_FaJianWT_Desk(void *p);
//extern int Operation_FaJianWtFee_Desk(void *p);
extern int Operation_FaJianQR_Desk(void *p);
extern int Operation_FaJianZZ_Desk(void *p);

/*到件操作*/
extern int Operation_DaoJian_Desk(void *p);
extern int Operation_DaoJianWT_Desk(void *p);
extern int Operation_DaoJianWtFee_Desk(void *p);
extern int Operation_DaoJianQR_Desk(void *p);

/*派件操作*/
extern int Operation_PaiJian_Desk(void *p);
extern int Operation_PaiJianWT_Desk(void *p);
extern int Operation_PaiJianWtFee_Desk(void *p);

//签收
extern int Operation_QianShou_Desk(void *p);

//装袋
extern int Operation_ZhuangDai_Desk(void *p);

//拆袋
extern int Operation_ChaiDai_Desk(void *p);


extern int Operation_WenTi_Desk(void *p);
extern int Operation_LiuCang_Desk(void *p);
extern int Operation_DaiShou_Desk(void *p);
/*------------------------------------------------------------------------------*/
//寄件录单
extern int LuDan_FillData(void *p);

/*收件操作*/
extern int ShouJian_FillData(void *p);
extern int ShouJianWT_FillData(void *p);
extern int ShouJianWtFee_FillData(void *p);


/*发件操作*/
extern int FaJian_FillData(void *p);
extern int FaJianWT_FillData(void *p);
extern int FaJianQR_FillData(void *p);
extern int FaJianZZ_FillData(void *p);

/*到件操作*/
extern int DaoJian_FillData(void *p);
extern int DaoJianWT_FillData(void *p);
extern int DaoJianWtFee_FillData(void *p);
extern int DaoJianQR_FillData(void *p);

/*派件操作*/
extern int PaiJian_FillData(void *p);
extern int PaiJianWT_FillData(void *p);
extern int PaiJianWtFee_FillData(void *p);

//签收
extern int QianShou_FillData(void *p);

//装袋
extern int ZhuangDai_FillData(void *p);

//拆袋
extern int ChaiDai_FillData(void *p);

extern int WenTi_FillData(void *p);
extern int LiuCang_FillData(void *p);
extern int DaiShou_FillData(void *p);
/*------------------------------------------------------------------------------*/
//寄件录单
extern int LuDan_SearchDisplay(void *p);

/*收件操作*/
extern int ShouJian_SearchDisplay(void *p);
extern int ShouJianWT_SearchDisplay(void *p);
extern int ShouJianWtFee_SearchDisplay(void *p);

/*发件操作*/
extern int FaJian_SearchDisplay(void *p);
extern int FaJianWT_SearchDisplay(void *p);
//extern int FaJianWtFee_SearchDisplay(void *p);
extern int FaJianQR_SearchDisplay(void *p);
extern int FaJianZZ_SearchDisplay(void *p);

/*到件操作*/
extern int DaoJian_SearchDisplay(void *p);
extern int DaoJianWT_SearchDisplay(void *p);
extern int DaoJianWtFee_SearchDisplay(void *p);
extern int DaoJianQR_SearchDisplay(void *p);

/*派件操作*/
extern int PaiJian_SearchDisplay(void *p);
extern int PaiJianWT_SearchDisplay(void *p);
extern int PaiJianWtFee_SearchDisplay(void *p);

//签收
extern int QianShou_SearchDisplay(void *p);

//装袋
extern int ZhuangDai_SearchDisplay(void *p);

//拆袋
extern int ChaiDai_SearchDisplay(void *p);

extern int WenTi_SearchDisplay(void *p);
extern int LiuCang_SearchDisplay(void *p);
extern int DaiShou_SearchDisplay(void *p);


//-------------------------------------易高业务--------------------------------------
//录单操作
extern int Operation_YGLuDan_Desk(void *pDeskFather);
extern int YGLuDan_FillData(void *p);
//收件
extern int Operation_YGShouJian_Desk(void *pDeskFather);
extern int YGShouJian_FillData(void *p);
//退件收件
extern int Operation_YGTuiJian_ShouJian_Desk(void *pDeskFather);
extern int YGTuiJian_ShouJian_FillData(void *p);
//发件
extern int Operation_YGFaJian_Desk(void *pDeskFather);
extern int YGFaJian_FillData(void *p);
//退件发件
extern int Operation_YGTuiJian_FaJian_Desk(void *pDeskFather);
extern int YGTuiJian_FaJian_FillData(void *p);
//回单发件
extern int Operation_YGHuiDan_FaJian_Desk(void *pDeskFather);
extern int YGHuiDan_FaJian_FillData(void *p);
//到件
extern int Operation_YGDaoJian_Desk(void *pDeskFather);
extern int YGDaoJian_FillData(void *p);
//到件称重
extern int Operation_YGDaoJianWT_Desk(void *pDeskFather);
extern int YGDaoJianWT_FillData(void *p);
//退件到件
extern int Operation_YGTuiJian_DaoJian_Desk(void *pDeskFather);
extern int YGTuiJian_DaoJian_FillData(void *p);
//回单到件
extern int Operation_YGHuiDan_DaoJian_Desk(void *pDeskFather);
extern int YGHuiDan_DaoJian_FillData(void *p);
//派件
extern int Operation_YGPaiJian_Desk(void *pDeskFather);
extern int YGPaiJian_FillData(void *p);
//签收
extern int Operation_YGQianShou_Desk(void *pDeskFather);
extern int YGQianShou_FillData(void *p);
//问题件
extern int Operation_YGWenTi_Desk(void *pDeskFather);
extern int YGWenTi_FillData(void *p);
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
权限    -----目前暂时没使用
业务类型  具体定义在Op_Types.h中
业务名称  最多8个字节(即4个汉字)

业务操作函数
业务网络发送函数
业务查询
业务查询结果显示
业务删除
一票多件发送
*/

const tOP_CONFIG gaOp_Config[]=//名称不要超过4个汉字 否则在统计页面显示不全
{
	//权限	类型			名称		操作函数							发送函数				查询				查询结果显示				删除					一票多件发送
	{0,		0,				"业绩统计",	NULL,								NULL,					Rec_Statistics,		NULL,						NULL,					NULL},
	//{0,		HYCO_TYPE_MAX,	"报表统计",	NULL,							NULL,					Rec_Statistics,		NULL,						NULL,					NULL},
	//以上为统计
	
	{0,		HYCO_OP_LD,		"录单",		Operation_YGLuDan_Desk,				YGLuDan_FillData,		Rec_Statistics,		LuDan_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_SJ,		"收件",		Operation_YGShouJian_Desk,			YGShouJian_FillData,	Rec_Statistics,		ShouJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_SJWT,	"收件",		Operation_ShouJianWT_Desk,		ShouJianWT_FillData,	Rec_Statistics,		ShouJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_SJWT,	"退件收件",	Operation_YGTuiJian_ShouJian_Desk,	YGTuiJian_ShouJian_FillData,Rec_Statistics,		ShouJianWtFee_SearchDisplay,Rec_DelErrScanDesk,		NULL},
		
	{0,		HYCO_OP_FJ,		"发件",		Operation_YGFaJian_Desk,			YGFaJian_FillData,		Rec_Statistics,		FaJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_FJQR,	"退件发件",	Operation_YGTuiJian_FaJian_Desk,	YGTuiJian_FaJian_FillData,		Rec_Statistics,		FaJianQR_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_ZZFJ,	"回单发件",	Operation_YGHuiDan_FaJian_Desk,		YGHuiDan_FaJian_FillData,		Rec_Statistics,		FaJianZZ_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	//{0,		HYCO_OP_FJWT,	"发件称重",	Operation_FaJianWT_Desk,		FaJianWT_FillData,		Rec_Statistics,		FaJianWT_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_DJ,		"到件",		Operation_YGDaoJian_Desk,			YGDaoJian_FillData,		Rec_Statistics,		DaoJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_DJQR,	"到件称重",	Operation_YGDaoJianWT_Desk,			YGDaoJianWT_FillData,		Rec_Statistics,		DaoJianQR_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_DJWT,	"到件称重",	Operation_DaoJianWT_Desk,		DaoJianWT_FillData,		Rec_Statistics,		DaoJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_DJWT,	"退件到件",	Operation_YGTuiJian_DaoJian_Desk,	YGTuiJian_DaoJian_FillData,	Rec_Statistics,		DaoJianWtFee_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_PJWT,	"回单到件",	Operation_YGHuiDan_DaoJian_Desk,	YGHuiDan_DaoJian_FillData,		Rec_Statistics,		PaiJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_PJ,		"派件",		Operation_YGPaiJian_Desk,			YGPaiJian_FillData,		Rec_Statistics,		PaiJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_QS,		"签收",		Operation_YGQianShou_Desk,			YGQianShou_FillData,		Rec_Statistics,		QianShou_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	//{0,		HYCO_OP_ZD,		"装袋",		Operation_ZhuangDai_Desk,		ZhuangDai_FillData,		Rec_Statistics,		ZhuangDai_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_CD,		"拆袋",		Operation_ChaiDai_Desk,			ChaiDai_FillData,		Rec_Statistics,		ChaiDai_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	{0,		HYCO_OP_WENTI,	"问题件",	Operation_YGWenTi_Desk,				YGWenTi_FillData,			Rec_Statistics,		WenTi_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_LIUCANG,"留仓件",	Operation_LiuCang_Desk,			LiuCang_FillData,		Rec_Statistics,		LiuCang_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	
	//{0,		HYCO_OP_DSHK,	"代收付款",	Operation_DaiShou_Desk,			DaiShou_FillData,		Rec_Statistics,		DaiShou_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

};

#define OP_CFG_TOTAL	(sizeof(gaOp_Config)/sizeof(tOP_CONFIG))

//////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------
* 函数:Op_Config_GetCntByRight
* 功能:查询指定权限的业务数量
* 参数:
* 返回:<=0失败
--------------------------------------------------------*/
int Op_Config_GetCntByRight(U16 right)
{
	int	total=0,i;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right)
		{
			total++;
		}
	}
	
	return total;
}

/*-------------------------------------------------------
* 函数:Op_Config_GetOpTypeByRight
* 功能:查询指定权限指定index的业务类型
* 参数:index:从0开始
* 返回:-1:错误
--------------------------------------------------------*/
int Op_Config_GetOpTypeByRight(U16 right, U16 index)
{
	int	cnt=0,i;
	int opType = -1;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right)
		{
			if(cnt == index)
			{
				opType = gaOp_Config[i].opType;
				break;
			}
			cnt++;
		}
	}
	
	return opType;
}

/*-------------------------------------------------------
* 函数:Op_Config_GetIndexByType
* 功能:根据指定业务类型,找到在gaOp_Config中的索引
* 参数:opType:业务类型
* 返回:-1:错误
--------------------------------------------------------*/
int Op_Config_GetIndexByType(U16 opType)
{
	int i,index = -1;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].opType == opType)
		{
			index = i;
			break;
		}
	}
	
	return index;
}

/*-------------------------------------------------------
* 函数:Op_Config_CallBack
* 功能:根据业务类型,选择执行配置好的功能函数
* 参数:
* 返回:0:成功    其他:失败
--------------------------------------------------------*/
int Op_Config_CallBack(tOP_CALLBACK *pOpCb)
{
	int		i,ret = 0;
	U8		right,funType;
	U16		opType;
	
	right   = pOpCb->right;
	funType = pOpCb->funType;
	opType  = pOpCb->opType;
	
	//找对应接口
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right && gaOp_Config[i].opType == opType)//权限和业务类型相同
		{
			//找对应接口
			switch(funType)
			{
			case 0://操作接口
				if(gaOp_Config[i].opFun != NULL)
				{
					ret = gaOp_Config[i].opFun(pOpCb->uOpcfg.p);
				}
				break;
			case 1://发送接口
				if(gaOp_Config[i].sendFun != NULL)
				{
				hyUsbPrintf("发送接口  i = %d  type = %d \r\n",i,opType);
					ret = gaOp_Config[i].sendFun((void *)&(pOpCb->uOpcfg.tSend));
				}
				break;
			case 2://查询接口
				if(gaOp_Config[i].searchFun != NULL)
				{
					ret = gaOp_Config[i].searchFun(pOpCb->uOpcfg.p);
				}
				break;
			case 3://查询显示接口
				if(gaOp_Config[i].dispFun != NULL)
				{
				hyUsbPrintf("i = %d  type = %d \r\n",i,opType);
					ret = gaOp_Config[i].dispFun((void *)&(pOpCb->uOpcfg.tDisp));
				}
				break;
			case 4://删除接口
				if(gaOp_Config[i].delFun != NULL)
				{
					ret = gaOp_Config[i].delFun(pOpCb->uOpcfg.p);
				}
				break;
			case 5://一票多件发送
				if(gaOp_Config[i].sendFunMore != NULL)
				{
					ret = gaOp_Config[i].sendFunMore((void *)&(pOpCb->uOpcfg.tSend));
				}
				break;
			default:
				break;
			}
			
			break;
		}
	}
	
	return ret;
}