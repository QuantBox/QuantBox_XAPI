#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "XApiImpl.h"

#include "../include/QueueEnum.h"
#include "../include/XApiC.h"

void* __stdcall CXApiImpl::OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (pApi2 == nullptr)
		return nullptr;

	CXApiImpl* pThisClass = (CXApiImpl*)pApi2;
	return pThisClass->_OnRespone(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
}

void* CXApiImpl::_OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (nullptr == m_pSpi)
		return nullptr;

	ResponeType rt = (ResponeType)type;
	switch (rt)
	{
	case OnConnectionStatus:
		m_pSpi->OnConnectionStatus(this, (ConnectionStatus)(char)double1, (RspUserLoginField*)ptr1, size1);
		break;
	case OnRtnError:
		m_pSpi->OnRtnError(this, (ErrorField*)ptr1);
		break;
	case OnRtnDepthMarketData:
		m_pSpi->OnRtnDepthMarketDataN(this, (DepthMarketDataNField*)ptr1);
		break;
	case OnRspQryInstrument:
		m_pSpi->OnRspQryInstrument(this, (InstrumentField*)ptr1, size1, double1 != 0);
		break;
	case OnRspQryTradingAccount:
		m_pSpi->OnRspQryTradingAccount(this, (AccountField*)ptr1, size1, double1 != 0);
		break;
	case OnRspQryInvestorPosition:
		m_pSpi->OnRspQryInvestorPosition(this, (PositionField*)ptr1, size1, double1 != 0);
		break;
	case OnRspQrySettlementInfo:
		m_pSpi->OnRspQrySettlementInfo(this, (SettlementInfoField*)ptr1, size1, double1 != 0);
		break;

	case OnRtnOrder:
		m_pSpi->OnRtnOrder(this, (OrderField*)ptr1);
		break;
	case OnRtnTrade:
		m_pSpi->OnRtnTrade(this, (TradeField*)ptr1);
		break;

	case OnRtnQuote:
		m_pSpi->OnRtnQuote(this, (QuoteField*)ptr1);
		break;
	case OnRtnQuoteRequest:
		m_pSpi->OnRtnQuoteRequest(this, (QuoteRequestField*)ptr1);
		break;

	case OnRspQryHistoricalTicks:
		m_pSpi->OnRspQryHistoricalTicks(this, (TickField*)ptr1, size1, (HistoricalDataRequestField*)ptr2, size2, double1 != 0);
		break;
	case OnRspQryHistoricalBars:
		m_pSpi->OnRspQryHistoricalBars(this, (BarField*)ptr1, size1, (HistoricalDataRequestField*)ptr2, size2, double1 != 0);
		break;

	case OnRspQryInvestor:
		m_pSpi->OnRspQryInvestor(this, (InvestorField*)ptr1, size1, double1 != 0);
		break;
	case OnFilterSubscribe:
		return (void*)m_pSpi->OnFilterSubscribe(this, (ExchangeType)(char)double1, (int)size1, (int)size1, (int)size3, (char*)ptr1);
	default:
		break;
	}
	return nullptr;
}

CXApiImpl::CXApiImpl(char* libPath) :CXApi()
{
	m_pLib = nullptr;
	m_pFun = nullptr;
	m_pApi = nullptr;
	m_pSpi = nullptr;

	strncpy(m_LibPath, libPath, sizeof(m_LibPath));
}


CXApiImpl::~CXApiImpl()
{
	Disconnect();
}

ApiType CXApiImpl::GetApiType()
{
	return X_GetApiType(m_pFun);
}

char* CXApiImpl::GetApiVersion()
{
	return X_GetApiVersion(m_pFun);
}

char* CXApiImpl::GetApiName()
{
	return X_GetApiName(m_pFun);
}

void CXApiImpl::RegisterSpi(CXSpi *pSpi)
{
	m_pSpi = pSpi;
}

bool CXApiImpl::Init()
{
	m_pLib = X_LoadLib(m_LibPath);
	if (m_pLib)
	{
		m_pFun = X_GetFunction(m_pLib);
		if (m_pFun)
		{
			return true;
		}
	}
	return false;
}

char* CXApiImpl::GetLastError()
{
	return X_GetLastError();
}

void CXApiImpl::Connect(char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count)
{
	m_pApi = X_Create(m_pFun);
	X_Register(m_pFun, m_pApi, (void*)OnRespone, this);
	X_Connect(m_pFun, m_pApi, szPath, pServerInfo, pUserInfo, count);
}

void CXApiImpl::Disconnect()
{
	X_Disconnect(m_pFun, m_pApi);
	X_FreeLib(m_pLib);

	m_pLib = nullptr;
	m_pFun = nullptr;
	m_pApi = nullptr;
	m_pSpi = nullptr;
}

void CXApiImpl::Subscribe(char* szInstrument, char* szExchange)
{
	X_Subscribe(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::Unsubscribe(char* szInstrument, char* szExchange)
{
	X_Unsubscribe(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::ReqQryInstrument(char* szInstrument, char* szExchange)
{
	X_ReqQryInstrument(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::ReqQryInvestorPosition(char* szInstrument, char* szExchange)
{
	X_ReqQryInvestorPosition(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::ReqQryTradingAccount()
{
	X_ReqQryTradingAccount(m_pFun, m_pApi);
}

void CXApiImpl::SendOrder(OrderField* pOrder, OrderIDType* pInOut, int count)
{
	X_SendOrder(m_pFun, m_pApi, pOrder, pInOut, count);
}

void CXApiImpl::CancelOrder(OrderIDType* pIn, OrderIDType* pOut, int count)
{
	X_CancelOrder(m_pFun, m_pApi, pIn, pOut, count);
}

void CXApiImpl::SendQuote(QuoteField* pQuote, OrderIDType* pAskOut, OrderIDType* pBidOut, int count)
{
	X_SendQuote(m_pFun, m_pApi, pQuote, pAskOut, pBidOut, count);
}
void CXApiImpl::CancelQuote(OrderIDType* pIn, OrderIDType* pOut, int count)
{
	X_CancelQuote(m_pFun, m_pApi, pIn, pOut, count);
}
