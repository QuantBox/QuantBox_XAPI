/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSCosApi.h
///@brief 定义了客户端条件单接口
/////////////////////////////////////////////////////////////////////////

#ifndef __KSCOSAPI_H_INCLUDED_
#define __KSCOSAPI_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KSUserApiStructEx.h"
#include "KSVocApiStruct.h"

#if defined(ISLIB) && defined(WIN32) && !defined(KSTRADEAPI_STATIC_LIB)

#ifdef LIB_TRADER_API_EXPORT
#define TRADER_COSAPI_EXPORT __declspec(dllexport)
#else
#define TRADER_COSAPI_EXPORT __declspec(dllimport)
#endif
#else
#ifdef WIN32
#define TRADER_COSAPI_EXPORT 
#else
#define TRADER_COSAPI_EXPORT __attribute__((visibility("default")))
#endif

#endif

namespace KingstarAPI
{

	class CKSCosSpi
	{
	public:
		///条件单录入响应
		virtual void OnRspInitInsertConditionalOrder(CKSConditionalOrderOperResultField *pInitInsertConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单查询响应
		virtual void OnRspQueryConditionalOrder(CKSConditionalOrderOperResultField *pQueryConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单修改响应
		virtual void OnRspModifyConditionalOrder(CKSConditionalOrderOperResultField *pModifyConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单暂停激活响应
		virtual void OnRspPauseConditionalOrder(CKSConditionalOrderOperResultField *pPauseConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单删除响应
		virtual void OnRspRemoveConditionalOrder(CKSConditionalOrderRspResultField *pRemoveConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单选择响应
		virtual void OnRspSelectConditionalOrder(CKSConditionalOrderRspResultField *pSelectConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///止损止盈单录入响应
		virtual void OnRspInsertProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pInsertProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///止损止盈单修改响应
		virtual void OnRspModifyProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pModifyProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///止损止盈单删除响应
		virtual void OnRspRemoveProfitAndLossOrder(CKSProfitAndLossOrderRemoveField *pRemoveProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///止损止盈单查询响应
		virtual void OnRspQueryProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pQueryProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///条件单请求选择通知
		virtual void OnRtnCOSAskSelect(CKSCOSAskSelectField *pCOSAskSelect) {};

		///条件单状态通知
		virtual void OnRtnCOSStatus(CKSCOSStatusField *pCOSStatus) {};

		///止损止盈单状态通知
		virtual void OnRtnPLStatus(CKSPLStatusField *pPLStatus) {};
	};

	class TRADER_COSAPI_EXPORT CKSCosApi
	{
	public:
		///条件单录入请求
		virtual int ReqInitInsertConditionalOrder(CKSConditionalOrderInitInsert *pConditionalOrderInitInsert, int nRequestID) = 0;

		///查询条件单请求
		virtual int ReqQueryConditionalOrder(CKSConditionalOrderQuery *pConditionalOrderQuery, int nRequestID) = 0;

		///修改条件单请求
		virtual int ReqModifyConditionalOrder(CKSConditionalOrderModify *pConditionalOrderModify, int nRequestID) = 0;

		///删除条件单请求
		virtual int ReqRemoveConditionalOrder(CKSConditionalOrderRemove *pConditionalOrderRemove, int nRequestID) = 0;

		///暂停或激活条件单请求
		virtual int ReqStateAlterConditionalOrder(CKSConditionalOrderStateAlter *pConditionalOrderStateAlter, int nRequestID) = 0;

		///选择条件单请求
		virtual int ReqSelectConditionalOrder(CKSConditionalOrderSelect *pConditionalOrderSelect, int nRequestID) = 0;

		///止损止盈单录入请求
		virtual int ReqInsertProfitAndLossOrder(CKSProfitAndLossOrderInsert *pProfitAndLossOrderInsert, int nRequestID) = 0;

		///止损止盈单修改请求
		virtual int ReqModifyProfitAndLossOrder(CKSProfitAndLossOrderModify *pProfitAndLossOrderModify, int nRequestID) = 0;

		///止损止盈单删除请求
		virtual int ReqRemoveProfitAndLossOrder(CKSProfitAndLossOrderRemove *pProfitAndLossOrderRemove, int nRequestID) = 0;

		///止损止盈单查询请求
		virtual int ReqQueryProfitAndLossOrder(CKSProfitAndLossOrderQuery *pProfitAndLossOrderQuery, int nRequestID) = 0;

	protected:
		~CKSCosApi(){};
	};

}	// end of namespace KingstarAPI
#endif