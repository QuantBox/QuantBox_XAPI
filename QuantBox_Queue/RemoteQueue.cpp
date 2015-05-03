#include "stdafx.h"
#include "RemoteQueue.h"


CRemoteQueue::CRemoteQueue(char* address) :CMsgQueue()
{
	m_pubisher = nullptr;

	strncpy(m_Address, address, sizeof(m_Address));
#ifdef _REMOTE
	m_ctx = nullptr;
#endif
}


CRemoteQueue::~CRemoteQueue()
{
}


void CRemoteQueue::RunInThread()
{
#ifdef _REMOTE
	m_ctx = zctx_new();
	m_pubisher = zsocket_new(m_ctx, ZMQ_PUB);
	int port = zsocket_bind(m_pubisher, m_Address);
#endif

	while (m_bRunning)
	{
		if (Process())
		{
		}
		else
		{
			// 空闲时等1ms,如果立即有事件过来就晚了1ms
			//this_thread::sleep_for(chrono::milliseconds(1));

			// 空闲时过来等1ms,没等到就回去再试
			// 如过正好等到了，就立即去试，应当会快一点吧?
			unique_lock<mutex> lck(m_mtx);
			m_cv.wait_for(lck, std::chrono::seconds(1));
		}
	}

#ifdef _REMOTE
	if (m_ctx)
	{
		zctx_destroy(&m_ctx);
		m_ctx = nullptr;
	}
#endif
	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}

void CRemoteQueue::Output(ResponeItem* pItem)
{
#ifdef _REMOTE
	// 发送数据
	if (pItem->ptr1 && pItem->size1>0)
	{
		int ret = zsocket_sendmem(m_pubisher, pItem->ptr1, pItem->size1, ZFRAME_DONTWAIT);
	}
#endif	
	return;
}