#include "stdafx.h"
#include "MsgQueue.h"
#include "../include/QueueHeader.h"
#include "../include/QueueEnum.h"
#include "../include/QueueStruct.h"

#include "../include/ApiHeader.h"

inline CMsgQueue* GetQueue(void* pMsgQueue)
{
	return static_cast<CMsgQueue*>(pMsgQueue);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	if (rt == RequestType::Create)
	{
		return new CMsgQueue();
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CMsgQueue* pQueue = GetQueue(pApi1);

	switch (rt)
	{
	case RequestType::Release:
		delete pQueue;
		return 0;
	case RequestType::Register:
		pQueue->Register(ptr1);
		break;
	case RequestType::Clear:
		pQueue->Clear();
		break;
	case RequestType::Process:
		pQueue->Process();
		break;
	case RequestType::Connect:
		pQueue->StartThread();
		break;
	case RequestType::Disconnect:
		pQueue->StopThread();
		break;
	default:
		break;
	}

	return pApi1;
}

void* __stdcall XRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CMsgQueue* pQueue = GetQueue(pApi1);
	pQueue->Input(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);

	return pApi1;
}