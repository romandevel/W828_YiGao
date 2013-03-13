#include "Op_BarRuler.h"


#ifdef VER_CONFIG_HYCO

/*--------------------------------------
*函数：OpCom_CheckBarcode
*功能：根据指定规则检查条码合法性
*参数：pCode:带检查的条码
       codeType:条码种类
*返回：1:成功    -1:失败     0:没有规则
*--------------------------------------*/
int OpCom_CheckBarcode(U8 *pCode, eBARCODE_TYPE codeType)
{
	int ret = 0, len;
	
	len = strlen(pCode);//条码长度
	if(len == 0)
	{
		ret = -1;
	}
	switch(codeType)
	{
	case TYPE_EMPLOYEID:
		/*if(len < 12)//
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_COMPANYID:
		/*if(len < 12)//
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_BARCODE://V或2 开头的12位长度条码
		//if(len == 12 && (pCode[0] == 'V' || pCode[0] == 'v' || pCode[0] == '2'))//
		/*if(len == 9 || len == 10 || len == 12 || hy_memcmp(pCode, "VIP", 3) == 0)
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_PACKET:
		/*if(len < 10 || len > 20)
		{
			ret = -1;
		}
		else
		{
			ret = 1;
		}*/
		break;
	default:
		break;
	}

	return ret;
}

#endif //VER_CONFIG_YUNDA