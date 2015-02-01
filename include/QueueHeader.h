#ifndef _QUEUE_HEADER_H_
#define _QUEUE_HEADER_H_

#include "../include/CrossPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* (__stdcall *fnOnRespone) (char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1,int size1, void* ptr2,int size2, void* ptr3,int size3);

	//DLL_PUBLIC void* __stdcall XRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif

#endif//end of _QUEUE_HEADER_H_
