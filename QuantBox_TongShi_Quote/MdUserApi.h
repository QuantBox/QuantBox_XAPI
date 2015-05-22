#pragma once
#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif
#include "../include/ApiStruct.h"
#include <stdlib.h>
#include "Stockdrv.h"
#include "../include/QueueHeader.h"


#ifdef _WIN64
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>

using namespace std;

typedef int  (WINAPI *Stock_Init_PROC)(HWND hWnd, UINT Msg, int nWorkMode);
typedef int  (WINAPI *Stock_Quit_PROC)(HWND hWnd);
typedef BOOL(WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT, DWORD);

class CMsgQueue;
class CDialogStockDrv;

class CMdUserApi
{
	enum RequestType
	{
		E_Init,
		E_ReqUserLoginField,
	};

public:
	LRESULT _OnMsg(WPARAM wParam, LPARAM lParam);

	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	

	void InitDriver(HWND hWnd, UINT Msg);
	void QuitDriver();
	//void Subscribe(const string& szInstrumentIDs, const string& szExchageID);
	//void Unsubscribe(const string& szInstrumentIDs, const string& szExchageID);

	//void SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
	//void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID);
private:
	bool FilterExchangeInstrument(WORD wMarket, int instrument);

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
	
	int _Init();
	//登录请求
	//void ReqUserLogin();
	//int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchageID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID);

	//virtual void OnFrontConnected();
	//virtual void OnFrontDisconnected(int nReason);
	//virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnDepthMarketData(RCV_REPORT_STRUCTEx *pDepthMarketData, int index, int Count);
	void OnRspQryInstrument(DepthMarketDataNField* pField, RCV_REPORT_STRUCTEx *pDepthMarketData, int index, int Count);


	//virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	//检查是否出错
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//将出错消息送到消息队列
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不送出错消息

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增

	set<string>					m_setInstrumentIDs;		//正在订阅的合约
	set<string>					m_setQuoteInstrumentIDs;		//正在订阅的合约
	//CThostFtdcMdApi*			m_pApi;					//行情API

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	//CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	volatile bool						m_bRunning;
	mutex								m_mtx;
	mutex								m_mtx_del;
	//condition_variable					m_cv;
	thread*								m_hThread;

	HWND						m_hWnd;
	void*						m_hModule;
	Stock_Init_PROC				m_pfnStock_Init;
	Stock_Quit_PROC				m_pfnStock_Quit;
	ChangeWindowMessageFilter_PROC m_pfnChangeWindowMessageFilter;

	set<string>					m_setInstrumentIDsReceived;		//正在订阅的合约
	int							m_nInited;

	CDialogStockDrv*			m_pDlg;
};

