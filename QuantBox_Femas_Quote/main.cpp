#include "stdafx.h"
#include "../include/ApiHeader.h"

#include "../include/QueueEnum.h"
#include "MdUserApi.h"

inline CMdUserApi* GetApi(void* pApi)
{
	return static_cast<CMdUserApi*>(pApi);
}

long long __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	if (rt == RequestType::Create)
	{
		return (long long)new CMdUserApi();
	}

	if (pApi1 == nullptr)
	{
		return 0;
	}

	CMdUserApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case RequestType::Release:
		delete pApi;
		return 0;
	case RequestType::Register:
		pApi->Register(ptr1);
		break;
	case RequestType::Config:
		return (long long)pApi->Config((ConfigInfoField*)ptr1);
		break;
	case RequestType::Connect:
		pApi->Connect((const char*)ptr3, (ServerInfoField*)ptr1, (UserInfoField*)ptr2);
		break;
	case RequestType::Disconnect:
		pApi->Disconnect();
		break;
	case RequestType::Subscribe:
		pApi->Subscribe((const char*)ptr1, (const char*)ptr2);
		break;
	case RequestType::Unsubscribe:
		pApi->Unsubscribe((const char*)ptr1, (const char*)ptr2);
		break;
	//case RequestType::SubscribeQuote:
	//	pApi->SubscribeQuote((const char*)ptr1, (const char*)ptr2);
	//	break;
	//case RequestType::UnsubscribeQuote:
	//	pApi->UnsubscribeQuote((const char*)ptr1, (const char*)ptr2);
	//	break;
	default:
		// 通知
		break;
	}

	return (long long)pApi1;
}
