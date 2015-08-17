/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSMarketDataAPI.h
///@brief 定义了客户端接口使用的业务通讯接口
///@history 
///20140801	Kingstar IRDG		创建该文件
/////////////////////////////////////////////////////////////////////////

#ifndef __KSFMDUSER_API_H__
#define __KSFMDUSER_API_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#ifdef KSFMDUSERAPI_EXPORTS
#define KSFMDUSERAPI __declspec(dllexport)
#else
#define KSFMDUSERAPI __declspec(dllimport)
#endif
#else
#ifdef KSFMDUSERAPI_EXPORTS
#define KSFMDUSERAPI __attribute__ ((visibility ("default"))) 
#else
#define KSFMDUSERAPI
#endif
#endif 

#include "../IncEx/KSMdApiEx.h"
#include "../IncEx/KSVocMdApi.h"

#endif