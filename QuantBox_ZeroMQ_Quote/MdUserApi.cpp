#include "stdafx.h"
#include "MdUserApi.h"
#include "../include/QueueEnum.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include <string.h>
#include <cfloat>

#include <mutex>
#include <vector>
using namespace std;

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

CMdUserApi::CMdUserApi(void)
{
	//m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	//m_msgQueue_Query = new CMsgQueue();

	//m_msgQueue_Query->Register((void*)Query, this);
	//m_msgQueue_Query->StartThread();

	m_msgQueue->m_bDirectOutput = false;

	m_hThread = nullptr;
	m_bRunning = false;
}

CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;

	/*switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_ReqUserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	default:
		break;
	}*/

	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		//m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void CMdUserApi::Register(void* pCallback,void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	//m_msgQueue_Query->Register((void*)Query,this);
	m_msgQueue->Register(pCallback,this);
	if (pCallback)
	{
		//m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
	}
	else
	{
		//m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
	}
}

ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	if (pConfigInfo)
		m_msgQueue->m_bDirectOutput = pConfigInfo->DirectOutput;
	return pConfigInfo;
}

//bool CMdUserApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
//	if(bRet)
//	{
//		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
//
//		pField->ErrorID = pRspInfo->ErrorID;
//		strcpy(pField->ErrorMsg, pRspInfo->ErrorMsg);
//
//		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
//	}
//	return bRet;
//}
//
//bool CMdUserApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
//{
//	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
//
//	return bRet;
//}

void CMdUserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	//m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	//m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
	//	nullptr, 0, nullptr, 0, nullptr, 0);

	//m_pContext = zmq_ctx_new();
	//void* p1 = zmq_socket(m_pContext, ZMQ_SUB);
	//zmq_connect(p1, m_ServerInfo.Address);
	//zmq_setsockopt(p1, ZMQ_SUBSCRIBE, "", 0);

	//m_ctx = zctx_new();

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	StartThread();
}
//
//int CMdUserApi::_Init()
//{
//	//char *pszPath = new char[m_szPath.length() + 1024];
//	//srand((unsigned int)time(NULL));
//	//sprintf(pszPath, "%s/%s/%s/Md/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
//	//makedirs(pszPath);
//
//	//m_pApi = CThostFtdcMdApi::CreateFtdcMdApi(pszPath, m_ServerInfo.IsUsingUdp, m_ServerInfo.IsMulticast);
//	//delete[] pszPath;
//
//	//m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//
//	//if (m_pApi)
//	//{
//	//	m_pApi->RegisterSpi(this);
//
//	//	//添加地址
//	//	size_t len = strlen(m_ServerInfo.Address) + 1;
//	//	char* buf = new char[len];
//	//	strncpy(buf, m_ServerInfo.Address, len);
//
//	//	char* token = strtok(buf, _QUANTBOX_SEPS_);
//	//	while (token)
//	//	{
//	//		if (strlen(token)>0)
//	//		{
//	//			m_pApi->RegisterFront(token);
//	//		}
//	//		token = strtok(NULL, _QUANTBOX_SEPS_);
//	//	}
//	//	delete[] buf;
//
//	//	//初始化连接
//	//	m_pApi->Init();
//	//	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//	//}
//
//	return 0;
//}
//
//void CMdUserApi::ReqUserLogin()
//{
//	/*CThostFtdcReqUserLoginField* pBody = (CThostFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcReqUserLoginField));
//
//	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
//	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TThostFtdcPasswordType));
//
//	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
//		pBody, sizeof(CThostFtdcReqUserLoginField), nullptr, 0, nullptr, 0);*/
//}
//
//int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
//{
//	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//	return m_pApi->ReqUserLogin((CThostFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
//}
//
void CMdUserApi::Disconnect()
{
	StopThread();

	// 清理查询队列
	//if (m_msgQueue_Query)
	//{
	//	m_msgQueue_Query->StopThread();
	//	m_msgQueue_Query->Register(nullptr,nullptr);
	//	m_msgQueue_Query->Clear();
	//	delete m_msgQueue_Query;
	//	m_msgQueue_Query = nullptr;
	//}

	//if(m_pApi)
	{
		//m_pApi->RegisterSpi(NULL);
		//m_pApi->Release();
		//m_pApi = NULL;

		// 全清理，只留最后一个
		// 由于这个dll中设计的线程在连接失败时直接退出，所以这个地方要加一个判断，防出错
		if (m_msgQueue)
		{
			m_msgQueue->Clear();
			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			// 主动触发
			m_msgQueue->Process();
		}
	}

	// 清理响应队列
	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr,nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}
}


void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	//if (nullptr == m_ctx)
	//	return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(szExchageID);
	if (it != m_mapInstrumentIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, _setInstrumentIDs);
	m_mapInstrumentIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		//m_pApi->SubscribeMarketData(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchageID)
{
	//if (nullptr == m_ctx)
	//	return;

	string szInstrumentIDs;
	for(set<string>::iterator i=instrumentIDs.begin();i!=instrumentIDs.end();++i)
	{
		szInstrumentIDs.append(*i);
		szInstrumentIDs.append(";");
	}

	if (szInstrumentIDs.length()>1)
	{
		Subscribe(szInstrumentIDs, szExchageID);
	}
}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	if (nullptr == m_ctx)
		return;

	vector<char*> vct;
	set<char*> st;

	lock_guard<mutex> cl(m_csMapInstrumentIDs);

	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(szExchageID);
	if (it != m_mapInstrumentIDs.end())
	{
		_setInstrumentIDs = it->second;
	}

	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, _setInstrumentIDs);
	m_mapInstrumentIDs[szExchageID] = _setInstrumentIDs;

	if (vct.size()>0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j<vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
		//m_pApi->UnSubscribeMarketData(pArray, (int)vct.size(), (char*)(szExchageID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

bool CMdUserApi::Contains(const string& szInstrumentID, const string& szExchageID)
{
	set<string> _setInstrumentIDs;
	map<string, set<string> >::iterator it = m_mapInstrumentIDs.find(szExchageID);
	if (it != m_mapInstrumentIDs.end())
	{
		_setInstrumentIDs = it->second;
		 set<string>::iterator it2 = _setInstrumentIDs.find(szInstrumentID);
		 if (it2 != _setInstrumentIDs.end())
		 {
			 return true;
		 }
	}
	return false;
}

//void CMdUserApi::SubscribeQuote(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, 1, m_setQuoteInstrumentIDs);
//
//	if (vct.size()>0)
//	{
//		//转成字符串数组
//		char** pArray = new char*[vct.size()];
//		for (size_t j = 0; j<vct.size(); ++j)
//		{
//			pArray[j] = vct[j];
//		}
//
//		//订阅
//		m_pApi->SubscribeForQuoteRsp(pArray, (int)vct.size());
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}
//
//void CMdUserApi::SubscribeQuote(const set<string>& instrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	string szInstrumentIDs;
//	for (set<string>::iterator i = instrumentIDs.begin(); i != instrumentIDs.end(); ++i)
//	{
//		szInstrumentIDs.append(*i);
//		szInstrumentIDs.append(";");
//	}
//
//	if (szInstrumentIDs.length()>1)
//	{
//		SubscribeQuote(szInstrumentIDs, szExchageID);
//	}
//}
//
//void CMdUserApi::UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchageID)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	vector<char*> vct;
//	set<char*> st;
//
//	lock_guard<mutex> cl(m_csMapQuoteInstrumentIDs);
//	char* pBuf = GetSetFromString(szInstrumentIDs.c_str(), _QUANTBOX_SEPS_, vct, st, -1, m_setQuoteInstrumentIDs);
//
//	if (vct.size()>0)
//	{
//		//转成字符串数组
//		char** pArray = new char*[vct.size()];
//		for (size_t j = 0; j<vct.size(); ++j)
//		{
//			pArray[j] = vct[j];
//		}
//
//		//订阅
//		m_pApi->UnSubscribeForQuoteRsp(pArray, (int)vct.size());
//
//		delete[] pArray;
//	}
//	delete[] pBuf;
//}
//
//void CMdUserApi::OnFrontConnected()
//{
//	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//
//	//连接成功后自动请求登录
//	ReqUserLogin();
//}
//
//void CMdUserApi::OnFrontDisconnected(int nReason)
//{
//	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
//	//连接失败返回的信息是拼接而成，主要是为了统一输出
//	pField->ErrorID = nReason;
//	GetOnFrontDisconnectedMsg(nReason, pField->ErrorMsg);
//
//	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
//}


void CMdUserApi::StartThread()
{
	if (nullptr == m_hThread)
	{
		m_bRunning = true;
		m_hThread = new thread(ProcessThread, this);
	}
}

void CMdUserApi::StopThread()
{
	m_bRunning = false;
	//m_cv.notify_all();
	lock_guard<mutex> cl(m_mtx_del);
	if (m_hThread)
	{
		m_hThread->join();
		delete m_hThread;
		m_hThread = nullptr;
	}
}

void CMdUserApi::RunInThread()
{
	auto ctx = zctx_new();
	auto subscriber = zsocket_new(ctx, ZMQ_SUB);
	// 只能全部订阅，部分订阅时，只能订一个，无法区分期权还是期货，只能交给上层处理
	zsocket_set_subscribe(subscriber, "");
	int ret = zsocket_connect(subscriber, m_ServerInfo.Address);
	if (ret == 0)
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		while (m_bRunning)
		{
			if (zsocket_poll(subscriber, ZMQ_POLL_MSEC))
			{
				auto zmsg = zmsg_recv(subscriber);
				if (zmsg)
				{
					int size = zmsg_size(zmsg);
					if (size > 0)
					{
						zframe_t *frame = zmsg_first(zmsg);
						// 如何把数据取出来
						DepthMarketDataField* pField = (DepthMarketDataField*)zframe_data(frame);

						// 底层的数据很多，是收到啥全提交，还是过滤一下？
						if (Contains(pField->InstrumentID, pField->ExchangeID))
						{
							m_msgQueue->Input_Copy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
						}
						else if (Contains(pField->InstrumentID, ""))
						{
							// 对股票是否会有问题？
							m_msgQueue->Input_Copy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
						}
					}
					zmsg_destroy(&zmsg);
				}
			}
		}
	}
	else
	{
		// 连接失败，报错
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->ErrorID = ret;
		strcpy(pField->ErrorMsg, m_ServerInfo.Address);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return;
	}

	if (ctx)
	{
		zctx_destroy(&ctx);
		ctx = nullptr;
	}

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}
