#ifndef __EDIT_H__
#define __EDIT_H__

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "glbVariable.h"
#include "grap_api.h"
#include "grap_view.h"
#include "base.h"

#define INPUT_BIG_ABS_START     (7)
#define INPUT_BIG_ABS_LEN		(30)
#define INPUT_SMAL_ABS_START    (INPUT_BIG_ABS_START+INPUT_BIG_ABS_LEN)
#define INPUT_SMAL_ABS_LEN		(30)
#define INPUT_DIGIT_START  		(INPUT_SMAL_ABS_START+INPUT_SMAL_ABS_LEN)
#define INPUT_DIGIT_LEN			(16)
#define INPUT_SIGN_START  		(INPUT_DIGIT_START+INPUT_DIGIT_LEN)
#define INPUT_SIGN_LEN			(36)
#define INPUT_WWW_START  		(INPUT_SIGN_START+INPUT_SIGN_LEN)
#define INPUT_WWW_LEN			(15)

#define INPUT_HANDW_START  		(INPUT_WWW_START+INPUT_WWW_LEN)
#define INPUT_HANDW_LEN			(12)//(18) //12

#define INPUT_IME_START  		(INPUT_HANDW_START+INPUT_HANDW_LEN)
#define INPUT_IME_LEN			(37) 

#define INPUT_SEL_WORD_START    (INPUT_IME_START+INPUT_IME_LEN)  //183
#define INPUT_SEL_WORD_LEN		(32)


#define INPUT_BTN_TOTAL         (INPUT_SEL_WORD_START+INPUT_SEL_WORD_LEN)  //215

#define INPUT_TOTAL_WIDGET      (INPUT_BTN_TOTAL+1)

#define INPUT_TOTAL_CTRL        (9)

//删除字符的值
#define INPUT_VALUE_DEL	  (10)

//代表控制板的模式
#define INPUT_CTR_BIGABC  1
#define INPUT_CTR_SMAABC  2
#define INPUT_CTR_DIGIT   3 
#define INPUT_CTR_SIGN    4
#define INPUT_CTR_IME     5 /* 拼音 */
#define INPUT_CTR_CHI     6 /* 手写 */
#define INPUT_CTR_WWW     7
#define INPUT_CTR_PREVFOCUS 8
#define INPUT_CTR_NEXTFOCUS 9

//用于显示的字符的空间个数
#define INPUT_SHOW_BUF_TOTAL  (40)

//允许使能的最大控件数
#define INPUT_REGISTER_MAX_WIDGET  (20)

typedef S32 (*Button_Return_Word)(void *pView, U16 word);
typedef S32 (*edit_change_focus)(void *pView, int type);

#define  MAX_SHOW_HZ			6
#define  MAX_IDENTIFY_HZ		5
#define ARROW_F_TAIL			20
#define ARROW_T_FRONT			220



//用于管理内部拼音输入法部分
typedef struct _tIME_SHOW
{
    char *pIme;
    char inBuf[9];//输入的BUF
    U8   pageIndex;
    U8   type; //目前buf的状态 0：拼音 1：联想字
}tIME_SHOW;

typedef struct _tINPUT_BMP_CLR
{
    U16 press;  //press color
    U16 normal; //normal color
    U16 writeClr; //写字时候的颜色  
    U8  inputMode; //
    U8  status;  //输入法的状态 打开还是关闭   
}tINPUT_BMP_CTR;

typedef struct _tASSN_CTR
{
    U8   flag;//是否进行联想
    U32  times;//进行联想的次数
    U16  lastWord; //上次输入的字符
    U8   assnBuf[INPUT_SHOW_BUF_TOTAL*2];//联想输入buf
    U8   validTotal;//有效字符的个数
    U8   willGetNum;  //需要得到的汉字个数
}tASSN_CTR;

typedef struct _tSEL_WORD_CTR
{
    U8  type;  //0 没有下拉备选字  1:拼音 2:手写
    U8  enable; //是否开启下拉备选字 0 或 1
}tSEL_WORD_CTR;

typedef struct _tCTR_PANEL
{
    U8 inputType;  //输入法类型  
    U8 alphaDisp;//是否显示 1：显示 0:不显示
    tINPUT_BMP_CTR tSysCtr; //总的面板控制
    tGRAP_RECT	tRect;
    U8  *pBtnEnable[250];//btn使能控制	  //INPUT_BTN_TOTAL
    U8  *pHandLabEnable; //手写板使能控制
    
    tGRAPEDIT  *pEditInput;  //键盘输入法使用
    U8  *pCptBtn;  //键盘输入法使用  输入完成按键
    
    U8  BtnCharShow[INPUT_SHOW_BUF_TOTAL][3];//字符显示的buf空间
    U8  HandWordShowFlag;//手写识别时，是否显示汉字
    tIME_SHOW tIme;
    Button_Return_Word edit_return_word;
    tASSN_CTR tAssnCtr; //联想输入法控制
    tSEL_WORD_CTR tSelWord;//下拉备选字控制
    edit_change_focus edit_change_widget_focus;
    tGRAPVIEW *pWidget[INPUT_REGISTER_MAX_WIDGET];
    U8        curRegCnt;
    
    //两个共用的btn按键
    tGRAPBUTTON *pShareBtn[2];
    
    //新增
    U8 Is_WriteHand;//0:联想的汉字   1：手写识别的汉字
    tGRAPBUTTON *gu_ShowHZ_sx[MAX_SHOW_HZ];
	tGRAPBUTTON *gu_ShowHZ_arrow[2];
	tGRAPBUTTON *gu_ShowHZ_identify[MAX_IDENTIFY_HZ];
	tGRAPBUTTON *gu_DeleteBtn;
	
}tCTR_PANEL;

extern const U16 gSelBtnShowId[];	  //INPUT_SEL_WORD_LEN
extern const U16 gImeBtnShowId[];
extern const U16 gHandBtnShowId[];
extern tINPUT_BMP_CTR gtBtnBmpCtr;


/*---------------------------------------------------------
*函数: edit_get_input_status
*功能: get edit status 
*参数: none
*返回: 1 : open 0:close
*---------------------------------------------------------*/
int  edit_get_input_status(tGRAPDESKBOX *pDesk);

/*---------------------------------------------------------
*函数: edit_auto_change_input_mode
*功能: aoto change mode: close or open
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_auto_change_input_mode(tGRAPDESKBOX *pDesk, U16 color);

/*---------------------------------------------------------
*函数: edit_get_special_string
*功能: 程序中是长字符串的时候返回这个值，否则返回none
*参数: none
*返回: none
*---------------------------------------------------------*/
char *edit_get_special_string(U16 word, void *pView);

/*---------------------------------------------------------
*函数: edit_modify_input_mode
*功能: 改变输入方式
*参数: none
*返回: none
*---------------------------------------------------------*/
void edit_modify_input_mode(tGRAPDESKBOX *pDesk, int mode, U16 color);

/*---------------------------------------------------------
*函数: edit_creat_panel   
*功能: 创建输入法			 
*参数: none
*返回: 创建
*---------------------------------------------------------*/
void edit_creat_panel(tGRAPDESKBOX *pDesk, int mode, Button_Return_Word returnWord, edit_change_focus changeFocus);

#endif
