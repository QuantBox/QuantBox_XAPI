#pragma once

#include "../include/XSpeed_Stock/DFITCSECMdApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/XSpeed_Stock/win64/DFITCSECMdApi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/XSpeed_Stock/win32/DFITCSECMdApi.lib")
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

class CMsgQueue;

class CMdUserApi :
	public DFITCSECMdSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init,

		E_StockUserLoginField,
		E_SOPUserLoginField,
		E_FASLUserLoginField,

		E_ReqStockQuotQryField,
		E_ReqSOPQuotQryField,
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	void SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
	void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);

	void ReqQryInstrument(const string& szInstrumentId, const string& szExchange);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();

	void ReqStockUserLogin();
	int _ReqStockUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void ReqStockAvailableQuotQry(const string& szInstrumentId, const string& szExchange);
	int _ReqStockAvailableQuotQry(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);


	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID);


	//登录请求
	//void ReqSOPUserLogin();
	//void ReqFASLUserLogin();

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspError(struct DFITCSECRspInfoField *pRspInfo);

	virtual void OnRspStockUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo);
	//virtual void OnRspSOPUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo);
	//virtual void OnRspFASLUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo);

	virtual void OnRspStockSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnRspStockUnSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnStockMarketData(struct DFITCStockDepthMarketDataField *pMarketDataField);

	//virtual void OnRspSOPSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	//virtual void OnRspSOPUnSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	//virtual void OnSOPMarketData(struct DFITCSOPDepthMarketDataField *pMarketDataField);

	//virtual void OnRspFASLSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo);
	//virtual void OnRspFASLUnSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo);
	//virtual void OnFASLMarketData(struct DFITCDepthMarketDataField *pMarketDataField);

	//virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	virtual void OnRspStockAvailableQuot(struct DFITCRspQuotQryField * pAvailableQuotInfo, struct DFITCSECRspInfoField * pRspInfo, bool flag);
	//virtual void OnRspSopAvailableQuot(struct DFITCRspQuotQryField * pAvailableQuotInfo, struct DFITCSECRspInfoField * pRspInfo, bool flag);

	//检查是否出错
	bool IsErrorRspInfo_Output(struct DFITCSECRspInfoField *pRspInfo);//将出错消息送到消息队列
	bool IsErrorRspInfo(struct DFITCSECRspInfoField *pRspInfo);//不送出错消息

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<long>				m_lRequestID;			//请求ID，每次请求前自增

	map<string, set<string> >	m_mapInstrumentIDs;		//正在订阅的合约

	DFITCSECMdApi*				m_pApi;					//行情API
	
	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;
	
	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;
};

