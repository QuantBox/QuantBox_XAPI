#pragma once

#include "../include/ApiStruct.h"
// 需要将zmq和czmq的目录在Additional Include Directories中添加
#include "zmq.h"
#include "czmq.h"

#ifdef _WIN64
#pragma comment(lib, "../include/CTP/win64/thostmduserapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/ZeroMQ/x86/czmq.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <map>

using namespace std;

class CMsgQueue;

class CMdUserApi
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

	void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	//void SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
	//void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
private:
	void StartThread();
	void StopThread();

	static void ProcessThread(CMdUserApi* lpParam)
	{
		if (lpParam)
			lpParam->RunInThread();
	}
	void RunInThread();

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	//int _Init();
	////登录请求
	//void ReqUserLogin();
	//int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	bool Contains(const string& szInstrumentID, const string& szExchageID);

	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	//void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID);

	//virtual void OnFrontConnected();
	//virtual void OnFrontDisconnected(int nReason);
	//virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	//virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	////检查是否出错
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//将出错消息送到消息队列
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不送出错消息

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增

	map<string, set<string> >	m_mapInstrumentIDs;		//正在订阅的合约
	map<string, set<string> >	m_mapQuoteInstrumentIDs;		//正在订阅的合约
	//CThostFtdcMdApi*			m_pApi;					//行情API
	void*						m_pContext;

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	//CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	//zctx_t*						m_ctx;

	volatile bool						m_bRunning;
	mutex								m_mtx;
	mutex								m_mtx_del;
	//condition_variable					m_cv;
	thread*								m_hThread;

	zctx_t*		m_ctx;
	void*		m_pubisher;
};

