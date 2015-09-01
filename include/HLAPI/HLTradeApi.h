#ifndef HLTRADEAPI_HHH_
#define HLTRADEAPI_HHH_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HLUserApiDefine.h"


#if defined(ISLIB) && defined(WIN32)
    #ifdef LIB_HLTRADEAPI_EXPORT
        #define HLTRADEAPI_EXPORT __declspec(dllexport)
    #else
        #define HLTRADEAPI_EXPORT __declspec(dllimport)
    #endif
#else
    #define HLTRADEAPI_EXPORT 
#endif

class HLTRADEAPI_EXPORT HLTradeSpi
{
public:
    ///当客户端与交易后台建立起通信连接，客户端需要进行登录。
    virtual void OnFrontConnected() {};

    ///当客户端与交易后台通信连接断开时，该方法被调用，客户端不需要处理，API会自动重连。
    virtual void OnFrontDisconnected(int nReason) {};

    ///错误应答
    //@cIsLast 包标志 API_PACKET_FIRST | API_PACKET_LAST 下同
    virtual void OnRspError(CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

	///用户登录_应答
	virtual void OnRspUserLogin(HLApiRspUserLoginData* pRspUserLogin, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///用户退出_应答
    //virtual void OnRspUserLogout(void *pRspUserLogout, void *pRspInfo, int nRequestID, char cIsLast) {};

    ///用户委托请求_应答
    virtual void OnRspOrder(HLApiRspOrder* pRspOrder, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///用户撤单请求_应答
    virtual void OnRspCancelOrder(HLApiRspCancelOrder* pRspCancelOrder, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///ETF清单请求_应答
    virtual void OnRspETFList(HLApiRspETFList* pRspETFList, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///ETF申赎请求_应答
    virtual void OnRspETFSubscribe(HLApiRspETFSubscribe* pRspETFSubscribe, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///查委托请求_应答
    virtual void OnRspOrderQry(HLApiRspOrderQry* pRspQryOrder, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///查当日成交请求_应答
    virtual void OnRspTodayDealQry(HLApiRspTodayDealQry* pRspQryTodayDeal, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///查资金请求_应答
    virtual void OnRspMoneyInfoQry(HLApiRspMoneyInfoQry* pRspQryMoneyInfo, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///可用证券查询请求_应答
    virtual void OnRspAvlStockInfoQry(HLApiRspAvlStocksInfoQry* pReqQryAvlStockInfo, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};

    ///委托、撤单、撤单失败通知
    virtual void OnRtnOrderInfo(HLApiRtnOrderInfo* pRtnOrderInfo, CHLRspInfoData* pRspInfo, int nRequestID) {};

    ///成交通知
    virtual void OnRtnMatchInfo(HLApiRtnMatchInfo* pRtnMatchInfo, CHLRspInfoData* pRspInfo, int nRequestID) {};

    ///可用证券变化通知
    virtual void OnRtnAvlStockInfo(HLApiRtnAvlStockInfo* pRtnAvlStockInfo, CHLRspInfoData* pRspInfo, int nRequestID) {};

    ///资金变化通知
    virtual void OnRtnMoneyInfo(HLApiRtnMoneyInfo* pRtnMoneyInfo, CHLRspInfoData* pRspInfo, int nRequestID) {};

	///分级基金成分信息查询_应答
	virtual void OnRspStruFundInfoQry(HLApiRspStruFundInfoQry* pRspQryStruFundInfo, CHLRspInfoData* pRspInfo, int nRequestID, char cIsLast) {};


protected:
    virtual ~HLTradeSpi(void) {};
};


class HLTRADEAPI_EXPORT HLTradeApi
{
public:
    ///创建一个HLTradeApi，可以创建多个，一个API最多只有一个网络通道
    static HLTradeApi* CreateHLTradeApi(void);

    ///获取系统版本号
    ///@return 系统版本号标识字符串
    static const char* GetVersion(void);

    virtual void RegisterNetwork(unsigned short nPort, const char* pszFrontAddr) = 0;

    virtual void RegisterSpi(HLTradeSpi* pHLTradeSpi) = 0;

    virtual int Init(void) = 0;

    virtual void Release (void) = 0;	

	///用户登录请求
	virtual int ReqUserLogin(HLApiReqUserLoginData* pReqUserLogin, int nRequestID) = 0;

    ///委托请求
    virtual int ReqOrder(HLApiCmdOrder* pReqOrder, int nRequestID) = 0;

    ///撤单请求
    virtual int ReqCancelOrder(HLApiCmdCancelOrder* pReqOrder, int nRequestID) = 0;

    ///批量委托请求
    virtual int ReqBatchOrder(HLApiCmdBatchOrder* pReqBatchOrder, int nRequestID) = 0;

    ///ETF清单请求
    virtual int ReqETFList(HLApiReqETFList* pReqETFList, int nRequestID) = 0;

    ///ETF申赎请求
    virtual int ReqETFSubscribe(HLApiCmdETFSubscribe* pReqETFSubscribe, int nRequestID) = 0;

    ///查委托请求
    virtual int ReqQryOrder(HLApiCmdOrderQry* pReqQryOrder, int nRequestID) = 0;

    ///资金查询请求
    virtual int ReqQryMoneyInfo(HLApiCmdMoneyInfoQry* pReqQryAvailableMoneyInfo, int nRequestID) = 0;

    ///可用证券查询请求
    virtual int ReqQryAvlStockInfo(HLApiCmdAvlStocksInfoQry* pReqQryAvailableStockInfo, int nRequestID) = 0;

    ///当日成交查询请求
    virtual int ReqQryTodayDeal(HLApiCmdTodayDealQry* pReqQryTodayDeal, int nRequestID) = 0;

	///分级基金成分信息查询
	virtual int ReqQryStruFundInfo(const HLApiCmdStruFundInfoQry* pReqQryStruFundInfo, int nRequestID) = 0;

	///修改委托状态请求
	virtual int ReqChangeOrderStatus (HLApiRspOrder *pReqUserLogin, int nRequestID) = 0;

protected:
    virtual ~HLTradeApi (void);

};


#endif /*HLTRADEAPI_HHH_*/


