#include "Test_MainList.h"



/*======================================================================================================*/
const U8* const opType_Test[]=//测试业务
{
	"状态:",
	
	"操作类型:",
	"扫描地点:",
	"员工编号:",
	"分组:",
	"扫描时间:",
	"单号:",
	"子单号",
	
	"下一站:",
};

/*---------------------------------------------------------------
*函数: Op_Test_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int Op_Test_SearchDisplay(tDATA_INFO *pData, int total, U8 *pDispStr, int dispBufLen)
{
	U8 *str[]={"未传", "已传", "已删", "待传", "可传"};
	int i, j, cnt,stat,len=0;
	tDATA_COMM 	*pComm;
	tTESTOP		*pTestOp;
	
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		pTestOp = &pData[i].uData.testOp;
		
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_Test[0], str[stat],
							opType_Test[1], pComm->opType,
							opType_Test[2], pComm->scanStat,
							opType_Test[3], pComm->userId,
							opType_Test[4], pComm->groupId,
							opType_Test[5], pComm->scanTime,
							opType_Test[6], pComm->code,
							opType_Test[7], pComm->subCode,
							
							opType_Test[8], pTestOp->Station
							);
		
		len += strlen(&pDispStr[len]);
		
		//长度判断
		if(len+1024 >= dispBufLen)
		{
			strcpy(&pDispStr[len], "内存不足,后续信息无法显示.");
			break;
		}
	}
	
	return 0;
}
