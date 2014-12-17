#include "stdafx.h"
#include "HistoricalDataApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include <cstring>
#include <assert.h>

void DateTimeChat2Int(char* time, int& yyyyMMdd, int& hhmmss)
{
	int yyyy = atoi(&time[0]);
	int MM = atoi(&time[5]);
	int dd = atoi(&time[8]);
	int hh = atoi(&time[11]);
	int mm = atoi(&time[14]);
	int ss = atoi(&time[17]);

	yyyyMMdd = yyyy * 10000 + MM * 100 + dd;
	hhmmss = hh * 10000 + mm * 100 + ss;
}

int GetNextTradingDate(int date)
{
	int yyyy = date / 10000;
	int MM = date % 10000 / 100;
	int dd = date % 100;

	tm start_tm;
	memset(&start_tm, 0, sizeof(tm));
	start_tm.tm_year = yyyy - 1900;
	start_tm.tm_mon = MM - 1;
	start_tm.tm_mday = dd;

	time_t temp_time_t = mktime(&start_tm);
	temp_time_t += 86400;
	while (true)
	{
		tm* next_tm = localtime(&temp_time_t);
		if (next_tm->tm_wday == 0 || next_tm->tm_wday == 6)
			temp_time_t += 86400;
		else
		{
			return (next_tm->tm_year + 1900) * 10000 + (next_tm->tm_mon + 1) * 100 + (next_tm->tm_mday);
		}
	}
}


CHistoricalDataApi::CHistoricalDataApi(void)
{
	m_pApi = nullptr;
	m_msgQueue = nullptr;
	m_lRequestID = 0;

	m_hThread = nullptr;
	m_bRunning = false;

	m_nHdRequestId = 0;
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
	case E_ReqQryHistoricalTicks:
	case E_ReqQryHistoricalBars:
		pRequest->pBuf = new HistoricalDataRequestField;
		break;
	case E_ReqQryHistoricalTicks_Check:
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
		case E_ReqQryHistoricalTicks:
		{
										HistoricalDataRequestField* pHDR = (HistoricalDataRequestField*)pRequest->pBuf;
										// 请求太快了，后面会没有回应
										if (pHDR->CurrentDate != pHDR->Date1)
											this_thread::sleep_for(chrono::milliseconds(1000 * 3));
										iRet = ReqQryHistoricalTicks_(pHDR, lRequest);
		}
			break;
		case E_ReqQryHistoricalBars:
		{
										HistoricalDataRequestField* pHDR = (HistoricalDataRequestField*)pRequest->pBuf;
										// 请求太快了，后面会没有回应
										this_thread::sleep_for(chrono::milliseconds(1000 * 3));
										iRet = ReqQryHistoricalBars_(pHDR, lRequest);
		}
			break;
		case E_ReqQryHistoricalTicks_Check:
			iRet = ReqQryHistoricalTicks_Check();
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
	BarField* pFields = new BarField[pHisData->nCount];

	for (short i = 0; i < pHisData->nCount; i++)
	{
		HISTORYDATA item = pHisData->HisData[i];

		BarField* pF = &pFields[i];
		memset(pF, 0, sizeof(BarField));
		DateTimeChat2Int(item.time, pF->Date, pF->Time);
		pF->Open = item.fOpen;
		pF->High = item.fHigh;
		pF->Low = item.fLow;
		pF->Close = item.fClose;
		pF->Volume = item.fVolume;
		pF->OpenInterest = item.fAmount;
	}

	XRespone(ResponeType::OnRspQryHistoricalBars, m_msgQueue, this, true, 0, pFields, sizeof(BarField)*pHisData->nCount, &m_RequestBar, sizeof(HistoricalDataRequestField), nullptr, 0);

	ReleaseRequestMapBuf(m_RequestBar.lRequest);

	if (pFields)
		delete[] pFields;

	return 0;
}



int CHistoricalDataApi::ReqQryHistoricalTicks(HistoricalDataRequestField* request)
{
	if (request->Date1 > request->Date2)
	{
		return 0;
	}

	SRequest* pRequest = MakeRequestBuf(E_ReqQryHistoricalTicks);
	if (nullptr == pRequest)
		return 0;

	++m_nHdRequestId;
	request->RequestId = m_nHdRequestId;
	request->CurrentDate = request->Date1;
	memcpy(pRequest->pBuf, request, sizeof(HistoricalDataRequestField));
	AddToSendQueue(pRequest);

	return m_nHdRequestId;
}

int CHistoricalDataApi::ReqQryHistoricalTicks_(HistoricalDataRequestField* request,int lRequest)
{
	request->lRequest = lRequest;
	if (request->CurrentDate > request->Date2)
	{
		return 0;
	}
	
	memcpy(&m_RequestTick, request, sizeof(HistoricalDataRequestField));

	char buf1[20] = { 0 };
	sprintf(buf1, "%d", request->CurrentDate);
	int iRet = m_pApi->RequestTrace(request->ExchangeID, request->InstrumentID, buf1);
	if (iRet == 0)
	{
		// 每天早上如9点前，前是查了没有返回的，所以要延时检查,没有就发回一个结束的标识
		SRequest* pRequest = MakeRequestBuf(E_ReqQryHistoricalTicks_Check);
		if (nullptr == pRequest)
			return 0;

		m_timer_1 = time(NULL);
		AddToSendQueue(pRequest);
	}
	else
	{
		RtnEmptyRspQryHistoricalTicks();
	}
	return 0;
}

int CHistoricalDataApi::ReqQryHistoricalTicks_Check()
{
	m_timer_2 = time(NULL);
	if (m_timer_1 == 0)
		return 0;

	if (m_timer_2 - m_timer_1 > 10)
	{
		RtnEmptyRspQryHistoricalTicks();
		return 0;
	}
	else
	{
		return -1;
	}
}

int CHistoricalDataApi::RtnEmptyRspQryHistoricalTicks()
{
	bool bIsLast = m_RequestTick.CurrentDate >= m_RequestTick.Date2;

	XRespone(ResponeType::OnRspQryHistoricalTicks, m_msgQueue, this, bIsLast, 0, 0, 0, &m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0);

	if (!bIsLast)
	{
		SRequest* pRequest = MakeRequestBuf(E_ReqQryHistoricalTicks);
		if (nullptr == pRequest)
			return 0;

		m_RequestTick.CurrentDate = GetNextTradingDate(m_RequestTick.CurrentDate);
		memcpy(pRequest->pBuf, &m_RequestTick, sizeof(HistoricalDataRequestField));
		AddToSendQueue(pRequest);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnRspTraceData(struct STKTRACEDATA *pTraceData)
{
	m_timer_1 = 0;

	TickField* pFields = new TickField[pTraceData->nCount];

	for (size_t i = 0; i < pTraceData->nCount; i++)
	{
		STOCKTRACEDATA item = pTraceData->TraceData[i];

		TickField* pF = &pFields[i];
		memset(pF, 0, sizeof(TickField));
		DateTimeChat2Int(item.time, pF->Date, pF->Time);
		pF->LastPrice = item.m_NewPrice;
		pF->Volume = item.m_Volume;
		pF->OpenInterest = item.m_Amount;
		pF->BidPrice1 = item.m_BuyPrice;
		pF->AskPrice1 = item.m_SellPrice;
		pF->BidSize1 = (VolumeType)item.m_BuyVol;
		pF->AskSize1 = (VolumeType)item.m_SellVol;
	}

	bool bIsLast = m_RequestTick.CurrentDate >= m_RequestTick.Date2;

	XRespone(ResponeType::OnRspQryHistoricalTicks, m_msgQueue, this, bIsLast, 0, pFields, sizeof(TickField)*pTraceData->nCount, &m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0);

	ReleaseRequestMapBuf(m_RequestTick.lRequest);

	if (pFields)
		delete[] pFields;

	if (!bIsLast)
	{
		SRequest* pRequest = MakeRequestBuf(E_ReqQryHistoricalTicks);
		if (nullptr == pRequest)
			return 0;

		m_RequestTick.CurrentDate = GetNextTradingDate(m_RequestTick.CurrentDate);
		memcpy(pRequest->pBuf, &m_RequestTick, sizeof(HistoricalDataRequestField));
		AddToSendQueue(pRequest);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnRspMarketInfo(struct MarketInfo *pMarketInfo, int bLast)
{
	for (unsigned short i = 0; i < pMarketInfo->stocknum; i++)
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


int Period2BarSize(int period)
{
	int barSize = 0;
	switch (period)
	{
	case 1:
		barSize = 60;
		break;
	case 2:
		barSize = 300;
		break;
	case 3:
		barSize = 3600;
		break;
	case 4:
		barSize = 86400;
		break;
	default:
		break;
	}
	return barSize;
}

int BarSize2Period(int barSize)
{
	int period = 0;
	switch (barSize)
	{
	case 60:
		period = 1;
		break;
	case 60 * 5:
		period = 2;
		break;
	case 3600:
		period = 3;
		break;
	case 86400:
		period = 4;
		break;
	default:
		break;
	}
	return period;
}

int CHistoricalDataApi::ReqQryHistoricalBars(HistoricalDataRequestField* request)
{
	SRequest* pRequest = MakeRequestBuf(E_ReqQryHistoricalBars);
	if (nullptr == pRequest)
		return 0;

	++m_nHdRequestId;
	request->RequestId = m_nHdRequestId;
	memcpy(pRequest->pBuf, request, sizeof(HistoricalDataRequestField));
	AddToSendQueue(pRequest);

	return m_nHdRequestId;
}

int CHistoricalDataApi::ReqQryHistoricalBars_(HistoricalDataRequestField* request, int lRequest)
{
	request->lRequest = lRequest;
	memcpy(&m_RequestBar, request, sizeof(HistoricalDataRequestField));

	int period = BarSize2Period(request->BarSize);
	if (period == 0)
		return 0;

	return m_pApi->RequestHistory(request->ExchangeID, request->InstrumentID, period);
}