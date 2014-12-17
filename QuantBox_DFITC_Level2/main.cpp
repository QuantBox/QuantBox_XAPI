#include "stdafx.h"
#include "../include/ApiHeader.h"

#include "../include/QueueEnum.h"
#include "Level2UserApi.h"

inline CLevel2UserApi* GetApi(void* pApi)
{
	return static_cast<CLevel2UserApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	switch (rt)
	{
	case GetApiType:
		return (void*)(ApiType::MarketData | ApiType::Level2);
	case GetApiVersion:
		return (void*)"0.1";
	case GetApiName:
		return (void*)"DFITC_Level2";
	case Create:
		return new CLevel2UserApi();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CLevel2UserApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case Release:
		delete pApi;
		return nullptr;
	case Register:
		pApi->Register(ptr1);
		break;
	case Connect:
		pApi->Connect((const char*)ptr3, (ServerInfoField*)ptr1, (UserInfoField*)ptr2);
		break;
	case Disconnect:
		pApi->Disconnect();
		break;
	case Subscribe:
		if (ptr1 == nullptr)
		{
			pApi->SubscribeAll();
		}
		else
		{
			pApi->Subscribe((const char*)ptr1, (const char*)ptr2);
		}
		break;
	case Unsubscribe:
		if (ptr1 == nullptr)
		{
			pApi->UnsubscribeAll();
		}
		else
		{
			pApi->Unsubscribe((const char*)ptr1, (const char*)ptr2);
		}
		break;
		//case SubscribeQuote:
		//	pApi->SubscribeQuote((const char*)ptr1, (const char*)ptr2);
		//	break;
		//case UnsubscribeQuote:
		//	pApi->UnsubscribeQuote((const char*)ptr1, (const char*)ptr2);
		//	break;
	default:
		break;
	}

	return pApi1;
}
