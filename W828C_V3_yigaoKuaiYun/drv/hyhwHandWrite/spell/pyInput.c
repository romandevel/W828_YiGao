/*
           pyInput.c - 拼音输入法模块

*/
#include "hyTypes.h"

char *py_ime(char *strInput_py_str)
{
	char *p;

	if (strInput_py_str[0] == 0) return "";
	
	
	/*zj20110123 去除耦合，换一个函数名调用*/
	phb_idxSearch_getChLtr(strInput_py_str, &p);
	//phb_if_Search_getChLtr(strInput_py_str, &p);

	if (p == NULL)
	{
		return "";
	}

	return p;  
}
















