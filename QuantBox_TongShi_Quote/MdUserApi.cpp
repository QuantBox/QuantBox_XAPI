#include "stdafx.h"
#include "MdUserApi.h"
#include "../include/QueueEnum.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include "../include/XApiC.h"

#include "../include/ChinaStock.h"
#include "TypeConvert.h"

#include <string.h>
#include <cfloat>
#include <tchar.h>

#include <mutex>
#include <vector>

using namespace std;


#define WM_USER_STOCK	2000

CMdUserApi* CMdUserApi::pThis = nullptr;

LRESULT CALLBACK CMdUserApi::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER_STOCK)
	{
		return pThis->_OnMsg(wParam, lParam);
	}
	else if (message == WM_NCDESTROY)
	{
		WriteLog("TS:5555,%d,%d", message, wParam);
	}
	WriteLog("TS:%d,%d", message, wParam);
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CMdUserApi::_OnMsg(WPARAM wParam, LPARAM lParam)
{
	RCV_DATA* pHeader;
	int i;

	pHeader = (RCV_DATA*)lParam;

	//	对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份, 再作处理
	switch (wParam)
	{
	case RCV_REPORT:						// 股票行情
		for (i = 0; i < pHeader->m_nPacketNum; i++)
		{
			// 数据处理
			OnRtnDepthMarketData((RCV_REPORT_STRUCTEx*)&(pHeader->m_pReport[i]), i, pHeader->m_nPacketNum);
		}
		break;

	case RCV_FILEDATA:						// 文件
		switch (pHeader->m_wDataType)
		{
		case FILE_HISTORY_EX:				// 补日线数据
			break;

		case FILE_MINUTE_EX:				// 补分钟线数据
			break;
		default:
			return 0;
		}
		break;

	default:
		return 0;							// unknown data
	}
	return 1;
}

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
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register((void*)Query, this);
	m_msgQueue_Query->StartThread();

	//m_msgQueue->m_bDirectOutput = true;

	m_bRunning = false;
	m_hThread = nullptr;
	m_hWnd = nullptr;
	m_hModule = nullptr;
	m_pfnStock_Init = nullptr;
	m_pfnStock_Quit = nullptr;

	pThis = this;
}

CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	//case E_ReqUserLoginField:
	//	iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	//	break;
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

void CMdUserApi::Register(void* pCallback,void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register((void*)Query,this);
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

ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
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
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));
	
	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_Init()
{
	StartThread();

	return 0;
}

void CMdUserApi::Disconnect()
{
	StopThread();

	// 清理查询队列
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr,nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	//if(m_pApi)
	//{
	//	m_pApi->RegisterSpi(NULL);
	//	m_pApi->Release();
	//	m_pApi = NULL;

		// 全清理，只留最后一个
		// 由于这个dll中设计的线程在连接失败时直接退出，所以这个地方要加一个判断，防出错
		if (m_msgQueue)
		{
			m_msgQueue->Clear();
			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			// 主动触发
			m_msgQueue->Process();
		}
	//}

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

void CMdUserApi::OnRspQryInstrument(DepthMarketDataField* _pField,RCV_REPORT_STRUCTEx *pDepthMarketData, int index, int Count)
{
	InstrumentField* pField = (InstrumentField*)m_msgQueue->new_block(sizeof(InstrumentField));

	strcpy(pField->InstrumentID, _pField->InstrumentID);
	strcpy(pField->ExchangeID, _pField->ExchangeID);
	strcpy(pField->Symbol, _pField->Symbol);

	strncpy(pField->InstrumentName, pDepthMarketData->m_szName, sizeof(InstrumentNameType));
	pField->VolumeMultiple = 1;

	int instrumentInt = atoi(pField->InstrumentID);

	switch (pDepthMarketData->m_wMarket)
	{
	case SH_MARKET_EX:
		pField->Type = InstrumentID_2_InstrumentType_SSE(instrumentInt);
		pField->PriceTick = InstrumentID_2_PriceTick_SSE(instrumentInt);
		break;
	case SZ_MARKET_EX:
		pField->Type = InstrumentID_2_InstrumentType_SZE(instrumentInt);
		pField->PriceTick = InstrumentID_2_PriceTick_SZE(instrumentInt);
		break;
	case SB_MARKET_EX:
		pField->Type = InstrumentID_2_InstrumentType_NEEQ(instrumentInt);
		pField->PriceTick = InstrumentID_2_PriceTick_NEEQ(instrumentInt);
		break;
	default:
		break;
	}

	m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInstrument, m_msgQueue, m_pClass, index >= Count - 1, 0, pField, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnRtnDepthMarketData(RCV_REPORT_STRUCTEx *pDepthMarketData, int index, int Count)
{
	DepthMarketDataField* pField = (DepthMarketDataField*)m_msgQueue->new_block(sizeof(DepthMarketDataField));

	strcpy(pField->InstrumentID, OldSymbol_2_NewSymbol(pDepthMarketData->m_szLabel, pDepthMarketData->m_wMarket));
	strcpy(pField->ExchangeID, Market_2_Exchange(pDepthMarketData->m_wMarket));

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

	// 为合约导入功能所加，做行情处理时还是注释了比较好
	set<string>::iterator it = m_setInstrumentIDsReceived.find(pField->Symbol);
	if (it == m_setInstrumentIDsReceived.end())
	{
		OnRspQryInstrument(pField, pDepthMarketData, index, Count);
		m_setInstrumentIDsReceived.insert(pField->Symbol);
	}

	GetExchangeTime(pDepthMarketData->m_time, &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime);
	
	pField->LastPrice = my_round(pDepthMarketData->m_fNewPrice);
	pField->Volume = pDepthMarketData->m_fVolume;
	pField->Turnover = pDepthMarketData->m_fAmount;

	pField->OpenPrice = my_round(pDepthMarketData->m_fOpen);
	pField->HighestPrice = my_round(pDepthMarketData->m_fHigh);
	pField->LowestPrice = my_round(pDepthMarketData->m_fLow);

	pField->PreClosePrice = my_round(pDepthMarketData->m_fLastClose);

	int nLots = 1;

	do
	{
		if (pDepthMarketData->m_fBuyVolume[0] == 0)
			break;
		pField->BidPrice1 = my_round(pDepthMarketData->m_fBuyPrice[0]);
		pField->BidVolume1 = pDepthMarketData->m_fBuyVolume[0] * nLots;

		if (pDepthMarketData->m_fBuyVolume[1] == 0)
			break;
		pField->BidPrice2 = my_round(pDepthMarketData->m_fBuyPrice[1]);
		pField->BidVolume2 = pDepthMarketData->m_fBuyVolume[1] * nLots;

		if (pDepthMarketData->m_fBuyVolume[2] == 0)
			break;
		pField->BidPrice3 = my_round(pDepthMarketData->m_fBuyPrice[2]);
		pField->BidVolume3 = pDepthMarketData->m_fBuyVolume[2] * nLots;

		if (pDepthMarketData->m_fBuyVolume4 == 0)
			break;
		pField->BidPrice4 = my_round(pDepthMarketData->m_fBuyPrice4);
		pField->BidVolume4 = pDepthMarketData->m_fBuyVolume4 * nLots;

		if (pDepthMarketData->m_fBuyVolume5 == 0)
			break;
		pField->BidPrice5 = my_round(pDepthMarketData->m_fBuyPrice5);
		pField->BidVolume5 = pDepthMarketData->m_fBuyVolume5 * nLots;
	} while (false);

	do
	{
		if (pDepthMarketData->m_fSellVolume[0] == 0)
			break;
		pField->AskPrice1 = my_round(pDepthMarketData->m_fSellPrice[0]);
		pField->AskVolume1 = pDepthMarketData->m_fSellVolume[0] * nLots;

		if (pDepthMarketData->m_fSellVolume[1] == 0)
			break;
		pField->AskPrice2 = my_round(pDepthMarketData->m_fSellPrice[1]);
		pField->AskVolume2 = pDepthMarketData->m_fSellVolume[1] * nLots;

		if (pDepthMarketData->m_fSellVolume[2] == 0)
			break;
		pField->AskPrice3 = my_round(pDepthMarketData->m_fSellPrice[2]);
		pField->AskVolume3 = pDepthMarketData->m_fSellVolume[2] * nLots;

		if (pDepthMarketData->m_fSellVolume4 == 0)
			break;
		pField->AskPrice4 = my_round(pDepthMarketData->m_fSellPrice4);
		pField->AskVolume4 = pDepthMarketData->m_fSellVolume4 * nLots;

		if (pDepthMarketData->m_fSellVolume5 == 0)
			break;
		pField->AskPrice5 = my_round(pDepthMarketData->m_fSellPrice5);
		pField->AskVolume5 = pDepthMarketData->m_fSellVolume5 * nLots;
	} while (false);

	m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, sizeof(DepthMarketDataField), nullptr, 0, nullptr, 0);
	//m_msgQueue->delete_block(pField);
}

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
	m_setInstrumentIDsReceived.clear();

	m_hWnd = CreateWindowA(
		"static",
		"MsgRecv",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL);

	m_hModule = X_LoadLib(m_ServerInfo.Address);
	if (m_hModule == nullptr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = GetLastError();
		strncpy(pField->ErrorMsg, X_GetLastError(), sizeof(ErrorMsgType));

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return;
	}

	m_pfnStock_Init = (Stock_Init_PROC)X_GetFunction(m_hModule, "Stock_Init");
	m_pfnStock_Quit = (Stock_Quit_PROC)X_GetFunction(m_hModule, "Stock_Quit");
	if (m_pfnStock_Init == nullptr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->ErrorID = GetLastError();
		strncpy(pField->ErrorMsg, X_GetLastError(), sizeof(ErrorMsgType));

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return;
	}

	m_pfnChangeWindowMessageFilter =
		(ChangeWindowMessageFilter_PROC)::GetProcAddress(::GetModuleHandle(_T("USER32")), "ChangeWindowMessageFilter");
	if (m_pfnChangeWindowMessageFilter)
	{
		m_pfnChangeWindowMessageFilter(WM_USER_STOCK, MSGFLT_ADD);
	}

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_hWnd != NULL && IsWindow(m_hWnd))
	{
		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)WndProc);
	}
	m_pfnStock_Init(m_hWnd, WM_USER_STOCK, RCV_WORK_SENDMSG);

	MSG msg;
	while (m_bRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			Sleep(1);
		}
	}
	m_pfnStock_Quit(m_hWnd);
	DestroyWindow(m_hWnd);

	X_FreeLib(m_hModule);
	m_hModule = nullptr;
	m_hWnd = nullptr;

	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}