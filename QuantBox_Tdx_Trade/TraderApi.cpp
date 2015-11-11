#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/ApiProcess.h"
#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include "TypeConvert.h"

#include "../include/Tdx/tdx_function.h"

#include <cstring>
#include <assert.h>


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
	case E_QryTradingAccountField:
		iRet = _ReqQryTradingAccount(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryInvestorPositionField:
		iRet = _ReqQryInvestorPosition(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryDepthMarketDataField:
		iRet = _Subscribe(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

void* __stdcall Test(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
	pApi->TestInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	bool bQryOrder = false;
	bool bQryTrade = false;
	//bool bQryInvestor = false;

	time_t _now = time(nullptr);

	// 想在整点时查询一下，因为整点时交易所和柜台变状态的可能性比较大这样

	if (_now > m_QueryOrderTime)
	{
		bQryOrder = true;
	}
	else
	{
		
	}

	if (_now > m_QueryTradeTime)
	{
		bQryTrade = true;
	}
	else
	{

	}

	//if (_now > m_QueryGDLBTime)
	//{
	//	bQryInvestor = true;
	//}
	//else
	//{

	//}

	//if (bQryInvestor)
	//{
	//	double _queryTime = QUERY_TIME_MAX * 60;
	//	m_QueryGDLBTime = time(nullptr) + _queryTime;
	//	OutputQueryTime(m_QueryGDLBTime, _queryTime, "QueryInvestor");

	//	ReqQryInvestor();
	//}

	if (bQryOrder)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "QueryOrder");

		ReqQryOrder();
	}

	if (bQryTrade)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "QueryTrade");

		ReqQryTrade();
	}

	this_thread::sleep_for(chrono::milliseconds(1000));
	m_msgQueue_Test->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
}

void CTraderApi::OutputQueryTime(time_t t, double db, const char* szSource)
{
	ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

	pField->ErrorID = 0;
	sprintf(pField->ErrorMsg, "Time:%s,Add:%d", ctime(&t), (int)db);
	strcpy(pField->Source, szSource);

	m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
}


int CTraderApi::_Init()
{
	if (m_pApi == nullptr)
	{
		m_pApi = CTdxApi::CreateApi(m_ServerInfo.ExtendInformation);
	}

	Error_STRUCT* pErr = nullptr;

	m_pApi->LoadScript(m_ServerInfo.Address, true, false);
	m_pApi->Init(m_ServerInfo.ExtendInformation, &pErr);

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

	m_pClient = m_pApi->Login(m_UserInfo.UserID, m_UserInfo.Password, m_UserInfo.ExtInfo, &ppResults, &pErr);

	if (m_pClient)
	{
		// 有授权信息要输出
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		if (pErr)
		{
			pField->ErrorID = pErr->ErrCode;
			strcpy(pField->ErrorMsg, pErr->ErrInfo);
		}

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Logined, 0, pField, sizeof(pField), nullptr, 0, nullptr, 0);
		
		// 以后支持多账号时这地方要改
		m_pApi->SetClient(m_pClient);
		m_pApi->SetAccount(m_UserInfo.UserID);

		// 查询股东列表，华泰证券可能一开始查会返回非知请求[1122]
		GDLB_STRUCT** ppRS = nullptr;
		CharTable2Login(ppResults, &ppRS);

		int count = GetCountStructs((void**)ppRS);

		if (count>0)
		{
			for (int i = 0; i < count; ++i)
			{
				InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

				GDLB_2_InvestorField(ppRS[i], pField);

				m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
			}
		}
		else
		{
			// 查通达信仿真实验室账号不直接返回股东列表
			ReqQryInvestor();
		}
		
		// 启动定时查询功能使用
		m_msgQueue_Test->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue_Test, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		// 登录下一个账号
		//++m_UserInfo_Pos;
		//ReqUserLogin();
	}
	else
	{
		if (pErr)
		{
			RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

			pField->ErrorID = pErr->ErrCode;
			strcpy(pField->ErrorMsg, pErr->ErrInfo);

			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		}
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
	if (m_pApi == nullptr)
		return 0;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_GDLB, &ppFieldInfos, &ppResults, &pErr);

	if (IsErrorRspInfo("ReqQryInvestor", pErr))
	{
		int _queryTime = QUERY_TIME_MIN;
		m_QueryGDLBTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryGDLBTime, _queryTime, "NextQueryInvestor_ReqQryInvestor");

		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	GDLB_STRUCT** ppRS = nullptr;
	CharTable2GDLB(ppFieldInfos, ppResults, &ppRS);

	int count = GetCountStructs((void**)ppRS);

	for (int i = 0; i < count; ++i)
	{
		InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

		GDLB_2_InvestorField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	return 0;
}

CTraderApi::CTraderApi(void)
{
	m_pIDGenerator = nullptr;
	m_pApi = nullptr;
	m_pClient = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();
	m_msgQueue_Test = new CMsgQueue();

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue_Query->StartThread();

	m_msgQueue_Test->Register(Test, this);
	m_msgQueue_Test->StartThread();
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
	m_msgQueue_Test->Register(Test, this);
	m_msgQueue->Register(pCallback,this);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
		m_msgQueue_Test->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
		m_msgQueue_Test->StopThread();
	}
}

bool CTraderApi::IsErrorRspInfo(const char* szSource, Error_STRUCT *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrType != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->ErrorID = pRspInfo->ErrCode;
		strcpy(pField->ErrorMsg, pRspInfo->ErrInfo);
		strcpy(pField->Source, szSource);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pUserInfos = (UserInfoField*)(new char[sizeof(UserInfoField)*count]);
	memcpy(m_pUserInfos, pUserInfo, sizeof(UserInfoField)*count);

	m_UserInfo_Pos = 0;
	m_UserInfo_Count = count;

	m_pIDGenerator = new CIDGenerator();
	m_pIDGenerator->SetPrefix(m_UserInfo.UserID);

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

	if (m_msgQueue_Test)
	{
		m_msgQueue_Test->StopThread();
		m_msgQueue_Test->Register(nullptr, nullptr);
		m_msgQueue_Test->Clear();
		delete m_msgQueue_Test;
		m_msgQueue_Test = nullptr;
	}

	if(m_pApi)
	{
		// 还没有登出
		m_pApi->Logout(m_pClient);
		m_pClient = nullptr;
		m_pApi->Exit();
		
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		if (m_msgQueue)
		{
			m_msgQueue->Clear();
			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			// 主动触发
			m_msgQueue->Process();
		}
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
	memset(pInOut, 0, sizeof(OrderIDType)*count);

	if (count < 1)
		return 0;	

	OrderField** ppOrders = new OrderField*[count];
	
	// 生成本地ID，供上层进行定位使用
	for (int i = 0; i < count; ++i)
	{
		OrderField* pNewOrder = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
		memcpy(pNewOrder, pOrder, sizeof(OrderField));

		strcpy(pInOut[i], m_pIDGenerator->GetIDString());
		strcpy(pNewOrder[i].LocalID, pInOut[i]);
		ppOrders[i] = pNewOrder;

		// 注意这里保存了最开始发单的结构体的备份
		m_id_platform_order.insert(pair<string, OrderField*>(pNewOrder->LocalID, pNewOrder));
	}

	m_msgQueue_Query->Input_Copy(RequestType::E_InputOrderField, m_msgQueue_Query, this, 0, 0,
		ppOrders, sizeof(OrderField*)*count, nullptr, 0, nullptr, 0);

	delete[] ppOrders;
	
	return 0;
}

int CTraderApi::_ReqOrderInsert(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	// 得到报单指针列表
	OrderField** ppOrders = (OrderField**)ptr1;
	int count = (int)size1 / sizeof(OrderField*);

	Order_STRUCT** ppTdxOrders = new Order_STRUCT*[count];
	for (int i = 0; i < count; ++i)
	{
		ppTdxOrders[i] = (Order_STRUCT*)m_msgQueue->new_block(sizeof(Order_STRUCT));
		OrderField_2_Order_STRUCT(ppOrders[i], ppTdxOrders[i]);
	}

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT** ppErrs = nullptr;

	// 注意：pTdxOrders在这里被修改了，需要使用修改后的东西
	int n = m_pApi->SendMultiOrders(ppTdxOrders, count, &ppFieldInfos, &ppResults, &ppErrs);

	// 标记批量下单是否有发成功过单的
	bool bSuccess = false;
	// 将结果立即取出来
	for (int i = 0; i < count;++i)
	{
		m_id_api_order.insert(pair<string, Order_STRUCT*>(ppOrders[i]->LocalID, ppTdxOrders[i]));
		// 处理错误
		if (ppErrs && ppErrs[i])
		{
			ppOrders[i]->ErrorID = ppErrs[i]->ErrCode;
			strcpy(ppOrders[i]->Text, ppErrs[i]->ErrInfo);
		}

		// 处理结果
		if (ppResults && ppResults[i*COL_EACH_ROW + 0])
		{
			bSuccess = true;
			// 写上柜台的ID，以后将基于此进行定位
			strcpy(ppOrders[i]->ID, ppResults[i*COL_EACH_ROW + 0]);

			m_id_api_order.erase(ppOrders[i]->LocalID);
			m_id_api_order.insert(pair<string, Order_STRUCT*>(ppOrders[i]->ID, ppTdxOrders[i]));

			m_id_platform_order.erase(ppOrders[i]->LocalID);
			m_id_platform_order.insert(pair<string, OrderField*>(ppOrders[i]->ID, ppOrders[i]));
		}

		// 现在有两个结构体，需要进行操作了
		// 1.通知下单的结果
		// 2.记录下单
		
		OrderField* pField = ppOrders[i];
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
		
		m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	// 测试用，不能写这，太快了，要等一下
	//ReqQryTrade();
	
	// 复制完了，可以删了以前东西
	delete[] ppTdxOrders;

	DeleteTableBody(ppResults, count);
	DeleteErrors(ppErrs, count);

	if (bSuccess)
	{
		// 有挂单的，需要进行查询了
		
		double  _queryTime = QUERY_TIME_MIN;

		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_Send");
	}

	return 0;
}

int CTraderApi::ReqOrderAction(OrderIDType* szId, int count, OrderIDType* pOutput)
{
	memset(pOutput, 0, sizeof(OrderIDType)*count);

	OrderField** ppOrders = new OrderField*[count];
	Order_STRUCT** ppTdxOrders = new Order_STRUCT*[count];

	for (int i = 0; i < count; ++i)
	{
		ppOrders[i] = nullptr;
		ppTdxOrders[i] = nullptr;

		{
			unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(szId[i]);
			if (it != m_id_platform_order.end())
				ppOrders[i] = it->second;
		}
		
		{
			unordered_map<string, Order_STRUCT*>::iterator it = m_id_api_order.find(szId[i]);
			if (it != m_id_api_order.end())
				ppTdxOrders[i] = it->second;
		}
	}

	m_msgQueue_Query->Input_Copy(RequestType::E_InputOrderActionField, m_msgQueue_Query, this, 0, 0,
		ppOrders, sizeof(OrderField*)*count, ppTdxOrders, sizeof(Order_STRUCT*)*count, nullptr, 0);

	delete[] ppOrders;
	delete[] ppTdxOrders;

	return 0;
}

int CTraderApi::_ReqOrderAction(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int count = (int)size1 / sizeof(OrderField*);
	// 通过ID找到原始结构，用于撤单
	// 通过ID找到通用结构，用于接收回报
	OrderField** ppOrders = (OrderField**)ptr1;
	Order_STRUCT** ppTdxOrders = (Order_STRUCT**)ptr2;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT** ppErrs = nullptr;

	int n = m_pApi->CancelMultiOrders(ppTdxOrders, count, &ppFieldInfos, &ppResults, &ppErrs);

	bool bSuccess = false;
	// 将结果立即取出来
	for (int i = 0; i < count; ++i)
	{
		if (ppErrs)
		{
			if (ppErrs[i])
			{
				ppOrders[i]->ErrorID = ppErrs[i]->ErrCode;
				strcpy(ppOrders[i]->Text, ppErrs[i]->ErrInfo);

				ppOrders[i]->ExecType = ExecType::ExecCancelReject;
				// 注意报单状态问题
			}
			else
			{
				bSuccess = true;

				// 会不会出现撤单时，当时不知道是否成功撤单，查询才得知没有撤成功？
				//ppOrders[i]->ExecType = ExecType::ExecCancelled;
				//ppOrders[i]->Status = OrderStatus::Cancelled;
				continue;
			}
		}
		// 撤单成功时，返回的东西还是null,所以这里使用错误信息来进行区分
		
		//if (ppResults)
		//{	
		//	if (ppResults[i*COL_EACH_ROW + 0])
		//	{
		//	}
		//}

		m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, ppOrders[i], sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults, count);
	DeleteErrors(ppErrs, count);

	if (bSuccess)
	{
		// 需要进行查询了
		double  _queryTime = QUERY_TIME_MIN;

		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_Cancel");
	}

	return 0;
}

void CTraderApi::ReqQryOrder()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryOrderField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_DRWT, &ppFieldInfos, &ppResults, &pErr);
	// 测试用，事后要删除
	//m_pApi->ReqQueryData(REQUEST_LSWT, &ppFieldInfos, &ppResults, &pErr, "20150801", "20151031");

	if (IsErrorRspInfo("ReqQryOrder", pErr))
	{
		double _queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		//OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_QueryOrder_Error");

		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	WTLB_STRUCT** ppRS = nullptr;
	CharTable2WTLB(ppFieldInfos, ppResults, &ppRS);

	// 操作前清空，按说之前已经清空过一次了
	m_NewOrderList.clear();

	// 有未完成的，标记为true
	bool IsDone = true;
	// 有未申报的，标记为true
	bool IsNotSent = false;
	// 有更新的
	bool IsUpdated = false;

	if (ppRS)
	{
		int i = 0;
		while (ppRS[i])
		{
			// 将撤单委托过滤
			if (ppRS[i]->MMBZ_ != MMBZ_Cancel)
			{
				// 需要将它输入到一个地方用于计算
				OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

				WTLB_2_OrderField_0(ppRS[i], pField);
				m_NewOrderList.push_back(pField);

				if (!ZTSM_IsDone(ppRS[i]->ZTSM_))
				{
					IsDone = false;
				}
				if (ZTSM_IsNotSent(ppRS[i]->ZTSM_))
				{
					IsNotSent = true;
				}
			}
			++i;
		}
	}

	// 委托列表
	// 1.新增的都需要输出
	// 2.老的看是否有变化
	++m_OrderNotUpdateCount;
	
	int i = 0;
	list<OrderField*>::iterator it2 = m_OldOrderList.begin();
	for (list<OrderField*>::iterator it = m_NewOrderList.begin(); it != m_NewOrderList.end(); ++it)
	{
		OrderField* pField = *it;

		bool bUpdate = false;
		if (i >= m_OldOrderList.size())
		{
			bUpdate = true;
		}
		else
		{
			// 相同位置的部分
			OrderField* pOldField = *it2;
			if (pOldField->LeavesQty != pField->LeavesQty || pOldField->Status != pField->Status)
			{
				bUpdate = true;
			}
		}

		if (bUpdate)
		{
			IsUpdated = true;
			m_OrderNotUpdateCount = 0;

			// 如果能找到下单时的委托，就修改后发出来
			unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(pField->ID);
			if (it != m_id_platform_order.end())
			{
				OrderField* pField_ = it->second;
				pField_->Date = pField->Date;
				pField_->Time = pField->Time;
				pField_->CumQty = pField->CumQty;
				pField_->LeavesQty = pField->LeavesQty;
				pField_->AvgPx = pField->AvgPx;
				pField_->Status = pField->Status;
				pField_->ExecType = pField->ExecType;
				strcpy(pField_->Text, pField->Text);

				pField = pField_;
			}
			
			m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}

		// 前一个可能为空，移动到下一个时需要注意
		if (it2 != m_OldOrderList.end())
		{
			++it2;
		}

		++i;
	}

	// 将老数据清理，防止内存泄漏
	for (list<OrderField*>::iterator it = m_OldOrderList.begin(); it != m_OldOrderList.end(); ++it)
	{
		OrderField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// 做交换
	m_OldOrderList.clear();
	m_OldOrderList = m_NewOrderList;
	m_NewOrderList.clear();

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	double _queryTime = 0;
	if (!IsDone)
	{
		if (!IsUpdated)
		{
			// 没有更新，是否要慢点查
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}
		
		// 有没有完成的，需要定时查询
		if (IsNotSent)
		{
			// 有没申报的，是否没在交易时间？慢点查
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}
		else
		{
			// 可能是交易时间了，是否需要考虑
			_queryTime = 2 * QUERY_TIME_MIN;
			// 可能有些挂单一天都不会成交，挂在这一直导致查太多，加一下查询计数
			if (m_OrderNotUpdateCount>=3)
			{
				_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
			}
		}
	}
	else
	{
		// 全完成了，可以不查或慢查
		_queryTime = 5 * QUERY_TIME_MAX;
	}

	m_QueryOrderTime = time(nullptr) + _queryTime;
	OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_QueryOrder");

	// 决定成交查询间隔
	if (IsUpdated)
	{
		// 委托可能是撤单，也有可能是成交了，赶紧查一下
		_queryTime = 0;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}
	else
	{
		// 委托没有变化，那成交就没有必要查那么快了
		_queryTime = 5 * QUERY_TIME_MAX;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}
	

	return 0;
}

void CTraderApi::ReqQryTrade()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryTradeField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_DRCJ, &ppFieldInfos, &ppResults, &pErr);
	// 测试用，事后要删除
	//m_pApi->ReqQueryData(REQUEST_LSCJ, &ppFieldInfos, &ppResults, &pErr, "20150801", "20151031");

	if (IsErrorRspInfo("ReqQryTrade", pErr))
	{
		double _queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		m_QueryTradeTime = time(nullptr) + _queryTime;

		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	CJLB_STRUCT** ppRS = nullptr;
	CharTable2CJLB(ppFieldInfos, ppResults, &ppRS);

	// 操作前清空，按说之前已经清空过一次了
	m_NewTradeList.clear();

	if (ppRS)
	{
		int i = 0;
		while (ppRS[i])
		{
			TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));

			CJLB_2_TradeField(ppRS[i], pField);

			m_NewTradeList.push_back(pField);

			++i;
		}
	}

	// 成交列表比较简单，只要新出现的数据就认为是有变化，需要输出 
	int i = 0;
	for (list<TradeField*>::iterator it = m_NewTradeList.begin(); it != m_NewTradeList.end(); ++it)
	{
		if (i >= m_OldTradeList.size())
		{
			TradeField* pField = *it;
			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}
		++i;
	}

	// 将老数据清理，防止内存泄漏
	for (list<TradeField*>::iterator it = m_OldTradeList.begin(); it != m_OldTradeList.end(); ++it)
	{
		TradeField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// 做交换
	m_OldTradeList.clear();
	m_OldTradeList = m_NewTradeList;
	m_NewTradeList.clear();

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	return 0;
}

void CTraderApi::ReqQryInstrument(const string& szInstrumentId, const string& szExchange)
{

}

void CTraderApi::ReqQryTradingAccount()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryTradingAccountField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_ZJYE, &ppFieldInfos, &ppResults, &pErr);

	if (IsErrorRspInfo("ReqQryTradingAccount", pErr))
	{
		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	ZJYE_STRUCT** ppRS = nullptr;
	CharTable2ZJYE(ppFieldInfos, ppResults, &ppRS);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));

		ZJYE_2_AccountField(ppRS[i], pField);

		// 可能资金账号查不出来，手工填上
		if (strlen(pField->Account) <= 0)
		{
			// 多账户会有问题
			strcpy(pField->Account, m_pApi->GetAccount());
		}

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	//double _queryTime = 5 * QUERY_TIME_MAX;
	//m_QueryTradeTime = time(nullptr) + _queryTime;
	//OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");

	return 0;
}

void CTraderApi::ReqQryInvestorPosition()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryInvestorPositionField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_GFLB, &ppFieldInfos, &ppResults, &pErr);

	if (IsErrorRspInfo("ReqQryInvestorPosition", pErr))
	{
		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	GFLB_STRUCT** ppRS = nullptr;
	CharTable2GFLB(ppFieldInfos, ppResults, &ppRS);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		GFLB_2_PositionField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	return 0;
}

void CTraderApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	char buf[64] = { 0 };
	strcpy(buf, szInstrumentIDs.c_str());

	m_msgQueue_Query->Input_Copy(RequestType::E_QryDepthMarketDataField, m_msgQueue_Query, this, 0, 0,
		buf, sizeof(buf), nullptr, 0, nullptr, 0);
}

int CTraderApi::_Subscribe(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	FieldInfo_STRUCT** ppFieldInfos = nullptr;
	char** ppResults = nullptr;
	Error_STRUCT* pErr = nullptr;

	m_pApi->ReqQueryData(REQUEST_HQ, &ppFieldInfos, &ppResults, &pErr, "", "", (char*)ptr1);

	if (IsErrorRspInfo("Subscribe", pErr))
	{
		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	HQ_STRUCT** ppRS = nullptr;
	CharTable2HQ(ppFieldInfos, ppResults, &ppRS);

	int count = GetCountStructs((void**)ppRS);

	for (int i = 0; i < count; ++i)
	{
		DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField)+sizeof(DepthField)* 10);

		HQ_STRUCT* pDepthMarketData = ppRS[i];

		strcpy(pField->InstrumentID, pDepthMarketData->ZQDM);
		//pField->Exchange = JYSDM_2_ExchangeType(pDepthMarketData->JYSDM);

		sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, "");

		// 交易时间
		GetExchangeTime(time(nullptr), &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime);


		pField->LastPrice = pDepthMarketData->DQJ_;
		//pField->Volume = 0;
		//pField->Turnover = pDepthMarketData->Turnover;
		//pField->OpenInterest = pDepthMarketData->OpenInterest;
		//pField->AveragePrice = pDepthMarketData->AveragePrice;
		pField->UpperLimitPrice = pDepthMarketData->ZTJG_;
		pField->LowerLimitPrice = pDepthMarketData->DTJG_;

		pField->PreClosePrice = pDepthMarketData->ZSJ_;
		pField->OpenPrice = pDepthMarketData->JKJ_;

		InitBidAsk(pField);

		do
		{
			if (pDepthMarketData->BidSize1_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice1_, pDepthMarketData->BidSize1_, 0);

			if (pDepthMarketData->BidSize2_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice2_, pDepthMarketData->BidSize2_, 0);

			if (pDepthMarketData->BidSize3_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice3_, pDepthMarketData->BidSize3_, 0);

			if (pDepthMarketData->BidSize4_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice4_, pDepthMarketData->BidSize4_, 0);

			if (pDepthMarketData->BidSize5_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice5_, pDepthMarketData->BidSize5_, 0);
		} while (false);

		do
		{
			if (pDepthMarketData->AskSize1_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice1_, pDepthMarketData->AskSize1_, 0);

			if (pDepthMarketData->AskSize2_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice2_, pDepthMarketData->AskSize2_, 0);

			if (pDepthMarketData->AskSize3_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice3_, pDepthMarketData->AskSize3_, 0);

			if (pDepthMarketData->AskSize4_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice4_, pDepthMarketData->AskSize4_, 0);

			if (pDepthMarketData->AskSize5_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice5_, pDepthMarketData->AskSize5_, 0);
		} while (false);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, DepthLevelType::FULL, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}

	DeleteTableBody(ppResults);
	DeleteError(pErr);

	return 0;
}