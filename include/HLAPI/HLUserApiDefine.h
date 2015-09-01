#ifndef HLUSERAPIDEFINE_HHH_
#define HLUSERAPIDEFINE_HHH_

//errcode
const int API_ERROR_OK = 0;             //OK
const int API_ERROR_DISCONNECTED = -1;  //未连接
const int API_ERROR_NORESULT = 0xD000;  //没有查询结果

#define API_PACKET_FIRST    0x01		//首包标记
#define API_PACKET_LAST     0x02		//尾包标记

#define API_ERRORMSG_LEN    256
#define API_APP_LEN         16
#define API_CLIENT_LEN      16
#define API_ORDERID_LEN     11
#define API_ACCOUNT_LEN     16
#define API_PSW_LEN         16
#define API_IPADDR          16
#define API_CLIENTINFO      32
#define API_DATE_LEN        9   //日期结构 yyyymmdd
#define API_TIME_LEN        9   //时间结构 hh:mm:ss
#define API_MARKET_LEN      4
#define API_SYMBOL_LEN      8
#define API_SYMBOLNAME_LEN  16
#define API_SAVE_LEN        32
#define API_PREFIX_LEN      4
#define API_MATCHED_SN_LEN  17  //成交编号
#define API_STOCK_PBU_LEN   9   //交易单元
#define API_TRADE_COUNT_LEN 11  //证券帐户
#define API_QUERY_POS_LEN   33  //定为串
#define API_ORD_TIME_LEN    33  //委托时间
#define API_ORDER_ID_LEN    11  //合同序号


#define API_MAX_BATCH_ORDER_COUNT           15  //批量委托最大数量
#define API_MAX_DAY_ORDER_RECORDSET_COUNT   100 //当日委托查询返回记录最大数量
#define API_MAX_AVL_STK_COUNTS              100 //查询可用股份时，每次返回最大股份数量 

//交易所
#define API_EXCH_SZ "SZ"    //深圳 
#define API_EXCH_SH "SH"    //上海 
#define API_EXCH_CF "CF"    //中国金融期货交易所 
#define API_EXCH_SP "SP"    //商品期货 

typedef char API_MARKETFLAG[API_MARKET_LEN];

//板块标志
typedef enum
{
	SectorSHA   = '1',      //上海A股
	SectorSZA   = '2',      //深圳A股
	SectorSHB   = '3',      //上海B股
	SectorSZB   = '4',      //深圳B股
	SectorSBA   = '5',      //三板A
	SectorSBB   = '6',      //三板B
} SECTORSYMBOL;

//成交状态集合
typedef enum
{
	MatchStatusDone     = 0,  //不包含撤单记录
	MatchStatusCancel   = 1,  //包含撤单记录
} MATCHSTATUSSET;

//证券业务
typedef enum
{
	ETFSubcribe     = 1,        //申购
	ETFRedeem       = 2,        //赎回
	StockBuy        = 100,      //证券买入
	StockSel        = 101,      //证券卖出

	Fund_Creation	= 102,		//基金申购
	Fund_Redemption	= 103,		//基金赎回
	Fund_Merger		= 104,		//基金合并
	Fund_Demerger	= 105,		//基金分拆

} STOCKBIZ;

//报价方式
typedef enum
{
	StockLimit = 100,   //订单申报-限价委托(支持深沪市)
	//StockCancel = 101,  //撤单申报
	StockAction1 = 120, //订单申报-最优成交转价(支持深沪市)
	StockAction2 = 121, //订单申报-最优成交剩撤(支持深市)
	StockAction3 = 122, //订单申报-全成交或撤销(支持深市)
	StockAction4 = 123, //订单申报-本方最优价格(支持深市)
	StockAction5 = 124, //订单申报-对手最优价格(支持深市)
	StockAction6 = 125, //订单申报-即时成交剩撤(支持沪市)
} STOCKBIZACTION;

//委托单类型
typedef enum
{
	OrderLimited    = 0,    //限价
	OrderAutoOffer  = 1,    //自动盘口
	OrderAskPrice1  = 2,    //卖一价
	OrderBidPrice1  = 3,    //买一价
} ORDERPRICETYPE;

//委托状态
typedef enum
{
	OrderNotSend        = 'o',  //未报
	OrderSending        = 's',  //正报
	OrderNewComer       = 'n',  //待报
	OrderSended         = 'a',  //已报
	OrderToBeDel        = '3',  //已报待撤
	OrderPartCToBeDel   = '4',  //部成待撤
	OrderPartDeleted    = 'b',  //部成部撤
	OrderToBeDelete     = 'f',  //场外撤单
	OrderPartCDeleted   = 'p',  //部成
	OrderCompleted      = 'c',  //全成
	OrderUseless        = 'e',  //错单
	OrderBeDeleted      = 'd',  //全撤
	OrderConfirmed      = 'D',  //已确认
	OrderDenied         = 'E',  //已否决
	OrderCanceling      = 'Z',  //正在撤单
	LogoicOrderAbandoned= 'L',  //逻辑单作废
} ORDERSTATUS;

//推送标志
typedef enum
{
	MatchPushDeal           = 0,    //成交推送
	MatchPushCancel         = 1,    //撤单推送
	MatchPushOrderFailed    = 2,    //委托失败推送
	MatchPushCancelFailed   = 4,    //撤单失败推送
} MATCHPUSHFLAG;

//委托有效标志
typedef enum
{
	OrderFlagInvalid    = 0,    //无效
	OrderFlagValid      = 1,    //有效
} ORDERVALIDFLAG;

//撤单标志
typedef enum
{
	FlagNormal      = 'F',  //正常
	FlagCanceled    = 'T',  //撤单
} ISWITHDRAW;

//币种
typedef enum
{
	CurrencyRMB    = 1,    //人民币
	CurrencyUSD    = 2,    //美元
	CurrencyHK     = 3,    //港币
} CURRENCYTYPE;

//资金状态
typedef enum
{
	FundStatusNormal    = 0,    //正常
	FundStatusCancel    = 9,    //销户
} FUNDSTATUS;

//ETF申赎标志
typedef enum
{
	ETFSubcribeFlag     = 1,    //申购
	ETFRedeemFlag       = 2,    //赎回
} ETFSUBSCRIBEFLAG;


//委托数据
typedef struct
{
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	double dOrderPrice;                     //委托价
	ORDERPRICETYPE eOrderPriceType;         //委托价方式
	int nOrderQty;                          //委托股数
	STOCKBIZ eStockBiz;                     //证券业务
	STOCKBIZACTION eStockBizAction;         //报价方式
} ORDERDATA;


//证券数据
struct HLApiSecurityCode
{
	int  nType;                 //证券类型
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	char chSymbol[API_SYMBOLNAME_LEN];     //证券名称
};

//ETF股票数据
struct ETFItemData
{
	char chSecurityCode[API_SYMBOL_LEN];        //股票代码
	char chSymbol[API_SYMBOLNAME_LEN];          //股票名称
	SECTORSYMBOL eSectorSymbol;                 //板块标志
	int nNum;                                   //股票数量
	int nFlag;                                  //现金替代标志
	double dRatio;                              //溢价比例
	double dReplaceAmount;                      //替代金额
};

///响应信息
struct CHLRspInfoData
{
	int  ErrorID;
	char ErrorMsg[API_ERRORMSG_LEN];
};

//用户登录请求
struct HLApiReqUserLoginData
{
	char chApplication[API_APP_LEN];    //应用类型
	char chClientID[API_CLIENT_LEN];    //客户号
	char chPassword[API_PSW_LEN];       //密码
	char chDynPassword[API_PSW_LEN];    //动态密码
	char chCurIP[API_IPADDR];           //IP
	int  nUserType;                     //客户类型
	char chClientInfo[API_CLIENTINFO];  //客户端信息
};

//用户登录响应
struct HLApiRspUserLoginData
{
	char chTradingDay[API_DATE_LEN];    //交易日
	char chLoginTime[API_TIME_LEN];     //登录时间
	char chClientID[API_CLIENT_LEN];    //客户号
	int  nSessionID;                    //session id
	char chSvrInfo[API_CLIENTINFO];     //服务端信息
	int  nMarkets;                      //市场个数
	API_MARKETFLAG* chMarketFlag;       //市场标志(SZ;SH;HK;CF)
	int*  nDynDate;                     //动态数据日期
};

//市场索引请求
struct HLApiReqCodeTable
{
	char    chMarket[API_MARKET_LEN];   //市场标志(SZ;SH;HK;CF)
};

//市场索引请求应答
struct HLApiRspCodeTable
{
	API_MARKETFLAG chMarketFlag;            //市场标志(SZ;SH;HK;CF)
	int  nDate;                             //数据日期
	int  nCount;                            //数据总数(-1:未授权)
	HLApiSecurityCode* pSecurityCode;       //数据
};

//当日ETF清单请求
struct HLApiReqETFList
{
	char    chSecurityCode[API_SYMBOL_LEN]; //证券代码
};

//ETF清单请求响应
struct HLApiRspETFList
{
	char chSecurityCode[API_SYMBOL_LEN];    //ETF代码
	char chSymbol[API_SYMBOLNAME_LEN];      //ETF名称
	char szFundid[API_SYMBOL_LEN];          //一级市场代码
	SECTORSYMBOL eSectorSymbol;             //板块标志
	int nCreationRedemptionUnit;            //最小申购赎回单位
	double dMaxCashRatio;                   //现金替代比例上限
	int nCreationRedemption;                //基金当天申购赎回状态(1,0)
	double dEstimateCashComponent;          //T日预估现金余额
	int nTradingDay;                        //T日日期
	double dCashComponent;                  //T-1日现金差额(单位：元)
	double dNAVperCU;                       //T-1日最小申购、赎回单位资产净值(单位：元)
	ETFItemData* pETFItemData;              //成份股数组指针
	int nItemDataSize;                      //成份股数量
};

//行情订阅请求
struct HLApiReqMarketData
{
	int     nType;                                  //命令类型(0订阅 1退订)
	char    chMarket[API_MARKET_LEN];               //市场标志(SZ;SH;HK;CF)
	char    chSecurityCode[API_SYMBOL_LEN];         //股票代码(空则为全市场行情)
};

//行情订阅请求应答
struct HLApiRspMarketData
{
	int     nResult;                                //命令结果(0成功 1失败)
	int     nType;                                  //命令类型(0订阅 1退订)
	char    chMarket[API_MARKET_LEN];               //市场标志(SZ;SH;HK;CF)
	char    chSecurityCode[API_SYMBOL_LEN];         //股票代码(空则为全市场行情)
};

//行情信息
struct HLApiMarketData
{
	char        chMarket[API_MARKET_LEN];   //市场标志(SZ;SH;HK;CF)
	char        chSymbol[API_SYMBOL_LEN];   //股票代码
	int         nTime;                      //时间(HHMMSSmmm)
	int         nStatus;                    //状态
	double      dPreClose;                  //前收盘价
	double      dOpen;                      //开盘价
	double      dHigh;                      //最高价
	double      dLow;                       //最低价
	double      dMatch;                     //最新价
	double      dAskPrice[10];              //申卖价
	unsigned int nAskVol[10];               //申卖量
	double      dBidPrice[10];				//申买价
	unsigned int nBidVol[10];				//申买量
	unsigned int nNumTrades;				//成交笔数
	long long   iVolume;					//成交总量
	long long   iTurnover;					//成交总金额
	long long   nTotalBidVol;				//委托买入总量
	long long   nTotalAskVol;				//委托卖出总量
	double      dHighLimited;				//涨停价
	double      dLowLimited;				//跌停价
};

//用户委托请求
struct HLApiCmdOrder
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	//int nOrderBsn;                          //委托批号（0则系统生成）
	ORDERDATA stOrderData;                  //委托数据
	int nSaveInt1;                          //客户保留字段1
	int nSaveInt2;                          //客户保留字段2
	char chSaveString[API_SAVE_LEN];        //客户保留字段3
};

//用户委托请求应答
struct HLApiRspOrder
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	char chSecurityName[API_SYMBOLNAME_LEN];//证券名称
	char chOrderID[API_ORDER_ID_LEN];       //合同序号
	ORDERSTATUS eOrderStatus;               //委托状态
	STOCKBIZ eStockBiz;                     //证券业务
	double dOrderPrice;                     //委托价格
	int nOrderQty;                          //委托数量
	double dMatchedPrice;                   //已成均价
	int nMatchedQty;                        //已成数量
	char chOrderTime[API_ORD_TIME_LEN];     //委托时间
	int nOrderDay;                          //委托日期
	int nSaveInt1;                          //客户保留字段1
	int nSaveInt2;                          //客户保留字段2
	char chSaveString[API_SAVE_LEN];        //客户保留字段3
};

//用户撤单请求
struct HLApiCmdCancelOrder
{
	char chOrderID[API_ORDERID_LEN];        //合同序号
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
};

//用户撤单请求应答
typedef struct HLApiCmdCancelOrder HLApiRspCancelOrder;

//用户批量委托请求
struct HLApiCmdBatchOrder
{
	char chClientID[API_CLIENT_LEN];                    //客户号
	char chAccountNO[API_ACCOUNT_LEN];                  //资金账户
	int nOrderCount;                                    //委托数，最大个数API_MAX_BATCH_ORDER_COUNT
	ORDERDATA stOrderData[API_MAX_BATCH_ORDER_COUNT];   //委托数据
	int nSaveInt1;                                      //客户保留字段1
	int nSaveInt2;                                      //客户保留字段2
	char chSaveString[API_SAVE_LEN];                    //客户保留字段3
};

//ETF申赎请求
struct HLApiCmdETFSubscribe
{
	char chClientID[API_CLIENT_LEN];                    //客户号
	char chAccountNO[API_ACCOUNT_LEN];                  //资金账户
	ETFSUBSCRIBEFLAG eSubscribeFlag;                    //申赎标志
	char chSecurityCode[API_SYMBOL_LEN];                //ETF代码
	int nSubscribeQty;                                  //申赎份额
};

//ETF申赎请求应答
struct HLApiRspETFSubscribe
{
	char chClientID[API_CLIENT_LEN];                    //客户号
	char chAccountNO[API_ACCOUNT_LEN];                  //资金账户
	ETFSUBSCRIBEFLAG eSubscribeFlag;                    //申赎标志
	char chSecurityCode[API_SYMBOL_LEN];                //ETF代码
	int nSubscribeQty;                                  //申赎份额
	char chOrderID[API_ORDERID_LEN];                    //合同序号
};

//成交、撤单、废单通知推送
struct HLApiRtnMatchInfo
{
	char chMatchedSN[API_MATCHED_SN_LEN];   //成交编号
	SECTORSYMBOL eSectorSymbol;             //板块标志
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	char chSecurityName[API_SYMBOLNAME_LEN];//证券名称
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	char chOrderID[API_ORDER_ID_LEN];       //合同序号
	int nMatchedQty;                        //成交或撤单数量
	double dMatchedPrice;                   //成交或撤单均价
	int nMatchedDate;                       //成交或撤单日期
	char chMatchedTime[API_ORD_TIME_LEN];   //成交或撤单时间
	MATCHPUSHFLAG eMatchPushFlag;           //推送类型标志
	STOCKBIZ eStockBiz;                     //证券业务(成交查询时有效)
};

typedef struct HLApiRspOrder HLApiRtnOrderInfo;

typedef struct HLApiRspAvlStocksInfoQry HLApiRtnAvlStockInfo;

typedef struct HLApiRspMoneyInfoQry HLApiRtnMoneyInfo;

//委托查询请求
struct HLApiCmdOrderQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	char chBeginDate[API_DATE_LEN];         //起始日期(空为查询所有)
	char chEndDate[API_DATE_LEN];           //终止日期(空为查询所有)
	char chOrderID[API_ORDERID_LEN];        //合同序号(空为查询所有)
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码(空为查询所有)
	STOCKBIZ eStockBiz;                     //证券业务(0为查询所有)
	double dOrderPrice;                     //委托价(0为查询所有)
	int nOrderQty;                          //委托股数(0为查询所有)
};

//委托查询请求应答
typedef struct HLApiRspOrder HLApiRspOrderQry;

//可用资金查询
struct HLApiCmdMoneyInfoQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	CURRENCYTYPE eCurrencyType;             //币种
};

//可用资金查询应答
struct HLApiRspMoneyInfoQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	CURRENCYTYPE eCurrencyType;             //币种
	//unsigned short nIntOrg;                 //内部机构
	//double dMarketValue;                    //资产总值
	//double dFundValue;                      //资金资产
	//double dStkValue;                       //市值
	//double dFundLoan;                       //融资总金额
	//double dFundPreBln;                     //资金昨日余额
	double dFundBln;                        //资金余额
	double dFundAvl;                        //可用资金
	double dFundWithDraw;                   //可取资金
	//double dFundUnFrz;                      //资金解冻金额
	//double dFundTradeFrz;                   //资金交易冻结金额
	//double dFundTradeUnFrz;                 //资金交易解冻金额
	//double dFundTradeInTransit;             //资金交易在途金额
	//double FundTradeInBln;                  //资金交易轧差金额
	//FUNDSTATUS nFundStatus;                 //资金状态(正常、销户)
};

//可用股份查询
struct HLApiCmdAvlStocksInfoQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码(空为查询所有)
};

//可用股份查询应答
struct HLApiRspAvlStocksInfoQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	SECTORSYMBOL eSectorSymbol;             //板块标志
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	char chSecurityName[API_SYMBOLNAME_LEN];//证券名称
	int nStockBln;                          //证券余额（含当日成交，既拥股数）
	int nStockAvlValue;                     //可用余额
	int nStockPreBln;                       //昨日库存
	double dStkValue;                       //市值
	double dProPrice;                       //成本价
	int nStockSR;                           //可申赎数量
	int nStockAskFrn;                       //卖出冻结
	int nStockBidFrn;                       //买入冻结
};

//当日成交查询
struct HLApiCmdTodayDealQry
{
	char chClientID[API_CLIENT_LEN];        //客户号
	char chAccountNO[API_ACCOUNT_LEN];      //资金账户
	char chSecurityCode[API_SYMBOL_LEN];    //证券代码
	MATCHSTATUSSET eMatchStatus;            //成交状态集合
};

//当日成交查询应答
typedef struct HLApiRtnMatchInfo HLApiRspTodayDealQry;


//分级基金成分信息查询
struct HLApiCmdStruFundInfoQry
{
	char chFundCode[API_SYMBOL_LEN];	//母基金代码,空串""则查询整个清单
};


//分级基金成分信息查询_应答
struct HLApiRspStruFundInfoQry
{
	char chFundCode[API_SYMBOL_LEN];		//母基金代码
	char chFundName[API_SYMBOLNAME_LEN];	//母基金名称
	char chFundACode[API_SYMBOL_LEN];		//分级A代码
	char chFundAName[API_SYMBOLNAME_LEN];	//分级A名称
	char chFundBCode[API_SYMBOL_LEN];		//分级B代码
	char chFundBName[API_SYMBOLNAME_LEN];	//分级B名称
	int  nFundAShare;						//分级A比例(0,100), 默认:50
	int	 nFundBShare;						//分级B比例(0,100), 默认:50
};



#endif /*HLUSERAPIDEFINE_HHH_*/



