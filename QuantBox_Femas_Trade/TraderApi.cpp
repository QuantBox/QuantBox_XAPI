#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "TypeConvert.h"

#include <cstring>
#include <assert.h>

CTraderApi::CTraderApi(void)
{
	m_pApi = nullptr;
	m_msgQueue = nullptr;
	m_lRequestID = 0;

	m_hThread = nullptr;
	m_bRunning = false;
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

void CTraderApi::StartThread()
{
    if(nullptr == m_hThread)
    {
        m_bRunning = true;
        m_hThread = new thread(ProcessThread,this);
    }
}

void CTraderApi::StopThread()
{
    m_bRunning = false;
    if(m_hThread)
    {
        m_hThread->join();
        delete m_hThread;
        m_hThread = nullptr;
    }
}

void CTraderApi::Register(void* pMsgQueue)
{
	m_msgQueue = pMsgQueue;
}

bool CTraderApi::IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bRet)
	{
		ErrorField field = { 0 };
		field.ErrorID = pRspInfo->ErrorID;
		strcpy(field.ErrorMsg, pRspInfo->ErrorMsg);

		XRespone(ResponeType::OnRtnError, m_msgQueue, this, bIsLast, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CTraderApi::IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo)
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
	
	char *pszPath = new char[szPath.length() + 1024];
	srand((unsigned int)time(nullptr));
	sprintf(pszPath, "%s/%s/%s/Td/%d/", szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	m_pApi = CUstpFtdcTraderApi::CreateFtdcTraderApi(pszPath);
	delete[] pszPath;

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		m_pApi->RegisterSpi(this);

		//添加地址
		size_t len = strlen(m_ServerInfo.Address) + 1;
		char* buf = new char[len];
		strncpy(buf, m_ServerInfo.Address, len);

		char* token = strtok(buf, _QUANTBOX_SEPS_);
		while(token)
		{
			if (strlen(token)>0)
			{
				m_pApi->RegisterFront(token);
			}
			token = strtok(nullptr, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		m_pApi->SubscribePublicTopic((USTP_TE_RESUME_TYPE)pServerInfo->PublicTopicResumeType);
		m_pApi->SubscribePrivateTopic((USTP_TE_RESUME_TYPE)pServerInfo->PrivateTopicResumeType);
		// 如果保留，成交回报会收两条
		//m_pApi->SubscribeUserTopic((USTP_TE_RESUME_TYPE)pServerInfo->Resume);
		m_pApi->SubscribeForQuote(USTP_TERT_RESTART); //订阅询价

		//初始化连接
		m_pApi->Init();
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::Disconnect()
{
	// 如果队列中有请求包，在后面又进行了Release,又回过头来发送，可能导致当了
	StopThread();

	if(m_pApi)
	{
		m_pApi->RegisterSpi(nullptr);
		m_pApi->Release();
		m_pApi = nullptr;

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	m_lRequestID = 0;//由于线程已经停止，没有必要用原子操作了

	ReleaseRequestListBuf();
	ReleaseRequestMapBuf();
}

CTraderApi::SRequest* CTraderApi::MakeRequestBuf(RequestType type)
{
	SRequest *pRequest = new SRequest;
	if (nullptr == pRequest)
		return nullptr;

	memset(pRequest,0,sizeof(SRequest));
	pRequest->type = type;

	return pRequest;
}

void CTraderApi::ReleaseRequestListBuf()
{
	lock_guard<mutex> cl(m_csList);
	while (!m_reqList.empty())
	{
		SRequest * pRequest = m_reqList.front();
		delete pRequest;
		m_reqList.pop_front();
	}
}

void CTraderApi::ReleaseRequestMapBuf()
{
	lock_guard<mutex> cl(m_csMap);
	for (map<int,SRequest*>::iterator it=m_reqMap.begin();it!=m_reqMap.end();++it)
	{
		delete (*it).second;
	}
	m_reqMap.clear();
}

void CTraderApi::ReleaseRequestMapBuf(int nRequestID)
{
	lock_guard<mutex> cl(m_csMap);
	map<int,SRequest*>::iterator it = m_reqMap.find(nRequestID);
	if (it!=m_reqMap.end())
	{
		delete it->second;
		m_reqMap.erase(nRequestID);
	}
}

void CTraderApi::AddRequestMapBuf(int nRequestID,SRequest* pRequest)
{
	if(nullptr == pRequest)
		return;

	lock_guard<mutex> cl(m_csMap);
	map<int,SRequest*>::iterator it = m_reqMap.find(nRequestID);
	if (it!=m_reqMap.end())
	{
		SRequest* p = it->second;
		if(pRequest != p)//如果实际上指的是同一内存，不再插入
		{
			delete p;
			m_reqMap[nRequestID] = pRequest;
		}
	}
}


void CTraderApi::AddToSendQueue(SRequest * pRequest)
{
	if (nullptr == pRequest)
		return;

	lock_guard<mutex> cl(m_csList);
	bool bFind = false;
	//目前不去除相同类型的请求，即没有对大量同类型请求进行优化
	//for (list<SRequest*>::iterator it = m_reqList.begin();it!= m_reqList.end();++it)
	//{
	//	if (pRequest->type == (*it)->type)
	//	{
	//		bFind = true;
	//		break;
	//	}
	//}

	if (!bFind)
		m_reqList.push_back(pRequest);

	if (!m_reqList.empty())
	{
		StartThread();
	}
}


void CTraderApi::RunInThread()
{
	int iRet = 0;

	while (!m_reqList.empty()&&m_bRunning)
	{
		SRequest * pRequest = m_reqList.front();
		int lRequest = ++m_lRequestID;// 这个地方是否会出现原子操作的问题呢？
		switch(pRequest->type)
		{
		case E_ReqUserLoginField:
			iRet = m_pApi->ReqUserLogin(&pRequest->ReqUserLoginField,lRequest);
			break;
		case E_QryUserInvestorField:
			iRet = m_pApi->ReqQryUserInvestor(&pRequest->QryUserInvestorField, lRequest);
			break;
		case E_QryInstrumentField:
			iRet = m_pApi->ReqQryInstrument(&pRequest->QryInstrumentField,lRequest);
			break;
		case E_QryInvestorAccountField:
			iRet = m_pApi->ReqQryInvestorAccount(&pRequest->QryInvestorAccountField, lRequest);
			break;
		case E_QryInvestorPositionField:
			iRet = m_pApi->ReqQryInvestorPosition(&pRequest->QryInvestorPositionField,lRequest);
			break;
		case E_QryInvestorFeeField:
			iRet = m_pApi->ReqQryInvestorFee(&pRequest->QryInvestorFeeField, lRequest);
			break;
		case E_QryInvestorMarginField:
			iRet = m_pApi->ReqQryInvestorMargin(&pRequest->QryInvestorMarginField, lRequest);
			break;
		case E_QryOrderField:
			iRet = m_pApi->ReqQryOrder(&pRequest->QryOrderField, lRequest);
			break;
		case E_QryTradeField:
			iRet = m_pApi->ReqQryTrade(&pRequest->QryTradeField, lRequest);
			break;
		default:
			assert(false);
			break;
		}

		if (0 == iRet)
		{
			//返回成功，填加到已发送池
			m_nSleep = 1;
			AddRequestMapBuf(lRequest,pRequest);

			lock_guard<mutex> cl(m_csList);
			m_reqList.pop_front();
		}
		else
		{
			//失败，按4的幂进行延时，但不超过1s
			m_nSleep *= 4;
			m_nSleep %= 1023;
		}
		this_thread::sleep_for(chrono::milliseconds(m_nSleep));
	}

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}

void CTraderApi::OnFrontConnected()
{
	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	ReqUserLogin();
}

void CTraderApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason,field.ErrorMsg);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

}

void CTraderApi::ReqUserLogin()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_ReqUserLoginField);
	if (pRequest)
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		CUstpFtdcReqUserLoginField& body = pRequest->ReqUserLoginField;

		strncpy(body.UserID, m_UserInfo.UserID, sizeof(TUstpFtdcInvestorIDType));
		strncpy(body.BrokerID, m_ServerInfo.BrokerID, sizeof(TUstpFtdcBrokerIDType));
		strncpy(body.Password, m_UserInfo.Password, sizeof(TUstpFtdcPasswordType));
		strncpy(body.UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TUstpFtdcProductInfoType));

		AddToSendQueue(pRequest);
	}
}

void CTraderApi::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		strncpy(field.TradingDay, pRspUserLogin->TradingDay, sizeof(DateType));
		strncpy(field.LoginTime, pRspUserLogin->LoginTime, sizeof(TimeType));
		//sprintf(field.SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		memcpy(&m_RspUserLogin__, &field, sizeof(RspUserLoginField));

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin,pRspUserLogin,sizeof(CUstpFtdcRspUserLoginField));
		long long x = 0;
		for (int i = 0; i<12; ++i)//这地方为何一定要用12
		{
			if (m_RspUserLogin.MaxOrderLocalID[i] == 0)
			{
				m_RspUserLogin.MaxOrderLocalID[i] = '0';
			}
			long long b = (m_RspUserLogin.MaxOrderLocalID[i] - '0');
			x = x * 10L + b;
		}
		
		m_nMaxOrderRef = x + 1;
		ReqQryUserInvestor();
	}
	else
	{
		field.ErrorID = pRspInfo->ErrorID;
		strcpy(field.ErrorMsg, pRspInfo->ErrorMsg);

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryUserInvestor()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryUserInvestorField);
	if (pRequest)
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Doing, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		CUstpFtdcQryUserInvestorField& body = pRequest->QryUserInvestorField;

		strncpy(body.BrokerID, m_ServerInfo.BrokerID, sizeof(TUstpFtdcBrokerIDType));
		strncpy(body.UserID, m_UserInfo.UserID, sizeof(TUstpFtdcInvestorIDType));

		AddToSendQueue(pRequest);
	}
}

void CTraderApi::OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&& pRspUserInvestor)
	{		
		memcpy(&m_RspUserInvestor, pRspUserInvestor, sizeof(CUstpFtdcRspUserInvestorField));

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		field.ErrorID = pRspInfo->ErrorID;
		strncpy(field.ErrorMsg, pRspInfo->ErrorMsg, sizeof(pRspInfo->ErrorMsg));

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

char* CTraderApi::ReqOrderInsert(
	int OrderRef,
	OrderField* pOrder1)
{
	if (nullptr == m_pApi)
		return nullptr;

	SRequest* pRequest = MakeRequestBuf(E_InputOrderField);
	if (nullptr == pRequest)
		return nullptr;

	CUstpFtdcInputOrderField& body = pRequest->InputOrderField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);

	strcpy(body.ExchangeID, "CFFEX");

	body.MinVolume = 1;
	body.ForceCloseReason = USTP_FTDC_FCR_NotForceClose;
	body.IsAutoSuspend = 0;
	//body.UserForceClose = 0;
	//body.IsSwapOrder = 0;

	//合约
	strncpy(body.InstrumentID, pOrder1->InstrumentID, sizeof(TUstpFtdcInstrumentIDType));
	//买卖
	body.Direction = OrderSide_2_TUstpFtdcDirectionType(pOrder1->Side);
	//开平
	body.OffsetFlag = OpenCloseType_2_TUstpFtdcOffsetFlagType(pOrder1->OpenClose);
	//投保
	body.HedgeFlag = HedgeFlagType_2_TUstpFtdcHedgeFlagType(pOrder1->HedgeFlag);
	//数量
	body.Volume = (int)pOrder1->Qty;

	// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
	body.LimitPrice = pOrder1->Price;
	body.StopPrice = pOrder1->StopPx;


	// 市价与限价
	switch (pOrder1->Type)
	{
	case Market:
	case Stop:
	case MarketOnClose:
	case TrailingStop:
		body.OrderPriceType = USTP_FTDC_OPT_AnyPrice;
		body.TimeCondition = USTP_FTDC_TC_IOC;
		break;
	case Limit:
	case StopLimit:
	case TrailingStopLimit:
	default:
		body.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		body.TimeCondition = USTP_FTDC_TC_GFD;
		break;
	}

	// IOC与FOK
	switch (pOrder1->TimeInForce)
	{
	case IOC:
		body.TimeCondition = USTP_FTDC_TC_IOC;
		body.VolumeCondition = USTP_FTDC_VC_AV;
		break;
	case FOK:
		body.TimeCondition = USTP_FTDC_TC_IOC;
		body.VolumeCondition = USTP_FTDC_VC_CV;
		//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
		break;
	default:
		body.VolumeCondition = USTP_FTDC_VC_AV;
		break;
	}

	long long nRet = 0;
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
		sprintf(body.UserOrderLocalID, "%012lld", nRet);

		//不保存到队列，而是直接发送
		int n = m_pApi->ReqOrderInsert(&pRequest->InputOrderField, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			delete pRequest;
			return nullptr;
		}
		else
		{
			// 用于各种情况下找到原订单，用于进行响应的通知
			sprintf(m_orderInsert_Id, "%s:%012lld", m_RspUserLogin__.SessionID, nRet);

			OrderField* pField = new OrderField();
			memcpy(pField, pOrder1, sizeof(OrderField));
			strcpy(pField->ID, m_orderInsert_Id);
			m_id_platform_order.insert(pair<string, OrderField*>(m_orderInsert_Id, pField));
		}
	}
	delete pRequest;//用完后直接删除

	return m_orderInsert_Id;
}

void CTraderApi::OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	// 只要下单会就第一步到这，并不代表柜台拒绝
	OrderIDType orderId = { 0 };
	sprintf(orderId, "%s:%s", m_RspUserLogin__.SessionID, pInputOrder->UserOrderLocalID);

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		if (pRspInfo->ErrorID == 0)
		{
			OrderField* pField = it->second;
			pField->ExecType = ExecType::ExecNew;
			pField->Status = OrderStatus::New;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspOrderInsert");
			XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			OrderField* pField = it->second;
			pField->ExecType = ExecType::ExecRejected;
			pField->Status = OrderStatus::Rejected;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspOrderInsert");
			XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	sprintf(orderId, "%s:%s", m_RspUserLogin__.SessionID, pInputOrder->UserOrderLocalID);

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecRejected;
		pField->Status = OrderStatus::Rejected;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
		strcat(pField->Text, "OnErrRtnOrderInsert");
		XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnTrade(CUstpFtdcTradeField *pTrade)
{
	OnTrade(pTrade);
}

int CTraderApi::ReqOrderAction(const string& szId)
{
	unordered_map<string, CUstpFtdcOrderField*>::iterator it = m_id_api_order.find(szId);
	if (it == m_id_api_order.end())
	{
		// <error id="ORDER_NOT_FOUND" value="25" prompt="CTP:撤单找不到相应报单"/>
		return -100;
	}
	else
	{
		// 找到了订单
		return ReqOrderAction(it->second);
	}
}

int CTraderApi::ReqOrderAction(CUstpFtdcOrderField *pOrder)
{
	if (nullptr == m_pApi)
		return 0;

	SRequest* pRequest = MakeRequestBuf(E_OrderActionField);
	if (nullptr == pRequest)
		return 0;

	CUstpFtdcOrderActionField& body = pRequest->OrderActionField;

	strcpy(body.BrokerID, pOrder->BrokerID);
	strcpy(body.InvestorID, pOrder->InvestorID);
	strcpy(body.UserID, pOrder->UserID);

	///报单引用
	strcpy(body.UserOrderLocalID, pOrder->UserOrderLocalID);
	
	///交易所代码
	strcpy(body.ExchangeID,pOrder->ExchangeID);
	///报单编号
	strcpy(body.OrderSysID,pOrder->OrderSysID);
	///操作标志
	body.ActionFlag = USTP_FTDC_AF_Delete;

	int nRet = 0;
	{
		lock_guard<mutex> cl(m_csOrderRef);
		sprintf(body.UserOrderActionLocalID, "%012lld", m_nMaxOrderRef);
		++m_nMaxOrderRef;
		nRet = m_pApi->ReqOrderAction(&pRequest->OrderActionField, ++m_lRequestID);
	}

	delete pRequest;
	return nRet;
}

void CTraderApi::OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	sprintf(orderId, "%s:%s", m_RspUserLogin__.SessionID, pOrderAction->UserOrderLocalID);

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		if (pRspInfo->ErrorID == 0)
		{
			OrderField* pField = it->second;
			strcpy(pField->ID, orderId);
			pField->ExecType = ExecType::ExecCancelled;
			pField->Status = OrderStatus::Cancelled;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspOrderAction");
			XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			// 找到了，要更新状态
			// 得使用上次的状态
			OrderField* pField = it->second;
			strcpy(pField->ID, orderId);
			pField->ExecType = ExecType::ExecCancelReject;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspOrderAction");
			XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	sprintf(orderId, "%s:%s", m_RspUserLogin__.SessionID, pOrderAction->UserOrderLocalID);

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		strcpy(pField->ID, orderId);
		pField->ExecType = ExecType::ExecCancelReject;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
		strcat(pField->Text, "OnErrRtnOrderAction");
		XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnOrder(CUstpFtdcOrderField *pOrder)
{
	OnOrder(pOrder);
}

char* CTraderApi::ReqQuoteInsert(
	int QuoteRef,
	QuoteField* pQuote)
{
	if (nullptr == m_pApi)
		return 0;

	SRequest* pRequest = MakeRequestBuf(E_InputQuoteField);
	if (nullptr == pRequest)
		return 0;

	CUstpFtdcInputQuoteField& body = pRequest->InputQuoteField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);

	//合约,目前只从订单1中取
	strncpy(body.InstrumentID, pQuote->InstrumentID, sizeof(TUstpFtdcInstrumentIDType));
	//开平
	body.AskOffsetFlag = OpenCloseType_2_TUstpFtdcOffsetFlagType(pQuote->AskOpenClose);
	body.BidOffsetFlag = OpenCloseType_2_TUstpFtdcOffsetFlagType(pQuote->BidOpenClose);
	//投保
	body.AskHedgeFlag = HedgeFlagType_2_TUstpFtdcHedgeFlagType(pQuote->AskHedgeFlag);
	body.BidHedgeFlag = HedgeFlagType_2_TUstpFtdcHedgeFlagType(pQuote->BidHedgeFlag);

	//价格
	body.AskPrice = pQuote->AskPrice;
	body.BidPrice = pQuote->BidPrice;

	//数量
	body.AskVolume = (int)pQuote->AskQty;
	body.BidVolume = (int)pQuote->BidQty;

	long long nRet = 0;
	{
		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
		lock_guard<mutex> cl(m_csOrderRef);

		if (QuoteRef < 0)
		{
			nRet = m_nMaxOrderRef;
			sprintf(body.UserQuoteLocalID, "%012lld", m_nMaxOrderRef);
			sprintf(body.AskUserOrderLocalID, "%012lld", m_nMaxOrderRef);
			sprintf(body.BidUserOrderLocalID, "%012lld", ++m_nMaxOrderRef);
			++m_nMaxOrderRef;
		}
		else
		{
			nRet = QuoteRef;
			sprintf(body.UserQuoteLocalID, "%012lld", QuoteRef);
			sprintf(body.AskUserOrderLocalID, "%012lld", QuoteRef);
			sprintf(body.BidUserOrderLocalID, "%012lld", ++QuoteRef);
			++QuoteRef;
		}

		//不保存到队列，而是直接发送
		int n = m_pApi->ReqQuoteInsert(&pRequest->InputQuoteField, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			delete pRequest;
			return nullptr;
		}
		else
		{
			// 用于各种情况下找到原订单，用于进行响应的通知
			sprintf(m_orderInsert_Id, "%s:%012lld", m_RspUserLogin__.SessionID, nRet);

			QuoteField* pField = new QuoteField();
			memcpy(pField, pQuote, sizeof(QuoteField));
			strcpy(pField->ID, m_orderInsert_Id);
			strcpy(pField->AskID, m_orderInsert_Id);
			sprintf(pField->BidID, "%s:%012lld", m_RspUserLogin__.SessionID, nRet + 1);

			m_id_platform_quote.insert(pair<string, QuoteField*>(m_orderInsert_Id, pField));
		}
	}
	delete pRequest;//用完后直接删除

	return m_orderInsert_Id;
}

void CTraderApi::OnRspQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%s:%s", m_RspUserLogin__.SessionID, pInputQuote->UserQuoteLocalID);

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		if (pRspInfo->ErrorID == 0)
		{
			QuoteField* pField = it->second;
			//strcpy(pField->ID, quoteId);
			pField->ExecType = ExecType::ExecNew;
			pField->Status = OrderStatus::New;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspQuoteInsert");
			XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
		}
		else
		{
			QuoteField* pField = it->second;
			//strcpy(pField->ID, quoteId);
			pField->ExecType = ExecType::ExecRejected;
			pField->Status = OrderStatus::Rejected;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspQuoteInsert");
			XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnQuoteInsert(CUstpFtdcInputQuoteField *pInputQuote, CUstpFtdcRspInfoField *pRspInfo)
{
	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%s:%s", m_RspUserLogin__.SessionID, pInputQuote->UserQuoteLocalID);

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		QuoteField* pField = it->second;
		//strcpy(pField->ID, quoteId);
		pField->ExecType = ExecType::ExecRejected;
		pField->Status = OrderStatus::Rejected;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
		strcat(pField->Text, "OnErrRtnQuoteInsert");
		XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnQuote(CUstpFtdcRtnQuoteField *pQuote)
{
	OnQuote(pQuote);
}

int CTraderApi::ReqQuoteAction(const string& szId)
{
	unordered_map<string, CUstpFtdcRtnQuoteField*>::iterator it = m_id_api_quote.find(szId);
	if (it == m_id_api_quote.end())
	{
		//// <error id="QUOTE_NOT_FOUND" value="86" prompt="CTP:报价撤单找不到相应报价"/>
		return -100;
	}
	else
	{
		// 找到了订单
		ReqQuoteAction(it->second);
	}
	return 0;
}

int CTraderApi::ReqQuoteAction(CUstpFtdcRtnQuoteField *pQuote)
{
	if (nullptr == m_pApi)
		return 0;

	SRequest* pRequest = MakeRequestBuf(E_QuoteActionField);
	if (nullptr == pRequest)
		return 0;

	CUstpFtdcQuoteActionField& body = pRequest->QuoteActionField;

	strcpy(body.BrokerID, pQuote->BrokerID);
	strcpy(body.InvestorID, pQuote->InvestorID);
	strcpy(body.UserID, pQuote->UserID);

	///报单引用
	strcpy(body.UserQuoteLocalID, pQuote->UserQuoteLocalID);

	///交易所代码
	strcpy(body.ExchangeID, pQuote->ExchangeID);
	///报单编号
	strcpy(body.QuoteSysID, pQuote->QuoteSysID);
	///操作标志
	body.ActionFlag = USTP_FTDC_AF_Delete;
	///合约代码
	int nRet = 0;
	{
		lock_guard<mutex> cl(m_csOrderRef);
		sprintf(body.UserQuoteActionLocalID, "%012lld", m_nMaxOrderRef);
		++m_nMaxOrderRef;
		nRet = m_pApi->ReqQuoteAction(&pRequest->QuoteActionField, ++m_lRequestID);
	}

	delete pRequest;
	return nRet;
}

void CTraderApi::OnRspQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%s:%s", m_RspUserLogin__.SessionID, pQuoteAction->UserQuoteLocalID);

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		if (pRspInfo->ErrorID == 0)
		{
			QuoteField* pField = it->second;
			strcpy(pField->ID, quoteId);
			pField->ExecType = ExecType::ExecCancelled;
			pField->Status = OrderStatus::Cancelled;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspQuoteAction");
			XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
		}
		else
		{
			// 找到了，要更新状态
			// 得使用上次的状态
			QuoteField* pField = it->second;
			strcpy(pField->ID, quoteId);
			pField->ExecType = ExecType::ExecCancelReject;
			pField->ErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
			strcat(pField->Text, "OnRspQuoteAction");
			XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnQuoteAction(CUstpFtdcQuoteActionField *pQuoteAction, CUstpFtdcRspInfoField *pRspInfo)
{
	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%s:%s", m_RspUserLogin__.SessionID, pQuoteAction->UserQuoteLocalID);

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		QuoteField* pField = it->second;
		strcpy(pField->ID, quoteId);
		pField->ExecType = ExecType::ExecCancelReject;
		pField->ErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TUstpFtdcErrorMsgType));
		strcat(pField->Text, "OnErrRtnQuoteAction");
		XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::ReqQryInvestorAccount()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInvestorAccountField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryInvestorAccountField& body = pRequest->QryInvestorAccountField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);

	AddToSendQueue(pRequest);
}

void CTraderApi::OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pRspInvestorAccount)
		{
			AccountField field = { 0 };
			field.PreBalance = pRspInvestorAccount->PreBalance;
			field.CurrMargin = pRspInvestorAccount->Margin;
			field.Commission = pRspInvestorAccount->Fee;
			field.CloseProfit = pRspInvestorAccount->CloseProfit;
			field.PositionProfit = pRspInvestorAccount->PositionProfit;
			//field.Balance = pRspInvestorAccount->DynamicRights;
			field.Available = pRspInvestorAccount->Available;

			XRespone(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, &field, sizeof(AccountField), nullptr, 0, nullptr, 0);
		}
		else
		{
			XRespone(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryInvestorPosition(const string& szInstrumentId)
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInvestorPositionField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryInvestorPositionField& body = pRequest->QryInvestorPositionField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);

	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TUstpFtdcInstrumentIDType));

	AddToSendQueue(pRequest);
}

void CTraderApi::OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pRspInvestorPosition)
		{
			PositionField field = { 0 };

			strcpy(field.Symbol, pRspInvestorPosition->InstrumentID);
			strcpy(field.InstrumentID, pRspInvestorPosition->InstrumentID);
			strcpy(field.ExchangeID, pRspInvestorPosition->ExchangeID);

			field.Side = TUstpFtdcDirectionType_2_PositionSide(pRspInvestorPosition->Direction);
			field.HedgeFlag = TUstpFtdcHedgeFlagType_2_HedgeFlagType(pRspInvestorPosition->HedgeFlag);
			field.Position = pRspInvestorPosition->Position;
			// 今仓与昨仓是什么关系，如何计算的？
			field.TdPosition = pRspInvestorPosition->Position - pRspInvestorPosition->YdPosition;
			field.YdPosition = pRspInvestorPosition->YdPosition;

			XRespone(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, bIsLast, 0, &field, sizeof(PositionField), nullptr, 0, nullptr, 0);
		}
		else
		{
			XRespone(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryInstrumentField& body = pRequest->QryInstrumentField;

	strncpy(body.ExchangeID, szExchange.c_str(), sizeof(TUstpFtdcExchangeIDType));
	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TUstpFtdcInstrumentIDType));
	

	AddToSendQueue(pRequest);
}


void CTraderApi::OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pRspInstrument)
		{
			InstrumentField field = { 0 };

			strcpy(field.InstrumentID, pRspInstrument->InstrumentID);
			strcpy(field.ExchangeID, pRspInstrument->ExchangeID);

			strcpy(field.Symbol, pRspInstrument->InstrumentID);

			strcpy(field.InstrumentName, pRspInstrument->InstrumentName);
			field.Type = CUstpFtdcRspInstrumentField_2_InstrumentType(pRspInstrument);
			field.VolumeMultiple = pRspInstrument->VolumeMultiple;
			field.PriceTick = pRspInstrument->PriceTick;
			strcpy(field.ExpireDate, pRspInstrument->ExpireDate);
			field.OptionsType = TUstpFtdcOptionsTypeType_2_PutCall(pRspInstrument->OptionsType);

			XRespone(ResponeType::OnRspQryInstrument, m_msgQueue, this, bIsLast, 0, &field, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
		}
		else
		{
			XRespone(ResponeType::OnRspQryInstrument, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryInvestorFee(const string& szInstrumentId)
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInvestorFeeField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryInvestorFeeField& body = pRequest->QryInvestorFeeField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);
	
	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TUstpFtdcInstrumentIDType));

	AddToSendQueue(pRequest);
}

void CTraderApi::OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}
//
//void CTraderApi::ReqQryInstrumentMarginRate(const string& szInstrumentId,TUstpFtdcHedgeFlagType HedgeFlag)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentMarginRateField);
//	if (nullptr == pRequest)
//		return;
//
//	CUstpFtdcQryInstrumentMarginRateField& body = pRequest->QryInstrumentMarginRateField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TUstpFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TUstpFtdcInvestorIDType));
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TUstpFtdcInstrumentIDType));
//	body.HedgeFlag = HedgeFlag;
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryInstrumentMarginRate(CUstpFtdcInstrumentMarginRateField *pInstrumentMarginRate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRspQryInstrumentMarginRate(this,pInstrumentMarginRate,pRspInfo,nRequestID,bIsLast);
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

void CTraderApi::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryOrder()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryOrderField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryOrderField& body = pRequest->QryOrderField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);

	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);

	AddToSendQueue(pRequest);
}

void CTraderApi::OnOrder(CUstpFtdcOrderField *pOrder)
{
	if (nullptr == pOrder)
		return;

	OrderIDType orderId = { 0 };
	sprintf(orderId, "%s:%s", m_RspUserLogin__.SessionID, pOrder->UserOrderLocalID);
	//OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单
		unordered_map<string, CUstpFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
		if (it == m_id_api_order.end())
		{
			// 找不到此订单，表示是新单
			CUstpFtdcOrderField* pField = new CUstpFtdcOrderField();
			memcpy(pField, pOrder, sizeof(CUstpFtdcOrderField));
			m_id_api_order.insert(pair<string, CUstpFtdcOrderField*>(orderId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CUstpFtdcOrderField* pField = it->second;
			memcpy(pField, pOrder, sizeof(CUstpFtdcOrderField));
		}

		//// 保存SysID用于定义成交回报与订单
		//sprintf(orderSydId, "%s:%s", pOrder->ExchangeID, pOrder->OrderSysID);
		//m_sysId_orderId.insert(pair<string, string>(orderSydId, orderId));
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
			pField->HedgeFlag = TUstpFtdcHedgeFlagType_2_HedgeFlagType(pOrder->HedgeFlag);
			pField->Side = TUstpFtdcDirectionType_2_OrderSide(pOrder->Direction);
			pField->Price = pOrder->LimitPrice;
			pField->StopPx = pOrder->StopPrice;
			//strcpy(pField->Text, pOrder->StatusMsg);
			pField->OpenClose = TUstpFtdcOffsetFlagType_2_OpenCloseType(pOrder->OffsetFlag);
			pField->Status = CUstpFtdcOrderField_2_OrderStatus(pOrder);
			pField->Qty = pOrder->Volume;
			pField->Type = CUstpFtdcOrderField_2_OrderType(pOrder);
			pField->TimeInForce = CUstpFtdcOrderField_2_TimeInForce(pOrder);
			pField->ExecType = ExecType::ExecNew;
			strcpy(pField->OrderID, pOrder->OrderSysID);
			strcat(pField->Text, "OnRtnOrder");


			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
		}
		else
		{
			pField = it->second;
			strcpy(pField->ID, orderId);
			pField->LeavesQty = pOrder->VolumeRemain;
			pField->Price = pOrder->LimitPrice;
			pField->Status = CUstpFtdcOrderField_2_OrderStatus(pOrder);
			pField->ExecType = CUstpFtdcOrderField_2_ExecType(pOrder);
			strcpy(pField->OrderID, pOrder->OrderSysID);
			strcat(pField->Text, "OnRtnOrder");
		}

		XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		OnOrder(pOrder);
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryTrade()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryTradeField);
	if (nullptr == pRequest)
		return;

	CUstpFtdcQryTradeField& body = pRequest->QryTradeField;

	strcpy(body.BrokerID, m_RspUserInvestor.BrokerID);
	strcpy(body.UserID, m_RspUserInvestor.UserID);
	strcpy(body.InvestorID, m_RspUserInvestor.InvestorID);

	AddToSendQueue(pRequest);
}

void CTraderApi::OnTrade(CUstpFtdcTradeField *pTrade)
{
	if (nullptr == pTrade)
		return;

	TradeField* pField = new TradeField();

	strcpy(pField->InstrumentID, pTrade->InstrumentID);
	strcpy(pField->ExchangeID, pTrade->ExchangeID);
	pField->Side = TUstpFtdcDirectionType_2_OrderSide(pTrade->Direction);
	pField->Qty = pTrade->TradeVolume;
	pField->Price = pTrade->TradePrice;
	pField->OpenClose = TUstpFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	pField->HedgeFlag = TUstpFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	pField->Commission = 0;//TODO收续费以后要计算出来
	strcpy(pField->Time, pTrade->TradeTime);
	strcpy(pField->TradeID, pTrade->TradeID);

	//OrderIDType orderSysId = { 0 };
	//sprintf(orderSysId, "%s:%s", pTrade->ExchangeID, pTrade->OrderSysID);
	//hash_map<string, string>::iterator it = m_sysId_orderId.find(orderSysId);
	//if (it == m_sysId_orderId.end())
	//{
	//	// 此成交找不到对应的报单
	//	assert(false);
	//}
	//else
	//{
		// 找到对应的报单
		//strcpy(pField->ID, it->second.c_str());
		//strcpy(pField->ID, pTrade->UserOrderLocalID);
		sprintf(pField->ID, "%s:%s", m_RspUserLogin__.SessionID, pTrade->UserOrderLocalID);
		

		XRespone(ResponeType::OnRtnTrade, m_msgQueue, this, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

		unordered_map<string, OrderField*>::iterator it2 = m_id_platform_order.find(pField->ID);
		if (it2 == m_id_platform_order.end())
		{
			// 此成交找不到对应的报单
			assert(false);
		}
		else
		{
			// 更新订单的状态
			// 是否要通知接口
		}
	//}
}

void CTraderApi::OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		OnTrade(pTrade);
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

//void CTraderApi::ReqQryQuote()
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryQuoteField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQryQuoteField& body = pRequest->QryQuoteField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//
//	AddToSendQueue(pRequest);
//}

void CTraderApi::OnQuote(CUstpFtdcRtnQuoteField *pQuote)
{
	if (nullptr == pQuote)
		return;

	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%s:%s", m_RspUserLogin__.SessionID, pQuote->UserQuoteLocalID);
	OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单

		unordered_map<string, CUstpFtdcRtnQuoteField*>::iterator it = m_id_api_quote.find(quoteId);
		if (it == m_id_api_quote.end())
		{
			// 找不到此订单，表示是新单
			CUstpFtdcRtnQuoteField* pField = new CUstpFtdcRtnQuoteField();
			memcpy(pField, pQuote, sizeof(CUstpFtdcRtnQuoteField));
			m_id_api_quote.insert(pair<string, CUstpFtdcRtnQuoteField*>(quoteId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CUstpFtdcRtnQuoteField* pField = it->second;
			memcpy(pField, pQuote, sizeof(CUstpFtdcRtnQuoteField));
		}

		// 这个地方是否要进行其它处理？

		// 保存SysID用于定义成交回报与订单
		//sprintf(orderSydId, "%s:%s", pQuote->ExchangeID, pQuote->QuoteSysID);
		//m_sysId_quoteId.insert(pair<string, string>(orderSydId, quoteId));
	}

	{
		// 从API的订单转换成自己的结构体

		QuoteField* pField = nullptr;
		unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
		if (it == m_id_platform_quote.end())
		{
			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			pField = new QuoteField();
			memset(pField, 0, sizeof(QuoteField));
			strcpy(pField->InstrumentID, pQuote->InstrumentID);
			strcpy(pField->ExchangeID, pQuote->ExchangeID);

			pField->AskQty = pQuote->AskVolume;
			pField->AskPrice = pQuote->AskPrice;
			pField->AskOpenClose = TUstpFtdcOffsetFlagType_2_OpenCloseType(pQuote->AskOffsetFlag);
			pField->AskHedgeFlag = TUstpFtdcHedgeFlagType_2_HedgeFlagType(pQuote->AskHedgeFlag);

			pField->BidQty = pQuote->BidVolume;
			pField->BidPrice = pQuote->BidPrice;
			pField->BidOpenClose = TUstpFtdcOffsetFlagType_2_OpenCloseType(pQuote->BidOffsetFlag);
			pField->BidHedgeFlag = TUstpFtdcHedgeFlagType_2_HedgeFlagType(pQuote->BidHedgeFlag);

			strcpy(pField->ID, quoteId);
			strcpy(pField->AskOrderID, pQuote->AskOrderSysID);
			strcpy(pField->BidOrderID, pQuote->BidOrderSysID);

			//strncpy(pField->Text, pQuote->, sizeof(TThostFtdcErrorMsgType));

			//pField->ExecType = ExecType::ExecNew;
			pField->Status = CUstpFtdcRtnQuoteField_2_OrderStatus(pQuote);
			pField->ExecType = ExecType::ExecNew;


			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			m_id_platform_quote.insert(pair<string, QuoteField*>(quoteId, pField));
		}
		else
		{
			pField = it->second;

			strcpy(pField->ID, quoteId);
			strcpy(pField->AskOrderID, pQuote->AskOrderSysID);
			strcpy(pField->BidOrderID, pQuote->BidOrderSysID);

			pField->Status = CUstpFtdcRtnQuoteField_2_OrderStatus(pQuote);
			pField->ExecType = CUstpFtdcRtnQuoteField_2_ExecType(pQuote);
		}

		XRespone(ResponeType::OnRtnQuote, m_msgQueue, this, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

//void CTraderApi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
//	{
//		OnQuote(pQuote);
//	}
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

void CTraderApi::OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus)
{
	//if(m_msgQueue)
	//	m_msgQueue->Input_OnRtnInstrumentStatus(this,pInstrumentStatus);
}

void CTraderApi::OnRtnForQuote(CUstpFtdcReqForQuoteField *pReqForQuote)
{
	QuoteRequestField field = { 0 };

	strcpy(field.Symbol, pReqForQuote->InstrumentID);
	strcpy(field.InstrumentID, pReqForQuote->InstrumentID);
	strcpy(field.ExchangeID, pReqForQuote->ExchangeID);
	strcpy(field.TradingDay, pReqForQuote->TradingDay);
	strcpy(field.QuoteID, pReqForQuote->ReqForQuoteID);
	strcpy(field.QuoteTime, pReqForQuote->ReqForQuoteTime);

	XRespone(ResponeType::OnRtnQuoteRequest, m_msgQueue, this, 0, 0, &field, sizeof(QuoteRequestField), nullptr, 0, nullptr, 0);
}
