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
    ///当客户端与交易后台建立起通信连接，客户端需要进行登录。
    virtual void OnFrontConnected() {};

    ///当客户端与交易后台通信连接断开时，该方法被调用，客户端不需要处理，API会自动重连。
    virtual void OnFrontDisconnected(int nReason) {};

    ///错误应答
    //@cIsLast 包标志 API_PACKET_FIRST | API_PACKET_LAST 下同
    virtual void OnRspError(CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast) {};

	///用户登录应答
	virtual void OnRspUserLogin(HLApiRspUserLoginData *pRspUserLogin, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast) {};

    ///用户退出应答
    //virtual void OnRspUserLogout(void *pRspUserLogout, void *pRspInfo, int nRequestID, char cIsLast) {};

    ///市场索引请求应答
    virtual void OnRspCodeTable (HLApiRspCodeTable *pRspCodeTable, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast) {};

    ///行情订阅请求应答
    virtual void OnRspMarketData (HLApiRspMarketData *pRspMarketData, CHLRspInfoData *pRspInfo, int nRequestID, char cIsLast) {};

    //订阅行情变化通知
    virtual void OnRtnMarketData (HLApiMarketData *pMarketData) {};

    //全市场行情变化通知(订阅股票代码为空时，为全市场行情推送)
    virtual void OnRtnMarketData (HLApiMarketData *pMarketData, int nSize) {};
	virtual void OnRtnPushStkData(HLApiMarketData *pMarketData, int nSize = 1){};

protected:
    virtual ~HLQuoteSpi (void) = 0 {};
};

class HLQUOTEAPI_EXPORT HLQuoteApi
{
public:
    //创建一个HLQuoteApi
    //可以创建多个API
    //一个API最多只有一个网络通道
    static HLQuoteApi* CreateHLQuoteApi (void);

    ///获取系统版本号
    ///@return 系统版本号标识字符串
    static const char *GetVersion (void);

    virtual void RegisterNetwork (unsigned short nPort, const char *pszFrontAddr) = 0;

    virtual void RegisterSpi (HLQuoteSpi *pSpi) = 0;

    virtual int Init (void) = 0;

    virtual void Release (void) = 0;

	///用户登录请求
	virtual int ReqUserLogin (HLApiReqUserLoginData *pReqUserLogin, int nRequestID) = 0;

	///市场索引请求
	virtual int ReqCodeTable (HLApiReqCodeTable *pReqCodeTable, int nRequestID) = 0;

    ///行情订阅请求
	virtual int ReqMarketData (HLApiReqMarketData *pReqMarketData, int nRequestID) = 0;

protected:
    virtual ~HLQuoteApi (void);
};

#endif /*HLQUOTEAPI_HHH_*/

