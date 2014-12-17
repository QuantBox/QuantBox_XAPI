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
	switch (rt)
	{
	case GetApiType:
		return nullptr;
	case GetApiVersion:
		return (void*)"0.1";
	case GetApiName:
		return (void*)"Queue";
	case Create:
		return new CMsgQueue();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CMsgQueue* pQueue = GetQueue(pApi1);

	switch (rt)
	{
	case Release:
		delete pQueue;
		return 0;
	case Register:
		pQueue->Register(ptr1);
		break;
	case Config:
		break;
	case Connect:
		pQueue->StartThread();
		break;
	case Disconnect:
		pQueue->StopThread();
		break;
	case Clear:
		pQueue->Clear();
		break;
	case Process:
		pQueue->Process();
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
