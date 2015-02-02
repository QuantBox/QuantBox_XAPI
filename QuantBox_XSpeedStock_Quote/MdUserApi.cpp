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

char* ExchangeID_2to3(char* exchange)
{
	switch (exchange[1])
	{
	case 'H':
		return "SSE";
	case 'Z':
		return "SZE";
	case 'K':
		return "HK";
	default:
		break;
	}
	return "SSE";
}

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
	case E_StockUserLoginField:
		iRet = _ReqStockUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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


ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
}

bool CMdUserApi::IsErrorRspInfo_Output(struct DFITCSECRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->errorID != 0));
	if(bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pRspInfo->errorID;
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(ErrorMsgType));

		m_msgQueue->Input(ResponeType::OnRtnError, m_msgQueue, this, true, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CMdUserApi::IsErrorRspInfo(struct DFITCSECRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->errorID != 0));

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
	m_pApi = DFITCSECMdApi::CreateDFITCMdApi();

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

void CMdUserApi::ReqStockUserLogin()
{
	DFITCSECReqUserLoginField body = {0};

	strncpy(body.accountID, m_UserInfo.UserID, sizeof(DFITCSECAccountIDType));
	strncpy(body.passWord, m_UserInfo.Password, sizeof(DFITCSECPasswordType));

	m_msgQueue_Query->Input(RequestType::E_StockUserLoginField, this, nullptr, 0, 0,
		&body, sizeof(DFITCSECReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqStockUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	DFITCSECReqUserLoginField* pBody = (DFITCSECReqUserLoginField*)ptr1;
	pBody->requestID = ++m_lRequestID;
	return m_pApi->ReqStockUserLogin(pBody);
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
}


void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if(NULL == m_pApi)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(szExchageID);
	if (it != m_mapInstrumentIDs.end())
	{
		_setInstrumentIDs = it->second;
	}
	
	// 修正成内部格式
	set<string> _setInstrumentIDs2;
	
	char* pBuf2 = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs2, 1, szExchageID.c_str());
	delete[] pBuf2;

	string _szInstrumentIDs;
	for (set<string>::iterator i = _setInstrumentIDs2.begin(); i != _setInstrumentIDs2.end(); ++i)
	{
		_szInstrumentIDs.append(*i);
		_szInstrumentIDs.append(";");
	}


	char* pBuf = GetSetFromString(_szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs);
	m_mapInstrumentIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->SubscribeStockMarketData(pArray, (int)vct.size(), ++m_lRequestID);

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

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(szExchageID);
	if (it != m_mapInstrumentIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	// 修正成内部格式
	set<string> _setInstrumentIDs2;

	char* pBuf2 = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs2, 1, szExchageID.c_str());
	delete[] pBuf2;

	string _szInstrumentIDs;
	for (set<string>::iterator i = _setInstrumentIDs2.begin(); i != _setInstrumentIDs2.end(); ++i)
	{
		_szInstrumentIDs.append(*i);
		_szInstrumentIDs.append(";");
	}

	char* pBuf = GetSetFromString(_szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, _setInstrumentIDs);
	m_mapInstrumentIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		m_pApi->UnSubscribeStockMarketData(pArray, (int)vct.size(), ++m_lRequestID);

		delete[] pArray;
	}
	delete[] pBuf;
}

//int CMdUserApi::ReqInit()
//{
//	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//	//初始化连接
//	int iRet = m_pApi->Init(m_ServerInfo.Address, this);
//	if (0 == iRet)
//	{
//	}
//	else
//	{
//		RspUserLoginField field = { 0 };
//		field.ErrorID = iRet;
//		strcpy(field.ErrorMsg, "连接超时");
//
//		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
//	}
//	return iRet;
//}

void CMdUserApi::OnFrontConnected()
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqStockUserLogin();
	//ReqSOPUserLogin();
	//ReqFASLUserLogin();
}

void CMdUserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, field.ErrorMsg);

	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnRspStockUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		GetExchangeTime(nullptr, nullptr, pRspUserLogin->loginTime,
			nullptr, nullptr, &field.LoginTime, nullptr);
		field.TradingDay = pRspUserLogin->tradingDay;

		sprintf(field.SessionID, "%d", pRspUserLogin->sessionID);

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		//有可能断线了，本处是断线重连后重新订阅
		map<string, set<string> > mapOld = m_mapInstrumentIDs;//记下上次订阅的合约

		for (map<string, set<string> >::iterator i = mapOld.begin(); i != mapOld.end(); ++i)
		{
			string strkey = i->first;
			set<string> setValue = i->second;

			Subscribe(setValue, strkey);//订阅
		}

		////有可能断线了，本处是断线重连后重新订阅
		//mapOld = m_setQuoteInstrumentIDs;//记下上次订阅的合约
		//SubscribeQuote(mapOld, "");//订阅
	}
	else
	{
		field.ErrorID = pRspInfo->errorID;
		strncpy(field.ErrorMsg, pRspInfo->errorMsg, sizeof(ErrorMsgType));

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CMdUserApi::OnRspError(struct DFITCSECRspInfoField *pRspInfo)
{
	IsErrorRspInfo_Output(pRspInfo);
}

void CMdUserApi::OnRspStockSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if (!IsErrorRspInfo_Output(pRspInfo)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(pSpecificInstrument->exchangeID);
		if (it != m_mapInstrumentIDs.end())
		{
			_setInstrumentIDs = it->second;
		}

		_setInstrumentIDs.insert(pSpecificInstrument->securityID);
		m_mapInstrumentIDs[pSpecificInstrument->exchangeID] = _setInstrumentIDs;
	}
}

void CMdUserApi::OnRspStockUnSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo)
{
	//模拟平台可能这个函数不会触发
	if (!IsErrorRspInfo_Output(pRspInfo)
		&& pSpecificInstrument)
	{
		lock_guard<mutex> cl(m_csMapInstrumentIDs);

		set<string> _setInstrumentIDs;
		map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(pSpecificInstrument->exchangeID);
		if (it != m_mapInstrumentIDs.end())
		{
			_setInstrumentIDs = it->second;
		}
		_setInstrumentIDs.erase(pSpecificInstrument->securityID);
		m_mapInstrumentIDs[pSpecificInstrument->exchangeID] = _setInstrumentIDs;
	}
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnStockMarketData(struct DFITCStockDepthMarketDataField *pMarketDataField)
{
	DepthMarketDataField marketData = {0};
	strcpy(marketData.InstrumentID, pMarketDataField->staticDataField.securityID);
	strcpy(marketData.ExchangeID, pMarketDataField->staticDataField.exchangeID);

	sprintf(marketData.Symbol, "%s.%s", marketData.InstrumentID, ExchangeID_2to3(marketData.ExchangeID));

	marketData.TradingDay = pMarketDataField->staticDataField.tradingDay;
	marketData.ActionDay = pMarketDataField->staticDataField.tradingDay;
	GetUpdateTime(pMarketDataField->sharedDataField.updateTime, &marketData.UpdateTime, &marketData.UpdateMillisec);

	marketData.LastPrice = pMarketDataField->sharedDataField.latestPrice;
	marketData.Volume = pMarketDataField->sharedDataField.tradeQty;
	marketData.Turnover = pMarketDataField->sharedDataField.turnover;
	//marketData.OpenInterest = 0;
	//marketData.AveragePrice = 0;

	marketData.OpenPrice = pMarketDataField->staticDataField.openPrice;
	//marketData.HighestPrice = 0;
	//marketData.LowestPrice = 0;
	//marketData.ClosePrice = 0;
	//marketData.SettlementPrice = 0;

	marketData.UpperLimitPrice = pMarketDataField->staticDataField.upperLimitPrice;
	marketData.LowerLimitPrice = pMarketDataField->staticDataField.lowerLimitPrice;
	marketData.PreClosePrice = pMarketDataField->staticDataField.preClosePrice;
	//marketData.PreSettlementPrice = pMarketDataField->preSettlementPrice;
	//marketData.PreOpenInterest = pMarketDataField->preOpenInterest;

	marketData.BidPrice1 = pMarketDataField->sharedDataField.bidPrice1;
	marketData.BidVolume1 = pMarketDataField->sharedDataField.bidQty1;
	marketData.AskPrice1 = pMarketDataField->sharedDataField.askPrice1;
	marketData.AskVolume1 = pMarketDataField->sharedDataField.askQty1;

	marketData.BidPrice2 = pMarketDataField->sharedDataField.bidPrice2;
	marketData.BidVolume2 = pMarketDataField->sharedDataField.bidQty2;
	marketData.AskPrice2 = pMarketDataField->sharedDataField.askPrice2;
	marketData.AskVolume2 = pMarketDataField->sharedDataField.askQty2;

	marketData.BidPrice3 = pMarketDataField->sharedDataField.bidPrice3;
	marketData.BidVolume3 = pMarketDataField->sharedDataField.bidQty3;
	marketData.AskPrice3 = pMarketDataField->sharedDataField.askPrice3;
	marketData.AskVolume3 = pMarketDataField->sharedDataField.askQty3;

	marketData.BidPrice4 = pMarketDataField->sharedDataField.bidPrice4;
	marketData.BidVolume4 = pMarketDataField->sharedDataField.bidQty4;
	marketData.AskPrice4 = pMarketDataField->sharedDataField.askPrice4;
	marketData.AskVolume4 = pMarketDataField->sharedDataField.askQty4;

	marketData.BidPrice5 = pMarketDataField->sharedDataField.bidPrice5;
	marketData.BidVolume5 = pMarketDataField->sharedDataField.bidQty5;
	marketData.AskPrice5 = pMarketDataField->sharedDataField.askPrice5;
	marketData.AskVolume5 = pMarketDataField->sharedDataField.askQty5;

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

void CMdUserApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{
	ReqStockAvailableQuotQry(szInstrumentId, "SH");
	ReqStockAvailableQuotQry(szInstrumentId, "SZ");
	ReqStockAvailableQuotQry(szInstrumentId, "HK");
}

void CMdUserApi::ReqStockAvailableQuotQry(const string& szInstrumentId, const string& szExchange)
{
	DFITCReqQuotQryField body = {0};

	strcpy(body.accountID, m_UserInfo.UserID);
	strncpy(body.exchangeID, szExchange.c_str(), sizeof(DFITCSECExchangeIDType));

	m_msgQueue_Query->Input(RequestType::E_ReqSOPQuotQryField, this, nullptr, 0, 0,
		&body, sizeof(DFITCReqQuotQryField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqStockAvailableQuotQry(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	DFITCReqQuotQryField* pBody = (DFITCReqQuotQryField*)ptr1;
	pBody->requestID = ++m_lRequestID;
	return m_pApi->ReqStockAvailableQuotQry(pBody);
}

void CMdUserApi::OnRspStockAvailableQuot(struct DFITCRspQuotQryField * pAvailableQuotInfo, struct DFITCSECRspInfoField * pRspInfo, bool flag)
{
	if (!IsErrorRspInfo(pRspInfo))
	{
		if (pAvailableQuotInfo)
		{
			InstrumentField field = { 0 };

			strncpy(field.InstrumentID, pAvailableQuotInfo->securityID, sizeof(InstrumentIDType));
			strncpy(field.ExchangeID, pAvailableQuotInfo->exchangeID, sizeof(InstrumentIDType));

			sprintf(field.Symbol, "%s.%s", field.InstrumentID, ExchangeID_2to3(field.ExchangeID));
			strncpy(field.InstrumentName, pAvailableQuotInfo->securityName, sizeof(InstrumentNameType));

			field.Type = InstrumentType::Stock;
			//field.VolumeMultiple = pInstrumentData->contractMultiplier;
			//field.PriceTick = pInstrumentData->minPriceFluctuation;
			//strncpy(field.ExpireDate, pInstrumentData->instrumentMaturity, sizeof(DFITCInstrumentMaturityType));
			//field.OptionsType = TThostFtdcOptionsTypeType_2_PutCall(pInstrument->OptionsType);

			m_msgQueue->Input(ResponeType::OnRspQryInstrument, m_msgQueue, this, flag, 0, &field, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
		}
		else
		{
			m_msgQueue->Input(ResponeType::OnRspQryInstrument, m_msgQueue, this, flag, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}
