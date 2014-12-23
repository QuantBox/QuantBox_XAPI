#pragma once

#include "../include/Femas/USTPFtdcTraderApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/Femas/win64/USTPtraderapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/Femas/win32/USTPtraderapi.lib")
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
	public CUstpFtdcTraderSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_ReqUserLoginField,
		E_QryUserInvestorField,
		E_QryInstrumentField,
		E_InputOrderField,
		//E_InputOrderActionField,
		E_InputQuoteField,
		//E_InputQuoteActionField,
		E_ParkedOrderField,
		E_QryInvestorAccountField,
		E_QryInvestorPositionField,
		E_QryInvestorPositionDetailField,
		E_QryInvestorFeeField,
		E_QryInvestorMarginField,
		E_QryDepthMarketDataField,
		E_QrySettlementInfoField,
		E_QryOrderField,
		E_QryTradeField,
		E_OrderActionField,
		E_QuoteActionField,
	};

	//请求数据包结构体
	struct SRequest
	{
		RequestType type;
		union{
			CUstpFtdcReqUserLoginField					ReqUserLoginField;
			CUstpFtdcQryUserInvestorField				QryUserInvestorField;
			CUstpFtdcQryInstrumentField					QryInstrumentField;
			CUstpFtdcQryInvestorPositionField			QryInvestorPositionField;
			CUstpFtdcInputOrderField					InputOrderField;
			CUstpFtdcInputQuoteField					InputQuoteField;
			CUstpFtdcQryOrderField						QryOrderField;
			CUstpFtdcQryTradeField						QryTradeField;
			CUstpFtdcQryInvestorAccountField			QryInvestorAccountField;
			CUstpFtdcQryInvestorFeeField				QryInvestorFeeField;
			CUstpFtdcQryInvestorMarginField				QryInvestorMarginField;
			CUstpFtdcOrderActionField					OrderActionField;
			CUstpFtdcQuoteActionField					QuoteActionField;
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
		OrderField* pOrder1);

	//int ReqParkedOrderInsert(int OrderRef,
	//	OrderField* pOrder1,
	//	OrderField* pOrder2);

	int ReqOrderAction(const string& szId);
	int ReqOrderAction(CUstpFtdcOrderField *pOrder);

	char* ReqQuoteInsert(
		int QuoteRef,
		QuoteField* pQuote);

	int ReqQuoteAction(CUstpFtdcRtnQuoteField *pQuote);
	int ReqQuoteAction(const string& szId);

	void ReqQryInvestorAccount();
	void ReqQryInvestorPosition(const string& szInstrumentId);
	void ReqQryInstrument(const string& szInstrumentId, const string& szExchange);
	void ReqQryInvestorFee(const string& szInstrumentId);
	//void ReqQryInvestorMargin(const string& szInstrumentId, TThostFtdcHedgeFlagType HedgeFlag = THOST_FTDC_HF_Speculation);

	void ReqQryOrder();
	void ReqQryTrade();
	void ReqQryQuote();

private:
	void OnOrder(CUstpFtdcOrderField *pOrder);
	void OnTrade(CUstpFtdcTradeField *pTrade);
	void OnQuote(CUstpFtdcRtnQuoteField *pQuote);

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

	void ReqUserLogin();
	void ReqQryUserInvestor();

	//检查是否出错
	bool IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo);//不输出信息

	//连接
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);

	//认证
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//下单
	virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);

	//撤单
	virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);

	//报单回报
	virtual void OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder);

	//成交回报
	virtual void OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade);

	//报价录入
	virtual void OnRspQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo);
	virtual void OnRtnQuote(CUstpFtdcRtnQuoteField *pRtnQuote);

	//报价撤单
	virtual void OnRspQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo);

	//仓位
	virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	//资金
	virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//合约、手续费
	virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//其它
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus);

	//询价回报
	virtual void OnRtnForQuote(CUstpFtdcReqForQuoteField *pReqForQuote);

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	RspUserLoginField			m_RspUserLogin__;
	CUstpFtdcRspUserLoginField	m_RspUserLogin;			//返回的登录成功响应，目前利用此内成员进行报单所属区分
	CUstpFtdcRspUserInvestorField m_RspUserInvestor;

	OrderIDType					m_orderInsert_Id;

	mutex						m_csOrderRef;
	long long					m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	CUstpFtdcTraderApi*			m_pApi;					//交易API
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
	unordered_map<string, CUstpFtdcOrderField*>		m_id_api_order;
	//unordered_map<string, string>					m_sysId_orderId;

	unordered_map<string, QuoteField*>				m_id_platform_quote;
	unordered_map<string, CUstpFtdcRtnQuoteField*>		m_id_api_quote;
	//unordered_map<string, string>					m_sysId_quoteId;
};

