#ifndef __KSVOCAPI_H_INCLUDED_
#define __KSVOCAPI_H_INCLUDED_
/**
* @file KSVocApi.h
* @author bonny.liu
* system：KSTradeAPI
* company：Kingstar
* history：
* 2014/05/08	bonny.liu	 create
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KSUserApiStructEx.h"
#include "KSVocApiStruct.h"

#if defined(ISLIB) && defined(WIN32) && !defined(KSTRADEAPI_STATIC_LIB)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_VOCAPI_EXPORT __declspec(dllexport)
#else
#define TRADER_VOCAPI_EXPORT __declspec(dllimport)
#endif
#else
#ifdef WIN32
#define TRADER_VOCAPI_EXPORT 
#else
#define TRADER_VOCAPI_EXPORT __attribute__((visibility("default")))
#endif

#endif

namespace KingstarAPI
{

	class CKSVocSpi
	{
	public:
		///查询开盘前的持仓明细应答
		virtual void OnRspQryInvestorOpenPosition(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///查询开盘前的组合持仓明细应答
		virtual void OnRspQryInvestorOpenCombinePosition(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///批量报单撤除请求响应
		virtual void OnRspBulkCancelOrder(CThostFtdcBulkCancelOrderField *pBulkCancelOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///平仓策略查询响应
		virtual void OnRspQryCloseStrategy(CKSCloseStrategyResultField *pCloseStrategy, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///组合策略查询响应
		virtual void OnRspQryCombStrategy(CKSCombStrategyResultField *pCombStrategy, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///期权组合策略查询响应
		virtual void OnRspQryOptionCombStrategy(CKSOptionCombStrategyResultField *pOptionCombStrategy, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

		///请求查询客户转帐信息响应
		virtual void OnRspQryTransferInfo(CKSTransferInfoResultField *pResultField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询交易通知响应
		virtual void OnRspQryKSTradingNotice(CKSTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///用户端产品资源查询应答
		virtual void OnRspQryUserProductUrl(CKSUserProductUrlField *pUserProductUrl, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询交叉汇率响应
		virtual void OnRspQryCrossRate(CKSCrossRateField *pCrossRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///组合拆分单录入响应
		virtual void OnRspMatchOrderInsert(CKSMatchOrderInsertField *pMatchOrderInsert, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///组合拆分单查询请求响应
		virtual void OnRspQryMatchOrder(CKSMatchOrderField *pMatchOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///最大组合拆分单量查询请求响应
		virtual void OnRspQryMaxMatchOrderVolume(CKSMaxMatchOrderVolumeField *pMaxMatchOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///交易通知
		virtual void OnRtnKSTradingNotice(CKSTradingNoticeField *pTradingNoticeInfo) {};

		///组合拆分单通知
		virtual void OnRtnMatchOrder(CKSMatchOrderField *pMatchOrder) {};
	};

	class TRADER_VOCAPI_EXPORT CKSVocApi
	{
	public:
		///查询开盘前的持仓明细
		virtual int ReqQueryInvestorOpenPosition(CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID) = 0;

		///查询开盘前的组合持仓明细
		virtual int ReqQueryInvestorOpenCombinePosition(CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID) = 0;

		///批量撤单
		virtual int ReqBulkCancelOrder (CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID) = 0;

		///平仓策略查询请求
		virtual int ReqQryCloseStrategy(CKSCloseStrategy *pCloseStrategy, int nRequestID) = 0;

		///组合策略查询请求
		virtual int ReqQryCombStrategy(CKSCombStrategy *pCombStrategy, int nRequestID) = 0;

		///期权组合策略查询请求
		virtual int ReqQryOptionCombStrategy(CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID) = 0;

		///请求查询客户转帐信息
		virtual int ReqQryTransferInfo(CKSTransferInfo *pTransferInfo, int nRequestID) = 0;

		///请求查询交易通知
		virtual int ReqQryKSTradingNotice(CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID) = 0;

		///用户端产品资源查询请求
		virtual int ReqQryUserProductUrl (CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID) = 0;

		///请求查询交叉汇率
		virtual int ReqQryCrossRate(CKSQryCrossRateField *pQryCrossRate, int nRequestID) = 0;

		///组合拆分单录入
		virtual int ReqMatchOrderInsert(CKSMatchOrderInsertField *pMatchOrderInsert, int nRequestID) = 0;

		///组合拆分单查询请求
		virtual int ReqQryMatchOrder(CKSQryMatchOrderField *pQryMatchOrder, int nRequestID) = 0;

		///最大组合拆分单量查询请求
		virtual int ReqQryMaxMatchOrderVolume(CKSQryMaxMatchOrderVolumeField *pQryMaxMatchOrderVolume, int nRequestID) = 0;

	protected:
		~CKSVocApi(){};
	};

}	// end of namespace KingstarAPI
#endif