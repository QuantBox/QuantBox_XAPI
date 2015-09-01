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
	/// 总持仓
	QtyType Position;
	/// 今日持仓
	QtyType TdPosition;
	/// 昨日持仓
	QtyType YdPosition;
	/// 可用持仓
	QtyType AvbPosition;
	/// 可用昨日持仓
	QtyType AvbYdPosition;
	/// 可用申赎数
	QtyType AvbETFPosition;

	InstrumentIDType InstrumentName;
	//QtyType 
	/// 成本价
	PriceType HoldPrice;
	/// 市值
	PriceType MktValue;

	HedgeFlagType HedgeFlag;

	int StockAskFrn;                       //卖出冻结
	int StockBidFrn;                       //买入冻结
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

	///询价编号
	OrderIDType	QuoteReqID;

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

struct EtfSubscribeField
{
	///用户代码
	UserIDType	UserID;
	//用户资金账号
	UserAccountNoType UserAccountNo;
	InstrumentIDType InstrumentID;
	QtyType Qty;
	OrderSide Side;
	OrderIDType OrderID;
};

struct OrderField
{
	InstrumentNameType InstrumentName;
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
	int DateTime;


	TimeType Time;
	long OrderRef;
	ShareholderIDType ShareHolderID;
};

struct OrderCancelField
{

	///用户代码
	UserIDType	UserID;
	//用户资金账号
	UserAccountNoType UserAccountNo;
	//撤销合同号
	OrderIDType OrderID;
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
	int MatchedDate;                       //成交或撤单日期
	TimeType Time;
	OrderIDType ID;
	TradeIDType TradeID;

	InstrumentNameType InstrumentName;
	ShareholderIDType ShareHolderID;
	OrderStatus Status;
	QtyType LeavesQty;
	OrderStatus OdStatus;
	OrderIDType RefID;

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
	ExtendInformationType	ExtendInformation;
};

struct ConfigInfoField
{

};

// 用户信息
struct UserInfoField
{
	///用户代码
	UserIDType	UserID;
	//用户资金账号
	UserAccountNoType UserAccountNo;
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
	// 交易所编号
	ExchangeIDType	ExchangeID;
	// 股东编号
	UserIDType		SecurityID;
	// 请求索引
	ReferIDType		ReferID;

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

	///合约类型
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

//ETF清单请求响应
struct ETFListField
{
	//ETF代码
	InstrumentIDType	ETFInstrumentID;
	//ETF名称
	InstrumentNameType ETFInstrumentName;
	///交易所代码
	ExchangeIDType	ExchangeID; //一级交易板块
	double dMaxCashRatio;                   //现金替代比例上限
	int nCreationRedemption;                //基金当天申购赎回状态(1,0)
	int nCreationRedemptionUnit;            //最小申购赎回单位
	PriceType dEstimateCashComponent;          //T日预估现金余额
	int nTradingDay;                        //T日日期
	InstrumentIDType	InstrumentID;
	PriceType dCashComponent;                  //T-1日现金差额(单位：元)
	double dNAVperCU;                       //T-1日最小申购、赎回单位资产净值(单位：元)
	int nItemDataSize;                      //成份股数量
	int  size;

};

struct ETFListDataField
{
	InstrumentIDType	InstrumentID;  //股票代码
	InstrumentNameType InstrumentName;//股票名称
	ExchangeIDType	ExchangeID;        //交易所代码
	int nNum;                                   //股票数量
	int nFlag;                                  //现金替代标志
	double dRatio;                              //溢价比例
	double dReplaceAmount;                      //替代金额
};


//ETF申赎请求
struct ETFSubscribeInfo
{
	int eSubscribeFlag;                    //申赎标志
	InstrumentIDType	InstrumentID;                 //ETF代码
	int nSubscribeQty;                                  //申赎份额
	OrderIDType OrderID;                    //合同序号
};

//分级基金
struct RspStruFundInfoQry
{
	InstrumentIDType	FundInstrumentID;
	InstrumentNameType FundInstrumentName;
	InstrumentIDType	FundAInstrumentID;
	InstrumentNameType FundAInstrumentName;
	InstrumentIDType	FundBInstrumentID;
	InstrumentNameType FundBInstrumentName;
	int  nFundAShare;						//分级A比例(0,100), 默认:50
	int	 nFundBShare;						//分级B比例(0,100), 默认:50
};
///深度行情N档
struct DepthMarketDataNField
{
	///占用总字节大小
	SizeType			Size;
	///交易所时间
	DateIntType			TradingDay; // 交易日，用于给数据接收器划分到同一文件使用，基本没啥别的用处
	DateIntType			ActionDay;
	TimeIntType			UpdateTime;
	TimeIntType			UpdateMillisec;

	///交易所代码
	ExchangeType	Exchange;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;

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
	///买档个数
	SizeType BidCount;
	char status;
};

struct DepthField
{
	PriceType	Price;
	VolumeType	Size;
	VolumeType	Count;
};
#endif
