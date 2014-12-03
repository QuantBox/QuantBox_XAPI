#include "stdafx.h"
#include "HistoricalDataApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include <cstring>
#include <assert.h>

CHistoricalDataApi::CHistoricalDataApi(void)
{
	m_pApi = nullptr;
	m_msgQueue = nullptr;
	m_lRequestID = 0;

	m_hThread = nullptr;
	m_bRunning = false;
}


CHistoricalDataApi::~CHistoricalDataApi(void)
{
	Disconnect();
}

void CHistoricalDataApi::StartThread()
{
	if (nullptr == m_hThread)
	{
		m_bRunning = true;
		m_hThread = new thread(ProcessThread, this);
	}
}

void CHistoricalDataApi::StopThread()
{
	m_bRunning = false;
	if (m_hThread)
	{
		m_hThread->join();
		delete m_hThread;
		m_hThread = nullptr;
	}
}

void CHistoricalDataApi::Register(void* pMsgQueue)
{
	m_msgQueue = pMsgQueue;
}

void CHistoricalDataApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pApi = CreateEsunnyQuotClient(this);
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

void CHistoricalDataApi::Disconnect()
{
	// 如果队列中有请求包，在后面又进行了Release,又回过头来发送，可能导致当了
	StopThread();

	if (m_pApi)
	{
		m_pApi->DisConnect();
		DelEsunnyQuotClient(m_pApi);
		m_pApi = nullptr;

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	m_lRequestID = 0;//由于线程已经停止，没有必要用原子操作了

	ReleaseRequestListBuf();
	ReleaseRequestMapBuf();
}

CHistoricalDataApi::SRequest* CHistoricalDataApi::MakeRequestBuf(RequestType type)
{
	SRequest *pRequest = new SRequest;
	if (nullptr == pRequest)
		return nullptr;

	memset(pRequest, 0, sizeof(SRequest));
	pRequest->type = type;
	switch (type)
	{
	case E_Init:
		pRequest->pBuf = nullptr;
		break;
	default:
		assert(false);
		break;
	}
	return pRequest;
}

void CHistoricalDataApi::ReleaseRequestListBuf()
{
	lock_guard<mutex> cl(m_csList);
	while (!m_reqList.empty())
	{
		SRequest * pRequest = m_reqList.front();
		delete pRequest;
		m_reqList.pop_front();
	}
}

void CHistoricalDataApi::ReleaseRequestMapBuf()
{
	lock_guard<mutex> cl(m_csMap);
	for (map<int, SRequest*>::iterator it = m_reqMap.begin(); it != m_reqMap.end(); ++it)
	{
		delete (*it).second;
	}
	m_reqMap.clear();
}

void CHistoricalDataApi::ReleaseRequestMapBuf(int nRequestID)
{
	lock_guard<mutex> cl(m_csMap);
	map<int, SRequest*>::iterator it = m_reqMap.find(nRequestID);
	if (it != m_reqMap.end())
	{
		delete it->second;
		m_reqMap.erase(nRequestID);
	}
}

void CHistoricalDataApi::AddRequestMapBuf(int nRequestID, SRequest* pRequest)
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
			delete p;
			m_reqMap[nRequestID] = pRequest;
		}
	}
}

void CHistoricalDataApi::AddToSendQueue(SRequest * pRequest)
{
	if (nullptr == pRequest)
		return;

	lock_guard<mutex> cl(m_csList);
	bool bFind = false;
	//目前不去除相同类型的请求，即没有对大量同类型请求进行优化
	//for (list<SRequest*>::iterator it = m_reqList.begin();it!= m_reqList.end();++it)
	//{
	//	if (pRequest->type == (*it)->type)
	//	{
	//		bFind = true;
	//		break;
	//	}
	//}

	if (!bFind)
		m_reqList.push_back(pRequest);

	if (!m_reqList.empty())
	{
		StartThread();
	}
}



void CHistoricalDataApi::RunInThread()
{
	int iRet = 0;

	while (!m_reqList.empty() && m_bRunning)
	{
		SRequest * pRequest = m_reqList.front();
		int lRequest = ++m_lRequestID;// 这个地方是否会出现原子操作的问题呢？
		switch (pRequest->type)
		{
		case E_Init:
			iRet = ReqInit();
			if (iRet != 0 && m_bRunning)
                this_thread::sleep_for(chrono::milliseconds(1000 * 20));
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

int CHistoricalDataApi::ReqInit()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	//初始化连接
	int iRet = m_pApi->Connect(m_ServerInfo.Address, m_ServerInfo.Port);
	if (0 == iRet)
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		iRet = m_pApi->Login(m_UserInfo.UserID, m_UserInfo.Password);
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		RspUserLoginField field = { 0 };
		field.ErrorID = iRet;
		strcpy(field.ErrorMsg, "连接超时");

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		return iRet;
	}
	return iRet;
}

int __cdecl CHistoricalDataApi::OnRspLogin(int err, const char *errtext)
{
	RspUserLoginField field = { 0 };
	field.ErrorID = err;
	strncpy(field.ErrorMsg, errtext, sizeof(ErrorMsgType));

	if (err == 0)
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnChannelLost(int err, const char *errtext)
{
	RspUserLoginField field = { 0 };
	field.ErrorID = err;
	strncpy(field.ErrorMsg, errtext, sizeof(ErrorMsgType));

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

	return 0;
}
int __cdecl CHistoricalDataApi::OnStkQuot(struct STKDATA *pData)
{
	return 0;
}

int __cdecl CHistoricalDataApi::OnRspHistoryQuot(struct STKHISDATA *pHisData)
{
	return 0;
}

int __cdecl CHistoricalDataApi::OnRspTraceData(struct STKTRACEDATA *pTraceData)
{
	for (size_t i = 0; i < pTraceData->nCount; i++)
	{
		STOCKTRACEDATA item = pTraceData->TraceData[i];

		DepthMarketDataField marketData = { 0 };

	}
	return 0;
}

int __cdecl CHistoricalDataApi::OnRspMarketInfo(struct MarketInfo *pMarketInfo, int bLast)
{
	for (size_t i = 0; i < pMarketInfo->stocknum; i++)
	{
		StockInfo item = pMarketInfo->stockdata[i];

		InstrumentField field = { 0 };

		strcpy(field.InstrumentID, item.szCode);
		strcpy(field.ExchangeID, pMarketInfo->Market);

		strcpy(field.Symbol, item.szCode);

		strcpy(field.InstrumentName, item.szName);
		field.Type = InstrumentType::Future;
		
		XRespone(ResponeType::OnRspQryInstrument, m_msgQueue, this, i >= pMarketInfo->stocknum -1, 0, &field, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
	}

	return 0;
}
