#pragma once

#include "../include/Esunny_HistoricalData/EsunnyQuot.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/Esunny_HistoricalData/win32/EsunnyQuot.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <hash_map>
#include <time.h>

using namespace std;

class CHistoricalDataApi :
	public IEsunnyQuotNotify
{
	//请求数据包类型
	enum RequestType
	{
		E_Init,
		E_ReqQryHistoricalTicks,
		E_ReqQryHistoricalBars,
		E_ReqQryHistoricalTicks_Check,
	};

	//请求数据包结构体
	struct SRequest
	{
		RequestType type;
		void* pBuf;
	};

public:
	CHistoricalDataApi(void);
	virtual ~CHistoricalDataApi(void);

	void Register(void* pMsgQueue);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	int ReqQryHistoricalTicks(HistoricalDataRequestField* request);
	int ReqQryHistoricalBars(HistoricalDataRequestField* request);

private:
	int ReqQryHistoricalTicks_(HistoricalDataRequestField* request, int lRequest);
	int ReqQryHistoricalBars_(HistoricalDataRequestField* request, int lRequest);

	int ReqQryHistoricalTicks_Check();
	int RtnEmptyRspQryHistoricalTicks();

	//数据包发送线程
	static void ProcessThread(CHistoricalDataApi* lpParam)
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

	int ReqInit();
	
	virtual int __cdecl OnRspLogin(int err, const char *errtext);
	virtual int __cdecl OnChannelLost(int err, const char *errtext);
	virtual int __cdecl OnStkQuot(struct STKDATA *pData);
	virtual int __cdecl OnRspHistoryQuot(struct STKHISDATA *pHisData);
	virtual int __cdecl OnRspTraceData(struct STKTRACEDATA *pTraceData);
	virtual int __cdecl OnRspMarketInfo(struct MarketInfo *pMarketInfo, int bLast);

private:
	atomic<long>				m_lRequestID;			//请求ID,得保持自增

	IEsunnyQuotClient*			m_pApi;					//交易API
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

	HistoricalDataRequestField	m_RequestTick;
	HistoricalDataRequestField	m_RequestBar;

	int							m_nHdRequestId;	//
	time_t						m_timer_1;
	time_t						m_timer_2;
};

