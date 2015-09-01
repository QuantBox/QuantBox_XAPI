#ifndef HLQUOTEAPI_HHH_
#define HLQUOTEAPI_HHH_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HLUserApiDefine.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_HLQUOTEAPI_EXPORT
#define HLQUOTEAPI_EXPORT __declspec(dllexport)
#else
#define HLQUOTEAPI_EXPORT __declspec(dllimport)
#endif
#else
#define HLQUOTEAPI_EXPORT
#endif


class HLQUOTEAPI_EXPORT HLQuoteSpi
{
public:
	//当客户端与交易后台建立起通信连接，客户端需要进行登录。
	virtual void OnFrontConnected() {};

	//当客户端与交易后台通信连接断开时，该方法被调用，客户端不需要处理，API会自动重连。
	virtual void OnFrontDisconnected(int nReason) {};

	//错误应答
	//@cIsLast 包标志 API_PACKET_FIRST | API_PACKET_LAST 下同
	virtual void OnRspError(CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

	//用户登录应答
	virtual void OnRspUserLogin(HLApiRspUserLoginData* pRspUserLogin, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

	//用户退出应答
	//virtual void OnRspUserLogout(void *pRspUserLogout, void *pRspInfo, int nRequestID, char cIsLast) {};

	//市场代码表请求应答
	virtual void OnRspCodeTable(HLApiRspCodeTable* pRspCodeTable, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

	//行情订阅请求应答
	virtual void OnRspMarketData(HLApiRspMarketData* pRspMarketData, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

	//行情变化通知
	virtual void OnRtnMarketData(HLApiMarketData* pMarketData) {};

	//全市场行情变化通知(订阅股票代码为空时，为全市场行情推送)
	virtual void OnRtnMarketData(HLApiMarketData* pMarketData, int nSize) {};

	//TCP实时推送的行情
	virtual void OnRtnPushStkData(HLApiMarketData* pMarketData, int nSize = 1) {};

	//组播实时推送的行情
	virtual void OnRtnMarketData_MC(HLApiMarketData* pMarketData, int nSize = 1) {};

protected:
	virtual ~HLQuoteSpi(void) = 0 {};
};

class HLQUOTEAPI_EXPORT HLQuoteApi
{
public:
	//创建一个HLQuoteApi
	static HLQuoteApi* CreateHLQuoteApi(void);

	//获取行情API版本号
	static const char* GetVersion(void);

	//注册行情服务器地址
	virtual void RegisterNetwork(unsigned short nPort, const char* pszFrontAddr, const char* pszMCGroup="") = 0;

	//注册行情回调接口
	virtual void RegisterSpi(HLQuoteSpi* pSpi) = 0;

	//行情API初始化
	virtual int Init(void) = 0;

	//用户登录请求
	virtual int ReqUserLogin(HLApiReqUserLoginData* pReqUserLogin, int nRequestID) = 0;

	//市场代码表请求
	virtual int ReqCodeTable(HLApiReqCodeTable* pReqCodeTable, int nRequestID) = 0;

	//行情订阅请求
	virtual int ReqMarketData(HLApiReqMarketData* pReqMarketData, int nRequestID) = 0;

	//行情API反初始化
	virtual void Release(void) = 0;

protected:
	virtual ~HLQuoteApi(void);
};

#endif /*HLQUOTEAPI_HHH_*/

