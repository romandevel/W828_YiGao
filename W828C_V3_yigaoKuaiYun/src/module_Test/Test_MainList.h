#ifndef _SYS_TEST_H
#define _SYS_TEST_H


#include "hyTypes.h"
#include "base.h"
#include "grap_api.h"
#include "grap_view.h"
#include "glbVariable.h"
#include "glbVariable_base.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Net_Evt.h"
#include "bmpComData.h"
#include "AbstractFileSystem.h"
#include "Rec_Info.h"
#include "Op_Common.h"
#include "Op_Types.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"

typedef int (*TestFun)(void *pView);

typedef struct _tTest_Fun
{
	U8		*pName;	//显示名称
	TestFun	testfun;//执行函数
	
}tTEST_FUN;


extern U8	gau8TestFunBuf[];


extern int TestFun_Serial(void *pDeskFather);

extern int TestFun_Scan(void *pDeskFather);

extern int TestFun_NetChange(void *pDeskFather);

extern int TestFun_DC(void *pDeskFather);

extern int TestFun_QR(void *pDeskFather);

extern int TestFun_LB(void *pDeskFather);

extern int TestFun_OnePiece(void *pDeskFather);

extern int TestFun_MorePiece(void *pDeskFather);

extern void Rec_SearchDesk(void *pDeskFather);

extern void Rec_DelErrScanDesk(void *pDeskFather);

extern int TestFun_LoadRec(void *pDeskFather);

extern int TestFun_LED(void *pDeskFather);

extern int TestFun_Factory(void *pDeskFather);

extern int TestFun_Vibrator(void *pDeskFather);

extern int TestFun_Beeper(void *pDeskFather);

extern int TestFun_BT(void *pDeskFather);

extern int TestFun_Time(void *pDeskFather);

extern int TestFun_GPS(void *pDeskFather);

extern int TestFun_ClearRec(void *pDeskFather);

#endif //_SYS_TEST_H