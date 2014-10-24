#ifndef _API_HEADER_H_
#define _API_HEADER_H_

#include "../include/CrossPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif
	//用于分隔输入的合列表，与前置机地址列表，所以不能出现“:”一类的
	#define _QUANTBOX_SEPS_ ",;"

	DLL_PUBLIC long long __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif

#endif//end of _API_HEADER_H_
