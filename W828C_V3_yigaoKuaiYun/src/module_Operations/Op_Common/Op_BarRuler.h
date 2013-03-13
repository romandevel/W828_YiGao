#ifndef _OP_BARRULER_H
#define _OP_BARRULER_H


#include "hyTypes.h"
#include "Op_Config.h"




typedef enum _barcode_type_e
{
	TYPE_EMPLOYEID=1,	//员工ID
	TYPE_COMPANYID,		//网点编号
	TYPE_BARCODE,		//运单号
	TYPE_CARID,			//车辆吗
	TYPE_CARLINE,		//车线码
	TYPE_PACKET,		//大包号
	TYPE_BAKCODE,		//回单号
	TYPE_TAG,			//封签
	
	TYPE_NULL=255
	
}eBARCODE_TYPE;




#endif //_OP_BARRULER_H