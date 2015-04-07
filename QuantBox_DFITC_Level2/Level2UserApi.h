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

class CMsgQueue;

class CLevel2UserApi :public DFITCL2Spi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init,
		E_UserLoginField,
	};

public:
	CLevel2UserApi(void);
	virtual ~CLevel2UserApi(void);

	void Register(void* pCallback, void* pClass);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	void SubscribeAll();
	void UnsubscribeAll();

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();
	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);


	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void Unsubscribe(const set<string>& instrumentIDs, const string& szExchageID);

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
	
	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;

	int							m_nSleep;
	
	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;
};

