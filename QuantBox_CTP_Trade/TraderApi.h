#pragma once

#include "../include/CTP/ThostFtdcTraderApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/CTP/win64/thosttraderapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/CTP/win32/thosttraderapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif


#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>

using namespace std;


class CTraderApi :
	public CThostFtdcTraderSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_ReqAuthenticateField,
		E_ReqUserLoginField,
		E_SettlementInfoConfirmField,
		E_QryInstrumentField,
		E_InputOrderField,
		E_InputOrderActionField,
		E_InputQuoteField,
		E_InputQuoteActionField,
		E_ParkedOrderField,
		E_QryTradingAccountField,
		E_QryInvestorPositionField,
		E_QryInvestorPositionDetailField,
		E_QryInstrumentCommissionRateField,
		E_QryInstrumentMarginRateField,
		E_QryDepthMarketDataField,
		E_QrySettlementInfoField,
		E_QryOrderField,
		E_QryTradeField,
		E_QryQuoteField,
	};

	//请求数据包结构体
	struct SRequest
	{
		RequestType type;
		union{
			CThostFtdcReqAuthenticateField				ReqAuthenticateField;
			CThostFtdcReqUserLoginField					ReqUserLoginField;
			CThostFtdcSettlementInfoConfirmField		SettlementInfoConfirmField;
			CThostFtdcQryDepthMarketDataField			QryDepthMarketDataField;
			CThostFtdcQryInstrumentField				QryInstrumentField;
			CThostFtdcQryInstrumentCommissionRateField	QryInstrumentCommissionRateField;
			CThostFtdcQryInstrumentMarginRateField		QryInstrumentMarginRateField;
			CThostFtdcQryInvestorPositionField			QryInvestorPositionField;
			CThostFtdcQryInvestorPositionDetailField    QryInvestorPositionDetailField;
			CThostFtdcQryTradingAccountField			QryTradingAccountField;
			CThostFtdcInputOrderField					InputOrderField;
			CThostFtdcInputOrderActionField				InputOrderActionField;
			CThostFtdcInputQuoteField					InputQuoteField;
			CThostFtdcInputQuoteActionField				InputQuoteActionField;
			CThostFtdcParkedOrderField					ParkedOrderField;
			CThostFtdcQrySettlementInfoField			QrySettlementInfoField;
			CThostFtdcQryOrderField						QryOrderField;
			CThostFtdcQryTradeField						QryTradeField;
			CThostFtdcQryQuoteField						QryQuoteField;
		};
	};

public:
	CTraderApi(void);
	virtual ~CTraderApi(void);

	void Register(void* pMsgQueue);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	char* ReqOrderInsert(
		int OrderRef,
		OrderField* pOrder1,
		OrderField* pOrder2);

	char* ReqParkedOrderInsert(int OrderRef,
		OrderField* pOrder1,
		OrderField* pOrder2);

	int ReqOrderAction(const string& szId);
	int ReqOrderAction(CThostFtdcOrderField *pOrder);

	char* ReqQuoteInsert(
		int QuoteRef,
		QuoteField* pQuote);

	int ReqQuoteAction(CThostFtdcQuoteField *pQuote);
	int ReqQuoteAction(const string& szId);

	void ReqQryTradingAccount();
	void ReqQryInvestorPosition(const string& szInstrumentId, const string& szExchange);
	void ReqQryInvestorPositionDetail(const string& szInstrumentId);
	void ReqQryInstrument(const string& szInstrumentId, const string& szExchange);
	void ReqQryInstrumentCommissionRate(const string& szInstrumentId);
	void ReqQryInstrumentMarginRate(const string& szInstrumentId,TThostFtdcHedgeFlagType HedgeFlag = THOST_FTDC_HF_Speculation);
	void ReqQryDepthMarketData(const string& szInstrumentId);
	void ReqQrySettlementInfo(const string& szTradingDay);

	void ReqQryOrder();
	void ReqQryTrade();
	void ReqQryQuote();

private:
	void OnOrder(CThostFtdcOrderField *pOrder);
	void OnTrade(CThostFtdcTradeField *pTrade);
	void OnQuote(CThostFtdcQuoteField *pQuote);

	void OnTrade(TradeField *pTrade);

	//数据包发送线程
	static void ProcessThread(CTraderApi* lpParam)
	{
		if (lpParam)
			lpParam->RunInThread();
	}
	void RunInThread();
	void StartThread();
	void StopThread();

	//指定数据包类型，生成对应数据包
	SRequest * MakeRequestBuf(RequestType type);
	//清除将发送请求包队列
	void ReleaseRequestListBuf();
	//清除已发送请求包池
	void ReleaseRequestMapBuf();
	//清除指定请求包池中指定包
	void ReleaseRequestMapBuf(int nRequestID);
	//添加到已经请求包池
	void AddRequestMapBuf(int nRequestID,SRequest* pRequest);
	//添加到将发送包队列
	void AddToSendQueue(SRequest * pRequest);

	void ReqAuthenticate();
	void ReqUserLogin();
	void ReqSettlementInfoConfirm();

	//检查是否出错
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不输出信息

	//连接
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);

	//认证
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//下单
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	//撤单
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	//报单回报
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	//成交回报
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	//报价录入
	virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	//报价撤单
	virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo);

	//仓位
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	//资金
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//合约、手续费
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//查询行情响应
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者结算结果响应
	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//其它
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	CThostFtdcRspUserLoginField m_RspUserLogin;			//返回的登录成功响应，目前利用此内成员进行报单所属区分
	
	OrderIDType					m_orderInsert_Id;

	mutex						m_csOrderRef;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	CThostFtdcTraderApi*		m_pApi;					//交易API
	void*						m_msgQueue;				//消息队列指针

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;

	int							m_nSleep;
	volatile bool				m_bRunning;
	thread*						m_hThread;

	mutex						m_csList;
	list<SRequest*>				m_reqList;				//将发送请求队列

	mutex						m_csMap;
	map<int,SRequest*>			m_reqMap;				//已发送请求池

	unordered_map<string, OrderField*>				m_id_platform_order;
	unordered_map<string, CThostFtdcOrderField*>		m_id_api_order;
	unordered_map<string, string>					m_sysId_orderId;

	unordered_map<string, QuoteField*>				m_id_platform_quote;
	unordered_map<string, CThostFtdcQuoteField*>		m_id_api_quote;
	unordered_map<string, string>					m_sysId_quoteId;

	unordered_map<string, PositionField*>			m_id_platform_position;
};

