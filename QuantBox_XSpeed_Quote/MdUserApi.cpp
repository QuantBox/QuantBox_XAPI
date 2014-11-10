#include "stdafx.h"
#include "MdUserApi.h"
#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include <string.h>
#include <cfloat>

#include <mutex>
#include <vector>
#include <assert.h>

using namespace std;

CMdUserApi::CMdUserApi(void)
{
	m_pApi = NULL;
	m_msgQueue = NULL;
	m_lRequestID = 0;
}

CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::StartThread()
{
	if (nullptr == m_hThread)
	{
		m_bRunning = true;
		m_hThread = new thread(ProcessThread, this);
	}
}

void CMdUserApi::StopThread()
{
	m_bRunning = false;
	if (m_hThread)
	{
		m_hThread->join();
		delete m_hThread;
		m_hThread = nullptr;
	}
}

void CMdUserApi::Register(void* pMsgQueue)
{
	m_msgQueue = pMsgQueue;
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
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(pRspInfo->errorMsg));

		XRespone(ResponeType::OnRtnError, m_msgQueue, this, true, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
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

	m_pApi = DFITCMdApi::CreateDFITCMdApi();

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

void CMdUserApi::ReqUserLogin()
{
	if (NULL == m_pApi)
		return;

	DFITCUserLoginField request = {};

	strncpy(request.accountID, m_UserInfo.UserID, sizeof(DFITCAccountIDType));
	strncpy(request.passwd, m_UserInfo.Password, sizeof(DFITCPasswdType));
	request.companyID = atoi(m_ServerInfo.BrokerID);

	//只有这一处用到了m_nRequestID，没有必要每次重连m_nRequestID都从0开始
	m_pApi->ReqUserLogin(&request);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CMdUserApi::Disconnect()
{
	StopThread();

	if(m_pApi)
	{
		//m_pApi->RegisterSpi(NULL);
		m_pApi->Release();
		m_pApi = NULL;

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	m_lRequestID = 0;//由于线程已经停止，没有必要用原子操作了

	ReleaseRequestListBuf();
	ReleaseRequestMapBuf();
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

CMdUserApi::SRequest* CMdUserApi::MakeRequestBuf(RequestType type)
{
	SRequest *pRequest = new SRequest;
	if (NULL == pRequest)
		return NULL;

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

void CMdUserApi::ReleaseRequestListBuf()
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

void CMdUserApi::ReleaseRequestMapBuf()
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

void CMdUserApi::ReleaseRequestMapBuf(int nRequestID)
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

void CMdUserApi::AddRequestMapBuf(int nRequestID, SRequest* pRequest)
{
	if (NULL == pRequest)
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

void CMdUserApi::AddToSendQueue(SRequest * pRequest)
{
	if (NULL == pRequest)
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


void CMdUserApi::RunInThread()
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
                this_thread::sleep_for(chrono::milliseconds(1000*20));
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

int CMdUserApi::ReqInit()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
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

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	return iRet;
}

void CMdUserApi::OnFrontConnected()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CMdUserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, field.ErrorMsg);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnRspUserLogin(struct DFITCUserLoginInfoRtnField * pRspUserLogin, struct DFITCErrorRtnField * pRspInfo)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		//strncpy(field.TradingDay, pRspUserLogin->TradingDay, sizeof(DateType));
		//strncpy(field.LoginTime, pRspUserLogin->LoginTime, sizeof(TimeType));
		sprintf(field.SessionID, "%d", pRspUserLogin->sessionID);

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

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
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(pRspInfo->errorMsg));

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
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
		, &marketData.TradingDay, &marketData.ActionDay, &marketData.UpdateTime);
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

	XRespone(ResponeType::OnRtnDepthMarketData, m_msgQueue, this, 0, 0, &marketData, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
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
