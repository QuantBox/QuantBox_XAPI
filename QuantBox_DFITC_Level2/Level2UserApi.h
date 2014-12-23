#pragma once

#include "../include/DFITC_L2/DFITCL2Api.h"

#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/DFITC_L2/win32/level2Api.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <list>
#include <thread>

using namespace std;
using namespace DFITC_L2;

class CLevel2UserApi :public DFITCL2Spi
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
	CLevel2UserApi(void);
	virtual ~CLevel2UserApi(void);

	void Register(void* pMsgQueue);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	void SubscribeAll();
	void UnsubscribeAll();

private:
	//数据包发送线程
	//数据包发送线程
	static void ProcessThread(CLevel2UserApi* lpParam)
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

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void Unsubscribe(const set<string>& instrumentIDs, const string& szExchageID);

	//登录请求
	void ReqUserLogin();
	int ReqInit();

	virtual void OnConnected();
	virtual void OnDisconnected(int nReason);
	virtual void OnRspUserLogin(struct ErrorRtnField * pErrorField);

	virtual void OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField);
	virtual void OnRspUnSubscribeMarketData(struct ErrorRtnField * pErrorField);
	virtual void OnRspSubscribeAll(struct ErrorRtnField * pErrorField);
	virtual void OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField);

	virtual void OnBestAndDeep(MDBestAndDeep * const pQuote);
	virtual void OnArbi(MDBestAndDeep * const pQuote);
	virtual void OnTenEntrust(MDTenEntrust * const pQuote);
	virtual void OnRealtime(MDRealTimePrice * const pQuote);
	virtual void OnOrderStatistic(MDOrderStatistic * const pQuote);
	virtual void OnMarchPrice(MDMarchPriceQty * const pQuote);

	virtual void OnHeartBeatLost() { }
	//检查是否出错
	bool IsErrorRspInfo_Output(struct ErrorRtnField * pErrorField);//将出错消息送到消息队列
	bool IsErrorRspInfo(struct ErrorRtnField * pErrorField); //不输出信息

private:
	mutex						m_csMapInstrumentIDs;

	atomic<long>				m_lRequestID;			//请求ID，每次请求前自增

	set<string>					m_setInstrumentIDs;

	DFITCL2Api*					m_pApi;					//行情API
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
	map<int, SRequest*>			m_reqMap;				//已发送请求池
};

