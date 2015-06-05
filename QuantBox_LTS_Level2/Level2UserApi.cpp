#include "stdafx.h"
#include "Level2UserApi.h"
#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"
#include "../include/ApiProcess.h"

#include "../include/ChinaStock.h"

#include "../QuantBox_Queue/MsgQueue.h"
#ifdef _REMOTE
#include "../QuantBox_Queue/RemoteQueue.h"
#endif

#include <mutex>
#include <vector>
#include <cstring>

ExchangeType TSecurityFtdcExchangeIDType_2_ExchangeType(TSecurityFtdcExchangeIDType In)
{
	switch (In[1])
	{
	case 'S':
		return ExchangeType::SSE;
	case 'Z':
		return ExchangeType::SZE;
	default:
		return ExchangeType::Undefined_;
	}
}

using namespace std;

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CLevel2UserApi* pApi = (CLevel2UserApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CLevel2UserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_ReqUserLoginField:
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
		m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

CLevel2UserApi::CLevel2UserApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register((void*)Query,this);
	m_msgQueue_Query->StartThread();

	m_remoteQueue = nullptr;
}

CLevel2UserApi::~CLevel2UserApi(void)
{
	Disconnect();
}

void CLevel2UserApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register((void*)Query, this);
	m_msgQueue->Register(pCallback, this);
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

bool CLevel2UserApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

bool CLevel2UserApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CLevel2UserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);

#ifdef _REMOTE
	// 将收到的行情通过ZeroMQ发送出去
	if (strlen(m_ServerInfo.ExtendInformation) > 0)
	{
		m_remoteQueue = new CRemoteQueue(m_ServerInfo.ExtendInformation);
		m_remoteQueue->StartThread();
	}
#endif
}

int CLevel2UserApi::_Init()
{
	m_pApi = CSecurityFtdcL2MDUserApi::CreateFtdcL2MDUserApi(m_ServerInfo.IsMulticast);

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		m_pApi->RegisterSpi(this);

		//添加地址
		size_t len = strlen(m_ServerInfo.Address) + 1;
		char* buf = new char[len];
		strncpy(buf, m_ServerInfo.Address, len);

		char* token = strtok(buf, _QUANTBOX_SEPS_);
		while (token)
		{
			if (strlen(token)>0)
			{
				m_pApi->RegisterFront(token);
			}
			token = strtok(NULL, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		//初始化连接
		m_pApi->Init();
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	return 0;
}

void CLevel2UserApi::ReqUserLogin()
{
	CSecurityFtdcUserLoginField* pBody = (CSecurityFtdcUserLoginField*)m_msgQueue_Query->new_block(sizeof(CSecurityFtdcUserLoginField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TSecurityFtdcUserIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TSecurityFtdcPasswordType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CSecurityFtdcUserLoginField), nullptr, 0, nullptr, 0);
}

int CLevel2UserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CSecurityFtdcUserLoginField*)ptr1, ++m_lRequestID);
}

void CLevel2UserApi::Disconnect()
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

	// 清理队列
	if (m_remoteQueue)
	{
		m_remoteQueue->StopThread();
		m_remoteQueue->Register(nullptr, nullptr);
		m_remoteQueue->Clear();
		delete m_remoteQueue;
		m_remoteQueue = nullptr;
	}
}

void CLevel2UserApi::OnFrontConnected()
{
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CLevel2UserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	pField->ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, pField->ErrorMsg);

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::OnRspUserLogin(CSecurityFtdcUserLoginField *pUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&& pUserLogin)
	{
		//GetExchangeTime(pUserLogin->TradingDay, nullptr, nullptr,
		//	&pField->TradingDay, nullptr, &pField->LoginTime, nullptr);
		pField->TradingDay = GetDate(pUserLogin->TradingDay);
		//pField->LoginTime = GetTime(pUserLogin->LoginTime);

		//strncpy(pField->LoginTime, pUserLogin->, sizeof(TimeType));
		//sprintf(pField->SessionID, "%d:%d", pUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		//有可能断线了，本处是断线重连后重新订阅
		map<string,set<string> > mapOld = m_mapSecurityIDs;//记下上次订阅的合约

		for(map<string,set<string> >::iterator i=mapOld.begin();i!=mapOld.end();++i)
		{
			string strkey = i->first;
			set<string> setValue = i->second;

			Subscribe(setValue, strkey); //订阅
		}

		mapOld = m_mapIndexIDs;//记下上次订阅的合约

		for(map<string,set<string> >::iterator i=mapOld.begin();i!=mapOld.end();++i)
		{
			string strkey = i->first;
			set<string> setValue = i->second;

			Subscribe(setValue, strkey); //订阅
		}
	}
	else
	{
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->ErrorMsg, pRspInfo->ErrorMsg, sizeof(pRspInfo->ErrorMsg));

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
}

char* GetSetFromString_Index_Stock(const char* szString, const char* seps,
	vector<char*>& vct_I, set<char*>& st_I, set<string>& st2_I,
	vector<char*>& vct_S, set<char*>& st_S, set<string>& st2_S,
	int modify, int before, const char* prefix, const char* exchange)
{
	vct_I.clear();
	st_I.clear();
	vct_S.clear();
	st_S.clear();

	if (nullptr == szString
		|| nullptr == seps)
		return nullptr;

	if (strlen(szString) == 0
		|| strlen(seps) == 0)
		return nullptr;

	//这里不知道要添加的字符有多长，很悲剧
	size_t len = (size_t)(strlen(szString)*1.5 + 1);
	char* buf = new char[len];
	strncpy(buf, szString, len);

	char* token = strtok(buf, seps);
	while (token)
	{
		if (strlen(token)>0)
		{
			char temp[64] = { 0 };
			if (prefix)
			{
				if (before>0)
				{
					sprintf(temp, "%s%s", prefix, token);
				}
				else
				{
					sprintf(temp, "%s%s", token, prefix);
				}
			}
			else
			{
				sprintf(temp, "%s", token);
			}

			int instrumentId = atoi(token);

			bool bIndex = false;
			if (strlen(token) == 8)
			{
				bIndex = false;
			}
			else if (exchange[1] == 'Z')
			{
				if (InstrumentType::Index == InstrumentID_2_InstrumentType_SZE(instrumentId))
					bIndex = true;
			}
			else
			{
				if (InstrumentType::Index == InstrumentID_2_InstrumentType_SSE(instrumentId))
					bIndex = true;
			}		

			if (modify > 0)
			{
				if (bIndex)
				{
					st2_I.insert(temp);
				}
				else
				{
					st2_S.insert(temp);
				}
			}
			else if (modify < 0)
			{
				if (bIndex)
				{
					st2_I.erase(temp);
				}
				else
				{
					st2_S.erase(temp);
				}
			}

			if (bIndex)
			{
				vct_I.push_back(token);
				st_I.insert(token);
			}
			else
			{
				vct_S.push_back(token);
				st_S.insert(token);
			}
		}
		token = strtok(nullptr, seps);
	}

	return buf;
}

void CLevel2UserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(nullptr == m_pApi)
		return;

	vector<char*> vct_I;
	set<char*> st_I;
	vector<char*> vct_S;
	set<char*> st_S;

	lock_guard<mutex> cl(m_csMapIDs);

	set<string> _setInstrumentIDs;
	set<string> _setIndexIDs;
	{
		map<string, set<string> >::iterator it = m_mapSecurityIDs.find(szExchageID);
		if (it != m_mapSecurityIDs.end())
		{
			_setInstrumentIDs = it->second;
		}
	}
	{
		map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
		if (it != m_mapIndexIDs.end())
		{
			_setIndexIDs = it->second;
		}
	}
	
	// 分解
	char* pBuf = GetSetFromString_Index_Stock(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_,
		vct_I, st_I, _setIndexIDs,
		vct_S, st_S, _setInstrumentIDs,
		1, 1, nullptr, szExchageID.c_str());

	m_mapSecurityIDs[szExchageID] = _setInstrumentIDs;
	m_mapIndexIDs[szExchageID] = _setIndexIDs;

	if (vct_I.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct_I.size()];
		for (size_t j = 0; j<vct_I.size(); ++j)
		{
			pArray[j] = vct_I[j];
		}

		//订阅
		m_pApi->SubscribeL2Index(pArray, (int)vct_I.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}

	if (vct_S.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct_S.size()];
		for (size_t j = 0; j<vct_S.size(); ++j)
		{
			pArray[j] = vct_S[j];
		}

		//订阅
		m_pApi->SubscribeL2MarketData(pArray, (int)vct_S.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}

	delete[] pBuf;
}

void CLevel2UserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	vector<char*> vct_I;
	set<char*> st_I;
	vector<char*> vct_S;
	set<char*> st_S;

	lock_guard<mutex> cl(m_csMapIDs);

	set<string> _setInstrumentIDs;
	set<string> _setIndexIDs;
	{
		map<string, set<string> >::iterator it = m_mapSecurityIDs.find(szExchageID);
		if (it != m_mapSecurityIDs.end())
		{
			_setInstrumentIDs = it->second;
		}
	}
	{
		map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
		if (it != m_mapIndexIDs.end())
		{
			_setIndexIDs = it->second;
		}
	}

	// 分解
	char* pBuf = GetSetFromString_Index_Stock(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_,
		vct_I, st_I, _setIndexIDs,
		vct_S, st_S, _setInstrumentIDs,
		-1, 1, nullptr, szExchageID.c_str());

	m_mapSecurityIDs[szExchageID] = _setInstrumentIDs;
	m_mapIndexIDs[szExchageID] = _setIndexIDs;

	if (vct_I.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct_I.size()];
		for (size_t j = 0; j<vct_I.size(); ++j)
		{
			pArray[j] = vct_I[j];
		}

		//订阅
		m_pApi->UnSubscribeL2Index(pArray, (int)vct_I.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}

	if (vct_S.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct_S.size()];
		for (size_t j = 0; j<vct_S.size(); ++j)
		{
			pArray[j] = vct_S[j];
		}

		//订阅
		m_pApi->UnSubscribeL2MarketData(pArray, (int)vct_S.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}

	delete[] pBuf;
}

void CLevel2UserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchageID)
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

void CLevel2UserApi::OnRspSubL2MarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if(!IsErrorRspInfo(pRspInfo,nRequestID,bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapSecurityIDs.find(pSpecificInstrument->ExchangeID);
		if (it!=m_mapSecurityIDs.end())
		{
			_setInstrumentIDs = it->second;
		}

		_setInstrumentIDs.insert(pSpecificInstrument->InstrumentID);
		m_mapSecurityIDs[pSpecificInstrument->ExchangeID] = _setInstrumentIDs;
	}
}

void CLevel2UserApi::OnRspUnSubL2MarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//模拟平台可能这个函数不会触发
	if(!IsErrorRspInfo(pRspInfo,nRequestID,bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapSecurityIDs.find(pSpecificInstrument->ExchangeID);
		if (it!=m_mapSecurityIDs.end())
		{
			_setInstrumentIDs = it->second;
		}
		_setInstrumentIDs.erase(pSpecificInstrument->InstrumentID);
		m_mapSecurityIDs[pSpecificInstrument->ExchangeID] = _setInstrumentIDs;
	}
}

void CLevel2UserApi::OnRtnL2MarketData(CSecurityFtdcL2MarketDataField *pL2MarketData)
{
	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField)+sizeof(DepthField)* 20);
	strncpy(pField->InstrumentID, pL2MarketData->InstrumentID, sizeof(InstrumentIDType));
	
	pField->Exchange = TSecurityFtdcExchangeIDType_2_ExchangeType(pL2MarketData->ExchangeID);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pL2MarketData->ExchangeID);

	GetExchangeTime(pL2MarketData->TradingDay, pL2MarketData->TradingDay, pL2MarketData->TimeStamp
		, &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->LastPrice = pL2MarketData->LastPrice;
	pField->Volume = pL2MarketData->TotalTradeVolume;
	pField->Turnover = pL2MarketData->TotalTradeValue;
	//marketData.OpenInterest = pL2MarketData->OpenInterest;
	//pField->AveragePrice = pL2MarketData->;

	pField->OpenPrice = pL2MarketData->OpenPrice == DBL_MAX ? 0 : pL2MarketData->OpenPrice;
	pField->HighestPrice = pL2MarketData->HighPrice == DBL_MAX ? 0 : pL2MarketData->HighPrice;
	pField->LowestPrice = pL2MarketData->LowPrice == DBL_MAX ? 0 : pL2MarketData->LowPrice;
	pField->ClosePrice = pL2MarketData->ClosePrice == DBL_MAX ? 0 : pL2MarketData->ClosePrice;
	//pField->SettlementPrice = pL2MarketData->SettlementPrice;

	//pField->UpperLimitPrice = pL2MarketData->UpperLimitPrice;
	//pField->LowerLimitPrice = pL2MarketData->LowerLimitPrice;
	pField->PreClosePrice = pL2MarketData->PreClosePrice;
	//pField->PreSettlementPrice = pL2MarketData->PreSettlementPrice;
	//pField->PreOpenInterest = pL2MarketData->PreOpenInterest;

	InitBidAsk(pField);

	do
	{
		if (pL2MarketData->BidVolume1 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice1, pL2MarketData->BidVolume1, pL2MarketData->BidCount1);

		if (pL2MarketData->BidVolume2 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice2, pL2MarketData->BidVolume2, pL2MarketData->BidCount2);

		if (pL2MarketData->BidVolume3 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice3, pL2MarketData->BidVolume3, pL2MarketData->BidCount3);

		if (pL2MarketData->BidVolume4 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice4, pL2MarketData->BidVolume4, pL2MarketData->BidCount4);

		if (pL2MarketData->BidVolume5 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice5, pL2MarketData->BidVolume5, pL2MarketData->BidCount5);

		if (pL2MarketData->BidVolume6 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice6, pL2MarketData->BidVolume6, pL2MarketData->BidCount6);

		if (pL2MarketData->BidVolume7 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice7, pL2MarketData->BidVolume7, pL2MarketData->BidCount7);

		if (pL2MarketData->BidVolume8 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice8, pL2MarketData->BidVolume8, pL2MarketData->BidCount8);

		if (pL2MarketData->BidVolume9 == 0)
			break;
		AddBid(pField, pL2MarketData->BidPrice9, pL2MarketData->BidVolume9, pL2MarketData->BidCount9);

		if (pL2MarketData->BidVolumeA == 0)
			break;
		AddBid(pField, pL2MarketData->BidPriceA, pL2MarketData->BidVolumeA, pL2MarketData->BidCountA);
	} while (false);

	do
	{
		if (pL2MarketData->OfferVolume1 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice1, pL2MarketData->OfferVolume1, pL2MarketData->OfferCount1);

		if (pL2MarketData->OfferVolume2 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice2, pL2MarketData->OfferVolume2, pL2MarketData->OfferCount2);

		if (pL2MarketData->OfferVolume3 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice3, pL2MarketData->OfferVolume3, pL2MarketData->OfferCount3);

		if (pL2MarketData->OfferVolume4 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice4, pL2MarketData->OfferVolume4, pL2MarketData->OfferCount4);

		if (pL2MarketData->OfferVolume5 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice5, pL2MarketData->OfferVolume5, pL2MarketData->OfferCount5);

		if (pL2MarketData->OfferVolume6 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice6, pL2MarketData->OfferVolume6, pL2MarketData->OfferCount6);

		if (pL2MarketData->OfferVolume7 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice7, pL2MarketData->OfferVolume7, pL2MarketData->OfferCount7);

		if (pL2MarketData->OfferVolume8 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice8, pL2MarketData->OfferVolume8, pL2MarketData->OfferCount8);

		if (pL2MarketData->OfferVolume9 == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPrice9, pL2MarketData->OfferVolume9, pL2MarketData->OfferCount9);

		if (pL2MarketData->OfferVolumeA == 0)
			break;
		AddAsk(pField, pL2MarketData->OfferPriceA, pL2MarketData->OfferVolumeA, pL2MarketData->OfferCountA);
	} while (false);


	// 这两个队列先头循序不要搞混，有删除功能的语句要放在后面
	// 如果放前面，会导致远程收到乱码
#ifdef _REMOTE
	if (m_remoteQueue)
	{
		m_remoteQueue->Input_Copy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}
#endif

	m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}

//void CLevel2UserApi::SubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapIDs);
//
//	set<string> _setInstrumentIDs;
//	map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
//	if (it != m_mapIndexIDs.end())
//	{
//		_setInstrumentIDs = it->second;
//	}
//
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs);
//	m_mapIndexIDs[szExchageID] = _setInstrumentIDs;
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
//		m_pApi->SubscribeL2Index(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}

//void CLevel2UserApi::UnSubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapIndexIDs);
//
//	set<string> _setInstrumentIDs;
//	map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
//	if (it != m_mapIndexIDs.end())
//	{
//		_setInstrumentIDs = it->second;
//	}
//
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, _setInstrumentIDs);
//	m_mapIndexIDs[szExchageID] = _setInstrumentIDs;
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
//		m_pApi->UnSubscribeL2Index(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}
//
//void CLevel2UserApi::SubscribeL2Index(const set<string>& instrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
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
//		UnSubscribeL2Index(szInstrumentIDs, szExchageID);
//	}
//}

void CLevel2UserApi::OnRspSubL2Index(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapIndexIDs.find(pSpecificInstrument->ExchangeID);
		if (it != m_mapIndexIDs.end())
		{
			_setInstrumentIDs = it->second;
		}

		_setInstrumentIDs.insert(pSpecificInstrument->InstrumentID);
		m_mapIndexIDs[pSpecificInstrument->ExchangeID] = _setInstrumentIDs;
	}
}

void CLevel2UserApi::OnRspUnSubL2Index(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//模拟平台可能这个函数不会触发
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapIndexIDs.find(pSpecificInstrument->ExchangeID);
		if (it != m_mapIndexIDs.end())
		{
			_setInstrumentIDs = it->second;
		}
		_setInstrumentIDs.erase(pSpecificInstrument->InstrumentID);
		m_mapIndexIDs[pSpecificInstrument->ExchangeID] = _setInstrumentIDs;
	}
}

void CLevel2UserApi::OnRtnL2Index(CSecurityFtdcL2IndexField *pL2Index)
{
	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField));
	strncpy(pField->InstrumentID, pL2Index->InstrumentID, sizeof(InstrumentIDType));

	pField->Exchange = TSecurityFtdcExchangeIDType_2_ExchangeType(pL2Index->ExchangeID);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pL2Index->ExchangeID);

	GetExchangeTime(pL2Index->TradingDay, pL2Index->TradingDay, pL2Index->TimeStamp
		, &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->LastPrice = pL2Index->LastIndex;
	pField->Volume = pL2Index->TotalVolume;
	pField->Turnover = pL2Index->TurnOver;
	//marketData.OpenInterest = pL2Index->OpenInterest;
	//marketData.AveragePrice = pL2Index->AveragePrice;

	pField->OpenPrice = pL2Index->OpenIndex;
	pField->HighestPrice = pL2Index->HighIndex;
	pField->LowestPrice = pL2Index->LowIndex;
	pField->ClosePrice = pL2Index->CloseIndex;
	//marketData.SettlementPrice = pL2Index->SettlementPrice;

	//marketData.UpperLimitPrice = pL2Index->UpperLimitPrice;
	//marketData.LowerLimitPrice = pL2Index->LowerLimitPrice;
	pField->PreClosePrice = pL2Index->PreCloseIndex;
	//marketData.PreSettlementPrice = pL2Index->PreSettlementPrice;
	//marketData.PreOpenInterest = pL2Index->PreOpenInterest;

	InitBidAsk(pField);

	//marketData.BidPrice1 = pL2MarketData->BidPrice1;
	//marketData.BidVolume1 = pL2MarketData->BidVolume1;
	//marketData.AskPrice1 = pL2MarketData->OfferPrice1;
	//marketData.AskVolume1 = pL2MarketData->OfferVolume1;

	////if (pDepthMarketData->BidPrice2 != DBL_MAX || pDepthMarketData->AskPrice2 != DBL_MAX)
	//{
	//	marketData.BidPrice2 = pL2MarketData->BidPrice2;
	//	marketData.BidVolume2 = pL2MarketData->BidVolume2;
	//	marketData.AskPrice2 = pL2MarketData->OfferPrice2;
	//	marketData.AskVolume2 = pL2MarketData->OfferVolume2;

	//	marketData.BidPrice3 = pL2MarketData->BidPrice3;
	//	marketData.BidVolume3 = pL2MarketData->BidVolume3;
	//	marketData.AskPrice3 = pL2MarketData->OfferPrice3;
	//	marketData.AskVolume3 = pL2MarketData->OfferVolume3;

	//	marketData.BidPrice4 = pL2MarketData->BidPrice4;
	//	marketData.BidVolume4 = pL2MarketData->BidVolume4;
	//	marketData.AskPrice4 = pL2MarketData->OfferPrice2;
	//	marketData.AskVolume4 = pL2MarketData->OfferVolume4;

	//	marketData.BidPrice5 = pL2MarketData->BidPrice5;
	//	marketData.BidVolume5 = pL2MarketData->BidVolume5;
	//	marketData.AskPrice5 = pL2MarketData->OfferPrice5;
	//	marketData.AskVolume5 = pL2MarketData->OfferVolume5;
	//}

	// 这两个队列先头循序不要搞混，有删除功能的语句要放在后面
	// 如果放前面，会导致远程收到乱码
#ifdef _REMOTE
	if (m_remoteQueue)
	{
		m_remoteQueue->Input_Copy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}
#endif

	m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}
