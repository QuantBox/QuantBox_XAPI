#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include "TypeConvert.h"

#include <cstring>
#include <assert.h>

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi1;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_ReqUserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryTradingAccountField:
		iRet = _ReqQryTradingAccount(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryInvestorPositionField:
		iRet = _ReqQryInvestorPosition(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryInstrumentField:
		iRet = _ReqQryInstrument(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	default:
		break;
	}

	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		m_msgQueue_Query->Input(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void CTraderApi::Register(void* pCallback)
{
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query);
	m_msgQueue->Register(pCallback);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
	}
}

CTraderApi::CTraderApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

bool CTraderApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pRspInfo->ErrorID;
		strcpy(field.ErrorMsg, pRspInfo->ErrorMsg);

		m_msgQueue->Input(ResponeType::OnRtnError, m_msgQueue, this, bIsLast, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CTraderApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input(RequestType::E_Init, this, nullptr, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Init()
{
	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(nullptr));
	sprintf(pszPath, "%s/%s/%s/Td/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	m_pApi = CSecurityFtdcTraderApi::CreateFtdcTraderApi(pszPath);
	delete[] pszPath;

	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		m_pApi->RegisterSpi(this);

		//添加地址
		size_t len = strlen(m_ServerInfo.Address) + 1;
		char* buf = new char[len];
		strncpy(buf, m_ServerInfo.Address, len);

		char* token = strtok(buf, _QUANTBOX_SEPS_);
		while (token)
		{
			if (strlen(token)>0)
			{
				m_pApi->RegisterFront(token);
			}
			token = strtok(nullptr, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		if (m_ServerInfo.PublicTopicResumeType<ResumeType::Undefined)
			m_pApi->SubscribePublicTopic((SECURITY_TE_RESUME_TYPE)m_ServerInfo.PublicTopicResumeType);
		if (m_ServerInfo.PrivateTopicResumeType<ResumeType::Undefined)
			m_pApi->SubscribePrivateTopic((SECURITY_TE_RESUME_TYPE)m_ServerInfo.PrivateTopicResumeType);

		//初始化连接
		m_pApi->Init();
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	return 0;
}

void CTraderApi::Disconnect()
{
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_pApi)
	{
		m_pApi->RegisterSpi(nullptr);
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	m_lRequestID = 0;//由于线程已经停止，没有必要用原子操作了
}
//
//CTraderApi::SRequest* CTraderApi::MakeRequestBuf(RequestType type)
//{
//	SRequest *pRequest = new SRequest;
//	if (nullptr == pRequest)
//		return nullptr;
//
//	memset(pRequest,0,sizeof(SRequest));
//	pRequest->type = type;
//
//	return pRequest;
//}
//
//void CTraderApi::ReleaseRequestListBuf()
//{
//	lock_guard<mutex> cl(m_csList);
//	while (!m_reqList.empty())
//	{
//		SRequest * pRequest = m_reqList.front();
//		delete pRequest;
//		m_reqList.pop_front();
//	}
//}
//
//void CTraderApi::ReleaseRequestMapBuf()
//{
//	lock_guard<mutex> cl(m_csMap);
//	for (map<int,SRequest*>::iterator it=m_reqMap.begin();it!=m_reqMap.end();++it)
//	{
//		delete (*it).second;
//	}
//	m_reqMap.clear();
//}
//
//void CTraderApi::ReleaseRequestMapBuf(int nRequestID)
//{
//	lock_guard<mutex> cl(m_csMap);
//	map<int,SRequest*>::iterator it = m_reqMap.find(nRequestID);
//	if (it!=m_reqMap.end())
//	{
//		delete it->second;
//		m_reqMap.erase(nRequestID);
//	}
//}
//
//void CTraderApi::AddRequestMapBuf(int nRequestID,SRequest* pRequest)
//{
//	if(nullptr == pRequest)
//		return;
//
//	lock_guard<mutex> cl(m_csMap);
//	map<int,SRequest*>::iterator it = m_reqMap.find(nRequestID);
//	if (it!=m_reqMap.end())
//	{
//		SRequest* p = it->second;
//		if(pRequest != p)//如果实际上指的是同一内存，不再插入
//		{
//			delete p;
//			m_reqMap[nRequestID] = pRequest;
//		}
//	}
//}
//
//void CTraderApi::AddToSendQueue(SRequest * pRequest)
//{
//	if (nullptr == pRequest)
//		return;
//
//	lock_guard<mutex> cl(m_csList);
//	bool bFind = false;
//	//目前不去除相同类型的请求，即没有对大量同类型请求进行优化
//	//for (list<SRequest*>::iterator it = m_reqList.begin();it!= m_reqList.end();++it)
//	//{
//	//	if (pRequest->type == (*it)->type)
//	//	{
//	//		bFind = true;
//	//		break;
//	//	}
//	//}
//
//	if (!bFind)
//		m_reqList.push_back(pRequest);
//
//	if (!m_reqList.empty())
//	{
//		StartThread();
//	}
//}
//
//
//void CTraderApi::RunInThread()
//{
//	int iRet = 0;
//
//	while (!m_reqList.empty()&&m_bRunning)
//	{
//		SRequest * pRequest = m_reqList.front();
//		int lRequest = ++m_lRequestID;// 这个地方是否会出现原子操作的问题呢？
//		switch(pRequest->type)
//		{
//		//case E_ReqAuthenticateField:
//		//	iRet = m_pApi->ReqAuthenticate(&pRequest->ReqAuthenticateField,lRequest);
//		//	break;
//		case E_ReqUserLoginField:
//			iRet = m_pApi->ReqUserLogin(&pRequest->ReqUserLoginField,lRequest);
//			break;
//		//case E_SettlementInfoConfirmField:
//		//	iRet = m_pApi->ReqSettlementInfoConfirm(&pRequest->SettlementInfoConfirmField,lRequest);
//		//	break;
//		case E_QryInstrumentField:
//			iRet = m_pApi->ReqQryInstrument(&pRequest->QryInstrumentField,lRequest);
//			break;
//		case E_QryTradingAccountField:
//			iRet = m_pApi->ReqQryTradingAccount(&pRequest->QryTradingAccountField,lRequest);
//			break;
//		case E_QryInvestorPositionField:
//			iRet = m_pApi->ReqQryInvestorPosition(&pRequest->QryInvestorPositionField,lRequest);
//			break;
//		//case E_QryInvestorPositionDetailField:
//		//	iRet=m_pApi->ReqQryInvestorPositionDetail(&pRequest->QryInvestorPositionDetailField,lRequest);
//		//	break;
//		//case E_QryInstrumentCommissionRateField:
//		//	iRet = m_pApi->ReqQryInstrumentCommissionRate(&pRequest->QryInstrumentCommissionRateField,lRequest);
//		//	break;
//		//case E_QryInstrumentMarginRateField:
//		//	iRet = m_pApi->ReqQryInstrumentMarginRate(&pRequest->QryInstrumentMarginRateField,lRequest);
//		//	break;
//		case E_QryDepthMarketDataField:
//			iRet = m_pApi->ReqQryDepthMarketData(&pRequest->QryDepthMarketDataField,lRequest);
//			break;
//		//case E_QrySettlementInfoField:
//		//	iRet = m_pApi->ReqQrySettlementInfo(&pRequest->QrySettlementInfoField, lRequest);
//		//	break;
//		case E_QryOrderField:
//			iRet = m_pApi->ReqQryOrder(&pRequest->QryOrderField, lRequest);
//			break;
//		case E_QryTradeField:
//			iRet = m_pApi->ReqQryTrade(&pRequest->QryTradeField, lRequest);
//			break;
//		default:
//			assert(false);
//			break;
//		}
//
//		if (0 == iRet)
//		{
//			//返回成功，填加到已发送池
//			m_nSleep = 1;
//			AddRequestMapBuf(lRequest,pRequest);
//
//			lock_guard<mutex> cl(m_csList);
//			m_reqList.pop_front();
//		}
//		else
//		{
//			//失败，按4的幂进行延时，但不超过1s
//			m_nSleep *= 4;
//			m_nSleep %= 1023;
//		}
//		this_thread::sleep_for(chrono::milliseconds(m_nSleep));
//	}
//
//	// 清理线程
//	m_hThread = nullptr;
//	m_bRunning = false;
//}

void CTraderApi::OnFrontConnected()
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	ReqUserLogin();
}

void CTraderApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason,field.ErrorMsg);

	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CTraderApi::ReqUserLogin()
{
	CSecurityFtdcReqUserLoginField body = {0};

	strncpy(body.BrokerID, m_ServerInfo.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(body.UserID, m_UserInfo.UserID, sizeof(TSecurityFtdcInvestorIDType));
	strncpy(body.Password, m_UserInfo.Password, sizeof(TSecurityFtdcPasswordType));
	strncpy(body.UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TSecurityFtdcProductInfoType));

	m_msgQueue_Query->Input(RequestType::E_ReqUserLoginField, this, nullptr, 0, 0,
		&body, sizeof(CSecurityFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CSecurityFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		GetExchangeTime(pRspUserLogin->TradingDay, nullptr, pRspUserLogin->LoginTime,
			&field.TradingDay, nullptr, &field.LoginTime, nullptr);

		sprintf(field.SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin,pRspUserLogin,sizeof(CSecurityFtdcRspUserLoginField));
		m_nMaxOrderRef = atol(pRspUserLogin->MaxOrderRef);
		// 自己发单时ID从1开始，不能从0开始
		m_nMaxOrderRef = m_nMaxOrderRef>1 ? m_nMaxOrderRef : 1;
		//ReqSettlementInfoConfirm();
		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		field.ErrorID = pRspInfo->ErrorID;
		strncpy(field.ErrorMsg, pRspInfo->ErrorMsg, sizeof(ErrorMsgType));

		m_msgQueue->Input(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

char* CTraderApi::ReqOrderInsert(
	int OrderRef,
	OrderField* pOrder1,
	OrderField* pOrder2)
{
	if (nullptr == m_pApi)
		return nullptr;

	CSecurityFtdcInputOrderField body = {0};

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TSecurityFtdcInvestorIDType));

	body.MinVolume = 1;
	body.ForceCloseReason = SECURITY_FTDC_FCC_NotForceClose;
	body.IsAutoSuspend = 0;
	body.UserForceClose = 0;
	//body.IsSwapOrder = 0;

	//合约
	strncpy(body.InstrumentID, pOrder1->InstrumentID, sizeof(TSecurityFtdcInstrumentIDType));
	strncpy(body.ExchangeID, pOrder1->ExchangeID, sizeof(TSecurityFtdcExchangeIDType));
	//买卖
	body.Direction = OrderSide_2_TSecurityFtdcDirectionType(pOrder1->Side);
	//开平
	body.CombOffsetFlag[0] = OpenCloseType_2_TSecurityFtdcOffsetFlagType(pOrder1->OpenClose);
	//投保
	body.CombHedgeFlag[0] = HedgeFlagType_2_TSecurityFtdcHedgeFlagType(pOrder1->HedgeFlag);
	//数量
	body.VolumeTotalOriginal = (int)pOrder1->Qty;

	// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
	//body.LimitPrice = pOrder1->Price;
	sprintf(body.LimitPrice, "%f", pOrder1->Price);
	body.StopPrice = pOrder1->StopPx;

	// 针对第二个进行处理，如果有第二个参数，认为是交易所套利单
	if (pOrder2)
	{
		body.CombOffsetFlag[1] = OpenCloseType_2_TSecurityFtdcOffsetFlagType(pOrder1->OpenClose);
		body.CombHedgeFlag[1] = HedgeFlagType_2_TSecurityFtdcHedgeFlagType(pOrder1->HedgeFlag);
		// 交易所的移仓换月功能，没有实测过
		//body.IsSwapOrder = (body.CombOffsetFlag[0] != body.CombOffsetFlag[1]);
	}

	//价格
	//body.OrderPriceType = OrderType_2_TSecurityFtdcOrderPriceTypeType(pOrder1->Type);

	// 市价与限价
	switch (pOrder1->Type)
	{
	case Market:
	case Stop:
	case MarketOnClose:
	case TrailingStop:
		body.OrderPriceType = SECURITY_FTDC_OPT_AnyPrice;
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		break;
	case Limit:
	case StopLimit:
	case TrailingStopLimit:
	default:
		body.OrderPriceType = SECURITY_FTDC_OPT_LimitPrice;
		body.TimeCondition = SECURITY_FTDC_TC_GFD;
		break;
	}

	// IOC与FOK
	switch (pOrder1->TimeInForce)
	{
	case IOC:
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		body.VolumeCondition = SECURITY_FTDC_VC_AV;
		break;
	case FOK:
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		body.VolumeCondition = SECURITY_FTDC_VC_CV;
		//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
		break;
	default:
		body.VolumeCondition = SECURITY_FTDC_VC_AV;
		break;
	}

	// 条件单
	switch (pOrder1->Type)
	{
	case Stop:
	case TrailingStop:
	case StopLimit:
	case TrailingStopLimit:
		// 条件单没有测试，先留空
		body.ContingentCondition = SECURITY_FTDC_CC_Immediately;
		break;
	default:
		body.ContingentCondition = SECURITY_FTDC_CC_Immediately;
		break;
	}

	int nRet = 0;
	{
		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
		lock_guard<mutex> cl(m_csOrderRef);

		if (OrderRef < 0)
		{
			nRet = m_nMaxOrderRef;
			++m_nMaxOrderRef;
		}
		else
		{
			nRet = OrderRef;
		}
		sprintf(body.OrderRef, "%d", nRet);

		//不保存到队列，而是直接发送
		int n = m_pApi->ReqOrderInsert(&body, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			return nullptr;
		}
		else
		{
			sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);

			OrderField* pField = new OrderField();
			memcpy(pField, pOrder1, sizeof(OrderField));
			strcpy(pField->ID, m_orderInsert_Id);
			m_id_platform_order.insert(pair<string, OrderField*>(m_orderInsert_Id, pField));
		}
	}

	return m_orderInsert_Id;
}

void CTraderApi::OnRspOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	if (pInputOrder)
	{
		sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);
	}
	else
	{
		IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		strcpy(pField->ID, orderId);
		pField->ExecType = ExecType::ExecRejected;
		pField->Status = OrderStatus::Rejected;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(ErrorMsgType));
		m_msgQueue->Input(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	if (pInputOrder)
	{
		sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);
	}
	else
	{
		IsErrorRspInfo(pRspInfo, 0, true);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
		// LTS在开始连接时也会收到此回报，无语啊
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		strcpy(pField->ID, orderId);
		pField->ExecType = ExecType::ExecRejected;
		pField->Status = OrderStatus::Rejected;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(ErrorMsgType));
		m_msgQueue->Input(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnTrade(CSecurityFtdcTradeField *pTrade)
{
	OnTrade(pTrade,false);
}

int CTraderApi::ReqOrderAction(const string& szId)
{
	unordered_map<string, CSecurityFtdcOrderField*>::iterator it = m_id_api_order.find(szId);
	if (it == m_id_api_order.end())
	{
		// <error id="ORDER_NOT_FOUND" value="25" prompt="CTP:撤单找不到相应报单"/>
		//ErrorField field = { 0 };
		//field.ErrorID = 25;
		//sprintf(field.ErrorMsg, "ORDER_NOT_FOUND");

		////TODO:应当通过报单回报通知订单找不到

		//XRespone(ResponeType::OnRtnError, m_msgQueue, this, 0, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
		return -100;
	}
	else
	{
		// 找到了订单
		return ReqOrderAction(it->second);
	}
}

int CTraderApi::ReqOrderAction(CSecurityFtdcOrderField *pOrder)
{
	if (nullptr == m_pApi)
		return 0;

	CSecurityFtdcInputOrderActionField body = {0};

	///经纪公司代码
	strncpy(body.BrokerID, pOrder->BrokerID,sizeof(TSecurityFtdcBrokerIDType));
	///投资者代码
	strncpy(body.InvestorID, pOrder->InvestorID,sizeof(TSecurityFtdcInvestorIDType));
	///报单引用
	strncpy(body.OrderRef, pOrder->OrderRef,sizeof(TSecurityFtdcOrderRefType));
	///前置编号
	body.FrontID = pOrder->FrontID;
	///会话编号
	body.SessionID = pOrder->SessionID;
	///交易所代码
	strncpy(body.ExchangeID,pOrder->ExchangeID,sizeof(TSecurityFtdcExchangeIDType));
	///报单编号
	//strncpy(body.OrderSysID,pOrder->OrderSysID,sizeof(TSecurityFtdcOrderSysIDType));
	///操作标志
	body.ActionFlag = SECURITY_FTDC_AF_Delete;
	///合约代码
	strncpy(body.InstrumentID, pOrder->InstrumentID,sizeof(TSecurityFtdcInstrumentIDType));

	int nRet = m_pApi->ReqOrderAction(&body, ++m_lRequestID);
	return nRet;
}

void CTraderApi::OnRspOrderAction(CSecurityFtdcInputOrderActionField *pInputOrderAction, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	if (pInputOrderAction)
	{
		sprintf(orderId, "%d:%d:%s", pInputOrderAction->FrontID, pInputOrderAction->SessionID, pInputOrderAction->OrderRef);
	}
	else
	{
		IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecCancelReject;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(ErrorMsgType));
		m_msgQueue->Input(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnOrderAction(CSecurityFtdcOrderActionField *pOrderAction, CSecurityFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	if (pOrderAction)
	{
		sprintf(orderId, "%d:%d:%s", pOrderAction->FrontID, pOrderAction->SessionID, pOrderAction->OrderRef);
	}
	else
	{
		IsErrorRspInfo(pRspInfo, 0, true);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecCancelReject;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(ErrorMsgType));
		m_msgQueue->Input(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnOrder(CSecurityFtdcOrderField *pOrder)
{
	OnOrder(pOrder,false);
}

void CTraderApi::ReqQryTradingAccount()
{
	CSecurityFtdcQryTradingAccountField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	m_msgQueue_Query->Input(RequestType::E_QryTradingAccountField, this, nullptr, 0, 0,
		&body, sizeof(CSecurityFtdcQryTradingAccountField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	return m_pApi->ReqQryTradingAccount((CSecurityFtdcQryTradingAccountField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspQryTradingAccount(CSecurityFtdcTradingAccountField *pTradingAccount, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pTradingAccount)
		{
			AccountField field = { 0 };
			field.PreBalance = pTradingAccount->PreBalance;
			field.CurrMargin = pTradingAccount->CurrMargin;
			//field.CloseProfit = pTradingAccount->CloseProfit;
			//field.PositionProfit = pTradingAccount->PositionProfit;
			field.Balance = pTradingAccount->Balance;
			field.Available = pTradingAccount->Available;
			field.Deposit = pTradingAccount->Deposit;
			field.Withdraw = pTradingAccount->Withdraw;
			field.FrozenTransferFee = pTradingAccount->FrozenTransferFee;
			field.FrozenStampTax = pTradingAccount->FrozenStampTax;
			field.FrozenCommission = pTradingAccount->FrozenCommission;
			field.FrozenCash = pTradingAccount->FrozenCash;
			field.TransferFee = pTradingAccount->TransferFee;
			field.StampTax = pTradingAccount->StampTax;
			field.Commission = pTradingAccount->Commission;
			field.CashIn = pTradingAccount->CashIn;

			m_msgQueue->Input(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, &field, sizeof(AccountField), nullptr, 0, nullptr, 0);
		}
		else
		{
			m_msgQueue->Input(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::ReqQryInvestorPosition(const string& szInstrumentId, const string& szExchange)
{
	CSecurityFtdcQryInvestorPositionField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);
	strncpy(body.InstrumentID, szInstrumentId.c_str(), sizeof(TSecurityFtdcInstrumentIDType));

	m_msgQueue_Query->Input(RequestType::E_QryInvestorPositionField, this, nullptr, 0, 0,
		&body, sizeof(CSecurityFtdcQryInvestorPositionField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	return m_pApi->ReqQryInvestorPosition((CSecurityFtdcQryInvestorPositionField*)ptr1, ++m_lRequestID);
}

// 国债逆回购，持仓是Net，我归类到了Long中，如果我直接从成交中分析出来的又归到了Net中
// 所有开平和投保类型都是空
void CTraderApi::OnRspQryInvestorPosition(CSecurityFtdcInvestorPositionField *pInvestorPosition, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pInvestorPosition)
		{
			PositionIDType positionId = { 0 };
			sprintf(positionId, "%s:%s:%d:%d",
				pInvestorPosition->InstrumentID, pInvestorPosition->ExchangeID,
				TSecurityFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection), TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag));

			PositionField* pField = nullptr;
			unordered_map<string, PositionField*>::iterator it = m_id_platform_position.find(positionId);
			if (it == m_id_platform_position.end())
			{
				pField = new PositionField();
				memset(pField, 0, sizeof(PositionField));

				sprintf(pField->Symbol, "%s.%s", pInvestorPosition->InstrumentID, pInvestorPosition->ExchangeID);
				strcpy(pField->InstrumentID, pInvestorPosition->InstrumentID);
				strcpy(pField->ExchangeID, pInvestorPosition->ExchangeID);
				pField->Side = TSecurityFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection);
				pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag);

				m_id_platform_position.insert(pair<string, PositionField*>(positionId, pField));
			}
			else
			{
				pField = it->second;
			}

			pField->Position = pInvestorPosition->Position;
			pField->TdPosition = pInvestorPosition->TodayPosition;
			pField->YdPosition = pInvestorPosition->YdPosition;

			// 等数据收集全了再遍历通知一次
			if (bIsLast)
			{
				int cnt = 0;
				int count = m_id_platform_position.size();
				for (unordered_map<string, PositionField*>::iterator iter = m_id_platform_position.begin(); iter != m_id_platform_position.end(); iter++)
				{
					++cnt;
					m_msgQueue->Input(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, cnt == count, 0, iter->second, sizeof(PositionField), nullptr, 0, nullptr, 0);
				}
			}
		}
	}
}

void CTraderApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{
	CSecurityFtdcQryInstrumentField body = { 0 };

	strncpy(body.InstrumentID, szInstrumentId.c_str(), sizeof(TSecurityFtdcInstrumentIDType));
	strncpy(body.ExchangeID, szExchange.c_str(), sizeof(TSecurityFtdcExchangeIDType));

	m_msgQueue_Query->Input(RequestType::E_QryInstrumentField, this, nullptr, 0, 0,
		&body, sizeof(CSecurityFtdcQryInstrumentField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInstrument(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	return m_pApi->ReqQryInstrument((CSecurityFtdcQryInstrumentField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspQryInstrument(CSecurityFtdcInstrumentField *pInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pInstrument)
		{
			InstrumentField field = { 0 };

			strncpy(field.InstrumentID, pInstrument->InstrumentID, sizeof(InstrumentIDType));
			strncpy(field.ExchangeID, pInstrument->ExchangeID, sizeof(ExchangeIDType));

			sprintf(field.Symbol,"%s.%s",pInstrument->InstrumentID,pInstrument->ExchangeID);

			strncpy(field.InstrumentName, pInstrument->InstrumentName, sizeof(InstrumentNameType));
			field.Type = CSecurityFtdcInstrumentField_2_InstrumentType(pInstrument);
			field.VolumeMultiple = pInstrument->VolumeMultiple;
			field.PriceTick = CSecurityFtdcInstrumentField_2_PriceTick(pInstrument);
			strncpy(field.ExpireDate, pInstrument->ExpireDate, sizeof(DateType));
			field.OptionsType = CSecurityFtdcInstrumentField_2_PutCall(pInstrument);
			field.StrikePrice = pInstrument->ExecPrice;
			

			// 期权的标的物
			if (strlen(pInstrument->InstrumentID) == 8)
			{
				strncpy(field.UnderlyingInstrID, pInstrument->ExchangeInstID, 6);
				sprintf(&field.UnderlyingInstrID[6], ".%s", pInstrument->ExchangeID);
			}
			

			m_msgQueue->Input(ResponeType::OnRspQryInstrument, m_msgQueue, this, bIsLast, 0, &field, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
		}
		else
		{
			m_msgQueue->Input(ResponeType::OnRspQryInstrument, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}
//
//void CTraderApi::ReqQryInstrumentCommissionRate(const string& szInstrumentId)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentCommissionRateField);
//	if (nullptr == pRequest)
//		return;
//
//	CSecurityFtdcQryInstrumentCommissionRateField& body = pRequest->QryInstrumentCommissionRateField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TSecurityFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TSecurityFtdcInvestorIDType));
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TSecurityFtdcInstrumentIDType));
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryInstrumentCommissionRate(CSecurityFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRspQryInstrumentCommissionRate(this,pInstrumentCommissionRate,pRspInfo,nRequestID,bIsLast);
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

void CTraderApi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);
}

void CTraderApi::ReqQryOrder()
{
	CSecurityFtdcQryOrderField body = { 0 };

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TSecurityFtdcInvestorIDType));
}

void CTraderApi::OnOrder(CSecurityFtdcOrderField *pOrder, bool bFromQry)
{
	if (nullptr == pOrder)
		return;

	OrderIDType orderId = { 0 };
	sprintf(orderId, "%d:%d:%s", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
	OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单

		unordered_map<string, CSecurityFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
		if (it == m_id_api_order.end())
		{
			// 找不到此订单，表示是新单
			CSecurityFtdcOrderField* pField = new CSecurityFtdcOrderField();
			memcpy(pField, pOrder, sizeof(CSecurityFtdcOrderField));
			m_id_api_order.insert(pair<string, CSecurityFtdcOrderField*>(orderId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CSecurityFtdcOrderField* pField = it->second;
			memcpy(pField, pOrder, sizeof(CSecurityFtdcOrderField));
		}

		// 保存SysID用于定义成交回报与订单
		sprintf(orderSydId, "%s:%s", pOrder->ExchangeID, pOrder->OrderSysID);
		m_sysId_orderId.insert(pair<string, string>(orderSydId, orderId));
	}

	{
		// 从API的订单转换成自己的结构体

		OrderField* pField = nullptr;
		unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
		if (it == m_id_platform_order.end())
		{
			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			pField = new OrderField();
			memset(pField, 0, sizeof(OrderField));
			strcpy(pField->ID, orderId);
			strcpy(pField->InstrumentID, pOrder->InstrumentID);
			strcpy(pField->ExchangeID, pOrder->ExchangeID);
			pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pOrder->CombHedgeFlag[0]);
			pField->Side = TSecurityFtdcDirectionType_2_OrderSide(pOrder->Direction);
			pField->Price = atof(pOrder->LimitPrice);
			pField->StopPx = pOrder->StopPrice;
			strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
			pField->OpenClose = TSecurityFtdcOffsetFlagType_2_OpenCloseType(pOrder->CombOffsetFlag[0]);
			pField->Status = CSecurityFtdcOrderField_2_OrderStatus(pOrder);
			pField->Qty = pOrder->VolumeTotalOriginal;
			pField->Type = CSecurityFtdcOrderField_2_OrderType(pOrder);
			pField->TimeInForce = CSecurityFtdcOrderField_2_TimeInForce(pOrder);
			pField->ExecType = ExecType::ExecNew;
			strcpy(pField->OrderID, pOrder->OrderSysID);


			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
		}
		else
		{
			pField = it->second;
			strcpy(pField->ID, orderId);
			pField->LeavesQty = pOrder->VolumeTotal;
			pField->Price = atof(pOrder->LimitPrice);
			pField->Status = CSecurityFtdcOrderField_2_OrderStatus(pOrder);
			pField->ExecType = CSecurityFtdcOrderField_2_ExecType(pOrder);
			strcpy(pField->OrderID, pOrder->OrderSysID);
			strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
		}

		m_msgQueue->Input(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRspQryOrder(CSecurityFtdcOrderField *pOrder, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		OnOrder(pOrder, true);
	}
}

void CTraderApi::ReqQryTrade()
{
	CSecurityFtdcQryTradeField body = {0};

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);
}

void CTraderApi::OnTrade(CSecurityFtdcTradeField *pTrade, bool bFromQry)
{
	if (nullptr == pTrade)
		return;

	TradeField* pField = new TradeField();
	strcpy(pField->InstrumentID, pTrade->InstrumentID);
	strcpy(pField->ExchangeID, pTrade->ExchangeID);
	pField->Side = TSecurityFtdcDirectionType_2_OrderSide(pTrade->Direction);
	pField->Qty = pTrade->Volume;
	pField->Price = atof(pTrade->Price);
	pField->OpenClose = TSecurityFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	pField->Commission = 0;//TODO收续费以后要计算出来
	strncpy(pField->Time, pTrade->TradeTime, sizeof(TimeType));
	strcpy(pField->TradeID, pTrade->TradeID);

	OrderIDType orderSysId = { 0 };
	sprintf(orderSysId, "%s:%s", pTrade->ExchangeID, pTrade->OrderSysID);
	unordered_map<string, string>::iterator it = m_sysId_orderId.find(orderSysId);
	if (it == m_sysId_orderId.end())
	{
		// 此成交找不到对应的报单
		//assert(false);
	}
	else
	{
		// 找到对应的报单
		strcpy(pField->ID, it->second.c_str());

		m_msgQueue->Input(ResponeType::OnRtnTrade, m_msgQueue, this, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

		unordered_map<string, OrderField*>::iterator it2 = m_id_platform_order.find(it->second);
		if (it2 == m_id_platform_order.end())
		{
			// 此成交找不到对应的报单
			//assert(false);
		}
		else
		{
			// 更新订单的状态
			// 是否要通知接口
		}

		OnTrade(pField, bFromQry);
	}
}

void CTraderApi::OnTrade(TradeField *pTrade, bool bFromQry)
{
	PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d",
		pTrade->InstrumentID, pTrade->ExchangeID, TradeField_2_PositionSide(pTrade), pTrade->HedgeFlag);

	PositionField* pField = nullptr;
	unordered_map<string, PositionField*>::iterator it = m_id_platform_position.find(positionId);
	if (it == m_id_platform_position.end())
	{
		pField = new PositionField();
		memset(pField, 0, sizeof(PositionField));

		sprintf(pField->Symbol, "%s.%s", pTrade->InstrumentID, pTrade->ExchangeID);
		strcpy(pField->InstrumentID, pTrade->InstrumentID);
		strcpy(pField->ExchangeID, pTrade->ExchangeID);
		pField->Side = TradeField_2_PositionSide(pTrade);
		pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);

		m_id_platform_position.insert(pair<string, PositionField*>(positionId, pField));
	}
	else
	{
		pField = it->second;
	}

	if (pTrade->Side == OrderSide::Buy)
	{
		pField->Position += pTrade->Qty;
		pField->TdPosition += pTrade->Qty;
	}
	else
	{
		pField->Position -= pTrade->Qty;
		if (pTrade->OpenClose == OpenCloseType::CloseToday)
		{
			pField->TdPosition -= pTrade->Qty;
		}
		else
		{
			pField->YdPosition -= pTrade->Qty;
			// 如果昨天的被减成负数，从今天开始继续减
			if (pField->YdPosition<0)
			{
				pField->TdPosition += pField->YdPosition;
				pField->YdPosition = 0;
			}
		}

		// 计算错误，直接重新查询
		if (pField->Position < 0 || pField->TdPosition < 0 || pField->YdPosition < 0)
		{
			ReqQryInvestorPosition("", "");
			return;
		}
	}

	m_msgQueue->Input(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, false, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRspQryTrade(CSecurityFtdcTradeField *pTrade, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		OnTrade(pTrade,true);
	}
}