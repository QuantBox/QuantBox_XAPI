#include "stdafx.h"
#include "HistoricalDataApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

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

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CHistoricalDataApi* pApi = (CHistoricalDataApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CHistoricalDataApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		if (iRet != 0)
			this_thread::sleep_for(chrono::milliseconds(1000 * 20));
		break;
	case E_ReqQryHistoricalTicks:
	{
									HistoricalDataRequestField* pHDR = (HistoricalDataRequestField*)ptr1;
									if (pHDR->CurrentDate != pHDR->Date1)
										this_thread::sleep_for(chrono::milliseconds(1000 * 6));
									iRet = ReqQryHistoricalTicks_(pHDR);
	}
		break;
	case E_ReqQryHistoricalBars:
	{
								   HistoricalDataRequestField* pHDR = (HistoricalDataRequestField*)ptr1;
								   if (pHDR->CurrentDate != pHDR->Date1)
									   this_thread::sleep_for(chrono::milliseconds(1000 * 6));
								   iRet = ReqQryHistoricalBars_(pHDR);
	}
		break;
	case E_ReqQryHistoricalTicks_Check:
		iRet = ReqQryHistoricalTicks_Check();
		break;
	default:
		break;
	}

	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

CHistoricalDataApi::CHistoricalDataApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue_Query->StartThread();

	m_nHdRequestId = 0;
}


CHistoricalDataApi::~CHistoricalDataApi(void)
{
	Disconnect();
}

void CHistoricalDataApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue->Register(pCallback,this);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
	}
}

void CHistoricalDataApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CHistoricalDataApi::_Init()
{
	m_pApi = CreateEsunnyQuotClient(this);
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	//初始化连接
	int iRet = m_pApi->Connect(m_ServerInfo.Address, m_ServerInfo.Port);
	if (0 == iRet)
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		iRet = m_pApi->Login(m_UserInfo.UserID, m_UserInfo.Password);
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = iRet;
		strcpy(pField->ErrorMsg, "连接超时");

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		return iRet;
	}
	return iRet;
}

void CHistoricalDataApi::Disconnect()
{
	// 清理查询队列
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr,nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_pApi)
	{
		m_pApi->DisConnect();
		DelEsunnyQuotClient(m_pApi);
		m_pApi = nullptr;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	// 清理响应队列
	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr,nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	m_lRequestID = 0;
}

int __cdecl CHistoricalDataApi::OnRspLogin(int err, const char *errtext)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	pField->ErrorID = err;
	strcpy(pField->ErrorMsg, errtext);

	if (err == 0)
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnChannelLost(int err, const char *errtext)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	pField->ErrorID = err;
	strcpy(pField->ErrorMsg, errtext);

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

	return 0;
}
int __cdecl CHistoricalDataApi::OnStkQuot(struct STKDATA *pData)
{
	return 0;
}

int __cdecl CHistoricalDataApi::OnRspHistoryQuot(struct STKHISDATA *pHisData)
{
	//BarField* pFields = new BarField[pHisData->nCount];
	BarField* pFields = (BarField*)m_msgQueue->new_block(sizeof(BarField)*pHisData->nCount);

	for (short i = 0; i < pHisData->nCount; i++)
	{
		HISTORYDATA item = pHisData->HisData[i];

		BarField* pF = &pFields[i];
		//memset(pF, 0, sizeof(BarField));
		DateTimeChat2Int(item.time, pF->Date, pF->Time);
		pF->Open = my_round(item.fOpen);
		pF->High = my_round(item.fHigh);
		pF->Low = item.fLow;
		pF->Close = item.fClose;
		pF->Volume = item.fVolume;
		pF->OpenInterest = item.fAmount;
	}

	m_msgQueue->Input_Copy(ResponeType::OnRspQryHistoricalBars, m_msgQueue, m_pClass, true, 0, pFields, sizeof(BarField)*pHisData->nCount, &m_RequestBar, sizeof(HistoricalDataRequestField), nullptr, 0);

	m_msgQueue->delete_block(pFields);

	return 0;
}

int CHistoricalDataApi::ReqQryHistoricalTicks(HistoricalDataRequestField* request)
{
	if (request->Date1 > request->Date2)
	{
		return 0;
	}

	++m_nHdRequestId;
	request->RequestId = m_nHdRequestId;
	request->CurrentDate = request->Date1;

	m_msgQueue_Query->Input_Copy(RequestType::E_ReqQryHistoricalTicks, m_msgQueue_Query, this, 0, 0,
		request, sizeof(HistoricalDataRequestField), nullptr, 0, nullptr, 0);

	return m_nHdRequestId;
}

int CHistoricalDataApi::ReqQryHistoricalTicks_(HistoricalDataRequestField* request)
{
	request->lRequest = ++m_lRequestID;
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
		m_timer_1 = time(NULL);
		m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqQryHistoricalTicks_Check, m_msgQueue_Query, this, 0, 0,
			nullptr, 0, nullptr, 0, nullptr, 0);
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

	m_msgQueue->Input_Copy(ResponeType::OnRspQryHistoricalTicks, m_msgQueue, m_pClass, bIsLast, 0, 0, 0, &m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0);

	if (!bIsLast)
	{
		m_RequestTick.CurrentDate = GetNextTradingDate(m_RequestTick.CurrentDate);

		m_msgQueue_Query->Input_Copy(RequestType::E_ReqQryHistoricalTicks, m_msgQueue_Query, this, 0, 0,
			&m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnRspTraceData(struct STKTRACEDATA *pTraceData)
{
	m_timer_1 = 0;

	//TickField* pFields = new TickField[pTraceData->nCount];
	TickField* pFields = (TickField*)m_msgQueue->new_block(sizeof(TickField)*pTraceData->nCount);

	for (size_t i = 0; i < pTraceData->nCount; i++)
	{
		STOCKTRACEDATA item = pTraceData->TraceData[i];

		TickField* pF = &pFields[i];
		//memset(pF, 0, sizeof(TickField));
		DateTimeChat2Int(item.time, pF->Date, pF->Time);
		pF->LastPrice = my_round(item.m_NewPrice);
		pF->Volume = item.m_Volume;
		pF->OpenInterest = item.m_Amount;
		pF->BidPrice1 = my_round(item.m_BuyPrice);
		pF->AskPrice1 = my_round(item.m_SellPrice);
		pF->BidSize1 = (VolumeType)item.m_BuyVol;
		pF->AskSize1 = (VolumeType)item.m_SellVol;
	}

	bool bIsLast = m_RequestTick.CurrentDate >= m_RequestTick.Date2;

	m_msgQueue->Input_Copy(ResponeType::OnRspQryHistoricalTicks, m_msgQueue, m_pClass, bIsLast, 0, pFields, sizeof(TickField)*pTraceData->nCount, &m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0);

	m_msgQueue->delete_block(pFields);

	if (!bIsLast)
	{
		m_RequestTick.CurrentDate = GetNextTradingDate(m_RequestTick.CurrentDate);
		m_msgQueue_Query->Input_Copy(RequestType::E_ReqQryHistoricalTicks, m_msgQueue_Query, this, 0, 0,
			&m_RequestTick, sizeof(HistoricalDataRequestField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int __cdecl CHistoricalDataApi::OnRspMarketInfo(struct MarketInfo *pMarketInfo, int bLast)
{
	for (unsigned short i = 0; i < pMarketInfo->stocknum; i++)
	{
		StockInfo item = pMarketInfo->stockdata[i];

		InstrumentField* pField = (InstrumentField*)m_msgQueue->new_block(sizeof(InstrumentField));

		strcpy(pField->InstrumentID, item.szCode);
		strcpy(pField->ExchangeID, pMarketInfo->Market);

		strcpy(pField->Symbol, item.szCode);

		strcpy(pField->InstrumentName, item.szName);
		pField->Type = InstrumentType::Future;

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInstrument, m_msgQueue, m_pClass, i >= pMarketInfo->stocknum - 1, 0, pField, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
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
	++m_nHdRequestId;
	request->RequestId = m_nHdRequestId;

	m_msgQueue_Query->Input_Copy(RequestType::E_ReqQryHistoricalBars, m_msgQueue_Query, this, 0, 0,
		request, sizeof(HistoricalDataRequestField), nullptr, 0, nullptr, 0);

	return m_nHdRequestId;
}

int CHistoricalDataApi::ReqQryHistoricalBars_(HistoricalDataRequestField* request)
{
	request->lRequest = ++m_lRequestID;
	memcpy(&m_RequestBar, request, sizeof(HistoricalDataRequestField));

	int period = BarSize2Period(request->BarSize);
	if (period == 0)
		return 0;

	return m_pApi->RequestHistory(request->ExchangeID, request->InstrumentID, period);
}