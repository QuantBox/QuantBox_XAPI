#ifndef _API_STRUCT_H_
#define _API_STRUCT_H_

#include "ApiDataType.h"
#include "ApiEnum.h"


struct PositionField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;

	PositionSide	Side;
	QtyType Position;
	QtyType TdPosition;
	QtyType YdPosition;
	HedgeFlagType HedgeFlag;
	//TThostFtdcPositionDateType 还没处理
};

struct QuoteField
{
	InstrumentIDType InstrumentID;
	ExchangeIDType	ExchangeID;

	QtyType AskQty;
	PriceType AskPrice;
	OpenCloseType AskOpenClose;
	HedgeFlagType AskHedgeFlag;

	QtyType BidQty;
	PriceType BidPrice;
	OpenCloseType BidOpenClose;
	HedgeFlagType BidHedgeFlag;

	OrderIDType ID;
	OrderIDType AskID;
	OrderIDType BidID;
	OrderIDType AskOrderID;
	OrderIDType BidOrderID;
	OrderStatus Status;
	ExecType ExecType;
	ErrorIDType ErrorID;
	ErrorMsgType Text;

	/*
	QtyType LeavesQty;
	QtyType CumQty;
	PriceType AvgPx;
	

	
	
	long DateTime;*/
};

struct OrderField
{
	InstrumentIDType InstrumentID;
	ExchangeIDType	ExchangeID;
	OrderType Type;
	OrderSide Side;
	QtyType Qty;
	PriceType Price;
	OpenCloseType OpenClose;
	HedgeFlagType HedgeFlag;

	PriceType StopPx;
	TimeInForce TimeInForce;

	OrderStatus Status;
	ExecType ExecType;
	QtyType LeavesQty;
	QtyType CumQty;
	PriceType AvgPx;
	ErrorIDType ErrorID;
	ErrorMsgType Text;

	OrderIDType ID;
	OrderIDType OrderID;
	long DateTime;
};

struct TradeField
{
	InstrumentIDType InstrumentID;
	ExchangeIDType	ExchangeID;

	OrderSide Side;
	QtyType Qty;
	PriceType Price;
	OpenCloseType OpenClose;
	HedgeFlagType HedgeFlag;
	MoneyType Commission;
	TimeType Time;
	OrderIDType ID;
	TradeIDType TradeID;
};

struct ServerInfoField
{
	bool IsUsingUdp;
	bool IsMulticast;
	int	TopicId;
	int Port;
	ResumeType	MarketDataTopicResumeType;
	ResumeType	PrivateTopicResumeType;
	ResumeType	PublicTopicResumeType;
	ResumeType	UserTopicResumeType;
	BrokerIDType	BrokerID;
	ProductInfoType	UserProductInfo;
	AuthCodeType	AuthCode;
	AddressType	Address;
};

struct ConfigInfoField
{

};

// 用户信息
struct UserInfoField
{
	///用户代码
	UserIDType	UserID;
	///密码
	PasswordType	Password;
};


// 错误信息
struct ErrorField
{
	// 错误代码
	ErrorIDType	ErrorID;
	// 错误信息
	ErrorMsgType	ErrorMsg;
};


// 登录回报
struct RspUserLoginField
{
	///交易日
	DateType	TradingDay;
	// 时间
	TimeType	LoginTime;
	// 会话ID
	SessionIDType	SessionID;
	// 错误代码
	ErrorIDType	ErrorID;
	// 错误信息
	ErrorMsgType	ErrorMsg;
};



///深度行情
struct DepthMarketDataField
{
	///交易所时间
	DateIntType			TradingDay; // 交易日，用于给数据接收器划分到同一文件使用，基本没啥别的用处
	DateIntType			ActionDay;
	TimeIntType			UpdateTime;
	TimeIntType			UpdateMillisec;

	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;

	///最新价
	PriceType	LastPrice;
	///数量
	LargeVolumeType	Volume;
	///成交金额
	MoneyType	Turnover;
	///持仓量
	LargeVolumeType	OpenInterest;
	///当日均价
	PriceType	AveragePrice;


	///今开盘
	PriceType	OpenPrice;
	///最高价
	PriceType	HighestPrice;
	///最低价
	PriceType	LowestPrice;
	///今收盘
	PriceType	ClosePrice;
	///本次结算价
	PriceType	SettlementPrice;

	///涨停板价
	PriceType	UpperLimitPrice;
	///跌停板价
	PriceType	LowerLimitPrice;
	///昨收盘
	PriceType	PreClosePrice;
	///上次结算价
	PriceType	PreSettlementPrice;
	///昨持仓量
	LargeVolumeType	PreOpenInterest;


	///申买价一
	PriceType	BidPrice1;
	///申买量一
	VolumeType	BidVolume1;
	///申卖价一
	PriceType	AskPrice1;
	///申卖量一
	VolumeType	AskVolume1;
	///申买价二
	PriceType	BidPrice2;
	///申买量二
	VolumeType	BidVolume2;
	///申卖价二
	PriceType	AskPrice2;
	///申卖量二
	VolumeType	AskVolume2;
	///申买价三
	PriceType	BidPrice3;
	///申买量三
	VolumeType	BidVolume3;
	///申卖价三
	PriceType	AskPrice3;
	///申卖量三
	VolumeType	AskVolume3;
	///申买价四
	PriceType	BidPrice4;
	///申买量四
	VolumeType	BidVolume4;
	///申卖价四
	PriceType	AskPrice4;
	///申卖量四
	VolumeType	AskVolume4;
	///申买价五
	PriceType	BidPrice5;
	///申买量五
	VolumeType	BidVolume5;
	///申卖价五
	PriceType	AskPrice5;
	///申卖量五
	VolumeType	AskVolume5;
};

///Tick行情
struct TickField
{
	///交易所时间
	DateIntType			Date;
	TimeIntType			Time;
	TimeIntType			Millisecond;

	PriceType	LastPrice;
	///数量
	LargeVolumeType	Volume;
	///持仓量
	LargeVolumeType	OpenInterest;
	PriceType	BidPrice1;
	PriceType	AskPrice1;
	VolumeType	BidSize1;
	VolumeType	AskSize1;
};


///Bar行情
struct BarField
{
	///交易所时间
	DateIntType			Date;
	TimeIntType			Time;

	///开
	PriceType	Open;
	///高
	PriceType	High;
	///低
	PriceType	Low;
	///收
	PriceType	Close;
	///数量
	LargeVolumeType	Volume;
	///持仓量
	LargeVolumeType	OpenInterest;
	///成交金额
	MoneyType	Turnover;
};

///发给做市商的询价请求
struct QuoteRequestField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;
	///交易日
	DateType	TradingDay;
	///询价编号
	OrderIDType	QuoteID;
	///询价时间
	TimeType	QuoteTime;
};

///合约
struct InstrumentField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;
	///合约名称
	InstrumentNameType InstrumentName;

	///合约名称
	InstrumentType		Type;
	///合约数量乘数
	VolumeMultipleType	VolumeMultiple;
	///最小变动价位
	PriceType	PriceTick;
	///到期日
	DateType	ExpireDate;
	///基础商品代码
	InstrumentIDType	UnderlyingInstrID;
	///执行价
	PriceType	StrikePrice;
	///期权类型
	PutCall	OptionsType;
};

///账号
struct AccountField
{
	///上次结算准备金
	MoneyType	PreBalance;
	///当前保证金总额
	MoneyType	CurrMargin;
	///平仓盈亏
	MoneyType	CloseProfit;
	///持仓盈亏
	MoneyType	PositionProfit;
	///期货结算准备金
	MoneyType	Balance;
	///可用资金
	MoneyType	Available;

	///入金金额
	MoneyType	Deposit;
	///出金金额
	MoneyType	Withdraw;

	///冻结的过户费
	MoneyType	FrozenTransferFee;
	///冻结的印花税
	MoneyType	FrozenStampTax;
	///冻结的手续费
	MoneyType	FrozenCommission;
	///冻结的资金
	MoneyType	FrozenCash;

	///过户费
	MoneyType	TransferFee;
	///印花税
	MoneyType	StampTax;
	///手续费
	MoneyType	Commission;
	///资金差额
	MoneyType	CashIn;

};

///账号
struct SettlementInfoField
{
	///交易日
	DateType	TradingDay;
	///消息正文
	ContentType	Content;
};

struct HistoricalDataRequestField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;

	int Date1;
	int Date2;
	int Time1;
	int Time2;

	DataObjetType DataType;
	BarType BarType;
	long BarSize;

	int RequestId;
	int CurrentDate;
	int lRequest;
};
#endif
