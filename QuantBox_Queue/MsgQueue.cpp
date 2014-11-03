#include "stdafx.h"
#include "MsgQueue.h"

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
	//m_mtx.lock();
	lock_guard<mutex> cl(m_mtx);

    m_bRunning = false;
    if(m_hThread)
    {
        m_hThread->join();
        delete m_hThread;
        m_hThread = nullptr;
    }
	//m_mtx.unlock();
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
			//挂起，等事件到来
			//m_mtx.lock();
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}