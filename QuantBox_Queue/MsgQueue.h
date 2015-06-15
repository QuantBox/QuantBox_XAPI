#pragma once

#include <thread>
#include <mutex>
#include <cstring>
#include <condition_variable>
#include <atomic>

#include "../include/CrossPlatform.h"

#include "../include/QueueHeader.h"
#include "../include/QueueStruct.h"
#include "../include/ApiStruct.h"

//#include "readerwriterqueue.h"
//#include "concurrentqueue.h"
#include "ArrayLockFreeQueue.h"


using namespace std;
//using namespace moodycamel;

#pragma warning(push)
#pragma warning(disable:4251)

class DLL_PUBLIC CMsgQueue
{
public:
	bool m_bDirectOutput;

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
	void Register(void* pCallback,void* pClass)
	{
		m_fnOnRespone = (fnOnRespone)pCallback;
		// 目前没啥用，只是为了与行情和交易的接口统一
		m_pClass = pClass;
	}

	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void* new_block(int size)
	{
		// 下次改用内存池
		void* p = new char[size];
		if (p == nullptr)
			return nullptr;

		memset(p, 0, size);
		return p;
	}

	void delete_block(void* p)
	{
		if (p != nullptr)
			delete[] p;
	}

	// 直接发送，不入队列
	void* Input_Output(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		try
		{
			if (m_fnOnRespone)
				return (*m_fnOnRespone)(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		}
		catch (...)
		{
			m_fnOnRespone = nullptr;
		}
		return nullptr;
	}

	// 对输入的数据做了一次复制，主要是为了解决转过来的指针可能失效的问题。
	// 比如说STL中的指针跨线程指向的地址就无效了。所以从map中取的OrderField等都是做了次复制
	void Input_Copy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			delete_block(ptr1);
			delete_block(ptr2);
			delete_block(ptr3);
			return;
		}
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

	// 对于过来的指针不做复制，直接使用
	// 由于是跨DLL进行操作，由另一个DLL创建的内存块交给此队列进行处理时delete可能实现有变化导致出错
	// 所以必须是由此DLL new出来的内存块交给另一DLL修改返回后，再由此DLL delete，所以提前用到new_block
	void Input_NoCopy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			delete_block(ptr1);
			delete_block(ptr2);
			delete_block(ptr3);
			return;
		}
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

	// 不做拷贝也不做delete，这种必须由其它DLL进行delete
	void Input_NoCopy_NoDelete(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			return;
		}
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

protected:
	virtual void RunInThread();
	virtual void Output(ResponeItem* pItem);

private:
	static void ProcessThread(CMsgQueue* lpParam)
	{
		if (lpParam)
			lpParam->RunInThread();
	}
protected:
	//volatile bool						m_bRunning;
	atomic_bool							m_bRunning;
	mutex								m_mtx;
	mutex								m_mtx_del;
	condition_variable					m_cv;
	thread*								m_hThread;

private:
	//	ConcurrentQueue<ResponeItem*>		m_queue;
	ArrayLockFreeQueue<ResponeItem*>	m_queue;

	fnOnRespone							m_fnOnRespone;
	void*								m_pClass;
};

#pragma warning(pop)