#ifndef _MINIDATABASE_H
#define _MINIDATABASE_H

#ifdef __cplusplus
extern "C"
{
#endif
//////////////////////////////////////////////////////////

#include "hyTypes.h"

#ifdef PC_PLATFORM
	#define HYDB_READ_MODE					"rb"
	#define HYDB_WRITE_MODE					"wb"
	#define HYDB_RDWR_MODE					"rb+"

	#define HYDB_SEEK_BEG					SEEK_SET
	#define HYDB_SEEK_CUR					SEEK_CUR
	#define HYDB_SEEK_END					SEEK_END

	#define HYDB_ZERO						'\0'
	
	#define HYDB_FILE						FILE*
	
	#define HYDB_CREATE(pPath)				fopen(pPath,"wb+")
	#define HYDB_OPEN(fname,mode)			fopen(fname,mode)
	#define HYDB_CLOSE(fp)					fclose(fp)
	#define HYDB_READ(fp,buf,size,count)	fread(buf,size,count,fp)
	#define HYDB_WRITE(fp,buf,size,count)	fwrite(buf,size,count,fp)
	#define HYDB_SEEK(fp,offset,mode)		fseek(fp,offset,mode)
	#define HYDB_REWIND(fp)					rewind(fp)
	#define HYDB_DELETE(fname,mode)			remove(fname)
	#define HYDB_SIZE(fp)					fseek(fp,0,HYDB_SEEK_END);ftell(fp)
#else
#include "AbstractFileSystem.h"

	#define HYDB_READ_MODE					0
	#define HYDB_WRITE_MODE					1
	#define HYDB_RDWR_MODE					2

	#define HYDB_SEEK_BEG					0
	#define HYDB_SEEK_CUR					1
	#define HYDB_SEEK_END					2

	#define HYDB_ZERO						0

	#define HYDB_FILE						int

	#define HYDB_CREATE(pPath)				AbstractFileSystem_Create(pPath)
	#define HYDB_OPEN(fname,mode)			AbstractFileSystem_Open(fname,mode)
	#define HYDB_CLOSE(fp)					AbstractFileSystem_Close(fp)
	#define HYDB_READ(fp,buf,size,count)	AbstractFileSystem_Read(fp,buf,size*count)
	#define HYDB_WRITE(fp,buf,size,count)	AbstractFileSystem_Write(fp,buf,size*count)
	#define HYDB_SEEK(fp,offset,mode)		AbstractFileSystem_Seek(fp,offset,mode)
	#define HYDB_REWIND(fp)					AbstractFileSystem_Seek(fp,0,HYDB_SEEK_BEG)
	#define HYDB_DELETE(fname,mode)			AbstractFileSystem_DeleteFile(fname,mode)
	#define HYDB_SIZE(fp)					AbstractFileSystem_FileSize(fp)
#endif

#define HYDB_RECORD_MAX						10000	//目前暂定最大记录数

#define MAKEDWORD(l, h)      ((U32)(((U16)(l)) | ((U32)((U16)(h))) << 16))

#define HYDB_KEY_MAXLEN			52	//key最大长度

#define HYDB_GET_KEYONLY		1	//只获取key信息
#define HYDB_GET_KEYVALUE		2	//获取key/value信息
#define HYDB_GET_DATAONLY		3	//只获取value信息


#define	HYDB_GET_CUR			4	//获取当前key记录
#define HYDB_GET_ALL			5	//获取全部记录 此时无需提供key信息
#define	HYDB_GET_INDEX			6	//根据索引号获取记录(排序后的索引)

#define HYDB_DEL_CUR			7	//删除当前记录
#define HYDB_DEL_ALL			8	//删除所有记录

#define HYDB_NOTOVERWRITE		10	//插入记录时不覆盖相同key的记录(返回记录已存在)
#define HYDB_OVERWRITE			11	//覆盖原来记录

#define HYDB_GET_NEW			12	//获取新增记录
#define HYDB_GET_ALERT			13	//获取有修改的记录
#define HYDB_GET_NEWANDALERT	14	//获取新增和有修改的记录

#define HYDB_SUCCESS			0	//操作成功
#define HYDB_FILEERROR			-1	//操作数据库文件失败
#define HYDB_DBEXIST			-2	//数据库文件已存在
#define HYDB_KEYEXIST			-3	//记录已存在
#define HYDB_MAXOVERFLOW		-4	//希望管理的最大记录数超出范围
#define HYDB_CREATEFAILED		-5	//创建文件失败
#define HYDB_NODBFILE			-6	//数据库不存在
#define HYDB_MEMORYSMALL		-7	//外部提供buf太小
#define HYDB_KEYNOTEXIST		-8	//记录不存在
#define HYDB_RECORDCOVERED		-9	//记录已覆盖
#define HYDB_NORECORD			-10	//没有记录
#define HYDB_RECORDOVERFLOW		-11	//记录个数超出范围
#define HYDB_NOTFOUND			-12	//没找到相同记录
#define HYDB_MALLOCERROR		-13	//内存申请失败
	
////////////////////////////////////////////////////////////////////////////////////
/************************************
* 数据库头信息 共20字节
************************************/
typedef struct _tdbhead
{
	U32  version;			//版本信息 低16bit主版本号 高16bit副版本号
	U32  record_num;		//RO,当前记录数目, 初值为0
	U32  record_max;		//希望支持的最大记录数   不能超过 HYDB_RECORD_MAX
	U32  record_size;		//每条记录的大小(字节数)
	U32  header_size;		//头信息大小(字节数)

}tDBHEAD;

/**********************************
* 记录以(key/value)对存储,value按字节流存放,解析由外部执行
* value的大小在创建数据库时由外部提供
* 大小:HYDB_KEY_MAXLEN+24
**********************************/
typedef struct _tkeydbt
{
	char	data[HYDB_KEY_MAXLEN];	//key的值,有长度限制

	U32		Lchildoffset;			//RO,左节点的offset 0表示无左节点
	U32		Rchildoffset;			//RO,右节点的offset 0表示无右节点
	U32		Parentoffset;			//RO,父节点的offset 0表示无父节点(根节点)
	U32		Recordoffset;			//RO,与该key对应的value的offset
	U32		Selfoffset;				//RO,当前key的offset
	U32		Keyflag;				//RO,记录该key信息
									//   0:无效key,该区域可重写 1:之前记录 2:修改 4:新增

}tKEYDBT;

///////////////////////////////////////// fun //////////////////////////////////////////
/********************************
* 函数:hyDbDeInit
* 功能:清空变量
* 参数:none
* 返回:none
*********************************/
void hyDbDeInit();

/********************************************
* 函数:hyDbInit
* 功能:初始化数据库,如果数据库文件不存在,则创建数据库,把头信息写入数据库并初始化全局变量
       如果数据库文件存在,直接从数据库中读取数据初始化全局变量(不从参数中取头信息),并把头信息带出
* 参数:pPath:数据库存放路径  
       pHead:包含头信息 
	   pCache:外界提供buf
	   Cachesize:buf大小 注意:必须保证cache大小一次可以读取所有key信息
	   pValue:存放value数据  外部提供
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbInit(char *pPath, tDBHEAD *pHead, char *pCache, U32 Cachesize, char *pValue);

/********************************************
* 函数:hyDbOpen
* 功能:打开数据库 读写方式打开 文件存在的话初始化变量
* 参数:pPath:数据库存放路径
       pHead:用来带回头信息
	   pCache:外界提供buf
	   Cachesize:buf大小
	   pValue:存放value信息 外部提供
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbOpen(char *pPath, tDBHEAD *pHead, char *pCache, U32 Cachesize, char *pValue);

/********************************************
* 函数:hyDbPut
* 功能:增加记录
* 参数:pKey:记录的key
	   pValue:记录的实际数据
	   flag:是否覆盖相同数据 HYDB_NOTOVERWRITE / HYDB_OVERWRITE
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbPut(tKEYDBT *pKey, char *pValue, int flag);

/********************************************
* 函数:hyDbDel
* 功能:删除记录
* 参数:pKey:要删除的记录
       flag:删除标记 HYDB_DEL_CUR / HYDB_DEL_ALL
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbDel(tKEYDBT *pKey, int flag);

/********************************************
* 函数:hyDbGet
* 功能:获取记录
* 参数:pKey:要获取记录的key信息 如果是获取全部信息,该参数无效
       pGetbuf:存放获取到的数据 由外部提供 数据格式由外部解析
	   bufsie:buf大小
	   flag:获取标识 HYDB_GET_CUR / HYDB_GET_ALL / HYDB_GET_INDEX
	   data:获取数据的种类 HYDB_GET_KEYONLY / HYDB_GET_KEYVALUE / HYDB_GET_DATAONLY
	   index:索引号 flag为HYDB_GET_INDEX时有效 (0开始)
* 返回:获取到的记录个数:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbGet(tKEYDBT *pKey, char *pGetbuf, U32 bufsie, int flag, int data, U32 index);

/********************************************
* 函数:hyDbAlert
* 功能:修改记录 如果key信息有变化 (pOldKey与pNewKey 的data不一样),需要重新排序
* 参数:pOldKey:之前关键字信息
       pNewKey:需要修改成的关键字信息
       pValue:新value
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbAlert(tKEYDBT *pOldKey, tKEYDBT *pNewKey, char *pValue);

/********************************************
* 函数:hyDbGetModifyNum
* 功能:获取本次有修改或新增的记录信息条数
* 参数:flag:欲获取的类型 HYDB_GET_NEW / HYDB_GET_ALERT / HYDB_GET_NEWANDALERT
* 返回:条数:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbGetModifyNum(int flag);

/********************************************
* 函数:hyDbGetModify
* 功能:获取本次有修改或新增的记录信息
* 参数:pBuf:存储信息 外部提供
       bufsize:pBuf大小
       flag:欲获取的类型 HYDB_GET_NEW / HYDB_GET_ALERT / HYDB_GET_NEWANDALERT
       num:欲取数量
* 返回:实际获取数量:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbGetModify(char *pBuf, U32 bufsize, int flag, U32 num);

/********************************************
* 函数:hyDbDestroy
* 功能:销毁数据库
* 参数:pPath:数据库存放路径
* 返回:0:成功 <0:失败(返回失败原因)
*********************************************/
int hyDbDestroy();

/********************************************
* 函数:hyDbGetRecordNum
* 功能:获取当前记录数目(调用前务必先调用hyDbInit)
* 参数:none
* 返回:记录数目
*********************************************/
U32 hyDbGetRecordNum();

#ifdef __cplusplus
}
#endif

#endif //_MINIDATABASE_H