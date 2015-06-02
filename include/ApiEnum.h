#ifndef _API_ENUM_H_
#define _API_ENUM_H_

//连接状态枚举
enum ConnectionStatus :char
{
	Uninitialized, //未初始化
	Initialized, //已经初始化
	Disconnected, //连接已经断开
	Connecting, //连接中
	Connected, //连接成功
	Authorizing, //授权中
	Authorized, //授权成功
	Logining, //登录中
	Logined, //登录成功
	Confirming, //结算单确认中
	Confirmed, //已经确认
	Doing,
	Done, //完成
	Unknown_, //未知
};

enum ApiType :char
{
	Nono = 0,
	Trade = 1,
	MarketData = 2,
	Level2 = 4,
	QuoteRequest = 8,
	HistoricalData = 16,
	Instrument = 32,
};

enum DepthLevelType:char
{
	L0,
	L1,
	L5,
	L10,
	FULL,
};

enum ResumeType :char
{
	Restart,
	Resume,
	Quick,
	Undefined,
};

enum PutCall :char
{
	Put,
	Call,
};

enum OrderStatus :char
{
	NotSent,
	PendingNew,
	New,
	Rejected,
	PartiallyFilled,
	Filled,
	PendingCancel,
	Cancelled,
	Expired,
	PendingReplace,
	Replaced,
};

enum OrderSide :char
{
	Buy,
	Sell,
};

enum OrderType :char
{
	Market,
	Stop,
	Limit,
	StopLimit,
	MarketOnClose,
	Pegged,
	TrailingStop,
	TrailingStopLimit,
};

enum TimeInForce :char
{
	ATC,
	Day,
	GTC,
	IOC,
	OPG,
	OC,
	FOK,
	GTX,
	GTD,
	GFS,
	AUC,
};


enum PositionSide :char
{
	Long,
	Short,
};

enum ExecType : char
{
	ExecNew,
	ExecRejected,
	ExecTrade,
	ExecPendingCancel,
	ExecCancelled,
	ExecCancelReject,
	ExecPendingReplace,
	ExecReplace,
	ExecReplaceReject,
};

enum OpenCloseType :char
{
	Open,
	Close,
	CloseToday,
};

enum HedgeFlagType :char
{
	Speculation,
	Arbitrage,
	Hedge,
	MarketMaker,
};

enum InstrumentType :char
{
	Stock,
	Future,
	Option,
	FutureOption,
	Bond,
	FX,
	Index,
	ETF,
	MultiLeg,
	Synthetic,
};


/*
发现融资融券的只有部分功能加入，还有一些功能缺失
这下回到XAPI的定位问题，到底是一个只服务自动套利交易的API,还是全功能的API
*/
enum SecurityType :char
{
	CS, // Common Stock
	CB, // Convertible Bond，标记后，深圳卖出当前可转债表示转股
	ETF_, // 标记后，买入表示申购，卖出表示赎回
	LOF, // 标记后，买入表示申购，卖出表示赎回
	SF, // 标记后，买入表示合并，卖出表示分拆
	CashMargin, // 标记后，开仓表示"融"，平仓表示"还"
};

enum BarType :char
{
	Time = 1,
	Tick,
	Volume,
	Range,
	Session,
};

enum DataObjetType : char
{
	DataObject,
	Tick_,
	Bid,
	Ask,
	Trade_,
	Quote,
	Bar,
	Level2_,
	Level2Snapshot,
	Level2Update,
};

enum IdCardType:char
{
	EID,
	IDCard,
	OfficerIDCard,
	PoliceIDCard,
	SoldierIDCard,
	HouseholdRegister,
	Passport,
	TaiwanCompatriotIDCard,
	HomeComingCard,
	LicenseNo,
	TaxNo,
	HMMainlandTravelPermit,
	TwMainlandTravelPermit,
	DrivingLicense,
	SocialID,
	LocalID,
	BusinessRegistration,
	HKMCIDCard,
	AccountsPermits,
	OtherCard,
};

enum ExchangeType :char
{
	Undefined_,
	SHFE, // 上期所
	DCE, // 大商所
	CZCE, // 郑商所
	CFFEX, // 中金所
	INE, // 能源中心
	SSE, // 上交所
	SZE, // 深交所
	NEEQ, // 全国中小企业股份转让系统,三板，临时这么写
	HKEx,
};

#endif
