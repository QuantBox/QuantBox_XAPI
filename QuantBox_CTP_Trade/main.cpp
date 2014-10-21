#include "stdafx.h"
#include "../include/ApiHeader.h"
//#include "../include/ApiStruct.h"
#include "../include/QueueEnum.h"
#include "TraderApi.h"

inline CTraderApi* GetApi(void* pApi)
{
	return static_cast<CTraderApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	if (rt == RequestType::Create)
	{
		return new CTraderApi();
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CTraderApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case Release:
		delete pApi;
		return nullptr;
	case Register:
		// 此处注册是将消息队列的指针传入
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
			return (void*)pApi->ReqOrderInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
		}
		else
		{
			return (void*)pApi->ReqParkedOrderInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
		}
	case ReqQuoteInsert:
		return (void*)pApi->ReqQuoteInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
	case ReqOrderAction:
		return (void*)pApi->ReqOrderAction((const char*)ptr1);
		break;
	default:
		// 通知
		break;
	}

	return pApi1;
}
