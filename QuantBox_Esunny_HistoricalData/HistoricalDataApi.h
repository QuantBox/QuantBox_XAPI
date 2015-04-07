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

class CMsgQueue;

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

public:
	CHistoricalDataApi(void);
	virtual ~CHistoricalDataApi(void);

	void Register(void* pCallback, void* pClass);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	int ReqQryHistoricalTicks(HistoricalDataRequestField* request);
	int ReqQryHistoricalBars(HistoricalDataRequestField* request);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();

	int ReqQryHistoricalTicks_(HistoricalDataRequestField* request);
	int ReqQryHistoricalBars_(HistoricalDataRequestField* request);

	int ReqQryHistoricalTicks_Check();
	int RtnEmptyRspQryHistoricalTicks();
	
	virtual int __cdecl OnRspLogin(int err, const char *errtext);
	virtual int __cdecl OnChannelLost(int err, const char *errtext);
	virtual int __cdecl OnStkQuot(struct STKDATA *pData);
	virtual int __cdecl OnRspHistoryQuot(struct STKHISDATA *pHisData);
	virtual int __cdecl OnRspTraceData(struct STKTRACEDATA *pTraceData);
	virtual int __cdecl OnRspMarketInfo(struct MarketInfo *pMarketInfo, int bLast);

private:
	atomic<long>				m_lRequestID;			//请求ID,得保持自增

	IEsunnyQuotClient*			m_pApi;					//交易API
	
	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;

	int							m_nSleep;

	HistoricalDataRequestField	m_RequestTick;
	HistoricalDataRequestField	m_RequestBar;

	int							m_nHdRequestId;	//
	time_t						m_timer_1;
	time_t						m_timer_2;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;
};

