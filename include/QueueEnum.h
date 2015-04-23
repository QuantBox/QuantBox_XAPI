#ifndef _QUEUE_ENUM_H_
#define _QUEUE_ENUM_H_

enum RequestType :char
{
	GetApiType = 0,
	GetApiVersion,
	GetApiName,

	Create, // 创建
	Release, // 销毁
	Register, // 注册接收队列回调
	Config,		// 配置参数

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
	

	ReqQryHistoricalTicks,
	ReqQryHistoricalBars,
	ReqQryInvestor,
};

enum ResponeType :char
{
	OnConnectionStatus = 64,
	OnRtnDepthMarketData,
	OnRtnError,
	OnRspQryInstrument,
	OnRspQryTradingAccount,
	OnRspQryInvestorPosition,
	OnRspQrySettlementInfo,
	OnRtnOrder,
	OnRtnTrade,

	OnRtnQuote,
	OnRtnQuoteRequest,

	OnRspQryHistoricalTicks,
	OnRspQryHistoricalBars,
	OnRspQryInvestor,

	OnFilterSubscribe,
};

#endif
