/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSOptionApi.h
///@brief 定义了客户端期权类接口
/////////////////////////////////////////////////////////////////////////

#ifndef __KSOPTIONAPI_H_INCLUDED__
#define __KSOPTIONAPI_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KSVocApiStruct.h"

#if defined(ISLIB) && defined(WIN32) && !defined(KSTRADEAPI_STATIC_LIB)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_OPTIONAPI_EXPORT __declspec(dllexport)
#else
#define TRADER_OPTIONAPI_EXPORT __declspec(dllimport)
#endif
#else
#ifdef WIN32
#define TRADER_OPTIONAPI_EXPORT 
#else
#define TRADER_OPTIONAPI_EXPORT __attribute__((visibility("default")))
#endif

#endif

namespace KingstarAPI
{

	class CKSOptionSpi
	{
	public:
		///查询股指现货指数
		virtual void OnRspQryIndexPrice(CKSIndexPriceField *pIndexPrice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///查询股指现货指数
		virtual void OnRspQryOptionInstrGuard(CKSOptionInstrGuardField *pOptionInstrGuard, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///查询宣告数量请求响应
		virtual void OnRspQryExecOrderVolume(CKSExecOrderVolumeField *pExecOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///查询个股行权指派信息请求响应
		virtual void OnRspQryStockOptionAssignment(CKSStockOptionAssignmentField *pStockOptionAssignment, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///查询客户交易级别响应
		virtual void OnRspQryInvestorTradeLevel(CKSInvestorTradeLevelField *pInvestorTradeLevel, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///查询个股限购额度响应
		virtual void OnRspQryPurchaseLimitAmt(CKSPurchaseLimitAmtField *pPurchaseLimitAmt, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///查询个股限仓额度响应
		virtual void OnRspQryPositionLimitVol(CKSPositionLimitVolField *pPositionLimitVol, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询个股历史报单响应
		virtual void OnRspQryHistoryOrder(CKSHistoryOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询个股历史成交响应
		virtual void OnRspQryHistoryTrade(CKSHistoryTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询个股历史行权指派明细响应
		virtual void OnRspQryStockOptionHistoryAssignment(CKSHistoryAssignmentField *pHistoryAssignment, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询个股行权交割明细响应
		virtual void OnRspQryStockOptionDelivDetail(CKSSODelivDetailField *pSODelivDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///自动行权执行操作响应
		virtual void OnRspAutoExecOrderAction(CKSAutoExecOrderActionField *pAutoExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///申请个股组合录入请求响应
		virtual void OnRspCombActionInsert(CKSInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///查询个股组合持仓明细应答
		virtual void OnRspQryInvestorCombinePosition(CKSInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///个股可组合可拆分手数请求响应
		virtual void OnRspQryCombActionVolume(CKSCombActionVolumeField *pCombActionVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///个股组合拆分委托通知
		virtual void OnRtnCombAction(CKSCombActionField *pCombAction) {};
	};

	class TRADER_OPTIONAPI_EXPORT CKSOptionApi
	{
	public:
		///查询股指现货指数
		virtual int ReqQryIndexPrice(CKSQryIndexPriceField *pIndexPrice, int nRequestID) = 0;

		///查询期权合约保障系数
		virtual int ReqQryOptionInstrGuard(CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID) = 0;

		///查询宣告数量请求
		virtual int ReqQryExecOrderVolume(CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID) = 0;

		///查询个股行权指派信息
		virtual int ReqQryStockOptionAssignment(CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID) = 0;

		///查询客户交易级别
		virtual int ReqQryInvestorTradeLevel(CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID) = 0;

		///查询个股限购额度
		virtual int ReqQryPurchaseLimitAmt(CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID) = 0;

		///查询个股限仓额度
		virtual int ReqQryPositionLimitVol(CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID) = 0;

		///请求查询个股历史报单
		virtual int ReqQryHistoryOrder(CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID) = 0;

		///请求查询个股历史成交
		virtual int ReqQryHistoryTrade(CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID) = 0;

		///请求查询个股历史行权指派明细
		virtual int ReqQryStockOptionHistoryAssignment(CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID) = 0;

		///请求查询个股行权交割明细
		virtual int ReqQryStockOptionDelivDetail(CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID) = 0;

		///自动行权执行操作
		virtual int ReqAutoExecOrderAction(CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID) = 0;

		///个股组合拆分委托请求
		virtual int ReqCombActionInsert(CKSInputCombActionField *pInputCombAction, int nRequestID) = 0;

		///查询个股组合持仓明细
		virtual int ReqQryInvestorCombinePosition(CKSQryInvestorPositionCombineDetailField *pQryInvestorCombinePosition, int nRequestID) = 0;

		///查询个股可组合可拆分手数请求
		virtual int ReqQryCombActionVolume(CKSQryCombActionVolumeField *pQryCombActionVolume, int nRequestID) = 0;

	protected:
		~CKSOptionApi(){};
	};

}	// end of namespace KingstarAPI
#endif