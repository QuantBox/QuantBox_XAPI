#ifndef _API_ENUM_H_
#define _API_ENUM_H_

enum LogLevel
{
	E_Fatal,
	E_Critical,
	E_Error,
	E_Warning,
	E_Notice,
	E_Information,
	E_Debug,
	E_Trace,
};


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
	Unknown, //未知
	ConnectFailed,
	InitializedFailed,
	LoginFailed,
	Logout,
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
	Creation,//申购
	Redemption,//赎回
	Merger,//合并
	Demerger,//拆分
};

enum OrderType :char
{
	Market,
	Stop,
	Limit,
	StopLimit,
	MarketOnClose,
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
	LOF,
	Other,
};

enum BarType :char
{
	Time,
	Tick,
	Volume,
	Range,
};

enum DataObjetType : char
{
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
