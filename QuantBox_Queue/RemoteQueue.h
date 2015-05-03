#pragma once
#include "MsgQueue.h"

// 需要将zmq和czmq的目录在Additional Include Directories中添加
#ifdef _REMOTE
#include "zmq.h"
#include "czmq.h"

#ifdef _WIN64
#else
#pragma comment(lib, "../include/ZeroMQ/x86/czmq.lib")
#endif

#endif



class DLL_PUBLIC CRemoteQueue :
	public CMsgQueue
{
public:
	CRemoteQueue(char* address);
	~CRemoteQueue();

protected:
	virtual void RunInThread();
	virtual void Output(ResponeItem* pItem);

private:

	void*		m_pubisher;

	char		m_Address[1024];
#ifdef _REMOTE
	zctx_t*		m_ctx;
#endif
};

