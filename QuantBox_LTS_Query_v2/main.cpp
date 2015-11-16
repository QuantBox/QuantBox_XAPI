#include "stdafx.h"
#include "../include/ApiHeader.h"
#include "../include/QueueEnum.h"
#include "QueryApi.h"

inline CQueryApi* GetApi(void* pApi)
{
	return static_cast<CQueryApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	switch (rt)
	{
	case GetApiType:
		return (void*)(ApiType::Query_ | ApiType::Instrument);
	case GetApiVersion:
		return (void*)"0.1.0.20151116";
	case GetApiName:
		return (void*)"LTS2";
	case Create:
		return new CQueryApi();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CQueryApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case Release:
		delete pApi;
		return nullptr;
	case Register:
		pApi->Register(ptr1, ptr2);
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
	case ReqQryInvestorPosition:
		pApi->ReqQryInvestorPosition((const char*)ptr1, (const char*)ptr2);
		break;
		//case ReqQrySettlementInfo:
		//	pApi->ReqQrySettlementInfo((const char*)ptr1);
		//	break;
	//case ReqOrderInsert:
	//	return (void*)pApi->ReqOrderInsert((OrderField*)ptr1, size1, (OrderIDType*)ptr2);
	//	//case ReqQuoteInsert:
	//	//	return (void*)pApi->ReqQuoteInsert((int)double1, (OrderField*)ptr1, (OrderField*)ptr2);
	//case ReqOrderAction:
	//	return (void*)pApi->ReqOrderAction((OrderIDType*)ptr1, size1, (OrderIDType*)ptr2);
	//	break;
	default:
		break;
	}

	return pApi1;
}
