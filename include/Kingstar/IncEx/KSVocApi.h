/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSVocApi.h
///@brief 定义了客户端客户定制接口
/////////////////////////////////////////////////////////////////////////

#ifndef __KSVOCAPI_H_INCLUDED_
#define __KSVOCAPI_H_INCLUDED_

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

		///最大组合拆分单量查询请求响应
		virtual void OnRspQryMaxCombActionVolume(CKSMaxCombActionVolumeField *pMaxCombActionVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///交易通知
		virtual void OnRtnKSTradingNotice(CKSTradingNoticeField *pTradingNoticeInfo) {};

		///请求查询期权合约手续费响应
		virtual void OnRspQryKSOptionInstrCommRate(CKSOptionInstrCommRateField *pOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///请求查询合约保证金率响应
		virtual void OnRspQryKSInstrumentMarginRate(CKSInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///客户每日出金额度申请响应
		virtual void OnRspFundOutCreditApply(CKSInputFundOutCreditApplyField *pFundOutCreditApply, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///客户每日出金额度查询响应
		virtual void OnRspQryFundOutCredit(CKSRspQryFundOutCreditField *pRspQryFundOutCredit, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///客户每日出金额度申请查询响应
		virtual void OnRspQryFundOutCreditApply(CKSRspQryFundOutCreditApplyField *pRspQryFundOutCreditApply, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///大额出金预约（取消）申请响应
		virtual void OnRspLargeFundOutApply(CKSLargeFundOutApplyField *pLargeFundOutApply, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

		///大额出金预约查询响应
		virtual void OnRspQryLargeFundOutApply(CKSRspLargeFundOutApplyField *pRspLargeFundOutApply, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
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

		///最大组合拆分单量查询请求
		virtual int ReqQryMaxCombActionVolume(CKSQryMaxCombActionVolumeField *pQryMaxCombActionVolume, int nRequestID) = 0;

		///请求查询期权合约手续费
		virtual int ReqQryKSOptionInstrCommRate(CKSQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID) = 0;

		///请求查询合约保证金率
		virtual int ReqQryKSInstrumentMarginRate(CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID) = 0;

		///客户每日出金额度申请
		virtual int ReqFundOutCreditApply(CKSInputFundOutCreditApplyField *pFundOutCreditApply, int nRequestID) = 0;

		///客户每日出金额度查询
		virtual int ReqQryFundOutCredit(CKSQryFundOutCreditField *pQryFundOutCredit, int nRequestID) = 0;

		///客户每日出金额度申请查询
		virtual int ReqQryFundOutCreditApply(CKSQryFundOutCreditApplyField *pQryFundOutCreditApply, int nRequestID) = 0;

		///大额出金预约（取消）申请
		virtual int ReqLargeFundOutApply(CKSLargeFundOutApplyField *pLargeFundOutApply, int nRequestID) = 0;

		///大额出金预约查询
		virtual int ReqQryLargeFundOutApply(CKSQryLargeFundOutApplyField *pQryLargeFundOutApply, int nRequestID) = 0;

	protected:
		~CKSVocApi(){};
	};

}	// end of namespace KingstarAPI
#endif