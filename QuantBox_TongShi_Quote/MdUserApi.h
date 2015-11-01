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
#include <map>

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
	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);

	//void SubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);
	//void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);
private:
	bool FilterExchangeInstrument(WORD wMarket, string instrument);

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
	//��¼����
	//void ReqUserLogin();
	//int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//��������
	void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID);

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

	//����Ƿ����
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//��������Ϣ�͵���Ϣ����
	//bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//���ͳ�����Ϣ

private:
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<int>					m_lRequestID;			//����ID��ÿ������ǰ����

	set<string>					m_setInstrumentIDs;		//���ڶ��ĵĺ�Լ
	set<string>					m_setQuoteInstrumentIDs;		//���ڶ��ĵĺ�Լ
	//CThostFtdcMdApi*			m_pApi;					//����API

	string						m_szPath;				//���������ļ���·��
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	map<string,set<string>>		m_DictSet;				//��Լ��

	CMsgQueue*					m_msgQueue;				//��Ϣ����ָ��
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

	set<string>					m_setInstrumentIDsReceived;		//���ڶ��ĵĺ�Լ
	int							m_nInited;

	CDialogStockDrv*			m_pDlg;
};

