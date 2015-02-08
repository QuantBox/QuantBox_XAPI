#pragma once

#include <thread>
#include <mutex>
#include <cstring>
#include <condition_variable>

#include "../include/CrossPlatform.h"

#include "../include/QueueHeader.h"
#include "../include/QueueStruct.h"
#include "readerwriterqueue.h"

using namespace std;
using namespace moodycamel;

#pragma warning(push)
#pragma warning(disable:4251)

class DLL_PUBLIC CMsgQueue
{
public:
	CMsgQueue();
	virtual ~CMsgQueue()
	{
		StopThread();
		Clear();
	}

public:
	//清空队列
	void Clear();

	//可以由外部发起，顺序处理队列触发回调函数
	bool Process();

	//由内部启动线程，内部主动调用Process触发回调
	void StartThread();
	void StopThread();

	//将外部的函数地址注册到队列
	void Register(void* pCallback)
	{
		m_fnOnRespone = (fnOnRespone)pCallback;
	}

	void Input_Copy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		ResponeItem* pItem = new ResponeItem;
		memset(pItem, 0, sizeof(ResponeItem));
		pItem->bNeedDelete = true;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		if (ptr1 && size1)
		{
			pItem->size1 = size1;
			pItem->ptr1 = new char[size1];
			memcpy(pItem->ptr1, ptr1, size1);
		}

		if (ptr2 && size2)
		{
			pItem->size2 = size2;
			pItem->ptr2 = new char[size2];
			memcpy(pItem->ptr2, ptr2, size2);
		}

		if (ptr3 && size3)
		{
			pItem->size3 = size3;
			pItem->ptr3 = new char[size3];
			memcpy(pItem->ptr3, ptr3, size3);
		}

		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

	void* new_block(int size)
	{
		// 下次改用内存池
		void* p = new char[size];
		if (p == nullptr)
			return nullptr;

		memset(p, 0, size);
		return p;
	}

	void Input_NoCopy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		// 不进行拷贝这种，由于必须自己生成，自己释放，所以这地方要由new_block来生成
		ResponeItem* pItem = new ResponeItem;
		pItem->bNeedDelete = true;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		pItem->ptr1 = ptr1;
		pItem->size1 = size1;
		pItem->ptr2 = ptr2;
		pItem->size2 = size2;
		pItem->ptr3 = ptr3;
		pItem->size3 = size3;

		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

	void Input_NoCopy_NoDelete(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		// 不进行拷贝这种，由于必须自己生成，自己释放，所以这地方要由new_block来生成
		ResponeItem* pItem = new ResponeItem;
		pItem->bNeedDelete = false;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		pItem->ptr1 = ptr1;
		pItem->size1 = size1;
		pItem->ptr2 = ptr2;
		pItem->size2 = size2;
		pItem->ptr3 = ptr3;
		pItem->size3 = size3;

		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

private:
	static void ProcessThread(CMsgQueue* lpParam)
	{
		if (lpParam)
			lpParam->RunInThread();
	}
	void RunInThread();
	void Output(ResponeItem* pItem)
	{
		try
		{
			if (m_fnOnRespone)
				(*m_fnOnRespone)(pItem->type, pItem->pApi1, pItem->pApi2, pItem->double1, pItem->double2, pItem->ptr1, pItem->size1, pItem->ptr2, pItem->size2, pItem->ptr3, pItem->size3);
		}
		catch (...)
		{
			m_fnOnRespone = nullptr;
		}
	}

private:
	volatile bool						m_bRunning;
	mutex								m_mtx;
	condition_variable					m_cv;
	thread*								m_hThread;
	ReaderWriterQueue<ResponeItem*>		m_queue;
	fnOnRespone							m_fnOnRespone;
};

#pragma warning(pop)