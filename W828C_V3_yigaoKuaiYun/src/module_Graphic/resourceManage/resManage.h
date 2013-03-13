#ifndef __RES_MANAGE_H__
#define __RES_MANAGE_H__

#define RES_TYPE_ZLIB     (0)
#define RES_TYPE_JPG      (1)
#define RES_TYPE_BITMAP   (2)

//
typedef struct _tRES_PARA
{
    unsigned int     id; //资源编号
    unsigned int     type; //资源定义的类型 , 用于解压或直接提取
    unsigned short 	 high;  //资源高
    unsigned short   width; //资源宽
    unsigned int     size;  //读取压缩图片的大小
    unsigned int     offset; //读取图片的起始位置
    unsigned int     crc; //校验数据
}tRES_PARA;






#endif