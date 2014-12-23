#pragma once

#include "../include/LTS/SecurityFtdcTraderApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/LTS/win64/securitytraderapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/LTS/win32/securitytraderapi.lib")
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
	public CSecurityFtdcTraderSpi
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
		E_QryTradingAccountField,
		E_QryInvestorPositionField,
		E_QryInvestorPositionDetailField,
		E_QryInstrumentCommissionRateField,
		E_QryInstrumentMarginRateField,
		E_QryDepthMarketDataField,
		E_QrySettlementInfoField,
		E_QryOrderField,
		E_QryTradeField,
	};

	//请求数据包结构体
	struct SRequest
	{
		RequestType type;
		union{
			//CSecurityFtdcReqAuthenticateField				ReqAuthenticateField;
			CSecurityFtdcReqUserLoginField					ReqUserLoginField;
			//CSecurityFtdcSettlementInfoConfirmField		SettlementInfoConfirmField;
			CSecurityFtdcQryDepthMarketDataField			QryDepthMarketDataField;
			CSecurityFtdcQryInstrumentField				QryInstrumentField;
			CSecurityFtdcQryInstrumentCommissionRateField	QryInstrumentCommissionRateField;
			//CSecurityFtdcQryInstrumentMarginRateField		QryInstrumentMarginRateField;
			CSecurityFtdcQryInvestorPositionField			QryInvestorPositionField;
			CSecurityFtdcQryInvestorPositionDetailField    QryInvestorPositionDetailField;
			CSecurityFtdcQryTradingAccountField			QryTradingAccountField;
			CSecurityFtdcInputOrderField					InputOrderField;
			CSecurityFtdcInputOrderActionField				InputOrderActionField;
			//CSecurityFtdcInputQuoteField					InputQuoteField;
			//CSecurityFtdcInputQuoteActionField				InputQuoteActionField;
			//CSecurityFtdcQrySettlementInfoField			QrySettlementInfoField;
			CSecurityFtdcQryOrderField						QryOrderField;
			CSecurityFtdcQryTradeField						QryTradeField;
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

	int ReqOrderAction(const string& szId);
	int ReqOrderAction(CSecurityFtdcOrderField *pOrder);

	int ReqQuoteInsert(
		int QuoteRef,
		OrderField* pOrderAsk,
		OrderField* pOrderBid);

	//int ReqQuoteAction(CSecurityFtdcQuoteField *pQuote);
	//int ReqQuoteAction(const string& szId);

	void ReqQryTradingAccount();
	void ReqQryInvestorPosition(const string& szInstrumentId, const string& szExchange);
	void ReqQryInvestorPositionDetail(const string& szInstrumentId);
	void ReqQryInstrument(const string& szInstrumentId, const string& szExchange);
	void ReqQryInstrumentCommissionRate(const string& szInstrumentId);
	void ReqQryInstrumentMarginRate(const string& szInstrumentId, TSecurityFtdcHedgeFlagType HedgeFlag = SECURITY_FTDC_HF_Speculation);
	void ReqQryDepthMarketData(const string& szInstrumentId);
	void ReqQrySettlementInfo(const string& szTradingDay);

	void ReqQryOrder();
	void ReqQryTrade();

private:
	void OnOrder(CSecurityFtdcOrderField *pOrder, bool bFromQry);
	void OnTrade(CSecurityFtdcTradeField *pTrade, bool bFromQry);

	void OnTrade(TradeField *pTrade, bool bFromQry);

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

	//void ReqAuthenticate();
	void ReqUserLogin();
	//void ReqSettlementInfoConfirm();

	//检查是否出错
	bool IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo);//不输出信息

	//连接
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);

	//认证
	//virtual void OnRspAuthenticate(CSecurityFtdcRspAuthenticateField *pRspAuthenticateField, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspSettlementInfoConfirm(CSecurityFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//下单
	virtual void OnRspOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo);

	//撤单
	virtual void OnRspOrderAction(CSecurityFtdcInputOrderActionField *pInputOrderAction, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderAction(CSecurityFtdcOrderActionField *pOrderAction, CSecurityFtdcRspInfoField *pRspInfo);

	//报单回报
	virtual void OnRspQryOrder(CSecurityFtdcOrderField *pOrder, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CSecurityFtdcOrderField *pOrder);

	//成交回报
	virtual void OnRspQryTrade(CSecurityFtdcTradeField *pTrade, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnTrade(CSecurityFtdcTradeField *pTrade);

	//报价录入
	//virtual void OnRspQuoteInsert(CSecurityFtdcInputQuoteField *pInputQuote, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnQuoteInsert(CSecurityFtdcInputQuoteField *pInputQuote, CSecurityFtdcRspInfoField *pRspInfo);
	//virtual void OnRtnQuote(CSecurityFtdcQuoteField *pQuote);

	//报价撤单
	//virtual void OnRspQuoteAction(CSecurityFtdcInputQuoteActionField *pInputQuoteAction, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnQuoteAction(CSecurityFtdcQuoteActionField *pQuoteAction, CSecurityFtdcRspInfoField *pRspInfo);

	//仓位
	virtual void OnRspQryInvestorPosition(CSecurityFtdcInvestorPositionField *pInvestorPosition, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInvestorPositionDetail(CSecurityFtdcInvestorPositionDetailField *pInvestorPositionDetail, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInvestorPositionCombineDetail(CSecurityFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	//资金
	virtual void OnRspQryTradingAccount(CSecurityFtdcTradingAccountField *pTradingAccount, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//合约、手续费
	virtual void OnRspQryInstrument(CSecurityFtdcInstrumentField *pInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInstrumentMarginRate(CSecurityFtdcInstrumentMarginRateField *pInstrumentMarginRate, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentCommissionRate(CSecurityFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//查询行情响应
	virtual void OnRspQryDepthMarketData(CSecurityFtdcDepthMarketDataField *pDepthMarketData, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者结算结果响应
	//virtual void OnRspQrySettlementInfo(CSecurityFtdcSettlementInfoField *pSettlementInfo, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//其它
	virtual void OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnInstrumentStatus(CSecurityFtdcInstrumentStatusField *pInstrumentStatus);

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	CSecurityFtdcRspUserLoginField m_RspUserLogin;			//返回的登录成功响应，目前利用此内成员进行报单所属区分
	OrderIDType					m_orderInsert_Id;

	mutex						m_csOrderRef;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	CSecurityFtdcTraderApi*		m_pApi;					//交易API
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
	unordered_map<string, CSecurityFtdcOrderField*>		m_id_api_order;
	unordered_map<string, string>					m_sysId_orderId;

	//hash_map<string, QuoteField*>				m_id_platform_quote;
	//hash_map<string, CSecurityFtdcQuoteField*>		m_id_api_quote;
	//hash_map<string, string>					m_sysId_quoteId;

	unordered_map<string, PositionField*>			m_id_platform_position;
};

