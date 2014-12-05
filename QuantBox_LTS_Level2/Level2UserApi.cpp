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

using namespace std;

CLevel2UserApi::CLevel2UserApi(void)
{
	m_pApi = nullptr;
	m_msgQueue = nullptr;
	m_nRequestID = 0;
}

CLevel2UserApi::~CLevel2UserApi(void)
{
	Disconnect();
}

void CLevel2UserApi::Register(void* pMsgQueue)
{
	m_msgQueue = pMsgQueue;
}

bool CLevel2UserApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pRspInfo->ErrorID;
		strcpy(field.ErrorMsg, pRspInfo->ErrorMsg);

		XRespone(ResponeType::OnRtnError, m_msgQueue, this, bIsLast, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
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

	m_pApi = CSecurityFtdcL2MDUserApi::CreateFtdcL2MDUserApi(pServerInfo->IsMulticast);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

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
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::ReqUserLogin()
{
	if (nullptr == m_pApi)
		return;

	CSecurityFtdcUserLoginField request = { 0 };

	strncpy(request.BrokerID, m_ServerInfo.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(request.UserID, m_UserInfo.UserID, sizeof(TSecurityFtdcUserIDType));
	strncpy(request.Password, m_UserInfo.Password, sizeof(TSecurityFtdcPasswordType));

	//只有这一处用到了m_nRequestID，没有必要每次重连m_nRequestID都从0开始
	m_pApi->ReqUserLogin(&request,++m_nRequestID);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::Disconnect()
{
	if (m_pApi)
	{
		m_pApi->RegisterSpi(NULL);
		m_pApi->Release();
		m_pApi = NULL;

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::OnFrontConnected()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CLevel2UserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, field.ErrorMsg);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::OnRspUserLogin(CSecurityFtdcUserLoginField *pUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&& pUserLogin)
	{
		strncpy(field.TradingDay, pUserLogin->TradingDay, sizeof(DateType));
		//strncpy(field.LoginTime, pUserLogin->, sizeof(TimeType));
		//sprintf(field.SessionID, "%d:%d", pUserLogin->FrontID, pRspUserLogin->SessionID);


		//有可能断线了，本处是断线重连后重新订阅
		map<string,set<string> > mapOld = m_mapSecurityIDs;//记下上次订阅的合约

		for(map<string,set<string> >::iterator i=mapOld.begin();i!=mapOld.end();++i)
		{
			string strkey = i->first;
			set<string> setValue = i->second;

			SubscribeL2MarketData(setValue, strkey);//订阅
		}

		mapOld = m_mapIndexIDs;//记下上次订阅的合约

		for(map<string,set<string> >::iterator i=mapOld.begin();i!=mapOld.end();++i)
		{
			string strkey = i->first;
			set<string> setValue = i->second;

			SubscribeL2Index(setValue, strkey);//订阅
		}
	}
	else
	{
		field.ErrorID = pRspInfo->ErrorID;
		strncpy(field.ErrorMsg, pRspInfo->ErrorMsg, sizeof(pRspInfo->ErrorMsg));

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CLevel2UserApi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
}

void CLevel2UserApi::SubscribeL2MarketData(const string& szInstrumentIDs, const string& szExchageID)
{
	if(nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapSecurityIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapSecurityIDs.find(szExchageID);
	if (it != m_mapSecurityIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs);
	m_mapSecurityIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->SubscribeL2MarketData(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::UnSubscribeL2MarketData(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapSecurityIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapSecurityIDs.find(szExchageID);
	if (it != m_mapSecurityIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, _setInstrumentIDs);
	m_mapSecurityIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->UnSubscribeL2MarketData(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::SubscribeL2MarketData(const set<string>& instrumentIDs, const string& szExchageID)
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
		SubscribeL2MarketData(szInstrumentIDs, szExchageID);
	}
}

void CLevel2UserApi::OnRspSubL2MarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if(!IsErrorRspInfo(pRspInfo,nRequestID,bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapSecurityIDs);

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
		lock_guard<mutex> cl(m_csMapSecurityIDs);

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
	DepthMarketDataField marketData = { 0 };
	strncpy(marketData.InstrumentID, pL2MarketData->InstrumentID, sizeof(TSecurityFtdcInstrumentIDType));
	strncpy(marketData.ExchangeID, pL2MarketData->ExchangeID, sizeof(TSecurityFtdcExchangeIDType));

	sprintf(marketData.Symbol, "%s.%s", marketData.InstrumentID, marketData.ExchangeID);

	GetExchangeTime(pL2MarketData->TradingDay, pL2MarketData->TradingDay, pL2MarketData->TimeStamp
		, &marketData.TradingDay, &marketData.ActionDay, &marketData.UpdateTime);
	marketData.UpdateMillisec = 0;

	marketData.LastPrice = pL2MarketData->LastPrice;
	marketData.Volume = pL2MarketData->TotalTradeVolume;
	marketData.Turnover = pL2MarketData->TotalTradeValue;
	//marketData.OpenInterest = pL2MarketData->OpenInterest;
	//marketData.AveragePrice = pL2MarketData->AveragePrice;

	marketData.OpenPrice = pL2MarketData->OpenPrice;
	marketData.HighestPrice = pL2MarketData->HighPrice;
	marketData.LowestPrice = pL2MarketData->LowPrice;
	marketData.ClosePrice = pL2MarketData->ClosePrice;
	//marketData.SettlementPrice = pL2MarketData->SettlementPrice;

	//marketData.UpperLimitPrice = pL2MarketData->UpperLimitPrice;
	//marketData.LowerLimitPrice = pL2MarketData->LowerLimitPrice;
	//marketData.PreClosePrice = pL2MarketData->PreClosePrice;
	//marketData.PreSettlementPrice = pL2MarketData->PreSettlementPrice;
	//marketData.PreOpenInterest = pL2MarketData->PreOpenInterest;

	marketData.BidPrice1 = pL2MarketData->BidPrice1;
	marketData.BidVolume1 = pL2MarketData->BidVolume1;
	marketData.AskPrice1 = pL2MarketData->OfferPrice1;
	marketData.AskVolume1 = pL2MarketData->OfferVolume1;

	//if (pDepthMarketData->BidPrice2 != DBL_MAX || pDepthMarketData->AskPrice2 != DBL_MAX)
	{
		marketData.BidPrice2 = pL2MarketData->BidPrice2;
		marketData.BidVolume2 = pL2MarketData->BidVolume2;
		marketData.AskPrice2 = pL2MarketData->OfferPrice2;
		marketData.AskVolume2 = pL2MarketData->OfferVolume2;

		marketData.BidPrice3 = pL2MarketData->BidPrice3;
		marketData.BidVolume3 = pL2MarketData->BidVolume3;
		marketData.AskPrice3 = pL2MarketData->OfferPrice3;
		marketData.AskVolume3 = pL2MarketData->OfferVolume3;

		marketData.BidPrice4 = pL2MarketData->BidPrice4;
		marketData.BidVolume4 = pL2MarketData->BidVolume4;
		marketData.AskPrice4 = pL2MarketData->OfferPrice2;
		marketData.AskVolume4 = pL2MarketData->OfferVolume4;

		marketData.BidPrice5 = pL2MarketData->BidPrice5;
		marketData.BidVolume5 = pL2MarketData->BidVolume5;
		marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		marketData.AskVolume5 = pL2MarketData->OfferVolume5;

		//marketData.BidPrice5 = pL2MarketData->BidPrice5;
		//marketData.BidVolume5 = pL2MarketData->BidVolume5;
		//marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		//marketData.AskVolume5 = pL2MarketData->OfferVolume5;

		//marketData.BidPrice5 = pL2MarketData->BidPrice5;
		//marketData.BidVolume5 = pL2MarketData->BidVolume5;
		//marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		//marketData.AskVolume5 = pL2MarketData->OfferVolume5;

		//marketData.BidPrice5 = pL2MarketData->BidPrice5;
		//marketData.BidVolume5 = pL2MarketData->BidVolume5;
		//marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		//marketData.AskVolume5 = pL2MarketData->OfferVolume5;

		//marketData.BidPrice5 = pL2MarketData->BidPrice5;
		//marketData.BidVolume5 = pL2MarketData->BidVolume5;
		//marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		//marketData.AskVolume5 = pL2MarketData->OfferVolume5;

		//marketData.BidPrice5 = pL2MarketData->BidPrice5;
		//marketData.BidVolume5 = pL2MarketData->BidVolume5;
		//marketData.AskPrice5 = pL2MarketData->OfferPrice5;
		//marketData.AskVolume5 = pL2MarketData->OfferVolume5;

	}

	XRespone(ResponeType::OnRtnDepthMarketData, m_msgQueue, this, 0, 0, &marketData, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
}

void CLevel2UserApi::SubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapIndexIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
	if (it != m_mapIndexIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs);
	m_mapIndexIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->SubscribeL2Index(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::UnSubscribeL2Index(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapIndexIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapIndexIDs.find(szExchageID);
	if (it != m_mapIndexIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, _setInstrumentIDs);
	m_mapIndexIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->UnSubscribeL2Index(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CLevel2UserApi::SubscribeL2Index(const set<string>& instrumentIDs, const string& szExchageID)
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
		UnSubscribeL2Index(szInstrumentIDs, szExchageID);
	}
}

void CLevel2UserApi::OnRspSubL2Index(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapIndexIDs);

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
		lock_guard<mutex> cl(m_csMapIndexIDs);

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
	DepthMarketDataField marketData = { 0 };
	strncpy(marketData.InstrumentID, pL2Index->InstrumentID, sizeof(TSecurityFtdcInstrumentIDType));
	strncpy(marketData.ExchangeID, pL2Index->ExchangeID, sizeof(TSecurityFtdcExchangeIDType));

	sprintf(marketData.Symbol, "%s.%s", marketData.InstrumentID, marketData.ExchangeID);
	GetExchangeTime(pL2Index->TradingDay, pL2Index->TradingDay, pL2Index->TimeStamp
		, &marketData.TradingDay, &marketData.ActionDay, &marketData.UpdateTime);
	marketData.UpdateMillisec = 0;

	marketData.LastPrice = pL2Index->LastIndex;
	marketData.Volume = pL2Index->TotalVolume;
	marketData.Turnover = pL2Index->TurnOver;
	//marketData.OpenInterest = pL2Index->OpenInterest;
	//marketData.AveragePrice = pL2Index->AveragePrice;

	marketData.OpenPrice = pL2Index->OpenIndex;
	marketData.HighestPrice = pL2Index->HighIndex;
	marketData.LowestPrice = pL2Index->LowIndex;
	marketData.ClosePrice = pL2Index->CloseIndex;
	//marketData.SettlementPrice = pL2Index->SettlementPrice;

	//marketData.UpperLimitPrice = pL2Index->UpperLimitPrice;
	//marketData.LowerLimitPrice = pL2Index->LowerLimitPrice;
	marketData.PreClosePrice = pL2Index->PreCloseIndex;
	//marketData.PreSettlementPrice = pL2Index->PreSettlementPrice;
	//marketData.PreOpenInterest = pL2Index->PreOpenInterest;

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

	XRespone(ResponeType::OnRtnDepthMarketData, m_msgQueue, this, DepthLevelType::L0, 0, &marketData, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
}
