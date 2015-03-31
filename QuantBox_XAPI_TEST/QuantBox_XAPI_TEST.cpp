// QuantBox_XAPI_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#include "../include/XApiCpp.h"

#ifdef _WIN64
#pragma comment(lib, "../lib/QuantBox_XAPI.lib")
#else
#pragma comment(lib, "../lib/QuantBox_XAPI.lib")
#endif

class CXSpiImpl :public CXSpi
{
public:
	virtual void OnConnectionStatus(ConnectionStatus status, RspUserLoginField* pUserLogin, int size1)
	{
		printf("%d\r\n", status);
	}
	virtual void OnRtnError(ErrorField* pError){};

	virtual void OnRtnDepthMarketData(DepthMarketDataField* pMarketData)
	{
		printf("%s,%f\r\n", pMarketData->Symbol,pMarketData->LastPrice);
	}

	virtual void OnRtnQuoteRequest(QuoteRequestField* pQuoteRequest){};

	virtual void OnRspQryInstrument(InstrumentField* pInstrument, int size1, bool bIsLast){};
	virtual void OnRspQryTradingAccount(AccountField* pAccount, int size1, bool bIsLast){};
	virtual void OnRspQryInvestorPosition(PositionField* pPosition, int size1, bool bIsLast){};
	virtual void OnRspQrySettlementInfo(SettlementInfoField* pSettlementInfo, int size1, bool bIsLast){};
	virtual void OnRspQryInvestor(InvestorField* pInvestor, int size1, bool bIsLast){};
	virtual void OnRtnOrder(OrderField* pOrder)
	{
		printf("%d,%s\r\n", pOrder->ErrorID,pOrder->Text);
	}
	virtual void OnRtnTrade(TradeField* pTrade){};
	virtual void OnRtnQuote(QuoteField* pQuote){};

	virtual void OnRspQryHistoricalTicks(TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
	virtual void OnRspQryHistoricalBars(BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
};

int _tmain(int argc, _TCHAR* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\CTP\\x86\\QuantBox_CTP_Quote.dll";
	char DLLPath2[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\CTP\\x86\\QuantBox_CTP_Trade.dll";
	
	ServerInfoField				m_ServerInfo1 = { 0 };
	ServerInfoField				m_ServerInfo2 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "1017");
	strcpy(m_ServerInfo1.Address, "tcp://ctpmn1-front1.citicsf.com:51213");

	strcpy(m_ServerInfo2.BrokerID, "1017");
	strcpy(m_ServerInfo2.Address, "tcp://ctpmn1-front1.citicsf.com:51205");

	strcpy(m_UserInfo.UserID, "00000025");
	strcpy(m_UserInfo.Password, "123456");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	CXApi* pApi2 = CXApi::CreateApi(DLLPath2);
	if (pApi1 && pApi2)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		if (!pApi2->Init())
		{
			printf("%s\r\n", pApi2->GetLastError());
			pApi2->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);
		pApi2->RegisterSpi(p);

		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
		pApi2->Connect("D:\\", &m_ServerInfo2, &m_UserInfo, 1);

		getchar();

		pApi1->Subscribe("IF1504", "");

		OrderIDType Out = { 0 };
		OrderField* pOrder = new OrderField();
		memset(pOrder, 0, sizeof(OrderField));
		strcpy(pOrder->InstrumentID, "IF1504");
		pOrder->OpenClose = OpenCloseType::Open;
		pOrder->HedgeFlag = HedgeFlagType::Speculation;
		pOrder->Price = 4000;
		pOrder->Qty = 1;
		pOrder->Type = OrderType::Limit;
		pOrder->Side = OrderSide::Buy;
		
		pApi2->SendOrder(pOrder, &Out, 1);

		printf("%s\r\n", Out);

		getchar();

		OrderIDType Out2 = { 0 };
		pApi2->CancelOrder(&Out, &Out2, 1);

		getchar();

		pApi1->Disconnect();
		pApi2->Disconnect();
	}

	return 0;
}

