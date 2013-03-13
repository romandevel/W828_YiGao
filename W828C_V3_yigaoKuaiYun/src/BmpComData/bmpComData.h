#ifndef __BMP_COM_DATA_H__
#define __BMP_COM_DATA_H__

extern const U8 gBmpSelect[] ;
extern const U8 gBmpUnselect[];
extern const U8 gBmpCircleData[];

typedef struct _tBMP_ATTR
{
   unsigned char *pBmp;
   unsigned short hig;
   unsigned short wid;
   //unsigned short selColor;
   //unsigned short unSelColor; 
}tBMP_ATTR;

typedef enum _eBMP_DATA_ID
{
     BMP_ID_CIRCLE,
     BMP_ID_UNREAD,
     BMP_ID_SENDED_FAIL,     
     BMP_ID_SENDED,
     BMP_ID_WASTE, 
     BMP_ID_READ,
     
     BMP_ID_DOWNARROW, //输入法中用到的向下箭头
     BMP_ID_MAX    
}eBMP_DATA_ID;

//公用圆形选择图标
#define COLOR_CIRBMP_UNSELECT  (0x9cd3)
#define COLOR_CIRBMP_SELECT    (0xFFE0)

//未选中时颜色为0x320 (绿色)，此时文字的颜色为黑色
//选中时带滑动条的为白色，此时文字颜色为白色
//选中时无滑动条的为0x339 (蓝色)，此时文字的颜色相同
#define  COLOR_BMP_UNSELECT   (0x320)
#define  COLOR_BMP_SELECT     (0x339)
#define  COLOR_BMP_BARSELECT  (0xFFFF)	   

/*---------------------------------------------------------------------------
* 函数: bmpcom_get_attr_data
* 说明: 获取公共bmp图片的属性，不是公共的图片不要加入其中的数组中
* 参数: 
* 返回:	
*-----------------------------------------------------------------------------*/
void bmpcom_get_attr_data(tBMP_ATTR **ppBmpAttr, eBMP_DATA_ID  id);

#endif