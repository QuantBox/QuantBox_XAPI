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

CTraderApi* CTraderApi::pThis = nullptr;

void __stdcall CTraderApi::OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData)
{
	// 这个类在一个dll不能实例化多次了，因为使用了static变量
	pThis->_OnReadPushData(FuncNO, pEtxPushData);
}

//客户端实现推送回调函数
void CTraderApi::_OnReadPushData(ETX_APP_FUNCNO FuncNO, void* pEtxPushData)
{
	switch (FuncNO)
	{
	case ETX_16203:
		OnPST16203PushData((PST16203PushData)pEtxPushData);
		break;
	case ETX_16204:
		OnPST16204PushData((PST16204PushData)pEtxPushData);
		break;
	default:
	{
			   char szmsg[128] = { 0 };
			   sprintf(szmsg, "无法识别的推送数据[%d]", FuncNO);

			   ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

			   pField->ErrorID = FuncNO;
			   strcpy(pField->ErrorMsg, szmsg);

			   m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, this, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
		break;
	}
}

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
	//网关地址与代理信息由客户自行配置（根据实际情况配置，无则不配置）
	//配置网关信息

	char szPost[20] = { 0 };
	_itoa(m_ServerInfo.Port, szPost, 10);

	SPX_API_SetParam(MAINSERVER_IP, m_ServerInfo.Address, &m_err_msg);
	SPX_API_SetParam(MAINSERVER_PORT, szPost, &m_err_msg);
	//SPX_API_SetParam(BACKSERVER_IP, "127.0.0.1", &m_err_msg);
	//SPX_API_SetParam(BACKSERVER_PORT, "17990", &m_err_msg);
	//配置代理信息
	//SPX_API_SetParam(PROXY_TYPE, "1", &m_err_msg);
	//SPX_API_SetParam(PROXY_IP, "127.0.0.1", &m_err_msg);
	//SPX_API_SetParam(PROXY_PORT, "9999", &m_err_msg);
	//SPX_API_SetParam(PROXY_USER, "", &m_err_msg);
	//SPX_API_SetParam(PROXY_PASS, "", &m_err_msg);


	STInitPara init_para;
	init_para.pOnReadPushData = OnReadPushData;
	init_para.bWriteLog = false;//这个有可能导致要用管理员权限才能读写目录
	init_para.emLogLevel = LL_INFO;
	init_para.nTimeOut = 60000;

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Uninitialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	bool bRet = SPX_API_Initialize(&init_para, &m_err_msg);

	if (!bRet)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		//连接失败返回的信息是拼接而成，主要是为了统一输出
		pField->ErrorID = m_err_msg.error_no;
		strcpy(pField->ErrorMsg, m_err_msg.msg);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		return 0;
	}

	if (m_err_msg.error_no != 0)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = m_err_msg.error_no;
		strcpy(pField->ErrorMsg, m_err_msg.msg);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	m_pApi = SPX_API_CreateHandle(&m_err_msg);
	if (m_pApi == nullptr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = m_err_msg.error_no;
		strcpy(pField->ErrorMsg, m_err_msg.msg);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}
	
	ReqUserLogin();

	return 0;
}

void CTraderApi::ReqUserLogin()
{
	STTraderLogin* pBody = (STTraderLogin*)m_msgQueue_Query->new_block(sizeof(STTraderLogin));

	strncpy(pBody->cust_no, m_UserInfo.UserID, sizeof(TCustNoType));
	strncpy(pBody->cust_pwd, m_UserInfo.Password, sizeof(TCustPwdType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, this, nullptr, 0, 0,
		pBody, sizeof(STTraderLogin), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int row_num = 0;
	STTraderLoginRsp *p_login_rsp = nullptr;

	bool bRet = SPX_API_Login(m_pApi, (STTraderLogin*)ptr1, &p_login_rsp, &row_num, &m_err_msg);

	if (bRet && m_err_msg.error_no == 0)
	{
		if (row_num <= 0)
		{
			printf("返回结果为空");
		}
		else if (p_login_rsp != nullptr)
		{
			for (int i = 0; i<row_num; i++)
			{
				//cout << p_login_rsp[i].cust_name << " " << p_login_rsp[i].market_code << " " << p_login_rsp[i].holder_acc_no << endl;

			}
		}
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = m_err_msg.error_no;
		strcpy(pField->ErrorMsg, m_err_msg.msg);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}

	return 0;
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

	pThis = this;
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
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

bool CTraderApi::IsErrorRspInfo(STRspMsg *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->error_no != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->ErrorID = pRspInfo->error_no;
		strcpy(pField->ErrorMsg, pRspInfo->msg);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, this, bIsLast, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CTraderApi::IsErrorRspInfo(STRspMsg *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->error_no != 0));

	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(E_Init, this, nullptr, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
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

	if(m_pApi)
	{
		SPX_API_DestroyHandle(&m_pApi, &m_err_msg);
		m_pApi = nullptr;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, this, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}
	SPX_API_Finalize(&m_err_msg);

	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}
}

void BuildOrder(OrderField* pIn, PSTOrder pOut)
{
	strncpy(pOut->sec_code, pIn->InstrumentID, sizeof(TSecCodeType));
	OrderField_2_TBSType(pIn, pOut);
	OrderField_2_TMarketOrderFlagType(pIn, pOut);
	pOut->price = pIn->Price;
	pOut->order_vol = pIn->Qty;
	//pOut->order_prop;
}

OrderIDType* CTraderApi::ReqOrderInsert(
	int OrderRef,
	OrderField* pOrder,
	int count)
{
	if (count < 1)
		return nullptr;

	// 如果是批量下单是不同市场，不同账号怎么办？是要自己分成两组吗？
	// 这个下单是阻塞模式，是否得先生成编号，否则无法正常映射
	
	int req_no = 0;

	int row_num = 0;
	STOrder *orderreq = new STOrder[count];
	STOrderRsp *p_order_rsp = NULL;

	memset(orderreq, 0, sizeof(STOrder)*count);

	for (int i = 0; i < count;++i)
	{
		BuildOrder(&pOrder[i], &orderreq[i]);
	}

	bool bRet = SPX_API_Order(m_pApi, "0050001236", "1", "A050001236", "0", orderreq, &p_order_rsp, count, &row_num, &m_err_msg);

	if (bRet && m_err_msg.error_no == 0)
	{
		if (p_order_rsp != NULL)
		{
			for (int i = 0; i<row_num; i++)
			{
				if (p_order_rsp[i].error_no == 0)
				{
					//cout << p_order_rsp[i].batch_no << " " << p_order_rsp[i].order_no << endl;
				}
				else
				{
					//cout << "Order Error:" << p_order_rsp[i].error_no << " " << p_order_rsp[i].err_msg << endl;
				}
			}
		}
		else
		{
			//cout << "返回结果为空" << endl;
		}
	}


	delete[] orderreq;

	return nullptr;
}



void CTraderApi::OnPST16203PushData(PST16203PushData pEtxPushData)
{
	OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d:%d:%s", pEtxPushData->batch_no, pOrder->SessionID, pOrder->OrderRef);
	//OrderIDType orderSydId = { 0 };

	//{
	//	// 保存原始订单信息，用于撤单

	//	unordered_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
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
	//	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	//	if (it == m_id_platform_order.end())
	//	{
	//		// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
	//		pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
	//		strcpy(pField->ID, orderId);
	//		strcpy(pField->InstrumentID, pOrder->InstrumentID);
	//		strcpy(pField->ExchangeID, pOrder->ExchangeID);
	//		pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pOrder->CombHedgeFlag[0]);
	//		pField->Side = TThostFtdcDirectionType_2_OrderSide(pOrder->Direction);
	//		pField->Price = pOrder->LimitPrice;
	//		pField->StopPx = pOrder->StopPrice;
	//		strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
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
	//		strncpy(pField->Text, pOrder->StatusMsg, sizeof(ErrorMsgType));
	//	}

	//	m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, this, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}
}

void CTraderApi::OnPST16204PushData(PST16204PushData pEtxPushData)
{

}