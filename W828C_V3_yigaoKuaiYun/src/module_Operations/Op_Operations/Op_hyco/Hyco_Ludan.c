/*-----------------------------------------------
录单   分4页显示
第一页
快件单号*
目的地*
重量(Kg)*
件数*     扫描子单
支付方式*  (现付,到付....)
代收签单
代收货款
费用
客户


第二页
运输方式
物品类别
物品名称
币别
签收短信通知  (是,  否 )  
寄件人手机号

第三页
       收件人信息
姓名
电话
地址  (省    市   县)
详细地址

第四页
        寄件人信息
姓名
电话
地址   (省    市    县)
详细地址
------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO

//支付方式
const U8 *const gauPayType[]=
{
	"现付","到付","月结"
};

//运输方式
const U8 *const gauTranType[]=
{
	"汽运",	"航空",	"海运"
};

//物品类型
const U8 *const gauGoodsType[]=
{
	"货样","文件"
};

//币种
const U8 *const gauMoneyType[]=
{
	"人民币", "港币", "台币", "美元", "英镑", "欧元", "日元"
};

//短信通知
const U8 *const gauSms[]=
{
	"是", "否"
};


tPANEL			*gpPanel[4];
tGRAPLABEL		*gpLable[5];

U8	gu8Sel_ShengS, gu8Sel_ShiS, gu8Sel_XianS;
U8	gu8Sel_ShengR, gu8Sel_ShiR, gu8Sel_XianR;
/*==========================================发件界面==========================================*/
const tGrapViewAttr LuDan_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//背景
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//完成
	{BUTTON_ID_2,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
#else	
	{BUTTON_ID_2,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//返回
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//完成
#endif
	{BUTTON_ID_3,	     0,0, 0,0,     0,0, 0,0,   0,0,0,0,1,0,0,0},//确定
	
	//页面控件
	{LABEL_ID_1,	   0,250, 50,30,   0,250,49,280,      TY_YS_EN,TY_SEL_YS_EN,0,0,1,0,0,0},//第一页
	{LABEL_ID_2,	   49,250, 50,30,  50,250,99,280,     TY_YS_EN,TY_SEL_YS_EN,0,0,1,0,0,0},//第二页
	{LABEL_ID_3,	   98,250, 50,30,  100,250,149,280,   TY_YS_EN,TY_SEL_YS_EN,0,0,1,0,0,0},//第三页
	{LABEL_ID_4,	   147,250, 50,30, 149,250,200,280,   TY_YS_EN,TY_SEL_YS_EN,0,0,1,0,0,0},//第四页
	{LABEL_ID_5,	   196,250, 44,30, 0,0,0,0,   		  TY_YS44_EN,TY_YS44_EN,0,0,1,0,0,0},//剩余空隙
	
	//第一页
	{PANEL_ID_1,	0, 30, 240, 215, 	0, 30, 240, 245, 0,0,0,0,1,0,0,0},
	{BUTTON_ID_10,	   5,  33 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//快件单号*
	{BUTTON_ID_10,	   5,  58 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//目的地*
	{BUTTON_ID_10,	   5,  83 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//重量(Kg)*
	{BUTTON_ID_11,	   165,84,70, 20,   165,84,238,104,   0,0,0,0,1,0,0,0},//蓝牙
	{BUTTON_ID_10,	   5, 110 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//件数*
	{BUTTON_ID_12,	   165,105,73, 34,  165,105,238,139,   YD_AN1_EN,YD_AN2_EN,0,0,0,0,0,0},//扫描子单
	{BUTTON_ID_10,	   5, 140 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//支付方式*
	{BUTTON_ID_10,	   5, 165 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//代收签单
	{BUTTON_ID_10,	   5, 190 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//代收货款
	{BUTTON_ID_10,	   140,190,32, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//费用
	{BUTTON_ID_10,	   5, 215 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//客户
	
	{EDIT_ID_1,	   	   78, 33,160, 22,   78,33,238,55,  0,0,0xffff,0,1,1,1,0},//快件单号
	{EDIT_ID_2,	   	   78, 58,83, 22,   78,58,161,80,   0,0,0xffff,0,1,0,1,0},//目的地
	{EDIT_ID_20,	  162, 58,76, 22,  162,58,238,80,   0,0,0xCE59,0,1,0,0,0},//目的地名称
	{EDIT_ID_3,	   	   78, 83,83, 22,   78,83,161,105,  0,0,0xCE59,0,1,0,1,0},//重量
	{EDIT_ID_4,	   	   78, 110,83, 22,  78,110,161,132, 0,0,0xffff,0,1,0,1,0},//件数
	{COMBOBOX_ID_1,	   78, 140,160,22,  78,140,238,162,0,0,0xffff,0,1,0,1,0},//支付方式
	{EDIT_ID_5,	   	   78, 165,160,22,  78,165,238,187,0,0,0xffff,0,1,0,1,0},//代收签单
	{EDIT_ID_6,	   	   78, 190,60, 22,  78,190,138,212,0,0,0xffff,0,1,0,1,0},//代收货款
	{EDIT_ID_7,	   	  178, 190,60, 22, 178,190,238,212,0,0,0xffff,0,1,0,1,0},//费用
	{EDIT_ID_8,	   	   78, 215,160,22,  78,215,238,237,0,0,0xffff,0,1,0,1,0},//客户
	
	
	//第二页
	{PANEL_ID_2,	0, 30, 240, 215, 	0, 30, 240, 245, 0,0,0,0,0,0,0,0},
	{BUTTON_ID_10,	   5,  33 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//运输方式
	{BUTTON_ID_10,	   5,  58 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//物品类别
	{BUTTON_ID_10,	   5,  88 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//物品名称
	{BUTTON_ID_10,	   5, 108 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//币种
	{BUTTON_ID_10,	   5, 133 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//短信通知
	{BUTTON_ID_21,	   5, 158 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//通知号码
	
	{COMBOBOX_ID_11,   78, 33,160,22,   78,33,238,55,  0,0,0xffff,0,1,0,1,0},//运输方式
	{COMBOBOX_ID_12,   78, 58,160,22,   78,58,238,80,  0,0,0xffff,0,1,0,1,0},//物品类别
	{EDIT_ID_11,	   78, 83,160, 22,  78,83,238,105, 0,0,0xffff,0,1,0,1,0},//物品名称
	{COMBOBOX_ID_13,   78, 108,160,22,  78,108,238,130,0,0,0xffff,0,1,0,1,0},//币种
	{COMBOBOX_ID_14,   78, 133,160,22,	78,133,238,155,0,0,0xffff,0,1,0,1,0},//短信通知
	{EDIT_ID_12,	   78, 158,160,22,	78,158,238,180,0,0,0xffff,0,1,0,1,0},//通知号码
	
	//第三页
	{PANEL_ID_3,	0, 30, 240, 215, 	0, 30, 240, 245, 0,0,0,0,0,0,0,0},
	{BUTTON_ID_10,	  10,  30 ,220,22,   0,0,0,0,   0,0,0,0,1,0,0,0},//收件人信息
	{BUTTON_ID_10,	   5,  55 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//姓名
	{BUTTON_ID_10,	   5,  80 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//电话
	{BUTTON_ID_10,	   5, 105 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//地址
	{BUTTON_ID_10,	   5, 180 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//详细地址
	
	{EDIT_ID_21,	   78, 55,160, 22,   78,55,238,77,    0,0,0xffff,0,1,1,1,0},//姓名
	{EDIT_ID_22,	   78, 80,160, 22,   78,80,238,102,   0,0,0xffff,0,1,0,1,0},//电话
	{COMBOBOX_ID_21,   78,105,160, 22,   78,105,238,127,  0,0,0xffff,0,1,0,1,0},//省
	{COMBOBOX_ID_22,   78,130,160, 22,   78,130,238,152,  0,0,0xffff,0,1,0,1,0},//市
	{COMBOBOX_ID_23,   78,155,160, 22,   78,155,238,177,  0,0,0xffff,0,1,0,1,0},//县
	{EDIT_ID_23,	   78,180,150, 64,   78,180,238,244,  0,0,0xffff,0,1,0,1,0},//详细地址
	
	
	//第四页
	{PANEL_ID_4,	0, 30, 240, 215, 	0, 30, 240, 245, 0,0,0,0,0,0,0,0},
	{BUTTON_ID_10,	  10,  30 ,220,22,   0,0,0,0,   0,0,0,0,1,0,0,0},//寄件人信息
	{BUTTON_ID_10,	   5,  55 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//姓名
	{BUTTON_ID_10,	   5,  80 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//电话
	{BUTTON_ID_10,	   5, 105 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//地址
	{BUTTON_ID_10,	   5, 180 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//详细地址
	
	{EDIT_ID_31,	   78, 55,160, 22,   78,55,238,77,    0,0,0xffff,0,1,1,1,0},//姓名
	{EDIT_ID_32,	   78, 80,160, 22,   78,80,238,102,   0,0,0xffff,0,1,0,1,0},//电话
	{COMBOBOX_ID_31,   78,105,160, 22,   78,105,238,127,  0,0,0xffff,0,1,0,1,0},//省
	{COMBOBOX_ID_32,   78,130,160, 22,   78,130,238,152,  0,0,0xffff,0,1,0,1,0},//市
	{COMBOBOX_ID_33,   78,155,160, 22,   78,155,238,177,  0,0,0xffff,0,1,0,1,0},//县
	{EDIT_ID_33,	   78,180,150, 64,   78,180,238,244,  0,0,0xffff,0,1,0,1,0},//详细地址
	
};

const tPANELPRIV LuDan_PanelAttr[]=
{
	{TY_UNITY_BG1_EN,0,NULL},
};

const tGrapLabelPriv LuDan_LableAttr[]=
{
	{"第一页",COLOR_RED,COLOR_YELLOW,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,1,TY_UNITY_BG1_EN},//页面选项卡
	{"第二页",COLOR_RED,COLOR_YELLOW,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,TY_UNITY_BG1_EN},
	{"第三页",COLOR_RED,COLOR_YELLOW,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,TY_UNITY_BG1_EN},
	{"第四页",COLOR_RED,COLOR_YELLOW,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,TY_UNITY_BG1_EN},
	{NULL,	  COLOR_RED,COLOR_YELLOW,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,TY_UNITY_BG1_EN},
};

const tGrapButtonPriv LuDan_BtnPriv[]=
{
#ifdef W818C
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//完成
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
#else
	{"返回",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//返回
	{"完成",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0,  TY_UNITY_BG1_EN,NULL},//完成
#endif
	{NULL,		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  0,NULL},//确定
	
	//第一页  3-13
	{"快件单号*",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//快件单号*
	{"目 的 地*",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//目的地*
	{"重量(Kg)*",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//重量(Kg)*
	{"蓝牙",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//蓝牙
	{"件   数*",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//件数*
	{"扫描子单",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//扫描子单
	{"支付方式*",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//支付方式*
	{"代收签单 ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//代收签单
	{"代收货款 ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//代收货款
	{"费用",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//费用
	{"客    户 ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//客户
	
	//第二页 14-19
	{"运输方式",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//运输方式
	{"物品类别",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//物品类别
	{"物品名称",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//物品名称
	{"币    种",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//币种
	{"短信通知",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//短信通知
	{"通知号码",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//手机号码
	
	//第三页 20-24
	{"收件人信息",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//收件人信息
	{"姓  名",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//姓名
	{"电  话",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//电话
	{"地  址",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//地址
	{"详细地址",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//详细地址
	
	//第四页 25-29
	{"寄件人信息",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//寄件人信息
	{"姓  名",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//姓名
	{"电  话",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//电话
	{"地  址",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//地址
	{"详细地址",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//详细地址
};

const tGrapEditPriv LuDan_EditPriv[] = 
{
	//第一页 0-8
	{NULL,0x0,0,0,2,4,19,  1, BARCODE_LEN-1,	0,0,2,0,0,1,  50,50,0,  0,1,1,1},//快件单号
	{NULL,0x0,0,0,2,4,8,   1, COMPANY_ID_LEN-1, 0,0,2,0,0,1,  50,50,0,  0,1,1,1},//目的地
	{gtOpInfoGlb.station2,0x0,0,0,2,4,9,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0,0},//目的地名称
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//重量
	{NULL,  0x0,0,0,2,4,9,  1, 3,	0,0,1,0,0,1,  50,50,0,  0,0,1, 1},//件数
	
	{NULL,	0x0,0,0,2,4,19,  1, BARCODE_LEN-1,0,0,2,0,0,1,  50,50,0,  0,1,1,1},//代收签单
	{NULL,  0x0,0,0,2,4,6,   1, FEE_LEN-1,	0,0,1,0,0,1,  50,50,0,  0,0,1,1},//代收货款
	{NULL,  0x0,0,0,2,4,6,   1, FEE_LEN-1,	0,0,1,0,0,1,  50,50,0,  0,0,1,1},//费用
	{NULL,	0x0,0,0,2,4,19,  1, CUSTOMER_ID_LEN-1,0,0,0,0,0,1,  50,50,0,  0,1,1,1},//客户
	
	//第二页 9-10
	{NULL,0x0,0,0,2,4,19,  1, GOODS_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  0,1,5,1},//物品名称
	{NULL,0x0,0,0,2,4,19,  1, PHONE_NUM_LEN-1,		0,0,1,0,0,1,  50,50,0,  0,1,1,1},//通知号码
	
	//第三页 11-12
	{NULL,0x0,0,0,2,4,19,  1, CUSTOMER_NAME_LEN-1,	0,0,0,0,0,1,  50,50,0,  0,1,5,1},//姓名
	{NULL,0x0,0,0,2,4,19,  1, PHONE_NUM_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//电话
	
	//第三页 13-14
	{NULL,0x0,0,0,2,4,19,  1, CUSTOMER_NAME_LEN-1,	0,0,0,0,0,1,  50,50,0,  0,1,5,1},//姓名
	{NULL,0x0,0,0,2,4,19,  1, PHONE_NUM_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//电话
};

const tGrapEdit2Priv LuDan_Edit2Priv[]=
{
 	{NULL,	0,0,0,0,0,1,80,60-1,2,2,0,5},//详细地址
};

const tComboboxPriv LuDan_ComAttr[]=
{
	//第一页 0
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//支付方式
	
	//第二页 1-3
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//运输方式
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//物品类别
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//币种
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//短息通知
	
	//第三页 5-7
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//省
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//市
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//县
	
	//第四页 8-10
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//省
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//市
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//县
	
};

int LuDan_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn, *pBtnSub;
	tGRAPEDIT 		*pEditWT, *pEditNum;
	tGRAPEDIT 		*pEdit;
	tGRAPEDIT2 		*pEdit2;
	tGRAPCOMBOBOX	*pCombox;
	tDATA_INFO 		Recd, *pSub;
	int ret,i;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if(gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("控件信息异常!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://完成
			if(strcmp(gpEdit_Code->edit_privattr.digt, SubCode_GetMainCode()) != 0)
			{
				Com_SpcDlgDeskbox("快件单号不一致!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			if(SubCode_GetSubTotal() > 1 && SubCode_GetSubTotal() == SubCode_GetSubScan())
			{
				if(SubCode_GetFinalState() != 0)
				{
					Com_SpcDlgDeskbox("该单已完成,不可操作!",6,pDesk,1,NULL,NULL,100);
					break;
				}
				if(strlen(gtOpInfoGlb.station2) == 0)
				{
					Com_SpcDlgDeskbox("请输入正确的站点!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(gpEdit_Stat2);
					break;
				}
				
				pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
				if(strlen(pEditWT->edit_privattr.digt) == 0)
				{
					Com_SpcDlgDeskbox("请输入正确的重量!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(pEditWT);
					break;
				}
				
				pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
				if(strlen(pEditNum->edit_privattr.digt) == 0 || atoi(pEditNum->edit_privattr.digt) < 1)
				{
					Com_SpcDlgDeskbox("请输入正确的件数!",0,pDesk,1,NULL,NULL,100);
					Grap_ChangeFocusEnable(pEditNum);
					break;
				}
				
				OpCom_FillCommInfo(&Recd, gpEdit_Code->edit_privattr.digt, gpEdit_Code->edit_privattr.digt, RECORD_WAITSEND);
				
				strcpy(Recd.uData.ludan.deststat, gtOpInfoGlb.station2);
				hyUsbPrintf("Recd.uData.ludan.deststat = %s \r\n",Recd.uData.ludan.deststat);
				strcpy(Recd.uData.ludan.obj_wei, gtOpInfoGlb.weight);
				hyUsbPrintf("Recd.uData.ludan.obj_wei = %s \r\n",Recd.uData.ludan.obj_wei);
				strcpy(Recd.uData.ludan.num, pEditNum->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.num = %s \r\n",Recd.uData.ludan.num);
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_1);
				Recd.uData.ludan.payType  = pCombox->comboPriv.cur_index;
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_5);
				strcpy(Recd.uData.ludan.daishouCode, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.daishouCode = %s \r\n",Recd.uData.ludan.daishouCode);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_6);
				strcpy(Recd.uData.ludan.feeDaishou, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.feeDaishou = %s \r\n",Recd.uData.ludan.feeDaishou);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_7);
				strcpy(Recd.uData.ludan.feeTotal, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.feeTotal = %s \r\n",Recd.uData.ludan.feeTotal);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_8);
				strcpy(Recd.uData.ludan.customer_id, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.customer_id = %s \r\n",Recd.uData.ludan.customer_id);
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_11);
				Recd.uData.ludan.tranType  = pCombox->comboPriv.cur_index;
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_12);
				Recd.uData.ludan.goodsType  = pCombox->comboPriv.cur_index;
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_11);
				strcpy(Recd.uData.ludan.goodsName, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.goodsName = %s \r\n",Recd.uData.ludan.goodsName);
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_13);
				Recd.uData.ludan.moneyType  = pCombox->comboPriv.cur_index;
				
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_14);
				Recd.uData.ludan.ifNotice  = pCombox->comboPriv.cur_index;
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_12);
				strcpy(Recd.uData.ludan.noticeNum, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.noticeNum = %s \r\n",Recd.uData.ludan.noticeNum);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_21);
				strcpy(Recd.uData.ludan.recvName, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.recvName = %s \r\n",Recd.uData.ludan.recvName);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_22);
				strcpy(Recd.uData.ludan.recvPhone, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.recvPhone = %s \r\n",Recd.uData.ludan.recvPhone);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_23);
				if(gu8Sel_ShengR == 0 || gu8Sel_ShengR == 1 || gu8Sel_ShengR == 8 || gu8Sel_ShengR == 21)//4个直辖市
				{
					sprintf(Recd.uData.ludan.recvAddr, "%s%s%s", province[gu8Sel_ShengR].area, province[gu8Sel_ShengR].next[gu8Sel_ShiR].area, pEdit->edit_privattr.digt);
				}
				else
				{
					sprintf(Recd.uData.ludan.recvAddr, "%s%s%s%s", province[gu8Sel_ShengR].area, province[gu8Sel_ShengR].next[gu8Sel_ShiR].next[gu8Sel_XianR].area, pEdit->edit_privattr.digt);
				}
				
				hyUsbPrintf("recvAddr = %s \r\n", Recd.uData.ludan.recvAddr);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_31);
				strcpy(Recd.uData.ludan.sendName, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.sendName = %s \r\n",Recd.uData.ludan.sendName);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_32);
				strcpy(Recd.uData.ludan.sendPhone, pEdit->edit_privattr.digt);
				hyUsbPrintf("Recd.uData.ludan.sendPhone = %s \r\n",Recd.uData.ludan.sendPhone);
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_33);
				if(gu8Sel_ShengS == 0 || gu8Sel_ShengS == 1 || gu8Sel_ShengS == 8 || gu8Sel_ShengS == 21)//4个直辖市
				{
					sprintf(Recd.uData.ludan.sendAddr, "%s%s%s", province[gu8Sel_ShengS].area, province[gu8Sel_ShengS].next[gu8Sel_ShiS].area, pEdit->edit_privattr.digt);
				}
				else
				{
					sprintf(Recd.uData.ludan.sendAddr, "%s%s%s%s", province[gu8Sel_ShengS].area, province[gu8Sel_ShengS].next[gu8Sel_ShiS].next[gu8Sel_XianS].area, pEdit->edit_privattr.digt);
				}
				
				hyUsbPrintf("sendAddr = %s \r\n", Recd.uData.ludan.sendAddr);
				
				//保存
				ret = Rec_WriteNewRecode(&Recd, NULL);
				if(ret != 0)
				{
					Com_SpcDlgDeskbox("记录保存失败!",0,pDesk,1,NULL,NULL,150);
					return RETURN_REDRAW;
				}
				
				//改变记录状态为 可传
				RecStore_ChangeMultiCodeState(Login_GetRight(), OpCom_GetCurOpType(), Recd.tComm.code, RECORD_ENABLESEND);
				
				//加载记录
				RecStore_GetUnreportedMulti(Login_GetRight(), 0);//后台发送
				
				Com_SpcDlgDeskbox("保存成功!",0,pDesk,1,NULL,NULL,100);
				return RETURN_QUIT;
			}
			else
			{
				Com_SpcDlgDeskbox("此单还未操作完!",0,pDesk,1,NULL,NULL,100);
			}
			break;
		case BUTTON_ID_3://确定
			if(gpEdit_Stat2->view_attr.curFocus == 1)//目的地
			{
				gpEdit_Stat2->editenter(gpEdit_Stat2);
				break;
			}
			
			if(gpEdit_Code->view_attr.curFocus == 1)
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					break;
				}
				
				//判断单号
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					SubCode_SetMainCode(gpEdit_Code->edit_privattr.digt);
					//加载该主单下面的子单
					ret = RecStore_GetSubCodeByMainCode(Login_GetRight(), OpCom_GetCurOpType(), NULL, 
													pCode, (U8 *)hsaSdram_UpgradeBuf(), hsaSdram_UpgradeSize());
					
					hyUsbPrintf("get subcode cnt = %d \r\n", ret);
					//有加载到子单
					if(ret > 0)
					{
						pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
						sprintf(pEditNum->edit_privattr.digt, "%d", ret);
						pBtnSub = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_12);
						SubCode_SetSubTotal(ret);
						SubCode_SetSubScan(ret);
						pBtnSub->view_attr.enable = 1;
						
						//判断是否已完成的单 (找主单和子单相同的记录),如果已完成,则不可再修改
						pSub = (tDATA_INFO *)hsaSdram_UpgradeBuf();
						for(i = 0; i < ret; i++)
						{
							if(strcmp(pSub->tComm.code,pSub->tComm.subCode) == 0)
							{
								//已经完成的单,总数不可再修改
								pEditNum->edit_privattr.enable = 0;
								pEditNum->view_attr.FocusEnable = 0;
								pEditNum->view_attr.color=0xCE59;
								SubCode_SetFinalState(1);
								break;
							}
							pSub++;
						}
					}
					
					Grap_ChangeFocus(gpEdit_Code,1);
				}
				else
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
				}
			}
			break;
		case BUTTON_ID_2://返回
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("是否退出该操作?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_11://蓝牙配对
			pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
			OpComm_ConnBT(pDesk, pEditWT);
			if(gpEdit_Code->view_attr.curFocus != 1)
			{
				Grap_ChangeFocusEnable(gpEdit_Code);
			}
			break;
		case BUTTON_ID_12://扫描子单
		hyUsbPrintf("maincode = %s \r\n", gpEdit_Code->edit_privattr.digt);
			if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
			{
				Com_SpcDlgDeskbox("请先扫描主单号!",0,pDesk,1,NULL,NULL,100);
				if(gpEdit_Code->view_attr.curFocus != 1)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
				}
				break;
			}
			else
			{
				ret = OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE);
				if(ret == -1)
				{
					Com_SpcDlgDeskbox("运单格式错误!",0,pDesk,1,NULL,NULL,100);
					if(gpEdit_Code->view_attr.curFocus != 1)
					{
						Grap_ChangeFocusEnable(gpEdit_Code);
					}
					break;
				}
			}
			
			pEditNum = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_4);
			ret = atoi(pEditNum->edit_privattr.digt);
			if(ret > 500)
			{
				Com_SpcDlgDeskbox("件数超出范围(最大500件)!",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
				break;
			}
			SubCode_SetSubTotal(ret);
			SubCode_SetMainCode(gpEdit_Code->edit_privattr.digt);
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			Operation_SubCode_Desk(pDesk);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

U32 LuDan_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0,num,scan;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_2)//目的地
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.station2, 0, sizeof(gtOpInfoGlb.station2));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID号一定确认好
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	else if(pEdit->view_attr.id == EDIT_ID_4)//件数
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			pDesk= (tGRAPDESKBOX *)pEdit->pParent;
			pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk,BUTTON_ID_12);
			
			num = atoi(pEdit->edit_privattr.digt);
			/*scan = SubCode_GetSubScan();
			if(num < scan)
			{
				//小于实际已扫描数,提示需要删除子单
				Com_SpcDlgDeskbox("小于已扫描子单数,请先删除子单!",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
				sprintf(pEdit->edit_privattr.digt, "%d", scan);
				ReDraw(pDesk, 0, 0);
				return SUCCESS ;
			}
			
			SubCode_SetSubTotal(num);*/
			if(num > 1)
			{
				pBtn->view_attr.enable = 1;
				if(pBtn) pBtn->draw(pBtn, 0, 0);
			}
			else
			{
				pBtn->view_attr.enable = 0;
				ReDraw(pDesk, 0, 0);
			}
		}
	}
	
	return SUCCESS ;
}

S32 LuDan_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	pCode = pEdit->edit_privattr.digt;

	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ret = Op_DownLoad_Company_Desk(pDesk, 1);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	//这里没有判断ret, 因为Company_GetNameById里面有判断
	Company_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
	pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
	
	if(ret != -1)
	{
		Grap_ChangeFocus(pEdit,1);
	}
	
	return RETURN_REDRAW;
}

S32 LuDan_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	tGRAPCOMBOBOX	*pCombox;
	AREAnode		*pCity;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	switch(pCombox->view_attr.id)
	{
	case COMBOBOX_ID_1://第一页   支付方式
		strcpy(pStr, gauPayType[i]);
		break;
	case COMBOBOX_ID_11://第二页  运输方式
		strcpy(pStr, gauTranType[i]);
		break;
	case COMBOBOX_ID_12://第二页  物品类别
		strcpy(pStr, gauGoodsType[i]);
		break;
	case COMBOBOX_ID_13://第二页  币种
		strcpy(pStr, gauMoneyType[i]);
		break;
	case COMBOBOX_ID_14://第二页  短信通知
		strcpy(pStr, gauSms[i]);
		break;
	case COMBOBOX_ID_21://第三页  省
		strcpy(pStr, province[i].area);
		break;
	case COMBOBOX_ID_22://第三页  市
		pCity = province[gu8Sel_ShengR].next;
		if(pCity)
		{
			strcpy(pStr, pCity[i].area);
		}
		break;
	case COMBOBOX_ID_23://第三页  县
		if(gu8Sel_ShengR == 0 || gu8Sel_ShengR == 1 || gu8Sel_ShengR == 8 || gu8Sel_ShengR == 21)//4个直辖市
		{
			strcpy(pStr, province[gu8Sel_ShengR].next[i].area);
		}
		else
		{
			pCity = province[gu8Sel_ShengR].next;
			if(pCity)
			{
				pCity = pCity[gu8Sel_ShiR].next;
				if(pCity)
				{
					strcpy(pStr, pCity[i].area);
				}
			}
		}
		break;
	case COMBOBOX_ID_31://第四页  省	
		strcpy(pStr, province[i].area);
		break;
	case COMBOBOX_ID_32://第四页  市
		pCity = province[gu8Sel_ShengS].next;
		if(pCity)
		{
			strcpy(pStr, pCity[i].area);
		}
		break;
	case COMBOBOX_ID_33://第四页  县	
		if(gu8Sel_ShengS == 0 || gu8Sel_ShengS == 1 || gu8Sel_ShengS == 8 || gu8Sel_ShengS == 21)//4个直辖市
		{
			strcpy(pStr, province[gu8Sel_ShengS].next[i].area);
		}
		else
		{
			pCity = province[gu8Sel_ShengS].next;
			if(pCity)
			{
				pCity = pCity[gu8Sel_ShiS].next;
				if(pCity)
				{
					strcpy(pStr, pCity[i].area);
				}
			}
		}
		break;
	default:
		break;
	}
	
	
	return SUCCESS;
}

S32 LuDan_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	tGRAPCOMBOBOX	*pComTemp;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->list_opened == 0)
	{
		return SUCCESS;
	}
	
	switch(pCombox->view_attr.id)
	{
	case COMBOBOX_ID_14://第二页  短信通知
		pBtn  = (tGRAPBUTTON *)Grap_GetViewByID(pCombox,BUTTON_ID_21);
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pCombox,EDIT_ID_12);
		if(i == 0)//是
		{
			pBtn->view_attr.enable  = 1;
			pEdit->view_attr.enable = 1;
		}
		else
		{
			pBtn->view_attr.enable  = 0;
			pEdit->view_attr.enable = 0;
		}
		break;
	case COMBOBOX_ID_21://第三页  省
		if(gu8Sel_ShengR == i)
		{
			break;
		}
		gu8Sel_ShengR = i;
		gu8Sel_ShiR   = 0;
		gu8Sel_XianR  = 0;
		pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_22);//市
		if (!pComTemp)  return FAIL;
		if(i == 0 || i == 1 || i == 8 || i == 21)//4个直辖市
		{
			pComTemp->view_attr.enable = 0;
			
			//调整 区/县 字符串
			pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_23);//区/县
			if (!pComTemp)  return FAIL;
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].nodenum;
			pComTemp->comboxinit(pComTemp);
		}
		else
		{
			pComTemp->view_attr.enable = 1;
			
			//调整 市 字符串
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].nodenum;
			pComTemp->comboxinit(pComTemp);
						
			//调整 区/县 字符串
			pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_23);//区/县
			if (!pComTemp)  return FAIL;
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].next[0].nodenum;
			pComTemp->comboxinit(pComTemp);
		}
		break;
	case COMBOBOX_ID_22://第三页  市
		if(gu8Sel_ShiR == i)
		{
			break;
		}
		gu8Sel_ShiR = i;
		gu8Sel_XianR= 0;
		//调整 区/县 字符串
		pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_23);//区/县
		if (!pComTemp)  return FAIL;
		pComTemp->comboPriv.cur_index 		= 0;
		pComTemp->comboPriv.opencur_index 	= 0;
		pComTemp->comboPriv.top_index 		= 0;
		pComTemp->comboPriv.total_num       = province[gu8Sel_ShengR].next[i].nodenum;
		pComTemp->comboxinit(pComTemp);
		break;
	case COMBOBOX_ID_23://第三页  县
		if(gu8Sel_XianR == i)
		{
			break;
		}
		gu8Sel_XianR = i;
		break;
	case COMBOBOX_ID_31://第四页  省
		if(gu8Sel_ShengS == i)
		{
			break;
		}
		gu8Sel_ShengS = i;
		gu8Sel_ShiS   = 0;
		gu8Sel_XianS  = 0;
		pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_32);//市
		if (!pComTemp)  return FAIL;
		if(i == 0 || i == 1 || i == 8 || i == 21)//4个直辖市
		{
			pComTemp->view_attr.enable = 0;
			
			//调整 区/县 字符串
			pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_33);//区/县
			if (!pComTemp)  return FAIL;
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].nodenum;
			pComTemp->comboxinit(pComTemp);
		}
		else
		{
			pComTemp->view_attr.enable = 1;
			
			//调整 市 字符串
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].nodenum;
			pComTemp->comboxinit(pComTemp);
						
			//调整 区/县 字符串
			pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_33);//区/县
			if (!pComTemp)  return FAIL;
			pComTemp->comboPriv.cur_index 		= 0;
			pComTemp->comboPriv.opencur_index 	= 0;
			pComTemp->comboPriv.top_index 		= 0;
			pComTemp->comboPriv.total_num       = province[i].next[0].nodenum;
			pComTemp->comboxinit(pComTemp);
		}
		break;
	case COMBOBOX_ID_32://第四页  市
		if(gu8Sel_ShiS == i)
		{
			break;
		}
		gu8Sel_ShiS = i;
		gu8Sel_XianS= 0;
		//调整 区/县 字符串
		pComTemp = (tGRAPCOMBOBOX *)Grap_GetViewByID(pCombox,COMBOBOX_ID_33);//区/县
		if (!pComTemp)  return FAIL;
		pComTemp->comboPriv.cur_index 		= 0;
		pComTemp->comboPriv.opencur_index 	= 0;
		pComTemp->comboPriv.top_index 		= 0;
		pComTemp->comboPriv.total_num       = province[gu8Sel_ShengS].next[i].nodenum;
		pComTemp->comboxinit(pComTemp);
		break;
	case COMBOBOX_ID_33://第四页  县
		if(gu8Sel_XianS == i)
		{
			break;
		}
		gu8Sel_XianS = i;
		break;
	default:
		break;
	}
	
	Grap_ChangeFocus(pCombox,1);
	
	return SUCCESS;
}

/*-----------------------------------------
*函数: LuDan_PageLablePull
*功能: 
*参数: 
*返回: 
*------------------------------------------*/
S32 LuDan_PageLablePull(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;
  
	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//是起始点

		if(GRAP_INWIDGET_RANGE(x,y,pLabel))
	    {//在有效范围内
	         ret = VIEW_PULL_START; 
	    }
	    else
	    {
	        return FAIL;
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		ret = pLabel->labelEnter(pLabel,x,y);
		ReDraw(pLabel, 0, 0);
	}
	else if (VIEW_PULL_ING == extParam)
	{//拖拽中
	    if(GRAP_INWIDGET_RANGE(x,y,pLabel)) ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pLabel)) 
        {
            return VIEW_PULL_END;
        }    

        
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pLabel, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pLabel->pParent))->pQuitCnt = 1;
		}
		
		pLabel->draw(pLabel, 0, 0);
		
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//超时
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;
}
/*-----------------------------------------
*函数: LuDan_LabelEnter
*功能: 
*参数: 
*返回: 
*------------------------------------------*/
S32 LuDan_LabelEnter(void *pView, U16 viewX, U16 viewY)
{
	tGRAPLABEL		*pLabel;
	tGRAPLABEL		*pLab2;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	tPANEL			*pPanel;
	tGRAPDESKBOX	*pDesk;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*plabel_privattr;
	int i;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);
	
	if(plabel_privattr->focus == 1)//本来就是焦点
	{
		return SUCCESS;
	}
	
	pDesk = (tGRAPDESKBOX *)pLabel->pParent;
	
	for(i = 0; i < 4; i++)
	{
		if(i+LABEL_ID_1 == pview_attr->id)
		{
			gpPanel[i]->view_attr.enable = 1;
			gpLable[i]->label_privattr.focus = 1;
		}
		else
		{
			gpPanel[i]->view_attr.enable = 0;
			gpLable[i]->label_privattr.focus = 0;
		}
	}
	
	if(pview_attr->id == LABEL_ID_1)
	{
		pDesk->editMinId = EDIT_ID_1;
		pDesk->editMaxId = EDIT_ID_8;
		
		pEdit  = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
		pEdit->view_attr.curFocus = 0;
		Grap_ChangeFocusEnable(pEdit);
	}
	else if(pview_attr->id == LABEL_ID_2)
	{
		pDesk->editMinId = EDIT_ID_11;
		pDesk->editMaxId = EDIT_ID_12;
		
		pComBox  = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_11);
		pComBox->view_attr.curFocus = 0;
		Grap_ChangeFocusEnable(pComBox);
	}
	else if(pview_attr->id == LABEL_ID_3)
	{
		pDesk->editMinId = EDIT_ID_21;
		pDesk->editMaxId = EDIT_ID_23;
		
		pEdit  = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_21);
		pEdit->view_attr.curFocus = 0;
		Grap_ChangeFocusEnable(pEdit);
	}
	else if(pview_attr->id == LABEL_ID_4)
	{
		pDesk->editMinId = EDIT_ID_31;
		pDesk->editMaxId = EDIT_ID_33;
		
		pEdit  = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_31);
		pEdit->view_attr.curFocus = 0;
		Grap_ChangeFocusEnable(pEdit);
	}
		
	return SUCCESS;
}

/*-----------------------------------------------
*函数:Operation_LuDan_Desk
*功能:录单
*参数:
*返回:
*-----------------------------------------------*/
int Operation_LuDan_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPEDIT2		*pEdit2;
	tGRAPCOMBOBOX	*pComBox;
	int				i,j,idx=0;
	
	gu8Sel_ShengS = 0;
	gu8Sel_ShiS   = 0;
	gu8Sel_XianS  = 0;
	gu8Sel_ShengR = 0;
	gu8Sel_ShiR   = 0;
	gu8Sel_XianR  = 0;
	
	SubCode_Init();
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&LuDan_ViewAttr[idx++], "寄件录单");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_8;

	for (i=0; i<3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &LuDan_ViewAttr[idx++], &LuDan_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		pBtn->pressEnter = LuDan_BtnPress;
	}

	for(i = 0; i < 5; i++)
	{
		gpLable[i] = Grap_InsertLabel(pDesk,&LuDan_ViewAttr[idx++],&LuDan_LableAttr[i]);
		if (NULL == gpLable[i])	return;
		gpLable[i]->recvPullMsg = LuDan_PageLablePull;
		gpLable[i]->labelEnter 	= LuDan_LabelEnter;
	}

	//插入4个页面及其元素
	for(i = 0; i < 4; i++)
	{
		gpPanel[i] = Grap_InsertPanel(pDesk,&LuDan_ViewAttr[idx++],&LuDan_PanelAttr[0]);
		if (NULL == gpPanel[i])	return;
		
		if(i == 0)//第一页
		{
			for(j = 0; j < 11; j++)//btn
			{
				pBtn = Grap_CreateButton(&LuDan_ViewAttr[idx++],&LuDan_BtnPriv[j+3]);
				if(NULL == pBtn) return;
				if(j == 3 || j == 5)//蓝牙   扫描子单
				{
					pBtn->pressEnter = LuDan_BtnPress;
					if(j == 3)
					{
						pBtn->draw = OpComm_BtnBTDraw;
					}
				}
				gpPanel[i]->panelInsert(gpPanel[i],pBtn);
			}
			
			for(j = 0; j < 5; j++)
			{
				pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[j]);
				pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
				gpPanel[i]->panelInsert(gpPanel[i],pEdit);
				if(j == 0)//单号
				{
					gpEdit_Code = pEdit;
				}
				else if(j == 1)//目的地
				{
					pEdit->editenter  = LuDan_EditEnter;
					pEdit->editKeyEnd = LuDan_EditKeyEnd;
					gpEdit_Stat2 = pEdit;
				}
				else if(j == 3)//重量
				{
					pEdit->handle = OpComm_EditHandle_Price;
					pEdit->editTimer = BT_WeightEditTimer;
					//strcpy(pEdit->edit_privattr.digt, "0.00");
				}
				else if(j == 4)//件数
				{
					pEdit->editKeyEnd = LuDan_EditKeyEnd;
					strcpy(pEdit->edit_privattr.digt, "1");
				}
			}
			
			//支付方式
			pComBox = Grap_CreateCombobox(&LuDan_ViewAttr[idx++], &LuDan_ComAttr[0]);
			if(NULL == pComBox ) return -1;
			gpPanel[i]->panelInsert(gpPanel[i],pComBox);
			pComBox->comboPriv.total_num = sizeof(gauPayType)/sizeof(U8 *);
			pComBox->comboxgetstr = LuDan_ComboboxGetStr;
			pComBox->comboxenter  = LuDan_ComboxEnter;
			
			for(j = 0; j < 4; j++)
			{
				pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[j+5]);
				pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
				gpPanel[i]->panelInsert(gpPanel[i],pEdit);
				if(j == 1 || j == 2)//费用
				{
					pEdit->handle = OpComm_EditHandle_Price;
				}
			}
		}
		else if(i == 1)//第二页
		{
			for(j = 0; j < 6; j++)//btn
			{
				pBtn = Grap_CreateButton(&LuDan_ViewAttr[idx++],&LuDan_BtnPriv[j+14]);
				if(NULL == pBtn) return;
				gpPanel[i]->panelInsert(gpPanel[i],pBtn);
			}
			
			//运输方式  物品类别
			for(j = 0; j < 2; j++)
			{
				pComBox = Grap_CreateCombobox(&LuDan_ViewAttr[idx++], &LuDan_ComAttr[j+1]);
				if(NULL == pComBox ) return -1;
				gpPanel[i]->panelInsert(gpPanel[i],pComBox);
				if(j == 0)//运输方式
				{
					pComBox->comboPriv.total_num = sizeof(gauTranType)/sizeof(U8 *);
				}
				else if(j == 1)//物品类型
				{
					pComBox->comboPriv.total_num = sizeof(gauGoodsType)/sizeof(U8 *);
				}
				pComBox->comboxgetstr = LuDan_ComboboxGetStr;
				pComBox->comboxenter  = LuDan_ComboxEnter;
			}
			//物品名称
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[9]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);
			
			//币种   短信通知
			for(j = 0; j < 2; j++)
			{
				pComBox = Grap_CreateCombobox(&LuDan_ViewAttr[idx++], &LuDan_ComAttr[j+3]);
				if(NULL == pComBox ) return -1;
				gpPanel[i]->panelInsert(gpPanel[i],pComBox);
				if(j == 0)//币种
				{
					pComBox->comboPriv.total_num = sizeof(gauMoneyType)/sizeof(U8 *);
				}
				else if(j == 1)//短信通知
				{
					pComBox->comboPriv.total_num = sizeof(gauSms)/sizeof(U8 *);
				}
				pComBox->comboxgetstr = LuDan_ComboboxGetStr;
				pComBox->comboxenter  = LuDan_ComboxEnter;
			}
			
			//通知号码
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[10]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);
		}
		else if(i == 2)//第三页
		{
			for(j = 0; j < 5; j++)//btn
			{
				pBtn = Grap_CreateButton(&LuDan_ViewAttr[idx++],&LuDan_BtnPriv[j+20]);
				if(NULL == pBtn) return;
				gpPanel[i]->panelInsert(gpPanel[i],pBtn);
			}
			
			//姓名
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[11]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);
			//电话
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[12]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);

			//省  市   县
			for(j = 0; j < 3; j++)
			{
				pComBox = Grap_CreateCombobox(&LuDan_ViewAttr[idx++], &LuDan_ComAttr[j+5]);
				if(NULL == pComBox ) return -1;
				gpPanel[i]->panelInsert(gpPanel[i],pComBox);
				if(j == 0)//省
				{
					pComBox->comboPriv.total_num = 34;
					pComBox->comboPriv.cur_index = gu8Sel_ShengR;
				}
				else if(j == 1)//市
				{
					if(gu8Sel_ShengR == 0 || gu8Sel_ShengR == 1 || gu8Sel_ShengR == 8 || gu8Sel_ShengR == 21)//4个直辖市
					{
						pComBox->view_attr.enable = 0;
					}
					pComBox->comboPriv.total_num = province[gu8Sel_ShengR].nodenum;
					pComBox->comboPriv.cur_index = gu8Sel_ShiR;
				}
				else if(j == 2)//县
				{
					if(gu8Sel_ShengR == 0 || gu8Sel_ShengR == 1 || gu8Sel_ShengR == 8 || gu8Sel_ShengR == 21)//4个直辖市
					{
						pComBox->comboPriv.total_num = province[gu8Sel_ShengR].nodenum;
						pComBox->comboPriv.cur_index = gu8Sel_XianR;
					}
					else
					{
						pComBox->comboPriv.total_num = province[gu8Sel_ShengR].next[gu8Sel_ShiR].nodenum;
						pComBox->comboPriv.cur_index = gu8Sel_XianR;
					}
				}
				pComBox->comboxgetstr = LuDan_ComboboxGetStr;
				pComBox->comboxenter  = LuDan_ComboxEnter;
			}
			
			//详细地址
			pEdit2 = Grap_CreateEdit2(&LuDan_ViewAttr[idx++], &LuDan_Edit2Priv[0], 0);
			gpPanel[i]->panelInsert(gpPanel[i],pEdit2);
		}
		else if(i == 3)//第四页
		{
			for(j = 0; j < 5; j++)//btn
			{
				pBtn = Grap_CreateButton(&LuDan_ViewAttr[idx++],&LuDan_BtnPriv[j+25]);
				if(NULL == pBtn) return;
				gpPanel[i]->panelInsert(gpPanel[i],pBtn);
			}
			
			//姓名
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[13]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);
			//电话
			pEdit = Grap_CreateEdit(&LuDan_ViewAttr[idx++], &LuDan_EditPriv[14]);
			pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
			gpPanel[i]->panelInsert(gpPanel[i],pEdit);
			
			//省  市   县
			for(j = 0; j < 3; j++)
			{
				pComBox = Grap_CreateCombobox(&LuDan_ViewAttr[idx++], &LuDan_ComAttr[j+8]);
				if(NULL == pComBox ) return -1;
				gpPanel[i]->panelInsert(gpPanel[i],pComBox);
				if(j == 0)//省
				{
					pComBox->comboPriv.total_num = 34;
					pComBox->comboPriv.cur_index = gu8Sel_ShengS;
				}
				else if(j == 1)//市
				{
					if(gu8Sel_ShengS == 0 || gu8Sel_ShengS == 1 || gu8Sel_ShengS == 8 || gu8Sel_ShengS == 21)//4个直辖市
					{
						pComBox->view_attr.enable = 0;
					}
					pComBox->comboPriv.total_num = province[gu8Sel_ShengS].nodenum;
					pComBox->comboPriv.cur_index = gu8Sel_ShiS;
				}
				else if(j == 2)//县
				{
					if(gu8Sel_ShengS == 0 || gu8Sel_ShengS == 1 || gu8Sel_ShengS == 8 || gu8Sel_ShengS == 21)//4个直辖市
					{
						pComBox->comboPriv.total_num = province[gu8Sel_ShengS].nodenum;
						pComBox->comboPriv.cur_index = gu8Sel_XianS;
					}
					else
					{
						pComBox->comboPriv.total_num = province[gu8Sel_ShengS].next[gu8Sel_ShiS].nodenum;
						pComBox->comboPriv.cur_index = gu8Sel_XianS;
					}
				}
				pComBox->comboxgetstr = LuDan_ComboboxGetStr;
				pComBox->comboxenter  = LuDan_ComboxEnter;
			}
			
			//详细地址
			pEdit2 = Grap_CreateEdit2(&LuDan_ViewAttr[idx++], &LuDan_Edit2Priv[0],0);
			gpPanel[i]->panelInsert(gpPanel[i],pEdit2);
		}
		
		gpPanel[i]->panelinit(gpPanel[i]);
	}
	
		
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	//退出就断开BT
	if (hyco_getConnectState() == TRUE)
	{
		bt_disconnect(pDeskFather);
	}
	
	return 0;
}

/*-----------------------------------------记录查询显示------------------------------------------------*/
const U8* const opType_LuDan[]=//录单
{
	"状态:",
	
	"操作类型:",
	"扫描网点:",
	"员工编号:",
	"扫描时间:",
	
	"主单号:",
	"目的站:",
	"重量:",
	"件数:",
	"支付方式:",
	"代收签单:",
	"代收货款:",
	"费用:",
	"客户:",
	"运输方式:",
	"物品类别:",
	"物品名称:",
	"币种:",
	"短信通知:",
	"通知号码:",
	"收件人姓名:",
	"收件人电话:",
	"收件人地址:",
	"寄件人姓名:",
	"寄件人电话:",
	"寄件人地址:",
	"子单号:"
};


/*---------------------------------------------------------------
*函数: LuDan_SearchDisplay
*功能: 显示记录查询出来的字符串
*参数: pData:查询到的结果
       total:查询到的总数
       pDispStr:(out)显示的buf
       dispBufLen:pDispStr的最大长度
*返回: 
----------------------------------------------------------------*/
int LuDan_SearchDisplay(void *p)
{
	U8 *str[]={"未传", "已传", "已删", "未传", "未传"};
	U8 *pDispStr;
	tOP_DISPINFO 	*pOpCb;
	tDATA_INFO		*pData;
	tDATA_COMM 		*pComm;
	tLUDAN			*ludan;
	tDATA_INFO 		*pSub;
	U8				*pSubCode;
	int total,dispBufLen, subCnt;
	int i, j, cnt,stat,len=0;
	
	pOpCb = (tOP_DISPINFO *)p;
	pData = (tDATA_INFO *)(pOpCb->pData);
	total = pOpCb->total;
	pDispStr = pOpCb->pDispBuf;
	dispBufLen = pOpCb->dispBufLen;
	
	pSubCode = (U8 *)hsaSdram_UpgradeBuf()+0x3D0000;
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		ludan = &(pData[i].uData.ludan);
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s\r\n",
							opType_LuDan[0], str[stat],
							opType_LuDan[1], pComm->opType,
							opType_LuDan[2], pComm->scanStat,
							opType_LuDan[3], pComm->userId,
							opType_LuDan[4], pComm->scanTime,
							
							opType_LuDan[5], pComm->code,
							opType_LuDan[6], ludan->deststat,
							opType_LuDan[7], ludan->obj_wei,
							opType_LuDan[8], ludan->num,
							opType_LuDan[9], gauPayType[ludan->payType],
							opType_LuDan[10], ludan->daishouCode,
							opType_LuDan[11], ludan->feeDaishou,
							opType_LuDan[12], ludan->feeTotal,
							opType_LuDan[13], ludan->customer_id,
							opType_LuDan[14], gauTranType[ludan->tranType],
							opType_LuDan[15], gauGoodsType[ludan->goodsType],
							opType_LuDan[16], ludan->goodsName,
							opType_LuDan[17], gauMoneyType[ludan->moneyType],
							opType_LuDan[18], gauSms[ludan->ifNotice],
							opType_LuDan[19], ludan->noticeNum,
							opType_LuDan[20], ludan->recvName,
							opType_LuDan[21], ludan->recvPhone,
							opType_LuDan[22], ludan->recvAddr,
							opType_LuDan[23], ludan->sendName,
							opType_LuDan[24], ludan->sendPhone,
							opType_LuDan[25], ludan->sendAddr,
							opType_LuDan[26]
							);
		
		
		len += strlen(&pDispStr[len]);
		//加载该主单下面的子单
		subCnt = RecStore_GetSubCodeByMainCode(Login_GetRight(), OpCom_GetCurOpType(), NULL, 
											pComm->code, pSubCode, 0x30000);
		if(subCnt > 0)
		{
			pSub = (tDATA_INFO *)pSubCode;
			for(j = 0; j < subCnt; j++)
			{
				if(strcmp(pSub[j].tComm.code,pSub[j].tComm.subCode) != 0)
				{
					sprintf(&pDispStr[len], "    %s\r\n", pSub[j].tComm.subCode);
					len += (strlen(pSub[j].tComm.subCode)+6);
				}
			}
		}
		strcat(&pDispStr[len], "-----------------------\r\n");
		
		len += strlen("-----------------------\r\n");
		//需要判断长度是否超出
		if(len + 512 >= dispBufLen)
		{
			strcpy(&pDispStr[len], "显示空间不足,后续记录无法显示.");
		}
	}
	
	return 0;
}


/*-----------------------------------------组织请求数据------------------------------------------------*/
/*---------------------------------------------------------------
*函数: LuDan_FillData
*功能: 填充请求数据
*参数: pSend:用于发送的buf
       pNode:准备打包发送的数据
       cnt:准备打包的个数
       pUrl:请求的url
*返回: 0:成功   -1:网络通讯异常
----------------------------------------------------------------*/
int LuDan_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*pSaomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

return 0;//test
	
	//可以批量上传	
	//填充请求头
	dataLen = NetReq_FillHead_yida(pSend, "", "addScanData");
	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		pSaomiao = &(pNode[i].tData.uData.saomiao);
		
		sprintf(&pSend[dataLen], "<detailValue><scanType>%02d</scanType><scanUser>%s</scanUser><scanSite>%s</scanSite><scanTime>%s</scanTime>\
<scanHawb>%s</scanHawb><preNextSite>%s</preNextSite><shiftTimes>%s</shiftTimes><weight></weight><bagNumber></bagNumber>\
<exceptionCode></exceptionCode><exceptionMemo></exceptionMemo><signatureType></signatureType><signAture></signAture>\
<businessMan>%s</businessMan></detailValue>",
					pComm->opType,pComm->userId,pComm->scanStat,pComm->scanTime,pComm->code,pSaomiao->nextstat,pSaomiao->tran_types,pComm->userId);
		
		dataLen = strlen(pSend);
	}
	
	strcpy(&pSend[dataLen], "</senddata>");
	dataLen += strlen("</senddata>");
	
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
#if 0	
	//编码转换
	//发送前需要先转成unicode  再转成utf-8
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	//1 转unicode
	Hyc_Gbk2Unicode(pSend,pTemp,0);
	//2 转utf-8
	Unicode2UTF8(pTemp, pSend);
#endif
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=GBK";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("LuDan send = ");
hyUsbMessageByLen(pSend, strlen(pSend));
hyUsbPrintf("\r\n");
	cnt = http_send_request(pUrl, bufLen, &tRequest);
	if(cnt > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		
		cnt = Net_DecodeData(pSend,cnt,pGbk);
		if(cnt > 0)
		{
			memcpy(pSend,pGbk,cnt);
			pSend[cnt] = 0;
			pSend[cnt+1] = 0;
	hyUsbPrintf("LuDan recv = %s \r\n",pSend);	
			pTemp = strstr(pSend, "<result>");
			pE = strstr(pSend, "</result>");
			if(pTemp != NULL && pE != NULL)
			{
				pTemp += strlen("<result>");
				dataLen = pE - pTemp;
				if(dataLen > 4) dataLen = 4;
				memcpy(buf,pTemp, dataLen);
				buf[dataLen] = 0;
				ret = atoi(buf);
			}
		}
	}
hyUsbPrintf("LuDan ret = %d \r\n",ret);	
	
	return ret;
}

#endif //VER_CONFIG_HYCO