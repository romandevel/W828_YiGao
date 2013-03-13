#ifndef __LEDCTL_H__
#define __LEDCTL_H__


#define LED_CLOSE		(0x00)
#define LED_RED			(0x01)
#define LED_GREEN		(0x02)
#define LED_BLUE		(0x04)

#define LED_YELLOW		(0x03)//(LED_RED|LED_GREEN)		//黄
#define LED_MAGENTA		(0x05)//(LED_RED|LED_BLUE)		//品红
#define LED_CYAN		(0x06)//(LED_GREEN|LED_BLUE)	//青色
#define LED_WHITE		(0x07)//(LED_RED|LED_GREEN|LED_BLUE)
/*-------------------------------------------------------------------------
* 函数: Led_init
* 说明: 三个LED控制IO的初始化
* 参数: 无
* 返回: 无
-------------------------------------------------------------------------*/
void Led_init(void);

/*-------------------------------------------------------------------------
* 函数: Led_ctrl
* 说明: LED控制
* 参数: color：0----7	0关闭，7白色
* 返回: 无
-------------------------------------------------------------------------*/
void Led_ctrl(int color);

/*-------------------------------------------------------------------------
* 函数: Led_keyPad_ctrl
* 说明: 键盘LED控制
* 参数: enable: 0----关闭，否则打开
* 返回: 无
-------------------------------------------------------------------------*/
void Led_keyPad_ctrl(int enable);

#endif

