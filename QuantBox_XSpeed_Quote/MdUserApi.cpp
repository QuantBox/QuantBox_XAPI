#include "stdafx.h"
#include "MdUserApi.h"
#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include <string.h>
#include <cfloat>

#include <mutex>
#include <vector>
#include <assert.h>

using namespace std;

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi1;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_UserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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
		m_msgQueue_Query->Input(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

CMdUserApi::CMdUserApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();
}

CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::Register(void* pCallback)
{
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query);
	m_msgQueue->Register(pCallback);
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

ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
}

bool CMdUserApi::IsErrorRspInfo_Output(struct DFITCErrorRtnField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->nErrorID != 0));
	if(bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pRspInfo->nErrorID;
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(ErrorMsgType));

		m_msgQueue->Input(ResponeType::OnRtnError, m_msgQueue, this, true, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CMdUserApi::IsErrorRspInfo(struct DFITCErrorRtnField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->nErrorID != 0));

	return bRet;
}

void CMdUserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input(RequestType::E_Init, this, nullptr, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_Init()
{
	m_pApi = DFITCMdApi::CreateDFITCMdApi();
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	//初始化连接
	int iRet = m_pApi->Init(m_ServerInfo.Address, this);
	if (0 == iRet)
	{
	}
	else
	{
		RspUserLoginField field = { 0 };
		field.ErrorID = iRet;
		strcpy(field.ErrorMsg, "连接超时");

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	return iRet;
}

void CMdUserApi::ReqUserLogin()
{
	DFITCUserLoginField body = {};

	strncpy(body.accountID, m_UserInfo.UserID, sizeof(DFITCAccountIDType));
	strncpy(body.passwd, m_UserInfo.Password, sizeof(DFITCPasswdType));
	body.companyID = atoi(m_ServerInfo.BrokerID);

	m_msgQueue_Query->Input(RequestType::E_UserLoginField, this, nullptr, 0, 0,
		&body, sizeof(DFITCUserLoginField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	DFITCUserLoginField* pBody = (DFITCUserLoginField*)ptr1;
	pBody->lRequestID = ++m_lRequestID;
	return m_pApi->ReqUserLogin(pBody);
}

void CMdUserApi::Disconnect()
{
	// 清理查询队列
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_pApi)
	{
		//m_pApi->RegisterSpi(NULL);
		m_pApi->Release();
		m_pApi = NULL;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	// 清理响应队列
	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	m_lRequestID = 0;
}


void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(NULL == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);
	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, m_setInstrumentIDs);

	if(vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->SubscribeMarketData(pArray, (int)vct.size(), ++m_lRequestID);

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchageID)
{
	if(NULL == m_pApi)
		return;

	string szInstrumentIDs;
	for(set<string>::iterator i=instrumentIDs.begin();i!=instrumentIDs.end();++i)
	{
		szInstrumentIDs.append(*i);
		szInstrumentIDs.append(";");
	}

	if (szInstrumentIDs.length()>1)
	{
		Subscribe(szInstrumentIDs, szExchageID);
	}
}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(NULL == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);
	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, m_setInstrumentIDs);

	if(vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->UnSubscribeMarketData(pArray, (int)vct.size(), ++m_lRequestID);

		delete[] pArray;
	}
	delete[] pBuf;
}

//void CMdUserApi::SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (NULL == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, m_setQuoteInstrumentIDs);
//
//	if (vct.size()>0)
//	{
//		//转成字符串数组
//		char** pArray = new char*[vct.size()];
//		for (size_t j = 0; j<vct.size(); ++j)
//		{
//			pArray[j] = vct[j];
//		}
//
//		//订阅
//		m_pApi->SubscribeForQuoteRsp(pArray, (int)vct.size());
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}
//
//void CMdUserApi::SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID)
//{
//	if (NULL == m_pApi)
//		return;
//
//	string szInstrumentIDs;
//	for (set<string>::iterator i = instrumentIDs.begin(); i != instrumentIDs.end(); ++i)
//	{
//		szInstrumentIDs.append(*i);
//		szInstrumentIDs.append(";");
//	}
//
//	if (szInstrumentIDs.length()>1)
//	{
//		SubscribeQuote(szInstrumentIDs, szExchageID);
//	}
//}
//
//void CMdUserApi::UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (NULL == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, m_setQuoteInstrumentIDs);
//
//	if (vct.size()>0)
//	{
//		//转成字符串数组
//		char** pArray = new char*[vct.size()];
//		for (size_t j = 0; j<vct.size(); ++j)
//		{
//			pArray[j] = vct[j];
//		}
//
//		//订阅
//		m_pApi->UnSubscribeForQuoteRsp(pArray, (int)vct.size());
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}
//
//CMdUserApi::SRequest* CMdUserApi::MakeRequestBuf(RequestType type)
//{
//	SRequest *pRequest = new SRequest;
//	if (NULL == pRequest)
//		return NULL;
//
//	memset(pRequest, 0, sizeof(SRequest));
//	pRequest->type = type;
//	switch (type)
//	{
//	case E_Init:
//		break;
//	case E_UserLoginField:
//		pRequest->pBuf = new DFITCUserLoginField();
//		break;
//	}
//	return pRequest;
//}
//
//void CMdUserApi::ReleaseRequestListBuf()
//{
//	lock_guard<mutex> cl(m_csList);
//	while (!m_reqList.empty())
//	{
//		SRequest * pRequest = m_reqList.front();
//		delete pRequest->pBuf;
//		delete pRequest;
//		m_reqList.pop_front();
//	}
//}
//
//void CMdUserApi::ReleaseRequestMapBuf()
//{
//	lock_guard<mutex> cl(m_csMap);
//	for (map<int, SRequest*>::iterator it = m_reqMap.begin(); it != m_reqMap.end(); ++it)
//	{
//		SRequest * pRequest = it->second;
//		delete pRequest->pBuf;
//		delete pRequest;
//	}
//	m_reqMap.clear();
//}
//
//void CMdUserApi::ReleaseRequestMapBuf(int nRequestID)
//{
//	lock_guard<mutex> cl(m_csMap);
//	map<int, SRequest*>::iterator it = m_reqMap.find(nRequestID);
//	if (it != m_reqMap.end())
//	{
//		SRequest * pRequest = it->second;
//		delete pRequest->pBuf;
//		delete pRequest;
//		m_reqMap.erase(nRequestID);
//	}
//}
//
//void CMdUserApi::AddRequestMapBuf(int nRequestID, SRequest* pRequest)
//{
//	if (NULL == pRequest)
//		return;
//
//	lock_guard<mutex> cl(m_csMap);
//	map<int, SRequest*>::iterator it = m_reqMap.find(nRequestID);
//	if (it != m_reqMap.end())
//	{
//		SRequest* p = it->second;
//		if (pRequest != p)//如果实际上指的是同一内存，不再插入
//		{
//			delete p->pBuf;
//			delete p;
//			m_reqMap[nRequestID] = pRequest;
//		}
//	}
//}
//
//void CMdUserApi::AddToSendQueue(SRequest * pRequest)
//{
//	if (NULL == pRequest)
//		return;
//
//	lock_guard<mutex> cl(m_csList);
//	bool bFind = false;
//
//	if (!bFind)
//		m_reqList.push_back(pRequest);
//
//	if (!m_reqList.empty())
//	{
//		StartThread();
//	}
//}
//
//
//void CMdUserApi::RunInThread()
//{
//	int iRet = 0;
//
//	while (!m_reqList.empty() && m_bRunning)
//	{
//		SRequest * pRequest = m_reqList.front();
//		long lRequest = ++m_lRequestID;
//		switch (pRequest->type)
//		{
//		case E_Init:
//			iRet = ReqInit();
//			if (iRet != 0 && m_bRunning)
//                this_thread::sleep_for(chrono::milliseconds(1000*20));
//			break;
//		case E_UserLoginField:
//			iRet = m_pApi->ReqUserLogin((DFITCUserLoginField*)pRequest->pBuf);
//			break;
//		default:
//			assert(false);
//			break;
//		}
//
//		if (0 == iRet)
//		{
//			//返回成功，填加到已发送池
//			m_nSleep = 1;
//			AddRequestMapBuf(lRequest, pRequest);
//
//			lock_guard<mutex> cl(m_csList);
//			m_reqList.pop_front();
//		}
//		else
//		{
//			//失败，按4的幂进行延时，但不超过1s
//			m_nSleep *= 4;
//			m_nSleep %= 1023;
//		}
//		this_thread::sleep_for(chrono::milliseconds(m_nSleep));
//	}
//
//	// 清理线程
//	m_hThread = nullptr;
//	m_bRunning = false;
//}

void CMdUserApi::OnFrontConnected()
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CMdUserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, field.ErrorMsg);

	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnRspUserLogin(struct DFITCUserLoginInfoRtnField * pRspUserLogin, struct DFITCErrorRtnField * pRspInfo)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		//strncpy(field.TradingDay, pRspUserLogin->TradingDay, sizeof(DateType));
		//strncpy(field.LoginTime, pRspUserLogin->LoginTime, sizeof(TimeType));
		//GetExchangeTime(pRspUserLogin->TradingDay, nullptr, pRspUserLogin->LoginTime,
		//	&field.TradingDay, nullptr, &field.LoginTime, nullptr);
		sprintf(field.SessionID, "%d", pRspUserLogin->sessionID);

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		//有可能断线了，本处是断线重连后重新订阅
		set<string> mapOld = m_setInstrumentIDs;//记下上次订阅的合约
		//Unsubscribe(mapOld);//由于已经断线了，没有必要再取消订阅
		Subscribe(mapOld,"");//订阅

		////有可能断线了，本处是断线重连后重新订阅
		//mapOld = m_setQuoteInstrumentIDs;//记下上次订阅的合约
		//SubscribeQuote(mapOld, "");//订阅
	}
	else
	{
		field.ErrorID = pRspInfo->nErrorID;
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(ErrorMsgType));

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CMdUserApi::OnRspError(struct DFITCErrorRtnField *pRspInfo)
{
	IsErrorRspInfo_Output(pRspInfo);
}

void CMdUserApi::OnRspSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if(!IsErrorRspInfo(pRspInfo)
		&&pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		m_setInstrumentIDs.insert(pSpecificInstrument->InstrumentID);
	}
}

void CMdUserApi::OnRspUnSubMarketData(struct DFITCSpecificInstrumentField * pSpecificInstrument, struct DFITCErrorRtnField * pRspInfo)
{
	//模拟平台可能这个函数不会触发
	if(!IsErrorRspInfo(pRspInfo)
		&&pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		m_setInstrumentIDs.erase(pSpecificInstrument->InstrumentID);
	}
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnMarketData(struct DFITCDepthMarketDataField *pMarketDataField)
{
	DepthMarketDataField marketData = {0};
	strcpy(marketData.InstrumentID, pMarketDataField->instrumentID);
	strcpy(marketData.ExchangeID, pMarketDataField->exchangeID);

	strcpy(marketData.Symbol, pMarketDataField->instrumentID);
	GetExchangeTime(pMarketDataField->tradingDay, nullptr, pMarketDataField->UpdateTime
		, &marketData.TradingDay, &marketData.ActionDay, &marketData.UpdateTime, &marketData.UpdateMillisec);
	marketData.UpdateMillisec = pMarketDataField->UpdateMillisec;

	marketData.LastPrice = pMarketDataField->lastPrice;
	marketData.Volume = pMarketDataField->Volume;
	marketData.Turnover = pMarketDataField->turnover;
	marketData.OpenInterest = pMarketDataField->openInterest;
	marketData.AveragePrice = pMarketDataField->AveragePrice;

	marketData.OpenPrice = pMarketDataField->openPrice;
	marketData.HighestPrice = pMarketDataField->highestPrice;
	marketData.LowestPrice = pMarketDataField->lowestPrice;
	marketData.ClosePrice = pMarketDataField->closePrice;
	marketData.SettlementPrice = pMarketDataField->settlementPrice;

	marketData.UpperLimitPrice = pMarketDataField->upperLimitPrice;
	marketData.LowerLimitPrice = pMarketDataField->lowerLimitPrice;
	marketData.PreClosePrice = pMarketDataField->preClosePrice;
	marketData.PreSettlementPrice = pMarketDataField->preSettlementPrice;
	marketData.PreOpenInterest = pMarketDataField->preOpenInterest;

	marketData.BidPrice1 = pMarketDataField->BidPrice1;
	marketData.BidVolume1 = pMarketDataField->BidVolume1;
	marketData.AskPrice1 = pMarketDataField->AskPrice1;
	marketData.AskVolume1 = pMarketDataField->AskVolume1;

	if (pMarketDataField->BidPrice2 != -1 || pMarketDataField->AskPrice2 != -1)
	{
		marketData.BidPrice2 = pMarketDataField->BidPrice2;
		marketData.BidVolume2 = pMarketDataField->BidVolume2;
		marketData.AskPrice2 = pMarketDataField->AskPrice2;
		marketData.AskVolume2 = pMarketDataField->AskVolume2;

		marketData.BidPrice3 = pMarketDataField->BidPrice3;
		marketData.BidVolume3 = pMarketDataField->BidVolume3;
		marketData.AskPrice3 = pMarketDataField->AskPrice3;
		marketData.AskVolume3 = pMarketDataField->AskVolume3;

		marketData.BidPrice4 = pMarketDataField->BidPrice4;
		marketData.BidVolume4 = pMarketDataField->BidVolume4;
		marketData.AskPrice4 = pMarketDataField->AskPrice4;
		marketData.AskVolume4 = pMarketDataField->AskVolume4;

		marketData.BidPrice5 = pMarketDataField->BidPrice5;
		marketData.BidVolume5 = pMarketDataField->BidVolume5;
		marketData.AskPrice5 = pMarketDataField->AskPrice5;
		marketData.AskVolume5 = pMarketDataField->AskVolume5;
	}

	m_msgQueue->Input(ResponeType::OnRtnDepthMarketData, m_msgQueue, this, 0, 0, &marketData, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
}

//void CMdUserApi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast)
//		&& pSpecificInstrument)
//	{
//		lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//
//		m_setQuoteInstrumentIDs.insert(pSpecificInstrument->InstrumentID);
//	}
//}
//
//void CMdUserApi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast)
//		&& pSpecificInstrument)
//	{
//		lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//
//		m_setQuoteInstrumentIDs.erase(pSpecificInstrument->InstrumentID);
//	}
//}
//
//void CMdUserApi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnRtnForQuoteRsp(this, pForQuoteRsp);
//
//
//	//XCall(m_msgQueue, ResponeType::OnConnectionStatus, 0, 0, 0, this, &field, nullptr);
//}
