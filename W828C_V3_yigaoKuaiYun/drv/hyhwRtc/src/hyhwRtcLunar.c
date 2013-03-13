

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyhwRtc.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/
#define REFERENCE_YEAR		1900//程序中的查询表都以1900为基准

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/

/*************************************************************************
 * EXTERNAL
 *************************************************************************/

/*************************************************************************
 * STATICS
 *************************************************************************/
/*------------------------------------------------------------------------
从阴历年1900年到2100年
the total day of each month pointer
from 1901~2100
(0110)110000001001 (0110)leap month,110000001001
lunar month total day: 1:29 0:30
*----------------------------------------------------------------------------*/
static const UInt8 lunar_month_table[]={//total 402 bytes
	0x00,0x04,0xad,0x08,0x5a,0x01,0xd5,0x54,0xb4,0x09,0x64,0x05,0x59,0x45,
	0x95,0x0a,0xa6,0x04,0x55,0x24,0xad,0x08,0x5a,0x62,0xda,0x04,0xb4,0x05,
	0xb4,0x55,0x52,0x0d,0x94,0x0a,0x4a,0x2a,0x56,0x02,0x6d,0x71,0x6d,0x01,
	0xda,0x02,0xd2,0x52,0xa9,0x05,0x49,0x0d,0x2a,0x45,0x2b,0x09,0x56,0x01,
	0xb5,0x20,0x6d,0x01,0x59,0x69,0xd4,0x0a,0xa8,0x05,0xa9,0x56,0xa5,0x04,
	0x2b,0x09,0x9e,0x38,0xb6,0x08,0xec,0x74,0x6c,0x05,0xd4,0x0a,0xe4,0x6a,
	0x52,0x05,0x95,0x0a,0x5a,0x42,0x5b,0x04,0xb6,0x04,0xb4,0x22,0x6a,0x05,
	0x52,0x75,0xc9,0x0a,0x52,0x05,0x35,0x55,0x4d,0x0a,0x5a,0x02,0x5d,0x31,
	0xb5,0x02,0x6a,0x8a,0x68,0x05,0xa9,0x0a,0x8a,0x6a,0x2a,0x05,0x2d,0x09,
	0xaa,0x48,0x5a,0x01,0xb5,0x09,0xb0,0x39,0x64,0x05,0x25,0x75,0x95,0x0a,
	0x96,0x04,0x4d,0x54,0xad,0x04,0xda,0x04,0xd4,0x44,0xb4,0x05,0x54,0x85,
	0x52,0x0d,0x92,0x0a,0x56,0x6a,0x56,0x02,0x6d,0x02,0x6a,0x41,0xda,0x02,
	0xb2,0xa1,0xa9,0x05,0x49,0x0d,0x0a,0x6d,0x2a,0x09,0x56,0x01,0xad,0x50,
	0x6d,0x01,0xd9,0x02,0xd1,0x3a,0xa8,0x05,0x29,0x85,0xa5,0x0c,0x2a,0x09,
	0x96,0x54,0xb6,0x08,0x6c,0x09,0x64,0x45,0xd4,0x0a,0xa4,0x05,0x51,0x25,
	0x95,0x0a,0x2a,0x72,0x5b,0x04,0xb6,0x04,0xac,0x52,0x6a,0x05,0xd2,0x0a, 
	0xa2,0x4a,0x4a,0x05,0x55,0x94,0x2d,0x0a,0x5a,0x02,0x75,0x61,0xb5,0x02, 
	0x6a,0x03,0x61,0x45,0xa9,0x0a,0x4a,0x05,0x25,0x25,0x2d,0x09,0x9a,0x68,
	0xda,0x08,0xb4,0x09,0xa8,0x59,0x54,0x03,0xa5,0x0a,0x91,0x3a,0x96,0x04,
	0xad,0xb0,0xad,0x04,0xda,0x04,0xf4,0x62,0xb4,0x05,0x54,0x0b,0x44,0x5d,
	0x52,0x0a,0x95,0x04,0x55,0x22,0x6d,0x02,0x5a,0x71,0xda,0x02,0xaa,0x05,
	0xb2,0x55,0x49,0x0b,0x4a,0x0a,0x2d,0x39,0x36,0x01,0x6d,0x80,0x6d,0x01,
	0xd9,0x02,0xe9,0x6a,0xa8,0x05,0x29,0x0b,0x9a,0x4c,0xaa,0x08,0xb6,0x08,
	0xb4,0x38,0x6c,0x09,0x54,0x75,0xd4,0x0a,0xa4,0x05,0x45,0x55,0x95,0x0a,
	0x9a,0x04,0x55,0x44,0xb5,0x04,0x6a,0x82,0x6a,0x05,0xd2,0x0a,0x92,0x6a,
	0x4a,0x05,0x55,0x0a,0x2a,0x4a,0x5a,0x02,0xb5,0x02,0xb2,0x31,0x69,0x03,
	0x31,0x73,0xa9,0x0a,0x4a,0x05,0x2d,0x55,0x2d,0x09,0x5a,0x01,0xd5,0x48,
	0xb4,0x09,0x68,0x89,0x54,0x0b,0xa4,0x0a,0xa5,0x6a,0x95,0x04,0xad,0x08,
	0x6a,0x44,0xda,0x04,0x74,0x05,0xb0,0x25,0x54,0x03,
};

//这个数据表表示了每个月第一个节气出现规律对应的阳历年份范围
//以1900为基础年
static const UInt8 calendar_term1_year[12][9] =
{
	{13,49,85,117,149,185,201,250,250}, //month 1
	{13,45,81,117,149,185,201,250,250}, //2
	{13,48,84,112,148,184,200,201,250}, //3
	{13,45,76,108,140,172,200,201,250}, //4
	{13,44,72,104,132,168,200,201,250}, //5
	{5 ,33,68,96 ,124,152,188,200,201}, //6
	{29,57,85,120,148,176,200,201,250}, //7
	{13,48,76,104,132,168,196,200,201}, //8
	{25,60,88,120,148,184,200,201,250}, //9
	{16,44,76,108,144,172,200,201,250}, //10
	{28,60,92,124,160,192,200,201,250}, //11
	{17,53,85,124,156,188,200,201,250}, //12
};

//这个数据表表示了每个月第二个节气出现规律对应的阳历年份范围
//以1900为基础年
static const UInt8 calendar_term2_year[12][8] =
{
	{13,45,81,113,149,185,201},	 //month 1
	{21,57,93,125,161,193,201},      //2
	{21,56,88,120,152,188,200,201},  //3
	{21,49,81,116,144,176,200,201},  //4
	{17,49,77,112,140,168,200,201},  //5
	{28,60,88,116,148,180,200,201},  //6
	{25,53,84,112,144,172,200,201},  //7
	{29,57,89,120,148,180,200,201},  //8
	{17,45,73,108,140,168,200,201},  //9
	{28,60,92,124,160,192,200,201},  //10
	{16,44,80,112,148,180,200,201},  //11
	{17,53,88,120,156,188,200,201},  //12
};

//这个数据表表示了每个月第一个节气出现的规律
static const UInt8 calendar_term1_table[12][33] =
{
	{7,6,6,6,6,6,6,6,6,5,6,6,6,5,5,6,6,5,5,5,5,5,5,5,5,4,5,5},
	{5,4,5,5,5,4,4,5,5,4,4,4,4,4,4,4,4,3,4,4,4,3,3,4,4,3,3,3},
	{6,6,6,7,6,6,6,6,5,6,6,6,5,5,6,6,5,5,5,6,5,5,5,5,4,5,5,5,5},
	{5,5,6,6,5,5,5,6,5,5,5,5,4,5,5,5,4,4,5,5,4,4,4,5,4,4,4,4,5},
	{6,6,6,7,6,6,6,6,5,6,6,6,5,5,6,6,5,5,5,6,5,5,5,5,4,5,5,5,5},
	{6,6,7,7,6,6,6,7,6,6,6,6,5,6,6,6,5,5,6,6,5,5,5,6,5,5,5,5,4,5,5,5,5},
	{7,8,8,8,7,7,8,8,7,7,7,8,7,7,7,7,6,7,7,7,6,6,7,7,6,6,6,7,7},
	{8,8,8,9,8,8,8,8,7,8,8,8,7,7,8,8,7,7,7,8,7,7,7,7,6,7,7,7,6,6,7,7,7},
	{8,8,8,9,8,8,8,8,7,8,8,8,7,7,8,8,7,7,7,8,7,7,7,7,6,7,7,7,7},
	{9,9,9,9,8,9,9,9,8,8,9,9,8,8,8,9,8,8,8,8,7,8,8,8,7,7,8,8,8},
	{8,8,8,8,7,8,8,8,7,7,8,8,7,7,7,8,7,7,7,7,6,7,7,7,6,6,7,7,7},
	{7,8,8,8,7,7,8,8,7,7,7,8,7,7,7,7,6,7,7,7,6,6,7,7,6,6,6,7,7},
};

//这个数据表表示了每个月第二个节气出现的规律
static const UInt8 calendar_term2_table[12][29] =
{
	{21,21,21,21,21,20,21,21,21,20,20,21,21,20,20,20,20,20,20,20,20,19,20,20,20,19,19,20},
	{20,19,19,20,20,19,19,19,19,19,19,19,19,18,19,19,19,18,18,19,19,18,18,18,18,18,18,18},
	{21,21,21,22,21,21,21,21,20,21,21,21,20,20,21,21,20,20,20,21,20,20,20,20,19,20,20,20,20},
	{20,21,21,21,20,20,21,21,20,20,20,21,20,20,20,20,19,20,20,20,19,19,20,20,19,19,19,20,20},
	{21,22,22,22,21,21,22,22,21,21,21,22,21,21,21,21,20,21,21,21,20,20,21,21,20,20,20,21,21},
	{22,22,22,22,21,22,22,22,21,21,22,22,21,21,21,22,21,21,21,21,20,21,21,21,20,20,21,21,21},
	{23,23,24,24,23,23,23,24,23,23,23,23,22,23,23,23,22,22,23,23,22,22,22,23,22,22,22,22,23},
	{23,24,24,24,23,23,24,24,23,23,23,24,23,23,23,23,22,23,23,23,22,22,23,23,22,22,22,23,23},
	{23,24,24,24,23,23,24,24,23,23,23,24,23,23,23,23,22,23,23,23,22,22,23,23,22,22,22,23,23},
	{24,24,24,24,23,24,24,24,23,23,24,24,23,23,23,24,23,23,23,23,22,23,23,23,22,22,23,23,23},
	{23,23,23,23,22,23,23,23,22,22,23,23,22,22,22,23,22,22,22,22,21,22,22,22,21,21,22,22,22},
	{22,22,23,23,22,22,22,23,22,22,22,22,21,22,22,22,21,21,22,22,21,21,21,22,21,21,21,21,22},
};

// static functions
static UInt32 hyhwRtc_GetSolarTotalDays( pRtcTime_st pSolarDate );
static UInt32 hyhwRtc_GetLunarMonthTotalDays(UInt32 lunarYear, UInt32 lunarMonth);


/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetSolarTotalDays
* 功能: 获得阳历初始日期到指定日期的天数
* 参数:	solarDate	--阳历日期
* 返回:	天数
*----------------------------------------------------------------------------*/
static UInt32 hyhwRtc_GetSolarTotalDays( pRtcTime_st pSolarDate )
{
	UInt32 seconds;
	UInt32 totalDays;
	UInt8  start_day_of_week;
	
	seconds = hyhwRtc_ConvertToSeconds(pSolarDate);
	
	//得到总的天数
	totalDays = seconds/86400;//60*60*24=86400
	start_day_of_week = (UInt8)(CALENDAR_START_YEAR_FIRST_DATE + (U32)(totalDays - 1) % 7);
	pSolarDate->week = (UInt8)(start_day_of_week % 7);
	//区分2000年前后
	if (pSolarDate->year < 2000)
	{
		//1900年到1950年有17896天
		totalDays += 17896;
	}
	else
	{
		//1950年到2000年有18263天
		totalDays -= 18263;
	}
	return totalDays;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetLunarMonthTotalDays
* 功能: 获得阴历指定年月的天数
* 参数:	lunarYear	--阴历年
*		lunarMonth	-- 阴历月
* 返回:	阴历天数
*----------------------------------------------------------------------------*/
static UInt32 hyhwRtc_GetLunarMonthTotalDays(UInt32 lunarYear, UInt32 lunarMonth)
{
	UInt8 tempData1;
	UInt8 tempData2;
	
	if(lunarMonth < 9)
	{
		tempData1 = lunar_month_table[2 * lunarYear];
		tempData2 = (UInt8)(lunarMonth - 1);
	}
	else
	{
		tempData1 = lunar_month_table[2 * lunarYear + 1];
		tempData2 = (UInt8)(lunarMonth - 9);
	}
	
	if((tempData1 >> tempData2) & 0x01)
		return (UInt32)29;
	else
		return (UInt32)30;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetLunarDate
* 功能:	获取阴历日期
* 参数:	pSolarDate	--输入阳历日期
*		pLunarDate  --输出阴历日期
* 返回:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_GetLunarDate( pRtcTime_st pSolarDate ,pRtcLunarDate_st pLunarDate)
{
	UInt32 temp_leap_month;
	UInt32 temp_total_day;
	UInt8  temp_flag;
	UInt8  calculate_temp;
	UInt8  mc_tpumenus_temp_loop;
	UInt8  mc_tpumenus_temp_01;
	UInt16 solarYear;
	UInt16 lunarYear;
	
	temp_leap_month = 0;
	temp_flag = 1;
	temp_total_day = hyhwRtc_GetSolarTotalDays(pSolarDate);
	//条件初始化二次，减少运算数据量.
	solarYear = pSolarDate->year - REFERENCE_YEAR;
	if(solarYear > 99)
	{
		lunarYear = 99;
		pLunarDate->month = 11;
		pLunarDate->day = 25;
		temp_total_day += 25;
		calculate_temp = 100;
	}
	else if (solarYear > 49)
	{
		lunarYear = 0;
		pLunarDate->month = 11;
		pLunarDate->day = 11;
		temp_total_day += 11;
		calculate_temp = 1;
	}
	
	if((solarYear > calculate_temp) || (pSolarDate->month > 1) || (pSolarDate->day > 1))
	{
		for(mc_tpumenus_temp_loop = 1; mc_tpumenus_temp_loop > 0;)
		{
			UInt8 tempDays;
			
			tempDays = (UInt8)hyhwRtc_GetLunarMonthTotalDays(lunarYear, pLunarDate->month);
			if (temp_total_day > tempDays)
			{
				temp_total_day -= tempDays;
				temp_leap_month = lunar_month_table[2 * lunarYear + 1];
				temp_leap_month = (temp_leap_month >> 4) & 0x0F;
			}
			else
			{
				temp_leap_month = pLunarDate->month;
			}

			if(pLunarDate->month == temp_leap_month)
			{
				switch(lunarYear)
				{
					case	6:
					case	14:
					case	19:
					case	25:
					case	33:
					case	36:
					case	38:
					case	41:
					case	44:
					case	52:
					case	55:
					case	79:
					case	99:
					case	117:
					case	136:
					case	147:
					case	150:
					case	155:
					case	158:
					case	185:
					case	193:
								if(temp_total_day < 31)
								{
									pLunarDate->day = (UInt8)temp_total_day;
									mc_tpumenus_temp_loop = 0;
									temp_flag = 0;
								}
								else
									temp_total_day -= 30;
								break; //current month:temp_leap_month
					default:
								if(temp_total_day < 30)
								{
									pLunarDate->day = (UInt8)temp_total_day;
									mc_tpumenus_temp_loop = 0;
									temp_flag = 0; //current month:temp_leap_month
								}
								else
									temp_total_day -= 29;
								break;
				}
			}
			if(temp_flag)
			{
				pLunarDate->month++;

				if(pLunarDate->month == 13)
				{
					pLunarDate->month = 1;
					lunarYear++;
				}
				if(temp_total_day < 61) //if temp_total_day>60,ignore compare
				{
					mc_tpumenus_temp_01 = (UInt8)hyhwRtc_GetLunarMonthTotalDays(lunarYear, pLunarDate->month);
					
					if(temp_total_day < (mc_tpumenus_temp_01 + 1))
					{
						mc_tpumenus_temp_loop = 0;
						pLunarDate->day = (UInt8)temp_total_day;
					}
				}
			}
		}
	}
	
	pLunarDate->chinese_eraT = (UInt8)((lunarYear-4)%10);//天干
	pLunarDate->chinese_eraD = (UInt8)(lunarYear%12);//地支
	
	return;
}

/*-----------------------------------------------------------------------------
* 函数:	hyhwRtc_GetCalendar_Term
* 功能:	获取节气
* 参数:	pSolarDate	--阳历日期
* 返回:	当前日期对应节气的index
*		非节气日HY_ERROR
*----------------------------------------------------------------------------*/
Int32 hyhwRtc_GetCalendar_Term( pRtcTime_st pSolarDate )
{
	UInt32 done_index;
	UInt8  solar_term;
	UInt16 solarYear;
	
	done_index = 0;
	solarYear = (UInt16)(pSolarDate->year - REFERENCE_YEAR);
	//第一个节气
	while (solarYear >= calendar_term1_year[pSolarDate->month - 1][done_index])
	{
		done_index++;
	}
	solar_term = calendar_term1_table[pSolarDate->month - 1][4*done_index + solarYear%4];
	
	if((solarYear == 121) && (pSolarDate->month == 4))
		solar_term = 5;
	if((solarYear == 132) && (pSolarDate->month == 4))
		solar_term = 5;
	if((solarYear == 194) && (pSolarDate->month == 6))
		solar_term = 6;
	
	if(pSolarDate->day == solar_term)
	{
		return (pSolarDate->month-1)*2;
	}
	else
	{
		//第二个节气
		while (solarYear >= calendar_term2_year[pSolarDate->month - 1][done_index])
		{
			done_index++;
		}
		solar_term = calendar_term2_table[pSolarDate->month - 1][4*done_index + solarYear%4];
		
		if((solarYear == 171) && (pSolarDate->month == 3))
			solar_term = 21;
		if((solarYear == 181) && (pSolarDate->month == 5))
			solar_term = 21;
		
		if(pSolarDate->day == solar_term)
		{
			return (pSolarDate->month-1)*2+1;
		}
		else
		{
			return HY_ERROR;
		}
	}
}
