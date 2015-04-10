#ifndef _API_HEADER_H_
#define _API_HEADER_H_

#include "../include/CrossPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif
	DLL_PUBLIC void* __stdcall CreateApi(char* libPath);

#ifdef __cplusplus
}
#endif

#endif//end of _API_HEADER_H_
