/*----------------------文件描述--------------------------------
*创建日期: 08/03/07
*描述    : 开机进程
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "HSA_API.h"

extern short dosTime;
extern short dosDate;

int HSA_SetSystemParameter( HSA_SystemParameter_en parameter, int value )
{
    Int32 volumeDAC;

    switch( parameter )
    {
        case HSA_DOS_DATETIME_SP:
            dosTime = (short) (value & 0xFFFF);
            dosDate = (short) (value >> 16);
            break;
    }


    return( HSA_CALL_NO_ERR );
}


/*-----------------------------------------------------------------------------
* 函数:	Hyc_Set_SysTime
* 功能:	设置系统的时间
* 参数:	pDateString：    年月日        pTimeString：分时秒
        输入格式为注意："2007/01/01"   "12:00:00"
* 返回:	none
*----------------------------------------------------------------------------*/
void Hyc_Set_SysTime(U8* pDateString,U8* pTimeString)
{
	U32 Year,Month,Day,Hour,Minute,Second;
	U32 time_date;

	//获得 年
	//Year=0;
	Year=(U32)((pDateString[0]-0x30)*1000);
	Year=Year+(U32)((pDateString[1]-0x30)*100);
	Year=Year+(U32)((pDateString[2]-0x30)*10);
	Year=Year+(U32)(pDateString[3]-0x30);
	if (Year>1980)//PSA以1980为基数
		Year=Year-1980;
	else
		Year=1980;

	//获得 月
	//Month=0;
	Month=(U32)((pDateString[5]-0x30)*10);
	Month=Month+(U32)(pDateString[6]-0x30);
	if (Month>12||Month==0)
		Month=1;

	//获得 日
	//Day=0;
	Day=(U32)((pDateString[8]-0x30)*10);
	Day=Day+(U32)(pDateString[9]-0x30);
	if (Day>31||Day==0)
		Day=1;

	//获得 小时
	//Hour=0;
	Hour=(U32)((pTimeString[0]-0x30)*10);
	Hour=Hour+(U32)(pTimeString[1]-0x30);
	if (Hour>23)
		Hour=0;

	//获得 分钟
	//Minute=0;
	Minute=(U32)((pTimeString[3]-0x30)*10);
	Minute=Minute+(U32)(pTimeString[4]-0x30);
	if (Minute>59)
		Minute=0;

	//获得 秒
	//Second=0;
	Second=(U32)((pTimeString[6]-0x30)*10);
	Second=Second+(U32)(pTimeString[7]-0x30);
	if (Second>59)
		Second=0;

	time_date = (U32)((U32)(Year << 25) + (U32)(Month << 21) + (U32)(Day << 16)
			+ (U32)(Hour << 11) + (U32)(Minute << 5) + (U32)(Second / 2));

	HSA_SetSystemParameter(HSA_DOS_DATETIME_SP, (int)time_date);

}

/*-------------------------修改记录--------------------------------
*
*-----------------------------------------------------------------*/