#pragma once

#include "../include/Kingstar_Stock/SPXAPI.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/Kingstar_Stock/win64/thosttraderapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/Kingstar_Stock/win32/SPXAPI.lib")
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

struct OrderFieldEx
{
	OrderField Field;
	TCustNoType         cust_no;                    ///< 客户号
	TMarketCodeType     market_code;                ///< 市场代码（按批撤必须填入）
	THolderNoType       holder_acc_no;              ///< 股东帐号
	TBatchNoType        batch_no;                   ///< 批号
	TContractNoType     order_no;                   ///< 流水号
};

class CMsgQueue;

class CTraderApi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init,
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

public:
	static CTraderApi * pThis;

	CTraderApi(void);
	virtual ~CTraderApi(void);

	void Register(void* pCallback, void* pClass);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	int ReqOrderInsert(
		OrderField* pOrder,
		int count,
		OrderIDType* pInOut);

	//char* ReqParkedOrderInsert(int OrderRef,
	//	OrderField* pOrder1,
	//	OrderField* pOrder2);

	int ReqOrderAction(OrderIDType* szId, int count, OrderIDType* pOutput);
	int ReqOrderAction(OrderFieldEx *pOrder, int count, OrderIDType* pOutput);

	//char* ReqQuoteInsert(
	//	int QuoteRef,
	//	QuoteField* pQuote);

	//int ReqQuoteAction(CThostFtdcQuoteField *pQuote);
	////int ReqQuoteAction(const string& szId);

	//void ReqQryTradingAccount();
	//void ReqQryInvestorPosition(const string& szInstrumentId, const string& szExchange);
	//void ReqQryInvestorPositionDetail(const string& szInstrumentId);
	void ReqQryInstrument(const string& szInstrumentId, const string& szExchange);
	//void ReqQryInstrumentCommissionRate(const string& szInstrumentId);
	////void ReqQryInstrumentMarginRate(const string& szInstrumentId,TThostFtdcHedgeFlagType HedgeFlag = THOST_FTDC_HF_Speculation);
	//void ReqQryDepthMarketData(const string& szInstrumentId);
	//void ReqQrySettlementInfo(const string& szTradingDay);

	void ReqQryOrder(TCustNoType cust_no, TSecCodeType sec_code);
	void ReqQryTrade(TCustNoType cust_no, TSecCodeType sec_code);
	//void ReqQryQuote();

	
private:
	static void __stdcall OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData);
	void _OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData);

	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	virtual void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	int _Init();

	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);


	void OnPST16203PushData(PST16203PushData pEtxPushData);
	void OnPST16204PushData(PST16204PushData pEtxPushData);

	

	//检查是否出错
	bool IsErrorRspInfo(STRspMsg *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(STRspMsg *pRspInfo);//不输出信息

	////连接
	//virtual void OnFrontConnected();
	//virtual void OnFrontDisconnected(int nReason);

	////认证
	//virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	////下单
	//virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	////撤单
	//virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	////报单回报
	//virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	////成交回报
	//virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	////报价录入
	//virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo);
	//virtual void OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	////报价撤单
	//virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo);

	////仓位
	//virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	////资金
	//virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	////合约、手续费
	//virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	////查询行情响应
	//virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	////请求查询投资者结算结果响应
	//virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	////其它
	//virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	//CThostFtdcRspUserLoginField m_RspUserLogin;			//返回的登录成功响应，目前利用此内成员进行报单所属区分
	
	OrderIDType					m_orderInsert_Id;

	mutex						m_csOrderRef;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	STRspMsg					m_err_msg;
	APIHandle					m_pApi;					//交易API


	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;

	int							m_nSleep;

	unordered_map<string, OrderFieldEx*>				m_id_platform_order;
	//unordered_map<string, STOrderInfo*>				m_id_api_order;
	//unordered_map<string, string>					m_sysId_orderId;//成交回报时使用找到原订单

	//unordered_map<string, QuoteField*>				m_id_platform_quote;
	//unordered_map<string, CThostFtdcQuoteField*>		m_id_api_quote;
	//unordered_map<string, string>					m_sysId_quoteId;

	//unordered_map<string, PositionField*>			m_id_platform_position;

	unordered_map<string, string>					m_cust_acc_no;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;		//发送消息队列指针
	CMsgQueue*					m_msgQueue_Order;		//报单消息队列指针

	UserInfoField*				m_pUserInfos;
	int							m_UserInfo_Pos;
	int							m_UserInfo_Count;

	STOrderCancel				m_temp_ordercancel;

	void*						m_pClass;
};

