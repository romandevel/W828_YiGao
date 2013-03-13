#include "hyTypes.h"
#include "touchKey.h"
#include "keyCodeEvent.h"
#include "grap_view.h"

/*
 * 主要用来通过触摸来模式三个机械按键动作，只有短按动作产生，没有长按
*/

//触摸状态区分
#define TOUCH_STATE_IDEL   (0)
#define TOUCH_STATE_TOUCH  (1)

//有效的Y触摸区域
#define TOUCH_EFFECT_Y   (410)

//触摸按键之间的有效间距
#define TOUCH_KEY_INTER_X  (80)

//触摸按键区域对应按键响应
static const U8 gu16TouchXTab[]=
{
    Keyb_Code_F2,     
    Keyb_Code_OK,
    Keyb_Code_Esc,

};

//触摸状态记录
typedef struct _tTOUCH_FSM
{
    U8 state;   //状态
    U8 curKey;  //当前的按键
}tTOUCH_FSM;

static tTOUCH_FSM  gtTouchFsm;

/*---------------------------------------------------------------------------
 函数：tk_check_point_key
 功能：用于记录触摸点的值，判断是否为触摸键
 参数：触摸点  (x,y)
 返回：-1 不是按键  否则为按键值
* -----------------------------------------------------------------------------*/
int tk_check_point_key(int x, int y, int state)
{
    int i;
    int stateFsm;
    int key;
    int index;
    int rv = -1;
    int temp;
    
    //需要判断横竖屏再确定x,y位置
    //if(1 == LcdModule_Get_ShowType())
    //{
    //    temp = x;
    //    x = 239-y;
    //    y = temp;
    //}
    
    //判断Y值是否合法
    if (y < TOUCH_EFFECT_Y)
    {
        gtTouchFsm.state = TOUCH_STATE_IDEL;
        
        return rv;
    }
    
     
    index = x/TOUCH_KEY_INTER_X;
    index = index > 2? 2: index;
    
    key = gu16TouchXTab[index];
    
    stateFsm = gtTouchFsm.state;
    
    switch(stateFsm)
    {
        case TOUCH_STATE_IDEL:
            if (VIEW_PULL_START != state)
            {
                return rv;
            }
            gtTouchFsm.state = TOUCH_STATE_TOUCH;
            gtTouchFsm.curKey = key;
            break;
            
        case TOUCH_STATE_TOUCH:
            if (gtTouchFsm.curKey != key)
            {
                gtTouchFsm.state = TOUCH_STATE_IDEL; 
            }
            
            //触摸按键抬起，可以发出该按键消息
            if (VIEW_PULL_END == state)
            {
                 gtTouchFsm.state = TOUCH_STATE_IDEL;
                 
                 return key;
            }
            break;
            
    }
    
    return rv;
}