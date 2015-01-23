#include "stdafx.h"
#include "MsgQueue.h"


//#include <stdlib.h>
//#include <crtdbg.h>  // For _CrtSetReportMode
//
//void myInvalidParameterHandler(const wchar_t* expression,
//	const wchar_t* function,
//	const wchar_t* file,
//	unsigned int line,
//	uintptr_t pReserved)
//{
//	wprintf(L"Invalid parameter detected in function %s."
//		L" File: %s Line: %d\n", function, file, line);
//	wprintf(L"Expression: %s\n", expression);
//	abort();
//}

CMsgQueue::CMsgQueue():m_queue(1024)
{
	m_hThread = nullptr;
	m_bRunning = false;

	//回调函数地址指针
	m_fnOnRespone = nullptr;

	//_invalid_parameter_handler oldHandler, newHandler;
	//newHandler = myInvalidParameterHandler;
	//oldHandler = _set_invalid_parameter_handler(newHandler);
	//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
}

void CMsgQueue::Clear()
{
	ResponeItem* pItem = nullptr;
	//清空队列
	while (m_queue.try_dequeue(pItem))
	{
		delete pItem->ptr1;
		delete pItem->ptr2;
		delete pItem->ptr3;
		delete pItem;
	}
}

bool CMsgQueue::Process()
{
	ResponeItem* pItem = nullptr;
	if (m_queue.try_dequeue(pItem))
	{
		Output(pItem);
		delete pItem->ptr1;
		delete pItem->ptr2;
		delete pItem->ptr3;
		delete pItem;
		return true;
	}
	return false;
}

void CMsgQueue::StartThread()
{
    if(nullptr == m_hThread)
    {
        m_bRunning = true;
        m_hThread = new thread(ProcessThread,this);
    }
}

void CMsgQueue::StopThread()
{
    m_bRunning = false;
	lock_guard<mutex> cl(m_mtx);
    if(m_hThread)
    {
        m_hThread->join();
        delete m_hThread;
        m_hThread = nullptr;
    }
}

void CMsgQueue::RunInThread()
{
	while (m_bRunning)
	{
		if (Process())
		{
		}
		else
		{
			// 空闲时等1ms,如果立即有事件过来就晚了1ms
			// this_thread::sleep_for(chrono::milliseconds(1));

			// 空闲时过来等1ms,没等到就回去再试
			// 如过正好等到了，就立即去试，应当会快一点吧?
			unique_lock<mutex> lck(m_mtx);
			m_cv.wait_for(lck, std::chrono::milliseconds(1));
		}
	}

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}