#ifndef _QUEUE_ENUM_H_
#define _QUEUE_ENUM_H_

enum RequestType :char
{
	Create = 0, // 创建
	Release, // 销毁
	Register, // 注册回调
	

	Connect, // 开始/连接
	Disconnect, // 停止/断开

	Clear, // 清理
	Process, // 处理

	Subscribe,	// 订阅
	Unsubscribe, // 取消订阅

	SubscribeQuote, // 订阅询价
	UnsubscribeQuote, // 取消订阅询价

	ReqOrderInsert,
	ReqQuoteInsert,
	ReqOrderAction,
	ReqQuoteAction,

	ReqQryOrder,
	ReqQryTrade,

	ReqQryInstrument,
	ReqQryTradingAccount,
	ReqQryInvestorPosition,
	ReqQryInvestorPositionDetail,
	ReqQryInstrumentCommissionRate,
	ReqQryInstrumentMarginRate,
	ReqQrySettlementInfo,
};

enum ResponeType :char
{
	OnConnectionStatus = 64,
	OnRtnDepthMarketData,
	OnRtnError,
	OnRspQryInstrument,
	OnRspQryTradingAccount,
	OnRspQrySettlementInfo,
	OnRtnOrder,
	OnRtnTrade,
	OnRtnQuoteRequest,
};

#endif
