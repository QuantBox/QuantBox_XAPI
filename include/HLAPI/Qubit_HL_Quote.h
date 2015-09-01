// Qubit_HL_Quote.h : Qubit_HL_Quote DLL 的主头文件
//

#pragma once



// CQubit_HL_QuoteApp
// 有关此类实现的信息，请参阅 Qubit_HL_Quote.cpp
//

#pragma once

#include "../include/ApiStruct.h"
#include "../include/HLAPI/HLQuoteApi.h"


#pragma comment(lib, "../include/HLAPI/HLQuoteApi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>

using namespace std;

class CMsgQueue;

class CMdUserApi :
	public HLQuoteSpi
{
	enum RequestType
	{
		E_Init,
		E_ReqUserLoginField,
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	//void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	//void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	//void SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
	//void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();
	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID);

	///当客户端与交易后台建立起通信连接，客户端需要进行登录。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用，客户端不需要处理，API会自动重连。
	virtual void OnFrontDisconnected(int nReason);

	///错误应答
	//@cIsLast 包标志 API_PACKET_FIRST | API_PACKET_LAST 下同
	virtual void OnRspError(CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast);

	///用户登录应答
	virtual void OnRspUserLogin(HLApiRspUserLoginData *pRspUserLogin, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast);

	///用户退出应答
	//virtual void OnRspUserLogout(void *pRspUserLogout, void *pRspInfo, int nRequestID, char cIsLast) {};

	///市场索引请求应答
	virtual void OnRspCodeTable(HLApiRspCodeTable *pRspCodeTable, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast);

	///行情订阅请求应答
	virtual void OnRspMarketData(HLApiRspMarketData *pRspMarketData, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast);

	//订阅行情变化通知
	virtual void OnRtnMarketData(HLApiMarketData *pMarketData);

	//全市场行情变化通知(订阅股票代码为空时，为全市场行情推送)
	virtual void OnRtnMarketData(HLApiMarketData *pMarketData, int nSize);
	virtual void OnRtnPushStkData(HLApiMarketData *pMarketData, int nSize);

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增

	set<string>					m_setInstrumentIDs;		//正在订阅的合约
	set<string>					m_setQuoteInstrumentIDs;		//正在订阅的合约
	HLQuoteApi*					m_pApi;					//行情API

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	CMsgQueue*					m_remoteQueue;
};


