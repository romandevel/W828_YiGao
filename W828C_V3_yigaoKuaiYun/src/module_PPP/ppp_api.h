#ifndef __PPP_API_H__
#define __PPP_API_H__

/*ppp当前状态*/
#define PPP_STATUS_CLOSE  -1
#define PPP_STATUS_START   0
#define PPP_STATUS_OPEN    1
#define PPP_STATUS_CLOSING 2
/*-----------------------------------------
* 函数：ppp_get_status
* 功能：得到ppp协议层的状态
* 参数：none
* 返回：ppp状态
*-------------------------------------------*/
int ppp_get_status(void);

/*-----------------------------------------
* 函数：ppp_close
* 功能：应用层接口，关闭ppp协议
* 参数：none
* 返回：none
*-------------------------------------------*/
void ppp_close(void);

/*-------------------------------------------
* 函数：ppp_intial 
* 功能：打开ppp协议层之前先调用该接口初始化ppp
* 参数：none
* 返回：none
*-----------------------------------------*/
void ppp_intial(void);

/*-------------------------------------------
* 函数：ppp_delete 
* 功能：删除协议
* 参数：none
* 返回：none
*-----------------------------------------*/
void ppp_delete(void);

/*-------------------------------------------
* 函数：ppp_open 
* 功能：打开ppp协议层，打开之前moderm必须准备好，
        即必须是CONNECT收到
* 参数：none
* 返回：none
*-----------------------------------------*/
int ppp_open(void);

/*---------------------------------------------------------
 * 函数 : ppp_read_data
 * 功能 : 供外部接口传输数据进入
 * 参数 : buf:数据指针  len ： buf的长度
 * 返回 ; >=0:传输成功  其他 失败
 *--------------------------------------------------------*/
int ppp_read_data(char *buf, int len);

#endif