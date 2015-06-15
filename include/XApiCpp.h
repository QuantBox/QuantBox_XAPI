#if !defined(XAPI_CPP_H)
#define XAPI_CPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/CrossPlatform.h"

#include "../include/ApiStruct.h"

class CXApi;

class CXSpi
{
public:
	virtual void OnConnectionStatus(CXApi* pApi, ConnectionStatus status, RspUserLoginField* pUserLogin, int size1){};
	virtual void OnRtnError(CXApi* pApi, ErrorField* pError){};

	virtual void OnRtnDepthMarketDataN(CXApi* pApi, DepthMarketDataNField* pMarketData){};
	virtual void OnRtnQuoteRequest(CXApi* pApi, QuoteRequestField* pQuoteRequest){};

	virtual void OnRspQryInstrument(CXApi* pApi, InstrumentField* pInstrument, int size1, bool bIsLast){};
	virtual void OnRspQryTradingAccount(CXApi* pApi, AccountField* pAccount, int size1, bool bIsLast){};
	virtual void OnRspQryInvestorPosition(CXApi* pApi, PositionField* pPosition, int size1, bool bIsLast){};
	virtual void OnRspQrySettlementInfo(CXApi* pApi, SettlementInfoField* pSettlementInfo, int size1, bool bIsLast){};
	virtual void OnRspQryInvestor(CXApi* pApi, InvestorField* pInvestor, int size1, bool bIsLast){};
	virtual void OnRtnOrder(CXApi* pApi, OrderField* pOrder){};
	virtual void OnRtnTrade(CXApi* pApi, TradeField* pTrade){};
	virtual void OnRtnQuote(CXApi* pApi, QuoteField* pQuote){};

	virtual void OnRspQryHistoricalTicks(CXApi* pApi, TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
	virtual void OnRspQryHistoricalBars(CXApi* pApi, BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};

	virtual bool OnFilterSubscribe(CXApi* pApi, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, char* pInstrument){ return true; };
};

class DLL_PUBLIC CXApi
{
public:
	static CXApi* CreateApi(char* libPath);
	virtual bool Init() = 0;
	virtual char* GetLastError() = 0;

	virtual ApiType GetApiType() = 0;
	virtual char* GetApiVersion() = 0;
	virtual char* GetApiName() = 0;

	virtual void RegisterSpi(CXSpi *pSpi) = 0;

	virtual void Connect(char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count) = 0;
	virtual void Disconnect() = 0;

	virtual void Subscribe(char* szInstrument, char* szExchange) = 0;
	virtual void Unsubscribe(char* szInstrument, char* szExchange) = 0;

	virtual void ReqQryInstrument(char* szInstrument, char* szExchange) = 0;
	virtual void ReqQryInvestorPosition(char* szInstrument, char* szExchange) = 0;
	virtual void ReqQryTradingAccount() = 0;

	virtual void SendOrder(OrderField* pOrder, OrderIDType* pInOut, int count) = 0;
	virtual void CancelOrder(OrderIDType* pIn, OrderIDType* pOut, int count) = 0;

	virtual void SendQuote(QuoteField* pQuote, OrderIDType* pAskOut, OrderIDType* pBidOut, int count) = 0;
	virtual void CancelQuote(OrderIDType* pIn, OrderIDType* pOut, int count) = 0;
protected:
	CXApi();
	~CXApi(){};
};

#endif
