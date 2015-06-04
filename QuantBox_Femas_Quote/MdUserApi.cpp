#include "stdafx.h"
#include "MdUserApi.h"
#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"
#include "../include/ApiProcess.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include <string.h>
#include <cfloat>

#include <mutex>
#include <vector>
using namespace std;

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi2;
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
	case E_ReqUserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

CMdUserApi::CMdUserApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register((void*)Query,this);
	m_msgQueue_Query->StartThread();
}

CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register((void*)Query,this);
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

ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
}

bool CMdUserApi::IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->ErrorID = pRspInfo->ErrorID;
		strcpy(pField->ErrorMsg, pRspInfo->ErrorMsg);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CMdUserApi::IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CMdUserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_Init()
{
	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(NULL));
	sprintf(pszPath, "%s/%s/%s/Md/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	m_pApi = CUstpFtdcMduserApi::CreateFtdcMduserApi(pszPath);
	delete[] pszPath;

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		m_pApi->RegisterSpi(this);

		//添加地址
		size_t len = strlen(m_ServerInfo.Address) + 1;
		char* buf = new char[len];
		strncpy(buf, m_ServerInfo.Address, len);

		m_pApi->SetUseMultiChannel(m_ServerInfo.IsMulticast);

		char* token = strtok(buf, _QUANTBOX_SEPS_);
		while (token)
		{
			if (strlen(token)>0)
			{
				if (m_ServerInfo.IsMulticast)
				{
					// RegisterMultiChannel("multi://[接收多播的网卡所属的地址段]@[接收多播的地址]:[端口号]")
					m_pApi->RegisterMultiChannel(token);
				}
				else
				{
					m_pApi->RegisterFront(token);
				}
			}
			token = strtok(NULL, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		if (m_ServerInfo.MarketDataTopicResumeType<ResumeType::Undefined)
			m_pApi->SubscribeMarketDataTopic(m_ServerInfo.TopicId, (USTP_TE_RESUME_TYPE)m_ServerInfo.MarketDataTopicResumeType);
		//初始化连接
		m_pApi->Init();
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	return 0;
}

void CMdUserApi::ReqUserLogin()
{
	CUstpFtdcReqUserLoginField* pBody = (CUstpFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CUstpFtdcReqUserLoginField));

	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TUstpFtdcUserIDType));
	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TUstpFtdcBrokerIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TUstpFtdcPasswordType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CUstpFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CUstpFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CMdUserApi::Disconnect()
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
		m_pApi->RegisterSpi(NULL);
		m_pApi->Release();
		m_pApi = NULL;

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
}


void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(nullptr == m_pApi)
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
		m_pApi->SubMarketData(pArray, (int)vct.size());

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchageID)
{
	if(nullptr == m_pApi)
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
	if(nullptr == m_pApi)
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
		m_pApi->UnSubMarketData(pArray, (int)vct.size());

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::OnFrontConnected()
{
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CMdUserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	pField->ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, pField->ErrorMsg);

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		pField->TradingDay = GetDate(pRspUserLogin->TradingDay);
		pField->LoginTime = GetTime(pRspUserLogin->LoginTime);

		//sprintf(pField->SessionID, "%d:%d", pRspUserLogin->, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		//有可能断线了，本处是断线重连后重新订阅
		set<string> mapOld = m_setInstrumentIDs;//记下上次订阅的合约
		//Unsubscribe(mapOld);//由于已经断线了，没有必要再取消订阅
		Subscribe(mapOld,"");//订阅
	}
	else
	{
		pField->ErrorID = pRspInfo->ErrorID;
		strcpy(pField->ErrorMsg, pRspInfo->ErrorMsg);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CMdUserApi::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
}

void CMdUserApi::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if(!IsErrorRspInfo(pRspInfo,nRequestID,bIsLast)
		&&pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		m_setInstrumentIDs.insert(pSpecificInstrument->InstrumentID);
	}
}

void CMdUserApi::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//模拟平台可能这个函数不会触发
	if(!IsErrorRspInfo(pRspInfo,nRequestID,bIsLast)
		&&pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		m_setInstrumentIDs.erase(pSpecificInstrument->InstrumentID);
	}
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData)
{
	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField)+sizeof(DepthField)* 10);

	strcpy(pField->InstrumentID, pDepthMarketData->InstrumentID);
	//strcpy(pField->ExchangeID, pDepthMarketData->ExchangeID);
	pField->Exchange = ExchangeType::CFFEX;

	sprintf(pField->Symbol, "%s.CFFEX", pField->InstrumentID);

	GetExchangeTime(pDepthMarketData->TradingDay, pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime
		, &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);
	pField->UpdateMillisec = pDepthMarketData->UpdateMillisec;

	pField->LastPrice = pDepthMarketData->LastPrice;
	pField->Volume = pDepthMarketData->Volume;
	pField->Turnover = pDepthMarketData->Turnover;
	pField->OpenInterest = pDepthMarketData->OpenInterest;
	//marketData.AveragePrice = pDepthMarketData->AveragePrice;

	pField->OpenPrice = pDepthMarketData->OpenPrice;
	pField->HighestPrice = pDepthMarketData->HighestPrice;
	pField->LowestPrice = pDepthMarketData->LowestPrice;
	pField->ClosePrice = pDepthMarketData->ClosePrice;
	pField->SettlementPrice = pDepthMarketData->SettlementPrice;

	pField->UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
	pField->LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
	pField->PreClosePrice = pDepthMarketData->PreClosePrice;
	pField->PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
	pField->PreOpenInterest = pDepthMarketData->PreOpenInterest;

	InitBidAsk(pField);

	do
	{
		if (pDepthMarketData->BidVolume1 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1, 0);

		if (pDepthMarketData->BidVolume2 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice2, pDepthMarketData->BidVolume2, 0);

		if (pDepthMarketData->BidVolume3 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice3, pDepthMarketData->BidVolume3, 0);

		if (pDepthMarketData->BidVolume4 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice4, pDepthMarketData->BidVolume4, 0);

		if (pDepthMarketData->BidVolume5 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice5, pDepthMarketData->BidVolume5, 0);
	} while (false);

	do
	{
		if (pDepthMarketData->AskVolume1 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1, 0);

		if (pDepthMarketData->AskVolume2 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice2, pDepthMarketData->AskVolume2, 0);

		if (pDepthMarketData->AskVolume3 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice3, pDepthMarketData->AskVolume3, 0);

		if (pDepthMarketData->AskVolume4 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice4, pDepthMarketData->AskVolume4, 0);

		if (pDepthMarketData->AskVolume5 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice5, pDepthMarketData->AskVolume5, 0);
	} while (false);

	m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}
