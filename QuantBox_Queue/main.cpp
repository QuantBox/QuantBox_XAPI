#include "stdafx.h"
#include "MsgQueue.h"
#include "../include/QueueHeader.h"
#include "../include/QueueEnum.h"
#include "../include/QueueStruct.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

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
		return (void*)"0.4.0.20150526";
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
		pQueue->Register(ptr1,ptr2);
		break;
	case Config:
		return (void*)pQueue->Config((ConfigInfoField*)ptr1);
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
		// 如果动态调用，使用直接输出的方式
		pQueue->Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	}

	return pApi1;
}