#include "stdafx.h"
#include "Level2UserApi.h"
#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include <mutex>
#include <vector>
#include <cstring>
#include <assert.h>

using namespace std;
using namespace DFITC_L2;

CLevel2UserApi::CLevel2UserApi(void)
{
	m_pApi = nullptr;
	m_msgQueue = nullptr;
	m_lRequestID = 0;
}

CLevel2UserApi::~CLevel2UserApi(void)
{
	Disconnect();
}

void CLevel2UserApi::StartThread()
{
	if (nullptr == m_hThread)
	{
		m_bRunning = true;
		m_hThread = new thread(ProcessThread, this);
	}
}

void CLevel2UserApi::StopThread()
{
	m_bRunning = false;
	if (m_hThread)
	{
		m_hThread->join();
		delete m_hThread;
		m_hThread = nullptr;
	}
}

void CLevel2UserApi::Register(void* pMsgQueue)
{
	m_msgQueue = pMsgQueue;
}

bool CLevel2UserApi::IsErrorRspInfo_Output(struct ErrorRtnField * pErrorField)
{
	bool bRet = ((pErrorField) && (pErrorField->ErrorID != 0));
	if (bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pErrorField->ErrorID;
		strcpy(field.ErrorMsg, pErrorField->ErrorMsg);

		XRespone(ResponeType::OnRtnError, m_msgQueue, this, true, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CLevel2UserApi::IsErrorRspInfo(struct ErrorRtnField * pErrorField)
{
	bool bRet = ((pErrorField) && (pErrorField->ErrorID != 0));

	return bRet;
}

void CLevel2UserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pApi = NEW_CONNECTOR();

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		// 停止已有线程，并清理
		StopThread();
		ReleaseRequestListBuf();
		ReleaseRequestMapBuf();

		SRequest* pRequest = MakeRequestBuf(E_Init);
		if (pRequest)
		{
			AddToSendQueue(pRequest);
		}
	}
}

CLevel2UserApi::SRequest* CLevel2UserApi::MakeRequestBuf(RequestType type)
{
	SRequest *pRequest = new SRequest;
	if (nullptr == pRequest)
		return nullptr;

	memset(pRequest, 0, sizeof(SRequest));
	pRequest->type = type;
	switch (type)
	{
	case E_Init:
		break;
	case E_UserLoginField:
		pRequest->pBuf = new DFITCUserLoginField();
		break;
	}
	return pRequest;
}

void CLevel2UserApi::ReleaseRequestListBuf()
{
	lock_guard<mutex> cl(m_csList);
	while (!m_reqList.empty())
	{
		SRequest * pRequest = m_reqList.front();
		delete pRequest->pBuf;
		delete pRequest;
		m_reqList.pop_front();
	}
}

void CLevel2UserApi::ReleaseRequestMapBuf()
{
	lock_guard<mutex> cl(m_csMap);
	for (map<int, SRequest*>::iterator it = m_reqMap.begin(); it != m_reqMap.end(); ++it)
	{
		SRequest * pRequest = it->second;
		delete pRequest->pBuf;
		delete pRequest;
	}
	m_reqMap.clear();
}

void CLevel2UserApi::ReleaseRequestMapBuf(int nRequestID)
{
	lock_guard<mutex> cl(m_csMap);
	map<int, SRequest*>::iterator it = m_reqMap.find(nRequestID);
	if (it != m_reqMap.end())
	{
		SRequest * pRequest = it->second;
		delete pRequest->pBuf;
		delete pRequest;
		m_reqMap.erase(nRequestID);
	}
}

void CLevel2UserApi::AddRequestMapBuf(int nRequestID, SRequest* pRequest)
{
	if (nullptr == pRequest)
		return;

	lock_guard<mutex> cl(m_csMap);
	map<int, SRequest*>::iterator it = m_reqMap.find(nRequestID);
	if (it != m_reqMap.end())
	{
		SRequest* p = it->second;
		if (pRequest != p)//如果实际上指的是同一内存，不再插入
		{
			delete p->pBuf;
			delete p;
			m_reqMap[nRequestID] = pRequest;
		}
	}
}

void CLevel2UserApi::AddToSendQueue(SRequest * pRequest)
{
	if (nullptr == pRequest)
		return;

	lock_guard<mutex> cl(m_csList);
	bool bFind = false;

	if (!bFind)
		m_reqList.push_back(pRequest);

	if (!m_reqList.empty())
	{
		StartThread();
	}
}


void CLevel2UserApi::RunInThread()
{
	int iRet = 0;

	while (!m_reqList.empty() && m_bRunning)
	{
		SRequest * pRequest = m_reqList.front();
		long lRequest = ++m_lRequestID;
		switch (pRequest->type)
		{
		case E_Init:
			iRet = ReqInit();
			if (iRet != 0 && m_bRunning)
				this_thread::sleep_for(chrono::milliseconds(1000 * 20));
			break;
		case E_UserLoginField:
			iRet = m_pApi->ReqUserLogin((DFITCUserLoginField*)pRequest->pBuf);
			break;
		default:
			assert(false);
			break;
		}

		if (0 == iRet)
		{
			//返回成功，填加到已发送池
			m_nSleep = 1;
			AddRequestMapBuf(lRequest, pRequest);

			lock_guard<mutex> cl(m_csList);
			m_reqList.pop_front();
		}
		else
		{
			//失败，按4的幂进行延时，但不超过1s
			m_nSleep *= 4;
			m_nSleep %= 1023;
		}
		this_thread::sleep_for(chrono::milliseconds(m_nSleep));
	}

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}

int CLevel2UserApi::ReqInit()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	//初始化连接
	int iRet = m_pApi->Connect(m_ServerInfo.Address, this, m_ServerInfo.IsUsingUdp);
	if (0 == iRet)
	{
	}
	else
	{
		RspUserLoginField field = { 0 };
		field.ErrorID = iRet;
		strcpy(field.ErrorMsg, "连接超时");

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	return iRet;
}

void CLevel2UserApi::ReqUserLogin()
{
	if (nullptr == m_pApi)
		return;

	DFITCUserLoginField request = { 0 };

	strcpy(request.accountID, m_UserInfo.UserID);
	strcpy(request.passwd, m_UserInfo.Password);

	//只有这一处用到了m_nRequestID，没有必要每次重连m_nRequestID都从0开始
	m_pApi->ReqUserLogin(&request);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::Disconnect()
{
	if (m_pApi)
	{
		DELETE_CONNECTOR(m_pApi);
		m_pApi = nullptr;

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::OnConnected()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CLevel2UserApi::OnDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, field.ErrorMsg);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::OnRspUserLogin(struct ErrorRtnField * pErrorField)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pErrorField))
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		//有可能断线了，本处是断线重连后重新订阅
		set<string> mapOld = m_setInstrumentIDs;//记下上次订阅的合约
		//Unsubscribe(mapOld);//由于已经断线了，没有必要再取消订阅
		Subscribe(mapOld,"");//订阅
	}
	else
	{
		field.ErrorID = pErrorField->ErrorID;
		strcpy(field.ErrorMsg, pErrorField->ErrorMsg);

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);
	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, m_setInstrumentIDs);

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->SubscribeMarketData(pArray, (int)vct.size());

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	string szInstrumentIDs;
	for (set<string>::iterator i = instrumentIDs.begin(); i != instrumentIDs.end(); ++i)
	{
		szInstrumentIDs.append(*i);
		szInstrumentIDs.append(";");
	}

	if (szInstrumentIDs.length()>1)
	{
		Subscribe(szInstrumentIDs, szExchageID);
	}
}

void CLevel2UserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);
	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, m_setInstrumentIDs);

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->UnSubscribeMarketData(pArray, (int)vct.size());

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::SubscribeAll()
{
	if (nullptr == m_pApi)
		return;

	m_pApi->SubscribeAll();
}
void CLevel2UserApi::UnsubscribeAll()
{
	if (nullptr == m_pApi)
		return;

	m_pApi->UnSubscribeAll();
}

void CLevel2UserApi::OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField)
{
	IsErrorRspInfo_Output(pErrorField);
}

void CLevel2UserApi::OnRspUnSubscribeMarketData(struct ErrorRtnField * pErrorField)
{
	IsErrorRspInfo_Output(pErrorField);
}

void CLevel2UserApi::OnRspSubscribeAll(struct ErrorRtnField * pErrorField)
{
	IsErrorRspInfo_Output(pErrorField);
}

void CLevel2UserApi::OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField)
{
	IsErrorRspInfo_Output(pErrorField);
}

void CLevel2UserApi::OnBestAndDeep(MDBestAndDeep * const pQuote)
{

}
void CLevel2UserApi::OnArbi(MDBestAndDeep * const pQuote)
{

}
void CLevel2UserApi::OnTenEntrust(MDTenEntrust * const pQuote)
{

}
void CLevel2UserApi::OnRealtime(MDRealTimePrice * const pQuote)
{

}
void CLevel2UserApi::OnOrderStatistic(MDOrderStatistic * const pQuote)
{

}
void CLevel2UserApi::OnMarchPrice(MDMarchPriceQty * const pQuote)
{

}
