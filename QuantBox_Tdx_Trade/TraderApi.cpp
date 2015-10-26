#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

//#include "TypeConvert.h"

#include <cstring>
#include <assert.h>

//CTraderApi* CTraderApi::pThis = nullptr;
//
//void __stdcall CTraderApi::OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData)
//{
//	// 这个类在一个dll不能实例化多次了，因为使用了static变量
//	pThis->_OnReadPushData(FuncNO, pEtxPushData);
//}
//
////客户端实现推送回调函数
//void CTraderApi::_OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData)
//{
//	switch (FuncNO)
//	{
//	case ETX_16203:
//		OnPST16203PushData((PST16203PushData)pEtxPushData);
//		break;
//	case ETX_16204:
//		OnPST16204PushData((PST16204PushData)pEtxPushData);
//		break;
//	default:
//	{
//			   ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
//
//			   pField->ErrorID = FuncNO;
//			   sprintf(pField->ErrorMsg, "无法识别的推送数据[%d]", FuncNO);
//
//			   m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
//	}
//		break;
//	}
//}

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
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
	case E_QryOrderField:
		iRet = _ReqQryOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryTradeField:
		iRet = _ReqQryTrade(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryInvestorField:
		iRet = _ReqQryInvestor(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_InputOrderField:
		iRet = _ReqOrderInsert(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_InputOrderActionField:
		iRet = _ReqOrderAction(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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
		m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

int CTraderApi::_Init()
{
	m_pRunner = CLuaRunner::CreateRunner();

	Error_STRUCT* pErr = nullptr;
	
	m_pRunner->LoadScript(m_ServerInfo.Address, true, false);
	m_pRunner->Init(m_ServerInfo.ExtendInformation, &pErr);

	if (pErr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = pErr->ErrCode;
		strcpy(pField->ErrorMsg, pErr->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		ReqUserLogin();
	}

	DeleteError(pErr);

	return 0;
}

void CTraderApi::ReqUserLogin()
{
	if (m_UserInfo_Pos >= m_UserInfo_Count)
		return;

	//STTraderLogin* pBody = (STTraderLogin*)m_msgQueue_Query->new_block(sizeof(STTraderLogin));

	//strncpy(pBody->cust_no, m_pUserInfos[m_UserInfo_Pos].UserID, sizeof(TCustNoType));
	//strncpy(pBody->cust_pwd, m_pUserInfos[m_UserInfo_Pos].Password, sizeof(TCustPwdType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	void* pClient = m_pRunner->Login(m_UserInfo.UserID, m_UserInfo.Password, &ppResults, &pErr);

	if (pErr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = pErr->ErrCode;
		strcpy(pField->ErrorMsg, pErr->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logined, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		
		// 目前这个地方先做成只支持一个账号的模式
		m_pApi = CTdxApi::CreateApi(m_ServerInfo.ExtendInformation);
		m_pApi->SetClient(pClient);
		m_pApi->SetAccount(m_UserInfo.UserID);

		// 查询股东列表
		ReqQryInvestor();

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		// 登录下一个账号
		//++m_UserInfo_Pos;
		//ReqUserLogin();
	}

	DeleteError(pErr);
	DeleteTableBody(ppResults);

	return 0;
}

void CTraderApi::ReqQryInvestor()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryInvestorField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;
	m_pApi->ReqQueryData(REQUEST_GDLB, &ppFieldInfos, &ppResults, &pErr);

	if (!IsErrorRspInfo(pErr))
	{
		if (ppResults)
		{
			int count = GetRowCountTableBody(ppResults);
			for (int i = 0; i < count; ++i)
			{
				InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

				//strcpy(pField->BrokerID, pInvestor->BrokerID);
				strcpy(pField->InvestorID, ppResults[i * COL_EACH_ROW + 0]);
				strcpy(pField->InvestorName, ppResults[i * COL_EACH_ROW + 1]);

				m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
			}
		}
	}

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	return 0;
}

CTraderApi::CTraderApi(void)
{
	m_pIDGenerator = nullptr;
	m_pApi = nullptr;
	m_pRunner = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue_Query->StartThread();
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

void CTraderApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue->Register(pCallback,this);
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

bool CTraderApi::IsErrorRspInfo(Error_STRUCT *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrType != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->ErrorID = pRspInfo->ErrCode;
		strcpy(pField->ErrorMsg, pRspInfo->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

//bool CTraderApi::IsErrorRspInfo(Error_STRUCT *pRspInfo)
//{
//	bool bRet = ((pRspInfo) && (pRspInfo->error_no != 0));
//
//	return bRet;
//}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	m_pIDGenerator = new CIDGenerator();
	m_pIDGenerator->SetPrefix(m_UserInfo.UserID);

	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pUserInfos = (UserInfoField*)(new char[sizeof(UserInfoField)*count]);
	memcpy(m_pUserInfos, pUserInfo, sizeof(UserInfoField)*count);

	m_UserInfo_Pos = 0;
	m_UserInfo_Count = count;

	m_msgQueue_Query->Input_NoCopy(E_Init, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CTraderApi::Disconnect()
{
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr,nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if(m_pApi)
	{
		m_pRunner->Logout(m_pApi->GetClient());
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	if (m_pRunner)
	{
		m_pRunner->Exit();
		m_pRunner->Release();
		m_pRunner = nullptr;
	}

	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr,nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	if (m_pIDGenerator)
	{
		delete m_pIDGenerator;
		m_pIDGenerator = nullptr;
	}
}

int CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	OrderIDType* pInOut)
{
	if (count < 1)
		return 0;

	memset(pInOut, 0, sizeof(OrderIDType)*count);

	OrderField* pNewOrder = new OrderField[count];
	memcpy(pNewOrder, pOrder, sizeof(OrderField)*count);

	// 生成本地ID，供上层进行定位使用
	for (int i = 0; i < count; ++i)
	{
		strcpy(pInOut[i], m_pIDGenerator->GetIDString());
		strcpy(pNewOrder[i].LocalID, pInOut[i]);		 
	}

	m_msgQueue_Query->Input_Copy(RequestType::E_InputOrderField, m_msgQueue_Query, this, 0, 0,
		pNewOrder, sizeof(OrderField)*count, nullptr, 0, nullptr, 0);

	delete[] pNewOrder;

	return 0;
}

void BuildOrder(OrderField* pIn, Order_STRUCT* pOut)
{
	strcpy(pOut->ZQDM, pIn->InstrumentID);
	pOut->Price = pIn->Price;
	pOut->Qty = pIn->Qty;

	// 这个地方后期要再改
	switch (pIn->Type)
	{
	case OrderType::Market:
		pOut->WTFS = WTFS_Five_IOC;
		if (pIn->Side == OrderSide::Buy)
		{
			pOut->MMBZ = MMBZ_Buy_Market;
		}
		else
		{
			pOut->MMBZ = MMBZ_Sell_Market;
		}
		break;
	case OrderType::Limit:
		pOut->WTFS = WTFS_Limit;
		if (pIn->Side == OrderSide::Buy)
		{
			pOut->MMBZ = MMBZ_Buy_Limit;
		}
		else
		{
			pOut->MMBZ = MMBZ_Sell_Limit;
		}
		break;
	}

	pOut->RZRQBS = RZRQBS_NO;
}

int CTraderApi::_ReqOrderInsert(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 这个函数返回后将被删除，所以需要进行复制
	OrderField* pOrders = (OrderField*)ptr1;
	int count = (int)size1 / sizeof(OrderField);

	Order_STRUCT* pTdxOrders = new Order_STRUCT[count];
	for (int i = 0; i < count; ++i)
	{
		BuildOrder(&pOrders[i], &pTdxOrders[i]);
	}

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT** ppErrs = nullptr;

	int n = m_pApi->SendMultiOrders(pTdxOrders, count, &ppFieldInfos, &ppResults, &ppErrs);

	// 将结果立即取出来
	for (int i = 0; i < count;++i)
	{
		// 处理错误
		if (ppErrs && ppErrs[i])
		{
			pOrders[i].ErrorID = ppErrs[i]->ErrCode;
			strcpy(pOrders[i].Text, ppErrs[i]->ErrInfo);
		}

		// 处理结果
		if (ppResults && ppResults[i*COL_EACH_ROW + 0])
		{
			strcpy(pOrders[i].ID, ppResults[i*COL_EACH_ROW + 0]);
		}

		// 现在有两个结构体


		OrderIDType orderId = { 0 };
		sprintf(orderId, "%s", pOrders[i].ID);

		OrderField* pField = nullptr;
		unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
		if (it == m_id_platform_order.end())
		{
			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

			memcpy(pField, &pOrders[i], sizeof(OrderField));

			//strcpy(pField->ID, orderId);
			//strcpy(pField->InstrumentID, pOrder->InstrumentID);
			//strcpy(pField->ExchangeID, pOrder->ExchangeID);
			//pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pOrder->CombHedgeFlag[0]);
			//pField->Side = TThostFtdcDirectionType_2_OrderSide(pOrder->Direction);
			//pField->Price = pOrder->LimitPrice;
			//pField->StopPx = pOrder->StopPrice;
			//strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
			//pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pOrder->CombOffsetFlag[0]);
			//pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
			//pField->Qty = pOrder->VolumeTotalOriginal;
			//pField->Type = CThostFtdcOrderField_2_OrderType(pOrder);
			//pField->TimeInForce = CThostFtdcOrderField_2_TimeInForce(pOrder);
			if (pField->ErrorID != 0)
			{
				pField->ExecType = ExecType::ExecRejected;
				pField->Status = OrderStatus::Rejected;
			}
			else
			{
				pField->ExecType = ExecType::ExecNew;
				pField->Status = OrderStatus::New;
			}

			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
		}
		else
		{
			pField = it->second;
			//strcpy(pField->ID, orderId);
			//pField->LeavesQty = pOrder->VolumeTotal;
			//pField->Price = pOrder->LimitPrice;
			//pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
			//pField->ExecType = CThostFtdcOrderField_2_ExecType(pOrder);
			//strcpy(pField->OrderID, pOrder->OrderSysID);
			//strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
		}

		m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults, count);
	DeleteErrors(ppErrs, count);

	return 0;
}

int CTraderApi::ReqOrderAction(OrderIDType* szId, int count, OrderIDType* pOutput)
{
	Order_STRUCT *pOrder = new Order_STRUCT[count];

	for (int i = 0; i < count; ++i)
	{
		unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(szId[i]);
		if (it == m_id_platform_order.end())
		{
			// 没找到怎么办？第一个就没找到怎么办？
			
		}
		else
		{
			memcpy(&pOrder[i], it->second, sizeof(Order_STRUCT));
		}
	}

	int nRet = ReqOrderAction(pOrder, count, pOutput);
	delete[] pOrder;
	return nRet;
}

//void BuildCancelOrder(OrderFieldEx* pIn, STOrderCancel* pOut)
//{
//	strcpy(pOut->cust_no,pIn->cust_no);
//	pOut->market_code = pIn->market_code;
//	pOut->ordercancel_type = 1;
//	pOut->order_no = pIn->order_no;
//}

int CTraderApi::ReqOrderAction(Order_STRUCT *pOrder, int count, OrderIDType* pOutput)
{
	int row_num = 0;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT** ppErrs = nullptr;

	int n = m_pApi->CancelMultiOrders(pOrder, count, &ppFieldInfos, &ppResults, &ppErrs);

	for (int i = 0; i < count; ++i)
	{
		// 是否要将Account写回到原Order中呢？
		if (ppErrs)
		{
			if (ppErrs[i])
			{
				//pOrders[i].ErrorID = ppErrs[i]->ErrCode;
				//strcpy(pOrders[i].Text, ppErrs[i]->ErrInfo);
			}
		}
		if (ppResults)
		{
			if (ppResults[i*COL_EACH_ROW + 0])
			{
				//strcpy(pOrders[i].ID, ppResults[i*COL_EACH_ROW + 0]);
			}
		}
	}

	//STOrderCancel *p_ordercancel_req = new STOrderCancel[count];
	//STOrderCancelRsp *p_ordercancel_rsp = NULL;

	//for (int i = 0; i < count; ++i)
	//{
	//	BuildCancelOrder(&pOrder[i], &p_ordercancel_req[i]);
	//}

	//bool bRet = SPX_API_OrderCancel(m_pApi, p_ordercancel_req, &p_ordercancel_rsp, count, &row_num, &m_err_msg);

	//if (bRet && m_err_msg.error_no == 0)
	//{
	//	if (p_ordercancel_rsp != NULL)
	//	{
	//		for (int i = 0; i<row_num; i++)
	//		{
	//			if (p_ordercancel_rsp[i].error_no == 0)
	//			{
	//				// 这是撤单成功还是撤单失败了呢？
	//				memset(pOutput[i], 0, sizeof(OrderIDType));
	//				
	//				pOrder[i].Field.ErrorID = p_ordercancel_rsp[i].error_no;
	//				strncpy(pOrder[i].Field.Text, p_ordercancel_rsp[i].err_msg, sizeof(ErrorMsgType));
	//				pOrder[i].Field.ExecType = ExecType::ExecCancelReject;
	//				pOrder[i].Field.Status = OrderStatus::Rejected;

	//				m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, &pOrder[i].Field, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//			}
	//			else
	//			{
	//				memset(pOutput[i], 0, sizeof(OrderIDType));
	//				//sprintf(pOutput[i],"%d",p_ordercancel_rsp[i].error_no);

	//				pOrder[i].Field.ErrorID = p_ordercancel_rsp[i].error_no;
	//				strncpy(pOrder[i].Field.Text, p_ordercancel_rsp[i].err_msg, sizeof(ErrorMsgType));
	//				pOrder[i].Field.ExecType = ExecType::ExecCancelReject;
	//				// 撤单拒绝，状态要用上次的
	//				//pOrder[i].Field.Status = OrderStatus::Rejected;

	//				m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, &pOrder[i].Field, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		for (int i = 0; i < count; ++i)
	//		{
	//			//sprintf(pOutput[i], "%d", 0);
	//			memset(pOutput[i], 0, sizeof(OrderIDType));

	//			pOrder[i].Field.ErrorID = 0;
	//			strcpy(pOrder[i].Field.Text, "返回结果为空");
	//			pOrder[i].Field.ExecType = ExecType::ExecCancelReject;
	//			//pOrder[i].Field.Status = OrderStatus::Rejected;

	//			m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, &pOrder[i].Field, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//		}
	//	}
	//}
	//else
	//{
	//	// 出错了，全输出
	//	for (int i = 0; i < count; ++i)
	//	{
	//		//sprintf(pOutput[i], "%d", m_err_msg.error_no);
	//		memset(pOutput[i], 0, sizeof(OrderIDType));

	//		pOrder[i].Field.ErrorID = m_err_msg.error_no;
	//		strcpy(pOrder[i].Field.Text, m_err_msg.msg);
	//		pOrder[i].Field.ExecType = ExecType::ExecCancelReject;
	//		//pOrder[i].Field.Status = OrderStatus::Rejected;

	//		m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, &pOrder[i].Field, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//	}
	//}

	//delete[] p_ordercancel_req;

	return 0;
}

int CTraderApi::_ReqOrderAction(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 通过ID找到原始结构，用于撤单
	// 通过ID找到通用结构，用于接收回报
	OrderField* pOrders = (OrderField*)ptr1;
	int count = (int)size1 / sizeof(OrderField);

	Order_STRUCT* pTdxOrders = new Order_STRUCT[count];
	for (int i = 0; i < count; ++i)
	{
		BuildOrder(&pOrders[i], &pTdxOrders[i]);
	}

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT** ppErrs = nullptr;

	int n = m_pApi->CancelMultiOrders(pTdxOrders, count, &ppFieldInfos, &ppResults, &ppErrs);

	// 将结果立即取出来
	for (int i = 0; i < count; ++i)
	{
		// 是否要将Account写回到原Order中呢？
		if (ppErrs)
		{
			if (ppErrs[i])
			{
				pOrders[i].ErrorID = ppErrs[i]->ErrCode;
				strcpy(pOrders[i].Text, ppErrs[i]->ErrInfo);
			}
		}
		if (ppResults)
		{
			if (ppResults[i*COL_EACH_ROW + 0])
			{
				strcpy(pOrders[i].ID, ppResults[i*COL_EACH_ROW + 0]);
			}
		}

		OrderIDType orderId = { 0 };
		sprintf(orderId, "%s", pOrders[i].ID);

		OrderField* pField = nullptr;
		unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
		if (it == m_id_platform_order.end())
		{
			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

			memcpy(pField, &pOrders[i], sizeof(OrderField));

			//strcpy(pField->ID, orderId);
			//strcpy(pField->InstrumentID, pOrder->InstrumentID);
			//strcpy(pField->ExchangeID, pOrder->ExchangeID);
			//pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pOrder->CombHedgeFlag[0]);
			//pField->Side = TThostFtdcDirectionType_2_OrderSide(pOrder->Direction);
			//pField->Price = pOrder->LimitPrice;
			//pField->StopPx = pOrder->StopPrice;
			//strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
			//pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pOrder->CombOffsetFlag[0]);
			//pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
			//pField->Qty = pOrder->VolumeTotalOriginal;
			//pField->Type = CThostFtdcOrderField_2_OrderType(pOrder);
			//pField->TimeInForce = CThostFtdcOrderField_2_TimeInForce(pOrder);
			if (pField->ErrorID != 0)
			{
				pField->ExecType = ExecType::ExecRejected;
				pField->Status = OrderStatus::Rejected;
			}
			else
			{
				pField->ExecType = ExecType::ExecNew;
				pField->Status = OrderStatus::New;
			}

			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
		}
		else
		{
			pField = it->second;
			//strcpy(pField->ID, orderId);
			//pField->LeavesQty = pOrder->VolumeTotal;
			//pField->Price = pOrder->LimitPrice;
			//pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
			//pField->ExecType = CThostFtdcOrderField_2_ExecType(pOrder);
			//strcpy(pField->OrderID, pOrder->OrderSysID);
			//strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
		}

		m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults, count);
	DeleteErrors(ppErrs, count);

	return 0;
}

//void CTraderApi::ReqQryOrder(TCustNoType cust_no, TSecCodeType sec_code)
//{
//	STQueryOrder* pField = (STQueryOrder*)m_msgQueue->new_block(sizeof(STQueryOrder));
//	strcpy(pField->cust_no, cust_no);
//	strcpy(pField->sec_code, sec_code);
//	pField->market_code = '0';
//	pField->order_no = 0;
//	pField->query_type = 0;
//
//	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryOrderField, m_msgQueue_Query, this, 0, 0,
//		pField, sizeof(STQueryOrder), nullptr, 0, nullptr, 0);
//}

int CTraderApi::_ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int row_num = 0;

	//STQueryOrder *p_qry_order_req = (STQueryOrder *)ptr1;
	//STOrderInfo *p_qry_order_rsp = NULL;

	//bool bRet = SPX_API_QueryOrder(m_pApi, p_qry_order_req, &p_qry_order_rsp, &row_num, &m_err_msg);

	//if (bRet && m_err_msg.error_no == 0)
	//{
	//	if (p_qry_order_rsp != NULL)
	//	{
	//		for (int i = 0; i<row_num; i++)
	//		{
	//			OrderIDType ID = { 0 };
	//			sprintf(ID,"%d",p_qry_order_rsp[i].order_no);

	//			OrderFieldEx* pField = nullptr;
	//			
	//			unordered_map<string, OrderFieldEx*>::iterator it = m_id_platform_order.find(ID);
	//			if (it == m_id_platform_order.end())
	//			{
	//				// 订单可能不存在，要进行还原
	//				pField = (OrderFieldEx*)m_msgQueue->new_block(sizeof(OrderFieldEx));

	//				strncpy(pField->cust_no, p_qry_order_req->cust_no, sizeof(TCustNoType));
	//				strncpy(pField->holder_acc_no, p_qry_order_rsp[i].holder_acc_no, sizeof(THolderNoType));
	//				pField->batch_no = p_qry_order_rsp[i].batch_no;
	//				pField->order_no = p_qry_order_rsp[i].order_no;
	//				pField->market_code = p_qry_order_rsp[i].market_code;

	//				pField->Field.Status = TOrderStatusType_2_OrderStatus(p_qry_order_rsp[i].order_status);
	//				pField->Field.ExecType = TOrderStatusType_2_ExecType(p_qry_order_rsp[i].order_status);
	//				pField->Field.Price = p_qry_order_rsp[i].price;
	//				pField->Field.Qty = p_qry_order_rsp[i].order_vol;
	//				pField->Field.CumQty = p_qry_order_rsp[i].done_vol;
	//				pField->Field.LeavesQty = p_qry_order_rsp[i].order_vol - p_qry_order_rsp[i].done_vol - p_qry_order_rsp[i].cancel_vol;
	//				pField->Field.Time = p_qry_order_rsp[i].order_time;
	//				strcpy(pField->Field.ID, ID);
	//				strcpy(pField->Field.InstrumentID, p_qry_order_rsp[i].sec_code);
	//				sprintf(pField->Field.ExchangeID, "%c", p_qry_order_rsp[i].market_code);

	//				m_id_platform_order.insert(pair<string, OrderFieldEx*>(ID, pField));
	//			}
	//			else
	//			{
	//				pField = it->second;

	//				pField->Field.Status = TOrderStatusType_2_OrderStatus(p_qry_order_rsp[i].order_status);
	//				pField->Field.ExecType = TOrderStatusType_2_ExecType(p_qry_order_rsp[i].order_status);

	//				pField->Field.Price = p_qry_order_rsp[i].price;
	//				pField->Field.Qty = p_qry_order_rsp[i].order_vol;
	//				pField->Field.CumQty = p_qry_order_rsp[i].done_vol;
	//				pField->Field.LeavesQty = p_qry_order_rsp[i].order_vol - p_qry_order_rsp[i].done_vol - p_qry_order_rsp[i].cancel_vol;
	//				pField->Field.Time = p_qry_order_rsp[i].order_time;
	//				strcpy(pField->Field.InstrumentID, p_qry_order_rsp[i].sec_code);
	//				sprintf(pField->Field.ExchangeID, "%c", p_qry_order_rsp[i].market_code);
	//				
	//			}
	//			
	//			m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, &pField->Field, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//		}
	//	}
	//	else
	//	{
	//		//cout << "返回结果为空" << endl;
	//		// 反回为空，表示已经查过一次，没有新的生成，不用处理
	//		// 但如果是多账号，会出现对每个查询都自动变成全量了，看来是游标被重置了,如何处理
	//		//return 0;
	//	}
	//}
	//else
	//{
	//	//cout << "QueryOrder error:" << err_msg.error_no << err_msg.msg << endl;
	//	ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

	//	pField->ErrorID = m_err_msg.error_no;
	//	strcpy(pField->ErrorMsg, m_err_msg.msg);

	//	m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	//}

	return 0;
}

//void CTraderApi::ReqQryTrade(TCustNoType cust_no, TSecCodeType sec_code)
//{
//	STQueryDone* pField = (STQueryDone*)m_msgQueue->new_block(sizeof(STQueryDone));
//	strcpy(pField->cust_no, cust_no);
//	strcpy(pField->sec_code, sec_code);
//	pField->market_code = '0';
//	pField->query_type = 0;
//
//	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryTradeField, m_msgQueue_Query, this, 0, 0,
//		pField, sizeof(STQueryDone), nullptr, 0, nullptr, 0);
//}

int CTraderApi::_ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int row_num = 0;

	//STQueryDone *p_qry_done_req = (STQueryDone *)ptr1;
	//STDoneInfo *p_qry_done_rsp = NULL;

	//bool bRet = SPX_API_QueryDone(m_pApi, p_qry_done_req, &p_qry_done_rsp, &row_num, &m_err_msg);

	//if (bRet && m_err_msg.error_no == 0)
	//{
	//	if (p_qry_done_rsp != NULL)
	//	{
	//		//cout << "QueryOrder OK." << endl;
	//		for (int i = 0; i<row_num; i++)
	//		{
	//			//cout << p_qry_order_rsp[i].market_code << " " << p_qry_order_rsp[i].sec_code << " " << p_qry_order_rsp[i].batch_no << " " << p_qry_order_rsp[i].order_no << " " << p_qry_order_rsp[i].bs << " ";
	//			//cout << p_qry_order_rsp[i].order_date << " " << p_qry_order_rsp[i].price << " " << p_qry_order_rsp[i].order_vol << " " << p_qry_order_rsp[i].order_status << " " << p_qry_order_rsp[i].done_price << endl;
	//			TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	//			strcpy(pField->InstrumentID, p_qry_done_rsp[i].sec_code);
	//			sprintf(pField->ExchangeID, "%c", p_qry_done_rsp[i].market_code);

	//			pField->Side = TBSFLAG_2_OrderSide(p_qry_done_rsp[i].bs);
	//			pField->Qty = p_qry_done_rsp[i].done_vol;
	//			pField->Price = p_qry_done_rsp[i].done_price;
	//			//pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	//			//pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	//			pField->Commission = 0;//TODO收续费以后要计算出来
	//			pField->Time = p_qry_done_rsp[i].done_time;
	//			strcpy(pField->TradeID, p_qry_done_rsp[i].done_no);
	//			sprintf(pField->ID, "%d", p_qry_done_rsp[i].order_no);

	//			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

	//			//OrderIDType orderSysId = { 0 };
	//			//sprintf(orderSysId, "%s:%s", pTrade->ExchangeID, pTrade->OrderSysID);
	//			//unordered_map<string, string>::iterator it = m_sysId_orderId.find(pField->ID);
	//			//if (it == m_sysId_orderId.end())
	//			//{
	//			//	// 此成交找不到对应的报单
	//			//	//assert(false);
	//			//}
	//			//else
	//			//{
	//			//	// 找到对应的报单
	//			//	strcpy(pField->ID, it->second.c_str());

	//			//	

	//			//	//unordered_map<string, OrderField*>::iterator it2 = m_id_platform_order.find(it->second);
	//			//	//if (it2 == m_id_platform_order.end())
	//			//	//{
	//			//	//	// 此成交找不到对应的报单
	//			//	//	//assert(false);
	//			//	//}
	//			//	//else
	//			//	//{
	//			//	//	// 更新订单的状态
	//			//	//	// 是否要通知接口
	//			//	//}

	//			//	//OnTrade(pField);
	//			//}
	//		}
	//	}
	//	else
	//	{
	//		//cout << "返回结果为空" << endl;
	//	}
	//}
	//else
	//{
	//	//cout << "QueryOrder error:" << err_msg.error_no << err_msg.msg << endl;
	//	ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

	//	pField->ErrorID = m_err_msg.error_no;
	//	strcpy(pField->ErrorMsg, m_err_msg.msg);

	//	m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	//}
	return 0;
}

void CTraderApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{

}

//void CTraderApi::OnPST16203PushData(PST16203PushData pEtxPushData)
//{
//	OrderIDType orderId = { 0 };
//
//	// 只是打印成交
//	ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
//
//	//pField->ErrorID = pRspInfo->error_no;
//	sprintf(pField->ErrorMsg,"OnPST16203PushData %s",pEtxPushData->order_status_name);
//
//	m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
//}
//
//void CTraderApi::OnPST16204PushData(PST16204PushData pEtxPushData)
//{
//	OrderIDType orderId = { 0 };
//
//	// 只是打印成交
//	ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
//
//	//pField->ErrorID = pRspInfo->error_no;
//	sprintf(pField->ErrorMsg, "OnPST16204PushData %s", pEtxPushData->order_status_name);
//
//	m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
//}