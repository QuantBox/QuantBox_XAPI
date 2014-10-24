#include "stdafx.h"
#include "../include/ApiHeader.h"
//#include "../include/ApiStruct.h"
#include "../include/QueueEnum.h"
#include "TraderApi.h"

inline CTraderApi* GetApi(void* pApi)
{
	return static_cast<CTraderApi*>(pApi);
}

long long __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	if (rt == RequestType::Create)
	{
		return (long long)new CTraderApi();
	}

	if (pApi1 == nullptr)
	{
		return 0;
	}

	CTraderApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case Release:
		delete pApi;
		return 0;
	case Register:
		pApi->Register(ptr1);
		break;
	case Connect:
		pApi->Connect((const char*)ptr3, (ServerInfoField*)ptr1, (UserInfoField*)ptr2);
		break;
	case Disconnect:
		pApi->Disconnect();
		break;
	case ReqQryInstrument:
		pApi->ReqQryInstrument((const char*)ptr1, (const char*)ptr2);
		break;
	case ReqQryTradingAccount:
		pApi->ReqQryTradingAccount();
		break;
	case ReqQrySettlementInfo:
		pApi->ReqQrySettlementInfo((const char*)ptr1);
		break;
	case ReqOrderInsert:
		if (double2 == 0)
		{
			return (long long)pApi->ReqOrderInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
		}
		else
		{
			return (long long)pApi->ReqParkedOrderInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
		}
	case ReqQuoteInsert:
		return (long long)pApi->ReqQuoteInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
	case ReqOrderAction:
		return (long long)pApi->ReqOrderAction((const char*)ptr1);
		break;
	default:
		// 通知
		break;
	}

	return (long long)pApi1;
}
