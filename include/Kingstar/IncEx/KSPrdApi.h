/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSPrdApi.h
///@brief 定义了客户端主数据业务接口
/////////////////////////////////////////////////////////////////////////

#ifndef __KSPRDAPI_H_INCLUDED__
#define __KSPRDAPI_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KSUserApiStructEx.h"
#include "KSVocApiStruct.h"

#if defined(ISLIB) && defined(WIN32) && !defined(KSTRADEAPI_STATIC_LIB)

#ifdef LIB_TRADER_API_EXPORT
#define TRADER_PRDAPI_EXPORT __declspec(dllexport)
#else
#define TRADER_PRDAPI_EXPORT __declspec(dllimport)
#endif
#else
#ifdef WIN32
#define TRADER_PRDAPI_EXPORT 
#else
#define TRADER_PRDAPI_EXPORT __attribute__((visibility("default")))
#endif

#endif

namespace KingstarAPI
{
	class CKSPrdSpi
	{
	public:
		///定制主数据业务响应
		virtual void OnRspSubPrimeData(CKSPrimeDataBusinessField *pPrimeDataBusiness, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	};

	class TRADER_PRDAPI_EXPORT CKSPrdApi
	{
		///定制主数据业务请求
		virtual int ReqSubPrimeData(CKSSubPrimeDataBusinessField *pSubPrimeDataBusiness, int nRequestID) = 0;

	protected:
		~CKSPrdApi(){};
	};

}	// end of namespace KingstarAPI
#endif