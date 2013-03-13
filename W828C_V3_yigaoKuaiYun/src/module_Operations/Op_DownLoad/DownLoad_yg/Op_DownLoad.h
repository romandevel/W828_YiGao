#ifndef OP_DOWNLOAD_H
#define OP_DOWNLOAD_H


#include "hyTypes.h"
#include "Op_Common.h"
#include "Op_Config.h"


//网点
#define LOAD_COMPANY_MAX   (hsaSdram_CompanySize()/sizeof(tCOMPANY))
#define COMPANY_FILE_PATH	"C:/data/Company.bin" //保存文件路径
#define LOAD_COMPANY_BAK    "C:/data/bak/Company.bak"
#define COMPANY_VER_LEN		38

//人员
#define LOAD_EMPLOYEE_MAX   (hsaSdram_EmployeSize()/sizeof(tEMPLOYEE))
#define EMPLOYEE_FILE_PATH	"C:/data/Employee.bin" //保存文件路径
#define LOAD_EMPLOYEE_BAK   "C:/data/bak/Employee.bak"
#define EMPLOYEE_VER_LEN	38

//问题原因
#define LOAD_PROBLEM_MAX   (hsaSdram_ProblemSize()/sizeof(tPROBLEM))
#define PROBLEM_FILE_PATH	"C:/data/Problem.bin" //保存文件路径
#define LOAD_PROBLEM_BAK    "C:/data/bak/Problem.bak"

//留仓原因
#define LOAD_LEAVE_MAX   (hsaSdram_LeaveSize()/sizeof(tLEAVE))
#define LEAVE_FILE_PATH	"C:/data/Leave.bin" //保存文件路径
#define LOAD_LEAVE_BAK    "C:/data/bak/Leave.bak"

//运输方式
#define LOAD_CLASS_MAX   (hsaSdram_WeiVolSize()/sizeof(tCLASS))
#define CLASS_FILE_PATH	  "C:/data/Class.bin" //保存文件路径
#define LOAD_CLASS_BAK    "C:/data/bak/Class.bak"


//目的地
#define LOAD_DESTADDR_MAX   (hsaSdram_RegionSize()/sizeof(tDESTADDR))
#define DESTADDR_FILE_PATH	"C:/data/DestAddr.bin" //保存文件路径
#define LOAD_DESTADDR_BAK    "C:/data/bak/DestAddr.bak"

//航空信息
#define LOAD_FLIGHT_MAX   (hsaSdram_FlightSize()/sizeof(tFLIGHT))
#define FLIGHT_FILE_PATH   "C:/data/Flight.bin" //保存文件路径
#define LOAD_FLIGHT_BAK    "C:/data/bak/Flight.bak"

//车辆信息
#define LOAD_CAR_MAX   (hsaSdram_CarSize()/sizeof(tCAR))
#define CAR_FILE_PATH	"C:/data/Car.bin" //保存文件路径
#define LOAD_CAR_BAK    "C:/data/bak/Car.bak"


//异常原因
#define ABNORMITY_FILE_PATH	"C:/data/Abnormity.bin" //保存文件路径

//派送区域
#define AREA_TEMPFILE_PATH	"C:/data/site_temp.zip"
#define AREA_CSVFILE_PATH	"C:/data/site.csv"
#define AREA_FILE_PATH		"C:/data/site2.bin" //保存文件路径
#define AREA_VER_LEN		32

#define LOAD_VER_NULL "2007-01-01 00:00:00"//
//重量体积编码
#define WEIVOL_FILE_PATH	"C:/data/WeiVol.bin" //保存文件路径

typedef int (*DownLoadFun)(void *pView, U8 flag);


typedef struct _download_cfg_t
{
	U8			*pName;	//显示名称
	DownLoadFun	fun;	//执行函数
	
}tDOWNLOAD_CFG;


typedef struct _employee_t
{
	U8	company[COMPANY_ID_LEN];		//所属公司
	U8	userid[EMPLOYEE_ID_LEN];		//用户编号
	U8	username[EMPLOYEE_NAME_LEN];	//用户名
	U8	userpw[EMPLOYEE_PW_LEN];		//用户密码
	U8	userrole[EMPLOYEE_ROLE_LEN];	//用户角色
	//U8	lasttime[20];	//最后更新时间

}tEMPLOYEE;



typedef struct _company_t
{
	U8	companyid[COMPANY_ID_LEN];	//公司编码
	U8	company[COMPANY_NAME_LEN];	//公司名称
	U8	right[COMPANY_RIGHT_LEN];	//

}tCOMPANY;

//问题件原因表
typedef struct _problem_t
{
	U8	code[PROBLEM_CODE_LEN];	//代码
	U8	name[PROBLEM_NAME_LEN];	//问题名称
	
}tPROBLEM;

//问题件原因表
typedef struct _leave_t
{
	U8	code[LEAVE_CODE_LEN];	//代码
	U8	name[LEAVE_NAME_LEN];	//问题名称
	
}tLEAVE;


//运输方式
typedef struct _tClass_
{
	U8	id[CLASS_ID_LEN];
	U8	name[CLASS_NAME_LEN];
	
}tCLASS;


typedef struct _Destination
{
	U8	id[COMPANY_ID_LEN];	//代码
	U8	name[COMPANY_NAME_LEN];	//名称
	
}tDESTADDR;

typedef struct _FLIGHT
{
	U8	id[FLIGHT_ID_LEN];	//代码
	U8	name[FLIGHT_NAME_LEN];	//名称
	
}tFLIGHT;

typedef struct _CAR
{
	U8	id[CAR_ID_LEN];	//代码
	U8	name[CAR_NAME_LEN];	//名称
	
}tCAR;

typedef struct _abnormal_t
{
	U8	code[PROBLEM_CODE_LEN];	//代码
	U8	name[PROBLEM_NAME_LEN];	//名称
	
}tABNORMAL;

//体积重量编码列表
typedef struct _volwei_t
{
	U8  type ;      //类型: 1 体积 2 重量
	U8	id[8];		//编号
	U8	name[100];	//名称
	
}tVOLWEI;

//收派区域
typedef  struct _areainfo_t
{
	U8  *code ;
	U8  *area ;	
	struct _areainfo_t *next ;
	U8  nodenum;
	
}tAREAINFO;


#define AREA_S_DID1			(0x2000)

#define SITE_MEM_MAX		(0x300000) //处理分配的最大缓冲3M

#define SITE_FILE_MAX		(0x280000)          //2.5M

#define SITE_MAX_RECORD		(8000) //最大记录条数,目前是8355

typedef struct csvst
{
	U32 offset[15] ; //字符串相对总buf的offset

}tCSV;

/*文件结构
CSV 解压后转换的文件格式 : name: siteshow.bin
 ____________________
| 32 Byte  ver       |
 --------------------
| 2 Byte CRC         |
 -------------------- 
| n Byte Data        |
 -------------------- 

断点续传的临时保存的文件格式 :name: sitedown.temp
 ____________________
| 32 Byte  ver       |
 --------------------
| 2 Byte CRC         |
 -------------------- 
| 4 Byte total size  |
 --------------------
| n Byte Data        |
 -------------------- 
 
*/

//网点
extern char	ga8CompanyVer[];
extern U32	gu32CompanyBufSize;
extern U32 	gu32CompanyTotal;//下载的总个数
extern tCOMPANY		*gptCompany;

//人员
extern char	ga8EmployeeVer[];
extern U32	gu32EmployeeBufSize;
extern U32 	gu32EmployeeTotal;//下载的员工总个数
extern tEMPLOYEE	*gptEmployee;

//问题件
extern U32	gu32ProblemTotal;
extern U32	gu32ProblemBufSize;
extern tPROBLEM	*gptProblem;

//留仓件
extern U32	gu32LeaveTotal;
extern U32	gu32LeaveBufSize;
extern tLEAVE	*gptLeave;

//运输方式
extern U32	gu32ClassBufSize;
extern U32 	gu32ClassTotal;//下载异常类型总数
extern tCLASS *gptClass;//存放异常类型

//目的地
extern U32	gu32DestAddrBufSize;
extern U32 	gu32DestAddrTotal;//下载目的地总数
extern tDESTADDR *gptDestAddr;//存放目的地

//航空信息
extern U32	gu32FlightBufSize;
extern U32 	gu32FlightTotal;//下载航空信息总数
extern tFLIGHT *gptFlight;//存放航空信息

//车辆信息
extern U32	gu32CarBufSize;
extern U32 	gu32CarTotal;//下载车辆信息总数
extern tCAR *gptCar;//存放车辆信息

//异常原因
extern U32	gu32AbnormalBufSize;
extern U32 	gu32AbnormalTotal;//下载异常类型总数
extern tABNORMAL *gptAbnormal;//存放异常类型



//派送区域
extern U8  gu8AreaVer[];


//重量体积编码
extern U32		gu32WeiVolTotal;
extern U32		gu32WeiVolBufSize;
extern U16		gu16VolIndex;
extern tVOLWEI	*gptWeiVol;




extern U16 *gpu16SearchInfo;
/*=====================================================================*/

#endif	//OP_DOWNLOAD_H