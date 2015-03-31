#if !defined(XAPI_CPP_H)
#define XAPI_CPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/CrossPlatform.h"

#include "../include/ApiStruct.h"



class CXSpi
{
public:
	virtual void OnConnectionStatus(ConnectionStatus status, RspUserLoginField* pUserLogin, int size1){};
	virtual void OnRtnError(ErrorField* pError){};

	virtual void OnRtnDepthMarketData(DepthMarketDataField* pMarketData){};
	virtual void OnRtnQuoteRequest(QuoteRequestField* pQuoteRequest){};

	virtual void OnRspQryInstrument(InstrumentField* pInstrument, int size1, bool bIsLast){};
	virtual void OnRspQryTradingAccount(AccountField* pAccount, int size1, bool bIsLast){};
	virtual void OnRspQryInvestorPosition(PositionField* pPosition, int size1, bool bIsLast){};
	virtual void OnRspQrySettlementInfo(SettlementInfoField* pSettlementInfo, int size1, bool bIsLast){};
	virtual void OnRspQryInvestor(InvestorField* pInvestor, int size1, bool bIsLast){};
	virtual void OnRtnOrder(OrderField* pOrder){};
	virtual void OnRtnTrade(TradeField* pTrade){};
	virtual void OnRtnQuote(QuoteField* pQuote){};

	virtual void OnRspQryHistoricalTicks(TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
	virtual void OnRspQryHistoricalBars(BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
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
