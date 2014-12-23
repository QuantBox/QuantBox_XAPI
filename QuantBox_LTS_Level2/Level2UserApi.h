#pragma once

#include "../include/LTS_L2/SecurityFtdcL2MDUserApi.h"
#include "../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../include/LTS_L2/win64/L2mduserapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/LTS_L2/win32/L2mduserapi.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>

using namespace std;
_USING_LTS_NS_

class CLevel2UserApi :public CSecurityFtdcL2MDUserSpi
{
public:
	CLevel2UserApi(void);
	virtual ~CLevel2UserApi(void);

	void Register(void* pMsgQueue);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void SubscribeL2MarketData(const string& szInstrumentIDs, const string& szExchageID);
	void UnSubscribeL2MarketData(const string& szInstrumentIDs, const string& szExchageID);
	void SubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID);
	void UnSubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID);

	void SubscribeL2OrderAndTrade();
	void UnSubscribeL2OrderAndTrade();

private:
	//订阅行情
	void SubscribeL2MarketData(const set<string>& instrumentIDs, const string& szExchageID);
	void SubscribeL2Index(const set<string>& instrumentIDs, const string& szExchageID);

	//登录请求
	void ReqUserLogin();

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(CSecurityFtdcUserLoginField *pUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///Level2行情
	virtual void OnRspSubL2MarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubL2MarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnL2MarketData(CSecurityFtdcL2MarketDataField *pL2MarketData);
	///Level2指数行情
	virtual void OnRspSubL2Index(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubL2Index(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnL2Index(CSecurityFtdcL2IndexField *pL2Index);

private:
	//检查是否出错
	bool IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//将出错消息送到消息队列
	bool IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo);//不送出错消息

private:
	mutex						m_csMapSecurityIDs;
	mutex						m_csMapIndexIDs;

	int							m_nRequestID;			//请求ID，每次请求前自增

	map<string,set<string> >	m_mapSecurityIDs;		//正在订阅的合约
	map<string,set<string> >	m_mapIndexIDs;			//正在订阅的合约

	CSecurityFtdcL2MDUserApi*	m_pApi;					//行情API
	void*						m_msgQueue;				//消息队列指针

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
};

