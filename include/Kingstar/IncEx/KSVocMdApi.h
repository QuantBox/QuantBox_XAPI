/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSVocMDApi.h
///@brief 定义了客户端客户定制行情接口
/////////////////////////////////////////////////////////////////////////

#ifndef __KSVOCMDAPI_H_INCLUDED_
#define __KSVOCMDAPI_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KSUserApiStructEx.h"
#include "KSVocApiStruct.h"

#if defined(ISLIB) && defined(WIN32) && !defined(KSMDAPI_STATIC_LIB)
#ifdef LIB_MD_API_EXPORT
#define TRADER_VOCMDAPI_EXPORT __declspec(dllexport)
#else
#define TRADER_VOCMDAPI_EXPORT __declspec(dllimport)
#endif
#else
#ifdef WIN32
#define TRADER_VOCMDAPI_EXPORT 
#else
#define TRADER_VOCMDAPI_EXPORT __attribute__((visibility("default")))
#endif

#endif

namespace KingstarAPI
{

	class CKSVocMdSpi
	{
	public:
		///订阅行情应答
		virtual void OnRspSubKSMarketData(CKSSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///取消订阅行情应答
		virtual void OnRspUnSubKSMarketData(CKSSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	};

	class TRADER_VOCMDAPI_EXPORT CKSVocMdApi
	{
	public:
		///订阅行情。
		///@param ppInstrumentID 合约
		///@param nCount 要订阅/退订行情的合约个数
		///@remark 
		virtual int SubscribeKSMarketData(CKSSpecificInstrumentField *ppInstrumentID[], int nCount) = 0;

		///退订行情。
		///@param ppInstrumentID 合约 
		///@param nCount 要订阅/退订行情的合约个数
		///@remark 
		virtual int UnSubscribeKSMarketData(CKSSpecificInstrumentField *ppInstrumentID[], int nCount) = 0;

	protected:
		~CKSVocMdApi(){};
	};

}	// end of namespace KingstarAPI
#endif