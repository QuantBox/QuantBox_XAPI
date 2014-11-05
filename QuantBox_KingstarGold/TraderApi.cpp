#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../include/Kingstar_Gold/Constant.h"
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

bool CTraderApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

bool CTraderApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
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

	m_pApi = CGoldTradeApi::CreateGoldTradeApi();


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

		//m_pApi->SubscribePublicTopic((THOST_TE_RESUME_TYPE)pServerInfo->Resume);
		//m_pApi->SubscribePrivateTopic((THOST_TE_RESUME_TYPE)pServerInfo->Resume);

		//初始化连接
		int ret = m_pApi->Init();
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		ReqUserLogin();		
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
		//case E_ReqAuthenticateField:
		//	iRet = m_pApi->ReqAuthenticate(&pRequest->ReqAuthenticateField,lRequest);
		//	break;
		case E_ReqUserLoginField:
			iRet = m_pApi->ReqUserLogin(&pRequest->ReqUserLoginField,lRequest);
			break;
		//case E_SettlementInfoConfirmField:
		//	iRet = m_pApi->ReqSettlementInfoConfirm(&pRequest->SettlementInfoConfirmField,lRequest);
		//	break;
		case E_QryInstrumentField:
			iRet = m_pApi->ReqQryInstrument(&pRequest->QryInstrumentField,lRequest);
			break;
		case E_QryTradingAccountField:
			iRet = m_pApi->ReqQryTradingAccount(&pRequest->QryTradingAccountField,lRequest);
			break;
		case E_QryInvestorPositionField:
			iRet = m_pApi->ReqQryInvestorPosition(&pRequest->QryInvestorPositionField,lRequest);
			break;
		//case E_QryInvestorPositionDetailField:
		//	iRet=m_pApi->ReqQryInvestorPositionDetail(&pRequest->QryInvestorPositionDetailField,lRequest);
		//	break;
		//case E_QryInstrumentCommissionRateField:
		//	iRet = m_pApi->ReqQryInstrumentCommissionRate(&pRequest->QryInstrumentCommissionRateField,lRequest);
		//	break;
		//case E_QryInstrumentMarginRateField:
		//	iRet = m_pApi->ReqQryInstrumentMarginRate(&pRequest->QryInstrumentMarginRateField,lRequest);
		//	break;
		//case E_QryDepthMarketDataField:
		//	iRet = m_pApi->ReqQryDepthMarketData(&pRequest->QryDepthMarketDataField,lRequest);
		//	break;
		//case E_QrySettlementInfoField:
		//	iRet = m_pApi->ReqQrySettlementInfo(&pRequest->QrySettlementInfoField, lRequest);
		//	break;
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

	////连接成功后自动请求认证或登录
	//if (strlen(m_ServerInfo.AuthCode)>0
	//	&& strlen(m_ServerInfo.UserProductInfo)>0)
	//{
	//	//填了认证码就先认证
	//	ReqAuthenticate();
	//}
	//else
	//{
		ReqUserLogin();
	//}
}

void CTraderApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField field = { 0 };
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	field.ErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason,field.ErrorMsg);

	XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

}

//void CTraderApi::ReqAuthenticate()
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_ReqAuthenticateField);
//	if (pRequest)
//	{
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Authorizing, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//
//		CThostFtdcReqAuthenticateField& body = pRequest->ReqAuthenticateField;
//
//		strncpy(body.BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
//		strncpy(body.UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
//		strncpy(body.UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TThostFtdcProductInfoType));
//		strncpy(body.AuthCode, m_ServerInfo.AuthCode, sizeof(TThostFtdcAuthCodeType));
//
//		AddToSendQueue(pRequest);
//	}
//}
//
//void CTraderApi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo)
//		&&pRspAuthenticateField)
//	{
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Authorized, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//
//		ReqUserLogin();
//	}
//	else
//	{
//		RspUserLoginField field = { 0 };
//		field.ErrorID = pRspInfo->ErrorID;
//		strncpy(field.ErrorMsg, pRspInfo->ErrorMsg, sizeof(pRspInfo->ErrorMsg));
//
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
//	}
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

void CTraderApi::ReqUserLogin()
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_ReqUserLoginField);
	if (pRequest)
	{
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		CThostFtdcReqUserLoginField& body = pRequest->ReqUserLoginField;

		strncpy(body.accountID, m_UserInfo.UserID, sizeof(TThostFtdcTraderIDType));
		strncpy(body.password, m_UserInfo.Password, sizeof(TThostFtdcPasswordType));
		body.loginType = BANKACC_TYPE;

		AddToSendQueue(pRequest);
	}
}

void CTraderApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField field = { 0 };

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		strncpy(field.TradingDay, pRspUserLogin->tradeDate, sizeof(DateType));
		strncpy(field.LoginTime, pRspUserLogin->lastLoginTime, sizeof(TimeType));
		//sprintf(field.SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Logined, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin,pRspUserLogin,sizeof(CThostFtdcRspUserLoginField));
		m_nMaxOrderRef = atol(pRspUserLogin->localOrderNo);
		// 自己发单时ID从1开始，不能从0开始
		m_nMaxOrderRef = m_nMaxOrderRef>1 ? m_nMaxOrderRef:1;
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

//void CTraderApi::ReqSettlementInfoConfirm()
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_SettlementInfoConfirmField);
//	if (pRequest)
//	{
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Confirming, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//
//		CThostFtdcSettlementInfoConfirmField& body = pRequest->SettlementInfoConfirmField;
//
//		strncpy(body.BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
//		strncpy(body.InvestorID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
//
//		AddToSendQueue(pRequest);
//	}
//}
//
//void CTraderApi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo)
//		&&pSettlementInfoConfirm)
//	{
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Confirmed, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//	}
//	else
//	{
//		RspUserLoginField field = { 0 };
//		field.ErrorID = pRspInfo->ErrorID;
//		strncpy(field.ErrorMsg, pRspInfo->ErrorMsg, sizeof(pRspInfo->ErrorMsg));
//
//		XRespone(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, &field, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
//	}
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

char* CTraderApi::ReqOrderInsert(
	int OrderRef,
	OrderField* pOrder1)
{
	if (nullptr == m_pApi)
		return nullptr;

	SRequest* pRequest = MakeRequestBuf(E_InputOrderField);
	if (nullptr == pRequest)
		return nullptr;

	CThostFtdcInputOrderField& body = pRequest->InputOrderField;

	strcpy(body.seatID, m_RspUserLogin.SeatNo);
	strcpy(body.tradeCode, m_RspUserLogin.tradeCode);
	strncpy(body.instID, pOrder1->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
	body.buyOrSell = OrderSide_2_TThostFtdcBsFlagType(pOrder1->Side);
	//body.offsetFlag;
	body.amount = (int)pOrder1->Qty;
	body.middleFlag;
	body.priceFlag;
	body.price;
	body.trigPrice;
	strcpy(body.marketID, pOrder1->ExchangeID);
	body.marketID;
	body.LocalOrderNo;
	body.tradeWay;


	////合约
	//
	////买卖
	//
	////开平
	//body.CombOffsetFlag[0] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder1->OpenClose);
	////投保
	//body.CombHedgeFlag[0] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder1->HedgeFlag);
	////数量
	//body.VolumeTotalOriginal = (int)pOrder1->Qty;

	//// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
	//body.LimitPrice = pOrder1->Price;
	//body.StopPrice = pOrder1->StopPx;

	//// 针对第二个进行处理，如果有第二个参数，认为是交易所套利单
	//if (pOrder2)
	//{
	//	body.CombOffsetFlag[1] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder1->OpenClose);
	//	body.CombHedgeFlag[1] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder1->HedgeFlag);
	//	// 交易所的移仓换月功能，没有实测过
	//	body.IsSwapOrder = (body.CombOffsetFlag[0] != body.CombOffsetFlag[1]);
	//}

	////价格
	////body.OrderPriceType = OrderType_2_TThostFtdcOrderPriceTypeType(pOrder1->Type);

	//// 市价与限价
	//switch (pOrder1->Type)
	//{
	//case Market:
	//case Stop:
	//case MarketOnClose:
	//case TrailingStop:
	//	body.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	//	body.TimeCondition = THOST_FTDC_TC_IOC;
	//	break;
	//case Limit:
	//case StopLimit:
	//case TrailingStopLimit:
	//default:
	//	body.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	//	body.TimeCondition = THOST_FTDC_TC_GFD;
	//	break;
	//}

	//// IOC与FOK
	//switch (pOrder1->TimeInForce)
	//{
	//case IOC:
	//	body.TimeCondition = THOST_FTDC_TC_IOC;
	//	body.VolumeCondition = THOST_FTDC_VC_AV;
	//	break;
	//case FOK:
	//	body.TimeCondition = THOST_FTDC_TC_IOC;
	//	body.VolumeCondition = THOST_FTDC_VC_CV;
	//	//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
	//	break;
	//default:
	//	body.VolumeCondition = THOST_FTDC_VC_AV;
	//	break;
	//}

	//// 条件单
	//switch (pOrder1->Type)
	//{
	//case Stop:
	//case TrailingStop:
	//case StopLimit:
	//case TrailingStopLimit:
	//	// 条件单没有测试，先留空
	//	body.ContingentCondition = THOST_FTDC_CC_Immediately;
	//	break;
	//default:
	//	body.ContingentCondition = THOST_FTDC_CC_Immediately;
	//	break;
	//}

	//int nRet = 0;
	//{
	//	//可能报单太快，m_nMaxOrderRef还没有改变就提交了
	//	lock_guard<mutex> cl(m_csOrderRef);

	//	if (OrderRef < 0)
	//	{
	//		nRet = m_nMaxOrderRef;
	//		++m_nMaxOrderRef;
	//	}
	//	else
	//	{
	//		nRet = OrderRef;
	//	}
	//	sprintf(body.OrderRef, "%d", nRet);

	//	//不保存到队列，而是直接发送
	//	int n = m_pApi->ReqOrderInsert(&pRequest->InputOrderField, ++m_lRequestID);
	//	if (n < 0)
	//	{
	//		nRet = n;
	//		delete pRequest;
	//		return nullptr;
	//	}
	//	else
	//	{
	//		// 用于各种情况下找到原订单，用于进行响应的通知
	//		sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);

	//		OrderField* pField = new OrderField();
	//		memcpy(pField, pOrder1, sizeof(OrderField));
	//		strcpy(pField->ID, m_orderInsert_Id);
	//		m_id_platform_order.insert(pair<string, OrderField*>(m_orderInsert_Id, pField));
	//	}
	//}
	//delete pRequest;//用完后直接删除

	return m_orderInsert_Id;
}

void CTraderApi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);

	//hash_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//if (it == m_id_platform_order.end())
	//{
	//	// 没找到？不应当，这表示出错了
	//	assert(false);
	//}
	//else
	//{
	//	// 找到了，要更新状态
	//	// 得使用上次的状态
	//	OrderField* pField = it->second;
	//	pField->ExecType = ExecType::ExecRejected;
	//	pField->Status = OrderStatus::Rejected;
	//	pField->ErrorID = pRspInfo->ErrorID;
	//	strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TThostFtdcErrorMsgType));
	//	XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

void CTraderApi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);

	//hash_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//if (it == m_id_platform_order.end())
	//{
	//	// 没找到？不应当，这表示出错了
	//	assert(false);
	//}
	//else
	//{
	//	// 找到了，要更新状态
	//	// 得使用上次的状态
	//	OrderField* pField = it->second;
	//	pField->ExecType = ExecType::ExecRejected;
	//	pField->Status = OrderStatus::Rejected;
	//	pField->ErrorID = pRspInfo->ErrorID;
	//	strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TThostFtdcErrorMsgType));
	//	XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

//char* CTraderApi::ReqParkedOrderInsert(
//	int OrderRef,
//	OrderField* pOrder1,
//	OrderField* pOrder2)
//{
//	if (nullptr == m_pApi)
//		return nullptr;
//
//	SRequest* pRequest = MakeRequestBuf(E_ParkedOrderField);
//	if (nullptr == pRequest)
//		return nullptr;
//
//	CThostFtdcParkedOrderField& body = pRequest->ParkedOrderField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//
//	body.MinVolume = 1;
//	body.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//	body.IsAutoSuspend = 0;
//	body.UserForceClose = 0;
//	body.IsSwapOrder = 0;
//
//	//合约
//	strncpy(body.InstrumentID, pOrder1->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
//	//买卖
//	body.Direction = OrderSide_2_TThostFtdcDirectionType(pOrder1->Side);
//	//开平
//	body.CombOffsetFlag[0] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder1->OpenClose);
//	//投保
//	body.CombHedgeFlag[0] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder1->HedgeFlag);
//	//数量
//	body.VolumeTotalOriginal = (int)pOrder1->Qty;
//
//	// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
//	body.LimitPrice = pOrder1->Price;
//	body.StopPrice = pOrder1->StopPx;
//
//	// 针对第二个进行处理，如果有第二个参数，认为是交易所套利单
//	if (pOrder2)
//	{
//		body.CombOffsetFlag[1] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder1->OpenClose);
//		body.CombHedgeFlag[1] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder1->HedgeFlag);
//		// 交易所的移仓换月功能，没有实测过
//		body.IsSwapOrder = (body.CombOffsetFlag[0] != body.CombOffsetFlag[1]);
//	}
//
//	//价格
//	//body.OrderPriceType = OrderType_2_TThostFtdcOrderPriceTypeType(pOrder1->Type);
//
//	// 市价与限价
//	switch (pOrder1->Type)
//	{
//	case Market:
//	case Stop:
//	case MarketOnClose:
//	case TrailingStop:
//		body.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
//		body.TimeCondition = THOST_FTDC_TC_IOC;
//		break;
//	case Limit:
//	case StopLimit:
//	case TrailingStopLimit:
//	default:
//		body.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
//		body.TimeCondition = THOST_FTDC_TC_GFD;
//		break;
//	}
//
//	// IOC与FOK
//	switch (pOrder1->TimeInForce)
//	{
//	case IOC:
//		body.TimeCondition = THOST_FTDC_TC_IOC;
//		body.VolumeCondition = THOST_FTDC_VC_AV;
//		break;
//	case FOK:
//		body.TimeCondition = THOST_FTDC_TC_IOC;
//		body.VolumeCondition = THOST_FTDC_VC_CV;
//		//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
//		break;
//	default:
//		body.VolumeCondition = THOST_FTDC_VC_AV;
//		break;
//	}
//
//	// 条件单
//	switch (pOrder1->Type)
//	{
//	case Stop:
//	case TrailingStop:
//	case StopLimit:
//	case TrailingStopLimit:
//		// 条件单没有测试，先留空
//		body.ContingentCondition = THOST_FTDC_CC_Immediately;
//		break;
//	default:
//		body.ContingentCondition = THOST_FTDC_CC_Immediately;
//		break;
//	}
//
//	int nRet = 0;
//	{
//		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
//		lock_guard<mutex> cl(m_csOrderRef);
//
//		if (OrderRef < 0)
//		{
//			nRet = m_nMaxOrderRef;
//			++m_nMaxOrderRef;
//		}
//		else
//		{
//			nRet = OrderRef;
//		}
//		sprintf(body.OrderRef, "%d", nRet);
//
//		//不保存到队列，而是直接发送
//		int n = m_pApi->ReqParkedOrderInsert(&pRequest->ParkedOrderField, ++m_lRequestID);
//		if (n < 0)
//		{
//			nRet = n;
//			delete pRequest;
//			return nullptr;
//		}
//		else
//		{
//			sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);
//
//			OrderField* pField = new OrderField();
//			memcpy(pField, pOrder1, sizeof(OrderField));
//			m_id_platform_order.insert(pair<string, OrderField*>(m_orderInsert_Id, pField));
//		}
//	}
//	delete pRequest;//用完后直接删除
//
//	return m_orderInsert_Id;
//}

void CTraderApi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	OnTrade(pTrade);
}

int CTraderApi::ReqOrderAction(const string& szId)
{
	hash_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.find(szId);
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

int CTraderApi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	/*if (nullptr == m_pApi)
		return 0;

	SRequest* pRequest = MakeRequestBuf(E_InputOrderActionField);
	if (nullptr == pRequest)
		return 0;

	CThostFtdcInputOrderActionField& body = pRequest->InputOrderActionField;

	///经纪公司代码
	strncpy(body.BrokerID, pOrder->BrokerID,sizeof(TThostFtdcBrokerIDType));
	///投资者代码
	strncpy(body.InvestorID, pOrder->InvestorID,sizeof(TThostFtdcInvestorIDType));
	///报单引用
	strncpy(body.OrderRef, pOrder->OrderRef,sizeof(TThostFtdcOrderRefType));
	///前置编号
	body.FrontID = pOrder->FrontID;
	///会话编号
	body.SessionID = pOrder->SessionID;
	///交易所代码
	strncpy(body.ExchangeID,pOrder->ExchangeID,sizeof(TThostFtdcExchangeIDType));
	///报单编号
	strncpy(body.OrderSysID,pOrder->OrderSysID,sizeof(TThostFtdcOrderSysIDType));
	///操作标志
	body.ActionFlag = THOST_FTDC_AF_Delete;
	///合约代码
	strncpy(body.InstrumentID, pOrder->InstrumentID,sizeof(TThostFtdcInstrumentIDType));

	int nRet = m_pApi->ReqOrderAction(&pRequest->InputOrderActionField, ++m_lRequestID);
	delete pRequest;
	return nRet;*/
	return 0;
}

void CTraderApi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", pInputOrderAction->FrontID, pInputOrderAction->SessionID, pInputOrderAction->OrderRef);

	//hash_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//if (it == m_id_platform_order.end())
	//{
	//	// 没找到？不应当，这表示出错了
	//	assert(false);
	//}
	//else
	//{
	//	// 找到了，要更新状态
	//	// 得使用上次的状态
	//	OrderField* pField = it->second;
	//	strcpy(pField->ID, orderId);
	//	pField->ExecType = ExecType::ExecCancelReject;
	//	pField->ErrorID = pRspInfo->ErrorID;
	//	strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TThostFtdcErrorMsgType));
	//	XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

void CTraderApi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", pOrderAction->FrontID, pOrderAction->SessionID, pOrderAction->OrderRef);

	//hash_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//if (it == m_id_platform_order.end())
	//{
	//	// 没找到？不应当，这表示出错了
	//	assert(false);
	//}
	//else
	//{
	//	// 找到了，要更新状态
	//	// 得使用上次的状态
	//	OrderField* pField = it->second;
	//	strcpy(pField->ID, orderId);
	//	pField->ExecType = ExecType::ExecCancelReject;
	//	pField->ErrorID = pRspInfo->ErrorID;
	//	strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(TThostFtdcErrorMsgType));
	//	XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

void CTraderApi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	OnOrder(pOrder);
}

//char* CTraderApi::ReqQuoteInsert(
//	int QuoteRef,
//	OrderField* pOrderAsk,
//	OrderField* pOrderBid)
//{
//	if (nullptr == m_pApi)
//		return nullptr;
//
//	SRequest* pRequest = MakeRequestBuf(E_InputQuoteField);
//	if (nullptr == pRequest)
//		return nullptr;
//
//	CThostFtdcInputQuoteField& body = pRequest->InputQuoteField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//
//	//合约,目前只从订单1中取
//	strncpy(body.InstrumentID, pOrderAsk->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
//	//开平
//	body.AskOffsetFlag = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrderAsk->OpenClose);
//	body.BidOffsetFlag = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrderBid->OpenClose);
//	//投保
//	body.AskHedgeFlag = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrderAsk->HedgeFlag);
//	body.BidHedgeFlag = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrderBid->HedgeFlag);
//
//	//价格
//	body.AskPrice = pOrderAsk->Price;
//	body.BidPrice = pOrderBid->Price;
//
//	//数量
//	body.AskVolume = (int)pOrderAsk->Qty;
//	body.BidVolume = (int)pOrderBid->Qty;
//
//	int nRet = 0;
//	{
//		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
//		lock_guard<mutex> cl(m_csOrderRef);
//
//		if (QuoteRef < 0)
//		{
//			nRet = m_nMaxOrderRef;
//			sprintf(body.QuoteRef, "%d", m_nMaxOrderRef);
//			sprintf(body.AskOrderRef, "%d", m_nMaxOrderRef);
//			sprintf(body.BidOrderRef, "%d", ++m_nMaxOrderRef);
//			++m_nMaxOrderRef;
//		}
//		else
//		{
//			nRet = QuoteRef;
//			sprintf(body.QuoteRef, "%d", QuoteRef);
//			sprintf(body.AskOrderRef, "%d", QuoteRef);
//			sprintf(body.BidOrderRef, "%d", ++QuoteRef);
//			++QuoteRef;
//		}
//
//		//不保存到队列，而是直接发送
//		int n = m_pApi->ReqQuoteInsert(&pRequest->InputQuoteField, ++m_lRequestID);
//		if (n < 0)
//		{
//			nRet = n;
//		}
//	}
//	delete pRequest;//用完后直接删除
//
//	return m_orderInsert_Id;
//}
//
//void CTraderApi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnRspQuoteInsert(this, pInputQuote, pRspInfo, nRequestID, bIsLast);
//}
//
//void CTraderApi::OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnErrRtnQuoteInsert(this, pInputQuote, pRspInfo);
//}
//
//void CTraderApi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnRtnQuote(this, pQuote);
//}
//
//int CTraderApi::ReqQuoteAction(const string& szId)
//{
//	hash_map<string, CThostFtdcQuoteField*>::iterator it = m_id_api_quote.find(szId);
//	if (it == m_id_api_quote.end())
//	{
//		// <error id="QUOTE_NOT_FOUND" value="86" prompt="CTP:报价撤单找不到相应报价"/>
//		ErrorField field = { 0 };
//		field.ErrorID = 86;
//		sprintf(field.ErrorMsg, "QUOTE_NOT_FOUND");
//
//		XRespone(ResponeType::OnRtnError, m_msgQueue, this, 0, 0, &field, sizeof(ErrorField), nullptr, 0, nullptr, 0);
//	}
//	else
//	{
//		// 找到了订单
//		ReqQuoteAction(it->second);
//	}
//	return 0;
//}
//
//int CTraderApi::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
//{
//	if (nullptr == m_pApi)
//		return 0;
//
//	SRequest* pRequest = MakeRequestBuf(E_InputQuoteActionField);
//	if (nullptr == pRequest)
//		return 0;
//
//	CThostFtdcInputQuoteActionField& body = pRequest->InputQuoteActionField;
//
//	///经纪公司代码
//	strncpy(body.BrokerID, pQuote->BrokerID, sizeof(TThostFtdcBrokerIDType));
//	///投资者代码
//	strncpy(body.InvestorID, pQuote->InvestorID, sizeof(TThostFtdcInvestorIDType));
//	///报单引用
//	strncpy(body.QuoteRef, pQuote->QuoteRef, sizeof(TThostFtdcOrderRefType));
//	///前置编号
//	body.FrontID = pQuote->FrontID;
//	///会话编号
//	body.SessionID = pQuote->SessionID;
//	///交易所代码
//	strncpy(body.ExchangeID, pQuote->ExchangeID, sizeof(TThostFtdcExchangeIDType));
//	///报单编号
//	strncpy(body.QuoteSysID, pQuote->QuoteSysID, sizeof(TThostFtdcOrderSysIDType));
//	///操作标志
//	body.ActionFlag = THOST_FTDC_AF_Delete;
//	///合约代码
//	strncpy(body.InstrumentID, pQuote->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
//
//	int nRet = m_pApi->ReqQuoteAction(&pRequest->InputQuoteActionField, ++m_lRequestID);
//	delete pRequest;
//	return nRet;
//}
//
//void CTraderApi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnRspQuoteAction(this, pInputQuoteAction, pRspInfo, nRequestID, bIsLast);
//}
//
//void CTraderApi::OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo)
//{
//	//if (m_msgQueue)
//	//	m_msgQueue->Input_OnErrRtnQuoteAction(this, pQuoteAction, pRspInfo);
//}

void CTraderApi::ReqQryTradingAccount()
{
	/*if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryTradingAccountField);
	if (nullptr == pRequest)
		return;

	CThostFtdcQryTradingAccountField& body = pRequest->QryTradingAccountField;

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));

	AddToSendQueue(pRequest);*/
}

void CTraderApi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pTradingAccount)
		{
			AccountField field = { 0 };
			field.PreBalance = pTradingAccount->PreBalance;
			field.CurrMargin = pTradingAccount->CurrMargin;
			field.Commission = pTradingAccount->Commission;
			field.CloseProfit = pTradingAccount->CloseProfit;
			field.PositionProfit = pTradingAccount->PositionProfit;
			field.Balance = pTradingAccount->Balance;
			field.Available = pTradingAccount->Available;

			XRespone(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, &field, sizeof(AccountField), nullptr, 0, nullptr, 0);
		}
		else
		{
			XRespone(ResponeType::OnRspQryTradingAccount, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);*/
}

void CTraderApi::ReqQryInvestorPosition(const string& szInstrumentId)
{
	/*if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInvestorPositionField);
	if (nullptr == pRequest)
		return;

	CThostFtdcQryInvestorPositionField& body = pRequest->QryInvestorPositionField;

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));

	AddToSendQueue(pRequest);*/
}

void CTraderApi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pInvestorPosition)
		{
			PositionField field = { 0 };

			strcpy(field.InstrumentID, pInvestorPosition->InstrumentID);

			field.Side = TThostFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection);
			field.HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag);
			field.Position = pInvestorPosition->Position;
			field.TdPosition = pInvestorPosition->TodayPosition;
			field.YdPosition = pInvestorPosition->Position - pInvestorPosition->TodayPosition;

			XRespone(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, bIsLast, 0, &field, sizeof(PositionField), nullptr, 0, nullptr, 0);
		}
		else
		{
			XRespone(ResponeType::OnRspQryInvestorPosition, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);*/
}

//void CTraderApi::ReqQryInvestorPositionDetail(const string& szInstrumentId)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryInvestorPositionDetailField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQryInvestorPositionDetailField& body = pRequest->QryInvestorPositionDetailField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	
//}

void CTraderApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{
	if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentField);
	if (nullptr == pRequest)
		return;

	CThostFtdcQryInstrumentField& body = pRequest->QryInstrumentField;

	strncpy(body.ContractID, szInstrumentId.c_str(), sizeof(TThostFtdcInstrumentIDType));
	//strncpy(body.ProductID, szExchange.c_str(), sizeof(TThostFtdcProductIDType));

	AddToSendQueue(pRequest);
}


void CTraderApi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		if (pInstrument)
		{
			InstrumentField field = { 0 };

			strcpy(field.InstrumentID, pInstrument->instID);
			strcpy(field.ExchangeID, pInstrument->exchangeID);

			strcpy(field.Symbol, pInstrument->instID);

			strcpy(field.InstrumentName, pInstrument->name);
			field.Type = CThostFtdcInstrumentField_2_InstrumentType(pInstrument);
			//field.VolumeMultiple = pInstrument->VolumeMultiple;
			field.PriceTick = pInstrument->tick;
			//strncpy(field.ExpireDate, pInstrument->ExpireDate, sizeof(TThostFtdcDateType));
			//field.OptionsType = TThostFtdcOptionsTypeType_2_PutCall(pInstrument->OptionsType);

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

//void CTraderApi::ReqQryInstrumentCommissionRate(const string& szInstrumentId)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentCommissionRateField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQryInstrumentCommissionRateField& body = pRequest->QryInstrumentCommissionRateField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRspQryInstrumentCommissionRate(this,pInstrumentCommissionRate,pRspInfo,nRequestID,bIsLast);
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}
//
//void CTraderApi::ReqQryInstrumentMarginRate(const string& szInstrumentId,TThostFtdcHedgeFlagType HedgeFlag)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryInstrumentMarginRateField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQryInstrumentMarginRateField& body = pRequest->QryInstrumentMarginRateField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//	body.HedgeFlag = HedgeFlag;
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRspQryInstrumentMarginRate(this,pInstrumentMarginRate,pRspInfo,nRequestID,bIsLast);
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

//void CTraderApi::ReqQryDepthMarketData(const string& szInstrumentId)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QryDepthMarketDataField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQryDepthMarketDataField& body = pRequest->QryDepthMarketDataField;
//
//	strncpy(body.InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRspQryDepthMarketData(this,pDepthMarketData,pRspInfo,nRequestID,bIsLast);
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}
//
//void CTraderApi::ReqQrySettlementInfo(const string& szTradingDay)
//{
//	if (nullptr == m_pApi)
//		return;
//
//	SRequest* pRequest = MakeRequestBuf(E_QrySettlementInfoField);
//	if (nullptr == pRequest)
//		return;
//
//	CThostFtdcQrySettlementInfoField& body = pRequest->QrySettlementInfoField;
//
//	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//	strncpy(body.TradingDay, szTradingDay.c_str(), sizeof(TThostFtdcDateType));
//
//	AddToSendQueue(pRequest);
//}
//
//void CTraderApi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
//	{
//		if (pSettlementInfo)
//		{
//			SettlementInfoField field = { 0 };
//			strncpy(field.TradingDay, pSettlementInfo->TradingDay, sizeof(TThostFtdcDateType));
//			strncpy(field.Content, pSettlementInfo->Content, sizeof(TThostFtdcContentType));
//
//			XRespone(ResponeType::OnRspQrySettlementInfo, m_msgQueue, this, bIsLast, 0, &field, sizeof(SettlementInfoField), nullptr, 0, nullptr, 0);
//		}
//		else
//		{
//			XRespone(ResponeType::OnRspQrySettlementInfo, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
//		}
//	}
//
//	if (bIsLast)
//		ReleaseRequestMapBuf(nRequestID);
//}

void CTraderApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo, nRequestID, bIsLast);

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

void CTraderApi::ReqQryOrder()
{
	/*if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryOrderField);
	if (nullptr == pRequest)
		return;

	CThostFtdcQryOrderField& body = pRequest->QryOrderField;

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));

	AddToSendQueue(pRequest);*/
}

void CTraderApi::OnOrder(CThostFtdcOrderField *pOrder)
{
	//if (nullptr == pOrder)
	//	return;

	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
	//OrderIDType orderSydId = { 0 };

	//{
	//	// 保存原始订单信息，用于撤单

	//	hash_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
	//	if (it == m_id_api_order.end())
	//	{
	//		// 找不到此订单，表示是新单
	//		CThostFtdcOrderField* pField = new CThostFtdcOrderField();
	//		memcpy(pField, pOrder, sizeof(CThostFtdcOrderField));
	//		m_id_api_order.insert(pair<string, CThostFtdcOrderField*>(orderId, pField));
	//	}
	//	else
	//	{
	//		// 找到了订单
	//		// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
	//		CThostFtdcOrderField* pField = it->second;
	//		memcpy(pField, pOrder, sizeof(CThostFtdcOrderField));
	//	}

	//	// 保存SysID用于定义成交回报与订单
	//	sprintf(orderSydId, "%s:%s", pOrder->ExchangeID, pOrder->OrderSysID);
	//	m_sysId_orderId.insert(pair<string, string>(orderSydId, orderId));
	//}

	//{
	//	// 从API的订单转换成自己的结构体

	//	OrderField* pField = nullptr;
	//	hash_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//	if (it == m_id_platform_order.end())
	//	{
	//		// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
	//		pField = new OrderField();
	//		memset(pField, 0, sizeof(OrderField));
	//		strcpy(pField->ID, orderId);
	//		strcpy(pField->InstrumentID, pOrder->InstrumentID);
	//		strcpy(pField->ExchangeID, pOrder->ExchangeID);
	//		pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pOrder->CombHedgeFlag[0]);
	//		pField->Side = TThostFtdcDirectionType_2_OrderSide(pOrder->Direction);
	//		pField->Price = pOrder->LimitPrice;
	//		pField->StopPx = pOrder->StopPrice;
	//		strcpy(pField->Text, pOrder->StatusMsg);
	//		pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pOrder->CombOffsetFlag[0]);
	//		pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
	//		pField->Qty = pOrder->VolumeTotalOriginal;
	//		pField->Type = CThostFtdcOrderField_2_OrderType(pOrder);
	//		pField->TimeInForce = CThostFtdcOrderField_2_TimeInForce(pOrder);
	//		pField->ExecType = ExecType::ExecNew;
	//		strcpy(pField->OrderID, pOrder->OrderSysID);


	//		// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
	//		m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
	//	}
	//	else
	//	{
	//		pField = it->second;
	//		strcpy(pField->ID, orderId);
	//		pField->LeavesQty = pOrder->VolumeTotal;
	//		pField->Price = pOrder->LimitPrice;
	//		pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
	//		pField->ExecType = CThostFtdcOrderField_2_ExecType(pOrder);
	//		strcpy(pField->OrderID, pOrder->OrderSysID);
	//		strcpy(pField->Text, pOrder->StatusMsg);
	//	}

	//	XRespone(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

void CTraderApi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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
	/*if (nullptr == m_pApi)
		return;

	SRequest* pRequest = MakeRequestBuf(E_QryTradeField);
	if (nullptr == pRequest)
		return;

	CThostFtdcQryTradeField& body = pRequest->QryTradeField;

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));

	AddToSendQueue(pRequest);*/
}

void CTraderApi::OnTrade(CThostFtdcTradeField *pTrade)
{
	//if (nullptr == pTrade)
	//	return;

	//TradeField* pField = new TradeField();
	//strcpy(pField->InstrumentID, pTrade->InstrumentID);
	//strcpy(pField->ExchangeID, pTrade->ExchangeID);
	//pField->Side = TThostFtdcDirectionType_2_OrderSide(pTrade->Direction);
	//pField->Qty = pTrade->Volume;
	//pField->Price = pTrade->Price;
	//pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	//pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	//pField->Commission = 0;//TODO收续费以后要计算出来
	//strcpy(pField->Time, pTrade->TradeTime);
	//strcpy(pField->TradeID, pTrade->TradeID);

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
	//	// 找到对应的报单
	//	strcpy(pField->ID, it->second.c_str());

	//	XRespone(ResponeType::OnRtnTrade, m_msgQueue, this, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

	//	hash_map<string, OrderField*>::iterator it2 = m_id_platform_order.find(it->second);
	//	if (it2 == m_id_platform_order.end())
	//	{
	//		// 此成交找不到对应的报单
	//		assert(false);
	//	}
	//	else
	//	{
	//		// 更新订单的状态
	//		// 是否要通知接口
	//	}
	//}
}

void CTraderApi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo, nRequestID, bIsLast))
	{
		OnTrade(pTrade);
	}

	if (bIsLast)
		ReleaseRequestMapBuf(nRequestID);
}

//void CTraderApi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
//{
//	//if(m_msgQueue)
//	//	m_msgQueue->Input_OnRtnInstrumentStatus(this,pInstrumentStatus);
//}

void CTraderApi::Subscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	char *ppInstrumentID[] = { DEFER, SPOT, FUTURES, FORWARD };
	int iInstrumentID = 4;
	m_pApi->SubscribeMarketData(ppInstrumentID,iInstrumentID);
}

void CTraderApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchageID)
{
	char *ppInstrumentID[] = { DEFER, SPOT, FUTURES, FORWARD };
	int iInstrumentID = 4;
	m_pApi->UnSubscribeMarketData(ppInstrumentID, iInstrumentID);
}

void CTraderApi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	DepthMarketDataField marketData = { 0 };
	strcpy(marketData.InstrumentID, pDepthMarketData->InstID);
	//strcpy(marketData.ExchangeID, pDepthMarketData->e);

	strcpy(marketData.Symbol, pDepthMarketData->InstID);
	// 这个日期在晚上12点时会怎么样？
	GetExchangeTime(pDepthMarketData->QuoteDate, nullptr, pDepthMarketData->QuoteTime
		, &marketData.TradingDay, &marketData.ActionDay, &marketData.UpdateTime);
	marketData.UpdateMillisec = 0;// pDepthMarketData->UpdateMillisec;

	marketData.LastPrice = pDepthMarketData->Last;
	marketData.Volume = pDepthMarketData->Volume;
	marketData.Turnover = pDepthMarketData->Turnover;
	marketData.OpenInterest = pDepthMarketData->OpenInt;
	marketData.AveragePrice = pDepthMarketData->Average;

	marketData.OpenPrice = pDepthMarketData->Open;
	marketData.HighestPrice = pDepthMarketData->High;
	marketData.LowestPrice = pDepthMarketData->Low;
	marketData.ClosePrice = pDepthMarketData->Close;
	marketData.SettlementPrice = pDepthMarketData->Settle;

	marketData.UpperLimitPrice = pDepthMarketData->UpDown;
	marketData.LowerLimitPrice = pDepthMarketData->lowLimit;
	marketData.PreClosePrice = pDepthMarketData->PreClose;
	marketData.PreSettlementPrice = pDepthMarketData->PreSettle;
	//marketData.PreOpenInterest = pDepthMarketData->PreOpenInterest;

	marketData.BidPrice1 = pDepthMarketData->Bid1;
	marketData.BidVolume1 = pDepthMarketData->BidLot1;
	marketData.AskPrice1 = pDepthMarketData->Ask1;
	marketData.AskVolume1 = pDepthMarketData->AskLot1;

	//if (pDepthMarketData->Bid2 != DBL_MAX || pDepthMarketData->Ask2 != DBL_MAX)
	{
		marketData.BidPrice2 = pDepthMarketData->Bid2;
		marketData.BidVolume2 = pDepthMarketData->BidLot2;
		marketData.AskPrice2 = pDepthMarketData->Ask2;
		marketData.AskVolume2 = pDepthMarketData->AskLot2;

		marketData.BidPrice3 = pDepthMarketData->Bid3;
		marketData.BidVolume3 = pDepthMarketData->BidLot3;
		marketData.AskPrice3 = pDepthMarketData->Ask3;
		marketData.AskVolume3 = pDepthMarketData->AskLot3;

		marketData.BidPrice4 = pDepthMarketData->Bid4;
		marketData.BidVolume4 = pDepthMarketData->BidLot4;
		marketData.AskPrice4 = pDepthMarketData->Ask4;
		marketData.AskVolume4 = pDepthMarketData->AskLot4;

		marketData.BidPrice5 = pDepthMarketData->Bid5;
		marketData.BidVolume5 = pDepthMarketData->BidLot5;
		marketData.AskPrice5 = pDepthMarketData->Ask5;
		marketData.AskVolume5 = pDepthMarketData->AskLot5;
	}

	XRespone(ResponeType::OnRtnDepthMarketData, m_msgQueue, this, 0, 0, &marketData, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
}