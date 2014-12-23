#pragma once

#include "../include/XSpeed/DFITCMdApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/XSpeed/win64/DFITCMdApi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/XSpeed/win32/DFITCMdApi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <list>
#include <map>
#include <hash_map>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;
using namespace DFITCXSPEEDMDAPI;

class CMdUserApi :
	public DFITCMdSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init,
		E_UserLoginField,
	};

	//请求数据包结构体
	struct SRequest
	{
		RequestType type;
		void* pBuf;
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pMsgQueue);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	void SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
	void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);

private:
	//数据包发送线程
	//数据包发送线程
	static void ProcessThread(CMdUserApi* lpParam)
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
	void AddRequestMapBuf(int nRequestID, SRequest* pRequest);
	//添加到将发送包队列
	void AddToSendQueue(SRequest * pRequest);

	int ReqInit();


	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID);
	//登录请求
	void ReqUserLogin();

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(struct DFITCUserLoginInfoRtnField * pRspUserLogin, struct DFITCErrorRtnField * pRspInfo);
	virtual void OnRspError(struct DFITCErrorRtnField *pRspInfo);

	virtual void OnRspSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo);
	virtual void OnRspUnSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo);
	virtual void OnMarketData(struct DFITCDepthMarketDataField *pMarketDataField);

	//virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	//检查是否出错
	bool IsErrorRspInfo_Output(struct DFITCErrorRtnField *pRspInfo);//将出错消息送到消息队列
	bool IsErrorRspInfo(struct DFITCErrorRtnField *pRspInfo);//不送出错消息

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<long>				m_lRequestID;			//请求ID，每次请求前自增

	set<string>					m_setInstrumentIDs;		//正在订阅的合约
	set<string>					m_setQuoteInstrumentIDs;		//正在订阅的合约
	DFITCMdApi*					m_pApi;					//行情API
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
	map<int, SRequest*>			m_reqMap;
};

