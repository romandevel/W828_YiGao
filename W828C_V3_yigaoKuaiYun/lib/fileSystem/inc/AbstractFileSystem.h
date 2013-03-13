#ifndef ABSTRACT_FILE_SYSTEM_H
#define ABSTRACT_FILE_SYSTEM_H

/*************************************************************************
 * INCLUDES
 *************************************************************************/

/*************************************************************************
 * DEFINES
 *************************************************************************/

#define AFS_READ    0
#define AFS_WRITE   1
#define AFS_RDWR    2

#define AFS_SEEK_OFFSET_FROM_START      0
#define AFS_SEEK_OFFSET_FROM_CURRENT    1
#define AFS_SEEK_OFFSET_FROM_END        2

#define AFS_NAME_SIZE   8
#define AFS_EXT_SIZE    3

#define AFS_DIRECTORY_ENTRY_NEVER_USED  0x00
#define AFS_DIRECTORY_ENTRY_ERASED      0xE5
#define AFS_DIRECTORY_ENTRY_DIRECTORY   0x2E

#define AFS_ATTRIBUTE_LONG_FILE_ENTRY   0x0F
#define AFS_ATTRIBUTE_DIRECTORY         0x10
#define AFS_ATTRIBUTE_SYSTEM            0x02
#define AFS_ATTRIBUTE_HIDDEN            0x04
#define AFS_ATTRIBUTE_VOLUME            0x08

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
#define	NAME_SIZE	8			/*	8.3的短文件名 */
#define	EXT_SIZE	3

#define NAND_BLOOK_POOL_MANAGER_SIZE		(256)//(128)

typedef struct
{
	char			name[NAME_SIZE];	/* filename */
	char 			ext[EXT_SIZE];		/* extension */
	unsigned char 	attribute;			/* offset 11 -- file attribute */
	
	unsigned char	reserved;			/* offset 12 -- 只有短文件时 0x08表示文件名小写，0x10表示扩展名小写 */
	unsigned char	creattime10th;		/* create time, 10ths of a second (0-199 are valid) */
	unsigned short	creattime;			/* offset 14 -- creation time  */
	unsigned short	creatdate;			/* offset 16 -- creation date	*/
	unsigned short	lastaccessdate;		/* offset 18 -- last access date  */
	unsigned short	startcluster_h;		/* high word of first cluster,  (FAT32) */
	unsigned short	time;				/* time created or last updated */
	unsigned short	date;				/* date created or last updated */
	unsigned short	startcluster;		/* starting cluster */
	unsigned int 	file_size;			/* file size */
} DIRECTORY,AFS_DirectoryEntry_st;


typedef enum 
{
    AFS_SINGLE_DELETE_FDM,
    AFS_RECURSIVE_DIRECTORIES_FDM    

} AFS_FileDeleteMode_en;

typedef void (* ProtectUnprotectFn) (void);

typedef struct
{
    ProtectUnprotectFn ProtectFunction;
    ProtectUnprotectFn UnprotectFunction;
} AFS_ProtectUnprotectFunctions_st;
    
extern Int8 *pNandBlockPoolManager;
/*************************************************************************
 * FUNCTION DECLARATIONS
 *************************************************************************/

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_IsLock
* 功能:	判断文件系统是否被锁定(是否正在使用)
* 参数:	none
* 返回:	0 --- 文件系统未lock
 		1 --- 文件系统已被lock
*----------------------------------------------------------------------------*/
int AbstractFileSystem_IsLock(void);

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Init
* 功能:	AbstractFileSystem初始化，准备buffer
*			spReadBuffer----AFS读缓存，申请1k，不释放
*			openfileMode----AFS读写模式记录，用于关闭nand写允许，申请90bytes，不释放
*			注：spReadBuffer和openfileMode空间申请后，是不释放的，
*				所以该函数必须在程序整个运行期间只能调用一次
* 参数:	none
* 返回:	HY_OK------初始化成功
*		HY_ERROR---初始化失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Init( void );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Format
* 功能:	格式化指定的磁盘
* 参数:	volume---盘符(如："C","D","E","F")
* 返回:	HY_OK------格式化成功
*		HY_ERROR---格式化失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Format( char *volume );
/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_ValidateBootSector
* 功能:	检查磁盘的BPB信息是否正确
* 参数:	volume		-- 盘符
* 返回:	HY_OK				-- no error
*		HY_ERROR			-- 盘符不存在或不能读，这种情况，不再进行格式化
*		HY_ERR_BAD_FORMAT	-- BPB参数不正确，只有这种情况，才可以进一步格式化
*----------------------------------------------------------------------------*/
int AbstractFileSystem_ValidateBootSector( char * volume );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Create
* 功能:	创建文件，创建成功后，文件已经以读写方式打开
*		如果输入的字符串格式为文件夹，则创建文件夹，且返回错误
* 参数:	fname-----文件名
* 返回:	成功：文件handle
*		失败：HY_ERROR
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Create( char *fname );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_CreateBySize
* 功能:	创建文件
* 参数:	fname-----文件名
*		mode------创建方式：
*					6----根据指定的size，申请连续的cluster
*		needsize--希望申请的文件大小
* 返回:	文件handle
*----------------------------------------------------------------------------*/
int AbstractFileSystem_CreateBySize( char *fname, int mode, unsigned int needsize );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Truncate
* 功能:	按传入的size截断文件
* 参数:	fhandle---文件hanle
*		size------截取后文件的大小
* 返回:	文件handle
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Truncate( int fhandle, unsigned int size );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Open
* 功能:	以指定的方式打开文件
* 参数:	fname---文件名
*		mode----打开方式(如：AFS_READ，AFS_WRITE，AFS_RDWR)
* 返回:	文件handle(>=0：打开正确；<0：打开错误)
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Open( char *fname, int mode );


/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Close
* 功能:	关闭已经打开的文件
* 参数:	fd---文件handle
* 返回:	0----关闭正确
*		-1---关闭错误
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Close( int fd );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Read
* 功能:	读取文件内容
* 参数:	fd-------文件handle
*		buffer---读取到该内存中
*		size-----读取大小，以字节计
* 返回:	实际读取的size
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Read( int fd, char *buffer, int size );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Write
* 功能:	写文件
* 参数:	fd-------文件handle
*		buffer---将该内存的内容写入文件
*		size-----写入大小，以字节计
* 返回:	实际写入的size
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Write( int fd, char * buffer, int size );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Seek
* 功能:	文件读写位置定位
* 参数:	fd-------文件handle
*		offset---偏移
*		sense----定位方式
*			AFS_SEEK_OFFSET_FROM_START-------从文件开始计
*			AFS_SEEK_OFFSET_FROM_CURRENT-----从当前位置计
*			AFS_SEEK_OFFSET_FROM_END---------从文件末尾计
* 返回:	实际定位的位置(相对文件开始)
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Seek( int fd, int offset, int sense );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_DeleteFile
* 功能:	删除文件
* 参数:	fname----文件名
*		mode-----删除方式
*			AFS_SINGLE_DELETE_FDM----删除单个文件
* 返回:	0-操作成功
*		-1 操作失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_DeleteFile( char *fname, AFS_FileDeleteMode_en mode );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_Rename
* 功能:	文件重命名
* 参数:	fname----待操作文件名
*		newName--新文件名
* 返回:	0-操作成功
*		-1 操作失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Rename( char *fname, char *newName );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_MakeDir
* 功能:	创建目录
* 参数:	dname----目录名(如："C:/音乐"，"C:/音乐/")
* 返回:	0-操作成功
*		-1 操作失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_MakeDir( char *dname );

/*-----------------------------------------------------------------------------
* 函数:	AbstractFileSystem_DeleteDir
* 功能:	删除目录
* 参数:	dname----目录名(如："C:/音乐"，"C:/音乐/")
* 返回:	0-操作成功
*		-1 操作失败
*----------------------------------------------------------------------------*/
int AbstractFileSystem_DeleteDir( char * dname );


/*---------------------------------------------------------
* 函数: AbstractFileSystem_VolumeSpace
* 功能: 获得指定盘符的空间状况(总容量和剩余容量)
* 参数: volume---盘符指针(如："C","D","E","F")
*		容量由pTotalSpace和pFreeSpace带回
*		注意：容量存放为64位宽度(考虑以后的容量大于4G情况)
* 返回:	HY_OK	-- no error
*		HY_ERROR-- 盘符不存在或参数错误
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeSpace( char *volume, U64 *pTotalSpace, U64 *pFreeSpace);

/*---------------------------------------------------------
* 函数: AbstractFileSystem_FileSize
* 功能: 获得已打开文件的大小
* 参数: fd------文件handle
* 返回:	fileSize----文件大小
*---------------------------------------------------------*/
int AbstractFileSystem_FileSize(int fd);

/*---------------------------------------------------------
* 函数: AbstractFileSystem_VolumePresent
* 功能: 检查指定的盘符是否存在
* 参数: volume---盘符指针(如："C","D","E","F")
* 返回: HY_OK------盘符存在
*		HY_ERROR---盘符不存在
*---------------------------------------------------------*/
int AbstractFileSystem_VolumePresent(char* volume);

/*---------------------------------------------------------
* 函数: AbstractFileSystem_VolumeFormatted
* 功能: 检查指定的盘符的磁盘格式
* 参数: volume---盘符指针(如："C","D","E","F")
* 返回:	HY_OK				-- no error
*		HY_ERROR			-- 盘符不存在或不能读，这种情况，不再进行格式化
*		HY_ERR_BAD_FORMAT	-- BPB参数不正确，只有这种情况，才可以进一步格式化
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeFormatted(char* volume);

/*---------------------------------------------------------
* 函数: AbstractFileSystem_VolumeMaintenance
* 功能: 维护指定的盘符
* 参数: volume---盘符指针(如："C","D","E","F")
* 返回:	HY_OK	-- no error
*		HY_ERROR-- 盘符不存在或不能读
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeMaintenance(char* volume);

/*---------------------------------------------------------
* 函数: AbstractFileSystem_NameFilter
* 功能: 检查文件名的语法是否有错、是否有不允许的字符。
* 		如果文件名不合法，则path带回修正后的文件名
* 参数: path -- 指向路径字符串，以字符结束；要求该内存必须可改
* 返回: HY_OK------ 文件名ok
* 		HY_ERROR--- 文件名有错误，且path已经被修正了
*---------------------------------------------------------*/
int AbstractFileSystem_NameFilter(char *path);

#endif /* ABSTRACT_FILE_SYSTEM_H */
 
