#ifndef __CONST_EDIT_DATA_H__
#define __CONST_EDIT_DATA_H__


#define  IME_FRESH_NONE     0
#define  IME_FRESH_IME      1
#define  IME_FRESH_CHT      2
#define  IME_FRESH_ALL      3


//输出的特殊值 例如:http:// | wap.  |www. |.com .cn |.net |.com.cn |.gov |smtp |pop |pop3
#define INPUT_NET_VALUE_1     0xFFE1
#define INPUT_NET_VALUE_2     0xFFE2
#define INPUT_NET_VALUE_3     0xFFE3
#define INPUT_NET_VALUE_4     0xFFE4
#define INPUT_NET_VALUE_5     0xFFE5
#define INPUT_NET_VALUE_6     0xFFE6
#define INPUT_NET_VALUE_7     0xFFE7
#define INPUT_NET_VALUE_8     0xFFE8
#define INPUT_NET_VALUE_9     0xFFE9
#define INPUT_NET_VALUE_A     0xFFEA
#define INPUT_NET_VALUE_B     0xFFEB

#define  LABEL_ID_1_EDIT    1000

#define EDIT_SEL_CUR_PAGE      700
#define EDIT_SEL_NEXT_PAGE     701
#define EDIT_SEL_PRV_PAGE      801
#define EDIT_SEL_ENABLE_PAGE   802

#define EDIT_SEL_PAGE_SIZE     35


#define IME_VALUE_NONE   0x7FE0
								 

extern const tGrapViewAttr EditLabelStruct[];
extern const tGrapLabelPriv LabelPriStruct[];
extern const tGrapViewAttr EditButtonStruct[];
extern const tGrapButtonPriv gtBtnPriAttr[];
extern const tGrapButtonPriv BottonPriStruct[];


#endif